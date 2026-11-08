/*
 * WinRT Windows.UI.Xaml.Application & Windows.UI.Xaml.Window Implementation
 *
 * Copyright (C) 2026 Julian / Xodus Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "private.h"
#include "winuser.h"
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(xaml);

static const GUID IID_IApplicationStatics = {0x06499997, 0xf7b4, 0x45fe, {0xb7, 0x63, 0x75, 0x77, 0xd1, 0xd3, 0xcb, 0x4a}};
static const GUID IID_IApplicationFactory = {0x93bbe361, 0xbe5a, 0x4ee3, {0xb4, 0xa3, 0x95, 0x11, 0x8d, 0xc9, 0x7a, 0x89}};
static const GUID IID_IApplication        = {0x74b861a1, 0x7487, 0x46a9, {0x9a, 0x6e, 0xc7, 0x8b, 0x51, 0x27, 0x26, 0xc5}};
static const GUID IID_IApplication2       = {0x019104be, 0x522a, 0x5904, {0xf5, 0x2f, 0xde, 0x72, 0x01, 0x04, 0x29, 0xe0}};
static const GUID IID_IApplicationOverrides = {0x25f99ff7, 0x9347, 0x459a, {0x9f, 0xac, 0xb2, 0xd0, 0xe1, 0x1c, 0x1a, 0x0f}};
static const GUID IID_IApplication_b775ad7c = {0xb775ad7c, 0x18b8, 0x45ca, {0xa1, 0xb0, 0xdc, 0x48, 0x3e, 0x4b, 0x10, 0x28}};

static const GUID IID_IWindowStatics      = {0x93328409, 0x4ea1, 0x4afa, {0x83, 0xdc, 0x0c, 0x4e, 0x73, 0xe8, 0x8b, 0xb1}};
static const GUID IID_IWindow             = {0x3e40409a, 0x0a1a, 0x4037, {0x87, 0x01, 0x43, 0x24, 0x81, 0x56, 0xa1, 0x48}};
static const GUID IID_IWindow_3276167d    = {0x3276167d, 0xc9f6, 0x462d, {0x9d, 0xe2, 0xae, 0x4c, 0x1f, 0xd8, 0xc2, 0xe5}};

static const GUID IID_ComWrappers_internal  = {0x11d3b13a, 0x180e, 0x4789, {0xa8, 0xbe, 0x77, 0x12, 0x88, 0x28, 0x93, 0xe6}};
static const GUID IID_ComWrappers_internal2 = {0x5c13e51c, 0x241d, 0x4859, {0x80, 0x05, 0x8a, 0x8d, 0x8d, 0x98, 0x15, 0xbf}};
static const GUID IID_IWeakReferenceSource  = {0x00000038, 0x0000, 0x0000, {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};

#define RuntimeClass_Application L"Windows.UI.Xaml.Application"
#define RuntimeClass_Window      L"Windows.UI.Xaml.Window"

/* Redhawk stubs */
static HRESULT WINAPI redhawk_interop_m3(void *This, void *a1, void *a2, void *a3) { return S_OK; }

/* -------------------------------------------------------------------------
 * Application Instance
 * ------------------------------------------------------------------------- */
struct application_impl
{
    struct { const void *lpVtbl; } IApplication_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    IInspectable *outer;
    LONG ref;
};

static void *current_application = NULL;

#define impl_from_IApplication(ptr) ((struct application_impl *)((char *)(ptr) - offsetof(struct application_impl, IApplication_iface)))
#define impl_from_IRedhawk_app(ptr) ((struct application_impl *)((char *)(ptr) - offsetof(struct application_impl, IRedhawkInterop_iface)))

static HRESULT WINAPI app_generic_stub(void *This)
{
    TRACE("app_generic_stub This %p, ret %p\n", This, __builtin_return_address(0));
    return S_OK;
}

static HRESULT WINAPI app_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    struct application_impl *impl = impl_from_IApplication(This);
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("app %p, riid %s\n", This, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IApplication) ||
        IsEqualGUID(riid, &IID_IApplication2) ||
        IsEqualGUID(riid, &IID_IApplicationOverrides) ||
        IsEqualGUID(riid, &IID_IApplication_b775ad7c))
    {
        *ppvObject = &impl->IApplication_iface;
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

    TRACE("app %p: unhandled QI %s\n", This, debugstr_guid(riid));
    return E_NOINTERFACE;
}
static ULONG WINAPI app_AddRef(void *This) { return InterlockedIncrement(&impl_from_IApplication(This)->ref); }
static ULONG WINAPI app_Release(void *This)
{
    struct application_impl *impl = impl_from_IApplication(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}
static HRESULT WINAPI app_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI app_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(RuntimeClass_Application, wcslen(RuntimeClass_Application), n); }
static HRESULT WINAPI app_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

HRESULT create_resource_dictionary_instance(void **value);
static HRESULT WINAPI app_get_Resources(void *This, void **value)
{
    TRACE("(%p, %p)\n", This, value);
    if (!value) return E_POINTER;
    return create_resource_dictionary_instance(value);
}
static HRESULT WINAPI app_put_Resources(void *This, void *value) { return S_OK; }
static HRESULT WINAPI app_get_DebugSettings(void *This, void **value) { if (value) *value = NULL; return S_OK; }
static HRESULT WINAPI app_get_RequestedTheme(void *This, int *value) { if (value) *value = 1; /* Dark */ return S_OK; }
static HRESULT WINAPI app_put_RequestedTheme(void *This, int value) { return S_OK; }
static HRESULT WINAPI app_add_event(void *This, void *h, EventRegistrationToken *t) { if (t) t->value = 1; return S_OK; }
static HRESULT WINAPI app_remove_event(void *This, EventRegistrationToken t) { return S_OK; }
static HRESULT WINAPI app_Exit(void *This) { return S_OK; }

static const void *application_vtbl[64] = {
    (void *)app_QueryInterface,           /* 0 */
    (void *)app_AddRef,                   /* 1 */
    (void *)app_Release,                  /* 2 */
    (void *)app_GetIids,                  /* 3 */
    (void *)app_GetRuntimeClassName,      /* 4 */
    (void *)app_GetTrustLevel,            /* 5 */
    (void *)app_get_Resources,            /* 6 */
    (void *)app_put_Resources,            /* 7 */
    (void *)app_get_DebugSettings,        /* 8 */
    (void *)app_get_RequestedTheme,       /* 9 */
    (void *)app_put_RequestedTheme,       /* 10 */
    (void *)app_add_event,                /* 11 */
    (void *)app_remove_event,             /* 12 */
    (void *)app_add_event,                /* 13 */
    (void *)app_remove_event,             /* 14 */
    (void *)app_add_event,                /* 15 */
    (void *)app_remove_event,             /* 16 */
    (void *)app_Exit,                     /* 17 */
    (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub,
    (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub,
    (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub,
    (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub,
    (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub,
    (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub,
    (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub,
    (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub,
    (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub,
    (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub,
    (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub,
    (void *)app_generic_stub, (void *)app_generic_stub
};

static HRESULT WINAPI redhawk_app_QI(void *This, REFIID riid, void **ppvObject)
{
    return app_QueryInterface(&impl_from_IRedhawk_app(This)->IApplication_iface, riid, ppvObject);
}
static ULONG WINAPI redhawk_app_AddRef(void *This) { return app_AddRef(&impl_from_IRedhawk_app(This)->IApplication_iface); }
static ULONG WINAPI redhawk_app_Release(void *This) { return app_Release(&impl_from_IRedhawk_app(This)->IApplication_iface); }

static const void *redhawk_app_vtbl[64] = {
    (void *)redhawk_app_QI,
    (void *)redhawk_app_AddRef,
    (void *)redhawk_app_Release,
    (void *)redhawk_interop_m3,
    (void *)app_GetRuntimeClassName,
    (void *)app_GetTrustLevel,
    (void *)app_get_Resources,
    (void *)app_put_Resources,
    (void *)app_get_DebugSettings,
    (void *)app_get_RequestedTheme,
    (void *)app_put_RequestedTheme,
    (void *)app_add_event,
    (void *)app_remove_event,
    (void *)app_add_event,
    (void *)app_remove_event,
    (void *)app_add_event,
    (void *)app_remove_event,
    (void *)app_Exit,
    (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub,
    (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub,
    (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub,
    (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub,
    (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub,
    (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub,
    (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub,
    (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub,
    (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub,
    (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub,
    (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub, (void *)app_generic_stub,
    (void *)app_generic_stub, (void *)app_generic_stub
};

void *global_xaml_metadata_provider = NULL;

HRESULT xaml_create_instance_by_name(const WCHAR *class_name, void **instance)
{
    HSTRING hstr = NULL;
    void *xamlType = NULL;
    void **prov_vtbl;
    HRESULT (WINAPI *pGetXamlTypeByName)(void *, HSTRING, void **);
    HRESULT hr;

    if (!instance) return E_POINTER;
    *instance = NULL;
    if (!global_xaml_metadata_provider) return E_FAIL;

    hr = WindowsCreateString(class_name, wcslen(class_name), &hstr);
    if (FAILED(hr)) return hr;

    prov_vtbl = *(void ***)global_xaml_metadata_provider;
    if (!prov_vtbl || (ULONG_PTR)prov_vtbl[7] <= 0x10000)
    {
        WindowsDeleteString(hstr);
        return E_FAIL;
    }

    pGetXamlTypeByName = (void *)prov_vtbl[7];
    hr = pGetXamlTypeByName(global_xaml_metadata_provider, hstr, &xamlType);
    WindowsDeleteString(hstr);

    TRACE("GetXamlTypeByName(%s) returned hr=0x%08lx, xamlType=%p\n", debugstr_w(class_name), hr, xamlType);
    if (SUCCEEDED(hr) && xamlType)
    {
        void **type_vtbl = *(void ***)xamlType;
        if (type_vtbl && (ULONG_PTR)type_vtbl[18] > 0x10000)
        {
            HRESULT (WINAPI *pActivateInstance)(void *, void **) = (void *)type_vtbl[18];
            hr = pActivateInstance(xamlType, instance);
            TRACE("ActivateInstance for %s returned hr=0x%08lx, instance=%p\n", debugstr_w(class_name), hr, *instance);
        }
        else
        {
            hr = E_FAIL;
        }
        IUnknown_Release((IUnknown *)xamlType);
        return hr;
    }
    return hr ? hr : E_FAIL;
}

static HRESULT create_application_instance(IInspectable *outer, IInspectable **inner, void **instance)
{
    struct application_impl *impl = calloc(1, sizeof(*impl));
    if (!impl) return E_OUTOFMEMORY;
    impl->IApplication_iface.lpVtbl = application_vtbl;
    impl->IRedhawkInterop_iface.lpVtbl = redhawk_app_vtbl;
    impl->outer = outer;
    impl->ref = 1;
    if (inner)
    {
        *inner = (IInspectable *)&impl->IApplication_iface;
        InterlockedIncrement(&impl->ref);
    }
    *instance = &impl->IApplication_iface;
    current_application = &impl->IApplication_iface;
    TRACE("Created Application instance %p (outer %p, inner %p)\n", *instance, outer, inner ? *inner : NULL);

    if (outer)
    {
        static const GUID IID_IXamlMetadataProvider_real = {0xb3765d69, 0x68a5, 0x4b32, {0x88, 0x61, 0xfd, 0xb9, 0x0c, 0x1f, 0x58, 0x36}};
        void **outer_vtbl = *(void ***)outer;
        if (outer_vtbl && (ULONG_PTR)outer_vtbl[0] > 0x10000)
        {
            HRESULT (WINAPI *pQI)(void *, REFIID, void **) = (void *)outer_vtbl[0];
            HRESULT hr_prov = pQI(outer, &IID_IXamlMetadataProvider_real, &global_xaml_metadata_provider);
            TRACE("pQI for IXamlMetadataProvider_real hr=0x%08lx, provider=%p\n", hr_prov, global_xaml_metadata_provider);
        }
    }
    return S_OK;
}

/* -------------------------------------------------------------------------
 * PropertyValue Double Implementation
 * ------------------------------------------------------------------------- */
static const GUID IID_IPropertyValue_local      = {0x4bd682dd, 0x7554, 0x40e9, {0x9a, 0x9b, 0x82, 0x65, 0x4e, 0xde, 0x7e, 0x62}};
static const GUID IID_IReference_DOUBLE_local  = {0x2f2d6c29, 0x5473, 0x5f3e, {0x92, 0xe7, 0x96, 0x57, 0x2b, 0xb9, 0x90, 0xe2}};

struct propval_double_impl
{
    struct { const void *lpVtbl; } IPropertyValue_iface;
    struct { const void *lpVtbl; } IReference_DOUBLE_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    double val;
    LONG ref;
};

#define impl_from_IPropVal(ptr) ((struct propval_double_impl *)((char *)(ptr) - offsetof(struct propval_double_impl, IPropertyValue_iface)))
#define impl_from_IRefDouble(ptr) ((struct propval_double_impl *)((char *)(ptr) - offsetof(struct propval_double_impl, IReference_DOUBLE_iface)))
#define impl_from_IRedhawkPropVal(ptr) ((struct propval_double_impl *)((char *)(ptr) - offsetof(struct propval_double_impl, IRedhawkInterop_iface)))

static HRESULT propval_QI_internal(struct propval_double_impl *impl, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("propval %p, riid %s\n", impl, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IPropertyValue_local))
    {
        *ppvObject = &impl->IPropertyValue_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IReference_DOUBLE_local))
    {
        *ppvObject = &impl->IReference_DOUBLE_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    TRACE("propval %p: unhandled QI %s\n", impl, debugstr_guid(riid));
    return E_NOINTERFACE;
}

static HRESULT WINAPI propval_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return propval_QI_internal(impl_from_IPropVal(This), riid, ppvObject);
}
static ULONG WINAPI propval_AddRef(void *This)
{
    return InterlockedIncrement(&impl_from_IPropVal(This)->ref);
}
static ULONG WINAPI propval_Release(void *This)
{
    struct propval_double_impl *impl = impl_from_IPropVal(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}
static HRESULT WINAPI propval_GetIids(void *This, ULONG *c, IID **i)
{
    IID *list;
    TRACE("(%p, %p, %p)\n", This, c, i);
    if (!c || !i) return E_POINTER;
    list = CoTaskMemAlloc(3 * sizeof(IID));
    if (!list) return E_OUTOFMEMORY;
    list[0] = IID_IPropertyValue_local;
    list[1] = IID_IAgileObject;
    list[2] = IID_IReference_DOUBLE_local;
    *c = 3;
    *i = list;
    return S_OK;
}
static HRESULT WINAPI propval_GetRuntimeClassName(void *This, HSTRING *n)
{
    TRACE("(%p, %p)\n", This, n);
    if (!n) return E_POINTER;
    return WindowsCreateString(L"Windows.Foundation.IReference`1<Double>", 39, n);
}
static HRESULT WINAPI propval_GetTrustLevel(void *This, TrustLevel *t)
{
    TRACE("(%p, %p)\n", This, t);
    if (!t) return E_POINTER;
    *t = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI propval_get_Type(void *This, int *value)
{
    TRACE("(%p, %p)\n", This, value);
    if (!value) return E_POINTER;
    *value = 10; /* PropertyType_Double */
    return S_OK;
}
static HRESULT WINAPI propval_get_IsNumericScalar(void *This, BOOLEAN *value)
{
    TRACE("(%p, %p)\n", This, value);
    if (!value) return E_POINTER;
    *value = TRUE;
    return S_OK;
}
static HRESULT WINAPI propval_GetSingle(void *This, float *value)
{
    struct propval_double_impl *impl = impl_from_IPropVal(This);
    TRACE("(%p, %p) -> %f\n", This, value, (float)impl->val);
    if (!value) return E_POINTER;
    *value = (float)impl->val;
    return S_OK;
}
static HRESULT WINAPI propval_GetDouble(void *This, double *value)
{
    struct propval_double_impl *impl = impl_from_IPropVal(This);
    TRACE("(%p, %p) -> %f\n", This, value, impl->val);
    if (!value) return E_POINTER;
    *value = impl->val;
    return S_OK;
}
static HRESULT WINAPI propval_GetInt32(void *This, int *value)
{
    struct propval_double_impl *impl = impl_from_IPropVal(This);
    TRACE("(%p, %p) -> %d\n", This, value, (int)impl->val);
    if (!value) return E_POINTER;
    *value = (int)impl->val;
    return S_OK;
}
static HRESULT WINAPI propval_generic_stub(void *This) { return E_NOTIMPL; }

static const void *propval_vtbl[64] = {
    (void *)propval_QueryInterface,       /* 0 */
    (void *)propval_AddRef,               /* 1 */
    (void *)propval_Release,              /* 2 */
    (void *)propval_GetIids,              /* 3 */
    (void *)propval_GetRuntimeClassName,  /* 4 */
    (void *)propval_GetTrustLevel,        /* 5 */
    (void *)propval_get_Type,             /* 6 */
    (void *)propval_get_IsNumericScalar,  /* 7 */
    (void *)propval_generic_stub,         /* 8: GetUInt8 */
    (void *)propval_generic_stub,         /* 9: GetInt16 */
    (void *)propval_generic_stub,         /* 10: GetUInt16 */
    (void *)propval_GetInt32,             /* 11: GetInt32 */
    (void *)propval_generic_stub,         /* 12: GetUInt32 */
    (void *)propval_generic_stub,         /* 13: GetInt64 */
    (void *)propval_generic_stub,         /* 14: GetUInt64 */
    (void *)propval_GetSingle,            /* 15: GetSingle */
    (void *)propval_GetDouble,            /* 16: GetDouble */
    (void *)propval_generic_stub,         /* 17: GetChar16 */
    (void *)propval_generic_stub,         /* 18: GetBoolean */
    (void *)propval_generic_stub,         /* 19: GetString */
    (void *)propval_GetDouble,            /* 20: slot 20 fallback */
    (void *)propval_generic_stub, (void *)propval_generic_stub, (void *)propval_generic_stub,
    (void *)propval_generic_stub, (void *)propval_generic_stub, (void *)propval_generic_stub, (void *)propval_generic_stub,
    (void *)propval_generic_stub, (void *)propval_generic_stub, (void *)propval_generic_stub, (void *)propval_generic_stub,
    (void *)propval_generic_stub, (void *)propval_generic_stub, (void *)propval_generic_stub, (void *)propval_generic_stub,
    (void *)propval_generic_stub, (void *)propval_generic_stub, (void *)propval_generic_stub, (void *)propval_generic_stub,
    (void *)propval_generic_stub, (void *)propval_generic_stub, (void *)propval_generic_stub, (void *)propval_generic_stub,
    (void *)propval_generic_stub, (void *)propval_generic_stub, (void *)propval_generic_stub, (void *)propval_generic_stub,
    (void *)propval_generic_stub, (void *)propval_generic_stub, (void *)propval_generic_stub, (void *)propval_generic_stub,
    (void *)propval_generic_stub, (void *)propval_generic_stub, (void *)propval_generic_stub, (void *)propval_generic_stub,
    (void *)propval_generic_stub, (void *)propval_generic_stub, (void *)propval_generic_stub, (void *)propval_generic_stub,
    (void *)propval_generic_stub, (void *)propval_generic_stub, (void *)propval_generic_stub, (void *)propval_generic_stub
};

/* IReference<DOUBLE> vtable */
static HRESULT WINAPI ref_double_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return propval_QI_internal(impl_from_IRefDouble(This), riid, ppvObject);
}
static ULONG WINAPI ref_double_AddRef(void *This)
{
    return InterlockedIncrement(&impl_from_IRefDouble(This)->ref);
}
static ULONG WINAPI ref_double_Release(void *This)
{
    return propval_Release(&impl_from_IRefDouble(This)->IPropertyValue_iface);
}
static HRESULT WINAPI ref_double_get_Value(void *This, double *value)
{
    struct propval_double_impl *impl = impl_from_IRefDouble(This);
    TRACE("(%p, %p) -> %f\n", This, value, impl->val);
    if (!value) return E_POINTER;
    *value = impl->val;
    return S_OK;
}

static const void *ref_double_vtbl[16] = {
    (void *)ref_double_QueryInterface,    /* 0 */
    (void *)ref_double_AddRef,            /* 1 */
    (void *)ref_double_Release,           /* 2 */
    (void *)propval_GetIids,              /* 3 */
    (void *)propval_GetRuntimeClassName,  /* 4 */
    (void *)propval_GetTrustLevel,        /* 5 */
    (void *)ref_double_get_Value,         /* 6: get_Value */
    (void *)propval_generic_stub, (void *)propval_generic_stub, (void *)propval_generic_stub,
    (void *)propval_generic_stub, (void *)propval_generic_stub, (void *)propval_generic_stub, (void *)propval_generic_stub
};

static HRESULT create_property_value_double(double val, void **value)
{
    struct propval_double_impl *impl;
    if (!value) return E_POINTER;
    impl = calloc(1, sizeof(*impl));
    if (!impl) return E_OUTOFMEMORY;
    impl->IPropertyValue_iface.lpVtbl = propval_vtbl;
    impl->IReference_DOUBLE_iface.lpVtbl = ref_double_vtbl;
    impl->val = val;
    impl->ref = 1;
    *value = &impl->IPropertyValue_iface;
    TRACE("Created PropertyValue double %f -> %p\n", val, *value);
    return S_OK;
}

/* -------------------------------------------------------------------------
 * ResourceDictionary Implementation
 * ------------------------------------------------------------------------- */
static const GUID IID_IResourceDictionary                 = {0xc1ea4f24, 0xd6de, 0x4191, {0x8e, 0x3a, 0xf4, 0x86, 0x01, 0xf7, 0x48, 0x9c}};
static const GUID IID_IResourceDictionary_old             = {0xc84c9ec8, 0x05b2, 0x48c3, {0xa9, 0xb8, 0x93, 0x98, 0x80, 0xdd, 0x24, 0x75}};
static const GUID IID_IMap_obj_obj                        = {0xf5f69427, 0x55ed, 0x5512, {0x84, 0x29, 0xd4, 0xf6, 0x62, 0x6d, 0xfc, 0xdd}};
static const GUID IID_IObservableMap_redhawk              = {0x1e036276, 0x2f60, 0x55f6, {0xb7, 0xf3, 0xf8, 0x60, 0x79, 0xe6, 0x90, 0x0b}};
static const GUID IID_FIMap_2_HSTRING_IInspectable        = {0x1b0d3570, 0x0877, 0x5ec2, {0x8a, 0x2c, 0x3b, 0x95, 0x39, 0x50, 0x6a, 0xca}};
static const GUID IID_FIObservableMap_2_HSTRING_IInspectable = {0x236aac9d, 0xfb12, 0x5c4d, {0xa4, 0x1c, 0x9e, 0x44, 0x5f, 0xb4, 0xd7, 0xec}};
static const GUID IID_FIMapView_2_HSTRING_IInspectable    = {0xbb78502a, 0xf79d, 0x54fa, {0x92, 0xc9, 0x90, 0xc5, 0x03, 0x9f, 0xdf, 0x7e}};
static const GUID IID_IMap_str_str1                       = {0x418bddce, 0x9ee6, 0x57fb, {0xa0, 0x6b, 0x88, 0x08, 0x88, 0xf3, 0x57, 0x99}};
static const GUID IID_IMap_str_str2                       = {0xa8a2794b, 0x143a, 0x5e78, {0x97, 0x82, 0x09, 0x56, 0x38, 0x9d, 0x7b, 0xdd}};
static const GUID IID_IObservableMap_str_str1             = {0x690184f4, 0x98d7, 0x5016, {0xa2, 0xb1, 0xb5, 0xff, 0x86, 0x19, 0x2b, 0x3a}};
static const GUID IID_IObservableMap_str_str2             = {0x6a4021a6, 0xe55c, 0x59e7, {0xab, 0xb4, 0xda, 0x42, 0xc9, 0x73, 0x11, 0x16}};

static const WCHAR *extract_key_string(void *key, HSTRING *out_hstr)
{
    const WCHAR *str = NULL;
    IInspectable *inspectable = key;
    IPropertyValue *propval = NULL;

    if (out_hstr) *out_hstr = NULL;
    if (!key) return NULL;

    if (IInspectable_QueryInterface(inspectable, &IID_IPropertyValue_local, (void **)&propval) == S_OK)
    {
        HSTRING hstr = NULL;
        if (SUCCEEDED(IPropertyValue_GetString(propval, &hstr)) && hstr)
        {
            str = WindowsGetStringRawBuffer(hstr, NULL);
            if (out_hstr) *out_hstr = hstr;
            else WindowsDeleteString(hstr);
        }
        IPropertyValue_Release(propval);
        if (str) return str;
    }

    str = WindowsGetStringRawBuffer((HSTRING)key, NULL);
    if (str && str[0] != 0 && str[0] < 0x80) return str;

    return NULL;
}

struct resource_dictionary_impl
{
    struct { const void *lpVtbl; } IResourceDictionary_iface;
    struct { const void *lpVtbl; } IObservableMap_iface;
    struct { const void *lpVtbl; } IMap_iface;
    struct { const void *lpVtbl; } IMapView_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    LONG ref;
};

#define impl_from_IResourceDictionary(ptr) ((struct resource_dictionary_impl *)((char *)(ptr) - offsetof(struct resource_dictionary_impl, IResourceDictionary_iface)))
#define impl_from_IObservableMap_rd(ptr)   ((struct resource_dictionary_impl *)((char *)(ptr) - offsetof(struct resource_dictionary_impl, IObservableMap_iface)))
#define impl_from_IMap_rd(ptr)             ((struct resource_dictionary_impl *)((char *)(ptr) - offsetof(struct resource_dictionary_impl, IMap_iface)))
#define impl_from_IMapView_rd(ptr)         ((struct resource_dictionary_impl *)((char *)(ptr) - offsetof(struct resource_dictionary_impl, IMapView_iface)))
#define impl_from_IRedhawk_rd(ptr)         ((struct resource_dictionary_impl *)((char *)(ptr) - offsetof(struct resource_dictionary_impl, IRedhawkInterop_iface)))

static HRESULT resdict_QI_internal(struct resource_dictionary_impl *impl, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("resdict %p, riid %s\n", impl, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IResourceDictionary) ||
        IsEqualGUID(riid, &IID_IResourceDictionary_old))
    {
        *ppvObject = &impl->IResourceDictionary_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IMap_obj_obj) ||
        IsEqualGUID(riid, &IID_FIMap_2_HSTRING_IInspectable) ||
        IsEqualGUID(riid, &IID_IMap_str_str1) ||
        IsEqualGUID(riid, &IID_IMap_str_str2))
    {
        *ppvObject = &impl->IMap_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IObservableMap_redhawk) ||
        IsEqualGUID(riid, &IID_FIObservableMap_2_HSTRING_IInspectable) ||
        IsEqualGUID(riid, &IID_IObservableMap_str_str1) ||
        IsEqualGUID(riid, &IID_IObservableMap_str_str2))
    {
        *ppvObject = &impl->IObservableMap_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_FIMapView_2_HSTRING_IInspectable))
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

    TRACE("resdict %p: unhandled QI %s\n", impl, debugstr_guid(riid));
    return E_NOINTERFACE;
}

static HRESULT WINAPI resdict_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return resdict_QI_internal(impl_from_IResourceDictionary(This), riid, ppvObject);
}
static ULONG WINAPI resdict_AddRef(void *This)
{
    return InterlockedIncrement(&impl_from_IResourceDictionary(This)->ref);
}
static ULONG WINAPI resdict_Release(void *This)
{
    struct resource_dictionary_impl *impl = impl_from_IResourceDictionary(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}
static HRESULT WINAPI resdict_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI resdict_GetRuntimeClassName(void *This, HSTRING *n)
{
    return WindowsCreateString(L"Windows.UI.Xaml.ResourceDictionary", 34, n);
}
static HRESULT WINAPI resdict_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI resdict_get_Source(void *This, void **value) { if (value) *value = NULL; return S_OK; }
static HRESULT WINAPI resdict_put_Source(void *This, void *value) { return S_OK; }
static HRESULT WINAPI resdict_get_MergedDictionaries(void *This, void **value) { if (value) *value = NULL; return S_OK; }
static HRESULT WINAPI resdict_get_ThemeDictionaries(void *This, void **value) { if (value) *value = NULL; return S_OK; }
static HRESULT WINAPI resdict_generic_stub(void *This) { return E_NOTIMPL; }

static const void *resource_dictionary_vtbl[64] = {
    (void *)resdict_QueryInterface,           /* 0 */
    (void *)resdict_AddRef,                   /* 1 */
    (void *)resdict_Release,                  /* 2 */
    (void *)resdict_GetIids,                  /* 3 */
    (void *)resdict_GetRuntimeClassName,      /* 4 */
    (void *)resdict_GetTrustLevel,            /* 5 */
    (void *)resdict_get_Source,               /* 6 */
    (void *)resdict_put_Source,               /* 7 */
    (void *)resdict_get_MergedDictionaries,   /* 8 */
    (void *)resdict_get_ThemeDictionaries,    /* 9 */
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub
};

/* IObservableMap methods */
static HRESULT WINAPI resdict_obs_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return resdict_QI_internal(impl_from_IObservableMap_rd(This), riid, ppvObject);
}
static ULONG WINAPI resdict_obs_AddRef(void *This)
{
    return InterlockedIncrement(&impl_from_IObservableMap_rd(This)->ref);
}
static ULONG WINAPI resdict_obs_Release(void *This)
{
    return resdict_Release(&impl_from_IObservableMap_rd(This)->IResourceDictionary_iface);
}
static HRESULT WINAPI resdict_obs_MapChanged(void *This, void *h, EventRegistrationToken *t)
{
    if (t) t->value = 1;
    return S_OK;
}
static HRESULT WINAPI resdict_obs_remove_MapChanged(void *This, EventRegistrationToken t) { return S_OK; }

static const void *resource_dictionary_obs_vtbl[64] = {
    (void *)resdict_obs_QueryInterface,       /* 0 */
    (void *)resdict_obs_AddRef,               /* 1 */
    (void *)resdict_obs_Release,              /* 2 */
    (void *)resdict_GetIids,                  /* 3 */
    (void *)resdict_GetRuntimeClassName,      /* 4 */
    (void *)resdict_GetTrustLevel,            /* 5 */
    (void *)resdict_obs_MapChanged,           /* 6 */
    (void *)resdict_obs_remove_MapChanged,    /* 7 */
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub
};

/* IMap methods */
static HRESULT WINAPI resdict_map_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return resdict_QI_internal(impl_from_IMap_rd(This), riid, ppvObject);
}
static ULONG WINAPI resdict_map_AddRef(void *This)
{
    return InterlockedIncrement(&impl_from_IMap_rd(This)->ref);
}
static ULONG WINAPI resdict_map_Release(void *This)
{
    return resdict_Release(&impl_from_IMap_rd(This)->IResourceDictionary_iface);
}

static HRESULT WINAPI resdict_map_Lookup(void *This, void *key, void **value)
{
    const WCHAR *str = NULL;
    HSTRING temp_hstr = NULL;
    TRACE("(%p, %p, %p)\n", This, key, value);
    if (!value) return E_POINTER;
    *value = NULL;

    str = extract_key_string(key, &temp_hstr);
    TRACE("resdict Lookup key string: %s\n", debugstr_w(str));

    if (str && !wcscmp(str, L"AppMinWindowWidth"))
    {
        if (temp_hstr) WindowsDeleteString(temp_hstr);
        return create_property_value_double(320.0, value);
    }
    if (str && !wcscmp(str, L"AppMinWindowHeight"))
    {
        if (temp_hstr) WindowsDeleteString(temp_hstr);
        return create_property_value_double(500.0, value);
    }
    if (str && !wcscmp(str, L"DisplayContextMenu"))
    {
        if (temp_hstr) WindowsDeleteString(temp_hstr);
        return create_menu_flyout_instance(value);
    }

    if (temp_hstr) WindowsDeleteString(temp_hstr);
    return create_property_value_double(0.0, value);
}

static HRESULT WINAPI resdict_map_get_Size(void *This, UINT32 *value)
{
    if (!value) return E_POINTER;
    *value = 2;
    return S_OK;
}

static HRESULT WINAPI resdict_map_HasKey(void *This, void *key, BOOLEAN *value)
{
    const WCHAR *str = NULL;
    HSTRING temp_hstr = NULL;
    TRACE("(%p, %p, %p)\n", This, key, value);
    if (!value) return E_POINTER;

    str = extract_key_string(key, &temp_hstr);
    if (str && (!wcscmp(str, L"AppMinWindowWidth") || !wcscmp(str, L"AppMinWindowHeight") || !wcscmp(str, L"DisplayContextMenu")))
        *value = TRUE;
    else
        *value = FALSE;

    if (temp_hstr) WindowsDeleteString(temp_hstr);
    return S_OK;
}

static HRESULT WINAPI resdict_map_GetView(void *This, void **value)
{
    struct resource_dictionary_impl *impl = impl_from_IMap_rd(This);
    if (!value) return E_POINTER;
    *value = &impl->IMapView_iface;
    InterlockedIncrement(&impl->ref);
    return S_OK;
}

static HRESULT WINAPI resdict_map_Insert(void *This, void *key, void *value, BOOLEAN *replaced)
{
    if (replaced) *replaced = FALSE;
    return S_OK;
}

static HRESULT WINAPI resdict_map_Remove(void *This, void *key) { return S_OK; }
static HRESULT WINAPI resdict_map_Clear(void *This) { return S_OK; }

static const void *resource_dictionary_map_vtbl[64] = {
    (void *)resdict_map_QueryInterface,       /* 0 */
    (void *)resdict_map_AddRef,               /* 1 */
    (void *)resdict_map_Release,              /* 2 */
    (void *)resdict_GetIids,                  /* 3 */
    (void *)resdict_GetRuntimeClassName,      /* 4 */
    (void *)resdict_GetTrustLevel,            /* 5 */
    (void *)resdict_map_Lookup,               /* 6 */
    (void *)resdict_map_get_Size,             /* 7 */
    (void *)resdict_map_HasKey,               /* 8 */
    (void *)resdict_map_GetView,              /* 9 */
    (void *)resdict_map_Insert,               /* 10 */
    (void *)resdict_map_Remove,               /* 11 */
    (void *)resdict_map_Clear,                /* 12 */
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub
};

/* IMapView methods */
static HRESULT WINAPI resdict_view_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return resdict_QI_internal(impl_from_IMapView_rd(This), riid, ppvObject);
}
static ULONG WINAPI resdict_view_AddRef(void *This)
{
    return InterlockedIncrement(&impl_from_IMapView_rd(This)->ref);
}
static ULONG WINAPI resdict_view_Release(void *This)
{
    return resdict_Release(&impl_from_IMapView_rd(This)->IResourceDictionary_iface);
}

static const void *resource_dictionary_view_vtbl[64] = {
    (void *)resdict_view_QueryInterface,      /* 0 */
    (void *)resdict_view_AddRef,              /* 1 */
    (void *)resdict_view_Release,             /* 2 */
    (void *)resdict_GetIids,                  /* 3 */
    (void *)resdict_GetRuntimeClassName,      /* 4 */
    (void *)resdict_GetTrustLevel,            /* 5 */
    (void *)resdict_map_Lookup,               /* 6 */
    (void *)resdict_map_get_Size,             /* 7 */
    (void *)resdict_map_HasKey,               /* 8 */
    (void *)resdict_generic_stub,             /* 9: Split */
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub, (void *)resdict_generic_stub,
    (void *)resdict_generic_stub, (void *)resdict_generic_stub
};

/* IRedhawkInterop */
static HRESULT WINAPI redhawk_resdict_QI(void *This, REFIID riid, void **ppvObject)
{
    return resdict_QI_internal(impl_from_IRedhawk_rd(This), riid, ppvObject);
}
static ULONG WINAPI redhawk_resdict_AddRef(void *This)
{
    return InterlockedIncrement(&impl_from_IRedhawk_rd(This)->ref);
}
static ULONG WINAPI redhawk_resdict_Release(void *This)
{
    return resdict_Release(&impl_from_IRedhawk_rd(This)->IResourceDictionary_iface);
}

static const void *redhawk_resdict_vtbl[16] = {
    (void *)redhawk_resdict_QI,
    (void *)redhawk_resdict_AddRef,
    (void *)redhawk_resdict_Release,
    (void *)redhawk_interop_m3,
    (void *)redhawk_interop_m3,
    (void *)redhawk_interop_m3,
    (void *)redhawk_interop_m3,
    (void *)redhawk_interop_m3, /* slot 7: safe hook invoked by Redhawk */
    (void *)redhawk_interop_m3, (void *)redhawk_interop_m3, (void *)redhawk_interop_m3, (void *)redhawk_interop_m3,
    (void *)redhawk_interop_m3, (void *)redhawk_interop_m3, (void *)redhawk_interop_m3, (void *)redhawk_interop_m3
};

HRESULT create_resource_dictionary_instance(void **value)
{
    struct resource_dictionary_impl *impl;
    if (!value) return E_POINTER;
    impl = calloc(1, sizeof(*impl));
    if (!impl) return E_OUTOFMEMORY;
    impl->IResourceDictionary_iface.lpVtbl = resource_dictionary_vtbl;
    impl->IObservableMap_iface.lpVtbl = resource_dictionary_obs_vtbl;
    impl->IMap_iface.lpVtbl = resource_dictionary_map_vtbl;
    impl->IMapView_iface.lpVtbl = resource_dictionary_view_vtbl;
    impl->IRedhawkInterop_iface.lpVtbl = redhawk_resdict_vtbl;
    impl->ref = 1;
    *value = &impl->IResourceDictionary_iface;
    TRACE("Created ResourceDictionary instance %p\n", *value);
    return S_OK;
}

/* -------------------------------------------------------------------------
 * Application Factory & Statics
 * ------------------------------------------------------------------------- */
struct application_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } IApplicationStatics_iface;
    struct { const void *lpVtbl; } IApplicationFactory_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    LONG ref;
};

#define impl_from_app_factory(ptr) ((struct application_factory_impl *)((char *)(ptr) - offsetof(struct application_factory_impl, IActivationFactory_iface)))
#define impl_from_app_statics(ptr) ((struct application_factory_impl *)((char *)(ptr) - offsetof(struct application_factory_impl, IApplicationStatics_iface)))
#define impl_from_app_creator(ptr) ((struct application_factory_impl *)((char *)(ptr) - offsetof(struct application_factory_impl, IApplicationFactory_iface)))
#define impl_from_app_redfac(ptr)  ((struct application_factory_impl *)((char *)(ptr) - offsetof(struct application_factory_impl, IRedhawkInterop_iface)))

static HRESULT app_fac_QI_internal(struct application_factory_impl *impl, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("app_fac %p, riid %s\n", impl, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppvObject = &impl->IActivationFactory_iface;
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IApplicationStatics))
    {
        *ppvObject = &impl->IApplicationStatics_iface;
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IApplicationFactory))
    {
        *ppvObject = &impl->IApplicationFactory_iface;
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

    TRACE("app_fac %p: unhandled QI %s\n", impl, debugstr_guid(riid));
    return E_NOINTERFACE;
}

static HRESULT WINAPI app_fac_QueryInterface(IActivationFactory *iface, REFIID riid, void **ppvObject)
{
    return app_fac_QI_internal(impl_from_app_factory(iface), riid, ppvObject);
}
static ULONG WINAPI app_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI app_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI app_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI app_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n) { return WindowsCreateString(RuntimeClass_Application, wcslen(RuntimeClass_Application), n); }
static HRESULT WINAPI app_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI app_fac_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    if (!instance) return E_POINTER;
    return create_application_instance(NULL, NULL, (void **)instance);
}

static const IActivationFactoryVtbl application_activation_factory_vtbl = {
    app_fac_QueryInterface,
    app_fac_AddRef,
    app_fac_Release,
    app_fac_GetIids,
    app_fac_GetRuntimeClassName,
    app_fac_GetTrustLevel,
    app_fac_ActivateInstance
};

/* IApplicationStatics */
static HRESULT WINAPI app_s_QueryInterface(void *This, REFIID riid, void **ppvObject) { return app_fac_QI_internal(impl_from_app_statics(This), riid, ppvObject); }
static ULONG WINAPI app_s_AddRef(void *This) { return 2; }
static ULONG WINAPI app_s_Release(void *This) { return 1; }
static HRESULT WINAPI app_s_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI app_s_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(RuntimeClass_Application, wcslen(RuntimeClass_Application), n); }
static HRESULT WINAPI app_s_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI app_s_get_Current(void *This, void **value)
{
    if (!value) return E_POINTER;
    if (!current_application)
    {
        create_application_instance(NULL, NULL, &current_application);
        TRACE("get_Current: created default application instance %p\n", current_application);
    }
    *value = current_application;
    TRACE("get_Current -> %p\n", *value);
    return S_OK;
}

static const GUID IID_IApplicationInitializationCallbackParams = {0x751b792e, 0x5772, 0x4488, {0x8b, 0x87, 0xf5, 0x47, 0xfa, 0xa6, 0x44, 0x74}};

struct callback_params_impl
{
    struct { const void *lpVtbl; } IParams_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    LONG ref;
};

#define impl_from_IParams(ptr) ((struct callback_params_impl *)((char *)(ptr) - offsetof(struct callback_params_impl, IParams_iface)))

static HRESULT WINAPI params_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    struct callback_params_impl *impl = impl_from_IParams(This);
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("params %p, riid %s\n", This, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IApplicationInitializationCallbackParams))
    {
        *ppvObject = &impl->IParams_iface;
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

static ULONG WINAPI params_AddRef(void *This)
{
    return InterlockedIncrement(&impl_from_IParams(This)->ref);
}

static ULONG WINAPI params_Release(void *This)
{
    struct callback_params_impl *impl = impl_from_IParams(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI params_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI params_GetRuntimeClassName(void *This, HSTRING *n)
{
    return WindowsCreateString(L"Windows.UI.Xaml.ApplicationInitializationCallbackParams", 55, n);
}
static HRESULT WINAPI params_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static const struct {
    HRESULT (WINAPI *QueryInterface)(void *, REFIID, void **);
    ULONG   (WINAPI *AddRef)(void *);
    ULONG   (WINAPI *Release)(void *);
    HRESULT (WINAPI *GetIids)(void *, ULONG *, IID **);
    HRESULT (WINAPI *GetRuntimeClassName)(void *, HSTRING *);
    HRESULT (WINAPI *GetTrustLevel)(void *, TrustLevel *);
} callback_params_vtbl = {
    params_QueryInterface,
    params_AddRef,
    params_Release,
    params_GetIids,
    params_GetRuntimeClassName,
    params_GetTrustLevel
};

static const void *redhawk_params_vtbl[16] = {
    params_QueryInterface, params_AddRef, params_Release,
    redhawk_interop_m3, redhawk_interop_m3, redhawk_interop_m3,
    redhawk_interop_m3, redhawk_interop_m3, redhawk_interop_m3,
    redhawk_interop_m3, redhawk_interop_m3, redhawk_interop_m3,
    redhawk_interop_m3, redhawk_interop_m3, redhawk_interop_m3,
    redhawk_interop_m3
};

static const GUID IID_IActivatedEventArgs = {0xcf651713, 0xcd08, 0x4fd8, {0xb6, 0x97, 0xa2, 0x81, 0xb6, 0x54, 0x4e, 0x2e}};
static const GUID IID_ILaunchActivatedEventArgs = {0xfbcbc926, 0xa18a, 0x4b3f, {0x82, 0xb0, 0x33, 0xbe, 0xe0, 0x20, 0x20, 0x52}};
static const GUID IID_IPrelaunchActivatedEventArgs  = {0x0c44717b, 0x19f7, 0x48d6, {0xb0, 0x46, 0xcf, 0x22, 0x82, 0x6e, 0xaa, 0x74}};
static const GUID IID_IPrelaunchActivatedEventArgs2 = {0xfbc93e26, 0xa14a, 0x4b4f, {0x82, 0xb0, 0x33, 0xbe, 0xd9, 0x20, 0xaf, 0x52}};
static const GUID IID_IActivatedEventArgsWithUser   = {0x1cf09b9e, 0x9962, 0x4936, {0x80, 0xff, 0xaf, 0xc8, 0xe8, 0xae, 0x5c, 0x8c}};
static const GUID IID_IUser                         = {0xdf9a26c6, 0xe746, 0x4bcd, {0xb5, 0xd4, 0x12, 0x01, 0x03, 0xc4, 0x20, 0x9b}};

struct launch_args_impl
{
    struct { const void *lpVtbl; } ILaunchArgs_iface;
    struct { const void *lpVtbl; } IActivatedArgs_iface;
    struct { const void *lpVtbl; } IPrelaunchArgs_iface;
    struct { const void *lpVtbl; } IUserArgs_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    LONG ref;
};

#define impl_from_ILaunchArgs(ptr)     ((struct launch_args_impl *)((char *)(ptr) - offsetof(struct launch_args_impl, ILaunchArgs_iface)))
#define impl_from_IActivatedArgs(ptr)  ((struct launch_args_impl *)((char *)(ptr) - offsetof(struct launch_args_impl, IActivatedArgs_iface)))
#define impl_from_IPrelaunchArgs(ptr)  ((struct launch_args_impl *)((char *)(ptr) - offsetof(struct launch_args_impl, IPrelaunchArgs_iface)))
#define impl_from_IUserArgs(ptr)       ((struct launch_args_impl *)((char *)(ptr) - offsetof(struct launch_args_impl, IUserArgs_iface)))
#define impl_from_IRedhawkArgs(ptr)    ((struct launch_args_impl *)((char *)(ptr) - offsetof(struct launch_args_impl, IRedhawkInterop_iface)))

static HRESULT WINAPI largs_generic_stub(void *This);

static HRESULT largs_QI_internal(struct launch_args_impl *impl, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("largs %p, riid %s\n", impl, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_ILaunchActivatedEventArgs))
    {
        *ppvObject = &impl->ILaunchArgs_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IActivatedEventArgs))
    {
        *ppvObject = &impl->IActivatedArgs_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IPrelaunchActivatedEventArgs) ||
        IsEqualGUID(riid, &IID_IPrelaunchActivatedEventArgs2))
    {
        *ppvObject = &impl->IPrelaunchArgs_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IActivatedEventArgsWithUser))
    {
        *ppvObject = &impl->IUserArgs_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ComWrappers_internal))
    {
        *ppvObject = &impl->IRedhawkInterop_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    TRACE("largs %p: unhandled QI %s\n", impl, debugstr_guid(riid));
    return E_NOINTERFACE;
}

static HRESULT WINAPI largs_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return largs_QI_internal(impl_from_ILaunchArgs(This), riid, ppvObject);
}
static ULONG WINAPI largs_AddRef(void *This)
{
    return InterlockedIncrement(&impl_from_ILaunchArgs(This)->ref);
}
static ULONG WINAPI largs_Release(void *This)
{
    struct launch_args_impl *impl = impl_from_ILaunchArgs(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI aargs_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return largs_QI_internal(impl_from_IActivatedArgs(This), riid, ppvObject);
}
static ULONG WINAPI aargs_AddRef(void *This)
{
    return InterlockedIncrement(&impl_from_IActivatedArgs(This)->ref);
}
static ULONG WINAPI aargs_Release(void *This)
{
    return largs_Release(&impl_from_IActivatedArgs(This)->ILaunchArgs_iface);
}

static HRESULT WINAPI pargs_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return largs_QI_internal(impl_from_IPrelaunchArgs(This), riid, ppvObject);
}
static ULONG WINAPI pargs_AddRef(void *This)
{
    return InterlockedIncrement(&impl_from_IPrelaunchArgs(This)->ref);
}
static ULONG WINAPI pargs_Release(void *This)
{
    return largs_Release(&impl_from_IPrelaunchArgs(This)->ILaunchArgs_iface);
}

static HRESULT WINAPI uargs_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return largs_QI_internal(impl_from_IUserArgs(This), riid, ppvObject);
}
static ULONG WINAPI uargs_AddRef(void *This)
{
    return InterlockedIncrement(&impl_from_IUserArgs(This)->ref);
}
static ULONG WINAPI uargs_Release(void *This)
{
    return largs_Release(&impl_from_IUserArgs(This)->ILaunchArgs_iface);
}

static HRESULT WINAPI largs_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI largs_GetRuntimeClassName(void *This, HSTRING *n)
{
    return WindowsCreateString(L"Windows.ApplicationModel.Activation.LaunchActivatedEventArgs", 60, n);
}
static HRESULT WINAPI largs_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI largs_get_Arguments(void *This, HSTRING *value)
{
    TRACE("get_Arguments\n");
    if (!value) return E_POINTER;
    return WindowsCreateString(L"", 0, value);
}

static HRESULT WINAPI largs_get_TileId(void *This, HSTRING *value)
{
    TRACE("get_TileId\n");
    if (!value) return E_POINTER;
    return WindowsCreateString(L"App", 3, value);
}

static HRESULT WINAPI largs_get_Kind(void *This, int *value)
{
    TRACE("get_Kind\n");
    if (!value) return E_POINTER;
    *value = 0; /* ActivationKind_Launch */
    return S_OK;
}

static HRESULT WINAPI largs_get_PreviousExecutionState(void *This, int *value)
{
    TRACE("get_PreviousExecutionState\n");
    if (!value) return E_POINTER;
    *value = 0; /* ApplicationExecutionState_NotRunning */
    return S_OK;
}

static const GUID IID_ISplashScreen = {0xca4d975c, 0xd4d6, 0x43f0, {0x97, 0xc0, 0x08, 0x33, 0xc6, 0x39, 0x1c, 0x24}};

struct splash_screen_impl
{
    struct { const void *lpVtbl; } ISplashScreen_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    LONG ref;
};

#define impl_from_ISplashScreen(ptr)  ((struct splash_screen_impl *)((char *)(ptr) - offsetof(struct splash_screen_impl, ISplashScreen_iface)))
#define impl_from_IRedhawkSplash(ptr) ((struct splash_screen_impl *)((char *)(ptr) - offsetof(struct splash_screen_impl, IRedhawkInterop_iface)))

static HRESULT splash_QI_internal(struct splash_screen_impl *impl, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("splash %p, riid %s\n", impl, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_ISplashScreen))
    {
        *ppvObject = &impl->ISplashScreen_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ComWrappers_internal))
    {
        *ppvObject = &impl->IRedhawkInterop_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    TRACE("splash %p: unhandled QI %s\n", impl, debugstr_guid(riid));
    return E_NOINTERFACE;
}

static HRESULT WINAPI splash_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return splash_QI_internal(impl_from_ISplashScreen(This), riid, ppvObject);
}
static ULONG WINAPI splash_AddRef(void *This)
{
    return InterlockedIncrement(&impl_from_ISplashScreen(This)->ref);
}
static ULONG WINAPI splash_Release(void *This)
{
    struct splash_screen_impl *impl = impl_from_ISplashScreen(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI redhawk_splash_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return splash_QI_internal(impl_from_IRedhawkSplash(This), riid, ppvObject);
}
static ULONG WINAPI redhawk_splash_AddRef(void *This)
{
    return InterlockedIncrement(&impl_from_IRedhawkSplash(This)->ref);
}
static ULONG WINAPI redhawk_splash_Release(void *This)
{
    return splash_Release(&impl_from_IRedhawkSplash(This)->ISplashScreen_iface);
}

static HRESULT WINAPI splash_GetRuntimeClassName(void *This, HSTRING *n)
{
    return WindowsCreateString(L"Windows.ApplicationModel.Activation.SplashScreen", 46, n);
}

struct win_rect { float x, y, width, height; };

static HRESULT WINAPI splash_get_ImageLocation(void *This, struct win_rect *value)
{
    TRACE("splash get_ImageLocation\n");
    if (!value) return E_POINTER;
    value->x = 0;
    value->y = 0;
    value->width = 640;
    value->height = 480;
    return S_OK;
}

static HRESULT WINAPI splash_add_Dismissed(void *This, void *handler, EventRegistrationToken *cookie)
{
    TRACE("splash add_Dismissed handler %p, ret %p\n", handler, __builtin_return_address(0));
    if (!cookie) return E_POINTER;
    cookie->value = 1;
    return S_OK;
}

static HRESULT WINAPI splash_remove_Dismissed(void *This, EventRegistrationToken cookie)
{
    TRACE("splash remove_Dismissed\n");
    return S_OK;
}

static const void *splash_screen_vtbl[16] = {
    (void *)splash_QueryInterface,
    (void *)splash_AddRef,
    (void *)splash_Release,
    (void *)largs_GetIids,
    (void *)splash_GetRuntimeClassName,
    (void *)largs_GetTrustLevel,
    (void *)splash_get_ImageLocation,
    (void *)splash_add_Dismissed,
    (void *)splash_remove_Dismissed,
    (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub,
    (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub
};

static const void *redhawk_splash_vtbl[16] = {
    (void *)redhawk_splash_QueryInterface,
    (void *)redhawk_splash_AddRef,
    (void *)redhawk_splash_Release,
    (void *)redhawk_interop_m3,
    (void *)splash_GetRuntimeClassName,
    (void *)largs_GetTrustLevel,
    (void *)largs_generic_stub,
    (void *)redhawk_interop_m3, /* slot 7: safe hook invoked by Redhawk */
    (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub,
    (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub
};

static HRESULT create_splash_screen_instance(void **out)
{
    struct splash_screen_impl *impl = calloc(1, sizeof(*impl));
    if (!impl) return E_OUTOFMEMORY;
    impl->ISplashScreen_iface.lpVtbl = splash_screen_vtbl;
    impl->IRedhawkInterop_iface.lpVtbl = redhawk_splash_vtbl;
    impl->ref = 1;
    *out = &impl->ISplashScreen_iface;
    TRACE("created splash screen instance %p\n", impl);
    return S_OK;
}

static HRESULT WINAPI largs_get_SplashScreen(void *This, void **value)
{
    TRACE("get_SplashScreen\n");
    if (!value) return E_POINTER;
    return create_splash_screen_instance(value);
}

static HRESULT WINAPI prelaunch_get_PrelaunchActivated(void *This, boolean *value)
{
    TRACE("get_PrelaunchActivated\n");
    if (!value) return E_POINTER;
    *value = FALSE;
    return S_OK;
}

static HRESULT WINAPI largs_generic_stub(void *This);

struct user_impl
{
    struct { const void *lpVtbl; } IUser_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    LONG ref;
};

#define impl_from_IUser(ptr)        ((struct user_impl *)((char *)(ptr) - offsetof(struct user_impl, IUser_iface)))
#define impl_from_IRedhawkUser(ptr) ((struct user_impl *)((char *)(ptr) - offsetof(struct user_impl, IRedhawkInterop_iface)))

static HRESULT user_QI_internal(struct user_impl *impl, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("user %p, riid %s\n", impl, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IUser))
    {
        *ppvObject = &impl->IUser_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ComWrappers_internal))
    {
        *ppvObject = &impl->IRedhawkInterop_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    TRACE("user %p: unhandled QI %s\n", impl, debugstr_guid(riid));
    return E_NOINTERFACE;
}

static HRESULT WINAPI user_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return user_QI_internal(impl_from_IUser(This), riid, ppvObject);
}
static ULONG WINAPI user_AddRef(void *This)
{
    return InterlockedIncrement(&impl_from_IUser(This)->ref);
}
static ULONG WINAPI user_Release(void *This)
{
    struct user_impl *impl = impl_from_IUser(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI redhawk_user_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return user_QI_internal(impl_from_IRedhawkUser(This), riid, ppvObject);
}
static ULONG WINAPI redhawk_user_AddRef(void *This)
{
    return InterlockedIncrement(&impl_from_IRedhawkUser(This)->ref);
}
static ULONG WINAPI redhawk_user_Release(void *This)
{
    return user_Release(&impl_from_IRedhawkUser(This)->IUser_iface);
}

static HRESULT WINAPI user_GetRuntimeClassName(void *This, HSTRING *n)
{
    return WindowsCreateString(L"Windows.System.User", 19, n);
}

static HRESULT WINAPI user_get_NonRoamableId(void *This, HSTRING *value)
{
    TRACE("user get_NonRoamableId\n");
    if (!value) return E_POINTER;
    return WindowsCreateString(L"DefaultUser", 11, value);
}

static HRESULT WINAPI user_get_AuthenticationStatus(void *This, int *value)
{
    TRACE("user get_AuthenticationStatus\n");
    if (!value) return E_POINTER;
    *value = 1; /* UserAuthenticationStatus_LocallyAuthenticated */
    return S_OK;
}

static HRESULT WINAPI user_get_Type(void *This, int *value)
{
    TRACE("user get_Type\n");
    if (!value) return E_POINTER;
    *value = 0; /* UserType_LocalUser */
    return S_OK;
}

static const void *user_vtbl[16] = {
    (void *)user_QueryInterface,
    (void *)user_AddRef,
    (void *)user_Release,
    (void *)largs_GetIids,
    (void *)user_GetRuntimeClassName,
    (void *)largs_GetTrustLevel,
    (void *)user_get_NonRoamableId,
    (void *)user_get_AuthenticationStatus,
    (void *)user_get_Type,
    (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub,
    (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub
};

static const void *redhawk_user_vtbl[16] = {
    (void *)redhawk_user_QueryInterface,
    (void *)redhawk_user_AddRef,
    (void *)redhawk_user_Release,
    (void *)redhawk_interop_m3,
    (void *)user_GetRuntimeClassName,
    (void *)largs_GetTrustLevel,
    (void *)largs_generic_stub,
    (void *)redhawk_interop_m3, /* slot 7: safe hook invoked by Redhawk */
    (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub,
    (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub
};

static HRESULT create_user_instance(void **out)
{
    struct user_impl *impl = calloc(1, sizeof(*impl));
    if (!impl) return E_OUTOFMEMORY;
    impl->IUser_iface.lpVtbl = user_vtbl;
    impl->IRedhawkInterop_iface.lpVtbl = redhawk_user_vtbl;
    impl->ref = 1;
    *out = &impl->IUser_iface;
    TRACE("created user instance %p\n", impl);
    return S_OK;
}

static HRESULT WINAPI user_get_User(void *This, void **value)
{
    TRACE("get_User\n");
    if (!value) return E_POINTER;
    return create_user_instance(value);
}

static HRESULT WINAPI largs_generic_stub(void *This)
{
    TRACE("largs_generic_stub This %p, ret %p\n", This, __builtin_return_address(0));
    return S_OK;
}

static const void *launch_args_vtbl[16] = {
    (void *)largs_QueryInterface,
    (void *)largs_AddRef,
    (void *)largs_Release,
    (void *)largs_GetIids,
    (void *)largs_GetRuntimeClassName,
    (void *)largs_GetTrustLevel,
    (void *)largs_get_Arguments,
    (void *)largs_get_TileId,
    (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub,
    (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub
};

static const void *activated_args_vtbl[16] = {
    (void *)aargs_QueryInterface,
    (void *)aargs_AddRef,
    (void *)aargs_Release,
    (void *)largs_GetIids,
    (void *)largs_GetRuntimeClassName,
    (void *)largs_GetTrustLevel,
    (void *)largs_get_Kind,
    (void *)largs_get_PreviousExecutionState,
    (void *)largs_get_SplashScreen,
    (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub,
    (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub
};

static const void *prelaunch_args_vtbl[16] = {
    (void *)pargs_QueryInterface,
    (void *)pargs_AddRef,
    (void *)pargs_Release,
    (void *)largs_GetIids,
    (void *)largs_GetRuntimeClassName,
    (void *)largs_GetTrustLevel,
    (void *)prelaunch_get_PrelaunchActivated,
    (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub,
    (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub,
    (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub
};

static const void *user_args_vtbl[16] = {
    (void *)uargs_QueryInterface,
    (void *)uargs_AddRef,
    (void *)uargs_Release,
    (void *)largs_GetIids,
    (void *)largs_GetRuntimeClassName,
    (void *)largs_GetTrustLevel,
    (void *)user_get_User,
    (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub,
    (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub,
    (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub
};

static const void *redhawk_largs_vtbl[16] = {
    (void *)largs_QueryInterface,
    (void *)largs_AddRef,
    (void *)largs_Release,
    (void *)redhawk_interop_m3,
    (void *)largs_GetRuntimeClassName,
    (void *)largs_GetTrustLevel,
    (void *)largs_generic_stub,
    (void *)redhawk_interop_m3, /* slot 7: safe hook invoked by Redhawk 0x180828fc0 */
    (void *)largs_generic_stub,
    (void *)largs_generic_stub,
    (void *)largs_generic_stub,
    (void *)largs_generic_stub,
    (void *)largs_generic_stub,
    (void *)largs_generic_stub,
    (void *)largs_generic_stub,
    (void *)largs_generic_stub
};

static DWORD xaml_ui_thread_id = 0;

static HRESULT WINAPI app_s_Start(void *This, void *callback)
{
    MSG msg;

    TRACE("Application::Start This %p, callback %p (UI thread %04lx)\n", This, callback, GetCurrentThreadId());
    xaml_ui_thread_id = GetCurrentThreadId();
    PeekMessageW(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);
    if ((ULONG_PTR)callback > 0x10000)
    {
        void **vtbl = *(void ***)callback;
        if (vtbl && (ULONG_PTR)vtbl > 0x10000)
        {
            HRESULT (WINAPI *invoke_fn)(void *This, void *params) = (void *)vtbl[3];
            TRACE("Invoking ApplicationInitializationCallback at %p...\n", invoke_fn);
            if ((ULONG_PTR)invoke_fn > 0x10000)
            {
                struct callback_params_impl *params = calloc(1, sizeof(*params));
                HRESULT hr;
                if (params)
                {
                    params->IParams_iface.lpVtbl = &callback_params_vtbl;
                    params->IRedhawkInterop_iface.lpVtbl = redhawk_params_vtbl;
                    params->ref = 1;
                }
                hr = invoke_fn(callback, params ? &params->IParams_iface : NULL);
                TRACE("ApplicationInitializationCallback returned hr 0x%08lx\n", hr);
            }
        }
    }

    /* Check outer application object and invoke OnLaunched */
    if (current_application)
    {
        struct application_impl *app_impl = impl_from_IApplication(current_application);
        void *outer = app_impl->outer;
        TRACE("Checking outer application object %p...\n", outer);
        if (outer)
        {
            void *overrides = NULL;
            void **outer_vtbl = *(void ***)outer;
            TRACE("outer application object is %p, *(void**)outer=%p\n", outer, outer_vtbl);
            if (outer_vtbl && (ULONG_PTR)outer_vtbl[0] > 0x10000)
            {
                HRESULT (WINAPI *pQI)(void *, REFIID, void **) = (void *)outer_vtbl[0];
                HRESULT hr = pQI(outer, &IID_IApplicationOverrides, &overrides);
                TRACE("pQI for IApplicationOverrides returned hr 0x%08lx, overrides %p\n", hr, overrides);
                if (SUCCEEDED(hr) && overrides)
                {
                    struct launch_args_impl *largs = calloc(1, sizeof(*largs));
                    void **ovtbl = *(void ***)overrides;
                    HRESULT (WINAPI *on_launched)(void *This, void *args);
                    if (largs)
                    {
                        largs->ILaunchArgs_iface.lpVtbl = launch_args_vtbl;
                        largs->IActivatedArgs_iface.lpVtbl = activated_args_vtbl;
                        largs->IPrelaunchArgs_iface.lpVtbl = prelaunch_args_vtbl;
                        largs->IUserArgs_iface.lpVtbl = user_args_vtbl;
                        largs->IRedhawkInterop_iface.lpVtbl = redhawk_largs_vtbl;
                        largs->ref = 1;
                    }
                    on_launched = (void *)ovtbl[7];
                    TRACE("Invoking OnLaunched at %p with args %p...\n", on_launched, largs);
                    hr = on_launched(overrides, largs ? &largs->ILaunchArgs_iface : NULL);
                    TRACE("OnLaunched returned hr 0x%08lx\n", hr);
                }
            }
        }
    }

    {
        MSG msg;
        TRACE("Entering Application::Start message loop...\n");
        while (GetMessageW(&msg, NULL, 0, 0))
        {
            if (msg.message == WM_APP + 10 && msg.wParam)
            {
                void *cb = (void *)msg.wParam;
                void **cb_vtbl = cb ? *(void ***)cb : NULL;
                TRACE("Invoking queued CoreDispatcher callback %p (vtbl %p)\n", cb, cb_vtbl);
                if (cb_vtbl && (uintptr_t)cb_vtbl[3] > 0x10000)
                {
                    HRESULT (WINAPI *pInvoke)(void *) = (void *)cb_vtbl[3];
                    HRESULT hr = pInvoke(cb);
                    TRACE("CoreDispatcher callback %p returned hr 0x%08lx\n", cb, hr);
                }
                IUnknown_Release((IUnknown *)cb);
                continue;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        TRACE("Exiting Application::Start message loop\n");
    }
    xaml_ui_thread_id = 0;
    return S_OK;
}

static HRESULT WINAPI app_s_LoadComponent(void *This, void *component, void *locator)
{
    TRACE("Application::LoadComponent component %p, locator %p\n", component, locator);
    return S_OK;
}
static HRESULT WINAPI app_s_LoadComponentWithResourceLocation(void *This, void *component, void *locator, int loc)
{
    TRACE("Application::LoadComponentWithResourceLocation component %p, locator %p, loc %d\n", component, locator, loc);
    return xaml_load_component(component, locator);
}

static const void *application_statics_vtbl[] = {
    (void *)app_s_QueryInterface,                     /* 0 */
    (void *)app_s_AddRef,                             /* 1 */
    (void *)app_s_Release,                            /* 2 */
    (void *)app_s_GetIids,                            /* 3 */
    (void *)app_s_GetRuntimeClassName,                /* 4 */
    (void *)app_s_GetTrustLevel,                      /* 5 */
    (void *)app_s_get_Current,                        /* 6 */
    (void *)app_s_Start,                              /* 7 */
    (void *)app_s_LoadComponent,                      /* 8 */
    (void *)app_s_LoadComponentWithResourceLocation,  /* 9 */
    (void *)app_s_LoadComponentWithResourceLocation,  /* 10 */
    (void *)app_s_LoadComponentWithResourceLocation,  /* 11 */
    (void *)app_s_LoadComponentWithResourceLocation,  /* 12 */
    (void *)app_s_LoadComponentWithResourceLocation,  /* 13 */
    (void *)app_s_LoadComponentWithResourceLocation,  /* 14 */
    (void *)app_s_LoadComponentWithResourceLocation   /* 15 */
};

/* IApplicationFactory */
static HRESULT WINAPI app_c_QueryInterface(void *This, REFIID riid, void **ppvObject) { return app_fac_QI_internal(impl_from_app_creator(This), riid, ppvObject); }
static ULONG WINAPI app_c_AddRef(void *This) { return 2; }
static ULONG WINAPI app_c_Release(void *This) { return 1; }
static HRESULT WINAPI app_c_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI app_c_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(RuntimeClass_Application, wcslen(RuntimeClass_Application), n); }
static HRESULT WINAPI app_c_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI app_c_CreateInstance(void *This, IInspectable *outer, IInspectable **inner, void **instance)
{
    return create_application_instance(outer, inner, instance);
}

static const void *application_factory_vtbl[] = {
    (void *)app_c_QueryInterface,                     /* 0 */
    (void *)app_c_AddRef,                             /* 1 */
    (void *)app_c_Release,                            /* 2 */
    (void *)app_c_GetIids,                            /* 3 */
    (void *)app_c_GetRuntimeClassName,                /* 4 */
    (void *)app_c_GetTrustLevel,                      /* 5 */
    (void *)app_c_CreateInstance,                     /* 6 */
    (void *)app_c_CreateInstance,                     /* 7 */
    (void *)app_c_CreateInstance,                     /* 8 */
    (void *)app_c_CreateInstance,                     /* 9 */
    (void *)app_c_CreateInstance,                     /* 10 */
    (void *)app_c_CreateInstance,                     /* 11 */
    (void *)app_c_CreateInstance,                     /* 12 */
    (void *)app_c_CreateInstance,                     /* 13 */
    (void *)app_c_CreateInstance,                     /* 14 */
    (void *)app_c_CreateInstance                      /* 15 */
};

static HRESULT WINAPI redhawk_app_fac_QI(void *This, REFIID riid, void **ppvObject) { return app_fac_QI_internal(impl_from_app_redfac(This), riid, ppvObject); }
static ULONG WINAPI redhawk_app_fac_AddRef(void *This) { return 2; }
static ULONG WINAPI redhawk_app_fac_Release(void *This) { return 1; }
static HRESULT WINAPI redhawk_app_fac_m(void *This)
{
    TRACE("redhawk_app_fac_m This %p, ret %p\n", This, __builtin_return_address(0));
    return S_OK;
}

static const void *redhawk_app_factory_vtbl[] = {
    (void *)redhawk_app_fac_QI,
    (void *)redhawk_app_fac_AddRef,
    (void *)redhawk_app_fac_Release,
    (void *)redhawk_interop_m3,
    (void *)app_s_GetRuntimeClassName,
    (void *)app_s_GetTrustLevel,
    (void *)app_s_get_Current,         /* Index 6: returns Application.Current */
    (void *)redhawk_app_fac_m,         /* Index 7: safe hook */
    (void *)redhawk_app_fac_m,         /* Index 8 */
    (void *)redhawk_app_fac_m,         /* Index 9 */
    (void *)redhawk_app_fac_m,         /* Index 10 */
    (void *)redhawk_app_fac_m,         /* Index 11 */
    (void *)redhawk_app_fac_m,         /* Index 12 */
    (void *)redhawk_app_fac_m,         /* Index 13 */
    (void *)redhawk_app_fac_m,         /* Index 14 */
    (void *)redhawk_app_fac_m          /* Index 15 */
};

static struct application_factory_impl default_application_factory = {
    {&application_activation_factory_vtbl},
    {application_statics_vtbl},
    {application_factory_vtbl},
    {redhawk_app_factory_vtbl},
    1
};

IActivationFactory *application_factory = &default_application_factory.IActivationFactory_iface;

/* -------------------------------------------------------------------------
 * AsyncAction Implementation
 * ------------------------------------------------------------------------- */
static const GUID IID_IAsyncAction_guid = {0x5a648006, 0x843a, 0x4da9, {0x86, 0x5b, 0x9d, 0x26, 0xe5, 0xdf, 0xad, 0x7b}};
static const GUID IID_IAsyncInfo_guid   = {0x00000036, 0x0000, 0x0000, {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};

struct async_action_impl
{
    struct { const void *lpVtbl; } IAsyncAction_iface;
    struct { const void *lpVtbl; } IAsyncInfo_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    LONG ref;
};

#define impl_from_IAsyncAction(ptr) ((struct async_action_impl *)((char *)(ptr) - offsetof(struct async_action_impl, IAsyncAction_iface)))
#define impl_from_IAsyncInfo(ptr)   ((struct async_action_impl *)((char *)(ptr) - offsetof(struct async_action_impl, IAsyncInfo_iface)))

static HRESULT async_action_QI_internal(struct async_action_impl *impl, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;
    TRACE("async_action %p, riid %s\n", impl, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IAsyncAction_guid))
    {
        *ppvObject = &impl->IAsyncAction_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IAsyncInfo_guid))
    {
        *ppvObject = &impl->IAsyncInfo_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_ComWrappers_internal) ||
        IsEqualGUID(riid, &IID_ComWrappers_internal2) ||
        IsEqualGUID(riid, &IID_IWeakReferenceSource))
    {
        return E_NOINTERFACE;
    }
    TRACE("async_action %p: unhandled QI %s\n", impl, debugstr_guid(riid));
    return E_NOINTERFACE;
}

static HRESULT WINAPI aa_QueryInterface(void *This, REFIID riid, void **ppv) { return async_action_QI_internal(impl_from_IAsyncAction(This), riid, ppv); }
static ULONG WINAPI aa_AddRef(void *This) { return InterlockedIncrement(&impl_from_IAsyncAction(This)->ref); }
static ULONG WINAPI aa_Release(void *This)
{
    struct async_action_impl *impl = impl_from_IAsyncAction(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    TRACE("async_action %p, ref %ld\n", impl, ref);
    if (!ref) free(impl);
    return ref;
}
static HRESULT WINAPI aa_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI aa_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.Foundation.IAsyncAction", 33, n); }
static HRESULT WINAPI aa_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI aa_put_Completed(void *This, void *handler)
{
    TRACE("IAsyncAction::put_Completed This %p, handler %p\n", This, handler);
    if (handler && (uintptr_t)handler > 0x10000)
    {
        void **h_vtbl = *(void ***)handler;
        if (h_vtbl && (uintptr_t)h_vtbl[3] > 0x10000)
        {
            HRESULT (WINAPI *pInvoke)(void *, void *, int) = (void *)h_vtbl[3];
            pInvoke(handler, This, 1 /* AsyncStatus_Completed */);
        }
    }
    return S_OK;
}
static HRESULT WINAPI aa_get_Completed(void *This, void **handler)
{
    TRACE("IAsyncAction::get_Completed This %p, handler %p\n", This, handler);
    if (handler && (uintptr_t)handler > 0x10000) *handler = NULL;
    return S_OK;
}
static HRESULT WINAPI aa_GetResults(void *This)
{
    TRACE("IAsyncAction::GetResults This %p\n", This);
    return S_OK;
}

static const void *async_action_vtbl[16] = {
    (void *)aa_QueryInterface, (void *)aa_AddRef, (void *)aa_Release,
    (void *)aa_GetIids, (void *)aa_GetRuntimeClassName, (void *)aa_GetTrustLevel,
    (void *)aa_put_Completed, (void *)aa_get_Completed, (void *)aa_GetResults
};

static HRESULT WINAPI ai_QueryInterface(void *This, REFIID riid, void **ppv) { return async_action_QI_internal(impl_from_IAsyncInfo(This), riid, ppv); }
static ULONG WINAPI ai_AddRef(void *This) { return aa_AddRef(&impl_from_IAsyncInfo(This)->IAsyncAction_iface); }
static ULONG WINAPI ai_Release(void *This) { return aa_Release(&impl_from_IAsyncInfo(This)->IAsyncAction_iface); }
static HRESULT WINAPI ai_get_Id(void *This, UINT32 *id)
{
    TRACE("IAsyncInfo::get_Id This %p, id %p\n", This, id);
    if (id && (uintptr_t)id > 0x10000) *id = 1;
    return S_OK;
}
static HRESULT WINAPI ai_get_Status(void *This, int *status)
{
    TRACE("IAsyncInfo::get_Status This %p, status %p\n", This, status);
    if (status && (uintptr_t)status > 0x10000) *status = 1 /* Completed */;
    return S_OK;
}
static HRESULT WINAPI ai_get_ErrorCode(void *This, HRESULT *code)
{
    TRACE("IAsyncInfo::get_ErrorCode This %p, code %p\n", This, code);
    if (code && (uintptr_t)code > 0x10000) *code = S_OK;
    return S_OK;
}
static HRESULT WINAPI ai_Cancel(void *This) { TRACE("IAsyncInfo::Cancel\n"); return S_OK; }
static HRESULT WINAPI ai_Close(void *This) { TRACE("IAsyncInfo::Close\n"); return S_OK; }

static const void *async_info_vtbl[16] = {
    (void *)ai_QueryInterface, (void *)ai_AddRef, (void *)ai_Release,
    (void *)aa_GetIids, (void *)aa_GetRuntimeClassName, (void *)aa_GetTrustLevel,
    (void *)ai_get_Id, (void *)ai_get_Status, (void *)ai_get_ErrorCode,
    (void *)ai_Cancel, (void *)ai_Close
};

static const void *redhawk_async_action_vtbl[16] = {
    (void *)aa_QueryInterface, (void *)aa_AddRef, (void *)aa_Release,
    (void *)redhawk_interop_m3, (void *)aa_GetRuntimeClassName, (void *)aa_GetTrustLevel,
    (void *)aa_put_Completed, (void *)aa_get_Completed, (void *)aa_GetResults
};

static HRESULT create_async_action(void **action)
{
    struct async_action_impl *impl;
    if (!action) return E_POINTER;
    impl = calloc(1, sizeof(*impl));
    if (!impl) return E_OUTOFMEMORY;
    impl->IAsyncAction_iface.lpVtbl = async_action_vtbl;
    impl->IAsyncInfo_iface.lpVtbl = async_info_vtbl;
    impl->IRedhawkInterop_iface.lpVtbl = redhawk_async_action_vtbl;
    impl->ref = 1;
    *action = &impl->IAsyncAction_iface;
    TRACE("Created AsyncAction instance %p\n", *action);
    return S_OK;
}

/* -------------------------------------------------------------------------
 * CoreDispatcher Implementation
 * ------------------------------------------------------------------------- */
static const GUID IID_ICoreDispatcher      = {0x60db2fa8, 0xb705, 0x4fde, {0xa7, 0xd6, 0xeb, 0xbb, 0x18, 0x91, 0xd3, 0x9e}};
static const GUID IID_ICoreAcceleratorKeys = {0x9ffdf7f5, 0xb8c9, 0x4ef0, {0xb7, 0xd2, 0x1d, 0xe6, 0x26, 0x56, 0x1f, 0xc8}};

struct dispatcher_impl
{
    struct { const void *lpVtbl; } ICoreDispatcher_iface;
    struct { const void *lpVtbl; } ICoreAcceleratorKeys_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    LONG ref;
};

static void *global_dispatcher = NULL;

#define impl_from_ICoreDispatcher(ptr) ((struct dispatcher_impl *)((char *)(ptr) - offsetof(struct dispatcher_impl, ICoreDispatcher_iface)))
#define impl_from_ICoreAccelKeys(ptr)  ((struct dispatcher_impl *)((char *)(ptr) - offsetof(struct dispatcher_impl, ICoreAcceleratorKeys_iface)))

static HRESULT dispatcher_QI_internal(struct dispatcher_impl *impl, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;
    TRACE("dispatcher %p, riid %s\n", impl, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_ICoreDispatcher))
    {
        *ppvObject = &impl->ICoreDispatcher_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_ICoreAcceleratorKeys))
    {
        *ppvObject = &impl->ICoreAcceleratorKeys_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_ComWrappers_internal) ||
        IsEqualGUID(riid, &IID_ComWrappers_internal2) ||
        IsEqualGUID(riid, &IID_IWeakReferenceSource))
    {
        return E_NOINTERFACE;
    }
    TRACE("dispatcher %p: unhandled QI %s\n", impl, debugstr_guid(riid));
    return E_NOINTERFACE;
}

static HRESULT WINAPI disp_QueryInterface(void *This, REFIID riid, void **ppv) { return dispatcher_QI_internal(impl_from_ICoreDispatcher(This), riid, ppv); }
static ULONG WINAPI disp_AddRef(void *This) { return InterlockedIncrement(&impl_from_ICoreDispatcher(This)->ref); }
static ULONG WINAPI disp_Release(void *This) { return InterlockedDecrement(&impl_from_ICoreDispatcher(This)->ref); }
static HRESULT WINAPI disp_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI disp_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Core.CoreDispatcher", 30, n); }
static HRESULT WINAPI disp_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI disp_get_HasThreadAccess(void *This, boolean *value)
{
    if (value)
    {
        DWORD tid = GetCurrentThreadId();
        *value = (xaml_ui_thread_id != 0) ? (tid == xaml_ui_thread_id) : TRUE;
    }
    return S_OK;
}
static HRESULT WINAPI disp_ProcessEvents(void *This, int options)
{
    MSG msg;
    while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_APP + 10 && msg.wParam)
        {
            void *cb = (void *)msg.wParam;
            void **cb_vtbl = cb ? *(void ***)cb : NULL;
            if (cb_vtbl && (uintptr_t)cb_vtbl[3] > 0x10000)
            {
                HRESULT (WINAPI *pInvoke)(void *) = (void *)cb_vtbl[3];
                pInvoke(cb);
            }
            IUnknown_Release((IUnknown *)cb);
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
        if (options == 2 /* ProcessOneIfPresent */) break;
    }
    return S_OK;
}
static HRESULT WINAPI disp_RunAsync(void *This, int priority, void *callback, void **action)
{
    DWORD target_thread = xaml_ui_thread_id ? xaml_ui_thread_id : GetCurrentThreadId();
    TRACE("CoreDispatcher::RunAsync priority %d, callback %p -> target_thread %04lx (current %04lx)\n",
          priority, callback, target_thread, GetCurrentThreadId());
    if (callback)
    {
        IUnknown_AddRef((IUnknown *)callback);
        if (!PostThreadMessageW(target_thread, WM_APP + 10, (WPARAM)callback, 0))
        {
            WARN("PostThreadMessageW to thread %04lx failed with error %lu\n", target_thread, GetLastError());
            IUnknown_Release((IUnknown *)callback);
        }
    }
    if (action) create_async_action(action);
    return S_OK;
}
static HRESULT WINAPI disp_RunIdleAsync(void *This, void *callback, void **action)
{
    DWORD target_thread = xaml_ui_thread_id ? xaml_ui_thread_id : GetCurrentThreadId();
    TRACE("CoreDispatcher::RunIdleAsync callback %p -> target_thread %04lx (current %04lx)\n",
          callback, target_thread, GetCurrentThreadId());
    if (callback)
    {
        IUnknown_AddRef((IUnknown *)callback);
        if (!PostThreadMessageW(target_thread, WM_APP + 10, (WPARAM)callback, 0))
        {
            WARN("PostThreadMessageW to thread %04lx failed with error %lu\n", target_thread, GetLastError());
            IUnknown_Release((IUnknown *)callback);
        }
    }
    if (action) create_async_action(action);
    return S_OK;
}

static const void *core_dispatcher_vtbl[16] = {
    (void *)disp_QueryInterface, (void *)disp_AddRef, (void *)disp_Release,
    (void *)disp_GetIids, (void *)disp_GetRuntimeClassName, (void *)disp_GetTrustLevel,
    (void *)disp_get_HasThreadAccess,
    (void *)disp_ProcessEvents,
    (void *)disp_RunAsync,
    (void *)disp_RunIdleAsync
};

static HRESULT WINAPI accel_QueryInterface(void *This, REFIID riid, void **ppv) { return dispatcher_QI_internal(impl_from_ICoreAccelKeys(This), riid, ppv); }
static ULONG WINAPI accel_AddRef(void *This) { return disp_AddRef(&impl_from_ICoreAccelKeys(This)->ICoreDispatcher_iface); }
static ULONG WINAPI accel_Release(void *This) { return disp_Release(&impl_from_ICoreAccelKeys(This)->ICoreDispatcher_iface); }
static HRESULT WINAPI accel_add_AcceleratorKeyActivated(void *This, void *handler, INT64 *cookie) { if (cookie) *cookie = 1; return S_OK; }
static HRESULT WINAPI accel_remove_AcceleratorKeyActivated(void *This, INT64 cookie) { return S_OK; }

static const void *core_accel_keys_vtbl[16] = {
    (void *)accel_QueryInterface, (void *)accel_AddRef, (void *)accel_Release,
    (void *)disp_GetIids, (void *)disp_GetRuntimeClassName, (void *)disp_GetTrustLevel,
    (void *)accel_add_AcceleratorKeyActivated,
    (void *)accel_remove_AcceleratorKeyActivated
};

static const void *redhawk_dispatcher_vtbl[16] = {
    (void *)disp_QueryInterface, (void *)disp_AddRef, (void *)disp_Release,
    (void *)redhawk_interop_m3, (void *)disp_GetRuntimeClassName, (void *)disp_GetTrustLevel,
    (void *)disp_get_HasThreadAccess,
    (void *)disp_ProcessEvents,
    (void *)disp_RunAsync,
    (void *)disp_RunIdleAsync
};

HRESULT get_global_dispatcher(void **value)
{
    struct dispatcher_impl *impl;
    if (!value) return E_POINTER;
    if (global_dispatcher)
    {
        impl = impl_from_ICoreDispatcher(global_dispatcher);
        InterlockedIncrement(&impl->ref);
        *value = global_dispatcher;
        return S_OK;
    }
    impl = calloc(1, sizeof(*impl));
    if (!impl) return E_OUTOFMEMORY;
    impl->ICoreDispatcher_iface.lpVtbl = core_dispatcher_vtbl;
    impl->ICoreAcceleratorKeys_iface.lpVtbl = core_accel_keys_vtbl;
    impl->IRedhawkInterop_iface.lpVtbl = redhawk_dispatcher_vtbl;
    impl->ref = 10000; /* Permanent singleton */
    *value = &impl->ICoreDispatcher_iface;
    global_dispatcher = *value;
    return S_OK;
}

/* -------------------------------------------------------------------------
 * Window Instance & Statics Declarations
 * ------------------------------------------------------------------------- */
static const IID IID_IWindow2 = {0xd384759f, 0x34f6, 0x4482, {0x84,0x35,0xf5,0x52,0xf9,0xb2,0x4c,0xc8}};
static const IID IID_IWindow3 = {0xb70bdc9d, 0x1c35, 0x462a, {0x9b,0x97,0x80,0x8d,0x5a,0xf9,0xf2,0x8e}};

struct window_impl
{
    struct { const void *lpVtbl; } IWindow_iface;
    struct { const void *lpVtbl; } IWindow2_iface;
    struct { const void *lpVtbl; } IWindow3_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    HWND hwnd;
    void *content;
    void *titlebar_element;
    LONG ref;
};

static void *current_window = NULL;

#define impl_from_IWindow(ptr) ((struct window_impl *)((char *)(ptr) - offsetof(struct window_impl, IWindow_iface)))
#define impl_from_IWindow2(ptr) ((struct window_impl *)((char *)(ptr) - offsetof(struct window_impl, IWindow2_iface)))
#define impl_from_IWindow3(ptr) ((struct window_impl *)((char *)(ptr) - offsetof(struct window_impl, IWindow3_iface)))
#define impl_from_IRedhawk_win(ptr) ((struct window_impl *)((char *)(ptr) - offsetof(struct window_impl, IRedhawkInterop_iface)))

/* -------------------------------------------------------------------------
 * CoreWindow Implementation
 * ------------------------------------------------------------------------- */
static const GUID IID_ICoreWindow = {0x79b9d5f2, 0x879e, 0x4b89, {0xb7, 0x98, 0x79, 0xe4, 0x75, 0x98, 0x03, 0x0c}};
static const GUID IID_ICoreWindowEventArgs = {0x272b1ef3, 0xc633, 0x4da5, {0xa2, 0x6c, 0xc6, 0xd0, 0xf5, 0x6b, 0x29, 0xda}};
static const GUID IID_IWindowActivatedEventArgs = {0x179d65e7, 0x4658, 0x4cb6, {0xaa, 0x13, 0x41, 0xd0, 0x94, 0xea, 0x25, 0x5e}};
static const GUID IID_IVisibilityChangedEventArgs = {0xbf9918ea, 0xd801, 0x4564, {0xa4, 0x95, 0xb1, 0xe8, 0x4f, 0x8a, 0xd0, 0x85}};
static const GUID IID_IWindowSizeChangedEventArgs = {0x5a200ec7, 0x0426, 0x47dc, {0xb8, 0x6c, 0x6f, 0x47, 0x59, 0x15, 0xe4, 0x51}};

struct core_window_handler
{
    INT64 token;
    IUnknown *handler;
};

#define MAX_CW_HANDLERS 16

struct core_window_impl
{
    struct { const void *lpVtbl; } ICoreWindow_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    LONG ref;
    struct core_window_handler activated_handlers[MAX_CW_HANDLERS];
    int activated_count;
    struct core_window_handler size_changed_handlers[MAX_CW_HANDLERS];
    int size_changed_count;
    struct core_window_handler visibility_changed_handlers[MAX_CW_HANDLERS];
    int visibility_changed_count;
    struct core_window_handler pointer_pressed_handlers[MAX_CW_HANDLERS];
    int pointer_pressed_count;
    struct core_window_handler pointer_released_handlers[MAX_CW_HANDLERS];
    int pointer_released_count;
    struct core_window_handler pointer_moved_handlers[MAX_CW_HANDLERS];
    int pointer_moved_count;
    struct core_window_handler pointer_wheel_handlers[MAX_CW_HANDLERS];
    int pointer_wheel_count;
    struct core_window_handler pointer_entered_handlers[MAX_CW_HANDLERS];
    int pointer_entered_count;
    struct core_window_handler pointer_exited_handlers[MAX_CW_HANDLERS];
    int pointer_exited_count;
    struct core_window_handler key_down_handlers[MAX_CW_HANDLERS];
    int key_down_count;
    struct core_window_handler key_up_handlers[MAX_CW_HANDLERS];
    int key_up_count;
    struct core_window_handler character_received_handlers[MAX_CW_HANDLERS];
    int character_received_count;
    INT64 next_token;
};

static void *global_core_window = NULL;

#define impl_from_ICoreWindow(ptr) ((struct core_window_impl *)((char *)(ptr) - offsetof(struct core_window_impl, ICoreWindow_iface)))

/* Event args implementations */
struct window_size_args
{
    struct { const void *lpVtbl; } IWindowSizeChangedEventArgs_iface;
    struct { const void *lpVtbl; } ICoreWindowEventArgs_iface;
    LONG ref;
    FLOAT width;
    FLOAT height;
    boolean handled;
};

#define impl_from_IWindowSizeChangedEventArgs(ptr) ((struct window_size_args *)((char *)(ptr) - offsetof(struct window_size_args, IWindowSizeChangedEventArgs_iface)))
#define impl_from_ICoreWindowEventArgs_size(ptr)   ((struct window_size_args *)((char *)(ptr) - offsetof(struct window_size_args, ICoreWindowEventArgs_iface)))

static HRESULT WINAPI size_args_QI(void *This, REFIID riid, void **ppv)
{
    struct window_size_args *impl = impl_from_IWindowSizeChangedEventArgs(This);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IWindowSizeChangedEventArgs))
    {
        *ppv = &impl->IWindowSizeChangedEventArgs_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_ICoreWindowEventArgs))
    {
        *ppv = &impl->ICoreWindowEventArgs_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI size_args_AddRef(void *This) { return InterlockedIncrement(&impl_from_IWindowSizeChangedEventArgs(This)->ref); }
static ULONG WINAPI size_args_Release(void *This)
{
    struct window_size_args *impl = impl_from_IWindowSizeChangedEventArgs(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}
static HRESULT WINAPI size_args_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI size_args_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Core.WindowSizeChangedEventArgs", 41, n); }
static HRESULT WINAPI size_args_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI size_args_get_Size(void *This, void *value)
{
    struct window_size_args *impl = impl_from_IWindowSizeChangedEventArgs(This);
    struct { FLOAT Width; FLOAT Height; } *sz = value;
    if (sz) { sz->Width = impl->width; sz->Height = impl->height; }
    return S_OK;
}

static const void *size_args_vtbl[7] = {
    (void *)size_args_QI, (void *)size_args_AddRef, (void *)size_args_Release,
    (void *)size_args_GetIids, (void *)size_args_GetRuntimeClassName, (void *)size_args_GetTrustLevel,
    (void *)size_args_get_Size
};

static HRESULT WINAPI size_core_QI(void *This, REFIID riid, void **ppv)
{
    struct window_size_args *impl = impl_from_ICoreWindowEventArgs_size(This);
    return size_args_QI(&impl->IWindowSizeChangedEventArgs_iface, riid, ppv);
}
static ULONG WINAPI size_core_AddRef(void *This)
{
    struct window_size_args *impl = impl_from_ICoreWindowEventArgs_size(This);
    return size_args_AddRef(&impl->IWindowSizeChangedEventArgs_iface);
}
static ULONG WINAPI size_core_Release(void *This)
{
    struct window_size_args *impl = impl_from_ICoreWindowEventArgs_size(This);
    return size_args_Release(&impl->IWindowSizeChangedEventArgs_iface);
}
static HRESULT WINAPI size_core_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI size_core_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Core.WindowSizeChangedEventArgs", 41, n); }
static HRESULT WINAPI size_core_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI size_core_get_Handled(void *This, boolean *value)
{
    struct window_size_args *impl = impl_from_ICoreWindowEventArgs_size(This);
    if (value) *value = impl->handled;
    return S_OK;
}
static HRESULT WINAPI size_core_put_Handled(void *This, boolean value)
{
    struct window_size_args *impl = impl_from_ICoreWindowEventArgs_size(This);
    impl->handled = value;
    return S_OK;
}

static const void *size_core_args_vtbl[8] = {
    (void *)size_core_QI, (void *)size_core_AddRef, (void *)size_core_Release,
    (void *)size_core_GetIids, (void *)size_core_GetRuntimeClassName, (void *)size_core_GetTrustLevel,
    (void *)size_core_get_Handled, (void *)size_core_put_Handled
};

struct window_activated_args
{
    struct { const void *lpVtbl; } IWindowActivatedEventArgs_iface;
    struct { const void *lpVtbl; } ICoreWindowEventArgs_iface;
    LONG ref;
    int state;
    boolean handled;
};

#define impl_from_IWindowActivatedEventArgs(ptr) ((struct window_activated_args *)((char *)(ptr) - offsetof(struct window_activated_args, IWindowActivatedEventArgs_iface)))
#define impl_from_ICoreWindowEventArgs_act(ptr)   ((struct window_activated_args *)((char *)(ptr) - offsetof(struct window_activated_args, ICoreWindowEventArgs_iface)))

static HRESULT WINAPI act_args_QI(void *This, REFIID riid, void **ppv)
{
    struct window_activated_args *impl = impl_from_IWindowActivatedEventArgs(This);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IWindowActivatedEventArgs))
    {
        *ppv = &impl->IWindowActivatedEventArgs_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_ICoreWindowEventArgs))
    {
        *ppv = &impl->ICoreWindowEventArgs_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI act_args_AddRef(void *This) { return InterlockedIncrement(&impl_from_IWindowActivatedEventArgs(This)->ref); }
static ULONG WINAPI act_args_Release(void *This)
{
    struct window_activated_args *impl = impl_from_IWindowActivatedEventArgs(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}
static HRESULT WINAPI act_args_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI act_args_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Core.WindowActivatedEventArgs", 39, n); }
static HRESULT WINAPI act_args_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI act_args_get_WindowActivationState(void *This, int *value)
{
    struct window_activated_args *impl = impl_from_IWindowActivatedEventArgs(This);
    if (value) *value = impl->state;
    return S_OK;
}

static const void *act_args_vtbl[7] = {
    (void *)act_args_QI, (void *)act_args_AddRef, (void *)act_args_Release,
    (void *)act_args_GetIids, (void *)act_args_GetRuntimeClassName, (void *)act_args_GetTrustLevel,
    (void *)act_args_get_WindowActivationState
};

static HRESULT WINAPI act_core_QI(void *This, REFIID riid, void **ppv)
{
    struct window_activated_args *impl = impl_from_ICoreWindowEventArgs_act(This);
    return act_args_QI(&impl->IWindowActivatedEventArgs_iface, riid, ppv);
}
static ULONG WINAPI act_core_AddRef(void *This)
{
    struct window_activated_args *impl = impl_from_ICoreWindowEventArgs_act(This);
    return act_args_AddRef(&impl->IWindowActivatedEventArgs_iface);
}
static ULONG WINAPI act_core_Release(void *This)
{
    struct window_activated_args *impl = impl_from_ICoreWindowEventArgs_act(This);
    return act_args_Release(&impl->IWindowActivatedEventArgs_iface);
}
static HRESULT WINAPI act_core_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI act_core_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Core.WindowActivatedEventArgs", 39, n); }
static HRESULT WINAPI act_core_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI act_core_get_Handled(void *This, boolean *value)
{
    struct window_activated_args *impl = impl_from_ICoreWindowEventArgs_act(This);
    if (value) *value = impl->handled;
    return S_OK;
}
static HRESULT WINAPI act_core_put_Handled(void *This, boolean value)
{
    struct window_activated_args *impl = impl_from_ICoreWindowEventArgs_act(This);
    impl->handled = value;
    return S_OK;
}

static const void *act_core_args_vtbl[8] = {
    (void *)act_core_QI, (void *)act_core_AddRef, (void *)act_core_Release,
    (void *)act_core_GetIids, (void *)act_core_GetRuntimeClassName, (void *)act_core_GetTrustLevel,
    (void *)act_core_get_Handled, (void *)act_core_put_Handled
};

struct window_visibility_args
{
    struct { const void *lpVtbl; } IVisibilityChangedEventArgs_iface;
    struct { const void *lpVtbl; } ICoreWindowEventArgs_iface;
    LONG ref;
    boolean visible;
    boolean handled;
};

#define impl_from_IVisibilityChangedEventArgs(ptr) ((struct window_visibility_args *)((char *)(ptr) - offsetof(struct window_visibility_args, IVisibilityChangedEventArgs_iface)))
#define impl_from_ICoreWindowEventArgs_vis(ptr)   ((struct window_visibility_args *)((char *)(ptr) - offsetof(struct window_visibility_args, ICoreWindowEventArgs_iface)))

static HRESULT WINAPI vis_args_QI(void *This, REFIID riid, void **ppv)
{
    struct window_visibility_args *impl = impl_from_IVisibilityChangedEventArgs(This);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IVisibilityChangedEventArgs))
    {
        *ppv = &impl->IVisibilityChangedEventArgs_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_ICoreWindowEventArgs))
    {
        *ppv = &impl->ICoreWindowEventArgs_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI vis_args_AddRef(void *This) { return InterlockedIncrement(&impl_from_IVisibilityChangedEventArgs(This)->ref); }
static ULONG WINAPI vis_args_Release(void *This)
{
    struct window_visibility_args *impl = impl_from_IVisibilityChangedEventArgs(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}
static HRESULT WINAPI vis_args_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI vis_args_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Core.VisibilityChangedEventArgs", 41, n); }
static HRESULT WINAPI vis_args_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI vis_args_get_Visible(void *This, boolean *value)
{
    struct window_visibility_args *impl = impl_from_IVisibilityChangedEventArgs(This);
    if (value) *value = impl->visible;
    return S_OK;
}

static const void *vis_args_vtbl[7] = {
    (void *)vis_args_QI, (void *)vis_args_AddRef, (void *)vis_args_Release,
    (void *)vis_args_GetIids, (void *)vis_args_GetRuntimeClassName, (void *)vis_args_GetTrustLevel,
    (void *)vis_args_get_Visible
};

static HRESULT WINAPI vis_core_QI(void *This, REFIID riid, void **ppv)
{
    struct window_visibility_args *impl = impl_from_ICoreWindowEventArgs_vis(This);
    return vis_args_QI(&impl->IVisibilityChangedEventArgs_iface, riid, ppv);
}
static ULONG WINAPI vis_core_AddRef(void *This)
{
    struct window_visibility_args *impl = impl_from_ICoreWindowEventArgs_vis(This);
    return vis_args_AddRef(&impl->IVisibilityChangedEventArgs_iface);
}
static ULONG WINAPI vis_core_Release(void *This)
{
    struct window_visibility_args *impl = impl_from_ICoreWindowEventArgs_vis(This);
    return vis_args_Release(&impl->IVisibilityChangedEventArgs_iface);
}
static HRESULT WINAPI vis_core_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI vis_core_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Core.VisibilityChangedEventArgs", 41, n); }
static HRESULT WINAPI vis_core_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI vis_core_get_Handled(void *This, boolean *value)
{
    struct window_visibility_args *impl = impl_from_ICoreWindowEventArgs_vis(This);
    if (value) *value = impl->handled;
    return S_OK;
}
static HRESULT WINAPI vis_core_put_Handled(void *This, boolean value)
{
    struct window_visibility_args *impl = impl_from_ICoreWindowEventArgs_vis(This);
    impl->handled = value;
    return S_OK;
}

static const void *vis_core_args_vtbl[8] = {
    (void *)vis_core_QI, (void *)vis_core_AddRef, (void *)vis_core_Release,
    (void *)vis_core_GetIids, (void *)vis_core_GetRuntimeClassName, (void *)vis_core_GetTrustLevel,
    (void *)vis_core_get_Handled, (void *)vis_core_put_Handled
};

void core_window_fire_size_changed(FLOAT width, FLOAT height)
{
    struct core_window_impl *impl;
    struct window_size_args *args;
    int i;

    if (!global_core_window) return;
    impl = impl_from_ICoreWindow(global_core_window);
    TRACE("core_window_fire_size_changed: count=%d, width=%.1f, height=%.1f\n", impl->size_changed_count, width, height);
    if (impl->size_changed_count <= 0) return;

    args = calloc(1, sizeof(*args));
    if (!args) return;
    args->IWindowSizeChangedEventArgs_iface.lpVtbl = size_args_vtbl;
    args->ICoreWindowEventArgs_iface.lpVtbl = size_core_args_vtbl;
    args->ref = 1;
    args->width = width;
    args->height = height;

    for (i = 0; i < impl->size_changed_count; i++)
    {
        struct typed_event_handler *handler = (struct typed_event_handler *)impl->size_changed_handlers[i].handler;
        if (handler && handler->lpVtbl && handler->lpVtbl->Invoke)
        {
            TRACE("Invoking size_changed_handler[%d] %p (invoke=%p)\n", i, handler, handler->lpVtbl->Invoke);
            handler->lpVtbl->Invoke(handler, global_core_window, &args->IWindowSizeChangedEventArgs_iface);
        }
    }
    IUnknown_Release((IUnknown *)&args->IWindowSizeChangedEventArgs_iface);
}

void core_window_fire_activated(int state)
{
    struct core_window_impl *impl;
    struct window_activated_args *args;
    int i;

    if (!global_core_window) return;
    impl = impl_from_ICoreWindow(global_core_window);
    TRACE("core_window_fire_activated: count=%d, state=%d\n", impl->activated_count, state);
    if (impl->activated_count <= 0) return;

    args = calloc(1, sizeof(*args));
    if (!args) return;
    args->IWindowActivatedEventArgs_iface.lpVtbl = act_args_vtbl;
    args->ICoreWindowEventArgs_iface.lpVtbl = act_core_args_vtbl;
    args->ref = 1;
    args->state = state;

    for (i = 0; i < impl->activated_count; i++)
    {
        struct typed_event_handler *handler = (struct typed_event_handler *)impl->activated_handlers[i].handler;
        if (handler && handler->lpVtbl && handler->lpVtbl->Invoke)
        {
            TRACE("Invoking activated_handler[%d] %p (invoke=%p)\n", i, handler, handler->lpVtbl->Invoke);
            handler->lpVtbl->Invoke(handler, global_core_window, &args->IWindowActivatedEventArgs_iface);
        }
    }
    IUnknown_Release((IUnknown *)&args->IWindowActivatedEventArgs_iface);
}

void core_window_fire_visibility(boolean visible)
{
    struct core_window_impl *impl;
    struct window_visibility_args *args;
    int i;

    if (!global_core_window) return;
    impl = impl_from_ICoreWindow(global_core_window);
    TRACE("core_window_fire_visibility: count=%d, visible=%d\n", impl->visibility_changed_count, visible);
    if (impl->visibility_changed_count <= 0) return;

    args = calloc(1, sizeof(*args));
    if (!args) return;
    args->IVisibilityChangedEventArgs_iface.lpVtbl = vis_args_vtbl;
    args->ICoreWindowEventArgs_iface.lpVtbl = vis_core_args_vtbl;
    args->ref = 1;
    args->visible = visible;

    for (i = 0; i < impl->visibility_changed_count; i++)
    {
        struct typed_event_handler *handler = (struct typed_event_handler *)impl->visibility_changed_handlers[i].handler;
        if (handler && handler->lpVtbl && handler->lpVtbl->Invoke)
        {
            TRACE("Invoking visibility_changed_handler[%d] %p\n", i, handler);
            handler->lpVtbl->Invoke(handler, global_core_window, &args->IVisibilityChangedEventArgs_iface);
        }
    }
    IUnknown_Release((IUnknown *)&args->IVisibilityChangedEventArgs_iface);
}

static HRESULT core_win_QI_internal(struct core_window_impl *impl, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;
    TRACE("core_win %p, riid %s\n", impl, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_ICoreWindow))
    {
        *ppvObject = &impl->ICoreWindow_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_ComWrappers_internal) ||
        IsEqualGUID(riid, &IID_ComWrappers_internal2) ||
        IsEqualGUID(riid, &IID_IWeakReferenceSource))
    {
        return E_NOINTERFACE;
    }
    TRACE("core_win %p: unhandled QI %s\n", impl, debugstr_guid(riid));
    return E_NOINTERFACE;
}

static HRESULT WINAPI cw_QueryInterface(void *This, REFIID riid, void **ppv) { return core_win_QI_internal(impl_from_ICoreWindow(This), riid, ppv); }
static ULONG WINAPI cw_AddRef(void *This) { return InterlockedIncrement(&impl_from_ICoreWindow(This)->ref); }
static ULONG WINAPI cw_Release(void *This) { return InterlockedDecrement(&impl_from_ICoreWindow(This)->ref); }
static HRESULT WINAPI cw_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI cw_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Core.CoreWindow", 26, n); }
static HRESULT WINAPI cw_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI cw_get_AutomationHostProvider(void *This, void **value) { if (value) *value = NULL; return S_OK; }
static HRESULT WINAPI cw_get_Bounds(void *This, void *value)
{
    float *f = (float *)value;
    if (!f) return E_POINTER;
    f[0] = 0.0f;
    f[1] = 0.0f;
    f[2] = 1016.0f;
    f[3] = 734.0f;
    if (current_window)
    {
        struct window_impl *win = impl_from_IWindow(current_window);
        if (win->hwnd)
        {
            RECT r;
            if (GetClientRect(win->hwnd, &r) && (r.right - r.left > 0))
            {
                f[2] = (float)(r.right - r.left);
                f[3] = (float)(r.bottom - r.top);
            }
        }
    }
    TRACE("CoreWindow::get_Bounds returning (%.1f, %.1f, %.1f, %.1f)\n", f[0], f[1], f[2], f[3]);
    return S_OK;
}
static HRESULT WINAPI cw_get_CustomProperties(void *This, void **value) { if (value) *value = NULL; return S_OK; }
static HRESULT WINAPI cw_get_Dispatcher(void *This, void **value) { return get_global_dispatcher(value); }
static HRESULT WINAPI cw_get_FlowDirection(void *This, int *value) { if (value) *value = 0; return S_OK; }
static HRESULT WINAPI cw_put_FlowDirection(void *This, int value) { return S_OK; }
static HRESULT WINAPI cw_get_IsInputEnabled(void *This, boolean *value) { if (value) *value = TRUE; return S_OK; }
static HRESULT WINAPI cw_put_IsInputEnabled(void *This, boolean value) { return S_OK; }
static HRESULT WINAPI cw_get_PointerCursor(void *This, void **value) { if (value) *value = NULL; return S_OK; }
static HRESULT WINAPI cw_put_PointerCursor(void *This, void *value) { return S_OK; }
extern HWND global_xaml_hwnd;

static HRESULT WINAPI cw_get_PointerPosition(void *This, void *value)
{
    if (value)
    {
        float *f = (float *)value;
        POINT pt;
        if (GetCursorPos(&pt) && global_xaml_hwnd)
        {
            ScreenToClient(global_xaml_hwnd, &pt);
            f[0] = (float)pt.x;
            f[1] = (float)pt.y;
        }
        else
        {
            f[0] = 0.0f;
            f[1] = 0.0f;
        }
    }
    return S_OK;
}
static HRESULT WINAPI cw_get_Visible(void *This, boolean *value) { if (value) *value = TRUE; return S_OK; }
static HRESULT WINAPI cw_Activate(void *This)
{
    FLOAT w = 1016.0f, h = 734.0f;
    TRACE("CoreWindow::Activate called!\n");
    if (current_window)
    {
        struct window_impl *win = impl_from_IWindow(current_window);
        if (win->hwnd)
        {
            RECT r;
            ShowWindow(win->hwnd, SW_SHOW);
            SetForegroundWindow(win->hwnd);
            if (GetClientRect(win->hwnd, &r) && (r.right - r.left > 0))
            {
                w = (FLOAT)(r.right - r.left);
                h = (FLOAT)(r.bottom - r.top);
            }
        }
    }
    core_window_fire_visibility(TRUE);
    core_window_fire_size_changed(w, h);
    core_window_fire_activated(0 /* CodeActivated */);
    return S_OK;
}
static HRESULT WINAPI cw_Close(void *This) { return S_OK; }
static HRESULT WINAPI cw_GetAsyncKeyState(void *This, int key, int *state) { if (state) *state = 0; return S_OK; }
static HRESULT WINAPI cw_GetKeyState(void *This, int key, int *state) { if (state) *state = 0; return S_OK; }
static HRESULT WINAPI cw_ReleasePointerCapture(void *This) { return S_OK; }
static HRESULT WINAPI cw_SetPointerCapture(void *This) { return S_OK; }

static HRESULT WINAPI cw_add_Activated(void *This, void *handler, INT64 *cookie)
{
    struct core_window_impl *impl = impl_from_ICoreWindow(This);
    TRACE("CoreWindow::add_Activated handler %p\n", handler);
    if (!cookie) return E_POINTER;
    *cookie = ++impl->next_token;
    if (handler && impl->activated_count < MAX_CW_HANDLERS)
    {
        IUnknown_AddRef((IUnknown *)handler);
        impl->activated_handlers[impl->activated_count].token = *cookie;
        impl->activated_handlers[impl->activated_count].handler = (IUnknown *)handler;
        impl->activated_count++;
    }
    return S_OK;
}

static HRESULT WINAPI cw_remove_Activated(void *This, INT64 cookie)
{
    struct core_window_impl *impl = impl_from_ICoreWindow(This);
    int i;
    for (i = 0; i < impl->activated_count; i++)
    {
        if (impl->activated_handlers[i].token == cookie)
        {
            if (impl->activated_handlers[i].handler)
                IUnknown_Release(impl->activated_handlers[i].handler);
            impl->activated_handlers[i] = impl->activated_handlers[--impl->activated_count];
            break;
        }
    }
    return S_OK;
}

static HRESULT WINAPI cw_add_SizeChanged(void *This, void *handler, INT64 *cookie)
{
    struct core_window_impl *impl = impl_from_ICoreWindow(This);
    FLOAT w = 1016.0f, h = 734.0f;
    TRACE("CoreWindow::add_SizeChanged handler %p\n", handler);
    if (!cookie) return E_POINTER;
    *cookie = ++impl->next_token;
    if (handler && impl->size_changed_count < MAX_CW_HANDLERS)
    {
        IUnknown_AddRef((IUnknown *)handler);
        impl->size_changed_handlers[impl->size_changed_count].token = *cookie;
        impl->size_changed_handlers[impl->size_changed_count].handler = (IUnknown *)handler;
        impl->size_changed_count++;

        if (current_window)
        {
            struct window_impl *win = impl_from_IWindow(current_window);
            if (win->hwnd)
            {
                RECT r;
                if (GetClientRect(win->hwnd, &r) && (r.right - r.left > 0))
                {
                    w = (FLOAT)(r.right - r.left);
                    h = (FLOAT)(r.bottom - r.top);
                }
            }
        }
        core_window_fire_size_changed(w, h);
    }
    return S_OK;
}

static HRESULT WINAPI cw_remove_SizeChanged(void *This, INT64 cookie)
{
    struct core_window_impl *impl = impl_from_ICoreWindow(This);
    int i;
    for (i = 0; i < impl->size_changed_count; i++)
    {
        if (impl->size_changed_handlers[i].token == cookie)
        {
            if (impl->size_changed_handlers[i].handler)
                IUnknown_Release(impl->size_changed_handlers[i].handler);
            impl->size_changed_handlers[i] = impl->size_changed_handlers[--impl->size_changed_count];
            break;
        }
    }
    return S_OK;
}

static HRESULT WINAPI cw_add_VisibilityChanged(void *This, void *handler, INT64 *cookie)
{
    struct core_window_impl *impl = impl_from_ICoreWindow(This);
    TRACE("CoreWindow::add_VisibilityChanged handler %p\n", handler);
    if (!cookie) return E_POINTER;
    *cookie = ++impl->next_token;
    if (handler && impl->visibility_changed_count < MAX_CW_HANDLERS)
    {
        IUnknown_AddRef((IUnknown *)handler);
        impl->visibility_changed_handlers[impl->visibility_changed_count].token = *cookie;
        impl->visibility_changed_handlers[impl->visibility_changed_count].handler = (IUnknown *)handler;
        impl->visibility_changed_count++;
    }
    return S_OK;
}

static HRESULT WINAPI cw_remove_VisibilityChanged(void *This, INT64 cookie)
{
    struct core_window_impl *impl = impl_from_ICoreWindow(This);
    int i;
    for (i = 0; i < impl->visibility_changed_count; i++)
    {
        if (impl->visibility_changed_handlers[i].token == cookie)
        {
            if (impl->visibility_changed_handlers[i].handler)
                IUnknown_Release(impl->visibility_changed_handlers[i].handler);
            impl->visibility_changed_handlers[i] = impl->visibility_changed_handlers[--impl->visibility_changed_count];
            break;
        }
    }
    return S_OK;
}

static const GUID IID_IPointerEventArgs = {0x920d9cb1, 0xa5fc, 0x4a21, {0x8c, 0x09, 0x49, 0xdf, 0xe6, 0xff, 0xe2, 0x5f}};
static const GUID IID_IPointerPoint = {0xe995317d, 0x7296, 0x42d9, {0x82, 0x33, 0xc5, 0xbe, 0x73, 0xb7, 0x4a, 0x4a}};
static const GUID IID_IPointerPointProperties = {0xc79d8a4b, 0xc163, 0x4ee7, {0x80, 0x3f, 0x67, 0xce, 0x79, 0xf9, 0x97, 0x2d}};

struct winrt_Point
{
    FLOAT X;
    FLOAT Y;
};

struct winrt_Rect
{
    FLOAT X;
    FLOAT Y;
    FLOAT Width;
    FLOAT Height;
};

/* IPointerPointProperties implementation */
struct pointer_properties_impl
{
    struct { const void *lpVtbl; } IPointerPointProperties_iface;
    LONG ref;
    FLOAT x, y;
    boolean left_pressed, right_pressed, middle_pressed;
    boolean in_contact;
    UINT32 update_kind;
    INT32 wheel_delta;
};

#define impl_from_IPointerPointProperties(ptr) ((struct pointer_properties_impl *)((char *)(ptr) - offsetof(struct pointer_properties_impl, IPointerPointProperties_iface)))

static HRESULT WINAPI props_QI(void *This, REFIID riid, void **ppv)
{
    struct pointer_properties_impl *impl = impl_from_IPointerPointProperties(This);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IPointerPointProperties))
    {
        *ppv = &impl->IPointerPointProperties_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI props_AddRef(void *This) { return InterlockedIncrement(&impl_from_IPointerPointProperties(This)->ref); }
static ULONG WINAPI props_Release(void *This)
{
    struct pointer_properties_impl *impl = impl_from_IPointerPointProperties(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}
static HRESULT WINAPI props_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI props_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Input.PointerPointProperties", 39, n); }
static HRESULT WINAPI props_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI props_get_Pressure(void *This, FLOAT *value) { if (value) *value = 0.5f; return S_OK; }
static HRESULT WINAPI props_get_IsInverted(void *This, boolean *value) { if (value) *value = FALSE; return S_OK; }
static HRESULT WINAPI props_get_IsEraser(void *This, boolean *value) { if (value) *value = FALSE; return S_OK; }
static HRESULT WINAPI props_get_Orientation(void *This, FLOAT *value) { if (value) *value = 0.0f; return S_OK; }
static HRESULT WINAPI props_get_XTilt(void *This, FLOAT *value) { if (value) *value = 0.0f; return S_OK; }
static HRESULT WINAPI props_get_YTilt(void *This, FLOAT *value) { if (value) *value = 0.0f; return S_OK; }
static HRESULT WINAPI props_get_Twist(void *This, FLOAT *value) { if (value) *value = 0.0f; return S_OK; }
static HRESULT WINAPI props_get_ContactRect(void *This, struct winrt_Rect *value)
{
    struct pointer_properties_impl *impl = impl_from_IPointerPointProperties(This);
    if (value) { value->X = impl->x; value->Y = impl->y; value->Width = 1.0f; value->Height = 1.0f; }
    return S_OK;
}
static HRESULT WINAPI props_get_ContactRectRaw(void *This, struct winrt_Rect *value) { return props_get_ContactRect(This, value); }
static HRESULT WINAPI props_get_TouchConfidence(void *This, boolean *value) { if (value) *value = TRUE; return S_OK; }
static HRESULT WINAPI props_get_IsLeftButtonPressed(void *This, boolean *value)
{
    struct pointer_properties_impl *impl = impl_from_IPointerPointProperties(This);
    if (value) *value = impl->left_pressed;
    return S_OK;
}
static HRESULT WINAPI props_get_IsRightButtonPressed(void *This, boolean *value)
{
    struct pointer_properties_impl *impl = impl_from_IPointerPointProperties(This);
    if (value) *value = impl->right_pressed;
    return S_OK;
}
static HRESULT WINAPI props_get_IsMiddleButtonPressed(void *This, boolean *value)
{
    struct pointer_properties_impl *impl = impl_from_IPointerPointProperties(This);
    if (value) *value = impl->middle_pressed;
    return S_OK;
}
static HRESULT WINAPI props_get_MouseWheelDelta(void *This, INT32 *value)
{
    struct pointer_properties_impl *impl = impl_from_IPointerPointProperties(This);
    if (value) *value = impl->wheel_delta;
    return S_OK;
}
static HRESULT WINAPI props_get_IsHorizontalMouseWheel(void *This, boolean *value) { if (value) *value = FALSE; return S_OK; }
static HRESULT WINAPI props_get_IsPrimary(void *This, boolean *value) { if (value) *value = TRUE; return S_OK; }
static HRESULT WINAPI props_get_IsInRange(void *This, boolean *value) { if (value) *value = TRUE; return S_OK; }
static HRESULT WINAPI props_get_IsCanceled(void *This, boolean *value) { if (value) *value = FALSE; return S_OK; }
static HRESULT WINAPI props_get_IsBarrelButtonPressed(void *This, boolean *value) { if (value) *value = FALSE; return S_OK; }
static HRESULT WINAPI props_get_IsXButton1Pressed(void *This, boolean *value) { if (value) *value = FALSE; return S_OK; }
static HRESULT WINAPI props_get_IsXButton2Pressed(void *This, boolean *value) { if (value) *value = FALSE; return S_OK; }
static HRESULT WINAPI props_get_PointerUpdateKind(void *This, UINT32 *value)
{
    struct pointer_properties_impl *impl = impl_from_IPointerPointProperties(This);
    if (value) *value = impl->update_kind;
    return S_OK;
}
static HRESULT WINAPI props_HasUsage(void *This, UINT32 p, UINT32 id, boolean *value) { if (value) *value = FALSE; return S_OK; }
static HRESULT WINAPI props_GetUsageValue(void *This, UINT32 p, UINT32 id, INT32 *value) { if (value) *value = 0; return S_OK; }

static const void *pointer_properties_vtbl[30] = {
    (void *)props_QI, (void *)props_AddRef, (void *)props_Release,
    (void *)props_GetIids, (void *)props_GetRuntimeClassName, (void *)props_GetTrustLevel,
    (void *)props_get_Pressure, (void *)props_get_IsInverted, (void *)props_get_IsEraser,
    (void *)props_get_Orientation, (void *)props_get_XTilt, (void *)props_get_YTilt,
    (void *)props_get_Twist, (void *)props_get_ContactRect, (void *)props_get_ContactRectRaw,
    (void *)props_get_TouchConfidence, (void *)props_get_IsLeftButtonPressed,
    (void *)props_get_IsRightButtonPressed, (void *)props_get_IsMiddleButtonPressed,
    (void *)props_get_MouseWheelDelta, (void *)props_get_IsHorizontalMouseWheel,
    (void *)props_get_IsPrimary, (void *)props_get_IsInRange, (void *)props_get_IsCanceled,
    (void *)props_get_IsBarrelButtonPressed, (void *)props_get_IsXButton1Pressed,
    (void *)props_get_IsXButton2Pressed, (void *)props_get_PointerUpdateKind,
    (void *)props_HasUsage, (void *)props_GetUsageValue
};

static const GUID IID_IPointerDevice = {0x93c9bafc, 0xebcb, 0x467e, {0x82, 0xc6, 0x27, 0x6f, 0xea, 0xe3, 0x6b, 0x5a}};

struct pointer_device_impl
{
    struct { const void *lpVtbl; } IPointerDevice_iface;
    LONG ref;
    UINT32 device_type; /* 2 = Mouse */
};

#define impl_from_IPointerDevice(ptr) ((struct pointer_device_impl *)((char *)(ptr) - offsetof(struct pointer_device_impl, IPointerDevice_iface)))

static HRESULT WINAPI pdev_QI(void *This, REFIID riid, void **ppv)
{
    struct pointer_device_impl *impl = impl_from_IPointerDevice(This);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IPointerDevice))
    {
        *ppv = &impl->IPointerDevice_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI pdev_AddRef(void *This) { return InterlockedIncrement(&impl_from_IPointerDevice(This)->ref); }
static ULONG WINAPI pdev_Release(void *This)
{
    struct pointer_device_impl *impl = impl_from_IPointerDevice(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}
static HRESULT WINAPI pdev_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI pdev_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.Devices.Input.PointerDevice", 35, n); }
static HRESULT WINAPI pdev_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI pdev_get_PointerDeviceType(void *This, UINT32 *value)
{
    struct pointer_device_impl *impl = impl_from_IPointerDevice(This);
    if (value) *value = impl->device_type;
    return S_OK;
}
static HRESULT WINAPI pdev_get_IsIntegrated(void *This, boolean *value) { if (value) *value = FALSE; return S_OK; }
static HRESULT WINAPI pdev_get_MaxContacts(void *This, UINT32 *value) { if (value) *value = 1; return S_OK; }
static HRESULT WINAPI pdev_get_PhysicalDeviceRect(void *This, void *value) { if (value) memset(value, 0, 16); return S_OK; }
static HRESULT WINAPI pdev_get_ScreenRect(void *This, void *value) { if (value) memset(value, 0, 16); return S_OK; }
static HRESULT WINAPI pdev_get_SupportedUsages(void *This, void **value) { if (value) *value = NULL; return S_OK; }

static const void *pointer_device_vtbl[12] = {
    (void *)pdev_QI, (void *)pdev_AddRef, (void *)pdev_Release,
    (void *)pdev_GetIids, (void *)pdev_GetRuntimeClassName, (void *)pdev_GetTrustLevel,
    (void *)pdev_get_PointerDeviceType, (void *)pdev_get_IsIntegrated, (void *)pdev_get_MaxContacts,
    (void *)pdev_get_PhysicalDeviceRect, (void *)pdev_get_ScreenRect, (void *)pdev_get_SupportedUsages
};

/* IPointerPoint implementation */
struct pointer_point_impl
{
    struct { const void *lpVtbl; } IPointerPoint_iface;
    LONG ref;
    FLOAT x, y;
    boolean in_contact;
    struct pointer_properties_impl *props;
    struct pointer_device_impl *device;
};

#define impl_from_IPointerPoint(ptr) ((struct pointer_point_impl *)((char *)(ptr) - offsetof(struct pointer_point_impl, IPointerPoint_iface)))

static HRESULT WINAPI pt_QI(void *This, REFIID riid, void **ppv)
{
    struct pointer_point_impl *impl = impl_from_IPointerPoint(This);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IPointerPoint))
    {
        *ppv = &impl->IPointerPoint_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI pt_AddRef(void *This) { return InterlockedIncrement(&impl_from_IPointerPoint(This)->ref); }
static ULONG WINAPI pt_Release(void *This)
{
    struct pointer_point_impl *impl = impl_from_IPointerPoint(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref)
    {
        if (impl->props) props_Release(&impl->props->IPointerPointProperties_iface);
        if (impl->device) pdev_Release(&impl->device->IPointerDevice_iface);
        free(impl);
    }
    return ref;
}
static HRESULT WINAPI pt_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI pt_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Input.PointerPoint", 30, n); }
static HRESULT WINAPI pt_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI pt_get_PointerDevice(void *This, void **value)
{
    struct pointer_point_impl *impl = impl_from_IPointerPoint(This);
    if (!value) return E_POINTER;
    *value = NULL;
    if (impl->device)
    {
        *value = &impl->device->IPointerDevice_iface;
        pdev_AddRef(&impl->device->IPointerDevice_iface);
    }
    return S_OK;
}
static HRESULT WINAPI pt_get_Position(void *This, struct winrt_Point *value)
{
    struct pointer_point_impl *impl = impl_from_IPointerPoint(This);
    if (value) { value->X = impl->x; value->Y = impl->y; }
    return S_OK;
}
static HRESULT WINAPI pt_get_RawPosition(void *This, struct winrt_Point *value) { return pt_get_Position(This, value); }
static HRESULT WINAPI pt_get_PointerId(void *This, UINT32 *value) { if (value) *value = 1; return S_OK; }
static HRESULT WINAPI pt_get_FrameId(void *This, UINT32 *value) { if (value) *value = 1; return S_OK; }
static HRESULT WINAPI pt_get_Timestamp(void *This, UINT64 *value) { if (value) *value = (UINT64)GetTickCount64() * 1000; return S_OK; }
static HRESULT WINAPI pt_get_IsInContact(void *This, boolean *value)
{
    struct pointer_point_impl *impl = impl_from_IPointerPoint(This);
    if (value) *value = impl->in_contact;
    return S_OK;
}
static HRESULT WINAPI pt_get_Properties(void *This, void **value)
{
    struct pointer_point_impl *impl = impl_from_IPointerPoint(This);
    if (!value) return E_POINTER;
    *value = &impl->props->IPointerPointProperties_iface;
    props_AddRef(&impl->props->IPointerPointProperties_iface);
    return S_OK;
}

static const void *pointer_point_vtbl[14] = {
    (void *)pt_QI, (void *)pt_AddRef, (void *)pt_Release,
    (void *)pt_GetIids, (void *)pt_GetRuntimeClassName, (void *)pt_GetTrustLevel,
    (void *)pt_get_PointerDevice, (void *)pt_get_Position, (void *)pt_get_RawPosition,
    (void *)pt_get_PointerId, (void *)pt_get_FrameId, (void *)pt_get_Timestamp,
    (void *)pt_get_IsInContact, (void *)pt_get_Properties
};

/* IPointerEventArgs implementation */
struct pointer_event_args_impl
{
    struct { const void *lpVtbl; } IPointerEventArgs_iface;
    struct { const void *lpVtbl; } ICoreWindowEventArgs_iface;
    LONG ref;
    struct pointer_point_impl *point;
    boolean handled;
    UINT32 key_modifiers;
};

#define impl_from_IPointerEventArgs(ptr) ((struct pointer_event_args_impl *)((char *)(ptr) - offsetof(struct pointer_event_args_impl, IPointerEventArgs_iface)))
#define impl_from_ICoreWindowEventArgs_ptr(ptr) ((struct pointer_event_args_impl *)((char *)(ptr) - offsetof(struct pointer_event_args_impl, ICoreWindowEventArgs_iface)))

static HRESULT WINAPI ptr_args_QI(void *This, REFIID riid, void **ppv)
{
    struct pointer_event_args_impl *impl = impl_from_IPointerEventArgs(This);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IPointerEventArgs))
    {
        *ppv = &impl->IPointerEventArgs_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_ICoreWindowEventArgs))
    {
        *ppv = &impl->ICoreWindowEventArgs_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI ptr_args_AddRef(void *This) { return InterlockedIncrement(&impl_from_IPointerEventArgs(This)->ref); }
static ULONG WINAPI ptr_args_Release(void *This)
{
    struct pointer_event_args_impl *impl = impl_from_IPointerEventArgs(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref)
    {
        if (impl->point) pt_Release(&impl->point->IPointerPoint_iface);
        free(impl);
    }
    return ref;
}
static HRESULT WINAPI ptr_args_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI ptr_args_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Core.PointerEventArgs", 33, n); }
static HRESULT WINAPI ptr_args_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI ptr_args_get_CurrentPoint(void *This, void **value)
{
    struct pointer_event_args_impl *impl = impl_from_IPointerEventArgs(This);
    if (!value) return E_POINTER;
    *value = &impl->point->IPointerPoint_iface;
    pt_AddRef(&impl->point->IPointerPoint_iface);
    return S_OK;
}
static HRESULT WINAPI ptr_args_get_KeyModifiers(void *This, UINT32 *value)
{
    struct pointer_event_args_impl *impl = impl_from_IPointerEventArgs(This);
    if (value) *value = impl->key_modifiers;
    return S_OK;
}
static HRESULT WINAPI ptr_args_GetIntermediatePoints(void *This, void **value)
{
    if (value) *value = NULL;
    return S_OK;
}

static const void *pointer_args_vtbl[10] = {
    (void *)ptr_args_QI, (void *)ptr_args_AddRef, (void *)ptr_args_Release,
    (void *)ptr_args_GetIids, (void *)ptr_args_GetRuntimeClassName, (void *)ptr_args_GetTrustLevel,
    (void *)ptr_args_get_CurrentPoint, (void *)ptr_args_get_KeyModifiers,
    (void *)ptr_args_GetIntermediatePoints
};

static HRESULT WINAPI ptr_core_QI(void *This, REFIID riid, void **ppv) { return ptr_args_QI(&impl_from_ICoreWindowEventArgs_ptr(This)->IPointerEventArgs_iface, riid, ppv); }
static ULONG WINAPI ptr_core_AddRef(void *This) { return ptr_args_AddRef(&impl_from_ICoreWindowEventArgs_ptr(This)->IPointerEventArgs_iface); }
static ULONG WINAPI ptr_core_Release(void *This) { return ptr_args_Release(&impl_from_ICoreWindowEventArgs_ptr(This)->IPointerEventArgs_iface); }
static HRESULT WINAPI ptr_core_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI ptr_core_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Core.PointerEventArgs", 33, n); }
static HRESULT WINAPI ptr_core_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI ptr_core_get_Handled(void *This, boolean *value)
{
    struct pointer_event_args_impl *impl = impl_from_ICoreWindowEventArgs_ptr(This);
    if (value) *value = impl->handled;
    return S_OK;
}
static HRESULT WINAPI ptr_core_put_Handled(void *This, boolean value)
{
    struct pointer_event_args_impl *impl = impl_from_ICoreWindowEventArgs_ptr(This);
    impl->handled = value;
    return S_OK;
}

static const void *pointer_core_args_vtbl[8] = {
    (void *)ptr_core_QI, (void *)ptr_core_AddRef, (void *)ptr_core_Release,
    (void *)ptr_core_GetIids, (void *)ptr_core_GetRuntimeClassName, (void *)ptr_core_GetTrustLevel,
    (void *)ptr_core_get_Handled, (void *)ptr_core_put_Handled
};

/* Dispatch pointer events */
void core_window_fire_pointer(UINT msg, WPARAM wparam, LPARAM lparam)
{
    struct core_window_impl *impl;
    struct pointer_event_args_impl *args;
    struct pointer_point_impl *point;
    struct pointer_properties_impl *props;
    struct pointer_device_impl *device;
    struct core_window_handler *handlers = NULL;
    int count = 0, i;
    FLOAT x = (FLOAT)(short)LOWORD(lparam);
    FLOAT y = (FLOAT)(short)HIWORD(lparam);
    boolean left_pressed = (wparam & MK_LBUTTON) != 0;
    boolean right_pressed = (wparam & MK_RBUTTON) != 0;
    boolean middle_pressed = (wparam & MK_MBUTTON) != 0;
    boolean in_contact = FALSE;
    UINT32 update_kind = 0;
    INT32 wheel_delta = 0;

    if (!global_core_window) return;
    impl = impl_from_ICoreWindow(global_core_window);

    switch (msg)
    {
        case WM_LBUTTONDOWN:
            in_contact = TRUE; left_pressed = TRUE; update_kind = 1;
            handlers = impl->pointer_pressed_handlers; count = impl->pointer_pressed_count;
            break;
        case WM_LBUTTONUP:
            in_contact = FALSE; left_pressed = FALSE; update_kind = 2;
            handlers = impl->pointer_released_handlers; count = impl->pointer_released_count;
            break;
        case WM_RBUTTONDOWN:
            in_contact = TRUE; right_pressed = TRUE; update_kind = 3;
            handlers = impl->pointer_pressed_handlers; count = impl->pointer_pressed_count;
            break;
        case WM_RBUTTONUP:
            in_contact = FALSE; right_pressed = FALSE; update_kind = 4;
            handlers = impl->pointer_released_handlers; count = impl->pointer_released_count;
            break;
        case WM_MBUTTONDOWN:
            in_contact = TRUE; middle_pressed = TRUE; update_kind = 5;
            handlers = impl->pointer_pressed_handlers; count = impl->pointer_pressed_count;
            break;
        case WM_MBUTTONUP:
            in_contact = FALSE; middle_pressed = FALSE; update_kind = 6;
            handlers = impl->pointer_released_handlers; count = impl->pointer_released_count;
            break;
        case WM_MOUSEMOVE:
            in_contact = left_pressed || right_pressed || middle_pressed;
            update_kind = 0;
            handlers = impl->pointer_moved_handlers; count = impl->pointer_moved_count;
            break;
        case WM_MOUSEWHEEL:
            wheel_delta = GET_WHEEL_DELTA_WPARAM(wparam);
            handlers = impl->pointer_wheel_handlers; count = impl->pointer_wheel_count;
            break;
        default:
            return;
    }

    if ((count <= 0 || !handlers) && !core_independent_input_source_has_handlers(msg)) return;

    props = calloc(1, sizeof(*props));
    point = calloc(1, sizeof(*point));
    args = calloc(1, sizeof(*args));
    device = calloc(1, sizeof(*device));
    if (!props || !point || !args || !device)
    {
        free(props); free(point); free(args); free(device);
        return;
    }

    device->IPointerDevice_iface.lpVtbl = pointer_device_vtbl;
    device->ref = 1;
    device->device_type = 2; /* PointerDeviceType::Mouse */

    props->IPointerPointProperties_iface.lpVtbl = pointer_properties_vtbl;
    props->ref = 1;
    props->x = x; props->y = y;
    props->left_pressed = left_pressed; props->right_pressed = right_pressed; props->middle_pressed = middle_pressed;
    props->in_contact = in_contact; props->update_kind = update_kind; props->wheel_delta = wheel_delta;

    point->IPointerPoint_iface.lpVtbl = pointer_point_vtbl;
    point->ref = 1;
    point->x = x; point->y = y;
    point->in_contact = in_contact;
    point->props = props;
    point->device = device;

    args->IPointerEventArgs_iface.lpVtbl = pointer_args_vtbl;
    args->ICoreWindowEventArgs_iface.lpVtbl = pointer_core_args_vtbl;
    args->ref = 1;
    args->point = point;

    if (handlers && count > 0)
    {
        for (i = 0; i < count; i++)
        {
            struct typed_event_handler *h = (struct typed_event_handler *)handlers[i].handler;
            if (h && h->lpVtbl && h->lpVtbl->Invoke)
            {
                TRACE("Invoking pointer handler[%d] %p (msg=0x%x, pt=%.0f,%.0f, kind=%u)\n",
                      i, h, msg, x, y, update_kind);
                h->lpVtbl->Invoke(h, global_core_window, &args->IPointerEventArgs_iface);
            }
        }
    }

    core_independent_input_source_fire_pointer(msg, wparam, lparam, &args->IPointerEventArgs_iface);

    ptr_args_Release(&args->IPointerEventArgs_iface);
}

void core_window_fire_key(UINT msg, WPARAM wparam, LPARAM lparam)
{
    /* Keyboard event stub */
}

void core_window_fire_char(WPARAM wparam, LPARAM lparam)
{
    /* Char event stub */
}

/* Event registration methods */
static HRESULT WINAPI cw_add_PointerPressed(void *This, void *handler, INT64 *cookie)
{
    struct core_window_impl *impl = impl_from_ICoreWindow(This);
    TRACE("CoreWindow::add_PointerPressed handler %p\n", handler);
    if (!cookie) return E_POINTER;
    *cookie = ++impl->next_token;
    if (handler && impl->pointer_pressed_count < MAX_CW_HANDLERS)
    {
        IUnknown_AddRef((IUnknown *)handler);
        impl->pointer_pressed_handlers[impl->pointer_pressed_count].token = *cookie;
        impl->pointer_pressed_handlers[impl->pointer_pressed_count].handler = (IUnknown *)handler;
        impl->pointer_pressed_count++;
    }
    return S_OK;
}
static HRESULT WINAPI cw_remove_PointerPressed(void *This, INT64 cookie)
{
    struct core_window_impl *impl = impl_from_ICoreWindow(This);
    int i;
    for (i = 0; i < impl->pointer_pressed_count; i++)
    {
        if (impl->pointer_pressed_handlers[i].token == cookie)
        {
            if (impl->pointer_pressed_handlers[i].handler)
                IUnknown_Release(impl->pointer_pressed_handlers[i].handler);
            impl->pointer_pressed_handlers[i] = impl->pointer_pressed_handlers[--impl->pointer_pressed_count];
            break;
        }
    }
    return S_OK;
}

static HRESULT WINAPI cw_add_PointerReleased(void *This, void *handler, INT64 *cookie)
{
    struct core_window_impl *impl = impl_from_ICoreWindow(This);
    TRACE("CoreWindow::add_PointerReleased handler %p\n", handler);
    if (!cookie) return E_POINTER;
    *cookie = ++impl->next_token;
    if (handler && impl->pointer_released_count < MAX_CW_HANDLERS)
    {
        IUnknown_AddRef((IUnknown *)handler);
        impl->pointer_released_handlers[impl->pointer_released_count].token = *cookie;
        impl->pointer_released_handlers[impl->pointer_released_count].handler = (IUnknown *)handler;
        impl->pointer_released_count++;
    }
    return S_OK;
}
static HRESULT WINAPI cw_remove_PointerReleased(void *This, INT64 cookie)
{
    struct core_window_impl *impl = impl_from_ICoreWindow(This);
    int i;
    for (i = 0; i < impl->pointer_released_count; i++)
    {
        if (impl->pointer_released_handlers[i].token == cookie)
        {
            if (impl->pointer_released_handlers[i].handler)
                IUnknown_Release(impl->pointer_released_handlers[i].handler);
            impl->pointer_released_handlers[i] = impl->pointer_released_handlers[--impl->pointer_released_count];
            break;
        }
    }
    return S_OK;
}

static HRESULT WINAPI cw_add_PointerMoved(void *This, void *handler, INT64 *cookie)
{
    struct core_window_impl *impl = impl_from_ICoreWindow(This);
    TRACE("CoreWindow::add_PointerMoved handler %p\n", handler);
    if (!cookie) return E_POINTER;
    *cookie = ++impl->next_token;
    if (handler && impl->pointer_moved_count < MAX_CW_HANDLERS)
    {
        IUnknown_AddRef((IUnknown *)handler);
        impl->pointer_moved_handlers[impl->pointer_moved_count].token = *cookie;
        impl->pointer_moved_handlers[impl->pointer_moved_count].handler = (IUnknown *)handler;
        impl->pointer_moved_count++;
    }
    return S_OK;
}
static HRESULT WINAPI cw_remove_PointerMoved(void *This, INT64 cookie)
{
    struct core_window_impl *impl = impl_from_ICoreWindow(This);
    int i;
    for (i = 0; i < impl->pointer_moved_count; i++)
    {
        if (impl->pointer_moved_handlers[i].token == cookie)
        {
            if (impl->pointer_moved_handlers[i].handler)
                IUnknown_Release(impl->pointer_moved_handlers[i].handler);
            impl->pointer_moved_handlers[i] = impl->pointer_moved_handlers[--impl->pointer_moved_count];
            break;
        }
    }
    return S_OK;
}

static HRESULT WINAPI cw_add_PointerWheelChanged(void *This, void *handler, INT64 *cookie)
{
    struct core_window_impl *impl = impl_from_ICoreWindow(This);
    TRACE("CoreWindow::add_PointerWheelChanged handler %p\n", handler);
    if (!cookie) return E_POINTER;
    *cookie = ++impl->next_token;
    if (handler && impl->pointer_wheel_count < MAX_CW_HANDLERS)
    {
        IUnknown_AddRef((IUnknown *)handler);
        impl->pointer_wheel_handlers[impl->pointer_wheel_count].token = *cookie;
        impl->pointer_wheel_handlers[impl->pointer_wheel_count].handler = (IUnknown *)handler;
        impl->pointer_wheel_count++;
    }
    return S_OK;
}
static HRESULT WINAPI cw_remove_PointerWheelChanged(void *This, INT64 cookie)
{
    struct core_window_impl *impl = impl_from_ICoreWindow(This);
    int i;
    for (i = 0; i < impl->pointer_wheel_count; i++)
    {
        if (impl->pointer_wheel_handlers[i].token == cookie)
        {
            if (impl->pointer_wheel_handlers[i].handler)
                IUnknown_Release(impl->pointer_wheel_handlers[i].handler);
            impl->pointer_wheel_handlers[i] = impl->pointer_wheel_handlers[--impl->pointer_wheel_count];
            break;
        }
    }
    return S_OK;
}

static HRESULT WINAPI cw_add_PointerEntered(void *This, void *handler, INT64 *cookie)
{
    struct core_window_impl *impl = impl_from_ICoreWindow(This);
    if (!cookie) return E_POINTER;
    *cookie = ++impl->next_token;
    if (handler && impl->pointer_entered_count < MAX_CW_HANDLERS)
    {
        IUnknown_AddRef((IUnknown *)handler);
        impl->pointer_entered_handlers[impl->pointer_entered_count].token = *cookie;
        impl->pointer_entered_handlers[impl->pointer_entered_count].handler = (IUnknown *)handler;
        impl->pointer_entered_count++;
    }
    return S_OK;
}
static HRESULT WINAPI cw_remove_PointerEntered(void *This, INT64 cookie)
{
    struct core_window_impl *impl = impl_from_ICoreWindow(This);
    int i;
    for (i = 0; i < impl->pointer_entered_count; i++)
    {
        if (impl->pointer_entered_handlers[i].token == cookie)
        {
            if (impl->pointer_entered_handlers[i].handler)
                IUnknown_Release(impl->pointer_entered_handlers[i].handler);
            impl->pointer_entered_handlers[i] = impl->pointer_entered_handlers[--impl->pointer_entered_count];
            break;
        }
    }
    return S_OK;
}

static HRESULT WINAPI cw_add_PointerExited(void *This, void *handler, INT64 *cookie)
{
    struct core_window_impl *impl = impl_from_ICoreWindow(This);
    if (!cookie) return E_POINTER;
    *cookie = ++impl->next_token;
    if (handler && impl->pointer_exited_count < MAX_CW_HANDLERS)
    {
        IUnknown_AddRef((IUnknown *)handler);
        impl->pointer_exited_handlers[impl->pointer_exited_count].token = *cookie;
        impl->pointer_exited_handlers[impl->pointer_exited_count].handler = (IUnknown *)handler;
        impl->pointer_exited_count++;
    }
    return S_OK;
}
static HRESULT WINAPI cw_remove_PointerExited(void *This, INT64 cookie)
{
    struct core_window_impl *impl = impl_from_ICoreWindow(This);
    int i;
    for (i = 0; i < impl->pointer_exited_count; i++)
    {
        if (impl->pointer_exited_handlers[i].token == cookie)
        {
            if (impl->pointer_exited_handlers[i].handler)
                IUnknown_Release(impl->pointer_exited_handlers[i].handler);
            impl->pointer_exited_handlers[i] = impl->pointer_exited_handlers[--impl->pointer_exited_count];
            break;
        }
    }
    return S_OK;
}

static HRESULT WINAPI cw_add_KeyDown(void *This, void *handler, INT64 *cookie)
{
    struct core_window_impl *impl = impl_from_ICoreWindow(This);
    if (!cookie) return E_POINTER;
    *cookie = ++impl->next_token;
    if (handler && impl->key_down_count < MAX_CW_HANDLERS)
    {
        IUnknown_AddRef((IUnknown *)handler);
        impl->key_down_handlers[impl->key_down_count].token = *cookie;
        impl->key_down_handlers[impl->key_down_count].handler = (IUnknown *)handler;
        impl->key_down_count++;
    }
    return S_OK;
}
static HRESULT WINAPI cw_remove_KeyDown(void *This, INT64 cookie)
{
    struct core_window_impl *impl = impl_from_ICoreWindow(This);
    int i;
    for (i = 0; i < impl->key_down_count; i++)
    {
        if (impl->key_down_handlers[i].token == cookie)
        {
            if (impl->key_down_handlers[i].handler)
                IUnknown_Release(impl->key_down_handlers[i].handler);
            impl->key_down_handlers[i] = impl->key_down_handlers[--impl->key_down_count];
            break;
        }
    }
    return S_OK;
}

static HRESULT WINAPI cw_add_KeyUp(void *This, void *handler, INT64 *cookie)
{
    struct core_window_impl *impl = impl_from_ICoreWindow(This);
    if (!cookie) return E_POINTER;
    *cookie = ++impl->next_token;
    if (handler && impl->key_up_count < MAX_CW_HANDLERS)
    {
        IUnknown_AddRef((IUnknown *)handler);
        impl->key_up_handlers[impl->key_up_count].token = *cookie;
        impl->key_up_handlers[impl->key_up_count].handler = (IUnknown *)handler;
        impl->key_up_count++;
    }
    return S_OK;
}
static HRESULT WINAPI cw_remove_KeyUp(void *This, INT64 cookie)
{
    struct core_window_impl *impl = impl_from_ICoreWindow(This);
    int i;
    for (i = 0; i < impl->key_up_count; i++)
    {
        if (impl->key_up_handlers[i].token == cookie)
        {
            if (impl->key_up_handlers[i].handler)
                IUnknown_Release(impl->key_up_handlers[i].handler);
            impl->key_up_handlers[i] = impl->key_up_handlers[--impl->key_up_count];
            break;
        }
    }
    return S_OK;
}

static HRESULT WINAPI cw_add_CharacterReceived(void *This, void *handler, INT64 *cookie)
{
    struct core_window_impl *impl = impl_from_ICoreWindow(This);
    if (!cookie) return E_POINTER;
    *cookie = ++impl->next_token;
    if (handler && impl->character_received_count < MAX_CW_HANDLERS)
    {
        IUnknown_AddRef((IUnknown *)handler);
        impl->character_received_handlers[impl->character_received_count].token = *cookie;
        impl->character_received_handlers[impl->character_received_count].handler = (IUnknown *)handler;
        impl->character_received_count++;
    }
    return S_OK;
}
static HRESULT WINAPI cw_remove_CharacterReceived(void *This, INT64 cookie)
{
    struct core_window_impl *impl = impl_from_ICoreWindow(This);
    int i;
    for (i = 0; i < impl->character_received_count; i++)
    {
        if (impl->character_received_handlers[i].token == cookie)
        {
            if (impl->character_received_handlers[i].handler)
                IUnknown_Release(impl->character_received_handlers[i].handler);
            impl->character_received_handlers[i] = impl->character_received_handlers[--impl->character_received_count];
            break;
        }
    }
    return S_OK;
}

static HRESULT WINAPI cw_event_add(void *This, void *handler, INT64 *cookie)
{
    TRACE("cw_event_add generic stub %p, handler %p\n", This, handler);
    if (cookie) *cookie = 1;
    return S_OK;
}
static HRESULT WINAPI cw_event_remove(void *This, INT64 cookie) { return S_OK; }

static const void *core_window_vtbl[64] = {
    (void *)cw_QueryInterface, (void *)cw_AddRef, (void *)cw_Release,
    (void *)cw_GetIids, (void *)cw_GetRuntimeClassName, (void *)cw_GetTrustLevel,
    (void *)cw_get_AutomationHostProvider,  /* 6 */
    (void *)cw_get_Bounds,                  /* 7 */
    (void *)cw_get_CustomProperties,        /* 8 */
    (void *)cw_get_Dispatcher,              /* 9 */
    (void *)cw_get_FlowDirection,           /* 10 */
    (void *)cw_put_FlowDirection,           /* 11 */
    (void *)cw_get_IsInputEnabled,          /* 12 */
    (void *)cw_put_IsInputEnabled,          /* 13 */
    (void *)cw_get_PointerCursor,           /* 14 */
    (void *)cw_put_PointerCursor,           /* 15 */
    (void *)cw_get_PointerPosition,         /* 16 */
    (void *)cw_get_Visible,                 /* 17 */
    (void *)cw_Activate,                    /* 18 */
    (void *)cw_Close,                       /* 19 */
    (void *)cw_GetAsyncKeyState,             /* 20 */
    (void *)cw_GetKeyState,                 /* 21 */
    (void *)cw_ReleasePointerCapture,       /* 22 */
    (void *)cw_SetPointerCapture,           /* 23 */
    (void *)cw_add_Activated,               /* 24: Activated */
    (void *)cw_remove_Activated,            /* 25 */
    (void *)cw_event_add, (void *)cw_event_remove, /* 26, 27: AutomationProviderRequested */
    (void *)cw_add_CharacterReceived, (void *)cw_remove_CharacterReceived, /* 28, 29: CharacterReceived */
    (void *)cw_event_add, (void *)cw_event_remove, /* 30, 31: Closed */
    (void *)cw_event_add, (void *)cw_event_remove, /* 32, 33: InputEnabled */
    (void *)cw_add_KeyDown, (void *)cw_remove_KeyDown, /* 34, 35: KeyDown */
    (void *)cw_add_KeyUp, (void *)cw_remove_KeyUp, /* 36, 37: KeyUp */
    (void *)cw_event_add, (void *)cw_event_remove, /* 38, 39: PointerCaptureLost */
    (void *)cw_add_PointerEntered, (void *)cw_remove_PointerEntered, /* 40, 41: PointerEntered */
    (void *)cw_add_PointerExited, (void *)cw_remove_PointerExited, /* 42, 43: PointerExited */
    (void *)cw_add_PointerMoved, (void *)cw_remove_PointerMoved, /* 44, 45: PointerMoved */
    (void *)cw_add_PointerPressed, (void *)cw_remove_PointerPressed, /* 46, 47: PointerPressed */
    (void *)cw_add_PointerReleased, (void *)cw_remove_PointerReleased, /* 48, 49: PointerReleased */
    (void *)cw_event_add, (void *)cw_event_remove, /* 50, 51: TouchHitTesting */
    (void *)cw_add_PointerWheelChanged, (void *)cw_remove_PointerWheelChanged, /* 52, 53: PointerWheelChanged */
    (void *)cw_add_SizeChanged,             /* 54: SizeChanged */
    (void *)cw_remove_SizeChanged,          /* 55 */
    (void *)cw_add_VisibilityChanged,       /* 56: VisibilityChanged */
    (void *)cw_remove_VisibilityChanged     /* 57 */
};

static const void *redhawk_core_window_vtbl[64] = {
    (void *)cw_QueryInterface, (void *)cw_AddRef, (void *)cw_Release,
    (void *)redhawk_interop_m3, (void *)cw_GetRuntimeClassName, (void *)cw_GetTrustLevel,
    (void *)cw_get_AutomationHostProvider,  /* 6 */
    (void *)cw_get_Bounds,                  /* 7 */
    (void *)cw_get_CustomProperties,        /* 8 */
    (void *)cw_get_Dispatcher,              /* 9 */
    (void *)cw_get_FlowDirection,           /* 10 */
    (void *)cw_put_FlowDirection,           /* 11 */
    (void *)cw_get_IsInputEnabled,          /* 12 */
    (void *)cw_put_IsInputEnabled,          /* 13 */
    (void *)cw_get_PointerCursor,           /* 14 */
    (void *)cw_put_PointerCursor,           /* 15 */
    (void *)cw_get_PointerPosition,         /* 16 */
    (void *)cw_get_Visible,                 /* 17 */
    (void *)cw_Activate,                    /* 18 */
    (void *)cw_Close,                       /* 19 */
    (void *)cw_GetAsyncKeyState,             /* 20 */
    (void *)cw_GetKeyState,                 /* 21 */
    (void *)cw_ReleasePointerCapture,       /* 22 */
    (void *)cw_SetPointerCapture,           /* 23 */
    (void *)cw_add_Activated,               /* 24: Activated */
    (void *)cw_remove_Activated,            /* 25 */
    (void *)cw_event_add, (void *)cw_event_remove, /* 26, 27: AutomationProviderRequested */
    (void *)cw_add_CharacterReceived, (void *)cw_remove_CharacterReceived, /* 28, 29: CharacterReceived */
    (void *)cw_event_add, (void *)cw_event_remove, /* 30, 31: Closed */
    (void *)cw_event_add, (void *)cw_event_remove, /* 32, 33: InputEnabled */
    (void *)cw_add_KeyDown, (void *)cw_remove_KeyDown, /* 34, 35: KeyDown */
    (void *)cw_add_KeyUp, (void *)cw_remove_KeyUp, /* 36, 37: KeyUp */
    (void *)cw_event_add, (void *)cw_event_remove, /* 38, 39: PointerCaptureLost */
    (void *)cw_add_PointerEntered, (void *)cw_remove_PointerEntered, /* 40, 41: PointerEntered */
    (void *)cw_add_PointerExited, (void *)cw_remove_PointerExited, /* 42, 43: PointerExited */
    (void *)cw_add_PointerMoved, (void *)cw_remove_PointerMoved, /* 44, 45: PointerMoved */
    (void *)cw_add_PointerPressed, (void *)cw_remove_PointerPressed, /* 46, 47: PointerPressed */
    (void *)cw_add_PointerReleased, (void *)cw_remove_PointerReleased, /* 48, 49: PointerReleased */
    (void *)cw_event_add, (void *)cw_event_remove, /* 50, 51: TouchHitTesting */
    (void *)cw_add_PointerWheelChanged, (void *)cw_remove_PointerWheelChanged, /* 52, 53: PointerWheelChanged */
    (void *)cw_add_SizeChanged,             /* 54: SizeChanged */
    (void *)cw_remove_SizeChanged,          /* 55 */
    (void *)cw_add_VisibilityChanged,       /* 56: VisibilityChanged */
    (void *)cw_remove_VisibilityChanged     /* 57 */
};

static HRESULT get_global_core_window(void **value)
{
    struct core_window_impl *impl;
    if (!value) return E_POINTER;
    if (global_core_window)
    {
        impl = impl_from_ICoreWindow(global_core_window);
        InterlockedIncrement(&impl->ref);
        *value = global_core_window;
        TRACE("get_global_core_window: returning existing %p\n", *value);
        return S_OK;
    }
    impl = calloc(1, sizeof(*impl));
    if (!impl) return E_OUTOFMEMORY;
    impl->ICoreWindow_iface.lpVtbl = core_window_vtbl;
    impl->IRedhawkInterop_iface.lpVtbl = redhawk_core_window_vtbl;
    impl->ref = 10000; /* Permanent singleton */
    *value = &impl->ICoreWindow_iface;
    global_core_window = *value;
    TRACE("get_global_core_window: created new %p\n", *value);
    return S_OK;
}

/* -------------------------------------------------------------------------
 * Window Instance & Statics
 * ------------------------------------------------------------------------- */

static HRESULT WINAPI win_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    struct window_impl *impl = impl_from_IWindow(This);
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("window %p, riid %s\n", This, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IWindow) ||
        IsEqualGUID(riid, &IID_IWindow_3276167d))
    {
        *ppvObject = &impl->IWindow_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IWindow2))
    {
        *ppvObject = &impl->IWindow2_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IWindow3))
    {
        *ppvObject = &impl->IWindow3_iface;
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

    TRACE("window %p: unhandled QI %s\n", This, debugstr_guid(riid));
    return E_NOINTERFACE;
}
static ULONG WINAPI win_AddRef(void *This) { return InterlockedIncrement(&impl_from_IWindow(This)->ref); }
static ULONG WINAPI win_Release(void *This)
{
    struct window_impl *impl = impl_from_IWindow(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}
static HRESULT WINAPI win_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI win_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(RuntimeClass_Window, wcslen(RuntimeClass_Window), n); }
static HRESULT WINAPI win_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI win_generic_stub(void *This) { return S_OK; }
static void ensure_window_hwnd(struct window_impl *impl)
{
    WNDCLASSEXW wc = {0};
    WCHAR title[128] = L"UWP Application";
    WCHAR modpath[MAX_PATH];
    int width = 1024, height = 768;

    if (impl->hwnd) return;

    if (GetModuleFileNameW(NULL, modpath, MAX_PATH))
    {
        WCHAR *p = wcsrchr(modpath, L'\\');
        if (p)
        {
            p++;
            if (wcsicmp(p, L"CalculatorApp.exe") == 0 || wcsicmp(p, L"Calculator.exe") == 0)
            {
                wcscpy(title, L"Calculator");
                width = 400;
                height = 600;
            }
            else if (wcsicmp(p, L"Solitaire.exe") == 0)
            {
                wcscpy(title, L"Microsoft Solitaire Collection");
                width = 1024;
                height = 768;
            }
            else
            {
                lstrcpynW(title, p, ARRAY_SIZE(title));
            }
        }
    }

    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = xaml_window_wndproc;
    wc.hInstance = GetModuleHandleW(NULL);
    wc.hCursor = LoadCursorW(NULL, (LPCWSTR)IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszClassName = L"Windows.UI.Xaml.Window";
    RegisterClassExW(&wc);

    impl->hwnd = CreateWindowExW(0, L"Windows.UI.Xaml.Window", title, WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                 CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, NULL, NULL);
    TRACE("CreateWindowExW created hwnd %p\n", impl->hwnd);
    if (impl->hwnd)
    {
        SetPropW(GetDesktopWindow(), L"__WineDCompTarget", impl->hwnd);
        xaml_window_init(impl->hwnd);
    }
}

static HRESULT WINAPI win_get_Bounds(void *This, void *bounds)
{
    struct window_impl *impl = impl_from_IWindow(This);
    struct { FLOAT X; FLOAT Y; FLOAT Width; FLOAT Height; } *rect = bounds;
    TRACE("Window::get_Bounds %p\n", bounds);
    if (!rect) return E_POINTER;
    rect->X = 0.0f;
    rect->Y = 0.0f;
    rect->Width = 1024.0f;
    rect->Height = 768.0f;
    if (impl->hwnd)
    {
        RECT r;
        if (GetClientRect(impl->hwnd, &r) && (r.right - r.left > 0))
        {
            rect->Width = (FLOAT)(r.right - r.left);
            rect->Height = (FLOAT)(r.bottom - r.top);
        }
    }
    return S_OK;
}
static HRESULT WINAPI win_get_Visible(void *This, boolean *value) { if (value) *value = TRUE; return S_OK; }
static HRESULT WINAPI win_get_Content(void *This, void **value)
{
    struct window_impl *impl = impl_from_IWindow(This);
    TRACE("Window::get_Content -> %p\n", impl->content);
    if (!value) return E_POINTER;
    *value = impl->content;
    if (impl->content) IUnknown_AddRef((IUnknown *)impl->content);
    return S_OK;
}
static HRESULT WINAPI win_put_Content(void *This, void *value)
{
    struct window_impl *impl = impl_from_IWindow(This);
    TRACE("Window::put_Content %p\n", value);
    if (value) IUnknown_AddRef((IUnknown *)value);
    if (impl->content) IUnknown_Release((IUnknown *)impl->content);
    impl->content = value;
    return S_OK;
}
static HRESULT WINAPI win_get_CoreWindow(void *This, void **value)
{
    TRACE("Window::get_CoreWindow\n");
    return get_global_core_window(value);
}
static HRESULT WINAPI win_get_Dispatcher(void *This, void **value)
{
    TRACE("Window::get_Dispatcher\n");
    return get_global_dispatcher(value);
}
static HRESULT WINAPI win_Activate(void *This)
{
    struct window_impl *impl = impl_from_IWindow(This);
    FLOAT w = 1016.0f, h = 734.0f;
    TRACE("Window::Activate called! hwnd=%p\n", impl->hwnd);
    ensure_window_hwnd(impl);

    if (impl->hwnd)
    {
        RECT r;
        ShowWindow(impl->hwnd, SW_SHOW);
        SetForegroundWindow(impl->hwnd);
        if (GetClientRect(impl->hwnd, &r) && (r.right - r.left > 0))
        {
            w = (FLOAT)(r.right - r.left);
            h = (FLOAT)(r.bottom - r.top);
        }
    }
    xaml_fire_loaded_events();
    core_window_fire_visibility(TRUE);
    core_window_fire_size_changed(w, h);
    xaml_fire_swapchain_size_changed(w, h);
    return S_OK;
}
static HRESULT WINAPI win_Close(void *This) { return S_OK; }
static HRESULT WINAPI win_event_add(void *This, void *handler, EventRegistrationToken *token)
{
    TRACE("Window::event_add handler %p\n", handler);
    if (token) token->value = 1;
    return S_OK;
}
static HRESULT WINAPI win_event_remove(void *This, EventRegistrationToken token)
{
    TRACE("Window::event_remove token %lld\n", token.value);
    return S_OK;
}

static const void *window_vtbl[32] = {
    (void *)win_QueryInterface,           /* 0 */
    (void *)win_AddRef,                   /* 1 */
    (void *)win_Release,                  /* 2 */
    (void *)win_GetIids,                  /* 3 */
    (void *)win_GetRuntimeClassName,      /* 4 */
    (void *)win_GetTrustLevel,            /* 5 */
    (void *)win_get_Bounds,               /* 6 */
    (void *)win_get_Visible,              /* 7 */
    (void *)win_get_Content,              /* 8 */
    (void *)win_put_Content,              /* 9 */
    (void *)win_get_CoreWindow,           /* 10 */
    (void *)win_get_Dispatcher,           /* 11 */
    (void *)win_event_add,                /* 12: add_Activated */
    (void *)win_event_remove,             /* 13: remove_Activated */
    (void *)win_event_add,                /* 14: add_Closed */
    (void *)win_event_remove,             /* 15: remove_Closed */
    (void *)win_event_add,                /* 16: add_SizeChanged */
    (void *)win_event_remove,             /* 17: remove_SizeChanged */
    (void *)win_event_add,                /* 18: add_VisibilityChanged */
    (void *)win_event_remove,             /* 19: remove_VisibilityChanged */
    (void *)win_Activate,                 /* 20: Activate */
    (void *)win_Close,                    /* 21: Close */
    (void *)win_generic_stub, (void *)win_generic_stub, (void *)win_generic_stub,
    (void *)win_generic_stub, (void *)win_generic_stub, (void *)win_generic_stub,
    (void *)win_generic_stub, (void *)win_generic_stub, (void *)win_generic_stub,
    (void *)win_generic_stub
};

static HRESULT WINAPI redhawk_win_QI(void *This, REFIID riid, void **ppvObject)
{
    return win_QueryInterface(&impl_from_IRedhawk_win(This)->IWindow_iface, riid, ppvObject);
}
static ULONG WINAPI redhawk_win_AddRef(void *This) { return win_AddRef(&impl_from_IRedhawk_win(This)->IWindow_iface); }
static ULONG WINAPI redhawk_win_Release(void *This) { return win_Release(&impl_from_IRedhawk_win(This)->IWindow_iface); }

static const void *redhawk_window_vtbl[32] = {
    (void *)redhawk_win_QI,
    (void *)redhawk_win_AddRef,
    (void *)redhawk_win_Release,
    (void *)redhawk_interop_m3,
    (void *)win_GetRuntimeClassName,
    (void *)win_GetTrustLevel,
    (void *)win_get_Bounds,
    (void *)redhawk_interop_m3, /* slot 7: safe hook */
    (void *)win_get_Content,
    (void *)win_put_Content,
    (void *)win_get_CoreWindow,
    (void *)win_get_Dispatcher,
    (void *)win_event_add,                /* 12: add_Activated */
    (void *)win_event_remove,             /* 13: remove_Activated */
    (void *)win_event_add,                /* 14: add_Closed */
    (void *)win_event_remove,             /* 15: remove_Closed */
    (void *)win_event_add,                /* 16: add_SizeChanged */
    (void *)win_event_remove,             /* 17: remove_SizeChanged */
    (void *)win_event_add,                /* 18: add_VisibilityChanged */
    (void *)win_event_remove,             /* 19: remove_VisibilityChanged */
    (void *)win_Activate,                 /* 20: Activate */
    (void *)win_Close,                    /* 21: Close */
    (void *)win_generic_stub, (void *)win_generic_stub, (void *)win_generic_stub,
    (void *)win_generic_stub, (void *)win_generic_stub, (void *)win_generic_stub,
    (void *)win_generic_stub, (void *)win_generic_stub, (void *)win_generic_stub,
    (void *)win_generic_stub
};

static HRESULT WINAPI win2_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return win_QueryInterface(&impl_from_IWindow2(This)->IWindow_iface, riid, ppvObject);
}
static ULONG WINAPI win2_AddRef(void *This) { return win_AddRef(&impl_from_IWindow2(This)->IWindow_iface); }
static ULONG WINAPI win2_Release(void *This) { return win_Release(&impl_from_IWindow2(This)->IWindow_iface); }
static HRESULT WINAPI win2_SetTitleBar(void *This, void *value)
{
    struct window_impl *impl = impl_from_IWindow2(This);
    TRACE("Window2::SetTitleBar value=%p\n", value);
    if (value) IUnknown_AddRef((IUnknown *)value);
    if (impl->titlebar_element) IUnknown_Release((IUnknown *)impl->titlebar_element);
    impl->titlebar_element = value;
    return S_OK;
}

static const void *window2_vtbl[7] = {
    (void *)win2_QueryInterface,
    (void *)win2_AddRef,
    (void *)win2_Release,
    (void *)win_GetIids,
    (void *)win_GetRuntimeClassName,
    (void *)win_GetTrustLevel,
    (void *)win2_SetTitleBar,
};

static HRESULT WINAPI win3_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return win_QueryInterface(&impl_from_IWindow3(This)->IWindow_iface, riid, ppvObject);
}
static ULONG WINAPI win3_AddRef(void *This) { return win_AddRef(&impl_from_IWindow3(This)->IWindow_iface); }
static ULONG WINAPI win3_Release(void *This) { return win_Release(&impl_from_IWindow3(This)->IWindow_iface); }
static HRESULT WINAPI win3_get_Compositor(void *This, void **value)
{
    TRACE("Window3::get_Compositor\n");
    if (!value) return E_POINTER;
    *value = NULL;
    return S_OK;
}

static const void *window3_vtbl[7] = {
    (void *)win3_QueryInterface,
    (void *)win3_AddRef,
    (void *)win3_Release,
    (void *)win_GetIids,
    (void *)win_GetRuntimeClassName,
    (void *)win_GetTrustLevel,
    (void *)win3_get_Compositor,
};

static HRESULT create_window_instance(void **value)
{
    struct window_impl *impl;
    if (!value || (uintptr_t)value < 0x10000) return E_POINTER;
    if (current_window)
    {
        impl = impl_from_IWindow(current_window);
        InterlockedIncrement(&impl->ref);
        *value = current_window;
        TRACE("Returning existing Window instance %p, ref %ld\n", *value, impl->ref);
        return S_OK;
    }
    impl = calloc(1, sizeof(*impl));
    if (!impl) return E_OUTOFMEMORY;
    impl->IWindow_iface.lpVtbl = window_vtbl;
    impl->IWindow2_iface.lpVtbl = window2_vtbl;
    impl->IWindow3_iface.lpVtbl = window3_vtbl;
    impl->IRedhawkInterop_iface.lpVtbl = redhawk_window_vtbl;
    impl->ref = 10000;
    *value = &impl->IWindow_iface;
    current_window = *value;
    ensure_window_hwnd(impl);
    TRACE("Created Window instance %p (hwnd %p)\n", *value, impl->hwnd);
    return S_OK;
}

/* Window Statics */
struct window_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } IWindowStatics_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    LONG ref;
};

#define impl_from_win_factory(ptr) ((struct window_factory_impl *)((char *)(ptr) - offsetof(struct window_factory_impl, IActivationFactory_iface)))
#define impl_from_win_statics(ptr) ((struct window_factory_impl *)((char *)(ptr) - offsetof(struct window_factory_impl, IWindowStatics_iface)))
#define impl_from_win_redfac(ptr)  ((struct window_factory_impl *)((char *)(ptr) - offsetof(struct window_factory_impl, IRedhawkInterop_iface)))

static HRESULT win_fac_QI_internal(struct window_factory_impl *impl, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("win_fac %p, riid %s\n", impl, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppvObject = &impl->IActivationFactory_iface;
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IWindowStatics))
    {
        *ppvObject = &impl->IWindowStatics_iface;
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

    TRACE("win_fac %p: unhandled QI %s\n", impl, debugstr_guid(riid));
    return E_NOINTERFACE;
}

static HRESULT WINAPI win_fac_QueryInterface(IActivationFactory *iface, REFIID riid, void **ppvObject) { return win_fac_QI_internal(impl_from_win_factory(iface), riid, ppvObject); }
static ULONG WINAPI win_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI win_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI win_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI win_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n) { return WindowsCreateString(RuntimeClass_Window, wcslen(RuntimeClass_Window), n); }
static HRESULT WINAPI win_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI win_fac_ActivateInstance(IActivationFactory *iface, IInspectable **instance) { return create_window_instance((void **)instance); }

static const IActivationFactoryVtbl window_activation_factory_vtbl = {
    win_fac_QueryInterface,
    win_fac_AddRef,
    win_fac_Release,
    win_fac_GetIids,
    win_fac_GetRuntimeClassName,
    win_fac_GetTrustLevel,
    win_fac_ActivateInstance
};

/* IWindowStatics */
static HRESULT WINAPI win_s_QueryInterface(void *This, REFIID riid, void **ppvObject) { return win_fac_QI_internal(impl_from_win_statics(This), riid, ppvObject); }
static ULONG WINAPI win_s_AddRef(void *This) { return 2; }
static ULONG WINAPI win_s_Release(void *This) { return 1; }
static HRESULT WINAPI win_s_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI win_s_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(RuntimeClass_Window, wcslen(RuntimeClass_Window), n); }
static HRESULT WINAPI win_s_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI win_s_get_Current(void *This, void **value)
{
    TRACE("Window::get_Current %p\n", value);
    if (!value || (uintptr_t)value < 0x10000) return E_POINTER;
    return create_window_instance(value);
}

static const void *window_statics_vtbl[] = {
    (void *)win_s_QueryInterface,                     /* 0 */
    (void *)win_s_AddRef,                             /* 1 */
    (void *)win_s_Release,                            /* 2 */
    (void *)win_s_GetIids,                            /* 3 */
    (void *)win_s_GetRuntimeClassName,                /* 4 */
    (void *)win_s_GetTrustLevel,                      /* 5 */
    (void *)win_s_get_Current,                        /* 6 */
    (void *)win_s_get_Current,                        /* 7 */
    (void *)win_s_get_Current,                        /* 8 */
    (void *)win_s_get_Current,                        /* 9 */
    (void *)win_s_get_Current                         /* 10 */
};

static HRESULT WINAPI redhawk_win_fac_QI(void *This, REFIID riid, void **ppvObject) { return win_fac_QI_internal(impl_from_win_redfac(This), riid, ppvObject); }
static ULONG WINAPI redhawk_win_fac_AddRef(void *This) { return 2; }
static ULONG WINAPI redhawk_win_fac_Release(void *This) { return 1; }

static const void *redhawk_window_factory_vtbl[] = {
    (void *)redhawk_win_fac_QI,
    (void *)redhawk_win_fac_AddRef,
    (void *)redhawk_win_fac_Release,
    (void *)redhawk_interop_m3,
    (void *)win_s_GetRuntimeClassName,
    (void *)win_s_GetTrustLevel,
    (void *)win_s_get_Current,
    (void *)redhawk_interop_m3,                       /* slot 7 safe hook */
    (void *)redhawk_interop_m3,
    (void *)redhawk_interop_m3,
    (void *)redhawk_interop_m3,
    (void *)redhawk_interop_m3,
    (void *)redhawk_interop_m3,
    (void *)redhawk_interop_m3,
    (void *)redhawk_interop_m3,
    (void *)redhawk_interop_m3
};

static struct window_factory_impl default_window_factory = {
    {&window_activation_factory_vtbl},
    {window_statics_vtbl},
    {redhawk_window_factory_vtbl},
    1
};

IActivationFactory *window_factory = &default_window_factory.IActivationFactory_iface;

/* -------------------------------------------------------------------------
 * Windows.System.User Factory & Statics
 * ------------------------------------------------------------------------- */
static const GUID IID_IUserStatics = {0x155eb23b, 0x242a, 0x45e0, {0xa2, 0xe9, 0x31, 0x71, 0xfc, 0x6a, 0x7f, 0xdd}};

struct user_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } IUserStatics_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    LONG ref;
};

#define impl_from_user_factory(ptr) ((struct user_factory_impl *)((char *)(ptr) - offsetof(struct user_factory_impl, IActivationFactory_iface)))
#define impl_from_user_statics(ptr) ((struct user_factory_impl *)((char *)(ptr) - offsetof(struct user_factory_impl, IUserStatics_iface)))
#define impl_from_user_redfac(ptr)  ((struct user_factory_impl *)((char *)(ptr) - offsetof(struct user_factory_impl, IRedhawkInterop_iface)))

static HRESULT user_fac_QI_internal(struct user_factory_impl *impl, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("user_fac %p, riid %s\n", impl, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppvObject = &impl->IActivationFactory_iface;
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IUserStatics))
    {
        *ppvObject = &impl->IUserStatics_iface;
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ComWrappers_internal))
    {
        *ppvObject = &impl->IRedhawkInterop_iface;
        return S_OK;
    }

    TRACE("user_fac %p: unhandled QI %s\n", impl, debugstr_guid(riid));
    return E_NOINTERFACE;
}

static HRESULT WINAPI user_fac_QueryInterface(IActivationFactory *iface, REFIID riid, void **ppvObject) { return user_fac_QI_internal(impl_from_user_factory(iface), riid, ppvObject); }
static ULONG WINAPI user_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI user_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI user_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI user_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n) { return WindowsCreateString(L"Windows.System.User", 19, n); }
static HRESULT WINAPI user_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI user_fac_ActivateInstance(IActivationFactory *iface, IInspectable **instance) { return create_user_instance((void **)instance); }

static const IActivationFactoryVtbl user_activation_factory_vtbl = {
    user_fac_QueryInterface,
    user_fac_AddRef,
    user_fac_Release,
    user_fac_GetIids,
    user_fac_GetRuntimeClassName,
    user_fac_GetTrustLevel,
    user_fac_ActivateInstance
};

static HRESULT WINAPI user_s_QueryInterface(void *This, REFIID riid, void **ppvObject) { return user_fac_QI_internal(impl_from_user_statics(This), riid, ppvObject); }
static ULONG WINAPI user_s_AddRef(void *This) { return 2; }
static ULONG WINAPI user_s_Release(void *This) { return 1; }
static HRESULT WINAPI user_s_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI user_s_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.System.User", 19, n); }
static HRESULT WINAPI user_s_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI user_s_CreateWatcher(void *This, void **result)
{
    TRACE("user_s_CreateWatcher\n");
    if (result) *result = NULL;
    return S_OK;
}
static HRESULT WINAPI user_s_FindAllAsync(void *This, void **op)
{
    TRACE("user_s_FindAllAsync\n");
    if (op) *op = NULL;
    return S_OK;
}
static HRESULT WINAPI user_s_FindAllAsyncByType(void *This, int type, void **op)
{
    TRACE("user_s_FindAllAsyncByType %d\n", type);
    if (op) *op = NULL;
    return S_OK;
}
static HRESULT WINAPI user_s_FindAllAsyncByTypeAndStatus(void *This, int type, int status, void **op)
{
    TRACE("user_s_FindAllAsyncByTypeAndStatus %d, %d\n", type, status);
    if (op) *op = NULL;
    return S_OK;
}
static HRESULT WINAPI user_s_GetDefault(void *This, void **result)
{
    TRACE("user_s_GetDefault\n");
    if (!result) return E_POINTER;
    return create_user_instance(result);
}
static HRESULT WINAPI user_s_GetFromId(void *This, HSTRING id, void **result)
{
    TRACE("user_s_GetFromId %s\n", debugstr_hstring(id));
    if (!result) return E_POINTER;
    return create_user_instance(result);
}

static const void *user_statics_vtbl[] = {
    (void *)user_s_QueryInterface,                     /* 0 */
    (void *)user_s_AddRef,                             /* 1 */
    (void *)user_s_Release,                            /* 2 */
    (void *)user_s_GetIids,                            /* 3 */
    (void *)user_s_GetRuntimeClassName,                /* 4 */
    (void *)user_s_GetTrustLevel,                      /* 5 */
    (void *)user_s_CreateWatcher,                      /* 6 */
    (void *)user_s_FindAllAsync,                       /* 7 */
    (void *)user_s_FindAllAsyncByType,                 /* 8 */
    (void *)user_s_FindAllAsyncByTypeAndStatus,        /* 9 */
    (void *)user_s_GetDefault,                         /* 10 */
    (void *)user_s_GetFromId,                          /* 11 */
    (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub
};

static HRESULT WINAPI redhawk_user_fac_QI(void *This, REFIID riid, void **ppvObject) { return user_fac_QI_internal(impl_from_user_redfac(This), riid, ppvObject); }
static ULONG WINAPI redhawk_user_fac_AddRef(void *This) { return 2; }
static ULONG WINAPI redhawk_user_fac_Release(void *This) { return 1; }

static const void *redhawk_user_factory_vtbl[] = {
    (void *)redhawk_user_fac_QI,
    (void *)redhawk_user_fac_AddRef,
    (void *)redhawk_user_fac_Release,
    (void *)redhawk_interop_m3,
    (void *)user_s_GetRuntimeClassName,
    (void *)user_s_GetTrustLevel,
    (void *)user_s_GetDefault,
    (void *)redhawk_interop_m3, /* slot 7: safe hook */
    (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub,
    (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub
};

static struct user_factory_impl default_user_factory = {
    {&user_activation_factory_vtbl},
    {user_statics_vtbl},
    {redhawk_user_factory_vtbl},
    1
};

IActivationFactory *user_factory = &default_user_factory.IActivationFactory_iface;

/* -------------------------------------------------------------------------
 * SizeHelper Implementation
 * ------------------------------------------------------------------------- */
static const GUID IID_ISizeHelperStatics = {0x6286c5b2, 0xcf78, 0x4915, {0xaa, 0x40, 0x76, 0x00, 0x4a, 0x16, 0x5f, 0x5e}};

struct winrt_size
{
    float width;
    float height;
};

struct size_helper_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } ISizeHelperStatics_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    LONG ref;
};

#define impl_from_size_helper_fac(ptr)     ((struct size_helper_factory_impl *)((char *)(ptr) - offsetof(struct size_helper_factory_impl, IActivationFactory_iface)))
#define impl_from_size_helper_statics(ptr) ((struct size_helper_factory_impl *)((char *)(ptr) - offsetof(struct size_helper_factory_impl, ISizeHelperStatics_iface)))
#define impl_from_size_helper_redfac(ptr)  ((struct size_helper_factory_impl *)((char *)(ptr) - offsetof(struct size_helper_factory_impl, IRedhawkInterop_iface)))

static HRESULT size_helper_fac_QI_internal(struct size_helper_factory_impl *impl, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("size_helper_fac %p, riid %s\n", impl, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppvObject = &impl->IActivationFactory_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ISizeHelperStatics))
    {
        *ppvObject = &impl->ISizeHelperStatics_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ComWrappers_internal))
    {
        *ppvObject = &impl->IRedhawkInterop_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    TRACE("size_helper_fac %p: unhandled QI %s\n", impl, debugstr_guid(riid));
    return E_NOINTERFACE;
}

static HRESULT WINAPI size_helper_fac_QueryInterface(IActivationFactory *iface, REFIID riid, void **ppvObject)
{
    return size_helper_fac_QI_internal(impl_from_size_helper_fac(iface), riid, ppvObject);
}
static ULONG WINAPI size_helper_fac_AddRef(IActivationFactory *iface)
{
    return InterlockedIncrement(&impl_from_size_helper_fac(iface)->ref);
}
static ULONG WINAPI size_helper_fac_Release(IActivationFactory *iface)
{
    return InterlockedDecrement(&impl_from_size_helper_fac(iface)->ref);
}
static HRESULT WINAPI size_helper_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i)
{
    if (c) *c = 0;
    return S_OK;
}
static HRESULT WINAPI size_helper_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n)
{
    return WindowsCreateString(L"Windows.UI.Xaml.SizeHelper", 26, n);
}
static HRESULT WINAPI size_helper_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t)
{
    if (t) *t = BaseTrust;
    return S_OK;
}
static HRESULT WINAPI size_helper_fac_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    if (instance) *instance = NULL;
    return E_NOTIMPL;
}

static const struct IActivationFactoryVtbl size_helper_activation_factory_vtbl = {
    size_helper_fac_QueryInterface,
    size_helper_fac_AddRef,
    size_helper_fac_Release,
    size_helper_fac_GetIids,
    size_helper_fac_GetRuntimeClassName,
    size_helper_fac_GetTrustLevel,
    size_helper_fac_ActivateInstance
};

static HRESULT WINAPI size_helper_s_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return size_helper_fac_QI_internal(impl_from_size_helper_statics(This), riid, ppvObject);
}
static ULONG WINAPI size_helper_s_AddRef(void *This)
{
    return InterlockedIncrement(&impl_from_size_helper_statics(This)->ref);
}
static ULONG WINAPI size_helper_s_Release(void *This)
{
    return InterlockedDecrement(&impl_from_size_helper_statics(This)->ref);
}

/* Method for FromDimensions / get_Empty */
static HRESULT WINAPI size_helper_s_slot6(void *This, void *rdx, void *r8, struct winrt_size *r9)
{
    float width, height;
    __asm__ __volatile__("movss %%xmm1, %0; movss %%xmm2, %1" : "=m"(width), "=m"(height));
    TRACE("slot 6: This %p, rdx %p, r8 %p, r9 %p, xmm1=%f, xmm2=%f\n", This, rdx, r8, r9, width, height);
    if (width > 0.0f || height > 0.0f)
    {
        if (r9) { r9->width = width; r9->height = height; }
        return S_OK;
    }
    if (rdx)
    {
        struct winrt_size *s = (struct winrt_size *)rdx;
        s->width = 0.0f;
        s->height = 0.0f;
    }
    return S_OK;
}

static HRESULT WINAPI size_helper_s_slot7(void *This, void *rdx, void *r8, struct winrt_size *r9)
{
    float width, height;
    __asm__ __volatile__("movss %%xmm1, %0; movss %%xmm2, %1" : "=m"(width), "=m"(height));
    TRACE("slot 7: This %p, rdx %p, r8 %p, r9 %p, xmm1=%f, xmm2=%f\n", This, rdx, r8, r9, width, height);
    if (r9)
    {
        r9->width = width;
        r9->height = height;
    }
    else if (rdx)
    {
        struct winrt_size *s = (struct winrt_size *)rdx;
        s->width = width;
        s->height = height;
    }
    return S_OK;
}

static const void *size_helper_statics_vtbl[] = {
    (void *)size_helper_s_QueryInterface,           /* 0 */
    (void *)size_helper_s_AddRef,                   /* 1 */
    (void *)size_helper_s_Release,                  /* 2 */
    (void *)size_helper_fac_GetIids,                /* 3 */
    (void *)size_helper_fac_GetRuntimeClassName,    /* 4 */
    (void *)size_helper_fac_GetTrustLevel,          /* 5 */
    (void *)size_helper_s_slot6,                    /* 6: get_Empty or FromDimensions */
    (void *)size_helper_s_slot7,                    /* 7: FromDimensions or get_Empty */
    (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub
};

static HRESULT WINAPI redhawk_size_helper_fac_QI(void *This, REFIID riid, void **ppvObject)
{
    return size_helper_fac_QI_internal(impl_from_size_helper_redfac(This), riid, ppvObject);
}
static ULONG WINAPI redhawk_size_helper_fac_AddRef(void *This) { return 2; }
static ULONG WINAPI redhawk_size_helper_fac_Release(void *This) { return 1; }

static const void *redhawk_size_helper_factory_vtbl[] = {
    (void *)redhawk_size_helper_fac_QI,
    (void *)redhawk_size_helper_fac_AddRef,
    (void *)redhawk_size_helper_fac_Release,
    (void *)redhawk_interop_m3,
    (void *)size_helper_fac_GetRuntimeClassName,
    (void *)size_helper_fac_GetTrustLevel,
    (void *)size_helper_s_slot7,
    (void *)redhawk_interop_m3, /* slot 7: safe hook */
    (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub, (void *)largs_generic_stub
};

static struct size_helper_factory_impl default_size_helper_factory = {
    {&size_helper_activation_factory_vtbl},
    {size_helper_statics_vtbl},
    {redhawk_size_helper_factory_vtbl},
    1
};

IActivationFactory *size_helper_factory = &default_size_helper_factory.IActivationFactory_iface;

/* -------------------------------------------------------------------------
 * Windows.UI.Xaml.Controls.Frame Implementation
 * ------------------------------------------------------------------------- */
static const GUID IID_IDependencyObject  = {0x5c526665, 0xf60e, 0x4912, {0xaf, 0x59, 0x5f, 0xe0, 0x68, 0x0f, 0x08, 0x9d}};
static const GUID IID_IDependencyObject2 = {0x29fed85d, 0x3d22, 0x43a1, {0xad, 0xd0, 0x17, 0x02, 0x7c, 0x08, 0xb2, 0x12}};
static const GUID IID_IFrameworkElement2 = {0xf19104be, 0x422a, 0x4904, {0xa5, 0x2f, 0xee, 0x72, 0x01, 0x04, 0x29, 0xe5}};
static const GUID IID_IFrameFactory    = {0x02ee93d4, 0x448e, 0x469e, {0x97, 0x99, 0x0a, 0x8a, 0x1f, 0x70, 0xf1, 0x71}};
static const GUID IID_IFrame           = {0x68ea500b, 0x3fe9, 0x4735, {0xb6, 0xa0, 0xe7, 0x60, 0x19, 0x33, 0xb0, 0x89}};
static const GUID IID_IContentControl  = {0x07e81761, 0x11b2, 0x52ae, {0x8f, 0x8b, 0x4d, 0x53, 0xd2, 0xb5, 0x90, 0x0a}};
static const GUID IID_IContentControl_a26dd1dc = {0xa26dd1dc, 0xcd44, 0x435c, {0xbe, 0x94, 0x01, 0xd6, 0x24, 0x1c, 0x23, 0x1c}};
static const GUID IID_IFrameworkElement = {0xa391d09b, 0x4a99, 0x4b7c, {0x9d, 0x8d, 0x6f, 0xa5, 0xd0, 0x1f, 0x6f, 0xbf}};
static const GUID IID_IUIElement        = {0x676d0be9, 0xb65c, 0x41c6, {0xba, 0x40, 0x58, 0xcf, 0x87, 0xf2, 0x01, 0xc1}};
static const GUID IID_INavigate        = {0xbf5dbe99, 0xf53a, 0x4336, {0x97, 0x7c, 0xf8, 0xfc, 0xf7, 0x8b, 0x0d, 0x9e}};
static const GUID IID_IControl     = {0xa8912263, 0x2951, 0x4f58, {0xa9, 0xc5, 0x5a, 0x13, 0x4e, 0xaa, 0x7f, 0x07}};
static const GUID IID_IControl_b4a04776 = {0xb4a04776, 0x4e88, 0x50ca, {0x8f, 0x2b, 0x08, 0x94, 0x0d, 0x6c, 0x5f, 0x94}};
static const GUID IID_IControl_legacy = {0xe5f7dfa7, 0xb1a1, 0x432d, {0x94, 0x77, 0xd7, 0x79, 0x0b, 0x16, 0x86, 0x7d}};

#define RuntimeClass_Frame L"Windows.UI.Xaml.Controls.Frame"

struct frame_impl
{
    struct { const void *lpVtbl; } IFrame_iface;
    struct { const void *lpVtbl; } IContentControl_iface;
    struct { const void *lpVtbl; } IFrameworkElement_iface;
    struct { const void *lpVtbl; } IUIElement_iface;
    struct { const void *lpVtbl; } INavigate_iface;
    struct { const void *lpVtbl; } IDependencyObject_iface;
    struct { const void *lpVtbl; } IDependencyObject2_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    IInspectable *outer;
    IInspectable *content;
    struct dp_entry dp_entries[64];
    UINT32 dp_count;
    LONG ref;
    UINT32 flow_direction;
};

#define impl_from_IFrame(ptr) ((struct frame_impl *)((char *)(ptr) - offsetof(struct frame_impl, IFrame_iface)))
#define impl_from_IContentControl(ptr) ((struct frame_impl *)((char *)(ptr) - offsetof(struct frame_impl, IContentControl_iface)))
#define impl_from_IFrameworkElement(ptr) ((struct frame_impl *)((char *)(ptr) - offsetof(struct frame_impl, IFrameworkElement_iface)))
#define impl_from_IUIElement(ptr) ((struct frame_impl *)((char *)(ptr) - offsetof(struct frame_impl, IUIElement_iface)))
#define impl_from_INavigate(ptr) ((struct frame_impl *)((char *)(ptr) - offsetof(struct frame_impl, INavigate_iface)))
#define impl_from_IFrame_do(ptr)  ((struct frame_impl *)((char *)(ptr) - offsetof(struct frame_impl, IDependencyObject_iface)))
#define impl_from_IFrame_do2(ptr) ((struct frame_impl *)((char *)(ptr) - offsetof(struct frame_impl, IDependencyObject2_iface)))
#define impl_from_IRedhawk_frame(ptr) ((struct frame_impl *)((char *)(ptr) - offsetof(struct frame_impl, IRedhawkInterop_iface)))

static HRESULT frame_QI_internal(struct frame_impl *impl, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("frame %p, riid %s\n", impl, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IFrame))
    {
        *ppvObject = &impl->IFrame_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IContentControl) ||
        IsEqualGUID(riid, &IID_IContentControl_a26dd1dc))
    {
        *ppvObject = &impl->IContentControl_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IControl) ||
        IsEqualGUID(riid, &IID_IControl_b4a04776) ||
        IsEqualGUID(riid, &IID_IControl_legacy))
    {
        *ppvObject = &impl->IContentControl_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IFrameworkElement))
    {
        *ppvObject = &impl->IFrameworkElement_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IUIElement))
    {
        *ppvObject = &impl->IUIElement_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IDependencyObject))
    {
        *ppvObject = &impl->IDependencyObject_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IDependencyObject2))
    {
        *ppvObject = &impl->IDependencyObject2_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IFrameworkElement2))
    {
        *ppvObject = &impl->IFrameworkElement_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_INavigate))
    {
        *ppvObject = &impl->INavigate_iface;
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

    TRACE("frame %p: unhandled QI %s\n", impl, debugstr_guid(riid));
    return E_NOINTERFACE;
}

static HRESULT WINAPI frame_QueryInterface(void *This, REFIID riid, void **ppvObject) { return frame_QI_internal(impl_from_IFrame(This), riid, ppvObject); }
static ULONG WINAPI frame_AddRef(void *This) { return InterlockedIncrement(&impl_from_IFrame(This)->ref); }
static ULONG WINAPI frame_Release(void *This)
{
    struct frame_impl *impl = impl_from_IFrame(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    TRACE("frame %p, ref %ld\n", impl, ref);
    if (!ref)
    {
        UINT32 i;
        if (impl->content) IInspectable_Release(impl->content);
        for (i = 0; i < impl->dp_count; i++)
        {
            if (impl->dp_entries[i].value) IInspectable_Release(impl->dp_entries[i].value);
        }
        free(impl);
    }
    return ref;
}
static HRESULT WINAPI frame_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI frame_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(RuntimeClass_Frame, wcslen(RuntimeClass_Frame), n); }
static HRESULT WINAPI frame_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI frame_get_BackStackDepth(void *This, INT32 *value) { if (value) *value = 0; return S_OK; }
static HRESULT WINAPI frame_get_CacheSize(void *This, INT32 *value) { if (value) *value = 10; return S_OK; }
static HRESULT WINAPI frame_put_CacheSize(void *This, INT32 value) { return S_OK; }
static HRESULT WINAPI frame_get_CanGoBack(void *This, boolean *value) { if (value) *value = FALSE; return S_OK; }
static HRESULT WINAPI frame_get_CanGoForward(void *This, boolean *value) { if (value) *value = FALSE; return S_OK; }
static HRESULT WINAPI frame_get_CurrentSourcePageType(void *This, void *value) { return S_OK; }
static HRESULT WINAPI frame_get_SourcePageType(void *This, void *value) { return S_OK; }
static HRESULT WINAPI frame_put_SourcePageType(void *This, void *value) { return S_OK; }
static HRESULT WINAPI frame_get_BackStack(void *This, void **value) { if (value) *value = NULL; return S_OK; }
static HRESULT WINAPI frame_get_ForwardStack(void *This, void **value) { if (value) *value = NULL; return S_OK; }
static HRESULT WINAPI frame_add_Navigated(void *This, void *handler, EventRegistrationToken *token) { if (token) token->value = 1; return S_OK; }
static HRESULT WINAPI frame_remove_Navigated(void *This, EventRegistrationToken token) { return S_OK; }
static HRESULT WINAPI frame_add_Navigating(void *This, void *handler, EventRegistrationToken *token) { if (token) token->value = 1; return S_OK; }
static HRESULT WINAPI frame_remove_Navigating(void *This, EventRegistrationToken token) { return S_OK; }
static HRESULT WINAPI frame_add_NavigationFailed(void *This, void *handler, EventRegistrationToken *token) { TRACE("frame_add_NavigationFailed %p\n", handler); if (token) token->value = 1; return S_OK; }
static HRESULT WINAPI frame_remove_NavigationFailed(void *This, EventRegistrationToken token) { return S_OK; }
static HRESULT WINAPI frame_add_NavigationStopped(void *This, void *handler, EventRegistrationToken *token) { if (token) token->value = 1; return S_OK; }
static HRESULT WINAPI frame_remove_NavigationStopped(void *This, EventRegistrationToken token) { return S_OK; }

/* -------------------------------------------------------------------------
 * NavigationEventArgs Implementation
 * ------------------------------------------------------------------------- */
static const GUID IID_IPageOverrides        = {0x230b9cb8, 0x3419, 0x4306, {0xaa, 0xbe, 0x4a, 0x8e, 0x68, 0x0d, 0x36, 0x27}};
static const GUID IID_INavigationEventArgs  = {0xb6aa9834, 0x6691, 0x44d1, {0xbd, 0xf7, 0x58, 0x82, 0x0c, 0x27, 0xb0, 0xd0}};
static const GUID IID_INavigationEventArgs2 = {0xdbff71d9, 0x979a, 0x4b2e, {0xa4, 0x9b, 0x3b, 0xb1, 0x7f, 0xde, 0xf5, 0x74}};

struct nav_args_impl
{
    struct { const void *lpVtbl; } INavigationEventArgs_iface;
    struct { const void *lpVtbl; } INavigationEventArgs2_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    LONG ref;
    void *content;
    void *parameter;
};

#define impl_from_INavigationEventArgs(ptr)  ((struct nav_args_impl *)((char *)(ptr) - offsetof(struct nav_args_impl, INavigationEventArgs_iface)))
#define impl_from_INavigationEventArgs2(ptr) ((struct nav_args_impl *)((char *)(ptr) - offsetof(struct nav_args_impl, INavigationEventArgs2_iface)))
#define impl_from_IRedhawk_nav_args(ptr)     ((struct nav_args_impl *)((char *)(ptr) - offsetof(struct nav_args_impl, IRedhawkInterop_iface)))

static HRESULT nav_args_QI_internal(struct nav_args_impl *impl, REFIID riid, void **ppv)
{
    if (!ppv) return E_POINTER;
    *ppv = NULL;

    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_INavigationEventArgs))
    {
        *ppv = &impl->INavigationEventArgs_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_INavigationEventArgs2))
    {
        *ppv = &impl->INavigationEventArgs2_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ComWrappers_internal))
    {
        *ppv = &impl->IRedhawkInterop_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    TRACE("nav_args %p: unhandled QI %s\n", impl, debugstr_guid(riid));
    return E_NOINTERFACE;
}

static HRESULT WINAPI nav_args_QueryInterface(void *This, REFIID riid, void **ppv) { return nav_args_QI_internal(impl_from_INavigationEventArgs(This), riid, ppv); }
static ULONG WINAPI nav_args_AddRef(void *This) { return InterlockedIncrement(&impl_from_INavigationEventArgs(This)->ref); }
static ULONG WINAPI nav_args_Release(void *This)
{
    struct nav_args_impl *impl = impl_from_INavigationEventArgs(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref)
    {
        if (impl->content) IUnknown_Release((IUnknown *)impl->content);
        if (impl->parameter) IUnknown_Release((IUnknown *)impl->parameter);
        free(impl);
    }
    return ref;
}
static HRESULT WINAPI nav_args_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI nav_args_GetRuntimeClassName(void *This, HSTRING *n)
{
    static const WCHAR name[] = L"Windows.UI.Xaml.Navigation.NavigationEventArgs";
    return WindowsCreateString(name, wcslen(name), n);
}
static HRESULT WINAPI nav_args_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI nav_args_get_Content(void *This, void **value)
{
    struct nav_args_impl *impl = impl_from_INavigationEventArgs(This);
    if (!value) return E_POINTER;
    *value = impl->content;
    if (*value) IUnknown_AddRef((IUnknown *)*value);
    return S_OK;
}
static HRESULT WINAPI nav_args_get_Parameter(void *This, void **value)
{
    struct nav_args_impl *impl = impl_from_INavigationEventArgs(This);
    if (!value) return E_POINTER;
    *value = impl->parameter;
    if (*value) IUnknown_AddRef((IUnknown *)*value);
    return S_OK;
}
static HRESULT WINAPI nav_args_get_SourcePageType(void *This, void *value) { return S_OK; }
static HRESULT WINAPI nav_args_get_NavigationMode(void *This, int *value)
{
    if (!value) return E_POINTER;
    *value = 0; /* NavigationMode_New */
    return S_OK;
}
static HRESULT WINAPI nav_args_get_Uri(void *This, void **value)
{
    if (!value) return E_POINTER;
    *value = NULL;
    return S_OK;
}
static HRESULT WINAPI nav_args_put_Uri(void *This, void *value) { return S_OK; }

static const void *nav_args_vtbl[] = {
    (void *)nav_args_QueryInterface,
    (void *)nav_args_AddRef,
    (void *)nav_args_Release,
    (void *)nav_args_GetIids,
    (void *)nav_args_GetRuntimeClassName,
    (void *)nav_args_GetTrustLevel,
    (void *)nav_args_get_Content,
    (void *)nav_args_get_Parameter,
    (void *)nav_args_get_SourcePageType,
    (void *)nav_args_get_NavigationMode,
    (void *)nav_args_get_Uri,
    (void *)nav_args_put_Uri
};

static HRESULT WINAPI nav_args2_QueryInterface(void *This, REFIID riid, void **ppv) { return nav_args_QI_internal(impl_from_INavigationEventArgs2(This), riid, ppv); }
static ULONG WINAPI nav_args2_AddRef(void *This) { return nav_args_AddRef(impl_from_INavigationEventArgs2(This)); }
static ULONG WINAPI nav_args2_Release(void *This) { return nav_args_Release(impl_from_INavigationEventArgs2(This)); }
static HRESULT WINAPI nav_args2_get_NavigationTransitionInfo(void *This, void **value)
{
    if (!value) return E_POINTER;
    *value = NULL;
    return S_OK;
}

static const void *nav_args2_vtbl[] = {
    (void *)nav_args2_QueryInterface,
    (void *)nav_args2_AddRef,
    (void *)nav_args2_Release,
    (void *)nav_args_GetIids,
    (void *)nav_args_GetRuntimeClassName,
    (void *)nav_args_GetTrustLevel,
    (void *)nav_args2_get_NavigationTransitionInfo
};

static HRESULT WINAPI redhawk_nav_args_QI(void *This, REFIID riid, void **ppv) { return nav_args_QI_internal(impl_from_IRedhawk_nav_args(This), riid, ppv); }
static ULONG WINAPI redhawk_nav_args_AddRef(void *This) { return nav_args_AddRef(impl_from_IRedhawk_nav_args(This)); }
static ULONG WINAPI redhawk_nav_args_Release(void *This) { return nav_args_Release(impl_from_IRedhawk_nav_args(This)); }

static const void *redhawk_nav_args_vtbl[32] = {
    (void *)redhawk_nav_args_QI,
    (void *)redhawk_nav_args_AddRef,
    (void *)redhawk_nav_args_Release,
    (void *)redhawk_interop_m3,
    (void *)nav_args_GetRuntimeClassName,
    (void *)nav_args_GetTrustLevel,
    (void *)nav_args_get_Content,
    (void *)redhawk_interop_m3, /* slot 7 safe hook */
    (void *)nav_args_get_Parameter,
    (void *)redhawk_interop_m3, (void *)redhawk_interop_m3, (void *)redhawk_interop_m3, (void *)redhawk_interop_m3,
    (void *)redhawk_interop_m3, (void *)redhawk_interop_m3, (void *)redhawk_interop_m3, (void *)redhawk_interop_m3
};

static HRESULT create_navigation_event_args(void *content, void *parameter, void **out)
{
    struct nav_args_impl *impl = calloc(1, sizeof(*impl));
    if (!impl) return E_OUTOFMEMORY;
    impl->INavigationEventArgs_iface.lpVtbl = nav_args_vtbl;
    impl->INavigationEventArgs2_iface.lpVtbl = nav_args2_vtbl;
    impl->IRedhawkInterop_iface.lpVtbl = redhawk_nav_args_vtbl;
    impl->ref = 1;
    impl->content = content;
    impl->parameter = parameter;
    if (content) IUnknown_AddRef((IUnknown *)content);
    if (parameter) IUnknown_AddRef((IUnknown *)parameter);
    *out = &impl->INavigationEventArgs_iface;
    return S_OK;
}

static HRESULT WINAPI frame_Navigate(void *This, void *sourcePageType, void *parameter, boolean *result)
{
    TRACE("Frame::Navigate called! This %p, type %p, param %p, result %p\n", This, sourcePageType, parameter, result);
    if (sourcePageType && (uintptr_t)sourcePageType > 0x10000)
    {
        HSTRING *hstr = (HSTRING *)sourcePageType;
        UINT32 *kind = (UINT32 *)((char *)sourcePageType + sizeof(HSTRING));
        TRACE("TypeName: Name %s, Kind %u\n", debugstr_hstring(*hstr), *kind);
    }
    if (current_application)
    {
        struct application_impl *app_impl = impl_from_IApplication(current_application);
        void *outer = app_impl->outer;
        if (outer)
        {
            void **outer_vtbl = *(void ***)outer;
            if (outer_vtbl && (ULONG_PTR)outer_vtbl[0] > 0x10000)
            {
                static const GUID IID_IXamlMetadataProvider_real = {0xb3765d69, 0x68a5, 0x4b32, {0x88, 0x61, 0xfd, 0xb9, 0x0c, 0x1f, 0x58, 0x36}};
                HRESULT (WINAPI *pQI)(void *, REFIID, void **) = (void *)outer_vtbl[0];
                HRESULT (WINAPI *pGetIids)(void *, ULONG *, GUID **) = (void *)outer_vtbl[3];
                ULONG count = 0;
                GUID *iids = NULL;
                void *provider = NULL;
                HRESULT hr;
                HRESULT hr_iids = pGetIids(outer, &count, &iids);
                TRACE("outer GetIids returned hr=0x%08lx, count=%lu\n", hr_iids, count);
                if (SUCCEEDED(hr_iids) && iids)
                {
                    ULONG i;
                    for (i = 0; i < count; i++)
                        TRACE("  outer IID[%lu]: %s\n", i, debugstr_guid(&iids[i]));
                }

                hr = pQI(outer, &IID_IXamlMetadataProvider_real, &provider);
                TRACE("pQI for IXamlMetadataProvider_real hr=0x%08lx, provider=%p\n", hr, provider);
                if (SUCCEEDED(hr) && provider)
                {
                    void **prov_vtbl = *(void ***)provider;
                    TRACE("prov_vtbl = %p, slots: 0=%p, 1=%p, 2=%p, 3=%p, 4=%p, 5=%p, 6=%p, 7=%p, 8=%p\n",
                          prov_vtbl, prov_vtbl[0], prov_vtbl[1], prov_vtbl[2], prov_vtbl[3], prov_vtbl[4], prov_vtbl[5], prov_vtbl[6], prov_vtbl[7], prov_vtbl[8]);
                    if (sourcePageType)
                    {
                        void *xamlType = NULL;
                        HRESULT (WINAPI *pGetXamlType)(void *, void *, void **) = (void *)prov_vtbl[6];
                        HRESULT hr_type = pGetXamlType(provider, sourcePageType, &xamlType);
                        TRACE("GetXamlType(slot 6) returned hr=0x%08lx, xamlType=%p\n", hr_type, xamlType);
                        if (FAILED(hr_type) && prov_vtbl[7])
                        {
                            HSTRING *hstr = (HSTRING *)sourcePageType;
                            HRESULT (WINAPI *pGetXamlTypeByName)(void *, HSTRING, void **) = (void *)prov_vtbl[7];
                            hr_type = pGetXamlTypeByName(provider, *hstr, &xamlType);
                            TRACE("GetXamlTypeByName(slot 7) returned hr=0x%08lx, xamlType=%p\n", hr_type, xamlType);
                        }
                        if (SUCCEEDED(hr_type) && xamlType)
                        {
                            void **type_vtbl = *(void ***)xamlType;
                            void *page = NULL;
                            HRESULT (WINAPI *pRunInit)(void *) = (void *)type_vtbl[23];
                            HRESULT (WINAPI *pActivateInstance)(void *, void **) = (void *)type_vtbl[18];
                            HRESULT hr_init;
                            HRESULT hr_act;

                            hr_init = pRunInit(xamlType);
                            TRACE("RunInitializer(slot 23) returned hr=0x%08lx\n", hr_init);

                            hr_act = pActivateInstance(xamlType, &page);
                            TRACE("ActivateInstance(slot 18) returned hr=0x%08lx, page=%p\n", hr_act, page);

                            if (SUCCEEDED(hr_act) && page)
                            {
                                struct frame_impl *f_impl = impl_from_IFrame(This);
                                f_impl->content = page;
                                TRACE("Frame::Navigate successfully instantiated MainPage and set Frame.Content=%p!\n", page);

                                /* Fire IPageOverrides::OnNavigatedTo */
                                {
                                    void *page_overrides = NULL;
                                    HRESULT hr_po = IUnknown_QueryInterface((IUnknown *)page, &IID_IPageOverrides, &page_overrides);
                                    TRACE("QI IPageOverrides on page %p returned hr=0x%08lx, page_overrides=%p\n", page, hr_po, page_overrides);
                                    if (SUCCEEDED(hr_po) && page_overrides)
                                    {
                                        void *nav_args = NULL;
                                        void **po_vtbl = *(void ***)page_overrides;
                                        HRESULT (WINAPI *pOnNavigatedTo)(void *, void *) = (void *)po_vtbl[7];
                                        create_navigation_event_args(page, parameter, &nav_args);
                                        TRACE("Invoking IPageOverrides::OnNavigatedTo(slot 7 = %p) with nav_args=%p\n", pOnNavigatedTo, nav_args);
                                        if (pOnNavigatedTo)
                                        {
                                            HRESULT hr_nav = pOnNavigatedTo(page_overrides, nav_args);
                                            TRACE("IPageOverrides::OnNavigatedTo returned hr=0x%08lx\n", hr_nav);
                                        }
                                        if (nav_args) IUnknown_Release((IUnknown *)nav_args);
                                        IUnknown_Release((IUnknown *)page_overrides);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if (result && (uintptr_t)result > 0x10000)
    {
        *result = TRUE;
        TRACE("Frame::Navigate: set *result = TRUE\n");
    }
    return S_OK;
}
static HRESULT WINAPI frame_NavigateWithParam(void *This, void *sourcePageType, void *parameter, boolean *result)
{
    TRACE("Frame::NavigateWithParam called! This %p, type %p, param %p, result %p\n", This, sourcePageType, parameter, result);
    if (result && (uintptr_t)result > 0x10000)
    {
        *result = TRUE;
        TRACE("Frame::NavigateWithParam: set *result = TRUE\n");
    }
    return S_OK;
}
static HRESULT WINAPI frame_GoBack(void *This) { return S_OK; }
static HRESULT WINAPI frame_GoForward(void *This) { return S_OK; }
static HRESULT WINAPI frame_GetNavigationState(void *This, HSTRING *result) { if (result) *result = NULL; return S_OK; }
static HRESULT WINAPI frame_SetNavigationState(void *This, HSTRING navigationState) { return S_OK; }

static const void *frame_vtbl[] = {
    (void *)frame_QueryInterface,           /* 0 */
    (void *)frame_AddRef,                   /* 1 */
    (void *)frame_Release,                  /* 2 */
    (void *)frame_GetIids,                  /* 3 */
    (void *)frame_GetRuntimeClassName,      /* 4 */
    (void *)frame_GetTrustLevel,            /* 5 */
    (void *)frame_get_BackStackDepth,       /* 6 */
    (void *)frame_get_CacheSize,            /* 7 */
    (void *)frame_put_CacheSize,            /* 8 */
    (void *)frame_get_CanGoBack,            /* 9 */
    (void *)frame_get_CanGoForward,         /* 10 */
    (void *)frame_get_CurrentSourcePageType, /* 11 */
    (void *)frame_get_SourcePageType,       /* 12 */
    (void *)frame_put_SourcePageType,       /* 13 */
    (void *)frame_get_BackStack,            /* 14 */
    (void *)frame_get_ForwardStack,         /* 15 */
    (void *)frame_add_Navigated,            /* 16 */
    (void *)frame_remove_Navigated,         /* 17 */
    (void *)frame_add_Navigating,           /* 18 */
    (void *)frame_remove_Navigating,        /* 19 */
    (void *)frame_add_NavigationFailed,     /* 20 */
    (void *)frame_remove_NavigationFailed,  /* 21 */
    (void *)frame_add_NavigationStopped,    /* 22 */
    (void *)frame_remove_NavigationStopped, /* 23 */
    (void *)frame_Navigate,                 /* 24 */
    (void *)frame_NavigateWithParam,        /* 25 */
    (void *)frame_GoBack,                   /* 26 */
    (void *)frame_GoForward,                /* 27 */
    (void *)frame_GetNavigationState,       /* 28 */
    (void *)frame_SetNavigationState        /* 29 */
};

/* IContentControl */
static HRESULT WINAPI frame_cc_QI(void *This, REFIID riid, void **ppvObject) { return frame_QI_internal(impl_from_IContentControl(This), riid, ppvObject); }
static ULONG WINAPI frame_cc_AddRef(void *This) { return frame_AddRef(&impl_from_IContentControl(This)->IFrame_iface); }
static ULONG WINAPI frame_cc_Release(void *This) { return frame_Release(&impl_from_IContentControl(This)->IFrame_iface); }
static HRESULT WINAPI frame_cc_get_Content(void *This, void **value)
{
    struct frame_impl *impl = impl_from_IContentControl(This);
    TRACE("Frame::get_Content -> %p\n", impl->content);
    if (!value) return E_POINTER;
    *value = impl->content;
    if (impl->content) IInspectable_AddRef(impl->content);
    return S_OK;
}
static HRESULT WINAPI frame_cc_put_Content(void *This, void *value)
{
    struct frame_impl *impl = impl_from_IContentControl(This);
    TRACE("Frame::put_Content %p\n", value);
    if (value) IInspectable_AddRef((IInspectable *)value);
    if (impl->content) IInspectable_Release(impl->content);
    impl->content = value;
    return S_OK;
}
static HRESULT WINAPI frame_generic_stub(void *This) { return S_OK; }

static const void *frame_content_control_vtbl[32] = {
    (void *)frame_cc_QI,
    (void *)frame_cc_AddRef,
    (void *)frame_cc_Release,
    (void *)frame_GetIids,
    (void *)frame_GetRuntimeClassName,
    (void *)frame_GetTrustLevel,
    (void *)frame_cc_get_Content,           /* 6 */
    (void *)frame_cc_put_Content,           /* 7 */
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub
};

/* IFrameworkElement */
static HRESULT WINAPI frame_fe_QI(void *This, REFIID riid, void **ppvObject) { return frame_QI_internal(impl_from_IFrameworkElement(This), riid, ppvObject); }
static ULONG WINAPI frame_fe_AddRef(void *This) { return frame_AddRef(&impl_from_IFrameworkElement(This)->IFrame_iface); }
static ULONG WINAPI frame_fe_Release(void *This) { return frame_Release(&impl_from_IFrameworkElement(This)->IFrame_iface); }

static HRESULT WINAPI frame_fe_get_FlowDirection(void *This, UINT32 *value)
{
    struct frame_impl *impl = impl_from_IFrameworkElement(This);
    TRACE("frame %p, FlowDirection -> %u\n", impl, impl->flow_direction);
    if (value) *value = impl->flow_direction;
    return S_OK;
}

static HRESULT WINAPI frame_fe_put_FlowDirection(void *This, UINT32 value)
{
    struct frame_impl *impl = impl_from_IFrameworkElement(This);
    TRACE("frame %p, FlowDirection <- %u\n", impl, value);
    impl->flow_direction = value;
    return S_OK;
}

static const void *frame_fe_vtbl[64] = {
    (void *)frame_fe_QI,
    (void *)frame_fe_AddRef,
    (void *)frame_fe_Release,
    (void *)frame_GetIids,
    (void *)frame_GetRuntimeClassName,
    (void *)frame_GetTrustLevel,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_fe_get_FlowDirection,
    (void *)frame_fe_put_FlowDirection, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub
};

/* IUIElement */
static HRESULT WINAPI frame_ui_QI(void *This, REFIID riid, void **ppvObject) { return frame_QI_internal(impl_from_IUIElement(This), riid, ppvObject); }
static ULONG WINAPI frame_ui_AddRef(void *This) { return frame_AddRef(&impl_from_IUIElement(This)->IFrame_iface); }
static ULONG WINAPI frame_ui_Release(void *This) { return frame_Release(&impl_from_IUIElement(This)->IFrame_iface); }

static const void *frame_ui_element_vtbl[32] = {
    (void *)frame_ui_QI,
    (void *)frame_ui_AddRef,
    (void *)frame_ui_Release,
    (void *)frame_GetIids,
    (void *)frame_GetRuntimeClassName,
    (void *)frame_GetTrustLevel,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub
};

/* INavigate */
static HRESULT WINAPI frame_nav_QI(void *This, REFIID riid, void **ppvObject) { return frame_QI_internal(impl_from_INavigate(This), riid, ppvObject); }
static ULONG WINAPI frame_nav_AddRef(void *This) { return frame_AddRef(&impl_from_INavigate(This)->IFrame_iface); }
static ULONG WINAPI frame_nav_Release(void *This) { return frame_Release(&impl_from_INavigate(This)->IFrame_iface); }

static const void *frame_navigate_vtbl[16] = {
    (void *)frame_nav_QI,
    (void *)frame_nav_AddRef,
    (void *)frame_nav_Release,
    (void *)frame_GetIids,
    (void *)frame_GetRuntimeClassName,
    (void *)frame_GetTrustLevel,
    (void *)frame_Navigate,                 /* 6 */
    (void *)redhawk_interop_m3,             /* 7 safe hook */
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub
};

/* Redhawk Frame wrapper */
static HRESULT WINAPI redhawk_frame_QI(void *This, REFIID riid, void **ppvObject) { return frame_QI_internal(impl_from_IRedhawk_frame(This), riid, ppvObject); }
static ULONG WINAPI redhawk_frame_AddRef(void *This) { return frame_AddRef(&impl_from_IRedhawk_frame(This)->IFrame_iface); }
static ULONG WINAPI redhawk_frame_Release(void *This) { return frame_Release(&impl_from_IRedhawk_frame(This)->IFrame_iface); }

static const void *redhawk_frame_vtbl[32] = {
    (void *)redhawk_frame_QI,
    (void *)redhawk_frame_AddRef,
    (void *)redhawk_frame_Release,
    (void *)redhawk_interop_m3,
    (void *)frame_GetRuntimeClassName,
    (void *)frame_GetTrustLevel,
    (void *)frame_generic_stub,             /* 6 */
    (void *)redhawk_interop_m3,             /* 7 safe hook */
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_Navigate,                 /* 24 */
    (void *)frame_NavigateWithParam,        /* 25 */
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub
};

/* DependencyObject methods for Frame */
static HRESULT WINAPI frame_do_QI(void *This, REFIID riid, void **ppvObject) { return frame_QI_internal(impl_from_IFrame_do(This), riid, ppvObject); }
static ULONG WINAPI frame_do_AddRef(void *This) { return frame_AddRef(&impl_from_IFrame_do(This)->IFrame_iface); }
static ULONG WINAPI frame_do_Release(void *This) { return frame_Release(&impl_from_IFrame_do(This)->IFrame_iface); }
static HRESULT WINAPI frame_do_GetValue(void *This, void *dp, IInspectable **result)
{
    struct frame_impl *impl = impl_from_IFrame_do(This);
    UINT32 i;
    TRACE("Frame DO::GetValue This %p, dp %p\n", This, dp);
    if (!result) return E_POINTER;
    *result = NULL;
    for (i = 0; i < impl->dp_count; i++)
    {
        if (impl->dp_entries[i].dp == dp)
        {
            *result = impl->dp_entries[i].value;
            if (*result) IInspectable_AddRef(*result);
            TRACE("Frame DO::GetValue found stored value %p\n", *result);
            return S_OK;
        }
    }
    return dependency_property_get_default_value(dp, result);
}
static HRESULT WINAPI frame_do_SetValue(void *This, void *dp, IInspectable *value)
{
    struct frame_impl *impl = impl_from_IFrame_do(This);
    UINT32 i;
    TRACE("Frame DO::SetValue This %p, dp %p, value %p\n", This, dp, value);
    if (!dp) return E_POINTER;
    for (i = 0; i < impl->dp_count; i++)
    {
        if (impl->dp_entries[i].dp == dp)
        {
            if (value) IInspectable_AddRef(value);
            if (impl->dp_entries[i].value) IInspectable_Release(impl->dp_entries[i].value);
            impl->dp_entries[i].value = value;
            return S_OK;
        }
    }
    if (impl->dp_count < ARRAY_SIZE(impl->dp_entries))
    {
        impl->dp_entries[impl->dp_count].dp = dp;
        impl->dp_entries[impl->dp_count].value = value;
        if (value) IInspectable_AddRef(value);
        impl->dp_count++;
    }
    return S_OK;
}
static HRESULT WINAPI frame_do_ClearValue(void *This, void *dp)
{
    TRACE("Frame DO::ClearValue This %p, dp %p\n", This, dp);
    return S_OK;
}
static HRESULT WINAPI frame_do_ReadLocalValue(void *This, void *dp, IInspectable **result)
{
    TRACE("Frame DO::ReadLocalValue This %p, dp %p\n", This, dp);
    return frame_do_GetValue(This, dp, result);
}
static HRESULT WINAPI frame_do_GetAnimationBaseValue(void *This, void *dp, IInspectable **result)
{
    TRACE("Frame DO::GetAnimationBaseValue This %p, dp %p\n", This, dp);
    return frame_do_GetValue(This, dp, result);
}
static HRESULT WINAPI frame_do_get_Dispatcher(void *This, void **value)
{
    TRACE("Frame DO::get_Dispatcher This %p\n", This);
    if (!value) return E_POINTER;
    return get_global_dispatcher(value);
}

static const void *frame_dependency_object_vtbl[16] = {
    (void *)frame_do_QI,
    (void *)frame_do_AddRef,
    (void *)frame_do_Release,
    (void *)frame_GetIids,
    (void *)frame_GetRuntimeClassName,
    (void *)frame_GetTrustLevel,
    (void *)frame_do_GetValue,
    (void *)frame_do_SetValue,
    (void *)frame_do_ClearValue,
    (void *)frame_do_ReadLocalValue,
    (void *)frame_do_GetAnimationBaseValue,
    (void *)frame_do_get_Dispatcher
};

/* IDependencyObject2 for Frame */
static HRESULT WINAPI frame_do2_QI(void *This, REFIID riid, void **ppvObject) { return frame_QI_internal(impl_from_IFrame_do2(This), riid, ppvObject); }
static ULONG WINAPI frame_do2_AddRef(void *This) { return frame_AddRef(&impl_from_IFrame_do2(This)->IFrame_iface); }
static ULONG WINAPI frame_do2_Release(void *This) { return frame_Release(&impl_from_IFrame_do2(This)->IFrame_iface); }
static HRESULT WINAPI frame_do2_RegisterPropertyChangedCallback(void *This, void *dp, void *callback, INT64 *result)
{
    TRACE("Frame DO2::RegisterPropertyChangedCallback\n");
    if (!result) return E_POINTER;
    *result = 1;
    return S_OK;
}
static HRESULT WINAPI frame_do2_UnregisterPropertyChangedCallback(void *This, void *dp, INT64 token)
{
    TRACE("Frame DO2::UnregisterPropertyChangedCallback token %lld\n", token);
    return S_OK;
}

static const void *frame_dependency_object2_vtbl[16] = {
    (void *)frame_do2_QI,
    (void *)frame_do2_AddRef,
    (void *)frame_do2_Release,
    (void *)frame_GetIids,
    (void *)frame_GetRuntimeClassName,
    (void *)frame_GetTrustLevel,
    (void *)frame_do2_RegisterPropertyChangedCallback,
    (void *)frame_do2_UnregisterPropertyChangedCallback
};

static HRESULT create_frame_instance(void *outer, void **inner, void **instance)
{
    struct frame_impl *impl;
    if (!instance) return E_POINTER;
    impl = calloc(1, sizeof(*impl));
    if (!impl) return E_OUTOFMEMORY;
    impl->IFrame_iface.lpVtbl = frame_vtbl;
    impl->IContentControl_iface.lpVtbl = frame_content_control_vtbl;
    impl->IFrameworkElement_iface.lpVtbl = frame_fe_vtbl;
    impl->IUIElement_iface.lpVtbl = frame_ui_element_vtbl;
    impl->INavigate_iface.lpVtbl = frame_navigate_vtbl;
    impl->IDependencyObject_iface.lpVtbl = frame_dependency_object_vtbl;
    impl->IDependencyObject2_iface.lpVtbl = frame_dependency_object2_vtbl;
    impl->IRedhawkInterop_iface.lpVtbl = redhawk_frame_vtbl;
    impl->outer = outer;
    impl->ref = 1;
    if (inner)
    {
        *inner = &impl->IFrame_iface;
        InterlockedIncrement(&impl->ref);
    }
    *instance = &impl->IFrame_iface;
    TRACE("Created Frame instance %p, inner %p, ref %ld\n", *instance, inner ? *inner : NULL, impl->ref);
    return S_OK;
}

/* Frame Factory */
struct frame_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } IFrameFactory_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    LONG ref;
};

#define impl_from_frame_factory(ptr)   ((struct frame_factory_impl *)((char *)(ptr) - offsetof(struct frame_factory_impl, IActivationFactory_iface)))
#define impl_from_frame_fac_iface(ptr) ((struct frame_factory_impl *)((char *)(ptr) - offsetof(struct frame_factory_impl, IFrameFactory_iface)))
#define impl_from_frame_redfac(ptr)    ((struct frame_factory_impl *)((char *)(ptr) - offsetof(struct frame_factory_impl, IRedhawkInterop_iface)))

static HRESULT frame_fac_QI_internal(struct frame_factory_impl *impl, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("frame_fac %p, riid %s\n", impl, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppvObject = &impl->IActivationFactory_iface;
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IFrameFactory))
    {
        *ppvObject = &impl->IFrameFactory_iface;
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

    TRACE("frame_fac %p: unhandled QI %s\n", impl, debugstr_guid(riid));
    return E_NOINTERFACE;
}

static HRESULT WINAPI frame_fac_QueryInterface(IActivationFactory *iface, REFIID riid, void **ppvObject) { return frame_fac_QI_internal(impl_from_frame_factory(iface), riid, ppvObject); }
static ULONG WINAPI frame_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI frame_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI frame_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI frame_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n) { return WindowsCreateString(RuntimeClass_Frame, wcslen(RuntimeClass_Frame), n); }
static HRESULT WINAPI frame_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI frame_fac_CreateInstance(void *This, void *outer, void **inner, void **instance)
{
    return create_frame_instance(outer, inner, instance);
}
static HRESULT WINAPI frame_fac_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    return create_frame_instance(NULL, NULL, (void **)instance);
}

static const IActivationFactoryVtbl frame_activation_factory_vtbl = {
    frame_fac_QueryInterface,
    frame_fac_AddRef,
    frame_fac_Release,
    frame_fac_GetIids,
    frame_fac_GetRuntimeClassName,
    frame_fac_GetTrustLevel,
    frame_fac_ActivateInstance
};

static HRESULT WINAPI frame_ff_QI(void *This, REFIID riid, void **ppvObject) { return frame_fac_QI_internal(impl_from_frame_fac_iface(This), riid, ppvObject); }
static ULONG WINAPI frame_ff_AddRef(void *This) { return 2; }
static ULONG WINAPI frame_ff_Release(void *This) { return 1; }

static const void *frame_factory_vtbl[] = {
    (void *)frame_ff_QI,
    (void *)frame_ff_AddRef,
    (void *)frame_ff_Release,
    (void *)frame_fac_GetIids,
    (void *)frame_fac_GetRuntimeClassName,
    (void *)frame_fac_GetTrustLevel,
    (void *)frame_fac_CreateInstance,
    (void *)redhawk_interop_m3,                       /* slot 7 safe hook */
    (void *)redhawk_interop_m3,
    (void *)redhawk_interop_m3,
    (void *)redhawk_interop_m3
};

static HRESULT WINAPI redhawk_frame_fac_QI(void *This, REFIID riid, void **ppvObject) { return frame_fac_QI_internal(impl_from_frame_redfac(This), riid, ppvObject); }
static ULONG WINAPI redhawk_frame_fac_AddRef(void *This) { return 2; }
static ULONG WINAPI redhawk_frame_fac_Release(void *This) { return 1; }

static const void *redhawk_frame_factory_vtbl[] = {
    (void *)redhawk_frame_fac_QI,
    (void *)redhawk_frame_fac_AddRef,
    (void *)redhawk_frame_fac_Release,
    (void *)redhawk_interop_m3,
    (void *)frame_fac_GetRuntimeClassName,
    (void *)frame_fac_GetTrustLevel,
    (void *)frame_fac_CreateInstance,
    (void *)redhawk_interop_m3,                       /* slot 7 safe hook */
    (void *)redhawk_interop_m3,
    (void *)redhawk_interop_m3,
    (void *)redhawk_interop_m3
};

static struct frame_factory_impl default_frame_factory = {
    {&frame_activation_factory_vtbl},
    {frame_factory_vtbl},
    {redhawk_frame_factory_vtbl},
    1
};

IActivationFactory *frame_factory = &default_frame_factory.IActivationFactory_iface;

/* -------------------------------------------------------------------------
 * Page Implementation
 * ------------------------------------------------------------------------- */
#define RuntimeClass_Page L"Windows.UI.Xaml.Controls.Page"

static const GUID IID_IPageFactory = {0xdfa149ac, 0x1849, 0x445e, {0x93, 0x7c, 0x40, 0xa9, 0x59, 0x0c, 0xc0, 0x76}};
static const GUID IID_IPage        = {0xc4b27075, 0xe641, 0x453e, {0x82, 0x4d, 0x01, 0x2f, 0xc7, 0xcf, 0x95, 0xcf}};
static const GUID IID_IUserControl = {0xa7a69ec9, 0xea35, 0x4679, {0xbf, 0x29, 0xf4, 0xf0, 0x92, 0x86, 0xd3, 0x14}};
static const GUID IID_IUserControl_legacy = {0x22880753, 0x3889, 0x4d6d, {0x88, 0x57, 0xe6, 0xfa, 0xe8, 0xdf, 0x31, 0x1d}};

struct page_impl
{
    IInspectable INonDelegatingInspectable_iface;
    struct { const void *lpVtbl; } IPage_iface;
    struct { const void *lpVtbl; } IUserControl_iface;
    struct { const void *lpVtbl; } IControl_iface;
    struct { const void *lpVtbl; } IFrameworkElement_iface;
    struct { const void *lpVtbl; } IUIElement_iface;
    struct { const void *lpVtbl; } IDependencyObject_iface;
    struct { const void *lpVtbl; } IDependencyObject2_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    void *outer;
    void *content;
    void *frame;
    struct dp_entry dp_entries[64];
    UINT32 dp_count;
    LONG ref;
};

#define impl_from_INonDelegatingInspectable(ptr) ((struct page_impl *)((char *)(ptr) - offsetof(struct page_impl, INonDelegatingInspectable_iface)))
#define impl_from_IPage(ptr) ((struct page_impl *)((char *)(ptr) - offsetof(struct page_impl, IPage_iface)))
#define impl_from_IUserControl(ptr) ((struct page_impl *)((char *)(ptr) - offsetof(struct page_impl, IUserControl_iface)))
#define impl_from_IControl(ptr) ((struct page_impl *)((char *)(ptr) - offsetof(struct page_impl, IControl_iface)))
#define impl_from_IPage_fe(ptr) ((struct page_impl *)((char *)(ptr) - offsetof(struct page_impl, IFrameworkElement_iface)))
#define impl_from_IPage_ui(ptr) ((struct page_impl *)((char *)(ptr) - offsetof(struct page_impl, IUIElement_iface)))
#define impl_from_IPage_do(ptr)  ((struct page_impl *)((char *)(ptr) - offsetof(struct page_impl, IDependencyObject_iface)))
#define impl_from_IPage_do2(ptr) ((struct page_impl *)((char *)(ptr) - offsetof(struct page_impl, IDependencyObject2_iface)))
#define impl_from_IRedhawk_page(ptr) ((struct page_impl *)((char *)(ptr) - offsetof(struct page_impl, IRedhawkInterop_iface)))

static HRESULT page_nondelegating_QI_internal(struct page_impl *impl, REFIID riid, void **ppvObject);
static ULONG page_nondelegating_Release(struct page_impl *impl);

static inline ULONG page_delegating_AddRef(struct page_impl *impl)
{
    if (impl->outer)
    {
        TRACE("page %p delegating AddRef to outer %p\n", impl, impl->outer);
        return IUnknown_AddRef((IUnknown *)impl->outer);
    }
    return InterlockedIncrement(&impl->ref);
}

static inline ULONG page_delegating_Release(struct page_impl *impl)
{
    if (impl->outer)
    {
        TRACE("page %p delegating Release to outer %p\n", impl, impl->outer);
        return IUnknown_Release((IUnknown *)impl->outer);
    }
    return page_nondelegating_Release(impl);
}

static inline HRESULT page_delegating_QI(struct page_impl *impl, REFIID riid, void **ppvObject)
{
    if (impl->outer)
    {
        TRACE("page %p delegating QI %s to outer %p\n", impl, debugstr_guid(riid), impl->outer);
        return IUnknown_QueryInterface((IUnknown *)impl->outer, riid, ppvObject);
    }
    return page_nondelegating_QI_internal(impl, riid, ppvObject);
}

static HRESULT page_nondelegating_QI_internal(struct page_impl *impl, REFIID riid, void **ppvObject)
{
    static const GUID IID_IComponentConnector_local  = {0xf6790987, 0xe6e5, 0x47f2, {0x92, 0xc6, 0xec, 0xcc, 0xe4, 0xba, 0x15, 0x9a}};
    static const GUID IID_IComponentConnector2_local = {0xdc8f368b, 0xeccc, 0x498e, {0xb1, 0x39, 0x91, 0x14, 0x22, 0x54, 0xd7, 0xae}};

    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("page %p nondelegating QI, riid %s\n", impl, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable))
    {
        *ppvObject = &impl->INonDelegatingInspectable_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IPage))
    {
        *ppvObject = &impl->IPage_iface;
    }
    else if (IsEqualGUID(riid, &IID_IUserControl) ||
             IsEqualGUID(riid, &IID_IUserControl_legacy) ||
             IsEqualGUID(riid, &IID_IContentControl) ||
             IsEqualGUID(riid, &IID_IContentControl_a26dd1dc))
    {
        *ppvObject = &impl->IUserControl_iface;
    }
    else if (IsEqualGUID(riid, &IID_IControl) ||
             IsEqualGUID(riid, &IID_IControl_b4a04776) ||
             IsEqualGUID(riid, &IID_IControl_legacy))
    {
        *ppvObject = &impl->IControl_iface;
    }
    else if (IsEqualGUID(riid, &IID_IFrameworkElement) ||
             IsEqualGUID(riid, &IID_IFrameworkElement2))
    {
        *ppvObject = &impl->IFrameworkElement_iface;
    }
    else if (IsEqualGUID(riid, &IID_IUIElement))
    {
        *ppvObject = &impl->IUIElement_iface;
    }
    else if (IsEqualGUID(riid, &IID_IDependencyObject))
    {
        *ppvObject = &impl->IDependencyObject_iface;
    }
    else if (IsEqualGUID(riid, &IID_IDependencyObject2))
    {
        *ppvObject = &impl->IDependencyObject2_iface;
    }
    else if (IsEqualGUID(riid, &IID_ComWrappers_internal))
    {
        *ppvObject = &impl->IRedhawkInterop_iface;
    }

    if (*ppvObject)
    {
        IUnknown_AddRef((IUnknown *)*ppvObject);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ComWrappers_internal2) ||
        IsEqualGUID(riid, &IID_IWeakReferenceSource))
    {
        return E_NOINTERFACE;
    }

    if (IsEqualGUID(riid, &IID_IComponentConnector_local) ||
        IsEqualGUID(riid, &IID_IComponentConnector2_local))
    {
        /* Avoid ping-pong recursion with outer CCW wrapper */
        return E_NOINTERFACE;
    }

    TRACE("page %p: unhandled QI %s\n", impl, debugstr_guid(riid));
    return E_NOINTERFACE;
}

static ULONG page_nondelegating_Release(struct page_impl *impl)
{
    ULONG ref = InterlockedDecrement(&impl->ref);
    TRACE("page %p, nondelegating ref %ld\n", impl, ref);
    if (!ref)
    {
        UINT32 i;
        if (impl->content) IInspectable_Release((IInspectable *)impl->content);
        for (i = 0; i < impl->dp_count; i++)
        {
            if (impl->dp_entries[i].value) IInspectable_Release(impl->dp_entries[i].value);
        }
        free(impl);
    }
    return ref;
}

static HRESULT WINAPI page_nondelegating_QueryInterface(IInspectable *iface, REFIID riid, void **ppvObject)
{
    struct page_impl *impl = impl_from_INonDelegatingInspectable(iface);
    return page_nondelegating_QI_internal(impl, riid, ppvObject);
}
static ULONG WINAPI page_nondelegating_AddRef_method(IInspectable *iface)
{
    struct page_impl *impl = impl_from_INonDelegatingInspectable(iface);
    ULONG ref = InterlockedIncrement(&impl->ref);
    TRACE("page %p nondelegating addref -> %ld\n", impl, ref);
    return ref;
}
static ULONG WINAPI page_nondelegating_Release_method(IInspectable *iface)
{
    struct page_impl *impl = impl_from_INonDelegatingInspectable(iface);
    return page_nondelegating_Release(impl);
}
static HRESULT WINAPI page_nondelegating_GetIids(IInspectable *iface, ULONG *c, IID **i)
{
    if (c) *c = 0;
    if (i) *i = NULL;
    return S_OK;
}
static HRESULT WINAPI page_nondelegating_GetRuntimeClassName(IInspectable *iface, HSTRING *n)
{
    return WindowsCreateString(RuntimeClass_Page, wcslen(RuntimeClass_Page), n);
}
static HRESULT WINAPI page_nondelegating_GetTrustLevel(IInspectable *iface, TrustLevel *t)
{
    if (t) *t = BaseTrust;
    return S_OK;
}

static const struct IInspectableVtbl page_nondelegating_vtbl = {
    page_nondelegating_QueryInterface,
    page_nondelegating_AddRef_method,
    page_nondelegating_Release_method,
    page_nondelegating_GetIids,
    page_nondelegating_GetRuntimeClassName,
    page_nondelegating_GetTrustLevel
};

static HRESULT WINAPI page_QueryInterface(void *This, REFIID riid, void **ppvObject) { return page_delegating_QI(impl_from_IPage(This), riid, ppvObject); }
static ULONG WINAPI page_AddRef(void *This) { return page_delegating_AddRef(impl_from_IPage(This)); }
static ULONG WINAPI page_Release(void *This) { return page_delegating_Release(impl_from_IPage(This)); }
static HRESULT WINAPI page_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI page_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(RuntimeClass_Page, wcslen(RuntimeClass_Page), n); }
static HRESULT WINAPI page_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI page_get_Frame(void *This, void **value)
{
    struct page_impl *impl = impl_from_IPage(This);
    TRACE("Page::get_Frame -> %p\n", impl->frame);
    if (!value) return E_POINTER;
    *value = impl->frame;
    if (impl->frame) IInspectable_AddRef(impl->frame);
    return S_OK;
}
static HRESULT WINAPI page_get_NavigationCacheMode(void *This, int *value) { if (value) *value = 0; return S_OK; }
static HRESULT WINAPI page_put_NavigationCacheMode(void *This, int value) { return S_OK; }
static HRESULT WINAPI page_get_TopAppBar(void *This, void **value) { if (value) *value = NULL; return S_OK; }
static HRESULT WINAPI page_put_TopAppBar(void *This, void *value) { return S_OK; }
static HRESULT WINAPI page_get_BottomAppBar(void *This, void **value) { if (value) *value = NULL; return S_OK; }
static HRESULT WINAPI page_put_BottomAppBar(void *This, void *value) { return S_OK; }

static const void *page_vtbl[] = {
    (void *)page_QueryInterface,           /* 0 */
    (void *)page_AddRef,                   /* 1 */
    (void *)page_Release,                  /* 2 */
    (void *)page_GetIids,                  /* 3 */
    (void *)page_GetRuntimeClassName,      /* 4 */
    (void *)page_GetTrustLevel,            /* 5 */
    (void *)page_get_Frame,                /* 6 */
    (void *)page_get_NavigationCacheMode,  /* 7 */
    (void *)page_put_NavigationCacheMode,  /* 8 */
    (void *)page_get_TopAppBar,            /* 9 */
    (void *)page_put_TopAppBar,            /* 10 */
    (void *)page_get_BottomAppBar,         /* 11 */
    (void *)page_put_BottomAppBar          /* 12 */
};

/* IUserControl */
static HRESULT WINAPI page_uc_QI(void *This, REFIID riid, void **ppvObject) { return page_delegating_QI(impl_from_IUserControl(This), riid, ppvObject); }
static ULONG WINAPI page_uc_AddRef(void *This) { return page_AddRef(&impl_from_IUserControl(This)->IPage_iface); }
static ULONG WINAPI page_uc_Release(void *This) { return page_Release(&impl_from_IUserControl(This)->IPage_iface); }
static HRESULT WINAPI page_uc_get_Content(void *This, void **value)
{
    struct page_impl *impl = impl_from_IUserControl(This);
    TRACE("Page UserControl::get_Content -> %p\n", impl->content);
    if (!value) return E_POINTER;
    *value = impl->content;
    if (impl->content) IInspectable_AddRef(impl->content);
    return S_OK;
}
static HRESULT WINAPI page_uc_put_Content(void *This, void *value)
{
    struct page_impl *impl = impl_from_IUserControl(This);
    TRACE("Page UserControl::put_Content %p\n", value);
    if (value) IInspectable_AddRef(value);
    if (impl->content) IInspectable_Release(impl->content);
    impl->content = value;
    return S_OK;
}

static const void *page_user_control_vtbl[16] = {
    (void *)page_uc_QI,
    (void *)page_uc_AddRef,
    (void *)page_uc_Release,
    (void *)page_GetIids,
    (void *)page_GetRuntimeClassName,
    (void *)page_GetTrustLevel,
    (void *)page_uc_get_Content,           /* 6 */
    (void *)page_uc_put_Content,           /* 7 */
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub
};

/* IControl */
static HRESULT WINAPI page_ctrl_QI(void *This, REFIID riid, void **ppvObject) { return page_delegating_QI(impl_from_IControl(This), riid, ppvObject); }
static ULONG WINAPI page_ctrl_AddRef(void *This) { return page_AddRef(&impl_from_IControl(This)->IPage_iface); }
static ULONG WINAPI page_ctrl_Release(void *This) { return page_Release(&impl_from_IControl(This)->IPage_iface); }

static const void *page_control_vtbl[32] = {
    (void *)page_ctrl_QI,
    (void *)page_ctrl_AddRef,
    (void *)page_ctrl_Release,
    (void *)page_GetIids,
    (void *)page_GetRuntimeClassName,
    (void *)page_GetTrustLevel,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub
};

/* IFrameworkElement */
static HRESULT WINAPI page_fe_QI(void *This, REFIID riid, void **ppvObject) { return page_delegating_QI(impl_from_IPage_fe(This), riid, ppvObject); }
static ULONG WINAPI page_fe_AddRef(void *This) { return page_AddRef(&impl_from_IPage_fe(This)->IPage_iface); }
static ULONG WINAPI page_fe_Release(void *This) { return page_Release(&impl_from_IPage_fe(This)->IPage_iface); }

static const void *page_fe_vtbl[32] = {
    (void *)page_fe_QI,
    (void *)page_fe_AddRef,
    (void *)page_fe_Release,
    (void *)page_GetIids,
    (void *)page_GetRuntimeClassName,
    (void *)page_GetTrustLevel,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub
};

/* IUIElement */
static HRESULT WINAPI page_ui_QI(void *This, REFIID riid, void **ppvObject) { return page_delegating_QI(impl_from_IPage_ui(This), riid, ppvObject); }
static ULONG WINAPI page_ui_AddRef(void *This) { return page_AddRef(&impl_from_IPage_ui(This)->IPage_iface); }
static ULONG WINAPI page_ui_Release(void *This) { return page_Release(&impl_from_IPage_ui(This)->IPage_iface); }

static const void *page_ui_element_vtbl[32] = {
    (void *)page_ui_QI,
    (void *)page_ui_AddRef,
    (void *)page_ui_Release,
    (void *)page_GetIids,
    (void *)page_GetRuntimeClassName,
    (void *)page_GetTrustLevel,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub
};

/* Redhawk Page wrapper */
static HRESULT WINAPI redhawk_page_QI(void *This, REFIID riid, void **ppvObject) { return page_delegating_QI(impl_from_IRedhawk_page(This), riid, ppvObject); }
static ULONG WINAPI redhawk_page_AddRef(void *This) { return page_AddRef(&impl_from_IRedhawk_page(This)->IPage_iface); }
static ULONG WINAPI page_Release_redhawk(void *This) { return page_Release(&impl_from_IRedhawk_page(This)->IPage_iface); }

static const void *redhawk_page_vtbl[32] = {
    (void *)redhawk_page_QI,
    (void *)redhawk_page_AddRef,
    (void *)page_Release_redhawk,
    (void *)redhawk_interop_m3,
    (void *)page_GetRuntimeClassName,
    (void *)page_GetTrustLevel,
    (void *)frame_generic_stub,
    (void *)redhawk_interop_m3,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub,
    (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub, (void *)frame_generic_stub
};

/* DependencyObject methods for Page */
static HRESULT WINAPI page_do_QI(void *This, REFIID riid, void **ppvObject) { return page_delegating_QI(impl_from_IPage_do(This), riid, ppvObject); }
static ULONG WINAPI page_do_AddRef(void *This) { return page_AddRef(&impl_from_IPage_do(This)->IPage_iface); }
static ULONG WINAPI page_do_Release(void *This) { return page_Release(&impl_from_IPage_do(This)->IPage_iface); }
static HRESULT WINAPI page_do_GetValue(void *This, void *dp, IInspectable **result)
{
    struct page_impl *impl = impl_from_IPage_do(This);
    UINT32 i;
    TRACE("Page DO::GetValue This %p, dp %p\n", This, dp);
    if (!result) return E_POINTER;
    *result = NULL;
    for (i = 0; i < impl->dp_count; i++)
    {
        if (impl->dp_entries[i].dp == dp)
        {
            *result = impl->dp_entries[i].value;
            if (*result) IInspectable_AddRef(*result);
            TRACE("Page DO::GetValue found stored value %p\n", *result);
            return S_OK;
        }
    }
    return dependency_property_get_default_value(dp, result);
}
static HRESULT WINAPI page_do_SetValue(void *This, void *dp, IInspectable *value)
{
    struct page_impl *impl = impl_from_IPage_do(This);
    UINT32 i;
    TRACE("Page DO::SetValue This %p, dp %p, value %p\n", This, dp, value);
    if (!dp) return E_POINTER;
    for (i = 0; i < impl->dp_count; i++)
    {
        if (impl->dp_entries[i].dp == dp)
        {
            if (value) IInspectable_AddRef(value);
            if (impl->dp_entries[i].value) IInspectable_Release(impl->dp_entries[i].value);
            impl->dp_entries[i].value = value;
            return S_OK;
        }
    }
    if (impl->dp_count < ARRAY_SIZE(impl->dp_entries))
    {
        impl->dp_entries[impl->dp_count].dp = dp;
        impl->dp_entries[impl->dp_count].value = value;
        if (value) IInspectable_AddRef(value);
        impl->dp_count++;
    }
    return S_OK;
}
static HRESULT WINAPI page_do_ClearValue(void *This, void *dp)
{
    TRACE("Page DO::ClearValue This %p, dp %p\n", This, dp);
    return S_OK;
}
static HRESULT WINAPI page_do_ReadLocalValue(void *This, void *dp, IInspectable **result)
{
    TRACE("Page DO::ReadLocalValue This %p, dp %p\n", This, dp);
    return page_do_GetValue(This, dp, result);
}
static HRESULT WINAPI page_do_GetAnimationBaseValue(void *This, void *dp, IInspectable **result)
{
    TRACE("Page DO::GetAnimationBaseValue This %p, dp %p\n", This, dp);
    return page_do_GetValue(This, dp, result);
}
static HRESULT WINAPI page_do_get_Dispatcher(void *This, void **value)
{
    TRACE("Page DO::get_Dispatcher This %p\n", This);
    if (!value) return E_POINTER;
    return get_global_dispatcher(value);
}

static const void *page_dependency_object_vtbl[16] = {
    (void *)page_do_QI,
    (void *)page_do_AddRef,
    (void *)page_do_Release,
    (void *)page_GetIids,
    (void *)page_GetRuntimeClassName,
    (void *)page_GetTrustLevel,
    (void *)page_do_GetValue,
    (void *)page_do_SetValue,
    (void *)page_do_ClearValue,
    (void *)page_do_ReadLocalValue,
    (void *)page_do_GetAnimationBaseValue,
    (void *)page_do_get_Dispatcher
};

/* IDependencyObject2 for Page */
static HRESULT WINAPI page_do2_QI(void *This, REFIID riid, void **ppvObject) { return page_delegating_QI(impl_from_IPage_do2(This), riid, ppvObject); }
static ULONG WINAPI page_do2_AddRef(void *This) { return page_AddRef(&impl_from_IPage_do2(This)->IPage_iface); }
static ULONG WINAPI page_do2_Release(void *This) { return page_Release(&impl_from_IPage_do2(This)->IPage_iface); }
static HRESULT WINAPI page_do2_RegisterPropertyChangedCallback(void *This, void *dp, void *callback, INT64 *result)
{
    TRACE("Page DO2::RegisterPropertyChangedCallback\n");
    if (!result) return E_POINTER;
    *result = 1;
    return S_OK;
}
static HRESULT WINAPI page_do2_UnregisterPropertyChangedCallback(void *This, void *dp, INT64 token)
{
    TRACE("Page DO2::UnregisterPropertyChangedCallback token %lld\n", token);
    return S_OK;
}

static const void *page_dependency_object2_vtbl[16] = {
    (void *)page_do2_QI,
    (void *)page_do2_AddRef,
    (void *)page_do2_Release,
    (void *)page_GetIids,
    (void *)page_GetRuntimeClassName,
    (void *)page_GetTrustLevel,
    (void *)page_do2_RegisterPropertyChangedCallback,
    (void *)page_do2_UnregisterPropertyChangedCallback
};

static HRESULT create_page_instance(void *outer, void **inner, void **instance)
{
    struct page_impl *impl;
    if (!instance) return E_POINTER;
    impl = calloc(1, sizeof(*impl));
    if (!impl) return E_OUTOFMEMORY;
    impl->INonDelegatingInspectable_iface.lpVtbl = &page_nondelegating_vtbl;
    impl->IPage_iface.lpVtbl = page_vtbl;
    impl->IUserControl_iface.lpVtbl = page_user_control_vtbl;
    impl->IControl_iface.lpVtbl = page_control_vtbl;
    impl->IFrameworkElement_iface.lpVtbl = page_fe_vtbl;
    impl->IUIElement_iface.lpVtbl = page_ui_element_vtbl;
    impl->IDependencyObject_iface.lpVtbl = page_dependency_object_vtbl;
    impl->IDependencyObject2_iface.lpVtbl = page_dependency_object2_vtbl;
    impl->IRedhawkInterop_iface.lpVtbl = redhawk_page_vtbl;
    impl->outer = outer;
    impl->ref = 1;

    if (outer)
    {
        if (inner)
            *inner = &impl->INonDelegatingInspectable_iface;
        *instance = &impl->IPage_iface;
        IUnknown_AddRef((IUnknown *)*instance);
    }
    else
    {
        if (inner)
        {
            *inner = &impl->INonDelegatingInspectable_iface;
            InterlockedIncrement(&impl->ref);
        }
        *instance = &impl->IPage_iface;
    }
    TRACE("Created Page instance %p, outer %p, inner %p, ref %ld\n", *instance, outer, inner ? *inner : NULL, impl->ref);
    return S_OK;
}

/* Page Factory */
struct page_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } IPageFactory_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    LONG ref;
};

#define impl_from_page_factory(ptr)   ((struct page_factory_impl *)((char *)(ptr) - offsetof(struct page_factory_impl, IActivationFactory_iface)))
#define impl_from_page_fac_iface(ptr) ((struct page_factory_impl *)((char *)(ptr) - offsetof(struct page_factory_impl, IPageFactory_iface)))
#define impl_from_page_redfac(ptr)    ((struct page_factory_impl *)((char *)(ptr) - offsetof(struct page_factory_impl, IRedhawkInterop_iface)))

static HRESULT page_fac_QI_internal(struct page_factory_impl *impl, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("page_fac %p, riid %s\n", impl, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppvObject = &impl->IActivationFactory_iface;
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IPageFactory))
    {
        *ppvObject = &impl->IPageFactory_iface;
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

    TRACE("page_fac %p: unhandled QI %s\n", impl, debugstr_guid(riid));
    return E_NOINTERFACE;
}

static HRESULT WINAPI page_fac_QueryInterface(IActivationFactory *iface, REFIID riid, void **ppvObject) { return page_fac_QI_internal(impl_from_page_factory(iface), riid, ppvObject); }
static ULONG WINAPI page_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI page_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI page_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI page_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n) { return WindowsCreateString(RuntimeClass_Page, wcslen(RuntimeClass_Page), n); }
static HRESULT WINAPI page_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI page_fac_CreateInstance(void *This, void *outer, void **inner, void **instance)
{
    return create_page_instance(outer, inner, instance);
}
static HRESULT WINAPI page_fac_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    return create_page_instance(NULL, NULL, (void **)instance);
}

static const IActivationFactoryVtbl page_activation_factory_vtbl = {
    page_fac_QueryInterface,
    page_fac_AddRef,
    page_fac_Release,
    page_fac_GetIids,
    page_fac_GetRuntimeClassName,
    page_fac_GetTrustLevel,
    page_fac_ActivateInstance
};

static HRESULT WINAPI page_pf_QI(void *This, REFIID riid, void **ppvObject) { return page_fac_QI_internal(impl_from_page_fac_iface(This), riid, ppvObject); }
static ULONG WINAPI page_pf_AddRef(void *This) { return 2; }
static ULONG WINAPI page_pf_Release(void *This) { return 1; }

static const void *page_factory_vtbl[] = {
    (void *)page_pf_QI,
    (void *)page_pf_AddRef,
    (void *)page_pf_Release,
    (void *)page_fac_GetIids,
    (void *)page_fac_GetRuntimeClassName,
    (void *)page_fac_GetTrustLevel,
    (void *)page_fac_CreateInstance,
    (void *)redhawk_interop_m3,
    (void *)redhawk_interop_m3,
    (void *)redhawk_interop_m3,
    (void *)redhawk_interop_m3
};

static HRESULT WINAPI redhawk_page_fac_QI(void *This, REFIID riid, void **ppvObject) { return page_fac_QI_internal(impl_from_page_redfac(This), riid, ppvObject); }
static ULONG WINAPI redhawk_page_fac_AddRef(void *This) { return 2; }
static ULONG WINAPI redhawk_page_fac_Release(void *This) { return 1; }

static const void *redhawk_page_factory_vtbl[] = {
    (void *)redhawk_page_fac_QI,
    (void *)redhawk_page_fac_AddRef,
    (void *)redhawk_page_fac_Release,
    (void *)redhawk_interop_m3,
    (void *)page_fac_GetRuntimeClassName,
    (void *)page_fac_GetTrustLevel,
    (void *)page_fac_CreateInstance,
    (void *)redhawk_interop_m3,
    (void *)redhawk_interop_m3,
    (void *)redhawk_interop_m3,
    (void *)redhawk_interop_m3
};

static struct page_factory_impl default_page_factory = {
    {&page_activation_factory_vtbl},
    {page_factory_vtbl},
    {redhawk_page_factory_vtbl},
    1
};

IActivationFactory *page_factory = &default_page_factory.IActivationFactory_iface;

/* -------------------------------------------------------------------------
 * CoreTextServicesManager Implementation
 * ------------------------------------------------------------------------- */
static const GUID IID_ICoreTextServicesManager = {0xc2507d83, 0x6e0a, 0x4a8a, {0xbd, 0xf8, 0x19, 0x48, 0x87, 0x48, 0x54, 0xba}};
static const GUID IID_ICoreTextServicesManagerStatics = {0x1520a388, 0xe2cf, 0x4d65, {0xae, 0xb9, 0xb3, 0x2d, 0x86, 0xfe, 0x39, 0xb9}};
static const GUID IID_ICoreTextEditContext = {0xbf500057, 0x010b, 0x4638, {0xb7, 0xeb, 0x07, 0x97, 0xab, 0x47, 0x5a, 0x89}};

struct text_edit_context_instance
{
    struct { const void *lpVtbl; } ICoreTextEditContext_iface;
    LONG ref;
};

static HRESULT WINAPI text_edit_ctx_QI(void *This, REFIID riid, void **ppvObject)
{
    struct text_edit_context_instance *impl = (struct text_edit_context_instance *)This;
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_ICoreTextEditContext))
    {
        *ppvObject = &impl->ICoreTextEditContext_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    TRACE("unhandled QI %s, returning self\n", debugstr_guid(riid));
    *ppvObject = &impl->ICoreTextEditContext_iface;
    InterlockedIncrement(&impl->ref);
    return S_OK;
}

static ULONG WINAPI text_edit_ctx_AddRef(void *This)
{
    struct text_edit_context_instance *impl = (struct text_edit_context_instance *)This;
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI text_edit_ctx_Release(void *This)
{
    struct text_edit_context_instance *impl = (struct text_edit_context_instance *)This;
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI text_edit_ctx_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }

static HRESULT WINAPI text_edit_ctx_GetRuntimeClassName(void *This, HSTRING *n)
{
    return WindowsCreateString(L"Windows.UI.Text.Core.CoreTextEditContext", 40, n);
}

static HRESULT WINAPI text_edit_ctx_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI text_edit_ctx_slot(void *This)
{
    TRACE("This %p stub!\n", This);
    return S_OK;
}

static void *text_edit_ctx_vtbl[50];

static void init_text_edit_ctx_vtbl(void)
{
    int i;
    if (text_edit_ctx_vtbl[0]) return;
    text_edit_ctx_vtbl[0] = text_edit_ctx_QI;
    text_edit_ctx_vtbl[1] = text_edit_ctx_AddRef;
    text_edit_ctx_vtbl[2] = text_edit_ctx_Release;
    text_edit_ctx_vtbl[3] = text_edit_ctx_GetIids;
    text_edit_ctx_vtbl[4] = text_edit_ctx_GetRuntimeClassName;
    text_edit_ctx_vtbl[5] = text_edit_ctx_GetTrustLevel;
    for (i = 6; i < 50; i++)
        text_edit_ctx_vtbl[i] = text_edit_ctx_slot;
}

struct text_manager_instance
{
    struct { const void *lpVtbl; } ICoreTextServicesManager_iface;
    LONG ref;
};

static HRESULT WINAPI text_mgr_QI(void *This, REFIID riid, void **ppvObject)
{
    struct text_manager_instance *impl = (struct text_manager_instance *)This;
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_ICoreTextServicesManager))
    {
        *ppvObject = &impl->ICoreTextServicesManager_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    return E_NOINTERFACE;
}

static ULONG WINAPI text_mgr_AddRef(void *This)
{
    struct text_manager_instance *impl = (struct text_manager_instance *)This;
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI text_mgr_Release(void *This)
{
    struct text_manager_instance *impl = (struct text_manager_instance *)This;
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI text_mgr_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }

static HRESULT WINAPI text_mgr_GetRuntimeClassName(void *This, HSTRING *n)
{
    return WindowsCreateString(L"Windows.UI.Text.Core.CoreTextServicesManager", 43, n);
}

static HRESULT WINAPI text_mgr_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI text_mgr_get_InputLanguage(void *This, void **value)
{
    if (value) *value = NULL;
    return S_OK;
}

static HRESULT WINAPI text_mgr_add_InputLanguageChanged(void *This, void *handler, EventRegistrationToken *token)
{
    if (token) token->value = 1;
    return S_OK;
}

static HRESULT WINAPI text_mgr_remove_InputLanguageChanged(void *This, EventRegistrationToken token)
{
    return S_OK;
}

static HRESULT WINAPI text_mgr_CreateEditContext(void *This, void **out)
{
    struct text_edit_context_instance *inst;
    TRACE("This %p, out %p\n", This, out);
    if (!out) return E_POINTER;
    init_text_edit_ctx_vtbl();
    if (!(inst = calloc(1, sizeof(*inst)))) return E_OUTOFMEMORY;
    inst->ICoreTextEditContext_iface.lpVtbl = text_edit_ctx_vtbl;
    inst->ref = 1;
    *out = &inst->ICoreTextEditContext_iface;
    return S_OK;
}

static const void *text_mgr_vtbl[] = {
    (void *)text_mgr_QI,
    (void *)text_mgr_AddRef,
    (void *)text_mgr_Release,
    (void *)text_mgr_GetIids,
    (void *)text_mgr_GetRuntimeClassName,
    (void *)text_mgr_GetTrustLevel,
    (void *)text_mgr_get_InputLanguage,
    (void *)text_mgr_add_InputLanguageChanged,
    (void *)text_mgr_remove_InputLanguageChanged,
    (void *)text_mgr_CreateEditContext
};

struct text_mgr_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } ICoreTextServicesManagerStatics_iface;
    LONG ref;
};

#define impl_from_text_mgr_fac(ptr) ((struct text_mgr_factory_impl *)((char *)(ptr) - offsetof(struct text_mgr_factory_impl, IActivationFactory_iface)))
#define impl_from_text_mgr_stat(ptr) ((struct text_mgr_factory_impl *)((char *)(ptr) - offsetof(struct text_mgr_factory_impl, ICoreTextServicesManagerStatics_iface)))

static HRESULT text_mgr_fac_QI_internal(struct text_mgr_factory_impl *impl, REFIID riid, void **ppvObject)
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
    if (IsEqualGUID(riid, &IID_ICoreTextServicesManagerStatics))
    {
        *ppvObject = &impl->ICoreTextServicesManagerStatics_iface;
        return S_OK;
    }
    return E_NOINTERFACE;
}

static HRESULT WINAPI text_mgr_fac_QueryInterface(IActivationFactory *iface, REFIID riid, void **ppvObject)
{
    return text_mgr_fac_QI_internal(impl_from_text_mgr_fac(iface), riid, ppvObject);
}
static ULONG WINAPI text_mgr_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI text_mgr_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI text_mgr_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI text_mgr_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n)
{
    return WindowsCreateString(L"Windows.UI.Text.Core.CoreTextServicesManager", 43, n);
}
static HRESULT WINAPI text_mgr_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI text_mgr_fac_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    struct text_manager_instance *inst = calloc(1, sizeof(*inst));
    if (!inst) return E_OUTOFMEMORY;
    inst->ICoreTextServicesManager_iface.lpVtbl = text_mgr_vtbl;
    inst->ref = 1;
    *instance = (IInspectable *)&inst->ICoreTextServicesManager_iface;
    return S_OK;
}

static const IActivationFactoryVtbl text_mgr_activation_factory_vtbl = {
    text_mgr_fac_QueryInterface,
    text_mgr_fac_AddRef,
    text_mgr_fac_Release,
    text_mgr_fac_GetIids,
    text_mgr_fac_GetRuntimeClassName,
    text_mgr_fac_GetTrustLevel,
    text_mgr_fac_ActivateInstance
};

static HRESULT WINAPI text_mgr_stat_QI(void *This, REFIID riid, void **ppvObject)
{
    return text_mgr_fac_QI_internal(impl_from_text_mgr_stat(This), riid, ppvObject);
}
static ULONG WINAPI text_mgr_stat_AddRef(void *This) { return 2; }
static ULONG WINAPI text_mgr_stat_Release(void *This) { return 1; }
static HRESULT WINAPI text_mgr_stat_GetForCurrentView(void *This, void **value)
{
    struct text_manager_instance *inst = calloc(1, sizeof(*inst));
    if (!inst) return E_OUTOFMEMORY;
    inst->ICoreTextServicesManager_iface.lpVtbl = text_mgr_vtbl;
    inst->ref = 1;
    *value = &inst->ICoreTextServicesManager_iface;
    TRACE("CoreTextServicesManager GetForCurrentView -> %p\n", *value);
    return S_OK;
}

static const void *text_mgr_statics_vtbl[] = {
    (void *)text_mgr_stat_QI,
    (void *)text_mgr_stat_AddRef,
    (void *)text_mgr_stat_Release,
    (void *)text_mgr_fac_GetIids,
    (void *)text_mgr_fac_GetRuntimeClassName,
    (void *)text_mgr_fac_GetTrustLevel,
    (void *)text_mgr_stat_GetForCurrentView
};

static struct text_mgr_factory_impl default_text_mgr_factory = {
    {&text_mgr_activation_factory_vtbl},
    {text_mgr_statics_vtbl},
    1
};

IActivationFactory *core_text_services_manager_factory = &default_text_mgr_factory.IActivationFactory_iface;
