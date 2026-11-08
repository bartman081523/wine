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
static const GUID IID_ICoreWindowInterop = {0x45d64a29, 0xa63e, 0x4cb6, {0xb4, 0x98, 0x57, 0x81, 0xd2, 0x98, 0xcb, 0x4f}};
static const GUID IID_IMouseDevice = {0x79b9d5f2, 0x879e, 0x4b89, {0xb7, 0x98, 0x79, 0xe4, 0x75, 0x98, 0x03, 0x0c}};

struct core_window
{
    ICoreWindow ICoreWindow_iface;
    ICoreWindow2 ICoreWindow2_iface;
    ICoreWindow3 ICoreWindow3_iface;
    ICoreWindow4 ICoreWindow4_iface;
    ICoreWindow5 ICoreWindow5_iface;
    struct { const void *lpVtbl; } ICoreWindowInterop_iface;
    HWND hwnd;
    LONG ref;
    ITypedEventHandler_CoreWindow_WindowActivatedEventArgs *activated_handler;
    ITypedEventHandler_CoreWindow_PointerEventArgs *pointer_pressed_handler;
    ITypedEventHandler_CoreWindow_PointerEventArgs *pointer_released_handler;
    ITypedEventHandler_CoreWindow_PointerEventArgs *pointer_moved_handler;
    ITypedEventHandler_CoreWindow_PointerEventArgs *pointer_entered_handler;
    ITypedEventHandler_CoreWindow_PointerEventArgs *pointer_exited_handler;
    ITypedEventHandler_CoreWindow_PointerEventArgs *pointer_wheel_handler;
    ITypedEventHandler_CoreWindow_PointerEventArgs *pointer_capture_lost_handler;
    ITypedEventHandler_CoreWindow_KeyEventArgs *key_down_handler;
    ITypedEventHandler_CoreWindow_KeyEventArgs *key_up_handler;
    ITypedEventHandler_CoreWindow_CharacterReceivedEventArgs *char_handler;
    ITypedEventHandler_CoreWindow_WindowSizeChangedEventArgs *size_changed_handler;
    ITypedEventHandler_CoreWindow_VisibilityChangedEventArgs *visibility_handler;
    void *mouse_device;
    BOOL pointer_captured;
    BOOL cursor_hidden;
    int last_mouse_x;
    int last_mouse_y;
    BOOL is_fullscreen;
    RECT saved_rect;
    DWORD saved_style;
    ICoreDispatcher *dispatcher;
};

static inline struct core_window *impl_from_ICoreWindow( ICoreWindow *iface )
{
    return CONTAINING_RECORD( iface, struct core_window, ICoreWindow_iface );
}

static inline struct core_window *impl_from_ICoreWindowInterop( void *iface )
{
    return CONTAINING_RECORD( iface, struct core_window, ICoreWindowInterop_iface );
}

static inline struct core_window *impl_from_ICoreWindow2( ICoreWindow2 *iface )
{
    return CONTAINING_RECORD( iface, struct core_window, ICoreWindow2_iface );
}

static inline struct core_window *impl_from_ICoreWindow3( ICoreWindow3 *iface )
{
    return CONTAINING_RECORD( iface, struct core_window, ICoreWindow3_iface );
}

static inline struct core_window *impl_from_ICoreWindow4( ICoreWindow4 *iface )
{
    return CONTAINING_RECORD( iface, struct core_window, ICoreWindow4_iface );
}

static inline struct core_window *impl_from_ICoreWindow5( ICoreWindow5 *iface )
{
    return CONTAINING_RECORD( iface, struct core_window, ICoreWindow5_iface );
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

    if (IsEqualGUID( iid, &IID_ICoreWindow2 ))
    {
        *out = &impl->ICoreWindow2_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_ICoreWindow3 ))
    {
        *out = &impl->ICoreWindow3_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_ICoreWindow4 ))
    {
        *out = &impl->ICoreWindow4_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_ICoreWindow5 ))
    {
        *out = &impl->ICoreWindow5_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_ICoreWindowInterop ))
    {
        *out = &impl->ICoreWindowInterop_iface;
        IInspectable_AddRef( (IInspectable *)&impl->ICoreWindow_iface );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IMouseDevice ))
    {
        if (!impl->mouse_device && mouse_device_factory)
            IActivationFactory_ActivateInstance( mouse_device_factory, (IInspectable **)&impl->mouse_device );
        *out = impl->mouse_device;
        if (*out)
        {
            IUnknown_AddRef( (IUnknown *)*out );
            return S_OK;
        }
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
    {
        if (impl->activated_handler)
            ITypedEventHandler_CoreWindow_WindowActivatedEventArgs_Release( impl->activated_handler );
        if (impl->pointer_pressed_handler)
            ITypedEventHandler_CoreWindow_PointerEventArgs_Release( impl->pointer_pressed_handler );
        if (impl->pointer_released_handler)
            ITypedEventHandler_CoreWindow_PointerEventArgs_Release( impl->pointer_released_handler );
        if (impl->pointer_moved_handler)
            ITypedEventHandler_CoreWindow_PointerEventArgs_Release( impl->pointer_moved_handler );
        if (impl->pointer_entered_handler)
            ITypedEventHandler_CoreWindow_PointerEventArgs_Release( impl->pointer_entered_handler );
        if (impl->pointer_exited_handler)
            ITypedEventHandler_CoreWindow_PointerEventArgs_Release( impl->pointer_exited_handler );
        if (impl->pointer_wheel_handler)
            ITypedEventHandler_CoreWindow_PointerEventArgs_Release( impl->pointer_wheel_handler );
        if (impl->pointer_capture_lost_handler)
            ITypedEventHandler_CoreWindow_PointerEventArgs_Release( impl->pointer_capture_lost_handler );
        if (impl->key_down_handler)
            ITypedEventHandler_CoreWindow_KeyEventArgs_Release( impl->key_down_handler );
        if (impl->key_up_handler)
            ITypedEventHandler_CoreWindow_KeyEventArgs_Release( impl->key_up_handler );
        if (impl->char_handler)
            ITypedEventHandler_CoreWindow_CharacterReceivedEventArgs_Release( impl->char_handler );
        if (impl->size_changed_handler)
            ITypedEventHandler_CoreWindow_WindowSizeChangedEventArgs_Release( impl->size_changed_handler );
        if (impl->visibility_handler)
            ITypedEventHandler_CoreWindow_VisibilityChangedEventArgs_Release( impl->visibility_handler );
        if (impl->dispatcher)
            ICoreDispatcher_Release( impl->dispatcher );
        if (impl->hwnd)
            SetWindowLongPtrW( impl->hwnd, GWLP_USERDATA, 0 );
        free( impl );
    }
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
    struct core_window *impl = impl_from_ICoreWindow( iface );
    RECT rect;
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    if (impl->hwnd && GetClientRect( impl->hwnd, &rect ))
    {
        value->X = 0;
        value->Y = 0;
        value->Width = rect.right - rect.left;
        value->Height = rect.bottom - rect.top;
    }
    else
    {
        value->X = 0;
        value->Y = 0;
        value->Width = 1280;
        value->Height = 720;
    }
    return S_OK;
}

static HRESULT WINAPI core_window_get_CustomProperties( ICoreWindow *iface, IPropertySet **value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    *value = NULL;
    return S_OK;
}

static const GUID IID_ICoreDispatcher2 =
    {0xbafaecad, 0x484d, 0x41be, {0xba, 0x80, 0x1d, 0x58, 0xc6, 0x52, 0x63, 0xea}};

struct core_dispatcher
{
    ICoreDispatcher ICoreDispatcher_iface;
    struct { const void *lpVtbl; } ICoreDispatcher2_iface;
    ICoreAcceleratorKeys ICoreAcceleratorKeys_iface;
    LONG ref;
    ITypedEventHandler_CoreDispatcher_AcceleratorKeyEventArgs *accel_key_handler;
};

static inline struct core_dispatcher *impl_from_ICoreDispatcher( ICoreDispatcher *iface )
{
    return CONTAINING_RECORD( iface, struct core_dispatcher, ICoreDispatcher_iface );
}

static inline struct core_dispatcher *impl_from_ICoreDispatcher2( void *iface )
{
    return CONTAINING_RECORD( iface, struct core_dispatcher, ICoreDispatcher2_iface );
}

static inline struct core_dispatcher *impl_from_ICoreAcceleratorKeys( ICoreAcceleratorKeys *iface )
{
    return CONTAINING_RECORD( iface, struct core_dispatcher, ICoreAcceleratorKeys_iface );
}

static HRESULT WINAPI core_dispatcher_QueryInterface( ICoreDispatcher *iface, REFIID iid, void **out )
{
    struct core_dispatcher *impl = impl_from_ICoreDispatcher( iface );

    if (!out) return E_POINTER;
    *out = NULL;

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_ICoreDispatcher ))
    {
        *out = &impl->ICoreDispatcher_iface;
        IInspectable_AddRef( (IInspectable *)*out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_ICoreDispatcher2 ))
    {
        *out = &impl->ICoreDispatcher2_iface;
        IInspectable_AddRef( (IInspectable *)&impl->ICoreDispatcher_iface );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_ICoreAcceleratorKeys ))
    {
        *out = &impl->ICoreAcceleratorKeys_iface;
        IInspectable_AddRef( (IInspectable *)&impl->ICoreDispatcher_iface );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    return E_NOINTERFACE;
}

static HRESULT WINAPI core_dispatcher2_QueryInterface( void *iface, REFIID riid, void **out )
{
    struct core_dispatcher *impl = impl_from_ICoreDispatcher2( iface );
    return core_dispatcher_QueryInterface( &impl->ICoreDispatcher_iface, riid, out );
}

static ULONG WINAPI core_dispatcher2_AddRef( void *iface )
{
    struct core_dispatcher *impl = impl_from_ICoreDispatcher2( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI core_dispatcher2_Release( void *iface )
{
    struct core_dispatcher *impl = impl_from_ICoreDispatcher2( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI core_dispatcher2_GetIids( void *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI core_dispatcher2_GetRuntimeClassName( void *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.UI.Core.CoreDispatcher", 30, class_name );
}

static HRESULT WINAPI core_dispatcher2_GetTrustLevel( void *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_POINTER;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI core_dispatcher2_TryRunAsync( void *iface, CoreDispatcherPriority priority,
                                                    void *callback, void **operation )
{
    TRACE( "iface %p, priority %d, callback %p, operation %p\n", iface, priority, callback, operation );
    if (operation) *operation = NULL;
    return S_OK;
}

static HRESULT WINAPI core_dispatcher2_ShouldYield( void *iface, boolean *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    *value = FALSE;
    return S_OK;
}

static HRESULT WINAPI core_dispatcher2_ShouldYieldToPriority( void *iface, CoreDispatcherPriority priority, boolean *value )
{
    TRACE( "iface %p, priority %d, value %p\n", iface, priority, value );
    if (!value) return E_POINTER;
    *value = FALSE;
    return S_OK;
}

static HRESULT WINAPI core_dispatcher2_stub( void *iface )
{
    WARN( "iface %p stub!\n", iface );
    return S_OK;
}

static const void *core_dispatcher2_vtbl[] =
{
    (void *)core_dispatcher2_QueryInterface,
    (void *)core_dispatcher2_AddRef,
    (void *)core_dispatcher2_Release,
    (void *)core_dispatcher2_GetIids,
    (void *)core_dispatcher2_GetRuntimeClassName,
    (void *)core_dispatcher2_GetTrustLevel,
    (void *)core_dispatcher2_TryRunAsync,          /* [6] 0x30 */
    (void *)core_dispatcher2_ShouldYield,          /* [7] 0x38 */
    (void *)core_dispatcher2_ShouldYield,          /* [8] 0x40 */
    (void *)core_dispatcher2_ShouldYieldToPriority,/* [9] 0x48 */
    (void *)core_dispatcher2_stub,                 /* [10] 0x50 */
    (void *)core_dispatcher2_stub,                 /* [11] 0x58 */
    (void *)core_dispatcher2_stub,                 /* [12] 0x60 */
    (void *)core_dispatcher2_stub,                 /* [13] 0x68 */
    (void *)core_dispatcher2_stub,                 /* [14] 0x70 */
    (void *)core_dispatcher2_stub                  /* [15] 0x78 */
};

static ULONG WINAPI core_dispatcher_AddRef( ICoreDispatcher *iface )
{
    struct core_dispatcher *impl = impl_from_ICoreDispatcher( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI core_dispatcher_Release( ICoreDispatcher *iface )
{
    struct core_dispatcher *impl = impl_from_ICoreDispatcher( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    if (!ref)
    {
        if (impl->accel_key_handler)
            ITypedEventHandler_CoreDispatcher_AcceleratorKeyEventArgs_Release( impl->accel_key_handler );
        free( impl );
    }
    return ref;
}

static HRESULT WINAPI core_dispatcher_GetIids( ICoreDispatcher *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI core_dispatcher_GetRuntimeClassName( ICoreDispatcher *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.UI.Core.CoreDispatcher", 30, class_name );
}

static HRESULT WINAPI core_dispatcher_GetTrustLevel( ICoreDispatcher *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_POINTER;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI core_dispatcher_get_HasThreadAccess( ICoreDispatcher *iface, boolean *value )
{
    if (!value) return E_POINTER;
    *value = TRUE;
    return S_OK;
}

static HRESULT WINAPI core_dispatcher_ProcessEvents( ICoreDispatcher *iface, CoreProcessEventsOption options )
{
    MSG msg;
    while (PeekMessageW( &msg, 0, 0, 0, PM_REMOVE ))
    {
        TranslateMessage( &msg );
        DispatchMessageW( &msg );
    }
    return S_OK;
}

struct async_action
{
    IAsyncAction IAsyncAction_iface;
    IAsyncInfo IAsyncInfo_iface;
    LONG ref;
    HRESULT hr;
    AsyncStatus status;
    IAsyncActionCompletedHandler *handler;
};

static inline struct async_action *impl_from_IAsyncAction( IAsyncAction *iface )
{
    return CONTAINING_RECORD( iface, struct async_action, IAsyncAction_iface );
}

static inline struct async_action *impl_from_async_action_IAsyncInfo( IAsyncInfo *iface )
{
    return CONTAINING_RECORD( iface, struct async_action, IAsyncInfo_iface );
}

static HRESULT WINAPI async_action_QueryInterface( IAsyncAction *iface, REFIID iid, void **out )
{
    struct async_action *impl = impl_from_IAsyncAction( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAsyncAction ))
    {
        *out = &impl->IAsyncAction_iface;
        IAsyncAction_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IAsyncInfo ))
    {
        *out = &impl->IAsyncInfo_iface;
        IAsyncInfo_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI async_action_AddRef( IAsyncAction *iface )
{
    struct async_action *impl = impl_from_IAsyncAction( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p, ref %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI async_action_Release( IAsyncAction *iface )
{
    struct async_action *impl = impl_from_IAsyncAction( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p, ref %lu.\n", iface, ref );
    if (!ref)
    {
        if (impl->handler) IAsyncActionCompletedHandler_Release( impl->handler );
        free( impl );
    }
    return ref;
}

static HRESULT WINAPI async_action_GetIids( IAsyncAction *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI async_action_GetRuntimeClassName( IAsyncAction *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.IAsyncAction", 31, class_name );
}

static HRESULT WINAPI async_action_GetTrustLevel( IAsyncAction *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI async_action_put_Completed( IAsyncAction *iface, IAsyncActionCompletedHandler *handler )
{
    struct async_action *impl = impl_from_IAsyncAction( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );

    if (impl->handler) IAsyncActionCompletedHandler_Release( impl->handler );
    if ((impl->handler = handler)) IAsyncActionCompletedHandler_AddRef( handler );

    if (handler)
        IAsyncActionCompletedHandler_Invoke( handler, &impl->IAsyncAction_iface, impl->status );

    return S_OK;
}

static HRESULT WINAPI async_action_get_Completed( IAsyncAction *iface, IAsyncActionCompletedHandler **handler )
{
    struct async_action *impl = impl_from_IAsyncAction( iface );
    if (!handler) return E_POINTER;
    *handler = impl->handler;
    if (*handler) IAsyncActionCompletedHandler_AddRef( *handler );
    return S_OK;
}

static HRESULT WINAPI async_action_GetResults( IAsyncAction *iface )
{
    struct async_action *impl = impl_from_IAsyncAction( iface );
    return impl->hr;
}

static const struct IAsyncActionVtbl async_action_vtbl =
{
    async_action_QueryInterface,
    async_action_AddRef,
    async_action_Release,
    async_action_GetIids,
    async_action_GetRuntimeClassName,
    async_action_GetTrustLevel,
    async_action_put_Completed,
    async_action_get_Completed,
    async_action_GetResults,
};

static HRESULT WINAPI async_action_info_QueryInterface( IAsyncInfo *iface, REFIID iid, void **out )
{
    struct async_action *impl = impl_from_async_action_IAsyncInfo( iface );
    return async_action_QueryInterface( &impl->IAsyncAction_iface, iid, out );
}

static ULONG WINAPI async_action_info_AddRef( IAsyncInfo *iface )
{
    struct async_action *impl = impl_from_async_action_IAsyncInfo( iface );
    return async_action_AddRef( &impl->IAsyncAction_iface );
}

static ULONG WINAPI async_action_info_Release( IAsyncInfo *iface )
{
    struct async_action *impl = impl_from_async_action_IAsyncInfo( iface );
    return async_action_Release( &impl->IAsyncAction_iface );
}

static HRESULT WINAPI async_action_info_GetIids( IAsyncInfo *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI async_action_info_GetRuntimeClassName( IAsyncInfo *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.IAsyncInfo", 27, class_name );
}

static HRESULT WINAPI async_action_info_GetTrustLevel( IAsyncInfo *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI async_action_info_get_Id( IAsyncInfo *iface, UINT32 *id )
{
    if (!id) return E_INVALIDARG;
    *id = 1;
    return S_OK;
}

static HRESULT WINAPI async_action_info_get_Status( IAsyncInfo *iface, AsyncStatus *status )
{
    struct async_action *impl = impl_from_async_action_IAsyncInfo( iface );
    if (!status) return E_INVALIDARG;
    *status = impl->status;
    return S_OK;
}

static HRESULT WINAPI async_action_info_get_ErrorCode( IAsyncInfo *iface, HRESULT *error_code )
{
    struct async_action *impl = impl_from_async_action_IAsyncInfo( iface );
    if (!error_code) return E_INVALIDARG;
    *error_code = impl->hr;
    return S_OK;
}

static HRESULT WINAPI async_action_info_Cancel( IAsyncInfo *iface )
{
    return S_OK;
}

static HRESULT WINAPI async_action_info_Close( IAsyncInfo *iface )
{
    return S_OK;
}

static const struct IAsyncInfoVtbl async_action_info_vtbl =
{
    async_action_info_QueryInterface,
    async_action_info_AddRef,
    async_action_info_Release,
    async_action_info_GetIids,
    async_action_info_GetRuntimeClassName,
    async_action_info_GetTrustLevel,
    async_action_info_get_Id,
    async_action_info_get_Status,
    async_action_info_get_ErrorCode,
    async_action_info_Cancel,
    async_action_info_Close,
};

static HRESULT async_action_create( HRESULT hr, AsyncStatus status, IAsyncAction **out )
{
    struct async_action *impl;
    if (!out) return E_INVALIDARG;
    *out = NULL;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IAsyncAction_iface.lpVtbl = &async_action_vtbl;
    impl->IAsyncInfo_iface.lpVtbl = &async_action_info_vtbl;
    impl->ref = 1;
    impl->hr = hr;
    impl->status = status;
    *out = &impl->IAsyncAction_iface;
    return S_OK;
}

static HRESULT WINAPI core_dispatcher_RunAsync( ICoreDispatcher *iface, CoreDispatcherPriority priority,
                                               IDispatchedHandler *callback, IAsyncAction **action )
{
    HRESULT hr = S_OK;
    TRACE( "priority %d, callback %p, action %p\n", priority, callback, action );
    if (callback)
        hr = IDispatchedHandler_Invoke( callback );
    if (action)
        return async_action_create( hr, Completed, action );
    return hr;
}

static HRESULT WINAPI core_dispatcher_RunIdleAsync( ICoreDispatcher *iface, IIdleDispatchedHandler *callback,
                                                   IAsyncAction **action )
{
    HRESULT hr = S_OK;
    TRACE( "callback %p, action %p\n", callback, action );
    if (callback)
        hr = IIdleDispatchedHandler_Invoke( callback, NULL );
    if (action)
        return async_action_create( hr, Completed, action );
    return hr;
}

static const ICoreDispatcherVtbl core_dispatcher_vtbl =
{
    core_dispatcher_QueryInterface,
    core_dispatcher_AddRef,
    core_dispatcher_Release,
    core_dispatcher_GetIids,
    core_dispatcher_GetRuntimeClassName,
    core_dispatcher_GetTrustLevel,
    core_dispatcher_get_HasThreadAccess,
    core_dispatcher_ProcessEvents,
    core_dispatcher_RunAsync,
    core_dispatcher_RunIdleAsync,
};

static HRESULT WINAPI core_dispatcher_accel_QueryInterface( ICoreAcceleratorKeys *iface, REFIID riid, void **out )
{
    struct core_dispatcher *impl = impl_from_ICoreAcceleratorKeys( iface );
    return core_dispatcher_QueryInterface( &impl->ICoreDispatcher_iface, riid, out );
}

static ULONG WINAPI core_dispatcher_accel_AddRef( ICoreAcceleratorKeys *iface )
{
    struct core_dispatcher *impl = impl_from_ICoreAcceleratorKeys( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI core_dispatcher_accel_Release( ICoreAcceleratorKeys *iface )
{
    struct core_dispatcher *impl = impl_from_ICoreAcceleratorKeys( iface );
    return core_dispatcher_Release( &impl->ICoreDispatcher_iface );
}

static HRESULT WINAPI core_dispatcher_accel_GetIids( ICoreAcceleratorKeys *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI core_dispatcher_accel_GetRuntimeClassName( ICoreAcceleratorKeys *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.UI.Core.CoreDispatcher", 30, class_name );
}

static HRESULT WINAPI core_dispatcher_accel_GetTrustLevel( ICoreAcceleratorKeys *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_POINTER;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI core_dispatcher_accel_add_AcceleratorKeyActivated( ICoreAcceleratorKeys *iface,
    ITypedEventHandler_CoreDispatcher_AcceleratorKeyEventArgs *handler,
    EventRegistrationToken *cookie )
{
    struct core_dispatcher *impl = impl_from_ICoreAcceleratorKeys( iface );
    TRACE( "iface %p, handler %p, cookie %p\n", iface, handler, cookie );
    if (!cookie) return E_POINTER;
    if (impl->accel_key_handler)
        ITypedEventHandler_CoreDispatcher_AcceleratorKeyEventArgs_Release( impl->accel_key_handler );
    impl->accel_key_handler = handler;
    if (handler)
        ITypedEventHandler_CoreDispatcher_AcceleratorKeyEventArgs_AddRef( handler );
    cookie->value = (UINT_PTR)handler;
    return S_OK;
}

static HRESULT WINAPI core_dispatcher_accel_remove_AcceleratorKeyActivated( ICoreAcceleratorKeys *iface,
    EventRegistrationToken cookie )
{
    struct core_dispatcher *impl = impl_from_ICoreAcceleratorKeys( iface );
    TRACE( "iface %p, cookie %I64u\n", iface, cookie.value );
    if (impl->accel_key_handler)
    {
        ITypedEventHandler_CoreDispatcher_AcceleratorKeyEventArgs_Release( impl->accel_key_handler );
        impl->accel_key_handler = NULL;
    }
    return S_OK;
}

static const struct ICoreAcceleratorKeysVtbl core_dispatcher_accel_vtbl =
{
    core_dispatcher_accel_QueryInterface,
    core_dispatcher_accel_AddRef,
    core_dispatcher_accel_Release,
    core_dispatcher_accel_GetIids,
    core_dispatcher_accel_GetRuntimeClassName,
    core_dispatcher_accel_GetTrustLevel,
    core_dispatcher_accel_add_AcceleratorKeyActivated,
    core_dispatcher_accel_remove_AcceleratorKeyActivated,
};

static HRESULT core_dispatcher_create( ICoreDispatcher **out )
{
    struct core_dispatcher *impl;
    if (!out) return E_POINTER;
    *out = NULL;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->ICoreDispatcher_iface.lpVtbl = &core_dispatcher_vtbl;
    impl->ICoreDispatcher2_iface.lpVtbl = core_dispatcher2_vtbl;
    impl->ICoreAcceleratorKeys_iface.lpVtbl = &core_dispatcher_accel_vtbl;
    impl->ref = 1;
    *out = &impl->ICoreDispatcher_iface;
    return S_OK;
}

static HRESULT WINAPI core_window_get_Dispatcher( ICoreWindow *iface, ICoreDispatcher **value )
{
    struct core_window *impl = impl_from_ICoreWindow( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    if (!impl->dispatcher)
    {
        HRESULT hr = core_dispatcher_create( &impl->dispatcher );
        if (FAILED(hr)) return hr;
    }
    *value = impl->dispatcher;
    ICoreDispatcher_AddRef( *value );
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
    struct core_window *impl = impl_from_ICoreWindow( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value)
    {
        impl->cursor_hidden = TRUE;
        impl->last_mouse_x = -32768;
        impl->last_mouse_y = -32768;
        if (impl->hwnd)
            PostMessageW( impl->hwnd, WM_SETCURSOR, (WPARAM)impl->hwnd, MAKELPARAM(HTCLIENT, WM_MOUSEMOVE) );
    }
    else
    {
        impl->cursor_hidden = FALSE;
        if (impl->hwnd)
            PostMessageW( impl->hwnd, WM_SETCURSOR, (WPARAM)impl->hwnd, MAKELPARAM(HTCLIENT, WM_MOUSEMOVE) );
    }
    return S_OK;
}

static HRESULT WINAPI core_window_get_PointerPosition( ICoreWindow *iface, Point *value )
{
    POINT pt;
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    if (GetCursorPos( &pt ))
    {
        value->X = (FLOAT)pt.x;
        value->Y = (FLOAT)pt.y;
    }
    else
    {
        value->X = 0.0f;
        value->Y = 0.0f;
    }
    return S_OK;
}

static HRESULT WINAPI core_window_get_Visible( ICoreWindow *iface, boolean *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    *value = TRUE;
    return S_OK;
}

#include "events.c"

static void core_dispatcher_dispatch_accel( ICoreDispatcher *iface, CoreAcceleratorKeyEventType event_type, VirtualKey key, LPARAM lparam )
{
    struct core_dispatcher *impl;
    if (!iface) return;
    impl = impl_from_ICoreDispatcher( iface );
    if (impl && impl->accel_key_handler)
    {
        IAcceleratorKeyEventArgs *args = NULL;
        if (SUCCEEDED(accel_key_event_args_create( event_type, key, lparam, &args )) && args)
        {
            ITypedEventHandler_CoreDispatcher_AcceleratorKeyEventArgs_Invoke(
                impl->accel_key_handler, &impl->ICoreDispatcher_iface, args );
            IAcceleratorKeyEventArgs_Release( args );
        }
    }
}

struct window_activated_args_impl
{
    IWindowActivatedEventArgs IWindowActivatedEventArgs_iface;
    ICoreWindowEventArgs ICoreWindowEventArgs_iface;
    LONG ref;
    boolean handled;
};

#define impl_from_IWindowActivatedEventArgs(ptr) ((struct window_activated_args_impl *)((char *)(ptr) - offsetof(struct window_activated_args_impl, IWindowActivatedEventArgs_iface)))
#define impl_from_ICoreWindowEventArgs(ptr) ((struct window_activated_args_impl *)((char *)(ptr) - offsetof(struct window_activated_args_impl, ICoreWindowEventArgs_iface)))

static HRESULT WINAPI wargs_QueryInterface( IWindowActivatedEventArgs *iface, REFIID riid, void **out )
{
    struct window_activated_args_impl *impl = impl_from_IWindowActivatedEventArgs( iface );
    if (!out) return E_POINTER;
    *out = NULL;

    if (IsEqualGUID( riid, &IID_IUnknown ) ||
        IsEqualGUID( riid, &IID_IInspectable ) ||
        IsEqualGUID( riid, &IID_IAgileObject ) ||
        IsEqualGUID( riid, &IID_IWindowActivatedEventArgs ))
    {
        *out = &impl->IWindowActivatedEventArgs_iface;
        IInspectable_AddRef( (IInspectable *)*out );
        return S_OK;
    }

    if (IsEqualGUID( riid, &IID_ICoreWindowEventArgs ))
    {
        *out = &impl->ICoreWindowEventArgs_iface;
        IInspectable_AddRef( (IInspectable *)&impl->IWindowActivatedEventArgs_iface );
        return S_OK;
    }

    TRACE( "wargs %p unhandled QI %s\n", impl, debugstr_guid( riid ) );
    return E_NOINTERFACE;
}

static ULONG WINAPI wargs_AddRef( IWindowActivatedEventArgs *iface )
{
    struct window_activated_args_impl *impl = impl_from_IWindowActivatedEventArgs( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI wargs_Release( IWindowActivatedEventArgs *iface )
{
    struct window_activated_args_impl *impl = impl_from_IWindowActivatedEventArgs( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI wargs_GetIids( IWindowActivatedEventArgs *iface, ULONG *count, IID **iids )
{
    if (count) *count = 0;
    return S_OK;
}

static HRESULT WINAPI wargs_GetRuntimeClassName( IWindowActivatedEventArgs *iface, HSTRING *name )
{
    return WindowsCreateString( L"Windows.UI.Core.WindowActivatedEventArgs", 39, name );
}

static HRESULT WINAPI wargs_GetTrustLevel( IWindowActivatedEventArgs *iface, TrustLevel *trust_level )
{
    if (trust_level) *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI wargs_get_WindowActivationState( IWindowActivatedEventArgs *iface, CoreWindowActivationState *value )
{
    if (!value) return E_POINTER;
    *value = CoreWindowActivationState_CodeActivated;
    return S_OK;
}

static const struct IWindowActivatedEventArgsVtbl window_activated_args_vtbl =
{
    wargs_QueryInterface,
    wargs_AddRef,
    wargs_Release,
    wargs_GetIids,
    wargs_GetRuntimeClassName,
    wargs_GetTrustLevel,
    wargs_get_WindowActivationState,
};

static HRESULT WINAPI wargs_core_QueryInterface( ICoreWindowEventArgs *iface, REFIID riid, void **out )
{
    struct window_activated_args_impl *impl = impl_from_ICoreWindowEventArgs( iface );
    return wargs_QueryInterface( &impl->IWindowActivatedEventArgs_iface, riid, out );
}

static ULONG WINAPI wargs_core_AddRef( ICoreWindowEventArgs *iface )
{
    struct window_activated_args_impl *impl = impl_from_ICoreWindowEventArgs( iface );
    return wargs_AddRef( &impl->IWindowActivatedEventArgs_iface );
}

static ULONG WINAPI wargs_core_Release( ICoreWindowEventArgs *iface )
{
    struct window_activated_args_impl *impl = impl_from_ICoreWindowEventArgs( iface );
    return wargs_Release( &impl->IWindowActivatedEventArgs_iface );
}

static HRESULT WINAPI wargs_core_get_Handled( ICoreWindowEventArgs *iface, boolean *value )
{
    struct window_activated_args_impl *impl = impl_from_ICoreWindowEventArgs( iface );
    if (!value) return E_POINTER;
    *value = impl->handled;
    return S_OK;
}

static HRESULT WINAPI wargs_core_put_Handled( ICoreWindowEventArgs *iface, boolean value )
{
    struct window_activated_args_impl *impl = impl_from_ICoreWindowEventArgs( iface );
    impl->handled = value;
    return S_OK;
}

static const struct ICoreWindowEventArgsVtbl core_window_event_args_vtbl =
{
    wargs_core_QueryInterface,
    wargs_core_AddRef,
    wargs_core_Release,
    (void *)wargs_GetIids,
    (void *)wargs_GetRuntimeClassName,
    (void *)wargs_GetTrustLevel,
    wargs_core_get_Handled,
    wargs_core_put_Handled,
};

static HRESULT window_activated_args_create( IWindowActivatedEventArgs **out )
{
    struct window_activated_args_impl *impl = calloc( 1, sizeof(*impl) );
    if (!impl) return E_OUTOFMEMORY;
    impl->IWindowActivatedEventArgs_iface.lpVtbl = &window_activated_args_vtbl;
    impl->ICoreWindowEventArgs_iface.lpVtbl = &core_window_event_args_vtbl;
    impl->ref = 1;
    *out = &impl->IWindowActivatedEventArgs_iface;
    return S_OK;
}

static HRESULT WINAPI core_window_Activate( ICoreWindow *iface )
{
    struct core_window *impl = impl_from_ICoreWindow( iface );
    TRACE( "iface %p\n", iface );
    if (impl->hwnd)
    {
        ShowWindow( impl->hwnd, SW_SHOW );
        SetForegroundWindow( impl->hwnd );
        SetActiveWindow( impl->hwnd );
        SetFocus( impl->hwnd );
    }
    if (impl->activated_handler)
    {
        IWindowActivatedEventArgs *args = NULL;
        HRESULT hr = window_activated_args_create( &args );
        if (SUCCEEDED(hr) && args)
        {
            TRACE( "Firing CoreWindow Activated handler %p\n", impl->activated_handler );
            hr = ITypedEventHandler_CoreWindow_WindowActivatedEventArgs_Invoke( impl->activated_handler, iface, args );
            TRACE( "CoreWindow Activated handler returned %#lx\n", hr );
            IWindowActivatedEventArgs_Release( args );
        }
    }
    return S_OK;
}

static HRESULT WINAPI core_window_Close( ICoreWindow *iface )
{
    TRACE( "iface %p\n", iface );
    return S_OK;
}

static HRESULT WINAPI core_window_GetAsyncKeyState( ICoreWindow *iface, VirtualKey key, CoreVirtualKeyStates *state )
{
    SHORT s;
    TRACE( "iface %p, key %d, state %p\n", iface, key, state );
    if (!state) return E_POINTER;
    s = GetAsyncKeyState( (int)key );
    *state = CoreVirtualKeyStates_None;
    if (s & 0x8000) *state |= CoreVirtualKeyStates_Down;
    if (s & 1) *state |= CoreVirtualKeyStates_Locked;
    return S_OK;
}

static HRESULT WINAPI core_window_GetKeyState( ICoreWindow *iface, VirtualKey key, CoreVirtualKeyStates *state )
{
    SHORT s;
    TRACE( "iface %p, key %d, state %p\n", iface, key, state );
    if (!state) return E_POINTER;
    s = GetKeyState( (int)key );
    *state = CoreVirtualKeyStates_None;
    if (s & 0x8000) *state |= CoreVirtualKeyStates_Down;
    if (s & 1) *state |= CoreVirtualKeyStates_Locked;
    return S_OK;
}

static HRESULT WINAPI core_window_ReleasePointerCapture( ICoreWindow *iface )
{
    struct core_window *impl = impl_from_ICoreWindow( iface );
    TRACE( "iface %p\n", iface );
    impl->pointer_captured = FALSE;
    ReleaseCapture();
    return S_OK;
}

static HRESULT WINAPI core_window_SetPointerCapture( ICoreWindow *iface )
{
    struct core_window *impl = impl_from_ICoreWindow( iface );
    TRACE( "iface %p\n", iface );
    impl->pointer_captured = TRUE;
    if (impl->hwnd)
        SetCapture( impl->hwnd );
    return S_OK;
}

static HRESULT WINAPI core_window_add_Activated( ICoreWindow *iface, ITypedEventHandler_CoreWindow_WindowActivatedEventArgs *handler, EventRegistrationToken *cookie )
{
    struct core_window *impl = impl_from_ICoreWindow( iface );
    TRACE( "iface %p, handler %p, cookie %p\n", iface, handler, cookie );
    if (impl->activated_handler)
        ITypedEventHandler_CoreWindow_WindowActivatedEventArgs_Release( impl->activated_handler );
    impl->activated_handler = handler;
    if (handler)
        ITypedEventHandler_CoreWindow_WindowActivatedEventArgs_AddRef( handler );
    if (!cookie) return E_POINTER;
    *cookie = dummy_cookie;
    return S_OK;
}

static HRESULT WINAPI core_window_remove_Activated( ICoreWindow *iface, EventRegistrationToken cookie )
{
    struct core_window *impl = impl_from_ICoreWindow( iface );
    TRACE( "iface %p, cookie %#I64x\n", iface, cookie.value );
    if (impl->activated_handler)
    {
        ITypedEventHandler_CoreWindow_WindowActivatedEventArgs_Release( impl->activated_handler );
        impl->activated_handler = NULL;
    }
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
    struct core_window *impl = impl_from_ICoreWindow( iface );
    TRACE( "iface %p, handler %p, cookie %p\n", iface, handler, cookie );
    if (impl->char_handler)
        ITypedEventHandler_CoreWindow_CharacterReceivedEventArgs_Release( impl->char_handler );
    impl->char_handler = handler;
    if (handler)
        ITypedEventHandler_CoreWindow_CharacterReceivedEventArgs_AddRef( handler );
    if (cookie) cookie->value = (UINT_PTR)handler;
    return S_OK;
}

static HRESULT WINAPI core_window_remove_CharacterReceived( ICoreWindow *iface, EventRegistrationToken cookie )
{
    struct core_window *impl = impl_from_ICoreWindow( iface );
    TRACE( "iface %p, cookie %#I64x\n", iface, cookie.value );
    if (impl->char_handler)
    {
        ITypedEventHandler_CoreWindow_CharacterReceivedEventArgs_Release( impl->char_handler );
        impl->char_handler = NULL;
    }
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
    struct core_window *impl = impl_from_ICoreWindow( iface );
    TRACE( "iface %p, handler %p, cookie %p\n", iface, handler, cookie );
    if (impl->key_down_handler)
        ITypedEventHandler_CoreWindow_KeyEventArgs_Release( impl->key_down_handler );
    impl->key_down_handler = handler;
    if (handler)
        ITypedEventHandler_CoreWindow_KeyEventArgs_AddRef( handler );
    if (cookie) cookie->value = (UINT_PTR)handler;
    return S_OK;
}

static HRESULT WINAPI core_window_remove_KeyDown( ICoreWindow *iface, EventRegistrationToken cookie )
{
    struct core_window *impl = impl_from_ICoreWindow( iface );
    TRACE( "iface %p, cookie %#I64x\n", iface, cookie.value );
    if (impl->key_down_handler)
    {
        ITypedEventHandler_CoreWindow_KeyEventArgs_Release( impl->key_down_handler );
        impl->key_down_handler = NULL;
    }
    return S_OK;
}

static HRESULT WINAPI core_window_add_KeyUp( ICoreWindow *iface, ITypedEventHandler_CoreWindow_KeyEventArgs *handler, EventRegistrationToken *cookie )
{
    struct core_window *impl = impl_from_ICoreWindow( iface );
    TRACE( "iface %p, handler %p, cookie %p\n", iface, handler, cookie );
    if (impl->key_up_handler)
        ITypedEventHandler_CoreWindow_KeyEventArgs_Release( impl->key_up_handler );
    impl->key_up_handler = handler;
    if (handler)
        ITypedEventHandler_CoreWindow_KeyEventArgs_AddRef( handler );
    if (cookie) cookie->value = (UINT_PTR)handler;
    return S_OK;
}

static HRESULT WINAPI core_window_remove_KeyUp( ICoreWindow *iface, EventRegistrationToken cookie )
{
    struct core_window *impl = impl_from_ICoreWindow( iface );
    TRACE( "iface %p, cookie %#I64x\n", iface, cookie.value );
    if (impl->key_up_handler)
    {
        ITypedEventHandler_CoreWindow_KeyEventArgs_Release( impl->key_up_handler );
        impl->key_up_handler = NULL;
    }
    return S_OK;
}

static HRESULT WINAPI core_window_add_PointerCaptureLost( ICoreWindow *iface, ITypedEventHandler_CoreWindow_PointerEventArgs *handler, EventRegistrationToken *cookie )
{
    struct core_window *impl = impl_from_ICoreWindow( iface );
    TRACE( "iface %p, handler %p, cookie %p\n", iface, handler, cookie );
    if (impl->pointer_capture_lost_handler)
        ITypedEventHandler_CoreWindow_PointerEventArgs_Release( impl->pointer_capture_lost_handler );
    impl->pointer_capture_lost_handler = handler;
    if (handler)
        ITypedEventHandler_CoreWindow_PointerEventArgs_AddRef( handler );
    if (cookie) cookie->value = (UINT_PTR)handler;
    return S_OK;
}

static HRESULT WINAPI core_window_remove_PointerCaptureLost( ICoreWindow *iface, EventRegistrationToken cookie )
{
    struct core_window *impl = impl_from_ICoreWindow( iface );
    TRACE( "iface %p, cookie %#I64x\n", iface, cookie.value );
    if (impl->pointer_capture_lost_handler)
    {
        ITypedEventHandler_CoreWindow_PointerEventArgs_Release( impl->pointer_capture_lost_handler );
        impl->pointer_capture_lost_handler = NULL;
    }
    return S_OK;
}

static HRESULT WINAPI core_window_add_PointerEntered( ICoreWindow *iface, ITypedEventHandler_CoreWindow_PointerEventArgs *handler, EventRegistrationToken *cookie )
{
    struct core_window *impl = impl_from_ICoreWindow( iface );
    TRACE( "iface %p, handler %p, cookie %p\n", iface, handler, cookie );
    if (impl->pointer_entered_handler)
        ITypedEventHandler_CoreWindow_PointerEventArgs_Release( impl->pointer_entered_handler );
    impl->pointer_entered_handler = handler;
    if (handler)
        ITypedEventHandler_CoreWindow_PointerEventArgs_AddRef( handler );
    if (cookie) cookie->value = (UINT_PTR)handler;
    return S_OK;
}

static HRESULT WINAPI core_window_remove_PointerEntered( ICoreWindow *iface, EventRegistrationToken cookie )
{
    struct core_window *impl = impl_from_ICoreWindow( iface );
    TRACE( "iface %p, cookie %#I64x\n", iface, cookie.value );
    if (impl->pointer_entered_handler)
    {
        ITypedEventHandler_CoreWindow_PointerEventArgs_Release( impl->pointer_entered_handler );
        impl->pointer_entered_handler = NULL;
    }
    return S_OK;
}

static HRESULT WINAPI core_window_add_PointerExited( ICoreWindow *iface, ITypedEventHandler_CoreWindow_PointerEventArgs *handler, EventRegistrationToken *cookie )
{
    struct core_window *impl = impl_from_ICoreWindow( iface );
    TRACE( "iface %p, handler %p, cookie %p\n", iface, handler, cookie );
    if (impl->pointer_exited_handler)
        ITypedEventHandler_CoreWindow_PointerEventArgs_Release( impl->pointer_exited_handler );
    impl->pointer_exited_handler = handler;
    if (handler)
        ITypedEventHandler_CoreWindow_PointerEventArgs_AddRef( handler );
    if (cookie) cookie->value = (UINT_PTR)handler;
    return S_OK;
}

static HRESULT WINAPI core_window_remove_PointerExited( ICoreWindow *iface, EventRegistrationToken cookie )
{
    struct core_window *impl = impl_from_ICoreWindow( iface );
    TRACE( "iface %p, cookie %#I64x\n", iface, cookie.value );
    if (impl->pointer_exited_handler)
    {
        ITypedEventHandler_CoreWindow_PointerEventArgs_Release( impl->pointer_exited_handler );
        impl->pointer_exited_handler = NULL;
    }
    return S_OK;
}

static HRESULT WINAPI core_window_add_PointerMoved( ICoreWindow *iface, ITypedEventHandler_CoreWindow_PointerEventArgs *handler, EventRegistrationToken *cookie )
{
    struct core_window *impl = impl_from_ICoreWindow( iface );
    TRACE( "iface %p, handler %p, cookie %p\n", iface, handler, cookie );
    if (impl->pointer_moved_handler)
        ITypedEventHandler_CoreWindow_PointerEventArgs_Release( impl->pointer_moved_handler );
    impl->pointer_moved_handler = handler;
    if (handler)
        ITypedEventHandler_CoreWindow_PointerEventArgs_AddRef( handler );
    if (cookie) cookie->value = (UINT_PTR)handler;
    return S_OK;
}

static HRESULT WINAPI core_window_remove_PointerMoved( ICoreWindow *iface, EventRegistrationToken cookie )
{
    struct core_window *impl = impl_from_ICoreWindow( iface );
    TRACE( "iface %p, cookie %#I64x\n", iface, cookie.value );
    if (impl->pointer_moved_handler)
    {
        ITypedEventHandler_CoreWindow_PointerEventArgs_Release( impl->pointer_moved_handler );
        impl->pointer_moved_handler = NULL;
    }
    return S_OK;
}

static HRESULT WINAPI core_window_add_PointerPressed( ICoreWindow *iface, ITypedEventHandler_CoreWindow_PointerEventArgs *handler, EventRegistrationToken *cookie )
{
    struct core_window *impl = impl_from_ICoreWindow( iface );
    TRACE( "iface %p, handler %p, cookie %p\n", iface, handler, cookie );
    if (impl->pointer_pressed_handler)
        ITypedEventHandler_CoreWindow_PointerEventArgs_Release( impl->pointer_pressed_handler );
    impl->pointer_pressed_handler = handler;
    if (handler)
        ITypedEventHandler_CoreWindow_PointerEventArgs_AddRef( handler );
    if (cookie) cookie->value = (UINT_PTR)handler;
    return S_OK;
}

static HRESULT WINAPI core_window_remove_PointerPressed( ICoreWindow *iface, EventRegistrationToken cookie )
{
    struct core_window *impl = impl_from_ICoreWindow( iface );
    TRACE( "iface %p, cookie %#I64x\n", iface, cookie.value );
    if (impl->pointer_pressed_handler)
    {
        ITypedEventHandler_CoreWindow_PointerEventArgs_Release( impl->pointer_pressed_handler );
        impl->pointer_pressed_handler = NULL;
    }
    return S_OK;
}

static HRESULT WINAPI core_window_add_PointerReleased( ICoreWindow *iface, ITypedEventHandler_CoreWindow_PointerEventArgs *handler, EventRegistrationToken *cookie )
{
    struct core_window *impl = impl_from_ICoreWindow( iface );
    TRACE( "iface %p, handler %p, cookie %p\n", iface, handler, cookie );
    if (impl->pointer_released_handler)
        ITypedEventHandler_CoreWindow_PointerEventArgs_Release( impl->pointer_released_handler );
    impl->pointer_released_handler = handler;
    if (handler)
        ITypedEventHandler_CoreWindow_PointerEventArgs_AddRef( handler );
    if (cookie) cookie->value = (UINT_PTR)handler;
    return S_OK;
}

static HRESULT WINAPI core_window_remove_PointerReleased( ICoreWindow *iface, EventRegistrationToken cookie )
{
    struct core_window *impl = impl_from_ICoreWindow( iface );
    TRACE( "iface %p, cookie %#I64x\n", iface, cookie.value );
    if (impl->pointer_released_handler)
    {
        ITypedEventHandler_CoreWindow_PointerEventArgs_Release( impl->pointer_released_handler );
        impl->pointer_released_handler = NULL;
    }
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
    struct core_window *impl = impl_from_ICoreWindow( iface );
    TRACE( "iface %p, handler %p, cookie %p\n", iface, handler, cookie );
    if (impl->pointer_wheel_handler)
        ITypedEventHandler_CoreWindow_PointerEventArgs_Release( impl->pointer_wheel_handler );
    impl->pointer_wheel_handler = handler;
    if (handler)
        ITypedEventHandler_CoreWindow_PointerEventArgs_AddRef( handler );
    if (cookie) cookie->value = (UINT_PTR)handler;
    return S_OK;
}

static HRESULT WINAPI core_window_remove_PointerWheelChanged( ICoreWindow *iface, EventRegistrationToken cookie )
{
    struct core_window *impl = impl_from_ICoreWindow( iface );
    TRACE( "iface %p, cookie %#I64x\n", iface, cookie.value );
    if (impl->pointer_wheel_handler)
    {
        ITypedEventHandler_CoreWindow_PointerEventArgs_Release( impl->pointer_wheel_handler );
        impl->pointer_wheel_handler = NULL;
    }
    return S_OK;
}

static HRESULT WINAPI core_window_add_SizeChanged( ICoreWindow *iface, ITypedEventHandler_CoreWindow_WindowSizeChangedEventArgs *handler, EventRegistrationToken *cookie )
{
    struct core_window *impl = impl_from_ICoreWindow( iface );
    TRACE( "iface %p, handler %p, cookie %p\n", iface, handler, cookie );
    if (impl->size_changed_handler)
        ITypedEventHandler_CoreWindow_WindowSizeChangedEventArgs_Release( impl->size_changed_handler );
    impl->size_changed_handler = handler;
    if (handler)
        ITypedEventHandler_CoreWindow_WindowSizeChangedEventArgs_AddRef( handler );
    if (cookie) cookie->value = (UINT_PTR)handler;
    return S_OK;
}

static HRESULT WINAPI core_window_remove_SizeChanged( ICoreWindow *iface, EventRegistrationToken cookie )
{
    struct core_window *impl = impl_from_ICoreWindow( iface );
    TRACE( "iface %p, cookie %#I64x\n", iface, cookie.value );
    if (impl->size_changed_handler)
    {
        ITypedEventHandler_CoreWindow_WindowSizeChangedEventArgs_Release( impl->size_changed_handler );
        impl->size_changed_handler = NULL;
    }
    return S_OK;
}

static HRESULT WINAPI core_window_add_VisibilityChanged( ICoreWindow *iface, ITypedEventHandler_CoreWindow_VisibilityChangedEventArgs *handler, EventRegistrationToken *cookie )
{
    struct core_window *impl = impl_from_ICoreWindow( iface );
    TRACE( "iface %p, handler %p, cookie %p\n", iface, handler, cookie );
    if (impl->visibility_handler)
        ITypedEventHandler_CoreWindow_VisibilityChangedEventArgs_Release( impl->visibility_handler );
    impl->visibility_handler = handler;
    if (handler)
        ITypedEventHandler_CoreWindow_VisibilityChangedEventArgs_AddRef( handler );
    if (cookie) cookie->value = (UINT_PTR)handler;
    return S_OK;
}

static HRESULT WINAPI core_window_remove_VisibilityChanged( ICoreWindow *iface, EventRegistrationToken cookie )
{
    struct core_window *impl = impl_from_ICoreWindow( iface );
    TRACE( "iface %p, cookie %#I64x\n", iface, cookie.value );
    if (impl->visibility_handler)
    {
        ITypedEventHandler_CoreWindow_VisibilityChangedEventArgs_Release( impl->visibility_handler );
        impl->visibility_handler = NULL;
    }
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

/* ICoreWindow2 methods */
static HRESULT WINAPI core_window2_QueryInterface( ICoreWindow2 *iface, REFIID iid, void **out )
{
    struct core_window *impl = impl_from_ICoreWindow2( iface );
    return core_window_QueryInterface( &impl->ICoreWindow_iface, iid, out );
}

static ULONG WINAPI core_window2_AddRef( ICoreWindow2 *iface )
{
    struct core_window *impl = impl_from_ICoreWindow2( iface );
    return core_window_AddRef( &impl->ICoreWindow_iface );
}

static ULONG WINAPI core_window2_Release( ICoreWindow2 *iface )
{
    struct core_window *impl = impl_from_ICoreWindow2( iface );
    return core_window_Release( &impl->ICoreWindow_iface );
}

static HRESULT WINAPI core_window2_GetIids( ICoreWindow2 *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI core_window2_GetRuntimeClassName( ICoreWindow2 *iface, HSTRING *class_name )
{
    return WindowsCreateString( RuntimeClass_Windows_UI_Core_CoreWindow,
                                ARRAY_SIZE(RuntimeClass_Windows_UI_Core_CoreWindow) - 1,
                                class_name );
}

static HRESULT WINAPI core_window2_GetTrustLevel( ICoreWindow2 *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_POINTER;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI core_window2_put_PointerPosition( ICoreWindow2 *iface, Point value )
{
    struct core_window *impl = impl_from_ICoreWindow2( iface );
    TRACE( "iface %p, value (%f, %f)\n", iface, value.X, value.Y );
    SetCursorPos( (int)value.X, (int)value.Y );
    if (impl && impl->hwnd)
    {
        POINT pt = { (int)value.X, (int)value.Y };
        ScreenToClient( impl->hwnd, &pt );
        impl->last_mouse_x = pt.x;
        impl->last_mouse_y = pt.y;
    }
    return S_OK;
}

static const struct ICoreWindow2Vtbl core_window2_vtbl =
{
    core_window2_QueryInterface,
    core_window2_AddRef,
    core_window2_Release,
    core_window2_GetIids,
    core_window2_GetRuntimeClassName,
    core_window2_GetTrustLevel,
    core_window2_put_PointerPosition,
};

/* ICoreWindow3 methods */
static HRESULT WINAPI core_window3_QueryInterface( ICoreWindow3 *iface, REFIID iid, void **out )
{
    struct core_window *impl = impl_from_ICoreWindow3( iface );
    return core_window_QueryInterface( &impl->ICoreWindow_iface, iid, out );
}

static ULONG WINAPI core_window3_AddRef( ICoreWindow3 *iface )
{
    struct core_window *impl = impl_from_ICoreWindow3( iface );
    return core_window_AddRef( &impl->ICoreWindow_iface );
}

static ULONG WINAPI core_window3_Release( ICoreWindow3 *iface )
{
    struct core_window *impl = impl_from_ICoreWindow3( iface );
    return core_window_Release( &impl->ICoreWindow_iface );
}

static HRESULT WINAPI core_window3_GetIids( ICoreWindow3 *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI core_window3_GetRuntimeClassName( ICoreWindow3 *iface, HSTRING *class_name )
{
    return WindowsCreateString( RuntimeClass_Windows_UI_Core_CoreWindow,
                                ARRAY_SIZE(RuntimeClass_Windows_UI_Core_CoreWindow) - 1,
                                class_name );
}

static HRESULT WINAPI core_window3_GetTrustLevel( ICoreWindow3 *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_POINTER;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI core_window3_add_ClosestInteractiveBoundsRequested( ICoreWindow3 *iface,
    __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_Windows__CUI__CCore__CClosestInteractiveBoundsRequestedEventArgs *handler,
    EventRegistrationToken *cookie )
{
    TRACE( "iface %p, handler %p, cookie %p\n", iface, handler, cookie );
    if (!cookie) return E_POINTER;
    *cookie = dummy_cookie;
    return S_OK;
}

static HRESULT WINAPI core_window3_remove_ClosestInteractiveBoundsRequested( ICoreWindow3 *iface, EventRegistrationToken cookie )
{
    TRACE( "iface %p, cookie %#I64x\n", iface, cookie.value );
    return S_OK;
}

static HRESULT WINAPI core_window3_GetCurrentKeyEventDeviceId( ICoreWindow3 *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    *value = NULL;
    return S_OK;
}

static const struct ICoreWindow3Vtbl core_window3_vtbl =
{
    core_window3_QueryInterface,
    core_window3_AddRef,
    core_window3_Release,
    core_window3_GetIids,
    core_window3_GetRuntimeClassName,
    core_window3_GetTrustLevel,
    core_window3_add_ClosestInteractiveBoundsRequested,
    core_window3_remove_ClosestInteractiveBoundsRequested,
    core_window3_GetCurrentKeyEventDeviceId,
};

/* ICoreWindow4 methods */
static HRESULT WINAPI core_window4_QueryInterface( ICoreWindow4 *iface, REFIID iid, void **out )
{
    struct core_window *impl = impl_from_ICoreWindow4( iface );
    return core_window_QueryInterface( &impl->ICoreWindow_iface, iid, out );
}

static ULONG WINAPI core_window4_AddRef( ICoreWindow4 *iface )
{
    struct core_window *impl = impl_from_ICoreWindow4( iface );
    return core_window_AddRef( &impl->ICoreWindow_iface );
}

static ULONG WINAPI core_window4_Release( ICoreWindow4 *iface )
{
    struct core_window *impl = impl_from_ICoreWindow4( iface );
    return core_window_Release( &impl->ICoreWindow_iface );
}

static HRESULT WINAPI core_window4_GetIids( ICoreWindow4 *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI core_window4_GetRuntimeClassName( ICoreWindow4 *iface, HSTRING *class_name )
{
    return WindowsCreateString( RuntimeClass_Windows_UI_Core_CoreWindow,
                                ARRAY_SIZE(RuntimeClass_Windows_UI_Core_CoreWindow) - 1,
                                class_name );
}

static HRESULT WINAPI core_window4_GetTrustLevel( ICoreWindow4 *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_POINTER;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI core_window4_add_ResizeStarted( ICoreWindow4 *iface,
    __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_IInspectable *handler,
    EventRegistrationToken *cookie )
{
    TRACE( "iface %p, handler %p, cookie %p\n", iface, handler, cookie );
    if (!cookie) return E_POINTER;
    *cookie = dummy_cookie;
    return S_OK;
}

static HRESULT WINAPI core_window4_remove_ResizeStarted( ICoreWindow4 *iface, EventRegistrationToken cookie )
{
    TRACE( "iface %p, cookie %#I64x\n", iface, cookie.value );
    return S_OK;
}

static HRESULT WINAPI core_window4_add_ResizeCompleted( ICoreWindow4 *iface,
    __FITypedEventHandler_2_Windows__CUI__CCore__CCoreWindow_IInspectable *handler,
    EventRegistrationToken *cookie )
{
    TRACE( "iface %p, handler %p, cookie %p\n", iface, handler, cookie );
    if (!cookie) return E_POINTER;
    *cookie = dummy_cookie;
    return S_OK;
}

static HRESULT WINAPI core_window4_remove_ResizeCompleted( ICoreWindow4 *iface, EventRegistrationToken cookie )
{
    TRACE( "iface %p, cookie %#I64x\n", iface, cookie.value );
    return S_OK;
}

static const struct ICoreWindow4Vtbl core_window4_vtbl =
{
    core_window4_QueryInterface,
    core_window4_AddRef,
    core_window4_Release,
    core_window4_GetIids,
    core_window4_GetRuntimeClassName,
    core_window4_GetTrustLevel,
    core_window4_add_ResizeStarted,
    core_window4_remove_ResizeStarted,
    core_window4_add_ResizeCompleted,
    core_window4_remove_ResizeCompleted,
};

/* ICoreWindow5 methods */
static HRESULT WINAPI core_window5_QueryInterface( ICoreWindow5 *iface, REFIID iid, void **out )
{
    struct core_window *impl = impl_from_ICoreWindow5( iface );
    return core_window_QueryInterface( &impl->ICoreWindow_iface, iid, out );
}

static ULONG WINAPI core_window5_AddRef( ICoreWindow5 *iface )
{
    struct core_window *impl = impl_from_ICoreWindow5( iface );
    return core_window_AddRef( &impl->ICoreWindow_iface );
}

static ULONG WINAPI core_window5_Release( ICoreWindow5 *iface )
{
    struct core_window *impl = impl_from_ICoreWindow5( iface );
    return core_window_Release( &impl->ICoreWindow_iface );
}

static HRESULT WINAPI core_window5_GetIids( ICoreWindow5 *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI core_window5_GetRuntimeClassName( ICoreWindow5 *iface, HSTRING *class_name )
{
    return WindowsCreateString( RuntimeClass_Windows_UI_Core_CoreWindow,
                                ARRAY_SIZE(RuntimeClass_Windows_UI_Core_CoreWindow) - 1,
                                class_name );
}

static HRESULT WINAPI core_window5_GetTrustLevel( ICoreWindow5 *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_POINTER;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI core_window5_get_DispatcherQueue( ICoreWindow5 *iface, __x_ABI_CWindows_CSystem_CIDispatcherQueue **value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    *value = NULL;
    return S_OK;
}

static HRESULT WINAPI core_window5_get_ActivationMode( ICoreWindow5 *iface, __x_ABI_CWindows_CUI_CCore_CCoreWindowActivationMode *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    *value = CoreWindowActivationMode_ActivatedInForeground;
    return S_OK;
}

static const struct ICoreWindow5Vtbl core_window5_vtbl =
{
    core_window5_QueryInterface,
    core_window5_AddRef,
    core_window5_Release,
    core_window5_GetIids,
    core_window5_GetRuntimeClassName,
    core_window5_GetTrustLevel,
    core_window5_get_DispatcherQueue,
    core_window5_get_ActivationMode,
};

static HRESULT WINAPI core_window_interop_QueryInterface( void *iface, REFIID riid, void **out )
{
    struct core_window *impl = impl_from_ICoreWindowInterop( iface );
    return core_window_QueryInterface( &impl->ICoreWindow_iface, riid, out );
}

static ULONG WINAPI core_window_interop_AddRef( void *iface )
{
    struct core_window *impl = impl_from_ICoreWindowInterop( iface );
    return core_window_AddRef( &impl->ICoreWindow_iface );
}

static ULONG WINAPI core_window_interop_Release( void *iface )
{
    struct core_window *impl = impl_from_ICoreWindowInterop( iface );
    return core_window_Release( &impl->ICoreWindow_iface );
}

static LRESULT CALLBACK core_window_proc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    struct core_window *impl = (struct core_window *)GetWindowLongPtrW( hwnd, GWLP_USERDATA );

    switch (msg)
    {
    case WM_NCCREATE:
    {
        CREATESTRUCTW *cs = (CREATESTRUCTW *)lparam;
        SetWindowLongPtrW( hwnd, GWLP_USERDATA, (LONG_PTR)cs->lpCreateParams );
        return DefWindowProcW( hwnd, msg, wparam, lparam );
    }

    case WM_CLOSE:
        PostQuitMessage( 0 );
        return 0;

    case WM_SHOWWINDOW:
        if (impl && impl->visibility_handler)
        {
            IVisibilityChangedEventArgs *args = NULL;
            if (SUCCEEDED(visibility_changed_event_args_create( (boolean)wparam, &args )) && args)
            {
                ITypedEventHandler_CoreWindow_VisibilityChangedEventArgs_Invoke(
                    impl->visibility_handler, &impl->ICoreWindow_iface, args );
                IVisibilityChangedEventArgs_Release( args );
            }
        }
        break;

    case WM_SETCURSOR:
        if (LOWORD(lparam) == HTCLIENT && impl && impl->cursor_hidden)
        {
            SetCursor( NULL );
            return TRUE;
        }
        break;

    case WM_ACTIVATE:
        if (LOWORD(wparam) != WA_INACTIVE && impl && impl->activated_handler)
        {
            IWindowActivatedEventArgs *args = NULL;
            if (SUCCEEDED(window_activated_args_create( &args )) && args)
            {
                ITypedEventHandler_CoreWindow_WindowActivatedEventArgs_Invoke(
                    impl->activated_handler, &impl->ICoreWindow_iface, args );
                IWindowActivatedEventArgs_Release( args );
            }
        }
        break;

    case WM_SETFOCUS:
        if (impl && impl->activated_handler)
        {
            IWindowActivatedEventArgs *args = NULL;
            if (SUCCEEDED(window_activated_args_create( &args )) && args)
            {
                ITypedEventHandler_CoreWindow_WindowActivatedEventArgs_Invoke(
                    impl->activated_handler, &impl->ICoreWindow_iface, args );
                IWindowActivatedEventArgs_Release( args );
            }
        }
        break;

    case WM_LBUTTONDOWN:
        if (impl && impl->pointer_pressed_handler)
        {
            IPointerEventArgs *args = NULL;
            if (SUCCEEDED(pointer_event_args_create( (short)LOWORD(lparam), (short)HIWORD(lparam),
                                                     TRUE, FALSE, FALSE,
                                                     PointerUpdateKind_LeftButtonPressed, 0, &args )) && args)
            {
                ITypedEventHandler_CoreWindow_PointerEventArgs_Invoke(
                    impl->pointer_pressed_handler, &impl->ICoreWindow_iface, args );
                IPointerEventArgs_Release( args );
            }
        }
        break;

    case WM_LBUTTONUP:
        if (impl && impl->pointer_released_handler)
        {
            IPointerEventArgs *args = NULL;
            if (SUCCEEDED(pointer_event_args_create( (short)LOWORD(lparam), (short)HIWORD(lparam),
                                                     FALSE, FALSE, FALSE,
                                                     PointerUpdateKind_LeftButtonReleased, 0, &args )) && args)
            {
                ITypedEventHandler_CoreWindow_PointerEventArgs_Invoke(
                    impl->pointer_released_handler, &impl->ICoreWindow_iface, args );
                IPointerEventArgs_Release( args );
            }
        }
        break;

    case WM_RBUTTONDOWN:
        if (impl && impl->pointer_pressed_handler)
        {
            IPointerEventArgs *args = NULL;
            if (SUCCEEDED(pointer_event_args_create( (short)LOWORD(lparam), (short)HIWORD(lparam),
                                                     FALSE, TRUE, FALSE,
                                                     PointerUpdateKind_RightButtonPressed, 0, &args )) && args)
            {
                ITypedEventHandler_CoreWindow_PointerEventArgs_Invoke(
                    impl->pointer_pressed_handler, &impl->ICoreWindow_iface, args );
                IPointerEventArgs_Release( args );
            }
        }
        break;

    case WM_RBUTTONUP:
        if (impl && impl->pointer_released_handler)
        {
            IPointerEventArgs *args = NULL;
            if (SUCCEEDED(pointer_event_args_create( (short)LOWORD(lparam), (short)HIWORD(lparam),
                                                     FALSE, FALSE, FALSE,
                                                     PointerUpdateKind_RightButtonReleased, 0, &args )) && args)
            {
                ITypedEventHandler_CoreWindow_PointerEventArgs_Invoke(
                    impl->pointer_released_handler, &impl->ICoreWindow_iface, args );
                IPointerEventArgs_Release( args );
            }
        }
        break;

    case WM_MOUSEMOVE:
    {
        short cur_x = (short)LOWORD(lparam);
        short cur_y = (short)HIWORD(lparam);
        int dx = 0, dy = 0;
        if (impl)
        {
            if (impl->last_mouse_x != -32768 && impl->last_mouse_y != -32768)
            {
                dx = cur_x - impl->last_mouse_x;
                dy = cur_y - impl->last_mouse_y;
            }
            impl->last_mouse_x = cur_x;
            impl->last_mouse_y = cur_y;
        }

        if (impl && impl->pointer_moved_handler)
        {
            IPointerEventArgs *args = NULL;
            boolean l = (wparam & MK_LBUTTON) != 0;
            boolean r = (wparam & MK_RBUTTON) != 0;
            boolean m = (wparam & MK_MBUTTON) != 0;
            if (SUCCEEDED(pointer_event_args_create( cur_x, cur_y,
                                                     l, r, m, PointerUpdateKind_Other, 0, &args )) && args)
            {
                ITypedEventHandler_CoreWindow_PointerEventArgs_Invoke(
                    impl->pointer_moved_handler, &impl->ICoreWindow_iface, args );
                IPointerEventArgs_Release( args );
            }
        }
        if (mouse_device_has_listener() && (dx != 0 || dy != 0))
        {
            mouse_device_dispatch_move( dx, dy );
        }
        break;
    }

    case WM_MOUSEWHEEL:
        if (impl && impl->pointer_wheel_handler)
        {
            POINT pt = {(short)LOWORD(lparam), (short)HIWORD(lparam)};
            INT delta = GET_WHEEL_DELTA_WPARAM( wparam );
            IPointerEventArgs *args = NULL;
            ScreenToClient( hwnd, &pt );
            if (SUCCEEDED(pointer_event_args_create( pt.x, pt.y,
                                                     (wparam & MK_LBUTTON) != 0,
                                                     (wparam & MK_RBUTTON) != 0,
                                                     (wparam & MK_MBUTTON) != 0,
                                                     PointerUpdateKind_Other, delta, &args )) && args)
            {
                ITypedEventHandler_CoreWindow_PointerEventArgs_Invoke(
                    impl->pointer_wheel_handler, &impl->ICoreWindow_iface, args );
                IPointerEventArgs_Release( args );
            }
        }
        break;

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        if (wparam == VK_F11 && impl)
        {
            if (!impl->is_fullscreen)
            {
                HMONITOR hmon = MonitorFromWindow( hwnd, MONITOR_DEFAULTTONEAREST );
                MONITORINFO mi = { sizeof(mi) };
                if (GetMonitorInfoW( hmon, &mi ))
                {
                    GetWindowRect( hwnd, &impl->saved_rect );
                    impl->saved_style = GetWindowLongW( hwnd, GWL_STYLE );
                    SetWindowLongW( hwnd, GWL_STYLE, (impl->saved_style & ~WS_OVERLAPPEDWINDOW) | WS_POPUP );
                    SetWindowPos( hwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top,
                                 mi.rcMonitor.right - mi.rcMonitor.left,
                                 mi.rcMonitor.bottom - mi.rcMonitor.top,
                                 SWP_FRAMECHANGED | SWP_SHOWWINDOW );
                    impl->is_fullscreen = TRUE;
                }
            }
            else
            {
                SetWindowLongW( hwnd, GWL_STYLE, impl->saved_style ? impl->saved_style : (WS_OVERLAPPEDWINDOW | WS_VISIBLE) );
                if (impl->saved_rect.right > impl->saved_rect.left)
                {
                    SetWindowPos( hwnd, NULL, impl->saved_rect.left, impl->saved_rect.top,
                                 impl->saved_rect.right - impl->saved_rect.left,
                                 impl->saved_rect.bottom - impl->saved_rect.top,
                                 SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW );
                }
                else
                {
                    ShowWindow( hwnd, SW_RESTORE );
                }
                impl->is_fullscreen = FALSE;
            }
        }
        if (impl && impl->key_down_handler)
        {
            IKeyEventArgs *args = NULL;
            if (SUCCEEDED(key_event_args_create( (VirtualKey)wparam, lparam, &args )) && args)
            {
                ITypedEventHandler_CoreWindow_KeyEventArgs_Invoke(
                    impl->key_down_handler, &impl->ICoreWindow_iface, args );
                IKeyEventArgs_Release( args );
            }
        }
        if (impl && impl->dispatcher)
        {
            CoreAcceleratorKeyEventType et = (msg == WM_SYSKEYDOWN) ?
                CoreAcceleratorKeyEventType_SystemKeyDown : CoreAcceleratorKeyEventType_KeyDown;
            core_dispatcher_dispatch_accel( impl->dispatcher, et, (VirtualKey)wparam, lparam );
        }
        break;

    case WM_KEYUP:
    case WM_SYSKEYUP:
        if (impl && impl->key_up_handler)
        {
            IKeyEventArgs *args = NULL;
            if (SUCCEEDED(key_event_args_create( (VirtualKey)wparam, lparam, &args )) && args)
            {
                ITypedEventHandler_CoreWindow_KeyEventArgs_Invoke(
                    impl->key_up_handler, &impl->ICoreWindow_iface, args );
                IKeyEventArgs_Release( args );
            }
        }
        if (impl && impl->dispatcher)
        {
            CoreAcceleratorKeyEventType et = (msg == WM_SYSKEYUP) ?
                CoreAcceleratorKeyEventType_SystemKeyUp : CoreAcceleratorKeyEventType_KeyUp;
            core_dispatcher_dispatch_accel( impl->dispatcher, et, (VirtualKey)wparam, lparam );
        }
        break;

    case WM_CHAR:
        if (impl && impl->char_handler)
        {
            ICharacterReceivedEventArgs *args = NULL;
            if (SUCCEEDED(character_received_event_args_create( (UINT32)wparam, lparam, &args )) && args)
            {
                ITypedEventHandler_CoreWindow_CharacterReceivedEventArgs_Invoke(
                    impl->char_handler, &impl->ICoreWindow_iface, args );
                ICharacterReceivedEventArgs_Release( args );
            }
        }
        break;

    case WM_SIZE:
        if (impl && impl->size_changed_handler)
        {
            IWindowSizeChangedEventArgs *args = NULL;
            if (SUCCEEDED(window_size_changed_event_args_create( LOWORD(lparam), HIWORD(lparam), &args )) && args)
            {
                ITypedEventHandler_CoreWindow_WindowSizeChangedEventArgs_Invoke(
                    impl->size_changed_handler, &impl->ICoreWindow_iface, args );
                IWindowSizeChangedEventArgs_Release( args );
            }
        }
        break;
    }
    return DefWindowProcW( hwnd, msg, wparam, lparam );
}

static HWND create_core_window_hwnd( struct core_window *impl )
{
    static const WCHAR class_name[] = L"Windows.UI.Core.CoreWindow";
    static BOOL registered = FALSE;
    WNDCLASSEXW wc = {sizeof(wc)};
    WCHAR title[MAX_PATH];
    HWND hwnd;

    if (!registered)
    {
        wc.lpfnWndProc = core_window_proc;
        wc.hInstance = GetModuleHandleW( NULL );
        wc.lpszClassName = class_name;
        wc.hCursor = LoadCursorW( NULL, (const WCHAR *)IDC_ARROW );
        wc.hbrBackground = NULL;
        RegisterClassExW( &wc );
        registered = TRUE;
    }

    title[0] = 0;
    if (GetModuleFileNameW( NULL, title, ARRAY_SIZE(title) ))
    {
        WCHAR *p = wcsrchr( title, '\\' );
        if (p) memmove( title, p + 1, (wcslen(p + 1) + 1) * sizeof(WCHAR) );
        p = wcsrchr( title, '.' );
        if (p) *p = 0;
    }

    hwnd = CreateWindowExW( 0, class_name, title[0] ? title : L"CoreWindow",
                            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                            CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720,
                            NULL, NULL, GetModuleHandleW( NULL ), (void *)impl );
    if (hwnd)
    {
        SetWindowLongPtrW( hwnd, GWLP_USERDATA, (LONG_PTR)impl );
        ShowWindow( hwnd, SW_SHOW );
        UpdateWindow( hwnd );
    }
    return hwnd;
}

static HRESULT WINAPI core_window_interop_get_WindowHandle( void *iface, HWND *hwnd )
{
    struct core_window *impl = impl_from_ICoreWindowInterop( iface );
    TRACE( "iface %p, hwnd %p\n", iface, hwnd );
    if (!hwnd) return E_POINTER;
    if (!impl->hwnd) impl->hwnd = create_core_window_hwnd( impl );
    *hwnd = impl->hwnd;
    return S_OK;
}

static HRESULT WINAPI core_window_interop_put_MessageHandled( void *iface, boolean value )
{
    return S_OK;
}

static const void *core_window_interop_vtbl[] =
{
    (void *)core_window_interop_QueryInterface,
    (void *)core_window_interop_AddRef,
    (void *)core_window_interop_Release,
    (void *)core_window_interop_get_WindowHandle,
    (void *)core_window_interop_put_MessageHandled,
};

static HRESULT core_window_create( ICoreWindow **window )
{
    struct core_window *impl;

    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->ICoreWindow_iface.lpVtbl = &core_window_vtbl;
    impl->ICoreWindow2_iface.lpVtbl = &core_window2_vtbl;
    impl->ICoreWindow3_iface.lpVtbl = &core_window3_vtbl;
    impl->ICoreWindow4_iface.lpVtbl = &core_window4_vtbl;
    impl->ICoreWindow5_iface.lpVtbl = &core_window5_vtbl;
    impl->ICoreWindowInterop_iface.lpVtbl = core_window_interop_vtbl;
    impl->hwnd = create_core_window_hwnd( impl );
    impl->last_mouse_x = -32768;
    impl->last_mouse_y = -32768;
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
    ICoreWindow *window;
    HRESULT hr;

    TRACE( "iface %p, instance %p\n", iface, instance );
    if (!instance) return E_POINTER;
    if (FAILED(hr = core_window_create( &window ))) return hr;
    *instance = (IInspectable *)window;
    return S_OK;
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

static ICoreWindow *current_thread_core_window = NULL;

static HRESULT WINAPI core_window_statics_GetForCurrentThread( ICoreWindowStatic *iface, ICoreWindow **windows )
{
    TRACE( "iface %p, windows %p\n", iface, windows );
    if (!windows) return E_POINTER;
    if (!current_thread_core_window)
    {
        HRESULT hr = core_window_create( &current_thread_core_window );
        if (FAILED(hr)) return hr;
    }
    *windows = current_thread_core_window;
    ICoreWindow_AddRef( *windows );
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

/* =========================================================================
 * CoreCursor Implementation & Factory
 * ========================================================================= */
struct core_cursor
{
    ICoreCursor ICoreCursor_iface;
    LONG ref;
    CoreCursorType type;
    UINT32 id;
};

static inline struct core_cursor *impl_from_ICoreCursor( ICoreCursor *iface )
{
    return CONTAINING_RECORD( iface, struct core_cursor, ICoreCursor_iface );
}

static HRESULT WINAPI core_cursor_QueryInterface( ICoreCursor *iface, REFIID riid, void **out )
{
    struct core_cursor *impl = impl_from_ICoreCursor( iface );
    if (!out) return E_POINTER;
    *out = NULL;
    if (IsEqualGUID( riid, &IID_IUnknown ) ||
        IsEqualGUID( riid, &IID_IInspectable ) ||
        IsEqualGUID( riid, &IID_IAgileObject ) ||
        IsEqualGUID( riid, &IID___x_ABI_CWindows_CUI_CCore_CICoreCursor ))
    {
        *out = &impl->ICoreCursor_iface;
        IInspectable_AddRef( (IInspectable *)*out );
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI core_cursor_AddRef( ICoreCursor *iface )
{
    struct core_cursor *impl = impl_from_ICoreCursor( iface );
    return InterlockedIncrement( &impl->ref );
}
static ULONG WINAPI core_cursor_Release( ICoreCursor *iface )
{
    struct core_cursor *impl = impl_from_ICoreCursor( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    if (!ref) free( impl );
    return ref;
}
static HRESULT WINAPI core_cursor_GetIids( ICoreCursor *iface, ULONG *c, IID **i ) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI core_cursor_GetRuntimeClassName( ICoreCursor *iface, HSTRING *n )
{
    return WindowsCreateString( L"Windows.UI.Core.CoreCursor", 26, n );
}
static HRESULT WINAPI core_cursor_GetTrustLevel( ICoreCursor *iface, TrustLevel *t ) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI core_cursor_get_Id( ICoreCursor *iface, UINT32 *value )
{
    struct core_cursor *impl = impl_from_ICoreCursor( iface );
    if (!value) return E_POINTER;
    *value = impl->id;
    return S_OK;
}
static HRESULT WINAPI core_cursor_get_Type( ICoreCursor *iface, CoreCursorType *value )
{
    struct core_cursor *impl = impl_from_ICoreCursor( iface );
    if (!value) return E_POINTER;
    *value = impl->type;
    return S_OK;
}

static const struct __x_ABI_CWindows_CUI_CCore_CICoreCursorVtbl core_cursor_vtbl =
{
    core_cursor_QueryInterface,
    core_cursor_AddRef,
    core_cursor_Release,
    core_cursor_GetIids,
    core_cursor_GetRuntimeClassName,
    core_cursor_GetTrustLevel,
    core_cursor_get_Type,
    core_cursor_get_Id
};

static HRESULT core_cursor_create( CoreCursorType type, UINT32 id, ICoreCursor **out )
{
    struct core_cursor *impl;
    if (!out) return E_POINTER;
    *out = NULL;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->ICoreCursor_iface.lpVtbl = &core_cursor_vtbl;
    impl->ref = 1;
    impl->type = type;
    impl->id = id;
    *out = &impl->ICoreCursor_iface;
    return S_OK;
}

struct core_cursor_factory
{
    IActivationFactory IActivationFactory_iface;
    ICoreCursorFactory ICoreCursorFactory_iface;
    LONG ref;
};

static inline struct core_cursor_factory *impl_from_core_cursor_factory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct core_cursor_factory, IActivationFactory_iface );
}

static inline struct core_cursor_factory *impl_from_ICoreCursorFactory( ICoreCursorFactory *iface )
{
    return CONTAINING_RECORD( iface, struct core_cursor_factory, ICoreCursorFactory_iface );
}

static HRESULT WINAPI ccf_QueryInterface( IActivationFactory *iface, REFIID riid, void **out )
{
    struct core_cursor_factory *impl = impl_from_core_cursor_factory( iface );
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
    if (IsEqualGUID( riid, &IID___x_ABI_CWindows_CUI_CCore_CICoreCursorFactory ))
    {
        *out = &impl->ICoreCursorFactory_iface;
        IActivationFactory_AddRef( iface );
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI ccf_AddRef( IActivationFactory *iface ) { return 2; }
static ULONG WINAPI ccf_Release( IActivationFactory *iface ) { return 1; }
static HRESULT WINAPI ccf_GetIids( IActivationFactory *iface, ULONG *c, IID **i ) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI ccf_GetRuntimeClassName( IActivationFactory *iface, HSTRING *n )
{
    return WindowsCreateString( L"Windows.UI.Core.CoreCursor", 26, n );
}
static HRESULT WINAPI ccf_GetTrustLevel( IActivationFactory *iface, TrustLevel *t ) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI ccf_ActivateInstance( IActivationFactory *iface, IInspectable **instance )
{
    return core_cursor_create( CoreCursorType_Arrow, 0, (ICoreCursor **)instance );
}

static const struct IActivationFactoryVtbl core_cursor_activation_factory_vtbl =
{
    ccf_QueryInterface,
    ccf_AddRef,
    ccf_Release,
    ccf_GetIids,
    ccf_GetRuntimeClassName,
    ccf_GetTrustLevel,
    ccf_ActivateInstance
};

static HRESULT WINAPI ccf_cursor_fac_QueryInterface( ICoreCursorFactory *iface, REFIID riid, void **out )
{
    struct core_cursor_factory *impl = impl_from_ICoreCursorFactory( iface );
    return ccf_QueryInterface( &impl->IActivationFactory_iface, riid, out );
}
static ULONG WINAPI ccf_cursor_fac_AddRef( ICoreCursorFactory *iface ) { return 2; }
static ULONG WINAPI ccf_cursor_fac_Release( ICoreCursorFactory *iface ) { return 1; }
static HRESULT WINAPI ccf_cursor_fac_GetIids( ICoreCursorFactory *iface, ULONG *c, IID **i ) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI ccf_cursor_fac_GetRuntimeClassName( ICoreCursorFactory *iface, HSTRING *n )
{
    return WindowsCreateString( L"Windows.UI.Core.CoreCursor", 26, n );
}
static HRESULT WINAPI ccf_cursor_fac_GetTrustLevel( ICoreCursorFactory *iface, TrustLevel *t ) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI ccf_cursor_fac_CreateCursor( ICoreCursorFactory *iface, CoreCursorType type, UINT32 id, ICoreCursor **cursor )
{
    return core_cursor_create( type, id, cursor );
}

static const struct __x_ABI_CWindows_CUI_CCore_CICoreCursorFactoryVtbl core_cursor_factory_vtbl =
{
    ccf_cursor_fac_QueryInterface,
    ccf_cursor_fac_AddRef,
    ccf_cursor_fac_Release,
    ccf_cursor_fac_GetIids,
    ccf_cursor_fac_GetRuntimeClassName,
    ccf_cursor_fac_GetTrustLevel,
    ccf_cursor_fac_CreateCursor
};

static struct core_cursor_factory core_cursor_factory_instance =
{
    {&core_cursor_activation_factory_vtbl},
    {&core_cursor_factory_vtbl},
    1
};
IActivationFactory *corecursor_factory = &core_cursor_factory_instance.IActivationFactory_iface;
