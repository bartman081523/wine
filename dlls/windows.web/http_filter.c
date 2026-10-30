/* WinRT Windows.Web.Http.Filters.HttpBaseProtocolFilter Implementation
 *
 * Copyright 2026 Xodus / Proton-GE contributors
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

/* {71C89B09-E131-4B54-A53C-EB43FF37E9BB} */
static const GUID IID_IHttpBaseProtocolFilter =
    {0x71c89b09, 0xe131, 0x4b54, {0xa5, 0x3c, 0xeb, 0x43, 0xff, 0x37, 0xe9, 0xbb}};
/* {A4CB6DD5-0902-439E-BFD7-E12552B165CE} */
static const GUID IID_IHttpFilter =
    {0xa4cb6dd5, 0x0902, 0x439e, {0xbf, 0xd7, 0xe1, 0x25, 0x52, 0xb1, 0x65, 0xce}};

struct http_base_protocol_filter
{
    IInspectable IInspectable_iface;
    LONG ref;
};

static inline struct http_base_protocol_filter *impl_from_IHttpBaseProtocolFilter( void *iface )
{
    return CONTAINING_RECORD( iface, struct http_base_protocol_filter, IInspectable_iface );
}

static HRESULT WINAPI http_filter_QueryInterface( void *iface, REFIID riid, void **out )
{
    struct http_base_protocol_filter *impl = impl_from_IHttpBaseProtocolFilter( iface );
    TRACE( "iface %p, riid %s, out %p.\n", iface, debugstr_guid( riid ), out );

    if (!out) return E_POINTER;
    *out = NULL;

    if (IsEqualGUID( riid, &IID_IUnknown ) ||
        IsEqualGUID( riid, &IID_IInspectable ) ||
        IsEqualGUID( riid, &IID_IAgileObject ) ||
        IsEqualGUID( riid, &IID_IHttpBaseProtocolFilter ) ||
        IsEqualGUID( riid, &IID_IHttpFilter ) ||
        IsEqualGUID( riid, &IID_IClosable ))
    {
        *out = &impl->IInspectable_iface;
        InterlockedIncrement( &impl->ref );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( riid ) );
    return E_NOINTERFACE;
}

static ULONG WINAPI http_filter_AddRef( void *iface )
{
    struct http_base_protocol_filter *impl = impl_from_IHttpBaseProtocolFilter( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI http_filter_Release( void *iface )
{
    struct http_base_protocol_filter *impl = impl_from_IHttpBaseProtocolFilter( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI http_filter_GetIids( void *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI http_filter_GetRuntimeClassName( void *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Web.Http.Filters.HttpBaseProtocolFilter", 46, class_name );
}

static HRESULT WINAPI http_filter_GetTrustLevel( void *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI http_filter_get_AllowAutoRedirect( void *This, boolean *value )
{
    if (value) *value = TRUE;
    return S_OK;
}

static HRESULT WINAPI http_filter_put_AllowAutoRedirect( void *This, boolean value )
{
    return S_OK;
}

static HRESULT WINAPI http_filter_get_AllowUI( void *This, boolean *value )
{
    if (value) *value = TRUE;
    return S_OK;
}

static HRESULT WINAPI http_filter_put_AllowUI( void *This, boolean value )
{
    return S_OK;
}

static HRESULT WINAPI http_filter_get_AutomaticDecompression( void *This, boolean *value )
{
    if (value) *value = TRUE;
    return S_OK;
}

static HRESULT WINAPI http_filter_put_AutomaticDecompression( void *This, boolean value )
{
    return S_OK;
}

static HRESULT WINAPI http_filter_get_CacheControl( void *This, void **value )
{
    if (value) *value = NULL;
    return S_OK;
}

static HRESULT WINAPI http_filter_get_CookieManager( void *This, void **value )
{
    if (value) *value = NULL;
    return S_OK;
}

static HRESULT WINAPI http_filter_get_ClientCertificate( void *This, void **value )
{
    if (value) *value = NULL;
    return S_OK;
}

static HRESULT WINAPI http_filter_put_ClientCertificate( void *This, void *value )
{
    return S_OK;
}

static HRESULT WINAPI http_filter_get_IgnorableServerCertificateErrors( void *This, void **value )
{
    if (value) *value = NULL;
    return S_OK;
}

static HRESULT WINAPI http_filter_get_MaxConnectionsPerServer( void *This, UINT32 *value )
{
    if (value) *value = 6;
    return S_OK;
}

static HRESULT WINAPI http_filter_put_MaxConnectionsPerServer( void *This, UINT32 value )
{
    return S_OK;
}

static HRESULT WINAPI http_filter_get_ProxyCredential( void *This, void **value )
{
    if (value) *value = NULL;
    return S_OK;
}

static HRESULT WINAPI http_filter_put_ProxyCredential( void *This, void *value )
{
    return S_OK;
}

static HRESULT WINAPI http_filter_get_ServerCredential( void *This, void **value )
{
    if (value) *value = NULL;
    return S_OK;
}

static HRESULT WINAPI http_filter_put_ServerCredential( void *This, void *value )
{
    return S_OK;
}

static HRESULT WINAPI http_filter_get_UseProxy( void *This, boolean *value )
{
    if (value) *value = TRUE;
    return S_OK;
}

static HRESULT WINAPI http_filter_put_UseProxy( void *This, boolean value )
{
    return S_OK;
}

/* IClosable method */
static HRESULT WINAPI http_filter_Close( void *This )
{
    return S_OK;
}

/* IHttpFilter method */
static HRESULT WINAPI http_filter_SendRequestAsync( void *This, void *request, void **operation )
{
    FIXME( "This %p, request %p, operation %p stub!\n", This, request, operation );
    if (operation) *operation = NULL;
    return E_NOTIMPL;
}

static const void *http_filter_vtbl[] =
{
    (void *)http_filter_QueryInterface,
    (void *)http_filter_AddRef,
    (void *)http_filter_Release,
    (void *)http_filter_GetIids,
    (void *)http_filter_GetRuntimeClassName,
    (void *)http_filter_GetTrustLevel,
    (void *)http_filter_get_AllowAutoRedirect,
    (void *)http_filter_put_AllowAutoRedirect,
    (void *)http_filter_get_AllowUI,
    (void *)http_filter_put_AllowUI,
    (void *)http_filter_get_AutomaticDecompression,
    (void *)http_filter_put_AutomaticDecompression,
    (void *)http_filter_get_CacheControl,
    (void *)http_filter_get_CookieManager,
    (void *)http_filter_get_ClientCertificate,
    (void *)http_filter_put_ClientCertificate,
    (void *)http_filter_get_IgnorableServerCertificateErrors,
    (void *)http_filter_get_MaxConnectionsPerServer,
    (void *)http_filter_put_MaxConnectionsPerServer,
    (void *)http_filter_get_ProxyCredential,
    (void *)http_filter_put_ProxyCredential,
    (void *)http_filter_get_ServerCredential,
    (void *)http_filter_put_ServerCredential,
    (void *)http_filter_get_UseProxy,
    (void *)http_filter_put_UseProxy,
    (void *)http_filter_Close,
    (void *)http_filter_SendRequestAsync
};

struct http_filter_factory
{
    IActivationFactory IActivationFactory_iface;
    LONG ref;
};

static inline struct http_filter_factory *impl_from_http_filter_factory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct http_filter_factory, IActivationFactory_iface );
}

static HRESULT WINAPI http_filter_factory_QueryInterface( IActivationFactory *iface, REFIID riid, void **out )
{
    struct http_filter_factory *impl = impl_from_http_filter_factory( iface );
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

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( riid ) );
    return E_NOINTERFACE;
}

static ULONG WINAPI http_filter_factory_AddRef( IActivationFactory *iface )
{
    struct http_filter_factory *impl = impl_from_http_filter_factory( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI http_filter_factory_Release( IActivationFactory *iface )
{
    struct http_filter_factory *impl = impl_from_http_filter_factory( iface );
    return InterlockedDecrement( &impl->ref );
}

static HRESULT WINAPI http_filter_factory_GetIids( IActivationFactory *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI http_filter_factory_GetRuntimeClassName( IActivationFactory *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Web.Http.Filters.HttpBaseProtocolFilter", 46, class_name );
}

static HRESULT WINAPI http_filter_factory_GetTrustLevel( IActivationFactory *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI http_filter_factory_ActivateInstance( IActivationFactory *iface, IInspectable **instance )
{
    struct http_base_protocol_filter *impl;
    TRACE( "iface %p, instance %p.\n", iface, instance );

    if (!instance) return E_POINTER;
    *instance = NULL;

    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IInspectable_iface.lpVtbl = (const struct IInspectableVtbl *)http_filter_vtbl;
    impl->ref = 1;
    *instance = &impl->IInspectable_iface;
    return S_OK;
}

static const struct IActivationFactoryVtbl http_filter_activation_factory_vtbl =
{
    http_filter_factory_QueryInterface,
    http_filter_factory_AddRef,
    http_filter_factory_Release,
    http_filter_factory_GetIids,
    http_filter_factory_GetRuntimeClassName,
    http_filter_factory_GetTrustLevel,
    http_filter_factory_ActivateInstance
};

static struct http_filter_factory http_filter_factory_instance =
{
    {&http_filter_activation_factory_vtbl},
    1
};

static IActivationFactory *http_filter_factory = &http_filter_factory_instance.IActivationFactory_iface;
