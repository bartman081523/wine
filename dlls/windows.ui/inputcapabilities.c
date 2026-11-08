/* WinRT Windows.Devices.Input Capabilities Implementation */

static const GUID IID_IKeyboardCapabilities = {0x3a3f9b56, 0x6798, 0x4bbc, {0x83, 0x3e, 0x0f, 0x34, 0xb1, 0x7c, 0x65, 0xff}};
static const GUID IID_IMouseCapabilities = {0xbca5e023, 0x7dd9, 0x4b6b, {0x9a, 0x92, 0x55, 0xd4, 0x3c, 0xb3, 0x8f, 0x73}};
static const GUID IID_ITouchCapabilities = {0x20dd55f9, 0x13f1, 0x46c8, {0x92, 0x85, 0x2c, 0x05, 0xfa, 0x3e, 0xda, 0x6f}};

#define RuntimeClass_Windows_Devices_Input_KeyboardCapabilities L"Windows.Devices.Input.KeyboardCapabilities"
#define RuntimeClass_Windows_Devices_Input_MouseCapabilities L"Windows.Devices.Input.MouseCapabilities"
#define RuntimeClass_Windows_Devices_Input_TouchCapabilities L"Windows.Devices.Input.TouchCapabilities"

/* KeyboardCapabilities */
struct kb_caps
{
    struct { const void *lpVtbl; } IKeyboardCapabilities_iface;
    LONG ref;
};

#define impl_from_kb_caps(ptr) ((struct kb_caps *)((char *)(ptr) - offsetof(struct kb_caps, IKeyboardCapabilities_iface)))

static HRESULT WINAPI kb_caps_QI(void *This, REFIID riid, void **ppvObject)
{
    struct kb_caps *impl = impl_from_kb_caps(This);
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IKeyboardCapabilities))
    {
        *ppvObject = &impl->IKeyboardCapabilities_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI kb_caps_AddRef(void *This) { return InterlockedIncrement(&impl_from_kb_caps(This)->ref); }
static ULONG WINAPI kb_caps_Release(void *This)
{
    struct kb_caps *impl = impl_from_kb_caps(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}
static HRESULT WINAPI kb_caps_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI kb_caps_GetRuntimeClassName(void *This, HSTRING *n)
{
    return WindowsCreateString(RuntimeClass_Windows_Devices_Input_KeyboardCapabilities, wcslen(RuntimeClass_Windows_Devices_Input_KeyboardCapabilities), n);
}
static HRESULT WINAPI kb_caps_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI kb_caps_get_KeyboardPresent(void *This, int *value)
{
    TRACE("get_KeyboardPresent %p\n", value);
    if (value) *value = 1;
    return S_OK;
}

static const void *kb_caps_vtbl[] = {
    (void *)kb_caps_QI,
    (void *)kb_caps_AddRef,
    (void *)kb_caps_Release,
    (void *)kb_caps_GetIids,
    (void *)kb_caps_GetRuntimeClassName,
    (void *)kb_caps_GetTrustLevel,
    (void *)kb_caps_get_KeyboardPresent
};

/* Factory for KeyboardCapabilities */
static HRESULT WINAPI kb_fac_QI(IActivationFactory *iface, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppvObject = iface;
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI kb_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI kb_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI kb_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI kb_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n)
{
    return WindowsCreateString(RuntimeClass_Windows_Devices_Input_KeyboardCapabilities, wcslen(RuntimeClass_Windows_Devices_Input_KeyboardCapabilities), n);
}
static HRESULT WINAPI kb_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI kb_fac_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    struct kb_caps *inst = calloc(1, sizeof(*inst));
    if (!inst) return E_OUTOFMEMORY;
    inst->IKeyboardCapabilities_iface.lpVtbl = kb_caps_vtbl;
    inst->ref = 1;
    *instance = (IInspectable *)&inst->IKeyboardCapabilities_iface;
    return S_OK;
}

static const IActivationFactoryVtbl kb_caps_factory_vtbl = {
    kb_fac_QI,
    kb_fac_AddRef,
    kb_fac_Release,
    kb_fac_GetIids,
    kb_fac_GetRuntimeClassName,
    kb_fac_GetTrustLevel,
    kb_fac_ActivateInstance
};

static struct {
    const IActivationFactoryVtbl *lpVtbl;
} default_kb_factory = { &kb_caps_factory_vtbl };

IActivationFactory *keyboard_capabilities_factory = (IActivationFactory *)&default_kb_factory;


/* MouseCapabilities */
struct mouse_caps
{
    struct { const void *lpVtbl; } IMouseCapabilities_iface;
    LONG ref;
};

#define impl_from_mouse_caps(ptr) ((struct mouse_caps *)((char *)(ptr) - offsetof(struct mouse_caps, IMouseCapabilities_iface)))

static HRESULT WINAPI mouse_caps_QI(void *This, REFIID riid, void **ppvObject)
{
    struct mouse_caps *impl = impl_from_mouse_caps(This);
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IMouseCapabilities))
    {
        *ppvObject = &impl->IMouseCapabilities_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI mouse_caps_AddRef(void *This) { return InterlockedIncrement(&impl_from_mouse_caps(This)->ref); }
static ULONG WINAPI mouse_caps_Release(void *This)
{
    struct mouse_caps *impl = impl_from_mouse_caps(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}
static HRESULT WINAPI mouse_caps_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI mouse_caps_GetRuntimeClassName(void *This, HSTRING *n)
{
    return WindowsCreateString(RuntimeClass_Windows_Devices_Input_MouseCapabilities, wcslen(RuntimeClass_Windows_Devices_Input_MouseCapabilities), n);
}
static HRESULT WINAPI mouse_caps_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI mouse_caps_get_MousePresent(void *This, int *value)
{
    TRACE("get_MousePresent %p\n", value);
    if (value) *value = 1;
    return S_OK;
}
static HRESULT WINAPI mouse_caps_get_VerticalWheelPresent(void *This, int *value)
{
    if (value) *value = 1;
    return S_OK;
}
static HRESULT WINAPI mouse_caps_get_HorizontalWheelPresent(void *This, int *value)
{
    if (value) *value = 0;
    return S_OK;
}
static HRESULT WINAPI mouse_caps_get_SwapButtons(void *This, int *value)
{
    if (value) *value = 0;
    return S_OK;
}
static HRESULT WINAPI mouse_caps_get_NumberOfButtons(void *This, UINT32 *value)
{
    if (value) *value = 3;
    return S_OK;
}

static const void *mouse_caps_vtbl[] = {
    (void *)mouse_caps_QI,
    (void *)mouse_caps_AddRef,
    (void *)mouse_caps_Release,
    (void *)mouse_caps_GetIids,
    (void *)mouse_caps_GetRuntimeClassName,
    (void *)mouse_caps_GetTrustLevel,
    (void *)mouse_caps_get_MousePresent,
    (void *)mouse_caps_get_VerticalWheelPresent,
    (void *)mouse_caps_get_HorizontalWheelPresent,
    (void *)mouse_caps_get_SwapButtons,
    (void *)mouse_caps_get_NumberOfButtons
};

/* Factory for MouseCapabilities */
static HRESULT WINAPI mouse_fac_QI(IActivationFactory *iface, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppvObject = iface;
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI mouse_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI mouse_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI mouse_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI mouse_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n)
{
    return WindowsCreateString(RuntimeClass_Windows_Devices_Input_MouseCapabilities, wcslen(RuntimeClass_Windows_Devices_Input_MouseCapabilities), n);
}
static HRESULT WINAPI mouse_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI mouse_fac_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    struct mouse_caps *inst = calloc(1, sizeof(*inst));
    if (!inst) return E_OUTOFMEMORY;
    inst->IMouseCapabilities_iface.lpVtbl = mouse_caps_vtbl;
    inst->ref = 1;
    *instance = (IInspectable *)&inst->IMouseCapabilities_iface;
    return S_OK;
}

static const IActivationFactoryVtbl mouse_caps_factory_vtbl = {
    mouse_fac_QI,
    mouse_fac_AddRef,
    mouse_fac_Release,
    mouse_fac_GetIids,
    mouse_fac_GetRuntimeClassName,
    mouse_fac_GetTrustLevel,
    mouse_fac_ActivateInstance
};

static struct {
    const IActivationFactoryVtbl *lpVtbl;
} default_mouse_factory = { &mouse_caps_factory_vtbl };

IActivationFactory *mouse_capabilities_factory = (IActivationFactory *)&default_mouse_factory;


/* TouchCapabilities */
struct touch_caps
{
    struct { const void *lpVtbl; } ITouchCapabilities_iface;
    LONG ref;
};

#define impl_from_touch_caps(ptr) ((struct touch_caps *)((char *)(ptr) - offsetof(struct touch_caps, ITouchCapabilities_iface)))

static HRESULT WINAPI touch_caps_QI(void *This, REFIID riid, void **ppvObject)
{
    struct touch_caps *impl = impl_from_touch_caps(This);
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_ITouchCapabilities))
    {
        *ppvObject = &impl->ITouchCapabilities_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI touch_caps_AddRef(void *This) { return InterlockedIncrement(&impl_from_touch_caps(This)->ref); }
static ULONG WINAPI touch_caps_Release(void *This)
{
    struct touch_caps *impl = impl_from_touch_caps(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}
static HRESULT WINAPI touch_caps_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI touch_caps_GetRuntimeClassName(void *This, HSTRING *n)
{
    return WindowsCreateString(RuntimeClass_Windows_Devices_Input_TouchCapabilities, wcslen(RuntimeClass_Windows_Devices_Input_TouchCapabilities), n);
}
static HRESULT WINAPI touch_caps_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI touch_caps_get_TouchPresent(void *This, int *value)
{
    TRACE("get_TouchPresent %p\n", value);
    if (value) *value = 0;
    return S_OK;
}
static HRESULT WINAPI touch_caps_get_Contacts(void *This, UINT32 *value)
{
    if (value) *value = 0;
    return S_OK;
}

static const void *touch_caps_vtbl[] = {
    (void *)touch_caps_QI,
    (void *)touch_caps_AddRef,
    (void *)touch_caps_Release,
    (void *)touch_caps_GetIids,
    (void *)touch_caps_GetRuntimeClassName,
    (void *)touch_caps_GetTrustLevel,
    (void *)touch_caps_get_TouchPresent,
    (void *)touch_caps_get_Contacts
};

/* Factory for TouchCapabilities */
static HRESULT WINAPI touch_fac_QI(IActivationFactory *iface, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppvObject = iface;
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI touch_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI touch_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI touch_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI touch_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n)
{
    return WindowsCreateString(RuntimeClass_Windows_Devices_Input_TouchCapabilities, wcslen(RuntimeClass_Windows_Devices_Input_TouchCapabilities), n);
}
static HRESULT WINAPI touch_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI touch_fac_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    struct touch_caps *inst = calloc(1, sizeof(*inst));
    if (!inst) return E_OUTOFMEMORY;
    inst->ITouchCapabilities_iface.lpVtbl = touch_caps_vtbl;
    inst->ref = 1;
    *instance = (IInspectable *)&inst->ITouchCapabilities_iface;
    return S_OK;
}

static const IActivationFactoryVtbl touch_caps_factory_vtbl = {
    touch_fac_QI,
    touch_fac_AddRef,
    touch_fac_Release,
    touch_fac_GetIids,
    touch_fac_GetRuntimeClassName,
    touch_fac_GetTrustLevel,
    touch_fac_ActivateInstance
};

static struct {
    const IActivationFactoryVtbl *lpVtbl;
} default_touch_factory = { &touch_caps_factory_vtbl };

IActivationFactory *touch_capabilities_factory = (IActivationFactory *)&default_touch_factory;

/* =========================================================================
 * Windows.Devices.Input.MouseDevice
 * ========================================================================= */
static const GUID IID_IMouseDeviceStatics = {0x484a9045, 0x6d70, 0x49db, {0x8e, 0x68, 0x46, 0xff, 0xbd, 0x17, 0xd3, 0x8d}};
static const GUID IID_IMouseDevice = {0x79b9d5f2, 0x879e, 0x4b89, {0xb7, 0x98, 0x79, 0xe4, 0x75, 0x98, 0x03, 0x0c}};
static const GUID IID_IMouseEventArgs = {0xf144d9ec, 0x8833, 0x4f0e, {0xad, 0xfa, 0xe8, 0x0f, 0x4d, 0xb2, 0x39, 0x9d}};
#define RuntimeClass_Windows_Devices_Input_MouseDevice L"Windows.Devices.Input.MouseDevice"
#define RuntimeClass_Windows_Devices_Input_MouseEventArgs L"Windows.Devices.Input.MouseEventArgs"

typedef struct MouseDelta
{
    INT32 X;
    INT32 Y;
} MouseDelta;

struct mouse_event_args
{
    struct { const void *lpVtbl; } IMouseEventArgs_iface;
    LONG ref;
    MouseDelta delta;
};

#define impl_from_mouse_event_args(ptr) ((struct mouse_event_args *)((char *)(ptr) - offsetof(struct mouse_event_args, IMouseEventArgs_iface)))

static HRESULT WINAPI mouse_event_args_QI(void *This, REFIID riid, void **ppvObject)
{
    struct mouse_event_args *impl = impl_from_mouse_event_args(This);
    TRACE("mouse_event_args_QI %s\n", debugstr_guid(riid));
    if (!ppvObject) return E_POINTER;
    *ppvObject = &impl->IMouseEventArgs_iface;
    InterlockedIncrement(&impl->ref);
    return S_OK;
}
static ULONG WINAPI mouse_event_args_AddRef(void *This) { return InterlockedIncrement(&impl_from_mouse_event_args(This)->ref); }
static ULONG WINAPI mouse_event_args_Release(void *This)
{
    struct mouse_event_args *impl = impl_from_mouse_event_args(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}
static HRESULT WINAPI mouse_event_args_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI mouse_event_args_GetRuntimeClassName(void *This, HSTRING *n)
{
    return WindowsCreateString(RuntimeClass_Windows_Devices_Input_MouseEventArgs, wcslen(RuntimeClass_Windows_Devices_Input_MouseEventArgs), n);
}
static HRESULT WINAPI mouse_event_args_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI mouse_event_args_get_MouseDelta(void *This, MouseDelta *value)
{
    struct mouse_event_args *impl = impl_from_mouse_event_args(This);
    TRACE("mouse_event_args_get_MouseDelta (%d,%d)\n", impl->delta.X, impl->delta.Y);
    if (!value) return E_POINTER;
    *value = impl->delta;
    return S_OK;
}

static const void *mouse_event_args_vtbl[] = {
    (void *)mouse_event_args_QI,
    (void *)mouse_event_args_AddRef,
    (void *)mouse_event_args_Release,
    (void *)mouse_event_args_GetIids,
    (void *)mouse_event_args_GetRuntimeClassName,
    (void *)mouse_event_args_GetTrustLevel,
    (void *)mouse_event_args_get_MouseDelta
};

static HRESULT mouse_event_args_create(INT32 dx, INT32 dy, void **out)
{
    struct mouse_event_args *impl = calloc(1, sizeof(*impl));
    if (!impl) return E_OUTOFMEMORY;
    impl->IMouseEventArgs_iface.lpVtbl = mouse_event_args_vtbl;
    impl->ref = 1;
    impl->delta.X = dx;
    impl->delta.Y = dy;
    *out = &impl->IMouseEventArgs_iface;
    return S_OK;
}

struct mouse_device
{
    struct { const void *lpVtbl; } IMouseDevice_iface;
    LONG ref;
    void *mouse_moved_handler;
};

static struct mouse_device *active_mouse_device = NULL;
static struct mouse_device current_view_mouse_device;
static BOOL mouse_device_initialized = FALSE;
static const void *mouse_dev_vtbl[];

static void init_current_view_mouse_device(void)
{
    if (!mouse_device_initialized)
    {
        current_view_mouse_device.IMouseDevice_iface.lpVtbl = mouse_dev_vtbl;
        current_view_mouse_device.ref = 1;
        current_view_mouse_device.mouse_moved_handler = NULL;
        active_mouse_device = &current_view_mouse_device;
        mouse_device_initialized = TRUE;
    }
}

#define impl_from_mouse_device(ptr) ((struct mouse_device *)((char *)(ptr) - offsetof(struct mouse_device, IMouseDevice_iface)))

static HRESULT WINAPI mouse_dev_QI(void *This, REFIID riid, void **ppvObject)
{
    struct mouse_device *impl = impl_from_mouse_device(This);
    TRACE("mouse_dev_QI %s\n", debugstr_guid(riid));
    if (!ppvObject) return E_POINTER;
    *ppvObject = &impl->IMouseDevice_iface;
    InterlockedIncrement(&impl->ref);
    return S_OK;
}
static ULONG WINAPI mouse_dev_AddRef(void *This) { return InterlockedIncrement(&impl_from_mouse_device(This)->ref); }
static ULONG WINAPI mouse_dev_Release(void *This)
{
    struct mouse_device *impl = impl_from_mouse_device(This);
    if (impl == &current_view_mouse_device)
    {
        LONG ref = InterlockedDecrement(&impl->ref);
        if (ref < 1) impl->ref = 1;
        return impl->ref;
    }
    else
    {
        ULONG ref = InterlockedDecrement(&impl->ref);
        if (!ref)
        {
            if (impl->mouse_moved_handler)
                IInspectable_Release((IInspectable *)impl->mouse_moved_handler);
            if (active_mouse_device == impl) active_mouse_device = NULL;
            free(impl);
        }
        return ref;
    }
}
static HRESULT WINAPI mouse_dev_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI mouse_dev_GetRuntimeClassName(void *This, HSTRING *n)
{
    return WindowsCreateString(RuntimeClass_Windows_Devices_Input_MouseDevice, wcslen(RuntimeClass_Windows_Devices_Input_MouseDevice), n);
}
static HRESULT WINAPI mouse_dev_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI mouse_dev_add_MouseMoved(void *This, void *handler, EventRegistrationToken *cookie)
{
    struct mouse_device *impl = impl_from_mouse_device(This);
    TRACE("mouse_dev add_MouseMoved handler %p\n", handler);
    if (impl->mouse_moved_handler)
        IInspectable_Release((IInspectable *)impl->mouse_moved_handler);
    impl->mouse_moved_handler = handler;
    if (handler)
        IInspectable_AddRef((IInspectable *)handler);
    active_mouse_device = impl;
    if (cookie) cookie->value = (UINT_PTR)handler;
    return S_OK;
}
static HRESULT WINAPI mouse_dev_remove_MouseMoved(void *This, EventRegistrationToken cookie)
{
    struct mouse_device *impl = impl_from_mouse_device(This);
    TRACE("mouse_dev remove_MouseMoved %#I64x\n", cookie.value);
    if (impl->mouse_moved_handler)
    {
        IInspectable_Release((IInspectable *)impl->mouse_moved_handler);
        impl->mouse_moved_handler = NULL;
    }
    return S_OK;
}

void mouse_device_dispatch_move(int dx, int dy)
{
    struct mouse_device *dev = current_view_mouse_device.mouse_moved_handler ?
                               &current_view_mouse_device : active_mouse_device;
    if (dev && dev->mouse_moved_handler)
    {
        void *args = NULL;
        TRACE("mouse_device_dispatch_move dx=%d, dy=%d, handler=%p\n", dx, dy, dev->mouse_moved_handler);
        if (SUCCEEDED(mouse_event_args_create(dx, dy, &args)) && args)
        {
            typedef HRESULT (WINAPI *p_Invoke)(void *, void *, void *);
            p_Invoke invoke = *(p_Invoke *)(*(void ***)dev->mouse_moved_handler + 3);
            HRESULT hr = invoke(dev->mouse_moved_handler, &dev->IMouseDevice_iface, args);
            TRACE("mouse_device_dispatch_move invoke returned %#lx\n", hr);
            IInspectable_Release((IInspectable *)args);
        }
    }
}

BOOL mouse_device_has_listener(void)
{
    return (current_view_mouse_device.mouse_moved_handler != NULL) ||
           (active_mouse_device != NULL && active_mouse_device->mouse_moved_handler != NULL);
}

static const void *mouse_dev_vtbl[] = {
    (void *)mouse_dev_QI,
    (void *)mouse_dev_AddRef,
    (void *)mouse_dev_Release,
    (void *)mouse_dev_GetIids,
    (void *)mouse_dev_GetRuntimeClassName,
    (void *)mouse_dev_GetTrustLevel,
    (void *)mouse_dev_add_MouseMoved,
    (void *)mouse_dev_remove_MouseMoved
};

static HRESULT mouse_device_create(void **out)
{
    struct mouse_device *inst = calloc(1, sizeof(*inst));
    if (!inst) return E_OUTOFMEMORY;
    inst->IMouseDevice_iface.lpVtbl = mouse_dev_vtbl;
    inst->ref = 1;
    *out = &inst->IMouseDevice_iface;
    return S_OK;
}

/* MouseDevice Factory / Statics */
static HRESULT WINAPI mouse_dev_fac_QI(IActivationFactory *iface, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IActivationFactory) ||
        IsEqualGUID(riid, &IID_IMouseDeviceStatics))
    {
        *ppvObject = iface;
        return S_OK;
    }
    TRACE("mouse_dev_fac %p unhandled QI %s\n", iface, debugstr_guid(riid));
    return E_NOINTERFACE;
}
static ULONG WINAPI mouse_dev_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI mouse_dev_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI mouse_dev_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI mouse_dev_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n)
{
    return WindowsCreateString(RuntimeClass_Windows_Devices_Input_MouseDevice, wcslen(RuntimeClass_Windows_Devices_Input_MouseDevice), n);
}
static HRESULT WINAPI mouse_dev_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI mouse_dev_fac_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    return mouse_device_create((void **)instance);
}
static HRESULT WINAPI mouse_dev_fac_GetForCurrentView(void *This, void **value)
{
    TRACE("mouse_dev_fac GetForCurrentView\n");
    if (!value) return E_POINTER;
    init_current_view_mouse_device();
    InterlockedIncrement(&current_view_mouse_device.ref);
    *value = &current_view_mouse_device.IMouseDevice_iface;
    return S_OK;
}

static const void *mouse_dev_factory_vtbl[] = {
    (void *)mouse_dev_fac_QI,
    (void *)mouse_dev_fac_AddRef,
    (void *)mouse_dev_fac_Release,
    (void *)mouse_dev_fac_GetIids,
    (void *)mouse_dev_fac_GetRuntimeClassName,
    (void *)mouse_dev_fac_GetTrustLevel,
    (void *)mouse_dev_fac_GetForCurrentView
};

static struct {
    const void *lpVtbl;
} default_mouse_device_factory = { mouse_dev_factory_vtbl };

IActivationFactory *mouse_device_factory = (IActivationFactory *)&default_mouse_device_factory;
