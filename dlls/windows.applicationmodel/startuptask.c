/*
 * WinRT Windows.ApplicationModel.StartupTask Implementation
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

/* GUID definitions */
static const GUID IID_IStartupTaskStatics = {0xee5b60bd, 0xa148, 0x41a7, {0xb2, 0x6e, 0xe8, 0xb8, 0x8a, 0x1e, 0x62, 0xf8}};
static const GUID IID_IStartupTask = {0xf75c23c8, 0xb5f2, 0x4f6c, {0x88, 0xdd, 0x36, 0xcb, 0x1d, 0x59, 0x9d, 0x17}};
static const GUID IID_IAsyncOperation_StartupTask = {0xf91c147e, 0x2fd8, 0x5e36, {0xb5, 0x4c, 0xad, 0x20, 0xc0, 0x82, 0x59, 0xf8}};
static const GUID IID_IAsyncOperation_StartupTask_cswinrt = {0xcbec7a4e, 0xa046, 0x5330, {0x87, 0x3d, 0x0f, 0xce, 0x22, 0x87, 0x92, 0xfa}};
static const GUID IID_IAsyncOperation_StartupTaskState = {0x8d68ba91, 0x3975, 0x5620, {0xb1, 0x2b, 0x0a, 0xa5, 0xc0, 0x99, 0xde, 0x67}};
static const GUID IID_IVectorView_StartupTask = {0xad57d05f, 0xf1bc, 0x51d8, {0xbe, 0x2a, 0xab, 0x4d, 0x04, 0xa1, 0x1e, 0x9c}};
static const GUID IID_IAsyncOperation_IVectorView_StartupTask = {0x937bc713, 0x0422, 0x5347, {0xa3, 0x42, 0x55, 0x58, 0x2a, 0xfc, 0x0d, 0x32}};

#define RuntimeClass_Windows_ApplicationModel_StartupTask \
    L"Windows.ApplicationModel.StartupTask"

typedef enum StartupTaskState
{
    StartupTaskState_Disabled = 0,
    StartupTaskState_DisabledByUser = 1,
    StartupTaskState_Enabled = 2,
    StartupTaskState_DisabledByPolicy = 3
} StartupTaskState;

/* Completed handler vtbl */
typedef struct IAsyncOperationCompletedHandlerVtbl
{
    HRESULT (WINAPI *QueryInterface)(IUnknown *This, REFIID riid, void **ppvObject);
    ULONG   (WINAPI *AddRef)(IUnknown *This);
    ULONG   (WINAPI *Release)(IUnknown *This);
    HRESULT (WINAPI *Invoke)(IUnknown *This, IInspectable *asyncInfo, AsyncStatus asyncStatus);
} IAsyncOperationCompletedHandlerVtbl;

/* IStartupTask declarations */
typedef struct IStartupTask IStartupTask;

typedef struct IStartupTaskVtbl
{
    /* IUnknown */
    HRESULT (WINAPI *QueryInterface)(IStartupTask *This, REFIID riid, void **ppvObject);
    ULONG   (WINAPI *AddRef)(IStartupTask *This);
    ULONG   (WINAPI *Release)(IStartupTask *This);
    /* IInspectable */
    HRESULT (WINAPI *GetIids)(IStartupTask *This, ULONG *iidCount, IID **iids);
    HRESULT (WINAPI *GetRuntimeClassName)(IStartupTask *This, HSTRING *className);
    HRESULT (WINAPI *GetTrustLevel)(IStartupTask *This, TrustLevel *trustLevel);
    /* IStartupTask */
    HRESULT (WINAPI *RequestEnableAsync)(IStartupTask *This, void **operation);
    HRESULT (WINAPI *Disable)(IStartupTask *This);
    HRESULT (WINAPI *get_State)(IStartupTask *This, StartupTaskState *value);
    HRESULT (WINAPI *get_TaskId)(IStartupTask *This, HSTRING *value);
} IStartupTaskVtbl;

struct IStartupTask
{
    const IStartupTaskVtbl *lpVtbl;
};

#define IStartupTask_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IStartupTask_Release(This) (This)->lpVtbl->Release(This)

struct startup_task_impl
{
    IStartupTask IStartupTask_iface;
    LONG ref;
    HSTRING task_id;
};

static inline struct startup_task_impl *impl_from_IStartupTask(IStartupTask *iface)
{
    return CONTAINING_RECORD(iface, struct startup_task_impl, IStartupTask_iface);
}

static HRESULT startup_task_create(HSTRING task_id, struct startup_task_impl **out);

/* IStartupTaskStatics declarations */
typedef struct IStartupTaskStatics IStartupTaskStatics;

typedef struct IStartupTaskStaticsVtbl
{
    /* IUnknown */
    HRESULT (WINAPI *QueryInterface)(IStartupTaskStatics *This, REFIID riid, void **ppvObject);
    ULONG   (WINAPI *AddRef)(IStartupTaskStatics *This);
    ULONG   (WINAPI *Release)(IStartupTaskStatics *This);
    /* IInspectable */
    HRESULT (WINAPI *GetIids)(IStartupTaskStatics *This, ULONG *iidCount, IID **iids);
    HRESULT (WINAPI *GetRuntimeClassName)(IStartupTaskStatics *This, HSTRING *className);
    HRESULT (WINAPI *GetTrustLevel)(IStartupTaskStatics *This, TrustLevel *trustLevel);
    /* IStartupTaskStatics */
    HRESULT (WINAPI *GetForCurrentPackageAsync)(IStartupTaskStatics *This, void **operation);
    HRESULT (WINAPI *GetAsync)(IStartupTaskStatics *This, HSTRING taskId, void **operation);
} IStartupTaskStaticsVtbl;

struct IStartupTaskStatics
{
    const IStartupTaskStaticsVtbl *lpVtbl;
};

/* IAsyncOperation<StartupTaskState> */
typedef struct IAsyncOperation_StartupTaskState IAsyncOperation_StartupTaskState;

typedef struct IAsyncOperation_StartupTaskStateVtbl
{
    HRESULT (WINAPI *QueryInterface)(IAsyncOperation_StartupTaskState *This, REFIID riid, void **ppvObject);
    ULONG   (WINAPI *AddRef)(IAsyncOperation_StartupTaskState *This);
    ULONG   (WINAPI *Release)(IAsyncOperation_StartupTaskState *This);
    HRESULT (WINAPI *GetIids)(IAsyncOperation_StartupTaskState *This, ULONG *iidCount, IID **iids);
    HRESULT (WINAPI *GetRuntimeClassName)(IAsyncOperation_StartupTaskState *This, HSTRING *className);
    HRESULT (WINAPI *GetTrustLevel)(IAsyncOperation_StartupTaskState *This, TrustLevel *trustLevel);
    HRESULT (WINAPI *put_Completed)(IAsyncOperation_StartupTaskState *This, IUnknown *handler);
    HRESULT (WINAPI *get_Completed)(IAsyncOperation_StartupTaskState *This, IUnknown **handler);
    HRESULT (WINAPI *GetResults)(IAsyncOperation_StartupTaskState *This, StartupTaskState *results);
} IAsyncOperation_StartupTaskStateVtbl;

struct IAsyncOperation_StartupTaskState
{
    const IAsyncOperation_StartupTaskStateVtbl *lpVtbl;
};

struct async_operation_state
{
    IAsyncOperation_StartupTaskState IAsyncOperation_iface;
    IAsyncInfo IAsyncInfo_iface;
    LONG ref;
    StartupTaskState state;
};

static inline struct async_operation_state *impl_from_IAsyncOperation_StartupTaskState(IAsyncOperation_StartupTaskState *iface)
{
    return CONTAINING_RECORD(iface, struct async_operation_state, IAsyncOperation_iface);
}

static inline struct async_operation_state *impl_from_state_IAsyncInfo(IAsyncInfo *iface)
{
    return CONTAINING_RECORD(iface, struct async_operation_state, IAsyncInfo_iface);
}

static HRESULT WINAPI async_state_QueryInterface(IAsyncOperation_StartupTaskState *iface, REFIID iid, void **out)
{
    struct async_operation_state *impl = impl_from_IAsyncOperation_StartupTaskState(iface);
    TRACE("iface %p, iid %s, out %p.\n", iface, debugstr_guid(iid), out);
    if (!out) return E_INVALIDARG;

    if (IsEqualIID(iid, &IID_IUnknown) ||
        IsEqualIID(iid, &IID_IInspectable) ||
        IsEqualIID(iid, &IID_IAgileObject) ||
        IsEqualIID(iid, &IID_IAsyncOperation_StartupTaskState))
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

static ULONG WINAPI async_state_AddRef(IAsyncOperation_StartupTaskState *iface)
{
    struct async_operation_state *impl = impl_from_IAsyncOperation_StartupTaskState(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI async_state_Release(IAsyncOperation_StartupTaskState *iface)
{
    struct async_operation_state *impl = impl_from_IAsyncOperation_StartupTaskState(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI async_state_GetIids(IAsyncOperation_StartupTaskState *iface, ULONG *iid_count, IID **iids)
{
    IID *array;
    if (!iid_count || !iids) return E_INVALIDARG;
    if (!(array = CoTaskMemAlloc(3 * sizeof(IID)))) return E_OUTOFMEMORY;
    array[0] = IID_IAgileObject;
    array[1] = IID_IAsyncInfo;
    array[2] = IID_IAsyncOperation_StartupTaskState;
    *iid_count = 3;
    *iids = array;
    return S_OK;
}

static HRESULT WINAPI async_state_GetRuntimeClassName(IAsyncOperation_StartupTaskState *iface, HSTRING *class_name)
{
    static const WCHAR name[] = L"Windows.Foundation.IAsyncOperation`1<Windows.ApplicationModel.StartupTaskState>";
    return WindowsCreateString(name, ARRAY_SIZE(name) - 1, class_name);
}

static HRESULT WINAPI async_state_GetTrustLevel(IAsyncOperation_StartupTaskState *iface, TrustLevel *trust_level)
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI async_state_put_Completed(IAsyncOperation_StartupTaskState *iface, IUnknown *handler)
{
    struct async_operation_state *impl = impl_from_IAsyncOperation_StartupTaskState(iface);
    TRACE("iface %p, handler %p.\n", iface, handler);
    if (handler)
    {
        IAsyncOperationCompletedHandlerVtbl *vtbl = *(IAsyncOperationCompletedHandlerVtbl **)handler;
        vtbl->Invoke(handler, (IInspectable *)&impl->IAsyncOperation_iface, Completed);
    }
    return S_OK;
}

static HRESULT WINAPI async_state_get_Completed(IAsyncOperation_StartupTaskState *iface, IUnknown **handler)
{
    if (!handler) return E_INVALIDARG;
    *handler = NULL;
    return S_OK;
}

static HRESULT WINAPI async_state_GetResults(IAsyncOperation_StartupTaskState *iface, StartupTaskState *results)
{
    struct async_operation_state *impl = impl_from_IAsyncOperation_StartupTaskState(iface);
    TRACE("iface %p, results %p.\n", iface, results);
    if (!results) return E_INVALIDARG;
    *results = impl->state;
    return S_OK;
}

static const struct IAsyncOperation_StartupTaskStateVtbl async_state_vtbl =
{
    async_state_QueryInterface,
    async_state_AddRef,
    async_state_Release,
    async_state_GetIids,
    async_state_GetRuntimeClassName,
    async_state_GetTrustLevel,
    async_state_put_Completed,
    async_state_get_Completed,
    async_state_GetResults,
};

static HRESULT WINAPI async_state_info_QueryInterface(IAsyncInfo *iface, REFIID iid, void **out)
{
    struct async_operation_state *impl = impl_from_state_IAsyncInfo(iface);
    return async_state_QueryInterface(&impl->IAsyncOperation_iface, iid, out);
}

static ULONG WINAPI async_state_info_AddRef(IAsyncInfo *iface)
{
    struct async_operation_state *impl = impl_from_state_IAsyncInfo(iface);
    return async_state_AddRef(&impl->IAsyncOperation_iface);
}

static ULONG WINAPI async_state_info_Release(IAsyncInfo *iface)
{
    struct async_operation_state *impl = impl_from_state_IAsyncInfo(iface);
    return async_state_Release(&impl->IAsyncOperation_iface);
}

static HRESULT WINAPI async_state_info_GetIids(IAsyncInfo *iface, ULONG *iid_count, IID **iids)
{
    struct async_operation_state *impl = impl_from_state_IAsyncInfo(iface);
    return async_state_GetIids(&impl->IAsyncOperation_iface, iid_count, iids);
}

static HRESULT WINAPI async_state_info_GetRuntimeClassName(IAsyncInfo *iface, HSTRING *class_name)
{
    struct async_operation_state *impl = impl_from_state_IAsyncInfo(iface);
    return async_state_GetRuntimeClassName(&impl->IAsyncOperation_iface, class_name);
}

static HRESULT WINAPI async_state_info_GetTrustLevel(IAsyncInfo *iface, TrustLevel *trust_level)
{
    struct async_operation_state *impl = impl_from_state_IAsyncInfo(iface);
    return async_state_GetTrustLevel(&impl->IAsyncOperation_iface, trust_level);
}

static HRESULT WINAPI async_state_info_get_Id(IAsyncInfo *iface, UINT32 *id)
{
    if (!id) return E_INVALIDARG;
    *id = 1;
    return S_OK;
}

static HRESULT WINAPI async_state_info_get_Status(IAsyncInfo *iface, AsyncStatus *status)
{
    if (!status) return E_INVALIDARG;
    *status = Completed;
    return S_OK;
}

static HRESULT WINAPI async_state_info_get_ErrorCode(IAsyncInfo *iface, HRESULT *error_code)
{
    if (!error_code) return E_INVALIDARG;
    *error_code = S_OK;
    return S_OK;
}

static HRESULT WINAPI async_state_info_Cancel(IAsyncInfo *iface)
{
    return S_OK;
}

static HRESULT WINAPI async_state_info_Close(IAsyncInfo *iface)
{
    return S_OK;
}

static const struct IAsyncInfoVtbl async_state_info_vtbl =
{
    async_state_info_QueryInterface,
    async_state_info_AddRef,
    async_state_info_Release,
    async_state_info_GetIids,
    async_state_info_GetRuntimeClassName,
    async_state_info_GetTrustLevel,
    async_state_info_get_Id,
    async_state_info_get_Status,
    async_state_info_get_ErrorCode,
    async_state_info_Cancel,
    async_state_info_Close,
};

static HRESULT async_operation_startup_task_state_create(StartupTaskState state, void **out)
{
    struct async_operation_state *impl;

    if (!out) return E_INVALIDARG;
    if (!(impl = calloc(1, sizeof(*impl)))) return E_OUTOFMEMORY;

    impl->IAsyncOperation_iface.lpVtbl = &async_state_vtbl;
    impl->IAsyncInfo_iface.lpVtbl = &async_state_info_vtbl;
    impl->ref = 1;
    impl->state = state;

    *out = &impl->IAsyncOperation_iface;
    return S_OK;
}

/* IStartupTask implementation */
static HRESULT WINAPI startup_task_QueryInterface(IStartupTask *iface, REFIID iid, void **out)
{
    struct startup_task_impl *impl = impl_from_IStartupTask(iface);
    MESSAGE("startuptask: startup_task_QueryInterface iid %s\n", debugstr_guid(iid));
    if (!out) return E_INVALIDARG;

    if (IsEqualIID(iid, &IID_IUnknown) ||
        IsEqualIID(iid, &IID_IInspectable) ||
        IsEqualIID(iid, &IID_IAgileObject) ||
        IsEqualIID(iid, &IID_IStartupTask))
    {
        *out = &impl->IStartupTask_iface;
        IUnknown_AddRef((IUnknown *)*out);
        return S_OK;
    }

    WARN("unsupported iid %s\n", debugstr_guid(iid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI startup_task_AddRef(IStartupTask *iface)
{
    struct startup_task_impl *impl = impl_from_IStartupTask(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI startup_task_Release(IStartupTask *iface)
{
    struct startup_task_impl *impl = impl_from_IStartupTask(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref)
    {
        if (impl->task_id) WindowsDeleteString(impl->task_id);
        free(impl);
    }
    return ref;
}

static HRESULT WINAPI startup_task_GetIids(IStartupTask *iface, ULONG *iid_count, IID **iids)
{
    IID *array;
    if (!iid_count || !iids) return E_INVALIDARG;
    if (!(array = CoTaskMemAlloc(2 * sizeof(IID)))) return E_OUTOFMEMORY;
    array[0] = IID_IAgileObject;
    array[1] = IID_IStartupTask;
    *iid_count = 2;
    *iids = array;
    return S_OK;
}

static HRESULT WINAPI startup_task_GetRuntimeClassName(IStartupTask *iface, HSTRING *class_name)
{
    return WindowsCreateString(RuntimeClass_Windows_ApplicationModel_StartupTask,
                               wcslen(RuntimeClass_Windows_ApplicationModel_StartupTask),
                               class_name);
}

static HRESULT WINAPI startup_task_GetTrustLevel(IStartupTask *iface, TrustLevel *trust_level)
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI startup_task_RequestEnableAsync(IStartupTask *iface, void **operation)
{
    TRACE("iface %p, operation %p.\n", iface, operation);
    return async_operation_startup_task_state_create(StartupTaskState_Disabled, operation);
}

static HRESULT WINAPI startup_task_Disable(IStartupTask *iface)
{
    TRACE("iface %p.\n", iface);
    return S_OK;
}

static HRESULT WINAPI startup_task_get_State(IStartupTask *iface, StartupTaskState *value)
{
    MESSAGE("startuptask: startup_task_get_State returning Disabled (0)\n");
    if (!value) return E_INVALIDARG;
    *value = StartupTaskState_Disabled;
    return S_OK;
}

static HRESULT WINAPI startup_task_get_TaskId(IStartupTask *iface, HSTRING *value)
{
    struct startup_task_impl *impl = impl_from_IStartupTask(iface);
    TRACE("iface %p, value %p.\n", iface, value);
    if (!value) return E_INVALIDARG;
    return WindowsDuplicateString(impl->task_id, value);
}

static const struct IStartupTaskVtbl startup_task_vtbl =
{
    startup_task_QueryInterface,
    startup_task_AddRef,
    startup_task_Release,
    startup_task_GetIids,
    startup_task_GetRuntimeClassName,
    startup_task_GetTrustLevel,
    startup_task_RequestEnableAsync,
    startup_task_Disable,
    startup_task_get_State,
    startup_task_get_TaskId,
};

static HRESULT startup_task_create(HSTRING task_id, struct startup_task_impl **out)
{
    struct startup_task_impl *impl;

    if (!out) return E_INVALIDARG;
    if (!(impl = calloc(1, sizeof(*impl)))) return E_OUTOFMEMORY;

    impl->IStartupTask_iface.lpVtbl = &startup_task_vtbl;
    impl->ref = 1;
    if (task_id) WindowsDuplicateString(task_id, &impl->task_id);

    *out = impl;
    return S_OK;
}

/* IAsyncOperation<StartupTask*> */
typedef struct IAsyncOperation_StartupTask IAsyncOperation_StartupTask;

typedef struct IAsyncOperation_StartupTaskVtbl
{
    HRESULT (WINAPI *QueryInterface)(IAsyncOperation_StartupTask *This, REFIID riid, void **ppvObject);
    ULONG   (WINAPI *AddRef)(IAsyncOperation_StartupTask *This);
    ULONG   (WINAPI *Release)(IAsyncOperation_StartupTask *This);
    HRESULT (WINAPI *GetIids)(IAsyncOperation_StartupTask *This, ULONG *iidCount, IID **iids);
    HRESULT (WINAPI *GetRuntimeClassName)(IAsyncOperation_StartupTask *This, HSTRING *className);
    HRESULT (WINAPI *GetTrustLevel)(IAsyncOperation_StartupTask *This, TrustLevel *trustLevel);
    HRESULT (WINAPI *put_Completed)(IAsyncOperation_StartupTask *This, IUnknown *handler);
    HRESULT (WINAPI *get_Completed)(IAsyncOperation_StartupTask *This, IUnknown **handler);
    HRESULT (WINAPI *GetResults)(IAsyncOperation_StartupTask *This, IStartupTask **results);
} IAsyncOperation_StartupTaskVtbl;

struct IAsyncOperation_StartupTask
{
    const IAsyncOperation_StartupTaskVtbl *lpVtbl;
};

struct async_operation_task
{
    IAsyncOperation_StartupTask IAsyncOperation_iface;
    IAsyncInfo IAsyncInfo_iface;
    LONG ref;
    struct startup_task_impl *task;
};

static inline struct async_operation_task *impl_from_IAsyncOperation_StartupTask(IAsyncOperation_StartupTask *iface)
{
    return CONTAINING_RECORD(iface, struct async_operation_task, IAsyncOperation_iface);
}

static inline struct async_operation_task *impl_from_task_IAsyncInfo(IAsyncInfo *iface)
{
    return CONTAINING_RECORD(iface, struct async_operation_task, IAsyncInfo_iface);
}

static HRESULT WINAPI async_task_QueryInterface(IAsyncOperation_StartupTask *iface, REFIID iid, void **out)
{
    struct async_operation_task *impl = impl_from_IAsyncOperation_StartupTask(iface);
    MESSAGE("startuptask: async_task_QueryInterface iid %s\n", debugstr_guid(iid));
    if (!out) return E_INVALIDARG;

    if (IsEqualIID(iid, &IID_IUnknown) ||
        IsEqualIID(iid, &IID_IInspectable) ||
        IsEqualIID(iid, &IID_IAgileObject) ||
        IsEqualIID(iid, &IID_IAsyncOperation_StartupTask) ||
        IsEqualIID(iid, &IID_IAsyncOperation_StartupTask_cswinrt))
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

static ULONG WINAPI async_task_AddRef(IAsyncOperation_StartupTask *iface)
{
    struct async_operation_task *impl = impl_from_IAsyncOperation_StartupTask(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI async_task_Release(IAsyncOperation_StartupTask *iface)
{
    struct async_operation_task *impl = impl_from_IAsyncOperation_StartupTask(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref)
    {
        if (impl->task) IStartupTask_Release(&impl->task->IStartupTask_iface);
        free(impl);
    }
    return ref;
}

static HRESULT WINAPI async_task_GetIids(IAsyncOperation_StartupTask *iface, ULONG *iid_count, IID **iids)
{
    IID *array;
    if (!iid_count || !iids) return E_INVALIDARG;
    if (!(array = CoTaskMemAlloc(4 * sizeof(IID)))) return E_OUTOFMEMORY;
    array[0] = IID_IAgileObject;
    array[1] = IID_IAsyncInfo;
    array[2] = IID_IAsyncOperation_StartupTask;
    array[3] = IID_IAsyncOperation_StartupTask_cswinrt;
    *iid_count = 4;
    *iids = array;
    return S_OK;
}

static HRESULT WINAPI async_task_GetRuntimeClassName(IAsyncOperation_StartupTask *iface, HSTRING *class_name)
{
    static const WCHAR name[] = L"Windows.Foundation.IAsyncOperation`1<Windows.ApplicationModel.StartupTask>";
    return WindowsCreateString(name, ARRAY_SIZE(name) - 1, class_name);
}

static HRESULT WINAPI async_task_GetTrustLevel(IAsyncOperation_StartupTask *iface, TrustLevel *trust_level)
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI async_task_put_Completed(IAsyncOperation_StartupTask *iface, IUnknown *handler)
{
    struct async_operation_task *impl = impl_from_IAsyncOperation_StartupTask(iface);
    MESSAGE("startuptask: put_Completed handler %p\n", handler);
    if (handler)
    {
        IAsyncOperationCompletedHandlerVtbl *vtbl = *(IAsyncOperationCompletedHandlerVtbl **)handler;
        vtbl->Invoke(handler, (IInspectable *)&impl->IAsyncOperation_iface, Completed);
    }
    return S_OK;
}

static HRESULT WINAPI async_task_get_Completed(IAsyncOperation_StartupTask *iface, IUnknown **handler)
{
    if (!handler) return E_INVALIDARG;
    *handler = NULL;
    return S_OK;
}

static HRESULT WINAPI async_task_GetResults(IAsyncOperation_StartupTask *iface, IStartupTask **results)
{
    struct async_operation_task *impl = impl_from_IAsyncOperation_StartupTask(iface);
    MESSAGE("startuptask: GetResults returning task %p\n", impl->task);
    if (!results) return E_INVALIDARG;
    *results = &impl->task->IStartupTask_iface;
    IStartupTask_AddRef(*results);
    return S_OK;
}

static const struct IAsyncOperation_StartupTaskVtbl async_task_vtbl =
{
    async_task_QueryInterface,
    async_task_AddRef,
    async_task_Release,
    async_task_GetIids,
    async_task_GetRuntimeClassName,
    async_task_GetTrustLevel,
    async_task_put_Completed,
    async_task_get_Completed,
    async_task_GetResults,
};

static HRESULT WINAPI async_task_info_QueryInterface(IAsyncInfo *iface, REFIID iid, void **out)
{
    struct async_operation_task *impl = impl_from_task_IAsyncInfo(iface);
    return async_task_QueryInterface(&impl->IAsyncOperation_iface, iid, out);
}

static ULONG WINAPI async_task_info_AddRef(IAsyncInfo *iface)
{
    struct async_operation_task *impl = impl_from_task_IAsyncInfo(iface);
    return async_task_AddRef(&impl->IAsyncOperation_iface);
}

static ULONG WINAPI async_task_info_Release(IAsyncInfo *iface)
{
    struct async_operation_task *impl = impl_from_task_IAsyncInfo(iface);
    return async_task_Release(&impl->IAsyncOperation_iface);
}

static HRESULT WINAPI async_task_info_GetIids(IAsyncInfo *iface, ULONG *iid_count, IID **iids)
{
    struct async_operation_task *impl = impl_from_task_IAsyncInfo(iface);
    return async_task_GetIids(&impl->IAsyncOperation_iface, iid_count, iids);
}

static HRESULT WINAPI async_task_info_GetRuntimeClassName(IAsyncInfo *iface, HSTRING *class_name)
{
    struct async_operation_task *impl = impl_from_task_IAsyncInfo(iface);
    return async_task_GetRuntimeClassName(&impl->IAsyncOperation_iface, class_name);
}

static HRESULT WINAPI async_task_info_GetTrustLevel(IAsyncInfo *iface, TrustLevel *trust_level)
{
    struct async_operation_task *impl = impl_from_task_IAsyncInfo(iface);
    return async_task_GetTrustLevel(&impl->IAsyncOperation_iface, trust_level);
}

static HRESULT WINAPI async_task_info_get_Id(IAsyncInfo *iface, UINT32 *id)
{
    if (!id) return E_INVALIDARG;
    *id = 1;
    return S_OK;
}

static HRESULT WINAPI async_task_info_get_Status(IAsyncInfo *iface, AsyncStatus *status)
{
    if (!status) return E_INVALIDARG;
    *status = Completed;
    return S_OK;
}

static HRESULT WINAPI async_task_info_get_ErrorCode(IAsyncInfo *iface, HRESULT *error_code)
{
    if (!error_code) return E_INVALIDARG;
    *error_code = S_OK;
    return S_OK;
}

static HRESULT WINAPI async_task_info_Cancel(IAsyncInfo *iface)
{
    return S_OK;
}

static HRESULT WINAPI async_task_info_Close(IAsyncInfo *iface)
{
    return S_OK;
}

static const struct IAsyncInfoVtbl async_task_info_vtbl =
{
    async_task_info_QueryInterface,
    async_task_info_AddRef,
    async_task_info_Release,
    async_task_info_GetIids,
    async_task_info_GetRuntimeClassName,
    async_task_info_GetTrustLevel,
    async_task_info_get_Id,
    async_task_info_get_Status,
    async_task_info_get_ErrorCode,
    async_task_info_Cancel,
    async_task_info_Close,
};

static HRESULT async_operation_startup_task_create(HSTRING task_id, void **out)
{
    struct async_operation_task *impl;
    struct startup_task_impl *task;
    HRESULT hr;

    if (!out) return E_INVALIDARG;

    if (FAILED(hr = startup_task_create(task_id, &task)))
        return hr;

    if (!(impl = calloc(1, sizeof(*impl))))
    {
        IStartupTask_Release(&task->IStartupTask_iface);
        return E_OUTOFMEMORY;
    }

    impl->IAsyncOperation_iface.lpVtbl = &async_task_vtbl;
    impl->IAsyncInfo_iface.lpVtbl = &async_task_info_vtbl;
    impl->ref = 1;
    impl->task = task;

    *out = &impl->IAsyncOperation_iface;
    return S_OK;
}

/* IVectorView<StartupTask*> implementation */
typedef struct IVectorView_StartupTask IVectorView_StartupTask;

typedef struct IVectorView_StartupTaskVtbl
{
    HRESULT (WINAPI *QueryInterface)(IVectorView_StartupTask *This, REFIID riid, void **ppvObject);
    ULONG   (WINAPI *AddRef)(IVectorView_StartupTask *This);
    ULONG   (WINAPI *Release)(IVectorView_StartupTask *This);
    HRESULT (WINAPI *GetIids)(IVectorView_StartupTask *This, ULONG *iidCount, IID **iids);
    HRESULT (WINAPI *GetRuntimeClassName)(IVectorView_StartupTask *This, HSTRING *className);
    HRESULT (WINAPI *GetTrustLevel)(IVectorView_StartupTask *This, TrustLevel *trustLevel);
    HRESULT (WINAPI *GetAt)(IVectorView_StartupTask *This, UINT32 index, IStartupTask **item);
    HRESULT (WINAPI *get_Size)(IVectorView_StartupTask *This, UINT32 *size);
    HRESULT (WINAPI *IndexOf)(IVectorView_StartupTask *This, IStartupTask *item, UINT32 *index, BOOLEAN *found);
    HRESULT (WINAPI *GetMany)(IVectorView_StartupTask *This, UINT32 startIndex, UINT32 capacity, IStartupTask **items, UINT32 *actual);
} IVectorView_StartupTaskVtbl;

struct IVectorView_StartupTask
{
    const IVectorView_StartupTaskVtbl *lpVtbl;
};

#define IVectorView_StartupTask_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IVectorView_StartupTask_Release(This) (This)->lpVtbl->Release(This)

struct vector_view_task
{
    IVectorView_StartupTask IVectorView_iface;
    LONG ref;
    struct startup_task_impl *task;
};

static inline struct vector_view_task *impl_from_IVectorView_StartupTask(IVectorView_StartupTask *iface)
{
    return CONTAINING_RECORD(iface, struct vector_view_task, IVectorView_iface);
}

static HRESULT WINAPI vector_view_QueryInterface(IVectorView_StartupTask *iface, REFIID iid, void **out)
{
    struct vector_view_task *impl = impl_from_IVectorView_StartupTask(iface);
    TRACE("iface %p, iid %s, out %p.\n", iface, debugstr_guid(iid), out);
    if (!out) return E_INVALIDARG;

    if (IsEqualIID(iid, &IID_IUnknown) ||
        IsEqualIID(iid, &IID_IInspectable) ||
        IsEqualIID(iid, &IID_IAgileObject) ||
        IsEqualIID(iid, &IID_IVectorView_StartupTask))
    {
        *out = &impl->IVectorView_iface;
        IUnknown_AddRef((IUnknown *)*out);
        return S_OK;
    }

    WARN("unsupported iid %s\n", debugstr_guid(iid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI vector_view_AddRef(IVectorView_StartupTask *iface)
{
    struct vector_view_task *impl = impl_from_IVectorView_StartupTask(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI vector_view_Release(IVectorView_StartupTask *iface)
{
    struct vector_view_task *impl = impl_from_IVectorView_StartupTask(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref)
    {
        if (impl->task) IStartupTask_Release(&impl->task->IStartupTask_iface);
        free(impl);
    }
    return ref;
}

static HRESULT WINAPI vector_view_GetIids(IVectorView_StartupTask *iface, ULONG *iid_count, IID **iids)
{
    IID *array;
    if (!iid_count || !iids) return E_INVALIDARG;
    if (!(array = CoTaskMemAlloc(2 * sizeof(IID)))) return E_OUTOFMEMORY;
    array[0] = IID_IAgileObject;
    array[1] = IID_IVectorView_StartupTask;
    *iid_count = 2;
    *iids = array;
    return S_OK;
}

static HRESULT WINAPI vector_view_GetRuntimeClassName(IVectorView_StartupTask *iface, HSTRING *class_name)
{
    static const WCHAR name[] = L"Windows.Foundation.Collections.IVectorView`1<Windows.ApplicationModel.StartupTask>";
    return WindowsCreateString(name, ARRAY_SIZE(name) - 1, class_name);
}

static HRESULT WINAPI vector_view_GetTrustLevel(IVectorView_StartupTask *iface, TrustLevel *trust_level)
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI vector_view_GetAt(IVectorView_StartupTask *iface, UINT32 index, IStartupTask **item)
{
    struct vector_view_task *impl = impl_from_IVectorView_StartupTask(iface);
    if (!item) return E_INVALIDARG;
    if (index != 0 || !impl->task) return E_BOUNDS;
    *item = &impl->task->IStartupTask_iface;
    IStartupTask_AddRef(*item);
    return S_OK;
}

static HRESULT WINAPI vector_view_get_Size(IVectorView_StartupTask *iface, UINT32 *size)
{
    struct vector_view_task *impl = impl_from_IVectorView_StartupTask(iface);
    if (!size) return E_INVALIDARG;
    *size = impl->task ? 1 : 0;
    return S_OK;
}

static HRESULT WINAPI vector_view_IndexOf(IVectorView_StartupTask *iface, IStartupTask *item, UINT32 *index, BOOLEAN *found)
{
    if (!index || !found) return E_INVALIDARG;
    *index = 0;
    *found = FALSE;
    return S_OK;
}

static HRESULT WINAPI vector_view_GetMany(IVectorView_StartupTask *iface, UINT32 startIndex, UINT32 capacity, IStartupTask **items, UINT32 *actual)
{
    struct vector_view_task *impl = impl_from_IVectorView_StartupTask(iface);
    if (!actual) return E_INVALIDARG;
    if (startIndex == 0 && capacity >= 1 && impl->task)
    {
        items[0] = &impl->task->IStartupTask_iface;
        IStartupTask_AddRef(items[0]);
        *actual = 1;
    }
    else
    {
        *actual = 0;
    }
    return S_OK;
}

static const struct IVectorView_StartupTaskVtbl vector_view_vtbl =
{
    vector_view_QueryInterface,
    vector_view_AddRef,
    vector_view_Release,
    vector_view_GetIids,
    vector_view_GetRuntimeClassName,
    vector_view_GetTrustLevel,
    vector_view_GetAt,
    vector_view_get_Size,
    vector_view_IndexOf,
    vector_view_GetMany,
};

/* IAsyncOperation<IVectorView<StartupTask*>> */
typedef struct IAsyncOperation_IVectorView_StartupTask IAsyncOperation_IVectorView_StartupTask;

typedef struct IAsyncOperation_IVectorView_StartupTaskVtbl
{
    HRESULT (WINAPI *QueryInterface)(IAsyncOperation_IVectorView_StartupTask *This, REFIID riid, void **ppvObject);
    ULONG   (WINAPI *AddRef)(IAsyncOperation_IVectorView_StartupTask *This);
    ULONG   (WINAPI *Release)(IAsyncOperation_IVectorView_StartupTask *This);
    HRESULT (WINAPI *GetIids)(IAsyncOperation_IVectorView_StartupTask *This, ULONG *iidCount, IID **iids);
    HRESULT (WINAPI *GetRuntimeClassName)(IAsyncOperation_IVectorView_StartupTask *This, HSTRING *className);
    HRESULT (WINAPI *GetTrustLevel)(IAsyncOperation_IVectorView_StartupTask *This, TrustLevel *trustLevel);
    HRESULT (WINAPI *put_Completed)(IAsyncOperation_IVectorView_StartupTask *This, IUnknown *handler);
    HRESULT (WINAPI *get_Completed)(IAsyncOperation_IVectorView_StartupTask *This, IUnknown **handler);
    HRESULT (WINAPI *GetResults)(IAsyncOperation_IVectorView_StartupTask *This, IVectorView_StartupTask **results);
} IAsyncOperation_IVectorView_StartupTaskVtbl;

struct IAsyncOperation_IVectorView_StartupTask
{
    const IAsyncOperation_IVectorView_StartupTaskVtbl *lpVtbl;
};

struct async_operation_vector_view
{
    IAsyncOperation_IVectorView_StartupTask IAsyncOperation_iface;
    IAsyncInfo IAsyncInfo_iface;
    LONG ref;
    struct vector_view_task *view;
};

static inline struct async_operation_vector_view *impl_from_IAsyncOperation_IVectorView(IAsyncOperation_IVectorView_StartupTask *iface)
{
    return CONTAINING_RECORD(iface, struct async_operation_vector_view, IAsyncOperation_iface);
}

static inline struct async_operation_vector_view *impl_from_view_IAsyncInfo(IAsyncInfo *iface)
{
    return CONTAINING_RECORD(iface, struct async_operation_vector_view, IAsyncInfo_iface);
}

static HRESULT WINAPI async_view_QueryInterface(IAsyncOperation_IVectorView_StartupTask *iface, REFIID iid, void **out)
{
    struct async_operation_vector_view *impl = impl_from_IAsyncOperation_IVectorView(iface);
    TRACE("iface %p, iid %s, out %p.\n", iface, debugstr_guid(iid), out);
    if (!out) return E_INVALIDARG;

    if (IsEqualIID(iid, &IID_IUnknown) ||
        IsEqualIID(iid, &IID_IInspectable) ||
        IsEqualIID(iid, &IID_IAgileObject) ||
        IsEqualIID(iid, &IID_IAsyncOperation_IVectorView_StartupTask))
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

static ULONG WINAPI async_view_AddRef(IAsyncOperation_IVectorView_StartupTask *iface)
{
    struct async_operation_vector_view *impl = impl_from_IAsyncOperation_IVectorView(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI async_view_Release(IAsyncOperation_IVectorView_StartupTask *iface)
{
    struct async_operation_vector_view *impl = impl_from_IAsyncOperation_IVectorView(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref)
    {
        if (impl->view) IVectorView_StartupTask_Release(&impl->view->IVectorView_iface);
        free(impl);
    }
    return ref;
}

static HRESULT WINAPI async_view_GetIids(IAsyncOperation_IVectorView_StartupTask *iface, ULONG *iid_count, IID **iids)
{
    IID *array;
    if (!iid_count || !iids) return E_INVALIDARG;
    if (!(array = CoTaskMemAlloc(3 * sizeof(IID)))) return E_OUTOFMEMORY;
    array[0] = IID_IAgileObject;
    array[1] = IID_IAsyncInfo;
    array[2] = IID_IAsyncOperation_IVectorView_StartupTask;
    *iid_count = 3;
    *iids = array;
    return S_OK;
}

static HRESULT WINAPI async_view_GetRuntimeClassName(IAsyncOperation_IVectorView_StartupTask *iface, HSTRING *class_name)
{
    static const WCHAR name[] = L"Windows.Foundation.IAsyncOperation`1<Windows.Foundation.Collections.IVectorView`1<Windows.ApplicationModel.StartupTask>>";
    return WindowsCreateString(name, ARRAY_SIZE(name) - 1, class_name);
}

static HRESULT WINAPI async_view_GetTrustLevel(IAsyncOperation_IVectorView_StartupTask *iface, TrustLevel *trust_level)
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI async_view_put_Completed(IAsyncOperation_IVectorView_StartupTask *iface, IUnknown *handler)
{
    struct async_operation_vector_view *impl = impl_from_IAsyncOperation_IVectorView(iface);
    TRACE("iface %p, handler %p.\n", iface, handler);
    if (handler)
    {
        IAsyncOperationCompletedHandlerVtbl *vtbl = *(IAsyncOperationCompletedHandlerVtbl **)handler;
        vtbl->Invoke(handler, (IInspectable *)&impl->IAsyncOperation_iface, Completed);
    }
    return S_OK;
}

static HRESULT WINAPI async_view_get_Completed(IAsyncOperation_IVectorView_StartupTask *iface, IUnknown **handler)
{
    if (!handler) return E_INVALIDARG;
    *handler = NULL;
    return S_OK;
}

static HRESULT WINAPI async_view_GetResults(IAsyncOperation_IVectorView_StartupTask *iface, IVectorView_StartupTask **results)
{
    struct async_operation_vector_view *impl = impl_from_IAsyncOperation_IVectorView(iface);
    TRACE("iface %p, results %p.\n", iface, results);
    if (!results) return E_INVALIDARG;
    *results = &impl->view->IVectorView_iface;
    IVectorView_StartupTask_AddRef(*results);
    return S_OK;
}

static const struct IAsyncOperation_IVectorView_StartupTaskVtbl async_view_vtbl =
{
    async_view_QueryInterface,
    async_view_AddRef,
    async_view_Release,
    async_view_GetIids,
    async_view_GetRuntimeClassName,
    async_view_GetTrustLevel,
    async_view_put_Completed,
    async_view_get_Completed,
    async_view_GetResults,
};

static HRESULT WINAPI async_view_info_QueryInterface(IAsyncInfo *iface, REFIID iid, void **out)
{
    struct async_operation_vector_view *impl = impl_from_view_IAsyncInfo(iface);
    return async_view_QueryInterface(&impl->IAsyncOperation_iface, iid, out);
}

static ULONG WINAPI async_view_info_AddRef(IAsyncInfo *iface)
{
    struct async_operation_vector_view *impl = impl_from_view_IAsyncInfo(iface);
    return async_view_AddRef(&impl->IAsyncOperation_iface);
}

static ULONG WINAPI async_view_info_Release(IAsyncInfo *iface)
{
    struct async_operation_vector_view *impl = impl_from_view_IAsyncInfo(iface);
    return async_view_Release(&impl->IAsyncOperation_iface);
}

static HRESULT WINAPI async_view_info_GetIids(IAsyncInfo *iface, ULONG *iid_count, IID **iids)
{
    struct async_operation_vector_view *impl = impl_from_view_IAsyncInfo(iface);
    return async_view_GetIids(&impl->IAsyncOperation_iface, iid_count, iids);
}

static HRESULT WINAPI async_view_info_GetRuntimeClassName(IAsyncInfo *iface, HSTRING *class_name)
{
    struct async_operation_vector_view *impl = impl_from_view_IAsyncInfo(iface);
    return async_view_GetRuntimeClassName(&impl->IAsyncOperation_iface, class_name);
}

static HRESULT WINAPI async_view_info_GetTrustLevel(IAsyncInfo *iface, TrustLevel *trust_level)
{
    struct async_operation_vector_view *impl = impl_from_view_IAsyncInfo(iface);
    return async_view_GetTrustLevel(&impl->IAsyncOperation_iface, trust_level);
}

static HRESULT WINAPI async_view_info_get_Id(IAsyncInfo *iface, UINT32 *id)
{
    if (!id) return E_INVALIDARG;
    *id = 1;
    return S_OK;
}

static HRESULT WINAPI async_view_info_get_Status(IAsyncInfo *iface, AsyncStatus *status)
{
    if (!status) return E_INVALIDARG;
    *status = Completed;
    return S_OK;
}

static HRESULT WINAPI async_view_info_get_ErrorCode(IAsyncInfo *iface, HRESULT *error_code)
{
    if (!error_code) return E_INVALIDARG;
    *error_code = S_OK;
    return S_OK;
}

static HRESULT WINAPI async_view_info_Cancel(IAsyncInfo *iface)
{
    return S_OK;
}

static HRESULT WINAPI async_view_info_Close(IAsyncInfo *iface)
{
    return S_OK;
}

static const struct IAsyncInfoVtbl async_view_info_vtbl =
{
    async_view_info_QueryInterface,
    async_view_info_AddRef,
    async_view_info_Release,
    async_view_info_GetIids,
    async_view_info_GetRuntimeClassName,
    async_view_info_GetTrustLevel,
    async_view_info_get_Id,
    async_view_info_get_Status,
    async_view_info_get_ErrorCode,
    async_view_info_Cancel,
    async_view_info_Close,
};

static HRESULT async_operation_vector_view_startup_task_create(void **out)
{
    struct async_operation_vector_view *impl;
    struct vector_view_task *view;
    struct startup_task_impl *task;
    HRESULT hr;

    if (!out) return E_INVALIDARG;

    if (FAILED(hr = startup_task_create(NULL, &task)))
        return hr;

    if (!(view = calloc(1, sizeof(*view))))
    {
        IStartupTask_Release(&task->IStartupTask_iface);
        return E_OUTOFMEMORY;
    }
    view->IVectorView_iface.lpVtbl = &vector_view_vtbl;
    view->ref = 1;
    view->task = task;

    if (!(impl = calloc(1, sizeof(*impl))))
    {
        IVectorView_StartupTask_Release(&view->IVectorView_iface);
        return E_OUTOFMEMORY;
    }

    impl->IAsyncOperation_iface.lpVtbl = &async_view_vtbl;
    impl->IAsyncInfo_iface.lpVtbl = &async_view_info_vtbl;
    impl->ref = 1;
    impl->view = view;

    *out = &impl->IAsyncOperation_iface;
    return S_OK;
}

/* Activation Factory & Statics */
struct startup_task_factory
{
    IActivationFactory IActivationFactory_iface;
    IStartupTaskStatics IStartupTaskStatics_iface;
    LONG ref;
};

static inline struct startup_task_factory *impl_from_IActivationFactory(IActivationFactory *iface)
{
    return CONTAINING_RECORD(iface, struct startup_task_factory, IActivationFactory_iface);
}

static inline struct startup_task_factory *impl_from_IStartupTaskStatics(IStartupTaskStatics *iface)
{
    return CONTAINING_RECORD(iface, struct startup_task_factory, IStartupTaskStatics_iface);
}

static HRESULT WINAPI factory_QueryInterface(IActivationFactory *iface, REFIID iid, void **out)
{
    struct startup_task_factory *impl = impl_from_IActivationFactory(iface);
    TRACE("iface %p, iid %s, out %p.\n", iface, debugstr_guid(iid), out);
    if (!out) return E_INVALIDARG;

    if (IsEqualIID(iid, &IID_IUnknown) ||
        IsEqualIID(iid, &IID_IInspectable) ||
        IsEqualIID(iid, &IID_IAgileObject) ||
        IsEqualIID(iid, &IID_IActivationFactory))
    {
        *out = &impl->IActivationFactory_iface;
    }
    else if (IsEqualIID(iid, &IID_IStartupTaskStatics))
    {
        *out = &impl->IStartupTaskStatics_iface;
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
    struct startup_task_factory *impl = impl_from_IActivationFactory(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI factory_Release(IActivationFactory *iface)
{
    struct startup_task_factory *impl = impl_from_IActivationFactory(iface);
    return InterlockedDecrement(&impl->ref);
}

static HRESULT WINAPI factory_GetIids(IActivationFactory *iface, ULONG *iid_count, IID **iids)
{
    IID *array;
    if (!iid_count || !iids) return E_INVALIDARG;
    if (!(array = CoTaskMemAlloc(3 * sizeof(IID)))) return E_OUTOFMEMORY;
    array[0] = IID_IAgileObject;
    array[1] = IID_IActivationFactory;
    array[2] = IID_IStartupTaskStatics;
    *iid_count = 3;
    *iids = array;
    return S_OK;
}

static HRESULT WINAPI factory_GetRuntimeClassName(IActivationFactory *iface, HSTRING *class_name)
{
    return WindowsCreateString(RuntimeClass_Windows_ApplicationModel_StartupTask,
                               wcslen(RuntimeClass_Windows_ApplicationModel_StartupTask),
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
    struct startup_task_impl *task;
    HRESULT hr;

    TRACE("iface %p, instance %p.\n", iface, instance);
    if (!instance) return E_INVALIDARG;

    if (SUCCEEDED(hr = startup_task_create(NULL, &task)))
        *instance = (IInspectable *)&task->IStartupTask_iface;
    else
        *instance = NULL;
    return hr;
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

static HRESULT WINAPI statics_QueryInterface(IStartupTaskStatics *iface, REFIID iid, void **out)
{
    struct startup_task_factory *impl = impl_from_IStartupTaskStatics(iface);
    return factory_QueryInterface(&impl->IActivationFactory_iface, iid, out);
}

static ULONG WINAPI statics_AddRef(IStartupTaskStatics *iface)
{
    struct startup_task_factory *impl = impl_from_IStartupTaskStatics(iface);
    return factory_AddRef(&impl->IActivationFactory_iface);
}

static ULONG WINAPI statics_Release(IStartupTaskStatics *iface)
{
    struct startup_task_factory *impl = impl_from_IStartupTaskStatics(iface);
    return factory_Release(&impl->IActivationFactory_iface);
}

static HRESULT WINAPI statics_GetIids(IStartupTaskStatics *iface, ULONG *iid_count, IID **iids)
{
    struct startup_task_factory *impl = impl_from_IStartupTaskStatics(iface);
    return factory_GetIids(&impl->IActivationFactory_iface, iid_count, iids);
}

static HRESULT WINAPI statics_GetRuntimeClassName(IStartupTaskStatics *iface, HSTRING *class_name)
{
    struct startup_task_factory *impl = impl_from_IStartupTaskStatics(iface);
    return factory_GetRuntimeClassName(&impl->IActivationFactory_iface, class_name);
}

static HRESULT WINAPI statics_GetTrustLevel(IStartupTaskStatics *iface, TrustLevel *trust_level)
{
    struct startup_task_factory *impl = impl_from_IStartupTaskStatics(iface);
    return factory_GetTrustLevel(&impl->IActivationFactory_iface, trust_level);
}

static HRESULT WINAPI statics_GetForCurrentPackageAsync(IStartupTaskStatics *iface, void **operation)
{
    TRACE("iface %p, operation %p.\n", iface, operation);
    return async_operation_vector_view_startup_task_create(operation);
}

static HRESULT WINAPI statics_GetAsync(IStartupTaskStatics *iface, HSTRING taskId, void **operation)
{
    MESSAGE("startuptask: statics_GetAsync taskId %s, operation %p\n", debugstr_hstring(taskId), operation);
    return async_operation_startup_task_create(taskId, operation);
}

static const struct IStartupTaskStaticsVtbl statics_vtbl =
{
    statics_QueryInterface,
    statics_AddRef,
    statics_Release,
    statics_GetIids,
    statics_GetRuntimeClassName,
    statics_GetTrustLevel,
    statics_GetForCurrentPackageAsync,
    statics_GetAsync,
};

static struct startup_task_factory startup_task_factory_obj =
{
    .IActivationFactory_iface.lpVtbl = &factory_vtbl,
    .IStartupTaskStatics_iface.lpVtbl = &statics_vtbl,
    .ref = 1,
};

IActivationFactory *startup_task_factory = &startup_task_factory_obj.IActivationFactory_iface;
