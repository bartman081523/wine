/* WinRT Windows.Security.Authentication.Web.Core.WebAuthenticationCoreManager Implementation
 *
 * Copyright 2026 Xodus / Proton-GE contributors
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

/* {6ACA7C92-A581-4479-9C10-752EFF44FD34} */
static const GUID IID_IWebAuthenticationCoreManagerStatics =
    {0x6aca7c92, 0xa581, 0x4479, {0x9c, 0x10, 0x75, 0x2e, 0xff, 0x44, 0xfd, 0x34}};

struct async_inspectable
{
    IInspectable IInspectable_iface;
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

static HRESULT async_inspectable_create( IInspectable *result, HRESULT hr, AsyncStatus status, void **out )
{
    struct async_inspectable *impl;
    if (!out) return E_POINTER;
    *out = NULL;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IInspectable_iface.lpVtbl = (const struct IInspectableVtbl *)async_inspectable_vtbl;
    impl->IAsyncInfo_iface.lpVtbl = &async_inspectable_info_vtbl;
    impl->ref = 1;
    impl->status = status;
    impl->hr = hr;
    impl->result = result;
    if (result) IInspectable_AddRef( result );
    *out = &impl->IInspectable_iface;
    TRACE( "created async_inspectable %p, result %p, hr %#lx, status %d\n", *out, result, hr, status );
    return S_OK;
}

struct web_auth_factory
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } IWebAuthenticationCoreManagerStatics_iface;
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
    return WindowsCreateString( L"Windows.Security.Authentication.Web.Core.WebAuthenticationCoreManager", 68, class_name );
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
    return WindowsCreateString( L"Windows.Security.Authentication.Web.Core.WebAuthenticationCoreManager", 68, class_name );
}

static HRESULT WINAPI web_auth_statics_GetTrustLevel( void *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI web_auth_statics_GetTokenSilentlyAsync( void *This, void *request, void **asyncInfo )
{
    TRACE( "This %p, request %p, asyncInfo %p\n", This, request, asyncInfo );
    return async_inspectable_create( NULL, S_OK, Completed, asyncInfo );
}

static HRESULT WINAPI web_auth_statics_GetTokenSilentlyWithWebAccountAsync( void *This, void *request, void *webAccount, void **asyncInfo )
{
    TRACE( "This %p, request %p, webAccount %p, asyncInfo %p\n", This, request, webAccount, asyncInfo );
    return async_inspectable_create( NULL, S_OK, Completed, asyncInfo );
}

static HRESULT WINAPI web_auth_statics_RequestTokenAsync( void *This, void *request, void **asyncInfo )
{
    TRACE( "This %p, request %p, asyncInfo %p\n", This, request, asyncInfo );
    return async_inspectable_create( NULL, S_OK, Completed, asyncInfo );
}

static HRESULT WINAPI web_auth_statics_RequestTokenWithWebAccountAsync( void *This, void *request, void *webAccount, void **asyncInfo )
{
    TRACE( "This %p, request %p, webAccount %p, asyncInfo %p\n", This, request, webAccount, asyncInfo );
    return async_inspectable_create( NULL, S_OK, Completed, asyncInfo );
}

static HRESULT WINAPI web_auth_statics_FindAccountAsync( void *This, void *provider, HSTRING webAccountId, void **asyncInfo )
{
    TRACE( "This %p, provider %p, webAccountId %s, asyncInfo %p\n", This, provider, debugstr_hstring(webAccountId), asyncInfo );
    return async_inspectable_create( NULL, S_OK, Completed, asyncInfo );
}

static HRESULT WINAPI web_auth_statics_FindAccountProviderAsync( void *This, HSTRING providerId, void **asyncInfo )
{
    TRACE( "This %p, providerId %s, asyncInfo %p\n", This, debugstr_hstring(providerId), asyncInfo );
    return async_inspectable_create( NULL, S_OK, Completed, asyncInfo );
}

static HRESULT WINAPI web_auth_statics_FindAccountProviderWithAuthorityAsync( void *This, HSTRING providerId, HSTRING authority, void **asyncInfo )
{
    TRACE( "This %p, providerId %s, authority %s, asyncInfo %p\n", This, debugstr_hstring(providerId), debugstr_hstring(authority), asyncInfo );
    return async_inspectable_create( NULL, S_OK, Completed, asyncInfo );
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

static struct web_auth_factory web_auth_factory_instance =
{
    {&web_auth_activation_factory_vtbl},
    {web_auth_statics_vtbl},
    1
};

static IActivationFactory *web_auth_factory = &web_auth_factory_instance.IActivationFactory_iface;
