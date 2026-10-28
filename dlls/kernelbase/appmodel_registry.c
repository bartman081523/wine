/* Reader for the prefix's seeded package-identity table.
 *
 * External MSIX installer tooling writes every package of an installed
 * app tree into HKCU\Software\Wine\AppModel: a CurrentPackageFullName
 * value naming the process's own identity, plus one
 * Packages\<full name> subkey per package whose values mirror that
 * package's manifest (Name, Version, Publisher, PublisherId,
 * ResourceId, Architecture, Path, AppId, IsFramework). The
 * WineAppModel* functions below serve that table to kernelbase's
 * AppModel functions and to appxdeploymentclient's WinRT package
 * objects. On an unseeded prefix the table stays empty and every
 * caller keeps its stock behaviour.
 */

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "ntstatus.h"
#define WIN32_NO_STATUS
#include "windef.h"
#include "winbase.h"
#include "winreg.h"
#include "winerror.h"
#include "winternl.h"
#include "appmodel.h"
/* maps the Heap* names onto their ntdll core (exporting them as forwards
 * means kernelbase must not import its own) */
#include "kernelbase.h"

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(appmodel);

static SRWLOCK table_lock = SRWLOCK_INIT;
static WINE_APPMODEL_ENTRY *entries;
static UINT32 entry_count;
static UINT32 current_index;
static BOOL table_loaded;

static HANDLE open_subkey( HANDLE parent, const WCHAR *name )
{
    OBJECT_ATTRIBUTES attr;
    UNICODE_STRING str;
    HANDLE key = NULL;

    RtlInitUnicodeString( &str, name );
    InitializeObjectAttributes( &attr, &str, OBJ_CASE_INSENSITIVE, parent, NULL );
    if (NtOpenKey( &key, KEY_READ, &attr )) return NULL;
    return key;
}

static KEY_VALUE_PARTIAL_INFORMATION *query_value( HANDLE key, const WCHAR *name )
{
    UNICODE_STRING str;
    KEY_VALUE_PARTIAL_INFORMATION *info;
    ULONG size = offsetof( KEY_VALUE_PARTIAL_INFORMATION, Data ) + 256;
    NTSTATUS status;

    RtlInitUnicodeString( &str, name );
    for (;;)
    {
        if (!(info = HeapAlloc( GetProcessHeap(), 0, size ))) return NULL;
        status = NtQueryValueKey( key, &str, KeyValuePartialInformation, info, size, &size );
        if (!status) return info;
        HeapFree( GetProcessHeap(), 0, info );
        if (status != STATUS_BUFFER_OVERFLOW) return NULL;
    }
}

static WCHAR *read_string_value( HANDLE key, const WCHAR *name )
{
    KEY_VALUE_PARTIAL_INFORMATION *info = query_value( key, name );
    const WCHAR *src;
    UINT32 chars, copied;
    WCHAR *ret = NULL;

    if (!info) return NULL;
    if (info->Type == REG_SZ && info->DataLength)
    {
        src = (const WCHAR *)info->Data;
        chars = info->DataLength / sizeof(WCHAR);
        copied = chars;
        if (src[chars - 1]) copied++;   /* registry strings are NUL-terminated; be defensive */
        if ((ret = HeapAlloc( GetProcessHeap(), 0, copied * sizeof(WCHAR) )))
        {
            memcpy( ret, src, chars * sizeof(WCHAR) );
            ret[copied - 1] = 0;
        }
    }
    HeapFree( GetProcessHeap(), 0, info );
    return ret;
}

static DWORD read_dword_value( HANDLE key, const WCHAR *name, DWORD def )
{
    KEY_VALUE_PARTIAL_INFORMATION *info = query_value( key, name );
    DWORD ret = def;

    if (!info) return def;
    if (info->Type == REG_DWORD && info->DataLength == sizeof(DWORD))
        memcpy( &ret, info->Data, sizeof(DWORD) );
    HeapFree( GetProcessHeap(), 0, info );
    return ret;
}

static WCHAR *next_subkey_name( HANDLE key, ULONG index )
{
    union { KEY_BASIC_INFORMATION key; char size[576]; } buffer;
    KEY_BASIC_INFORMATION *info = &buffer.key;
    ULONG size = sizeof(buffer);
    NTSTATUS status;
    WCHAR *name = NULL;
    UINT32 chars;

    status = NtEnumerateKey( key, index, KeyBasicInformation, info, size, &size );
    if (status == STATUS_BUFFER_OVERFLOW)
    {
        if (!(info = HeapAlloc( GetProcessHeap(), 0, size ))) return NULL;
        status = NtEnumerateKey( key, index, KeyBasicInformation, info, size, &size );
    }
    if (!status)
    {
        /* KEY_BASIC_INFORMATION.Name is not NUL-terminated */
        chars = info->NameLength / sizeof(WCHAR);
        if ((name = HeapAlloc( GetProcessHeap(), 0, (chars + 1) * sizeof(WCHAR) )))
        {
            memcpy( name, info->Name, chars * sizeof(WCHAR) );
            name[chars] = 0;
        }
    }
    if (info != &buffer.key) HeapFree( GetProcessHeap(), 0, info );
    return name;
}

static PACKAGE_VERSION parse_version( const WCHAR *str )
{
    PACKAGE_VERSION ret;
    USHORT parsed[4] = {0};
    const WCHAR *p = str, *end;
    int i;

    ret.Version = 0;
    if (!str) return ret;

    for (i = 0; i < 4; i++)
    {
        ULONG val = wcstoul( p, (WCHAR **)&end, 10 );
        if (end == p) break;
        parsed[i] = min( val, 0xffff );
        if (*end != L'.' && *end != L'_') break;
        p = end + 1;
    }
    ret.Major = parsed[0];
    ret.Minor = parsed[1];
    ret.Build = parsed[2];
    ret.Revision = parsed[3];
    return ret;
}

static UINT32 parse_architecture( const WCHAR *arch )
{
    if (!arch) return PROCESSOR_ARCHITECTURE_NEUTRAL;
    if (!wcsicmp( arch, L"x86" )) return PROCESSOR_ARCHITECTURE_INTEL;
    if (!wcsicmp( arch, L"arm" )) return PROCESSOR_ARCHITECTURE_ARM;
    if (!wcsicmp( arch, L"x64" )) return PROCESSOR_ARCHITECTURE_AMD64;
    if (!wcsicmp( arch, L"arm64" )) return PROCESSOR_ARCHITECTURE_ARM64;
    return PROCESSOR_ARCHITECTURE_NEUTRAL;
}

static WCHAR *derive_family_name( const WCHAR *name, const WCHAR *publisher_id )
{
    UINT32 len = lstrlenW( name ) + 1 + lstrlenW( publisher_id );
    WCHAR *ret = HeapAlloc( GetProcessHeap(), 0, (len + 1) * sizeof(WCHAR) );

    if (!ret) return NULL;
    lstrcpyW( ret, name );
    lstrcatW( ret, L"_" );
    lstrcatW( ret, publisher_id );
    return ret;
}

static void free_entry_strings( WINE_APPMODEL_ENTRY *entry )
{
    HeapFree( GetProcessHeap(), 0, (WCHAR *)entry->full_name );
    HeapFree( GetProcessHeap(), 0, (WCHAR *)entry->name );
    HeapFree( GetProcessHeap(), 0, (WCHAR *)entry->version );
    HeapFree( GetProcessHeap(), 0, (WCHAR *)entry->publisher );
    HeapFree( GetProcessHeap(), 0, (WCHAR *)entry->publisher_id );
    HeapFree( GetProcessHeap(), 0, (WCHAR *)entry->resource_id );
    HeapFree( GetProcessHeap(), 0, (WCHAR *)entry->architecture );
    HeapFree( GetProcessHeap(), 0, (WCHAR *)entry->path );
    HeapFree( GetProcessHeap(), 0, (WCHAR *)entry->app_id );
    HeapFree( GetProcessHeap(), 0, (WCHAR *)entry->family_name );
}

/* Fills *entry with the identity stored under one Packages subkey.
 * Takes ownership of full_name, which doubles as the package's full
 * name, and reads the remaining fields from that subkey's values. */
static BOOL load_entry( HANDLE packages, WINE_APPMODEL_ENTRY *entry )
{
    HANDLE key;
    BOOL ok = FALSE;

    if (!(key = open_subkey( packages, entry->full_name )))
    {
        ERR( "package key %s not openable\n", debugstr_w( entry->full_name ) );
        return FALSE;
    }

    if (!(entry->name = read_string_value( key, L"Name" )))
    {
        ERR( "package %s has no Name value\n", debugstr_w( entry->full_name ) );
        NtClose( key );
        return FALSE;
    }

    entry->publisher_id = read_string_value( key, L"PublisherId" );
    entry->publisher = read_string_value( key, L"Publisher" );
    entry->resource_id = read_string_value( key, L"ResourceId" );
    entry->architecture = read_string_value( key, L"Architecture" );
    entry->version = read_string_value( key, L"Version" );
    entry->path = read_string_value( key, L"Path" );
    entry->app_id = read_string_value( key, L"AppId" );
    entry->is_framework = read_dword_value( key, L"IsFramework", 0 );
    if (entry->app_id && !*entry->app_id)
    {
        HeapFree( GetProcessHeap(), 0, (WCHAR *)entry->app_id );
        entry->app_id = NULL;
    }
    entry->package_version = parse_version( entry->version );
    entry->processor_architecture = parse_architecture( entry->architecture );
    /* the family name is the identity name plus the publisher hash */
    entry->family_name = derive_family_name( entry->name,
                                             entry->publisher_id ? entry->publisher_id : L"" );
    ok = entry->family_name != NULL;
    if (!ok) ERR( "family name derivation failed for %s\n", debugstr_w( entry->full_name ) );
    NtClose( key );
    return ok;
}

static void load_registry_table( void )
{
    HANDLE user = NULL, root, packages;
    WCHAR *subkey, *current_full = NULL;
    WINE_APPMODEL_ENTRY *tmp;
    ULONG index;

    if (RtlOpenCurrentUser( KEY_READ, &user ))
    {
        ERR( "RtlOpenCurrentUser failed\n" );
        return;
    }
    root = open_subkey( user, L"Software\\Wine\\AppModel" );
    NtClose( user );
    if (!root)
    {
        ERR( "root key HKCU\\Software\\Wine\\AppModel not found (seed it with the tool's appmodel reg file)\n" );
        return;
    }

    packages = open_subkey( root, L"Packages" );
    if (packages)
    {
        current_full = read_string_value( root, L"CurrentPackageFullName" );

        for (index = 0; (subkey = next_subkey_name( packages, index )); index++)
        {
            /* the subkey name is the package's full name */
            WINE_APPMODEL_ENTRY entry = {0};

            entry.full_name = subkey;
            if (!load_entry( packages, &entry ))
            {
                ERR( "skipping package %s (entry load failed)\n", debugstr_w( entry.full_name ) );
                free_entry_strings( &entry );
                continue;
            }
            /* HeapReAlloc does not allocate from a NULL pointer */
            tmp = entries ? HeapReAlloc( GetProcessHeap(), 0, entries, (entry_count + 1) * sizeof(*entries) )
                          : HeapAlloc( GetProcessHeap(), 0, (entry_count + 1) * sizeof(*entries) );
            if (!tmp)
            {
                ERR( "skipping package %s (out of memory)\n", debugstr_w( entry.full_name ) );
                free_entry_strings( &entry );
                break;
            }
            entries = tmp;
            entries[entry_count++] = entry;
        }
        NtClose( packages );
    }
    else ERR( "Packages subkey not found under HKCU\\Software\\Wine\\AppModel\n" );
    NtClose( root );

    if (entry_count)
    {
        current_index = 0;
        if (current_full)
        {
            for (index = 0; index < entry_count; index++)
                if (!wcscmp( entries[index].full_name, current_full ))
                {
                    current_index = index;
                    break;
                }
        }
    }
    HeapFree( GetProcessHeap(), 0, current_full );

    TRACE( "loaded %u package entries from HKCU\\Software\\Wine\\AppModel\n", entry_count );
}

/* One exclusive pass per table read; every reader calls this first, so
 * the lock provides the release/acquire pairing that publishes the
 * heap-allocated entries before anyone dereferences them. */
static void ensure_loaded( void )
{
    RtlAcquireSRWLockExclusive( &table_lock );
    if (!table_loaded)
    {
        load_registry_table();
        table_loaded = TRUE;
    }
    RtlReleaseSRWLockExclusive( &table_lock );
}

UINT32 WINAPI WineAppModelGetEntryCount(void)
{
    ensure_loaded();
    return entry_count;
}

UINT32 WINAPI WineAppModelGetEntryIndex( const WINE_APPMODEL_ENTRY *entry )
{
    UINT32 i;

    ensure_loaded();
    for (i = 0; i < entry_count; i++)
        if (&entries[i] == entry) return i;
    return ~0u;
}

const WINE_APPMODEL_ENTRY * WINAPI WineAppModelGetEntry( UINT32 index )
{
    ensure_loaded();
    if (index >= entry_count) return NULL;
    return &entries[index];
}

const WINE_APPMODEL_ENTRY * WINAPI WineAppModelGetEntryByFullName( const WCHAR *full_name )
{
    UINT32 i;

    ensure_loaded();
    if (!full_name) return NULL;
    for (i = 0; i < entry_count; i++)
        if (!wcscmp( entries[i].full_name, full_name )) return &entries[i];
    return NULL;
}

const WINE_APPMODEL_ENTRY * WINAPI WineAppModelGetCurrentEntry(void)
{
    ensure_loaded();
    return entry_count ? &entries[current_index] : NULL;
}

/* Returns the number of packages in the family; up to max_entries are
 * copied into entries (NULL just counts). */
UINT32 WINAPI WineAppModelGetEntriesByFamily( const WCHAR *family_name,
                                              const WINE_APPMODEL_ENTRY **out_entries,
                                              UINT32 max_entries )
{
    UINT32 i, matched = 0;

    ensure_loaded();
    if (!family_name) return 0;
    for (i = 0; i < entry_count; i++)
    {
        if (!wcsicmp( entries[i].family_name, family_name ))
        {
            if (out_entries && matched < max_entries) out_entries[matched] = &entries[i];
            matched++;
        }
    }
    return matched;
}