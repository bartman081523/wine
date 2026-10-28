/*
 * WinRT Windows.ApplicationModel.Resources.ResourceLoader Implementation
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

static const GUID IID_IResourceLoader          = {0x08524908, 0x16ef, 0x45ad, {0xa6, 0x02, 0x29, 0x36, 0x37, 0xd7, 0xe6, 0x1a}};
static const GUID IID_IResourceLoader2         = {0x10eb6ec6, 0x8138, 0x48c1, {0xbc, 0x65, 0xe1, 0xf1, 0x42, 0x07, 0x36, 0x7c}};
static const GUID IID_IResourceLoaderFactory   = {0xc33a3603, 0x69dc, 0x4285, {0xa0, 0x77, 0xd5, 0xc0, 0xe4, 0x7c, 0xcb, 0xe8}};
static const GUID IID_IResourceLoaderStatics   = {0xbf777ce1, 0x19c8, 0x49c2, {0x95, 0x3c, 0x47, 0xe9, 0x22, 0x7b, 0x33, 0x4e}};
static const GUID IID_IResourceLoaderStatics2  = {0x0cc04141, 0x6466, 0x4989, {0x94, 0x94, 0x0b, 0x82, 0xdf, 0xc5, 0x3f, 0x1f}};
static const GUID IID_IResourceLoaderStatics3  = {0x64609dfb, 0x64ac, 0x491b, {0x81, 0x00, 0x0e, 0x55, 0x8d, 0x61, 0xc1, 0xd0}};

#define RuntimeClass_Windows_ApplicationModel_Resources_ResourceLoader L"Windows.ApplicationModel.Resources.ResourceLoader"

typedef struct IResourceLoader IResourceLoader;
typedef struct IResourceLoader2 IResourceLoader2;
typedef struct IResourceLoaderFactory IResourceLoaderFactory;
typedef struct IResourceLoaderStatics IResourceLoaderStatics;
typedef struct IResourceLoaderStatics2 IResourceLoaderStatics2;
typedef struct IResourceLoaderStatics3 IResourceLoaderStatics3;

/* --- IResourceLoader --- */
typedef struct IResourceLoaderVtbl
{
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IResourceLoader *This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IResourceLoader *This);
    ULONG (STDMETHODCALLTYPE *Release)(IResourceLoader *This);
    HRESULT (STDMETHODCALLTYPE *GetIids)(IResourceLoader *This, ULONG *iidCount, IID **iids);
    HRESULT (STDMETHODCALLTYPE *GetRuntimeClassName)(IResourceLoader *This, HSTRING *className);
    HRESULT (STDMETHODCALLTYPE *GetTrustLevel)(IResourceLoader *This, TrustLevel *trustLevel);
    HRESULT (STDMETHODCALLTYPE *GetString)(IResourceLoader *This, HSTRING resource, HSTRING *value);
} IResourceLoaderVtbl;

struct IResourceLoader
{
    const IResourceLoaderVtbl *lpVtbl;
};

/* --- IResourceLoader2 --- */
typedef struct IResourceLoader2Vtbl
{
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IResourceLoader2 *This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IResourceLoader2 *This);
    ULONG (STDMETHODCALLTYPE *Release)(IResourceLoader2 *This);
    HRESULT (STDMETHODCALLTYPE *GetIids)(IResourceLoader2 *This, ULONG *iidCount, IID **iids);
    HRESULT (STDMETHODCALLTYPE *GetRuntimeClassName)(IResourceLoader2 *This, HSTRING *className);
    HRESULT (STDMETHODCALLTYPE *GetTrustLevel)(IResourceLoader2 *This, TrustLevel *trustLevel);
    HRESULT (STDMETHODCALLTYPE *GetStringForUri)(IResourceLoader2 *This, IInspectable *uri, HSTRING *value);
} IResourceLoader2Vtbl;

struct IResourceLoader2
{
    const IResourceLoader2Vtbl *lpVtbl;
};

/* --- IResourceLoaderStatics --- */
typedef struct IResourceLoaderStaticsVtbl
{
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IResourceLoaderStatics *This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IResourceLoaderStatics *This);
    ULONG (STDMETHODCALLTYPE *Release)(IResourceLoaderStatics *This);
    HRESULT (STDMETHODCALLTYPE *GetIids)(IResourceLoaderStatics *This, ULONG *iidCount, IID **iids);
    HRESULT (STDMETHODCALLTYPE *GetRuntimeClassName)(IResourceLoaderStatics *This, HSTRING *className);
    HRESULT (STDMETHODCALLTYPE *GetTrustLevel)(IResourceLoaderStatics *This, TrustLevel *trustLevel);
    HRESULT (STDMETHODCALLTYPE *GetStringForReference)(IResourceLoaderStatics *This, IInspectable *uri, HSTRING *value);
} IResourceLoaderStaticsVtbl;

struct IResourceLoaderStatics
{
    const IResourceLoaderStaticsVtbl *lpVtbl;
};

/* --- IResourceLoaderStatics2 --- */
typedef struct IResourceLoaderStatics2Vtbl
{
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IResourceLoaderStatics2 *This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IResourceLoaderStatics2 *This);
    ULONG (STDMETHODCALLTYPE *Release)(IResourceLoaderStatics2 *This);
    HRESULT (STDMETHODCALLTYPE *GetIids)(IResourceLoaderStatics2 *This, ULONG *iidCount, IID **iids);
    HRESULT (STDMETHODCALLTYPE *GetRuntimeClassName)(IResourceLoaderStatics2 *This, HSTRING *className);
    HRESULT (STDMETHODCALLTYPE *GetTrustLevel)(IResourceLoaderStatics2 *This, TrustLevel *trustLevel);
    HRESULT (STDMETHODCALLTYPE *GetForCurrentView)(IResourceLoaderStatics2 *This, IResourceLoader **value);
    HRESULT (STDMETHODCALLTYPE *GetForCurrentViewWithName)(IResourceLoaderStatics2 *This, HSTRING name, IResourceLoader **value);
    HRESULT (STDMETHODCALLTYPE *GetForViewIndependentUse)(IResourceLoaderStatics2 *This, IResourceLoader **value);
    HRESULT (STDMETHODCALLTYPE *GetForViewIndependentUseWithName)(IResourceLoaderStatics2 *This, HSTRING name, IResourceLoader **value);
} IResourceLoaderStatics2Vtbl;

struct IResourceLoaderStatics2
{
    const IResourceLoaderStatics2Vtbl *lpVtbl;
};

/* --- IResourceLoaderStatics3 --- */
typedef struct IResourceLoaderStatics3Vtbl
{
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IResourceLoaderStatics3 *This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IResourceLoaderStatics3 *This);
    ULONG (STDMETHODCALLTYPE *Release)(IResourceLoaderStatics3 *This);
    HRESULT (STDMETHODCALLTYPE *GetIids)(IResourceLoaderStatics3 *This, ULONG *iidCount, IID **iids);
    HRESULT (STDMETHODCALLTYPE *GetRuntimeClassName)(IResourceLoaderStatics3 *This, HSTRING *className);
    HRESULT (STDMETHODCALLTYPE *GetTrustLevel)(IResourceLoaderStatics3 *This, TrustLevel *trustLevel);
    HRESULT (STDMETHODCALLTYPE *GetForUIContext)(IResourceLoaderStatics3 *This, IInspectable *context, IResourceLoader **value);
} IResourceLoaderStatics3Vtbl;

struct IResourceLoaderStatics3
{
    const IResourceLoaderStatics3Vtbl *lpVtbl;
};

/* --- IResourceLoaderFactory --- */
typedef struct IResourceLoaderFactoryVtbl
{
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IResourceLoaderFactory *This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IResourceLoaderFactory *This);
    ULONG (STDMETHODCALLTYPE *Release)(IResourceLoaderFactory *This);
    HRESULT (STDMETHODCALLTYPE *GetIids)(IResourceLoaderFactory *This, ULONG *iidCount, IID **iids);
    HRESULT (STDMETHODCALLTYPE *GetRuntimeClassName)(IResourceLoaderFactory *This, HSTRING *className);
    HRESULT (STDMETHODCALLTYPE *GetTrustLevel)(IResourceLoaderFactory *This, TrustLevel *trustLevel);
    HRESULT (STDMETHODCALLTYPE *CreateResourceLoaderByName)(IResourceLoaderFactory *This, HSTRING name, IResourceLoader **value);
} IResourceLoaderFactoryVtbl;

struct IResourceLoaderFactory
{
    const IResourceLoaderFactoryVtbl *lpVtbl;
};

/* --- Instance implementation --- */
struct resource_loader_impl
{
    IResourceLoader IResourceLoader_iface;
    IResourceLoader2 IResourceLoader2_iface;
    LONG ref;
};

static inline struct resource_loader_impl *impl_from_IResourceLoader(IResourceLoader *iface)
{
    return CONTAINING_RECORD(iface, struct resource_loader_impl, IResourceLoader_iface);
}

static inline struct resource_loader_impl *impl_from_IResourceLoader2(IResourceLoader2 *iface)
{
    return CONTAINING_RECORD(iface, struct resource_loader_impl, IResourceLoader2_iface);
}

static HRESULT WINAPI loader_QueryInterface(IResourceLoader *iface, REFIID riid, void **ppvObject)
{
    struct resource_loader_impl *impl = impl_from_IResourceLoader(iface);

    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IResourceLoader))
    {
        *ppvObject = &impl->IResourceLoader_iface;
        impl->IResourceLoader_iface.lpVtbl->AddRef(&impl->IResourceLoader_iface);
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IResourceLoader2))
    {
        *ppvObject = &impl->IResourceLoader2_iface;
        impl->IResourceLoader2_iface.lpVtbl->AddRef(&impl->IResourceLoader2_iface);
        return S_OK;
    }

    FIXME("loader %p: unhandled interface %s\n", iface, debugstr_guid(riid));
    *ppvObject = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI loader_AddRef(IResourceLoader *iface)
{
    struct resource_loader_impl *impl = impl_from_IResourceLoader(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI loader_Release(IResourceLoader *iface)
{
    struct resource_loader_impl *impl = impl_from_IResourceLoader(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI loader_GetIids(IResourceLoader *iface, ULONG *iidCount, IID **iids)
{
    FIXME("stub!\n");
    return E_NOTIMPL;
}

static HRESULT WINAPI loader_GetRuntimeClassName(IResourceLoader *iface, HSTRING *className)
{
    return WindowsCreateString(RuntimeClass_Windows_ApplicationModel_Resources_ResourceLoader,
                               wcslen(RuntimeClass_Windows_ApplicationModel_Resources_ResourceLoader),
                               className);
}

static HRESULT WINAPI loader_GetTrustLevel(IResourceLoader *iface, TrustLevel *trustLevel)
{
    *trustLevel = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI loader_GetString(IResourceLoader *iface, HSTRING resource, HSTRING *value)
{
    const WCHAR *str = WindowsGetStringRawBuffer(resource, NULL);
    TRACE("(%p, %s, %p)\n", iface, debugstr_w(str), value);
    if (!value) return E_POINTER;
    return WindowsCreateString(L"", 0, value);
}

static const IResourceLoaderVtbl resource_loader_vtbl =
{
    loader_QueryInterface,
    loader_AddRef,
    loader_Release,
    loader_GetIids,
    loader_GetRuntimeClassName,
    loader_GetTrustLevel,
    loader_GetString,
};

static HRESULT WINAPI loader2_QueryInterface(IResourceLoader2 *iface, REFIID riid, void **ppvObject)
{
    struct resource_loader_impl *impl = impl_from_IResourceLoader2(iface);
    return loader_QueryInterface(&impl->IResourceLoader_iface, riid, ppvObject);
}

static ULONG WINAPI loader2_AddRef(IResourceLoader2 *iface)
{
    struct resource_loader_impl *impl = impl_from_IResourceLoader2(iface);
    return loader_AddRef(&impl->IResourceLoader_iface);
}

static ULONG WINAPI loader2_Release(IResourceLoader2 *iface)
{
    struct resource_loader_impl *impl = impl_from_IResourceLoader2(iface);
    return loader_Release(&impl->IResourceLoader_iface);
}

static HRESULT WINAPI loader2_GetIids(IResourceLoader2 *iface, ULONG *iidCount, IID **iids)
{
    struct resource_loader_impl *impl = impl_from_IResourceLoader2(iface);
    return loader_GetIids(&impl->IResourceLoader_iface, iidCount, iids);
}

static HRESULT WINAPI loader2_GetRuntimeClassName(IResourceLoader2 *iface, HSTRING *className)
{
    struct resource_loader_impl *impl = impl_from_IResourceLoader2(iface);
    return loader_GetRuntimeClassName(&impl->IResourceLoader_iface, className);
}

static HRESULT WINAPI loader2_GetTrustLevel(IResourceLoader2 *iface, TrustLevel *trustLevel)
{
    struct resource_loader_impl *impl = impl_from_IResourceLoader2(iface);
    return loader_GetTrustLevel(&impl->IResourceLoader_iface, trustLevel);
}

static HRESULT WINAPI loader2_GetStringForUri(IResourceLoader2 *iface, IInspectable *uri, HSTRING *value)
{
    TRACE("(%p, %p, %p)\n", iface, uri, value);
    if (!value) return E_POINTER;
    return WindowsCreateString(L"", 0, value);
}

static const IResourceLoader2Vtbl resource_loader2_vtbl =
{
    loader2_QueryInterface,
    loader2_AddRef,
    loader2_Release,
    loader2_GetIids,
    loader2_GetRuntimeClassName,
    loader2_GetTrustLevel,
    loader2_GetStringForUri,
};

static HRESULT create_resource_loader_instance(IResourceLoader **out)
{
    struct resource_loader_impl *impl = calloc(1, sizeof(*impl));
    if (!impl) return E_OUTOFMEMORY;
    impl->IResourceLoader_iface.lpVtbl = &resource_loader_vtbl;
    impl->IResourceLoader2_iface.lpVtbl = &resource_loader2_vtbl;
    impl->ref = 1;
    *out = &impl->IResourceLoader_iface;
    return S_OK;
}

/* --- Factory & Statics implementation --- */
struct resource_loader_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    IResourceLoaderStatics IResourceLoaderStatics_iface;
    IResourceLoaderStatics2 IResourceLoaderStatics2_iface;
    IResourceLoaderStatics3 IResourceLoaderStatics3_iface;
    IResourceLoaderFactory IResourceLoaderFactory_iface;
};

static inline struct resource_loader_factory_impl *impl_from_IActivationFactory(IActivationFactory *iface)
{
    return CONTAINING_RECORD(iface, struct resource_loader_factory_impl, IActivationFactory_iface);
}

static inline struct resource_loader_factory_impl *impl_from_IResourceLoaderStatics(IResourceLoaderStatics *iface)
{
    return CONTAINING_RECORD(iface, struct resource_loader_factory_impl, IResourceLoaderStatics_iface);
}

static inline struct resource_loader_factory_impl *impl_from_IResourceLoaderStatics2(IResourceLoaderStatics2 *iface)
{
    return CONTAINING_RECORD(iface, struct resource_loader_factory_impl, IResourceLoaderStatics2_iface);
}

static inline struct resource_loader_factory_impl *impl_from_IResourceLoaderStatics3(IResourceLoaderStatics3 *iface)
{
    return CONTAINING_RECORD(iface, struct resource_loader_factory_impl, IResourceLoaderStatics3_iface);
}

static inline struct resource_loader_factory_impl *impl_from_IResourceLoaderFactory(IResourceLoaderFactory *iface)
{
    return CONTAINING_RECORD(iface, struct resource_loader_factory_impl, IResourceLoaderFactory_iface);
}

static HRESULT WINAPI fac_QueryInterface(IActivationFactory *iface, REFIID riid, void **ppvObject)
{
    struct resource_loader_factory_impl *impl = impl_from_IActivationFactory(iface);

    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppvObject = &impl->IActivationFactory_iface;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IResourceLoaderStatics))
    {
        *ppvObject = &impl->IResourceLoaderStatics_iface;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IResourceLoaderStatics2))
    {
        *ppvObject = &impl->IResourceLoaderStatics2_iface;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IResourceLoaderStatics3))
    {
        *ppvObject = &impl->IResourceLoaderStatics3_iface;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IResourceLoaderFactory))
    {
        *ppvObject = &impl->IResourceLoaderFactory_iface;
        return S_OK;
    }

    FIXME("resource_loader_factory %p: unhandled interface %s\n", iface, debugstr_guid(riid));
    *ppvObject = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI fac_AddRef(IActivationFactory *iface)
{
    return 2;
}

static ULONG WINAPI fac_Release(IActivationFactory *iface)
{
    return 1;
}

static HRESULT WINAPI fac_GetIids(IActivationFactory *iface, ULONG *iidCount, IID **iids)
{
    FIXME("stub!\n");
    return E_NOTIMPL;
}

static HRESULT WINAPI fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *className)
{
    return WindowsCreateString(RuntimeClass_Windows_ApplicationModel_Resources_ResourceLoader,
                               wcslen(RuntimeClass_Windows_ApplicationModel_Resources_ResourceLoader),
                               className);
}

static HRESULT WINAPI fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *trustLevel)
{
    *trustLevel = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI fac_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    IResourceLoader *loader;
    HRESULT hr = create_resource_loader_instance(&loader);
    if (SUCCEEDED(hr)) *instance = (IInspectable *)loader;
    return hr;
}

static const IActivationFactoryVtbl resource_loader_factory_vtbl =
{
    fac_QueryInterface,
    fac_AddRef,
    fac_Release,
    fac_GetIids,
    fac_GetRuntimeClassName,
    fac_GetTrustLevel,
    fac_ActivateInstance,
};

/* --- IResourceLoaderStatics methods --- */
static HRESULT WINAPI statics_QueryInterface(IResourceLoaderStatics *iface, REFIID riid, void **ppvObject)
{
    struct resource_loader_factory_impl *impl = impl_from_IResourceLoaderStatics(iface);
    return fac_QueryInterface(&impl->IActivationFactory_iface, riid, ppvObject);
}

static ULONG WINAPI statics_AddRef(IResourceLoaderStatics *iface)
{
    return 2;
}

static ULONG WINAPI statics_Release(IResourceLoaderStatics *iface)
{
    return 1;
}

static HRESULT WINAPI statics_GetIids(IResourceLoaderStatics *iface, ULONG *iidCount, IID **iids)
{
    FIXME("stub!\n");
    return E_NOTIMPL;
}

static HRESULT WINAPI statics_GetRuntimeClassName(IResourceLoaderStatics *iface, HSTRING *className)
{
    return WindowsCreateString(RuntimeClass_Windows_ApplicationModel_Resources_ResourceLoader,
                               wcslen(RuntimeClass_Windows_ApplicationModel_Resources_ResourceLoader),
                               className);
}

static HRESULT WINAPI statics_GetTrustLevel(IResourceLoaderStatics *iface, TrustLevel *trustLevel)
{
    *trustLevel = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI statics_GetStringForReference(IResourceLoaderStatics *iface, IInspectable *uri, HSTRING *value)
{
    TRACE("(%p, %p, %p)\n", iface, uri, value);
    if (!value) return E_POINTER;
    return WindowsCreateString(L"", 0, value);
}

static const IResourceLoaderStaticsVtbl resource_loader_statics_vtbl =
{
    statics_QueryInterface,
    statics_AddRef,
    statics_Release,
    statics_GetIids,
    statics_GetRuntimeClassName,
    statics_GetTrustLevel,
    statics_GetStringForReference,
};

/* --- IResourceLoaderStatics2 methods --- */
static HRESULT WINAPI statics2_QueryInterface(IResourceLoaderStatics2 *iface, REFIID riid, void **ppvObject)
{
    struct resource_loader_factory_impl *impl = impl_from_IResourceLoaderStatics2(iface);
    return fac_QueryInterface(&impl->IActivationFactory_iface, riid, ppvObject);
}

static ULONG WINAPI statics2_AddRef(IResourceLoaderStatics2 *iface)
{
    return 2;
}

static ULONG WINAPI statics2_Release(IResourceLoaderStatics2 *iface)
{
    return 1;
}

static HRESULT WINAPI statics2_GetIids(IResourceLoaderStatics2 *iface, ULONG *iidCount, IID **iids)
{
    FIXME("stub!\n");
    return E_NOTIMPL;
}

static HRESULT WINAPI statics2_GetRuntimeClassName(IResourceLoaderStatics2 *iface, HSTRING *className)
{
    return WindowsCreateString(RuntimeClass_Windows_ApplicationModel_Resources_ResourceLoader,
                               wcslen(RuntimeClass_Windows_ApplicationModel_Resources_ResourceLoader),
                               className);
}

static HRESULT WINAPI statics2_GetTrustLevel(IResourceLoaderStatics2 *iface, TrustLevel *trustLevel)
{
    *trustLevel = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI statics2_GetForCurrentView(IResourceLoaderStatics2 *iface, IResourceLoader **value)
{
    TRACE("(%p, %p)\n", iface, value);
    if (!value) return E_POINTER;
    return create_resource_loader_instance(value);
}

static HRESULT WINAPI statics2_GetForCurrentViewWithName(IResourceLoaderStatics2 *iface, HSTRING name, IResourceLoader **value)
{
    TRACE("(%p, %p)\n", iface, value);
    if (!value) return E_POINTER;
    return create_resource_loader_instance(value);
}

static HRESULT WINAPI statics2_GetForViewIndependentUse(IResourceLoaderStatics2 *iface, IResourceLoader **value)
{
    TRACE("(%p, %p)\n", iface, value);
    if (!value) return E_POINTER;
    return create_resource_loader_instance(value);
}

static HRESULT WINAPI statics2_GetForViewIndependentUseWithName(IResourceLoaderStatics2 *iface, HSTRING name, IResourceLoader **value)
{
    TRACE("(%p, %p)\n", iface, value);
    if (!value) return E_POINTER;
    return create_resource_loader_instance(value);
}

static const IResourceLoaderStatics2Vtbl resource_loader_statics2_vtbl =
{
    statics2_QueryInterface,
    statics2_AddRef,
    statics2_Release,
    statics2_GetIids,
    statics2_GetRuntimeClassName,
    statics2_GetTrustLevel,
    statics2_GetForCurrentView,
    statics2_GetForCurrentViewWithName,
    statics2_GetForViewIndependentUse,
    statics2_GetForViewIndependentUseWithName,
};

/* --- IResourceLoaderStatics3 methods --- */
static HRESULT WINAPI statics3_QueryInterface(IResourceLoaderStatics3 *iface, REFIID riid, void **ppvObject)
{
    struct resource_loader_factory_impl *impl = impl_from_IResourceLoaderStatics3(iface);
    return fac_QueryInterface(&impl->IActivationFactory_iface, riid, ppvObject);
}

static ULONG WINAPI statics3_AddRef(IResourceLoaderStatics3 *iface)
{
    return 2;
}

static ULONG WINAPI statics3_Release(IResourceLoaderStatics3 *iface)
{
    return 1;
}

static HRESULT WINAPI statics3_GetIids(IResourceLoaderStatics3 *iface, ULONG *iidCount, IID **iids)
{
    FIXME("stub!\n");
    return E_NOTIMPL;
}

static HRESULT WINAPI statics3_GetRuntimeClassName(IResourceLoaderStatics3 *iface, HSTRING *className)
{
    return WindowsCreateString(RuntimeClass_Windows_ApplicationModel_Resources_ResourceLoader,
                               wcslen(RuntimeClass_Windows_ApplicationModel_Resources_ResourceLoader),
                               className);
}

static HRESULT WINAPI statics3_GetTrustLevel(IResourceLoaderStatics3 *iface, TrustLevel *trustLevel)
{
    *trustLevel = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI statics3_GetForUIContext(IResourceLoaderStatics3 *iface, IInspectable *context, IResourceLoader **value)
{
    TRACE("(%p, %p, %p)\n", iface, context, value);
    if (!value) return E_POINTER;
    return create_resource_loader_instance(value);
}

static const IResourceLoaderStatics3Vtbl resource_loader_statics3_vtbl =
{
    statics3_QueryInterface,
    statics3_AddRef,
    statics3_Release,
    statics3_GetIids,
    statics3_GetRuntimeClassName,
    statics3_GetTrustLevel,
    statics3_GetForUIContext,
};

/* --- IResourceLoaderFactory methods --- */
static HRESULT WINAPI fac_methods_QueryInterface(IResourceLoaderFactory *iface, REFIID riid, void **ppvObject)
{
    struct resource_loader_factory_impl *impl = impl_from_IResourceLoaderFactory(iface);
    return fac_QueryInterface(&impl->IActivationFactory_iface, riid, ppvObject);
}

static ULONG WINAPI fac_methods_AddRef(IResourceLoaderFactory *iface)
{
    return 2;
}

static ULONG WINAPI fac_methods_Release(IResourceLoaderFactory *iface)
{
    return 1;
}

static HRESULT WINAPI fac_methods_GetIids(IResourceLoaderFactory *iface, ULONG *iidCount, IID **iids)
{
    FIXME("stub!\n");
    return E_NOTIMPL;
}

static HRESULT WINAPI fac_methods_GetRuntimeClassName(IResourceLoaderFactory *iface, HSTRING *className)
{
    return WindowsCreateString(RuntimeClass_Windows_ApplicationModel_Resources_ResourceLoader,
                               wcslen(RuntimeClass_Windows_ApplicationModel_Resources_ResourceLoader),
                               className);
}

static HRESULT WINAPI fac_methods_GetTrustLevel(IResourceLoaderFactory *iface, TrustLevel *trustLevel)
{
    *trustLevel = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI fac_methods_CreateResourceLoaderByName(IResourceLoaderFactory *iface, HSTRING name, IResourceLoader **value)
{
    TRACE("(%p, %p)\n", iface, value);
    if (!value) return E_POINTER;
    return create_resource_loader_instance(value);
}

static const IResourceLoaderFactoryVtbl resource_loader_factory_methods_vtbl =
{
    fac_methods_QueryInterface,
    fac_methods_AddRef,
    fac_methods_Release,
    fac_methods_GetIids,
    fac_methods_GetRuntimeClassName,
    fac_methods_GetTrustLevel,
    fac_methods_CreateResourceLoaderByName,
};

static struct resource_loader_factory_impl default_resource_loader_factory =
{
    {&resource_loader_factory_vtbl},
    {&resource_loader_statics_vtbl},
    {&resource_loader_statics2_vtbl},
    {&resource_loader_statics3_vtbl},
    {&resource_loader_factory_methods_vtbl},
};

IActivationFactory *resource_loader_factory = &default_resource_loader_factory.IActivationFactory_iface;
