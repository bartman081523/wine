/*
 * Copyright 2025 Zhiyi Zhang for CodeWeavers
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
#include <stdlib.h>

#include "windef.h"
#include "winbase.h"
#include "winnls.h"
#include "winstring.h"

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(bcp47langs);

HRESULT WINAPI GetFontFallbackLanguageList(const WCHAR *lang, size_t buffer_length, WCHAR *buffer,
                                           size_t *required_buffer_length)
{
    WCHAR locale[LOCALE_NAME_MAX_LENGTH];
    size_t lang_length;

    FIXME("lang %s, buffer_length %Iu, buffer %p, required_buffer_length %p stub!\n",
          wine_dbgstr_w(lang), buffer_length, buffer, required_buffer_length);

    if (!buffer_length || !buffer)
        return E_INVALIDARG;

    if (!lang)
    {
        if (!GetUserDefaultLocaleName(locale, LOCALE_NAME_MAX_LENGTH))
            return E_FAIL;

        lang = locale;
    }

    /* Return the original language as the fallback language list */
    lang_length = wcslen(lang) + 1;
    *required_buffer_length = lang_length;
    if (buffer_length < lang_length)
        return E_NOT_SUFFICIENT_BUFFER;

    memcpy(buffer, lang, lang_length * sizeof(WCHAR));
    return S_OK;
}

HRESULT WINAPI GetUserLanguages(WCHAR delim, HSTRING *langs)
{
    WCHAR locale[LOCALE_NAME_MAX_LENGTH];

    TRACE("%lc, %p\n", delim, langs);

    if (!langs)
        return E_INVALIDARG;

    if (!GetUserDefaultLocaleName(locale, LOCALE_NAME_MAX_LENGTH) || !locale[0])
        wcscpy(locale, L"en-US");

    return WindowsCreateString(locale, wcslen(locale), langs);
}

HRESULT WINAPI GetApplicationLanguages(const WCHAR *package_name, WCHAR delim, HSTRING *langs)
{
    WCHAR locale[LOCALE_NAME_MAX_LENGTH];
    WCHAR list[LOCALE_NAME_MAX_LENGTH * 2 + 10];
    WCHAR sep[2] = { delim ? delim : L';', 0 };
    HRESULT hr;

    TRACE("package_name %s, delim %lc, langs %p\n", wine_dbgstr_w(package_name), delim, langs);

    if (!langs)
        return E_INVALIDARG;

    locale[0] = 0;
    if (!GetUserDefaultLocaleName(locale, LOCALE_NAME_MAX_LENGTH) || !locale[0])
        wcscpy(locale, L"en-US");

    if (_wcsicmp(locale, L"en-US") == 0 || _wcsicmp(locale, L"en") == 0)
    {
        wcscpy(list, L"en-US");
    }
    else
    {
        /* Include user locale followed by en-US fallback */
        swprintf(list, ARRAY_SIZE(list), L"%s%s%s", locale, sep, L"en-US");
    }

    hr = WindowsCreateString(list, wcslen(list), langs);
    TRACE("list %s, hr %#lx, *langs %p\n", wine_dbgstr_w(list), hr, *langs);
    return hr;
}

HRESULT WINAPI LanguageListAsMuiForm(WCHAR delim, HSTRING languages, ULONG flags, HSTRING *mui_languages)
{
    const WCHAR *str;
    UINT32 in_len = 0;
    WCHAR *buf;
    size_t i;
    HRESULT hr;

    TRACE("delim %lc, languages %p, flags %#lx, mui_languages %p\n",
          delim, languages, flags, mui_languages);

    if (!mui_languages)
        return E_INVALIDARG;

    *mui_languages = NULL;

    str = WindowsGetStringRawBuffer(languages, &in_len);
    TRACE("input string: %s (len %u)\n", wine_dbgstr_wn(str, in_len), in_len);

    if (!str || !in_len)
        return S_OK;

    /* Multi-string format requires double null termination.
     * We create an HSTRING of length in_len + 1 where delim is replaced by '\0',
     * followed by an additional '\0', so that PCZZWSTR has two null characters. */
    buf = malloc((in_len + 2) * sizeof(WCHAR));
    if (!buf)
        return E_OUTOFMEMORY;

    memcpy(buf, str, in_len * sizeof(WCHAR));
    for (i = 0; i < in_len; i++)
    {
        if (buf[i] == delim)
            buf[i] = L'\0';
    }
    buf[in_len] = L'\0';
    buf[in_len + 1] = L'\0';

    hr = WindowsCreateString(buf, in_len + 1, mui_languages);
    free(buf);
    return hr;
}
