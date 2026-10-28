/*
 * WinRT Windows.ApplicationModel.UserActivities.UserActivityRequestManager Implementation
 *
 * Copyright (C) 2026 Julian / Xodus Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

static const GUID IID_IUserActivityRequestManagerStatics = {0xc0392df1, 0x224a, 0x432c, {0x81, 0xe5, 0x0c, 0x76, 0xb4, 0xc4, 0xce, 0xfa}};
static const GUID IID_IUserActivityRequestManager        = {0x0c30be4e, 0x903d, 0x48d6, {0x82, 0xd4, 0x40, 0x43, 0xed, 0x57, 0x79, 0x1b}};

#define RuntimeClass_UserActivityRequestManager L"Windows.ApplicationModel.UserActivities.UserActivityRequestManager"

struct user_activity_manager_impl
{
    struct { const void *lpVtbl; } IUserActivityRequestManager_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    LONG ref;
};

#define impl_from_uam(ptr) ((struct user_activity_manager_impl *)((char *)(ptr) - offsetof(struct user_activity_manager_impl, IUserActivityRequestManager_iface)))

static HRESULT uam_QI_internal(struct user_activity_manager_impl *impl, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("uam %p, riid %s\n", impl, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IUserActivityRequestManager))
    {
        *ppvObject = &impl->IUserActivityRequestManager_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ComWrappers_internal) ||
        IsEqualGUID(riid, &IID_ComWrappers_internal2) ||
        IsEqualGUID(riid, &IID_IWeakReferenceSource))
    {
        return E_NOINTERFACE;
    }

    TRACE("uam %p: unhandled QI %s\n", impl, debugstr_guid(riid));
    return E_NOINTERFACE;
}

static HRESULT WINAPI uam_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return uam_QI_internal(impl_from_uam(This), riid, ppvObject);
}
static ULONG WINAPI uam_AddRef(void *This)
{
    return InterlockedIncrement(&impl_from_uam(This)->ref);
}
static ULONG WINAPI uam_Release(void *This)
{
    struct user_activity_manager_impl *impl = impl_from_uam(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}
static HRESULT WINAPI uam_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI uam_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(RuntimeClass_UserActivityRequestManager, wcslen(RuntimeClass_UserActivityRequestManager), n); }
static HRESULT WINAPI uam_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI uam_add_UserActivityRequested(void *This, void *handler, EventRegistrationToken *token)
{
    TRACE("uam_add_UserActivityRequested handler %p\n", handler);
    if (token) token->value = 1;
    return S_OK;
}
static HRESULT WINAPI uam_remove_UserActivityRequested(void *This, EventRegistrationToken token)
{
    TRACE("uam_remove_UserActivityRequested token %lld\n", token.value);
    return S_OK;
}

static const void *uam_vtbl[16] = {
    (void *)uam_QueryInterface,
    (void *)uam_AddRef,
    (void *)uam_Release,
    (void *)uam_GetIids,
    (void *)uam_GetRuntimeClassName,
    (void *)uam_GetTrustLevel,
    (void *)uam_add_UserActivityRequested,
    (void *)uam_remove_UserActivityRequested
};

static HRESULT create_uam_instance(void **instance)
{
    struct user_activity_manager_impl *impl;
    if (!instance) return E_POINTER;
    impl = calloc(1, sizeof(*impl));
    if (!impl) return E_OUTOFMEMORY;
    impl->IUserActivityRequestManager_iface.lpVtbl = uam_vtbl;
    impl->ref = 1;
    *instance = &impl->IUserActivityRequestManager_iface;
    return S_OK;
}

/* UserActivityRequestManager Factory */
struct uam_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } IUserActivityRequestManagerStatics_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    LONG ref;
};

#define impl_from_uam_factory(ptr) ((struct uam_factory_impl *)((char *)(ptr) - offsetof(struct uam_factory_impl, IActivationFactory_iface)))
#define impl_from_uam_statics(ptr) ((struct uam_factory_impl *)((char *)(ptr) - offsetof(struct uam_factory_impl, IUserActivityRequestManagerStatics_iface)))
#define impl_from_uam_redhawk(ptr) ((struct uam_factory_impl *)((char *)(ptr) - offsetof(struct uam_factory_impl, IRedhawkInterop_iface)))

static HRESULT uam_fac_QI_internal(struct uam_factory_impl *impl, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("uam_fac %p, riid %s\n", impl, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppvObject = &impl->IActivationFactory_iface;
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IUserActivityRequestManagerStatics))
    {
        *ppvObject = &impl->IUserActivityRequestManagerStatics_iface;
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ComWrappers_internal) ||
        IsEqualGUID(riid, &IID_ComWrappers_internal2) ||
        IsEqualGUID(riid, &IID_IWeakReferenceSource))
    {
        return E_NOINTERFACE;
    }

    TRACE("uam_fac %p: unhandled QI %s\n", impl, debugstr_guid(riid));
    return E_NOINTERFACE;
}

static HRESULT WINAPI uam_fac_QueryInterface(IActivationFactory *iface, REFIID riid, void **ppvObject) { return uam_fac_QI_internal(impl_from_uam_factory(iface), riid, ppvObject); }
static ULONG WINAPI uam_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI uam_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI uam_fac_GetIids(IActivationFactory *iface, ULONG *iidCount, IID **iids) { if (iidCount) *iidCount = 0; return S_OK; }
static HRESULT WINAPI uam_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *className) { return WindowsCreateString(RuntimeClass_UserActivityRequestManager, wcslen(RuntimeClass_UserActivityRequestManager), className); }
static HRESULT WINAPI uam_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *trustLevel) { if (trustLevel) *trustLevel = BaseTrust; return S_OK; }
static HRESULT WINAPI uam_fac_ActivateInstance(IActivationFactory *iface, IInspectable **instance) { return create_uam_instance((void **)instance); }

static const IActivationFactoryVtbl uam_activation_factory_vtbl = {
    uam_fac_QueryInterface,
    uam_fac_AddRef,
    uam_fac_Release,
    uam_fac_GetIids,
    uam_fac_GetRuntimeClassName,
    uam_fac_GetTrustLevel,
    uam_fac_ActivateInstance
};

static HRESULT WINAPI uam_s_QueryInterface(void *This, REFIID riid, void **ppvObject) { return uam_fac_QI_internal(impl_from_uam_statics(This), riid, ppvObject); }
static ULONG WINAPI uam_s_AddRef(void *This) { return 2; }
static ULONG WINAPI uam_s_Release(void *This) { return 1; }
static HRESULT WINAPI uam_s_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI uam_s_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(RuntimeClass_UserActivityRequestManager, wcslen(RuntimeClass_UserActivityRequestManager), n); }
static HRESULT WINAPI uam_s_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI uam_s_GetForCurrentView(void *This, void **result)
{
    TRACE("uam_s_GetForCurrentView\n");
    return create_uam_instance(result);
}

static const void *uam_statics_vtbl[16] = {
    (void *)uam_s_QueryInterface,
    (void *)uam_s_AddRef,
    (void *)uam_s_Release,
    (void *)uam_s_GetIids,
    (void *)uam_s_GetRuntimeClassName,
    (void *)uam_s_GetTrustLevel,
    (void *)uam_s_GetForCurrentView
};

static struct uam_factory_impl default_uam_factory = {
    {&uam_activation_factory_vtbl},
    {uam_statics_vtbl},
    {uam_statics_vtbl},
    1
};

IActivationFactory *user_activity_request_manager_factory = &default_uam_factory.IActivationFactory_iface;
