/* WinRT CoreWindow Event Arguments and Input Objects
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

/* PointerDevice */
struct pointer_device_impl
{
    IPointerDevice IPointerDevice_iface;
    LONG ref;
};

static inline struct pointer_device_impl *impl_from_IPointerDevice( IPointerDevice *iface )
{
    return CONTAINING_RECORD( iface, struct pointer_device_impl, IPointerDevice_iface );
}

static HRESULT WINAPI pdev_QueryInterface( IPointerDevice *iface, REFIID riid, void **out )
{
    struct pointer_device_impl *impl = impl_from_IPointerDevice( iface );
    if (!out) return E_POINTER;
    *out = NULL;
    if (IsEqualGUID( riid, &IID_IUnknown ) ||
        IsEqualGUID( riid, &IID_IInspectable ) ||
        IsEqualGUID( riid, &IID_IAgileObject ) ||
        IsEqualGUID( riid, &IID_IPointerDevice ))
    {
        *out = &impl->IPointerDevice_iface;
        IInspectable_AddRef( (IInspectable *)*out );
        return S_OK;
    }
    return E_NOINTERFACE;
}

static ULONG WINAPI pdev_AddRef( IPointerDevice *iface )
{
    struct pointer_device_impl *impl = impl_from_IPointerDevice( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI pdev_Release( IPointerDevice *iface )
{
    struct pointer_device_impl *impl = impl_from_IPointerDevice( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI pdev_GetIids( IPointerDevice *iface, ULONG *count, IID **iids )
{
    if (count) *count = 0;
    return S_OK;
}

static HRESULT WINAPI pdev_GetRuntimeClassName( IPointerDevice *iface, HSTRING *name )
{
    return WindowsCreateString( L"Windows.Devices.Input.PointerDevice", 35, name );
}

static HRESULT WINAPI pdev_GetTrustLevel( IPointerDevice *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_POINTER;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI pdev_get_PointerDeviceType( IPointerDevice *iface, PointerDeviceType *value )
{
    if (!value) return E_POINTER;
    *value = PointerDeviceType_Mouse;
    return S_OK;
}

static HRESULT WINAPI pdev_get_IsIntegrated( IPointerDevice *iface, boolean *value )
{
    if (!value) return E_POINTER;
    *value = FALSE;
    return S_OK;
}

static HRESULT WINAPI pdev_get_MaxContacts( IPointerDevice *iface, UINT32 *value )
{
    if (!value) return E_POINTER;
    *value = 1;
    return S_OK;
}

static HRESULT WINAPI pdev_get_PhysicalDeviceRect( IPointerDevice *iface, Rect *value )
{
    if (!value) return E_POINTER;
    value->X = 0; value->Y = 0; value->Width = 1920; value->Height = 1080;
    return S_OK;
}

static HRESULT WINAPI pdev_get_ScreenRect( IPointerDevice *iface, Rect *value )
{
    if (!value) return E_POINTER;
    value->X = 0; value->Y = 0; value->Width = 1920; value->Height = 1080;
    return S_OK;
}

static HRESULT WINAPI pdev_get_SupportedUsages( IPointerDevice *iface, void **value )
{
    if (!value) return E_POINTER;
    *value = NULL;
    return S_OK;
}

static const struct IPointerDeviceVtbl pointer_device_vtbl =
{
    pdev_QueryInterface,
    pdev_AddRef,
    pdev_Release,
    pdev_GetIids,
    pdev_GetRuntimeClassName,
    pdev_GetTrustLevel,
    pdev_get_PointerDeviceType,
    pdev_get_IsIntegrated,
    pdev_get_MaxContacts,
    pdev_get_PhysicalDeviceRect,
    pdev_get_ScreenRect,
    (void *)pdev_get_SupportedUsages,
};

static HRESULT pointer_device_create( IPointerDevice **out )
{
    struct pointer_device_impl *impl = calloc( 1, sizeof(*impl) );
    if (!impl) return E_OUTOFMEMORY;
    impl->IPointerDevice_iface.lpVtbl = &pointer_device_vtbl;
    impl->ref = 1;
    *out = &impl->IPointerDevice_iface;
    return S_OK;
}

/* PointerPointProperties */
struct pointer_point_props_impl
{
    IPointerPointProperties IPointerPointProperties_iface;
    LONG ref;
    FLOAT pressure;
    boolean is_left_button_pressed;
    boolean is_right_button_pressed;
    boolean is_middle_button_pressed;
    INT32 mouse_wheel_delta;
    PointerUpdateKind update_kind;
};

static inline struct pointer_point_props_impl *impl_from_IPointerPointProperties( IPointerPointProperties *iface )
{
    return CONTAINING_RECORD( iface, struct pointer_point_props_impl, IPointerPointProperties_iface );
}

static HRESULT WINAPI props_QueryInterface( IPointerPointProperties *iface, REFIID riid, void **out )
{
    struct pointer_point_props_impl *impl = impl_from_IPointerPointProperties( iface );
    if (!out) return E_POINTER;
    *out = NULL;
    if (IsEqualGUID( riid, &IID_IUnknown ) ||
        IsEqualGUID( riid, &IID_IInspectable ) ||
        IsEqualGUID( riid, &IID_IAgileObject ) ||
        IsEqualGUID( riid, &IID_IPointerPointProperties ))
    {
        *out = &impl->IPointerPointProperties_iface;
        IInspectable_AddRef( (IInspectable *)*out );
        return S_OK;
    }
    return E_NOINTERFACE;
}

static ULONG WINAPI props_AddRef( IPointerPointProperties *iface )
{
    struct pointer_point_props_impl *impl = impl_from_IPointerPointProperties( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI props_Release( IPointerPointProperties *iface )
{
    struct pointer_point_props_impl *impl = impl_from_IPointerPointProperties( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI props_GetIids( IPointerPointProperties *iface, ULONG *count, IID **iids )
{
    if (count) *count = 0;
    return S_OK;
}

static HRESULT WINAPI props_GetRuntimeClassName( IPointerPointProperties *iface, HSTRING *name )
{
    return WindowsCreateString( L"Windows.UI.Input.PointerPointProperties", 39, name );
}

static HRESULT WINAPI props_GetTrustLevel( IPointerPointProperties *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_POINTER;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI props_get_Pressure( IPointerPointProperties *iface, FLOAT *value )
{
    struct pointer_point_props_impl *impl = impl_from_IPointerPointProperties( iface );
    if (!value) return E_POINTER;
    *value = impl->pressure;
    return S_OK;
}

static HRESULT WINAPI props_get_IsInverted( IPointerPointProperties *iface, boolean *value )
{
    if (!value) return E_POINTER;
    *value = FALSE;
    return S_OK;
}

static HRESULT WINAPI props_get_IsEraser( IPointerPointProperties *iface, boolean *value )
{
    if (!value) return E_POINTER;
    *value = FALSE;
    return S_OK;
}

static HRESULT WINAPI props_get_Orientation( IPointerPointProperties *iface, FLOAT *value )
{
    if (!value) return E_POINTER;
    *value = 0.0f;
    return S_OK;
}

static HRESULT WINAPI props_get_XTilt( IPointerPointProperties *iface, FLOAT *value )
{
    if (!value) return E_POINTER;
    *value = 0.0f;
    return S_OK;
}

static HRESULT WINAPI props_get_YTilt( IPointerPointProperties *iface, FLOAT *value )
{
    if (!value) return E_POINTER;
    *value = 0.0f;
    return S_OK;
}

static HRESULT WINAPI props_get_Twist( IPointerPointProperties *iface, FLOAT *value )
{
    if (!value) return E_POINTER;
    *value = 0.0f;
    return S_OK;
}

static HRESULT WINAPI props_get_ContactRect( IPointerPointProperties *iface, Rect *value )
{
    if (!value) return E_POINTER;
    value->X = 0; value->Y = 0; value->Width = 0; value->Height = 0;
    return S_OK;
}

static HRESULT WINAPI props_get_ContactRectRaw( IPointerPointProperties *iface, Rect *value )
{
    if (!value) return E_POINTER;
    value->X = 0; value->Y = 0; value->Width = 0; value->Height = 0;
    return S_OK;
}

static HRESULT WINAPI props_get_TouchConfidence( IPointerPointProperties *iface, boolean *value )
{
    if (!value) return E_POINTER;
    *value = TRUE;
    return S_OK;
}

static HRESULT WINAPI props_get_IsLeftButtonPressed( IPointerPointProperties *iface, boolean *value )
{
    struct pointer_point_props_impl *impl = impl_from_IPointerPointProperties( iface );
    if (!value) return E_POINTER;
    *value = impl->is_left_button_pressed;
    return S_OK;
}

static HRESULT WINAPI props_get_IsRightButtonPressed( IPointerPointProperties *iface, boolean *value )
{
    struct pointer_point_props_impl *impl = impl_from_IPointerPointProperties( iface );
    if (!value) return E_POINTER;
    *value = impl->is_right_button_pressed;
    return S_OK;
}

static HRESULT WINAPI props_get_IsMiddleButtonPressed( IPointerPointProperties *iface, boolean *value )
{
    struct pointer_point_props_impl *impl = impl_from_IPointerPointProperties( iface );
    if (!value) return E_POINTER;
    *value = impl->is_middle_button_pressed;
    return S_OK;
}

static HRESULT WINAPI props_get_MouseWheelDelta( IPointerPointProperties *iface, INT32 *value )
{
    struct pointer_point_props_impl *impl = impl_from_IPointerPointProperties( iface );
    if (!value) return E_POINTER;
    *value = impl->mouse_wheel_delta;
    return S_OK;
}

static HRESULT WINAPI props_get_IsHorizontalMouseWheel( IPointerPointProperties *iface, boolean *value )
{
    if (!value) return E_POINTER;
    *value = FALSE;
    return S_OK;
}

static HRESULT WINAPI props_get_IsPrimary( IPointerPointProperties *iface, boolean *value )
{
    if (!value) return E_POINTER;
    *value = TRUE;
    return S_OK;
}

static HRESULT WINAPI props_get_IsInRange( IPointerPointProperties *iface, boolean *value )
{
    if (!value) return E_POINTER;
    *value = TRUE;
    return S_OK;
}

static HRESULT WINAPI props_get_IsCanceled( IPointerPointProperties *iface, boolean *value )
{
    if (!value) return E_POINTER;
    *value = FALSE;
    return S_OK;
}

static HRESULT WINAPI props_get_IsBarrelButtonPressed( IPointerPointProperties *iface, boolean *value )
{
    if (!value) return E_POINTER;
    *value = FALSE;
    return S_OK;
}

static HRESULT WINAPI props_get_IsXButton1Pressed( IPointerPointProperties *iface, boolean *value )
{
    if (!value) return E_POINTER;
    *value = FALSE;
    return S_OK;
}

static HRESULT WINAPI props_get_IsXButton2Pressed( IPointerPointProperties *iface, boolean *value )
{
    if (!value) return E_POINTER;
    *value = FALSE;
    return S_OK;
}

static HRESULT WINAPI props_get_PointerUpdateKind( IPointerPointProperties *iface, PointerUpdateKind *value )
{
    struct pointer_point_props_impl *impl = impl_from_IPointerPointProperties( iface );
    if (!value) return E_POINTER;
    *value = impl->update_kind;
    return S_OK;
}

static HRESULT WINAPI props_HasUsage( IPointerPointProperties *iface, UINT32 usagePage, UINT32 usageId, boolean *value )
{
    if (!value) return E_POINTER;
    *value = FALSE;
    return S_OK;
}

static HRESULT WINAPI props_GetUsageValue( IPointerPointProperties *iface, UINT32 usagePage, UINT32 usageId, INT32 *value )
{
    if (!value) return E_POINTER;
    *value = 0;
    return S_OK;
}

static const struct IPointerPointPropertiesVtbl pointer_point_props_vtbl =
{
    props_QueryInterface,
    props_AddRef,
    props_Release,
    props_GetIids,
    props_GetRuntimeClassName,
    props_GetTrustLevel,
    props_get_Pressure,
    props_get_IsInverted,
    props_get_IsEraser,
    props_get_Orientation,
    props_get_XTilt,
    props_get_YTilt,
    props_get_Twist,
    props_get_ContactRect,
    props_get_ContactRectRaw,
    props_get_TouchConfidence,
    props_get_IsLeftButtonPressed,
    props_get_IsRightButtonPressed,
    props_get_IsMiddleButtonPressed,
    props_get_MouseWheelDelta,
    props_get_IsHorizontalMouseWheel,
    props_get_IsPrimary,
    props_get_IsInRange,
    props_get_IsCanceled,
    props_get_IsBarrelButtonPressed,
    props_get_IsXButton1Pressed,
    props_get_IsXButton2Pressed,
    props_get_PointerUpdateKind,
    props_HasUsage,
    props_GetUsageValue,
};

static HRESULT pointer_point_props_create( boolean left, boolean right, boolean middle,
                                           PointerUpdateKind update_kind, INT32 wheel_delta,
                                           IPointerPointProperties **out )
{
    struct pointer_point_props_impl *impl = calloc( 1, sizeof(*impl) );
    if (!impl) return E_OUTOFMEMORY;
    impl->IPointerPointProperties_iface.lpVtbl = &pointer_point_props_vtbl;
    impl->ref = 1;
    impl->is_left_button_pressed = left;
    impl->is_right_button_pressed = right;
    impl->is_middle_button_pressed = middle;
    impl->pressure = (left || right || middle) ? 1.0f : 0.0f;
    impl->update_kind = update_kind;
    impl->mouse_wheel_delta = wheel_delta;
    *out = &impl->IPointerPointProperties_iface;
    return S_OK;
}

/* PointerPoint */
struct pointer_point_impl
{
    IPointerPoint IPointerPoint_iface;
    LONG ref;
    Point position;
    Point raw_position;
    UINT32 pointer_id;
    UINT32 frame_id;
    UINT64 timestamp;
    boolean in_contact;
    IPointerPointProperties *properties;
};

static inline struct pointer_point_impl *impl_from_IPointerPoint( IPointerPoint *iface )
{
    return CONTAINING_RECORD( iface, struct pointer_point_impl, IPointerPoint_iface );
}

static HRESULT WINAPI ppoint_QueryInterface( IPointerPoint *iface, REFIID riid, void **out )
{
    struct pointer_point_impl *impl = impl_from_IPointerPoint( iface );
    if (!out) return E_POINTER;
    *out = NULL;
    if (IsEqualGUID( riid, &IID_IUnknown ) ||
        IsEqualGUID( riid, &IID_IInspectable ) ||
        IsEqualGUID( riid, &IID_IAgileObject ) ||
        IsEqualGUID( riid, &IID_IPointerPoint ))
    {
        *out = &impl->IPointerPoint_iface;
        IInspectable_AddRef( (IInspectable *)*out );
        return S_OK;
    }
    return E_NOINTERFACE;
}

static ULONG WINAPI ppoint_AddRef( IPointerPoint *iface )
{
    struct pointer_point_impl *impl = impl_from_IPointerPoint( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI ppoint_Release( IPointerPoint *iface )
{
    struct pointer_point_impl *impl = impl_from_IPointerPoint( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    if (!ref)
    {
        if (impl->properties) IPointerPointProperties_Release( impl->properties );
        free( impl );
    }
    return ref;
}

static HRESULT WINAPI ppoint_GetIids( IPointerPoint *iface, ULONG *count, IID **iids )
{
    if (count) *count = 0;
    return S_OK;
}

static HRESULT WINAPI ppoint_GetRuntimeClassName( IPointerPoint *iface, HSTRING *name )
{
    return WindowsCreateString( L"Windows.UI.Input.PointerPoint", 29, name );
}

static HRESULT WINAPI ppoint_GetTrustLevel( IPointerPoint *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_POINTER;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI ppoint_get_PointerDevice( IPointerPoint *iface, IPointerDevice **value )
{
    if (!value) return E_POINTER;
    return pointer_device_create( value );
}

static HRESULT WINAPI ppoint_get_Position( IPointerPoint *iface, Point *value )
{
    struct pointer_point_impl *impl = impl_from_IPointerPoint( iface );
    if (!value) return E_POINTER;
    *value = impl->position;
    return S_OK;
}

static HRESULT WINAPI ppoint_get_RawPosition( IPointerPoint *iface, Point *value )
{
    struct pointer_point_impl *impl = impl_from_IPointerPoint( iface );
    if (!value) return E_POINTER;
    *value = impl->raw_position;
    return S_OK;
}

static HRESULT WINAPI ppoint_get_pointer_id( IPointerPoint *iface, UINT32 *value )
{
    struct pointer_point_impl *impl = impl_from_IPointerPoint( iface );
    if (!value) return E_POINTER;
    *value = impl->pointer_id;
    return S_OK;
}

static HRESULT WINAPI ppoint_get_FrameId( IPointerPoint *iface, UINT32 *value )
{
    struct pointer_point_impl *impl = impl_from_IPointerPoint( iface );
    if (!value) return E_POINTER;
    *value = impl->frame_id;
    return S_OK;
}

static HRESULT WINAPI ppoint_get_Timestamp( IPointerPoint *iface, UINT64 *value )
{
    struct pointer_point_impl *impl = impl_from_IPointerPoint( iface );
    if (!value) return E_POINTER;
    *value = impl->timestamp;
    return S_OK;
}

static HRESULT WINAPI ppoint_get_IsInContact( IPointerPoint *iface, boolean *value )
{
    struct pointer_point_impl *impl = impl_from_IPointerPoint( iface );
    if (!value) return E_POINTER;
    *value = impl->in_contact;
    return S_OK;
}

static HRESULT WINAPI ppoint_get_Properties( IPointerPoint *iface, IPointerPointProperties **value )
{
    struct pointer_point_impl *impl = impl_from_IPointerPoint( iface );
    if (!value) return E_POINTER;
    *value = impl->properties;
    if (*value) IPointerPointProperties_AddRef( *value );
    return S_OK;
}

static const struct IPointerPointVtbl pointer_point_vtbl =
{
    ppoint_QueryInterface,
    ppoint_AddRef,
    ppoint_Release,
    ppoint_GetIids,
    ppoint_GetRuntimeClassName,
    ppoint_GetTrustLevel,
    ppoint_get_PointerDevice,
    ppoint_get_Position,
    ppoint_get_RawPosition,
    ppoint_get_pointer_id,
    ppoint_get_FrameId,
    ppoint_get_Timestamp,
    ppoint_get_IsInContact,
    ppoint_get_Properties,
};

static HRESULT pointer_point_create( FLOAT x, FLOAT y, boolean left, boolean right, boolean middle,
                                     PointerUpdateKind update_kind, INT32 wheel_delta,
                                     IPointerPoint **out )
{
    static UINT32 frame_counter = 1;
    struct pointer_point_impl *impl = calloc( 1, sizeof(*impl) );
    if (!impl) return E_OUTOFMEMORY;
    impl->IPointerPoint_iface.lpVtbl = &pointer_point_vtbl;
    impl->ref = 1;
    impl->position.X = x;
    impl->position.Y = y;
    impl->raw_position.X = x;
    impl->raw_position.Y = y;
    impl->pointer_id = 1;
    impl->frame_id = frame_counter++;
    impl->timestamp = (UINT64)GetTickCount64() * 1000;
    impl->in_contact = left || right || middle;
    pointer_point_props_create( left, right, middle, update_kind, wheel_delta, &impl->properties );
    *out = &impl->IPointerPoint_iface;
    return S_OK;
}

/* PointerEventArgs */
struct pointer_event_args_impl
{
    IPointerEventArgs IPointerEventArgs_iface;
    ICoreWindowEventArgs ICoreWindowEventArgs_iface;
    LONG ref;
    boolean handled;
    IPointerPoint *point;
    VirtualKeyModifiers key_modifiers;
};

#define impl_from_IPointerEventArgs(ptr) ((struct pointer_event_args_impl *)((char *)(ptr) - offsetof(struct pointer_event_args_impl, IPointerEventArgs_iface)))
#define impl_from_ICoreWindowEventArgs_p(ptr) ((struct pointer_event_args_impl *)((char *)(ptr) - offsetof(struct pointer_event_args_impl, ICoreWindowEventArgs_iface)))

static HRESULT WINAPI pargs_QueryInterface( IPointerEventArgs *iface, REFIID riid, void **out )
{
    struct pointer_event_args_impl *impl = impl_from_IPointerEventArgs( iface );
    if (!out) return E_POINTER;
    *out = NULL;
    if (IsEqualGUID( riid, &IID_IUnknown ) ||
        IsEqualGUID( riid, &IID_IInspectable ) ||
        IsEqualGUID( riid, &IID_IAgileObject ) ||
        IsEqualGUID( riid, &IID_IPointerEventArgs ))
    {
        *out = &impl->IPointerEventArgs_iface;
        IInspectable_AddRef( (IInspectable *)*out );
        return S_OK;
    }
    if (IsEqualGUID( riid, &IID_ICoreWindowEventArgs ))
    {
        *out = &impl->ICoreWindowEventArgs_iface;
        IInspectable_AddRef( (IInspectable *)&impl->IPointerEventArgs_iface );
        return S_OK;
    }
    return E_NOINTERFACE;
}

static ULONG WINAPI pargs_AddRef( IPointerEventArgs *iface )
{
    struct pointer_event_args_impl *impl = impl_from_IPointerEventArgs( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI pargs_Release( IPointerEventArgs *iface )
{
    struct pointer_event_args_impl *impl = impl_from_IPointerEventArgs( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    if (!ref)
    {
        if (impl->point) IPointerPoint_Release( impl->point );
        free( impl );
    }
    return ref;
}

static HRESULT WINAPI pargs_GetIids( IPointerEventArgs *iface, ULONG *count, IID **iids )
{
    if (count) *count = 0;
    return S_OK;
}

static HRESULT WINAPI pargs_GetRuntimeClassName( IPointerEventArgs *iface, HSTRING *name )
{
    return WindowsCreateString( L"Windows.UI.Core.PointerEventArgs", 31, name );
}

static HRESULT WINAPI pargs_GetTrustLevel( IPointerEventArgs *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_POINTER;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI pargs_get_CurrentPoint( IPointerEventArgs *iface, IPointerPoint **value )
{
    struct pointer_event_args_impl *impl = impl_from_IPointerEventArgs( iface );
    if (!value) return E_POINTER;
    *value = impl->point;
    if (*value) IPointerPoint_AddRef( *value );
    return S_OK;
}

static HRESULT WINAPI pargs_get_KeyModifiers( IPointerEventArgs *iface, VirtualKeyModifiers *value )
{
    struct pointer_event_args_impl *impl = impl_from_IPointerEventArgs( iface );
    if (!value) return E_POINTER;
    *value = impl->key_modifiers;
    return S_OK;
}

static HRESULT WINAPI pargs_GetIntermediatePoints( IPointerEventArgs *iface, void **value )
{
    if (!value) return E_POINTER;
    *value = NULL;
    return S_OK;
}

static const struct IPointerEventArgsVtbl pointer_event_args_vtbl =
{
    pargs_QueryInterface,
    pargs_AddRef,
    pargs_Release,
    pargs_GetIids,
    pargs_GetRuntimeClassName,
    pargs_GetTrustLevel,
    pargs_get_CurrentPoint,
    pargs_get_KeyModifiers,
    (void *)pargs_GetIntermediatePoints,
};

static HRESULT WINAPI pargs_core_QueryInterface( ICoreWindowEventArgs *iface, REFIID riid, void **out )
{
    struct pointer_event_args_impl *impl = impl_from_ICoreWindowEventArgs_p( iface );
    return pargs_QueryInterface( &impl->IPointerEventArgs_iface, riid, out );
}

static ULONG WINAPI pargs_core_AddRef( ICoreWindowEventArgs *iface )
{
    struct pointer_event_args_impl *impl = impl_from_ICoreWindowEventArgs_p( iface );
    return pargs_AddRef( &impl->IPointerEventArgs_iface );
}

static ULONG WINAPI pargs_core_Release( ICoreWindowEventArgs *iface )
{
    struct pointer_event_args_impl *impl = impl_from_ICoreWindowEventArgs_p( iface );
    return pargs_Release( &impl->IPointerEventArgs_iface );
}

static HRESULT WINAPI pargs_core_get_Handled( ICoreWindowEventArgs *iface, boolean *value )
{
    struct pointer_event_args_impl *impl = impl_from_ICoreWindowEventArgs_p( iface );
    if (!value) return E_POINTER;
    *value = impl->handled;
    return S_OK;
}

static HRESULT WINAPI pargs_core_put_Handled( ICoreWindowEventArgs *iface, boolean value )
{
    struct pointer_event_args_impl *impl = impl_from_ICoreWindowEventArgs_p( iface );
    impl->handled = value;
    return S_OK;
}

static const struct ICoreWindowEventArgsVtbl pargs_core_event_args_vtbl =
{
    pargs_core_QueryInterface,
    pargs_core_AddRef,
    pargs_core_Release,
    (void *)pargs_GetIids,
    (void *)pargs_GetRuntimeClassName,
    (void *)pargs_GetTrustLevel,
    pargs_core_get_Handled,
    pargs_core_put_Handled,
};

static HRESULT pointer_event_args_create( FLOAT x, FLOAT y, boolean left, boolean right, boolean middle,
                                         PointerUpdateKind update_kind, INT32 wheel_delta,
                                         IPointerEventArgs **out )
{
    struct pointer_event_args_impl *impl = calloc( 1, sizeof(*impl) );
    VirtualKeyModifiers mods = VirtualKeyModifiers_None;
    if (!impl) return E_OUTOFMEMORY;
    impl->IPointerEventArgs_iface.lpVtbl = &pointer_event_args_vtbl;
    impl->ICoreWindowEventArgs_iface.lpVtbl = &pargs_core_event_args_vtbl;
    impl->ref = 1;

    if (GetKeyState( VK_SHIFT ) & 0x8000) mods |= VirtualKeyModifiers_Shift;
    if (GetKeyState( VK_CONTROL ) & 0x8000) mods |= VirtualKeyModifiers_Control;
    if (GetKeyState( VK_MENU ) & 0x8000) mods |= VirtualKeyModifiers_Menu;
    if ((GetKeyState( VK_LWIN ) | GetKeyState( VK_RWIN )) & 0x8000) mods |= VirtualKeyModifiers_Windows;
    impl->key_modifiers = mods;

    pointer_point_create( x, y, left, right, middle, update_kind, wheel_delta, &impl->point );
    *out = &impl->IPointerEventArgs_iface;
    return S_OK;
}

/* KeyEventArgs */
static const GUID IID_IAcceleratorKeyEventArgs2 = {0xd300a9f6, 0x2f7e, 0x4873, {0xa5, 0x55, 0x16, 0x6e, 0x59, 0x6e, 0xe1, 0xc5}};

typedef struct IAcceleratorKeyEventArgs2 IAcceleratorKeyEventArgs2;
typedef struct IAcceleratorKeyEventArgs2Vtbl
{
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IAcceleratorKeyEventArgs2 *This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IAcceleratorKeyEventArgs2 *This);
    ULONG (STDMETHODCALLTYPE *Release)(IAcceleratorKeyEventArgs2 *This);
    HRESULT (STDMETHODCALLTYPE *GetIids)(IAcceleratorKeyEventArgs2 *This, ULONG *iidCount, IID **iids);
    HRESULT (STDMETHODCALLTYPE *GetRuntimeClassName)(IAcceleratorKeyEventArgs2 *This, HSTRING *className);
    HRESULT (STDMETHODCALLTYPE *GetTrustLevel)(IAcceleratorKeyEventArgs2 *This, TrustLevel *trustLevel);
    HRESULT (STDMETHODCALLTYPE *get_DeviceId)(IAcceleratorKeyEventArgs2 *This, HSTRING *value);
} IAcceleratorKeyEventArgs2Vtbl;

struct IAcceleratorKeyEventArgs2
{
    const IAcceleratorKeyEventArgs2Vtbl *lpVtbl;
};

struct key_event_args_impl
{
    IKeyEventArgs IKeyEventArgs_iface;
    ICoreWindowEventArgs ICoreWindowEventArgs_iface;
    IKeyEventArgs2 IKeyEventArgs2_iface;
    LONG ref;
    boolean handled;
    VirtualKey virtual_key;
    CorePhysicalKeyStatus key_status;
};

#define impl_from_IKeyEventArgs(ptr) ((struct key_event_args_impl *)((char *)(ptr) - offsetof(struct key_event_args_impl, IKeyEventArgs_iface)))
#define impl_from_ICoreWindowEventArgs_k(ptr) ((struct key_event_args_impl *)((char *)(ptr) - offsetof(struct key_event_args_impl, ICoreWindowEventArgs_iface)))
#define impl_from_IKeyEventArgs2(ptr) ((struct key_event_args_impl *)((char *)(ptr) - offsetof(struct key_event_args_impl, IKeyEventArgs2_iface)))

static HRESULT WINAPI kargs_QueryInterface( IKeyEventArgs *iface, REFIID riid, void **out )
{
    struct key_event_args_impl *impl = impl_from_IKeyEventArgs( iface );
    if (!out) return E_POINTER;
    *out = NULL;
    if (IsEqualGUID( riid, &IID_IUnknown ) ||
        IsEqualGUID( riid, &IID_IInspectable ) ||
        IsEqualGUID( riid, &IID_IAgileObject ) ||
        IsEqualGUID( riid, &IID_IKeyEventArgs ))
    {
        *out = &impl->IKeyEventArgs_iface;
        IInspectable_AddRef( (IInspectable *)*out );
        return S_OK;
    }
    if (IsEqualGUID( riid, &IID_ICoreWindowEventArgs ))
    {
        *out = &impl->ICoreWindowEventArgs_iface;
        IInspectable_AddRef( (IInspectable *)&impl->IKeyEventArgs_iface );
        return S_OK;
    }
    if (IsEqualGUID( riid, &IID_IKeyEventArgs2 ))
    {
        *out = &impl->IKeyEventArgs2_iface;
        IInspectable_AddRef( (IInspectable *)&impl->IKeyEventArgs_iface );
        return S_OK;
    }
    return E_NOINTERFACE;
}

static ULONG WINAPI kargs_AddRef( IKeyEventArgs *iface )
{
    struct key_event_args_impl *impl = impl_from_IKeyEventArgs( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI kargs_Release( IKeyEventArgs *iface )
{
    struct key_event_args_impl *impl = impl_from_IKeyEventArgs( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI kargs_GetIids( IKeyEventArgs *iface, ULONG *count, IID **iids )
{
    if (count) *count = 0;
    return S_OK;
}

static HRESULT WINAPI kargs_GetRuntimeClassName( IKeyEventArgs *iface, HSTRING *name )
{
    return WindowsCreateString( L"Windows.UI.Core.KeyEventArgs", 27, name );
}

static HRESULT WINAPI kargs_GetTrustLevel( IKeyEventArgs *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_POINTER;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI kargs_get_VirtualKey( IKeyEventArgs *iface, VirtualKey *value )
{
    struct key_event_args_impl *impl = impl_from_IKeyEventArgs( iface );
    if (!value) return E_POINTER;
    *value = impl->virtual_key;
    return S_OK;
}

static HRESULT WINAPI kargs_get_KeyStatus( IKeyEventArgs *iface, CorePhysicalKeyStatus *value )
{
    struct key_event_args_impl *impl = impl_from_IKeyEventArgs( iface );
    if (!value) return E_POINTER;
    *value = impl->key_status;
    return S_OK;
}

static const struct IKeyEventArgsVtbl key_event_args_vtbl =
{
    kargs_QueryInterface,
    kargs_AddRef,
    kargs_Release,
    kargs_GetIids,
    kargs_GetRuntimeClassName,
    kargs_GetTrustLevel,
    kargs_get_VirtualKey,
    kargs_get_KeyStatus,
};

static HRESULT WINAPI kargs2_QueryInterface( IKeyEventArgs2 *iface, REFIID riid, void **out )
{
    struct key_event_args_impl *impl = impl_from_IKeyEventArgs2( iface );
    return kargs_QueryInterface( &impl->IKeyEventArgs_iface, riid, out );
}

static ULONG WINAPI kargs2_AddRef( IKeyEventArgs2 *iface )
{
    struct key_event_args_impl *impl = impl_from_IKeyEventArgs2( iface );
    return kargs_AddRef( &impl->IKeyEventArgs_iface );
}

static ULONG WINAPI kargs2_Release( IKeyEventArgs2 *iface )
{
    struct key_event_args_impl *impl = impl_from_IKeyEventArgs2( iface );
    return kargs_Release( &impl->IKeyEventArgs_iface );
}

static HRESULT WINAPI kargs2_get_DeviceId( IKeyEventArgs2 *iface, HSTRING *value )
{
    if (!value) return E_POINTER;
    return WindowsCreateString( L"", 0, value );
}

static const struct IKeyEventArgs2Vtbl key_event_args2_vtbl =
{
    kargs2_QueryInterface,
    kargs2_AddRef,
    kargs2_Release,
    (void *)kargs_GetIids,
    (void *)kargs_GetRuntimeClassName,
    (void *)kargs_GetTrustLevel,
    kargs2_get_DeviceId,
};

static HRESULT WINAPI kargs_core_QueryInterface( ICoreWindowEventArgs *iface, REFIID riid, void **out )
{
    struct key_event_args_impl *impl = impl_from_ICoreWindowEventArgs_k( iface );
    return kargs_QueryInterface( &impl->IKeyEventArgs_iface, riid, out );
}

static ULONG WINAPI kargs_core_AddRef( ICoreWindowEventArgs *iface )
{
    struct key_event_args_impl *impl = impl_from_ICoreWindowEventArgs_k( iface );
    return kargs_AddRef( &impl->IKeyEventArgs_iface );
}

static ULONG WINAPI kargs_core_Release( ICoreWindowEventArgs *iface )
{
    struct key_event_args_impl *impl = impl_from_ICoreWindowEventArgs_k( iface );
    return kargs_Release( &impl->IKeyEventArgs_iface );
}

static HRESULT WINAPI kargs_core_get_Handled( ICoreWindowEventArgs *iface, boolean *value )
{
    struct key_event_args_impl *impl = impl_from_ICoreWindowEventArgs_k( iface );
    if (!value) return E_POINTER;
    *value = impl->handled;
    return S_OK;
}

static HRESULT WINAPI kargs_core_put_Handled( ICoreWindowEventArgs *iface, boolean value )
{
    struct key_event_args_impl *impl = impl_from_ICoreWindowEventArgs_k( iface );
    impl->handled = value;
    return S_OK;
}

static const struct ICoreWindowEventArgsVtbl kargs_core_event_args_vtbl =
{
    kargs_core_QueryInterface,
    kargs_core_AddRef,
    kargs_core_Release,
    (void *)kargs_GetIids,
    (void *)kargs_GetRuntimeClassName,
    (void *)kargs_GetTrustLevel,
    kargs_core_get_Handled,
    kargs_core_put_Handled,
};

static HRESULT key_event_args_create( VirtualKey key, LPARAM lparam, IKeyEventArgs **out )
{
    struct key_event_args_impl *impl = calloc( 1, sizeof(*impl) );
    if (!impl) return E_OUTOFMEMORY;
    impl->IKeyEventArgs_iface.lpVtbl = &key_event_args_vtbl;
    impl->ICoreWindowEventArgs_iface.lpVtbl = &kargs_core_event_args_vtbl;
    impl->IKeyEventArgs2_iface.lpVtbl = &key_event_args2_vtbl;
    impl->ref = 1;
    impl->virtual_key = key;
    impl->key_status.RepeatCount = LOWORD( lparam );
    impl->key_status.ScanCode = LOBYTE( HIWORD( lparam ) );
    impl->key_status.IsExtendedKey = (HIWORD( lparam ) & KF_EXTENDED) != 0;
    impl->key_status.IsMenuKeyDown = (HIWORD( lparam ) & KF_ALTDOWN) != 0;
    impl->key_status.WasKeyDown = (HIWORD( lparam ) & KF_REPEAT) != 0;
    impl->key_status.IsKeyReleased = (HIWORD( lparam ) & KF_UP) != 0;
    *out = &impl->IKeyEventArgs_iface;
    return S_OK;
}

/* AcceleratorKeyEventArgs */
struct accel_key_event_args_impl
{
    IAcceleratorKeyEventArgs IAcceleratorKeyEventArgs_iface;
    ICoreWindowEventArgs ICoreWindowEventArgs_iface;
    IAcceleratorKeyEventArgs2 IAcceleratorKeyEventArgs2_iface;
    LONG ref;
    boolean handled;
    CoreAcceleratorKeyEventType event_type;
    VirtualKey virtual_key;
    CorePhysicalKeyStatus key_status;
};

#define impl_from_IAcceleratorKeyEventArgs(ptr) ((struct accel_key_event_args_impl *)((char *)(ptr) - offsetof(struct accel_key_event_args_impl, IAcceleratorKeyEventArgs_iface)))
#define impl_from_ICoreWindowEventArgs_accel(ptr) ((struct accel_key_event_args_impl *)((char *)(ptr) - offsetof(struct accel_key_event_args_impl, ICoreWindowEventArgs_iface)))
#define impl_from_IAcceleratorKeyEventArgs2(ptr) ((struct accel_key_event_args_impl *)((char *)(ptr) - offsetof(struct accel_key_event_args_impl, IAcceleratorKeyEventArgs2_iface)))

static HRESULT WINAPI accel_args_QueryInterface( IAcceleratorKeyEventArgs *iface, REFIID riid, void **out )
{
    struct accel_key_event_args_impl *impl = impl_from_IAcceleratorKeyEventArgs( iface );
    if (!out) return E_POINTER;
    *out = NULL;
    if (IsEqualGUID( riid, &IID_IUnknown ) ||
        IsEqualGUID( riid, &IID_IInspectable ) ||
        IsEqualGUID( riid, &IID_IAgileObject ) ||
        IsEqualGUID( riid, &IID_IAcceleratorKeyEventArgs ))
    {
        *out = &impl->IAcceleratorKeyEventArgs_iface;
        IInspectable_AddRef( (IInspectable *)*out );
        return S_OK;
    }
    if (IsEqualGUID( riid, &IID_ICoreWindowEventArgs ))
    {
        *out = &impl->ICoreWindowEventArgs_iface;
        IInspectable_AddRef( (IInspectable *)&impl->IAcceleratorKeyEventArgs_iface );
        return S_OK;
    }
    if (IsEqualGUID( riid, &IID_IAcceleratorKeyEventArgs2 ))
    {
        *out = &impl->IAcceleratorKeyEventArgs2_iface;
        IInspectable_AddRef( (IInspectable *)&impl->IAcceleratorKeyEventArgs_iface );
        return S_OK;
    }
    return E_NOINTERFACE;
}

static ULONG WINAPI accel_args_AddRef( IAcceleratorKeyEventArgs *iface )
{
    struct accel_key_event_args_impl *impl = impl_from_IAcceleratorKeyEventArgs( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI accel_args_Release( IAcceleratorKeyEventArgs *iface )
{
    struct accel_key_event_args_impl *impl = impl_from_IAcceleratorKeyEventArgs( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI accel_args_GetIids( IAcceleratorKeyEventArgs *iface, ULONG *count, IID **iids )
{
    if (count) *count = 0;
    return S_OK;
}

static HRESULT WINAPI accel_args_GetRuntimeClassName( IAcceleratorKeyEventArgs *iface, HSTRING *name )
{
    return WindowsCreateString( L"Windows.UI.Core.AcceleratorKeyEventArgs", 38, name );
}

static HRESULT WINAPI accel_args_GetTrustLevel( IAcceleratorKeyEventArgs *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_POINTER;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI accel_args_get_EventType( IAcceleratorKeyEventArgs *iface, CoreAcceleratorKeyEventType *value )
{
    struct accel_key_event_args_impl *impl = impl_from_IAcceleratorKeyEventArgs( iface );
    if (!value) return E_POINTER;
    *value = impl->event_type;
    return S_OK;
}

static HRESULT WINAPI accel_args_get_VirtualKey( IAcceleratorKeyEventArgs *iface, VirtualKey *value )
{
    struct accel_key_event_args_impl *impl = impl_from_IAcceleratorKeyEventArgs( iface );
    if (!value) return E_POINTER;
    *value = impl->virtual_key;
    return S_OK;
}

static HRESULT WINAPI accel_args_get_KeyStatus( IAcceleratorKeyEventArgs *iface, CorePhysicalKeyStatus *value )
{
    struct accel_key_event_args_impl *impl = impl_from_IAcceleratorKeyEventArgs( iface );
    if (!value) return E_POINTER;
    *value = impl->key_status;
    return S_OK;
}

static const struct IAcceleratorKeyEventArgsVtbl accel_key_event_args_vtbl =
{
    accel_args_QueryInterface,
    accel_args_AddRef,
    accel_args_Release,
    accel_args_GetIids,
    accel_args_GetRuntimeClassName,
    accel_args_GetTrustLevel,
    accel_args_get_EventType,
    accel_args_get_VirtualKey,
    accel_args_get_KeyStatus,
};

static HRESULT WINAPI accel_args2_QueryInterface( IAcceleratorKeyEventArgs2 *iface, REFIID riid, void **out )
{
    struct accel_key_event_args_impl *impl = impl_from_IAcceleratorKeyEventArgs2( iface );
    return accel_args_QueryInterface( &impl->IAcceleratorKeyEventArgs_iface, riid, out );
}

static ULONG WINAPI accel_args2_AddRef( IAcceleratorKeyEventArgs2 *iface )
{
    struct accel_key_event_args_impl *impl = impl_from_IAcceleratorKeyEventArgs2( iface );
    return accel_args_AddRef( &impl->IAcceleratorKeyEventArgs_iface );
}

static ULONG WINAPI accel_args2_Release( IAcceleratorKeyEventArgs2 *iface )
{
    struct accel_key_event_args_impl *impl = impl_from_IAcceleratorKeyEventArgs2( iface );
    return accel_args_Release( &impl->IAcceleratorKeyEventArgs_iface );
}

static HRESULT WINAPI accel_args2_get_DeviceId( IAcceleratorKeyEventArgs2 *iface, HSTRING *value )
{
    if (!value) return E_POINTER;
    return WindowsCreateString( L"", 0, value );
}

static const struct IAcceleratorKeyEventArgs2Vtbl accel_key_event_args2_vtbl =
{
    accel_args2_QueryInterface,
    accel_args2_AddRef,
    accel_args2_Release,
    (void *)accel_args_GetIids,
    (void *)accel_args_GetRuntimeClassName,
    (void *)accel_args_GetTrustLevel,
    accel_args2_get_DeviceId,
};

static HRESULT WINAPI accel_args_core_QueryInterface( ICoreWindowEventArgs *iface, REFIID riid, void **out )
{
    struct accel_key_event_args_impl *impl = impl_from_ICoreWindowEventArgs_accel( iface );
    return accel_args_QueryInterface( &impl->IAcceleratorKeyEventArgs_iface, riid, out );
}

static ULONG WINAPI accel_args_core_AddRef( ICoreWindowEventArgs *iface )
{
    struct accel_key_event_args_impl *impl = impl_from_ICoreWindowEventArgs_accel( iface );
    return accel_args_AddRef( &impl->IAcceleratorKeyEventArgs_iface );
}

static ULONG WINAPI accel_args_core_Release( ICoreWindowEventArgs *iface )
{
    struct accel_key_event_args_impl *impl = impl_from_ICoreWindowEventArgs_accel( iface );
    return accel_args_Release( &impl->IAcceleratorKeyEventArgs_iface );
}

static HRESULT WINAPI accel_args_core_get_Handled( ICoreWindowEventArgs *iface, boolean *value )
{
    struct accel_key_event_args_impl *impl = impl_from_ICoreWindowEventArgs_accel( iface );
    if (!value) return E_POINTER;
    *value = impl->handled;
    return S_OK;
}

static HRESULT WINAPI accel_args_core_put_Handled( ICoreWindowEventArgs *iface, boolean value )
{
    struct accel_key_event_args_impl *impl = impl_from_ICoreWindowEventArgs_accel( iface );
    impl->handled = value;
    return S_OK;
}

static const struct ICoreWindowEventArgsVtbl accel_args_core_event_args_vtbl =
{
    accel_args_core_QueryInterface,
    accel_args_core_AddRef,
    accel_args_core_Release,
    (void *)accel_args_GetIids,
    (void *)accel_args_GetRuntimeClassName,
    (void *)accel_args_GetTrustLevel,
    accel_args_core_get_Handled,
    accel_args_core_put_Handled,
};

static HRESULT accel_key_event_args_create( CoreAcceleratorKeyEventType event_type, VirtualKey key, LPARAM lparam, IAcceleratorKeyEventArgs **out )
{
    struct accel_key_event_args_impl *impl = calloc( 1, sizeof(*impl) );
    if (!impl) return E_OUTOFMEMORY;
    impl->IAcceleratorKeyEventArgs_iface.lpVtbl = &accel_key_event_args_vtbl;
    impl->ICoreWindowEventArgs_iface.lpVtbl = &accel_args_core_event_args_vtbl;
    impl->IAcceleratorKeyEventArgs2_iface.lpVtbl = &accel_key_event_args2_vtbl;
    impl->ref = 1;
    impl->event_type = event_type;
    impl->virtual_key = key;
    impl->key_status.RepeatCount = LOWORD( lparam );
    impl->key_status.ScanCode = LOBYTE( HIWORD( lparam ) );
    impl->key_status.IsExtendedKey = (HIWORD( lparam ) & KF_EXTENDED) != 0;
    impl->key_status.IsMenuKeyDown = (HIWORD( lparam ) & KF_ALTDOWN) != 0;
    impl->key_status.WasKeyDown = (HIWORD( lparam ) & KF_REPEAT) != 0;
    impl->key_status.IsKeyReleased = (HIWORD( lparam ) & KF_UP) != 0;
    *out = &impl->IAcceleratorKeyEventArgs_iface;
    return S_OK;
}

/* CharacterReceivedEventArgs */
struct char_event_args_impl
{
    ICharacterReceivedEventArgs ICharacterReceivedEventArgs_iface;
    ICoreWindowEventArgs ICoreWindowEventArgs_iface;
    LONG ref;
    boolean handled;
    UINT32 key_code;
    CorePhysicalKeyStatus key_status;
};

#define impl_from_ICharacterReceivedEventArgs(ptr) ((struct char_event_args_impl *)((char *)(ptr) - offsetof(struct char_event_args_impl, ICharacterReceivedEventArgs_iface)))
#define impl_from_ICoreWindowEventArgs_c(ptr) ((struct char_event_args_impl *)((char *)(ptr) - offsetof(struct char_event_args_impl, ICoreWindowEventArgs_iface)))

static HRESULT WINAPI cargs_QueryInterface( ICharacterReceivedEventArgs *iface, REFIID riid, void **out )
{
    struct char_event_args_impl *impl = impl_from_ICharacterReceivedEventArgs( iface );
    if (!out) return E_POINTER;
    *out = NULL;
    if (IsEqualGUID( riid, &IID_IUnknown ) ||
        IsEqualGUID( riid, &IID_IInspectable ) ||
        IsEqualGUID( riid, &IID_IAgileObject ) ||
        IsEqualGUID( riid, &IID_ICharacterReceivedEventArgs ))
    {
        *out = &impl->ICharacterReceivedEventArgs_iface;
        IInspectable_AddRef( (IInspectable *)*out );
        return S_OK;
    }
    if (IsEqualGUID( riid, &IID_ICoreWindowEventArgs ))
    {
        *out = &impl->ICoreWindowEventArgs_iface;
        IInspectable_AddRef( (IInspectable *)&impl->ICharacterReceivedEventArgs_iface );
        return S_OK;
    }
    return E_NOINTERFACE;
}

static ULONG WINAPI cargs_AddRef( ICharacterReceivedEventArgs *iface )
{
    struct char_event_args_impl *impl = impl_from_ICharacterReceivedEventArgs( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI cargs_Release( ICharacterReceivedEventArgs *iface )
{
    struct char_event_args_impl *impl = impl_from_ICharacterReceivedEventArgs( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI cargs_GetIids( ICharacterReceivedEventArgs *iface, ULONG *count, IID **iids )
{
    if (count) *count = 0;
    return S_OK;
}

static HRESULT WINAPI cargs_GetRuntimeClassName( ICharacterReceivedEventArgs *iface, HSTRING *name )
{
    return WindowsCreateString( L"Windows.UI.Core.CharacterReceivedEventArgs", 41, name );
}

static HRESULT WINAPI cargs_GetTrustLevel( ICharacterReceivedEventArgs *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_POINTER;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI cargs_get_KeyCode( ICharacterReceivedEventArgs *iface, UINT32 *value )
{
    struct char_event_args_impl *impl = impl_from_ICharacterReceivedEventArgs( iface );
    if (!value) return E_POINTER;
    *value = impl->key_code;
    return S_OK;
}

static HRESULT WINAPI cargs_get_KeyStatus( ICharacterReceivedEventArgs *iface, CorePhysicalKeyStatus *value )
{
    struct char_event_args_impl *impl = impl_from_ICharacterReceivedEventArgs( iface );
    if (!value) return E_POINTER;
    *value = impl->key_status;
    return S_OK;
}

static const struct ICharacterReceivedEventArgsVtbl char_event_args_vtbl =
{
    cargs_QueryInterface,
    cargs_AddRef,
    cargs_Release,
    cargs_GetIids,
    cargs_GetRuntimeClassName,
    cargs_GetTrustLevel,
    cargs_get_KeyCode,
    cargs_get_KeyStatus,
};

static HRESULT WINAPI cargs_core_QueryInterface( ICoreWindowEventArgs *iface, REFIID riid, void **out )
{
    struct char_event_args_impl *impl = impl_from_ICoreWindowEventArgs_c( iface );
    return cargs_QueryInterface( &impl->ICharacterReceivedEventArgs_iface, riid, out );
}

static ULONG WINAPI cargs_core_AddRef( ICoreWindowEventArgs *iface )
{
    struct char_event_args_impl *impl = impl_from_ICoreWindowEventArgs_c( iface );
    return cargs_AddRef( &impl->ICharacterReceivedEventArgs_iface );
}

static ULONG WINAPI cargs_core_Release( ICoreWindowEventArgs *iface )
{
    struct char_event_args_impl *impl = impl_from_ICoreWindowEventArgs_c( iface );
    return cargs_Release( &impl->ICharacterReceivedEventArgs_iface );
}

static HRESULT WINAPI cargs_core_get_Handled( ICoreWindowEventArgs *iface, boolean *value )
{
    struct char_event_args_impl *impl = impl_from_ICoreWindowEventArgs_c( iface );
    if (!value) return E_POINTER;
    *value = impl->handled;
    return S_OK;
}

static HRESULT WINAPI cargs_core_put_Handled( ICoreWindowEventArgs *iface, boolean value )
{
    struct char_event_args_impl *impl = impl_from_ICoreWindowEventArgs_c( iface );
    impl->handled = value;
    return S_OK;
}

static const struct ICoreWindowEventArgsVtbl cargs_core_event_args_vtbl =
{
    cargs_core_QueryInterface,
    cargs_core_AddRef,
    cargs_core_Release,
    (void *)cargs_GetIids,
    (void *)cargs_GetRuntimeClassName,
    (void *)cargs_GetTrustLevel,
    cargs_core_get_Handled,
    cargs_core_put_Handled,
};

static HRESULT character_received_event_args_create( UINT32 key_code, LPARAM lparam, ICharacterReceivedEventArgs **out )
{
    struct char_event_args_impl *impl = calloc( 1, sizeof(*impl) );
    if (!impl) return E_OUTOFMEMORY;
    impl->ICharacterReceivedEventArgs_iface.lpVtbl = &char_event_args_vtbl;
    impl->ICoreWindowEventArgs_iface.lpVtbl = &cargs_core_event_args_vtbl;
    impl->ref = 1;
    impl->key_code = key_code;
    impl->key_status.RepeatCount = LOWORD( lparam );
    impl->key_status.ScanCode = LOBYTE( HIWORD( lparam ) );
    impl->key_status.IsExtendedKey = (HIWORD( lparam ) & KF_EXTENDED) != 0;
    impl->key_status.IsMenuKeyDown = (HIWORD( lparam ) & KF_ALTDOWN) != 0;
    impl->key_status.WasKeyDown = (HIWORD( lparam ) & KF_REPEAT) != 0;
    impl->key_status.IsKeyReleased = (HIWORD( lparam ) & KF_UP) != 0;
    *out = &impl->ICharacterReceivedEventArgs_iface;
    return S_OK;
}

/* WindowSizeChangedEventArgs */
struct size_event_args_impl
{
    IWindowSizeChangedEventArgs IWindowSizeChangedEventArgs_iface;
    ICoreWindowEventArgs ICoreWindowEventArgs_iface;
    LONG ref;
    boolean handled;
    Size size;
};

#define impl_from_IWindowSizeChangedEventArgs(ptr) ((struct size_event_args_impl *)((char *)(ptr) - offsetof(struct size_event_args_impl, IWindowSizeChangedEventArgs_iface)))
#define impl_from_ICoreWindowEventArgs_s(ptr) ((struct size_event_args_impl *)((char *)(ptr) - offsetof(struct size_event_args_impl, ICoreWindowEventArgs_iface)))

static HRESULT WINAPI sargs_QueryInterface( IWindowSizeChangedEventArgs *iface, REFIID riid, void **out )
{
    struct size_event_args_impl *impl = impl_from_IWindowSizeChangedEventArgs( iface );
    if (!out) return E_POINTER;
    *out = NULL;
    if (IsEqualGUID( riid, &IID_IUnknown ) ||
        IsEqualGUID( riid, &IID_IInspectable ) ||
        IsEqualGUID( riid, &IID_IAgileObject ) ||
        IsEqualGUID( riid, &IID_IWindowSizeChangedEventArgs ))
    {
        *out = &impl->IWindowSizeChangedEventArgs_iface;
        IInspectable_AddRef( (IInspectable *)*out );
        return S_OK;
    }
    if (IsEqualGUID( riid, &IID_ICoreWindowEventArgs ))
    {
        *out = &impl->ICoreWindowEventArgs_iface;
        IInspectable_AddRef( (IInspectable *)&impl->IWindowSizeChangedEventArgs_iface );
        return S_OK;
    }
    return E_NOINTERFACE;
}

static ULONG WINAPI sargs_AddRef( IWindowSizeChangedEventArgs *iface )
{
    struct size_event_args_impl *impl = impl_from_IWindowSizeChangedEventArgs( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI sargs_Release( IWindowSizeChangedEventArgs *iface )
{
    struct size_event_args_impl *impl = impl_from_IWindowSizeChangedEventArgs( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI sargs_GetIids( IWindowSizeChangedEventArgs *iface, ULONG *count, IID **iids )
{
    if (count) *count = 0;
    return S_OK;
}

static HRESULT WINAPI sargs_GetRuntimeClassName( IWindowSizeChangedEventArgs *iface, HSTRING *name )
{
    return WindowsCreateString( L"Windows.UI.Core.WindowSizeChangedEventArgs", 41, name );
}

static HRESULT WINAPI sargs_GetTrustLevel( IWindowSizeChangedEventArgs *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_POINTER;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI sargs_get_Size( IWindowSizeChangedEventArgs *iface, Size *value )
{
    struct size_event_args_impl *impl = impl_from_IWindowSizeChangedEventArgs( iface );
    if (!value) return E_POINTER;
    *value = impl->size;
    return S_OK;
}

static const struct IWindowSizeChangedEventArgsVtbl size_event_args_vtbl =
{
    sargs_QueryInterface,
    sargs_AddRef,
    sargs_Release,
    sargs_GetIids,
    sargs_GetRuntimeClassName,
    sargs_GetTrustLevel,
    sargs_get_Size,
};

static HRESULT WINAPI sargs_core_QueryInterface( ICoreWindowEventArgs *iface, REFIID riid, void **out )
{
    struct size_event_args_impl *impl = impl_from_ICoreWindowEventArgs_s( iface );
    return sargs_QueryInterface( &impl->IWindowSizeChangedEventArgs_iface, riid, out );
}

static ULONG WINAPI sargs_core_AddRef( ICoreWindowEventArgs *iface )
{
    struct size_event_args_impl *impl = impl_from_ICoreWindowEventArgs_s( iface );
    return sargs_AddRef( &impl->IWindowSizeChangedEventArgs_iface );
}

static ULONG WINAPI sargs_core_Release( ICoreWindowEventArgs *iface )
{
    struct size_event_args_impl *impl = impl_from_ICoreWindowEventArgs_s( iface );
    return sargs_Release( &impl->IWindowSizeChangedEventArgs_iface );
}

static HRESULT WINAPI sargs_core_get_Handled( ICoreWindowEventArgs *iface, boolean *value )
{
    struct size_event_args_impl *impl = impl_from_ICoreWindowEventArgs_s( iface );
    if (!value) return E_POINTER;
    *value = impl->handled;
    return S_OK;
}

static HRESULT WINAPI sargs_core_put_Handled( ICoreWindowEventArgs *iface, boolean value )
{
    struct size_event_args_impl *impl = impl_from_ICoreWindowEventArgs_s( iface );
    impl->handled = value;
    return S_OK;
}

static const struct ICoreWindowEventArgsVtbl sargs_core_event_args_vtbl =
{
    sargs_core_QueryInterface,
    sargs_core_AddRef,
    sargs_core_Release,
    (void *)sargs_GetIids,
    (void *)sargs_GetRuntimeClassName,
    (void *)sargs_GetTrustLevel,
    sargs_core_get_Handled,
    sargs_core_put_Handled,
};

static HRESULT window_size_changed_event_args_create( FLOAT width, FLOAT height, IWindowSizeChangedEventArgs **out )
{
    struct size_event_args_impl *impl = calloc( 1, sizeof(*impl) );
    if (!impl) return E_OUTOFMEMORY;
    impl->IWindowSizeChangedEventArgs_iface.lpVtbl = &size_event_args_vtbl;
    impl->ICoreWindowEventArgs_iface.lpVtbl = &sargs_core_event_args_vtbl;
    impl->ref = 1;
    impl->size.Width = width;
    impl->size.Height = height;
    *out = &impl->IWindowSizeChangedEventArgs_iface;
    return S_OK;
}

/* VisibilityChangedEventArgs */
struct visibility_event_args_impl
{
    IVisibilityChangedEventArgs IVisibilityChangedEventArgs_iface;
    ICoreWindowEventArgs ICoreWindowEventArgs_iface;
    LONG ref;
    boolean handled;
    boolean visible;
};

#define impl_from_IVisibilityChangedEventArgs(ptr) ((struct visibility_event_args_impl *)((char *)(ptr) - offsetof(struct visibility_event_args_impl, IVisibilityChangedEventArgs_iface)))
#define impl_from_ICoreWindowEventArgs_v(ptr) ((struct visibility_event_args_impl *)((char *)(ptr) - offsetof(struct visibility_event_args_impl, ICoreWindowEventArgs_iface)))

static HRESULT WINAPI vargs_QueryInterface( IVisibilityChangedEventArgs *iface, REFIID riid, void **out )
{
    struct visibility_event_args_impl *impl = impl_from_IVisibilityChangedEventArgs( iface );
    if (!out) return E_POINTER;
    *out = NULL;
    if (IsEqualGUID( riid, &IID_IUnknown ) ||
        IsEqualGUID( riid, &IID_IInspectable ) ||
        IsEqualGUID( riid, &IID_IAgileObject ) ||
        IsEqualGUID( riid, &IID_IVisibilityChangedEventArgs ))
    {
        *out = &impl->IVisibilityChangedEventArgs_iface;
        IInspectable_AddRef( (IInspectable *)*out );
        return S_OK;
    }
    if (IsEqualGUID( riid, &IID_ICoreWindowEventArgs ))
    {
        *out = &impl->ICoreWindowEventArgs_iface;
        IInspectable_AddRef( (IInspectable *)&impl->IVisibilityChangedEventArgs_iface );
        return S_OK;
    }
    return E_NOINTERFACE;
}

static ULONG WINAPI vargs_AddRef( IVisibilityChangedEventArgs *iface )
{
    struct visibility_event_args_impl *impl = impl_from_IVisibilityChangedEventArgs( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI vargs_Release( IVisibilityChangedEventArgs *iface )
{
    struct visibility_event_args_impl *impl = impl_from_IVisibilityChangedEventArgs( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI vargs_GetIids( IVisibilityChangedEventArgs *iface, ULONG *count, IID **iids )
{
    if (count) *count = 0;
    return S_OK;
}

static HRESULT WINAPI vargs_GetRuntimeClassName( IVisibilityChangedEventArgs *iface, HSTRING *name )
{
    return WindowsCreateString( L"Windows.UI.Core.VisibilityChangedEventArgs", 41, name );
}

static HRESULT WINAPI vargs_GetTrustLevel( IVisibilityChangedEventArgs *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_POINTER;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI vargs_get_Visible( IVisibilityChangedEventArgs *iface, boolean *value )
{
    struct visibility_event_args_impl *impl = impl_from_IVisibilityChangedEventArgs( iface );
    if (!value) return E_POINTER;
    *value = impl->visible;
    return S_OK;
}

static const struct IVisibilityChangedEventArgsVtbl visibility_event_args_vtbl =
{
    vargs_QueryInterface,
    vargs_AddRef,
    vargs_Release,
    vargs_GetIids,
    vargs_GetRuntimeClassName,
    vargs_GetTrustLevel,
    vargs_get_Visible,
};

static HRESULT WINAPI vargs_core_QueryInterface( ICoreWindowEventArgs *iface, REFIID riid, void **out )
{
    struct visibility_event_args_impl *impl = impl_from_ICoreWindowEventArgs_v( iface );
    return vargs_QueryInterface( &impl->IVisibilityChangedEventArgs_iface, riid, out );
}

static ULONG WINAPI vargs_core_AddRef( ICoreWindowEventArgs *iface )
{
    struct visibility_event_args_impl *impl = impl_from_ICoreWindowEventArgs_v( iface );
    return vargs_AddRef( &impl->IVisibilityChangedEventArgs_iface );
}

static ULONG WINAPI vargs_core_Release( ICoreWindowEventArgs *iface )
{
    struct visibility_event_args_impl *impl = impl_from_ICoreWindowEventArgs_v( iface );
    return vargs_Release( &impl->IVisibilityChangedEventArgs_iface );
}

static HRESULT WINAPI vargs_core_get_Handled( ICoreWindowEventArgs *iface, boolean *value )
{
    struct visibility_event_args_impl *impl = impl_from_ICoreWindowEventArgs_v( iface );
    if (!value) return E_POINTER;
    *value = impl->handled;
    return S_OK;
}

static HRESULT WINAPI vargs_core_put_Handled( ICoreWindowEventArgs *iface, boolean value )
{
    struct visibility_event_args_impl *impl = impl_from_ICoreWindowEventArgs_v( iface );
    impl->handled = value;
    return S_OK;
}

static const struct ICoreWindowEventArgsVtbl vargs_core_event_args_vtbl =
{
    vargs_core_QueryInterface,
    vargs_core_AddRef,
    vargs_core_Release,
    (void *)vargs_GetIids,
    (void *)vargs_GetRuntimeClassName,
    (void *)vargs_GetTrustLevel,
    vargs_core_get_Handled,
    vargs_core_put_Handled,
};

static HRESULT visibility_changed_event_args_create( boolean visible, IVisibilityChangedEventArgs **out )
{
    struct visibility_event_args_impl *impl = calloc( 1, sizeof(*impl) );
    if (!impl) return E_OUTOFMEMORY;
    impl->IVisibilityChangedEventArgs_iface.lpVtbl = &visibility_event_args_vtbl;
    impl->ICoreWindowEventArgs_iface.lpVtbl = &vargs_core_event_args_vtbl;
    impl->ref = 1;
    impl->visible = visible;
    *out = &impl->IVisibilityChangedEventArgs_iface;
    return S_OK;
}
