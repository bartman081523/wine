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

    FIXME("unsupported class %s\n", debugstr_w(name));
    return CLASS_E_CLASSNOTAVAILABLE;
}
