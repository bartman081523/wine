/*
 * User Profile API
 *
 * Copyright 2026 Antigravity
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

#include <stdarg.h>
#include "windef.h"
#include "winbase.h"
#include "winreg.h"
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(profapi);

/***********************************************************************
 *         profapi_114   (profapi.@)
 */
LONG WINAPI profapi_114(const WCHAR *family_name, DWORD flags, const WCHAR *subkey, REGSAM access, HKEY *hkey)
{
    WCHAR key_path[512];
    LONG ret;

    TRACE("(%s, %#lx, %s, %#lx, %p)\n", debugstr_w(family_name), flags, debugstr_w(subkey), access, hkey);

    if (!hkey)
        return ERROR_INVALID_PARAMETER;

    lstrcpyW(key_path, L"Software\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\CurrentVersion\\AppModel\\Repository\\Families");
    if (family_name)
    {
        lstrcatW(key_path, L"\\");
        lstrcatW(key_path, family_name);
    }
    if (subkey)
    {
        lstrcatW(key_path, L"\\");
        lstrcatW(key_path, subkey);
    }

    ret = RegCreateKeyExW(HKEY_CURRENT_USER, key_path, 0, NULL, 0, access ? access : KEY_READ | KEY_WRITE, NULL, hkey, NULL);
    if (ret != ERROR_SUCCESS)
    {
        ret = RegCreateKeyExW(HKEY_CURRENT_USER, key_path, 0, NULL, REG_OPTION_VOLATILE, KEY_ALL_ACCESS, NULL, hkey, NULL);
    }

    TRACE("returning ret=%ld, hkey=%p\n", ret, *hkey);
    return ret;
}
