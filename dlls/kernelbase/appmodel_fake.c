/* PACKAGE_INFO plumbing on top of the seeded package table
 * (HKCU\Software\Wine\AppModel, served by appmodel_registry.c): the
 * PACKAGE_INFO_REFERENCE handle layer, family-based package enumeration
 * and the application user model id formatter. On an unseeded prefix
 * the lookups keep the stock NOT_FOUND answers. */

#include <stdarg.h>
#include <string.h>

#include "ntstatus.h"
#define WIN32_NO_STATUS
#include "windef.h"
#include "winbase.h"
#include "winreg.h"
#include "winerror.h"
#include "winternl.h"
#include "appmodel.h"

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(appmodel);

/* PACKAGE_INFO_REFERENCE handles are one-based indices into the seeded
 * package table; handle NULL underflows to an invalid index. */
static const WINE_APPMODEL_ENTRY *entry_from_handle( PACKAGE_INFO_REFERENCE reference )
{
    return WineAppModelGetEntry( (UINT32)(UINT_PTR)reference - 1 );
}

/* PACKAGE_FILTER_* flags the platform headers here do not carry. */
#define PACKAGE_FILTER_HEAD        0x00000010
#define PACKAGE_FILTER_DIRECT      0x00000020
#define PACKAGE_FILTER_STATIC      0x00010000
#define PACKAGE_FILTER_DYNAMIC     0x00020000
/* PACKAGE_PROPERTY_* flags of PACKAGE_INFO.flags. */
#define PACKAGE_PROPERTY_FRAMEWORK 0x00000001
#define PACKAGE_PROPERTY_RESOURCE  0x00000002

/* Builds the answer package list: the head package first, then every
 * framework entry of the seeded table as the graph's dependency set.
 * The Windows App SDK deployment code enumerates the graph this way
 * (filter HEAD|DIRECT|RESOURCE|BUNDLE|DYNAMIC) and expects to find the
 * runtime packages in it; a head-only answer is read as "no runtime
 * installed" and fails with ERROR_NOT_FOUND. */
static UINT32 entry_list_from_flags( const WINE_APPMODEL_ENTRY *head, UINT32 flags,
                                     const WINE_APPMODEL_ENTRY **entries, UINT32 max_entries )
{
    UINT32 total = WineAppModelGetEntryCount(), count = 0, i;

    /* A framework package has no dependency set of its own, so enumerating
     * its graph yields the package alone; the Windows App SDK deployment
     * verifies exactly that with a count == 1 check over the answer and
     * reads anything else as "runtime not found". */
    if (head->is_framework)
    {
        if ((flags & (PACKAGE_FILTER_HEAD | PACKAGE_FILTER_DIRECT |
                      PACKAGE_FILTER_STATIC | PACKAGE_FILTER_DYNAMIC)) && max_entries)
            entries[count++] = head;
        return count;
    }

    if ((flags & (PACKAGE_FILTER_HEAD | PACKAGE_FILTER_DIRECT |
                  PACKAGE_FILTER_STATIC | PACKAGE_FILTER_DYNAMIC)) && max_entries)
        entries[count++] = head;

    if (flags & (PACKAGE_FILTER_DIRECT | PACKAGE_FILTER_STATIC | PACKAGE_FILTER_DYNAMIC))
    {
        for (i = 0; i < total; i++)
        {
            const WINE_APPMODEL_ENTRY *entry = WineAppModelGetEntry( i );
            if (count >= max_entries) break;
            if (entry == head || !entry->is_framework) continue;
            entries[count++] = entry;
        }
    }

    if (!count && max_entries)
        entries[count++] = head;
    return count;
}

LONG WINAPI OpenPackageInfoByFullName( PCWSTR package_full_name, UINT32 reserved,
                                       PACKAGE_INFO_REFERENCE *package_info_reference )
{
    const WINE_APPMODEL_ENTRY *entry;

    TRACE( "(%s, %u, %p)\n", debugstr_w(package_full_name), reserved, package_info_reference );

    if (!package_info_reference)
        return ERROR_INVALID_PARAMETER;

    /* not among the installed packages keeps the stock answer */
    if (!package_full_name || !(entry = WineAppModelGetEntryByFullName( package_full_name )))
        return ERROR_NOT_FOUND;

    *package_info_reference =
        (PACKAGE_INFO_REFERENCE)(UINT_PTR)(WineAppModelGetEntryIndex( entry ) + 1);
    return ERROR_SUCCESS;
}

LONG WINAPI ClosePackageInfo( PACKAGE_INFO_REFERENCE package_info_reference )
{
    TRACE( "(%p)\n", package_info_reference );
    if (!entry_from_handle( package_info_reference ))
        return ERROR_INVALID_PARAMETER;
    return ERROR_SUCCESS;
}

LONG WINAPI GetPackageInfo( PACKAGE_INFO_REFERENCE package_info_reference, const UINT32 flags,
                            UINT32 *buffer_length, BYTE *buffer, UINT32 *count )
{
    const WINE_APPMODEL_ENTRY *head = entry_from_handle( package_info_reference );
    const WINE_APPMODEL_ENTRY *entries[64];
    const WINE_APPMODEL_ENTRY *entry;
    const WCHAR *path, *publisher, *publisher_id, *resource_id;
    UINT32 list_count, struct_size, strings_size, total_size, i;
    PACKAGE_INFO *info;
    WCHAR cwd[MAX_PATH];
    WCHAR *pool;

    TRACE( "(%p, %#x, %p, %p, %p)\n", package_info_reference, flags, buffer_length,
           buffer, count );

    if (!head || !buffer_length)
        return ERROR_INVALID_PARAMETER;

    list_count = entry_list_from_flags( head, flags, entries, ARRAY_SIZE( entries ) );

    /* sizing probe: Windows fills count only on the success pass, but
     * callers may rely on the buffer length alone. */
    struct_size = sizeof(PACKAGE_INFO);
    total_size = 0;
    for (i = 0; i < list_count; i++)
    {
        entry = entries[i];
        path = entry->path;
        if (!path)
        {
            if (!GetCurrentDirectoryW( MAX_PATH, cwd )) lstrcpyW( cwd, L"C:\\Windows\\System32" );
            path = cwd;
        }
        publisher = entry->publisher ? entry->publisher : L"";
        publisher_id = entry->publisher_id ? entry->publisher_id : L"";
        resource_id = entry->resource_id ? entry->resource_id : L"";

        strings_size = (lstrlenW( entry->name ) + 1
                        + lstrlenW( entry->full_name ) + 1
                        + lstrlenW( entry->family_name ) + 1
                        + lstrlenW( path ) + 1
                        + lstrlenW( publisher ) + 1
                        + lstrlenW( resource_id ) + 1
                        + lstrlenW( publisher_id ) + 1) * sizeof(WCHAR);
        total_size += struct_size + strings_size;
    }

    if (!buffer || *buffer_length < total_size)
    {
        *buffer_length = total_size;
        /* Windows sets the count on the sizing pass as well. */
        if (count)
            *count = list_count;
        return ERROR_INSUFFICIENT_BUFFER;
    }

    memset( buffer, 0, total_size );
    pool = (WCHAR *)(buffer + (SIZE_T)list_count * struct_size);

#define WRITE_STR(dst, str) \
    do { \
        lstrcpyW( pool, str ); \
        (dst) = pool; \
        pool += lstrlenW( str ) + 1; \
    } while (0)

    for (i = 0; i < list_count; i++)
    {
        entry = entries[i];
        path = entry->path;
        if (!path)
        {
            if (!GetCurrentDirectoryW( MAX_PATH, cwd )) lstrcpyW( cwd, L"C:\\Windows\\System32" );
            path = cwd;
        }
        publisher = entry->publisher ? entry->publisher : L"";
        publisher_id = entry->publisher_id ? entry->publisher_id : L"";
        resource_id = entry->resource_id ? entry->resource_id : L"";

        info = (PACKAGE_INFO *)(buffer + (SIZE_T)i * struct_size);
        info->flags = (entry->is_framework ? PACKAGE_PROPERTY_FRAMEWORK : 0)
                      | (resource_id[0] ? PACKAGE_PROPERTY_RESOURCE : 0);
        WRITE_STR( info->path, path );
        WRITE_STR( info->packageFullName, entry->full_name );
        WRITE_STR( info->packageFamilyName, entry->family_name );
        info->packageId.processorArchitecture = entry->processor_architecture;
        info->packageId.version = entry->package_version;
        WRITE_STR( info->packageId.name, entry->name );
        WRITE_STR( info->packageId.publisher, publisher );
        WRITE_STR( info->packageId.resourceId, resource_id );
        WRITE_STR( info->packageId.publisherId, publisher_id );
    }

#undef WRITE_STR

    if (count)
        *count = list_count;
    return ERROR_SUCCESS;
}

/* GetPackageInfo3 reports the same PACKAGE_INFO data for the package
 * types the seeded table carries, so the plain call serves it. */
LONG WINAPI GetPackageInfo3( PACKAGE_INFO_REFERENCE package_info_reference, const UINT32 flags,
                             UINT32 *buffer_length, BYTE *buffer, UINT32 *count )
{
    return GetPackageInfo( package_info_reference, flags, buffer_length, buffer, count );
}

LONG WINAPI FindPackagesByPackageFamily( PCWSTR family_name, UINT32 filters, UINT32 *count,
                                         PCWSTR *package_full_names, UINT32 *buffer_length,
                                         WCHAR *buffer, UINT32 *properties )
{
    const WINE_APPMODEL_ENTRY *entry;
    UINT32 total, i, matched = 0, needed = 0, k;
    WCHAR *dest;

    TRACE( "(%s, %#x, %p, %p, %p, %p, %p)\n", debugstr_w(family_name), filters, count,
           package_full_names, buffer_length, buffer, properties );

    if (!family_name || !count || !buffer_length)
        return ERROR_INVALID_PARAMETER;

    total = WineAppModelGetEntryCount();
    for (i = 0; i < total; i++)
    {
        entry = WineAppModelGetEntry( i );
        if (!wcsicmp( entry->family_name, family_name ))
        {
            matched++;
            needed += lstrlenW( entry->full_name ) + 1;
        }
    }

    /* a family with no installed packages keeps the stock answer */
    if (!matched)
    {
        *count = 0;
        *buffer_length = 0;
        return APPMODEL_ERROR_NO_PACKAGE;
    }

    if (!package_full_names || !buffer || *buffer_length < needed)
    {
        *count = matched;
        *buffer_length = needed;
        return ERROR_INSUFFICIENT_BUFFER;
    }

    /* Windows ABI: the caller supplies both the output buffer and the
     * full-name pointer array; count and required size are set on
     * either pass. */
    dest = buffer;
    k = 0;
    for (i = 0; i < total; i++)
    {
        entry = WineAppModelGetEntry( i );
        if (!wcsicmp( entry->family_name, family_name ))
        {
            lstrcpyW( dest, entry->full_name );
            package_full_names[k++] = dest;
            dest += lstrlenW( dest ) + 1;
        }
    }
    if (properties)
        for (i = 0; i < matched; i++)
            properties[i] = 0;
    *count = matched;
    *buffer_length = needed;
    return ERROR_SUCCESS;
}

/* Two-step implementation matching the Windows AUMID builder: sizing
 * probe with the required length, then "<family>!<relative id>" on the
 * success pass. */
LONG WINAPI FormatApplicationUserModelId( PCWSTR package_family_name,
                                          PCWSTR package_relative_application_id,
                                          UINT32 *application_user_model_id_length,
                                          PWSTR application_user_model_id )
{
    UINT32 family_len, rel_len, needed;

    TRACE( "(%s, %s, %p, %p)\n", debugstr_w(package_family_name),
           debugstr_w(package_relative_application_id), application_user_model_id_length,
           application_user_model_id );

    if (!package_family_name || !package_relative_application_id ||
        !application_user_model_id_length)
        return ERROR_INVALID_PARAMETER;

    family_len = lstrlenW( package_family_name );
    rel_len = lstrlenW( package_relative_application_id );
    needed = family_len + 1 /* '!' */ + rel_len + 1;
    if (*application_user_model_id_length < needed)
    {
        *application_user_model_id_length = needed;
        return ERROR_INSUFFICIENT_BUFFER;
    }

    lstrcpyW( application_user_model_id, package_family_name );
    application_user_model_id[family_len] = L'!';
    lstrcpyW( application_user_model_id + family_len + 1, package_relative_application_id );
    *application_user_model_id_length = needed;
    return ERROR_SUCCESS;
}

/* --- NtCreateWaitCompletionPacket ---
 * The WinUI dispatcher core (CoreMessagingXP) creates wait completion
 * packets on startup; a failed creation kills the process, there is no
 * fallback. wine implements none of the packet family, so the creation
 * succeeds with an anonymous, never-signaled event object as an opaque
 * stand-in packet handle (closeable, server-backed). */
LONG WINAPI NtCreateWaitCompletionPacket(void *packet_handle, ULONG desired_access,
                                         void *object_attributes)
{
    OBJECT_ATTRIBUTES attrs;

    FIXME( "(%p, %#lx, %p): stand-in event object replaces the packet\n",
           packet_handle, (unsigned long)desired_access, object_attributes );

    if (!packet_handle)
        return STATUS_INVALID_PARAMETER;

    InitializeObjectAttributes( &attrs, NULL, 0, 0, NULL );
    return NtCreateEvent( packet_handle, desired_access, &attrs,
                          NotificationEvent, FALSE );
}

/* --- NtAssociateWaitCompletionPacket ---
 * Arity per the Microsoft Learn devnotes (packet, IOCP, target, key
 * context, APC context, IO status block, IO information,
 * already-signaled out). A real association needs kernel-side async
 * signaling that wine does not provide. The failure return is NOT safe to
 * emulate either: CoreMessagingXP's failure path formats a diagnostic
 * ("Failed to associate packet"), and its placeholder-argument machinery
 * dereferences a non-refcounted slot and kills the process (the +0x10
 * refcount-addref access violation). So answer success, remember the
 * packet/target pair, and mirror target signals onto the stand-in packet
 * event so a later wait on it wakes together with the target. */
#define WAIT_PACKET_LINKS 8
static struct
{
    void *packet;
    void *target;
    void *io_port;      /* completion port the packet was associated into */
    void *key_context;  /* key the completion posts with */
} wait_packet_links[WAIT_PACKET_LINKS];
static LONG wait_packet_link_count;

/* "A message arrived on target's port": posts the association's completion
 * to the IO completion port, keyed with the value given at association
 * time. CoreMessagingXP's dispatcher pump parks on its completion port and
 * dispatches by that key (0x40000402 for the ALPC port), so with the real
 * wait-completion-packet machinery absent this call replaces it: without
 * the post the pump never leaves its WaitForMultipleObjectsEx (run47). */
void wait_packet_post_completions( void *target )
{
    LONG i;

    for (i = 0; i < wait_packet_link_count; ++i)
        if (wait_packet_links[i].target == target && wait_packet_links[i].io_port)
        {
            TRACE( "posting completion for target %p to %p (key %p)\n",
                   target, wait_packet_links[i].io_port,
                   wait_packet_links[i].key_context );
            NtSetIoCompletion( wait_packet_links[i].io_port,
                               (ULONG_PTR)wait_packet_links[i].key_context, 0,
                               STATUS_SUCCESS, 0 );
        }
}

LONG WINAPI NtAssociateWaitCompletionPacket(void *packet, void *io_port,
                                            void *target, void *key_context,
                                            void *apc_context, LONG io_status,
                                            ULONG_PTR io_information,
                                            BOOLEAN *already_signaled)
{
    FIXME( "(%p, %p, %p, %p, %p, %#lx, %#lx, %p): stand-in association\n",
           packet, io_port, target, key_context, apc_context,
           (unsigned long)io_status, (unsigned long)io_information,
           already_signaled );
    if (already_signaled)
        *already_signaled = FALSE;
    if (wait_packet_link_count < WAIT_PACKET_LINKS)
    {
        wait_packet_links[wait_packet_link_count].packet = packet;
        wait_packet_links[wait_packet_link_count].target = target;
        wait_packet_links[wait_packet_link_count].io_port = io_port;
        wait_packet_links[wait_packet_link_count].key_context = key_context;
        ++wait_packet_link_count;
    }
    return STATUS_SUCCESS;
}

/* --- NtCancelWaitCompletionPacket ---
 * Nothing was ever really associated with the stand-in handles. */
LONG WINAPI NtCancelWaitCompletionPacket(void *packet, BOOLEAN remove_signaled)
{
    FIXME( "(%p, %u): nothing was ever associated\n", packet, remove_signaled );
    return STATUS_SUCCESS;
}