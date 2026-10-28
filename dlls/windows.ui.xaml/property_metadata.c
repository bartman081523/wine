/* WinRT Windows.UI.Xaml PropertyMetadata, DependencyProperty, DependencyObject Implementation
 *
 * Copyright (C) 2026 Antigravity
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "private.h"

WINE_DEFAULT_DEBUG_CHANNEL(xaml);

#define RuntimeClass_PropertyMetadata   L"Windows.UI.Xaml.PropertyMetadata"
#define RuntimeClass_DependencyProperty L"Windows.UI.Xaml.DependencyProperty"
#define RuntimeClass_DependencyObject   L"Windows.UI.Xaml.DependencyObject"

static const GUID IID_IPropertyMetadata = {0x814ef30d, 0x8d18, 0x448a, {0x86, 0x44, 0xf2, 0xcb, 0x51, 0xe7, 0x03, 0x80}};
static const GUID IID_IPropertyMetadataFactory = {0xc1b81cc0, 0x57cd, 0x4f2f, {0xb0, 0xa9, 0xe1, 0x80, 0x1b, 0x28, 0xf7, 0x6b}};
static const GUID IID_IPropertyMetadataStatics = {0x3b01077a, 0x6e06, 0x45e9, {0x8b, 0x5c, 0xaf, 0x24, 0x34, 0x58, 0xc0, 0x62}};

static const GUID IID_IDependencyProperty = {0x85b13970, 0x9bc4, 0x4e96, {0xac, 0xf1, 0x30, 0xc8, 0xfd, 0x3d, 0x55, 0xc8}};
static const GUID IID_IDependencyPropertyStatics = {0x49e5f28f, 0x8259, 0x4d5c, {0xaa, 0xe0, 0x83, 0xd5, 0x6d, 0xbb, 0x68, 0xd9}};

static const GUID IID_IDependencyObject = {0x5c526665, 0xf60e, 0x4912, {0xaf, 0x59, 0x5f, 0xe0, 0x68, 0x0f, 0x08, 0x9d}};
static const GUID IID_IDependencyObject2 = {0x29fed85d, 0x3d22, 0x43a1, {0xad, 0xd0, 0x17, 0x02, 0x7c, 0x08, 0xb2, 0x12}};
static const GUID IID_IDependencyObjectFactory = {0x9a03af92, 0x7d8a, 0x4937, {0x88, 0x4f, 0xec, 0xf3, 0x4f, 0xe0, 0x2a, 0xcb}};

static const GUID IID_ComWrappers_internal = {0x247e6ffb, 0x5524, 0x54bf, {0xa2, 0x75, 0x82, 0x3d, 0x16, 0x77, 0x57, 0x68}};

struct TypeName_raw
{
    HSTRING Name;
    int Kind;
};

/* Redhawk safe hooks */
static HRESULT WINAPI redhawk_safe_stub(void *This) { return S_OK; }
static HRESULT WINAPI redhawk_safe_stub_arg(void *This, void *arg) { return S_OK; }

/* -------------------------------------------------------------------------
 * PropertyMetadata Instance
 * ------------------------------------------------------------------------- */
struct property_metadata_impl
{
    struct { const void *lpVtbl; } IPropertyMetadata_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    IInspectable *default_value;
    void *callback;
    LONG ref;
};

#define impl_from_IPropertyMetadata(ptr) ((struct property_metadata_impl *)((char *)(ptr) - offsetof(struct property_metadata_impl, IPropertyMetadata_iface)))
#define impl_from_IRedhawk_pm(ptr) ((struct property_metadata_impl *)((char *)(ptr) - offsetof(struct property_metadata_impl, IRedhawkInterop_iface)))

static HRESULT WINAPI pm_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    struct property_metadata_impl *impl = impl_from_IPropertyMetadata(This);
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("pm %p, riid %s\n", This, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IPropertyMetadata))
    {
        *ppvObject = &impl->IPropertyMetadata_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ComWrappers_internal))
    {
        *ppvObject = &impl->IRedhawkInterop_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    WARN("unhandled riid %s\n", debugstr_guid(riid));
    return E_NOINTERFACE;
}

static ULONG WINAPI pm_AddRef(void *This)
{
    struct property_metadata_impl *impl = impl_from_IPropertyMetadata(This);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI pm_Release(void *This)
{
    struct property_metadata_impl *impl = impl_from_IPropertyMetadata(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref)
    {
        if (impl->default_value) IInspectable_Release(impl->default_value);
        free(impl);
    }
    return ref;
}

static HRESULT WINAPI pm_GetIids(void *This, ULONG *iid_count, IID **iids)
{
    FIXME("stub\n");
    return E_NOTIMPL;
}

static HRESULT WINAPI pm_GetRuntimeClassName(void *This, HSTRING *class_name)
{
    return WindowsCreateString(RuntimeClass_PropertyMetadata, wcslen(RuntimeClass_PropertyMetadata), class_name);
}

static HRESULT WINAPI pm_GetTrustLevel(void *This, TrustLevel *trust_level)
{
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI pm_get_DefaultValue(void *This, IInspectable **value)
{
    struct property_metadata_impl *impl = impl_from_IPropertyMetadata(This);
    TRACE("pm %p, default_value %p\n", impl, impl->default_value);
    if (!value) return E_POINTER;
    *value = impl->default_value;
    if (*value) IInspectable_AddRef(*value);
    return S_OK;
}

static HRESULT WINAPI pm_get_CreateDefaultValueCallback(void *This, void **value)
{
    if (!value) return E_POINTER;
    *value = NULL;
    return S_OK;
}

static const struct {
    HRESULT (WINAPI *QueryInterface)(void *, REFIID, void **);
    ULONG   (WINAPI *AddRef)(void *);
    ULONG   (WINAPI *Release)(void *);
    HRESULT (WINAPI *GetIids)(void *, ULONG *, IID **);
    HRESULT (WINAPI *GetRuntimeClassName)(void *, HSTRING *);
    HRESULT (WINAPI *GetTrustLevel)(void *, TrustLevel *);
    HRESULT (WINAPI *get_DefaultValue)(void *, IInspectable **);
    HRESULT (WINAPI *get_CreateDefaultValueCallback)(void *, void **);
} property_metadata_vtbl = {
    pm_QueryInterface,
    pm_AddRef,
    pm_Release,
    pm_GetIids,
    pm_GetRuntimeClassName,
    pm_GetTrustLevel,
    pm_get_DefaultValue,
    pm_get_CreateDefaultValueCallback
};

static const void *redhawk_pm_vtbl[16] = {
    pm_QueryInterface, pm_AddRef, pm_Release,
    redhawk_safe_stub, redhawk_safe_stub, redhawk_safe_stub,
    pm_get_DefaultValue, redhawk_safe_stub, redhawk_safe_stub,
    redhawk_safe_stub, redhawk_safe_stub, redhawk_safe_stub,
    redhawk_safe_stub, redhawk_safe_stub, redhawk_safe_stub,
    redhawk_safe_stub
};

static struct property_metadata_impl *create_property_metadata(IInspectable *default_value, void *callback)
{
    struct property_metadata_impl *impl = calloc(1, sizeof(*impl));
    if (!impl) return NULL;
    impl->IPropertyMetadata_iface.lpVtbl = &property_metadata_vtbl;
    impl->IRedhawkInterop_iface.lpVtbl = redhawk_pm_vtbl;
    impl->ref = 1;
    impl->default_value = default_value;
    if (impl->default_value) IInspectable_AddRef(impl->default_value);
    impl->callback = callback;
    return impl;
}

/* -------------------------------------------------------------------------
 * PropertyMetadata Factory & Statics
 * ------------------------------------------------------------------------- */
struct property_metadata_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } IPropertyMetadataFactory_iface;
    struct { const void *lpVtbl; } IPropertyMetadataStatics_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    LONG ref;
};

#define impl_from_pm_IActivationFactory(ptr) ((struct property_metadata_factory_impl *)((char *)(ptr) - offsetof(struct property_metadata_factory_impl, IActivationFactory_iface)))
#define impl_from_pm_IFactory(ptr) ((struct property_metadata_factory_impl *)((char *)(ptr) - offsetof(struct property_metadata_factory_impl, IPropertyMetadataFactory_iface)))
#define impl_from_pm_IStatics(ptr) ((struct property_metadata_factory_impl *)((char *)(ptr) - offsetof(struct property_metadata_factory_impl, IPropertyMetadataStatics_iface)))
#define impl_from_pm_IRedhawk(ptr) ((struct property_metadata_factory_impl *)((char *)(ptr) - offsetof(struct property_metadata_factory_impl, IRedhawkInterop_iface)))

static HRESULT WINAPI pm_fac_QueryInterface(IActivationFactory *iface, REFIID riid, void **ppvObject)
{
    struct property_metadata_factory_impl *impl = impl_from_pm_IActivationFactory(iface);
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("pm_fac %p, riid %s\n", iface, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppvObject = &impl->IActivationFactory_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IPropertyMetadataFactory))
    {
        *ppvObject = &impl->IPropertyMetadataFactory_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IPropertyMetadataStatics))
    {
        *ppvObject = &impl->IPropertyMetadataStatics_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ComWrappers_internal))
    {
        *ppvObject = &impl->IRedhawkInterop_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    WARN("unhandled riid %s\n", debugstr_guid(riid));
    return E_NOINTERFACE;
}

static ULONG WINAPI pm_fac_AddRef(IActivationFactory *iface)
{
    struct property_metadata_factory_impl *impl = impl_from_pm_IActivationFactory(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI pm_fac_Release(IActivationFactory *iface)
{
    struct property_metadata_factory_impl *impl = impl_from_pm_IActivationFactory(iface);
    return InterlockedDecrement(&impl->ref);
}

static HRESULT WINAPI pm_fac_GetIids(IActivationFactory *iface, ULONG *iid_count, IID **iids)
{
    FIXME("stub\n");
    return E_NOTIMPL;
}

static HRESULT WINAPI pm_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *class_name)
{
    return WindowsCreateString(RuntimeClass_PropertyMetadata, wcslen(RuntimeClass_PropertyMetadata), class_name);
}

static HRESULT WINAPI pm_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *trust_level)
{
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI pm_fac_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    struct property_metadata_impl *impl;
    TRACE("iface %p, instance %p\n", iface, instance);
    if (!instance) return E_POINTER;
    impl = create_property_metadata(NULL, NULL);
    if (!impl) return E_OUTOFMEMORY;
    *instance = (IInspectable *)&impl->IPropertyMetadata_iface;
    return S_OK;
}

static const struct IActivationFactoryVtbl pm_activation_factory_vtbl = {
    pm_fac_QueryInterface,
    pm_fac_AddRef,
    pm_fac_Release,
    pm_fac_GetIids,
    pm_fac_GetRuntimeClassName,
    pm_fac_GetTrustLevel,
    pm_fac_ActivateInstance
};

/* IPropertyMetadataFactory */
static HRESULT WINAPI pm_f_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return pm_fac_QueryInterface((IActivationFactory *)impl_from_pm_IFactory(This), riid, ppvObject);
}
static ULONG WINAPI pm_f_AddRef(void *This)
{
    return pm_fac_AddRef((IActivationFactory *)impl_from_pm_IFactory(This));
}
static ULONG WINAPI pm_f_Release(void *This)
{
    return pm_fac_Release((IActivationFactory *)impl_from_pm_IFactory(This));
}
static HRESULT WINAPI pm_f_GetIids(void *This, ULONG *iid_count, IID **iids)
{
    return pm_fac_GetIids((IActivationFactory *)impl_from_pm_IFactory(This), iid_count, iids);
}
static HRESULT WINAPI pm_f_GetRuntimeClassName(void *This, HSTRING *class_name)
{
    return pm_fac_GetRuntimeClassName((IActivationFactory *)impl_from_pm_IFactory(This), class_name);
}
static HRESULT WINAPI pm_f_GetTrustLevel(void *This, TrustLevel *trust_level)
{
    return pm_fac_GetTrustLevel((IActivationFactory *)impl_from_pm_IFactory(This), trust_level);
}

static HRESULT WINAPI pm_f_CreateInstanceWithDefaultValue(void *This, IInspectable *default_value, IInspectable *base_interface, IInspectable **inner_interface, void **value)
{
    struct property_metadata_impl *impl;
    TRACE("This %p, default_value %p\n", This, default_value);
    if (!value) return E_POINTER;
    impl = create_property_metadata(default_value, NULL);
    if (!impl) return E_OUTOFMEMORY;
    if (inner_interface)
    {
        *inner_interface = (IInspectable *)&impl->IPropertyMetadata_iface;
        IInspectable_AddRef(*inner_interface);
    }
    *value = &impl->IPropertyMetadata_iface;
    return S_OK;
}

static HRESULT WINAPI pm_f_CreateInstanceWithDefaultValueAndCallback(void *This, IInspectable *default_value, void *callback, IInspectable *base_interface, IInspectable **inner_interface, void **value)
{
    struct property_metadata_impl *impl;
    TRACE("This %p, default_value %p, callback %p\n", This, default_value, callback);
    if (!value) return E_POINTER;
    impl = create_property_metadata(default_value, callback);
    if (!impl) return E_OUTOFMEMORY;
    if (inner_interface)
    {
        *inner_interface = (IInspectable *)&impl->IPropertyMetadata_iface;
        IInspectable_AddRef(*inner_interface);
    }
    *value = &impl->IPropertyMetadata_iface;
    return S_OK;
}

static const struct {
    HRESULT (WINAPI *QueryInterface)(void *, REFIID, void **);
    ULONG   (WINAPI *AddRef)(void *);
    ULONG   (WINAPI *Release)(void *);
    HRESULT (WINAPI *GetIids)(void *, ULONG *, IID **);
    HRESULT (WINAPI *GetRuntimeClassName)(void *, HSTRING *);
    HRESULT (WINAPI *GetTrustLevel)(void *, TrustLevel *);
    HRESULT (WINAPI *CreateInstanceWithDefaultValue)(void *, IInspectable *, IInspectable *, IInspectable **, void **);
    HRESULT (WINAPI *CreateInstanceWithDefaultValueAndCallback)(void *, IInspectable *, void *, IInspectable *, IInspectable **, void **);
} pm_factory_vtbl = {
    pm_f_QueryInterface, pm_f_AddRef, pm_f_Release,
    pm_f_GetIids, pm_f_GetRuntimeClassName, pm_f_GetTrustLevel,
    pm_f_CreateInstanceWithDefaultValue,
    pm_f_CreateInstanceWithDefaultValueAndCallback
};

/* IPropertyMetadataStatics */
static HRESULT WINAPI pm_s_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return pm_fac_QueryInterface((IActivationFactory *)impl_from_pm_IStatics(This), riid, ppvObject);
}
static ULONG WINAPI pm_s_AddRef(void *This)
{
    return pm_fac_AddRef((IActivationFactory *)impl_from_pm_IStatics(This));
}
static ULONG WINAPI pm_s_Release(void *This)
{
    return pm_fac_Release((IActivationFactory *)impl_from_pm_IStatics(This));
}
static HRESULT WINAPI pm_s_GetIids(void *This, ULONG *iid_count, IID **iids)
{
    return pm_fac_GetIids((IActivationFactory *)impl_from_pm_IStatics(This), iid_count, iids);
}
static HRESULT WINAPI pm_s_GetRuntimeClassName(void *This, HSTRING *class_name)
{
    return pm_fac_GetRuntimeClassName((IActivationFactory *)impl_from_pm_IStatics(This), class_name);
}
static HRESULT WINAPI pm_s_GetTrustLevel(void *This, TrustLevel *trust_level)
{
    return pm_fac_GetTrustLevel((IActivationFactory *)impl_from_pm_IStatics(This), trust_level);
}

static HRESULT WINAPI pm_s_CreateWithDefaultValue(void *This, IInspectable *default_value, void **result)
{
    struct property_metadata_impl *impl;
    TRACE("default_value %p\n", default_value);
    if (!result) return E_POINTER;
    impl = create_property_metadata(default_value, NULL);
    if (!impl) return E_OUTOFMEMORY;
    *result = &impl->IPropertyMetadata_iface;
    return S_OK;
}

static HRESULT WINAPI pm_s_CreateWithDefaultValueAndCallback(void *This, IInspectable *default_value, void *callback, void **result)
{
    struct property_metadata_impl *impl;
    TRACE("default_value %p, callback %p\n", default_value, callback);
    if (!result) return E_POINTER;
    impl = create_property_metadata(default_value, callback);
    if (!impl) return E_OUTOFMEMORY;
    *result = &impl->IPropertyMetadata_iface;
    return S_OK;
}

static HRESULT WINAPI pm_s_CreateWithFactory(void *This, void *create_default_value_callback, void **result)
{
    struct property_metadata_impl *impl;
    if (!result) return E_POINTER;
    impl = create_property_metadata(NULL, NULL);
    if (!impl) return E_OUTOFMEMORY;
    *result = &impl->IPropertyMetadata_iface;
    return S_OK;
}

static HRESULT WINAPI pm_s_CreateWithFactoryAndCallback(void *This, void *create_default_value_callback, void *callback, void **result)
{
    struct property_metadata_impl *impl;
    if (!result) return E_POINTER;
    impl = create_property_metadata(NULL, callback);
    if (!impl) return E_OUTOFMEMORY;
    *result = &impl->IPropertyMetadata_iface;
    return S_OK;
}

static const struct {
    HRESULT (WINAPI *QueryInterface)(void *, REFIID, void **);
    ULONG   (WINAPI *AddRef)(void *);
    ULONG   (WINAPI *Release)(void *);
    HRESULT (WINAPI *GetIids)(void *, ULONG *, IID **);
    HRESULT (WINAPI *GetRuntimeClassName)(void *, HSTRING *);
    HRESULT (WINAPI *GetTrustLevel)(void *, TrustLevel *);
    HRESULT (WINAPI *CreateWithDefaultValue)(void *, IInspectable *, void **);
    HRESULT (WINAPI *CreateWithDefaultValueAndCallback)(void *, IInspectable *, void *, void **);
    HRESULT (WINAPI *CreateWithFactory)(void *, void *, void **);
    HRESULT (WINAPI *CreateWithFactoryAndCallback)(void *, void *, void *, void **);
} pm_statics_vtbl = {
    pm_s_QueryInterface, pm_s_AddRef, pm_s_Release,
    pm_s_GetIids, pm_s_GetRuntimeClassName, pm_s_GetTrustLevel,
    pm_s_CreateWithDefaultValue,
    pm_s_CreateWithDefaultValueAndCallback,
    pm_s_CreateWithFactory,
    pm_s_CreateWithFactoryAndCallback
};

static const void *redhawk_pm_fac_vtbl[16] = {
    pm_fac_QueryInterface, pm_fac_AddRef, pm_fac_Release,
    redhawk_safe_stub, redhawk_safe_stub, redhawk_safe_stub,
    pm_s_CreateWithDefaultValue, redhawk_safe_stub, redhawk_safe_stub,
    redhawk_safe_stub, redhawk_safe_stub, redhawk_safe_stub,
    redhawk_safe_stub, redhawk_safe_stub, redhawk_safe_stub,
    redhawk_safe_stub
};

static struct property_metadata_factory_impl default_pm_factory = {
    { &pm_activation_factory_vtbl },
    { &pm_factory_vtbl },
    { &pm_statics_vtbl },
    { redhawk_pm_fac_vtbl },
    1
};

IActivationFactory *property_metadata_factory = &default_pm_factory.IActivationFactory_iface;


/* -------------------------------------------------------------------------
 * DependencyProperty Instance
 * ------------------------------------------------------------------------- */
struct dependency_property_impl
{
    struct { const void *lpVtbl; } IDependencyProperty_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    HSTRING name;
    void *metadata;
    LONG ref;
};

#define impl_from_IDependencyProperty(ptr) ((struct dependency_property_impl *)((char *)(ptr) - offsetof(struct dependency_property_impl, IDependencyProperty_iface)))

static HRESULT WINAPI dp_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    struct dependency_property_impl *impl = impl_from_IDependencyProperty(This);
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("dp %p, riid %s\n", This, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IDependencyProperty))
    {
        *ppvObject = &impl->IDependencyProperty_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ComWrappers_internal))
    {
        *ppvObject = &impl->IRedhawkInterop_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    WARN("unhandled riid %s\n", debugstr_guid(riid));
    return E_NOINTERFACE;
}

static ULONG WINAPI dp_AddRef(void *This)
{
    struct dependency_property_impl *impl = impl_from_IDependencyProperty(This);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI dp_Release(void *This)
{
    struct dependency_property_impl *impl = impl_from_IDependencyProperty(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref)
    {
        if (impl->name) WindowsDeleteString(impl->name);
        if (impl->metadata) IInspectable_Release((IInspectable *)impl->metadata);
        free(impl);
    }
    return ref;
}

static HRESULT WINAPI dp_GetIids(void *This, ULONG *iid_count, IID **iids)
{
    FIXME("stub\n");
    return E_NOTIMPL;
}

static HRESULT WINAPI dp_GetRuntimeClassName(void *This, HSTRING *class_name)
{
    return WindowsCreateString(RuntimeClass_DependencyProperty, wcslen(RuntimeClass_DependencyProperty), class_name);
}

static HRESULT WINAPI dp_GetTrustLevel(void *This, TrustLevel *trust_level)
{
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI dp_GetMetadata(void *This, struct TypeName_raw type, void **result)
{
    struct dependency_property_impl *impl = impl_from_IDependencyProperty(This);
    TRACE("dp %p, metadata %p\n", impl, impl->metadata);
    if (!result) return E_POINTER;
    *result = impl->metadata;
    if (*result) IInspectable_AddRef((IInspectable *)*result);
    return S_OK;
}

static const struct {
    HRESULT (WINAPI *QueryInterface)(void *, REFIID, void **);
    ULONG   (WINAPI *AddRef)(void *);
    ULONG   (WINAPI *Release)(void *);
    HRESULT (WINAPI *GetIids)(void *, ULONG *, IID **);
    HRESULT (WINAPI *GetRuntimeClassName)(void *, HSTRING *);
    HRESULT (WINAPI *GetTrustLevel)(void *, TrustLevel *);
    HRESULT (WINAPI *GetMetadata)(void *, struct TypeName_raw, void **);
} dependency_property_vtbl = {
    dp_QueryInterface,
    dp_AddRef,
    dp_Release,
    dp_GetIids,
    dp_GetRuntimeClassName,
    dp_GetTrustLevel,
    dp_GetMetadata
};

static const void *redhawk_dp_vtbl[16] = {
    dp_QueryInterface, dp_AddRef, dp_Release,
    redhawk_safe_stub, redhawk_safe_stub, redhawk_safe_stub,
    dp_GetMetadata, redhawk_safe_stub, redhawk_safe_stub,
    redhawk_safe_stub, redhawk_safe_stub, redhawk_safe_stub,
    redhawk_safe_stub, redhawk_safe_stub, redhawk_safe_stub,
    redhawk_safe_stub
};

static struct dependency_property_impl *create_dependency_property(HSTRING name, void *metadata)
{
    struct dependency_property_impl *impl = calloc(1, sizeof(*impl));
    if (!impl) return NULL;
    impl->IDependencyProperty_iface.lpVtbl = &dependency_property_vtbl;
    impl->IRedhawkInterop_iface.lpVtbl = redhawk_dp_vtbl;
    impl->ref = 1;
    if (name) WindowsDuplicateString(name, &impl->name);
    impl->metadata = metadata;
    if (impl->metadata) IInspectable_AddRef((IInspectable *)impl->metadata);
    return impl;
}

/* -------------------------------------------------------------------------
 * DependencyProperty Factory & Statics
 * ------------------------------------------------------------------------- */
struct dependency_property_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } IDependencyPropertyStatics_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    LONG ref;
};

#define impl_from_dp_IActivationFactory(ptr) ((struct dependency_property_factory_impl *)((char *)(ptr) - offsetof(struct dependency_property_factory_impl, IActivationFactory_iface)))
#define impl_from_dp_IStatics(ptr) ((struct dependency_property_factory_impl *)((char *)(ptr) - offsetof(struct dependency_property_factory_impl, IDependencyPropertyStatics_iface)))

static HRESULT WINAPI dp_fac_QueryInterface(IActivationFactory *iface, REFIID riid, void **ppvObject)
{
    struct dependency_property_factory_impl *impl = impl_from_dp_IActivationFactory(iface);
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("dp_fac %p, riid %s\n", iface, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppvObject = &impl->IActivationFactory_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IDependencyPropertyStatics))
    {
        *ppvObject = &impl->IDependencyPropertyStatics_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ComWrappers_internal))
    {
        *ppvObject = &impl->IRedhawkInterop_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    WARN("unhandled riid %s\n", debugstr_guid(riid));
    return E_NOINTERFACE;
}

static ULONG WINAPI dp_fac_AddRef(IActivationFactory *iface)
{
    struct dependency_property_factory_impl *impl = impl_from_dp_IActivationFactory(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI dp_fac_Release(IActivationFactory *iface)
{
    struct dependency_property_factory_impl *impl = impl_from_dp_IActivationFactory(iface);
    return InterlockedDecrement(&impl->ref);
}

static HRESULT WINAPI dp_fac_GetIids(IActivationFactory *iface, ULONG *iid_count, IID **iids)
{
    FIXME("stub\n");
    return E_NOTIMPL;
}

static HRESULT WINAPI dp_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *class_name)
{
    return WindowsCreateString(RuntimeClass_DependencyProperty, wcslen(RuntimeClass_DependencyProperty), class_name);
}

static HRESULT WINAPI dp_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *trust_level)
{
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI dp_fac_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    if (!instance) return E_POINTER;
    *instance = NULL;
    return E_NOTIMPL;
}

static const struct IActivationFactoryVtbl dp_activation_factory_vtbl = {
    dp_fac_QueryInterface,
    dp_fac_AddRef,
    dp_fac_Release,
    dp_fac_GetIids,
    dp_fac_GetRuntimeClassName,
    dp_fac_GetTrustLevel,
    dp_fac_ActivateInstance
};

/* IDependencyPropertyStatics */
static HRESULT WINAPI dps_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return dp_fac_QueryInterface((IActivationFactory *)impl_from_dp_IStatics(This), riid, ppvObject);
}
static ULONG WINAPI dps_AddRef(void *This)
{
    return dp_fac_AddRef((IActivationFactory *)impl_from_dp_IStatics(This));
}
static ULONG WINAPI dps_Release(void *This)
{
    return dp_fac_Release((IActivationFactory *)impl_from_dp_IStatics(This));
}
static HRESULT WINAPI dps_GetIids(void *This, ULONG *iid_count, IID **iids)
{
    return dp_fac_GetIids((IActivationFactory *)impl_from_dp_IStatics(This), iid_count, iids);
}
static HRESULT WINAPI dps_GetRuntimeClassName(void *This, HSTRING *class_name)
{
    return dp_fac_GetRuntimeClassName((IActivationFactory *)impl_from_dp_IStatics(This), class_name);
}
static HRESULT WINAPI dps_GetTrustLevel(void *This, TrustLevel *trust_level)
{
    return dp_fac_GetTrustLevel((IActivationFactory *)impl_from_dp_IStatics(This), trust_level);
}

static HRESULT WINAPI dps_get_UnsetValue(void *This, IInspectable **value)
{
    TRACE("get_UnsetValue\n");
    if (!value) return E_POINTER;
    *value = NULL;
    return S_OK;
}

static HRESULT WINAPI dps_Register(void *This, HSTRING name, struct TypeName_raw prop_type, struct TypeName_raw owner_type, void *type_metadata, void **result)
{
    struct dependency_property_impl *impl;
    TRACE("name %s, metadata %p\n", debugstr_hstring(name), type_metadata);
    if (!result) return E_POINTER;
    impl = create_dependency_property(name, type_metadata);
    if (!impl) return E_OUTOFMEMORY;
    *result = &impl->IDependencyProperty_iface;
    return S_OK;
}

static HRESULT WINAPI dps_RegisterAttached(void *This, HSTRING name, struct TypeName_raw prop_type, struct TypeName_raw owner_type, void *default_metadata, void **result)
{
    struct dependency_property_impl *impl;
    TRACE("attached name %s, metadata %p\n", debugstr_hstring(name), default_metadata);
    if (!result) return E_POINTER;
    impl = create_dependency_property(name, default_metadata);
    if (!impl) return E_OUTOFMEMORY;
    *result = &impl->IDependencyProperty_iface;
    return S_OK;
}

static const struct {
    HRESULT (WINAPI *QueryInterface)(void *, REFIID, void **);
    ULONG   (WINAPI *AddRef)(void *);
    ULONG   (WINAPI *Release)(void *);
    HRESULT (WINAPI *GetIids)(void *, ULONG *, IID **);
    HRESULT (WINAPI *GetRuntimeClassName)(void *, HSTRING *);
    HRESULT (WINAPI *GetTrustLevel)(void *, TrustLevel *);
    HRESULT (WINAPI *get_UnsetValue)(void *, IInspectable **);
    HRESULT (WINAPI *Register)(void *, HSTRING, struct TypeName_raw, struct TypeName_raw, void *, void **);
    HRESULT (WINAPI *RegisterAttached)(void *, HSTRING, struct TypeName_raw, struct TypeName_raw, void *, void **);
} dp_statics_vtbl = {
    dps_QueryInterface, dps_AddRef, dps_Release,
    dps_GetIids, dps_GetRuntimeClassName, dps_GetTrustLevel,
    dps_get_UnsetValue,
    dps_Register,
    dps_RegisterAttached
};

static const void *redhawk_dp_fac_vtbl[16] = {
    dp_fac_QueryInterface, dp_fac_AddRef, dp_fac_Release,
    redhawk_safe_stub, redhawk_safe_stub, redhawk_safe_stub,
    dps_get_UnsetValue, dps_Register, dps_RegisterAttached,
    redhawk_safe_stub, redhawk_safe_stub, redhawk_safe_stub,
    redhawk_safe_stub, redhawk_safe_stub, redhawk_safe_stub,
    redhawk_safe_stub
};

static struct dependency_property_factory_impl default_dp_factory = {
    { &dp_activation_factory_vtbl },
    { &dp_statics_vtbl },
    { redhawk_dp_fac_vtbl },
    1
};

IActivationFactory *dependency_property_factory = &default_dp_factory.IActivationFactory_iface;


/* -------------------------------------------------------------------------
 * DependencyObject Instance
 * ------------------------------------------------------------------------- */
struct dependency_object_impl
{
    struct { const void *lpVtbl; } IDependencyObject_iface;
    struct { const void *lpVtbl; } IDependencyObject2_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    LONG ref;
};

#define impl_from_IDependencyObject(ptr) ((struct dependency_object_impl *)((char *)(ptr) - offsetof(struct dependency_object_impl, IDependencyObject_iface)))
#define impl_from_IDependencyObject2(ptr) ((struct dependency_object_impl *)((char *)(ptr) - offsetof(struct dependency_object_impl, IDependencyObject2_iface)))

static HRESULT WINAPI do_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    struct dependency_object_impl *impl = impl_from_IDependencyObject(This);
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("do %p, riid %s\n", This, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IDependencyObject))
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

    if (IsEqualGUID(riid, &IID_ComWrappers_internal))
    {
        *ppvObject = &impl->IRedhawkInterop_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    WARN("unhandled riid %s\n", debugstr_guid(riid));
    return E_NOINTERFACE;
}

static ULONG WINAPI do_AddRef(void *This)
{
    struct dependency_object_impl *impl = impl_from_IDependencyObject(This);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI do_Release(void *This)
{
    struct dependency_object_impl *impl = impl_from_IDependencyObject(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI do_GetIids(void *This, ULONG *iid_count, IID **iids)
{
    FIXME("stub\n");
    return E_NOTIMPL;
}

static HRESULT WINAPI do_GetRuntimeClassName(void *This, HSTRING *class_name)
{
    return WindowsCreateString(RuntimeClass_DependencyObject, wcslen(RuntimeClass_DependencyObject), class_name);
}

static HRESULT WINAPI do_GetTrustLevel(void *This, TrustLevel *trust_level)
{
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI do_GetValue(void *This, void *dp, IInspectable **result)
{
    struct dependency_property_impl *prop = (struct dependency_property_impl *)dp;
    TRACE("This %p, dp %p\n", This, dp);
    if (!result) return E_POINTER;
    *result = NULL;
    if (prop && prop->metadata)
    {
        struct property_metadata_impl *meta = (struct property_metadata_impl *)prop->metadata;
        if (meta->default_value)
        {
            *result = meta->default_value;
            IInspectable_AddRef(*result);
        }
    }
    return S_OK;
}

static HRESULT WINAPI do_SetValue(void *This, void *dp, IInspectable *value)
{
    TRACE("This %p, dp %p, value %p\n", This, dp, value);
    return S_OK;
}

static HRESULT WINAPI do_ClearValue(void *This, void *dp)
{
    TRACE("This %p, dp %p\n", This, dp);
    return S_OK;
}

static HRESULT WINAPI do_ReadLocalValue(void *This, void *dp, IInspectable **result)
{
    TRACE("This %p, dp %p\n", This, dp);
    if (!result) return E_POINTER;
    *result = NULL;
    return S_OK;
}

static HRESULT WINAPI do_GetAnimationBaseValue(void *This, void *dp, IInspectable **result)
{
    TRACE("This %p, dp %p\n", This, dp);
    if (!result) return E_POINTER;
    *result = NULL;
    return S_OK;
}

static HRESULT WINAPI do_get_Dispatcher(void *This, void **value)
{
    TRACE("This %p\n", This);
    if (!value) return E_POINTER;
    *value = NULL;
    return S_OK;
}

static const struct {
    HRESULT (WINAPI *QueryInterface)(void *, REFIID, void **);
    ULONG   (WINAPI *AddRef)(void *);
    ULONG   (WINAPI *Release)(void *);
    HRESULT (WINAPI *GetIids)(void *, ULONG *, IID **);
    HRESULT (WINAPI *GetRuntimeClassName)(void *, HSTRING *);
    HRESULT (WINAPI *GetTrustLevel)(void *, TrustLevel *);
    HRESULT (WINAPI *GetValue)(void *, void *, IInspectable **);
    HRESULT (WINAPI *SetValue)(void *, void *, IInspectable *);
    HRESULT (WINAPI *ClearValue)(void *, void *);
    HRESULT (WINAPI *ReadLocalValue)(void *, void *, IInspectable **);
    HRESULT (WINAPI *GetAnimationBaseValue)(void *, void *, IInspectable **);
    HRESULT (WINAPI *get_Dispatcher)(void *, void **);
} dependency_object_vtbl = {
    do_QueryInterface, do_AddRef, do_Release,
    do_GetIids, do_GetRuntimeClassName, do_GetTrustLevel,
    do_GetValue, do_SetValue, do_ClearValue,
    do_ReadLocalValue, do_GetAnimationBaseValue,
    do_get_Dispatcher
};

/* IDependencyObject2 */
static HRESULT WINAPI do2_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return do_QueryInterface((void *)impl_from_IDependencyObject2(This), riid, ppvObject);
}
static ULONG WINAPI do2_AddRef(void *This)
{
    return do_AddRef((void *)impl_from_IDependencyObject2(This));
}
static ULONG WINAPI do2_Release(void *This)
{
    return do_Release((void *)impl_from_IDependencyObject2(This));
}
static HRESULT WINAPI do2_GetIids(void *This, ULONG *iid_count, IID **iids)
{
    return do_GetIids((void *)impl_from_IDependencyObject2(This), iid_count, iids);
}
static HRESULT WINAPI do2_GetRuntimeClassName(void *This, HSTRING *class_name)
{
    return do_GetRuntimeClassName((void *)impl_from_IDependencyObject2(This), class_name);
}
static HRESULT WINAPI do2_GetTrustLevel(void *This, TrustLevel *trust_level)
{
    return do_GetTrustLevel((void *)impl_from_IDependencyObject2(This), trust_level);
}

static HRESULT WINAPI do2_RegisterPropertyChangedCallback(void *This, void *dp, void *callback, INT64 *result)
{
    TRACE("RegisterPropertyChangedCallback\n");
    if (!result) return E_POINTER;
    *result = 1;
    return S_OK;
}

static HRESULT WINAPI do2_UnregisterPropertyChangedCallback(void *This, void *dp, INT64 token)
{
    TRACE("UnregisterPropertyChangedCallback token %lld\n", token);
    return S_OK;
}

static const struct {
    HRESULT (WINAPI *QueryInterface)(void *, REFIID, void **);
    ULONG   (WINAPI *AddRef)(void *);
    ULONG   (WINAPI *Release)(void *);
    HRESULT (WINAPI *GetIids)(void *, ULONG *, IID **);
    HRESULT (WINAPI *GetRuntimeClassName)(void *, HSTRING *);
    HRESULT (WINAPI *GetTrustLevel)(void *, TrustLevel *);
    HRESULT (WINAPI *RegisterPropertyChangedCallback)(void *, void *, void *, INT64 *);
    HRESULT (WINAPI *UnregisterPropertyChangedCallback)(void *, void *, INT64);
} dependency_object2_vtbl = {
    do2_QueryInterface, do2_AddRef, do2_Release,
    do2_GetIids, do2_GetRuntimeClassName, do2_GetTrustLevel,
    do2_RegisterPropertyChangedCallback,
    do2_UnregisterPropertyChangedCallback
};

static const void *redhawk_do_vtbl[16] = {
    do_QueryInterface, do_AddRef, do_Release,
    redhawk_safe_stub, redhawk_safe_stub, redhawk_safe_stub,
    do_GetValue, do_SetValue, do_ClearValue,
    do_ReadLocalValue, do_GetAnimationBaseValue, do_get_Dispatcher,
    redhawk_safe_stub, redhawk_safe_stub, redhawk_safe_stub,
    redhawk_safe_stub
};

static struct dependency_object_impl *create_dependency_object(void)
{
    struct dependency_object_impl *impl = calloc(1, sizeof(*impl));
    if (!impl) return NULL;
    impl->IDependencyObject_iface.lpVtbl = &dependency_object_vtbl;
    impl->IDependencyObject2_iface.lpVtbl = &dependency_object2_vtbl;
    impl->IRedhawkInterop_iface.lpVtbl = redhawk_do_vtbl;
    impl->ref = 1;
    return impl;
}

/* -------------------------------------------------------------------------
 * DependencyObject Factory
 * ------------------------------------------------------------------------- */
struct dependency_object_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } IDependencyObjectFactory_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    LONG ref;
};

#define impl_from_dofac_IActivationFactory(ptr) ((struct dependency_object_factory_impl *)((char *)(ptr) - offsetof(struct dependency_object_factory_impl, IActivationFactory_iface)))
#define impl_from_dofac_IFactory(ptr) ((struct dependency_object_factory_impl *)((char *)(ptr) - offsetof(struct dependency_object_factory_impl, IDependencyObjectFactory_iface)))

static HRESULT WINAPI do_fac_QueryInterface(IActivationFactory *iface, REFIID riid, void **ppvObject)
{
    struct dependency_object_factory_impl *impl = impl_from_dofac_IActivationFactory(iface);
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("do_fac %p, riid %s\n", iface, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppvObject = &impl->IActivationFactory_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IDependencyObjectFactory))
    {
        *ppvObject = &impl->IDependencyObjectFactory_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ComWrappers_internal))
    {
        *ppvObject = &impl->IRedhawkInterop_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    WARN("unhandled riid %s\n", debugstr_guid(riid));
    return E_NOINTERFACE;
}

static ULONG WINAPI do_fac_AddRef(IActivationFactory *iface)
{
    struct dependency_object_factory_impl *impl = impl_from_dofac_IActivationFactory(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI do_fac_Release(IActivationFactory *iface)
{
    struct dependency_object_factory_impl *impl = impl_from_dofac_IActivationFactory(iface);
    return InterlockedDecrement(&impl->ref);
}

static HRESULT WINAPI do_fac_GetIids(IActivationFactory *iface, ULONG *iid_count, IID **iids)
{
    FIXME("stub\n");
    return E_NOTIMPL;
}

static HRESULT WINAPI do_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *class_name)
{
    return WindowsCreateString(RuntimeClass_DependencyObject, wcslen(RuntimeClass_DependencyObject), class_name);
}

static HRESULT WINAPI do_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *trust_level)
{
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI do_fac_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    struct dependency_object_impl *impl;
    TRACE("iface %p, instance %p\n", iface, instance);
    if (!instance) return E_POINTER;
    impl = create_dependency_object();
    if (!impl) return E_OUTOFMEMORY;
    *instance = (IInspectable *)&impl->IDependencyObject_iface;
    return S_OK;
}

static const struct IActivationFactoryVtbl do_activation_factory_vtbl = {
    do_fac_QueryInterface,
    do_fac_AddRef,
    do_fac_Release,
    do_fac_GetIids,
    do_fac_GetRuntimeClassName,
    do_fac_GetTrustLevel,
    do_fac_ActivateInstance
};

/* IDependencyObjectFactory */
static HRESULT WINAPI do_f_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return do_fac_QueryInterface((IActivationFactory *)impl_from_dofac_IFactory(This), riid, ppvObject);
}
static ULONG WINAPI do_f_AddRef(void *This)
{
    return do_fac_AddRef((IActivationFactory *)impl_from_dofac_IFactory(This));
}
static ULONG WINAPI do_f_Release(void *This)
{
    return do_fac_Release((IActivationFactory *)impl_from_dofac_IFactory(This));
}
static HRESULT WINAPI do_f_GetIids(void *This, ULONG *iid_count, IID **iids)
{
    return do_fac_GetIids((IActivationFactory *)impl_from_dofac_IFactory(This), iid_count, iids);
}
static HRESULT WINAPI do_f_GetRuntimeClassName(void *This, HSTRING *class_name)
{
    return do_fac_GetRuntimeClassName((IActivationFactory *)impl_from_dofac_IFactory(This), class_name);
}
static HRESULT WINAPI do_f_GetTrustLevel(void *This, TrustLevel *trust_level)
{
    return do_fac_GetTrustLevel((IActivationFactory *)impl_from_dofac_IFactory(This), trust_level);
}

static HRESULT WINAPI do_f_CreateInstance(void *This, IInspectable *base_interface, IInspectable **inner_interface, void **value)
{
    struct dependency_object_impl *impl;
    TRACE("This %p, base_interface %p\n", This, base_interface);
    if (!value) return E_POINTER;
    impl = create_dependency_object();
    if (!impl) return E_OUTOFMEMORY;
    if (inner_interface)
    {
        *inner_interface = (IInspectable *)&impl->IDependencyObject_iface;
        IInspectable_AddRef(*inner_interface);
    }
    *value = &impl->IDependencyObject_iface;
    return S_OK;
}

static const struct {
    HRESULT (WINAPI *QueryInterface)(void *, REFIID, void **);
    ULONG   (WINAPI *AddRef)(void *);
    ULONG   (WINAPI *Release)(void *);
    HRESULT (WINAPI *GetIids)(void *, ULONG *, IID **);
    HRESULT (WINAPI *GetRuntimeClassName)(void *, HSTRING *);
    HRESULT (WINAPI *GetTrustLevel)(void *, TrustLevel *);
    HRESULT (WINAPI *CreateInstance)(void *, IInspectable *, IInspectable **, void **);
} do_factory_vtbl = {
    do_f_QueryInterface, do_f_AddRef, do_f_Release,
    do_f_GetIids, do_f_GetRuntimeClassName, do_f_GetTrustLevel,
    do_f_CreateInstance
};

static const void *redhawk_do_fac_vtbl[16] = {
    do_fac_QueryInterface, do_fac_AddRef, do_fac_Release,
    redhawk_safe_stub, redhawk_safe_stub, redhawk_safe_stub,
    do_f_CreateInstance, redhawk_safe_stub, redhawk_safe_stub,
    redhawk_safe_stub, redhawk_safe_stub, redhawk_safe_stub,
    redhawk_safe_stub, redhawk_safe_stub, redhawk_safe_stub,
    redhawk_safe_stub
};

static struct dependency_object_factory_impl default_do_factory = {
    { &do_activation_factory_vtbl },
    { &do_factory_vtbl },
    { redhawk_do_fac_vtbl },
    1
};

IActivationFactory *dependency_object_factory = &default_do_factory.IActivationFactory_iface;
