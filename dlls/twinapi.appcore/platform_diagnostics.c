/* WinRT Windows.System.Profile.PlatformDiagnosticsAndUsageDataSettings Implementation
 *
 * Copyright 2026 Xodus / Proton-GE contributors
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

/* {B6E24C1B-7B1C-4B32-8C62-A66597CE723A} */
static const GUID IID_IPlatformDiagnosticsAndUsageDataSettingsStatics =
    {0xb6e24c1b, 0x7b1c, 0x4b32, {0x8c, 0x62, 0xa6, 0x65, 0x97, 0xce, 0x72, 0x3a}};

struct platform_diagnostics_factory
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } IPlatformDiagnosticsAndUsageDataSettingsStatics_iface;
    LONG ref;
};

static inline struct platform_diagnostics_factory *impl_from_platform_diagnostics_factory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct platform_diagnostics_factory, IActivationFactory_iface );
}

static inline struct platform_diagnostics_factory *impl_from_IPlatformDiagnosticsAndUsageDataSettingsStatics( void *iface )
{
    return CONTAINING_RECORD( iface, struct platform_diagnostics_factory, IPlatformDiagnosticsAndUsageDataSettingsStatics_iface );
}

static HRESULT WINAPI platform_diagnostics_factory_QueryInterface( IActivationFactory *iface, REFIID riid, void **out )
{
    struct platform_diagnostics_factory *impl = impl_from_platform_diagnostics_factory( iface );
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

    if (IsEqualGUID( riid, &IID_IPlatformDiagnosticsAndUsageDataSettingsStatics ))
    {
        *out = &impl->IPlatformDiagnosticsAndUsageDataSettingsStatics_iface;
        IActivationFactory_AddRef( iface );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( riid ) );
    return E_NOINTERFACE;
}

static ULONG WINAPI platform_diagnostics_factory_AddRef( IActivationFactory *iface )
{
    struct platform_diagnostics_factory *impl = impl_from_platform_diagnostics_factory( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI platform_diagnostics_factory_Release( IActivationFactory *iface )
{
    struct platform_diagnostics_factory *impl = impl_from_platform_diagnostics_factory( iface );
    return InterlockedDecrement( &impl->ref );
}

static HRESULT WINAPI platform_diagnostics_factory_GetIids( IActivationFactory *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI platform_diagnostics_factory_GetRuntimeClassName( IActivationFactory *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.System.Profile.PlatformDiagnosticsAndUsageDataSettings", 61, class_name );
}

static HRESULT WINAPI platform_diagnostics_factory_GetTrustLevel( IActivationFactory *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI platform_diagnostics_factory_ActivateInstance( IActivationFactory *iface, IInspectable **instance )
{
    FIXME( "iface %p, instance %p stub!\n", iface, instance );
    return E_NOTIMPL;
}

static const struct IActivationFactoryVtbl platform_diagnostics_activation_factory_vtbl =
{
    platform_diagnostics_factory_QueryInterface,
    platform_diagnostics_factory_AddRef,
    platform_diagnostics_factory_Release,
    platform_diagnostics_factory_GetIids,
    platform_diagnostics_factory_GetRuntimeClassName,
    platform_diagnostics_factory_GetTrustLevel,
    platform_diagnostics_factory_ActivateInstance
};

static HRESULT WINAPI platform_diagnostics_statics_QueryInterface( void *iface, REFIID riid, void **out )
{
    struct platform_diagnostics_factory *impl = impl_from_IPlatformDiagnosticsAndUsageDataSettingsStatics( iface );
    return platform_diagnostics_factory_QueryInterface( &impl->IActivationFactory_iface, riid, out );
}

static ULONG WINAPI platform_diagnostics_statics_AddRef( void *iface )
{
    struct platform_diagnostics_factory *impl = impl_from_IPlatformDiagnosticsAndUsageDataSettingsStatics( iface );
    return platform_diagnostics_factory_AddRef( &impl->IActivationFactory_iface );
}

static ULONG WINAPI platform_diagnostics_statics_Release( void *iface )
{
    struct platform_diagnostics_factory *impl = impl_from_IPlatformDiagnosticsAndUsageDataSettingsStatics( iface );
    return platform_diagnostics_factory_Release( &impl->IActivationFactory_iface );
}

static HRESULT WINAPI platform_diagnostics_statics_GetIids( void *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI platform_diagnostics_statics_GetRuntimeClassName( void *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.System.Profile.PlatformDiagnosticsAndUsageDataSettings", 61, class_name );
}

static HRESULT WINAPI platform_diagnostics_statics_GetTrustLevel( void *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI platform_diagnostics_statics_get_CollectionLevel( void *This, int *value )
{
    TRACE( "This %p, value %p\n", This, value );
    if (!value) return E_POINTER;
    *value = 0; /* PlatformDataCollectionLevel_Security */
    return S_OK;
}

static HRESULT WINAPI platform_diagnostics_statics_add_CollectionLevelChanged( void *This, void *handler, EventRegistrationToken *token )
{
    TRACE( "This %p, handler %p, token %p\n", This, handler, token );
    if (!token) return E_POINTER;
    token->value = 1;
    return S_OK;
}

static HRESULT WINAPI platform_diagnostics_statics_remove_CollectionLevelChanged( void *This, EventRegistrationToken token )
{
    TRACE( "This %p, token %llu\n", This, token.value );
    return S_OK;
}

static HRESULT WINAPI platform_diagnostics_statics_CanCollectDiagnostics( void *This, int level, boolean *result )
{
    TRACE( "This %p, level %d, result %p\n", This, level, result );
    if (!result) return E_POINTER;
    *result = FALSE;
    return S_OK;
}

static const void *platform_diagnostics_statics_vtbl[] =
{
    (void *)platform_diagnostics_statics_QueryInterface,
    (void *)platform_diagnostics_statics_AddRef,
    (void *)platform_diagnostics_statics_Release,
    (void *)platform_diagnostics_statics_GetIids,
    (void *)platform_diagnostics_statics_GetRuntimeClassName,
    (void *)platform_diagnostics_statics_GetTrustLevel,
    (void *)platform_diagnostics_statics_get_CollectionLevel,
    (void *)platform_diagnostics_statics_add_CollectionLevelChanged,
    (void *)platform_diagnostics_statics_remove_CollectionLevelChanged,
    (void *)platform_diagnostics_statics_CanCollectDiagnostics
};

static struct platform_diagnostics_factory platform_diagnostics_factory_instance =
{
    {&platform_diagnostics_activation_factory_vtbl},
    {platform_diagnostics_statics_vtbl},
    1
};

static IActivationFactory *platform_diagnostics_factory = &platform_diagnostics_factory_instance.IActivationFactory_iface;
