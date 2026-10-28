/*
 * WinRT Windows.ApplicationModel.Background.BackgroundExecutionManager Implementation
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
#include <asyncinfo.h>
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(model);

/* IBackgroundExecutionManagerStatics: GUID {e826ea58-66a9-4d41-83d4-b4c18c87b846} */
static const GUID IID_IBackgroundExecutionManagerStatics = {0xe826ea58, 0x66a9, 0x4d41, {0x83, 0xd4, 0xb4, 0xc1, 0x8c, 0x87, 0xb8, 0x46}};
static const GUID IID_IAsyncOperation_BackgroundAccessStatus = {0x7b44e581, 0xcfa9, 0x5763, {0xbe, 0xd7, 0x6a, 0x65, 0x73, 0x9f, 0x0d, 0xbf}};
static const GUID IID_IAsyncOperation_BackgroundAccessStatus_alt = {0xc5cdc96a, 0xc61d, 0x5cf1, {0xa9, 0x0a, 0x91, 0x67, 0x2e, 0x43, 0x88, 0xeb}};

#define RuntimeClass_Windows_ApplicationModel_Background_BackgroundExecutionManager \
    L"Windows.ApplicationModel.Background.BackgroundExecutionManager"

typedef enum BackgroundAccessStatus
{
    BackgroundAccessStatus_Unspecified = 0,
    BackgroundAccessStatus_AllowedWithAlwaysOnRealTimeConnectivity = 1,
    BackgroundAccessStatus_AllowedMayUseActiveRealTimeConnectivity = 2,
    BackgroundAccessStatus_Denied = 3,
    BackgroundAccessStatus_AlwaysAllowed = 4,
    BackgroundAccessStatus_AllowedSubjectToSystemPolicy = 5,
    BackgroundAccessStatus_DeniedBySystemPolicy = 6,
    BackgroundAccessStatus_DeniedByUser = 7
} BackgroundAccessStatus;

typedef struct IBackgroundExecutionManagerStatics IBackgroundExecutionManagerStatics;

typedef struct IBackgroundExecutionManagerStaticsVtbl
{
    /* IUnknown */
    HRESULT (WINAPI *QueryInterface)(IBackgroundExecutionManagerStatics *This, REFIID riid, void **ppvObject);
    ULONG   (WINAPI *AddRef)(IBackgroundExecutionManagerStatics *This);
    ULONG   (WINAPI *Release)(IBackgroundExecutionManagerStatics *This);
    /* IInspectable */
    HRESULT (WINAPI *GetIids)(IBackgroundExecutionManagerStatics *This, ULONG *iidCount, IID **iids);
    HRESULT (WINAPI *GetRuntimeClassName)(IBackgroundExecutionManagerStatics *This, HSTRING *className);
    HRESULT (WINAPI *GetTrustLevel)(IBackgroundExecutionManagerStatics *This, TrustLevel *trustLevel);
    /* IBackgroundExecutionManagerStatics */
    HRESULT (WINAPI *RequestAccessAsync)(IBackgroundExecutionManagerStatics *This, void **operation);
    HRESULT (WINAPI *RequestAccessForApplicationAsync)(IBackgroundExecutionManagerStatics *This, HSTRING applicationId, void **operation);
    HRESULT (WINAPI *RemoveAccess)(IBackgroundExecutionManagerStatics *This);
    HRESULT (WINAPI *RemoveAccessForApplication)(IBackgroundExecutionManagerStatics *This, HSTRING applicationId);
    HRESULT (WINAPI *GetAccessStatus)(IBackgroundExecutionManagerStatics *This, BackgroundAccessStatus *status);
    HRESULT (WINAPI *GetAccessStatusForApplication)(IBackgroundExecutionManagerStatics *This, HSTRING applicationId, BackgroundAccessStatus *status);
} IBackgroundExecutionManagerStaticsVtbl;

struct IBackgroundExecutionManagerStatics
{
    const IBackgroundExecutionManagerStaticsVtbl *lpVtbl;
};

/* Completed Handler vtbl */
typedef struct IAsyncOperationCompletedHandlerVtbl
{
    HRESULT (WINAPI *QueryInterface)(IUnknown *This, REFIID riid, void **ppvObject);
    ULONG   (WINAPI *AddRef)(IUnknown *This);
    ULONG   (WINAPI *Release)(IUnknown *This);
    HRESULT (WINAPI *Invoke)(IUnknown *This, IInspectable *asyncInfo, AsyncStatus asyncStatus);
} IAsyncOperationCompletedHandlerVtbl;

/* IAsyncOperation for BackgroundAccessStatus */
typedef struct IAsyncOperation_BackgroundAccessStatus IAsyncOperation_BackgroundAccessStatus;

typedef struct IAsyncOperation_BackgroundAccessStatusVtbl
{
    /* IUnknown */
    HRESULT (WINAPI *QueryInterface)(IAsyncOperation_BackgroundAccessStatus *This, REFIID riid, void **ppvObject);
    ULONG   (WINAPI *AddRef)(IAsyncOperation_BackgroundAccessStatus *This);
    ULONG   (WINAPI *Release)(IAsyncOperation_BackgroundAccessStatus *This);
    /* IInspectable */
    HRESULT (WINAPI *GetIids)(IAsyncOperation_BackgroundAccessStatus *This, ULONG *iidCount, IID **iids);
    HRESULT (WINAPI *GetRuntimeClassName)(IAsyncOperation_BackgroundAccessStatus *This, HSTRING *className);
    HRESULT (WINAPI *GetTrustLevel)(IAsyncOperation_BackgroundAccessStatus *This, TrustLevel *trustLevel);
    /* IAsyncOperation<BackgroundAccessStatus> */
    HRESULT (WINAPI *put_Completed)(IAsyncOperation_BackgroundAccessStatus *This, IUnknown *handler);
    HRESULT (WINAPI *get_Completed)(IAsyncOperation_BackgroundAccessStatus *This, IUnknown **handler);
    HRESULT (WINAPI *GetResults)(IAsyncOperation_BackgroundAccessStatus *This, BackgroundAccessStatus *results);
} IAsyncOperation_BackgroundAccessStatusVtbl;

struct IAsyncOperation_BackgroundAccessStatus
{
    const IAsyncOperation_BackgroundAccessStatusVtbl *lpVtbl;
};

struct async_operation
{
    IAsyncOperation_BackgroundAccessStatus IAsyncOperation_iface;
    IAsyncInfo IAsyncInfo_iface;
    LONG ref;
};

static inline struct async_operation *impl_from_IAsyncOperation(IAsyncOperation_BackgroundAccessStatus *iface)
{
    return CONTAINING_RECORD(iface, struct async_operation, IAsyncOperation_iface);
}

static inline struct async_operation *impl_from_IAsyncInfo(IAsyncInfo *iface)
{
    return CONTAINING_RECORD(iface, struct async_operation, IAsyncInfo_iface);
}

static HRESULT WINAPI async_QueryInterface(IAsyncOperation_BackgroundAccessStatus *iface, REFIID iid, void **out)
{
    struct async_operation *impl = impl_from_IAsyncOperation(iface);

    TRACE("iface %p, iid %s, out %p.\n", iface, debugstr_guid(iid), out);

    if (!out) return E_INVALIDARG;

    if (IsEqualIID(iid, &IID_IUnknown) ||
        IsEqualIID(iid, &IID_IInspectable) ||
        IsEqualIID(iid, &IID_IAgileObject) ||
        IsEqualIID(iid, &IID_IAsyncOperation_BackgroundAccessStatus) ||
        IsEqualIID(iid, &IID_IAsyncOperation_BackgroundAccessStatus_alt))
    {
        *out = &impl->IAsyncOperation_iface;
    }
    else if (IsEqualIID(iid, &IID_IAsyncInfo))
    {
        *out = &impl->IAsyncInfo_iface;
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

static ULONG WINAPI async_AddRef(IAsyncOperation_BackgroundAccessStatus *iface)
{
    struct async_operation *impl = impl_from_IAsyncOperation(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI async_Release(IAsyncOperation_BackgroundAccessStatus *iface)
{
    struct async_operation *impl = impl_from_IAsyncOperation(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI async_GetIids(IAsyncOperation_BackgroundAccessStatus *iface, ULONG *iid_count, IID **iids)
{
    IID *array;
    if (!iid_count || !iids) return E_INVALIDARG;
    if (!(array = CoTaskMemAlloc(4 * sizeof(IID)))) return E_OUTOFMEMORY;
    array[0] = IID_IAgileObject;
    array[1] = IID_IAsyncInfo;
    array[2] = IID_IAsyncOperation_BackgroundAccessStatus;
    array[3] = IID_IAsyncOperation_BackgroundAccessStatus_alt;
    *iid_count = 4;
    *iids = array;
    return S_OK;
}

static HRESULT WINAPI async_GetRuntimeClassName(IAsyncOperation_BackgroundAccessStatus *iface, HSTRING *class_name)
{
    static const WCHAR name[] = L"Windows.Foundation.IAsyncOperation`1<Windows.ApplicationModel.Background.BackgroundAccessStatus>";
    return WindowsCreateString(name, ARRAY_SIZE(name) - 1, class_name);
}

static HRESULT WINAPI async_GetTrustLevel(IAsyncOperation_BackgroundAccessStatus *iface, TrustLevel *trust_level)
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI async_put_Completed(IAsyncOperation_BackgroundAccessStatus *iface, IUnknown *handler)
{
    struct async_operation *impl = impl_from_IAsyncOperation(iface);
    TRACE("iface %p, handler %p.\n", iface, handler);
    if (handler)
    {
        IAsyncOperationCompletedHandlerVtbl *vtbl = *(IAsyncOperationCompletedHandlerVtbl **)handler;
        vtbl->Invoke(handler, (IInspectable *)&impl->IAsyncOperation_iface, Completed);
    }
    return S_OK;
}

static HRESULT WINAPI async_get_Completed(IAsyncOperation_BackgroundAccessStatus *iface, IUnknown **handler)
{
    if (!handler) return E_INVALIDARG;
    *handler = NULL;
    return S_OK;
}

static HRESULT WINAPI async_GetResults(IAsyncOperation_BackgroundAccessStatus *iface, BackgroundAccessStatus *results)
{
    TRACE("iface %p, results %p.\n", iface, results);
    if (!results) return E_INVALIDARG;
    *results = BackgroundAccessStatus_AlwaysAllowed;
    return S_OK;
}

static const struct IAsyncOperation_BackgroundAccessStatusVtbl async_operation_vtbl =
{
    async_QueryInterface,
    async_AddRef,
    async_Release,
    async_GetIids,
    async_GetRuntimeClassName,
    async_GetTrustLevel,
    async_put_Completed,
    async_get_Completed,
    async_GetResults,
};

/* IAsyncInfo methods */
static HRESULT WINAPI async_info_QueryInterface(IAsyncInfo *iface, REFIID iid, void **out)
{
    struct async_operation *impl = impl_from_IAsyncInfo(iface);
    return async_QueryInterface(&impl->IAsyncOperation_iface, iid, out);
}

static ULONG WINAPI async_info_AddRef(IAsyncInfo *iface)
{
    struct async_operation *impl = impl_from_IAsyncInfo(iface);
    return async_AddRef(&impl->IAsyncOperation_iface);
}

static ULONG WINAPI async_info_Release(IAsyncInfo *iface)
{
    struct async_operation *impl = impl_from_IAsyncInfo(iface);
    return async_Release(&impl->IAsyncOperation_iface);
}

static HRESULT WINAPI async_info_GetIids(IAsyncInfo *iface, ULONG *iid_count, IID **iids)
{
    struct async_operation *impl = impl_from_IAsyncInfo(iface);
    return async_GetIids(&impl->IAsyncOperation_iface, iid_count, iids);
}

static HRESULT WINAPI async_info_GetRuntimeClassName(IAsyncInfo *iface, HSTRING *class_name)
{
    struct async_operation *impl = impl_from_IAsyncInfo(iface);
    return async_GetRuntimeClassName(&impl->IAsyncOperation_iface, class_name);
}

static HRESULT WINAPI async_info_GetTrustLevel(IAsyncInfo *iface, TrustLevel *trust_level)
{
    struct async_operation *impl = impl_from_IAsyncInfo(iface);
    return async_GetTrustLevel(&impl->IAsyncOperation_iface, trust_level);
}

static HRESULT WINAPI async_info_get_Id(IAsyncInfo *iface, UINT32 *id)
{
    if (!id) return E_INVALIDARG;
    *id = 1;
    return S_OK;
}

static HRESULT WINAPI async_info_get_Status(IAsyncInfo *iface, AsyncStatus *status)
{
    if (!status) return E_INVALIDARG;
    *status = Completed;
    return S_OK;
}

static HRESULT WINAPI async_info_get_ErrorCode(IAsyncInfo *iface, HRESULT *error_code)
{
    if (!error_code) return E_INVALIDARG;
    *error_code = S_OK;
    return S_OK;
}

static HRESULT WINAPI async_info_Cancel(IAsyncInfo *iface)
{
    return S_OK;
}

static HRESULT WINAPI async_info_Close(IAsyncInfo *iface)
{
    return S_OK;
}

static const struct IAsyncInfoVtbl async_info_vtbl =
{
    async_info_QueryInterface,
    async_info_AddRef,
    async_info_Release,
    async_info_GetIids,
    async_info_GetRuntimeClassName,
    async_info_GetTrustLevel,
    async_info_get_Id,
    async_info_get_Status,
    async_info_get_ErrorCode,
    async_info_Cancel,
    async_info_Close,
};

static HRESULT async_operation_create(void **out)
{
    struct async_operation *impl;

    if (!out) return E_INVALIDARG;
    if (!(impl = calloc(1, sizeof(*impl)))) return E_OUTOFMEMORY;

    impl->IAsyncOperation_iface.lpVtbl = &async_operation_vtbl;
    impl->IAsyncInfo_iface.lpVtbl = &async_info_vtbl;
    impl->ref = 1;

    *out = &impl->IAsyncOperation_iface;
    return S_OK;
}

/* BackgroundExecutionManager activation factory & statics */
struct background_execution_manager_factory
{
    IActivationFactory IActivationFactory_iface;
    IBackgroundExecutionManagerStatics IBackgroundExecutionManagerStatics_iface;
    LONG ref;
};

static inline struct background_execution_manager_factory *impl_from_IActivationFactory(IActivationFactory *iface)
{
    return CONTAINING_RECORD(iface, struct background_execution_manager_factory, IActivationFactory_iface);
}

static inline struct background_execution_manager_factory *impl_from_IBackgroundExecutionManagerStatics(IBackgroundExecutionManagerStatics *iface)
{
    return CONTAINING_RECORD(iface, struct background_execution_manager_factory, IBackgroundExecutionManagerStatics_iface);
}

static HRESULT WINAPI factory_QueryInterface(IActivationFactory *iface, REFIID iid, void **out)
{
    struct background_execution_manager_factory *impl = impl_from_IActivationFactory(iface);

    TRACE("iface %p, iid %s, out %p.\n", iface, debugstr_guid(iid), out);

    if (!out) return E_INVALIDARG;

    if (IsEqualIID(iid, &IID_IUnknown) ||
        IsEqualIID(iid, &IID_IInspectable) ||
        IsEqualIID(iid, &IID_IAgileObject) ||
        IsEqualIID(iid, &IID_IActivationFactory))
    {
        *out = &impl->IActivationFactory_iface;
    }
    else if (IsEqualIID(iid, &IID_IBackgroundExecutionManagerStatics))
    {
        *out = &impl->IBackgroundExecutionManagerStatics_iface;
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
    struct background_execution_manager_factory *impl = impl_from_IActivationFactory(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI factory_Release(IActivationFactory *iface)
{
    struct background_execution_manager_factory *impl = impl_from_IActivationFactory(iface);
    return InterlockedDecrement(&impl->ref);
}

static HRESULT WINAPI factory_GetIids(IActivationFactory *iface, ULONG *iid_count, IID **iids)
{
    IID *array;
    if (!iid_count || !iids) return E_INVALIDARG;
    if (!(array = CoTaskMemAlloc(3 * sizeof(IID)))) return E_OUTOFMEMORY;
    array[0] = IID_IAgileObject;
    array[1] = IID_IActivationFactory;
    array[2] = IID_IBackgroundExecutionManagerStatics;
    *iid_count = 3;
    *iids = array;
    return S_OK;
}

static HRESULT WINAPI factory_GetRuntimeClassName(IActivationFactory *iface, HSTRING *class_name)
{
    return WindowsCreateString(RuntimeClass_Windows_ApplicationModel_Background_BackgroundExecutionManager,
                               wcslen(RuntimeClass_Windows_ApplicationModel_Background_BackgroundExecutionManager),
                               class_name);
}

static HRESULT WINAPI factory_GetTrustLevel(IActivationFactory *iface, TrustLevel *trust_level)
{
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

/* IBackgroundExecutionManagerStatics methods */
static HRESULT WINAPI statics_QueryInterface(IBackgroundExecutionManagerStatics *iface, REFIID iid, void **out)
{
    struct background_execution_manager_factory *impl = impl_from_IBackgroundExecutionManagerStatics(iface);
    return factory_QueryInterface(&impl->IActivationFactory_iface, iid, out);
}

static ULONG WINAPI statics_AddRef(IBackgroundExecutionManagerStatics *iface)
{
    struct background_execution_manager_factory *impl = impl_from_IBackgroundExecutionManagerStatics(iface);
    return factory_AddRef(&impl->IActivationFactory_iface);
}

static ULONG WINAPI statics_Release(IBackgroundExecutionManagerStatics *iface)
{
    struct background_execution_manager_factory *impl = impl_from_IBackgroundExecutionManagerStatics(iface);
    return factory_Release(&impl->IActivationFactory_iface);
}

static HRESULT WINAPI statics_GetIids(IBackgroundExecutionManagerStatics *iface, ULONG *iid_count, IID **iids)
{
    struct background_execution_manager_factory *impl = impl_from_IBackgroundExecutionManagerStatics(iface);
    return factory_GetIids(&impl->IActivationFactory_iface, iid_count, iids);
}

static HRESULT WINAPI statics_GetRuntimeClassName(IBackgroundExecutionManagerStatics *iface, HSTRING *class_name)
{
    struct background_execution_manager_factory *impl = impl_from_IBackgroundExecutionManagerStatics(iface);
    return factory_GetRuntimeClassName(&impl->IActivationFactory_iface, class_name);
}

static HRESULT WINAPI statics_GetTrustLevel(IBackgroundExecutionManagerStatics *iface, TrustLevel *trust_level)
{
    struct background_execution_manager_factory *impl = impl_from_IBackgroundExecutionManagerStatics(iface);
    return factory_GetTrustLevel(&impl->IActivationFactory_iface, trust_level);
}

static HRESULT WINAPI statics_RequestAccessAsync(IBackgroundExecutionManagerStatics *iface, void **operation)
{
    TRACE("iface %p, operation %p.\n", iface, operation);
    return async_operation_create(operation);
}

static HRESULT WINAPI statics_RequestAccessForApplicationAsync(IBackgroundExecutionManagerStatics *iface, HSTRING appid, void **operation)
{
    TRACE("iface %p, appid %s, operation %p.\n", iface, debugstr_hstring(appid), operation);
    return async_operation_create(operation);
}

static HRESULT WINAPI statics_RemoveAccess(IBackgroundExecutionManagerStatics *iface)
{
    TRACE("iface %p.\n", iface);
    return S_OK;
}

static HRESULT WINAPI statics_RemoveAccessForApplication(IBackgroundExecutionManagerStatics *iface, HSTRING appid)
{
    TRACE("iface %p, appid %s.\n", iface, debugstr_hstring(appid));
    return S_OK;
}

static HRESULT WINAPI statics_GetAccessStatus(IBackgroundExecutionManagerStatics *iface, BackgroundAccessStatus *status)
{
    TRACE("iface %p, status %p.\n", iface, status);
    if (!status) return E_INVALIDARG;
    *status = BackgroundAccessStatus_AlwaysAllowed;
    return S_OK;
}

static HRESULT WINAPI statics_GetAccessStatusForApplication(IBackgroundExecutionManagerStatics *iface, HSTRING appid, BackgroundAccessStatus *status)
{
    TRACE("iface %p, appid %s, status %p.\n", iface, debugstr_hstring(appid), status);
    if (!status) return E_INVALIDARG;
    *status = BackgroundAccessStatus_AlwaysAllowed;
    return S_OK;
}

static const struct IBackgroundExecutionManagerStaticsVtbl statics_vtbl =
{
    statics_QueryInterface,
    statics_AddRef,
    statics_Release,
    statics_GetIids,
    statics_GetRuntimeClassName,
    statics_GetTrustLevel,
    statics_RequestAccessAsync,
    statics_RequestAccessForApplicationAsync,
    statics_RemoveAccess,
    statics_RemoveAccessForApplication,
    statics_GetAccessStatus,
    statics_GetAccessStatusForApplication,
};

static struct background_execution_manager_factory background_execution_manager_factory_obj =
{
    .IActivationFactory_iface.lpVtbl = &factory_vtbl,
    .IBackgroundExecutionManagerStatics_iface.lpVtbl = &statics_vtbl,
    .ref = 1,
};

IActivationFactory *background_execution_manager_factory = &background_execution_manager_factory_obj.IActivationFactory_iface;
