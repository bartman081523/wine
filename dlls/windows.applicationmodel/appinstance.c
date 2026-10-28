/*
 * WinRT Windows.ApplicationModel.AppInstance Implementation
 *
 * Copyright (C) 2026 Julian / Xodus Team
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
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(model);

/* IAppInstanceStatics: GUID {9d11e77f-9ea6-47af-a6ec-46784c5ba254} */
static const GUID IID_IAppInstanceStatics = {0x9d11e77f, 0x9ea6, 0x47af, {0xa6, 0xec, 0x46, 0x78, 0x4c, 0x5b, 0xa2, 0x54}};

/* IAppInstance: GUID {4189fbf4-465e-45bf-907f-165b3fb23758} */
static const GUID IID_IAppInstance = {0x4189fbf4, 0x465e, 0x45bf, {0x90, 0x7f, 0x16, 0x5b, 0x3f, 0xb2, 0x37, 0x58}};

#define RuntimeClass_Windows_ApplicationModel_AppInstance L"Windows.ApplicationModel.AppInstance"

typedef struct IAppInstance IAppInstance;
typedef struct IAppInstanceStatics IAppInstanceStatics;

typedef struct IAppInstanceVtbl
{
    /* IUnknown */
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IAppInstance *This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IAppInstance *This);
    ULONG (STDMETHODCALLTYPE *Release)(IAppInstance *This);
    /* IInspectable */
    HRESULT (STDMETHODCALLTYPE *GetIids)(IAppInstance *This, ULONG *iidCount, IID **iids);
    HRESULT (STDMETHODCALLTYPE *GetRuntimeClassName)(IAppInstance *This, HSTRING *className);
    HRESULT (STDMETHODCALLTYPE *GetTrustLevel)(IAppInstance *This, TrustLevel *trustLevel);
    /* IAppInstance */
    HRESULT (STDMETHODCALLTYPE *Unregister)(IAppInstance *This);
    HRESULT (STDMETHODCALLTYPE *RedirectActivationTo)(IAppInstance *This);
    HRESULT (STDMETHODCALLTYPE *get_IsCurrentInstance)(IAppInstance *This, boolean *value);
    HRESULT (STDMETHODCALLTYPE *get_Key)(IAppInstance *This, HSTRING *value);
} IAppInstanceVtbl;

struct IAppInstance
{
    const IAppInstanceVtbl *lpVtbl;
};

typedef struct IAppInstanceStaticsVtbl
{
    /* IUnknown */
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IAppInstanceStatics *This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IAppInstanceStatics *This);
    ULONG (STDMETHODCALLTYPE *Release)(IAppInstanceStatics *This);
    /* IInspectable */
    HRESULT (STDMETHODCALLTYPE *GetIids)(IAppInstanceStatics *This, ULONG *iidCount, IID **iids);
    HRESULT (STDMETHODCALLTYPE *GetRuntimeClassName)(IAppInstanceStatics *This, HSTRING *className);
    HRESULT (STDMETHODCALLTYPE *GetTrustLevel)(IAppInstanceStatics *This, TrustLevel *trustLevel);
    /* IAppInstanceStatics */
    HRESULT (STDMETHODCALLTYPE *GetActivatedEventArgs)(IAppInstanceStatics *This, IInspectable **value);
    HRESULT (STDMETHODCALLTYPE *FindOrRegisterInstanceForKey)(IAppInstanceStatics *This, HSTRING key, IAppInstance **value);
    HRESULT (STDMETHODCALLTYPE *Unregister)(IAppInstanceStatics *This);
    HRESULT (STDMETHODCALLTYPE *GetInstances)(IAppInstanceStatics *This, IInspectable **value);
    HRESULT (STDMETHODCALLTYPE *get_RecommendedInstance)(IAppInstanceStatics *This, IAppInstance **value);
} IAppInstanceStaticsVtbl;

struct IAppInstanceStatics
{
    const IAppInstanceStaticsVtbl *lpVtbl;
};

struct app_instance
{
    IAppInstance IAppInstance_iface;
    LONG ref;
    HSTRING key;
    boolean is_current;
};

static inline struct app_instance *impl_from_IAppInstance(IAppInstance *iface)
{
    return CONTAINING_RECORD(iface, struct app_instance, IAppInstance_iface);
}

static HRESULT WINAPI app_instance_QueryInterface(IAppInstance *iface, REFIID iid, void **out)
{
    struct app_instance *impl = impl_from_IAppInstance(iface);

    TRACE("iface %p, iid %s, out %p.\n", iface, debugstr_guid(iid), out);

    if (IsEqualGUID(iid, &IID_IUnknown) ||
        IsEqualGUID(iid, &IID_IInspectable) ||
        IsEqualGUID(iid, &IID_IAgileObject) ||
        IsEqualGUID(iid, &IID_IAppInstance))
    {
        IInspectable_AddRef((*out = &impl->IAppInstance_iface));
        return S_OK;
    }

    FIXME("%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid(iid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI app_instance_AddRef(IAppInstance *iface)
{
    struct app_instance *impl = impl_from_IAppInstance(iface);
    ULONG ref = InterlockedIncrement(&impl->ref);
    TRACE("iface %p, ref %lu.\n", iface, ref);
    return ref;
}

static ULONG WINAPI app_instance_Release(IAppInstance *iface)
{
    struct app_instance *impl = impl_from_IAppInstance(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    TRACE("iface %p, ref %lu.\n", iface, ref);
    if (!ref)
    {
        if (impl->key) WindowsDeleteString(impl->key);
        free(impl);
    }
    return ref;
}

static HRESULT WINAPI app_instance_GetIids(IAppInstance *iface, ULONG *iid_count, IID **iids)
{
    FIXME("iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids);
    return E_NOTIMPL;
}

static HRESULT WINAPI app_instance_GetRuntimeClassName(IAppInstance *iface, HSTRING *class_name)
{
    return WindowsCreateString(RuntimeClass_Windows_ApplicationModel_AppInstance,
                               wcslen(RuntimeClass_Windows_ApplicationModel_AppInstance),
                               class_name);
}

static HRESULT WINAPI app_instance_GetTrustLevel(IAppInstance *iface, TrustLevel *trust_level)
{
    FIXME("iface %p, trust_level %p stub!\n", iface, trust_level);
    return E_NOTIMPL;
}

static HRESULT WINAPI app_instance_Unregister(IAppInstance *iface)
{
    TRACE("iface %p.\n", iface);
    return S_OK;
}

static HRESULT WINAPI app_instance_RedirectActivationTo(IAppInstance *iface)
{
    TRACE("iface %p.\n", iface);
    return S_OK;
}

static HRESULT WINAPI app_instance_get_IsCurrentInstance(IAppInstance *iface, boolean *value)
{
    struct app_instance *impl = impl_from_IAppInstance(iface);
    TRACE("iface %p, value %p.\n", iface, value);
    if (!value) return E_INVALIDARG;
    *value = impl->is_current;
    return S_OK;
}

static HRESULT WINAPI app_instance_get_Key(IAppInstance *iface, HSTRING *value)
{
    struct app_instance *impl = impl_from_IAppInstance(iface);
    TRACE("iface %p, value %p.\n", iface, value);
    if (!value) return E_INVALIDARG;
    if (impl->key)
        return WindowsDuplicateString(impl->key, value);
    return WindowsCreateString(L"", 0, value);
}

static const struct IAppInstanceVtbl app_instance_vtbl =
{
    app_instance_QueryInterface,
    app_instance_AddRef,
    app_instance_Release,
    app_instance_GetIids,
    app_instance_GetRuntimeClassName,
    app_instance_GetTrustLevel,
    app_instance_Unregister,
    app_instance_RedirectActivationTo,
    app_instance_get_IsCurrentInstance,
    app_instance_get_Key,
};

static HRESULT app_instance_create(HSTRING key, boolean is_current, IAppInstance **out)
{
    struct app_instance *impl;

    *out = NULL;
    if (!(impl = calloc(1, sizeof(*impl)))) return E_OUTOFMEMORY;
    impl->IAppInstance_iface.lpVtbl = &app_instance_vtbl;
    impl->ref = 1;
    impl->is_current = is_current;
    if (key) WindowsDuplicateString(key, &impl->key);

    *out = &impl->IAppInstance_iface;
    TRACE("created AppInstance %p (is_current=%d)\n", *out, is_current);
    return S_OK;
}

struct app_instance_factory
{
    IActivationFactory IActivationFactory_iface;
    IAppInstanceStatics IAppInstanceStatics_iface;
    LONG ref;
};

static inline struct app_instance_factory *impl_from_IActivationFactory(IActivationFactory *iface)
{
    return CONTAINING_RECORD(iface, struct app_instance_factory, IActivationFactory_iface);
}

static inline struct app_instance_factory *impl_from_IAppInstanceStatics(IAppInstanceStatics *iface)
{
    return CONTAINING_RECORD(iface, struct app_instance_factory, IAppInstanceStatics_iface);
}

static HRESULT WINAPI app_instance_factory_QueryInterface(IActivationFactory *iface, REFIID iid, void **out)
{
    struct app_instance_factory *impl = impl_from_IActivationFactory(iface);

    TRACE("iface %p, iid %s, out %p.\n", iface, debugstr_guid(iid), out);

    if (IsEqualGUID(iid, &IID_IUnknown) ||
        IsEqualGUID(iid, &IID_IInspectable) ||
        IsEqualGUID(iid, &IID_IAgileObject) ||
        IsEqualGUID(iid, &IID_IActivationFactory))
    {
        IUnknown_AddRef(iface);
        *out = &impl->IActivationFactory_iface;
        return S_OK;
    }

    if (IsEqualGUID(iid, &IID_IAppInstanceStatics))
    {
        IUnknown_AddRef(iface);
        *out = &impl->IAppInstanceStatics_iface;
        return S_OK;
    }

    FIXME("%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid(iid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI app_instance_factory_AddRef(IActivationFactory *iface)
{
    struct app_instance_factory *impl = impl_from_IActivationFactory(iface);
    ULONG ref = InterlockedIncrement(&impl->ref);
    TRACE("iface %p, ref %lu.\n", iface, ref);
    return ref;
}

static ULONG WINAPI app_instance_factory_Release(IActivationFactory *iface)
{
    struct app_instance_factory *impl = impl_from_IActivationFactory(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    TRACE("iface %p, ref %lu.\n", iface, ref);
    return ref;
}

static HRESULT WINAPI app_instance_factory_GetIids(IActivationFactory *iface, ULONG *iid_count, IID **iids)
{
    FIXME("iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids);
    return E_NOTIMPL;
}

static HRESULT WINAPI app_instance_factory_GetRuntimeClassName(IActivationFactory *iface, HSTRING *class_name)
{
    return WindowsCreateString(RuntimeClass_Windows_ApplicationModel_AppInstance,
                               wcslen(RuntimeClass_Windows_ApplicationModel_AppInstance),
                               class_name);
}

static HRESULT WINAPI app_instance_factory_GetTrustLevel(IActivationFactory *iface, TrustLevel *trust_level)
{
    FIXME("iface %p, trust_level %p stub!\n", iface, trust_level);
    return E_NOTIMPL;
}

static HRESULT WINAPI app_instance_factory_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    IAppInstance *app_inst;
    HRESULT hr;

    TRACE("iface %p, instance %p.\n", iface, instance);

    hr = app_instance_create(NULL, TRUE, &app_inst);
    if (SUCCEEDED(hr))
        *instance = (IInspectable *)app_inst;
    return hr;
}

static const struct IActivationFactoryVtbl app_instance_factory_vtbl =
{
    app_instance_factory_QueryInterface,
    app_instance_factory_AddRef,
    app_instance_factory_Release,
    app_instance_factory_GetIids,
    app_instance_factory_GetRuntimeClassName,
    app_instance_factory_GetTrustLevel,
    app_instance_factory_ActivateInstance,
};

static HRESULT WINAPI app_instance_statics_QueryInterface(IAppInstanceStatics *iface, REFIID iid, void **out)
{
    struct app_instance_factory *impl = impl_from_IAppInstanceStatics(iface);
    return IActivationFactory_QueryInterface(&impl->IActivationFactory_iface, iid, out);
}

static ULONG WINAPI app_instance_statics_AddRef(IAppInstanceStatics *iface)
{
    struct app_instance_factory *impl = impl_from_IAppInstanceStatics(iface);
    return IActivationFactory_AddRef(&impl->IActivationFactory_iface);
}

static ULONG WINAPI app_instance_statics_Release(IAppInstanceStatics *iface)
{
    struct app_instance_factory *impl = impl_from_IAppInstanceStatics(iface);
    return IActivationFactory_Release(&impl->IActivationFactory_iface);
}

static HRESULT WINAPI app_instance_statics_GetIids(IAppInstanceStatics *iface, ULONG *iid_count, IID **iids)
{
    FIXME("iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids);
    return E_NOTIMPL;
}

static HRESULT WINAPI app_instance_statics_GetRuntimeClassName(IAppInstanceStatics *iface, HSTRING *class_name)
{
    return WindowsCreateString(RuntimeClass_Windows_ApplicationModel_AppInstance,
                               wcslen(RuntimeClass_Windows_ApplicationModel_AppInstance),
                               class_name);
}

static HRESULT WINAPI app_instance_statics_GetTrustLevel(IAppInstanceStatics *iface, TrustLevel *trust_level)
{
    FIXME("iface %p, trust_level %p stub!\n", iface, trust_level);
    return E_NOTIMPL;
}

static HRESULT WINAPI app_instance_statics_GetActivatedEventArgs(IAppInstanceStatics *iface, IInspectable **value)
{
    TRACE("iface %p, value %p.\n", iface, value);
    if (!value) return E_INVALIDARG;
    *value = NULL;
    return S_OK;
}

static HRESULT WINAPI app_instance_statics_FindOrRegisterInstanceForKey(IAppInstanceStatics *iface, HSTRING key, IAppInstance **value)
{
    TRACE("iface %p, key %s, value %p.\n", iface, debugstr_hstring(key), value);
    if (!value) return E_INVALIDARG;
    return app_instance_create(key, TRUE, value);
}

static HRESULT WINAPI app_instance_statics_Unregister(IAppInstanceStatics *iface)
{
    TRACE("iface %p.\n", iface);
    return S_OK;
}

static HRESULT WINAPI app_instance_statics_GetInstances(IAppInstanceStatics *iface, IInspectable **value)
{
    FIXME("iface %p, value %p stub!\n", iface, value);
    if (!value) return E_INVALIDARG;
    *value = NULL;
    return S_OK;
}

static HRESULT WINAPI app_instance_statics_get_RecommendedInstance(IAppInstanceStatics *iface, IAppInstance **value)
{
    TRACE("iface %p, value %p.\n", iface, value);
    if (!value) return E_INVALIDARG;
    *value = NULL;
    return S_OK;
}

static const struct IAppInstanceStaticsVtbl app_instance_statics_vtbl =
{
    app_instance_statics_QueryInterface,
    app_instance_statics_AddRef,
    app_instance_statics_Release,
    app_instance_statics_GetIids,
    app_instance_statics_GetRuntimeClassName,
    app_instance_statics_GetTrustLevel,
    app_instance_statics_GetActivatedEventArgs,
    app_instance_statics_FindOrRegisterInstanceForKey,
    app_instance_statics_Unregister,
    app_instance_statics_GetInstances,
    app_instance_statics_get_RecommendedInstance,
};

static struct app_instance_factory app_instance_factory_obj =
{
    .IActivationFactory_iface.lpVtbl = &app_instance_factory_vtbl,
    .IAppInstanceStatics_iface.lpVtbl = &app_instance_statics_vtbl,
    .ref = 1,
};

IActivationFactory *app_instance_factory = &app_instance_factory_obj.IActivationFactory_iface;
