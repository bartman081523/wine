/*
 * WinRT Windows.System.MemoryManager Implementation
 *
 * Copyright (C) 2026 Julian / Xodus Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

static const GUID IID_IMemoryManagerStatics = {0x5c6c279c, 0xd7ca, 0x4779, {0x91, 0x88, 0x40, 0x57, 0x21, 0x9c, 0xe6, 0x4c}};
static const GUID IID_IMemoryManagerStatics2 = {0x6eee351f, 0x6d62, 0x423f, {0x94, 0x79, 0xb0, 0x1f, 0x9c, 0x9f, 0x76, 0x69}};
static const GUID IID_IMemoryManagerStatics3 = {0x149b59ce, 0x92ad, 0x4e35, {0x89, 0xeb, 0x50, 0xdf, 0xb4, 0xc0, 0xd9, 0x1c}};
static const GUID IID_IMemoryManagerStatics4 = {0xc2c96c4b, 0xecb4, 0x42f2, {0xbb, 0x9d, 0xa9, 0x84, 0x7b, 0x00, 0x06, 0x21}};
static const GUID IID_IMemoryManagerStaticsOld2 = {0x6db5661a, 0x367e, 0x47e1, {0x87, 0x7e, 0xa1, 0x8a, 0x22, 0x5a, 0x5a, 0x4d}};
static const GUID IID_IMemoryManagerStaticsOld3 = {0x14589d8d, 0x9866, 0x4290, {0x8a, 0x53, 0x24, 0x9e, 0x7e, 0xa2, 0x05, 0x80}};

#define RuntimeClass_MemoryManager L"Windows.System.MemoryManager"

struct memory_manager_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } IMemoryManagerStatics_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    LONG ref;
};

#define impl_from_mm_factory(ptr) ((struct memory_manager_factory_impl *)((char *)(ptr) - offsetof(struct memory_manager_factory_impl, IActivationFactory_iface)))
#define impl_from_mm_statics(ptr) ((struct memory_manager_factory_impl *)((char *)(ptr) - offsetof(struct memory_manager_factory_impl, IMemoryManagerStatics_iface)))
#define impl_from_mm_redhawk(ptr) ((struct memory_manager_factory_impl *)((char *)(ptr) - offsetof(struct memory_manager_factory_impl, IRedhawkInterop_iface)))

static HRESULT mm_fac_QI_internal(struct memory_manager_factory_impl *impl, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("mm_fac %p, riid %s\n", impl, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppvObject = &impl->IActivationFactory_iface;
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IMemoryManagerStatics) ||
        IsEqualGUID(riid, &IID_IMemoryManagerStatics2) ||
        IsEqualGUID(riid, &IID_IMemoryManagerStatics3) ||
        IsEqualGUID(riid, &IID_IMemoryManagerStatics4) ||
        IsEqualGUID(riid, &IID_IMemoryManagerStaticsOld2) ||
        IsEqualGUID(riid, &IID_IMemoryManagerStaticsOld3))
    {
        *ppvObject = &impl->IMemoryManagerStatics_iface;
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

    TRACE("mm_fac %p: unhandled QI %s\n", impl, debugstr_guid(riid));
    return E_NOINTERFACE;
}

static HRESULT WINAPI mm_fac_QueryInterface(IActivationFactory *iface, REFIID riid, void **ppvObject)
{
    return mm_fac_QI_internal(impl_from_mm_factory(iface), riid, ppvObject);
}
static ULONG WINAPI mm_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI mm_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI mm_fac_GetIids(IActivationFactory *iface, ULONG *iidCount, IID **iids) { if (iidCount) *iidCount = 0; return S_OK; }
static HRESULT WINAPI mm_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *className) { return WindowsCreateString(RuntimeClass_MemoryManager, wcslen(RuntimeClass_MemoryManager), className); }
static HRESULT WINAPI mm_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *trustLevel) { if (trustLevel) *trustLevel = BaseTrust; return S_OK; }
static HRESULT WINAPI mm_fac_ActivateInstance(IActivationFactory *iface, IInspectable **instance) { if (instance) *instance = NULL; return E_NOTIMPL; }

static const IActivationFactoryVtbl memory_manager_activation_factory_vtbl = {
    mm_fac_QueryInterface,
    mm_fac_AddRef,
    mm_fac_Release,
    mm_fac_GetIids,
    mm_fac_GetRuntimeClassName,
    mm_fac_GetTrustLevel,
    mm_fac_ActivateInstance
};

static HRESULT WINAPI mm_s_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return mm_fac_QI_internal(impl_from_mm_statics(This), riid, ppvObject);
}
static ULONG WINAPI mm_s_AddRef(void *This) { return 2; }
static ULONG WINAPI mm_s_Release(void *This) { return 1; }
static HRESULT WINAPI mm_s_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI mm_s_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(RuntimeClass_MemoryManager, wcslen(RuntimeClass_MemoryManager), n); }
static HRESULT WINAPI mm_s_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI mm_s_get_AppMemoryUsage(void *This, UINT64 *value)
{
    TRACE("get_AppMemoryUsage %p\n", value);
    if (value) *value = 100 * 1024 * 1024; /* 100 MB */
    return S_OK;
}
static HRESULT WINAPI mm_s_get_AppMemoryUsageLimit(void *This, UINT64 *value)
{
    TRACE("get_AppMemoryUsageLimit %p\n", value);
    if (value) *value = 1024ULL * 1024 * 1024 * 4; /* 4 GB */
    return S_OK;
}
static HRESULT WINAPI mm_s_get_AppMemoryUsageLevel(void *This, int *value)
{
    TRACE("get_AppMemoryUsageLevel %p\n", value);
    if (value) *value = 0; /* Low */
    return S_OK;
}
static HRESULT WINAPI mm_s_add_event(void *This, void *h, EventRegistrationToken *t)
{
    if (t) t->value = 1;
    return S_OK;
}
static HRESULT WINAPI mm_s_remove_event(void *This, EventRegistrationToken t)
{
    return S_OK;
}

static const void *memory_manager_statics_vtbl[] = {
    (void *)mm_s_QueryInterface,
    (void *)mm_s_AddRef,
    (void *)mm_s_Release,
    (void *)mm_s_GetIids,
    (void *)mm_s_GetRuntimeClassName,
    (void *)mm_s_GetTrustLevel,
    (void *)mm_s_get_AppMemoryUsage,         /* Index 6 */
    (void *)mm_s_get_AppMemoryUsageLimit,    /* Index 7 */
    (void *)mm_s_get_AppMemoryUsageLevel,    /* Index 8 */
    (void *)mm_s_add_event,                  /* Index 9 */
    (void *)mm_s_remove_event,               /* Index 10 */
    (void *)mm_s_add_event,                  /* Index 11 */
    (void *)mm_s_remove_event,               /* Index 12 */
    (void *)mm_s_add_event,                  /* Index 13 */
    (void *)mm_s_remove_event,               /* Index 14 */
    (void *)mm_s_get_AppMemoryUsage          /* Index 15 */
};

static HRESULT WINAPI redhawk_mm_QI(void *This, REFIID riid, void **ppvObject)
{
    return mm_fac_QI_internal(impl_from_mm_redhawk(This), riid, ppvObject);
}
static ULONG WINAPI redhawk_mm_AddRef(void *This) { return 2; }
static ULONG WINAPI redhawk_mm_Release(void *This) { return 1; }

static const void *redhawk_mm_vtbl[] = {
    (void *)redhawk_mm_QI,
    (void *)redhawk_mm_AddRef,
    (void *)redhawk_mm_Release,
    (void *)redhawk_interop_m3,
    (void *)mm_s_GetRuntimeClassName,
    (void *)mm_s_GetTrustLevel,
    (void *)mm_s_get_AppMemoryUsage,
    (void *)mm_s_get_AppMemoryUsageLimit,
    (void *)mm_s_get_AppMemoryUsageLevel,
    (void *)mm_s_add_event,
    (void *)mm_s_remove_event,
    (void *)mm_s_add_event,
    (void *)mm_s_remove_event,
    (void *)mm_s_add_event,
    (void *)mm_s_remove_event,
    (void *)mm_s_get_AppMemoryUsage
};

static struct memory_manager_factory_impl default_memory_manager_factory = {
    {&memory_manager_activation_factory_vtbl},
    {memory_manager_statics_vtbl},
    {redhawk_mm_vtbl},
    1
};

IActivationFactory *memory_manager_factory = &default_memory_manager_factory.IActivationFactory_iface;
