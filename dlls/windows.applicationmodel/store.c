/* WinRT Windows.Services.Store Implementation
 *
 * Copyright (C) 2026 Julian / Xodus Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

/* Windows.Services.Store GUIDs */
static const GUID IID_IStoreContextStatics          = {0x9c06ee5f,0x15c0,0x4e72,{0x93,0x30,0xd6,0x19,0x1c,0xeb,0xd1,0x9c}};
static const GUID IID_IStoreContext                 = {0xac98b6be,0xf4fd,0x4912,{0xba,0xbd,0x50,0x35,0xe5,0xe8,0xbc,0xab}};
static const GUID IID_IStoreContext2                = {0x18bc54da,0x7bd9,0x452c,{0x91,0x16,0x3b,0xbd,0x06,0xff,0xc6,0x3a}};
static const GUID IID_IStoreContext3                = {0xe26226ca,0x1a01,0x4730,{0x85,0xa6,0xec,0xc8,0x96,0xe4,0xae,0x38}};
static const GUID IID_IStoreContext4                = {0xaf9c6f69,0xbea1,0x4bf4,{0x8e,0x74,0xae,0x03,0xe2,0x06,0xc6,0xb0}};
static const GUID IID_IStoreProductResult           = {0xb7674f73,0x3c87,0x4ee1,{0x82,0x01,0xf4,0x28,0x35,0x9b,0xd3,0xaf}};
static const GUID IID_IStoreProduct                 = {0x320e2c52,0xd760,0x450a,{0xa4,0x2b,0x67,0xd1,0xe9,0x01,0xac,0x90}};
static const GUID IID_IStoreAppLicense              = {0xf389f9de,0x73c0,0x45ce,{0x9b,0xab,0xb2,0xfe,0x3e,0x5e,0xaf,0xd3}};
static const GUID IID_IStoreRateAndReviewResult     = {0x9d209d56,0xa6b5,0x4121,{0x9b,0x61,0xee,0x6d,0x0f,0xbd,0xbd,0xbb}};
static const GUID IID_IStoreProductPagedQueryResult  = {0xc92718c5,0x4dd5,0x4869,{0xa4,0x62,0xec,0xc6,0x87,0x2e,0x43,0xc5}};
static const GUID IID_IStoreProductQueryResult       = {0xd805e674,0x3656,0x4279,{0xa7,0x8b,0x37,0x29,0x9f,0x51,0x66,0x24}};
static const GUID IID_IStorePackageUpdate           = {0x1408e327,0x0467,0x4589,{0xa2,0x46,0xb7,0x41,0x64,0x62,0x78,0x50}};
static const GUID IID_IVectorView                   = {0xbbe1fa4c,0xb0e3,0x4583,{0xba,0xef,0x1a,0x1b,0x4e,0x5f,0x5f,0x66}};
static const GUID IID_IVectorView_StorePackageUpdate = {0x971c3ea6,0x4388,0x5a38,{0xae,0x13,0x49,0x29,0xb6,0xd6,0xd7,0x80}};
static const GUID IID_IIterable                     = {0xfaa585ea,0x6214,0x4217,{0xaf,0xda,0x07,0x0b,0xef,0x65,0x23,0x41}};
static const GUID IID_IIterable_StorePackageUpdate   = {0xcb5cc29e,0x4475,0x5e19,{0xba,0xc4,0x46,0x04,0x9b,0x50,0xc6,0xb1}};
static const GUID IID_IIterator                     = {0x6a1d6c07,0x076d,0x49f2,{0x83,0x14,0xf5,0x2c,0x9c,0x9a,0x83,0x31}};
static const GUID IID_IIterator_StorePackageUpdate   = {0x3acd0e4e,0x5133,0x54df,{0x8b,0xfd,0x11,0x02,0x2d,0xcd,0x5f,0xf0}};
static const GUID IID_IMapView_StoreProductMap       = {0xdbaac6e5,0x61a4,0x5c88,{0xb5,0xd8,0x3a,0x3e,0x16,0x1c,0x3e,0x4a}};
static const GUID IID_IIterable_StoreProductMap      = {0x78a33722,0xabfb,0x57c0,{0x85,0x3f,0x56,0x16,0xa3,0xab,0x8d,0x57}};
static const GUID IID_IIterator_StoreProductMap      = {0x0edb142c,0x9d04,0x532b,{0x81,0xe9,0x4c,0x84,0xab,0x09,0xb3,0x4b}};

/* CLR / WinRT interop probe GUIDs that must be rejected with E_NOINTERFACE */
static const GUID IID_IWeakReferenceSource_CsWinRT = {0x11d3b13a,0x180e,0x4789,{0xa8,0xbe,0x77,0x12,0x88,0x28,0x93,0xe6}};
static const GUID IID_ILanguageExceptionErrorInfo   = {0x5c13e51c,0x4f32,0x4726,{0xa3,0xfd,0xf3,0xed,0xd6,0x3d,0xa3,0xa0}};
static const GUID IID_IWeakReferenceSource         = {0x00000038,0x0000,0x0000,{0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}};
static const GUID IID_IWeakReference               = {0x00000037,0x0000,0x0000,{0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}};
/* IID_IMarshal is defined in objidlbase.h */
static const GUID IID_ICustomQueryInterface        = {0x49691c54,0x7d41,0x4c7e,{0x85,0x5e,0x04,0x9f,0x93,0xb5,0xa4,0x42}};

static inline void set_out_ptr(void *p, void *val)
{
    if (p && (ULONG_PTR)p > 0x10000 && !IsBadWritePtr(p, sizeof(void *)))
        *(void **)p = val;
}

enum store_async_type
{
    STORE_ASYNC_DEFAULT,
    STORE_ASYNC_APP_LICENSE,
    STORE_ASYNC_QUERY_RESULT,
    STORE_ASYNC_PAGED_QUERY_RESULT,
    STORE_ASYNC_PRODUCT_RESULT,
    STORE_ASYNC_RATE_RESULT,
    STORE_ASYNC_VECTOR_VIEW,
};

static HRESULT store_async_op_create(enum store_async_type type, void **out);

/* ========================================================================= */
/* Unified Store result stub                                                 */
/* ========================================================================= */
struct store_result
{
    IInspectable IStoreProductResult_iface;
    IInspectable IStoreProduct_iface;
    IInspectable IStoreRateAndReviewResult_iface;
    IInspectable IStoreAppLicense_iface;
    IInspectable IVectorView_iface;
    IInspectable IMapView_iface;
    IInspectable IIterable_iface;
    IInspectable IStoreQueryResult_iface;
    IInspectable IStoreProductPagedQueryResult_iface;
    LONG ref;
};

static inline struct store_result *impl_from_store_result_product_result(void *iface)
{
    return CONTAINING_RECORD(iface, struct store_result, IStoreProductResult_iface);
}

static inline struct store_result *impl_from_store_result_product(void *iface)
{
    return CONTAINING_RECORD(iface, struct store_result, IStoreProduct_iface);
}

static inline struct store_result *impl_from_store_result_rate(void *iface)
{
    return CONTAINING_RECORD(iface, struct store_result, IStoreRateAndReviewResult_iface);
}

static inline struct store_result *impl_from_store_result_license(void *iface)
{
    return CONTAINING_RECORD(iface, struct store_result, IStoreAppLicense_iface);
}

static inline struct store_result *impl_from_store_result_vector(void *iface)
{
    return CONTAINING_RECORD(iface, struct store_result, IVectorView_iface);
}

static inline struct store_result *impl_from_store_result_map(void *iface)
{
    return CONTAINING_RECORD(iface, struct store_result, IMapView_iface);
}

static inline struct store_result *impl_from_store_result_iterable(void *iface)
{
    return CONTAINING_RECORD(iface, struct store_result, IIterable_iface);
}

static inline struct store_result *impl_from_store_result_query(void *iface)
{
    return CONTAINING_RECORD(iface, struct store_result, IStoreQueryResult_iface);
}

static inline struct store_result *impl_from_store_result_paged_query(void *iface)
{
    return CONTAINING_RECORD(iface, struct store_result, IStoreProductPagedQueryResult_iface);
}

static HRESULT store_result_query_interface_internal(struct store_result *impl, REFIID iid, void **out)
{
    TRACE("impl %p, iid %s, out %p.\n", impl, debugstr_guid(iid), out);
    if (!out) return E_INVALIDARG;

    if (IsEqualIID(iid, &IID_IWeakReferenceSource_CsWinRT) ||
        IsEqualIID(iid, &IID_ILanguageExceptionErrorInfo) ||
        IsEqualIID(iid, &IID_IWeakReferenceSource) ||
        IsEqualIID(iid, &IID_IWeakReference) ||
        IsEqualIID(iid, &IID_IMarshal) ||
        IsEqualIID(iid, &IID_ICustomQueryInterface))
    {
        *out = NULL;
        return E_NOINTERFACE;
    }

    if (IsEqualIID(iid, &IID_IUnknown) ||
        IsEqualIID(iid, &IID_IInspectable) ||
        IsEqualIID(iid, &IID_IAgileObject) ||
        IsEqualIID(iid, &IID_IStoreProductResult))
    {
        *out = &impl->IStoreProductResult_iface;
    }
    else if (IsEqualIID(iid, &IID_IStoreProduct))
    {
        *out = &impl->IStoreProduct_iface;
    }
    else if (IsEqualIID(iid, &IID_IStoreRateAndReviewResult))
    {
        *out = &impl->IStoreRateAndReviewResult_iface;
    }
    else if (IsEqualIID(iid, &IID_IStoreAppLicense))
    {
        *out = &impl->IStoreAppLicense_iface;
    }
    else if (IsEqualIID(iid, &IID_IStoreProductPagedQueryResult))
    {
        *out = &impl->IStoreProductPagedQueryResult_iface;
    }
    else if (IsEqualIID(iid, &IID_IStoreProductQueryResult))
    {
        *out = &impl->IStoreQueryResult_iface;
    }
    else if (IsEqualIID(iid, &IID_IVectorView) ||
             IsEqualIID(iid, &IID_IVectorView_StorePackageUpdate))
    {
        *out = &impl->IVectorView_iface;
    }
    else if (IsEqualIID(iid, &IID_IMapView_StoreProductMap))
    {
        *out = &impl->IMapView_iface;
    }
    else if (IsEqualIID(iid, &IID_IIterable) ||
             IsEqualIID(iid, &IID_IIterable_StorePackageUpdate) ||
             IsEqualIID(iid, &IID_IIterable_StoreProductMap))
    {
        *out = &impl->IIterable_iface;
    }
    else
    {
        WARN("unsupported iid %s\n", debugstr_guid(iid));
        *out = NULL;
        return E_NOINTERFACE;
    }

    IUnknown_AddRef((IUnknown *)*out);
    return S_OK;
}

/* 1. IStoreProductResult vtbl methods */
static HRESULT WINAPI store_result_product_result_QueryInterface(void *iface, REFIID iid, void **out)
{
    return store_result_query_interface_internal(impl_from_store_result_product_result(iface), iid, out);
}

static ULONG WINAPI store_result_product_result_AddRef(void *iface)
{
    struct store_result *impl = impl_from_store_result_product_result(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI store_result_product_result_Release(void *iface)
{
    struct store_result *impl = impl_from_store_result_product_result(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI store_result_GetIids(void *iface, ULONG *iid_count, IID **iids)
{
    if (!iid_count || !iids) return E_INVALIDARG;
    *iid_count = 0;
    *iids = NULL;
    return S_OK;
}

static HRESULT WINAPI store_result_GetRuntimeClassName(void *iface, HSTRING *class_name)
{
    static const WCHAR name[] = L"Windows.Services.Store.StoreProductResult";
    return WindowsCreateString(name, ARRAY_SIZE(name) - 1, class_name);
}

static HRESULT WINAPI store_result_GetTrustLevel(void *iface, TrustLevel *trust_level)
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI store_result_product_result_get_Product(void *iface, void **value)
{
    struct store_result *impl = impl_from_store_result_product_result(iface);
    TRACE("iface %p, value %p.\n", iface, value);
    if (!value) return E_INVALIDARG;
    *value = &impl->IStoreProduct_iface;
    InterlockedIncrement(&impl->ref);
    return S_OK;
}

static HRESULT WINAPI store_result_product_result_get_ExtendedError(void *iface, HRESULT *value)
{
    TRACE("iface %p, value %p.\n", iface, value);
    if (!value) return E_INVALIDARG;
    *value = S_OK;
    return S_OK;
}

static const void *store_result_product_result_vtbl[] =
{
    store_result_product_result_QueryInterface,
    store_result_product_result_AddRef,
    store_result_product_result_Release,
    store_result_GetIids,
    store_result_GetRuntimeClassName,
    store_result_GetTrustLevel,
    /* IStoreProductResult methods (slots 6..7) */
    store_result_product_result_get_Product,
    store_result_product_result_get_ExtendedError,
};

/* 2. IStoreProduct vtbl methods */
static HRESULT WINAPI store_result_product_QueryInterface(void *iface, REFIID iid, void **out)
{
    return store_result_query_interface_internal(impl_from_store_result_product(iface), iid, out);
}

static ULONG WINAPI store_result_product_AddRef(void *iface)
{
    struct store_result *impl = impl_from_store_result_product(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI store_result_product_Release(void *iface)
{
    struct store_result *impl = impl_from_store_result_product(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI store_result_product_get_StoreId(void *iface, HSTRING *value)
{
    if (!value) return E_INVALIDARG;
    return WindowsCreateString(L"9NKSQGP7F2NH", 12, value);
}

static HRESULT WINAPI store_result_product_get_Language(void *iface, HSTRING *value)
{
    if (!value) return E_INVALIDARG;
    return WindowsCreateString(L"en", 2, value);
}

static HRESULT WINAPI store_result_product_get_Title(void *iface, HSTRING *value)
{
    if (!value) return E_INVALIDARG;
    return WindowsCreateString(L"Application", 11, value);
}

static HRESULT WINAPI store_result_product_get_Description(void *iface, HSTRING *value)
{
    if (!value) return E_INVALIDARG;
    return WindowsCreateString(L"Application Product", 19, value);
}

static HRESULT WINAPI store_result_product_get_ProductKind(void *iface, HSTRING *value)
{
    if (!value) return E_INVALIDARG;
    return WindowsCreateString(L"Application", 11, value);
}

static HRESULT WINAPI store_result_product_get_HasDigitalDownload(void *iface, boolean *value)
{
    if (!value) return E_INVALIDARG;
    *value = 1;
    return S_OK;
}

static HRESULT WINAPI store_result_product_get_Keywords(void *iface, void **value)
{
    struct store_result *impl = impl_from_store_result_product(iface);
    if (!value) return E_INVALIDARG;
    *value = &impl->IVectorView_iface;
    InterlockedIncrement(&impl->ref);
    return S_OK;
}

static HRESULT WINAPI store_result_product_get_Images(void *iface, void **value)
{
    struct store_result *impl = impl_from_store_result_product(iface);
    if (!value) return E_INVALIDARG;
    *value = &impl->IVectorView_iface;
    InterlockedIncrement(&impl->ref);
    return S_OK;
}

static HRESULT WINAPI store_result_product_get_Videos(void *iface, void **value)
{
    struct store_result *impl = impl_from_store_result_product(iface);
    if (!value) return E_INVALIDARG;
    *value = &impl->IVectorView_iface;
    InterlockedIncrement(&impl->ref);
    return S_OK;
}

static HRESULT WINAPI store_result_product_get_ExtendedJsonData(void *iface, HSTRING *value)
{
    if (!value) return E_INVALIDARG;
    return WindowsCreateString(L"{}", 2, value);
}

static HRESULT WINAPI store_result_product_get_LinkUri(void *iface, void **value)
{
    if (value) *value = NULL;
    return S_OK;
}

static HRESULT WINAPI store_result_product_get_Skus(void *iface, void **value)
{
    struct store_result *impl = impl_from_store_result_product(iface);
    if (!value) return E_INVALIDARG;
    *value = &impl->IVectorView_iface;
    InterlockedIncrement(&impl->ref);
    return S_OK;
}

static HRESULT WINAPI store_result_product_get_Price(void *iface, void **value)
{
    if (value) *value = NULL;
    return S_OK;
}

static HRESULT WINAPI store_result_product_get_IsInUserCollection(void *iface, boolean *value)
{
    if (!value) return E_INVALIDARG;
    *value = 1;
    return S_OK;
}

static const void *store_result_product_vtbl[] =
{
    store_result_product_QueryInterface,
    store_result_product_AddRef,
    store_result_product_Release,
    store_result_GetIids,
    store_result_GetRuntimeClassName,
    store_result_GetTrustLevel,
    /* IStoreProduct methods (slots 6..19) */
    store_result_product_get_StoreId,
    store_result_product_get_Language,
    store_result_product_get_Title,
    store_result_product_get_Description,
    store_result_product_get_ProductKind,
    store_result_product_get_HasDigitalDownload,
    store_result_product_get_Keywords,
    store_result_product_get_Images,
    store_result_product_get_Videos,
    store_result_product_get_ExtendedJsonData,
    store_result_product_get_LinkUri,
    store_result_product_get_Skus,
    store_result_product_get_Price,
    store_result_product_get_IsInUserCollection,
};

/* 3. IStoreRateAndReviewResult vtbl methods */
static HRESULT WINAPI store_result_rate_QueryInterface(void *iface, REFIID iid, void **out)
{
    return store_result_query_interface_internal(impl_from_store_result_rate(iface), iid, out);
}

static ULONG WINAPI store_result_rate_AddRef(void *iface)
{
    struct store_result *impl = impl_from_store_result_rate(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI store_result_rate_Release(void *iface)
{
    struct store_result *impl = impl_from_store_result_rate(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI store_result_rate_get_ExtendedError(void *iface, HRESULT *value)
{
    if (!value) return E_INVALIDARG;
    *value = S_OK;
    return S_OK;
}

static HRESULT WINAPI store_result_rate_get_ExtendedJsonData(void *iface, HSTRING *value)
{
    if (!value) return E_INVALIDARG;
    return WindowsCreateString(L"{}", 2, value);
}

static HRESULT WINAPI store_result_rate_get_Status(void *iface, UINT32 *value)
{
    if (!value) return E_INVALIDARG;
    *value = 0;
    return S_OK;
}

static HRESULT WINAPI store_result_rate_get_WasUpdated(void *iface, boolean *value)
{
    if (!value) return E_INVALIDARG;
    *value = 0;
    return S_OK;
}

static const void *store_result_rate_vtbl[] =
{
    store_result_rate_QueryInterface,
    store_result_rate_AddRef,
    store_result_rate_Release,
    store_result_GetIids,
    store_result_GetRuntimeClassName,
    store_result_GetTrustLevel,
    /* IStoreRateAndReviewResult methods (slots 6..9) */
    store_result_rate_get_ExtendedError,
    store_result_rate_get_ExtendedJsonData,
    store_result_rate_get_Status,
    store_result_rate_get_WasUpdated,
};

/* 4. IStoreAppLicense vtbl methods */
static HRESULT WINAPI store_result_license_QueryInterface(void *iface, REFIID iid, void **out)
{
    return store_result_query_interface_internal(impl_from_store_result_license(iface), iid, out);
}

static ULONG WINAPI store_result_license_AddRef(void *iface)
{
    struct store_result *impl = impl_from_store_result_license(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI store_result_license_Release(void *iface)
{
    struct store_result *impl = impl_from_store_result_license(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI store_result_license_get_SkuStoreId(void *iface, HSTRING *value)
{
    if (!value) return E_INVALIDARG;
    return WindowsCreateString(L"0010", 4, value);
}

static HRESULT WINAPI store_result_license_get_IsActive(void *iface, boolean *value)
{
    if (!value) return E_INVALIDARG;
    *value = 1;
    return S_OK;
}

static HRESULT WINAPI store_result_license_get_IsTrial(void *iface, boolean *value)
{
    if (!value) return E_INVALIDARG;
    *value = 0;
    return S_OK;
}

static HRESULT WINAPI store_result_license_get_ExpirationDate(void *iface, void *value)
{
    if (value) memset(value, 0, 8);
    return S_OK;
}

static HRESULT WINAPI store_result_license_get_ExtendedJsonData(void *iface, HSTRING *value)
{
    if (!value) return E_INVALIDARG;
    return WindowsCreateString(L"{}", 2, value);
}

static HRESULT WINAPI store_result_license_GetRuntimeClassName(void *iface, HSTRING *class_name)
{
    static const WCHAR name[] = L"Windows.Services.Store.StoreAppLicense";
    return WindowsCreateString(name, ARRAY_SIZE(name) - 1, class_name);
}

static HRESULT WINAPI store_result_license_get_AddOnLicenses(void *iface, void **value)
{
    struct store_result *impl = impl_from_store_result_license(iface);
    if (!value) return E_INVALIDARG;
    *value = &impl->IMapView_iface;
    InterlockedIncrement(&impl->ref);
    return S_OK;
}

static HRESULT WINAPI store_result_license_get_TrialTimeRemaining(void *iface, void *value)
{
    if (value) memset(value, 0, 8);
    return S_OK;
}

static HRESULT WINAPI store_result_license_get_IsTrialOwnedByThisUser(void *iface, boolean *value)
{
    if (!value) return E_INVALIDARG;
    *value = 0;
    return S_OK;
}

static HRESULT WINAPI store_result_license_get_TrialUniqueId(void *iface, HSTRING *value)
{
    if (!value) return E_INVALIDARG;
    return WindowsCreateString(L"", 0, value);
}

static const void *store_result_license_vtbl[] =
{
    store_result_license_QueryInterface,
    store_result_license_AddRef,
    store_result_license_Release,
    store_result_GetIids,
    store_result_license_GetRuntimeClassName,
    store_result_GetTrustLevel,
    /* IStoreAppLicense methods (slots 6..14) */
    store_result_license_get_SkuStoreId,
    store_result_license_get_IsActive,
    store_result_license_get_IsTrial,
    store_result_license_get_ExpirationDate,
    store_result_license_get_ExtendedJsonData,
    store_result_license_get_AddOnLicenses,
    store_result_license_get_TrialTimeRemaining,
    store_result_license_get_IsTrialOwnedByThisUser,
    store_result_license_get_TrialUniqueId,
};

/* 5. IVectorView vtbl methods */
static HRESULT WINAPI store_result_vector_QueryInterface(void *iface, REFIID iid, void **out)
{
    struct store_result *impl = impl_from_store_result_vector(iface);
    if (!out) return E_INVALIDARG;

    if (IsEqualIID(iid, &IID_IWeakReferenceSource_CsWinRT) ||
        IsEqualIID(iid, &IID_ILanguageExceptionErrorInfo) ||
        IsEqualIID(iid, &IID_IWeakReferenceSource) ||
        IsEqualIID(iid, &IID_IWeakReference) ||
        IsEqualIID(iid, &IID_IMarshal) ||
        IsEqualIID(iid, &IID_ICustomQueryInterface))
    {
        *out = NULL;
        return E_NOINTERFACE;
    }

    if (IsEqualIID(iid, &IID_IIterable) ||
        IsEqualIID(iid, &IID_IIterable_StorePackageUpdate) ||
        IsEqualIID(iid, &IID_IIterable_StoreProductMap))
    {
        *out = &impl->IIterable_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    *out = &impl->IVectorView_iface;
    InterlockedIncrement(&impl->ref);
    return S_OK;
}

static ULONG WINAPI store_result_vector_AddRef(void *iface)
{
    struct store_result *impl = impl_from_store_result_vector(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI store_result_vector_Release(void *iface)
{
    struct store_result *impl = impl_from_store_result_vector(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI store_result_vector_GetRuntimeClassName(void *iface, HSTRING *class_name)
{
    static const WCHAR name[] = L"Windows.Foundation.Collections.IVectorView`1<IInspectable>";
    return WindowsCreateString(name, ARRAY_SIZE(name) - 1, class_name);
}

static HRESULT WINAPI store_result_vector_GetAt(void *iface, UINT32 index, void **item)
{
    if (item) *item = NULL;
    return E_BOUNDS;
}

static HRESULT WINAPI store_result_vector_get_Size(void *iface, UINT32 *size)
{
    if (!size) return E_INVALIDARG;
    *size = 0;
    return S_OK;
}

static HRESULT WINAPI store_result_vector_IndexOf(void *iface, void *item, UINT32 *index, boolean *found)
{
    if (index) *index = 0;
    if (found) *found = 0;
    return S_OK;
}

static HRESULT WINAPI store_result_vector_GetMany(void *iface, UINT32 startIndex, UINT32 capacity, void **items, UINT32 *actual)
{
    if (actual) *actual = 0;
    return S_OK;
}

static const void *store_result_vector_vtbl[] =
{
    store_result_vector_QueryInterface,
    store_result_vector_AddRef,
    store_result_vector_Release,
    store_result_GetIids,
    store_result_vector_GetRuntimeClassName,
    store_result_GetTrustLevel,
    /* IVectorView methods (slots 6..9) */
    store_result_vector_GetAt,
    store_result_vector_get_Size,
    store_result_vector_IndexOf,
    store_result_vector_GetMany,
};

/* 5b. IMapView vtbl methods */
static HRESULT WINAPI store_result_map_QueryInterface(void *iface, REFIID iid, void **out)
{
    struct store_result *impl = impl_from_store_result_map(iface);
    if (!out) return E_INVALIDARG;

    if (IsEqualIID(iid, &IID_IWeakReferenceSource_CsWinRT) ||
        IsEqualIID(iid, &IID_ILanguageExceptionErrorInfo) ||
        IsEqualIID(iid, &IID_IWeakReferenceSource) ||
        IsEqualIID(iid, &IID_IWeakReference) ||
        IsEqualIID(iid, &IID_IMarshal) ||
        IsEqualIID(iid, &IID_ICustomQueryInterface))
    {
        *out = NULL;
        return E_NOINTERFACE;
    }

    if (IsEqualIID(iid, &IID_IIterable) ||
        IsEqualIID(iid, &IID_IIterable_StorePackageUpdate) ||
        IsEqualIID(iid, &IID_IIterable_StoreProductMap))
    {
        *out = &impl->IIterable_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    *out = &impl->IMapView_iface;
    InterlockedIncrement(&impl->ref);
    return S_OK;
}

static ULONG WINAPI store_result_map_AddRef(void *iface)
{
    struct store_result *impl = impl_from_store_result_map(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI store_result_map_Release(void *iface)
{
    struct store_result *impl = impl_from_store_result_map(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI store_result_map_GetRuntimeClassName(void *iface, HSTRING *class_name)
{
    static const WCHAR name[] = L"Windows.Foundation.Collections.IMapView`2<HSTRING, IInspectable>";
    return WindowsCreateString(name, ARRAY_SIZE(name) - 1, class_name);
}

static HRESULT WINAPI store_result_map_Lookup(void *iface, HSTRING key, void **value)
{
    if (value) *value = NULL;
    return 0x8000000B; /* E_BOUNDS */
}

static HRESULT WINAPI store_result_map_get_Size(void *iface, UINT32 *size)
{
    if (!size) return E_INVALIDARG;
    *size = 0;
    return S_OK;
}

static HRESULT WINAPI store_result_map_HasKey(void *iface, HSTRING key, boolean *found)
{
    if (!found) return E_INVALIDARG;
    *found = FALSE;
    return S_OK;
}

static HRESULT WINAPI store_result_map_Split(void *iface, void **first, void **second)
{
    if (first) *first = NULL;
    if (second) *second = NULL;
    return S_OK;
}

static const void *store_result_map_vtbl[] =
{
    store_result_map_QueryInterface,
    store_result_map_AddRef,
    store_result_map_Release,
    store_result_GetIids,
    store_result_map_GetRuntimeClassName,
    store_result_GetTrustLevel,
    /* IMapView methods (slots 6..9) */
    store_result_map_Lookup,
    store_result_map_get_Size,
    store_result_map_HasKey,
    store_result_map_Split,
};

/* IIterator implementation for Store empty collections */
struct store_iterator
{
    IInspectable IIterator_iface;
    LONG ref;
};

static inline struct store_iterator *impl_from_store_iterator(void *iface)
{
    return CONTAINING_RECORD(iface, struct store_iterator, IIterator_iface);
}

static HRESULT WINAPI store_iterator_QueryInterface(void *iface, REFIID iid, void **out)
{
    struct store_iterator *impl = impl_from_store_iterator(iface);
    if (!out) return E_INVALIDARG;

    if (IsEqualIID(iid, &IID_IWeakReferenceSource_CsWinRT) ||
        IsEqualIID(iid, &IID_ILanguageExceptionErrorInfo) ||
        IsEqualIID(iid, &IID_IWeakReferenceSource) ||
        IsEqualIID(iid, &IID_IWeakReference) ||
        IsEqualIID(iid, &IID_IMarshal) ||
        IsEqualIID(iid, &IID_ICustomQueryInterface))
    {
        *out = NULL;
        return E_NOINTERFACE;
    }

    if (IsEqualIID(iid, &IID_IUnknown) ||
        IsEqualIID(iid, &IID_IInspectable) ||
        IsEqualIID(iid, &IID_IAgileObject) ||
        IsEqualIID(iid, &IID_IIterator) ||
        IsEqualIID(iid, &IID_IIterator_StorePackageUpdate) ||
        IsEqualIID(iid, &IID_IIterator_StoreProductMap))
    {
        *out = &impl->IIterator_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    /* Fallback: any other IIterator<T> generic query on empty iterator yields the same interface */
    *out = &impl->IIterator_iface;
    InterlockedIncrement(&impl->ref);
    return S_OK;
}

static ULONG WINAPI store_iterator_AddRef(void *iface)
{
    struct store_iterator *impl = impl_from_store_iterator(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI store_iterator_Release(void *iface)
{
    struct store_iterator *impl = impl_from_store_iterator(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI store_iterator_get_Current(void *iface, void **current)
{
    if (current) *current = NULL;
    return E_BOUNDS;
}

static HRESULT WINAPI store_iterator_get_HasCurrent(void *iface, boolean *has_current)
{
    if (!has_current) return E_INVALIDARG;
    *has_current = 0;
    return S_OK;
}

static HRESULT WINAPI store_iterator_MoveNext(void *iface, boolean *has_current)
{
    if (!has_current) return E_INVALIDARG;
    *has_current = 0;
    return S_OK;
}

static HRESULT WINAPI store_iterator_GetMany(void *iface, UINT32 count, void **items, UINT32 *actual)
{
    if (actual) *actual = 0;
    return S_OK;
}

static const void *store_iterator_vtbl[] =
{
    store_iterator_QueryInterface,
    store_iterator_AddRef,
    store_iterator_Release,
    store_result_GetIids,
    store_result_GetRuntimeClassName,
    store_result_GetTrustLevel,
    /* IIterator methods (slots 6..9) */
    store_iterator_get_Current,
    store_iterator_get_HasCurrent,
    store_iterator_MoveNext,
    store_iterator_GetMany,
};

static HRESULT store_iterator_create(void **out)
{
    struct store_iterator *impl;
    if (!out) return E_INVALIDARG;
    if (!(impl = calloc(1, sizeof(*impl)))) return E_OUTOFMEMORY;
    impl->IIterator_iface.lpVtbl = (void *)store_iterator_vtbl;
    impl->ref = 1;
    *out = &impl->IIterator_iface;
    return S_OK;
}

/* 6. IIterable vtbl methods */
static HRESULT WINAPI store_result_iterable_QueryInterface(void *iface, REFIID iid, void **out)
{
    return store_result_query_interface_internal(impl_from_store_result_iterable(iface), iid, out);
}

static ULONG WINAPI store_result_iterable_AddRef(void *iface)
{
    struct store_result *impl = impl_from_store_result_iterable(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI store_result_iterable_Release(void *iface)
{
    struct store_result *impl = impl_from_store_result_iterable(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI store_result_iterable_First(void *iface, void **first)
{
    if (!first) return E_INVALIDARG;
    return store_iterator_create(first);
}

static const void *store_result_iterable_vtbl[] =
{
    store_result_iterable_QueryInterface,
    store_result_iterable_AddRef,
    store_result_iterable_Release,
    store_result_GetIids,
    store_result_GetRuntimeClassName,
    store_result_GetTrustLevel,
    /* IIterable method (slot 6) */
    store_result_iterable_First,
};

/* 7a. IStoreProductQueryResult vtbl methods (slots 6..7) */
static HRESULT WINAPI store_result_query_QueryInterface(void *iface, REFIID iid, void **out)
{
    return store_result_query_interface_internal(impl_from_store_result_query(iface), iid, out);
}

static ULONG WINAPI store_result_query_AddRef(void *iface)
{
    struct store_result *impl = impl_from_store_result_query(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI store_result_query_Release(void *iface)
{
    struct store_result *impl = impl_from_store_result_query(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI store_result_query_GetRuntimeClassName(void *iface, HSTRING *class_name)
{
    static const WCHAR name[] = L"Windows.Services.Store.StoreProductQueryResult";
    return WindowsCreateString(name, ARRAY_SIZE(name) - 1, class_name);
}

static HRESULT WINAPI store_result_query_get_Products(void *iface, void **value)
{
    struct store_result *impl = impl_from_store_result_query(iface);
    if (!value) return E_INVALIDARG;
    *value = &impl->IMapView_iface;
    InterlockedIncrement(&impl->ref);
    return S_OK;
}

static HRESULT WINAPI store_result_query_get_ExtendedError(void *iface, HRESULT *value)
{
    if (!value) return E_INVALIDARG;
    *value = S_OK;
    return S_OK;
}

static const void *store_result_query_vtbl[] =
{
    store_result_query_QueryInterface,
    store_result_query_AddRef,
    store_result_query_Release,
    store_result_GetIids,
    store_result_query_GetRuntimeClassName,
    store_result_GetTrustLevel,
    /* IStoreProductQueryResult methods (slots 6..7) */
    store_result_query_get_Products,
    store_result_query_get_ExtendedError,
};

/* 7b. IStoreProductPagedQueryResult vtbl methods (slots 6..9) */
static HRESULT WINAPI store_result_paged_query_QueryInterface(void *iface, REFIID iid, void **out)
{
    return store_result_query_interface_internal(impl_from_store_result_paged_query(iface), iid, out);
}

static ULONG WINAPI store_result_paged_query_AddRef(void *iface)
{
    struct store_result *impl = impl_from_store_result_paged_query(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI store_result_paged_query_Release(void *iface)
{
    struct store_result *impl = impl_from_store_result_paged_query(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI store_result_paged_query_GetRuntimeClassName(void *iface, HSTRING *class_name)
{
    static const WCHAR name[] = L"Windows.Services.Store.StoreProductPagedQueryResult";
    return WindowsCreateString(name, ARRAY_SIZE(name) - 1, class_name);
}

static HRESULT WINAPI store_result_query_get_HasMoreResults(void *iface, boolean *value)
{
    if (!value) return E_INVALIDARG;
    *value = 0;
    return S_OK;
}

static HRESULT WINAPI store_result_paged_query_GetNextAsync(void *iface, void **operation)
{
    TRACE("iface %p, operation %p.\n", iface, operation);
    return store_async_op_create(STORE_ASYNC_PAGED_QUERY_RESULT, operation);
}

static const void *store_result_paged_query_vtbl[] =
{
    store_result_paged_query_QueryInterface,
    store_result_paged_query_AddRef,
    store_result_paged_query_Release,
    store_result_GetIids,
    store_result_paged_query_GetRuntimeClassName,
    store_result_GetTrustLevel,
    /* IStoreProductPagedQueryResult methods (slots 6..9) */
    store_result_query_get_Products,
    store_result_query_get_HasMoreResults,
    store_result_query_get_ExtendedError,
    store_result_paged_query_GetNextAsync,
};

static HRESULT store_result_create(enum store_async_type type, void **out)
{
    struct store_result *impl;
    if (!out) return E_INVALIDARG;
    if (!(impl = calloc(1, sizeof(*impl)))) return E_OUTOFMEMORY;
    impl->IStoreProductResult_iface.lpVtbl = (void *)store_result_product_result_vtbl;
    impl->IStoreProduct_iface.lpVtbl = (void *)store_result_product_vtbl;
    impl->IStoreRateAndReviewResult_iface.lpVtbl = (void *)store_result_rate_vtbl;
    impl->IStoreAppLicense_iface.lpVtbl = (void *)store_result_license_vtbl;
    impl->IVectorView_iface.lpVtbl = (void *)store_result_vector_vtbl;
    impl->IMapView_iface.lpVtbl = (void *)store_result_map_vtbl;
    impl->IIterable_iface.lpVtbl = (void *)store_result_iterable_vtbl;
    impl->IStoreQueryResult_iface.lpVtbl = (void *)store_result_query_vtbl;
    impl->IStoreProductPagedQueryResult_iface.lpVtbl = (void *)store_result_paged_query_vtbl;
    impl->ref = 1;

    switch (type)
    {
    case STORE_ASYNC_APP_LICENSE:
        *out = &impl->IStoreAppLicense_iface;
        break;
    case STORE_ASYNC_QUERY_RESULT:
        *out = &impl->IStoreQueryResult_iface;
        break;
    case STORE_ASYNC_PAGED_QUERY_RESULT:
        *out = &impl->IStoreProductPagedQueryResult_iface;
        break;
    case STORE_ASYNC_RATE_RESULT:
        *out = &impl->IStoreRateAndReviewResult_iface;
        break;
    case STORE_ASYNC_VECTOR_VIEW:
        *out = &impl->IVectorView_iface;
        break;
    case STORE_ASYNC_PRODUCT_RESULT:
    default:
        *out = &impl->IStoreProductResult_iface;
        break;
    }
    return S_OK;
}

/* ========================================================================= */
/* Generic completed async operation for store queries                       */
/* ========================================================================= */
struct store_async_op
{
    IInspectable IAsyncOperation_iface;
    IAsyncInfo IAsyncInfo_iface;
    enum store_async_type type;
    LONG ref;
};

static inline struct store_async_op *impl_from_store_IAsyncOperation(IInspectable *iface)
{
    return CONTAINING_RECORD(iface, struct store_async_op, IAsyncOperation_iface);
}

static inline struct store_async_op *impl_from_store_IAsyncInfo(IAsyncInfo *iface)
{
    return CONTAINING_RECORD(iface, struct store_async_op, IAsyncInfo_iface);
}

static HRESULT WINAPI store_async_QueryInterface(IInspectable *iface, REFIID iid, void **out)
{
    struct store_async_op *impl = impl_from_store_IAsyncOperation(iface);
    TRACE("iface %p, iid %s, out %p.\n", iface, debugstr_guid(iid), out);
    if (!out) return E_INVALIDARG;

    if (IsEqualIID(iid, &IID_IWeakReferenceSource_CsWinRT) ||
        IsEqualIID(iid, &IID_ILanguageExceptionErrorInfo) ||
        IsEqualIID(iid, &IID_IWeakReferenceSource) ||
        IsEqualIID(iid, &IID_IWeakReference) ||
        IsEqualIID(iid, &IID_IMarshal) ||
        IsEqualIID(iid, &IID_ICustomQueryInterface))
    {
        *out = NULL;
        return E_NOINTERFACE;
    }

    if (IsEqualIID(iid, &IID_IUnknown) ||
        IsEqualIID(iid, &IID_IInspectable) ||
        IsEqualIID(iid, &IID_IAgileObject))
    {
        *out = &impl->IAsyncOperation_iface;
    }
    else if (IsEqualIID(iid, &IID_IAsyncInfo))
    {
        *out = &impl->IAsyncInfo_iface;
    }
    else
    {
        /* Accept any IAsyncOperation`1 specialization */
        *out = &impl->IAsyncOperation_iface;
    }

    IUnknown_AddRef((IUnknown *)*out);
    return S_OK;
}

static ULONG WINAPI store_async_AddRef(IInspectable *iface)
{
    struct store_async_op *impl = impl_from_store_IAsyncOperation(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI store_async_Release(IInspectable *iface)
{
    struct store_async_op *impl = impl_from_store_IAsyncOperation(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI store_async_GetIids(IInspectable *iface, ULONG *iid_count, IID **iids)
{
    if (!iid_count || !iids) return E_INVALIDARG;
    *iid_count = 0;
    *iids = NULL;
    return S_OK;
}

static HRESULT WINAPI store_async_GetRuntimeClassName(IInspectable *iface, HSTRING *class_name)
{
    struct store_async_op *impl = impl_from_store_IAsyncOperation(iface);
    const WCHAR *name;

    switch (impl->type)
    {
    case STORE_ASYNC_APP_LICENSE:
        name = L"Windows.Foundation.IAsyncOperation`1<Windows.Services.Store.StoreAppLicense>";
        break;
    case STORE_ASYNC_QUERY_RESULT:
        name = L"Windows.Foundation.IAsyncOperation`1<Windows.Services.Store.StoreProductQueryResult>";
        break;
    case STORE_ASYNC_PAGED_QUERY_RESULT:
        name = L"Windows.Foundation.IAsyncOperation`1<Windows.Services.Store.StoreProductPagedQueryResult>";
        break;
    case STORE_ASYNC_RATE_RESULT:
        name = L"Windows.Foundation.IAsyncOperation`1<Windows.Services.Store.StoreRateAndReviewResult>";
        break;
    case STORE_ASYNC_VECTOR_VIEW:
        name = L"Windows.Foundation.IAsyncOperation`1<Windows.Foundation.Collections.IVectorView`1<Windows.Services.Store.StorePackageUpdate>>";
        break;
    case STORE_ASYNC_PRODUCT_RESULT:
    default:
        name = L"Windows.Foundation.IAsyncOperation`1<Windows.Services.Store.StoreProductResult>";
        break;
    }
    return WindowsCreateString(name, wcslen(name), class_name);
}

static HRESULT WINAPI store_async_GetTrustLevel(IInspectable *iface, TrustLevel *trust_level)
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

typedef struct
{
    HRESULT (WINAPI *QueryInterface)(void *this, REFIID riid, void **out);
    ULONG   (WINAPI *AddRef)(void *this);
    ULONG   (WINAPI *Release)(void *this);
    HRESULT (WINAPI *Invoke)(void *this, void *asyncInfo, AsyncStatus status);
} store_async_handler_vtbl;

static HRESULT WINAPI store_async_put_Completed(IInspectable *iface, IUnknown *handler)
{
    struct store_async_op *impl = impl_from_store_IAsyncOperation(iface);
    TRACE("iface %p, handler %p.\n", iface, handler);
    if (handler)
    {
        store_async_handler_vtbl *vtbl = *(store_async_handler_vtbl **)handler;
        vtbl->Invoke(handler, &impl->IAsyncOperation_iface, Completed);
    }
    return S_OK;
}

static HRESULT WINAPI store_async_get_Completed(IInspectable *iface, IUnknown **handler)
{
    if (!handler) return E_INVALIDARG;
    *handler = NULL;
    return S_OK;
}

static HRESULT WINAPI store_async_GetResults(IInspectable *iface, void *results)
{
    struct store_async_op *impl = impl_from_store_IAsyncOperation(iface);
    TRACE("iface %p, results %p, type %d.\n", iface, results, impl->type);
    if (!results) return E_INVALIDARG;
    return store_result_create(impl->type, (void **)results);
}

static const struct
{
    HRESULT (WINAPI *QueryInterface)(IInspectable *iface, REFIID iid, void **out);
    ULONG   (WINAPI *AddRef)(IInspectable *iface);
    ULONG   (WINAPI *Release)(IInspectable *iface);
    HRESULT (WINAPI *GetIids)(IInspectable *iface, ULONG *iid_count, IID **iids);
    HRESULT (WINAPI *GetRuntimeClassName)(IInspectable *iface, HSTRING *class_name);
    HRESULT (WINAPI *GetTrustLevel)(IInspectable *iface, TrustLevel *trust_level);
    HRESULT (WINAPI *put_Completed)(IInspectable *iface, IUnknown *handler);
    HRESULT (WINAPI *get_Completed)(IInspectable *iface, IUnknown **handler);
    HRESULT (WINAPI *GetResults)(IInspectable *iface, void *results);
} store_async_op_vtbl =
{
    store_async_QueryInterface,
    store_async_AddRef,
    store_async_Release,
    store_async_GetIids,
    store_async_GetRuntimeClassName,
    store_async_GetTrustLevel,
    store_async_put_Completed,
    store_async_get_Completed,
    store_async_GetResults,
};

static HRESULT WINAPI store_async_info_QueryInterface(IAsyncInfo *iface, REFIID iid, void **out)
{
    struct store_async_op *impl = impl_from_store_IAsyncInfo(iface);
    return store_async_QueryInterface(&impl->IAsyncOperation_iface, iid, out);
}

static ULONG WINAPI store_async_info_AddRef(IAsyncInfo *iface)
{
    struct store_async_op *impl = impl_from_store_IAsyncInfo(iface);
    return store_async_AddRef(&impl->IAsyncOperation_iface);
}

static ULONG WINAPI store_async_info_Release(IAsyncInfo *iface)
{
    struct store_async_op *impl = impl_from_store_IAsyncInfo(iface);
    return store_async_Release(&impl->IAsyncOperation_iface);
}

static HRESULT WINAPI store_async_info_GetIids(IAsyncInfo *iface, ULONG *iid_count, IID **iids)
{
    struct store_async_op *impl = impl_from_store_IAsyncInfo(iface);
    return store_async_GetIids(&impl->IAsyncOperation_iface, iid_count, iids);
}

static HRESULT WINAPI store_async_info_GetRuntimeClassName(IAsyncInfo *iface, HSTRING *class_name)
{
    struct store_async_op *impl = impl_from_store_IAsyncInfo(iface);
    return store_async_GetRuntimeClassName(&impl->IAsyncOperation_iface, class_name);
}

static HRESULT WINAPI store_async_info_GetTrustLevel(IAsyncInfo *iface, TrustLevel *trust_level)
{
    struct store_async_op *impl = impl_from_store_IAsyncInfo(iface);
    return store_async_GetTrustLevel(&impl->IAsyncOperation_iface, trust_level);
}

static HRESULT WINAPI store_async_info_get_Id(IAsyncInfo *iface, UINT32 *id)
{
    if (!id) return E_INVALIDARG;
    *id = 1;
    return S_OK;
}

static HRESULT WINAPI store_async_info_get_Status(IAsyncInfo *iface, AsyncStatus *status)
{
    if (!status) return E_INVALIDARG;
    *status = Completed;
    return S_OK;
}

static HRESULT WINAPI store_async_info_get_ErrorCode(IAsyncInfo *iface, HRESULT *error_code)
{
    if (!error_code) return E_INVALIDARG;
    *error_code = S_OK;
    return S_OK;
}

static HRESULT WINAPI store_async_info_Cancel(IAsyncInfo *iface)
{
    return S_OK;
}

static HRESULT WINAPI store_async_info_Close(IAsyncInfo *iface)
{
    return S_OK;
}

static const struct IAsyncInfoVtbl store_async_info_vtbl =
{
    store_async_info_QueryInterface,
    store_async_info_AddRef,
    store_async_info_Release,
    store_async_info_GetIids,
    store_async_info_GetRuntimeClassName,
    store_async_info_GetTrustLevel,
    store_async_info_get_Id,
    store_async_info_get_Status,
    store_async_info_get_ErrorCode,
    store_async_info_Cancel,
    store_async_info_Close,
};

static HRESULT store_async_op_create(enum store_async_type type, void **out)
{
    struct store_async_op *impl;
    if (!out) return E_INVALIDARG;
    if (!(impl = calloc(1, sizeof(*impl)))) return E_OUTOFMEMORY;
    impl->IAsyncOperation_iface.lpVtbl = (void *)&store_async_op_vtbl;
    impl->IAsyncInfo_iface.lpVtbl = &store_async_info_vtbl;
    impl->type = type;
    impl->ref = 1;
    *out = &impl->IAsyncOperation_iface;
    return S_OK;
}

/* ========================================================================= */
/* StoreContext instance                                                     */
/* ========================================================================= */
struct store_context
{
    IInspectable IStoreContext_iface;
    IInspectable IStoreContext2_iface;
    IInspectable IStoreContext3_iface;
    IInspectable IStoreContext4_iface;
    LONG ref;
};

static inline struct store_context *impl_from_store_context(void *iface)
{
    return CONTAINING_RECORD(iface, struct store_context, IStoreContext_iface);
}

static inline struct store_context *impl_from_store_context2(void *iface)
{
    return CONTAINING_RECORD(iface, struct store_context, IStoreContext2_iface);
}

static inline struct store_context *impl_from_store_context3(void *iface)
{
    return CONTAINING_RECORD(iface, struct store_context, IStoreContext3_iface);
}

static inline struct store_context *impl_from_store_context4(void *iface)
{
    return CONTAINING_RECORD(iface, struct store_context, IStoreContext4_iface);
}

static HRESULT store_context_query_interface_internal(struct store_context *impl, REFIID iid, void **out)
{
    TRACE("impl %p, iid %s, out %p.\n", impl, debugstr_guid(iid), out);
    if (!out) return E_INVALIDARG;

    if (IsEqualIID(iid, &IID_IWeakReferenceSource_CsWinRT) ||
        IsEqualIID(iid, &IID_ILanguageExceptionErrorInfo) ||
        IsEqualIID(iid, &IID_IWeakReferenceSource) ||
        IsEqualIID(iid, &IID_IWeakReference) ||
        IsEqualIID(iid, &IID_IMarshal) ||
        IsEqualIID(iid, &IID_ICustomQueryInterface))
    {
        *out = NULL;
        return E_NOINTERFACE;
    }

    if (IsEqualIID(iid, &IID_IUnknown) ||
        IsEqualIID(iid, &IID_IInspectable) ||
        IsEqualIID(iid, &IID_IAgileObject) ||
        IsEqualIID(iid, &IID_IStoreContext))
    {
        *out = &impl->IStoreContext_iface;
    }
    else if (IsEqualIID(iid, &IID_IStoreContext2))
    {
        *out = &impl->IStoreContext2_iface;
    }
    else if (IsEqualIID(iid, &IID_IStoreContext3))
    {
        *out = &impl->IStoreContext3_iface;
    }
    else if (IsEqualIID(iid, &IID_IStoreContext4))
    {
        *out = &impl->IStoreContext4_iface;
    }
    else
    {
        WARN("unsupported iid %s\n", debugstr_guid(iid));
        *out = NULL;
        return E_NOINTERFACE;
    }

    IUnknown_AddRef((IUnknown *)*out);
    return S_OK;
}

static HRESULT WINAPI store_context_QueryInterface(void *iface, REFIID iid, void **out)
{
    return store_context_query_interface_internal(impl_from_store_context(iface), iid, out);
}

static ULONG WINAPI store_context_AddRef(void *iface)
{
    struct store_context *impl = impl_from_store_context(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI store_context_Release(void *iface)
{
    struct store_context *impl = impl_from_store_context(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI store_context_GetIids(void *iface, ULONG *iid_count, IID **iids)
{
    IID *array;
    if (!iid_count || !iids) return E_INVALIDARG;
    if (!(array = CoTaskMemAlloc(4 * sizeof(IID)))) return E_OUTOFMEMORY;
    array[0] = IID_IAgileObject;
    array[1] = IID_IStoreContext;
    array[2] = IID_IStoreContext2;
    array[3] = IID_IStoreContext4;
    *iid_count = 4;
    *iids = array;
    return S_OK;
}

static HRESULT WINAPI store_context_GetRuntimeClassName(void *iface, HSTRING *class_name)
{
    static const WCHAR name[] = L"Windows.Services.Store.StoreContext";
    return WindowsCreateString(name, ARRAY_SIZE(name) - 1, class_name);
}

static HRESULT WINAPI store_context_GetTrustLevel(void *iface, TrustLevel *trust_level)
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

/* ========================================================================= */
/* IStoreContext methods (CIL-aligned slots 6..26)                           */
/* ========================================================================= */
static HRESULT WINAPI store_context_get_User(void *iface, void **out)
{
    TRACE("iface %p, out %p.\n", iface, out);
    if (out) *out = NULL;
    return S_OK;
}

static HRESULT WINAPI store_context_add_OfflineLicensesChanged(void *iface, void *handler, EventRegistrationToken *token)
{
    TRACE("iface %p, handler %p, token %p.\n", iface, handler, token);
    if (token) token->value = 1;
    return S_OK;
}

static HRESULT WINAPI store_context_remove_OfflineLicensesChanged(void *iface, EventRegistrationToken token)
{
    TRACE("iface %p, token %lld.\n", iface, (long long)token.value);
    return S_OK;
}

static HRESULT WINAPI store_context_GetCustomerPurchaseIdAsync(void *iface, HSTRING ticket, HSTRING user_id, void **out)
{
    TRACE("iface %p, out %p.\n", iface, out);
    return store_async_op_create(STORE_ASYNC_DEFAULT, out);
}

static HRESULT WINAPI store_context_GetCustomerCollectionsIdAsync(void *iface, HSTRING ticket, HSTRING user_id, void **out)
{
    TRACE("iface %p, out %p.\n", iface, out);
    return store_async_op_create(STORE_ASYNC_DEFAULT, out);
}

static HRESULT WINAPI store_context_GetAppLicenseAsync(void *iface, void **out)
{
    TRACE("iface %p, out %p.\n", iface, out);
    return store_async_op_create(STORE_ASYNC_APP_LICENSE, out);
}

static HRESULT WINAPI store_context_GetStoreProductForCurrentAppAsync(void *iface, void **out)
{
    TRACE("iface %p, out %p.\n", iface, out);
    return store_async_op_create(STORE_ASYNC_PRODUCT_RESULT, out);
}

static HRESULT WINAPI store_context_GetStoreProductsAsync(void *iface, void *kinds, void *ids, void **out)
{
    TRACE("iface %p, out %p.\n", iface, out);
    return store_async_op_create(STORE_ASYNC_QUERY_RESULT, out);
}

static HRESULT WINAPI store_context_GetAssociatedStoreProductsAsync(void *iface, void *kinds, void **out)
{
    TRACE("iface %p, out %p.\n", iface, out);
    return store_async_op_create(STORE_ASYNC_QUERY_RESULT, out);
}

static HRESULT WINAPI store_context_GetAssociatedStoreProductsWithPagingAsync(void *iface, void *kinds, UINT32 max_items, void **out)
{
    TRACE("iface %p, out %p.\n", iface, out);
    return store_async_op_create(STORE_ASYNC_PAGED_QUERY_RESULT, out);
}

static HRESULT WINAPI store_context_GetUserCollectionAsync(void *iface, void *kinds, void **out)
{
    TRACE("iface %p, out %p.\n", iface, out);
    return store_async_op_create(STORE_ASYNC_QUERY_RESULT, out);
}

static HRESULT WINAPI store_context_GetUserCollectionWithPagingAsync(void *iface, void *kinds, UINT32 max_items, void **out)
{
    TRACE("iface %p, out %p.\n", iface, out);
    return store_async_op_create(STORE_ASYNC_PAGED_QUERY_RESULT, out);
}

static HRESULT WINAPI store_context_ReportConsumableFulfillmentAsync(void *iface, HSTRING store_id, UINT32 qty, GUID tracking_id, void **out)
{
    TRACE("iface %p, out %p.\n", iface, out);
    return store_async_op_create(STORE_ASYNC_DEFAULT, out);
}

static HRESULT WINAPI store_context_GetConsumableBalanceRemainingAsync(void *iface, HSTRING store_id, void **out)
{
    TRACE("iface %p, out %p.\n", iface, out);
    return store_async_op_create(STORE_ASYNC_DEFAULT, out);
}

static HRESULT WINAPI store_context_AcquireStoreLicenseForOptionalPackageAsync(void *iface, void *pkg, void **out)
{
    TRACE("iface %p, out %p.\n", iface, out);
    return store_async_op_create(STORE_ASYNC_DEFAULT, out);
}

static HRESULT WINAPI store_context_RequestPurchaseAsync(void *iface, HSTRING store_id, void **out)
{
    TRACE("iface %p, out %p.\n", iface, out);
    return store_async_op_create(STORE_ASYNC_PRODUCT_RESULT, out);
}

static HRESULT WINAPI store_context_RequestPurchaseWithPropertiesAsync(void *iface, HSTRING store_id, void *props, void **out)
{
    TRACE("iface %p, out %p.\n", iface, out);
    return store_async_op_create(STORE_ASYNC_PRODUCT_RESULT, out);
}

static HRESULT WINAPI store_context_GetAppAndOptionalStorePackageUpdatesAsync(void *iface, void **out)
{
    TRACE("iface %p, out %p.\n", iface, out);
    return store_async_op_create(STORE_ASYNC_VECTOR_VIEW, out);
}

static HRESULT WINAPI store_context_RequestDownloadStorePackageUpdatesAsync(void *iface, void *updates, void **out)
{
    TRACE("iface %p, out %p.\n", iface, out);
    return store_async_op_create(STORE_ASYNC_DEFAULT, out);
}

static HRESULT WINAPI store_context_RequestDownloadAndInstallStorePackageUpdatesAsync(void *iface, void *updates, void **out)
{
    TRACE("iface %p, out %p.\n", iface, out);
    return store_async_op_create(STORE_ASYNC_DEFAULT, out);
}

static HRESULT WINAPI store_context_RequestDownloadAndInstallStorePackagesAsync(void *iface, void *ids, void **out)
{
    TRACE("iface %p, out %p.\n", iface, out);
    return store_async_op_create(STORE_ASYNC_DEFAULT, out);
}

static const void *store_context_vtbl[] =
{
    store_context_QueryInterface,
    store_context_AddRef,
    store_context_Release,
    store_context_GetIids,
    store_context_GetRuntimeClassName,
    store_context_GetTrustLevel,
    /* IStoreContext methods (slots 6..26) */
    /* 6  */ store_context_get_User,
    /* 7  */ store_context_add_OfflineLicensesChanged,
    /* 8  */ store_context_remove_OfflineLicensesChanged,
    /* 9  */ store_context_GetCustomerPurchaseIdAsync,
    /* 10 */ store_context_GetCustomerCollectionsIdAsync,
    /* 11 */ store_context_GetAppLicenseAsync,
    /* 12 */ store_context_GetStoreProductForCurrentAppAsync,
    /* 13 */ store_context_GetStoreProductsAsync,
    /* 14 */ store_context_GetAssociatedStoreProductsAsync,
    /* 15 */ store_context_GetAssociatedStoreProductsWithPagingAsync,
    /* 16 */ store_context_GetUserCollectionAsync,
    /* 17 */ store_context_GetUserCollectionWithPagingAsync,
    /* 18 */ store_context_ReportConsumableFulfillmentAsync,
    /* 19 */ store_context_GetConsumableBalanceRemainingAsync,
    /* 20 */ store_context_AcquireStoreLicenseForOptionalPackageAsync,
    /* 21 */ store_context_RequestPurchaseAsync,
    /* 22 */ store_context_RequestPurchaseWithPropertiesAsync,
    /* 23 */ store_context_GetAppAndOptionalStorePackageUpdatesAsync,
    /* 24 */ store_context_RequestDownloadStorePackageUpdatesAsync,
    /* 25 */ store_context_RequestDownloadAndInstallStorePackageUpdatesAsync,
    /* 26 */ store_context_RequestDownloadAndInstallStorePackagesAsync,
};

/* ========================================================================= */
/* IStoreContext2 methods (slot 6)                                           */
/* ========================================================================= */
static HRESULT WINAPI store_context2_QueryInterface(void *iface, REFIID iid, void **out)
{
    return store_context_query_interface_internal(impl_from_store_context2(iface), iid, out);
}

static ULONG WINAPI store_context2_AddRef(void *iface)
{
    struct store_context *impl = impl_from_store_context2(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI store_context2_Release(void *iface)
{
    struct store_context *impl = impl_from_store_context2(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI store_context2_FindStoreProductForPackageAsync(void *iface, void *kinds, void *pkg, void **out)
{
    TRACE("iface %p, out %p.\n", iface, out);
    return store_async_op_create(STORE_ASYNC_PRODUCT_RESULT, out);
}

static const void *store_context2_vtbl[] =
{
    store_context2_QueryInterface,
    store_context2_AddRef,
    store_context2_Release,
    store_context_GetIids,
    store_context_GetRuntimeClassName,
    store_context_GetTrustLevel,
    /* IStoreContext2 methods (slot 6) */
    store_context2_FindStoreProductForPackageAsync,
};

/* ========================================================================= */
/* IStoreContext3 methods (slots 6..19)                                      */
/* ========================================================================= */
static HRESULT WINAPI store_context3_QueryInterface(void *iface, REFIID iid, void **out)
{
    return store_context_query_interface_internal(impl_from_store_context3(iface), iid, out);
}

static ULONG WINAPI store_context3_AddRef(void *iface)
{
    struct store_context *impl = impl_from_store_context3(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI store_context3_Release(void *iface)
{
    struct store_context *impl = impl_from_store_context3(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI store_context3_get_CanSilentlyDownloadStorePackageUpdates(void *iface, boolean *val)
{
    if (val) *val = 0;
    return S_OK;
}

static HRESULT WINAPI store_context3_TrySilentDownloadStorePackageUpdatesAsync(void *iface, void *updates, void **out)
{
    return store_async_op_create(STORE_ASYNC_DEFAULT, out);
}

static HRESULT WINAPI store_context3_TrySilentDownloadAndInstallStorePackageUpdatesAsync(void *iface, void *updates, void **out)
{
    return store_async_op_create(STORE_ASYNC_DEFAULT, out);
}

static HRESULT WINAPI store_context3_CanAcquireStoreLicenseForOptionalPackageAsync(void *iface, void *pkg, void **out)
{
    return store_async_op_create(STORE_ASYNC_DEFAULT, out);
}

static HRESULT WINAPI store_context3_CanAcquireStoreLicenseAsync(void *iface, HSTRING id, void **out)
{
    return store_async_op_create(STORE_ASYNC_DEFAULT, out);
}

static HRESULT WINAPI store_context3_GetStoreProductsWithOptionsAsync(void *iface, void *kinds, void *ids, void *opts, void **out)
{
    return store_async_op_create(STORE_ASYNC_QUERY_RESULT, out);
}

static HRESULT WINAPI store_context3_GetAssociatedStoreQueueItemsAsync(void *iface, void **out)
{
    return store_async_op_create(STORE_ASYNC_DEFAULT, out);
}

static HRESULT WINAPI store_context3_GetStoreQueueItemsAsync(void *iface, void *ids, void **out)
{
    return store_async_op_create(STORE_ASYNC_DEFAULT, out);
}

static HRESULT WINAPI store_context3_RequestDownloadAndInstallStorePackagesWithOptionsAsync(void *iface, void *ids, void *opts, void **out)
{
    return store_async_op_create(STORE_ASYNC_DEFAULT, out);
}

static HRESULT WINAPI store_context3_DownloadAndInstallStorePackagesAsync(void *iface, void *ids, void **out)
{
    return store_async_op_create(STORE_ASYNC_DEFAULT, out);
}

static HRESULT WINAPI store_context3_RequestUninstallStorePackageAsync(void *iface, void *pkg, void **out)
{
    return store_async_op_create(STORE_ASYNC_DEFAULT, out);
}

static HRESULT WINAPI store_context3_RequestUninstallStorePackageByStoreIdAsync(void *iface, HSTRING id, void **out)
{
    return store_async_op_create(STORE_ASYNC_DEFAULT, out);
}

static HRESULT WINAPI store_context3_UninstallStorePackageAsync(void *iface, void *pkg, void **out)
{
    return store_async_op_create(STORE_ASYNC_DEFAULT, out);
}

static HRESULT WINAPI store_context3_UninstallStorePackageByStoreIdAsync(void *iface, HSTRING id, void **out)
{
    return store_async_op_create(STORE_ASYNC_DEFAULT, out);
}

static const void *store_context3_vtbl[] =
{
    store_context3_QueryInterface,
    store_context3_AddRef,
    store_context3_Release,
    store_context_GetIids,
    store_context_GetRuntimeClassName,
    store_context_GetTrustLevel,
    /* IStoreContext3 methods (slots 6..19) */
    store_context3_get_CanSilentlyDownloadStorePackageUpdates,
    store_context3_TrySilentDownloadStorePackageUpdatesAsync,
    store_context3_TrySilentDownloadAndInstallStorePackageUpdatesAsync,
    store_context3_CanAcquireStoreLicenseForOptionalPackageAsync,
    store_context3_CanAcquireStoreLicenseAsync,
    store_context3_GetStoreProductsWithOptionsAsync,
    store_context3_GetAssociatedStoreQueueItemsAsync,
    store_context3_GetStoreQueueItemsAsync,
    store_context3_RequestDownloadAndInstallStorePackagesWithOptionsAsync,
    store_context3_DownloadAndInstallStorePackagesAsync,
    store_context3_RequestUninstallStorePackageAsync,
    store_context3_RequestUninstallStorePackageByStoreIdAsync,
    store_context3_UninstallStorePackageAsync,
    store_context3_UninstallStorePackageByStoreIdAsync,
};

/* ========================================================================= */
/* IStoreContext4 methods (slots 6..7)                                       */
/* ========================================================================= */
static HRESULT WINAPI store_context4_QueryInterface(void *iface, REFIID iid, void **out)
{
    return store_context_query_interface_internal(impl_from_store_context4(iface), iid, out);
}

static ULONG WINAPI store_context4_AddRef(void *iface)
{
    struct store_context *impl = impl_from_store_context4(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI store_context4_Release(void *iface)
{
    struct store_context *impl = impl_from_store_context4(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI store_context4_RequestRateAndReviewAppAsync(void *iface, void **out)
{
    TRACE("iface %p, out %p.\n", iface, out);
    return store_async_op_create(STORE_ASYNC_RATE_RESULT, out);
}

static HRESULT WINAPI store_context4_SetInstallOrderForAssociatedStoreQueueItemsAsync(void *iface, void *items, void **out)
{
    TRACE("iface %p, out %p.\n", iface, out);
    return store_async_op_create(STORE_ASYNC_DEFAULT, out);
}

static const void *store_context4_vtbl[] =
{
    store_context4_QueryInterface,
    store_context4_AddRef,
    store_context4_Release,
    store_context_GetIids,
    store_context_GetRuntimeClassName,
    store_context_GetTrustLevel,
    /* IStoreContext4 methods (slots 6..7) */
    store_context4_RequestRateAndReviewAppAsync,
    store_context4_SetInstallOrderForAssociatedStoreQueueItemsAsync,
};

static HRESULT store_context_create(void **out)
{
    struct store_context *impl;
    if (!out) return E_INVALIDARG;
    if (!(impl = calloc(1, sizeof(*impl)))) return E_OUTOFMEMORY;
    impl->IStoreContext_iface.lpVtbl = (void *)store_context_vtbl;
    impl->IStoreContext2_iface.lpVtbl = (void *)store_context2_vtbl;
    impl->IStoreContext3_iface.lpVtbl = (void *)store_context3_vtbl;
    impl->IStoreContext4_iface.lpVtbl = (void *)store_context4_vtbl;
    impl->ref = 1;
    *out = &impl->IStoreContext_iface;
    return S_OK;
}

/* ========================================================================= */
/* IStoreContextStatics factory                                              */
/* ========================================================================= */
struct store_context_factory
{
    IActivationFactory IActivationFactory_iface;
    IInspectable IStoreContextStatics_iface;
    LONG ref;
};

static inline struct store_context_factory *impl_from_store_factory(IActivationFactory *iface)
{
    return CONTAINING_RECORD(iface, struct store_context_factory, IActivationFactory_iface);
}

static inline struct store_context_factory *impl_from_store_statics(void *iface)
{
    return CONTAINING_RECORD(iface, struct store_context_factory, IStoreContextStatics_iface);
}

static HRESULT WINAPI store_factory_QueryInterface(IActivationFactory *iface, REFIID iid, void **out)
{
    struct store_context_factory *impl = impl_from_store_factory(iface);
    TRACE("iface %p, iid %s, out %p.\n", iface, debugstr_guid(iid), out);
    if (!out) return E_INVALIDARG;

    if (IsEqualIID(iid, &IID_IWeakReferenceSource_CsWinRT) ||
        IsEqualIID(iid, &IID_ILanguageExceptionErrorInfo) ||
        IsEqualIID(iid, &IID_IWeakReferenceSource) ||
        IsEqualIID(iid, &IID_IWeakReference) ||
        IsEqualIID(iid, &IID_IMarshal) ||
        IsEqualIID(iid, &IID_ICustomQueryInterface))
    {
        *out = NULL;
        return E_NOINTERFACE;
    }

    if (IsEqualIID(iid, &IID_IUnknown) ||
        IsEqualIID(iid, &IID_IInspectable) ||
        IsEqualIID(iid, &IID_IAgileObject) ||
        IsEqualIID(iid, &IID_IActivationFactory))
    {
        *out = &impl->IActivationFactory_iface;
        IUnknown_AddRef((IUnknown *)*out);
        return S_OK;
    }

    if (IsEqualIID(iid, &IID_IStoreContextStatics))
    {
        *out = &impl->IStoreContextStatics_iface;
        IUnknown_AddRef((IUnknown *)*out);
        return S_OK;
    }

    WARN("unsupported iid %s\n", debugstr_guid(iid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI store_factory_AddRef(IActivationFactory *iface)
{
    struct store_context_factory *impl = impl_from_store_factory(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI store_factory_Release(IActivationFactory *iface)
{
    struct store_context_factory *impl = impl_from_store_factory(iface);
    return InterlockedDecrement(&impl->ref);
}

static HRESULT WINAPI store_factory_GetIids(IActivationFactory *iface, ULONG *iid_count, IID **iids)
{
    if (!iid_count || !iids) return E_INVALIDARG;
    *iid_count = 0;
    *iids = NULL;
    return S_OK;
}

static HRESULT WINAPI store_factory_GetRuntimeClassName(IActivationFactory *iface, HSTRING *class_name)
{
    static const WCHAR name[] = L"Windows.Services.Store.StoreContext";
    return WindowsCreateString(name, ARRAY_SIZE(name) - 1, class_name);
}

static HRESULT WINAPI store_factory_GetTrustLevel(IActivationFactory *iface, TrustLevel *trust_level)
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI store_factory_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    TRACE("iface %p, instance %p.\n", iface, instance);
    return store_context_create((void **)instance);
}

static const struct IActivationFactoryVtbl store_factory_vtbl =
{
    store_factory_QueryInterface,
    store_factory_AddRef,
    store_factory_Release,
    store_factory_GetIids,
    store_factory_GetRuntimeClassName,
    store_factory_GetTrustLevel,
    store_factory_ActivateInstance,
};

static HRESULT WINAPI store_statics_QueryInterface(void *iface, REFIID iid, void **out)
{
    struct store_context_factory *impl = impl_from_store_statics(iface);
    return store_factory_QueryInterface(&impl->IActivationFactory_iface, iid, out);
}

static ULONG WINAPI store_statics_AddRef(void *iface)
{
    struct store_context_factory *impl = impl_from_store_statics(iface);
    return store_factory_AddRef(&impl->IActivationFactory_iface);
}

static ULONG WINAPI store_statics_Release(void *iface)
{
    struct store_context_factory *impl = impl_from_store_statics(iface);
    return store_factory_Release(&impl->IActivationFactory_iface);
}

static HRESULT WINAPI store_statics_GetIids(void *iface, ULONG *iid_count, IID **iids)
{
    struct store_context_factory *impl = impl_from_store_statics(iface);
    return store_factory_GetIids(&impl->IActivationFactory_iface, iid_count, iids);
}

static HRESULT WINAPI store_statics_GetRuntimeClassName(void *iface, HSTRING *class_name)
{
    struct store_context_factory *impl = impl_from_store_statics(iface);
    return store_factory_GetRuntimeClassName(&impl->IActivationFactory_iface, class_name);
}

static HRESULT WINAPI store_statics_GetTrustLevel(void *iface, TrustLevel *trust_level)
{
    struct store_context_factory *impl = impl_from_store_statics(iface);
    return store_factory_GetTrustLevel(&impl->IActivationFactory_iface, trust_level);
}

static HRESULT WINAPI store_statics_GetDefault(void *iface, void **out)
{
    TRACE("iface %p, out %p.\n", iface, out);
    return store_context_create(out);
}

static HRESULT WINAPI store_statics_GetForUser(void *iface, void *user, void **out)
{
    return store_statics_GetDefault(iface, out);
}

static const void *store_statics_vtbl[] =
{
    store_statics_QueryInterface,
    store_statics_AddRef,
    store_statics_Release,
    store_statics_GetIids,
    store_statics_GetRuntimeClassName,
    store_statics_GetTrustLevel,
    /* IStoreContextStatics methods */
    store_statics_GetDefault,
    store_statics_GetForUser,
};

static struct store_context_factory store_context_factory_obj =
{
    {&store_factory_vtbl},
    {(const struct IInspectableVtbl *)store_statics_vtbl},
    1
};

IActivationFactory *store_context_factory = &store_context_factory_obj.IActivationFactory_iface;

/* ========================================================================= */
/* Windows.ApplicationModel.Store.CurrentApp & LicenseInformation            */
/* ========================================================================= */
static const GUID IID_ICurrentAppStatics = {0xd52dc065, 0xda3f, 0x4685, {0x99, 0x5e, 0x9b, 0x48, 0x2e, 0xb5, 0xe6, 0x03}};
static const GUID IID_ILicenseInformation = {0x8eb7dc30, 0xf170, 0x4ed5, {0x8e, 0x21, 0x15, 0x16, 0xda, 0x3f, 0xd3, 0x67}};

struct license_info
{
    struct { const void *lpVtbl; } ILicenseInformation_iface;
    LONG ref;
};

static HRESULT WINAPI license_info_QI(void *This, REFIID riid, void **ppv)
{
    TRACE("This %p, riid %s, ppv %p\n", This, debugstr_guid(riid), ppv);
    if (!ppv) return E_POINTER;
    *ppv = This;
    return S_OK;
}
static ULONG WINAPI license_info_AddRef(void *This) { return 2; }
static ULONG WINAPI license_info_Release(void *This) { return 1; }
static HRESULT WINAPI license_info_GetIids(void *This, ULONG *count, IID **iids) { if (count) *count = 0; return S_OK; }
static HRESULT WINAPI license_info_GetRuntimeClassName(void *This, HSTRING *name)
{
    return WindowsCreateString(L"Windows.ApplicationModel.Store.LicenseInformation", 48, name);
}
static HRESULT WINAPI license_info_GetTrustLevel(void *This, TrustLevel *trust) { if (trust) *trust = BaseTrust; return S_OK; }

static HRESULT WINAPI license_info_get_IsActive(void *This, boolean *value)
{
    TRACE("This %p, value %p -> TRUE\n", This, value);
    if (value && (ULONG_PTR)value > 0x10000 && !IsBadWritePtr(value, sizeof(*value)))
        *value = TRUE;
    return S_OK;
}

static HRESULT WINAPI license_info_get_IsTrial(void *This, boolean *value)
{
    TRACE("This %p, value %p -> FALSE\n", This, value);
    if (value && (ULONG_PTR)value > 0x10000 && !IsBadWritePtr(value, sizeof(*value)))
        *value = FALSE;
    return S_OK;
}

static HRESULT WINAPI license_info_get_ExpirationDate(void *This, void *value)
{
    if (value && (ULONG_PTR)value > 0x10000 && !IsBadWritePtr(value, 8))
        memset(value, 0, 8);
    return S_OK;
}

static HRESULT WINAPI license_info_get_LicenseId(void *This, HSTRING *value)
{
    return WindowsCreateString(L"StandardLicense", 15, value);
}

static HRESULT WINAPI license_info_add_LicenseChanged(void *This, void *handler)
{
    TRACE("This %p, handler %p\n", This, handler);
    return S_OK;
}

static HRESULT WINAPI license_info_remove_LicenseChanged(void *This, EventRegistrationToken token)
{
    return S_OK;
}

static HRESULT WINAPI license_info_slot(void *This, void *arg1, void *arg2)
{
    TRACE("This %p stub!\n", This);
    if (arg1 && (ULONG_PTR)arg1 > 0x10000 && !IsBadWritePtr(arg1, sizeof(void *)))
        *(void **)arg1 = NULL;
    return S_OK;
}

static void *license_info_vtbl[50];
static struct license_info global_license_info;

static void init_license_info_vtbl(void)
{
    int i;
    if (license_info_vtbl[0]) return;
    license_info_vtbl[0] = license_info_QI;
    license_info_vtbl[1] = license_info_AddRef;
    license_info_vtbl[2] = license_info_Release;
    license_info_vtbl[3] = license_info_GetIids;
    license_info_vtbl[4] = license_info_GetRuntimeClassName;
    license_info_vtbl[5] = license_info_GetTrustLevel;
    license_info_vtbl[6] = license_info_add_LicenseChanged;    /* offset 0x30 */
    license_info_vtbl[7] = license_info_get_IsActive;          /* offset 0x38 */
    license_info_vtbl[8] = license_info_get_IsTrial;           /* offset 0x40 */
    license_info_vtbl[9] = license_info_get_ExpirationDate;    /* offset 0x48 */
    license_info_vtbl[10] = license_info_add_LicenseChanged;   /* offset 0x50 */
    license_info_vtbl[11] = license_info_remove_LicenseChanged;/* offset 0x58 */
    for (i = 12; i < 50; i++)
        license_info_vtbl[i] = license_info_slot;
    global_license_info.ILicenseInformation_iface.lpVtbl = license_info_vtbl;
    global_license_info.ref = 1;
}

static HRESULT WINAPI current_app_fac_QI(IActivationFactory *iface, REFIID riid, void **ppv)
{
    TRACE("iface %p, riid %s, ppv %p\n", iface, debugstr_guid(riid), ppv);
    if (!ppv) return E_POINTER;
    *ppv = NULL;

    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IActivationFactory) ||
        IsEqualGUID(riid, &IID_ICurrentAppStatics))
    {
        *ppv = iface;
        return S_OK;
    }
    FIXME("unhandled QI %s\n", debugstr_guid(riid));
    return E_NOINTERFACE;
}
static ULONG WINAPI current_app_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI current_app_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI current_app_fac_GetIids(IActivationFactory *iface, ULONG *count, IID **iids) { if (count) *count = 0; return S_OK; }
static HRESULT WINAPI current_app_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *name)
{
    return WindowsCreateString(L"Windows.ApplicationModel.Store.CurrentApp", 39, name);
}
static HRESULT WINAPI current_app_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *trust) { if (trust) *trust = BaseTrust; return S_OK; }

static HRESULT WINAPI current_app_fac_get_LicenseInformation(void *iface, void **value)
{
    init_license_info_vtbl();
    TRACE("iface %p, value %p -> %p\n", iface, value, &global_license_info);
    if (!value) return E_POINTER;
    *value = &global_license_info;
    return S_OK;
}

static HRESULT WINAPI current_app_fac_get_LinkUri(void *iface, void **value)
{
    if (value) *value = NULL;
    return S_OK;
}

static HRESULT WINAPI current_app_fac_get_AppId(void *iface, GUID *value)
{
    if (value) memset(value, 0, sizeof(*value));
    return S_OK;
}

static HRESULT WINAPI current_app_fac_slot(void *iface, void **value)
{
    init_license_info_vtbl();
    TRACE("iface %p, value %p stub!\n", iface, value);
    if (value && (ULONG_PTR)value > 0x10000 && !IsBadWritePtr(value, sizeof(void *)))
        *value = &global_license_info;
    return S_OK;
}

static void *current_app_fac_vtbl[50];
static struct { void *vtbl; LONG ref; } global_current_app_factory;

static void init_current_app_fac_vtbl(void)
{
    int i;
    if (current_app_fac_vtbl[0]) return;
    init_license_info_vtbl();
    current_app_fac_vtbl[0] = current_app_fac_QI;
    current_app_fac_vtbl[1] = current_app_fac_AddRef;
    current_app_fac_vtbl[2] = current_app_fac_Release;
    current_app_fac_vtbl[3] = current_app_fac_GetIids;
    current_app_fac_vtbl[4] = current_app_fac_GetRuntimeClassName;
    current_app_fac_vtbl[5] = current_app_fac_GetTrustLevel;
    current_app_fac_vtbl[6] = current_app_fac_get_LicenseInformation; /* offset 0x30 */
    current_app_fac_vtbl[7] = current_app_fac_get_LinkUri;            /* offset 0x38 */
    current_app_fac_vtbl[8] = current_app_fac_get_AppId;              /* offset 0x40 */
    for (i = 9; i < 50; i++)
        current_app_fac_vtbl[i] = current_app_fac_slot;
    current_app_fac_vtbl[12] = current_app_fac_get_LicenseInformation; /* offset 0x60 (C++/WinRT CurrentApp::LicenseInformation) */
    global_current_app_factory.vtbl = current_app_fac_vtbl;
    global_current_app_factory.ref = 1;
}

static IActivationFactory *get_current_app_factory(void)
{
    init_current_app_fac_vtbl();
    return (IActivationFactory *)&global_current_app_factory;
}
