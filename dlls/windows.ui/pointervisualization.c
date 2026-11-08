/* WinRT Windows.UI.Input.PointerVisualizationSettings Implementation
 *
 * Copyright 2026 Xodus / Proton-GE contributors
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

static const GUID IID_IPointerVisualizationSettingsStatics =
    {0x68870edb, 0x165b, 0x4214, {0xb4, 0xf3, 0x58, 0x4e, 0xca, 0x8c, 0x8a, 0x69}};

#define RuntimeClass_Windows_UI_Input_PointerVisualizationSettings L"Windows.UI.Input.PointerVisualizationSettings"

struct pointer_visualization_settings
{
    struct { const void *lpVtbl; } IInspectable_iface;
    LONG ref;
    boolean contact_feedback;
    boolean barrel_feedback;
};

static inline struct pointer_visualization_settings *impl_from_pointer_vis( void *iface )
{
    return CONTAINING_RECORD( iface, struct pointer_visualization_settings, IInspectable_iface );
}

static HRESULT WINAPI pointer_vis_QueryInterface( void *iface, REFIID riid, void **out )
{
    struct pointer_visualization_settings *impl = impl_from_pointer_vis( iface );

    TRACE( "iface %p, riid %s, out %p\n", iface, debugstr_guid( riid ), out );
    if (!out) return E_POINTER;
    *out = &impl->IInspectable_iface;
    InterlockedIncrement( &impl->ref );
    return S_OK;
}

static ULONG WINAPI pointer_vis_AddRef( void *iface )
{
    struct pointer_visualization_settings *impl = impl_from_pointer_vis( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI pointer_vis_Release( void *iface )
{
    struct pointer_visualization_settings *impl = impl_from_pointer_vis( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI pointer_vis_GetIids( void *iface, ULONG *count, IID **iids )
{
    if (count) *count = 0;
    return S_OK;
}

static HRESULT WINAPI pointer_vis_GetRuntimeClassName( void *iface, HSTRING *name )
{
    return WindowsCreateString( RuntimeClass_Windows_UI_Input_PointerVisualizationSettings,
                                wcslen( RuntimeClass_Windows_UI_Input_PointerVisualizationSettings ), name );
}

static HRESULT WINAPI pointer_vis_GetTrustLevel( void *iface, TrustLevel *level )
{
    if (!level) return E_POINTER;
    *level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI pointer_vis_contact( void *iface, void *arg )
{
    struct pointer_visualization_settings *impl = impl_from_pointer_vis( iface );
    TRACE( "iface %p, arg %p\n", iface, arg );
    if ((uintptr_t)arg <= 1)
    {
        impl->contact_feedback = (boolean)(uintptr_t)arg;
        return S_OK;
    }
    *(boolean *)arg = impl->contact_feedback;
    return S_OK;
}

static HRESULT WINAPI pointer_vis_barrel( void *iface, void *arg )
{
    struct pointer_visualization_settings *impl = impl_from_pointer_vis( iface );
    TRACE( "iface %p, arg %p\n", iface, arg );
    if ((uintptr_t)arg <= 1)
    {
        impl->barrel_feedback = (boolean)(uintptr_t)arg;
        return S_OK;
    }
    *(boolean *)arg = impl->barrel_feedback;
    return S_OK;
}

static const void *pointer_vis_vtbl[] =
{
    (void *)pointer_vis_QueryInterface,
    (void *)pointer_vis_AddRef,
    (void *)pointer_vis_Release,
    (void *)pointer_vis_GetIids,
    (void *)pointer_vis_GetRuntimeClassName,
    (void *)pointer_vis_GetTrustLevel,
    (void *)pointer_vis_contact,
    (void *)pointer_vis_contact,
    (void *)pointer_vis_barrel,
    (void *)pointer_vis_barrel,
    (void *)pointer_vis_contact,
    (void *)pointer_vis_barrel
};

static HRESULT pointer_vis_create( void **out )
{
    struct pointer_visualization_settings *impl;
    if (!out) return E_POINTER;
    *out = NULL;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IInspectable_iface.lpVtbl = pointer_vis_vtbl;
    impl->ref = 1;
    impl->contact_feedback = TRUE;
    impl->barrel_feedback = TRUE;
    *out = &impl->IInspectable_iface;
    return S_OK;
}

/* Statics / Factory */
static HRESULT WINAPI pointer_vis_factory_QueryInterface( IActivationFactory *iface, REFIID riid, void **out )
{
    TRACE( "iface %p, riid %s, out %p\n", iface, debugstr_guid( riid ), out );
    if (!out) return E_POINTER;
    *out = NULL;

    if (IsEqualGUID( riid, &IID_IUnknown ) ||
        IsEqualGUID( riid, &IID_IInspectable ) ||
        IsEqualGUID( riid, &IID_IAgileObject ) ||
        IsEqualGUID( riid, &IID_IActivationFactory ))
    {
        *out = iface;
        IActivationFactory_AddRef( iface );
        return S_OK;
    }

    if (IsEqualGUID( riid, &IID_IPointerVisualizationSettingsStatics ))
    {
        *out = iface;
        IActivationFactory_AddRef( iface );
        return S_OK;
    }

    FIXME( "unhandled QI %s\n", debugstr_guid( riid ) );
    return E_NOINTERFACE;
}

static ULONG WINAPI pointer_vis_factory_AddRef( IActivationFactory *iface ) { return 2; }
static ULONG WINAPI pointer_vis_factory_Release( IActivationFactory *iface ) { return 1; }

static HRESULT WINAPI pointer_vis_factory_GetIids( IActivationFactory *iface, ULONG *count, IID **iids )
{
    if (count) *count = 0;
    return S_OK;
}

static HRESULT WINAPI pointer_vis_factory_GetRuntimeClassName( IActivationFactory *iface, HSTRING *name )
{
    return WindowsCreateString( RuntimeClass_Windows_UI_Input_PointerVisualizationSettings,
                                wcslen( RuntimeClass_Windows_UI_Input_PointerVisualizationSettings ), name );
}

static HRESULT WINAPI pointer_vis_factory_GetTrustLevel( IActivationFactory *iface, TrustLevel *level )
{
    if (!level) return E_POINTER;
    *level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI pointer_vis_factory_ActivateInstance( IActivationFactory *iface, IInspectable **instance )
{
    return pointer_vis_create( (void **)instance );
}

static HRESULT WINAPI pointer_vis_statics_GetForCurrentView( void *iface, void **value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    return pointer_vis_create( value );
}

static const void *pointer_vis_factory_vtbl[] =
{
    (void *)pointer_vis_factory_QueryInterface,
    (void *)pointer_vis_factory_AddRef,
    (void *)pointer_vis_factory_Release,
    (void *)pointer_vis_factory_GetIids,
    (void *)pointer_vis_factory_GetRuntimeClassName,
    (void *)pointer_vis_factory_GetTrustLevel,
    (void *)pointer_vis_factory_ActivateInstance,
    (void *)pointer_vis_statics_GetForCurrentView
};

static struct { const void *lpVtbl; } default_pointer_vis_factory = { pointer_vis_factory_vtbl };
IActivationFactory *pointer_visualization_settings_factory = (IActivationFactory *)&default_pointer_vis_factory;
