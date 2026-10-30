/* WinRT Windows.UI.Core.SystemNavigationManager Implementation */

static const GUID IID_ISystemNavigationManager = {0x93023151, 0x4130, 0x4270, {0x83, 0x48, 0x14, 0x23, 0xfa, 0x96, 0xd3, 0x9e}};
static const GUID IID_ISystemNavigationManager2 = {0x677c5787, 0x8a74, 0x4c5c, {0x9a, 0x52, 0x09, 0x43, 0xa5, 0xce, 0xf0, 0x40}};
static const GUID IID_ISystemNavigationManagerStatics = {0xdc52b5ce, 0xbee0, 0x4305, {0x8c, 0x54, 0x68, 0x22, 0x8e, 0xd6, 0x83, 0xb5}};

#define RuntimeClass_Windows_UI_Core_SystemNavigationManager L"Windows.UI.Core.SystemNavigationManager"

struct system_nav_manager
{
    struct { const void *lpVtbl; } ISystemNavigationManager_iface;
    struct { const void *lpVtbl; } ISystemNavigationManager2_iface;
    LONG ref;
};

#define impl_from_sys_nav(ptr) ((struct system_nav_manager *)((char *)(ptr) - offsetof(struct system_nav_manager, ISystemNavigationManager_iface)))
#define impl_from_sys_nav2(ptr) ((struct system_nav_manager *)((char *)(ptr) - offsetof(struct system_nav_manager, ISystemNavigationManager2_iface)))

static HRESULT sys_nav_QI_internal(struct system_nav_manager *impl, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_ISystemNavigationManager))
    {
        *ppvObject = &impl->ISystemNavigationManager_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ISystemNavigationManager2))
    {
        *ppvObject = &impl->ISystemNavigationManager2_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    TRACE("sys_nav %p: unhandled QI %s\n", impl, debugstr_guid(riid));
    return E_NOINTERFACE;
}

static HRESULT WINAPI sys_nav_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return sys_nav_QI_internal(impl_from_sys_nav(This), riid, ppvObject);
}
static ULONG WINAPI sys_nav_AddRef(void *This)
{
    struct system_nav_manager *impl = impl_from_sys_nav(This);
    return InterlockedIncrement(&impl->ref);
}
static ULONG WINAPI sys_nav_Release(void *This)
{
    struct system_nav_manager *impl = impl_from_sys_nav(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}
static HRESULT WINAPI sys_nav_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI sys_nav_GetRuntimeClassName(void *This, HSTRING *n)
{
    return WindowsCreateString(RuntimeClass_Windows_UI_Core_SystemNavigationManager, wcslen(RuntimeClass_Windows_UI_Core_SystemNavigationManager), n);
}
static HRESULT WINAPI sys_nav_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI sys_nav_add_BackRequested(void *This, void *handler, EventRegistrationToken *token)
{
    TRACE("sys_nav add_BackRequested handler %p\n", handler);
    if (token) token->value = 1;
    return S_OK;
}

static HRESULT WINAPI sys_nav_remove_BackRequested(void *This, EventRegistrationToken token)
{
    TRACE("sys_nav remove_BackRequested token %I64u\n", token.value);
    return S_OK;
}

static const void *sys_nav_vtbl[] = {
    (void *)sys_nav_QueryInterface,
    (void *)sys_nav_AddRef,
    (void *)sys_nav_Release,
    (void *)sys_nav_GetIids,
    (void *)sys_nav_GetRuntimeClassName,
    (void *)sys_nav_GetTrustLevel,
    (void *)sys_nav_add_BackRequested,
    (void *)sys_nav_remove_BackRequested
};

static HRESULT WINAPI sys_nav2_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return sys_nav_QI_internal(impl_from_sys_nav2(This), riid, ppvObject);
}
static ULONG WINAPI sys_nav2_AddRef(void *This)
{
    return sys_nav_AddRef(&impl_from_sys_nav2(This)->ISystemNavigationManager_iface);
}
static ULONG WINAPI sys_nav2_Release(void *This)
{
    return sys_nav_Release(&impl_from_sys_nav2(This)->ISystemNavigationManager_iface);
}

static HRESULT WINAPI sys_nav2_get_AppViewBackButtonVisibility(void *This, int *value)
{
    TRACE("get_AppViewBackButtonVisibility %p\n", value);
    if (value) *value = 1; /* AppViewBackButtonVisibility_Collapsed = 1 */
    return S_OK;
}

static HRESULT WINAPI sys_nav2_put_AppViewBackButtonVisibility(void *This, int value)
{
    TRACE("put_AppViewBackButtonVisibility %d\n", value);
    return S_OK;
}

static const void *sys_nav2_vtbl[] = {
    (void *)sys_nav2_QueryInterface,
    (void *)sys_nav2_AddRef,
    (void *)sys_nav2_Release,
    (void *)sys_nav_GetIids,
    (void *)sys_nav_GetRuntimeClassName,
    (void *)sys_nav_GetTrustLevel,
    (void *)sys_nav2_get_AppViewBackButtonVisibility,
    (void *)sys_nav2_put_AppViewBackButtonVisibility
};

/* Factory & Statics */
struct sys_nav_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } ISystemNavigationManagerStatics_iface;
    LONG ref;
};

#define impl_from_sys_nav_fac(ptr) ((struct sys_nav_factory_impl *)((char *)(ptr) - offsetof(struct sys_nav_factory_impl, IActivationFactory_iface)))
#define impl_from_sys_nav_stat(ptr) ((struct sys_nav_factory_impl *)((char *)(ptr) - offsetof(struct sys_nav_factory_impl, ISystemNavigationManagerStatics_iface)))

static HRESULT sys_nav_fac_QI_internal(struct sys_nav_factory_impl *impl, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppvObject = &impl->IActivationFactory_iface;
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ISystemNavigationManagerStatics))
    {
        *ppvObject = &impl->ISystemNavigationManagerStatics_iface;
        return S_OK;
    }

    TRACE("sys_nav_fac %p: unhandled QI %s\n", impl, debugstr_guid(riid));
    return E_NOINTERFACE;
}

static HRESULT WINAPI sys_nav_fac_QueryInterface(IActivationFactory *iface, REFIID riid, void **ppvObject)
{
    return sys_nav_fac_QI_internal(impl_from_sys_nav_fac(iface), riid, ppvObject);
}
static ULONG WINAPI sys_nav_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI sys_nav_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI sys_nav_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI sys_nav_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n)
{
    return WindowsCreateString(RuntimeClass_Windows_UI_Core_SystemNavigationManager, wcslen(RuntimeClass_Windows_UI_Core_SystemNavigationManager), n);
}
static HRESULT WINAPI sys_nav_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI sys_nav_fac_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    struct system_nav_manager *inst = calloc(1, sizeof(*inst));
    if (!inst) return E_OUTOFMEMORY;
    inst->ISystemNavigationManager_iface.lpVtbl = sys_nav_vtbl;
    inst->ISystemNavigationManager2_iface.lpVtbl = sys_nav2_vtbl;
    inst->ref = 1;
    *instance = (IInspectable *)&inst->ISystemNavigationManager_iface;
    return S_OK;
}

static const IActivationFactoryVtbl sys_nav_activation_factory_vtbl = {
    sys_nav_fac_QueryInterface,
    sys_nav_fac_AddRef,
    sys_nav_fac_Release,
    sys_nav_fac_GetIids,
    sys_nav_fac_GetRuntimeClassName,
    sys_nav_fac_GetTrustLevel,
    sys_nav_fac_ActivateInstance
};

static HRESULT WINAPI sys_nav_stat_QI(void *This, REFIID riid, void **ppvObject)
{
    return sys_nav_fac_QI_internal(impl_from_sys_nav_stat(This), riid, ppvObject);
}
static ULONG WINAPI sys_nav_stat_AddRef(void *This) { return 2; }
static ULONG WINAPI sys_nav_stat_Release(void *This) { return 1; }
static HRESULT WINAPI sys_nav_stat_GetForCurrentView(void *This, void **loader)
{
    struct system_nav_manager *inst = calloc(1, sizeof(*inst));
    if (!inst) return E_OUTOFMEMORY;
    inst->ISystemNavigationManager_iface.lpVtbl = sys_nav_vtbl;
    inst->ISystemNavigationManager2_iface.lpVtbl = sys_nav2_vtbl;
    inst->ref = 1;
    *loader = &inst->ISystemNavigationManager_iface;
    TRACE("SystemNavigationManager GetForCurrentView -> %p\n", *loader);
    return S_OK;
}

static const void *sys_nav_statics_vtbl[] = {
    (void *)sys_nav_stat_QI,
    (void *)sys_nav_stat_AddRef,
    (void *)sys_nav_stat_Release,
    (void *)sys_nav_fac_GetIids,
    (void *)sys_nav_fac_GetRuntimeClassName,
    (void *)sys_nav_fac_GetTrustLevel,
    (void *)sys_nav_stat_GetForCurrentView
};

static struct sys_nav_factory_impl default_sys_nav_factory = {
    {&sys_nav_activation_factory_vtbl},
    {sys_nav_statics_vtbl},
    1
};

IActivationFactory *system_navigation_manager_factory = &default_sys_nav_factory.IActivationFactory_iface;

/* WinRT Windows.UI.Core.Preview.SystemNavigationManagerPreview Implementation */

static const GUID IID_ISystemNavigationManagerPreview = {0xec5f0488, 0x6425, 0x4777, {0xa5, 0x36, 0xcb, 0x56, 0x34, 0x42, 0x7f, 0x0d}};
static const GUID IID_ISystemNavigationManagerPreviewStatics = {0x0e971360, 0xdf74, 0x4bce, {0x84, 0xcb, 0xbd, 0x11, 0x81, 0xac, 0x0a, 0x71}};

#define RuntimeClass_Windows_UI_Core_Preview_SystemNavigationManagerPreview L"Windows.UI.Core.Preview.SystemNavigationManagerPreview"

struct system_nav_manager_preview
{
    struct { const void *lpVtbl; } ISystemNavigationManagerPreview_iface;
    LONG ref;
};

#define impl_from_sys_nav_prev(ptr) ((struct system_nav_manager_preview *)((char *)(ptr) - offsetof(struct system_nav_manager_preview, ISystemNavigationManagerPreview_iface)))

static HRESULT sys_nav_prev_QI_internal(struct system_nav_manager_preview *impl, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_ISystemNavigationManagerPreview))
    {
        *ppvObject = &impl->ISystemNavigationManagerPreview_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    TRACE("sys_nav_prev %p: unhandled QI %s\n", impl, debugstr_guid(riid));
    return E_NOINTERFACE;
}

static HRESULT WINAPI sys_nav_prev_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return sys_nav_prev_QI_internal(impl_from_sys_nav_prev(This), riid, ppvObject);
}
static ULONG WINAPI sys_nav_prev_AddRef(void *This)
{
    struct system_nav_manager_preview *impl = impl_from_sys_nav_prev(This);
    return InterlockedIncrement(&impl->ref);
}
static ULONG WINAPI sys_nav_prev_Release(void *This)
{
    struct system_nav_manager_preview *impl = impl_from_sys_nav_prev(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}
static HRESULT WINAPI sys_nav_prev_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI sys_nav_prev_GetRuntimeClassName(void *This, HSTRING *n)
{
    return WindowsCreateString(RuntimeClass_Windows_UI_Core_Preview_SystemNavigationManagerPreview, wcslen(RuntimeClass_Windows_UI_Core_Preview_SystemNavigationManagerPreview), n);
}
static HRESULT WINAPI sys_nav_prev_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI sys_nav_prev_add_CloseRequested(void *This, void *handler, EventRegistrationToken *token)
{
    TRACE("sys_nav_prev add_CloseRequested handler %p\n", handler);
    if (token) token->value = 1;
    return S_OK;
}

static HRESULT WINAPI sys_nav_prev_remove_CloseRequested(void *This, EventRegistrationToken token)
{
    TRACE("sys_nav_prev remove_CloseRequested token %I64u\n", token.value);
    return S_OK;
}

static const void *sys_nav_prev_vtbl[] = {
    (void *)sys_nav_prev_QueryInterface,
    (void *)sys_nav_prev_AddRef,
    (void *)sys_nav_prev_Release,
    (void *)sys_nav_prev_GetIids,
    (void *)sys_nav_prev_GetRuntimeClassName,
    (void *)sys_nav_prev_GetTrustLevel,
    (void *)sys_nav_prev_add_CloseRequested,
    (void *)sys_nav_prev_remove_CloseRequested
};

struct sys_nav_prev_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } ISystemNavigationManagerPreviewStatics_iface;
    LONG ref;
};

#define impl_from_sys_nav_prev_fac(ptr) ((struct sys_nav_prev_factory_impl *)((char *)(ptr) - offsetof(struct sys_nav_prev_factory_impl, IActivationFactory_iface)))
#define impl_from_sys_nav_prev_stat(ptr) ((struct sys_nav_prev_factory_impl *)((char *)(ptr) - offsetof(struct sys_nav_prev_factory_impl, ISystemNavigationManagerPreviewStatics_iface)))

static HRESULT sys_nav_prev_fac_QI_internal(struct sys_nav_prev_factory_impl *impl, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppvObject = &impl->IActivationFactory_iface;
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ISystemNavigationManagerPreviewStatics))
    {
        *ppvObject = &impl->ISystemNavigationManagerPreviewStatics_iface;
        return S_OK;
    }

    TRACE("sys_nav_prev_fac %p: unhandled QI %s\n", impl, debugstr_guid(riid));
    return E_NOINTERFACE;
}

static HRESULT WINAPI sys_nav_prev_fac_QueryInterface(IActivationFactory *iface, REFIID riid, void **ppvObject)
{
    return sys_nav_prev_fac_QI_internal(impl_from_sys_nav_prev_fac(iface), riid, ppvObject);
}
static ULONG WINAPI sys_nav_prev_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI sys_nav_prev_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI sys_nav_prev_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI sys_nav_prev_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n)
{
    return WindowsCreateString(RuntimeClass_Windows_UI_Core_Preview_SystemNavigationManagerPreview, wcslen(RuntimeClass_Windows_UI_Core_Preview_SystemNavigationManagerPreview), n);
}
static HRESULT WINAPI sys_nav_prev_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI sys_nav_prev_fac_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    struct system_nav_manager_preview *inst = calloc(1, sizeof(*inst));
    if (!inst) return E_OUTOFMEMORY;
    inst->ISystemNavigationManagerPreview_iface.lpVtbl = sys_nav_prev_vtbl;
    inst->ref = 1;
    *instance = (IInspectable *)&inst->ISystemNavigationManagerPreview_iface;
    return S_OK;
}

static const IActivationFactoryVtbl sys_nav_prev_activation_factory_vtbl = {
    sys_nav_prev_fac_QueryInterface,
    sys_nav_prev_fac_AddRef,
    sys_nav_prev_fac_Release,
    sys_nav_prev_fac_GetIids,
    sys_nav_prev_fac_GetRuntimeClassName,
    sys_nav_prev_fac_GetTrustLevel,
    sys_nav_prev_fac_ActivateInstance
};

static HRESULT WINAPI sys_nav_prev_stat_QI(void *This, REFIID riid, void **ppvObject)
{
    return sys_nav_prev_fac_QI_internal(impl_from_sys_nav_prev_stat(This), riid, ppvObject);
}
static ULONG WINAPI sys_nav_prev_stat_AddRef(void *This) { return 2; }
static ULONG WINAPI sys_nav_prev_stat_Release(void *This) { return 1; }
static HRESULT WINAPI sys_nav_prev_stat_GetForCurrentView(void *This, void **loader)
{
    struct system_nav_manager_preview *inst = calloc(1, sizeof(*inst));
    if (!inst) return E_OUTOFMEMORY;
    inst->ISystemNavigationManagerPreview_iface.lpVtbl = sys_nav_prev_vtbl;
    inst->ref = 1;
    *loader = &inst->ISystemNavigationManagerPreview_iface;
    TRACE("SystemNavigationManagerPreview GetForCurrentView -> %p\n", *loader);
    return S_OK;
}

static const void *sys_nav_prev_statics_vtbl[] = {
    (void *)sys_nav_prev_stat_QI,
    (void *)sys_nav_prev_stat_AddRef,
    (void *)sys_nav_prev_stat_Release,
    (void *)sys_nav_prev_fac_GetIids,
    (void *)sys_nav_prev_fac_GetRuntimeClassName,
    (void *)sys_nav_prev_fac_GetTrustLevel,
    (void *)sys_nav_prev_stat_GetForCurrentView
};

static struct sys_nav_prev_factory_impl default_sys_nav_prev_factory = {
    {&sys_nav_prev_activation_factory_vtbl},
    {sys_nav_prev_statics_vtbl},
    1
};

IActivationFactory *system_navigation_manager_preview_factory = &default_sys_nav_prev_factory.IActivationFactory_iface;
