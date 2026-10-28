/*
 * CryptoWinRT DataProtectionProvider Implementation
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

#include <assert.h>
#include "wine/debug.h"
#include "objbase.h"
#include "wincrypt.h"

#define WIDL_using_Windows_Security_Cryptography
#include "windows.security.cryptography.h"
#include "robuffer.h"

WINE_DEFAULT_DEBUG_CHANNEL(crypto);

/* IDataProtectionProvider: GUID {09639948-ed22-4270-bd1c-6d72c00f8787} */
static const GUID IID_IDataProtectionProvider = {0x09639948, 0xed22, 0x4270, {0xbd, 0x1c, 0x6d, 0x72, 0xc0, 0x0f, 0x87, 0x87}};

/* IDataProtectionProviderFactory: GUID {adf33dac-4932-4cdf-ac41-7214333514ca} */
static const GUID IID_IDataProtectionProviderFactory = {0xadf33dac, 0x4932, 0x4cdf, {0xac, 0x41, 0x72, 0x14, 0x33, 0x35, 0x14, 0xca}};

#define RuntimeClass_Windows_Security_Cryptography_DataProtection_DataProtectionProvider \
    L"Windows.Security.Cryptography.DataProtection.DataProtectionProvider"

typedef struct IDataProtectionProvider IDataProtectionProvider;
typedef struct IDataProtectionProviderFactory IDataProtectionProviderFactory;

typedef struct IDataProtectionProviderVtbl
{
    /* IUnknown */
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IDataProtectionProvider *This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IDataProtectionProvider *This);
    ULONG (STDMETHODCALLTYPE *Release)(IDataProtectionProvider *This);
    /* IInspectable */
    HRESULT (STDMETHODCALLTYPE *GetIids)(IDataProtectionProvider *This, ULONG *iidCount, IID **iids);
    HRESULT (STDMETHODCALLTYPE *GetRuntimeClassName)(IDataProtectionProvider *This, HSTRING *className);
    HRESULT (STDMETHODCALLTYPE *GetTrustLevel)(IDataProtectionProvider *This, TrustLevel *trustLevel);
    /* IDataProtectionProvider */
    HRESULT (STDMETHODCALLTYPE *ProtectAsync)(IDataProtectionProvider *This, IBuffer *data, IAsyncOperation_IBuffer **value);
    HRESULT (STDMETHODCALLTYPE *UnprotectAsync)(IDataProtectionProvider *This, IBuffer *data, IAsyncOperation_IBuffer **value);
    HRESULT (STDMETHODCALLTYPE *ProtectStreamAsync)(IDataProtectionProvider *This, IInputStream *src, IOutputStream *dest, IAsyncAction **value);
    HRESULT (STDMETHODCALLTYPE *UnprotectStreamAsync)(IDataProtectionProvider *This, IInputStream *src, IOutputStream *dest, IAsyncAction **value);
} IDataProtectionProviderVtbl;

struct IDataProtectionProvider
{
    const IDataProtectionProviderVtbl *lpVtbl;
};

typedef struct IDataProtectionProviderFactoryVtbl
{
    /* IUnknown */
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IDataProtectionProviderFactory *This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IDataProtectionProviderFactory *This);
    ULONG (STDMETHODCALLTYPE *Release)(IDataProtectionProviderFactory *This);
    /* IInspectable */
    HRESULT (STDMETHODCALLTYPE *GetIids)(IDataProtectionProviderFactory *This, ULONG *iidCount, IID **iids);
    HRESULT (STDMETHODCALLTYPE *GetRuntimeClassName)(IDataProtectionProviderFactory *This, HSTRING *className);
    HRESULT (STDMETHODCALLTYPE *GetTrustLevel)(IDataProtectionProviderFactory *This, TrustLevel *trustLevel);
    /* IDataProtectionProviderFactory */
    HRESULT (STDMETHODCALLTYPE *CreateOverloadExplicit)(IDataProtectionProviderFactory *This, HSTRING protectionDescriptor, IDataProtectionProvider **value);
} IDataProtectionProviderFactoryVtbl;

struct IDataProtectionProviderFactory
{
    const IDataProtectionProviderFactoryVtbl *lpVtbl;
};

struct dataprotection_provider
{
    IDataProtectionProvider IDataProtectionProvider_iface;
    LONG ref;
    HSTRING descriptor;
};

static inline struct dataprotection_provider *impl_from_IDataProtectionProvider(IDataProtectionProvider *iface)
{
    return CONTAINING_RECORD(iface, struct dataprotection_provider, IDataProtectionProvider_iface);
}

static HRESULT WINAPI dataprotection_provider_QueryInterface(IDataProtectionProvider *iface, REFIID iid, void **out)
{
    struct dataprotection_provider *impl = impl_from_IDataProtectionProvider(iface);

    TRACE("iface %p, iid %s, out %p.\n", iface, debugstr_guid(iid), out);

    if (IsEqualGUID(iid, &IID_IUnknown) ||
        IsEqualGUID(iid, &IID_IInspectable) ||
        IsEqualGUID(iid, &IID_IAgileObject) ||
        IsEqualGUID(iid, &IID_IDataProtectionProvider))
    {
        IInspectable_AddRef((*out = &impl->IDataProtectionProvider_iface));
        return S_OK;
    }

    FIXME("%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid(iid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI dataprotection_provider_AddRef(IDataProtectionProvider *iface)
{
    struct dataprotection_provider *impl = impl_from_IDataProtectionProvider(iface);
    ULONG ref = InterlockedIncrement(&impl->ref);
    TRACE("iface %p, ref %lu.\n", iface, ref);
    return ref;
}

static ULONG WINAPI dataprotection_provider_Release(IDataProtectionProvider *iface)
{
    struct dataprotection_provider *impl = impl_from_IDataProtectionProvider(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    TRACE("iface %p, ref %lu.\n", iface, ref);
    if (!ref)
    {
        if (impl->descriptor) WindowsDeleteString(impl->descriptor);
        free(impl);
    }
    return ref;
}

static HRESULT WINAPI dataprotection_provider_GetIids(IDataProtectionProvider *iface, ULONG *iid_count, IID **iids)
{
    FIXME("iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids);
    return E_NOTIMPL;
}

static HRESULT WINAPI dataprotection_provider_GetRuntimeClassName(IDataProtectionProvider *iface, HSTRING *class_name)
{
    return WindowsCreateString(RuntimeClass_Windows_Security_Cryptography_DataProtection_DataProtectionProvider,
                               wcslen(RuntimeClass_Windows_Security_Cryptography_DataProtection_DataProtectionProvider),
                               class_name);
}

static HRESULT WINAPI dataprotection_provider_GetTrustLevel(IDataProtectionProvider *iface, TrustLevel *trust_level)
{
    FIXME("iface %p, trust_level %p stub!\n", iface, trust_level);
    return E_NOTIMPL;
}

static HRESULT protect_async_callback(IUnknown *invoker, IUnknown *param, PROPVARIANT *result, BOOL called_async)
{
    IBuffer *data = (IBuffer *)param;
    IBufferByteAccess *byte_access;
    DATA_BLOB in_blob, out_blob;
    UINT32 length = 0;
    BYTE *buffer_data = NULL;
    ICryptographicBufferStatics *crypto_statics = NULL;
    IBuffer *out_buf = NULL;
    HRESULT hr;

    TRACE("invoker %p, param %p, result %p, called_async %d.\n", invoker, param, result, called_async);

    if (!data) return E_INVALIDARG;

    hr = IBuffer_get_Length(data, &length);
    if (FAILED(hr)) return hr;

    if (SUCCEEDED(IBuffer_QueryInterface(data, &IID_IBufferByteAccess, (void **)&byte_access)))
    {
        IBufferByteAccess_Buffer(byte_access, &buffer_data);
        IBufferByteAccess_Release(byte_access);
    }

    in_blob.pbData = buffer_data;
    in_blob.cbData = length;

    if (CryptProtectData(&in_blob, L"WinRT DataProtection", NULL, NULL, NULL, 0, &out_blob))
    {
        hr = IActivationFactory_QueryInterface(cryptobuffer_activation_factory, &IID_ICryptographicBufferStatics, (void **)&crypto_statics);
        if (SUCCEEDED(hr) && crypto_statics)
        {
            hr = ICryptographicBufferStatics_CreateFromByteArray(crypto_statics, out_blob.cbData, out_blob.pbData, &out_buf);
            ICryptographicBufferStatics_Release(crypto_statics);
        }
        LocalFree(out_blob.pbData);

        if (SUCCEEDED(hr) && out_buf)
        {
            result->vt = VT_UNKNOWN;
            result->punkVal = (IUnknown *)out_buf;
            return S_OK;
        }
    }

    IBuffer_AddRef(data);
    result->vt = VT_UNKNOWN;
    result->punkVal = (IUnknown *)data;
    return S_OK;
}

static HRESULT unprotect_async_callback(IUnknown *invoker, IUnknown *param, PROPVARIANT *result, BOOL called_async)
{
    IBuffer *data = (IBuffer *)param;
    IBufferByteAccess *byte_access;
    DATA_BLOB in_blob, out_blob;
    UINT32 length = 0;
    BYTE *buffer_data = NULL;
    ICryptographicBufferStatics *crypto_statics = NULL;
    IBuffer *out_buf = NULL;
    HRESULT hr;

    TRACE("invoker %p, param %p, result %p, called_async %d.\n", invoker, param, result, called_async);

    if (!data) return E_INVALIDARG;

    hr = IBuffer_get_Length(data, &length);
    if (FAILED(hr)) return hr;

    if (SUCCEEDED(IBuffer_QueryInterface(data, &IID_IBufferByteAccess, (void **)&byte_access)))
    {
        IBufferByteAccess_Buffer(byte_access, &buffer_data);
        IBufferByteAccess_Release(byte_access);
    }

    in_blob.pbData = buffer_data;
    in_blob.cbData = length;

    if (CryptUnprotectData(&in_blob, NULL, NULL, NULL, NULL, 0, &out_blob))
    {
        hr = IActivationFactory_QueryInterface(cryptobuffer_activation_factory, &IID_ICryptographicBufferStatics, (void **)&crypto_statics);
        if (SUCCEEDED(hr) && crypto_statics)
        {
            hr = ICryptographicBufferStatics_CreateFromByteArray(crypto_statics, out_blob.cbData, out_blob.pbData, &out_buf);
            ICryptographicBufferStatics_Release(crypto_statics);
        }
        LocalFree(out_blob.pbData);

        if (SUCCEEDED(hr) && out_buf)
        {
            result->vt = VT_UNKNOWN;
            result->punkVal = (IUnknown *)out_buf;
            return S_OK;
        }
    }

    IBuffer_AddRef(data);
    result->vt = VT_UNKNOWN;
    result->punkVal = (IUnknown *)data;
    return S_OK;
}

static HRESULT WINAPI dataprotection_provider_ProtectAsync(IDataProtectionProvider *iface, IBuffer *data, IAsyncOperation_IBuffer **value)
{
    TRACE("iface %p, data %p, value %p.\n", iface, data, value);
    return async_operation_inspectable_create(&IID_IAsyncOperation_IBuffer, (IUnknown *)iface, (IUnknown *)data, protect_async_callback, (IAsyncOperation_IInspectable **)value);
}

static HRESULT WINAPI dataprotection_provider_UnprotectAsync(IDataProtectionProvider *iface, IBuffer *data, IAsyncOperation_IBuffer **value)
{
    TRACE("iface %p, data %p, value %p.\n", iface, data, value);
    return async_operation_inspectable_create(&IID_IAsyncOperation_IBuffer, (IUnknown *)iface, (IUnknown *)data, unprotect_async_callback, (IAsyncOperation_IInspectable **)value);
}

static HRESULT dummy_stream_async_callback(IUnknown *invoker, IUnknown *param, PROPVARIANT *result, BOOL called_async)
{
    return S_OK;
}

static HRESULT WINAPI dataprotection_provider_ProtectStreamAsync(IDataProtectionProvider *iface, IInputStream *src, IOutputStream *dest, IAsyncAction **value)
{
    FIXME("iface %p, src %p, dest %p, value %p stub!\n", iface, src, dest, value);
    return async_action_create((IUnknown *)iface, dummy_stream_async_callback, value);
}

static HRESULT WINAPI dataprotection_provider_UnprotectStreamAsync(IDataProtectionProvider *iface, IInputStream *src, IOutputStream *dest, IAsyncAction **value)
{
    FIXME("iface %p, src %p, dest %p, value %p stub!\n", iface, src, dest, value);
    return async_action_create((IUnknown *)iface, dummy_stream_async_callback, value);
}

static const struct IDataProtectionProviderVtbl dataprotection_provider_vtbl =
{
    dataprotection_provider_QueryInterface,
    dataprotection_provider_AddRef,
    dataprotection_provider_Release,
    dataprotection_provider_GetIids,
    dataprotection_provider_GetRuntimeClassName,
    dataprotection_provider_GetTrustLevel,
    dataprotection_provider_ProtectAsync,
    dataprotection_provider_UnprotectAsync,
    dataprotection_provider_ProtectStreamAsync,
    dataprotection_provider_UnprotectStreamAsync,
};

static HRESULT dataprotection_provider_create(HSTRING descriptor, IDataProtectionProvider **out)
{
    struct dataprotection_provider *impl;

    *out = NULL;
    if (!(impl = calloc(1, sizeof(*impl)))) return E_OUTOFMEMORY;
    impl->IDataProtectionProvider_iface.lpVtbl = &dataprotection_provider_vtbl;
    impl->ref = 1;
    if (descriptor) WindowsDuplicateString(descriptor, &impl->descriptor);

    *out = &impl->IDataProtectionProvider_iface;
    TRACE("created DataProtectionProvider %p\n", *out);
    return S_OK;
}

struct dataprotection_factory
{
    IActivationFactory IActivationFactory_iface;
    IDataProtectionProviderFactory IDataProtectionProviderFactory_iface;
    LONG ref;
};

static inline struct dataprotection_factory *impl_from_IActivationFactory(IActivationFactory *iface)
{
    return CONTAINING_RECORD(iface, struct dataprotection_factory, IActivationFactory_iface);
}

static inline struct dataprotection_factory *impl_from_IDataProtectionProviderFactory(IDataProtectionProviderFactory *iface)
{
    return CONTAINING_RECORD(iface, struct dataprotection_factory, IDataProtectionProviderFactory_iface);
}

static HRESULT WINAPI dataprotection_factory_QueryInterface(IActivationFactory *iface, REFIID iid, void **out)
{
    struct dataprotection_factory *impl = impl_from_IActivationFactory(iface);

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

    if (IsEqualGUID(iid, &IID_IDataProtectionProviderFactory))
    {
        IUnknown_AddRef(iface);
        *out = &impl->IDataProtectionProviderFactory_iface;
        return S_OK;
    }

    FIXME("%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid(iid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI dataprotection_factory_AddRef(IActivationFactory *iface)
{
    struct dataprotection_factory *impl = impl_from_IActivationFactory(iface);
    ULONG ref = InterlockedIncrement(&impl->ref);
    TRACE("iface %p, ref %lu.\n", iface, ref);
    return ref;
}

static ULONG WINAPI dataprotection_factory_Release(IActivationFactory *iface)
{
    struct dataprotection_factory *impl = impl_from_IActivationFactory(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    TRACE("iface %p, ref %lu.\n", iface, ref);
    return ref;
}

static HRESULT WINAPI dataprotection_factory_GetIids(IActivationFactory *iface, ULONG *iid_count, IID **iids)
{
    FIXME("iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids);
    return E_NOTIMPL;
}

static HRESULT WINAPI dataprotection_factory_GetRuntimeClassName(IActivationFactory *iface, HSTRING *class_name)
{
    return WindowsCreateString(RuntimeClass_Windows_Security_Cryptography_DataProtection_DataProtectionProvider,
                               wcslen(RuntimeClass_Windows_Security_Cryptography_DataProtection_DataProtectionProvider),
                               class_name);
}

static HRESULT WINAPI dataprotection_factory_GetTrustLevel(IActivationFactory *iface, TrustLevel *trust_level)
{
    FIXME("iface %p, trust_level %p stub!\n", iface, trust_level);
    return E_NOTIMPL;
}

static HRESULT WINAPI dataprotection_factory_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    IDataProtectionProvider *provider;
    HRESULT hr;

    TRACE("iface %p, instance %p.\n", iface, instance);

    hr = dataprotection_provider_create(NULL, &provider);
    if (SUCCEEDED(hr))
        *instance = (IInspectable *)provider;
    return hr;
}

static const struct IActivationFactoryVtbl dataprotection_factory_vtbl =
{
    dataprotection_factory_QueryInterface,
    dataprotection_factory_AddRef,
    dataprotection_factory_Release,
    dataprotection_factory_GetIids,
    dataprotection_factory_GetRuntimeClassName,
    dataprotection_factory_GetTrustLevel,
    dataprotection_factory_ActivateInstance,
};

static HRESULT WINAPI dataprotection_factory_overload_QueryInterface(IDataProtectionProviderFactory *iface, REFIID iid, void **out)
{
    struct dataprotection_factory *impl = impl_from_IDataProtectionProviderFactory(iface);
    return IActivationFactory_QueryInterface(&impl->IActivationFactory_iface, iid, out);
}

static ULONG WINAPI dataprotection_factory_overload_AddRef(IDataProtectionProviderFactory *iface)
{
    struct dataprotection_factory *impl = impl_from_IDataProtectionProviderFactory(iface);
    return IActivationFactory_AddRef(&impl->IActivationFactory_iface);
}

static ULONG WINAPI dataprotection_factory_overload_Release(IDataProtectionProviderFactory *iface)
{
    struct dataprotection_factory *impl = impl_from_IDataProtectionProviderFactory(iface);
    return IActivationFactory_Release(&impl->IActivationFactory_iface);
}

static HRESULT WINAPI dataprotection_factory_overload_GetIids(IDataProtectionProviderFactory *iface, ULONG *iid_count, IID **iids)
{
    FIXME("iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids);
    return E_NOTIMPL;
}

static HRESULT WINAPI dataprotection_factory_overload_GetRuntimeClassName(IDataProtectionProviderFactory *iface, HSTRING *class_name)
{
    return WindowsCreateString(RuntimeClass_Windows_Security_Cryptography_DataProtection_DataProtectionProvider,
                               wcslen(RuntimeClass_Windows_Security_Cryptography_DataProtection_DataProtectionProvider),
                               class_name);
}

static HRESULT WINAPI dataprotection_factory_overload_GetTrustLevel(IDataProtectionProviderFactory *iface, TrustLevel *trust_level)
{
    FIXME("iface %p, trust_level %p stub!\n", iface, trust_level);
    return E_NOTIMPL;
}

static HRESULT WINAPI dataprotection_factory_CreateOverloadExplicit(IDataProtectionProviderFactory *iface, HSTRING protectionDescriptor, IDataProtectionProvider **value)
{
    TRACE("iface %p, descriptor %s, value %p.\n", iface, debugstr_hstring(protectionDescriptor), value);
    return dataprotection_provider_create(protectionDescriptor, value);
}

static const struct IDataProtectionProviderFactoryVtbl dataprotection_factory_overload_vtbl =
{
    dataprotection_factory_overload_QueryInterface,
    dataprotection_factory_overload_AddRef,
    dataprotection_factory_overload_Release,
    dataprotection_factory_overload_GetIids,
    dataprotection_factory_overload_GetRuntimeClassName,
    dataprotection_factory_overload_GetTrustLevel,
    dataprotection_factory_CreateOverloadExplicit,
};

static struct dataprotection_factory dataprotection_factory =
{
    .IActivationFactory_iface.lpVtbl = &dataprotection_factory_vtbl,
    .IDataProtectionProviderFactory_iface.lpVtbl = &dataprotection_factory_overload_vtbl,
    .ref = 1,
};

IActivationFactory *dataprotection_activation_factory = &dataprotection_factory.IActivationFactory_iface;
