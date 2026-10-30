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
static const GUID IID_IStreamSocket = {0x69a22cf3, 0xfc7b, 0x4857, {0xaf, 0x38, 0xf6, 0xe7, 0xde, 0x6a, 0x5b, 0x49}};
static const GUID IID_IStreamSocketControl = {0xfe25adf1, 0x92ab, 0x4af3, {0x99, 0x92, 0x0f, 0x4c, 0x85, 0xe3, 0x6c, 0xc4}};
static const GUID IID_IStreamSocketInformation = {0x3b80ae30, 0x5e68, 0x4205, {0x88, 0xf0, 0xdc, 0x85, 0xd2, 0xe2, 0x5d, 0xed}};
static const GUID IID_IAsyncAction = {0x5a648006, 0x843a, 0x4da9, {0x86, 0x5b, 0x9d, 0x26, 0xe5, 0xdf, 0xad, 0x7b}};
static const GUID IID_IClosable = {0x30d5a829, 0x7fa4, 0x4026, {0x83, 0xbb, 0xd7, 0x5b, 0xae, 0x4e, 0xa9, 0x9e}};

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

struct stream_socket_impl
{
    struct { const void *lpVtbl; } IStreamSocket_iface;
    struct { const void *lpVtbl; } IClosable_iface;
    LONG ref;
    struct stream_socket_control_impl control;
    struct stream_socket_information_impl information;
};

#define impl_from_stream_socket(ptr) ((struct stream_socket_impl *)((char *)(ptr) - offsetof(struct stream_socket_impl, IStreamSocket_iface)))
#define impl_from_stream_socket_closable(ptr) ((struct stream_socket_impl *)((char *)(ptr) - offsetof(struct stream_socket_impl, IClosable_iface)))
#define impl_from_stream_sock_control(ptr) ((struct stream_socket_impl *)((char *)(ptr) - offsetof(struct stream_socket_impl, control.IStreamSocketControl_iface)))
#define impl_from_stream_sock_information(ptr) ((struct stream_socket_impl *)((char *)(ptr) - offsetof(struct stream_socket_impl, information.IStreamSocketInformation_iface)))

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
    if (!ref) free(impl);
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
static HRESULT WINAPI stream_sock_get_InputStream(void *This, void **value)
{
    if (value) *value = NULL;
    return S_OK;
}
static HRESULT WINAPI stream_sock_get_OutputStream(void *This, void **value)
{
    if (value) *value = NULL;
    return S_OK;
}

static HRESULT WINAPI stream_sock_ConnectWithEndpointPairAsync(void *This, void *endpoint_pair, void **operation)
{
    TRACE("ConnectWithEndpointPairAsync %p\n", endpoint_pair);
    return create_socket_async_action(0x8007274c /* WSAETIMEDOUT */, operation);
}

static HRESULT WINAPI stream_sock_ConnectAsync(void *This, void *remote_host, HSTRING remote_service, void **operation)
{
    TRACE("ConnectAsync remote_host %p, service %s\n", remote_host, debugstr_hstring(remote_service));
    return create_socket_async_action(0x8007274c /* WSAETIMEDOUT */, operation);
}

static HRESULT WINAPI stream_sock_ConnectWithEndpointPairAndProtectionLevelAsync(void *This, void *endpoint_pair, int level, void **operation)
{
    return create_socket_async_action(0x8007274c /* WSAETIMEDOUT */, operation);
}

static HRESULT WINAPI stream_sock_ConnectWithProtectionLevelAsync(void *This, void *remote_host, HSTRING remote_service, int level, void **operation)
{
    return create_socket_async_action(0x8007274c /* WSAETIMEDOUT */, operation);
}

static HRESULT WINAPI stream_sock_UpgradeToSslAsync(void *This, int level, void *host, void **operation)
{
    return create_socket_async_action(0x8007274c /* WSAETIMEDOUT */, operation);
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
    TRACE("StreamSocket::Close\n");
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
    impl->ref = 1;
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
