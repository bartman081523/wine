/* WinRT Windows.Graphics.Display.DisplayInformation Implementation */

static const GUID IID_IDisplayInformation = {0xbed112ae, 0xadc3, 0x4dc9, {0xae, 0x65, 0x85, 0x1f, 0x4d, 0x7d, 0x47, 0x99}};
static const GUID IID_IDisplayInformation2 = {0x4dcd0021, 0xfad1, 0x4b8e, {0x8e, 0xdf, 0x77, 0x58, 0x87, 0xb8, 0xbf, 0x19}};
static const GUID IID_IDisplayInformation3 = {0xdb15011d, 0x0f09, 0x4466, {0x8f, 0xf3, 0x11, 0xde, 0x9a, 0x3c, 0x92, 0x9a}};
static const GUID IID_IDisplayInformation4 = {0xc972ce2f, 0x1242, 0x46be, {0xb5, 0x36, 0xe1, 0xaa, 0xfe, 0x9e, 0x7a, 0xcf}};
static const GUID IID_IDisplayInformationStatics = {0xc6a02a6c, 0xd452, 0x44dc, {0xba, 0x07, 0x96, 0xf3, 0xc6, 0xad, 0xf9, 0xd1}};

#define RuntimeClass_Windows_Graphics_Display_DisplayInformation L"Windows.Graphics.Display.DisplayInformation"

struct display_info
{
    struct { const void *lpVtbl; } IDisplayInformation_iface;
    struct { const void *lpVtbl; } IDisplayInformation2_iface;
    struct { const void *lpVtbl; } IDisplayInformation3_iface;
    struct { const void *lpVtbl; } IDisplayInformation4_iface;
    LONG ref;
};

#define impl_from_disp_info(ptr) ((struct display_info *)((char *)(ptr) - offsetof(struct display_info, IDisplayInformation_iface)))
#define impl_from_disp_info2(ptr) ((struct display_info *)((char *)(ptr) - offsetof(struct display_info, IDisplayInformation2_iface)))
#define impl_from_disp_info3(ptr) ((struct display_info *)((char *)(ptr) - offsetof(struct display_info, IDisplayInformation3_iface)))
#define impl_from_disp_info4(ptr) ((struct display_info *)((char *)(ptr) - offsetof(struct display_info, IDisplayInformation4_iface)))

static HRESULT disp_info_QI_internal(struct display_info *impl, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IDisplayInformation))
    {
        *ppvObject = &impl->IDisplayInformation_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IDisplayInformation2))
    {
        *ppvObject = &impl->IDisplayInformation2_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IDisplayInformation3))
    {
        *ppvObject = &impl->IDisplayInformation3_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IDisplayInformation4))
    {
        *ppvObject = &impl->IDisplayInformation4_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    TRACE("disp_info %p: unhandled QI %s\n", impl, debugstr_guid(riid));
    return E_NOINTERFACE;
}

static HRESULT WINAPI disp_info_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return disp_info_QI_internal(impl_from_disp_info(This), riid, ppvObject);
}
static ULONG WINAPI disp_info_AddRef(void *This)
{
    struct display_info *impl = impl_from_disp_info(This);
    return InterlockedIncrement(&impl->ref);
}
static ULONG WINAPI disp_info_Release(void *This)
{
    struct display_info *impl = impl_from_disp_info(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}
static HRESULT WINAPI disp_info_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI disp_info_GetRuntimeClassName(void *This, HSTRING *n)
{
    return WindowsCreateString(RuntimeClass_Windows_Graphics_Display_DisplayInformation, wcslen(RuntimeClass_Windows_Graphics_Display_DisplayInformation), n);
}
static HRESULT WINAPI disp_info_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI disp_info_get_CurrentOrientation(void *This, int *value)
{
    TRACE("get_CurrentOrientation %p\n", value);
    if (value) *value = 1; /* Landscape */
    return S_OK;
}
static HRESULT WINAPI disp_info_get_NativeOrientation(void *This, int *value)
{
    TRACE("get_NativeOrientation %p\n", value);
    if (value) *value = 1; /* Landscape */
    return S_OK;
}
static HRESULT WINAPI disp_info_add_OrientationChanged(void *This, void *h, EventRegistrationToken *t)
{
    if (t) t->value = 1;
    return S_OK;
}
static HRESULT WINAPI disp_info_remove_OrientationChanged(void *This, EventRegistrationToken t) { return S_OK; }
static HRESULT WINAPI disp_info_get_ResolutionScale(void *This, int *value)
{
    TRACE("get_ResolutionScale %p\n", value);
    if (value) *value = 100; /* Scale100Percent */
    return S_OK;
}
static HRESULT WINAPI disp_info_get_LogicalDpi(void *This, float *value)
{
    TRACE("get_LogicalDpi %p\n", value);
    if (value) *value = 96.0f;
    return S_OK;
}
static HRESULT WINAPI disp_info_get_RawDpiX(void *This, float *value)
{
    TRACE("get_RawDpiX %p\n", value);
    if (value) *value = 96.0f;
    return S_OK;
}
static HRESULT WINAPI disp_info_get_RawDpiY(void *This, float *value)
{
    TRACE("get_RawDpiY %p\n", value);
    if (value) *value = 96.0f;
    return S_OK;
}
static HRESULT WINAPI disp_info_add_DpiChanged(void *This, void *h, EventRegistrationToken *t)
{
    if (t) t->value = 1;
    return S_OK;
}
static HRESULT WINAPI disp_info_remove_DpiChanged(void *This, EventRegistrationToken t) { return S_OK; }
static HRESULT WINAPI disp_info_get_StereoEnabled(void *This, boolean *value)
{
    if (value) *value = FALSE;
    return S_OK;
}
static HRESULT WINAPI disp_info_add_StereoEnabledChanged(void *This, void *h, EventRegistrationToken *t)
{
    if (t) t->value = 1;
    return S_OK;
}
static HRESULT WINAPI disp_info_remove_StereoEnabledChanged(void *This, EventRegistrationToken t) { return S_OK; }
static HRESULT WINAPI disp_info_GetColorProfileAsync(void *This, void **async)
{
    if (async) *async = NULL;
    return S_OK;
}
static HRESULT WINAPI disp_info_add_ColorProfileChanged(void *This, void *h, EventRegistrationToken *t)
{
    if (t) t->value = 1;
    return S_OK;
}
static HRESULT WINAPI disp_info_remove_ColorProfileChanged(void *This, EventRegistrationToken t) { return S_OK; }

static const void *disp_info_vtbl[] = {
    (void *)disp_info_QueryInterface,
    (void *)disp_info_AddRef,
    (void *)disp_info_Release,
    (void *)disp_info_GetIids,
    (void *)disp_info_GetRuntimeClassName,
    (void *)disp_info_GetTrustLevel,
    (void *)disp_info_get_CurrentOrientation,
    (void *)disp_info_get_NativeOrientation,
    (void *)disp_info_add_OrientationChanged,
    (void *)disp_info_remove_OrientationChanged,
    (void *)disp_info_get_ResolutionScale,
    (void *)disp_info_get_LogicalDpi,
    (void *)disp_info_get_RawDpiX,
    (void *)disp_info_get_RawDpiY,
    (void *)disp_info_add_DpiChanged,
    (void *)disp_info_remove_DpiChanged,
    (void *)disp_info_get_StereoEnabled,
    (void *)disp_info_add_StereoEnabledChanged,
    (void *)disp_info_remove_StereoEnabledChanged,
    (void *)disp_info_GetColorProfileAsync,
    (void *)disp_info_add_ColorProfileChanged,
    (void *)disp_info_remove_ColorProfileChanged
};

static HRESULT WINAPI disp_info2_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return disp_info_QI_internal(impl_from_disp_info2(This), riid, ppvObject);
}
static ULONG WINAPI disp_info2_AddRef(void *This)
{
    return disp_info_AddRef(&impl_from_disp_info2(This)->IDisplayInformation_iface);
}
static ULONG WINAPI disp_info2_Release(void *This)
{
    return disp_info_Release(&impl_from_disp_info2(This)->IDisplayInformation_iface);
}
static HRESULT WINAPI disp_info2_get_RawPixelsPerViewPixel(void *This, double *value)
{
    TRACE("get_RawPixelsPerViewPixel %p\n", value);
    if (value) *value = 1.0;
    return S_OK;
}

static const void *disp_info2_vtbl[] = {
    (void *)disp_info2_QueryInterface,
    (void *)disp_info2_AddRef,
    (void *)disp_info2_Release,
    (void *)disp_info_GetIids,
    (void *)disp_info_GetRuntimeClassName,
    (void *)disp_info_GetTrustLevel,
    (void *)disp_info2_get_RawPixelsPerViewPixel
};

static HRESULT WINAPI disp_info3_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return disp_info_QI_internal(impl_from_disp_info3(This), riid, ppvObject);
}
static ULONG WINAPI disp_info3_AddRef(void *This)
{
    return disp_info_AddRef(&impl_from_disp_info3(This)->IDisplayInformation_iface);
}
static ULONG WINAPI disp_info3_Release(void *This)
{
    return disp_info_Release(&impl_from_disp_info3(This)->IDisplayInformation_iface);
}
static HRESULT WINAPI disp_info3_get_DiagonalSizeInInches(void *This, void **value)
{
    TRACE("get_DiagonalSizeInInches %p\n", value);
    if (!value) return E_POINTER;
    *value = NULL; /* null when unknown per WinRT spec */
    return S_OK;
}

static const void *disp_info3_vtbl[] = {
    (void *)disp_info3_QueryInterface,
    (void *)disp_info3_AddRef,
    (void *)disp_info3_Release,
    (void *)disp_info_GetIids,
    (void *)disp_info_GetRuntimeClassName,
    (void *)disp_info_GetTrustLevel,
    (void *)disp_info3_get_DiagonalSizeInInches
};

static HRESULT WINAPI disp_info4_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return disp_info_QI_internal(impl_from_disp_info4(This), riid, ppvObject);
}
static ULONG WINAPI disp_info4_AddRef(void *This)
{
    return disp_info_AddRef(&impl_from_disp_info4(This)->IDisplayInformation_iface);
}
static ULONG WINAPI disp_info4_Release(void *This)
{
    return disp_info_Release(&impl_from_disp_info4(This)->IDisplayInformation_iface);
}
static HRESULT WINAPI disp_info4_get_ScreenWidthInRawPixels(void *This, UINT32 *value)
{
    TRACE("get_ScreenWidthInRawPixels %p\n", value);
    if (!value) return E_POINTER;
    *value = GetSystemMetrics(SM_CXSCREEN);
    if (!*value) *value = 1920;
    return S_OK;
}
static HRESULT WINAPI disp_info4_get_ScreenHeightInRawPixels(void *This, UINT32 *value)
{
    TRACE("get_ScreenHeightInRawPixels %p\n", value);
    if (!value) return E_POINTER;
    *value = GetSystemMetrics(SM_CYSCREEN);
    if (!*value) *value = 1080;
    return S_OK;
}

static const void *disp_info4_vtbl[] = {
    (void *)disp_info4_QueryInterface,
    (void *)disp_info4_AddRef,
    (void *)disp_info4_Release,
    (void *)disp_info_GetIids,
    (void *)disp_info_GetRuntimeClassName,
    (void *)disp_info_GetTrustLevel,
    (void *)disp_info4_get_ScreenWidthInRawPixels,
    (void *)disp_info4_get_ScreenHeightInRawPixels
};

/* Factory & Statics */
struct disp_info_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } IDisplayInformationStatics_iface;
    LONG ref;
};

#define impl_from_disp_fac(ptr) ((struct disp_info_factory_impl *)((char *)(ptr) - offsetof(struct disp_info_factory_impl, IActivationFactory_iface)))
#define impl_from_disp_stat(ptr) ((struct disp_info_factory_impl *)((char *)(ptr) - offsetof(struct disp_info_factory_impl, IDisplayInformationStatics_iface)))

static HRESULT disp_info_fac_QI_internal(struct disp_info_factory_impl *impl, REFIID riid, void **ppvObject)
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

    if (IsEqualGUID(riid, &IID_IDisplayInformationStatics))
    {
        *ppvObject = &impl->IDisplayInformationStatics_iface;
        return S_OK;
    }

    TRACE("disp_info_fac %p: unhandled QI %s\n", impl, debugstr_guid(riid));
    return E_NOINTERFACE;
}

static HRESULT WINAPI disp_info_fac_QueryInterface(IActivationFactory *iface, REFIID riid, void **ppvObject)
{
    return disp_info_fac_QI_internal(impl_from_disp_fac(iface), riid, ppvObject);
}
static ULONG WINAPI disp_info_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI disp_info_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI disp_info_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI disp_info_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n)
{
    return WindowsCreateString(RuntimeClass_Windows_Graphics_Display_DisplayInformation, wcslen(RuntimeClass_Windows_Graphics_Display_DisplayInformation), n);
}
static HRESULT WINAPI disp_info_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI disp_info_fac_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    struct display_info *inst = calloc(1, sizeof(*inst));
    if (!inst) return E_OUTOFMEMORY;
    inst->IDisplayInformation_iface.lpVtbl = disp_info_vtbl;
    inst->IDisplayInformation2_iface.lpVtbl = disp_info2_vtbl;
    inst->IDisplayInformation3_iface.lpVtbl = disp_info3_vtbl;
    inst->IDisplayInformation4_iface.lpVtbl = disp_info4_vtbl;
    inst->ref = 1;
    *instance = (IInspectable *)&inst->IDisplayInformation_iface;
    return S_OK;
}

static const IActivationFactoryVtbl disp_info_activation_factory_vtbl = {
    disp_info_fac_QueryInterface,
    disp_info_fac_AddRef,
    disp_info_fac_Release,
    disp_info_fac_GetIids,
    disp_info_fac_GetRuntimeClassName,
    disp_info_fac_GetTrustLevel,
    disp_info_fac_ActivateInstance
};

static HRESULT WINAPI disp_info_stat_QI(void *This, REFIID riid, void **ppvObject)
{
    return disp_info_fac_QI_internal(impl_from_disp_stat(This), riid, ppvObject);
}
static ULONG WINAPI disp_info_stat_AddRef(void *This) { return 2; }
static ULONG WINAPI disp_info_stat_Release(void *This) { return 1; }
static HRESULT WINAPI disp_info_stat_GetForCurrentView(void *This, void **current)
{
    struct display_info *inst = calloc(1, sizeof(*inst));
    if (!inst) return E_OUTOFMEMORY;
    inst->IDisplayInformation_iface.lpVtbl = disp_info_vtbl;
    inst->IDisplayInformation2_iface.lpVtbl = disp_info2_vtbl;
    inst->IDisplayInformation3_iface.lpVtbl = disp_info3_vtbl;
    inst->IDisplayInformation4_iface.lpVtbl = disp_info4_vtbl;
    inst->ref = 1;
    *current = &inst->IDisplayInformation_iface;
    TRACE("DisplayInformation GetForCurrentView -> %p\n", *current);
    return S_OK;
}
static HRESULT WINAPI disp_info_stat_get_AutoRotationPreferences(void *This, int *value)
{
    if (value) *value = 0;
    return S_OK;
}
static HRESULT WINAPI disp_info_stat_put_AutoRotationPreferences(void *This, int value) { return S_OK; }
static HRESULT WINAPI disp_info_stat_add_DisplayContentsInvalidated(void *This, void *h, EventRegistrationToken *t)
{
    if (t) t->value = 1;
    return S_OK;
}
static HRESULT WINAPI disp_info_stat_remove_DisplayContentsInvalidated(void *This, EventRegistrationToken t) { return S_OK; }

static const void *disp_info_statics_vtbl[] = {
    (void *)disp_info_stat_QI,
    (void *)disp_info_stat_AddRef,
    (void *)disp_info_stat_Release,
    (void *)disp_info_fac_GetIids,
    (void *)disp_info_fac_GetRuntimeClassName,
    (void *)disp_info_fac_GetTrustLevel,
    (void *)disp_info_stat_GetForCurrentView,
    (void *)disp_info_stat_get_AutoRotationPreferences,
    (void *)disp_info_stat_put_AutoRotationPreferences,
    (void *)disp_info_stat_add_DisplayContentsInvalidated,
    (void *)disp_info_stat_remove_DisplayContentsInvalidated
};

static struct disp_info_factory_impl default_disp_info_factory = {
    {&disp_info_activation_factory_vtbl},
    {disp_info_statics_vtbl},
    1
};

IActivationFactory *display_information_factory = &default_disp_info_factory.IActivationFactory_iface;
