/* WinRT Windows.Security.Authentication.Web.Core Implementation
 *
 * Copyright 2026 Xodus / Proton-GE contributors
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* GUID definitions */
/* {6ACA7C92-A581-4479-9C10-752EFF44FD34} */
static const GUID IID_IWebAuthenticationCoreManagerStatics =
    {0x6aca7c92, 0xa581, 0x4479, {0x9c, 0x10, 0x75, 0x2e, 0xff, 0x44, 0xfd, 0x34}};

/* {F584184A-8B57-4820-B6A4-70A5B6FCF44A} */
static const GUID IID_IWebAuthenticationCoreManagerStatics2 =
    {0xf584184a, 0x8b57, 0x4820, {0xb6, 0xa4, 0x70, 0xa5, 0xb6, 0xfc, 0xf4, 0x4a}};

/* {29DCC8C3-7AB9-4A7C-A336-B942F9DBF7C7} */
static const GUID IID_IWebAccountProvider =
    {0x29dcc8c3, 0x7ab9, 0x4a7c, {0xa3, 0x36, 0xb9, 0x42, 0xf9, 0xdb, 0xf7, 0xc7}};

/* {4A01EB05-4E42-41D4-B518-E008A5163614} */
static const GUID IID_IWebAccountProvider2 =
    {0x4a01eb05, 0x4e42, 0x41d4, {0xb5, 0x18, 0xe0, 0x08, 0xa5, 0x16, 0x36, 0x14}};

/* {1D767DF1-E1E1-4B9A-A774-5C7C7E3BF371} */
static const GUID IID_IWebAccountProviderFactory =
    {0x1d767df1, 0xe1e1, 0x4b9a, {0xa7, 0x74, 0x5c, 0x7c, 0x7e, 0x3b, 0xf3, 0x71}};

/* {69473EB2-8031-49BE-80BB-96CB46D99ABA} */
static const GUID IID_IWebAccount =
    {0x69473eb2, 0x8031, 0x49be, {0x80, 0xbb, 0x96, 0xcb, 0x46, 0xd9, 0x9a, 0xba}};

/* {080D8C97-6E80-4EFD-8995-19F4A8DC6E7B} */
static const GUID IID_IWebAccount2 =
    {0x080d8c97, 0x6e80, 0x4efd, {0x89, 0x95, 0x19, 0xf4, 0xa8, 0xdc, 0x6e, 0x7b}};

/* {7B56D6F8-990B-4EB5-94A7-5621F3A8B824} */
static const GUID IID_IWebAccount2_Alt =
    {0x7b56d6f8, 0x990b, 0x4eb5, {0x94, 0xa7, 0x56, 0x21, 0xf3, 0xa8, 0xb8, 0x24}};

/* {AC9AFB39-1DE9-4E92-B78F-0581A87F6E5C} */
static const GUID IID_IWebAccountFactory =
    {0xac9afb39, 0x1de9, 0x4e92, {0xb7, 0x8f, 0x05, 0x81, 0xa8, 0x7f, 0x6e, 0x5c}};

/* {B77B4D68-ADCB-4673-B364-0CF7B35CAF97} */
static const GUID IID_IWebTokenRequest =
    {0xb77b4d68, 0xadcb, 0x4673, {0xb3, 0x64, 0x0c, 0xf7, 0xb3, 0x5c, 0xaf, 0x97}};

/* {6CF2141C-0FF0-4C67-B84F-99DDBE4A72C9} */
static const GUID IID_IWebTokenRequestFactory =
    {0x6cf2141c, 0x0ff0, 0x4c67, {0xb8, 0x4f, 0x99, 0xdd, 0xbe, 0x4a, 0x72, 0xc9}};

/* {C12A8305-D1F8-4483-8D54-38FE292784FF} */
static const GUID IID_IWebTokenRequestResult =
    {0xc12a8305, 0xd1f8, 0x4483, {0x8d, 0x54, 0x38, 0xfe, 0x29, 0x27, 0x84, 0xff}};

/* {67A7C5CA-83F6-44C6-A3B1-0EB69E41FA8A} */
static const GUID IID_IWebTokenResponse =
    {0x67a7c5ca, 0x83f6, 0x44c6, {0xa3, 0xb1, 0x0e, 0xb6, 0x9e, 0x41, 0xfa, 0x8a}};

/* {199E065C-8195-55DA-9C10-8AEAF9AC1062} */
static const GUID IID_IVectorView_WebTokenResponse =
    {0x199e065c, 0x8195, 0x55da, {0x9c, 0x10, 0x8a, 0xea, 0xf9, 0xac, 0x10, 0x62}};

/* {7E5BB7EC-BBD7-5575-9A61-F5815FA22A0E} */
static const GUID IID_IIterable_WebTokenResponse =
    {0x7e5bb7ec, 0xbbd7, 0x5575, {0x9a, 0x61, 0xf5, 0x81, 0x5f, 0xa2, 0x2a, 0x0e}};

/* Token cache */
struct xodus_token_cache {
    BOOL loaded;
    WCHAR username[256];
    WCHAR puid[64];
    WCHAR xuid[64];
    WCHAR gamertag[128];
    WCHAR user_hash[64];
    WCHAR age_group[32];
    WCHAR privileges[256];
    WCHAR *token;
};
static struct xodus_token_cache token_cache;

static const char b64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static int b64_val(char c)
{
    const char *p = strchr(b64_table, c);
    return p ? (int)(p - b64_table) : -1;
}

static char *base64_decode(const char *in, int in_len, int *out_len)
{
    int i = 0, j = 0, pad = 0, olen;
    char *out;

    if (in_len >= 2 && in[in_len - 1] == '=') pad++;
    if (in_len >= 2 && in[in_len - 2] == '=') pad++;
    olen = (in_len * 3) / 4 - pad;
    out = malloc(olen + 1);
    if (!out) return NULL;

    while (i < in_len)
    {
        int v[4], k;
        for (k = 0; k < 4; k++)
        {
            if (i < in_len && in[i] != '=')
                v[k] = b64_val(in[i++]);
            else
            {
                v[k] = 0;
                i++;
            }
        }
        if (j < olen) out[j++] = (v[0] << 2) | (v[1] >> 4);
        if (j < olen) out[j++] = ((v[1] & 0xf) << 4) | (v[2] >> 2);
        if (j < olen) out[j++] = ((v[2] & 0x3) << 6) | v[3];
    }
    out[olen] = '\0';
    if (out_len) *out_len = olen;
    return out;
}

static char *json_get_str(const char *json, const char *key)
{
    char pattern[256];
    const char *p, *end;
    char *res;
    int len, ri, i;

    snprintf(pattern, sizeof(pattern), "\"%s\"", key);
    p = strstr(json, pattern);
    if (!p) return NULL;
    p += strlen(pattern);
    while (*p == ' ' || *p == ':' || *p == '\t' || *p == '\r' || *p == '\n') p++;
    if (*p != '\"') return NULL;
    p++;
    end = p;
    while (*end && *end != '\"')
    {
        if (*end == '\\' && *(end + 1)) end += 2;
        else end++;
    }
    len = end - p;
    res = malloc(len + 1);
    ri = 0;
    for (i = 0; i < len; i++)
    {
        if (p[i] == '\\' && p[i+1] == '\"') {
            res[ri++] = '\"';
            i++;
        } else if (p[i] == '\\' && p[i+1] == '\\') {
            res[ri++] = '\\';
            i++;
        } else {
            res[ri++] = p[i];
        }
    }
    res[ri] = '\0';
    return res;
}

static void load_xodus_tokens(void)
{
    WCHAR path[MAX_PATH];
    WCHAR home[MAX_PATH];
    HANDLE hFile;
    DWORD size;

    if (token_cache.loaded) return;

    memset(path, 0, sizeof(path));
    memset(home, 0, sizeof(home));

    if (GetEnvironmentVariableW(L"XODUS_TOKENS_PATH", path, MAX_PATH))
    {
        /* explicit path */
    }
    else if (GetEnvironmentVariableW(L"WINE_HOST_HOME", home, MAX_PATH))
    {
        swprintf(path, MAX_PATH, L"Z:%s/.config/xodus/tokens.json", home);
    }
    else if (GetEnvironmentVariableW(L"WINEHOMEDIR", home, MAX_PATH))
    {
        WCHAR *h = home;
        if (wcsncmp(h, L"\\??\\", 4) == 0) h += 4;
        swprintf(path, MAX_PATH, L"%s\\.config\\xodus\\tokens.json", h);
    }
    else
    {
        wcscpy(path, L"Z:\\home\\julian\\.config\\xodus\\tokens.json");
    }

    {
        WCHAR *p;
        for (p = path; *p; p++) if (*p == '/') *p = '\\';
    }

    if (GetFileAttributesW(path) == INVALID_FILE_ATTRIBUTES)
    {
        wcscpy(path, L"Z:\\home\\julian\\.config\\xodus\\tokens.json");
    }

    hFile = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        TRACE("Could not open xodus tokens from %s\n", debugstr_w(path));
        wcscpy(token_cache.username, L"chlorophyll2730@outlook.com");
        wcscpy(token_cache.puid, L"00034001209222CE");
        token_cache.loaded = TRUE;
        return;
    }

    size = GetFileSize(hFile, NULL);
    if (size != INVALID_FILE_SIZE && size > 0)
    {
        char *buf = malloc(size + 1);
        if (buf)
        {
            DWORD read = 0;
            char *user_da_b64;
            char *user_tokens_b64;
            char *xbox_token_b64;

            ReadFile(hFile, buf, size, &read, NULL);
            buf[read] = '\0';

            user_da_b64 = json_get_str(buf, "user-DA");
            if (user_da_b64)
            {
                int dec_len = 0;
                char *user_da = base64_decode(user_da_b64, strlen(user_da_b64), &dec_len);
                if (user_da)
                {
                    char *username = json_get_str(user_da, "username");
                    char *puid = json_get_str(user_da, "puid");
                    if (username)
                    {
                        MultiByteToWideChar(CP_UTF8, 0, username, -1, token_cache.username, ARRAYSIZE(token_cache.username));
                        free(username);
                    }
                    if (puid)
                    {
                        MultiByteToWideChar(CP_UTF8, 0, puid, -1, token_cache.puid, ARRAYSIZE(token_cache.puid));
                        free(puid);
                    }
                    free(user_da);
                }
                free(user_da_b64);
            }

            xbox_token_b64 = json_get_str(buf, "xbox-token");
            if (xbox_token_b64)
            {
                int dec_len = 0;
                char *xbox_token = base64_decode(xbox_token_b64, strlen(xbox_token_b64), &dec_len);
                if (xbox_token)
                {
                    char *xsts_tok = json_get_str(xbox_token, "Token");
                    char *uhs = json_get_str(xbox_token, "uhs");
                    char *gtg = json_get_str(xbox_token, "gtg");
                    char *xid = json_get_str(xbox_token, "xid");
                    char *agg = json_get_str(xbox_token, "agg");
                    char *prv = json_get_str(xbox_token, "prv");

                    if (uhs)
                    {
                        MultiByteToWideChar(CP_UTF8, 0, uhs, -1, token_cache.user_hash, ARRAYSIZE(token_cache.user_hash));
                        free(uhs);
                    }
                    if (gtg)
                    {
                        MultiByteToWideChar(CP_UTF8, 0, gtg, -1, token_cache.gamertag, ARRAYSIZE(token_cache.gamertag));
                        free(gtg);
                    }
                    if (xid)
                    {
                        MultiByteToWideChar(CP_UTF8, 0, xid, -1, token_cache.xuid, ARRAYSIZE(token_cache.xuid));
                        free(xid);
                    }
                    if (agg)
                    {
                        MultiByteToWideChar(CP_UTF8, 0, agg, -1, token_cache.age_group, ARRAYSIZE(token_cache.age_group));
                        free(agg);
                    }
                    if (prv)
                    {
                        MultiByteToWideChar(CP_UTF8, 0, prv, -1, token_cache.privileges, ARRAYSIZE(token_cache.privileges));
                        free(prv);
                    }
                    if (xsts_tok)
                    {
                        int tok_wlen = MultiByteToWideChar(CP_UTF8, 0, xsts_tok, -1, NULL, 0);
                        token_cache.token = malloc((tok_wlen + 1) * sizeof(WCHAR));
                        if (token_cache.token)
                        {
                            MultiByteToWideChar(CP_UTF8, 0, xsts_tok, -1, token_cache.token, tok_wlen);
                        }
                        free(xsts_tok);
                    }
                    free(xbox_token);
                }
                free(xbox_token_b64);
            }

            if (!token_cache.token)
            {
                user_tokens_b64 = json_get_str(buf, "user-tokens");
                if (user_tokens_b64)
                {
                    int dec_len = 0;
                    char *user_tokens = base64_decode(user_tokens_b64, strlen(user_tokens_b64), &dec_len);
                    if (user_tokens)
                    {
                        char *xbl_token = json_get_str(user_tokens, "scope=service::user.auth.xboxlive.com::MBI_SSL&api-version=2.0");
                        if (!xbl_token)
                        {
                            const char *p = strstr(user_tokens, "\"d=ey");
                            if (p)
                            {
                                const char *q;
                                p++;
                                q = strchr(p, '\"');
                                if (q)
                                {
                                    int tlen = q - p;
                                    xbl_token = malloc(tlen + 1);
                                    memcpy(xbl_token, p, tlen);
                                    xbl_token[tlen] = '\0';
                                }
                            }
                        }

                        if (xbl_token)
                        {
                            int wlen = MultiByteToWideChar(CP_UTF8, 0, xbl_token, -1, NULL, 0);
                            token_cache.token = malloc((wlen + 4) * sizeof(WCHAR));
                            if (token_cache.token)
                            {
                                if (xbl_token[0] == 't' && xbl_token[1] == '=')
                                {
                                    MultiByteToWideChar(CP_UTF8, 0, xbl_token, -1, token_cache.token, wlen);
                                }
                                else
                                {
                                    wcscpy(token_cache.token, L"t=");
                                    MultiByteToWideChar(CP_UTF8, 0, xbl_token, -1, token_cache.token + 2, wlen);
                                }
                            }
                            free(xbl_token);
                        }
                        free(user_tokens);
                    }
                    free(user_tokens_b64);
                }
            }
            free(buf);
        }
    }
    CloseHandle(hFile);

    if (!token_cache.username[0]) wcscpy(token_cache.username, L"chlorophyll2730@outlook.com");
    if (!token_cache.puid[0]) wcscpy(token_cache.puid, L"00034001209222CE");
    token_cache.loaded = TRUE;
    TRACE("Loaded Xodus tokens: user %s, puid %s, has_token %d\n",
          debugstr_w(token_cache.username), debugstr_w(token_cache.puid), token_cache.token != NULL);
}

/* Forward declarations */
static HRESULT map_hstring_hstring_create( void **out );
static HRESULT async_inspectable_create( void *result, HRESULT hr, AsyncStatus status, void **out );
static HRESULT web_account_provider_create( const WCHAR *id_str, const WCHAR *name_str, const WCHAR *auth_str, void **out );
static HRESULT web_account_create( void *provider, const WCHAR *user_str, const WCHAR *id_str, void **out );
static HRESULT web_token_response_create( const WCHAR *token_str, void *account, void **out );
static HRESULT vector_view_response_create( void *response, void **out );
static HRESULT web_token_request_result_create( void *response_view, UINT32 status, void **out );

/* async_inspectable implementation */
struct async_inspectable
{
    struct { const void *lpVtbl; } IInspectable_iface;
    IAsyncInfo IAsyncInfo_iface;
    LONG ref;
    AsyncStatus status;
    HRESULT hr;
    IInspectable *result;
    IInspectable *handler;
};

static inline struct async_inspectable *impl_from_async_inspectable( void *iface )
{
    return CONTAINING_RECORD( iface, struct async_inspectable, IInspectable_iface );
}

static inline struct async_inspectable *impl_from_async_inspectable_IAsyncInfo( IAsyncInfo *iface )
{
    return CONTAINING_RECORD( iface, struct async_inspectable, IAsyncInfo_iface );
}

static HRESULT WINAPI async_inspectable_QueryInterface( void *iface, REFIID iid, void **out )
{
    struct async_inspectable *impl = impl_from_async_inspectable( iface );
    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (!out) return E_POINTER;
    *out = NULL;

    if (IsEqualGUID( iid, &IID_IAsyncInfo ))
    {
        *out = &impl->IAsyncInfo_iface;
        InterlockedIncrement( &impl->ref );
        return S_OK;
    }

    *out = &impl->IInspectable_iface;
    InterlockedIncrement( &impl->ref );
    return S_OK;
}

static ULONG WINAPI async_inspectable_AddRef( void *iface )
{
    struct async_inspectable *impl = impl_from_async_inspectable( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI async_inspectable_Release( void *iface )
{
    struct async_inspectable *impl = impl_from_async_inspectable( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    if (!ref)
    {
        if (impl->handler) IInspectable_Release( impl->handler );
        if (impl->result) IInspectable_Release( impl->result );
        free( impl );
    }
    return ref;
}

static HRESULT WINAPI async_inspectable_GetIids( void *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI async_inspectable_GetRuntimeClassName( void *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.IAsyncOperation`1<IInspectable>", 48, class_name );
}

static HRESULT WINAPI async_inspectable_GetTrustLevel( void *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI async_inspectable_put_Completed( void *iface, void *handler )
{
    struct async_inspectable *impl = impl_from_async_inspectable( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );

    if (impl->handler) IInspectable_Release( impl->handler );
    if ((impl->handler = handler)) IInspectable_AddRef( (IInspectable *)handler );

    if (handler && impl->status != Started)
    {
        HRESULT (STDMETHODCALLTYPE *invoke)(void *This, void *info, AsyncStatus status) =
            ((void **)*(void **)handler)[3];
        invoke( handler, &impl->IInspectable_iface, impl->status );
    }

    return S_OK;
}

static HRESULT WINAPI async_inspectable_get_Completed( void *iface, void **handler )
{
    struct async_inspectable *impl = impl_from_async_inspectable( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );
    if (!handler) return E_POINTER;
    *handler = impl->handler;
    if (*handler) IInspectable_AddRef( (IInspectable *)*handler );
    return S_OK;
}

static HRESULT WINAPI async_inspectable_GetResults( void *iface, void **results )
{
    struct async_inspectable *impl = impl_from_async_inspectable( iface );
    TRACE( "iface %p, results %p.\n", iface, results );
    if (results)
    {
        *results = impl->result;
        if (*results) IInspectable_AddRef( (IInspectable *)*results );
    }
    return impl->hr;
}

static const void *async_inspectable_vtbl[] =
{
    (void *)async_inspectable_QueryInterface,
    (void *)async_inspectable_AddRef,
    (void *)async_inspectable_Release,
    (void *)async_inspectable_GetIids,
    (void *)async_inspectable_GetRuntimeClassName,
    (void *)async_inspectable_GetTrustLevel,
    (void *)async_inspectable_put_Completed,
    (void *)async_inspectable_get_Completed,
    (void *)async_inspectable_GetResults
};

static HRESULT WINAPI async_inspectable_info_QueryInterface( IAsyncInfo *iface, REFIID iid, void **out )
{
    struct async_inspectable *impl = impl_from_async_inspectable_IAsyncInfo( iface );
    return async_inspectable_QueryInterface( &impl->IInspectable_iface, iid, out );
}

static ULONG WINAPI async_inspectable_info_AddRef( IAsyncInfo *iface )
{
    struct async_inspectable *impl = impl_from_async_inspectable_IAsyncInfo( iface );
    return async_inspectable_AddRef( &impl->IInspectable_iface );
}

static ULONG WINAPI async_inspectable_info_Release( IAsyncInfo *iface )
{
    struct async_inspectable *impl = impl_from_async_inspectable_IAsyncInfo( iface );
    return async_inspectable_Release( &impl->IInspectable_iface );
}

static HRESULT WINAPI async_inspectable_info_GetIids( IAsyncInfo *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI async_inspectable_info_GetRuntimeClassName( IAsyncInfo *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.IAsyncInfo", 27, class_name );
}

static HRESULT WINAPI async_inspectable_info_GetTrustLevel( IAsyncInfo *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI async_inspectable_info_get_Id( IAsyncInfo *iface, UINT32 *id )
{
    if (!id) return E_POINTER;
    *id = 1;
    return S_OK;
}

static HRESULT WINAPI async_inspectable_info_get_Status( IAsyncInfo *iface, AsyncStatus *status )
{
    struct async_inspectable *impl = impl_from_async_inspectable_IAsyncInfo( iface );
    if (!status) return E_POINTER;
    *status = impl->status;
    return S_OK;
}

static HRESULT WINAPI async_inspectable_info_get_ErrorCode( IAsyncInfo *iface, HRESULT *error_code )
{
    struct async_inspectable *impl = impl_from_async_inspectable_IAsyncInfo( iface );
    if (!error_code) return E_POINTER;
    *error_code = impl->hr;
    return S_OK;
}

static HRESULT WINAPI async_inspectable_info_Cancel( IAsyncInfo *iface )
{
    return S_OK;
}

static HRESULT WINAPI async_inspectable_info_Close( IAsyncInfo *iface )
{
    return S_OK;
}

static const struct IAsyncInfoVtbl async_inspectable_info_vtbl =
{
    async_inspectable_info_QueryInterface,
    async_inspectable_info_AddRef,
    async_inspectable_info_Release,
    async_inspectable_info_GetIids,
    async_inspectable_info_GetRuntimeClassName,
    async_inspectable_info_GetTrustLevel,
    async_inspectable_info_get_Id,
    async_inspectable_info_get_Status,
    async_inspectable_info_get_ErrorCode,
    async_inspectable_info_Cancel,
    async_inspectable_info_Close
};

static HRESULT async_inspectable_create( void *result, HRESULT hr, AsyncStatus status, void **out )
{
    struct async_inspectable *impl;
    if (!out) return E_POINTER;
    *out = NULL;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IInspectable_iface.lpVtbl = async_inspectable_vtbl;
    impl->IAsyncInfo_iface.lpVtbl = &async_inspectable_info_vtbl;
    impl->ref = 1;
    impl->status = status;
    impl->hr = hr;
    impl->result = (IInspectable *)result;
    if (result) IInspectable_AddRef( (IInspectable *)result );
    *out = &impl->IInspectable_iface;
    TRACE( "created async_inspectable %p, result %p, hr %#lx, status %d\n", *out, result, hr, status );
    return S_OK;
}

/* =========================================================================
 * IMap<HSTRING, HSTRING> implementation
 * ========================================================================= */
struct map_entry
{
    HSTRING key;
    HSTRING value;
};

struct map_hstring_hstring
{
    struct { const void *lpVtbl; } IMap_iface;
    struct { const void *lpVtbl; } IMapView_iface;
    LONG ref;
    UINT32 count;
    UINT32 capacity;
    struct map_entry *entries;
    CRITICAL_SECTION cs;
};

static inline struct map_hstring_hstring *impl_from_IMap( void *iface )
{
    return CONTAINING_RECORD( iface, struct map_hstring_hstring, IMap_iface );
}

static inline struct map_hstring_hstring *impl_from_IMapView( void *iface )
{
    return CONTAINING_RECORD( iface, struct map_hstring_hstring, IMapView_iface );
}

static HRESULT WINAPI map_QueryInterface( void *iface, REFIID riid, void **out )
{
    struct map_hstring_hstring *impl = impl_from_IMap( iface );
    if (!out) return E_POINTER;
    *out = NULL;

    if (IsEqualGUID( riid, &IID_IUnknown ) ||
        IsEqualGUID( riid, &IID_IInspectable ) ||
        IsEqualGUID( riid, &IID_IAgileObject ) ||
        IsEqualGUID( riid, &IID___FIMap_2_HSTRING_HSTRING ))
    {
        *out = &impl->IMap_iface;
        InterlockedIncrement( &impl->ref );
        return S_OK;
    }

    if (IsEqualGUID( riid, &IID___FIMapView_2_HSTRING_HSTRING ))
    {
        *out = &impl->IMapView_iface;
        InterlockedIncrement( &impl->ref );
        return S_OK;
    }

    FIXME( "map %p, %s not implemented\n", iface, debugstr_guid( riid ) );
    return E_NOINTERFACE;
}

static ULONG WINAPI map_AddRef( void *iface )
{
    struct map_hstring_hstring *impl = impl_from_IMap( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI map_Release( void *iface )
{
    struct map_hstring_hstring *impl = impl_from_IMap( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    if (!ref)
    {
        UINT32 i;
        for (i = 0; i < impl->count; i++)
        {
            if (impl->entries[i].key) WindowsDeleteString( impl->entries[i].key );
            if (impl->entries[i].value) WindowsDeleteString( impl->entries[i].value );
        }
        free( impl->entries );
        DeleteCriticalSection( &impl->cs );
        free( impl );
    }
    return ref;
}

static HRESULT WINAPI map_GetIids( void *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI map_GetRuntimeClassName( void *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.Collections.IMap`2<String, String>", 49, class_name );
}

static HRESULT WINAPI map_GetTrustLevel( void *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI map_Lookup( void *iface, HSTRING key, HSTRING *value )
{
    struct map_hstring_hstring *impl = impl_from_IMap( iface );
    const WCHAR *k = WindowsGetStringRawBuffer( key, NULL );
    UINT32 i;
    TRACE( "iface %p, key %s\n", iface, debugstr_w( k ) );
    if (!value) return E_POINTER;
    *value = NULL;
    EnterCriticalSection( &impl->cs );
    for (i = 0; i < impl->count; i++)
    {
        const WCHAR *ek = WindowsGetStringRawBuffer( impl->entries[i].key, NULL );
        if (wcscmp( k, ek ) == 0)
        {
            HRESULT hr = WindowsDuplicateString( impl->entries[i].value, value );
            TRACE( "returning value %s\n", debugstr_w( WindowsGetStringRawBuffer( *value, NULL ) ) );
            LeaveCriticalSection( &impl->cs );
            return hr;
        }
    }
    LeaveCriticalSection( &impl->cs );
    TRACE( "key %s NOT FOUND\n", debugstr_w( k ) );
    return 0x8000000B; /* E_BOUNDS */
}

static HRESULT WINAPI map_get_Size( void *iface, UINT32 *size )
{
    struct map_hstring_hstring *impl = impl_from_IMap( iface );
    if (!size) return E_POINTER;
    *size = impl->count;
    return S_OK;
}

static HRESULT WINAPI map_HasKey( void *iface, HSTRING key, BOOLEAN *found )
{
    struct map_hstring_hstring *impl = impl_from_IMap( iface );
    const WCHAR *k = WindowsGetStringRawBuffer( key, NULL );
    UINT32 i;
    TRACE( "iface %p, key %s\n", iface, debugstr_w( k ) );
    if (!found) return E_POINTER;
    *found = FALSE;
    EnterCriticalSection( &impl->cs );
    for (i = 0; i < impl->count; i++)
    {
        const WCHAR *ek = WindowsGetStringRawBuffer( impl->entries[i].key, NULL );
        if (wcscmp( k, ek ) == 0)
        {
            *found = TRUE;
            break;
        }
    }
    LeaveCriticalSection( &impl->cs );
    TRACE( "returning found %d\n", *found );
    return S_OK;
}

static HRESULT WINAPI map_GetView( void *iface, void **view )
{
    struct map_hstring_hstring *impl = impl_from_IMap( iface );
    if (!view) return E_POINTER;
    *view = &impl->IMapView_iface;
    InterlockedIncrement( &impl->ref );
    return S_OK;
}

static HRESULT WINAPI map_Insert( void *iface, HSTRING key, HSTRING value, BOOLEAN *replaced )
{
    struct map_hstring_hstring *impl = impl_from_IMap( iface );
    const WCHAR *k = WindowsGetStringRawBuffer( key, NULL );
    UINT32 i;

    TRACE( "iface %p, key %s, value %s\n", iface, debugstr_hstring( key ), debugstr_hstring( value ) );

    EnterCriticalSection( &impl->cs );
    for (i = 0; i < impl->count; i++)
    {
        const WCHAR *ek = WindowsGetStringRawBuffer( impl->entries[i].key, NULL );
        if (wcscmp( k, ek ) == 0)
        {
            WindowsDeleteString( impl->entries[i].value );
            WindowsDuplicateString( value, &impl->entries[i].value );
            if (replaced) *replaced = TRUE;
            LeaveCriticalSection( &impl->cs );
            return S_OK;
        }
    }

    if (impl->count >= impl->capacity)
    {
        UINT32 new_cap = impl->capacity ? impl->capacity * 2 : 8;
        struct map_entry *new_entries = realloc( impl->entries, new_cap * sizeof(*new_entries) );
        if (!new_entries)
        {
            LeaveCriticalSection( &impl->cs );
            return E_OUTOFMEMORY;
        }
        impl->entries = new_entries;
        impl->capacity = new_cap;
    }

    WindowsDuplicateString( key, &impl->entries[impl->count].key );
    WindowsDuplicateString( value, &impl->entries[impl->count].value );
    impl->count++;
    if (replaced) *replaced = FALSE;
    LeaveCriticalSection( &impl->cs );
    return S_OK;
}

static HRESULT WINAPI map_Remove( void *iface, HSTRING key )
{
    struct map_hstring_hstring *impl = impl_from_IMap( iface );
    const WCHAR *k = WindowsGetStringRawBuffer( key, NULL );
    UINT32 i;
    EnterCriticalSection( &impl->cs );
    for (i = 0; i < impl->count; i++)
    {
        const WCHAR *ek = WindowsGetStringRawBuffer( impl->entries[i].key, NULL );
        if (wcscmp( k, ek ) == 0)
        {
            WindowsDeleteString( impl->entries[i].key );
            WindowsDeleteString( impl->entries[i].value );
            if (i < impl->count - 1)
                memmove( &impl->entries[i], &impl->entries[i + 1], (impl->count - 1 - i) * sizeof(struct map_entry) );
            impl->count--;
            LeaveCriticalSection( &impl->cs );
            return S_OK;
        }
    }
    LeaveCriticalSection( &impl->cs );
    return 0x8000000B; /* E_BOUNDS */
}

static HRESULT WINAPI map_Clear( void *iface )
{
    struct map_hstring_hstring *impl = impl_from_IMap( iface );
    UINT32 i;
    EnterCriticalSection( &impl->cs );
    for (i = 0; i < impl->count; i++)
    {
        WindowsDeleteString( impl->entries[i].key );
        WindowsDeleteString( impl->entries[i].value );
    }
    impl->count = 0;
    LeaveCriticalSection( &impl->cs );
    return S_OK;
}

static const void *map_vtbl[] =
{
    (void *)map_QueryInterface,         /* 0: 0x00 */
    (void *)map_AddRef,                 /* 1: 0x08 */
    (void *)map_Release,                /* 2: 0x10 */
    (void *)map_GetIids,                /* 3: 0x18 */
    (void *)map_GetRuntimeClassName,    /* 4: 0x20 */
    (void *)map_GetTrustLevel,          /* 5: 0x28 */
    (void *)map_Lookup,                 /* 6: 0x30 */
    (void *)map_get_Size,               /* 7: 0x38 */
    (void *)map_HasKey,                 /* 8: 0x40 */
    (void *)map_GetView,                /* 9: 0x48 */
    (void *)map_Insert,                 /* 10: 0x50 */
    (void *)map_Remove,                 /* 11: 0x58 */
    (void *)map_Clear                   /* 12: 0x60 */
};

static HRESULT WINAPI map_view_QueryInterface( void *iface, REFIID riid, void **out )
{
    struct map_hstring_hstring *impl = impl_from_IMapView( iface );
    return map_QueryInterface( &impl->IMap_iface, riid, out );
}

static ULONG WINAPI map_view_AddRef( void *iface )
{
    struct map_hstring_hstring *impl = impl_from_IMapView( iface );
    return map_AddRef( &impl->IMap_iface );
}

static ULONG WINAPI map_view_Release( void *iface )
{
    struct map_hstring_hstring *impl = impl_from_IMapView( iface );
    return map_Release( &impl->IMap_iface );
}

static HRESULT WINAPI map_view_Lookup( void *iface, HSTRING key, HSTRING *value )
{
    struct map_hstring_hstring *impl = impl_from_IMapView( iface );
    return map_Lookup( &impl->IMap_iface, key, value );
}

static HRESULT WINAPI map_view_get_Size( void *iface, UINT32 *size )
{
    struct map_hstring_hstring *impl = impl_from_IMapView( iface );
    return map_get_Size( &impl->IMap_iface, size );
}

static HRESULT WINAPI map_view_HasKey( void *iface, HSTRING key, BOOLEAN *found )
{
    struct map_hstring_hstring *impl = impl_from_IMapView( iface );
    return map_HasKey( &impl->IMap_iface, key, found );
}

static HRESULT WINAPI map_view_Split( void *iface, void **first, void **second )
{
    struct map_hstring_hstring *impl = impl_from_IMapView( iface );
    if (!first || !second) return E_POINTER;
    *first = &impl->IMapView_iface;
    InterlockedIncrement( &impl->ref );
    *second = NULL;
    return S_OK;
}

static const void *map_view_vtbl[] =
{
    (void *)map_view_QueryInterface,     /* 0: 0x00 */
    (void *)map_view_AddRef,             /* 1: 0x08 */
    (void *)map_view_Release,            /* 2: 0x10 */
    (void *)map_GetIids,                 /* 3: 0x18 */
    (void *)map_GetRuntimeClassName,     /* 4: 0x20 */
    (void *)map_GetTrustLevel,           /* 5: 0x28 */
    (void *)map_view_Lookup,             /* 6: 0x30 */
    (void *)map_view_get_Size,           /* 7: 0x38 */
    (void *)map_view_HasKey,             /* 8: 0x40 */
    (void *)map_view_Split               /* 9: 0x48 */
};

static HRESULT map_hstring_hstring_create( void **out )
{
    struct map_hstring_hstring *impl;
    if (!out) return E_POINTER;
    *out = NULL;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IMap_iface.lpVtbl = map_vtbl;
    impl->IMapView_iface.lpVtbl = map_view_vtbl;
    impl->ref = 1;
    InitializeCriticalSection( &impl->cs );
    *out = &impl->IMap_iface;
    return S_OK;
}

/* =========================================================================
 * WebAccountProvider implementation
 * ========================================================================= */
struct web_account_provider
{
    struct { const void *lpVtbl; } IWebAccountProvider_iface;
    struct { const void *lpVtbl; } IWebAccountProvider2_iface;
    LONG ref;
    HSTRING id;
    HSTRING display_name;
    HSTRING authority;
};

static inline struct web_account_provider *impl_from_IWebAccountProvider( void *iface )
{
    return CONTAINING_RECORD( iface, struct web_account_provider, IWebAccountProvider_iface );
}

static inline struct web_account_provider *impl_from_IWebAccountProvider2( void *iface )
{
    return CONTAINING_RECORD( iface, struct web_account_provider, IWebAccountProvider2_iface );
}

static HRESULT WINAPI web_account_provider_QueryInterface( void *iface, REFIID riid, void **out )
{
    struct web_account_provider *impl = impl_from_IWebAccountProvider( iface );
    if (!out) return E_POINTER;
    *out = NULL;

    if (IsEqualGUID( riid, &IID_IUnknown ) ||
        IsEqualGUID( riid, &IID_IInspectable ) ||
        IsEqualGUID( riid, &IID_IAgileObject ) ||
        IsEqualGUID( riid, &IID_IWebAccountProvider ))
    {
        *out = &impl->IWebAccountProvider_iface;
        InterlockedIncrement( &impl->ref );
        return S_OK;
    }
    if (IsEqualGUID( riid, &IID_IWebAccountProvider2 ))
    {
        *out = &impl->IWebAccountProvider2_iface;
        InterlockedIncrement( &impl->ref );
        return S_OK;
    }

    FIXME( "provider %p, %s not implemented\n", iface, debugstr_guid( riid ) );
    return E_NOINTERFACE;
}

static ULONG WINAPI web_account_provider_AddRef( void *iface )
{
    struct web_account_provider *impl = impl_from_IWebAccountProvider( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI web_account_provider_Release( void *iface )
{
    struct web_account_provider *impl = impl_from_IWebAccountProvider( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    if (!ref)
    {
        if (impl->id) WindowsDeleteString( impl->id );
        if (impl->display_name) WindowsDeleteString( impl->display_name );
        if (impl->authority) WindowsDeleteString( impl->authority );
        free( impl );
    }
    return ref;
}

static HRESULT WINAPI web_account_provider_GetIids( void *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI web_account_provider_GetRuntimeClassName( void *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Security.Credentials.WebAccountProvider", 46, class_name );
}

static HRESULT WINAPI web_account_provider_GetTrustLevel( void *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI web_account_provider_get_Id( void *iface, HSTRING *value )
{
    struct web_account_provider *impl = impl_from_IWebAccountProvider( iface );
    if (!value) return E_POINTER;
    return WindowsDuplicateString( impl->id, value );
}

static HRESULT WINAPI web_account_provider_get_DisplayName( void *iface, HSTRING *value )
{
    struct web_account_provider *impl = impl_from_IWebAccountProvider( iface );
    if (!value) return E_POINTER;
    return WindowsDuplicateString( impl->display_name, value );
}

static HRESULT WINAPI web_account_provider_get_IconUri( void *iface, void **value )
{
    if (value) *value = NULL;
    return S_OK;
}

static const void *web_account_provider1_vtbl[] =
{
    (void *)web_account_provider_QueryInterface,
    (void *)web_account_provider_AddRef,
    (void *)web_account_provider_Release,
    (void *)web_account_provider_GetIids,
    (void *)web_account_provider_GetRuntimeClassName,
    (void *)web_account_provider_GetTrustLevel,
    (void *)web_account_provider_get_Id,
    (void *)web_account_provider_get_DisplayName,
    (void *)web_account_provider_get_IconUri
};

static HRESULT WINAPI web_account_provider2_QueryInterface( void *iface, REFIID riid, void **out )
{
    struct web_account_provider *impl = impl_from_IWebAccountProvider2( iface );
    return web_account_provider_QueryInterface( &impl->IWebAccountProvider_iface, riid, out );
}

static ULONG WINAPI web_account_provider2_AddRef( void *iface )
{
    struct web_account_provider *impl = impl_from_IWebAccountProvider2( iface );
    return web_account_provider_AddRef( &impl->IWebAccountProvider_iface );
}

static ULONG WINAPI web_account_provider2_Release( void *iface )
{
    struct web_account_provider *impl = impl_from_IWebAccountProvider2( iface );
    return web_account_provider_Release( &impl->IWebAccountProvider_iface );
}

static HRESULT WINAPI web_account_provider2_get_DisplayPurpose( void *iface, HSTRING *value )
{
    if (!value) return E_POINTER;
    return WindowsCreateString( L"", 0, value );
}

static HRESULT WINAPI web_account_provider2_get_Authority( void *iface, HSTRING *value )
{
    struct web_account_provider *impl = impl_from_IWebAccountProvider2( iface );
    if (!value) return E_POINTER;
    return WindowsDuplicateString( impl->authority, value );
}

static const void *web_account_provider2_vtbl[] =
{
    (void *)web_account_provider2_QueryInterface,
    (void *)web_account_provider2_AddRef,
    (void *)web_account_provider2_Release,
    (void *)web_account_provider_GetIids,
    (void *)web_account_provider_GetRuntimeClassName,
    (void *)web_account_provider_GetTrustLevel,
    (void *)web_account_provider2_get_DisplayPurpose,
    (void *)web_account_provider2_get_Authority
};

static HRESULT web_account_provider_create( const WCHAR *id_str, const WCHAR *name_str, const WCHAR *auth_str, void **out )
{
    struct web_account_provider *impl;
    if (!out) return E_POINTER;
    *out = NULL;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IWebAccountProvider_iface.lpVtbl = web_account_provider1_vtbl;
    impl->IWebAccountProvider2_iface.lpVtbl = web_account_provider2_vtbl;
    impl->ref = 1;

    WindowsCreateString( id_str ? id_str : L"https://login.microsoft.com",
                        wcslen( id_str ? id_str : L"https://login.microsoft.com" ), &impl->id );
    WindowsCreateString( name_str ? name_str : L"Microsoft account",
                        wcslen( name_str ? name_str : L"Microsoft account" ), &impl->display_name );
    WindowsCreateString( auth_str ? auth_str : L"consumers",
                        wcslen( auth_str ? auth_str : L"consumers" ), &impl->authority );

    *out = &impl->IWebAccountProvider_iface;
    return S_OK;
}

/* =========================================================================
 * WebAccount implementation
 * ========================================================================= */
struct web_account
{
    struct { const void *lpVtbl; } IWebAccount_iface;
    struct { const void *lpVtbl; } IWebAccount2_iface;
    LONG ref;
    void *provider;
    HSTRING username;
    HSTRING id;
    UINT32 state;
    void *properties;
};

static inline struct web_account *impl_from_IWebAccount( void *iface )
{
    return CONTAINING_RECORD( iface, struct web_account, IWebAccount_iface );
}

static inline struct web_account *impl_from_IWebAccount2( void *iface )
{
    return CONTAINING_RECORD( iface, struct web_account, IWebAccount2_iface );
}

static HRESULT WINAPI web_account_QueryInterface( void *iface, REFIID riid, void **out )
{
    struct web_account *impl = impl_from_IWebAccount( iface );
    if (!out) return E_POINTER;
    *out = NULL;

    if (IsEqualGUID( riid, &IID_IUnknown ) ||
        IsEqualGUID( riid, &IID_IInspectable ) ||
        IsEqualGUID( riid, &IID_IAgileObject ) ||
        IsEqualGUID( riid, &IID_IWebAccount ))
    {
        *out = &impl->IWebAccount_iface;
        InterlockedIncrement( &impl->ref );
        return S_OK;
    }
    if (IsEqualGUID( riid, &IID_IWebAccount2 ) ||
        IsEqualGUID( riid, &IID_IWebAccount2_Alt ))
    {
        *out = &impl->IWebAccount2_iface;
        InterlockedIncrement( &impl->ref );
        return S_OK;
    }

    FIXME( "account %p, %s not implemented\n", iface, debugstr_guid( riid ) );
    return E_NOINTERFACE;
}

static ULONG WINAPI web_account_AddRef( void *iface )
{
    struct web_account *impl = impl_from_IWebAccount( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI web_account_Release( void *iface )
{
    struct web_account *impl = impl_from_IWebAccount( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    if (!ref)
    {
        if (impl->provider) IInspectable_Release( (IInspectable *)impl->provider );
        if (impl->properties) IInspectable_Release( (IInspectable *)impl->properties );
        if (impl->username) WindowsDeleteString( impl->username );
        if (impl->id) WindowsDeleteString( impl->id );
        free( impl );
    }
    return ref;
}

static HRESULT WINAPI web_account_GetIids( void *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI web_account_GetRuntimeClassName( void *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Security.Credentials.WebAccount", 38, class_name );
}

static HRESULT WINAPI web_account_GetTrustLevel( void *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI web_account1_get_WebAccountProvider( void *iface, void **value )
{
    struct web_account *impl = impl_from_IWebAccount( iface );
    if (!value) return E_POINTER;
    *value = impl->provider;
    if (*value) IInspectable_AddRef( (IInspectable *)*value );
    return S_OK;
}

static HRESULT WINAPI web_account1_get_UserName( void *iface, HSTRING *value )
{
    struct web_account *impl = impl_from_IWebAccount( iface );
    if (!value) return E_POINTER;
    return WindowsDuplicateString( impl->username, value );
}

static HRESULT WINAPI web_account1_get_State( void *iface, UINT32 *value )
{
    struct web_account *impl = impl_from_IWebAccount( iface );
    if (!value) return E_POINTER;
    *value = impl->state;
    return S_OK;
}

static const void *web_account1_vtbl[] =
{
    (void *)web_account_QueryInterface,
    (void *)web_account_AddRef,
    (void *)web_account_Release,
    (void *)web_account_GetIids,
    (void *)web_account_GetRuntimeClassName,
    (void *)web_account_GetTrustLevel,
    (void *)web_account1_get_WebAccountProvider,
    (void *)web_account1_get_UserName,
    (void *)web_account1_get_State
};

static HRESULT WINAPI web_account2_QueryInterface( void *iface, REFIID riid, void **out )
{
    struct web_account *impl = impl_from_IWebAccount2( iface );
    return web_account_QueryInterface( &impl->IWebAccount_iface, riid, out );
}

static ULONG WINAPI web_account2_AddRef( void *iface )
{
    struct web_account *impl = impl_from_IWebAccount2( iface );
    return web_account_AddRef( &impl->IWebAccount_iface );
}

static ULONG WINAPI web_account2_Release( void *iface )
{
    struct web_account *impl = impl_from_IWebAccount2( iface );
    return web_account_Release( &impl->IWebAccount_iface );
}

static HRESULT WINAPI web_account2_get_Id( void *iface, HSTRING *value )
{
    struct web_account *impl = impl_from_IWebAccount2( iface );
    if (!value) return E_POINTER;
    return WindowsDuplicateString( impl->id, value );
}

static HRESULT WINAPI web_account2_get_Properties( void *iface, void **value )
{
    struct web_account *impl = impl_from_IWebAccount2( iface );
    if (!value) return E_POINTER;
    *value = impl->properties;
    if (*value) IInspectable_AddRef( (IInspectable *)*value );
    return S_OK;
}

static HRESULT WINAPI web_account2_GetPictureAsync( void *iface, UINT32 size, void **asyncInfo )
{
    return async_inspectable_create( NULL, S_OK, Completed, asyncInfo );
}

static HRESULT WINAPI web_account2_SignOutAsync( void *iface, void **asyncInfo )
{
    return async_inspectable_create( NULL, S_OK, Completed, asyncInfo );
}

static HRESULT WINAPI web_account2_SignOutWithClientIdAsync( void *iface, HSTRING clientId, void **asyncInfo )
{
    return async_inspectable_create( NULL, S_OK, Completed, asyncInfo );
}

static const void *web_account2_vtbl[] =
{
    (void *)web_account2_QueryInterface,
    (void *)web_account2_AddRef,
    (void *)web_account2_Release,
    (void *)web_account_GetIids,
    (void *)web_account_GetRuntimeClassName,
    (void *)web_account_GetTrustLevel,
    (void *)web_account2_get_Id,
    (void *)web_account2_get_Properties,
    (void *)web_account2_GetPictureAsync,
    (void *)web_account2_SignOutAsync,
    (void *)web_account2_SignOutWithClientIdAsync
};

static void map_insert_str( void *map, const WCHAR *key, const WCHAR *val )
{
    HSTRING hkey = NULL, hval = NULL;
    if (!map || !key) return;
    WindowsCreateString( key, wcslen( key ), &hkey );
    WindowsCreateString( val ? val : L"", wcslen( val ? val : L"" ), &hval );
    map_Insert( map, hkey, hval, NULL );
    WindowsDeleteString( hkey );
    WindowsDeleteString( hval );
}

static void get_xuid_dec( const WCHAR *puid_hex, WCHAR *out_dec, size_t max_len )
{
    unsigned __int64 val = 0;
    if (puid_hex && *puid_hex)
    {
        val = wcstoull( puid_hex, NULL, 16 );
    }
    if (val)
        swprintf( out_dec, max_len, L"%llu", (unsigned long long)val );
    else
        wcscpy( out_dec, L"914798515725006" );
}

static void get_gamertag( const WCHAR *username, WCHAR *out_gt, size_t max_len )
{
    const WCHAR *at;
    if (!username || !*username)
    {
        lstrcpynW( out_gt, L"chlorophyll2730", max_len );
        return;
    }
    at = wcschr( username, L'@' );
    if (at && (size_t)(at - username) < max_len)
    {
        size_t len = at - username;
        memcpy( out_gt, username, len * sizeof(WCHAR) );
        out_gt[len] = L'\0';
    }
    else
    {
        lstrcpynW( out_gt, username, max_len );
    }
}

static HRESULT web_account_create( void *provider, const WCHAR *user_str, const WCHAR *id_str, void **out )
{
    struct web_account *impl;
    WCHAR xuid_str[64];
    WCHAR gamertag[128];
    const WCHAR *eff_id;

    if (!out) return E_POINTER;
    *out = NULL;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IWebAccount_iface.lpVtbl = web_account1_vtbl;
    impl->IWebAccount2_iface.lpVtbl = web_account2_vtbl;
    impl->ref = 1;
    impl->provider = provider;
    if (provider) IInspectable_AddRef( (IInspectable *)provider );
    impl->state = 1; /* WebAccountState_Connected */

    load_xodus_tokens();
    if (token_cache.xuid[0])
        wcscpy(xuid_str, token_cache.xuid);
    else
        get_xuid_dec( token_cache.puid, xuid_str, ARRAYSIZE(xuid_str) );

    if (token_cache.gamertag[0])
        wcscpy(gamertag, token_cache.gamertag);
    else
        get_gamertag( token_cache.username, gamertag, ARRAYSIZE(gamertag) );

    eff_id = id_str ? id_str : (token_cache.xuid[0] ? token_cache.xuid : token_cache.puid);
    WindowsCreateString( user_str ? user_str : L"", wcslen( user_str ? user_str : L"" ), &impl->username );
    WindowsCreateString( eff_id, wcslen( eff_id ), &impl->id );
    map_hstring_hstring_create( &impl->properties );

    if (impl->properties)
    {
        map_insert_str( impl->properties, L"XboxUserId", xuid_str );
        map_insert_str( impl->properties, L"Gamertag", gamertag );
        map_insert_str( impl->properties, L"Sandbox", L"RETAIL" );
        if (token_cache.user_hash[0])
            map_insert_str( impl->properties, L"UserHash", token_cache.user_hash );
    }

    *out = &impl->IWebAccount_iface;
    return S_OK;
}

/* =========================================================================
 * WebTokenResponse implementation
 * ========================================================================= */
struct web_token_response
{
    struct { const void *lpVtbl; } IWebTokenResponse_iface;
    LONG ref;
    HSTRING token;
    void *account;
    void *properties;
};

static inline struct web_token_response *impl_from_IWebTokenResponse( void *iface )
{
    return CONTAINING_RECORD( iface, struct web_token_response, IWebTokenResponse_iface );
}

static HRESULT WINAPI web_token_response_QueryInterface( void *iface, REFIID riid, void **out )
{
    struct web_token_response *impl = impl_from_IWebTokenResponse( iface );
    TRACE( "iface %p, riid %s\n", iface, debugstr_guid( riid ) );
    if (!out) return E_POINTER;
    *out = NULL;

    if (IsEqualGUID( riid, &IID_IUnknown ) ||
        IsEqualGUID( riid, &IID_IInspectable ) ||
        IsEqualGUID( riid, &IID_IAgileObject ) ||
        IsEqualGUID( riid, &IID_IWebTokenResponse ))
    {
        *out = &impl->IWebTokenResponse_iface;
        InterlockedIncrement( &impl->ref );
        return S_OK;
    }

    FIXME( "response %p, %s not implemented\n", iface, debugstr_guid( riid ) );
    return E_NOINTERFACE;
}

static ULONG WINAPI web_token_response_AddRef( void *iface )
{
    struct web_token_response *impl = impl_from_IWebTokenResponse( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI web_token_response_Release( void *iface )
{
    struct web_token_response *impl = impl_from_IWebTokenResponse( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    if (!ref)
    {
        if (impl->token) WindowsDeleteString( impl->token );
        if (impl->account) IInspectable_Release( (IInspectable *)impl->account );
        if (impl->properties) IInspectable_Release( (IInspectable *)impl->properties );
        free( impl );
    }
    return ref;
}

static HRESULT WINAPI web_token_response_GetIids( void *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI web_token_response_GetRuntimeClassName( void *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Security.Authentication.Web.Core.WebTokenResponse", 58, class_name );
}

static HRESULT WINAPI web_token_response_GetTrustLevel( void *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI web_token_response_get_Token( void *iface, HSTRING *value )
{
    struct web_token_response *impl = impl_from_IWebTokenResponse( iface );
    if (!value) return E_POINTER;
    TRACE( "caller %p, returning token %s\n", __builtin_return_address(0), debugstr_hstring( impl->token ) );
    return WindowsDuplicateString( impl->token, value );
}

static HRESULT WINAPI web_token_response_get_ProviderError( void *iface, void **value )
{
    TRACE( "caller %p\n", __builtin_return_address(0) );
    if (value) *value = NULL;
    return S_OK;
}

static HRESULT WINAPI web_token_response_get_WebAccount( void *iface, void **value )
{
    struct web_token_response *impl = impl_from_IWebTokenResponse( iface );
    TRACE( "caller %p, iface %p, returning account %p\n", __builtin_return_address(0), iface, impl->account );
    if (!value) return E_POINTER;
    *value = impl->account;
    if (*value) IInspectable_AddRef( (IInspectable *)*value );
    return S_OK;
}

static HRESULT WINAPI web_token_response_get_Properties( void *iface, void **value )
{
    struct web_token_response *impl = impl_from_IWebTokenResponse( iface );
    TRACE( "caller %p, iface %p, returning properties %p\n", __builtin_return_address(0), iface, impl->properties );
    if (!value) return E_POINTER;
    *value = impl->properties;
    if (*value) IInspectable_AddRef( (IInspectable *)*value );
    return S_OK;
}

static const void *web_token_response_vtbl[] =
{
    (void *)web_token_response_QueryInterface,
    (void *)web_token_response_AddRef,
    (void *)web_token_response_Release,
    (void *)web_token_response_GetIids,
    (void *)web_token_response_GetRuntimeClassName,
    (void *)web_token_response_GetTrustLevel,
    (void *)web_token_response_get_Token,
    (void *)web_token_response_get_ProviderError,
    (void *)web_token_response_get_WebAccount,
    (void *)web_token_response_get_Properties
};

static HRESULT web_token_response_create( const WCHAR *token_str, void *account, void **out )
{
    struct web_token_response *impl;
    WCHAR xuid_str[64];
    WCHAR gamertag[128];

    if (!out) return E_POINTER;
    *out = NULL;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IWebTokenResponse_iface.lpVtbl = web_token_response_vtbl;
    impl->ref = 1;
    impl->account = account;
    if (account) IInspectable_AddRef( (IInspectable *)account );

    load_xodus_tokens();
    if (token_cache.xuid[0])
        wcscpy(xuid_str, token_cache.xuid);
    else
        get_xuid_dec( token_cache.puid, xuid_str, ARRAYSIZE(xuid_str) );

    if (token_cache.gamertag[0])
        wcscpy(gamertag, token_cache.gamertag);
    else
        get_gamertag( token_cache.username, gamertag, ARRAYSIZE(gamertag) );

    WindowsCreateString( token_str ? token_str : L"", wcslen( token_str ? token_str : L"" ), &impl->token );
    map_hstring_hstring_create( &impl->properties );
    if (impl->properties)
    {
        map_insert_str( impl->properties, L"Sandbox", L"RETAIL" );
        map_insert_str( impl->properties, L"XboxUserId", xuid_str );
        map_insert_str( impl->properties, L"Gamertag", gamertag );
        map_insert_str( impl->properties, L"ModernGamertag", gamertag );
        map_insert_str( impl->properties, L"ModernGamertagSuffix", L"" );
        map_insert_str( impl->properties, L"UniqueModernGamertag", gamertag );
        map_insert_str( impl->properties, L"AgeGroup", token_cache.age_group[0] ? token_cache.age_group : L"Adult" );
        map_insert_str( impl->properties, L"Privileges", token_cache.privileges );
        map_insert_str( impl->properties, L"TitleRestrictions", L"" );
        if (token_cache.user_hash[0])
            map_insert_str( impl->properties, L"UserHash", token_cache.user_hash );
    }
    *out = &impl->IWebTokenResponse_iface;
    return S_OK;
}

/* =========================================================================
 * IVectorView<WebTokenResponse*> and IIterable<WebTokenResponse*> implementation
 * ========================================================================= */
struct vector_view_response
{
    struct { const void *lpVtbl; } IVectorView_iface;
    struct { const void *lpVtbl; } IIterable_iface;
    LONG ref;
    void *item;
};

/* Forward declaration */
static HRESULT iterator_response_create( void *item, void **out );

static inline struct vector_view_response *impl_from_IVectorView( void *iface )
{
    return CONTAINING_RECORD( iface, struct vector_view_response, IVectorView_iface );
}

static inline struct vector_view_response *impl_from_IIterable( void *iface )
{
    return CONTAINING_RECORD( iface, struct vector_view_response, IIterable_iface );
}

static HRESULT WINAPI vector_view_QueryInterface( void *iface, REFIID riid, void **out )
{
    struct vector_view_response *impl = impl_from_IVectorView( iface );
    TRACE( "iface %p, riid %s\n", iface, debugstr_guid( riid ) );
    if (!out) return E_POINTER;
    *out = NULL;

    if (IsEqualGUID( riid, &IID_IUnknown ) ||
        IsEqualGUID( riid, &IID_IInspectable ) ||
        IsEqualGUID( riid, &IID_IAgileObject ) ||
        IsEqualGUID( riid, &IID_IVectorView_WebTokenResponse ))
    {
        *out = &impl->IVectorView_iface;
        InterlockedIncrement( &impl->ref );
        return S_OK;
    }

    if (IsEqualGUID( riid, &IID_IIterable_WebTokenResponse ))
    {
        *out = &impl->IIterable_iface;
        InterlockedIncrement( &impl->ref );
        return S_OK;
    }

    *out = &impl->IVectorView_iface;
    InterlockedIncrement( &impl->ref );
    return S_OK;
}

static ULONG WINAPI vector_view_AddRef( void *iface )
{
    struct vector_view_response *impl = impl_from_IVectorView( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI vector_view_Release( void *iface )
{
    struct vector_view_response *impl = impl_from_IVectorView( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    if (!ref)
    {
        if (impl->item) IInspectable_Release( (IInspectable *)impl->item );
        free( impl );
    }
    return ref;
}

static HRESULT WINAPI vector_view_GetIids( void *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI vector_view_GetRuntimeClassName( void *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.Collections.IVectorView`1<Windows.Security.Authentication.Web.Core.WebTokenResponse>", 103, class_name );
}

static HRESULT WINAPI vector_view_GetTrustLevel( void *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI vector_view_GetAt( void *iface, UINT32 index, void **item )
{
    struct vector_view_response *impl = impl_from_IVectorView( iface );
    TRACE( "iface %p, index %u, item %p\n", iface, index, item );
    if (!item) return E_POINTER;
    if (index == 0 && impl->item)
    {
        *item = impl->item;
        IInspectable_AddRef( (IInspectable *)impl->item );
        return S_OK;
    }
    return 0x8000000B; /* E_BOUNDS */
}

static HRESULT WINAPI vector_view_get_Size( void *iface, UINT32 *size )
{
    struct vector_view_response *impl = impl_from_IVectorView( iface );
    TRACE( "iface %p, size %p\n", iface, size );
    if (!size) return E_POINTER;
    *size = impl->item ? 1 : 0;
    return S_OK;
}

static HRESULT WINAPI vector_view_IndexOf( void *iface, void *value, UINT32 *index, BOOLEAN *found )
{
    struct vector_view_response *impl = impl_from_IVectorView( iface );
    if (!index || !found) return E_POINTER;
    *index = 0;
    *found = (impl->item != NULL);
    return S_OK;
}

static HRESULT WINAPI vector_view_GetMany( void *iface, UINT32 start, UINT32 capacity, void **items, UINT32 *actual )
{
    struct vector_view_response *impl = impl_from_IVectorView( iface );
    if (!actual) return E_POINTER;
    if (start == 0 && capacity >= 1 && impl->item)
    {
        items[0] = impl->item;
        IInspectable_AddRef( (IInspectable *)impl->item );
        *actual = 1;
    }
    else
    {
        *actual = 0;
    }
    return S_OK;
}

static const void *vector_view_vtbl[] =
{
    (void *)vector_view_QueryInterface,
    (void *)vector_view_AddRef,
    (void *)vector_view_Release,
    (void *)vector_view_GetIids,
    (void *)vector_view_GetRuntimeClassName,
    (void *)vector_view_GetTrustLevel,
    (void *)vector_view_GetAt,
    (void *)vector_view_get_Size,
    (void *)vector_view_IndexOf,
    (void *)vector_view_GetMany
};

/* IIterable methods */
static HRESULT WINAPI iterable_QueryInterface( void *iface, REFIID riid, void **out )
{
    struct vector_view_response *impl = impl_from_IIterable( iface );
    return vector_view_QueryInterface( &impl->IVectorView_iface, riid, out );
}

static ULONG WINAPI iterable_AddRef( void *iface )
{
    struct vector_view_response *impl = impl_from_IIterable( iface );
    return vector_view_AddRef( &impl->IVectorView_iface );
}

static ULONG WINAPI iterable_Release( void *iface )
{
    struct vector_view_response *impl = impl_from_IIterable( iface );
    return vector_view_Release( &impl->IVectorView_iface );
}

static HRESULT WINAPI iterable_GetIids( void *iface, ULONG *iid_count, IID **iids )
{
    struct vector_view_response *impl = impl_from_IIterable( iface );
    return vector_view_GetIids( &impl->IVectorView_iface, iid_count, iids );
}

static HRESULT WINAPI iterable_GetRuntimeClassName( void *iface, HSTRING *class_name )
{
    struct vector_view_response *impl = impl_from_IIterable( iface );
    return vector_view_GetRuntimeClassName( &impl->IVectorView_iface, class_name );
}

static HRESULT WINAPI iterable_GetTrustLevel( void *iface, TrustLevel *trust_level )
{
    struct vector_view_response *impl = impl_from_IIterable( iface );
    return vector_view_GetTrustLevel( &impl->IVectorView_iface, trust_level );
}

static HRESULT WINAPI iterable_First( void *iface, void **first )
{
    struct vector_view_response *impl = impl_from_IIterable( iface );
    TRACE( "iface %p, first %p, item %p\n", iface, first, impl->item );
    return iterator_response_create( impl->item, first );
}

static const void *iterable_response_vtbl[] =
{
    (void *)iterable_QueryInterface,
    (void *)iterable_AddRef,
    (void *)iterable_Release,
    (void *)iterable_GetIids,
    (void *)iterable_GetRuntimeClassName,
    (void *)iterable_GetTrustLevel,
    (void *)iterable_First
};

/* =========================================================================
 * IIterator<WebTokenResponse*> implementation
 * ========================================================================= */
struct iterator_response
{
    struct { const void *lpVtbl; } IIterator_iface;
    LONG ref;
    void *item;
    UINT32 index;
};

static inline struct iterator_response *impl_from_IIterator( void *iface )
{
    return CONTAINING_RECORD( iface, struct iterator_response, IIterator_iface );
}

static HRESULT WINAPI iterator_QueryInterface( void *iface, REFIID riid, void **out )
{
    struct iterator_response *impl = impl_from_IIterator( iface );
    TRACE( "iface %p, riid %s\n", iface, debugstr_guid( riid ) );
    if (!out) return E_POINTER;
    *out = &impl->IIterator_iface;
    InterlockedIncrement( &impl->ref );
    return S_OK;
}

static ULONG WINAPI iterator_AddRef( void *iface )
{
    struct iterator_response *impl = impl_from_IIterator( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI iterator_Release( void *iface )
{
    struct iterator_response *impl = impl_from_IIterator( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    if (!ref)
    {
        if (impl->item) IInspectable_Release( (IInspectable *)impl->item );
        free( impl );
    }
    return ref;
}

static HRESULT WINAPI iterator_GetIids( void *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI iterator_GetRuntimeClassName( void *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.Collections.IIterator`1<Windows.Security.Authentication.Web.Core.WebTokenResponse>", 101, class_name );
}

static HRESULT WINAPI iterator_GetTrustLevel( void *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI iterator_get_Current( void *iface, void **current )
{
    struct iterator_response *impl = impl_from_IIterator( iface );
    TRACE( "iface %p, current %p (index %u)\n", iface, current, impl->index );
    if (!current) return E_POINTER;
    if (impl->index == 0 && impl->item)
    {
        *current = impl->item;
        IInspectable_AddRef( (IInspectable *)impl->item );
        return S_OK;
    }
    return 0x8000000B; /* E_BOUNDS */
}

static HRESULT WINAPI iterator_get_HasCurrent( void *iface, BOOLEAN *has_current )
{
    struct iterator_response *impl = impl_from_IIterator( iface );
    if (!has_current) return E_POINTER;
    *has_current = (impl->index == 0 && impl->item != NULL);
    TRACE( "iface %p, has_current %d (index %u)\n", iface, *has_current, impl->index );
    return S_OK;
}

static HRESULT WINAPI iterator_MoveNext( void *iface, BOOLEAN *has_current )
{
    struct iterator_response *impl = impl_from_IIterator( iface );
    if (impl->index == 0 && impl->item)
        impl->index++;
    if (has_current)
        *has_current = (impl->index == 0 && impl->item != NULL);
    TRACE( "iface %p, MoveNext now has_current %d (index %u)\n", iface, has_current ? *has_current : 0, impl->index );
    return S_OK;
}

static HRESULT WINAPI iterator_GetMany( void *iface, UINT32 capacity, void **items, UINT32 *actual )
{
    struct iterator_response *impl = impl_from_IIterator( iface );
    if (!actual) return E_POINTER;
    if (capacity > 0 && impl->index == 0 && impl->item)
    {
        items[0] = impl->item;
        IInspectable_AddRef( (IInspectable *)impl->item );
        impl->index++;
        *actual = 1;
    }
    else
    {
        *actual = 0;
    }
    return S_OK;
}

static const void *iterator_response_vtbl[] =
{
    (void *)iterator_QueryInterface,
    (void *)iterator_AddRef,
    (void *)iterator_Release,
    (void *)iterator_GetIids,
    (void *)iterator_GetRuntimeClassName,
    (void *)iterator_GetTrustLevel,
    (void *)iterator_get_Current,
    (void *)iterator_get_HasCurrent,
    (void *)iterator_MoveNext,
    (void *)iterator_GetMany
};

static HRESULT iterator_response_create( void *item, void **out )
{
    struct iterator_response *impl;
    if (!out) return E_POINTER;
    *out = NULL;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IIterator_iface.lpVtbl = iterator_response_vtbl;
    impl->ref = 1;
    impl->item = item;
    if (item) IInspectable_AddRef( (IInspectable *)item );
    impl->index = 0;
    *out = &impl->IIterator_iface;
    return S_OK;
}

static HRESULT vector_view_response_create( void *response, void **out )
{
    struct vector_view_response *impl;
    if (!out) return E_POINTER;
    *out = NULL;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IVectorView_iface.lpVtbl = vector_view_vtbl;
    impl->IIterable_iface.lpVtbl = iterable_response_vtbl;
    impl->ref = 1;
    impl->item = response;
    if (response) IInspectable_AddRef( (IInspectable *)response );
    *out = &impl->IVectorView_iface;
    return S_OK;
}

/* =========================================================================
 * WebTokenRequestResult implementation
 * ========================================================================= */
struct web_token_request_result
{
    struct { const void *lpVtbl; } IWebTokenRequestResult_iface;
    LONG ref;
    void *response_data;
    UINT32 response_status;
};

static inline struct web_token_request_result *impl_from_IWebTokenRequestResult( void *iface )
{
    return CONTAINING_RECORD( iface, struct web_token_request_result, IWebTokenRequestResult_iface );
}

static HRESULT WINAPI web_token_request_result_QueryInterface( void *iface, REFIID riid, void **out )
{
    struct web_token_request_result *impl = impl_from_IWebTokenRequestResult( iface );
    TRACE( "iface %p, riid %s\n", iface, debugstr_guid( riid ) );
    if (!out) return E_POINTER;
    *out = NULL;

    if (IsEqualGUID( riid, &IID_IUnknown ) ||
        IsEqualGUID( riid, &IID_IInspectable ) ||
        IsEqualGUID( riid, &IID_IAgileObject ) ||
        IsEqualGUID( riid, &IID_IWebTokenRequestResult ))
    {
        *out = &impl->IWebTokenRequestResult_iface;
        InterlockedIncrement( &impl->ref );
        return S_OK;
    }

    FIXME( "result %p, %s not implemented\n", iface, debugstr_guid( riid ) );
    return E_NOINTERFACE;
}

static ULONG WINAPI web_token_request_result_AddRef( void *iface )
{
    struct web_token_request_result *impl = impl_from_IWebTokenRequestResult( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI web_token_request_result_Release( void *iface )
{
    struct web_token_request_result *impl = impl_from_IWebTokenRequestResult( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    if (!ref)
    {
        if (impl->response_data) IInspectable_Release( (IInspectable *)impl->response_data );
        free( impl );
    }
    return ref;
}

static HRESULT WINAPI web_token_request_result_GetIids( void *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI web_token_request_result_GetRuntimeClassName( void *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Security.Authentication.Web.Core.WebTokenRequestResult", 64, class_name );
}

static HRESULT WINAPI web_token_request_result_GetTrustLevel( void *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI web_token_request_result_get_ResponseData( void *iface, void **value )
{
    struct web_token_request_result *impl = impl_from_IWebTokenRequestResult( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    *value = impl->response_data;
    if (*value) IInspectable_AddRef( (IInspectable *)*value );
    return S_OK;
}

static HRESULT WINAPI web_token_request_result_get_ResponseStatus( void *iface, UINT32 *value )
{
    struct web_token_request_result *impl = impl_from_IWebTokenRequestResult( iface );
    TRACE( "iface %p, value %p, returning status %u\n", iface, value, impl->response_status );
    if (!value) return E_POINTER;
    *value = impl->response_status;
    return S_OK;
}

static HRESULT WINAPI web_token_request_result_get_ResponseError( void *iface, void **value )
{
    if (value) *value = NULL;
    return S_OK;
}

static HRESULT WINAPI web_token_request_result_InvalidateCacheAsync( void *iface, void **asyncInfo )
{
    return async_inspectable_create( NULL, S_OK, Completed, asyncInfo );
}

static const void *web_token_request_result_vtbl[] =
{
    (void *)web_token_request_result_QueryInterface,
    (void *)web_token_request_result_AddRef,
    (void *)web_token_request_result_Release,
    (void *)web_token_request_result_GetIids,
    (void *)web_token_request_result_GetRuntimeClassName,
    (void *)web_token_request_result_GetTrustLevel,
    (void *)web_token_request_result_get_ResponseData,
    (void *)web_token_request_result_get_ResponseStatus,
    (void *)web_token_request_result_get_ResponseError,
    (void *)web_token_request_result_InvalidateCacheAsync
};

static HRESULT web_token_request_result_create( void *response_view, UINT32 status, void **out )
{
    struct web_token_request_result *impl;
    if (!out) return E_POINTER;
    *out = NULL;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IWebTokenRequestResult_iface.lpVtbl = web_token_request_result_vtbl;
    impl->ref = 1;
    impl->response_data = response_view;
    if (response_view) IInspectable_AddRef( (IInspectable *)response_view );
    impl->response_status = status;
    *out = &impl->IWebTokenRequestResult_iface;
    return S_OK;
}

/* =========================================================================
 * WebTokenRequest implementation & factory
 * ========================================================================= */
struct web_token_request
{
    struct { const void *lpVtbl; } IWebTokenRequest_iface;
    LONG ref;
    void *provider;
    HSTRING scope;
    HSTRING client_id;
    UINT32 prompt_type;
    void *properties;
};

static inline struct web_token_request *impl_from_IWebTokenRequest( void *iface )
{
    return CONTAINING_RECORD( iface, struct web_token_request, IWebTokenRequest_iface );
}

static HRESULT WINAPI web_token_request_QueryInterface( void *iface, REFIID riid, void **out )
{
    struct web_token_request *impl = impl_from_IWebTokenRequest( iface );
    if (!out) return E_POINTER;
    *out = NULL;

    if (IsEqualGUID( riid, &IID_IUnknown ) ||
        IsEqualGUID( riid, &IID_IInspectable ) ||
        IsEqualGUID( riid, &IID_IAgileObject ) ||
        IsEqualGUID( riid, &IID_IWebTokenRequest ))
    {
        *out = &impl->IWebTokenRequest_iface;
        InterlockedIncrement( &impl->ref );
        return S_OK;
    }

    FIXME( "request %p, %s not implemented\n", iface, debugstr_guid( riid ) );
    return E_NOINTERFACE;
}

static ULONG WINAPI web_token_request_AddRef( void *iface )
{
    struct web_token_request *impl = impl_from_IWebTokenRequest( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI web_token_request_Release( void *iface )
{
    struct web_token_request *impl = impl_from_IWebTokenRequest( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    if (!ref)
    {
        if (impl->provider) IInspectable_Release( (IInspectable *)impl->provider );
        if (impl->properties) IInspectable_Release( (IInspectable *)impl->properties );
        if (impl->scope) WindowsDeleteString( impl->scope );
        if (impl->client_id) WindowsDeleteString( impl->client_id );
        free( impl );
    }
    return ref;
}

static HRESULT WINAPI web_token_request_GetIids( void *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI web_token_request_GetRuntimeClassName( void *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Security.Authentication.Web.Core.WebTokenRequest", 57, class_name );
}

static HRESULT WINAPI web_token_request_GetTrustLevel( void *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI web_token_request_get_WebAccountProvider( void *iface, void **value )
{
    struct web_token_request *impl = impl_from_IWebTokenRequest( iface );
    if (!value) return E_POINTER;
    *value = impl->provider;
    if (*value) IInspectable_AddRef( (IInspectable *)*value );
    return S_OK;
}

static HRESULT WINAPI web_token_request_get_Scope( void *iface, HSTRING *value )
{
    struct web_token_request *impl = impl_from_IWebTokenRequest( iface );
    if (!value) return E_POINTER;
    return WindowsDuplicateString( impl->scope, value );
}

static HRESULT WINAPI web_token_request_get_ClientId( void *iface, HSTRING *value )
{
    struct web_token_request *impl = impl_from_IWebTokenRequest( iface );
    if (!value) return E_POINTER;
    return WindowsDuplicateString( impl->client_id, value );
}

static HRESULT WINAPI web_token_request_get_PromptType( void *iface, UINT32 *value )
{
    struct web_token_request *impl = impl_from_IWebTokenRequest( iface );
    if (!value) return E_POINTER;
    *value = impl->prompt_type;
    return S_OK;
}

static HRESULT WINAPI web_token_request_get_Properties( void *iface, void **value )
{
    struct web_token_request *impl = impl_from_IWebTokenRequest( iface );
    if (!value) return E_POINTER;
    *value = impl->properties;
    if (*value) IInspectable_AddRef( (IInspectable *)*value );
    TRACE( "iface %p returning properties %p\n", iface, *value );
    return S_OK;
}

static const void *web_token_request_vtbl[] =
{
    (void *)web_token_request_QueryInterface,
    (void *)web_token_request_AddRef,
    (void *)web_token_request_Release,
    (void *)web_token_request_GetIids,
    (void *)web_token_request_GetRuntimeClassName,
    (void *)web_token_request_GetTrustLevel,
    (void *)web_token_request_get_WebAccountProvider,
    (void *)web_token_request_get_Scope,
    (void *)web_token_request_get_ClientId,
    (void *)web_token_request_get_PromptType,
    (void *)web_token_request_get_Properties
};

static HRESULT web_token_request_create( void *provider, HSTRING scope, HSTRING client_id, UINT32 prompt_type, void **out )
{
    struct web_token_request *impl;
    if (!out) return E_POINTER;
    *out = NULL;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IWebTokenRequest_iface.lpVtbl = web_token_request_vtbl;
    impl->ref = 1;
    impl->provider = provider;
    if (provider) IInspectable_AddRef( (IInspectable *)provider );
    if (scope) WindowsDuplicateString( scope, &impl->scope );
    if (client_id) WindowsDuplicateString( client_id, &impl->client_id );
    impl->prompt_type = prompt_type;
    map_hstring_hstring_create( &impl->properties );
    *out = &impl->IWebTokenRequest_iface;
    return S_OK;
}

/* WebTokenRequest factory */
struct web_token_request_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    LONG ref;
};

static inline struct web_token_request_factory_impl *impl_from_web_token_request_factory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct web_token_request_factory_impl, IActivationFactory_iface );
}

static HRESULT WINAPI wtr_factory_QueryInterface( IActivationFactory *iface, REFIID riid, void **out )
{
    struct web_token_request_factory_impl *impl = impl_from_web_token_request_factory( iface );
    if (!out) return E_POINTER;
    *out = NULL;

    if (IsEqualGUID( riid, &IID_IUnknown ) ||
        IsEqualGUID( riid, &IID_IInspectable ) ||
        IsEqualGUID( riid, &IID_IAgileObject ) ||
        IsEqualGUID( riid, &IID_IActivationFactory ) ||
        IsEqualGUID( riid, &IID_IWebTokenRequestFactory ))
    {
        *out = &impl->IActivationFactory_iface;
        IActivationFactory_AddRef( iface );
        return S_OK;
    }

    return E_NOINTERFACE;
}

static ULONG WINAPI wtr_factory_AddRef( IActivationFactory *iface )
{
    struct web_token_request_factory_impl *impl = impl_from_web_token_request_factory( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI wtr_factory_Release( IActivationFactory *iface )
{
    struct web_token_request_factory_impl *impl = impl_from_web_token_request_factory( iface );
    return InterlockedDecrement( &impl->ref );
}

static HRESULT WINAPI wtr_factory_GetIids( IActivationFactory *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI wtr_factory_GetRuntimeClassName( IActivationFactory *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Security.Authentication.Web.Core.WebTokenRequest", 57, class_name );
}

static HRESULT WINAPI wtr_factory_GetTrustLevel( IActivationFactory *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI wtr_factory_ActivateInstance( IActivationFactory *iface, IInspectable **instance )
{
    return web_token_request_create( NULL, NULL, NULL, 0, (void **)instance );
}

static HRESULT WINAPI wtr_factory_Create( void *iface, void *provider, void **out )
{
    TRACE( "iface %p, provider %p, out %p\n", iface, provider, out );
    return web_token_request_create( provider, NULL, NULL, 0, out );
}

static HRESULT WINAPI wtr_factory_CreateWithScope( void *iface, void *provider, HSTRING scope, void **out )
{
    TRACE( "iface %p, provider %p, scope %s, out %p\n", iface, provider, debugstr_hstring(scope), out );
    return web_token_request_create( provider, scope, NULL, 0, out );
}

static HRESULT WINAPI wtr_factory_CreateWithScopeAndClientId( void *iface, void *provider, HSTRING scope, HSTRING client_id, void **out )
{
    TRACE( "iface %p, provider %p, scope %s, client_id %s, out %p\n",
          iface, provider, debugstr_hstring(scope), debugstr_hstring(client_id), out );
    return web_token_request_create( provider, scope, client_id, 0, out );
}

static HRESULT WINAPI wtr_factory_CreateWithScopeAndClientIdAndPromptType( void *iface, void *provider, HSTRING scope, HSTRING client_id, UINT32 prompt_type, void **out )
{
    TRACE( "iface %p, provider %p, scope %s, client_id %s, prompt_type %u, out %p\n",
          iface, provider, debugstr_hstring(scope), debugstr_hstring(client_id), prompt_type, out );
    return web_token_request_create( provider, scope, client_id, prompt_type, out );
}

static const void *wtr_factory_vtbl[] =
{
    (void *)wtr_factory_QueryInterface,                             /* 0:  0x00 */
    (void *)wtr_factory_AddRef,                                     /* 1:  0x08 */
    (void *)wtr_factory_Release,                                    /* 2:  0x10 */
    (void *)wtr_factory_GetIids,                                     /* 3:  0x18 */
    (void *)wtr_factory_GetRuntimeClassName,                        /* 4:  0x20 */
    (void *)wtr_factory_GetTrustLevel,                              /* 5:  0x28 */
    (void *)wtr_factory_ActivateInstance,                          /* 6:  0x30 */
    (void *)wtr_factory_Create,                                     /* 7:  0x38 */
    (void *)wtr_factory_Create,                                     /* 8:  0x40 (Create provider) */
    (void *)wtr_factory_CreateWithScope,                            /* 9:  0x48 (CreateWithScope provider, scope) */
    (void *)wtr_factory_CreateWithScopeAndClientId,                 /* 10: 0x50 */
    (void *)wtr_factory_CreateWithScopeAndClientIdAndPromptType     /* 11: 0x58 */
};

static struct web_token_request_factory_impl web_token_request_factory_instance =
{
    {(const struct IActivationFactoryVtbl *)wtr_factory_vtbl},
    1
};
static IActivationFactory *web_token_request_factory = &web_token_request_factory_instance.IActivationFactory_iface;

/* =========================================================================
 * WebAccountProvider factory
 * ========================================================================= */
struct web_account_provider_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    LONG ref;
};

static inline struct web_account_provider_factory_impl *impl_from_web_account_provider_factory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct web_account_provider_factory_impl, IActivationFactory_iface );
}

static HRESULT WINAPI wap_factory_QueryInterface( IActivationFactory *iface, REFIID riid, void **out )
{
    struct web_account_provider_factory_impl *impl = impl_from_web_account_provider_factory( iface );
    if (!out) return E_POINTER;
    *out = NULL;

    if (IsEqualGUID( riid, &IID_IUnknown ) ||
        IsEqualGUID( riid, &IID_IInspectable ) ||
        IsEqualGUID( riid, &IID_IAgileObject ) ||
        IsEqualGUID( riid, &IID_IActivationFactory ) ||
        IsEqualGUID( riid, &IID_IWebAccountProviderFactory ))
    {
        *out = &impl->IActivationFactory_iface;
        IActivationFactory_AddRef( iface );
        return S_OK;
    }

    return E_NOINTERFACE;
}

static ULONG WINAPI wap_factory_AddRef( IActivationFactory *iface )
{
    struct web_account_provider_factory_impl *impl = impl_from_web_account_provider_factory( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI wap_factory_Release( IActivationFactory *iface )
{
    struct web_account_provider_factory_impl *impl = impl_from_web_account_provider_factory( iface );
    return InterlockedDecrement( &impl->ref );
}

static HRESULT WINAPI wap_factory_GetIids( IActivationFactory *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI wap_factory_GetRuntimeClassName( IActivationFactory *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Security.Credentials.WebAccountProvider", 46, class_name );
}

static HRESULT WINAPI wap_factory_GetTrustLevel( IActivationFactory *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI wap_factory_ActivateInstance( IActivationFactory *iface, IInspectable **instance )
{
    return web_account_provider_create( NULL, NULL, NULL, (void **)instance );
}

static HRESULT WINAPI wap_factory_CreateWebAccountProvider( void *iface, HSTRING id, HSTRING display_name, void *icon_uri, void **out )
{
    const WCHAR *id_str = WindowsGetStringRawBuffer( id, NULL );
    const WCHAR *name_str = WindowsGetStringRawBuffer( display_name, NULL );
    return web_account_provider_create( id_str, name_str, NULL, out );
}

static const void *wap_factory_vtbl[] =
{
    (void *)wap_factory_QueryInterface,             /* 0: 0x00 */
    (void *)wap_factory_AddRef,                     /* 1: 0x08 */
    (void *)wap_factory_Release,                    /* 2: 0x10 */
    (void *)wap_factory_GetIids,                    /* 3: 0x18 */
    (void *)wap_factory_GetRuntimeClassName,        /* 4: 0x20 */
    (void *)wap_factory_GetTrustLevel,              /* 5: 0x28 */
    (void *)wap_factory_ActivateInstance,          /* 6: 0x30 */
    (void *)wap_factory_CreateWebAccountProvider,   /* 7: 0x38 */
    (void *)wap_factory_CreateWebAccountProvider    /* 8: 0x40 */
};

static struct web_account_provider_factory_impl web_account_provider_factory_instance =
{
    {(const struct IActivationFactoryVtbl *)wap_factory_vtbl},
    1
};
static IActivationFactory *web_account_provider_factory = &web_account_provider_factory_instance.IActivationFactory_iface;

/* =========================================================================
 * WebAccount factory
 * ========================================================================= */
struct web_account_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    LONG ref;
};

static inline struct web_account_factory_impl *impl_from_web_account_factory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct web_account_factory_impl, IActivationFactory_iface );
}

static HRESULT WINAPI wa_factory_QueryInterface( IActivationFactory *iface, REFIID riid, void **out )
{
    struct web_account_factory_impl *impl = impl_from_web_account_factory( iface );
    if (!out) return E_POINTER;
    *out = NULL;

    if (IsEqualGUID( riid, &IID_IUnknown ) ||
        IsEqualGUID( riid, &IID_IInspectable ) ||
        IsEqualGUID( riid, &IID_IAgileObject ) ||
        IsEqualGUID( riid, &IID_IActivationFactory ) ||
        IsEqualGUID( riid, &IID_IWebAccountFactory ))
    {
        *out = &impl->IActivationFactory_iface;
        IActivationFactory_AddRef( iface );
        return S_OK;
    }

    return E_NOINTERFACE;
}

static ULONG WINAPI wa_factory_AddRef( IActivationFactory *iface )
{
    struct web_account_factory_impl *impl = impl_from_web_account_factory( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI wa_factory_Release( IActivationFactory *iface )
{
    struct web_account_factory_impl *impl = impl_from_web_account_factory( iface );
    return InterlockedDecrement( &impl->ref );
}

static HRESULT WINAPI wa_factory_GetIids( IActivationFactory *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI wa_factory_GetRuntimeClassName( IActivationFactory *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Security.Credentials.WebAccount", 38, class_name );
}

static HRESULT WINAPI wa_factory_GetTrustLevel( IActivationFactory *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI wa_factory_ActivateInstance( IActivationFactory *iface, IInspectable **instance )
{
    load_xodus_tokens();
    return web_account_create( NULL, token_cache.username, token_cache.puid, (void **)instance );
}

static HRESULT WINAPI wa_factory_CreateWebAccount( void *iface, void *provider, HSTRING username, UINT32 state, void **out )
{
    const WCHAR *user_str = WindowsGetStringRawBuffer( username, NULL );
    load_xodus_tokens();
    return web_account_create( provider, user_str ? user_str : token_cache.username, token_cache.puid, out );
}

static const void *wa_factory_vtbl[] =
{
    (void *)wa_factory_QueryInterface,      /* 0: 0x00 */
    (void *)wa_factory_AddRef,              /* 1: 0x08 */
    (void *)wa_factory_Release,             /* 2: 0x10 */
    (void *)wa_factory_GetIids,             /* 3: 0x18 */
    (void *)wa_factory_GetRuntimeClassName, /* 4: 0x20 */
    (void *)wa_factory_GetTrustLevel,       /* 5: 0x28 */
    (void *)wa_factory_ActivateInstance,   /* 6: 0x30 */
    (void *)wa_factory_CreateWebAccount,    /* 7: 0x38 */
    (void *)wa_factory_CreateWebAccount     /* 8: 0x40 */
};

static struct web_account_factory_impl web_account_factory_instance =
{
    {(const struct IActivationFactoryVtbl *)wa_factory_vtbl},
    1
};
static IActivationFactory *web_account_factory = &web_account_factory_instance.IActivationFactory_iface;

/* =========================================================================
 * WebAuthenticationCoreManager statics & factory
 * ========================================================================= */
struct web_auth_factory
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } IWebAuthenticationCoreManagerStatics_iface;
    struct { const void *lpVtbl; } IWebAuthenticationCoreManagerStatics2_iface;
    LONG ref;
};

static inline struct web_auth_factory *impl_from_web_auth_factory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct web_auth_factory, IActivationFactory_iface );
}

static inline struct web_auth_factory *impl_from_IWebAuthenticationCoreManagerStatics( void *iface )
{
    return CONTAINING_RECORD( iface, struct web_auth_factory, IWebAuthenticationCoreManagerStatics_iface );
}

static inline struct web_auth_factory *impl_from_IWebAuthenticationCoreManagerStatics2( void *iface )
{
    return CONTAINING_RECORD( iface, struct web_auth_factory, IWebAuthenticationCoreManagerStatics2_iface );
}

static HRESULT WINAPI web_auth_factory_QueryInterface( IActivationFactory *iface, REFIID riid, void **out )
{
    struct web_auth_factory *impl = impl_from_web_auth_factory( iface );
    TRACE( "iface %p, riid %s, out %p.\n", iface, debugstr_guid( riid ), out );

    if (!out) return E_POINTER;
    *out = NULL;

    if (IsEqualGUID( riid, &IID_IUnknown ) ||
        IsEqualGUID( riid, &IID_IInspectable ) ||
        IsEqualGUID( riid, &IID_IAgileObject ) ||
        IsEqualGUID( riid, &IID_IActivationFactory ))
    {
        *out = &impl->IActivationFactory_iface;
        IActivationFactory_AddRef( iface );
        return S_OK;
    }

    if (IsEqualGUID( riid, &IID_IWebAuthenticationCoreManagerStatics ))
    {
        *out = &impl->IWebAuthenticationCoreManagerStatics_iface;
        IActivationFactory_AddRef( iface );
        return S_OK;
    }

    if (IsEqualGUID( riid, &IID_IWebAuthenticationCoreManagerStatics2 ))
    {
        *out = &impl->IWebAuthenticationCoreManagerStatics2_iface;
        IActivationFactory_AddRef( iface );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( riid ) );
    return E_NOINTERFACE;
}

static ULONG WINAPI web_auth_factory_AddRef( IActivationFactory *iface )
{
    struct web_auth_factory *impl = impl_from_web_auth_factory( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI web_auth_factory_Release( IActivationFactory *iface )
{
    struct web_auth_factory *impl = impl_from_web_auth_factory( iface );
    return InterlockedDecrement( &impl->ref );
}

static HRESULT WINAPI web_auth_factory_GetIids( IActivationFactory *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI web_auth_factory_GetRuntimeClassName( IActivationFactory *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Security.Authentication.Web.Core.WebAuthenticationCoreManager", 69, class_name );
}

static HRESULT WINAPI web_auth_factory_GetTrustLevel( IActivationFactory *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI web_auth_factory_ActivateInstance( IActivationFactory *iface, IInspectable **instance )
{
    FIXME( "iface %p, instance %p stub!\n", iface, instance );
    return E_NOTIMPL;
}

static const struct IActivationFactoryVtbl web_auth_activation_factory_vtbl =
{
    web_auth_factory_QueryInterface,
    web_auth_factory_AddRef,
    web_auth_factory_Release,
    web_auth_factory_GetIids,
    web_auth_factory_GetRuntimeClassName,
    web_auth_factory_GetTrustLevel,
    web_auth_factory_ActivateInstance
};

static HRESULT WINAPI web_auth_statics_QueryInterface( void *iface, REFIID riid, void **out )
{
    struct web_auth_factory *impl = impl_from_IWebAuthenticationCoreManagerStatics( iface );
    return web_auth_factory_QueryInterface( &impl->IActivationFactory_iface, riid, out );
}

static ULONG WINAPI web_auth_statics_AddRef( void *iface )
{
    struct web_auth_factory *impl = impl_from_IWebAuthenticationCoreManagerStatics( iface );
    return web_auth_factory_AddRef( &impl->IActivationFactory_iface );
}

static ULONG WINAPI web_auth_statics_Release( void *iface )
{
    struct web_auth_factory *impl = impl_from_IWebAuthenticationCoreManagerStatics( iface );
    return web_auth_factory_Release( &impl->IActivationFactory_iface );
}

static HRESULT WINAPI web_auth_statics_GetIids( void *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI web_auth_statics_GetRuntimeClassName( void *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Security.Authentication.Web.Core.WebAuthenticationCoreManager", 69, class_name );
}

static HRESULT WINAPI web_auth_statics_GetTrustLevel( void *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT build_token_request_result( void *req, void *webAccount, void **out_result )
{
    void *provider = NULL;
    void *account = NULL;
    void *token_response = NULL;
    void *vector_view = NULL;
    void *result = NULL;
    struct web_token_request *req_impl = req ? impl_from_IWebTokenRequest( req ) : NULL;

    load_xodus_tokens();

    if (webAccount)
    {
        account = webAccount;
        IInspectable_AddRef( (IInspectable *)account );
    }
    else
    {
        if (req_impl && req_impl->provider)
        {
            provider = req_impl->provider;
            IInspectable_AddRef( (IInspectable *)provider );
        }
        else
        {
            web_account_provider_create( L"https://xsts.auth.xboxlive.com", L"Microsoft account", L"consumers", &provider );
        }
        web_account_create( provider, token_cache.username, token_cache.xuid[0] ? token_cache.xuid : token_cache.puid, &account );
    }

    web_token_response_create( token_cache.token ? token_cache.token : L"", account, &token_response );
    vector_view_response_create( token_response, &vector_view );
    web_token_request_result_create( vector_view, 0 /* WebTokenRequestStatus_Success */, &result );

    if (provider) IInspectable_Release( (IInspectable *)provider );
    if (account) IInspectable_Release( (IInspectable *)account );
    if (token_response) IInspectable_Release( (IInspectable *)token_response );
    if (vector_view) IInspectable_Release( (IInspectable *)vector_view );

    *out_result = result;
    return S_OK;
}

static HRESULT WINAPI web_auth_statics_GetTokenSilentlyAsync( void *This, void *request, void **asyncInfo )
{
    void *result = NULL;
    HRESULT hr;

    TRACE( "This %p, request %p, asyncInfo %p\n", This, request, asyncInfo );
    build_token_request_result( request, NULL, &result );
    hr = async_inspectable_create( result, S_OK, Completed, asyncInfo );
    if (result) IInspectable_Release( (IInspectable *)result );
    return hr;
}

static HRESULT WINAPI web_auth_statics_GetTokenSilentlyWithWebAccountAsync( void *This, void *request, void *webAccount, void **asyncInfo )
{
    void *result = NULL;
    HRESULT hr;

    TRACE( "This %p, request %p, webAccount %p, asyncInfo %p\n", This, request, webAccount, asyncInfo );
    build_token_request_result( request, webAccount, &result );
    hr = async_inspectable_create( result, S_OK, Completed, asyncInfo );
    if (result) IInspectable_Release( (IInspectable *)result );
    return hr;
}

static HRESULT WINAPI web_auth_statics_RequestTokenAsync( void *This, void *request, void **asyncInfo )
{
    void *result = NULL;
    HRESULT hr;

    TRACE( "This %p, request %p, asyncInfo %p\n", This, request, asyncInfo );
    build_token_request_result( request, NULL, &result );
    hr = async_inspectable_create( result, S_OK, Completed, asyncInfo );
    if (result) IInspectable_Release( (IInspectable *)result );
    return hr;
}

static HRESULT WINAPI web_auth_statics_RequestTokenWithWebAccountAsync( void *This, void *request, void *webAccount, void **asyncInfo )
{
    void *result = NULL;
    HRESULT hr;

    TRACE( "This %p, request %p, webAccount %p, asyncInfo %p\n", This, request, webAccount, asyncInfo );
    build_token_request_result( request, webAccount, &result );
    hr = async_inspectable_create( result, S_OK, Completed, asyncInfo );
    if (result) IInspectable_Release( (IInspectable *)result );
    return hr;
}

static HRESULT WINAPI web_auth_statics_FindAccountAsync( void *This, void *provider, HSTRING webAccountId, void **asyncInfo )
{
    void *account = NULL;
    HRESULT hr;

    TRACE( "This %p, provider %p, webAccountId %s, asyncInfo %p\n", This, provider, debugstr_hstring(webAccountId), asyncInfo );
    load_xodus_tokens();
    web_account_create( provider, token_cache.username, token_cache.xuid[0] ? token_cache.xuid : token_cache.puid, &account );
    hr = async_inspectable_create( account, S_OK, Completed, asyncInfo );
    if (account) IInspectable_Release( (IInspectable *)account );
    return hr;
}

static HRESULT WINAPI web_auth_statics_FindAccountProviderAsync( void *This, HSTRING providerId, void **asyncInfo )
{
    const WCHAR *pid = WindowsGetStringRawBuffer( providerId, NULL );
    void *provider = NULL;
    HRESULT hr;

    TRACE( "This %p, providerId %s, asyncInfo %p\n", This, debugstr_hstring(providerId), asyncInfo );
    web_account_provider_create( pid, L"Microsoft account", L"consumers", &provider );
    hr = async_inspectable_create( provider, S_OK, Completed, asyncInfo );
    if (provider) IInspectable_Release( (IInspectable *)provider );
    return hr;
}

static HRESULT WINAPI web_auth_statics_FindAccountProviderWithAuthorityAsync( void *This, HSTRING providerId, HSTRING authority, void **asyncInfo )
{
    const WCHAR *pid = WindowsGetStringRawBuffer( providerId, NULL );
    const WCHAR *auth = WindowsGetStringRawBuffer( authority, NULL );
    void *provider = NULL;
    HRESULT hr;

    TRACE( "This %p, providerId %s, authority %s, asyncInfo %p\n", This, debugstr_hstring(providerId), debugstr_hstring(authority), asyncInfo );
    web_account_provider_create( pid, L"Microsoft account", auth, &provider );
    hr = async_inspectable_create( provider, S_OK, Completed, asyncInfo );
    if (provider) IInspectable_Release( (IInspectable *)provider );
    return hr;
}

static const void *web_auth_statics_vtbl[] =
{
    (void *)web_auth_statics_QueryInterface,
    (void *)web_auth_statics_AddRef,
    (void *)web_auth_statics_Release,
    (void *)web_auth_statics_GetIids,
    (void *)web_auth_statics_GetRuntimeClassName,
    (void *)web_auth_statics_GetTrustLevel,
    (void *)web_auth_statics_GetTokenSilentlyAsync,
    (void *)web_auth_statics_GetTokenSilentlyWithWebAccountAsync,
    (void *)web_auth_statics_RequestTokenAsync,
    (void *)web_auth_statics_RequestTokenWithWebAccountAsync,
    (void *)web_auth_statics_FindAccountAsync,
    (void *)web_auth_statics_FindAccountProviderAsync,
    (void *)web_auth_statics_FindAccountProviderWithAuthorityAsync
};

/* IWebAuthenticationCoreManagerStatics2 methods */
static HRESULT WINAPI web_auth_statics2_QueryInterface( void *iface, REFIID riid, void **out )
{
    struct web_auth_factory *impl = impl_from_IWebAuthenticationCoreManagerStatics2( iface );
    return web_auth_factory_QueryInterface( &impl->IActivationFactory_iface, riid, out );
}

static ULONG WINAPI web_auth_statics2_AddRef( void *iface )
{
    struct web_auth_factory *impl = impl_from_IWebAuthenticationCoreManagerStatics2( iface );
    return web_auth_factory_AddRef( &impl->IActivationFactory_iface );
}

static ULONG WINAPI web_auth_statics2_Release( void *iface )
{
    struct web_auth_factory *impl = impl_from_IWebAuthenticationCoreManagerStatics2( iface );
    return web_auth_factory_Release( &impl->IActivationFactory_iface );
}

static HRESULT WINAPI web_auth_statics2_FindAccountProviderWithAuthorityForUserAsync( void *This, HSTRING providerId, HSTRING authority, void *user, void **asyncInfo )
{
    const WCHAR *pid = WindowsGetStringRawBuffer( providerId, NULL );
    const WCHAR *auth = WindowsGetStringRawBuffer( authority, NULL );
    void *provider = NULL;
    HRESULT hr;

    TRACE( "This %p, providerId %s, authority %s, user %p, asyncInfo %p\n",
          This, debugstr_hstring(providerId), debugstr_hstring(authority), user, asyncInfo );
    web_account_provider_create( pid, L"Microsoft account", auth, &provider );
    hr = async_inspectable_create( provider, S_OK, Completed, asyncInfo );
    if (provider) IInspectable_Release( (IInspectable *)provider );
    return hr;
}

static const void *web_auth_statics2_vtbl[] =
{
    (void *)web_auth_statics2_QueryInterface,
    (void *)web_auth_statics2_AddRef,
    (void *)web_auth_statics2_Release,
    (void *)web_auth_statics_GetIids,
    (void *)web_auth_statics_GetRuntimeClassName,
    (void *)web_auth_statics_GetTrustLevel,
    (void *)web_auth_statics2_FindAccountProviderWithAuthorityForUserAsync
};

static struct web_auth_factory web_auth_factory_instance =
{
    {&web_auth_activation_factory_vtbl},
    {web_auth_statics_vtbl},
    {web_auth_statics2_vtbl},
    1
};

static IActivationFactory *web_auth_factory = &web_auth_factory_instance.IActivationFactory_iface;
