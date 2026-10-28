/*
 * Copyright (C) 2017 Alistair Leslie-Hughes
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */
#ifndef _APPMODEL_H_
#define _APPMODEL_H_

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum AppPolicyMediaFoundationCodecLoading
{
    AppPolicyMediaFoundationCodecLoading_All       = 0,
    AppPolicyMediaFoundationCodecLoading_InboxOnly = 1,
} AppPolicyMediaFoundationCodecLoading;

typedef enum AppPolicyProcessTerminationMethod
{
    AppPolicyProcessTerminationMethod_ExitProcess      = 0,
    AppPolicyProcessTerminationMethod_TerminateProcess = 1,
} AppPolicyProcessTerminationMethod;

typedef enum AppPolicyThreadInitializationType
{
    AppPolicyThreadInitializationType_None            = 0,
    AppPolicyThreadInitializationType_InitializeWinRT = 1,
} AppPolicyThreadInitializationType;

typedef enum AppPolicyShowDeveloperDiagnostic
{
    AppPolicyShowDeveloperDiagnostic_None   = 0,
    AppPolicyShowDeveloperDiagnostic_ShowUI = 1,
} AppPolicyShowDeveloperDiagnostic;

typedef enum AppPolicyWindowingModel
{
    AppPolicyWindowingModel_None           = 0,
    AppPolicyWindowingModel_Universal      = 1,
    AppPolicyWindowingModel_ClassicDesktop = 2,
    AppPolicyWindowingModel_ClassicPhone   = 3
} AppPolicyWindowingModel;

typedef struct PACKAGE_VERSION
{
    union
    {
        UINT64 Version;
        struct
        {
            USHORT Revision;
            USHORT Build;
            USHORT Minor;
            USHORT Major;
        }
        DUMMYSTRUCTNAME;
    }
    DUMMYUNIONNAME;
}
PACKAGE_VERSION;

typedef struct PACKAGE_ID
{
    UINT32 reserved;
    UINT32 processorArchitecture;
    PACKAGE_VERSION version;
    WCHAR *name;
    WCHAR *publisher;
    WCHAR *resourceId;
    WCHAR *publisherId;
}
PACKAGE_ID;

typedef struct PACKAGE_INFO
{
    UINT32 reserved;
    UINT32 flags;
    const WCHAR *path;
    const WCHAR *packageFullName;
    const WCHAR *packageFamilyName;
    PACKAGE_ID packageId;
}
PACKAGE_INFO;

typedef void *PACKAGE_INFO_REFERENCE;

/* One entry of the package-identity table read by kernelbase's
 * appmodel_registry.c. External installer tooling seeds that table
 * (HKCU\Software\Wine\AppModel) with one Packages\<full name> subkey
 * per installed package plus CurrentPackageFullName marking the
 * running process's own identity; all string fields point into the
 * reader's stable storage, and path and app_id may be NULL. On an
 * unseeded prefix the table is empty and every function below keeps
 * its stock behaviour. */
typedef struct WINE_APPMODEL_ENTRY
{
    const WCHAR *name;
    const WCHAR *version;
    const WCHAR *publisher;
    const WCHAR *publisher_id;
    const WCHAR *resource_id;
    const WCHAR *architecture;
    const WCHAR *path;
    const WCHAR *app_id;
    const WCHAR *full_name;
    const WCHAR *family_name;
    PACKAGE_VERSION package_version;
    UINT32 processor_architecture;
    BOOL is_framework;
}
WINE_APPMODEL_ENTRY;

LONG WINAPI AppPolicyGetMediaFoundationCodecLoading(HANDLE token, AppPolicyMediaFoundationCodecLoading *policy);
LONG WINAPI AppPolicyGetProcessTerminationMethod(HANDLE token, AppPolicyProcessTerminationMethod *policy);
LONG WINAPI AppPolicyGetShowDeveloperDiagnostic(HANDLE token, AppPolicyShowDeveloperDiagnostic *policy);
LONG WINAPI AppPolicyGetThreadInitializationType(HANDLE token, AppPolicyThreadInitializationType *policy);
LONG WINAPI AppPolicyGetWindowingModel(HANDLE processToken, AppPolicyWindowingModel *policy);
LONG WINAPI GetPackagePath(const PACKAGE_ID *package_id, const UINT32 reserved, UINT32 *length, WCHAR *path);
LONG WINAPI GetPackagesByPackageFamily(const WCHAR *family_name, UINT32 *count, WCHAR **full_names,
        UINT32 *buffer_length, WCHAR *buffer);
LONG WINAPI PackageFullNameFromId(const PACKAGE_ID *package_id, UINT32 *length, WCHAR *full_name);
LONG WINAPI PackageIdFromFullName(const WCHAR *full_name, UINT32 flags, UINT32 *buffer_length, BYTE *buffer);

UINT32 WINAPI WineAppModelGetEntryCount(void);
UINT32 WINAPI WineAppModelGetEntryIndex(const WINE_APPMODEL_ENTRY *entry);
const WINE_APPMODEL_ENTRY * WINAPI WineAppModelGetEntry(UINT32 index);
const WINE_APPMODEL_ENTRY * WINAPI WineAppModelGetEntryByFullName(const WCHAR *full_name);
const WINE_APPMODEL_ENTRY * WINAPI WineAppModelGetCurrentEntry(void);
UINT32 WINAPI WineAppModelGetEntriesByFamily(const WCHAR *family_name,
        const WINE_APPMODEL_ENTRY **entries, UINT32 max_entries);

#if defined(__cplusplus)
}
#endif

#endif /* _APPMODEL_H_ */
