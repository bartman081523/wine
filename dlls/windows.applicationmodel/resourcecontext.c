/*
 * WinRT Windows.ApplicationModel.Resources.Core.ResourceContext & ResourceManager Implementation
 *
 * Copyright (C) 2026 Julian / Xodus Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

static const GUID IID_IResourceContextStatics  = {0x98b04530, 0x4be3, 0x4eb6, {0xa6, 0x09, 0x67, 0x9d, 0x98, 0x74, 0x56, 0x98}};
static const GUID IID_IResourceContextStatics2 = {0x41f752ef, 0x12af, 0x41b9, {0xab, 0x36, 0xb1, 0xeb, 0x4b, 0x51, 0x24, 0x60}};
static const GUID IID_IResourceContextStatics3 = {0x20cf292c, 0x1934, 0x494f, {0xa7, 0x98, 0x4b, 0x30, 0x6e, 0x23, 0x6f, 0x6f}};
static const GUID IID_IResourceContext         = {0x29849285, 0x4d99, 0x4934, {0xa0, 0x6a, 0x6b, 0xb6, 0x30, 0xc6, 0x82, 0x59}};
static const GUID IID_IResourceContext_real    = {0x2fa22f4b, 0x707e, 0x4b27, {0xad, 0x0d, 0xd0, 0xd8, 0xcd, 0x46, 0x8f, 0xd2}};
static const GUID IID_IObservableMap_redhawk   = {0x1e036276, 0x2f60, 0x55f6, {0xb7, 0xf3, 0xf8, 0x60, 0x79, 0xe6, 0x90, 0x0b}};

static const GUID IID_IResourceManagerStatics  = {0x11e405d4, 0x0c93, 0x4708, {0xab, 0x7c, 0xf6, 0x72, 0x05, 0x98, 0x2b, 0x90}};
static const GUID IID_IResourceManager         = {0xdb254dc5, 0x5a18, 0x48df, {0x8a, 0x3d, 0x36, 0x0f, 0xc6, 0x73, 0xc1, 0x7c}};
static const GUID IID_IResourceMap             = {0x72284824, 0xdb8c, 0x42f8, {0xb0, 0x8c, 0x53, 0xff, 0x35, 0x7d, 0xad, 0x82}};
static const GUID IID_IResourceCandidate       = {0xaf843f42, 0xde00, 0x416f, {0x90, 0x33, 0x3c, 0x5f, 0x41, 0x23, 0x6e, 0x79}};

static const GUID IID_ComWrappers_internal     = {0x11d3b13a, 0x180e, 0x4789, {0xa8, 0xbe, 0x77, 0x12, 0x88, 0x28, 0x93, 0xe6}};
static const GUID IID_ComWrappers_internal2    = {0x5c13e51c, 0x241d, 0x4859, {0x80, 0x05, 0x8a, 0x8d, 0x8d, 0x98, 0x15, 0xbf}};

static const GUID IID_IObservableMap_str_str1  = {0x690184f4, 0x98d7, 0x5016, {0xa2, 0xb1, 0xb5, 0xff, 0x86, 0x19, 0x2b, 0x3a}};
static const GUID IID_IObservableMap_str_str2  = {0x6a4021a6, 0xe55c, 0x59e7, {0xab, 0xb4, 0xda, 0x42, 0xc9, 0x73, 0x11, 0x16}};
static const GUID IID_IMap_str_str1            = {0x418bddce, 0x9ee6, 0x57fb, {0xa0, 0x6b, 0x88, 0x08, 0x88, 0xf3, 0x57, 0x99}};
static const GUID IID_IMap_str_str2            = {0xa8a2794b, 0x143a, 0x5e78, {0x97, 0x82, 0x09, 0x56, 0x38, 0x9d, 0x7b, 0xdd}};
static const GUID IID_IMapView_str_str1        = {0xa7672c3d, 0x1523, 0x54a6, {0xa8, 0x62, 0xc1, 0xf1, 0xd1, 0xbb, 0xa5, 0x1d}};
static const GUID IID_IMapView_str_str2        = {0x0c15314f, 0x57f9, 0x5ccd, {0xa4, 0x94, 0x3e, 0xcb, 0x9a, 0x50, 0x56, 0x55}};
static const GUID IID_IVector_str1             = {0x99b27783, 0xccfa, 0x5922, {0x8a, 0x36, 0x97, 0x1d, 0x96, 0xf7, 0xbf, 0xfa}};
static const GUID IID_IVector_str2             = {0x3f041445, 0x0675, 0x511a, {0x95, 0x56, 0xf4, 0xef, 0xd3, 0x58, 0x9c, 0x53}};
static const GUID IID_IVectorView_str1         = {0x2f13c006, 0xa03a, 0x5f69, {0xb0, 0x90, 0x75, 0xa4, 0x3e, 0x33, 0x42, 0x3e}};
static const GUID IID_IIterable_str1           = {0x6c1f7230, 0xa834, 0x5da9, {0xb8, 0xb9, 0x3c, 0x3d, 0xc8, 0x2e, 0x62, 0x29}};

#define RuntimeClass_ResourceContext L"Windows.ApplicationModel.Resources.Core.ResourceContext"
#define RuntimeClass_ResourceManager L"Windows.ApplicationModel.Resources.Core.ResourceManager"

static HRESULT create_resource_context_instance(void **value);
static HRESULT create_qualifier_map_instance(void **value);
static HRESULT create_languages_vector_instance(void **value);
static HRESULT create_resource_map_instance(void **value);
static HRESULT create_resource_manager_instance(void **value);

/* -------------------------------------------------------------------------
 * Internal Redhawk Interop Helper
 * ------------------------------------------------------------------------- */

static HRESULT WINAPI redhawk_interop_m3(void *This, void *a1, void *a2, void *a3) { TRACE("m3 (0x18): %p, %p, %p, %p\n", This, a1, a2, a3); return S_OK; }
static HRESULT WINAPI redhawk_interop_m4(void *This, void *a1, void *a2, void *a3) { TRACE("m4 (0x20): %p, %p, %p, %p\n", This, a1, a2, a3); return S_OK; }
static HRESULT WINAPI redhawk_interop_m5(void *This, void *a1, void *a2, void *a3) { TRACE("m5 (0x28): %p, %p, %p, %p\n", This, a1, a2, a3); return S_OK; }
static HRESULT WINAPI redhawk_interop_m6(void *This, void *a1, void *a2, void *a3) { TRACE("m6 (0x30): %p, %p, %p, %p\n", This, a1, a2, a3); return S_OK; }
static HRESULT WINAPI redhawk_interop_m7(void *This, void *a1, void *a2, void *a3) { TRACE("m7 (0x38): %p, %p, %p, %p, ret=%p\n", This, a1, a2, a3, __builtin_return_address(0)); return S_OK; }
static HRESULT WINAPI redhawk_interop_m8(void *This, void *a1, void *a2, void *a3) { TRACE("m8 (0x40): %p, %p, %p, %p\n", This, a1, a2, a3); return S_OK; }
static HRESULT WINAPI redhawk_interop_m9(void *This, void *a1, void *a2, void *a3) { TRACE("m9 (0x48): %p, %p, %p, %p\n", This, a1, a2, a3); return S_OK; }
static HRESULT WINAPI redhawk_interop_m10(void *This, void *a1, void *a2, void *a3) { TRACE("m10 (0x50): %p, %p, %p, %p\n", This, a1, a2, a3); return S_OK; }
static HRESULT WINAPI redhawk_interop_m11(void *This, void *a1, void *a2, void *a3) { TRACE("m11 (0x58): %p, %p, %p, %p\n", This, a1, a2, a3); return S_OK; }
static HRESULT WINAPI redhawk_interop_m12(void *This, void *a1, void *a2, void *a3) { TRACE("m12 (0x60): %p, %p, %p, %p\n", This, a1, a2, a3); return S_OK; }
static HRESULT WINAPI redhawk_interop_m13(void *This, void *a1, void *a2, void *a3) { TRACE("m13 (0x68): %p, %p, %p, %p\n", This, a1, a2, a3); return S_OK; }
static HRESULT WINAPI redhawk_interop_m14(void *This, void *a1, void *a2, void *a3) { TRACE("m14 (0x70): %p, %p, %p, %p\n", This, a1, a2, a3); return S_OK; }
static HRESULT WINAPI redhawk_interop_m15(void *This, void *a1, void *a2, void *a3) { TRACE("m15 (0x78): %p, %p, %p, %p\n", This, a1, a2, a3); return S_OK; }


/* -------------------------------------------------------------------------
 * Qualifier Map: Implements IObservableMap<HSTRING, HSTRING> and IMap<HSTRING, HSTRING>
 * ------------------------------------------------------------------------- */
struct qualifier_map_impl
{
    /* Interface 1: IObservableMap<HSTRING, HSTRING> */
    struct { const void *lpVtbl; } IObservableMap_iface;
    /* Interface 2: IMap<HSTRING, HSTRING> */
    struct { const void *lpVtbl; } IMap_iface;
    /* Interface 3: IMapView<HSTRING, HSTRING> */
    struct { const void *lpVtbl; } IMapView_iface;
    /* Interface 4: IRedhawkInterop_iface */
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    LONG ref;
};

#define impl_from_IObservableMap(ptr) ((struct qualifier_map_impl *)((char *)(ptr) - offsetof(struct qualifier_map_impl, IObservableMap_iface)))
#define impl_from_IMap(ptr)           ((struct qualifier_map_impl *)((char *)(ptr) - offsetof(struct qualifier_map_impl, IMap_iface)))
#define impl_from_IMapView(ptr)       ((struct qualifier_map_impl *)((char *)(ptr) - offsetof(struct qualifier_map_impl, IMapView_iface)))
#define impl_from_IRedhawk_qmap(ptr)  ((struct qualifier_map_impl *)((char *)(ptr) - offsetof(struct qualifier_map_impl, IRedhawkInterop_iface)))

static HRESULT qmap_QI_internal(struct qualifier_map_impl *impl, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("qmap %p: QI %s\n", impl, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IObservableMap_str_str1) ||
        IsEqualGUID(riid, &IID_IObservableMap_str_str2) ||
        IsEqualGUID(riid, &IID_IObservableMap_redhawk))
    {
        *ppvObject = &impl->IObservableMap_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IMap_str_str1) ||
        IsEqualGUID(riid, &IID_IMap_str_str2) ||
        IsEqualGUID(riid, &IID___FIMap_2_HSTRING_HSTRING))
    {
        *ppvObject = &impl->IMap_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IMapView_str_str1) ||
        IsEqualGUID(riid, &IID_IMapView_str_str2) ||
        IsEqualGUID(riid, &IID___FIMapView_2_HSTRING_HSTRING))
    {
        *ppvObject = &impl->IMapView_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ComWrappers_internal))
    {
        *ppvObject = &impl->IRedhawkInterop_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ComWrappers_internal2) ||
        IsEqualGUID(riid, &IID_IWeakReferenceSource))
    {
        return E_NOINTERFACE;
    }

    TRACE("qmap %p: unhandled QI %s\n", impl, debugstr_guid(riid));
    return E_NOINTERFACE;
}

/* IObservableMap methods */
static HRESULT WINAPI qmap_obs_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return qmap_QI_internal(impl_from_IObservableMap(This), riid, ppvObject);
}
static ULONG WINAPI qmap_obs_AddRef(void *This)
{
    return InterlockedIncrement(&impl_from_IObservableMap(This)->ref);
}
static ULONG WINAPI qmap_obs_Release(void *This)
{
    struct qualifier_map_impl *impl = impl_from_IObservableMap(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}
static HRESULT WINAPI qmap_obs_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI qmap_obs_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.Foundation.Collections.PropertySet", 41, n); }
static HRESULT WINAPI qmap_obs_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI qmap_obs_MapChanged(void *This, void *handler, EventRegistrationToken *token)
{
    if (token) token->value = 1;
    return S_OK;
}
static HRESULT WINAPI qmap_obs_remove_MapChanged(void *This, EventRegistrationToken token) { return S_OK; }

static const void *qualifier_obs_map_vtbl[] = {
    (void *)qmap_obs_QueryInterface,
    (void *)qmap_obs_AddRef,
    (void *)qmap_obs_Release,
    (void *)qmap_obs_GetIids,
    (void *)qmap_obs_GetRuntimeClassName,
    (void *)qmap_obs_GetTrustLevel,
    (void *)qmap_obs_MapChanged,        /* Index 6 */
    (void *)qmap_obs_remove_MapChanged  /* Index 7 */
};

/* IMap methods */
static HRESULT WINAPI qmap_map_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return qmap_QI_internal(impl_from_IMap(This), riid, ppvObject);
}
static ULONG WINAPI qmap_map_AddRef(void *This)
{
    return InterlockedIncrement(&impl_from_IMap(This)->ref);
}
static ULONG WINAPI qmap_map_Release(void *This)
{
    struct qualifier_map_impl *impl = impl_from_IMap(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}
static HRESULT WINAPI qmap_map_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI qmap_map_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.Foundation.Collections.PropertySet", 41, n); }
static HRESULT WINAPI qmap_map_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI qmap_map_Lookup(void *This, HSTRING key, HSTRING *value)
{
    const WCHAR *str = WindowsGetStringRawBuffer(key, NULL);
    TRACE("(%p, %s, %p)\n", This, debugstr_w(str), value);
    if (!value) return E_POINTER;
    if (str)
    {
        if (!wcscmp(str, L"Language") || !wcscmp(str, L"language"))
            return WindowsCreateString(L"de-DE", 5, value);
        if (!wcscmp(str, L"Scale") || !wcscmp(str, L"scale"))
            return WindowsCreateString(L"100", 3, value);
        if (!wcscmp(str, L"Contrast") || !wcscmp(str, L"contrast"))
            return WindowsCreateString(L"standard", 8, value);
        if (!wcscmp(str, L"Theme") || !wcscmp(str, L"theme"))
            return WindowsCreateString(L"dark", 4, value);
        if (!wcscmp(str, L"DXFeatureLevel"))
            return WindowsCreateString(L"dx11", 4, value);
    }
    return WindowsCreateString(L"", 0, value);
}

static HRESULT WINAPI qmap_map_get_Size(void *This, UINT32 *size) { if (size) *size = 5; return S_OK; }
static HRESULT WINAPI qmap_map_HasKey(void *This, HSTRING key, boolean *found) { if (found) *found = TRUE; return S_OK; }
static HRESULT WINAPI qmap_map_GetView(void *This, void **view)
{
    struct qualifier_map_impl *impl = impl_from_IMap(This);
    if (!view) return E_POINTER;
    *view = &impl->IMapView_iface;
    InterlockedIncrement(&impl->ref);
    return S_OK;
}
static HRESULT WINAPI qmap_map_Insert(void *This, HSTRING key, HSTRING value, boolean *replaced) { if (replaced) *replaced = TRUE; return S_OK; }
static HRESULT WINAPI qmap_map_Remove(void *This, HSTRING key) { return S_OK; }
static HRESULT WINAPI qmap_map_Clear(void *This) { return S_OK; }

static const void *qualifier_map_vtbl[] = {
    (void *)qmap_map_QueryInterface,
    (void *)qmap_map_AddRef,
    (void *)qmap_map_Release,
    (void *)qmap_map_GetIids,
    (void *)qmap_map_GetRuntimeClassName,
    (void *)qmap_map_GetTrustLevel,
    (void *)qmap_map_Lookup,     /* Index 6 */
    (void *)qmap_map_get_Size,    /* Index 7 */
    (void *)qmap_map_HasKey,     /* Index 8 */
    (void *)qmap_map_GetView,    /* Index 9 */
    (void *)qmap_map_Insert,     /* Index 10 */
    (void *)qmap_map_Remove,     /* Index 11 */
    (void *)qmap_map_Clear       /* Index 12 */
};

/* IMapView methods */
static HRESULT WINAPI qmap_view_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return qmap_QI_internal(impl_from_IMapView(This), riid, ppvObject);
}
static ULONG WINAPI qmap_view_AddRef(void *This) { return InterlockedIncrement(&impl_from_IMapView(This)->ref); }
static ULONG WINAPI qmap_view_Release(void *This)
{
    struct qualifier_map_impl *impl = impl_from_IMapView(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}
static HRESULT WINAPI qmap_view_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI qmap_view_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.Foundation.Collections.PropertySet", 41, n); }
static HRESULT WINAPI qmap_view_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI qmap_view_Lookup(void *This, HSTRING key, HSTRING *value) { return qmap_map_Lookup(This, key, value); }
static HRESULT WINAPI qmap_view_get_Size(void *This, UINT32 *size) { if (size) *size = 5; return S_OK; }
static HRESULT WINAPI qmap_view_HasKey(void *This, HSTRING key, boolean *found) { if (found) *found = TRUE; return S_OK; }
static HRESULT WINAPI qmap_view_Split(void *This, void **first, void **second) { if (first) *first = NULL; if (second) *second = NULL; return S_OK; }

static const void *qualifier_map_view_vtbl[] = {
    (void *)qmap_view_QueryInterface,
    (void *)qmap_view_AddRef,
    (void *)qmap_view_Release,
    (void *)qmap_view_GetIids,
    (void *)qmap_view_GetRuntimeClassName,
    (void *)qmap_view_GetTrustLevel,
    (void *)qmap_view_Lookup,   /* Index 6 */
    (void *)qmap_view_get_Size,  /* Index 7 */
    (void *)qmap_view_HasKey,   /* Index 8 */
    (void *)qmap_view_Split     /* Index 9 */
};

static HRESULT WINAPI redhawk_qmap_QI(void *This, REFIID riid, void **ppvObject)
{
    return qmap_QI_internal(impl_from_IRedhawk_qmap(This), riid, ppvObject);
}
static ULONG WINAPI redhawk_qmap_AddRef(void *This)
{
    return InterlockedIncrement(&impl_from_IRedhawk_qmap(This)->ref);
}
static ULONG WINAPI redhawk_qmap_Release(void *This)
{
    struct qualifier_map_impl *impl = impl_from_IRedhawk_qmap(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static const void *redhawk_qmap_vtbl[] = {
    (void *)redhawk_qmap_QI,
    (void *)redhawk_qmap_AddRef,
    (void *)redhawk_qmap_Release,
    (void *)redhawk_interop_m3,
    (void *)qmap_obs_GetRuntimeClassName,
    (void *)qmap_obs_GetTrustLevel,
    (void *)qmap_obs_MapChanged,        /* Index 6: 0x30 */
    (void *)qmap_obs_remove_MapChanged, /* Index 7: 0x38 */
    (void *)qmap_map_Lookup,            /* Index 8: 0x40 */
    (void *)qmap_map_get_Size,          /* Index 9: 0x48 */
    (void *)qmap_map_HasKey,            /* Index 10: 0x50 */
    (void *)qmap_map_GetView,           /* Index 11: 0x58 */
    (void *)qmap_map_Insert,            /* Index 12: 0x60 */
    (void *)qmap_map_Remove,            /* Index 13: 0x68 */
    (void *)qmap_map_Clear,             /* Index 14: 0x70 */
    (void *)redhawk_interop_m15
};

static HRESULT create_qualifier_map_instance(void **value)
{
    struct qualifier_map_impl *impl = calloc(1, sizeof(*impl));
    if (!impl) return E_OUTOFMEMORY;
    impl->IObservableMap_iface.lpVtbl = qualifier_obs_map_vtbl;
    impl->IMap_iface.lpVtbl = qualifier_map_vtbl;
    impl->IMapView_iface.lpVtbl = qualifier_map_view_vtbl;
    impl->IRedhawkInterop_iface.lpVtbl = redhawk_qmap_vtbl;
    impl->ref = 1;
    *value = &impl->IObservableMap_iface;
    return S_OK;
}

/* -------------------------------------------------------------------------
 * Languages Vector (IVector<HSTRING> & IIterable<HSTRING>)
 * ------------------------------------------------------------------------- */
struct languages_vector_impl
{
    struct { const void *lpVtbl; } IVector_iface;
    struct { const void *lpVtbl; } IVectorView_iface;
    struct { const void *lpVtbl; } IIterable_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    LONG ref;
};

#define impl_from_IVector(ptr)         ((struct languages_vector_impl *)((char *)(ptr) - offsetof(struct languages_vector_impl, IVector_iface)))
#define impl_from_IVectorView_lvec(ptr)((struct languages_vector_impl *)((char *)(ptr) - offsetof(struct languages_vector_impl, IVectorView_iface)))
#define impl_from_IIterable(ptr)       ((struct languages_vector_impl *)((char *)(ptr) - offsetof(struct languages_vector_impl, IIterable_iface)))
#define impl_from_IRedhawk_lvec(ptr)   ((struct languages_vector_impl *)((char *)(ptr) - offsetof(struct languages_vector_impl, IRedhawkInterop_iface)))

static HRESULT lvec_QI_internal(struct languages_vector_impl *impl, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("lvec %p: QI %s\n", impl, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IVector_HSTRING) ||
        IsEqualGUID(riid, &IID_IVector_str1) ||
        IsEqualGUID(riid, &IID_IVector_str2))
    {
        *ppvObject = &impl->IVector_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IVectorView_str1) ||
        IsEqualGUID(riid, &IID___FIVectorView_1_HSTRING))
    {
        *ppvObject = &impl->IVectorView_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IIterable_HSTRING) ||
        IsEqualGUID(riid, &IID_IIterable_str1))
    {
        *ppvObject = &impl->IIterable_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ComWrappers_internal))
    {
        *ppvObject = &impl->IRedhawkInterop_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ComWrappers_internal2) ||
        IsEqualGUID(riid, &IID_IWeakReferenceSource))
    {
        return E_NOINTERFACE;
    }

    TRACE("lvec %p: unhandled QI %s\n", impl, debugstr_guid(riid));
    return E_NOINTERFACE;
}

static HRESULT WINAPI lvec_QueryInterface(void *This, REFIID riid, void **ppvObject) { return lvec_QI_internal(impl_from_IVector(This), riid, ppvObject); }
static ULONG WINAPI lvec_AddRef(void *This) { return InterlockedIncrement(&impl_from_IVector(This)->ref); }
static ULONG WINAPI lvec_Release(void *This)
{
    struct languages_vector_impl *impl = impl_from_IVector(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}
static HRESULT WINAPI lvec_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI lvec_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.Foundation.Collections.Vector", 37, n); }
static HRESULT WINAPI lvec_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI lvec_GetAt(void *This, UINT32 index, HSTRING *value)
{
    TRACE("lvec_GetAt This %p, index %u, value %p, ret %p\n", This, index, value, __builtin_return_address(0));
    if (!value) return E_POINTER;
    if (index == 0) return WindowsCreateString(L"de-DE", 5, value);
    if (index == 1) return WindowsCreateString(L"en-US", 5, value);
    return 0x8000000b; /* E_BOUNDS */
}
static HRESULT WINAPI lvec_get_Size(void *This, UINT32 *size)
{
    TRACE("lvec_get_Size This %p, size %p, ret %p\n", This, size, __builtin_return_address(0));
    if (size) *size = 2;
    return S_OK;
}
static HRESULT WINAPI lvec_GetView(void *This, void **view)
{
    TRACE("lvec_GetView This %p, view %p\n", This, view);
    if (!view) return E_POINTER;
    *view = This;
    lvec_AddRef(This);
    return S_OK;
}
static HRESULT WINAPI lvec_IndexOf(void *This, HSTRING value, UINT32 *index, boolean *found)
{
    TRACE("lvec_IndexOf This %p\n", This);
    if (index) *index = 0;
    if (found) *found = TRUE;
    return S_OK;
}
static HRESULT WINAPI lvec_SetAt(void *This, UINT32 index, HSTRING value) { return S_OK; }
static HRESULT WINAPI lvec_InsertAt(void *This, UINT32 index, HSTRING value) { return S_OK; }
static HRESULT WINAPI lvec_RemoveAt(void *This, UINT32 index) { return S_OK; }
static HRESULT WINAPI lvec_Append(void *This, HSTRING value) { return S_OK; }
static HRESULT WINAPI lvec_RemoveAtEnd(void *This) { return S_OK; }
static HRESULT WINAPI lvec_Clear(void *This) { return S_OK; }
static HRESULT WINAPI lvec_GetMany(void *This, UINT32 s, UINT32 cap, HSTRING *val, UINT32 *act) { if (act) *act = 0; return S_OK; }
static HRESULT WINAPI lvec_ReplaceAll(void *This, UINT32 count, HSTRING *val) { return S_OK; }

static const void *languages_vector_vtbl[] = {
    (void *)lvec_QueryInterface,
    (void *)lvec_AddRef,
    (void *)lvec_Release,
    (void *)lvec_GetIids,
    (void *)lvec_GetRuntimeClassName,
    (void *)lvec_GetTrustLevel,
    (void *)lvec_GetAt,         /* Index 6 */
    (void *)lvec_get_Size,       /* Index 7 */
    (void *)lvec_GetView,       /* Index 8 */
    (void *)lvec_IndexOf,       /* Index 9 */
    (void *)lvec_SetAt,         /* Index 10 */
    (void *)lvec_InsertAt,      /* Index 11 */
    (void *)lvec_RemoveAt,      /* Index 12 */
    (void *)lvec_Append,        /* Index 13 */
    (void *)lvec_RemoveAtEnd,   /* Index 14 */
    (void *)lvec_Clear,         /* Index 15 */
    (void *)lvec_GetMany,       /* Index 16 */
    (void *)lvec_ReplaceAll     /* Index 17 */
};

/* IVectorView methods */
static HRESULT WINAPI lvec_view_QueryInterface(void *This, REFIID riid, void **ppvObject) { return lvec_QI_internal(impl_from_IVectorView_lvec(This), riid, ppvObject); }
static ULONG WINAPI lvec_view_AddRef(void *This) { return InterlockedIncrement(&impl_from_IVectorView_lvec(This)->ref); }
static ULONG WINAPI lvec_view_Release(void *This)
{
    struct languages_vector_impl *impl = impl_from_IVectorView_lvec(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static const void *languages_vector_view_vtbl[] = {
    (void *)lvec_view_QueryInterface,
    (void *)lvec_view_AddRef,
    (void *)lvec_view_Release,
    (void *)lvec_GetIids,
    (void *)lvec_GetRuntimeClassName,
    (void *)lvec_GetTrustLevel,
    (void *)lvec_GetAt,         /* Index 6 */
    (void *)lvec_get_Size,       /* Index 7 */
    (void *)lvec_IndexOf,       /* Index 8 */
    (void *)lvec_GetMany        /* Index 9 */
};

/* -------------------------------------------------------------------------
 * Iterator<HSTRING> Implementation
 * ------------------------------------------------------------------------- */
struct languages_iterator_impl
{
    struct { const void *lpVtbl; } IIterator_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    LONG ref;
    UINT32 current_index;
};

#define impl_from_IIterator(ptr) ((struct languages_iterator_impl *)((char *)(ptr) - offsetof(struct languages_iterator_impl, IIterator_iface)))
#define impl_from_IRedhawk_iter(ptr) ((struct languages_iterator_impl *)((char *)(ptr) - offsetof(struct languages_iterator_impl, IRedhawkInterop_iface)))

static HRESULT iter_QI_internal(struct languages_iterator_impl *impl, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("iter %p: QI %s\n", impl, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID___FIIterator_1_HSTRING))
    {
        *ppvObject = &impl->IIterator_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_ComWrappers_internal))
    {
        *ppvObject = &impl->IRedhawkInterop_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    return E_NOINTERFACE;
}

static HRESULT WINAPI iter_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return iter_QI_internal(impl_from_IIterator(This), riid, ppvObject);
}
static ULONG WINAPI iter_AddRef(void *This)
{
    return InterlockedIncrement(&impl_from_IIterator(This)->ref);
}
static ULONG WINAPI iter_Release(void *This)
{
    struct languages_iterator_impl *impl = impl_from_IIterator(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}
static HRESULT WINAPI iter_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI iter_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.Foundation.Collections.Iterator", 39, n); }
static HRESULT WINAPI iter_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI iter_get_Current(void *This, HSTRING *value)
{
    struct languages_iterator_impl *impl = impl_from_IIterator(This);
    TRACE("iter_get_Current %p, index %u, value %p\n", impl, impl->current_index, value);
    if (!value) return E_POINTER;
    if (impl->current_index == 0) return WindowsCreateString(L"de-DE", 5, value);
    if (impl->current_index == 1) return WindowsCreateString(L"en-US", 5, value);
    return 0x8000000b; /* E_BOUNDS */
}

static HRESULT WINAPI iter_get_HasCurrent(void *This, boolean *hasCurrent)
{
    struct languages_iterator_impl *impl = impl_from_IIterator(This);
    TRACE("iter_get_HasCurrent %p, index %u\n", impl, impl->current_index);
    if (!hasCurrent) return E_POINTER;
    *hasCurrent = (impl->current_index < 2);
    return S_OK;
}

static HRESULT WINAPI iter_MoveNext(void *This, boolean *hasCurrent)
{
    struct languages_iterator_impl *impl = impl_from_IIterator(This);
    TRACE("iter_MoveNext %p, old index %u\n", impl, impl->current_index);
    if (impl->current_index < 2) impl->current_index++;
    if (hasCurrent) *hasCurrent = (impl->current_index < 2);
    return S_OK;
}

static HRESULT WINAPI iter_GetMany(void *This, UINT32 count, HSTRING *items, UINT32 *actual)
{
    if (actual) *actual = 0;
    return S_OK;
}

static const void *languages_iterator_vtbl[] = {
    (void *)iter_QueryInterface,
    (void *)iter_AddRef,
    (void *)iter_Release,
    (void *)iter_GetIids,
    (void *)iter_GetRuntimeClassName,
    (void *)iter_GetTrustLevel,
    (void *)iter_get_Current,       /* Index 6: 0x30 */
    (void *)iter_get_HasCurrent,    /* Index 7: 0x38 */
    (void *)iter_MoveNext,          /* Index 8: 0x40 */
    (void *)iter_GetMany            /* Index 9: 0x48 */
};

static HRESULT WINAPI redhawk_iter_QI(void *This, REFIID riid, void **ppvObject)
{
    return iter_QI_internal(impl_from_IRedhawk_iter(This), riid, ppvObject);
}
static ULONG WINAPI redhawk_iter_AddRef(void *This)
{
    return InterlockedIncrement(&impl_from_IRedhawk_iter(This)->ref);
}
static ULONG WINAPI redhawk_iter_Release(void *This)
{
    struct languages_iterator_impl *impl = impl_from_IRedhawk_iter(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static const void *redhawk_iterator_vtbl[] = {
    (void *)redhawk_iter_QI,
    (void *)redhawk_iter_AddRef,
    (void *)redhawk_iter_Release,
    (void *)redhawk_interop_m3,
    (void *)iter_GetRuntimeClassName,
    (void *)iter_GetTrustLevel,
    (void *)iter_get_Current,       /* Index 6: 0x30 */
    (void *)iter_get_HasCurrent,    /* Index 7: 0x38 */
    (void *)iter_MoveNext,          /* Index 8: 0x40 */
    (void *)iter_GetMany            /* Index 9: 0x48 */
};

static HRESULT languages_iterator_create(void **out)
{
    struct languages_iterator_impl *impl = calloc(1, sizeof(*impl));
    if (!impl) return E_OUTOFMEMORY;
    impl->IIterator_iface.lpVtbl = languages_iterator_vtbl;
    impl->IRedhawkInterop_iface.lpVtbl = redhawk_iterator_vtbl;
    impl->ref = 1;
    impl->current_index = 0;
    *out = &impl->IIterator_iface;
    TRACE("Created languages iterator %p\n", *out);
    return S_OK;
}

/* IIterable methods */
static HRESULT WINAPI lvec_iter_QueryInterface(void *This, REFIID riid, void **ppvObject) { return lvec_QI_internal(impl_from_IIterable(This), riid, ppvObject); }
static ULONG WINAPI lvec_iter_AddRef(void *This) { return InterlockedIncrement(&impl_from_IIterable(This)->ref); }
static ULONG WINAPI lvec_iter_Release(void *This)
{
    struct languages_vector_impl *impl = impl_from_IIterable(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}
static HRESULT WINAPI lvec_iter_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI lvec_iter_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.Foundation.Collections.Vector", 37, n); }
static HRESULT WINAPI lvec_iter_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI lvec_iter_First(void *This, void **first)
{
    TRACE("lvec_iter_First This %p, first %p\n", This, first);
    if (!first) return E_POINTER;
    return languages_iterator_create(first);
}

static const void *languages_iterable_vtbl[] = {
    (void *)lvec_iter_QueryInterface,
    (void *)lvec_iter_AddRef,
    (void *)lvec_iter_Release,
    (void *)lvec_iter_GetIids,
    (void *)lvec_iter_GetRuntimeClassName,
    (void *)lvec_iter_GetTrustLevel,
    (void *)lvec_iter_First     /* Index 6 */
};

static HRESULT WINAPI redhawk_lvec_QI(void *This, REFIID riid, void **ppvObject)
{
    return lvec_QI_internal(impl_from_IRedhawk_lvec(This), riid, ppvObject);
}
static ULONG WINAPI redhawk_lvec_AddRef(void *This)
{
    return InterlockedIncrement(&impl_from_IRedhawk_lvec(This)->ref);
}
static ULONG WINAPI redhawk_lvec_Release(void *This)
{
    struct languages_vector_impl *impl = impl_from_IRedhawk_lvec(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI redhawk_lvec_m7(void *This)
{
    TRACE("redhawk_lvec_m7 This %p\n", This);
    return S_OK;
}

static const void *redhawk_lvec_vtbl[] = {
    (void *)redhawk_lvec_QI,
    (void *)redhawk_lvec_AddRef,
    (void *)redhawk_lvec_Release,
    (void *)redhawk_interop_m3,
    (void *)lvec_GetRuntimeClassName,
    (void *)lvec_GetTrustLevel,
    (void *)lvec_iter_First,            /* Index 6: 0x30 -> First! */
    (void *)redhawk_lvec_m7,            /* Index 7: 0x38 -> m7 hook (no deref) */
    (void *)lvec_IndexOf,               /* Index 8: 0x40 */
    (void *)lvec_GetMany,               /* Index 9: 0x48 */
    (void *)redhawk_interop_m10,
    (void *)redhawk_interop_m11,
    (void *)redhawk_interop_m12,
    (void *)redhawk_interop_m13,
    (void *)redhawk_interop_m14,
    (void *)redhawk_interop_m15
};

static HRESULT create_languages_vector_instance(void **value)
{
    struct languages_vector_impl *impl = calloc(1, sizeof(*impl));
    if (!impl) return E_OUTOFMEMORY;
    impl->IVector_iface.lpVtbl = languages_vector_vtbl;
    impl->IVectorView_iface.lpVtbl = languages_vector_view_vtbl;
    impl->IIterable_iface.lpVtbl = languages_iterable_vtbl;
    impl->IRedhawkInterop_iface.lpVtbl = redhawk_lvec_vtbl;
    impl->ref = 1;
    *value = &impl->IVectorView_iface;
    return S_OK;
}

/* -------------------------------------------------------------------------
 * ResourceContext Instance (IResourceContext)
 * ------------------------------------------------------------------------- */
struct resource_context_impl
{
    struct { const void *lpVtbl; } IResourceContext_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    LONG ref;
};

#define impl_from_IResourceContext(ptr) ((struct resource_context_impl *)((char *)(ptr) - offsetof(struct resource_context_impl, IResourceContext_iface)))
#define impl_from_IRedhawk_rctx(ptr)    ((struct resource_context_impl *)((char *)(ptr) - offsetof(struct resource_context_impl, IRedhawkInterop_iface)))

static HRESULT WINAPI rctx_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    struct resource_context_impl *impl = impl_from_IResourceContext(This);
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("rctx %p, riid %s\n", This, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IResourceContext) ||
        IsEqualGUID(riid, &IID_IResourceContext_real))
    {
        *ppvObject = &impl->IResourceContext_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ComWrappers_internal))
    {
        *ppvObject = &impl->IRedhawkInterop_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ComWrappers_internal2) ||
        IsEqualGUID(riid, &IID_IWeakReferenceSource))
    {
        return E_NOINTERFACE;
    }

    TRACE("rctx %p: unhandled QI %s\n", This, debugstr_guid(riid));
    return E_NOINTERFACE;
}
static ULONG WINAPI rctx_AddRef(void *This) { return InterlockedIncrement(&impl_from_IResourceContext(This)->ref); }
static ULONG WINAPI rctx_Release(void *This)
{
    struct resource_context_impl *impl = impl_from_IResourceContext(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}
static HRESULT WINAPI rctx_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI rctx_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(RuntimeClass_ResourceContext, wcslen(RuntimeClass_ResourceContext), n); }
static HRESULT WINAPI rctx_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI redhawk_rctx_QI(void *This, REFIID riid, void **ppvObject)
{
    return rctx_QueryInterface(&impl_from_IRedhawk_rctx(This)->IResourceContext_iface, riid, ppvObject);
}
static ULONG WINAPI redhawk_rctx_AddRef(void *This)
{
    return rctx_AddRef(&impl_from_IRedhawk_rctx(This)->IResourceContext_iface);
}
static ULONG WINAPI redhawk_rctx_Release(void *This)
{
    return rctx_Release(&impl_from_IRedhawk_rctx(This)->IResourceContext_iface);
}

static HRESULT WINAPI rctx_get_QualifierValues(void *This, void **value)
{
    TRACE("(%p, %p)\n", This, value);
    if (!value) return E_POINTER;
    return create_qualifier_map_instance(value);
}
static HRESULT WINAPI rctx_Reset(void *This) { return S_OK; }
static HRESULT WINAPI rctx_ResetQualifierValues(void *This, void *names) { return S_OK; }
static HRESULT WINAPI rctx_OverrideToMatch(void *This, void *qualifiers) { return S_OK; }
static HRESULT WINAPI rctx_Clone(void *This, void **result)
{
    if (!result) return E_POINTER;
    return create_resource_context_instance(result);
}
static HRESULT WINAPI rctx_get_Languages(void *This, void **value)
{
    TRACE("(%p, %p)\n", This, value);
    if (!value) return E_POINTER;
    return create_languages_vector_instance(value);
}
static HRESULT WINAPI rctx_put_Languages(void *This, void *value) { return S_OK; }

static const void *resource_context_vtbl[] = {
    (void *)rctx_QueryInterface,           /* Index 0 */
    (void *)rctx_AddRef,                   /* Index 1 */
    (void *)rctx_Release,                  /* Index 2 */
    (void *)rctx_GetIids,                  /* Index 3 */
    (void *)rctx_GetRuntimeClassName,      /* Index 4 */
    (void *)rctx_GetTrustLevel,            /* Index 5 */
    (void *)rctx_get_QualifierValues,      /* Index 6 */
    (void *)rctx_Reset,                   /* Index 7 */
    (void *)rctx_ResetQualifierValues,    /* Index 8 */
    (void *)rctx_OverrideToMatch,         /* Index 9 */
    (void *)rctx_Clone,                   /* Index 10 */
    (void *)rctx_get_Languages,           /* Index 11 */
    (void *)rctx_put_Languages,           /* Index 12 */
    (void *)rctx_get_Languages,           /* Index 13 */
    (void *)rctx_get_Languages,           /* Index 14 */
    (void *)rctx_get_Languages            /* Index 15 */
};

static const void *redhawk_rctx_vtbl[] = {
    (void *)redhawk_rctx_QI,              /* Index 0 */
    (void *)redhawk_rctx_AddRef,          /* Index 1 */
    (void *)redhawk_rctx_Release,         /* Index 2 */
    (void *)redhawk_interop_m3,           /* Index 3 */
    (void *)rctx_GetRuntimeClassName,     /* Index 4 */
    (void *)rctx_GetTrustLevel,           /* Index 5 */
    (void *)rctx_get_QualifierValues,     /* Index 6 */
    (void *)rctx_Reset,                   /* Index 7 */
    (void *)rctx_ResetQualifierValues,    /* Index 8 */
    (void *)rctx_OverrideToMatch,         /* Index 9 */
    (void *)rctx_Clone,                   /* Index 10 */
    (void *)rctx_get_Languages,           /* Index 11 */
    (void *)rctx_put_Languages,           /* Index 12 */
    (void *)rctx_get_Languages,           /* Index 13 */
    (void *)rctx_get_Languages,           /* Index 14 */
    (void *)rctx_get_Languages            /* Index 15 */
};

static HRESULT create_resource_context_instance(void **value)
{
    struct resource_context_impl *impl = calloc(1, sizeof(*impl));
    if (!impl) return E_OUTOFMEMORY;
    impl->IResourceContext_iface.lpVtbl = resource_context_vtbl;
    impl->IRedhawkInterop_iface.lpVtbl = redhawk_rctx_vtbl;
    impl->ref = 1;
    *value = &impl->IResourceContext_iface;
    return S_OK;
}

/* -------------------------------------------------------------------------
 * ResourceContext Factory: Implements IActivationFactory, IResourceContextStatics, IResourceContextStatics2, IResourceContextStatics3
 * ------------------------------------------------------------------------- */
struct resource_context_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } IResourceContextStatics_iface;
    struct { const void *lpVtbl; } IResourceContextStatics2_iface;
    struct { const void *lpVtbl; } IResourceContextStatics3_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
};

#define impl_from_IActivationFactory(ptr) ((struct resource_context_factory_impl *)((char *)(ptr) - offsetof(struct resource_context_factory_impl, IActivationFactory_iface)))
#define impl_from_IStatics(ptr)           ((struct resource_context_factory_impl *)((char *)(ptr) - offsetof(struct resource_context_factory_impl, IResourceContextStatics_iface)))
#define impl_from_IStatics2(ptr)          ((struct resource_context_factory_impl *)((char *)(ptr) - offsetof(struct resource_context_factory_impl, IResourceContextStatics2_iface)))
#define impl_from_IStatics3(ptr)          ((struct resource_context_factory_impl *)((char *)(ptr) - offsetof(struct resource_context_factory_impl, IResourceContextStatics3_iface)))
#define impl_from_IRedhawk_fac(ptr)       ((struct resource_context_factory_impl *)((char *)(ptr) - offsetof(struct resource_context_factory_impl, IRedhawkInterop_iface)))

static HRESULT rctx_fac_QI_internal(struct resource_context_factory_impl *impl, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("rctx_fac %p, riid %s\n", impl, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppvObject = &impl->IActivationFactory_iface;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IResourceContextStatics))
    {
        *ppvObject = &impl->IResourceContextStatics_iface;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IResourceContextStatics2))
    {
        *ppvObject = &impl->IResourceContextStatics2_iface;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IResourceContextStatics3))
    {
        *ppvObject = &impl->IResourceContextStatics3_iface;
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ComWrappers_internal))
    {
        *ppvObject = &impl->IRedhawkInterop_iface;
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ComWrappers_internal2) ||
        IsEqualGUID(riid, &IID_IWeakReferenceSource))
    {
        return E_NOINTERFACE;
    }

    TRACE("rctx_fac %p: unhandled QI %s\n", impl, debugstr_guid(riid));
    return E_NOINTERFACE;
}

/* Factory methods */
static HRESULT WINAPI fac_act_QueryInterface(IActivationFactory *iface, REFIID riid, void **ppvObject) { return rctx_fac_QI_internal(impl_from_IActivationFactory(iface), riid, ppvObject); }
static ULONG WINAPI fac_act_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI fac_act_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI fac_act_GetIids(IActivationFactory *iface, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI fac_act_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n) { return WindowsCreateString(RuntimeClass_ResourceContext, wcslen(RuntimeClass_ResourceContext), n); }
static HRESULT WINAPI fac_act_GetTrustLevel(IActivationFactory *iface, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI fac_act_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    if (!instance) return E_POINTER;
    return create_resource_context_instance((void **)instance);
}

static const IActivationFactoryVtbl resource_context_activation_factory_vtbl = {
    fac_act_QueryInterface,
    fac_act_AddRef,
    fac_act_Release,
    fac_act_GetIids,
    fac_act_GetRuntimeClassName,
    fac_act_GetTrustLevel,
    fac_act_ActivateInstance
};

/* Statics 1 methods */
static HRESULT WINAPI fac_s1_QueryInterface(void *This, REFIID riid, void **ppvObject) { return rctx_fac_QI_internal(impl_from_IStatics(This), riid, ppvObject); }
static ULONG WINAPI fac_s1_AddRef(void *This) { return 2; }
static ULONG WINAPI fac_s1_Release(void *This) { return 1; }
static HRESULT WINAPI fac_s1_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI fac_s1_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(RuntimeClass_ResourceContext, wcslen(RuntimeClass_ResourceContext), n); }
static HRESULT WINAPI fac_s1_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI fac_s1_GetForCurrentView(void *This, void **value)
{
    if (!value) return E_POINTER;
    return create_resource_context_instance(value);
}
static HRESULT WINAPI fac_s1_SetGlobalQualifierValue(void *This, HSTRING k, HSTRING v) { return S_OK; }
static HRESULT WINAPI fac_s1_SetGlobalQualifierValueWithPersistence(void *This, HSTRING k, HSTRING v, int p) { return S_OK; }

static const void *resource_context_statics_vtbl[] = {
    (void *)fac_s1_QueryInterface,
    (void *)fac_s1_AddRef,
    (void *)fac_s1_Release,
    (void *)fac_s1_GetIids,
    (void *)fac_s1_GetRuntimeClassName,
    (void *)fac_s1_GetTrustLevel,
    (void *)fac_s1_GetForCurrentView,                           /* Index 6 */
    (void *)fac_s1_SetGlobalQualifierValue,                     /* Index 7 */
    (void *)fac_s1_SetGlobalQualifierValueWithPersistence       /* Index 8 */
};

/* Statics 2 methods */
static HRESULT WINAPI fac_s2_QueryInterface(void *This, REFIID riid, void **ppvObject) { return rctx_fac_QI_internal(impl_from_IStatics2(This), riid, ppvObject); }
static ULONG WINAPI fac_s2_AddRef(void *This) { return 2; }
static ULONG WINAPI fac_s2_Release(void *This) { return 1; }
static HRESULT WINAPI fac_s2_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI fac_s2_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(RuntimeClass_ResourceContext, wcslen(RuntimeClass_ResourceContext), n); }
static HRESULT WINAPI fac_s2_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI fac_s2_GetForViewIndependentUse(void *This, void **value)
{
    TRACE("GetForViewIndependentUse(%p, %p)\n", This, value);
    if (!value) return E_POINTER;
    return create_resource_context_instance(value);
}

static const void *resource_context_statics2_vtbl[] = {
    (void *)fac_s2_QueryInterface,                      /* 0 */
    (void *)fac_s2_AddRef,                              /* 1 */
    (void *)fac_s2_Release,                             /* 2 */
    (void *)fac_s2_GetIids,                             /* 3 */
    (void *)fac_s2_GetRuntimeClassName,                 /* 4 */
    (void *)fac_s2_GetTrustLevel,                       /* 5 */
    (void *)fac_s2_GetForViewIndependentUse,            /* 6 */
    (void *)fac_s2_GetForViewIndependentUse,            /* 7 */
    (void *)fac_s2_GetForViewIndependentUse,            /* 8 */
    (void *)fac_s2_GetForViewIndependentUse,            /* 9 */
    (void *)fac_s2_GetForViewIndependentUse,            /* 10 */
    (void *)fac_s2_GetForViewIndependentUse,            /* 11 */
    (void *)fac_s2_GetForViewIndependentUse,            /* 12 */
    (void *)fac_s2_GetForViewIndependentUse,            /* 13 */
    (void *)fac_s2_GetForViewIndependentUse,            /* 14 */
    (void *)fac_s2_GetForViewIndependentUse             /* 15 */
};

/* Statics 3 methods */
static HRESULT WINAPI fac_s3_QueryInterface(void *This, REFIID riid, void **ppvObject) { return rctx_fac_QI_internal(impl_from_IStatics3(This), riid, ppvObject); }
static ULONG WINAPI fac_s3_AddRef(void *This) { return 2; }
static ULONG WINAPI fac_s3_Release(void *This) { return 1; }
static HRESULT WINAPI fac_s3_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI fac_s3_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(RuntimeClass_ResourceContext, wcslen(RuntimeClass_ResourceContext), n); }
static HRESULT WINAPI fac_s3_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI fac_s3_GetForUIContext(void *This, void *c, void **value) { if (!value) return E_POINTER; return create_resource_context_instance(value); }
static HRESULT WINAPI fac_s3_CreateCopyForUIContext(void *This, void *c, void **value) { if (!value) return E_POINTER; return create_resource_context_instance(value); }

static const void *resource_context_statics3_vtbl[] = {
    (void *)fac_s3_QueryInterface,
    (void *)fac_s3_AddRef,
    (void *)fac_s3_Release,
    (void *)fac_s3_GetIids,
    (void *)fac_s3_GetRuntimeClassName,
    (void *)fac_s3_GetTrustLevel,
    (void *)fac_s3_GetForUIContext,                             /* Index 6 */
    (void *)fac_s3_CreateCopyForUIContext                       /* Index 7 */
};

static HRESULT WINAPI redhawk_fac_QI(void *This, REFIID riid, void **ppvObject)
{
    return rctx_fac_QI_internal(impl_from_IRedhawk_fac(This), riid, ppvObject);
}
static ULONG WINAPI redhawk_fac_AddRef(void *This) { return 2; }
static ULONG WINAPI redhawk_fac_Release(void *This) { return 1; }

static HRESULT WINAPI redhawk_fac_m7(void *This, void *a1, void *a2, void *a3)
{
    TRACE("fac_m7: This %p, a1 %p, a2 %p, a3 %p, ret %p\n", This, a1, a2, a3, __builtin_return_address(0));
    return S_OK;
}

static const void *redhawk_fac_vtbl[] = {
    (void *)redhawk_fac_QI,
    (void *)redhawk_fac_AddRef,
    (void *)redhawk_fac_Release,
    (void *)redhawk_interop_m3,
    (void *)fac_s2_GetRuntimeClassName,
    (void *)fac_s2_GetTrustLevel,
    (void *)fac_s2_GetForViewIndependentUse, /* Index 6: 0x30 */
    (void *)redhawk_fac_m7,                 /* Index 7: 0x38 - interop hook */
    (void *)redhawk_fac_m7,                 /* Index 8: 0x40 */
    (void *)redhawk_fac_m7,                 /* Index 9: 0x48 */
    (void *)fac_s2_GetForViewIndependentUse, /* Index 10: 0x50 */
    (void *)fac_s2_GetForViewIndependentUse, /* Index 11: 0x58 */
    (void *)fac_s2_GetForViewIndependentUse, /* Index 12: 0x60 */
    (void *)fac_s2_GetForViewIndependentUse, /* Index 13: 0x68 */
    (void *)fac_s2_GetForViewIndependentUse, /* Index 14: 0x70 */
    (void *)fac_s2_GetForViewIndependentUse  /* Index 15: 0x78 */
};

static struct resource_context_factory_impl default_resource_context_factory = {
    {&resource_context_activation_factory_vtbl},
    {resource_context_statics_vtbl},
    {resource_context_statics2_vtbl},
    {resource_context_statics3_vtbl},
    {redhawk_fac_vtbl}
};

IActivationFactory *resource_context_factory = &default_resource_context_factory.IActivationFactory_iface;

/* -------------------------------------------------------------------------
 * ResourceMap & ResourceManager Instance
 * ------------------------------------------------------------------------- */
struct resource_map_impl
{
    struct { const void *lpVtbl; } IResourceMap_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    LONG ref;
};

#define impl_from_IResourceMap(ptr)  ((struct resource_map_impl *)((char *)(ptr) - offsetof(struct resource_map_impl, IResourceMap_iface)))
#define impl_from_IRedhawk_rmap(ptr) ((struct resource_map_impl *)((char *)(ptr) - offsetof(struct resource_map_impl, IRedhawkInterop_iface)))

static HRESULT WINAPI rmap_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    struct resource_map_impl *impl = impl_from_IResourceMap(This);
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("rmap %p, riid %s\n", This, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IResourceMap))
    {
        *ppvObject = &impl->IResourceMap_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ComWrappers_internal))
    {
        *ppvObject = &impl->IRedhawkInterop_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ComWrappers_internal2) ||
        IsEqualGUID(riid, &IID_IWeakReferenceSource))
    {
        return E_NOINTERFACE;
    }

    TRACE("rmap %p: unhandled QI %s\n", This, debugstr_guid(riid));
    return E_NOINTERFACE;
}
static ULONG WINAPI rmap_AddRef(void *This) { return InterlockedIncrement(&impl_from_IResourceMap(This)->ref); }
static ULONG WINAPI rmap_Release(void *This)
{
    struct resource_map_impl *impl = impl_from_IResourceMap(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}
static HRESULT WINAPI rmap_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI rmap_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.ApplicationModel.Resources.Core.ResourceMap", 48, n); }
static HRESULT WINAPI rmap_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI redhawk_rmap_QI(void *This, REFIID riid, void **ppvObject)
{
    return rmap_QueryInterface(&impl_from_IRedhawk_rmap(This)->IResourceMap_iface, riid, ppvObject);
}
static ULONG WINAPI redhawk_rmap_AddRef(void *This)
{
    return rmap_AddRef(&impl_from_IRedhawk_rmap(This)->IResourceMap_iface);
}
static ULONG WINAPI redhawk_rmap_Release(void *This)
{
    return rmap_Release(&impl_from_IRedhawk_rmap(This)->IResourceMap_iface);
}

static const void *redhawk_rmap_vtbl[] = {
    (void *)redhawk_rmap_QI,
    (void *)redhawk_rmap_AddRef,
    (void *)redhawk_rmap_Release,
    (void *)redhawk_interop_m3,
    (void *)redhawk_interop_m4,
    (void *)redhawk_interop_m5,
    (void *)redhawk_interop_m6,
    (void *)redhawk_interop_m7,
    (void *)redhawk_interop_m8,
    (void *)redhawk_interop_m9,
    (void *)redhawk_interop_m10,
    (void *)redhawk_interop_m11,
    (void *)redhawk_interop_m12,
    (void *)redhawk_interop_m13,
    (void *)redhawk_interop_m14,
    (void *)redhawk_interop_m15
};

static HRESULT WINAPI rmap_get_Uri(void *This, void **value) { if (value) *value = NULL; return S_OK; }
static HRESULT WINAPI rmap_GetValue(void *This, HSTRING resourceKey, void **value) { if (value) *value = NULL; return S_OK; }
static HRESULT WINAPI rmap_GetValueForContext(void *This, HSTRING resourceKey, void *context, void **value) { if (value) *value = NULL; return S_OK; }
static HRESULT WINAPI rmap_GetSubtree(void *This, HSTRING reference, void **value)
{
    if (!value) return E_POINTER;
    return create_resource_map_instance(value);
}

static const void *resource_map_vtbl[] = {
    (void *)rmap_QueryInterface,
    (void *)rmap_AddRef,
    (void *)rmap_Release,
    (void *)rmap_GetIids,
    (void *)rmap_GetRuntimeClassName,
    (void *)rmap_GetTrustLevel,
    (void *)rmap_get_Uri,
    (void *)rmap_GetValue,
    (void *)rmap_GetValueForContext,
    (void *)rmap_GetSubtree
};

static HRESULT create_resource_map_instance(void **value)
{
    struct resource_map_impl *impl = calloc(1, sizeof(*impl));
    if (!impl) return E_OUTOFMEMORY;
    impl->IResourceMap_iface.lpVtbl = resource_map_vtbl;
    impl->IRedhawkInterop_iface.lpVtbl = redhawk_rmap_vtbl;
    impl->ref = 1;
    *value = &impl->IResourceMap_iface;
    return S_OK;
}

/* -------------------------------------------------------------------------
 * ResourceManager Instance
 * ------------------------------------------------------------------------- */
struct resource_manager_impl
{
    struct { const void *lpVtbl; } IResourceManager_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    LONG ref;
};

#define impl_from_IResourceManager(ptr) ((struct resource_manager_impl *)((char *)(ptr) - offsetof(struct resource_manager_impl, IResourceManager_iface)))
#define impl_from_IRedhawk_rman(ptr)    ((struct resource_manager_impl *)((char *)(ptr) - offsetof(struct resource_manager_impl, IRedhawkInterop_iface)))

static HRESULT WINAPI rman_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    struct resource_manager_impl *impl = impl_from_IResourceManager(This);
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("rman %p, riid %s\n", This, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IResourceManager))
    {
        *ppvObject = &impl->IResourceManager_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ComWrappers_internal))
    {
        *ppvObject = &impl->IRedhawkInterop_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ComWrappers_internal2) ||
        IsEqualGUID(riid, &IID_IWeakReferenceSource))
    {
        return E_NOINTERFACE;
    }

    TRACE("rman %p: unhandled QI %s\n", This, debugstr_guid(riid));
    return E_NOINTERFACE;
}
static ULONG WINAPI rman_AddRef(void *This) { return InterlockedIncrement(&impl_from_IResourceManager(This)->ref); }
static ULONG WINAPI rman_Release(void *This)
{
    struct resource_manager_impl *impl = impl_from_IResourceManager(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}
static HRESULT WINAPI rman_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI rman_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(RuntimeClass_ResourceManager, wcslen(RuntimeClass_ResourceManager), n); }
static HRESULT WINAPI rman_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI redhawk_rman_QI(void *This, REFIID riid, void **ppvObject)
{
    return rman_QueryInterface(&impl_from_IRedhawk_rman(This)->IResourceManager_iface, riid, ppvObject);
}
static ULONG WINAPI redhawk_rman_AddRef(void *This)
{
    return rman_AddRef(&impl_from_IRedhawk_rman(This)->IResourceManager_iface);
}
static ULONG WINAPI redhawk_rman_Release(void *This)
{
    return rman_Release(&impl_from_IRedhawk_rman(This)->IResourceManager_iface);
}

static const void *redhawk_rman_vtbl[] = {
    (void *)redhawk_rman_QI,
    (void *)redhawk_rman_AddRef,
    (void *)redhawk_rman_Release,
    (void *)redhawk_interop_m3,
    (void *)redhawk_interop_m4,
    (void *)redhawk_interop_m5,
    (void *)redhawk_interop_m6,
    (void *)redhawk_interop_m7,
    (void *)redhawk_interop_m8,
    (void *)redhawk_interop_m9,
    (void *)redhawk_interop_m10,
    (void *)redhawk_interop_m11,
    (void *)redhawk_interop_m12,
    (void *)redhawk_interop_m13,
    (void *)redhawk_interop_m14,
    (void *)redhawk_interop_m15
};

static HRESULT WINAPI rman_get_MainResourceMap(void *This, void **value)
{
    if (!value) return E_POINTER;
    return create_resource_map_instance(value);
}
static HRESULT WINAPI rman_get_AllResourceMaps(void *This, void **value)
{
    if (!value) return E_POINTER;
    return create_resource_map_instance(value);
}
static HRESULT WINAPI rman_get_DefaultContext(void *This, void **value)
{
    if (!value) return E_POINTER;
    return create_resource_context_instance(value);
}
static HRESULT WINAPI rman_LoadPriFiles(void *This, void *files) { return S_OK; }
static HRESULT WINAPI rman_UnloadPriFiles(void *This, void *files) { return S_OK; }

static const void *resource_manager_vtbl[] = {
    (void *)rman_QueryInterface,
    (void *)rman_AddRef,
    (void *)rman_Release,
    (void *)rman_GetIids,
    (void *)rman_GetRuntimeClassName,
    (void *)rman_GetTrustLevel,
    (void *)rman_get_MainResourceMap,
    (void *)rman_get_AllResourceMaps,
    (void *)rman_get_DefaultContext,
    (void *)rman_LoadPriFiles,
    (void *)rman_UnloadPriFiles
};

static HRESULT create_resource_manager_instance(void **value)
{
    struct resource_manager_impl *impl = calloc(1, sizeof(*impl));
    if (!impl) return E_OUTOFMEMORY;
    impl->IResourceManager_iface.lpVtbl = resource_manager_vtbl;
    impl->IRedhawkInterop_iface.lpVtbl = redhawk_rman_vtbl;
    impl->ref = 1;
    *value = &impl->IResourceManager_iface;
    return S_OK;
}

/* -------------------------------------------------------------------------
 * ResourceManager Factory
 * ------------------------------------------------------------------------- */
struct resource_manager_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } IResourceManagerStatics_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
};

#define impl_from_IRmanActFac(ptr)  ((struct resource_manager_factory_impl *)((char *)(ptr) - offsetof(struct resource_manager_factory_impl, IActivationFactory_iface)))
#define impl_from_IRmanStatics(ptr) ((struct resource_manager_factory_impl *)((char *)(ptr) - offsetof(struct resource_manager_factory_impl, IResourceManagerStatics_iface)))
#define impl_from_IRedhawk_rfac(ptr) ((struct resource_manager_factory_impl *)((char *)(ptr) - offsetof(struct resource_manager_factory_impl, IRedhawkInterop_iface)))

static HRESULT rman_fac_QI_internal(struct resource_manager_factory_impl *impl, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("rman_fac %p, riid %s\n", impl, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppvObject = &impl->IActivationFactory_iface;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IResourceManagerStatics))
    {
        *ppvObject = &impl->IResourceManagerStatics_iface;
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ComWrappers_internal))
    {
        *ppvObject = &impl->IRedhawkInterop_iface;
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ComWrappers_internal2) ||
        IsEqualGUID(riid, &IID_IWeakReferenceSource))
    {
        return E_NOINTERFACE;
    }

    TRACE("rman_fac %p: unhandled QI %s\n", impl, debugstr_guid(riid));
    return E_NOINTERFACE;
}

static HRESULT WINAPI rman_fac_QueryInterface(IActivationFactory *iface, REFIID riid, void **ppvObject) { return rman_fac_QI_internal(impl_from_IRmanActFac(iface), riid, ppvObject); }
static ULONG WINAPI rman_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI rman_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI rman_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI rman_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n) { return WindowsCreateString(RuntimeClass_ResourceManager, wcslen(RuntimeClass_ResourceManager), n); }
static HRESULT WINAPI rman_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI rman_fac_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    if (!instance) return E_POINTER;
    return create_resource_manager_instance((void **)instance);
}

static const IActivationFactoryVtbl resource_manager_activation_factory_vtbl = {
    rman_fac_QueryInterface,
    rman_fac_AddRef,
    rman_fac_Release,
    rman_fac_GetIids,
    rman_fac_GetRuntimeClassName,
    rman_fac_GetTrustLevel,
    rman_fac_ActivateInstance
};

static HRESULT WINAPI rman_s1_QueryInterface(void *This, REFIID riid, void **ppvObject) { return rman_fac_QI_internal(impl_from_IRmanStatics(This), riid, ppvObject); }
static ULONG WINAPI rman_s1_AddRef(void *This) { return 2; }
static ULONG WINAPI rman_s1_Release(void *This) { return 1; }
static HRESULT WINAPI rman_s1_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI rman_s1_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(RuntimeClass_ResourceManager, wcslen(RuntimeClass_ResourceManager), n); }
static HRESULT WINAPI rman_s1_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI rman_s1_get_Current(void *This, void **value)
{
    if (!value) return E_POINTER;
    return create_resource_manager_instance(value);
}
static HRESULT WINAPI rman_s1_IsResourceNotFoundWarning(void *This, HRESULT hr, boolean *result) { if (result) *result = FALSE; return S_OK; }

static const void *resource_manager_statics_vtbl[] = {
    (void *)rman_s1_QueryInterface,
    (void *)rman_s1_AddRef,
    (void *)rman_s1_Release,
    (void *)rman_s1_GetIids,
    (void *)rman_s1_GetRuntimeClassName,
    (void *)rman_s1_GetTrustLevel,
    (void *)rman_s1_get_Current,                    /* Index 6 */
    (void *)rman_s1_IsResourceNotFoundWarning       /* Index 7 */
};

static HRESULT WINAPI redhawk_rfac_QI(void *This, REFIID riid, void **ppvObject)
{
    return rman_fac_QI_internal(impl_from_IRedhawk_rfac(This), riid, ppvObject);
}
static ULONG WINAPI redhawk_rfac_AddRef(void *This) { return 2; }
static ULONG WINAPI redhawk_rfac_Release(void *This) { return 1; }

static const void *redhawk_rfac_vtbl[] = {
    (void *)redhawk_rfac_QI,
    (void *)redhawk_rfac_AddRef,
    (void *)redhawk_rfac_Release,
    (void *)redhawk_interop_m3,
    (void *)redhawk_interop_m4,
    (void *)redhawk_interop_m5,
    (void *)redhawk_interop_m6,
    (void *)redhawk_interop_m7,
    (void *)redhawk_interop_m8,
    (void *)redhawk_interop_m9,
    (void *)redhawk_interop_m10,
    (void *)redhawk_interop_m11,
    (void *)redhawk_interop_m12,
    (void *)redhawk_interop_m13,
    (void *)redhawk_interop_m14,
    (void *)redhawk_interop_m15
};

static struct resource_manager_factory_impl default_resource_manager_factory = {
    {&resource_manager_activation_factory_vtbl},
    {resource_manager_statics_vtbl},
    {redhawk_rfac_vtbl}
};

IActivationFactory *resource_manager_factory = &default_resource_manager_factory.IActivationFactory_iface;
