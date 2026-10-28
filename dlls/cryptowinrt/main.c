/*
 * Copyright 2022 Nikolay Sivov for CodeWeavers
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

#include "initguid.h"
#include "private.h"

#include <assert.h>

#include "wine/debug.h"
#include "objbase.h"

#include "bcrypt.h"
#include "wincrypt.h"

#define WIDL_using_Windows_Security_Cryptography
#include "windows.security.cryptography.h"
#include "robuffer.h"

WINE_DEFAULT_DEBUG_CHANNEL(crypto);

struct cryptobuffer_factory
{
    IActivationFactory IActivationFactory_iface;
    ICryptographicBufferStatics ICryptographicBufferStatics_iface;
    LONG refcount;
};

static inline struct cryptobuffer_factory *impl_from_IActivationFactory(IActivationFactory *iface)
{
    return CONTAINING_RECORD(iface, struct cryptobuffer_factory, IActivationFactory_iface);
}

static HRESULT STDMETHODCALLTYPE cryptobuffer_factory_QueryInterface(
        IActivationFactory *iface, REFIID iid, void **out)
{
    struct cryptobuffer_factory *factory = impl_from_IActivationFactory(iface);

    TRACE("iface %p, iid %s, out %p.\n", iface, debugstr_guid(iid), out);

    if (IsEqualGUID(iid, &IID_IUnknown) ||
        IsEqualGUID(iid, &IID_IInspectable) ||
        IsEqualGUID(iid, &IID_IAgileObject) ||
        IsEqualGUID(iid, &IID_IActivationFactory))
    {
        IUnknown_AddRef(iface);
        *out = &factory->IActivationFactory_iface;
        return S_OK;
    }

    if (IsEqualGUID(iid, &IID_ICryptographicBufferStatics))
    {
        IUnknown_AddRef(iface);
        *out = &factory->ICryptographicBufferStatics_iface;
        return S_OK;
    }

    FIXME("%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid(iid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG STDMETHODCALLTYPE cryptobuffer_factory_AddRef(IActivationFactory *iface)
{
    struct cryptobuffer_factory *factory = impl_from_IActivationFactory(iface);
    ULONG refcount = InterlockedIncrement(&factory->refcount);

    TRACE("iface %p, refcount %lu.\n", iface, refcount);

    return refcount;
}

static ULONG STDMETHODCALLTYPE cryptobuffer_factory_Release(IActivationFactory *iface)
{
    struct cryptobuffer_factory *factory = impl_from_IActivationFactory(iface);
    ULONG refcount = InterlockedDecrement(&factory->refcount);

    TRACE("iface %p, refcount %lu.\n", iface, refcount);

    return refcount;
}

static HRESULT STDMETHODCALLTYPE cryptobuffer_factory_GetIids(
        IActivationFactory *iface, ULONG *iid_count, IID **iids)
{
    FIXME("iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids);
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE cryptobuffer_factory_GetRuntimeClassName(
        IActivationFactory *iface, HSTRING *class_name)
{
    FIXME("iface %p, class_name %p stub!\n", iface, class_name);
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE cryptobuffer_factory_GetTrustLevel(
        IActivationFactory *iface, TrustLevel *trust_level)
{
    FIXME("iface %p, trust_level %p stub!\n", iface, trust_level);
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE cryptobuffer_factory_ActivateInstance(
        IActivationFactory *iface, IInspectable **instance)
{
    FIXME("iface %p, instance %p stub!\n", iface, instance);
    return E_NOTIMPL;
}

static const struct IActivationFactoryVtbl cryptobuffer_factory_vtbl =
{
    cryptobuffer_factory_QueryInterface,
    cryptobuffer_factory_AddRef,
    cryptobuffer_factory_Release,
    /* IInspectable methods */
    cryptobuffer_factory_GetIids,
    cryptobuffer_factory_GetRuntimeClassName,
    cryptobuffer_factory_GetTrustLevel,
    /* IActivationFactory methods */
    cryptobuffer_factory_ActivateInstance,
};

DEFINE_IINSPECTABLE(cryptobuffer_statics, ICryptographicBufferStatics, struct cryptobuffer_factory, IActivationFactory_iface);

struct cryptowinrt_buffer
{
    IBuffer IBuffer_iface;
    IBufferByteAccess IBufferByteAccess_iface;
    LONG ref;
    UINT32 capacity;
    UINT32 length;
    BYTE data[];
};

static inline struct cryptowinrt_buffer *impl_from_IBuffer(IBuffer *iface)
{
    return CONTAINING_RECORD(iface, struct cryptowinrt_buffer, IBuffer_iface);
}

static inline struct cryptowinrt_buffer *impl_from_IBufferByteAccess(IBufferByteAccess *iface)
{
    return CONTAINING_RECORD(iface, struct cryptowinrt_buffer, IBufferByteAccess_iface);
}

static HRESULT WINAPI cryptowinrt_buffer_QueryInterface(IBuffer *iface, REFIID iid, void **out)
{
    struct cryptowinrt_buffer *impl = impl_from_IBuffer(iface);

    if (IsEqualGUID(iid, &IID_IUnknown) ||
        IsEqualGUID(iid, &IID_IInspectable) ||
        IsEqualGUID(iid, &IID_IAgileObject) ||
        IsEqualGUID(iid, &IID_IBuffer))
    {
        *out = &impl->IBuffer_iface;
        IBuffer_AddRef(&impl->IBuffer_iface);
        return S_OK;
    }

    if (IsEqualGUID(iid, &IID_IBufferByteAccess))
    {
        *out = &impl->IBufferByteAccess_iface;
        IBuffer_AddRef(&impl->IBuffer_iface);
        return S_OK;
    }

    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI cryptowinrt_buffer_AddRef(IBuffer *iface)
{
    struct cryptowinrt_buffer *impl = impl_from_IBuffer(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI cryptowinrt_buffer_Release(IBuffer *iface)
{
    struct cryptowinrt_buffer *impl = impl_from_IBuffer(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI cryptowinrt_buffer_GetIids(IBuffer *iface, ULONG *iid_count, IID **iids)
{
    *iid_count = 0;
    *iids = NULL;
    return S_OK;
}

static HRESULT WINAPI cryptowinrt_buffer_GetRuntimeClassName(IBuffer *iface, HSTRING *class_name)
{
    return WindowsCreateString(L"Windows.Storage.Streams.Buffer", wcslen(L"Windows.Storage.Streams.Buffer"), class_name);
}

static HRESULT WINAPI cryptowinrt_buffer_GetTrustLevel(IBuffer *iface, TrustLevel *trust_level)
{
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI cryptowinrt_buffer_get_Capacity(IBuffer *iface, UINT32 *value)
{
    struct cryptowinrt_buffer *impl = impl_from_IBuffer(iface);
    *value = impl->capacity;
    return S_OK;
}

static HRESULT WINAPI cryptowinrt_buffer_get_Length(IBuffer *iface, UINT32 *value)
{
    struct cryptowinrt_buffer *impl = impl_from_IBuffer(iface);
    *value = impl->length;
    return S_OK;
}

static HRESULT WINAPI cryptowinrt_buffer_put_Length(IBuffer *iface, UINT32 value)
{
    struct cryptowinrt_buffer *impl = impl_from_IBuffer(iface);
    if (value > impl->capacity) return E_INVALIDARG;
    impl->length = value;
    return S_OK;
}

static const struct IBufferVtbl cryptowinrt_buffer_vtbl =
{
    cryptowinrt_buffer_QueryInterface,
    cryptowinrt_buffer_AddRef,
    cryptowinrt_buffer_Release,
    cryptowinrt_buffer_GetIids,
    cryptowinrt_buffer_GetRuntimeClassName,
    cryptowinrt_buffer_GetTrustLevel,
    cryptowinrt_buffer_get_Capacity,
    cryptowinrt_buffer_get_Length,
    cryptowinrt_buffer_put_Length,
};

static HRESULT WINAPI cryptowinrt_buffer_byte_access_QueryInterface(IBufferByteAccess *iface, REFIID iid, void **out)
{
    struct cryptowinrt_buffer *impl = impl_from_IBufferByteAccess(iface);
    return IBuffer_QueryInterface(&impl->IBuffer_iface, iid, out);
}

static ULONG WINAPI cryptowinrt_buffer_byte_access_AddRef(IBufferByteAccess *iface)
{
    struct cryptowinrt_buffer *impl = impl_from_IBufferByteAccess(iface);
    return IBuffer_AddRef(&impl->IBuffer_iface);
}

static ULONG WINAPI cryptowinrt_buffer_byte_access_Release(IBufferByteAccess *iface)
{
    struct cryptowinrt_buffer *impl = impl_from_IBufferByteAccess(iface);
    return IBuffer_Release(&impl->IBuffer_iface);
}

static HRESULT WINAPI cryptowinrt_buffer_byte_access_Buffer(IBufferByteAccess *iface, byte **value)
{
    struct cryptowinrt_buffer *impl = impl_from_IBufferByteAccess(iface);
    *value = impl->data;
    return S_OK;
}

static const struct IBufferByteAccessVtbl cryptowinrt_buffer_byte_access_vtbl =
{
    cryptowinrt_buffer_byte_access_QueryInterface,
    cryptowinrt_buffer_byte_access_AddRef,
    cryptowinrt_buffer_byte_access_Release,
    cryptowinrt_buffer_byte_access_Buffer,
};

static HRESULT buffer_create(UINT32 capacity, IBuffer **value)
{
    struct cryptowinrt_buffer *impl;

    if (!value) return E_POINTER;
    if (!(impl = calloc(1, sizeof(*impl) + capacity))) return E_OUTOFMEMORY;
    impl->IBuffer_iface.lpVtbl = &cryptowinrt_buffer_vtbl;
    impl->IBufferByteAccess_iface.lpVtbl = &cryptowinrt_buffer_byte_access_vtbl;
    impl->ref = 1;
    impl->capacity = capacity;
    impl->length = 0;
    *value = &impl->IBuffer_iface;
    return S_OK;
}

static HRESULT STDMETHODCALLTYPE cryptobuffer_statics_Compare(
        ICryptographicBufferStatics *iface, IBuffer *object1, IBuffer *object2, boolean *is_equal)
{
    IBufferByteAccess *acc1 = NULL, *acc2 = NULL;
    BYTE *d1 = NULL, *d2 = NULL;
    UINT32 l1 = 0, l2 = 0;

    TRACE("iface %p, object1 %p, object2 %p, is_equal %p.\n", iface, object1, object2, is_equal);

    if (!is_equal) return E_POINTER;
    *is_equal = FALSE;

    if (object1 == object2)
    {
        *is_equal = TRUE;
        return S_OK;
    }
    if (!object1 || !object2) return S_OK;

    IBuffer_get_Length(object1, &l1);
    IBuffer_get_Length(object2, &l2);
    if (l1 != l2) return S_OK;
    if (!l1)
    {
        *is_equal = TRUE;
        return S_OK;
    }

    IBuffer_QueryInterface(object1, &IID_IBufferByteAccess, (void **)&acc1);
    IBuffer_QueryInterface(object2, &IID_IBufferByteAccess, (void **)&acc2);
    if (acc1 && acc2)
    {
        IBufferByteAccess_Buffer(acc1, &d1);
        IBufferByteAccess_Buffer(acc2, &d2);
        if (d1 && d2 && !memcmp(d1, d2, l1))
            *is_equal = TRUE;
    }
    if (acc1) IBufferByteAccess_Release(acc1);
    if (acc2) IBufferByteAccess_Release(acc2);

    return S_OK;
}

static HRESULT STDMETHODCALLTYPE cryptobuffer_statics_GenerateRandom(
        ICryptographicBufferStatics *iface, UINT32 length, IBuffer **buffer)
{
    struct cryptowinrt_buffer *impl;
    HRESULT hr;

    TRACE("iface %p, length %u, buffer %p.\n", iface, length, buffer);

    if (!buffer) return E_POINTER;
    hr = buffer_create(length, buffer);
    if (FAILED(hr)) return hr;

    impl = impl_from_IBuffer(*buffer);
    if (length)
        BCryptGenRandom(NULL, (UCHAR *)impl->data, length, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    impl->length = length;

    return S_OK;
}

static HRESULT STDMETHODCALLTYPE cryptobuffer_statics_GenerateRandomNumber(
        ICryptographicBufferStatics *iface, UINT32 *value)
{
    TRACE("iface %p, value %p.\n", iface, value);

    BCryptGenRandom(NULL, (UCHAR *)value, sizeof(*value), BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    return S_OK;
}

static HRESULT STDMETHODCALLTYPE cryptobuffer_statics_CreateFromByteArray(
        ICryptographicBufferStatics *iface, UINT32 value_size, BYTE *value, IBuffer **buffer)
{
    struct cryptowinrt_buffer *impl;
    HRESULT hr;

    TRACE("iface %p, value_size %u, value %p, buffer %p.\n", iface, value_size, value, buffer);

    if (!buffer) return E_POINTER;
    hr = buffer_create(value_size, buffer);
    if (FAILED(hr)) return hr;

    impl = impl_from_IBuffer(*buffer);
    if (value_size && value)
        memcpy(impl->data, value, value_size);
    impl->length = value_size;

    return S_OK;
}

static HRESULT STDMETHODCALLTYPE cryptobuffer_statics_CopyToByteArray(
        ICryptographicBufferStatics *iface, IBuffer *buffer, UINT32 *value_size, BYTE **value)
{
    IBufferByteAccess *byte_access;
    BYTE *data = NULL;
    UINT32 length = 0;
    HRESULT hr;

    TRACE("iface %p, buffer %p, value_size %p, value %p.\n", iface, buffer, value_size, value);

    if (!buffer || !value_size || !value) return E_POINTER;

    hr = IBuffer_get_Length(buffer, &length);
    if (FAILED(hr)) return hr;

    *value_size = length;
    if (!length)
    {
        *value = CoTaskMemAlloc(0);
        return S_OK;
    }

    if (SUCCEEDED(IBuffer_QueryInterface(buffer, &IID_IBufferByteAccess, (void **)&byte_access)))
    {
        IBufferByteAccess_Buffer(byte_access, &data);
        IBufferByteAccess_Release(byte_access);
    }

    if (!data) return E_FAIL;

    *value = CoTaskMemAlloc(length);
    if (!*value) return E_OUTOFMEMORY;
    memcpy(*value, data, length);

    return S_OK;
}

static HRESULT STDMETHODCALLTYPE cryptobuffer_statics_DecodeFromHexString(
        ICryptographicBufferStatics *iface, HSTRING value, IBuffer **buffer)
{
    const WCHAR *str;
    UINT32 len = 0;
    DWORD byte_count = 0;
    struct cryptowinrt_buffer *impl;
    HRESULT hr;

    TRACE("iface %p, value %p, buffer %p.\n", iface, value, buffer);

    if (!buffer) return E_POINTER;
    str = WindowsGetStringRawBuffer(value, &len);
    if (!str || !len)
        return buffer_create(0, buffer);

    if (!CryptStringToBinaryW(str, len, CRYPT_STRING_HEX, NULL, &byte_count, NULL, NULL))
        return E_INVALIDARG;

    hr = buffer_create(byte_count, buffer);
    if (FAILED(hr)) return hr;

    impl = impl_from_IBuffer(*buffer);
    if (!CryptStringToBinaryW(str, len, CRYPT_STRING_HEX, impl->data, &byte_count, NULL, NULL))
    {
        IBuffer_Release(*buffer);
        *buffer = NULL;
        return E_FAIL;
    }
    impl->length = byte_count;
    return S_OK;
}

static HRESULT STDMETHODCALLTYPE cryptobuffer_statics_EncodeToHexString(
        ICryptographicBufferStatics *iface, IBuffer *buffer, HSTRING *value)
{
    IBufferByteAccess *byte_access;
    HSTRING_BUFFER str_buffer;
    BYTE *data = NULL;
    UINT32 length = 0;
    DWORD ret_length = 0;
    WCHAR *str;
    HRESULT hr;

    TRACE("iface %p, buffer %p, value %p.\n", iface, buffer, value);

    if (!value) return E_POINTER;
    if (buffer)
    {
        IBuffer_get_Length(buffer, &length);
        if (length && SUCCEEDED(IBuffer_QueryInterface(buffer, &IID_IBufferByteAccess, (void **)&byte_access)))
        {
            IBufferByteAccess_Buffer(byte_access, &data);
            IBufferByteAccess_Release(byte_access);
        }
    }

    if (!length)
        return WindowsCreateString(NULL, 0, value);

    if (!data) return E_FAIL;

    if (!CryptBinaryToStringW(data, length, CRYPT_STRING_HEXRAW | CRYPT_STRING_NOCRLF, NULL, &ret_length))
        return E_FAIL;

    if (FAILED(hr = WindowsPreallocateStringBuffer(ret_length, &str, &str_buffer)))
        return hr;

    if (!CryptBinaryToStringW(data, length, CRYPT_STRING_HEXRAW | CRYPT_STRING_NOCRLF, str, &ret_length))
    {
        WindowsDeleteStringBuffer(str_buffer);
        return E_FAIL;
    }

    return WindowsPromoteStringBuffer(str_buffer, value);
}

static HRESULT STDMETHODCALLTYPE cryptobuffer_statics_DecodeFromBase64String(
        ICryptographicBufferStatics *iface, HSTRING value, IBuffer **buffer)
{
    const WCHAR *str;
    UINT32 len = 0;
    DWORD byte_count = 0;
    struct cryptowinrt_buffer *impl;
    HRESULT hr;

    TRACE("iface %p, value %p, buffer %p.\n", iface, value, buffer);

    if (!buffer) return E_POINTER;
    str = WindowsGetStringRawBuffer(value, &len);
    if (!str || !len)
        return buffer_create(0, buffer);

    if (!CryptStringToBinaryW(str, len, CRYPT_STRING_BASE64, NULL, &byte_count, NULL, NULL))
        return E_INVALIDARG;

    hr = buffer_create(byte_count, buffer);
    if (FAILED(hr)) return hr;

    impl = impl_from_IBuffer(*buffer);
    if (!CryptStringToBinaryW(str, len, CRYPT_STRING_BASE64, impl->data, &byte_count, NULL, NULL))
    {
        IBuffer_Release(*buffer);
        *buffer = NULL;
        return E_FAIL;
    }
    impl->length = byte_count;
    return S_OK;
}

static HRESULT STDMETHODCALLTYPE cryptobuffer_statics_EncodeToBase64String(
        ICryptographicBufferStatics *iface, IBuffer *buffer, HSTRING *value)
{
    IBufferByteAccess *buffer_access;
    HSTRING_BUFFER str_buffer;
    void *data = NULL;
    UINT32 length = 0;
    DWORD ret_length;
    WCHAR *str;
    HRESULT hr;

    TRACE("iface %p, buffer %p, value %p.\n", iface, buffer, value);

    if (buffer)
    {
        IBuffer_get_Length(buffer, &length);
        if (length)
        {
            if (SUCCEEDED(IBuffer_QueryInterface(buffer, &IID_IBufferByteAccess, (void **)&buffer_access)))
            {
                IBufferByteAccess_Buffer(buffer_access, (byte **)&data);
                IBufferByteAccess_Release(buffer_access);
            }
        }
    }

    if (!length)
        return WindowsCreateString(NULL, 0, value);

    if (!data)
        return E_FAIL;

    if (!CryptBinaryToStringW(data, length, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &ret_length))
        return E_FAIL;

    if (FAILED(hr = WindowsPreallocateStringBuffer(ret_length, &str, &str_buffer)))
        return hr;

    if (!CryptBinaryToStringW(data, length, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, str, &ret_length))
    {
        WindowsDeleteStringBuffer(str_buffer);
        return E_FAIL;
    }

    return WindowsPromoteStringBuffer(str_buffer, value);
}

static HRESULT STDMETHODCALLTYPE cryptobuffer_statics_ConvertStringToBinary(
        ICryptographicBufferStatics *iface, HSTRING value, BinaryStringEncoding encoding,
        IBuffer **buffer)
{
    const WCHAR *str;
    UINT32 len = 0;
    struct cryptowinrt_buffer *impl;
    HRESULT hr;
    int bytes_needed;

    TRACE("iface %p, value %p, encoding %d, buffer %p.\n", iface, value, encoding, buffer);

    if (!buffer) return E_POINTER;
    str = WindowsGetStringRawBuffer(value, &len);
    if (!str || !len)
        return buffer_create(0, buffer);

    if (encoding == BinaryStringEncoding_Utf16LE)
    {
        UINT32 byte_count = len * sizeof(WCHAR);
        hr = buffer_create(byte_count, buffer);
        if (FAILED(hr)) return hr;
        impl = impl_from_IBuffer(*buffer);
        memcpy(impl->data, str, byte_count);
        impl->length = byte_count;
        return S_OK;
    }

    /* Default / Utf8 */
    bytes_needed = WideCharToMultiByte(CP_UTF8, 0, str, len, NULL, 0, NULL, NULL);
    if (bytes_needed <= 0)
        return buffer_create(0, buffer);

    hr = buffer_create(bytes_needed, buffer);
    if (FAILED(hr)) return hr;

    impl = impl_from_IBuffer(*buffer);
    WideCharToMultiByte(CP_UTF8, 0, str, len, (char *)impl->data, bytes_needed, NULL, NULL);
    impl->length = bytes_needed;
    return S_OK;
}

static HRESULT STDMETHODCALLTYPE cryptobuffer_statics_ConvertBinaryToString(
        ICryptographicBufferStatics *iface, BinaryStringEncoding encoding, IBuffer *buffer, HSTRING *value)
{
    IBufferByteAccess *byte_access;
    BYTE *data = NULL;
    UINT32 length = 0;
    int chars_needed;
    WCHAR *str;
    HSTRING_BUFFER str_buffer;
    HRESULT hr;

    TRACE("iface %p, encoding %d, buffer %p, value %p.\n", iface, encoding, buffer, value);

    if (!value) return E_POINTER;
    if (buffer)
    {
        IBuffer_get_Length(buffer, &length);
        if (length && SUCCEEDED(IBuffer_QueryInterface(buffer, &IID_IBufferByteAccess, (void **)&byte_access)))
        {
            IBufferByteAccess_Buffer(byte_access, &data);
            IBufferByteAccess_Release(byte_access);
        }
    }

    if (!length || !data)
        return WindowsCreateString(NULL, 0, value);

    if (encoding == BinaryStringEncoding_Utf16LE)
        return WindowsCreateString((const WCHAR *)data, length / sizeof(WCHAR), value);

    chars_needed = MultiByteToWideChar(CP_UTF8, 0, (const char *)data, length, NULL, 0);
    if (chars_needed <= 0)
        return WindowsCreateString(NULL, 0, value);

    if (FAILED(hr = WindowsPreallocateStringBuffer(chars_needed, &str, &str_buffer)))
        return hr;

    MultiByteToWideChar(CP_UTF8, 0, (const char *)data, length, str, chars_needed);
    return WindowsPromoteStringBuffer(str_buffer, value);
}

static const struct ICryptographicBufferStaticsVtbl cryptobuffer_statics_vtbl =
{
    cryptobuffer_statics_QueryInterface,
    cryptobuffer_statics_AddRef,
    cryptobuffer_statics_Release,
    /* IInspectable methods */
    cryptobuffer_statics_GetIids,
    cryptobuffer_statics_GetRuntimeClassName,
    cryptobuffer_statics_GetTrustLevel,
    /* ICryptographicBufferStatics methods */
    cryptobuffer_statics_Compare,
    cryptobuffer_statics_GenerateRandom,
    cryptobuffer_statics_GenerateRandomNumber,
    cryptobuffer_statics_CreateFromByteArray,
    cryptobuffer_statics_CopyToByteArray,
    cryptobuffer_statics_DecodeFromHexString,
    cryptobuffer_statics_EncodeToHexString,
    cryptobuffer_statics_DecodeFromBase64String,
    cryptobuffer_statics_EncodeToBase64String,
    cryptobuffer_statics_ConvertStringToBinary,
    cryptobuffer_statics_ConvertBinaryToString,
};

static struct cryptobuffer_factory cryptobuffer_factory =
{
    .IActivationFactory_iface.lpVtbl = &cryptobuffer_factory_vtbl,
    .ICryptographicBufferStatics_iface.lpVtbl = &cryptobuffer_statics_vtbl,
    .refcount = 1,
};

IActivationFactory *cryptobuffer_activation_factory = &cryptobuffer_factory.IActivationFactory_iface;

HRESULT WINAPI DllGetClassObject(REFCLSID clsid, REFIID riid, void **out)
{
    FIXME("clsid %s, riid %s, out %p stub!\n", debugstr_guid(clsid), debugstr_guid(riid), out);
    return CLASS_E_CLASSNOTAVAILABLE;
}

HRESULT WINAPI DllGetActivationFactory(HSTRING classid, IActivationFactory **factory)
{
    const WCHAR *name = WindowsGetStringRawBuffer(classid, NULL);

    TRACE("classid %s, factory %p.\n", debugstr_hstring(classid), factory);

    *factory = NULL;

    if (!wcscmp(name, RuntimeClass_Windows_Security_Cryptography_CryptographicBuffer))
        IActivationFactory_QueryInterface(cryptobuffer_activation_factory, &IID_IActivationFactory, (void **)factory);
    if (!wcscmp(name, RuntimeClass_Windows_Security_Credentials_KeyCredentialManager))
        IActivationFactory_QueryInterface(credentials_activation_factory, &IID_IActivationFactory, (void **)factory);
    if (!wcscmp(name, L"Windows.Security.Cryptography.DataProtection.DataProtectionProvider"))
        IActivationFactory_QueryInterface(dataprotection_activation_factory, &IID_IActivationFactory, (void **)factory);

    if (*factory) return S_OK;
    return CLASS_E_CLASSNOTAVAILABLE;
}
