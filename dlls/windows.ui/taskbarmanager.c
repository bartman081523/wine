/* WinRT Windows.UI.Shell.TaskbarManager Implementation
 *
 * Copyright 2026 Xodus / Proton-GE contributors
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

/* {87490A19-1AD9-49F4-B2E8-86738DC5AC40} */
static const GUID IID_ITaskbarManager = {0x87490a19, 0x1ad9, 0x49f4, {0xb2, 0xe8, 0x86, 0x73, 0x8d, 0xc5, 0xac, 0x40}};
/* {DB32AB74-DE52-4FE6-B7B6-95FF9F8395DF} */
static const GUID IID_ITaskbarManagerStatics = {0xdb32ab74, 0xde52, 0x4fe6, {0xb7, 0xb6, 0x95, 0xff, 0x9f, 0x83, 0x95, 0xdf}};

struct async_bool
{
    IAsyncOperation_boolean IAsyncOperation_boolean_iface;
    IAsyncInfo IAsyncInfo_iface;
    LONG ref;
    AsyncStatus status;
    HRESULT hr;
    boolean result;
    IAsyncOperationCompletedHandler_boolean *handler;
};

static inline struct async_bool *impl_from_IAsyncOperation_boolean( IAsyncOperation_boolean *iface )
{
    return CONTAINING_RECORD( iface, struct async_bool, IAsyncOperation_boolean_iface );
}

static inline struct async_bool *impl_from_async_bool_IAsyncInfo( IAsyncInfo *iface )
{
    return CONTAINING_RECORD( iface, struct async_bool, IAsyncInfo_iface );
}

static HRESULT WINAPI async_bool_QueryInterface( IAsyncOperation_boolean *iface, REFIID iid, void **out )
{
    struct async_bool *impl = impl_from_IAsyncOperation_boolean( iface );
    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (!out) return E_POINTER;
    *out = NULL;

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAsyncOperation_boolean ))
    {
        *out = &impl->IAsyncOperation_boolean_iface;
        IAsyncOperation_boolean_AddRef( iface );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IAsyncInfo ))
    {
        *out = &impl->IAsyncInfo_iface;
        IAsyncOperation_boolean_AddRef( iface );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    return E_NOINTERFACE;
}

static ULONG WINAPI async_bool_AddRef( IAsyncOperation_boolean *iface )
{
    struct async_bool *impl = impl_from_IAsyncOperation_boolean( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI async_bool_Release( IAsyncOperation_boolean *iface )
{
    struct async_bool *impl = impl_from_IAsyncOperation_boolean( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    if (!ref)
    {
        if (impl->handler) IAsyncOperationCompletedHandler_boolean_Release( impl->handler );
        free( impl );
    }
    return ref;
}

static HRESULT WINAPI async_bool_GetIids( IAsyncOperation_boolean *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI async_bool_GetRuntimeClassName( IAsyncOperation_boolean *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.IAsyncOperation`1<Boolean>", 42, class_name );
}

static HRESULT WINAPI async_bool_GetTrustLevel( IAsyncOperation_boolean *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI async_bool_put_Completed( IAsyncOperation_boolean *iface,
                                                IAsyncOperationCompletedHandler_boolean *handler )
{
    struct async_bool *impl = impl_from_IAsyncOperation_boolean( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );

    if (impl->handler) IAsyncOperationCompletedHandler_boolean_Release( impl->handler );
    if ((impl->handler = handler)) IAsyncOperationCompletedHandler_boolean_AddRef( handler );

    if (handler && impl->status != Started)
        IAsyncOperationCompletedHandler_boolean_Invoke( handler, &impl->IAsyncOperation_boolean_iface, impl->status );

    return S_OK;
}

static HRESULT WINAPI async_bool_get_Completed( IAsyncOperation_boolean *iface,
                                                IAsyncOperationCompletedHandler_boolean **handler )
{
    struct async_bool *impl = impl_from_IAsyncOperation_boolean( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );
    if (!handler) return E_POINTER;
    *handler = impl->handler;
    if (*handler) IAsyncOperationCompletedHandler_boolean_AddRef( *handler );
    return S_OK;
}

static HRESULT WINAPI async_bool_GetResults( IAsyncOperation_boolean *iface, boolean *results )
{
    struct async_bool *impl = impl_from_IAsyncOperation_boolean( iface );
    TRACE( "iface %p, results %p.\n", iface, results );
    if (results) *results = impl->result;
    return impl->hr;
}

static const struct IAsyncOperation_booleanVtbl async_bool_vtbl =
{
    async_bool_QueryInterface,
    async_bool_AddRef,
    async_bool_Release,
    async_bool_GetIids,
    async_bool_GetRuntimeClassName,
    async_bool_GetTrustLevel,
    async_bool_put_Completed,
    async_bool_get_Completed,
    async_bool_GetResults
};

static HRESULT WINAPI async_bool_info_QueryInterface( IAsyncInfo *iface, REFIID iid, void **out )
{
    struct async_bool *impl = impl_from_async_bool_IAsyncInfo( iface );
    return async_bool_QueryInterface( &impl->IAsyncOperation_boolean_iface, iid, out );
}

static ULONG WINAPI async_bool_info_AddRef( IAsyncInfo *iface )
{
    struct async_bool *impl = impl_from_async_bool_IAsyncInfo( iface );
    return async_bool_AddRef( &impl->IAsyncOperation_boolean_iface );
}

static ULONG WINAPI async_bool_info_Release( IAsyncInfo *iface )
{
    struct async_bool *impl = impl_from_async_bool_IAsyncInfo( iface );
    return async_bool_Release( &impl->IAsyncOperation_boolean_iface );
}

static HRESULT WINAPI async_bool_info_GetIids( IAsyncInfo *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI async_bool_info_GetRuntimeClassName( IAsyncInfo *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.IAsyncInfo", 27, class_name );
}

static HRESULT WINAPI async_bool_info_GetTrustLevel( IAsyncInfo *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI async_bool_info_get_Id( IAsyncInfo *iface, UINT32 *id )
{
    if (!id) return E_POINTER;
    *id = 1;
    return S_OK;
}

static HRESULT WINAPI async_bool_info_get_Status( IAsyncInfo *iface, AsyncStatus *status )
{
    struct async_bool *impl = impl_from_async_bool_IAsyncInfo( iface );
    if (!status) return E_POINTER;
    *status = impl->status;
    return S_OK;
}

static HRESULT WINAPI async_bool_info_get_ErrorCode( IAsyncInfo *iface, HRESULT *error_code )
{
    struct async_bool *impl = impl_from_async_bool_IAsyncInfo( iface );
    if (!error_code) return E_POINTER;
    *error_code = impl->hr;
    return S_OK;
}

static HRESULT WINAPI async_bool_info_Cancel( IAsyncInfo *iface )
{
    return S_OK;
}

static HRESULT WINAPI async_bool_info_Close( IAsyncInfo *iface )
{
    return S_OK;
}

static const struct IAsyncInfoVtbl async_bool_info_vtbl =
{
    async_bool_info_QueryInterface,
    async_bool_info_AddRef,
    async_bool_info_Release,
    async_bool_info_GetIids,
    async_bool_info_GetRuntimeClassName,
    async_bool_info_GetTrustLevel,
    async_bool_info_get_Id,
    async_bool_info_get_Status,
    async_bool_info_get_ErrorCode,
    async_bool_info_Cancel,
    async_bool_info_Close
};

static HRESULT async_bool_create( boolean result, HRESULT hr, AsyncStatus status, void **out )
{
    struct async_bool *impl;
    if (!out) return E_POINTER;
    *out = NULL;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IAsyncOperation_boolean_iface.lpVtbl = &async_bool_vtbl;
    impl->IAsyncInfo_iface.lpVtbl = &async_bool_info_vtbl;
    impl->ref = 1;
    impl->status = status;
    impl->hr = hr;
    impl->result = result;
    *out = &impl->IAsyncOperation_boolean_iface;
    TRACE( "created async_bool %p, result %d, hr %#lx, status %d\n", *out, result, hr, status );
    return S_OK;
}

struct taskbar_manager
{
    IInspectable IInspectable_iface;
    LONG ref;
};

static inline struct taskbar_manager *impl_from_ITaskbarManager( void *iface )
{
    return CONTAINING_RECORD( iface, struct taskbar_manager, IInspectable_iface );
}

static HRESULT WINAPI taskbar_manager_QueryInterface( void *iface, REFIID riid, void **out )
{
    struct taskbar_manager *impl = impl_from_ITaskbarManager( iface );
    TRACE( "iface %p, riid %s, out %p.\n", iface, debugstr_guid( riid ), out );

    if (!out) return E_POINTER;
    *out = NULL;

    if (IsEqualGUID( riid, &IID_IUnknown ) ||
        IsEqualGUID( riid, &IID_IInspectable ) ||
        IsEqualGUID( riid, &IID_IAgileObject ) ||
        IsEqualGUID( riid, &IID_ITaskbarManager ))
    {
        *out = &impl->IInspectable_iface;
        InterlockedIncrement( &impl->ref );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( riid ) );
    return E_NOINTERFACE;
}

static ULONG WINAPI taskbar_manager_AddRef( void *iface )
{
    struct taskbar_manager *impl = impl_from_ITaskbarManager( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI taskbar_manager_Release( void *iface )
{
    struct taskbar_manager *impl = impl_from_ITaskbarManager( iface );
    return InterlockedDecrement( &impl->ref );
}

static HRESULT WINAPI taskbar_manager_GetIids( void *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI taskbar_manager_GetRuntimeClassName( void *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.UI.Shell.TaskbarManager", 31, class_name );
}

static HRESULT WINAPI taskbar_manager_GetTrustLevel( void *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI taskbar_manager_get_IsSupported( void *This, boolean *value )
{
    TRACE( "This %p, value %p\n", This, value );
    if (!value) return E_POINTER;
    *value = FALSE;
    return S_OK;
}

static HRESULT WINAPI taskbar_manager_get_IsPinningAllowed( void *This, boolean *value )
{
    TRACE( "This %p, value %p\n", This, value );
    if (!value) return E_POINTER;
    *value = FALSE;
    return S_OK;
}

static HRESULT WINAPI taskbar_manager_IsCurrentAppPinnedAsync( void *This, void **operation )
{
    TRACE( "This %p, operation %p\n", This, operation );
    return async_bool_create( FALSE, S_OK, Completed, operation );
}

static HRESULT WINAPI taskbar_manager_IsAppListEntryPinnedAsync( void *This, void *appListEntry, void **operation )
{
    TRACE( "This %p, appListEntry %p, operation %p\n", This, appListEntry, operation );
    return async_bool_create( FALSE, S_OK, Completed, operation );
}

static HRESULT WINAPI taskbar_manager_RequestPinCurrentAppAsync( void *This, void **operation )
{
    TRACE( "This %p, operation %p\n", This, operation );
    return async_bool_create( TRUE, S_OK, Completed, operation );
}

static HRESULT WINAPI taskbar_manager_RequestPinAppListEntryAsync( void *This, void *appListEntry, void **operation )
{
    TRACE( "This %p, appListEntry %p, operation %p\n", This, appListEntry, operation );
    return async_bool_create( TRUE, S_OK, Completed, operation );
}

static const void *taskbar_manager_vtbl[] =
{
    (void *)taskbar_manager_QueryInterface,
    (void *)taskbar_manager_AddRef,
    (void *)taskbar_manager_Release,
    (void *)taskbar_manager_GetIids,
    (void *)taskbar_manager_GetRuntimeClassName,
    (void *)taskbar_manager_GetTrustLevel,
    (void *)taskbar_manager_get_IsSupported,
    (void *)taskbar_manager_get_IsPinningAllowed,
    (void *)taskbar_manager_IsCurrentAppPinnedAsync,
    (void *)taskbar_manager_IsAppListEntryPinnedAsync,
    (void *)taskbar_manager_RequestPinCurrentAppAsync,
    (void *)taskbar_manager_RequestPinAppListEntryAsync
};

static struct taskbar_manager taskbar_manager_instance =
{
    {(void *)taskbar_manager_vtbl},
    1
};

struct taskbar_manager_factory
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } ITaskbarManagerStatics_iface;
    LONG ref;
};

static inline struct taskbar_manager_factory *impl_from_taskbar_factory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct taskbar_manager_factory, IActivationFactory_iface );
}

static inline struct taskbar_manager_factory *impl_from_ITaskbarManagerStatics( void *iface )
{
    return CONTAINING_RECORD( iface, struct taskbar_manager_factory, ITaskbarManagerStatics_iface );
}

static HRESULT WINAPI taskbar_factory_QueryInterface( IActivationFactory *iface, REFIID riid, void **out )
{
    struct taskbar_manager_factory *impl = impl_from_taskbar_factory( iface );
    TRACE( "iface %p, riid %s, out %p.\n", iface, debugstr_guid( riid ), out );

    if (!out) return E_POINTER;
    *out = NULL;

    if (IsEqualGUID( riid, &IID_IUnknown ) ||
        IsEqualGUID( riid, &IID_IInspectable ) ||
        IsEqualGUID( riid, &IID_IAgileObject ) ||
        IsEqualGUID( riid, &IID_IActivationFactory ))
    {
        *out = &impl->IActivationFactory_iface;
        IActivationFactory_AddRef( iface );
        return S_OK;
    }

    if (IsEqualGUID( riid, &IID_ITaskbarManagerStatics ))
    {
        *out = &impl->ITaskbarManagerStatics_iface;
        IActivationFactory_AddRef( iface );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( riid ) );
    return E_NOINTERFACE;
}

static ULONG WINAPI taskbar_factory_AddRef( IActivationFactory *iface )
{
    struct taskbar_manager_factory *impl = impl_from_taskbar_factory( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI taskbar_factory_Release( IActivationFactory *iface )
{
    struct taskbar_manager_factory *impl = impl_from_taskbar_factory( iface );
    return InterlockedDecrement( &impl->ref );
}

static HRESULT WINAPI taskbar_factory_GetIids( IActivationFactory *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI taskbar_factory_GetRuntimeClassName( IActivationFactory *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.UI.Shell.TaskbarManager", 31, class_name );
}

static HRESULT WINAPI taskbar_factory_GetTrustLevel( IActivationFactory *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI taskbar_factory_ActivateInstance( IActivationFactory *iface, IInspectable **instance )
{
    FIXME( "iface %p, instance %p stub!\n", iface, instance );
    return E_NOTIMPL;
}

static const struct IActivationFactoryVtbl taskbar_activation_factory_vtbl =
{
    taskbar_factory_QueryInterface,
    taskbar_factory_AddRef,
    taskbar_factory_Release,
    taskbar_factory_GetIids,
    taskbar_factory_GetRuntimeClassName,
    taskbar_factory_GetTrustLevel,
    taskbar_factory_ActivateInstance
};

static HRESULT WINAPI taskbar_statics_QueryInterface( void *iface, REFIID riid, void **out )
{
    struct taskbar_manager_factory *impl = impl_from_ITaskbarManagerStatics( iface );
    return taskbar_factory_QueryInterface( &impl->IActivationFactory_iface, riid, out );
}

static ULONG WINAPI taskbar_statics_AddRef( void *iface )
{
    struct taskbar_manager_factory *impl = impl_from_ITaskbarManagerStatics( iface );
    return taskbar_factory_AddRef( &impl->IActivationFactory_iface );
}

static ULONG WINAPI taskbar_statics_Release( void *iface )
{
    struct taskbar_manager_factory *impl = impl_from_ITaskbarManagerStatics( iface );
    return taskbar_factory_Release( &impl->IActivationFactory_iface );
}

static HRESULT WINAPI taskbar_statics_GetIids( void *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI taskbar_statics_GetRuntimeClassName( void *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.UI.Shell.TaskbarManager", 31, class_name );
}

static HRESULT WINAPI taskbar_statics_GetTrustLevel( void *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI taskbar_statics_GetDefault( void *iface, void **result )
{
    TRACE( "iface %p, result %p\n", iface, result );
    if (!result) return E_POINTER;
    *result = &taskbar_manager_instance.IInspectable_iface;
    taskbar_manager_AddRef( *result );
    return S_OK;
}

static const void *taskbar_statics_vtbl[] =
{
    (void *)taskbar_statics_QueryInterface,
    (void *)taskbar_statics_AddRef,
    (void *)taskbar_statics_Release,
    (void *)taskbar_statics_GetIids,
    (void *)taskbar_statics_GetRuntimeClassName,
    (void *)taskbar_statics_GetTrustLevel,
    (void *)taskbar_statics_GetDefault
};

static struct taskbar_manager_factory taskbar_manager_factory_instance =
{
    {&taskbar_activation_factory_vtbl},
    {taskbar_statics_vtbl},
    1
};

IActivationFactory *taskbar_manager_factory = &taskbar_manager_factory_instance.IActivationFactory_iface;
