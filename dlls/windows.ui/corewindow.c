/* WinRT Windows.UI.Core.CoreWindow Implementation
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

#include "private.h"
#include <initguid.h>
#include <eventtoken.h>
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(ui);

static EventRegistrationToken dummy_cookie = {.value = 0xdeadbeef};

static const GUID IID_IWeakReferenceSource = {0x00000038, 0x0000, 0x0000, {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
static const GUID IID_ComWrappers_internal = {0x11d3b13a, 0x180e, 0x4789, {0xa8, 0xbe, 0x77, 0x12, 0x88, 0x28, 0x93, 0xe6}};
static const GUID IID_IManagedObjectTracker1 = {0x5c13e51c, 0x4f32, 0x4726, {0xa3, 0xfd, 0xf3, 0xed, 0xd6, 0x3d, 0xa3, 0xa0}};
static const GUID IID_IManagedObjectTracker2 = {0x5c13e51c, 0xc760, 0x496f, {0xa6, 0x5a, 0x09, 0x65, 0xd1, 0xd6, 0x04, 0xa8}};

struct core_window
{
    ICoreWindow ICoreWindow_iface;
    LONG ref;
};

static inline struct core_window *impl_from_ICoreWindow( ICoreWindow *iface )
{
    return CONTAINING_RECORD( iface, struct core_window, ICoreWindow_iface );
}

static HRESULT WINAPI core_window_QueryInterface( ICoreWindow *iface, REFIID iid, void **out )
{
    struct core_window *impl = impl_from_ICoreWindow( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_ICoreWindow ))
    {
        *out = &impl->ICoreWindow_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IWeakReferenceSource ) ||
        IsEqualGUID( iid, &IID_ComWrappers_internal ) ||
        IsEqualGUID( iid, &IID_IManagedObjectTracker1 ) ||
        IsEqualGUID( iid, &IID_IManagedObjectTracker2 ))
    {
        *out = NULL;
        return E_NOINTERFACE;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI core_window_AddRef( ICoreWindow *iface )
{
    struct core_window *impl = impl_from_ICoreWindow( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI core_window_Release( ICoreWindow *iface )
{
    struct core_window *impl = impl_from_ICoreWindow( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );
    if (!ref)
        free( impl );
    return ref;
}

static HRESULT WINAPI core_window_GetIids( ICoreWindow *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI core_window_GetRuntimeClassName( ICoreWindow *iface, HSTRING *class_name )
{
    return WindowsCreateString( RuntimeClass_Windows_UI_Core_CoreWindow,
                                ARRAY_SIZE(RuntimeClass_Windows_UI_Core_CoreWindow) - 1,
                                class_name );
}

static HRESULT WINAPI core_window_GetTrustLevel( ICoreWindow *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_POINTER;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI core_window_get_AutomationHostProvider( ICoreWindow *iface, IInspectable **value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    *value = NULL;
    return S_OK;
}

static HRESULT WINAPI core_window_get_Bounds( ICoreWindow *iface, Rect *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    value->X = 0;
    value->Y = 0;
    value->Width = 1152;
    value->Height = 578;
    return S_OK;
}

static HRESULT WINAPI core_window_get_CustomProperties( ICoreWindow *iface, IPropertySet **value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    *value = NULL;
    return S_OK;
}

static HRESULT WINAPI core_window_get_Dispatcher( ICoreWindow *iface, ICoreDispatcher **value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    *value = NULL;
    return S_OK;
}

static HRESULT WINAPI core_window_get_FlowDirection( ICoreWindow *iface, CoreWindowFlowDirection *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    *value = 0;
    return S_OK;
}

static HRESULT WINAPI core_window_put_FlowDirection( ICoreWindow *iface, CoreWindowFlowDirection value )
{
    TRACE( "iface %p, value %d\n", iface, value );
    return S_OK;
}

static HRESULT WINAPI core_window_get_IsInputEnabled( ICoreWindow *iface, boolean *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    *value = TRUE;
    return S_OK;
}

static HRESULT WINAPI core_window_put_IsInputEnabled( ICoreWindow *iface, boolean value )
{
    TRACE( "iface %p, value %d\n", iface, value );
    return S_OK;
}

static HRESULT WINAPI core_window_get_PointerCursor( ICoreWindow *iface, ICoreCursor **value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    *value = NULL;
    return S_OK;
}

static HRESULT WINAPI core_window_put_PointerCursor( ICoreWindow *iface, ICoreCursor *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    return S_OK;
}

static HRESULT WINAPI core_window_get_PointerPosition( ICoreWindow *iface, Point *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    value->X = 0;
    value->Y = 0;
    return S_OK;
}

static HRESULT WINAPI core_window_get_Visible( ICoreWindow *iface, boolean *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    *value = TRUE;
    return S_OK;
}

static HRESULT WINAPI core_window_Activate( ICoreWindow *iface )
{
    TRACE( "iface %p\n", iface );
    return S_OK;
}

static HRESULT WINAPI core_window_Close( ICoreWindow *iface )
{
    TRACE( "iface %p\n", iface );
    return S_OK;
}

static HRESULT WINAPI core_window_GetAsyncKeyState( ICoreWindow *iface, VirtualKey key, CoreVirtualKeyStates *state )
{
    TRACE( "iface %p, key %d, state %p\n", iface, key, state );
    if (!state) return E_POINTER;
    *state = 0;
    return S_OK;
}

static HRESULT WINAPI core_window_GetKeyState( ICoreWindow *iface, VirtualKey key, CoreVirtualKeyStates *state )
{
    TRACE( "iface %p, key %d, state %p\n", iface, key, state );
    if (!state) return E_POINTER;
    *state = 0;
    return S_OK;
}

static HRESULT WINAPI core_window_ReleasePointerCapture( ICoreWindow *iface )
{
    TRACE( "iface %p\n", iface );
    return S_OK;
}

static HRESULT WINAPI core_window_SetPointerCapture( ICoreWindow *iface )
{
    TRACE( "iface %p\n", iface );
    return S_OK;
}

static HRESULT WINAPI core_window_add_Activated( ICoreWindow *iface, ITypedEventHandler_CoreWindow_WindowActivatedEventArgs *handler, EventRegistrationToken *cookie )
{
    TRACE( "iface %p, handler %p, cookie %p\n", iface, handler, cookie );
    if (!cookie) return E_POINTER;
    *cookie = dummy_cookie;
    return S_OK;
}

static HRESULT WINAPI core_window_remove_Activated( ICoreWindow *iface, EventRegistrationToken cookie )
{
    TRACE( "iface %p, cookie %#I64x\n", iface, cookie.value );
    return S_OK;
}

static HRESULT WINAPI core_window_add_AutomationProviderRequested( ICoreWindow *iface, ITypedEventHandler_CoreWindow_AutomationProviderRequestedEventArgs *handler, EventRegistrationToken *cookie )
{
    TRACE( "iface %p, handler %p, cookie %p\n", iface, handler, cookie );
    if (!cookie) return E_POINTER;
    *cookie = dummy_cookie;
    return S_OK;
}

static HRESULT WINAPI core_window_remove_AutomationProviderRequested( ICoreWindow *iface, EventRegistrationToken cookie )
{
    TRACE( "iface %p, cookie %#I64x\n", iface, cookie.value );
    return S_OK;
}

static HRESULT WINAPI core_window_add_CharacterReceived( ICoreWindow *iface, ITypedEventHandler_CoreWindow_CharacterReceivedEventArgs *handler, EventRegistrationToken *cookie )
{
    TRACE( "iface %p, handler %p, cookie %p\n", iface, handler, cookie );
    if (!cookie) return E_POINTER;
    *cookie = dummy_cookie;
    return S_OK;
}

static HRESULT WINAPI core_window_remove_CharacterReceived( ICoreWindow *iface, EventRegistrationToken cookie )
{
    TRACE( "iface %p, cookie %#I64x\n", iface, cookie.value );
    return S_OK;
}

static HRESULT WINAPI core_window_add_Closed( ICoreWindow *iface, ITypedEventHandler_CoreWindow_CoreWindowEventArgs *handler, EventRegistrationToken *cookie )
{
    TRACE( "iface %p, handler %p, cookie %p\n", iface, handler, cookie );
    if (!cookie) return E_POINTER;
    *cookie = dummy_cookie;
    return S_OK;
}

static HRESULT WINAPI core_window_remove_Closed( ICoreWindow *iface, EventRegistrationToken cookie )
{
    TRACE( "iface %p, cookie %#I64x\n", iface, cookie.value );
    return S_OK;
}

static HRESULT WINAPI core_window_add_InputEnabled( ICoreWindow *iface, ITypedEventHandler_CoreWindow_InputEnabledEventArgs *handler, EventRegistrationToken *cookie )
{
    TRACE( "iface %p, handler %p, cookie %p\n", iface, handler, cookie );
    if (!cookie) return E_POINTER;
    *cookie = dummy_cookie;
    return S_OK;
}

static HRESULT WINAPI core_window_remove_InputEnabled( ICoreWindow *iface, EventRegistrationToken cookie )
{
    TRACE( "iface %p, cookie %#I64x\n", iface, cookie.value );
    return S_OK;
}

static HRESULT WINAPI core_window_add_KeyDown( ICoreWindow *iface, ITypedEventHandler_CoreWindow_KeyEventArgs *handler, EventRegistrationToken *cookie )
{
    TRACE( "iface %p, handler %p, cookie %p\n", iface, handler, cookie );
    if (!cookie) return E_POINTER;
    *cookie = dummy_cookie;
    return S_OK;
}

static HRESULT WINAPI core_window_remove_KeyDown( ICoreWindow *iface, EventRegistrationToken cookie )
{
    TRACE( "iface %p, cookie %#I64x\n", iface, cookie.value );
    return S_OK;
}

static HRESULT WINAPI core_window_add_KeyUp( ICoreWindow *iface, ITypedEventHandler_CoreWindow_KeyEventArgs *handler, EventRegistrationToken *cookie )
{
    TRACE( "iface %p, handler %p, cookie %p\n", iface, handler, cookie );
    if (!cookie) return E_POINTER;
    *cookie = dummy_cookie;
    return S_OK;
}

static HRESULT WINAPI core_window_remove_KeyUp( ICoreWindow *iface, EventRegistrationToken cookie )
{
    TRACE( "iface %p, cookie %#I64x\n", iface, cookie.value );
    return S_OK;
}

static HRESULT WINAPI core_window_add_PointerCaptureLost( ICoreWindow *iface, ITypedEventHandler_CoreWindow_PointerEventArgs *handler, EventRegistrationToken *cookie )
{
    TRACE( "iface %p, handler %p, cookie %p\n", iface, handler, cookie );
    if (!cookie) return E_POINTER;
    *cookie = dummy_cookie;
    return S_OK;
}

static HRESULT WINAPI core_window_remove_PointerCaptureLost( ICoreWindow *iface, EventRegistrationToken cookie )
{
    TRACE( "iface %p, cookie %#I64x\n", iface, cookie.value );
    return S_OK;
}

static HRESULT WINAPI core_window_add_PointerEntered( ICoreWindow *iface, ITypedEventHandler_CoreWindow_PointerEventArgs *handler, EventRegistrationToken *cookie )
{
    TRACE( "iface %p, handler %p, cookie %p\n", iface, handler, cookie );
    if (!cookie) return E_POINTER;
    *cookie = dummy_cookie;
    return S_OK;
}

static HRESULT WINAPI core_window_remove_PointerEntered( ICoreWindow *iface, EventRegistrationToken cookie )
{
    TRACE( "iface %p, cookie %#I64x\n", iface, cookie.value );
    return S_OK;
}

static HRESULT WINAPI core_window_add_PointerExited( ICoreWindow *iface, ITypedEventHandler_CoreWindow_PointerEventArgs *handler, EventRegistrationToken *cookie )
{
    TRACE( "iface %p, handler %p, cookie %p\n", iface, handler, cookie );
    if (!cookie) return E_POINTER;
    *cookie = dummy_cookie;
    return S_OK;
}

static HRESULT WINAPI core_window_remove_PointerExited( ICoreWindow *iface, EventRegistrationToken cookie )
{
    TRACE( "iface %p, cookie %#I64x\n", iface, cookie.value );
    return S_OK;
}

static HRESULT WINAPI core_window_add_PointerMoved( ICoreWindow *iface, ITypedEventHandler_CoreWindow_PointerEventArgs *handler, EventRegistrationToken *cookie )
{
    TRACE( "iface %p, handler %p, cookie %p\n", iface, handler, cookie );
    if (!cookie) return E_POINTER;
    *cookie = dummy_cookie;
    return S_OK;
}

static HRESULT WINAPI core_window_remove_PointerMoved( ICoreWindow *iface, EventRegistrationToken cookie )
{
    TRACE( "iface %p, cookie %#I64x\n", iface, cookie.value );
    return S_OK;
}

static HRESULT WINAPI core_window_add_PointerPressed( ICoreWindow *iface, ITypedEventHandler_CoreWindow_PointerEventArgs *handler, EventRegistrationToken *cookie )
{
    TRACE( "iface %p, handler %p, cookie %p\n", iface, handler, cookie );
    if (!cookie) return E_POINTER;
    *cookie = dummy_cookie;
    return S_OK;
}

static HRESULT WINAPI core_window_remove_PointerPressed( ICoreWindow *iface, EventRegistrationToken cookie )
{
    TRACE( "iface %p, cookie %#I64x\n", iface, cookie.value );
    return S_OK;
}

static HRESULT WINAPI core_window_add_PointerReleased( ICoreWindow *iface, ITypedEventHandler_CoreWindow_PointerEventArgs *handler, EventRegistrationToken *cookie )
{
    TRACE( "iface %p, handler %p, cookie %p\n", iface, handler, cookie );
    if (!cookie) return E_POINTER;
    *cookie = dummy_cookie;
    return S_OK;
}

static HRESULT WINAPI core_window_remove_PointerReleased( ICoreWindow *iface, EventRegistrationToken cookie )
{
    TRACE( "iface %p, cookie %#I64x\n", iface, cookie.value );
    return S_OK;
}

static HRESULT WINAPI core_window_add_TouchHitTesting( ICoreWindow *iface, ITypedEventHandler_CoreWindow_TouchHitTestingEventArgs *handler, EventRegistrationToken *cookie )
{
    TRACE( "iface %p, handler %p, cookie %p\n", iface, handler, cookie );
    if (!cookie) return E_POINTER;
    *cookie = dummy_cookie;
    return S_OK;
}

static HRESULT WINAPI core_window_remove_TouchHitTesting( ICoreWindow *iface, EventRegistrationToken cookie )
{
    TRACE( "iface %p, cookie %#I64x\n", iface, cookie.value );
    return S_OK;
}

static HRESULT WINAPI core_window_add_PointerWheelChanged( ICoreWindow *iface, ITypedEventHandler_CoreWindow_PointerEventArgs *handler, EventRegistrationToken *cookie )
{
    TRACE( "iface %p, handler %p, cookie %p\n", iface, handler, cookie );
    if (!cookie) return E_POINTER;
    *cookie = dummy_cookie;
    return S_OK;
}

static HRESULT WINAPI core_window_remove_PointerWheelChanged( ICoreWindow *iface, EventRegistrationToken cookie )
{
    TRACE( "iface %p, cookie %#I64x\n", iface, cookie.value );
    return S_OK;
}

static HRESULT WINAPI core_window_add_SizeChanged( ICoreWindow *iface, ITypedEventHandler_CoreWindow_WindowSizeChangedEventArgs *handler, EventRegistrationToken *cookie )
{
    TRACE( "iface %p, handler %p, cookie %p\n", iface, handler, cookie );
    if (!cookie) return E_POINTER;
    *cookie = dummy_cookie;
    return S_OK;
}

static HRESULT WINAPI core_window_remove_SizeChanged( ICoreWindow *iface, EventRegistrationToken cookie )
{
    TRACE( "iface %p, cookie %#I64x\n", iface, cookie.value );
    return S_OK;
}

static HRESULT WINAPI core_window_add_VisibilityChanged( ICoreWindow *iface, ITypedEventHandler_CoreWindow_VisibilityChangedEventArgs *handler, EventRegistrationToken *cookie )
{
    TRACE( "iface %p, handler %p, cookie %p\n", iface, handler, cookie );
    if (!cookie) return E_POINTER;
    *cookie = dummy_cookie;
    return S_OK;
}

static HRESULT WINAPI core_window_remove_VisibilityChanged( ICoreWindow *iface, EventRegistrationToken cookie )
{
    TRACE( "iface %p, cookie %#I64x\n", iface, cookie.value );
    return S_OK;
}

static const ICoreWindowVtbl core_window_vtbl =
{
    core_window_QueryInterface,
    core_window_AddRef,
    core_window_Release,
    /* IInspectable methods */
    core_window_GetIids,
    core_window_GetRuntimeClassName,
    core_window_GetTrustLevel,
    /* ICoreWindow methods */
    core_window_get_AutomationHostProvider,
    core_window_get_Bounds,
    core_window_get_CustomProperties,
    core_window_get_Dispatcher,
    core_window_get_FlowDirection,
    core_window_put_FlowDirection,
    core_window_get_IsInputEnabled,
    core_window_put_IsInputEnabled,
    core_window_get_PointerCursor,
    core_window_put_PointerCursor,
    core_window_get_PointerPosition,
    core_window_get_Visible,
    core_window_Activate,
    core_window_Close,
    core_window_GetAsyncKeyState,
    core_window_GetKeyState,
    core_window_ReleasePointerCapture,
    core_window_SetPointerCapture,
    core_window_add_Activated,
    core_window_remove_Activated,
    core_window_add_AutomationProviderRequested,
    core_window_remove_AutomationProviderRequested,
    core_window_add_CharacterReceived,
    core_window_remove_CharacterReceived,
    core_window_add_Closed,
    core_window_remove_Closed,
    core_window_add_InputEnabled,
    core_window_remove_InputEnabled,
    core_window_add_KeyDown,
    core_window_remove_KeyDown,
    core_window_add_KeyUp,
    core_window_remove_KeyUp,
    core_window_add_PointerCaptureLost,
    core_window_remove_PointerCaptureLost,
    core_window_add_PointerEntered,
    core_window_remove_PointerEntered,
    core_window_add_PointerExited,
    core_window_remove_PointerExited,
    core_window_add_PointerMoved,
    core_window_remove_PointerMoved,
    core_window_add_PointerPressed,
    core_window_remove_PointerPressed,
    core_window_add_PointerReleased,
    core_window_remove_PointerReleased,
    core_window_add_TouchHitTesting,
    core_window_remove_TouchHitTesting,
    core_window_add_PointerWheelChanged,
    core_window_remove_PointerWheelChanged,
    core_window_add_SizeChanged,
    core_window_remove_SizeChanged,
    core_window_add_VisibilityChanged,
    core_window_remove_VisibilityChanged,
};

static HRESULT core_window_create( ICoreWindow **window )
{
    struct core_window *impl;

    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->ICoreWindow_iface.lpVtbl = &core_window_vtbl;
    impl->ref = 1;
    *window = &impl->ICoreWindow_iface;
    return S_OK;
}

/* CoreWindow factory and statics */
struct core_window_statics
{
    IActivationFactory IActivationFactory_iface;
    ICoreWindowStatic ICoreWindowStatic_iface;
    LONG ref;
};

static inline struct core_window_statics *impl_from_IActivationFactory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct core_window_statics, IActivationFactory_iface );
}

static inline struct core_window_statics *impl_from_ICoreWindowStatic( ICoreWindowStatic *iface )
{
    return CONTAINING_RECORD( iface, struct core_window_statics, ICoreWindowStatic_iface );
}

static HRESULT WINAPI core_window_factory_QueryInterface( IActivationFactory *iface, REFIID iid, void **out )
{
    struct core_window_statics *impl = impl_from_IActivationFactory( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IActivationFactory ))
    {
        *out = &impl->IActivationFactory_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_ICoreWindowStatic ))
    {
        *out = &impl->ICoreWindowStatic_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IWeakReferenceSource ) ||
        IsEqualGUID( iid, &IID_ComWrappers_internal ) ||
        IsEqualGUID( iid, &IID_IManagedObjectTracker1 ) ||
        IsEqualGUID( iid, &IID_IManagedObjectTracker2 ))
    {
        *out = NULL;
        return E_NOINTERFACE;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI core_window_factory_AddRef( IActivationFactory *iface )
{
    struct core_window_statics *impl = impl_from_IActivationFactory( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI core_window_factory_Release( IActivationFactory *iface )
{
    struct core_window_statics *impl = impl_from_IActivationFactory( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );
    return ref;
}

static HRESULT WINAPI core_window_factory_GetIids( IActivationFactory *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI core_window_factory_GetRuntimeClassName( IActivationFactory *iface, HSTRING *class_name )
{
    return WindowsCreateString( RuntimeClass_Windows_UI_Core_CoreWindow,
                                ARRAY_SIZE(RuntimeClass_Windows_UI_Core_CoreWindow) - 1,
                                class_name );
}

static HRESULT WINAPI core_window_factory_GetTrustLevel( IActivationFactory *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_POINTER;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI core_window_factory_ActivateInstance( IActivationFactory *iface, IInspectable **instance )
{
    FIXME( "iface %p, instance %p stub!\n", iface, instance );
    return E_NOTIMPL;
}

static const struct IActivationFactoryVtbl core_window_factory_vtbl =
{
    core_window_factory_QueryInterface,
    core_window_factory_AddRef,
    core_window_factory_Release,
    /* IInspectable methods */
    core_window_factory_GetIids,
    core_window_factory_GetRuntimeClassName,
    core_window_factory_GetTrustLevel,
    /* IActivationFactory methods */
    core_window_factory_ActivateInstance,
};

static HRESULT WINAPI core_window_statics_QueryInterface( ICoreWindowStatic *iface, REFIID iid, void **out )
{
    struct core_window_statics *impl = impl_from_ICoreWindowStatic( iface );
    return core_window_factory_QueryInterface( &impl->IActivationFactory_iface, iid, out );
}

static ULONG WINAPI core_window_statics_AddRef( ICoreWindowStatic *iface )
{
    struct core_window_statics *impl = impl_from_ICoreWindowStatic( iface );
    return core_window_factory_AddRef( &impl->IActivationFactory_iface );
}

static ULONG WINAPI core_window_statics_Release( ICoreWindowStatic *iface )
{
    struct core_window_statics *impl = impl_from_ICoreWindowStatic( iface );
    return core_window_factory_Release( &impl->IActivationFactory_iface );
}

static HRESULT WINAPI core_window_statics_GetIids( ICoreWindowStatic *iface, ULONG *iid_count, IID **iids )
{
    struct core_window_statics *impl = impl_from_ICoreWindowStatic( iface );
    return core_window_factory_GetIids( &impl->IActivationFactory_iface, iid_count, iids );
}

static HRESULT WINAPI core_window_statics_GetRuntimeClassName( ICoreWindowStatic *iface, HSTRING *class_name )
{
    struct core_window_statics *impl = impl_from_ICoreWindowStatic( iface );
    return core_window_factory_GetRuntimeClassName( &impl->IActivationFactory_iface, class_name );
}

static HRESULT WINAPI core_window_statics_GetTrustLevel( ICoreWindowStatic *iface, TrustLevel *trust_level )
{
    struct core_window_statics *impl = impl_from_ICoreWindowStatic( iface );
    return core_window_factory_GetTrustLevel( &impl->IActivationFactory_iface, trust_level );
}

static HRESULT WINAPI core_window_statics_GetForCurrentThread( ICoreWindowStatic *iface, ICoreWindow **windows )
{
    TRACE( "iface %p, windows %p\n", iface, windows );
    if (!windows) return E_POINTER;
    /* In a Win32 desktop application, threads hosting AppWindow/HWND do not have a CoreWindow.
     * Returning a non-NULL CoreWindow violates WinUI 3 desktop thread state and triggers
     * ERROR_INVALID_STATE (0x8007139f) FailFast. */
    *windows = NULL;
    return S_OK;
}

static const ICoreWindowStaticVtbl core_window_statics_vtbl =
{
    core_window_statics_QueryInterface,
    core_window_statics_AddRef,
    core_window_statics_Release,
    /* IInspectable methods */
    core_window_statics_GetIids,
    core_window_statics_GetRuntimeClassName,
    core_window_statics_GetTrustLevel,
    /* ICoreWindowStatic methods */
    core_window_statics_GetForCurrentThread,
};

static struct core_window_statics core_window_statics =
{
    {&core_window_factory_vtbl},
    {&core_window_statics_vtbl},
    1,
};

IActivationFactory *corewindow_factory = &core_window_statics.IActivationFactory_iface;
