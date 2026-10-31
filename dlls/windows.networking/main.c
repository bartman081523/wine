/*
 * Copyright 2022 Zhiyi Zhang for CodeWeavers
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

#include <stdarg.h>
#include <stdlib.h>

#define COBJMACROS
#include <initguid.h>
#include <windef.h>
#include <winbase.h>
#include <winnt.h>
#include <winstring.h>
#include <roapi.h>
#include <activation.h>
#include <eventtoken.h>
#include <asyncinfo.h>

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(winsock);

DEFINE_GUID(IID_IPushNotificationChannelManagerStatics, 0x8baf9b65, 0x77a1, 0x4588, 0xbd, 0x19, 0x86, 0x15, 0x29, 0xa9, 0xdc, 0xf0);
DEFINE_GUID(IID_IPushNotificationChannelManagerStatics4, 0xbc540efb, 0x7820, 0x5a5b, 0x9c, 0x01, 0xb4, 0x75, 0x7f, 0x77, 0x40, 0x25);
DEFINE_GUID(IID_IPushNotificationChannel, 0x2b28102e, 0xef0b, 0x4f39, 0x9b, 0x8a, 0xa3, 0xc1, 0x94, 0xde, 0x70, 0x81);

typedef struct DateTime
{
    INT64 UniversalTime;
} DateTime;

/* Completed handler vtbl */
typedef struct IAsyncOperationCompletedHandlerVtbl
{
    HRESULT (WINAPI *QueryInterface)(IUnknown *This, REFIID riid, void **ppvObject);
    ULONG   (WINAPI *AddRef)(IUnknown *This);
    ULONG   (WINAPI *Release)(IUnknown *This);
    HRESULT (WINAPI *Invoke)(IUnknown *This, IInspectable *asyncInfo, AsyncStatus asyncStatus);
} IAsyncOperationCompletedHandlerVtbl;

/* IPushNotificationChannel declarations */
typedef struct IPushNotificationChannel IPushNotificationChannel;

typedef struct IPushNotificationChannelVtbl
{
    /* IUnknown */
    HRESULT (WINAPI *QueryInterface)(IPushNotificationChannel *This, REFIID riid, void **ppvObject);
    ULONG   (WINAPI *AddRef)(IPushNotificationChannel *This);
    ULONG   (WINAPI *Release)(IPushNotificationChannel *This);
    /* IInspectable */
    HRESULT (WINAPI *GetIids)(IPushNotificationChannel *This, ULONG *iidCount, IID **iids);
    HRESULT (WINAPI *GetRuntimeClassName)(IPushNotificationChannel *This, HSTRING *className);
    HRESULT (WINAPI *GetTrustLevel)(IPushNotificationChannel *This, TrustLevel *trustLevel);
    /* IPushNotificationChannel */
    HRESULT (WINAPI *get_Uri)(IPushNotificationChannel *This, HSTRING *value);
    HRESULT (WINAPI *get_ExpirationTime)(IPushNotificationChannel *This, DateTime *value);
    HRESULT (WINAPI *Close)(IPushNotificationChannel *This);
    HRESULT (WINAPI *add_PushNotificationReceived)(IPushNotificationChannel *This, IInspectable *handler, EventRegistrationToken *token);
    HRESULT (WINAPI *remove_PushNotificationReceived)(IPushNotificationChannel *This, EventRegistrationToken token);
} IPushNotificationChannelVtbl;

struct IPushNotificationChannel
{
    const IPushNotificationChannelVtbl *lpVtbl;
};

#define IPushNotificationChannel_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IPushNotificationChannel_Release(This) (This)->lpVtbl->Release(This)

struct push_notification_channel
{
    IPushNotificationChannel IPushNotificationChannel_iface;
    LONG ref;
    HSTRING uri;
};

static inline struct push_notification_channel *impl_from_IPushNotificationChannel(IPushNotificationChannel *iface)
{
    return CONTAINING_RECORD(iface, struct push_notification_channel, IPushNotificationChannel_iface);
}

static HRESULT WINAPI push_channel_QueryInterface(IPushNotificationChannel *iface, REFIID iid, void **out)
{
    struct push_notification_channel *impl = impl_from_IPushNotificationChannel(iface);
    static const GUID IID_ComWrappers_internal = {0x11d3b13a, 0x180e, 0x4789, {0xa8, 0xbe, 0x77, 0x12, 0x88, 0x28, 0x93, 0xe6}};
    static const GUID IID_ComWrappers_internal2 = {0x5c13e51c, 0x241d, 0x4859, {0x80, 0x05, 0x8a, 0x8d, 0x8d, 0x98, 0x15, 0xbf}};

    MESSAGE("windows.networking: push_channel_QueryInterface iid %s\n", debugstr_guid(iid));
    if (!out) return E_INVALIDARG;

    if (IsEqualIID(iid, &IID_ComWrappers_internal) || IsEqualIID(iid, &IID_ComWrappers_internal2))
    {
        *out = NULL;
        return E_NOINTERFACE;
    }

    if (IsEqualIID(iid, &IID_IUnknown) ||
        IsEqualIID(iid, &IID_IInspectable) ||
        IsEqualIID(iid, &IID_IAgileObject) ||
        IsEqualIID(iid, &IID_IPushNotificationChannel))
    {
        *out = &impl->IPushNotificationChannel_iface;
        IUnknown_AddRef((IUnknown *)*out);
        return S_OK;
    }

    WARN("unsupported iid %s\n", debugstr_guid(iid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI push_channel_AddRef(IPushNotificationChannel *iface)
{
    struct push_notification_channel *impl = impl_from_IPushNotificationChannel(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI push_channel_Release(IPushNotificationChannel *iface)
{
    struct push_notification_channel *impl = impl_from_IPushNotificationChannel(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref)
    {
        if (impl->uri) WindowsDeleteString(impl->uri);
        free(impl);
    }
    return ref;
}

static HRESULT WINAPI push_channel_GetIids(IPushNotificationChannel *iface, ULONG *iid_count, IID **iids)
{
    IID *array;
    if (!iid_count || !iids) return E_INVALIDARG;
    if (!(array = CoTaskMemAlloc(2 * sizeof(IID)))) return E_OUTOFMEMORY;
    array[0] = IID_IAgileObject;
    array[1] = IID_IPushNotificationChannel;
    *iid_count = 2;
    *iids = array;
    return S_OK;
}

static HRESULT WINAPI push_channel_GetRuntimeClassName(IPushNotificationChannel *iface, HSTRING *class_name)
{
    static const WCHAR name[] = L"Windows.Networking.PushNotifications.PushNotificationChannel";
    return WindowsCreateString(name, ARRAY_SIZE(name) - 1, class_name);
}

static HRESULT WINAPI push_channel_GetTrustLevel(IPushNotificationChannel *iface, TrustLevel *trust_level)
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI push_channel_get_Uri(IPushNotificationChannel *iface, HSTRING *value)
{
    struct push_notification_channel *impl = impl_from_IPushNotificationChannel(iface);
    MESSAGE("windows.networking: push_channel_get_Uri returning uri %s\n", debugstr_hstring(impl->uri));
    if (!value) return E_INVALIDARG;
    return WindowsDuplicateString(impl->uri, value);
}

static HRESULT WINAPI push_channel_get_ExpirationTime(IPushNotificationChannel *iface, DateTime *value)
{
    MESSAGE("windows.networking: push_channel_get_ExpirationTime returning 2099\n");
    if (!value) return E_INVALIDARG;
    value->UniversalTime = 0x7fffffffffffffffLL;
    return S_OK;
}

static HRESULT WINAPI push_channel_Close(IPushNotificationChannel *iface)
{
    MESSAGE("windows.networking: push_channel_Close called\n");
    return S_OK;
}

static HRESULT WINAPI push_channel_add_PushNotificationReceived(IPushNotificationChannel *iface, IInspectable *handler, EventRegistrationToken *token)
{
    MESSAGE("windows.networking: push_channel_add_PushNotificationReceived handler %p\n", handler);
    if (!token) return E_INVALIDARG;
    token->value = 1;
    return S_OK;
}

static HRESULT WINAPI push_channel_remove_PushNotificationReceived(IPushNotificationChannel *iface, EventRegistrationToken token)
{
    MESSAGE("windows.networking: push_channel_remove_PushNotificationReceived token %I64d\n", token.value);
    return S_OK;
}

static const struct IPushNotificationChannelVtbl push_channel_vtbl =
{
    push_channel_QueryInterface,
    push_channel_AddRef,
    push_channel_Release,
    push_channel_GetIids,
    push_channel_GetRuntimeClassName,
    push_channel_GetTrustLevel,
    push_channel_get_Uri,
    push_channel_get_ExpirationTime,
    push_channel_Close,
    push_channel_add_PushNotificationReceived,
    push_channel_remove_PushNotificationReceived,
};

static HRESULT push_channel_create(struct push_notification_channel **out)
{
    static const WCHAR default_uri[] = L"https://wns.windows.com/dummy/push_channel";
    struct push_notification_channel *impl;

    if (!out) return E_INVALIDARG;
    if (!(impl = calloc(1, sizeof(*impl)))) return E_OUTOFMEMORY;

    impl->IPushNotificationChannel_iface.lpVtbl = &push_channel_vtbl;
    impl->ref = 1;
    WindowsCreateString(default_uri, ARRAY_SIZE(default_uri) - 1, &impl->uri);

    *out = impl;
    return S_OK;
}

/* IAsyncOperation<PushNotificationChannel*> declarations */
typedef struct IAsyncOperation_PushNotificationChannel IAsyncOperation_PushNotificationChannel;

typedef struct IAsyncOperation_PushNotificationChannelVtbl
{
    /* IUnknown */
    HRESULT (WINAPI *QueryInterface)(IAsyncOperation_PushNotificationChannel *This, REFIID riid, void **ppvObject);
    ULONG   (WINAPI *AddRef)(IAsyncOperation_PushNotificationChannel *This);
    ULONG   (WINAPI *Release)(IAsyncOperation_PushNotificationChannel *This);
    /* IInspectable */
    HRESULT (WINAPI *GetIids)(IAsyncOperation_PushNotificationChannel *This, ULONG *iidCount, IID **iids);
    HRESULT (WINAPI *GetRuntimeClassName)(IAsyncOperation_PushNotificationChannel *This, HSTRING *className);
    HRESULT (WINAPI *GetTrustLevel)(IAsyncOperation_PushNotificationChannel *This, TrustLevel *trustLevel);
    /* IAsyncOperation<PushNotificationChannel*> */
    HRESULT (WINAPI *put_Completed)(IAsyncOperation_PushNotificationChannel *This, IUnknown *handler);
    HRESULT (WINAPI *get_Completed)(IAsyncOperation_PushNotificationChannel *This, IUnknown **handler);
    HRESULT (WINAPI *GetResults)(IAsyncOperation_PushNotificationChannel *This, IPushNotificationChannel **results);
} IAsyncOperation_PushNotificationChannelVtbl;

struct IAsyncOperation_PushNotificationChannel
{
    const IAsyncOperation_PushNotificationChannelVtbl *lpVtbl;
};

struct async_operation_push_channel
{
    IAsyncOperation_PushNotificationChannel IAsyncOperation_iface;
    IAsyncInfo IAsyncInfo_iface;
    LONG ref;
    struct push_notification_channel *channel;
};

static inline struct async_operation_push_channel *impl_from_IAsyncOperation_PushNotificationChannel(IAsyncOperation_PushNotificationChannel *iface)
{
    return CONTAINING_RECORD(iface, struct async_operation_push_channel, IAsyncOperation_iface);
}

static inline struct async_operation_push_channel *impl_from_push_IAsyncInfo(IAsyncInfo *iface)
{
    return CONTAINING_RECORD(iface, struct async_operation_push_channel, IAsyncInfo_iface);
}

static HRESULT WINAPI async_push_channel_QueryInterface(IAsyncOperation_PushNotificationChannel *iface, REFIID iid, void **out)
{
    struct async_operation_push_channel *impl = impl_from_IAsyncOperation_PushNotificationChannel(iface);
    static const GUID IID_ComWrappers_internal = {0x11d3b13a, 0x180e, 0x4789, {0xa8, 0xbe, 0x77, 0x12, 0x88, 0x28, 0x93, 0xe6}};
    static const GUID IID_ComWrappers_internal2 = {0x5c13e51c, 0x241d, 0x4859, {0x80, 0x05, 0x8a, 0x8d, 0x8d, 0x98, 0x15, 0xbf}};

    MESSAGE("windows.networking: async_push_channel_QueryInterface iid %s\n", debugstr_guid(iid));
    if (!out) return E_INVALIDARG;

    if (IsEqualIID(iid, &IID_ComWrappers_internal) || IsEqualIID(iid, &IID_ComWrappers_internal2))
    {
        *out = NULL;
        return E_NOINTERFACE;
    }

    if (IsEqualIID(iid, &IID_IAsyncInfo))
    {
        *out = &impl->IAsyncInfo_iface;
        IUnknown_AddRef((IUnknown *)*out);
        return S_OK;
    }

    /* Any other interface queried on this async operation is accepted as the IAsyncOperation interface */
    *out = &impl->IAsyncOperation_iface;
    IUnknown_AddRef((IUnknown *)*out);
    return S_OK;
}

static ULONG WINAPI async_push_channel_AddRef(IAsyncOperation_PushNotificationChannel *iface)
{
    struct async_operation_push_channel *impl = impl_from_IAsyncOperation_PushNotificationChannel(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI async_push_channel_Release(IAsyncOperation_PushNotificationChannel *iface)
{
    struct async_operation_push_channel *impl = impl_from_IAsyncOperation_PushNotificationChannel(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref)
    {
        if (impl->channel) IPushNotificationChannel_Release(&impl->channel->IPushNotificationChannel_iface);
        free(impl);
    }
    return ref;
}

static HRESULT WINAPI async_push_channel_GetIids(IAsyncOperation_PushNotificationChannel *iface, ULONG *iid_count, IID **iids)
{
    IID *array;
    if (!iid_count || !iids) return E_INVALIDARG;
    if (!(array = CoTaskMemAlloc(2 * sizeof(IID)))) return E_OUTOFMEMORY;
    array[0] = IID_IAgileObject;
    array[1] = IID_IAsyncInfo;
    *iid_count = 2;
    *iids = array;
    return S_OK;
}

static HRESULT WINAPI async_push_channel_GetRuntimeClassName(IAsyncOperation_PushNotificationChannel *iface, HSTRING *class_name)
{
    static const WCHAR name[] = L"Windows.Foundation.IAsyncOperation`1<Windows.Networking.PushNotifications.PushNotificationChannel>";
    return WindowsCreateString(name, ARRAY_SIZE(name) - 1, class_name);
}

static HRESULT WINAPI async_push_channel_GetTrustLevel(IAsyncOperation_PushNotificationChannel *iface, TrustLevel *trust_level)
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI async_push_channel_put_Completed(IAsyncOperation_PushNotificationChannel *iface, IUnknown *handler)
{
    struct async_operation_push_channel *impl = impl_from_IAsyncOperation_PushNotificationChannel(iface);
    MESSAGE("windows.networking: async_push_channel_put_Completed handler %p\n", handler);
    if (handler)
    {
        IAsyncOperationCompletedHandlerVtbl *vtbl = *(IAsyncOperationCompletedHandlerVtbl **)handler;
        vtbl->Invoke(handler, (IInspectable *)&impl->IAsyncOperation_iface, Completed);
    }
    return S_OK;
}

static HRESULT WINAPI async_push_channel_get_Completed(IAsyncOperation_PushNotificationChannel *iface, IUnknown **handler)
{
    if (!handler) return E_INVALIDARG;
    *handler = NULL;
    return S_OK;
}

static HRESULT WINAPI async_push_channel_GetResults(IAsyncOperation_PushNotificationChannel *iface, IPushNotificationChannel **results)
{
    struct async_operation_push_channel *impl = impl_from_IAsyncOperation_PushNotificationChannel(iface);
    MESSAGE("windows.networking: async_push_channel_GetResults returning channel %p\n", impl->channel);
    if (!results) return E_INVALIDARG;
    *results = &impl->channel->IPushNotificationChannel_iface;
    IPushNotificationChannel_AddRef(*results);
    return S_OK;
}

static const struct IAsyncOperation_PushNotificationChannelVtbl async_push_channel_vtbl =
{
    async_push_channel_QueryInterface,
    async_push_channel_AddRef,
    async_push_channel_Release,
    async_push_channel_GetIids,
    async_push_channel_GetRuntimeClassName,
    async_push_channel_GetTrustLevel,
    async_push_channel_put_Completed,
    async_push_channel_get_Completed,
    async_push_channel_GetResults,
};

static HRESULT WINAPI async_push_info_QueryInterface(IAsyncInfo *iface, REFIID iid, void **out)
{
    struct async_operation_push_channel *impl = impl_from_push_IAsyncInfo(iface);
    return async_push_channel_QueryInterface(&impl->IAsyncOperation_iface, iid, out);
}

static ULONG WINAPI async_push_info_AddRef(IAsyncInfo *iface)
{
    struct async_operation_push_channel *impl = impl_from_push_IAsyncInfo(iface);
    return async_push_channel_AddRef(&impl->IAsyncOperation_iface);
}

static ULONG WINAPI async_push_info_Release(IAsyncInfo *iface)
{
    struct async_operation_push_channel *impl = impl_from_push_IAsyncInfo(iface);
    return async_push_channel_Release(&impl->IAsyncOperation_iface);
}

static HRESULT WINAPI async_push_info_GetIids(IAsyncInfo *iface, ULONG *iid_count, IID **iids)
{
    struct async_operation_push_channel *impl = impl_from_push_IAsyncInfo(iface);
    return async_push_channel_GetIids(&impl->IAsyncOperation_iface, iid_count, iids);
}

static HRESULT WINAPI async_push_info_GetRuntimeClassName(IAsyncInfo *iface, HSTRING *class_name)
{
    struct async_operation_push_channel *impl = impl_from_push_IAsyncInfo(iface);
    return async_push_channel_GetRuntimeClassName(&impl->IAsyncOperation_iface, class_name);
}

static HRESULT WINAPI async_push_info_GetTrustLevel(IAsyncInfo *iface, TrustLevel *trust_level)
{
    struct async_operation_push_channel *impl = impl_from_push_IAsyncInfo(iface);
    return async_push_channel_GetTrustLevel(&impl->IAsyncOperation_iface, trust_level);
}

static HRESULT WINAPI async_push_info_get_Id(IAsyncInfo *iface, UINT32 *id)
{
    if (!id) return E_INVALIDARG;
    *id = 1;
    return S_OK;
}

static HRESULT WINAPI async_push_info_get_Status(IAsyncInfo *iface, AsyncStatus *status)
{
    if (!status) return E_INVALIDARG;
    *status = Completed;
    return S_OK;
}

static HRESULT WINAPI async_push_info_get_ErrorCode(IAsyncInfo *iface, HRESULT *error_code)
{
    if (!error_code) return E_INVALIDARG;
    *error_code = S_OK;
    return S_OK;
}

static HRESULT WINAPI async_push_info_Cancel(IAsyncInfo *iface)
{
    return S_OK;
}

static HRESULT WINAPI async_push_info_Close(IAsyncInfo *iface)
{
    return S_OK;
}

static const struct IAsyncInfoVtbl async_push_info_vtbl =
{
    async_push_info_QueryInterface,
    async_push_info_AddRef,
    async_push_info_Release,
    async_push_info_GetIids,
    async_push_info_GetRuntimeClassName,
    async_push_info_GetTrustLevel,
    async_push_info_get_Id,
    async_push_info_get_Status,
    async_push_info_get_ErrorCode,
    async_push_info_Cancel,
    async_push_info_Close,
};

static HRESULT async_operation_push_channel_create(void **out)
{
    struct async_operation_push_channel *impl;
    HRESULT hr;

    if (!out) return E_INVALIDARG;
    if (!(impl = calloc(1, sizeof(*impl)))) return E_OUTOFMEMORY;

    impl->IAsyncOperation_iface.lpVtbl = &async_push_channel_vtbl;
    impl->IAsyncInfo_iface.lpVtbl = &async_push_info_vtbl;
    impl->ref = 1;

    hr = push_channel_create(&impl->channel);
    if (FAILED(hr))
    {
        free(impl);
        return hr;
    }

    *out = &impl->IAsyncOperation_iface;
    return S_OK;
}

typedef struct IPushNotificationChannelManagerStatics IPushNotificationChannelManagerStatics;
typedef struct IPushNotificationChannelManagerStatics4 IPushNotificationChannelManagerStatics4;

typedef struct IPushNotificationChannelManagerStaticsVtbl {
    /* IUnknown */
    HRESULT (WINAPI *QueryInterface)(IPushNotificationChannelManagerStatics *This, REFIID riid, void **ppvObject);
    ULONG   (WINAPI *AddRef)(IPushNotificationChannelManagerStatics *This);
    ULONG   (WINAPI *Release)(IPushNotificationChannelManagerStatics *This);
    /* IInspectable */
    HRESULT (WINAPI *GetIids)(IPushNotificationChannelManagerStatics *This, ULONG *iidCount, IID **iids);
    HRESULT (WINAPI *GetRuntimeClassName)(IPushNotificationChannelManagerStatics *This, HSTRING *className);
    HRESULT (WINAPI *GetTrustLevel)(IPushNotificationChannelManagerStatics *This, TrustLevel *trustLevel);
    /* IPushNotificationChannelManagerStatics */
    HRESULT (WINAPI *CreatePushNotificationChannelForApplicationAsync)(IPushNotificationChannelManagerStatics *This, void **operation);
    HRESULT (WINAPI *CreatePushNotificationChannelForApplicationAsyncWithId)(IPushNotificationChannelManagerStatics *This, HSTRING applicationId, void **operation);
    HRESULT (WINAPI *CreatePushNotificationChannelForSecondaryTileAsync)(IPushNotificationChannelManagerStatics *This, HSTRING tileId, void **operation);
} IPushNotificationChannelManagerStaticsVtbl;

struct IPushNotificationChannelManagerStatics {
    const IPushNotificationChannelManagerStaticsVtbl *lpVtbl;
};

typedef struct IPushNotificationChannelManagerStatics4Vtbl {
    /* IUnknown */
    HRESULT (WINAPI *QueryInterface)(IPushNotificationChannelManagerStatics4 *This, REFIID riid, void **ppvObject);
    ULONG   (WINAPI *AddRef)(IPushNotificationChannelManagerStatics4 *This);
    ULONG   (WINAPI *Release)(IPushNotificationChannelManagerStatics4 *This);
    /* IInspectable */
    HRESULT (WINAPI *GetIids)(IPushNotificationChannelManagerStatics4 *This, ULONG *iidCount, IID **iids);
    HRESULT (WINAPI *GetRuntimeClassName)(IPushNotificationChannelManagerStatics4 *This, HSTRING *className);
    HRESULT (WINAPI *GetTrustLevel)(IPushNotificationChannelManagerStatics4 *This, TrustLevel *trustLevel);
    /* IPushNotificationChannelManagerStatics4 */
    HRESULT (WINAPI *add_ChannelsRevoked)(IPushNotificationChannelManagerStatics4 *This, IInspectable *handler, EventRegistrationToken *token);
    HRESULT (WINAPI *remove_ChannelsRevoked)(IPushNotificationChannelManagerStatics4 *This, EventRegistrationToken token);
} IPushNotificationChannelManagerStatics4Vtbl;

struct IPushNotificationChannelManagerStatics4 {
    const IPushNotificationChannelManagerStatics4Vtbl *lpVtbl;
};

struct push_notification_factory
{
    IActivationFactory IActivationFactory_iface;
    IPushNotificationChannelManagerStatics IPushNotificationChannelManagerStatics_iface;
    IPushNotificationChannelManagerStatics4 IPushNotificationChannelManagerStatics4_iface;
    LONG refcount;
};

static inline struct push_notification_factory *impl_from_IActivationFactory(IActivationFactory *iface)
{
    return CONTAINING_RECORD(iface, struct push_notification_factory, IActivationFactory_iface);
}

static inline struct push_notification_factory *impl_from_IPushNotificationChannelManagerStatics(IPushNotificationChannelManagerStatics *iface)
{
    return CONTAINING_RECORD(iface, struct push_notification_factory, IPushNotificationChannelManagerStatics_iface);
}

static inline struct push_notification_factory *impl_from_IPushNotificationChannelManagerStatics4(IPushNotificationChannelManagerStatics4 *iface)
{
    return CONTAINING_RECORD(iface, struct push_notification_factory, IPushNotificationChannelManagerStatics4_iface);
}

static HRESULT WINAPI factory_QueryInterface(IActivationFactory *iface, REFIID iid, void **out)
{
    struct push_notification_factory *factory = impl_from_IActivationFactory(iface);
    static const GUID IID_ComWrappers_internal = {0x11d3b13a, 0x180e, 0x4789, {0xa8, 0xbe, 0x77, 0x12, 0x88, 0x28, 0x93, 0xe6}};
    static const GUID IID_ComWrappers_internal2 = {0x5c13e51c, 0x241d, 0x4859, {0x80, 0x05, 0x8a, 0x8d, 0x8d, 0x98, 0x15, 0xbf}};

    TRACE("iface %p, iid %s, out %p.\n", iface, debugstr_guid(iid), out);

    if (!out) return E_INVALIDARG;

    if (IsEqualIID(iid, &IID_ComWrappers_internal) || IsEqualIID(iid, &IID_ComWrappers_internal2))
    {
        *out = NULL;
        return E_NOINTERFACE;
    }

    if (IsEqualIID(iid, &IID_IUnknown) ||
        IsEqualIID(iid, &IID_IInspectable) ||
        IsEqualIID(iid, &IID_IAgileObject) ||
        IsEqualIID(iid, &IID_IActivationFactory))
    {
        *out = &factory->IActivationFactory_iface;
    }
    else if (IsEqualIID(iid, &IID_IPushNotificationChannelManagerStatics))
    {
        *out = &factory->IPushNotificationChannelManagerStatics_iface;
    }
    else if (IsEqualIID(iid, &IID_IPushNotificationChannelManagerStatics4))
    {
        *out = &factory->IPushNotificationChannelManagerStatics4_iface;
    }
    else
    {
        FIXME("unsupported iid %s\n", debugstr_guid(iid));
        *out = NULL;
        return E_NOINTERFACE;
    }

    IUnknown_AddRef((IUnknown *)*out);
    return S_OK;
}

static ULONG WINAPI factory_AddRef(IActivationFactory *iface)
{
    struct push_notification_factory *factory = impl_from_IActivationFactory(iface);
    return InterlockedIncrement(&factory->refcount);
}

static ULONG WINAPI factory_Release(IActivationFactory *iface)
{
    struct push_notification_factory *factory = impl_from_IActivationFactory(iface);
    return InterlockedDecrement(&factory->refcount);
}

static HRESULT WINAPI factory_GetIids(IActivationFactory *iface, ULONG *iid_count, IID **iids)
{
    IID *array;

    TRACE("iface %p, iid_count %p, iids %p.\n", iface, iid_count, iids);

    if (!iid_count || !iids) return E_INVALIDARG;

    if (!(array = CoTaskMemAlloc(4 * sizeof(IID)))) return E_OUTOFMEMORY;
    array[0] = IID_IActivationFactory;
    array[1] = IID_IAgileObject;
    array[2] = IID_IPushNotificationChannelManagerStatics;
    array[3] = IID_IPushNotificationChannelManagerStatics4;
    *iid_count = 4;
    *iids = array;
    return S_OK;
}

static HRESULT WINAPI factory_GetRuntimeClassName(IActivationFactory *iface, HSTRING *class_name)
{
    static const WCHAR name[] = L"Windows.Networking.PushNotifications.PushNotificationChannelManager";
    return WindowsCreateString(name, ARRAY_SIZE(name) - 1, class_name);
}

static HRESULT WINAPI factory_GetTrustLevel(IActivationFactory *iface, TrustLevel *trust_level)
{
    TRACE("iface %p, trust_level %p.\n", iface, trust_level);
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI factory_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    FIXME("iface %p, instance %p stub!\n", iface, instance);
    if (instance) *instance = NULL;
    return E_NOTIMPL;
}

static const struct IActivationFactoryVtbl factory_vtbl =
{
    factory_QueryInterface,
    factory_AddRef,
    factory_Release,
    factory_GetIids,
    factory_GetRuntimeClassName,
    factory_GetTrustLevel,
    factory_ActivateInstance,
};

/* IPushNotificationChannelManagerStatics */
static HRESULT WINAPI statics_QueryInterface(IPushNotificationChannelManagerStatics *iface, REFIID iid, void **out)
{
    struct push_notification_factory *factory = impl_from_IPushNotificationChannelManagerStatics(iface);
    return factory_QueryInterface(&factory->IActivationFactory_iface, iid, out);
}

static ULONG WINAPI statics_AddRef(IPushNotificationChannelManagerStatics *iface)
{
    struct push_notification_factory *factory = impl_from_IPushNotificationChannelManagerStatics(iface);
    return factory_AddRef(&factory->IActivationFactory_iface);
}

static ULONG WINAPI statics_Release(IPushNotificationChannelManagerStatics *iface)
{
    struct push_notification_factory *factory = impl_from_IPushNotificationChannelManagerStatics(iface);
    return factory_Release(&factory->IActivationFactory_iface);
}

static HRESULT WINAPI statics_GetIids(IPushNotificationChannelManagerStatics *iface, ULONG *iid_count, IID **iids)
{
    struct push_notification_factory *factory = impl_from_IPushNotificationChannelManagerStatics(iface);
    return factory_GetIids(&factory->IActivationFactory_iface, iid_count, iids);
}

static HRESULT WINAPI statics_GetRuntimeClassName(IPushNotificationChannelManagerStatics *iface, HSTRING *class_name)
{
    struct push_notification_factory *factory = impl_from_IPushNotificationChannelManagerStatics(iface);
    return factory_GetRuntimeClassName(&factory->IActivationFactory_iface, class_name);
}

static HRESULT WINAPI statics_GetTrustLevel(IPushNotificationChannelManagerStatics *iface, TrustLevel *trust_level)
{
    struct push_notification_factory *factory = impl_from_IPushNotificationChannelManagerStatics(iface);
    return factory_GetTrustLevel(&factory->IActivationFactory_iface, trust_level);
}

static HRESULT WINAPI statics_CreatePushNotificationChannelForApplicationAsync(IPushNotificationChannelManagerStatics *iface, void **operation)
{
    MESSAGE("windows.networking: statics_CreatePushNotificationChannelForApplicationAsync iface %p, operation %p\n", iface, operation);
    return async_operation_push_channel_create(operation);
}

static HRESULT WINAPI statics_CreatePushNotificationChannelForApplicationAsyncWithId(IPushNotificationChannelManagerStatics *iface, HSTRING appid, void **operation)
{
    MESSAGE("windows.networking: statics_CreatePushNotificationChannelForApplicationAsyncWithId iface %p, appid %s, operation %p\n",
            iface, debugstr_hstring(appid), operation);
    return async_operation_push_channel_create(operation);
}

static HRESULT WINAPI statics_CreatePushNotificationChannelForSecondaryTileAsync(IPushNotificationChannelManagerStatics *iface, HSTRING tileid, void **operation)
{
    MESSAGE("windows.networking: statics_CreatePushNotificationChannelForSecondaryTileAsync iface %p, tileid %s, operation %p\n",
            iface, debugstr_hstring(tileid), operation);
    return async_operation_push_channel_create(operation);
}

static const struct IPushNotificationChannelManagerStaticsVtbl statics_vtbl =
{
    statics_QueryInterface,
    statics_AddRef,
    statics_Release,
    statics_GetIids,
    statics_GetRuntimeClassName,
    statics_GetTrustLevel,
    statics_CreatePushNotificationChannelForApplicationAsync,
    statics_CreatePushNotificationChannelForApplicationAsyncWithId,
    statics_CreatePushNotificationChannelForSecondaryTileAsync,
};

/* IPushNotificationChannelManagerStatics4 */
static HRESULT WINAPI statics4_QueryInterface(IPushNotificationChannelManagerStatics4 *iface, REFIID iid, void **out)
{
    struct push_notification_factory *factory = impl_from_IPushNotificationChannelManagerStatics4(iface);
    return factory_QueryInterface(&factory->IActivationFactory_iface, iid, out);
}

static ULONG WINAPI statics4_AddRef(IPushNotificationChannelManagerStatics4 *iface)
{
    struct push_notification_factory *factory = impl_from_IPushNotificationChannelManagerStatics4(iface);
    return factory_AddRef(&factory->IActivationFactory_iface);
}

static ULONG WINAPI statics4_Release(IPushNotificationChannelManagerStatics4 *iface)
{
    struct push_notification_factory *factory = impl_from_IPushNotificationChannelManagerStatics4(iface);
    return factory_Release(&factory->IActivationFactory_iface);
}

static HRESULT WINAPI statics4_GetIids(IPushNotificationChannelManagerStatics4 *iface, ULONG *iid_count, IID **iids)
{
    struct push_notification_factory *factory = impl_from_IPushNotificationChannelManagerStatics4(iface);
    return factory_GetIids(&factory->IActivationFactory_iface, iid_count, iids);
}

static HRESULT WINAPI statics4_GetRuntimeClassName(IPushNotificationChannelManagerStatics4 *iface, HSTRING *class_name)
{
    struct push_notification_factory *factory = impl_from_IPushNotificationChannelManagerStatics4(iface);
    return factory_GetRuntimeClassName(&factory->IActivationFactory_iface, class_name);
}

static HRESULT WINAPI statics4_GetTrustLevel(IPushNotificationChannelManagerStatics4 *iface, TrustLevel *trust_level)
{
    struct push_notification_factory *factory = impl_from_IPushNotificationChannelManagerStatics4(iface);
    return factory_GetTrustLevel(&factory->IActivationFactory_iface, trust_level);
}

static HRESULT WINAPI statics4_add_ChannelsRevoked(IPushNotificationChannelManagerStatics4 *iface, IInspectable *handler, EventRegistrationToken *token)
{
    TRACE("iface %p, handler %p, token %p\n", iface, handler, token);
    if (!token) return E_INVALIDARG;
    token->value = 1;
    return S_OK;
}

static HRESULT WINAPI statics4_remove_ChannelsRevoked(IPushNotificationChannelManagerStatics4 *iface, EventRegistrationToken token)
{
    TRACE("iface %p, token %I64d\n", iface, token.value);
    return S_OK;
}

static const struct IPushNotificationChannelManagerStatics4Vtbl statics4_vtbl =
{
    statics4_QueryInterface,
    statics4_AddRef,
    statics4_Release,
    statics4_GetIids,
    statics4_GetRuntimeClassName,
    statics4_GetTrustLevel,
    statics4_add_ChannelsRevoked,
    statics4_remove_ChannelsRevoked,
};

static struct push_notification_factory push_notification_factory =
{
    .IActivationFactory_iface.lpVtbl = &factory_vtbl,
    .IPushNotificationChannelManagerStatics_iface.lpVtbl = &statics_vtbl,
    .IPushNotificationChannelManagerStatics4_iface.lpVtbl = &statics4_vtbl,
    .refcount = 1,
};

/* -------------------------------------------------------------------------
 * Windows.Networking.EndpointPair Implementation
 * ------------------------------------------------------------------------- */
DEFINE_GUID(IID_IEndpointPair, 0x33a0aa36, 0xf8fa, 0x4b30, 0xb8, 0x56, 0x76, 0x51, 0x7c, 0x3b, 0xd0, 0x6d);
DEFINE_GUID(IID_IEndpointPairFactory, 0xb609d971, 0x64e0, 0x442b, 0xaa, 0x6f, 0xcc, 0x8c, 0x8f, 0x18, 0x1f, 0x78);

struct endpoint_pair_impl
{
    struct { const void *lpVtbl; } IEndpointPair_iface;
    LONG ref;
    IUnknown *local_host;
    HSTRING local_service;
    IUnknown *remote_host;
    HSTRING remote_service;
};

static HRESULT WINAPI ep_QI(void *This, REFIID riid, void **ppv)
{
    struct endpoint_pair_impl *impl = This;
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IEndpointPair))
    {
        *ppv = This;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    return E_NOINTERFACE;
}

static ULONG WINAPI ep_AddRef(void *This)
{
    struct endpoint_pair_impl *impl = This;
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI ep_Release(void *This)
{
    struct endpoint_pair_impl *impl = This;
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref)
    {
        if (impl->local_host) IUnknown_Release(impl->local_host);
        if (impl->remote_host) IUnknown_Release(impl->remote_host);
        if (impl->local_service) WindowsDeleteString(impl->local_service);
        if (impl->remote_service) WindowsDeleteString(impl->remote_service);
        free(impl);
    }
    return ref;
}

static HRESULT WINAPI ep_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI ep_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.Networking.EndpointPair", 31, n); }
static HRESULT WINAPI ep_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI ep_get_LocalHostName(void *This, void **value)
{
    struct endpoint_pair_impl *impl = This;
    if (!value) return E_POINTER;
    *value = impl->local_host;
    if (impl->local_host) IUnknown_AddRef(impl->local_host);
    return S_OK;
}

static HRESULT WINAPI ep_put_LocalHostName(void *This, void *value)
{
    struct endpoint_pair_impl *impl = This;
    if (impl->local_host) IUnknown_Release(impl->local_host);
    impl->local_host = (IUnknown *)value;
    if (impl->local_host) IUnknown_AddRef(impl->local_host);
    return S_OK;
}

static HRESULT WINAPI ep_get_LocalServiceName(void *This, HSTRING *value)
{
    struct endpoint_pair_impl *impl = This;
    if (!value) return E_POINTER;
    return WindowsDuplicateString(impl->local_service, value);
}

static HRESULT WINAPI ep_put_LocalServiceName(void *This, HSTRING value)
{
    struct endpoint_pair_impl *impl = This;
    if (impl->local_service) WindowsDeleteString(impl->local_service);
    return WindowsDuplicateString(value, &impl->local_service);
}

static HRESULT WINAPI ep_get_RemoteHostName(void *This, void **value)
{
    struct endpoint_pair_impl *impl = This;
    if (!value) return E_POINTER;
    *value = impl->remote_host;
    if (impl->remote_host) IUnknown_AddRef(impl->remote_host);
    return S_OK;
}

static HRESULT WINAPI ep_put_RemoteHostName(void *This, void *value)
{
    struct endpoint_pair_impl *impl = This;
    if (impl->remote_host) IUnknown_Release(impl->remote_host);
    impl->remote_host = (IUnknown *)value;
    if (impl->remote_host) IUnknown_AddRef(impl->remote_host);
    return S_OK;
}

static HRESULT WINAPI ep_get_RemoteServiceName(void *This, HSTRING *value)
{
    struct endpoint_pair_impl *impl = This;
    if (!value) return E_POINTER;
    return WindowsDuplicateString(impl->remote_service, value);
}

static HRESULT WINAPI ep_put_RemoteServiceName(void *This, HSTRING value)
{
    struct endpoint_pair_impl *impl = This;
    if (impl->remote_service) WindowsDeleteString(impl->remote_service);
    return WindowsDuplicateString(value, &impl->remote_service);
}

static const void *endpoint_pair_vtbl[] = {
    (void *)ep_QI,
    (void *)ep_AddRef,
    (void *)ep_Release,
    (void *)ep_GetIids,
    (void *)ep_GetRuntimeClassName,
    (void *)ep_GetTrustLevel,
    (void *)ep_get_LocalHostName,
    (void *)ep_put_LocalHostName,
    (void *)ep_get_LocalServiceName,
    (void *)ep_put_LocalServiceName,
    (void *)ep_get_RemoteHostName,
    (void *)ep_put_RemoteHostName,
    (void *)ep_get_RemoteServiceName,
    (void *)ep_put_RemoteServiceName
};

static HRESULT create_endpoint_pair(void *local_host, HSTRING local_service,
                                    void *remote_host, HSTRING remote_service,
                                    void **out)
{
    struct endpoint_pair_impl *impl = calloc(1, sizeof(*impl));
    if (!impl) return E_OUTOFMEMORY;
    impl->IEndpointPair_iface.lpVtbl = endpoint_pair_vtbl;
    impl->ref = 1;
    impl->local_host = (IUnknown *)local_host;
    if (impl->local_host) IUnknown_AddRef(impl->local_host);
    impl->remote_host = (IUnknown *)remote_host;
    if (impl->remote_host) IUnknown_AddRef(impl->remote_host);
    if (local_service) WindowsDuplicateString(local_service, &impl->local_service);
    if (remote_service) WindowsDuplicateString(remote_service, &impl->remote_service);
    *out = impl;
    return S_OK;
}

struct endpoint_pair_factory_impl
{
    struct { const void *lpVtbl; } IActivationFactory_iface;
    struct { const void *lpVtbl; } IEndpointPairFactory_iface;
};

#define impl_from_ep_factory(ptr) ((struct endpoint_pair_factory_impl *)((char *)(ptr) - offsetof(struct endpoint_pair_factory_impl, IActivationFactory_iface)))
#define impl_from_ep_pair_factory(ptr) ((struct endpoint_pair_factory_impl *)((char *)(ptr) - offsetof(struct endpoint_pair_factory_impl, IEndpointPairFactory_iface)))

static HRESULT WINAPI ep_fac_QI(void *This, REFIID riid, void **ppv)
{
    struct endpoint_pair_factory_impl *impl = impl_from_ep_factory(This);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppv = &impl->IActivationFactory_iface;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IEndpointPairFactory))
    {
        *ppv = &impl->IEndpointPairFactory_iface;
        return S_OK;
    }
    return E_NOINTERFACE;
}

static HRESULT WINAPI ep_pair_fac_QI(void *This, REFIID riid, void **ppv)
{
    struct endpoint_pair_factory_impl *impl = impl_from_ep_pair_factory(This);
    return ep_fac_QI(&impl->IActivationFactory_iface, riid, ppv);
}
static ULONG WINAPI ep_fac_AddRef(void *This) { return 2; }
static ULONG WINAPI ep_fac_Release(void *This) { return 1; }
static HRESULT WINAPI ep_fac_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI ep_fac_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.Networking.EndpointPair", 31, n); }
static HRESULT WINAPI ep_fac_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI ep_fac_ActivateInstance(void *This, IInspectable **instance)
{
    if (!instance) return E_POINTER;
    return create_endpoint_pair(NULL, NULL, NULL, NULL, (void **)instance);
}

static HRESULT WINAPI ep_pair_fac_CreateEndpointPair(void *This, void *local_host, HSTRING local_service,
                                                     void *remote_host, HSTRING remote_service,
                                                     void **value)
{
    TRACE("CreateEndpointPair local_host=%p, remote_host=%p, value=%p\n", local_host, remote_host, value);
    if (!value) return E_POINTER;
    return create_endpoint_pair(local_host, local_service, remote_host, remote_service, value);
}

static const void *endpoint_pair_activation_factory_vtbl[] = {
    (void *)ep_fac_QI,
    (void *)ep_fac_AddRef,
    (void *)ep_fac_Release,
    (void *)ep_fac_GetIids,
    (void *)ep_fac_GetRuntimeClassName,
    (void *)ep_fac_GetTrustLevel,
    (void *)ep_fac_ActivateInstance
};

static const void *endpoint_pair_custom_factory_vtbl[] = {
    (void *)ep_pair_fac_QI,
    (void *)ep_fac_AddRef,
    (void *)ep_fac_Release,
    (void *)ep_fac_GetIids,
    (void *)ep_fac_GetRuntimeClassName,
    (void *)ep_fac_GetTrustLevel,
    (void *)ep_pair_fac_CreateEndpointPair
};

static struct endpoint_pair_factory_impl endpoint_pair_factory = {
    { endpoint_pair_activation_factory_vtbl },
    { endpoint_pair_custom_factory_vtbl }
};

/* -------------------------------------------------------------------------
 * Windows.Networking.Sockets.StreamSocket Implementation
 * ------------------------------------------------------------------------- */
#include "manifest_data.h"
#include "archive_manifests_data.h"
#include "archive_manifests_data_ext.h"

static const GUID IID_IStreamSocket = {0x69a22cf3, 0xfc7b, 0x4857, {0xaf, 0x38, 0xf6, 0xe7, 0xde, 0x6a, 0x5b, 0x49}};
static const GUID IID_IStreamSocketControl = {0xfe25adf1, 0x92ab, 0x4af3, {0x99, 0x92, 0x0f, 0x4c, 0x85, 0xe3, 0x6c, 0xc4}};
static const GUID IID_IStreamSocketInformation = {0x3b80ae30, 0x5e68, 0x4205, {0x88, 0xf0, 0xdc, 0x85, 0xd2, 0xe2, 0x5d, 0xed}};
static const GUID IID_IAsyncAction = {0x5a648006, 0x843a, 0x4da9, {0x86, 0x5b, 0x9d, 0x26, 0xe5, 0xdf, 0xad, 0x7b}};
static const GUID IID_IClosable = {0x30d5a829, 0x7fa4, 0x4026, {0x83, 0xbb, 0xd7, 0x5b, 0xae, 0x4e, 0xa9, 0x9e}};
static const GUID IID_IInputStream = {0x905a0fe2, 0xbc53, 0x11df, {0x8c, 0x49, 0x00, 0x1e, 0x4f, 0xc6, 0x86, 0xda}};
static const GUID IID_IOutputStream = {0x905a0fe6, 0xbc53, 0x11df, {0x8c, 0x49, 0x00, 0x1e, 0x4f, 0xc6, 0x86, 0xda}};
static const GUID IID_IBufferByteAccess = {0x905a0fef, 0xbc53, 0x11df, {0x8c, 0x49, 0x00, 0x1e, 0x4f, 0xc6, 0x86, 0xda}};

struct socket_async_action
{
    struct { const void *lpVtbl; } IAsyncAction_iface;
    struct { const void *lpVtbl; } IAsyncInfo_iface;
    LONG ref;
    AsyncStatus status;
    HRESULT error_code;
    IUnknown *completed_handler;
};

#define impl_from_socket_action(ptr) ((struct socket_async_action *)((char *)(ptr) - offsetof(struct socket_async_action, IAsyncAction_iface)))
#define impl_from_socket_action_info(ptr) ((struct socket_async_action *)((char *)(ptr) - offsetof(struct socket_async_action, IAsyncInfo_iface)))

static HRESULT WINAPI sock_action_QI(void *This, REFIID riid, void **ppv)
{
    struct socket_async_action *impl = impl_from_socket_action(This);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IAsyncAction))
    {
        *ppv = &impl->IAsyncAction_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IAsyncInfo))
    {
        *ppv = &impl->IAsyncInfo_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    return E_NOINTERFACE;
}

static ULONG WINAPI sock_action_AddRef(void *This)
{
    struct socket_async_action *impl = impl_from_socket_action(This);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI sock_action_Release(void *This)
{
    struct socket_async_action *impl = impl_from_socket_action(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref)
    {
        if (impl->completed_handler) IUnknown_Release(impl->completed_handler);
        free(impl);
    }
    return ref;
}

static HRESULT WINAPI sock_action_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI sock_action_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.Foundation.IAsyncAction", 31, n); }
static HRESULT WINAPI sock_action_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI sock_action_put_Completed(void *This, void *handler)
{
    struct socket_async_action *impl = impl_from_socket_action(This);
    MESSAGE("windows.networking: sock_action_put_Completed impl %p, handler %p, status %d, error_code 0x%08lx\n",
            impl, handler, impl->status, impl->error_code);
    if (impl->completed_handler) IUnknown_Release(impl->completed_handler);
    impl->completed_handler = (IUnknown *)handler;
    if (impl->completed_handler)
    {
        void **vtbl = *(void ***)handler;
        HRESULT (WINAPI *invoke_fn)(void *, void *, AsyncStatus) = (void *)vtbl[3];
        IUnknown_AddRef(impl->completed_handler);
        invoke_fn(handler, &impl->IAsyncAction_iface, impl->status);
    }
    return S_OK;
}

static HRESULT WINAPI sock_action_get_Completed(void *This, void **handler)
{
    struct socket_async_action *impl = impl_from_socket_action(This);
    if (!handler) return E_POINTER;
    *handler = impl->completed_handler;
    if (impl->completed_handler) IUnknown_AddRef(impl->completed_handler);
    return S_OK;
}

static HRESULT WINAPI sock_action_GetResults(void *This)
{
    struct socket_async_action *impl = impl_from_socket_action(This);
    MESSAGE("windows.networking: sock_action_GetResults impl %p, error_code 0x%08lx\n", impl, impl->error_code);
    return impl->error_code;
}

static const void *sock_action_vtbl[] = {
    (void *)sock_action_QI,
    (void *)sock_action_AddRef,
    (void *)sock_action_Release,
    (void *)sock_action_GetIids,
    (void *)sock_action_GetRuntimeClassName,
    (void *)sock_action_GetTrustLevel,
    (void *)sock_action_put_Completed,
    (void *)sock_action_get_Completed,
    (void *)sock_action_GetResults
};

static HRESULT WINAPI sock_info_QI(void *This, REFIID riid, void **ppv)
{
    struct socket_async_action *impl = impl_from_socket_action_info(This);
    return sock_action_QI(&impl->IAsyncAction_iface, riid, ppv);
}
static ULONG WINAPI sock_info_AddRef(void *This)
{
    struct socket_async_action *impl = impl_from_socket_action_info(This);
    return sock_action_AddRef(&impl->IAsyncAction_iface);
}
static ULONG WINAPI sock_info_Release(void *This)
{
    struct socket_async_action *impl = impl_from_socket_action_info(This);
    return sock_action_Release(&impl->IAsyncAction_iface);
}
static HRESULT WINAPI sock_info_get_Id(void *This, UINT32 *id) { if (id) *id = 1; return S_OK; }
static HRESULT WINAPI sock_info_get_Status(void *This, AsyncStatus *status)
{
    struct socket_async_action *impl = impl_from_socket_action_info(This);
    if (!status) return E_POINTER;
    *status = impl->status;
    return S_OK;
}
static HRESULT WINAPI sock_info_get_ErrorCode(void *This, HRESULT *ec)
{
    struct socket_async_action *impl = impl_from_socket_action_info(This);
    if (!ec) return E_POINTER;
    *ec = impl->error_code;
    return S_OK;
}
static HRESULT WINAPI sock_info_Cancel(void *This) { return S_OK; }
static HRESULT WINAPI sock_info_Close(void *This) { return S_OK; }

static const void *sock_info_vtbl[] = {
    (void *)sock_info_QI,
    (void *)sock_info_AddRef,
    (void *)sock_info_Release,
    (void *)sock_action_GetIids,
    (void *)sock_action_GetRuntimeClassName,
    (void *)sock_action_GetTrustLevel,
    (void *)sock_info_get_Id,
    (void *)sock_info_get_Status,
    (void *)sock_info_get_ErrorCode,
    (void *)sock_info_Cancel,
    (void *)sock_info_Close
};

static HRESULT create_socket_async_action(HRESULT error_code, void **out)
{
    struct socket_async_action *impl = calloc(1, sizeof(*impl));
    if (!impl) return E_OUTOFMEMORY;
    impl->IAsyncAction_iface.lpVtbl = sock_action_vtbl;
    impl->IAsyncInfo_iface.lpVtbl = sock_info_vtbl;
    impl->ref = 1;
    impl->status = FAILED(error_code) ? Error : Completed;
    impl->error_code = error_code;
    *out = &impl->IAsyncAction_iface;
    return S_OK;
}

struct stream_socket_control_impl
{
    struct { const void *lpVtbl; } IStreamSocketControl_iface;
    boolean no_delay;
    boolean keep_alive;
    UINT32 outbound_buffer_size;
    int qos;
    BYTE hop_limit;
};

struct stream_socket_information_impl
{
    struct { const void *lpVtbl; } IStreamSocketInformation_iface;
};

struct socket_async_op_read_buffer;
struct stream_socket_impl;
static void stream_socket_cleanup(struct stream_socket_impl *impl);

struct stream_socket_impl
{
    struct { const void *lpVtbl; } IStreamSocket_iface;
    struct { const void *lpVtbl; } IClosable_iface;
    LONG ref;
    struct stream_socket_control_impl control;
    struct stream_socket_information_impl information;
    struct {
        struct { const void *lpVtbl; } IInputStream_iface;
        struct { const void *lpVtbl; } IClosable_iface;
    } input_stream;
    struct {
        struct { const void *lpVtbl; } IOutputStream_iface;
        struct { const void *lpVtbl; } IClosable_iface;
    } output_stream;
    char remote_host[256];
    char req_buf[8192];
    UINT32 req_len;
    char resp_buf[65536];
    UINT32 resp_len;
    UINT32 resp_pos;
    struct socket_async_op_read_buffer *pending_read;
    CRITICAL_SECTION cs;
};

#define impl_from_stream_socket(ptr) ((struct stream_socket_impl *)((char *)(ptr) - offsetof(struct stream_socket_impl, IStreamSocket_iface)))
#define impl_from_stream_socket_closable(ptr) ((struct stream_socket_impl *)((char *)(ptr) - offsetof(struct stream_socket_impl, IClosable_iface)))
#define impl_from_stream_sock_control(ptr) ((struct stream_socket_impl *)((char *)(ptr) - offsetof(struct stream_socket_impl, control.IStreamSocketControl_iface)))
#define impl_from_stream_sock_information(ptr) ((struct stream_socket_impl *)((char *)(ptr) - offsetof(struct stream_socket_impl, information.IStreamSocketInformation_iface)))
#define impl_from_stream_input(ptr) ((struct stream_socket_impl *)((char *)(ptr) - offsetof(struct stream_socket_impl, input_stream.IInputStream_iface)))
#define impl_from_stream_input_closable(ptr) ((struct stream_socket_impl *)((char *)(ptr) - offsetof(struct stream_socket_impl, input_stream.IClosable_iface)))
#define impl_from_stream_output(ptr) ((struct stream_socket_impl *)((char *)(ptr) - offsetof(struct stream_socket_impl, output_stream.IOutputStream_iface)))
#define impl_from_stream_output_closable(ptr) ((struct stream_socket_impl *)((char *)(ptr) - offsetof(struct stream_socket_impl, output_stream.IClosable_iface)))

static HRESULT WINAPI stream_sock_QI(void *This, REFIID riid, void **ppv)
{
    struct stream_socket_impl *impl = impl_from_stream_socket(This);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IStreamSocket))
    {
        *ppv = &impl->IStreamSocket_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IClosable))
    {
        *ppv = &impl->IClosable_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    return E_NOINTERFACE;
}

static ULONG WINAPI stream_sock_AddRef(void *This)
{
    struct stream_socket_impl *impl = impl_from_stream_socket(This);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI stream_sock_Release(void *This)
{
    struct stream_socket_impl *impl = impl_from_stream_socket(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref)
    {
        stream_socket_cleanup(impl);
        DeleteCriticalSection(&impl->cs);
        free(impl);
    }
    return ref;
}

static HRESULT WINAPI stream_sock_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI stream_sock_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.Networking.Sockets.StreamSocket", 39, n); }
static HRESULT WINAPI stream_sock_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

/* IStreamSocketControl */
static HRESULT WINAPI sock_ctrl_QI(void *This, REFIID riid, void **ppv)
{
    struct stream_socket_impl *impl = impl_from_stream_sock_control(This);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IStreamSocketControl))
    {
        *ppv = &impl->control.IStreamSocketControl_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI sock_ctrl_AddRef(void *This)
{
    struct stream_socket_impl *impl = impl_from_stream_sock_control(This);
    return InterlockedIncrement(&impl->ref);
}
static ULONG WINAPI sock_ctrl_Release(void *This)
{
    struct stream_socket_impl *impl = impl_from_stream_sock_control(This);
    return stream_sock_Release(&impl->IStreamSocket_iface);
}
static HRESULT WINAPI sock_ctrl_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI sock_ctrl_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.Networking.Sockets.StreamSocketControl", 46, n); }
static HRESULT WINAPI sock_ctrl_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI sock_ctrl_get_NoDelay(void *This, boolean *value)
{
    struct stream_socket_impl *impl = impl_from_stream_sock_control(This);
    if (!value) return E_POINTER;
    *value = impl->control.no_delay;
    return S_OK;
}
static HRESULT WINAPI sock_ctrl_put_NoDelay(void *This, boolean value)
{
    struct stream_socket_impl *impl = impl_from_stream_sock_control(This);
    impl->control.no_delay = value;
    return S_OK;
}
static HRESULT WINAPI sock_ctrl_get_KeepAlive(void *This, boolean *value)
{
    struct stream_socket_impl *impl = impl_from_stream_sock_control(This);
    if (!value) return E_POINTER;
    *value = impl->control.keep_alive;
    return S_OK;
}
static HRESULT WINAPI sock_ctrl_put_KeepAlive(void *This, boolean value)
{
    struct stream_socket_impl *impl = impl_from_stream_sock_control(This);
    impl->control.keep_alive = value;
    return S_OK;
}
static HRESULT WINAPI sock_ctrl_get_OutboundBufferSizeInBytes(void *This, UINT32 *value)
{
    struct stream_socket_impl *impl = impl_from_stream_sock_control(This);
    if (!value) return E_POINTER;
    *value = impl->control.outbound_buffer_size;
    return S_OK;
}
static HRESULT WINAPI sock_ctrl_put_OutboundBufferSizeInBytes(void *This, UINT32 value)
{
    struct stream_socket_impl *impl = impl_from_stream_sock_control(This);
    impl->control.outbound_buffer_size = value;
    return S_OK;
}
static HRESULT WINAPI sock_ctrl_get_QualityOfService(void *This, int *value)
{
    struct stream_socket_impl *impl = impl_from_stream_sock_control(This);
    if (!value) return E_POINTER;
    *value = impl->control.qos;
    return S_OK;
}
static HRESULT WINAPI sock_ctrl_put_QualityOfService(void *This, int value)
{
    struct stream_socket_impl *impl = impl_from_stream_sock_control(This);
    impl->control.qos = value;
    return S_OK;
}
static HRESULT WINAPI sock_ctrl_get_OutboundUnicastHopLimit(void *This, BYTE *value)
{
    struct stream_socket_impl *impl = impl_from_stream_sock_control(This);
    if (!value) return E_POINTER;
    *value = impl->control.hop_limit;
    return S_OK;
}
static HRESULT WINAPI sock_ctrl_put_OutboundUnicastHopLimit(void *This, BYTE value)
{
    struct stream_socket_impl *impl = impl_from_stream_sock_control(This);
    impl->control.hop_limit = value;
    return S_OK;
}

static const void *stream_socket_control_vtbl[] = {
    (void *)sock_ctrl_QI,
    (void *)sock_ctrl_AddRef,
    (void *)sock_ctrl_Release,
    (void *)sock_ctrl_GetIids,
    (void *)sock_ctrl_GetRuntimeClassName,
    (void *)sock_ctrl_GetTrustLevel,
    (void *)sock_ctrl_get_NoDelay,
    (void *)sock_ctrl_put_NoDelay,
    (void *)sock_ctrl_get_KeepAlive,
    (void *)sock_ctrl_put_KeepAlive,
    (void *)sock_ctrl_get_OutboundBufferSizeInBytes,
    (void *)sock_ctrl_put_OutboundBufferSizeInBytes,
    (void *)sock_ctrl_get_QualityOfService,
    (void *)sock_ctrl_put_QualityOfService,
    (void *)sock_ctrl_get_OutboundUnicastHopLimit,
    (void *)sock_ctrl_put_OutboundUnicastHopLimit
};

/* IStreamSocketInformation */
static HRESULT WINAPI sock_info_stat_QI(void *This, REFIID riid, void **ppv)
{
    struct stream_socket_impl *impl = impl_from_stream_sock_information(This);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IStreamSocketInformation))
    {
        *ppv = &impl->information.IStreamSocketInformation_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI sock_info_stat_AddRef(void *This)
{
    struct stream_socket_impl *impl = impl_from_stream_sock_information(This);
    return InterlockedIncrement(&impl->ref);
}
static ULONG WINAPI sock_info_stat_Release(void *This)
{
    struct stream_socket_impl *impl = impl_from_stream_sock_information(This);
    return stream_sock_Release(&impl->IStreamSocket_iface);
}
static HRESULT WINAPI sock_info_stat_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI sock_info_stat_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.Networking.Sockets.StreamSocketInformation", 50, n); }
static HRESULT WINAPI sock_info_stat_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI sock_info_stat_get_LocalAddress(void *This, void **value) { if (value) *value = NULL; return S_OK; }
static HRESULT WINAPI sock_info_stat_get_LocalPort(void *This, HSTRING *value) { return WindowsCreateString(L"", 0, value); }
static HRESULT WINAPI sock_info_stat_get_RemoteAddress(void *This, void **value) { if (value) *value = NULL; return S_OK; }
static HRESULT WINAPI sock_info_stat_get_RemotePort(void *This, HSTRING *value) { return WindowsCreateString(L"", 0, value); }
static HRESULT WINAPI sock_info_stat_get_RoundTripTimeStatistics(void *This, void *value) { return S_OK; }
static HRESULT WINAPI sock_info_stat_get_BandwidthStatistics(void *This, void *value) { return S_OK; }
static HRESULT WINAPI sock_info_stat_get_ProtectionLevel(void *This, int *value) { if (value) *value = 0; return S_OK; }
static HRESULT WINAPI sock_info_stat_get_ServerCertificate(void *This, void **value) { if (value) *value = NULL; return S_OK; }

static const void *stream_socket_information_vtbl[] = {
    (void *)sock_info_stat_QI,
    (void *)sock_info_stat_AddRef,
    (void *)sock_info_stat_Release,
    (void *)sock_info_stat_GetIids,
    (void *)sock_info_stat_GetRuntimeClassName,
    (void *)sock_info_stat_GetTrustLevel,
    (void *)sock_info_stat_get_LocalAddress,
    (void *)sock_info_stat_get_LocalPort,
    (void *)sock_info_stat_get_RemoteAddress,
    (void *)sock_info_stat_get_RemotePort,
    (void *)sock_info_stat_get_RoundTripTimeStatistics,
    (void *)sock_info_stat_get_BandwidthStatistics,
    (void *)sock_info_stat_get_ProtectionLevel,
    (void *)sock_info_stat_get_ServerCertificate
};

static HRESULT WINAPI stream_sock_get_Control(void *This, void **value)
{
    struct stream_socket_impl *impl = impl_from_stream_socket(This);
    if (!value) return E_POINTER;
    *value = &impl->control.IStreamSocketControl_iface;
    sock_ctrl_AddRef(*value);
    return S_OK;
}
static HRESULT WINAPI stream_sock_get_Information(void *This, void **value)
{
    struct stream_socket_impl *impl = impl_from_stream_socket(This);
    if (!value) return E_POINTER;
    *value = &impl->information.IStreamSocketInformation_iface;
    sock_info_stat_AddRef(*value);
    return S_OK;
}
/* Forward declarations of stream vtables */
static const void *stream_input_vtbl[];
static const void *stream_input_closable_vtbl[];
static const void *stream_output_vtbl[];
static const void *stream_output_closable_vtbl[];

/* Async operations for stream reading, writing, and flushing */
struct socket_async_op_read_buffer
{
    struct { const void *lpVtbl; } IAsyncOperationWithProgress_iface;
    struct { const void *lpVtbl; } IAsyncInfo_iface;
    LONG ref;
    AsyncStatus status;
    HRESULT error_code;
    void *buffer;
    UINT32 bytes_read;
    UINT32 requested_count;
    IUnknown *completed_handler;
    struct stream_socket_impl *sock_impl;
};
#define impl_from_async_read_op(ptr) ((struct socket_async_op_read_buffer *)((char *)(ptr) - offsetof(struct socket_async_op_read_buffer, IAsyncOperationWithProgress_iface)))
#define impl_from_async_read_info(ptr) ((struct socket_async_op_read_buffer *)((char *)(ptr) - offsetof(struct socket_async_op_read_buffer, IAsyncInfo_iface)))

static HRESULT WINAPI async_read_QI(void *This, REFIID riid, void **ppv)
{
    struct socket_async_op_read_buffer *impl = impl_from_async_read_op(This);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IAsyncInfo))
    {
        *ppv = &impl->IAsyncInfo_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    *ppv = &impl->IAsyncOperationWithProgress_iface;
    InterlockedIncrement(&impl->ref);
    return S_OK;
}
static ULONG WINAPI async_read_AddRef(void *This)
{
    struct socket_async_op_read_buffer *impl = impl_from_async_read_op(This);
    return InterlockedIncrement(&impl->ref);
}
static ULONG WINAPI async_read_Release(void *This)
{
    struct socket_async_op_read_buffer *impl = impl_from_async_read_op(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref)
    {
        if (impl->completed_handler) IUnknown_Release(impl->completed_handler);
        if (impl->buffer)
        {
            void **bvtbl = *(void ***)impl->buffer;
            ULONG (WINAPI *b_rel)(void *) = (void *)bvtbl[2];
            b_rel(impl->buffer);
        }
        if (impl->sock_impl && impl->sock_impl->pending_read == impl)
        {
            impl->sock_impl->pending_read = NULL;
        }
        free(impl);
    }
    return ref;
}

static void stream_socket_cleanup(struct stream_socket_impl *impl)
{
    struct socket_async_op_read_buffer *op = NULL;
    EnterCriticalSection(&impl->cs);
    op = impl->pending_read;
    impl->pending_read = NULL;
    if (op) op->sock_impl = NULL;
    LeaveCriticalSection(&impl->cs);
    if (op)
    {
        IUnknown_Release((IUnknown *)&op->IAsyncOperationWithProgress_iface);
    }
}

static HRESULT WINAPI async_read_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI async_read_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.Foundation.IAsyncOperationWithProgress`2<Windows.Storage.Streams.IBuffer, UInt32>", 76, n); }
static HRESULT WINAPI async_read_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI async_read_put_Progress(void *This, void *handler) { return S_OK; }
static HRESULT WINAPI async_read_get_Progress(void *This, void **handler) { if (handler) *handler = NULL; return S_OK; }
static DWORD WINAPI async_read_worker(void *param)
{
    struct socket_async_op_read_buffer *impl = (struct socket_async_op_read_buffer *)param;
    IUnknown *handler;
    AsyncStatus status;

    if ((handler = impl->completed_handler))
    {
        void **vtbl = *(void ***)handler;
        HRESULT (WINAPI *invoke_fn)(void *, void *, AsyncStatus) = (void *)vtbl[3];
        IUnknown_AddRef(handler);
        status = impl->status;
        MESSAGE("windows.networking: async_read_worker invoking handler %p, status %d, bytes %u\n",
                handler, status, impl->bytes_read);
        invoke_fn(handler, &impl->IAsyncOperationWithProgress_iface, status);
        IUnknown_Release(handler);
    }
    async_read_Release(&impl->IAsyncOperationWithProgress_iface);
    return 0;
}

static HRESULT WINAPI async_read_put_Completed(void *This, void *handler)
{
    struct socket_async_op_read_buffer *impl = impl_from_async_read_op(This);
    MESSAGE("windows.networking: async_read_put_Completed impl %p, handler %p, status %d, bytes %u\n",
            impl, handler, impl->status, impl->bytes_read);
    if (impl->completed_handler) IUnknown_Release(impl->completed_handler);
    impl->completed_handler = (IUnknown *)handler;
    if (impl->completed_handler)
    {
        IUnknown_AddRef(impl->completed_handler);
        if (impl->status != Started)
        {
            async_read_AddRef(&impl->IAsyncOperationWithProgress_iface);
            if (!QueueUserWorkItem(async_read_worker, impl, WT_EXECUTEDEFAULT))
            {
                void **vtbl = *(void ***)handler;
                HRESULT (WINAPI *invoke_fn)(void *, void *, AsyncStatus) = (void *)vtbl[3];
                WARN("QueueUserWorkItem failed, falling back to synchronous completion\n");
                invoke_fn(handler, &impl->IAsyncOperationWithProgress_iface, impl->status);
                async_read_Release(&impl->IAsyncOperationWithProgress_iface);
            }
        }
    }
    return S_OK;
}
static HRESULT WINAPI async_read_get_Completed(void *This, void **handler)
{
    struct socket_async_op_read_buffer *impl = impl_from_async_read_op(This);
    if (!handler) return E_POINTER;
    *handler = impl->completed_handler;
    if (*handler) IUnknown_AddRef((IUnknown *)*handler);
    return S_OK;
}
static HRESULT WINAPI async_read_GetResults(void *This, void **results)
{
    struct socket_async_op_read_buffer *impl = impl_from_async_read_op(This);
    MESSAGE("windows.networking: async_read_GetResults impl %p, buffer %p, bytes %u\n", impl, impl->buffer, impl->bytes_read);
    if (!results) return E_POINTER;
    *results = impl->buffer;
    if (*results)
    {
        void **bvtbl = *(void ***)impl->buffer;
        ULONG (WINAPI *b_addref)(void *) = (void *)bvtbl[1];
        b_addref(impl->buffer);
    }
    return S_OK;
}
static const void *async_read_op_vtbl[] = {
    (void *)async_read_QI,
    (void *)async_read_AddRef,
    (void *)async_read_Release,
    (void *)async_read_GetIids,
    (void *)async_read_GetRuntimeClassName,
    (void *)async_read_GetTrustLevel,
    (void *)async_read_put_Progress,
    (void *)async_read_get_Progress,
    (void *)async_read_put_Completed,
    (void *)async_read_get_Completed,
    (void *)async_read_GetResults
};

static HRESULT WINAPI async_read_info_QI(void *This, REFIID riid, void **ppv)
{
    struct socket_async_op_read_buffer *impl = impl_from_async_read_info(This);
    return async_read_QI(&impl->IAsyncOperationWithProgress_iface, riid, ppv);
}
static ULONG WINAPI async_read_info_AddRef(void *This)
{
    struct socket_async_op_read_buffer *impl = impl_from_async_read_info(This);
    return async_read_AddRef(&impl->IAsyncOperationWithProgress_iface);
}
static ULONG WINAPI async_read_info_Release(void *This)
{
    struct socket_async_op_read_buffer *impl = impl_from_async_read_info(This);
    return async_read_Release(&impl->IAsyncOperationWithProgress_iface);
}
static HRESULT WINAPI async_read_info_get_Id(void *This, UINT32 *id) { if (id) *id = 1; return S_OK; }
static HRESULT WINAPI async_read_info_get_Status(void *This, AsyncStatus *status)
{
    struct socket_async_op_read_buffer *impl = impl_from_async_read_info(This);
    if (!status) return E_POINTER;
    *status = impl->status;
    return S_OK;
}
static HRESULT WINAPI async_read_info_get_ErrorCode(void *This, HRESULT *ec)
{
    struct socket_async_op_read_buffer *impl = impl_from_async_read_info(This);
    if (!ec) return E_POINTER;
    *ec = impl->error_code;
    return S_OK;
}
static HRESULT WINAPI async_read_info_Cancel(void *This) { return S_OK; }
static HRESULT WINAPI async_read_info_Close(void *This) { return S_OK; }

static const void *async_read_info_vtbl[] = {
    (void *)async_read_info_QI,
    (void *)async_read_info_AddRef,
    (void *)async_read_info_Release,
    (void *)async_read_GetIids,
    (void *)async_read_GetRuntimeClassName,
    (void *)async_read_GetTrustLevel,
    (void *)async_read_info_get_Id,
    (void *)async_read_info_get_Status,
    (void *)async_read_info_get_ErrorCode,
    (void *)async_read_info_Cancel,
    (void *)async_read_info_Close
};

static HRESULT create_async_op_read_buffer(void *buffer, UINT32 bytes_read, void **out)
{
    struct socket_async_op_read_buffer *impl = calloc(1, sizeof(*impl));
    if (!impl) return E_OUTOFMEMORY;
    impl->IAsyncOperationWithProgress_iface.lpVtbl = async_read_op_vtbl;
    impl->IAsyncInfo_iface.lpVtbl = async_read_info_vtbl;
    impl->ref = 1;
    impl->status = Completed;
    impl->error_code = S_OK;
    impl->buffer = buffer;
    impl->bytes_read = bytes_read;
    if (buffer)
    {
        void **bvtbl = *(void ***)buffer;
        ULONG (WINAPI *b_addref)(void *) = (void *)bvtbl[1];
        b_addref(buffer);
    }
    *out = &impl->IAsyncOperationWithProgress_iface;
    return S_OK;
}

static HRESULT create_async_op_read_buffer_pending(void *buffer, UINT32 requested_count, struct stream_socket_impl *sock_impl, struct socket_async_op_read_buffer **out)
{
    struct socket_async_op_read_buffer *impl = calloc(1, sizeof(*impl));
    if (!impl) return E_OUTOFMEMORY;
    impl->IAsyncOperationWithProgress_iface.lpVtbl = async_read_op_vtbl;
    impl->IAsyncInfo_iface.lpVtbl = async_read_info_vtbl;
    impl->ref = 1;
    impl->status = Started;
    impl->error_code = S_OK;
    impl->buffer = buffer;
    impl->requested_count = requested_count;
    impl->sock_impl = sock_impl;
    if (buffer)
    {
        void **bvtbl = *(void ***)buffer;
        ULONG (WINAPI *b_addref)(void *) = (void *)bvtbl[1];
        b_addref(buffer);
    }
    *out = impl;
    return S_OK;
}

struct socket_async_op_write_uint32
{
    struct { const void *lpVtbl; } IAsyncOperationWithProgress_iface;
    struct { const void *lpVtbl; } IAsyncInfo_iface;
    LONG ref;
    AsyncStatus status;
    HRESULT error_code;
    UINT32 bytes_written;
    IUnknown *completed_handler;
};
#define impl_from_async_write_op(ptr) ((struct socket_async_op_write_uint32 *)((char *)(ptr) - offsetof(struct socket_async_op_write_uint32, IAsyncOperationWithProgress_iface)))
#define impl_from_async_write_info(ptr) ((struct socket_async_op_write_uint32 *)((char *)(ptr) - offsetof(struct socket_async_op_write_uint32, IAsyncInfo_iface)))

static HRESULT WINAPI async_write_QI(void *This, REFIID riid, void **ppv)
{
    struct socket_async_op_write_uint32 *impl = impl_from_async_write_op(This);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IAsyncInfo))
    {
        *ppv = &impl->IAsyncInfo_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    *ppv = &impl->IAsyncOperationWithProgress_iface;
    InterlockedIncrement(&impl->ref);
    return S_OK;
}
static ULONG WINAPI async_write_AddRef(void *This)
{
    struct socket_async_op_write_uint32 *impl = impl_from_async_write_op(This);
    return InterlockedIncrement(&impl->ref);
}
static ULONG WINAPI async_write_Release(void *This)
{
    struct socket_async_op_write_uint32 *impl = impl_from_async_write_op(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref)
    {
        if (impl->completed_handler) IUnknown_Release(impl->completed_handler);
        free(impl);
    }
    return ref;
}
static HRESULT WINAPI async_write_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI async_write_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.Foundation.IAsyncOperationWithProgress`2<UInt32, UInt32>", 62, n); }
static HRESULT WINAPI async_write_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI async_write_put_Progress(void *This, void *handler) { return S_OK; }
static HRESULT WINAPI async_write_get_Progress(void *This, void **handler) { if (handler) *handler = NULL; return S_OK; }
static DWORD WINAPI async_write_worker(void *param)
{
    struct socket_async_op_write_uint32 *impl = (struct socket_async_op_write_uint32 *)param;
    IUnknown *handler;
    AsyncStatus status;

    if ((handler = impl->completed_handler))
    {
        void **vtbl = *(void ***)handler;
        HRESULT (WINAPI *invoke_fn)(void *, void *, AsyncStatus) = (void *)vtbl[3];
        IUnknown_AddRef(handler);
        status = impl->status;
        MESSAGE("windows.networking: async_write_worker invoking handler %p, status %d, bytes %u\n",
                handler, status, impl->bytes_written);
        invoke_fn(handler, &impl->IAsyncOperationWithProgress_iface, status);
        IUnknown_Release(handler);
    }
    async_write_Release(&impl->IAsyncOperationWithProgress_iface);
    return 0;
}

static HRESULT WINAPI async_write_put_Completed(void *This, void *handler)
{
    struct socket_async_op_write_uint32 *impl = impl_from_async_write_op(This);
    MESSAGE("windows.networking: async_write_put_Completed impl %p, handler %p, status %d, bytes %u\n",
            impl, handler, impl->status, impl->bytes_written);
    if (impl->completed_handler) IUnknown_Release(impl->completed_handler);
    impl->completed_handler = (IUnknown *)handler;
    if (impl->completed_handler)
    {
        IUnknown_AddRef(impl->completed_handler);
        async_write_AddRef(&impl->IAsyncOperationWithProgress_iface);
        if (!QueueUserWorkItem(async_write_worker, impl, WT_EXECUTEDEFAULT))
        {
            void **vtbl = *(void ***)handler;
            HRESULT (WINAPI *invoke_fn)(void *, void *, AsyncStatus) = (void *)vtbl[3];
            WARN("QueueUserWorkItem failed, falling back to synchronous completion\n");
            invoke_fn(handler, &impl->IAsyncOperationWithProgress_iface, impl->status);
            async_write_Release(&impl->IAsyncOperationWithProgress_iface);
        }
    }
    return S_OK;
}
static HRESULT WINAPI async_write_get_Completed(void *This, void **handler)
{
    struct socket_async_op_write_uint32 *impl = impl_from_async_write_op(This);
    if (!handler) return E_POINTER;
    *handler = impl->completed_handler;
    if (*handler) IUnknown_AddRef((IUnknown *)*handler);
    return S_OK;
}
static HRESULT WINAPI async_write_GetResults(void *This, UINT32 *results)
{
    struct socket_async_op_write_uint32 *impl = impl_from_async_write_op(This);
    MESSAGE("windows.networking: async_write_GetResults impl %p, results %p (returning %u)\n",
            impl, results, impl->bytes_written);
    if (!results) return E_POINTER;
    *results = impl->bytes_written;
    return S_OK;
}
static const void *async_write_op_vtbl[] = {
    (void *)async_write_QI,
    (void *)async_write_AddRef,
    (void *)async_write_Release,
    (void *)async_write_GetIids,
    (void *)async_write_GetRuntimeClassName,
    (void *)async_write_GetTrustLevel,
    (void *)async_write_put_Progress,
    (void *)async_write_get_Progress,
    (void *)async_write_put_Completed,
    (void *)async_write_get_Completed,
    (void *)async_write_GetResults
};

static HRESULT WINAPI async_write_info_QI(void *This, REFIID riid, void **ppv)
{
    struct socket_async_op_write_uint32 *impl = impl_from_async_write_info(This);
    return async_write_QI(&impl->IAsyncOperationWithProgress_iface, riid, ppv);
}
static ULONG WINAPI async_write_info_AddRef(void *This)
{
    struct socket_async_op_write_uint32 *impl = impl_from_async_write_info(This);
    return async_write_AddRef(&impl->IAsyncOperationWithProgress_iface);
}
static ULONG WINAPI async_write_info_Release(void *This)
{
    struct socket_async_op_write_uint32 *impl = impl_from_async_write_info(This);
    return async_write_Release(&impl->IAsyncOperationWithProgress_iface);
}
static HRESULT WINAPI async_write_info_get_Id(void *This, UINT32 *id) { if (id) *id = 1; return S_OK; }
static HRESULT WINAPI async_write_info_get_Status(void *This, AsyncStatus *status)
{
    struct socket_async_op_write_uint32 *impl = impl_from_async_write_info(This);
    if (!status) return E_POINTER;
    *status = impl->status;
    return S_OK;
}
static HRESULT WINAPI async_write_info_get_ErrorCode(void *This, HRESULT *ec)
{
    struct socket_async_op_write_uint32 *impl = impl_from_async_write_info(This);
    if (!ec) return E_POINTER;
    *ec = impl->error_code;
    return S_OK;
}
static HRESULT WINAPI async_write_info_Cancel(void *This) { return S_OK; }
static HRESULT WINAPI async_write_info_Close(void *This) { return S_OK; }

static const void *async_write_info_vtbl[] = {
    (void *)async_write_info_QI,
    (void *)async_write_info_AddRef,
    (void *)async_write_info_Release,
    (void *)async_write_GetIids,
    (void *)async_write_GetRuntimeClassName,
    (void *)async_write_GetTrustLevel,
    (void *)async_write_info_get_Id,
    (void *)async_write_info_get_Status,
    (void *)async_write_info_get_ErrorCode,
    (void *)async_write_info_Cancel,
    (void *)async_write_info_Close
};

static HRESULT create_async_op_write_uint32(UINT32 bytes_written, void **out)
{
    struct socket_async_op_write_uint32 *impl = calloc(1, sizeof(*impl));
    if (!impl) return E_OUTOFMEMORY;
    impl->IAsyncOperationWithProgress_iface.lpVtbl = async_write_op_vtbl;
    impl->IAsyncInfo_iface.lpVtbl = async_write_info_vtbl;
    impl->ref = 1;
    impl->status = Completed;
    impl->error_code = S_OK;
    impl->bytes_written = bytes_written;
    *out = &impl->IAsyncOperationWithProgress_iface;
    return S_OK;
}

struct socket_async_op_flush_bool
{
    struct { const void *lpVtbl; } IAsyncOperation_iface;
    struct { const void *lpVtbl; } IAsyncInfo_iface;
    LONG ref;
    AsyncStatus status;
    HRESULT error_code;
    boolean result;
    IUnknown *completed_handler;
};
#define impl_from_async_flush_op(ptr) ((struct socket_async_op_flush_bool *)((char *)(ptr) - offsetof(struct socket_async_op_flush_bool, IAsyncOperation_iface)))
#define impl_from_async_flush_info(ptr) ((struct socket_async_op_flush_bool *)((char *)(ptr) - offsetof(struct socket_async_op_flush_bool, IAsyncInfo_iface)))

static HRESULT WINAPI async_flush_QI(void *This, REFIID riid, void **ppv)
{
    struct socket_async_op_flush_bool *impl = impl_from_async_flush_op(This);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IAsyncInfo))
    {
        *ppv = &impl->IAsyncInfo_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    *ppv = &impl->IAsyncOperation_iface;
    InterlockedIncrement(&impl->ref);
    return S_OK;
}
static ULONG WINAPI async_flush_AddRef(void *This)
{
    struct socket_async_op_flush_bool *impl = impl_from_async_flush_op(This);
    return InterlockedIncrement(&impl->ref);
}
static ULONG WINAPI async_flush_Release(void *This)
{
    struct socket_async_op_flush_bool *impl = impl_from_async_flush_op(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref)
    {
        if (impl->completed_handler) IUnknown_Release(impl->completed_handler);
        free(impl);
    }
    return ref;
}
static HRESULT WINAPI async_flush_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI async_flush_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.Foundation.IAsyncOperation`1<Boolean>", 43, n); }
static HRESULT WINAPI async_flush_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static DWORD WINAPI async_flush_worker(void *param)
{
    struct socket_async_op_flush_bool *impl = (struct socket_async_op_flush_bool *)param;
    IUnknown *handler;
    AsyncStatus status;

    if ((handler = impl->completed_handler))
    {
        void **vtbl = *(void ***)handler;
        HRESULT (WINAPI *invoke_fn)(void *, void *, AsyncStatus) = (void *)vtbl[3];
        IUnknown_AddRef(handler);
        status = impl->status;
        MESSAGE("windows.networking: async_flush_worker invoking handler %p, status %d\n", handler, status);
        invoke_fn(handler, &impl->IAsyncOperation_iface, status);
        IUnknown_Release(handler);
    }
    async_flush_Release(&impl->IAsyncOperation_iface);
    return 0;
}

static HRESULT WINAPI async_flush_put_Completed(void *This, void *handler)
{
    struct socket_async_op_flush_bool *impl = impl_from_async_flush_op(This);
    MESSAGE("windows.networking: async_flush_put_Completed impl %p, handler %p, status %d\n",
            impl, handler, impl->status);
    if (impl->completed_handler) IUnknown_Release(impl->completed_handler);
    impl->completed_handler = (IUnknown *)handler;
    if (impl->completed_handler)
    {
        IUnknown_AddRef(impl->completed_handler);
        async_flush_AddRef(&impl->IAsyncOperation_iface);
        if (!QueueUserWorkItem(async_flush_worker, impl, WT_EXECUTEDEFAULT))
        {
            void **vtbl = *(void ***)handler;
            HRESULT (WINAPI *invoke_fn)(void *, void *, AsyncStatus) = (void *)vtbl[3];
            WARN("QueueUserWorkItem failed, falling back to synchronous completion\n");
            invoke_fn(handler, &impl->IAsyncOperation_iface, impl->status);
            async_flush_Release(&impl->IAsyncOperation_iface);
        }
    }
    return S_OK;
}
static HRESULT WINAPI async_flush_get_Completed(void *This, void **handler)
{
    struct socket_async_op_flush_bool *impl = impl_from_async_flush_op(This);
    if (!handler) return E_POINTER;
    *handler = impl->completed_handler;
    if (*handler) IUnknown_AddRef((IUnknown *)*handler);
    return S_OK;
}
static HRESULT WINAPI async_flush_GetResults(void *This, boolean *results)
{
    struct socket_async_op_flush_bool *impl = impl_from_async_flush_op(This);
    if (!results) return E_POINTER;
    *results = impl->result;
    return S_OK;
}
static const void *async_flush_op_vtbl[] = {
    (void *)async_flush_QI,
    (void *)async_flush_AddRef,
    (void *)async_flush_Release,
    (void *)async_flush_GetIids,
    (void *)async_flush_GetRuntimeClassName,
    (void *)async_flush_GetTrustLevel,
    (void *)async_flush_put_Completed,
    (void *)async_flush_get_Completed,
    (void *)async_flush_GetResults
};

static HRESULT WINAPI async_flush_info_QI(void *This, REFIID riid, void **ppv)
{
    struct socket_async_op_flush_bool *impl = impl_from_async_flush_info(This);
    return async_flush_QI(&impl->IAsyncOperation_iface, riid, ppv);
}
static ULONG WINAPI async_flush_info_AddRef(void *This)
{
    struct socket_async_op_flush_bool *impl = impl_from_async_flush_info(This);
    return async_flush_AddRef(&impl->IAsyncOperation_iface);
}
static ULONG WINAPI async_flush_info_Release(void *This)
{
    struct socket_async_op_flush_bool *impl = impl_from_async_flush_info(This);
    return async_flush_Release(&impl->IAsyncOperation_iface);
}
static HRESULT WINAPI async_flush_info_get_Id(void *This, UINT32 *id) { if (id) *id = 1; return S_OK; }
static HRESULT WINAPI async_flush_info_get_Status(void *This, AsyncStatus *status)
{
    struct socket_async_op_flush_bool *impl = impl_from_async_flush_info(This);
    if (!status) return E_POINTER;
    *status = impl->status;
    return S_OK;
}
static HRESULT WINAPI async_flush_info_get_ErrorCode(void *This, HRESULT *ec)
{
    struct socket_async_op_flush_bool *impl = impl_from_async_flush_info(This);
    if (!ec) return E_POINTER;
    *ec = impl->error_code;
    return S_OK;
}
static HRESULT WINAPI async_flush_info_Cancel(void *This) { return S_OK; }
static HRESULT WINAPI async_flush_info_Close(void *This) { return S_OK; }

static const void *async_flush_info_vtbl[] = {
    (void *)async_flush_info_QI,
    (void *)async_flush_info_AddRef,
    (void *)async_flush_info_Release,
    (void *)async_flush_GetIids,
    (void *)async_flush_GetRuntimeClassName,
    (void *)async_flush_GetTrustLevel,
    (void *)async_flush_info_get_Id,
    (void *)async_flush_info_get_Status,
    (void *)async_flush_info_get_ErrorCode,
    (void *)async_flush_info_Cancel,
    (void *)async_flush_info_Close
};

static HRESULT create_async_op_flush_bool(boolean result, void **out)
{
    struct socket_async_op_flush_bool *impl = calloc(1, sizeof(*impl));
    if (!impl) return E_OUTOFMEMORY;
    impl->IAsyncOperation_iface.lpVtbl = async_flush_op_vtbl;
    impl->IAsyncInfo_iface.lpVtbl = async_flush_info_vtbl;
    impl->ref = 1;
    impl->status = Completed;
    impl->error_code = S_OK;
    impl->result = result;
    *out = &impl->IAsyncOperation_iface;
    return S_OK;
}

/* HTTP response synthesis helper */
static void stream_socket_synthesize_response(struct stream_socket_impl *impl)
{
    if (impl->resp_len > 0) return;

    MESSAGE("windows.networking: synthesize_response for host [%s], request: %s\n", impl->remote_host, impl->req_buf);

    if (strstr(impl->remote_host, "solitaireevents.microsoftcasualgames.com") ||
             strstr(impl->remote_host, "microsoftcasualgames.com"))
    {
        static const char events_ok[] =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json; charset=utf-8\r\n"
            "Content-Length: 0\r\n"
            "Connection: close\r\n"
            "\r\n";
        memcpy(impl->resp_buf, events_ok, sizeof(events_ok) - 1);
        impl->resp_len = sizeof(events_ok) - 1;
        impl->resp_pos = 0;
        MESSAGE("windows.networking: Serving 200 OK (0 bytes) to telemetry host [%s]\n", impl->remote_host);
    }
    else if (strstr(impl->req_buf, "contentmanifest.json") ||
             strstr(impl->req_buf, "fwlink") ||
             strstr(impl->req_buf, "2378018") ||
             strstr(impl->remote_host, "go.microsoft.com") ||
             (strstr(impl->remote_host, "msgamestudios.com") && strstr(impl->req_buf, "contentmanifest")))
    {
        const unsigned char *json_payload = solitaire_manifest_yffi + solitaire_manifest_json_offset;
        unsigned int json_len = solitaire_manifest_json_len;
        int hlen = snprintf(impl->resp_buf, sizeof(impl->resp_buf),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json; charset=utf-8\r\n"
            "ETag: \"776c5c7cd1736290b28f562771bc2d80:1789661554.828492\"\r\n"
            "Server: AkamaiNetStorage\r\n"
            "Content-Length: %u\r\n"
            "Connection: close\r\n"
            "\r\n", json_len);
        if (hlen > 0 && (size_t)hlen + json_len < sizeof(impl->resp_buf))
        {
            memcpy(impl->resp_buf + hlen, json_payload, json_len);
            impl->resp_len = hlen + json_len;
            impl->resp_pos = 0;
            MESSAGE("windows.networking: Serving 200 OK with official JSON contentmanifest to [%s] (%u total bytes)\n",
                    impl->remote_host, impl->resp_len);
        }
    }
    else if (strstr(impl->req_buf, "manifest_4_27_0") || strstr(impl->req_buf, "versioned"))
    {
        const unsigned char *payload = solitaire_versioned_4_27_0_data;
        unsigned int payload_len = solitaire_versioned_4_27_0_len;
        int hlen = snprintf(impl->resp_buf, sizeof(impl->resp_buf),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json; charset=utf-8\r\n"
            "ETag: \"9f0243f0fba72ee7ca0388a67f50b280:1789661557.491549\"\r\n"
            "Content-Length: %u\r\n"
            "Connection: close\r\n"
            "\r\n", payload_len);
        if (hlen > 0 && (size_t)hlen + payload_len < sizeof(impl->resp_buf))
        {
            memcpy(impl->resp_buf + hlen, payload, payload_len);
            impl->resp_len = hlen + payload_len;
            impl->resp_pos = 0;
            MESSAGE("windows.networking: Serving versioned manifest 200 OK to [%s] (%u total bytes)\n",
                    impl->remote_host, impl->resp_len);
        }
    }
    else if (strstr(impl->req_buf, "cardbacks") || strstr(impl->req_buf, "06cf768377e5d4aebad5a069bfde8c09d4dd44f5"))
    {
        const unsigned char *payload = solitaire_cardbacks_data;
        unsigned int payload_len = solitaire_cardbacks_len;
        int hlen = snprintf(impl->resp_buf, sizeof(impl->resp_buf),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json; charset=utf-8\r\n"
            "ETag: \"19a2d80099c878f61cc43c2f50768649:1749252656.65227\"\r\n"
            "Content-Length: %u\r\n"
            "Connection: close\r\n"
            "\r\n", payload_len);
        if (hlen > 0 && (size_t)hlen + payload_len < sizeof(impl->resp_buf))
        {
            memcpy(impl->resp_buf + hlen, payload, payload_len);
            impl->resp_len = hlen + payload_len;
            impl->resp_pos = 0;
            MESSAGE("windows.networking: Serving cardbacks archive manifest (%u bytes)\n", impl->resp_len);
        }
    }
    else if (strstr(impl->req_buf, "cardbundles") || strstr(impl->req_buf, "046c0c9c03fc8f7b49becdcc23f4aa3d19b6725d"))
    {
        const unsigned char *payload = solitaire_cardbundles_data;
        unsigned int payload_len = solitaire_cardbundles_len;
        int hlen = snprintf(impl->resp_buf, sizeof(impl->resp_buf),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json; charset=utf-8\r\n"
            "ETag: \"6c83043900478c518dabed9d87b4ff2d:1687377437.647538\"\r\n"
            "Content-Length: %u\r\n"
            "Connection: close\r\n"
            "\r\n", payload_len);
        if (hlen > 0 && (size_t)hlen + payload_len < sizeof(impl->resp_buf))
        {
            memcpy(impl->resp_buf + hlen, payload, payload_len);
            impl->resp_len = hlen + payload_len;
            impl->resp_pos = 0;
            MESSAGE("windows.networking: Serving cardbundles archive manifest (%u bytes)\n", impl->resp_len);
        }
    }
    else if (strstr(impl->req_buf, "htp") || strstr(impl->req_buf, "b35f599964fdcf7070575e4fe1a466527ef6bc34"))
    {
        const unsigned char *payload = solitaire_htp_data;
        unsigned int payload_len = solitaire_htp_len;
        int hlen = snprintf(impl->resp_buf, sizeof(impl->resp_buf),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json; charset=utf-8\r\n"
            "ETag: \"0aede919904f01d214bdf894bda34368:1748643211.504534\"\r\n"
            "Content-Length: %u\r\n"
            "Connection: close\r\n"
            "\r\n", payload_len);
        if (hlen > 0 && (size_t)hlen + payload_len < sizeof(impl->resp_buf))
        {
            memcpy(impl->resp_buf + hlen, payload, payload_len);
            impl->resp_len = hlen + payload_len;
            impl->resp_pos = 0;
            MESSAGE("windows.networking: Serving htp archive manifest (%u bytes)\n", impl->resp_len);
        }
    }
    else if (strstr(impl->req_buf, "starclub") || strstr(impl->req_buf, "26d39e507cac8bebf57eb2dbe7e08cc169d4e420"))
    {
        const unsigned char *payload = solitaire_starclub_data;
        unsigned int payload_len = solitaire_starclub_len;
        int hlen = snprintf(impl->resp_buf, sizeof(impl->resp_buf),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json; charset=utf-8\r\n"
            "ETag: \"1fbb17d035feac529162888c60fd83dc:1649892693.161427\"\r\n"
            "Content-Length: %u\r\n"
            "Connection: close\r\n"
            "\r\n", payload_len);
        if (hlen > 0 && (size_t)hlen + payload_len < sizeof(impl->resp_buf))
        {
            memcpy(impl->resp_buf + hlen, payload, payload_len);
            impl->resp_len = hlen + payload_len;
            impl->resp_pos = 0;
            MESSAGE("windows.networking: Serving starclub archive manifest (%u bytes)\n", impl->resp_len);
        }
    }
    else if (strstr(impl->req_buf, "storefront") || strstr(impl->req_buf, "04c142852f6752531ec52bcedcee1b200beb40fc"))
    {
        const unsigned char *payload = solitaire_storefront_data;
        unsigned int payload_len = solitaire_storefront_len;
        int hlen = snprintf(impl->resp_buf, sizeof(impl->resp_buf),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json; charset=utf-8\r\n"
            "ETag: \"dc79219e61c71bd525d8f8a241e65bc6:1749252618.355252\"\r\n"
            "Content-Length: %u\r\n"
            "Connection: close\r\n"
            "\r\n", payload_len);
        if (hlen > 0 && (size_t)hlen + payload_len < sizeof(impl->resp_buf))
        {
            memcpy(impl->resp_buf + hlen, payload, payload_len);
            impl->resp_len = hlen + payload_len;
            impl->resp_pos = 0;
            MESSAGE("windows.networking: Serving storefront archive manifest (%u bytes)\n", impl->resp_len);
        }
    }
    else if (strstr(impl->req_buf, "themes") || strstr(impl->req_buf, "0cc191cd99068a0efa6a39dd3d430b2492cfd0f1"))
    {
        const unsigned char *payload = solitaire_themes_data;
        unsigned int payload_len = solitaire_themes_len;
        int hlen = snprintf(impl->resp_buf, sizeof(impl->resp_buf),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json; charset=utf-8\r\n"
            "ETag: \"43589ad88e6b2a44e3990a8179ce4fe5:1789661555.440097\"\r\n"
            "Content-Length: %u\r\n"
            "Connection: close\r\n"
            "\r\n", payload_len);
        if (hlen > 0 && (size_t)hlen + payload_len < sizeof(impl->resp_buf))
        {
            memcpy(impl->resp_buf + hlen, payload, payload_len);
            impl->resp_len = hlen + payload_len;
            impl->resp_pos = 0;
            MESSAGE("windows.networking: Serving themes archive manifest (%u bytes)\n", impl->resp_len);
        }
    }
    else if (strstr(impl->req_buf, "solvableseedpacks") || strstr(impl->req_buf, "1a82242d90effa4009e2cae3c1f8353cea358453"))
    {
        const unsigned char *payload = solitaire_solvableseedpacks_data;
        unsigned int payload_len = solitaire_solvableseedpacks_len;
        int hlen = snprintf(impl->resp_buf, sizeof(impl->resp_buf),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json; charset=utf-8\r\n"
            "ETag: \"solvableseedpacks_etag\"\r\n"
            "Content-Length: %u\r\n"
            "Connection: close\r\n"
            "\r\n", payload_len);
        if (hlen > 0 && (size_t)hlen + payload_len < sizeof(impl->resp_buf))
        {
            memcpy(impl->resp_buf + hlen, payload, payload_len);
            impl->resp_len = hlen + payload_len;
            impl->resp_pos = 0;
            MESSAGE("windows.networking: Serving solvableseedpacks archive manifest (%u bytes)\n", impl->resp_len);
        }
    }
    else if (strstr(impl->req_buf, "xpconfigs") || strstr(impl->req_buf, "ac6a624763470086fd8d397c325b6a2c0bcfbe47"))
    {
        const unsigned char *payload = solitaire_xpconfigs_data;
        unsigned int payload_len = solitaire_xpconfigs_len;
        int hlen = snprintf(impl->resp_buf, sizeof(impl->resp_buf),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json; charset=utf-8\r\n"
            "ETag: \"xpconfigs_etag\"\r\n"
            "Content-Length: %u\r\n"
            "Connection: close\r\n"
            "\r\n", payload_len);
        if (hlen > 0 && (size_t)hlen + payload_len < sizeof(impl->resp_buf))
        {
            memcpy(impl->resp_buf + hlen, payload, payload_len);
            impl->resp_len = hlen + payload_len;
            impl->resp_pos = 0;
            MESSAGE("windows.networking: Serving xpconfigs archive manifest (%u bytes)\n", impl->resp_len);
        }
    }
    else if (strstr(impl->req_buf, "archive_manifest"))
    {
        static const char archive_resp[] =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json; charset=utf-8\r\n"
            "ETag: \"generic_archive\"\r\n"
            "Content-Length: 18\r\n"
            "Connection: close\r\n"
            "\r\n"
            "{\"categories\": []}";
        memcpy(impl->resp_buf, archive_resp, sizeof(archive_resp) - 1);
        impl->resp_len = sizeof(archive_resp) - 1;
        impl->resp_pos = 0;
        MESSAGE("windows.networking: Serving generic archive_manifest 200 OK (%u bytes)\n", impl->resp_len);
    }
    else
    {
        static const char default_ok[] =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json; charset=utf-8\r\n"
            "ETag: \"default_etag\"\r\n"
            "Content-Length: 2\r\n"
            "Connection: close\r\n"
            "\r\n"
            "{}";
        memcpy(impl->resp_buf, default_ok, sizeof(default_ok) - 1);
        impl->resp_len = sizeof(default_ok) - 1;
        impl->resp_pos = 0;
        MESSAGE("windows.networking: Serving default 200 OK (%u bytes)\n", impl->resp_len);
    }
}

/* StreamSocket IInputStream implementation */
static HRESULT WINAPI stream_input_QI(void *This, REFIID riid, void **ppv)
{
    struct stream_socket_impl *impl = impl_from_stream_input(This);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IInputStream))
    {
        *ppv = &impl->input_stream.IInputStream_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IClosable))
    {
        *ppv = &impl->input_stream.IClosable_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI stream_input_AddRef(void *This)
{
    struct stream_socket_impl *impl = impl_from_stream_input(This);
    return InterlockedIncrement(&impl->ref);
}
static ULONG WINAPI stream_input_Release(void *This)
{
    struct stream_socket_impl *impl = impl_from_stream_input(This);
    return stream_sock_Release(&impl->IStreamSocket_iface);
}
static HRESULT WINAPI stream_input_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI stream_input_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.Storage.Streams.IInputStream", 37, n); }
static HRESULT WINAPI stream_input_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static void complete_pending_read(struct stream_socket_impl *impl)
{
    struct socket_async_op_read_buffer *read_op = NULL;
    UINT32 available, to_read;
    void **vtbl;
    HRESULT (WINAPI *invoke_fn)(void *, void *, AsyncStatus);

    EnterCriticalSection(&impl->cs);
    read_op = impl->pending_read;
    if (!read_op)
    {
        LeaveCriticalSection(&impl->cs);
        return;
    }
    available = (impl->resp_len > impl->resp_pos) ? (impl->resp_len - impl->resp_pos) : 0;
    if (available == 0)
    {
        if (impl->resp_len > 0)
        {
            impl->pending_read = NULL;
            if (read_op->buffer)
            {
                void **buf_vtbl = *(void ***)read_op->buffer;
                HRESULT (WINAPI *put_len_fn)(void *, UINT32) = (void *)buf_vtbl[8];
                put_len_fn(read_op->buffer, 0);
            }
            read_op->bytes_read = 0;
            read_op->status = Completed;
            MESSAGE("windows.networking: Fulfilling pending ReadAsync with EOF (0 bytes, pos %u/%u)\n",
                    impl->resp_pos, impl->resp_len);

            IUnknown_AddRef((IUnknown *)&read_op->IAsyncOperationWithProgress_iface);
            LeaveCriticalSection(&impl->cs);

            if (read_op->completed_handler)
            {
                vtbl = *(void ***)read_op->completed_handler;
                invoke_fn = (void *)vtbl[3];
                invoke_fn(read_op->completed_handler, &read_op->IAsyncOperationWithProgress_iface, Completed);
            }
            IUnknown_Release((IUnknown *)&read_op->IAsyncOperationWithProgress_iface);
            IUnknown_Release((IUnknown *)&read_op->IAsyncOperationWithProgress_iface);
            return;
        }
        LeaveCriticalSection(&impl->cs);
        return;
    }

    impl->pending_read = NULL;
    to_read = min(read_op->requested_count, available);

    if (read_op->buffer)
    {
        void **buf_vtbl = *(void ***)read_op->buffer;
        void *byte_access = NULL;
        HRESULT (WINAPI *qi_fn)(void *, REFIID, void **) = (void *)buf_vtbl[0];
        HRESULT (WINAPI *put_len_fn)(void *, UINT32) = (void *)buf_vtbl[8];

        if (SUCCEEDED(qi_fn(read_op->buffer, &IID_IBufferByteAccess, &byte_access)))
        {
            BYTE *raw_data = NULL;
            void **ba_vtbl = *(void ***)byte_access;
            HRESULT (WINAPI *get_buf_fn)(void *, BYTE **) = (void *)ba_vtbl[3];
            ULONG (WINAPI *rel_fn)(void *) = (void *)ba_vtbl[2];
            if (SUCCEEDED(get_buf_fn(byte_access, &raw_data)) && raw_data)
            {
                memcpy(raw_data, impl->resp_buf + impl->resp_pos, to_read);
                impl->resp_pos += to_read;
            }
            rel_fn(byte_access);
        }
        put_len_fn(read_op->buffer, to_read);
    }
    read_op->bytes_read = to_read;
    read_op->status = Completed;
    MESSAGE("windows.networking: Fulfilling pending ReadAsync with %u bytes (pos %u/%u)\n",
            to_read, impl->resp_pos, impl->resp_len);

    /* Keep a reference while calling handler outside critical section */
    IUnknown_AddRef((IUnknown *)&read_op->IAsyncOperationWithProgress_iface);
    LeaveCriticalSection(&impl->cs);

    if (read_op->completed_handler)
    {
        vtbl = *(void ***)read_op->completed_handler;
        invoke_fn = (void *)vtbl[3];
        invoke_fn(read_op->completed_handler, &read_op->IAsyncOperationWithProgress_iface, Completed);
    }
    IUnknown_Release((IUnknown *)&read_op->IAsyncOperationWithProgress_iface);
    IUnknown_Release((IUnknown *)&read_op->IAsyncOperationWithProgress_iface);
}

static DWORD WINAPI complete_pending_read_worker(void *param)
{
    struct stream_socket_impl *impl = (struct stream_socket_impl *)param;
    complete_pending_read(impl);
    stream_sock_Release(&impl->IStreamSocket_iface);
    return 0;
}

static void schedule_complete_pending_read(struct stream_socket_impl *impl)
{
    stream_sock_AddRef(&impl->IStreamSocket_iface);
    if (!QueueUserWorkItem(complete_pending_read_worker, impl, WT_EXECUTEDEFAULT))
    {
        WARN("QueueUserWorkItem failed, falling back to synchronous completion\n");
        complete_pending_read(impl);
        stream_sock_Release(&impl->IStreamSocket_iface);
    }
}

static HRESULT WINAPI stream_input_ReadAsync(void *This, void *buffer, UINT32 count, int options, void **operation)
{
    struct stream_socket_impl *impl = impl_from_stream_input(This);
    UINT32 available, to_read;

    if (!operation) return E_POINTER;
    *operation = NULL;

    EnterCriticalSection(&impl->cs);
    available = (impl->resp_len > impl->resp_pos) ? (impl->resp_len - impl->resp_pos) : 0;
    if (available > 0)
    {
        void **buf_vtbl = *(void ***)buffer;
        void *byte_access = NULL;
        HRESULT (WINAPI *qi_fn)(void *, REFIID, void **) = (void *)buf_vtbl[0];
        HRESULT (WINAPI *put_len_fn)(void *, UINT32) = (void *)buf_vtbl[8];

        to_read = min(count, available);
        if (SUCCEEDED(qi_fn(buffer, &IID_IBufferByteAccess, &byte_access)))
        {
            BYTE *raw_data = NULL;
            void **ba_vtbl = *(void ***)byte_access;
            HRESULT (WINAPI *get_buf_fn)(void *, BYTE **) = (void *)ba_vtbl[3];
            ULONG (WINAPI *rel_fn)(void *) = (void *)ba_vtbl[2];
            if (SUCCEEDED(get_buf_fn(byte_access, &raw_data)) && raw_data)
            {
                memcpy(raw_data, impl->resp_buf + impl->resp_pos, to_read);
                impl->resp_pos += to_read;
            }
            rel_fn(byte_access);
        }
        put_len_fn(buffer, to_read);
        MESSAGE("windows.networking: ReadAsync immediate read %u bytes (pos %u/%u)\n",
                to_read, impl->resp_pos, impl->resp_len);
        LeaveCriticalSection(&impl->cs);
        return create_async_op_read_buffer(buffer, to_read, operation);
    }
    else
    {
        /* No data ready yet; wait pending without returning premature EOF */
        struct socket_async_op_read_buffer *op_impl = NULL;
        HRESULT hr;

        MESSAGE("windows.networking: ReadAsync requested %u bytes, waiting pending (resp_len %u, resp_pos %u)\n",
                count, impl->resp_len, impl->resp_pos);
        hr = create_async_op_read_buffer_pending(buffer, count, impl, &op_impl);
        if (FAILED(hr))
        {
            LeaveCriticalSection(&impl->cs);
            return hr;
        }

        if (impl->pending_read)
        {
            IUnknown_Release((IUnknown *)&impl->pending_read->IAsyncOperationWithProgress_iface);
        }
        impl->pending_read = op_impl;
        IUnknown_AddRef((IUnknown *)&op_impl->IAsyncOperationWithProgress_iface);

        *operation = &op_impl->IAsyncOperationWithProgress_iface;
        LeaveCriticalSection(&impl->cs);
        return S_OK;
    }
}

static const void *stream_input_vtbl[] = {
    (void *)stream_input_QI,
    (void *)stream_input_AddRef,
    (void *)stream_input_Release,
    (void *)stream_input_GetIids,
    (void *)stream_input_GetRuntimeClassName,
    (void *)stream_input_GetTrustLevel,
    (void *)stream_input_ReadAsync
};

static HRESULT WINAPI stream_input_closable_QI(void *This, REFIID riid, void **ppv)
{
    struct stream_socket_impl *impl = impl_from_stream_input_closable(This);
    return stream_input_QI(&impl->input_stream.IInputStream_iface, riid, ppv);
}
static ULONG WINAPI stream_input_closable_AddRef(void *This)
{
    struct stream_socket_impl *impl = impl_from_stream_input_closable(This);
    return InterlockedIncrement(&impl->ref);
}
static ULONG WINAPI stream_input_closable_Release(void *This)
{
    struct stream_socket_impl *impl = impl_from_stream_input_closable(This);
    return stream_sock_Release(&impl->IStreamSocket_iface);
}
static HRESULT WINAPI stream_input_Close(void *This) { return S_OK; }

static const void *stream_input_closable_vtbl[] = {
    (void *)stream_input_closable_QI,
    (void *)stream_input_closable_AddRef,
    (void *)stream_input_closable_Release,
    (void *)stream_input_GetIids,
    (void *)stream_input_GetRuntimeClassName,
    (void *)stream_input_GetTrustLevel,
    (void *)stream_input_Close
};

/* StreamSocket IOutputStream implementation */
static HRESULT WINAPI stream_output_QI(void *This, REFIID riid, void **ppv)
{
    struct stream_socket_impl *impl = impl_from_stream_output(This);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IOutputStream))
    {
        *ppv = &impl->output_stream.IOutputStream_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IClosable))
    {
        *ppv = &impl->output_stream.IClosable_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI stream_output_AddRef(void *This)
{
    struct stream_socket_impl *impl = impl_from_stream_output(This);
    return InterlockedIncrement(&impl->ref);
}
static ULONG WINAPI stream_output_Release(void *This)
{
    struct stream_socket_impl *impl = impl_from_stream_output(This);
    return stream_sock_Release(&impl->IStreamSocket_iface);
}
static HRESULT WINAPI stream_output_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI stream_output_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.Storage.Streams.IOutputStream", 38, n); }
static HRESULT WINAPI stream_output_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI stream_output_WriteAsync(void *This, void *buffer, void **operation)
{
    struct stream_socket_impl *impl = impl_from_stream_output(This);
    UINT32 len = 0;
    void **buf_vtbl = *(void ***)buffer;
    HRESULT (WINAPI *get_len_fn)(void *, UINT32 *) = (void *)buf_vtbl[7];
    void *byte_access = NULL;
    HRESULT (WINAPI *qi_fn)(void *, REFIID, void **) = (void *)buf_vtbl[0];
    BOOL has_full_req = FALSE;

    EnterCriticalSection(&impl->cs);
    get_len_fn(buffer, &len);
    if (SUCCEEDED(qi_fn(buffer, &IID_IBufferByteAccess, &byte_access)))
    {
        BYTE *raw_data = NULL;
        void **ba_vtbl = *(void ***)byte_access;
        HRESULT (WINAPI *get_buf_fn)(void *, BYTE **) = (void *)ba_vtbl[3];
        ULONG (WINAPI *rel_fn)(void *) = (void *)ba_vtbl[2];
        if (SUCCEEDED(get_buf_fn(byte_access, &raw_data)) && raw_data && len > 0)
        {
            UINT32 copy_len;
            if (impl->resp_len > 0 && impl->resp_pos >= impl->resp_len)
            {
                impl->req_len = 0;
                impl->req_buf[0] = '\0';
                impl->resp_len = 0;
                impl->resp_pos = 0;
            }
            copy_len = min(len, (UINT32)(sizeof(impl->req_buf) - 1 - impl->req_len));
            memcpy(impl->req_buf + impl->req_len, raw_data, copy_len);
            impl->req_len += copy_len;
            impl->req_buf[impl->req_len] = '\0';
            MESSAGE("windows.networking: WriteAsync received %u bytes: %.*s\n", copy_len, (int)min(copy_len, 200), (char *)raw_data);
        }
        rel_fn(byte_access);
    }
    if (strstr(impl->req_buf, "\r\n\r\n"))
    {
        stream_socket_synthesize_response(impl);
        has_full_req = TRUE;
    }
    LeaveCriticalSection(&impl->cs);

    if (has_full_req)
    {
        schedule_complete_pending_read(impl);
    }
    return create_async_op_write_uint32(len, operation);
}

static HRESULT WINAPI stream_output_FlushAsync(void *This, void **operation)
{
    struct stream_socket_impl *impl = impl_from_stream_output(This);
    MESSAGE("windows.networking: FlushAsync called\n");
    EnterCriticalSection(&impl->cs);
    if (impl->resp_len == 0)
    {
        stream_socket_synthesize_response(impl);
    }
    LeaveCriticalSection(&impl->cs);
    schedule_complete_pending_read(impl);
    return create_async_op_flush_bool(TRUE, operation);
}

static const void *stream_output_vtbl[] = {
    (void *)stream_output_QI,
    (void *)stream_output_AddRef,
    (void *)stream_output_Release,
    (void *)stream_output_GetIids,
    (void *)stream_output_GetRuntimeClassName,
    (void *)stream_output_GetTrustLevel,
    (void *)stream_output_WriteAsync,
    (void *)stream_output_FlushAsync
};

static HRESULT WINAPI stream_output_closable_QI(void *This, REFIID riid, void **ppv)
{
    struct stream_socket_impl *impl = impl_from_stream_output_closable(This);
    return stream_output_QI(&impl->output_stream.IOutputStream_iface, riid, ppv);
}
static ULONG WINAPI stream_output_closable_AddRef(void *This)
{
    struct stream_socket_impl *impl = impl_from_stream_output_closable(This);
    return InterlockedIncrement(&impl->ref);
}
static ULONG WINAPI stream_output_closable_Release(void *This)
{
    struct stream_socket_impl *impl = impl_from_stream_output_closable(This);
    return stream_sock_Release(&impl->IStreamSocket_iface);
}
static HRESULT WINAPI stream_output_Close(void *This) { return S_OK; }

static const void *stream_output_closable_vtbl[] = {
    (void *)stream_output_closable_QI,
    (void *)stream_output_closable_AddRef,
    (void *)stream_output_closable_Release,
    (void *)stream_output_GetIids,
    (void *)stream_output_GetRuntimeClassName,
    (void *)stream_output_GetTrustLevel,
    (void *)stream_output_Close
};

static HRESULT WINAPI stream_sock_get_InputStream(void *This, void **value)
{
    struct stream_socket_impl *impl = impl_from_stream_socket(This);
    if (!value) return E_POINTER;
    *value = &impl->input_stream.IInputStream_iface;
    stream_sock_AddRef(&impl->IStreamSocket_iface);
    MESSAGE("windows.networking: get_InputStream -> %p\n", *value);
    return S_OK;
}
static HRESULT WINAPI stream_sock_get_OutputStream(void *This, void **value)
{
    struct stream_socket_impl *impl = impl_from_stream_socket(This);
    if (!value) return E_POINTER;
    *value = &impl->output_stream.IOutputStream_iface;
    stream_sock_AddRef(&impl->IStreamSocket_iface);
    MESSAGE("windows.networking: get_OutputStream -> %p\n", *value);
    return S_OK;
}

static void extract_endpoint_pair_host(void *endpoint_pair, char *out, size_t out_len)
{
    struct endpoint_pair_impl *ep = endpoint_pair;
    void **vtbl;
    HRESULT (WINAPI *get_name_fn)(void *, HSTRING *);
    HSTRING name = NULL;

    out[0] = '\0';
    if (!ep || !ep->remote_host) return;

    vtbl = *(void ***)ep->remote_host;
    get_name_fn = (void *)vtbl[7];
    if (SUCCEEDED(get_name_fn(ep->remote_host, &name)) && name)
    {
        UINT32 len = 0;
        const WCHAR *wstr = WindowsGetStringRawBuffer(name, &len);
        if (wstr && len > 0)
        {
            WideCharToMultiByte(CP_UTF8, 0, wstr, len, out, out_len - 1, NULL, NULL);
            out[min(len, out_len - 1)] = '\0';
        }
        WindowsDeleteString(name);
    }
}

static void extract_hostname(void *remote_host, char *out, size_t out_len)
{
    void **vtbl;
    HRESULT (WINAPI *get_name_fn)(void *, HSTRING *);
    HSTRING name = NULL;

    out[0] = '\0';
    if (!remote_host) return;

    vtbl = *(void ***)remote_host;
    get_name_fn = (void *)vtbl[7];
    if (SUCCEEDED(get_name_fn(remote_host, &name)) && name)
    {
        UINT32 len = 0;
        const WCHAR *wstr = WindowsGetStringRawBuffer(name, &len);
        if (wstr && len > 0)
        {
            WideCharToMultiByte(CP_UTF8, 0, wstr, len, out, out_len - 1, NULL, NULL);
            out[min(len, out_len - 1)] = '\0';
        }
        WindowsDeleteString(name);
    }
}

static HRESULT WINAPI stream_sock_ConnectWithEndpointPairAsync(void *This, void *endpoint_pair, void **operation)
{
    struct stream_socket_impl *impl = impl_from_stream_socket(This);
    extract_endpoint_pair_host(endpoint_pair, impl->remote_host, sizeof(impl->remote_host));
    MESSAGE("windows.networking: ConnectWithEndpointPairAsync this %p, ep %p, remote_host [%s] -> SUCCESS\n",
            This, endpoint_pair, impl->remote_host);
    return create_socket_async_action(S_OK, operation);
}

static HRESULT WINAPI stream_sock_ConnectAsync(void *This, void *remote_host, HSTRING remote_service, void **operation)
{
    struct stream_socket_impl *impl = impl_from_stream_socket(This);
    extract_hostname(remote_host, impl->remote_host, sizeof(impl->remote_host));
    MESSAGE("windows.networking: ConnectAsync this %p, host %p, remote_host [%s], service %s -> SUCCESS\n",
            This, remote_host, impl->remote_host, debugstr_hstring(remote_service));
    return create_socket_async_action(S_OK, operation);
}

static HRESULT WINAPI stream_sock_ConnectWithEndpointPairAndProtectionLevelAsync(void *This, void *endpoint_pair, int level, void **operation)
{
    struct stream_socket_impl *impl = impl_from_stream_socket(This);
    extract_endpoint_pair_host(endpoint_pair, impl->remote_host, sizeof(impl->remote_host));
    MESSAGE("windows.networking: ConnectWithEndpointPairAndProtectionLevelAsync this %p, ep %p, remote_host [%s], level %d -> SUCCESS\n",
            This, endpoint_pair, impl->remote_host, level);
    return create_socket_async_action(S_OK, operation);
}

static HRESULT WINAPI stream_sock_ConnectWithProtectionLevelAsync(void *This, void *remote_host, HSTRING remote_service, int level, void **operation)
{
    struct stream_socket_impl *impl = impl_from_stream_socket(This);
    extract_hostname(remote_host, impl->remote_host, sizeof(impl->remote_host));
    MESSAGE("windows.networking: ConnectWithProtectionLevelAsync this %p, host %p, remote_host [%s], service %s, level %d -> SUCCESS\n",
            This, remote_host, impl->remote_host, debugstr_hstring(remote_service), level);
    return create_socket_async_action(S_OK, operation);
}

static HRESULT WINAPI stream_sock_UpgradeToSslAsync(void *This, int level, void *host, void **operation)
{
    MESSAGE("windows.networking: UpgradeToSslAsync this %p, level %d, host %p -> SUCCESS\n", This, level, host);
    return create_socket_async_action(S_OK, operation);
}

static const void *stream_socket_vtbl[] = {
    (void *)stream_sock_QI,
    (void *)stream_sock_AddRef,
    (void *)stream_sock_Release,
    (void *)stream_sock_GetIids,
    (void *)stream_sock_GetRuntimeClassName,
    (void *)stream_sock_GetTrustLevel,
    (void *)stream_sock_get_Control,
    (void *)stream_sock_get_Information,
    (void *)stream_sock_get_InputStream,
    (void *)stream_sock_get_OutputStream,
    (void *)stream_sock_ConnectWithEndpointPairAsync,
    (void *)stream_sock_ConnectAsync,
    (void *)stream_sock_ConnectWithEndpointPairAndProtectionLevelAsync,
    (void *)stream_sock_ConnectWithProtectionLevelAsync,
    (void *)stream_sock_UpgradeToSslAsync
};

static HRESULT WINAPI stream_sock_closable_QI(void *This, REFIID riid, void **ppv)
{
    struct stream_socket_impl *impl = impl_from_stream_socket_closable(This);
    return stream_sock_QI(&impl->IStreamSocket_iface, riid, ppv);
}
static ULONG WINAPI stream_sock_closable_AddRef(void *This)
{
    struct stream_socket_impl *impl = impl_from_stream_socket_closable(This);
    return InterlockedIncrement(&impl->ref);
}
static ULONG WINAPI stream_sock_closable_Release(void *This)
{
    struct stream_socket_impl *impl = impl_from_stream_socket_closable(This);
    return stream_sock_Release(&impl->IStreamSocket_iface);
}
static HRESULT WINAPI stream_sock_Close(void *This)
{
    struct stream_socket_impl *impl = impl_from_stream_socket_closable(This);
    TRACE("StreamSocket::Close\n");
    stream_socket_cleanup(impl);
    return S_OK;
}

static const void *stream_socket_closable_vtbl[] = {
    (void *)stream_sock_closable_QI,
    (void *)stream_sock_closable_AddRef,
    (void *)stream_sock_closable_Release,
    (void *)stream_sock_GetIids,
    (void *)stream_sock_GetRuntimeClassName,
    (void *)stream_sock_GetTrustLevel,
    (void *)stream_sock_Close
};

static HRESULT create_stream_socket(void **out)
{
    struct stream_socket_impl *impl = calloc(1, sizeof(*impl));
    if (!impl) return E_OUTOFMEMORY;
    impl->IStreamSocket_iface.lpVtbl = stream_socket_vtbl;
    impl->IClosable_iface.lpVtbl = stream_socket_closable_vtbl;
    impl->control.IStreamSocketControl_iface.lpVtbl = stream_socket_control_vtbl;
    impl->control.no_delay = TRUE;
    impl->information.IStreamSocketInformation_iface.lpVtbl = stream_socket_information_vtbl;
    impl->input_stream.IInputStream_iface.lpVtbl = stream_input_vtbl;
    impl->input_stream.IClosable_iface.lpVtbl = stream_input_closable_vtbl;
    impl->output_stream.IOutputStream_iface.lpVtbl = stream_output_vtbl;
    impl->output_stream.IClosable_iface.lpVtbl = stream_output_closable_vtbl;
    impl->ref = 1;
    InitializeCriticalSection(&impl->cs);
    *out = &impl->IStreamSocket_iface;
    return S_OK;
}

static HRESULT WINAPI stream_sock_fac_QI(void *This, REFIID riid, void **ppv)
{
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppv = This;
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI stream_sock_fac_AddRef(void *This) { return 2; }
static ULONG WINAPI stream_sock_fac_Release(void *This) { return 1; }
static HRESULT WINAPI stream_sock_fac_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI stream_sock_fac_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.Networking.Sockets.StreamSocket", 39, n); }
static HRESULT WINAPI stream_sock_fac_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI stream_sock_fac_ActivateInstance(void *This, IInspectable **instance)
{
    if (!instance) return E_POINTER;
    return create_stream_socket((void **)instance);
}

static const void *stream_socket_factory_vtbl[] = {
    (void *)stream_sock_fac_QI,
    (void *)stream_sock_fac_AddRef,
    (void *)stream_sock_fac_Release,
    (void *)stream_sock_fac_GetIids,
    (void *)stream_sock_fac_GetRuntimeClassName,
    (void *)stream_sock_fac_GetTrustLevel,
    (void *)stream_sock_fac_ActivateInstance
};

static struct { const void *lpVtbl; } stream_socket_factory = { stream_socket_factory_vtbl };

HRESULT WINAPI SetSocketMediaStreamingMode(BOOL value)
{
    FIXME("value %d stub!\n", value);
    return S_OK;
}

HRESULT WINAPI DllGetActivationFactory(HSTRING classid, void **factory)
{
    const WCHAR *name = WindowsGetStringRawBuffer(classid, NULL);

    TRACE("class %s, factory %p.\n", debugstr_hstring(classid), factory);

    if (!factory) return E_INVALIDARG;
    *factory = NULL;

    if (!wcscmp(name, L"Windows.Networking.PushNotifications.PushNotificationChannelManager"))
    {
        *factory = &push_notification_factory.IActivationFactory_iface;
        IActivationFactory_AddRef((IActivationFactory *)*factory);
        return S_OK;
    }
    if (!wcscmp(name, L"Windows.Networking.EndpointPair"))
    {
        *factory = &endpoint_pair_factory.IActivationFactory_iface;
        return S_OK;
    }
    if (!wcscmp(name, L"Windows.Networking.Sockets.StreamSocket"))
    {
        *factory = &stream_socket_factory;
        return S_OK;
    }

    FIXME("unsupported class %s\n", debugstr_w(name));
    return CLASS_E_CLASSNOTAVAILABLE;
}
