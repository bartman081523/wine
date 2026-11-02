/*
 * WinRT Windows.Storage.StorageFile Implementation
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
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(storage);

/* IStorageFileStatics: GUID {5984c710-daf2-43c8-8bb4-a4d3eacfd03f} */
static const GUID IID_IStorageFileStatics = {0x5984c710, 0xdaf2, 0x43c8, {0x8b, 0xb4, 0xa4, 0xd3, 0xea, 0xcf, 0xd0, 0x3f}};

typedef struct IStorageFileStatics IStorageFileStatics;

typedef struct IStorageFileStaticsVtbl
{
    /* IUnknown */
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IStorageFileStatics *This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IStorageFileStatics *This);
    ULONG (STDMETHODCALLTYPE *Release)(IStorageFileStatics *This);
    /* IInspectable */
    HRESULT (STDMETHODCALLTYPE *GetIids)(IStorageFileStatics *This, ULONG *iidCount, IID **iids);
    HRESULT (STDMETHODCALLTYPE *GetRuntimeClassName)(IStorageFileStatics *This, HSTRING *className);
    HRESULT (STDMETHODCALLTYPE *GetTrustLevel)(IStorageFileStatics *This, TrustLevel *trustLevel);
    /* IStorageFileStatics */
    HRESULT (STDMETHODCALLTYPE *GetFileFromPathAsync)(IStorageFileStatics *This, HSTRING path,
        __FIAsyncOperation_1_Windows__CStorage__CStorageFile **operation);
    HRESULT (STDMETHODCALLTYPE *GetFileFromApplicationUriAsync)(IStorageFileStatics *This, IUriRuntimeClass *uri,
        __FIAsyncOperation_1_Windows__CStorage__CStorageFile **operation);
    HRESULT (STDMETHODCALLTYPE *CreateStreamedFileAsync)(IStorageFileStatics *This, HSTRING displayNameWithExtension,
        void *dataRequested, void *thumbnail, __FIAsyncOperation_1_Windows__CStorage__CStorageFile **operation);
    HRESULT (STDMETHODCALLTYPE *ReplaceWithStreamedFileAsync)(IStorageFileStatics *This, IStorageFile *fileToReplace,
        void *dataRequested, void *thumbnail, __FIAsyncOperation_1_Windows__CStorage__CStorageFile **operation);
    HRESULT (STDMETHODCALLTYPE *CreateStreamedFileFromUriAsync)(IStorageFileStatics *This, HSTRING displayNameWithExtension,
        IUriRuntimeClass *uri, void *thumbnail, __FIAsyncOperation_1_Windows__CStorage__CStorageFile **operation);
} IStorageFileStaticsVtbl;

struct IStorageFileStatics
{
    const IStorageFileStaticsVtbl *lpVtbl;
};

/* Forward declarations */
static HRESULT storage_file_create( const WCHAR *path, IStorageFile **out );
static HRESULT async_storage_file_create( IStorageFile *file, __FIAsyncOperation_1_Windows__CStorage__CStorageFile **out );
static HRESULT async_random_access_stream_create( IRandomAccessStream *stream,
    __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream **out );

/*
 * AsyncOperation<StorageFile*>
 */

struct async_storage_file
{
    __FIAsyncOperation_1_Windows__CStorage__CStorageFile IAsyncOperation_StorageFile_iface;
    IAsyncInfo IAsyncInfo_iface;
    LONG ref;
    IStorageFile *result;
    __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStorageFile *handler;
};

static inline struct async_storage_file *impl_from_IAsyncOperation_StorageFile( __FIAsyncOperation_1_Windows__CStorage__CStorageFile *iface )
{
    return CONTAINING_RECORD( iface, struct async_storage_file, IAsyncOperation_StorageFile_iface );
}

static inline struct async_storage_file *impl_from_async_storage_file_IAsyncInfo( IAsyncInfo *iface )
{
    return CONTAINING_RECORD( iface, struct async_storage_file, IAsyncInfo_iface );
}

static HRESULT WINAPI async_storage_file_QueryInterface( __FIAsyncOperation_1_Windows__CStorage__CStorageFile *iface, REFIID iid, void **out )
{
    struct async_storage_file *impl = impl_from_IAsyncOperation_StorageFile( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAsyncOperation_StorageFile ))
    {
        *out = &impl->IAsyncOperation_StorageFile_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IAsyncInfo ))
    {
        *out = &impl->IAsyncInfo_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI async_storage_file_AddRef( __FIAsyncOperation_1_Windows__CStorage__CStorageFile *iface )
{
    struct async_storage_file *impl = impl_from_IAsyncOperation_StorageFile( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI async_storage_file_Release( __FIAsyncOperation_1_Windows__CStorage__CStorageFile *iface )
{
    struct async_storage_file *impl = impl_from_IAsyncOperation_StorageFile( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );
    if (!ref)
    {
        if (impl->result) IStorageFile_Release( impl->result );
        if (impl->handler) __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStorageFile_Release( impl->handler );
        free( impl );
    }
    return ref;
}

static HRESULT WINAPI async_storage_file_GetIids( __FIAsyncOperation_1_Windows__CStorage__CStorageFile *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI async_storage_file_GetRuntimeClassName( __FIAsyncOperation_1_Windows__CStorage__CStorageFile *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.IAsyncOperation`1<Windows.Storage.StorageFile>",
                                ARRAY_SIZE(L"Windows.Foundation.IAsyncOperation`1<Windows.Storage.StorageFile>") - 1,
                                class_name );
}

static HRESULT WINAPI async_storage_file_GetTrustLevel( __FIAsyncOperation_1_Windows__CStorage__CStorageFile *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI async_storage_file_put_Completed( __FIAsyncOperation_1_Windows__CStorage__CStorageFile *iface,
    __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStorageFile *handler )
{
    struct async_storage_file *impl = impl_from_IAsyncOperation_StorageFile( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );
    impl->handler = handler;
    if (handler)
    {
        __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStorageFile_AddRef( handler );
        __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStorageFile_Invoke( handler, iface, Completed );
    }
    return S_OK;
}

static HRESULT WINAPI async_storage_file_get_Completed( __FIAsyncOperation_1_Windows__CStorage__CStorageFile *iface,
    __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStorageFile **handler )
{
    struct async_storage_file *impl = impl_from_IAsyncOperation_StorageFile( iface );
    if (!handler) return E_INVALIDARG;
    *handler = impl->handler;
    if (*handler) __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStorageFile_AddRef( *handler );
    return S_OK;
}

static HRESULT WINAPI async_storage_file_GetResults( __FIAsyncOperation_1_Windows__CStorage__CStorageFile *iface,
    IStorageFile **results )
{
    struct async_storage_file *impl = impl_from_IAsyncOperation_StorageFile( iface );
    TRACE( "iface %p, results %p.\n", iface, results );
    if (!results) return E_INVALIDARG;
    *results = impl->result;
    if (*results) IStorageFile_AddRef( *results );
    return S_OK;
}

static const struct __FIAsyncOperation_1_Windows__CStorage__CStorageFileVtbl async_storage_file_vtbl =
{
    async_storage_file_QueryInterface,
    async_storage_file_AddRef,
    async_storage_file_Release,
    /* IInspectable methods */
    async_storage_file_GetIids,
    async_storage_file_GetRuntimeClassName,
    async_storage_file_GetTrustLevel,
    /* IAsyncOperation<StorageFile*> methods */
    async_storage_file_put_Completed,
    async_storage_file_get_Completed,
    async_storage_file_GetResults,
};

static HRESULT WINAPI async_storage_file_info_QueryInterface( IAsyncInfo *iface, REFIID iid, void **out )
{
    struct async_storage_file *impl = impl_from_async_storage_file_IAsyncInfo( iface );
    return async_storage_file_QueryInterface( &impl->IAsyncOperation_StorageFile_iface, iid, out );
}

static ULONG WINAPI async_storage_file_info_AddRef( IAsyncInfo *iface )
{
    struct async_storage_file *impl = impl_from_async_storage_file_IAsyncInfo( iface );
    return async_storage_file_AddRef( &impl->IAsyncOperation_StorageFile_iface );
}

static ULONG WINAPI async_storage_file_info_Release( IAsyncInfo *iface )
{
    struct async_storage_file *impl = impl_from_async_storage_file_IAsyncInfo( iface );
    return async_storage_file_Release( &impl->IAsyncOperation_StorageFile_iface );
}

static HRESULT WINAPI async_storage_file_info_GetIids( IAsyncInfo *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI async_storage_file_info_GetRuntimeClassName( IAsyncInfo *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.IAsyncInfo", ARRAY_SIZE(L"Windows.Foundation.IAsyncInfo") - 1, class_name );
}

static HRESULT WINAPI async_storage_file_info_GetTrustLevel( IAsyncInfo *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI async_storage_file_info_get_Id( IAsyncInfo *iface, UINT32 *id )
{
    if (!id) return E_INVALIDARG;
    *id = 1;
    return S_OK;
}

static HRESULT WINAPI async_storage_file_info_get_Status( IAsyncInfo *iface, AsyncStatus *status )
{
    if (!status) return E_INVALIDARG;
    *status = Completed;
    return S_OK;
}

static HRESULT WINAPI async_storage_file_info_get_ErrorCode( IAsyncInfo *iface, HRESULT *error_code )
{
    if (!error_code) return E_INVALIDARG;
    *error_code = S_OK;
    return S_OK;
}

static HRESULT WINAPI async_storage_file_info_Cancel( IAsyncInfo *iface )
{
    return S_OK;
}

static HRESULT WINAPI async_storage_file_info_Close( IAsyncInfo *iface )
{
    return S_OK;
}

static const struct IAsyncInfoVtbl async_storage_file_info_vtbl =
{
    async_storage_file_info_QueryInterface,
    async_storage_file_info_AddRef,
    async_storage_file_info_Release,
    /* IInspectable methods */
    async_storage_file_info_GetIids,
    async_storage_file_info_GetRuntimeClassName,
    async_storage_file_info_GetTrustLevel,
    /* IAsyncInfo methods */
    async_storage_file_info_get_Id,
    async_storage_file_info_get_Status,
    async_storage_file_info_get_ErrorCode,
    async_storage_file_info_Cancel,
    async_storage_file_info_Close,
};

static HRESULT async_storage_file_create( IStorageFile *file, __FIAsyncOperation_1_Windows__CStorage__CStorageFile **out )
{
    struct async_storage_file *impl;

    if (!out) return E_INVALIDARG;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;

    impl->IAsyncOperation_StorageFile_iface.lpVtbl = &async_storage_file_vtbl;
    impl->IAsyncInfo_iface.lpVtbl = &async_storage_file_info_vtbl;
    impl->ref = 1;
    impl->result = file;
    if (file) IStorageFile_AddRef( file );

    *out = &impl->IAsyncOperation_StorageFile_iface;
    return S_OK;
}

/*
 * AsyncOperation<IRandomAccessStream*>
 */

struct async_stream
{
    __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream IAsyncOperation_IRandomAccessStream_iface;
    IAsyncInfo IAsyncInfo_iface;
    LONG ref;
    IRandomAccessStream *result;
    __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIRandomAccessStream *handler;
};

static inline struct async_stream *impl_from_IAsyncOperation_IRandomAccessStream( __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream *iface )
{
    return CONTAINING_RECORD( iface, struct async_stream, IAsyncOperation_IRandomAccessStream_iface );
}

static inline struct async_stream *impl_from_async_stream_IAsyncInfo( IAsyncInfo *iface )
{
    return CONTAINING_RECORD( iface, struct async_stream, IAsyncInfo_iface );
}

static HRESULT WINAPI async_stream_QueryInterface( __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream *iface, REFIID iid, void **out )
{
    struct async_stream *impl = impl_from_IAsyncOperation_IRandomAccessStream( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAsyncOperation_IRandomAccessStream ) ||
        IsEqualGUID( iid, &IID_IAsyncOperation_IRandomAccessStreamWithContentType ))
    {
        *out = &impl->IAsyncOperation_IRandomAccessStream_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IAsyncInfo ))
    {
        *out = &impl->IAsyncInfo_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI async_stream_AddRef( __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream *iface )
{
    struct async_stream *impl = impl_from_IAsyncOperation_IRandomAccessStream( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI async_stream_Release( __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream *iface )
{
    struct async_stream *impl = impl_from_IAsyncOperation_IRandomAccessStream( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );
    if (!ref)
    {
        if (impl->result) IRandomAccessStream_Release( impl->result );
        if (impl->handler) __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIRandomAccessStream_Release( impl->handler );
        free( impl );
    }
    return ref;
}

static HRESULT WINAPI async_stream_GetIids( __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI async_stream_GetRuntimeClassName( __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.IAsyncOperation`1<Windows.Storage.Streams.IRandomAccessStream>",
                                ARRAY_SIZE(L"Windows.Foundation.IAsyncOperation`1<Windows.Storage.Streams.IRandomAccessStream>") - 1,
                                class_name );
}

static HRESULT WINAPI async_stream_GetTrustLevel( __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI async_stream_put_Completed( __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream *iface,
    __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIRandomAccessStream *handler )
{
    struct async_stream *impl = impl_from_IAsyncOperation_IRandomAccessStream( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );
    impl->handler = handler;
    if (handler)
    {
        __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIRandomAccessStream_AddRef( handler );
        __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIRandomAccessStream_Invoke( handler, iface, Completed );
    }
    return S_OK;
}

static HRESULT WINAPI async_stream_get_Completed( __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream *iface,
    __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIRandomAccessStream **handler )
{
    struct async_stream *impl = impl_from_IAsyncOperation_IRandomAccessStream( iface );
    if (!handler) return E_INVALIDARG;
    *handler = impl->handler;
    if (*handler) __FIAsyncOperationCompletedHandler_1_Windows__CStorage__CStreams__CIRandomAccessStream_AddRef( *handler );
    return S_OK;
}

static HRESULT WINAPI async_stream_GetResults( __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream *iface,
    IRandomAccessStream **results )
{
    struct async_stream *impl = impl_from_IAsyncOperation_IRandomAccessStream( iface );
    TRACE( "iface %p, results %p.\n", iface, results );
    if (!results) return E_INVALIDARG;
    *results = impl->result;
    if (*results) IRandomAccessStream_AddRef( *results );
    return S_OK;
}

static const struct __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStreamVtbl async_stream_vtbl =
{
    async_stream_QueryInterface,
    async_stream_AddRef,
    async_stream_Release,
    /* IInspectable methods */
    async_stream_GetIids,
    async_stream_GetRuntimeClassName,
    async_stream_GetTrustLevel,
    /* IAsyncOperation<IRandomAccessStream*> methods */
    async_stream_put_Completed,
    async_stream_get_Completed,
    async_stream_GetResults,
};

static HRESULT WINAPI async_stream_info_QueryInterface( IAsyncInfo *iface, REFIID iid, void **out )
{
    struct async_stream *impl = impl_from_async_stream_IAsyncInfo( iface );
    return async_stream_QueryInterface( &impl->IAsyncOperation_IRandomAccessStream_iface, iid, out );
}

static ULONG WINAPI async_stream_info_AddRef( IAsyncInfo *iface )
{
    struct async_stream *impl = impl_from_async_stream_IAsyncInfo( iface );
    return async_stream_AddRef( &impl->IAsyncOperation_IRandomAccessStream_iface );
}

static ULONG WINAPI async_stream_info_Release( IAsyncInfo *iface )
{
    struct async_stream *impl = impl_from_async_stream_IAsyncInfo( iface );
    return async_stream_Release( &impl->IAsyncOperation_IRandomAccessStream_iface );
}

static HRESULT WINAPI async_stream_info_GetIids( IAsyncInfo *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI async_stream_info_GetRuntimeClassName( IAsyncInfo *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.IAsyncInfo", ARRAY_SIZE(L"Windows.Foundation.IAsyncInfo") - 1, class_name );
}

static HRESULT WINAPI async_stream_info_GetTrustLevel( IAsyncInfo *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI async_stream_info_get_Id( IAsyncInfo *iface, UINT32 *id )
{
    if (!id) return E_INVALIDARG;
    *id = 1;
    return S_OK;
}

static HRESULT WINAPI async_stream_info_get_Status( IAsyncInfo *iface, AsyncStatus *status )
{
    if (!status) return E_INVALIDARG;
    *status = Completed;
    return S_OK;
}

static HRESULT WINAPI async_stream_info_get_ErrorCode( IAsyncInfo *iface, HRESULT *error_code )
{
    if (!error_code) return E_INVALIDARG;
    *error_code = S_OK;
    return S_OK;
}

static HRESULT WINAPI async_stream_info_Cancel( IAsyncInfo *iface )
{
    return S_OK;
}

static HRESULT WINAPI async_stream_info_Close( IAsyncInfo *iface )
{
    return S_OK;
}

static const struct IAsyncInfoVtbl async_stream_info_vtbl =
{
    async_stream_info_QueryInterface,
    async_stream_info_AddRef,
    async_stream_info_Release,
    /* IInspectable methods */
    async_stream_info_GetIids,
    async_stream_info_GetRuntimeClassName,
    async_stream_info_GetTrustLevel,
    /* IAsyncInfo methods */
    async_stream_info_get_Id,
    async_stream_info_get_Status,
    async_stream_info_get_ErrorCode,
    async_stream_info_Cancel,
    async_stream_info_Close,
};

static HRESULT async_random_access_stream_create( IRandomAccessStream *stream,
    __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream **out )
{
    struct async_stream *impl;

    if (!out) return E_INVALIDARG;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;

    impl->IAsyncOperation_IRandomAccessStream_iface.lpVtbl = &async_stream_vtbl;
    impl->IAsyncInfo_iface.lpVtbl = &async_stream_info_vtbl;
    impl->ref = 1;
    impl->result = stream;
    if (stream) IRandomAccessStream_AddRef( stream );

    *out = &impl->IAsyncOperation_IRandomAccessStream_iface;
    return S_OK;
}

/*
 * StorageFile
 */

struct storage_file
{
    IStorageFile IStorageFile_iface;
    IStorageItem IStorageItem_iface;
    IRandomAccessStreamReference IRandomAccessStreamReference_iface;
    IInputStreamReference IInputStreamReference_iface;
    LONG ref;
    WCHAR path[MAX_PATH];
    WCHAR name[MAX_PATH];
    WCHAR file_type[32];
    WCHAR content_type[64];
};

static inline struct storage_file *impl_from_IStorageFile( IStorageFile *iface )
{
    return CONTAINING_RECORD( iface, struct storage_file, IStorageFile_iface );
}

static inline struct storage_file *impl_from_IStorageItem( IStorageItem *iface )
{
    return CONTAINING_RECORD( iface, struct storage_file, IStorageItem_iface );
}

static inline struct storage_file *impl_from_IRandomAccessStreamReference( IRandomAccessStreamReference *iface )
{
    return CONTAINING_RECORD( iface, struct storage_file, IRandomAccessStreamReference_iface );
}

static inline struct storage_file *impl_from_IInputStreamReference( IInputStreamReference *iface )
{
    return CONTAINING_RECORD( iface, struct storage_file, IInputStreamReference_iface );
}

static HRESULT WINAPI storage_file_QueryInterface( IStorageFile *iface, REFIID iid, void **out )
{
    struct storage_file *impl = impl_from_IStorageFile( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IStorageFile ))
    {
        *out = &impl->IStorageFile_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IStorageItem ))
    {
        *out = &impl->IStorageItem_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IRandomAccessStreamReference ))
    {
        *out = &impl->IRandomAccessStreamReference_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IInputStreamReference ))
    {
        *out = &impl->IInputStreamReference_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI storage_file_AddRef( IStorageFile *iface )
{
    struct storage_file *impl = impl_from_IStorageFile( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI storage_file_Release( IStorageFile *iface )
{
    struct storage_file *impl = impl_from_IStorageFile( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );
    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI storage_file_GetIids( IStorageFile *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_GetRuntimeClassName( IStorageFile *iface, HSTRING *class_name )
{
    return WindowsCreateString( RuntimeClass_Windows_Storage_StorageFile,
                                ARRAY_SIZE(RuntimeClass_Windows_Storage_StorageFile) - 1,
                                class_name );
}

static HRESULT WINAPI storage_file_GetTrustLevel( IStorageFile *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_get_FileType( IStorageFile *iface, HSTRING *value )
{
    struct storage_file *impl = impl_from_IStorageFile( iface );
    TRACE( "iface %p, value %p -> %s\n", iface, value, debugstr_w(impl->file_type) );
    if (!value) return E_INVALIDARG;
    return WindowsCreateString( impl->file_type, wcslen(impl->file_type), value );
}

static HRESULT WINAPI storage_file_get_ContentType( IStorageFile *iface, HSTRING *value )
{
    struct storage_file *impl = impl_from_IStorageFile( iface );
    TRACE( "iface %p, value %p -> %s\n", iface, value, debugstr_w(impl->content_type) );
    if (!value) return E_INVALIDARG;
    return WindowsCreateString( impl->content_type, wcslen(impl->content_type), value );
}

static HRESULT WINAPI storage_file_OpenAsync( IStorageFile *iface, FileAccessMode mode,
                                              __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStream **operation )
{
    struct storage_file *impl = impl_from_IStorageFile( iface );
    IRandomAccessStream *stream = NULL;
    HRESULT hr;

    TRACE( "iface %p, mode %d, operation %p.\n", iface, mode, operation );

    if (!operation) return E_INVALIDARG;
    hr = memory_stream_from_file( impl->path, impl->content_type, &stream );
    if (FAILED(hr)) return hr;

    return async_random_access_stream_create( stream, operation );
}

static HRESULT WINAPI storage_file_OpenTransactedWriteAsync( IStorageFile *iface,
    __FIAsyncOperation_1_Windows__CStorage__CStorageStreamTransaction **operation )
{
    FIXME( "iface %p, operation %p stub!\n", iface, operation );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_CopyOverloadDefaultNameAndOptions( IStorageFile *iface, IStorageFolder *folder,
    __FIAsyncOperation_1_Windows__CStorage__CStorageFile **operation )
{
    FIXME( "iface %p stub!\n", iface );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_CopyOverloadDefaultOptions( IStorageFile *iface, IStorageFolder *folder,
    HSTRING name, __FIAsyncOperation_1_Windows__CStorage__CStorageFile **operation )
{
    FIXME( "iface %p stub!\n", iface );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_CopyOverload( IStorageFile *iface, IStorageFolder *folder,
    HSTRING name, NameCollisionOption option, __FIAsyncOperation_1_Windows__CStorage__CStorageFile **operation )
{
    FIXME( "iface %p stub!\n", iface );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_CopyAndReplaceAsync( IStorageFile *iface, IStorageFile *file,
    IAsyncAction **operation )
{
    FIXME( "iface %p stub!\n", iface );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_MoveOverloadDefaultNameAndOptions( IStorageFile *iface, IStorageFolder *folder,
    IAsyncAction **operation )
{
    FIXME( "iface %p stub!\n", iface );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_MoveOverloadDefaultOptions( IStorageFile *iface, IStorageFolder *folder,
    HSTRING name, IAsyncAction **operation )
{
    FIXME( "iface %p stub!\n", iface );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_MoveOverload( IStorageFile *iface, IStorageFolder *folder,
    HSTRING name, NameCollisionOption option, IAsyncAction **operation )
{
    FIXME( "iface %p stub!\n", iface );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_MoveAndReplaceAsync( IStorageFile *iface, IStorageFile *file,
    IAsyncAction **operation )
{
    FIXME( "iface %p stub!\n", iface );
    return E_NOTIMPL;
}

static const struct __x_ABI_CWindows_CStorage_CIStorageFileVtbl storage_file_vtbl =
{
    storage_file_QueryInterface,
    storage_file_AddRef,
    storage_file_Release,
    /* IInspectable methods */
    storage_file_GetIids,
    storage_file_GetRuntimeClassName,
    storage_file_GetTrustLevel,
    /* IStorageFile methods */
    storage_file_get_FileType,
    storage_file_get_ContentType,
    storage_file_OpenAsync,
    storage_file_OpenTransactedWriteAsync,
    storage_file_CopyOverloadDefaultNameAndOptions,
    storage_file_CopyOverloadDefaultOptions,
    storage_file_CopyOverload,
    storage_file_CopyAndReplaceAsync,
    storage_file_MoveOverloadDefaultNameAndOptions,
    storage_file_MoveOverloadDefaultOptions,
    storage_file_MoveOverload,
    storage_file_MoveAndReplaceAsync,
};

/*
 * StorageItem methods
 */
static HRESULT WINAPI storage_file_item_QueryInterface( IStorageItem *iface, REFIID iid, void **out )
{
    struct storage_file *impl = impl_from_IStorageItem( iface );
    return storage_file_QueryInterface( &impl->IStorageFile_iface, iid, out );
}

static ULONG WINAPI storage_file_item_AddRef( IStorageItem *iface )
{
    struct storage_file *impl = impl_from_IStorageItem( iface );
    return storage_file_AddRef( &impl->IStorageFile_iface );
}

static ULONG WINAPI storage_file_item_Release( IStorageItem *iface )
{
    struct storage_file *impl = impl_from_IStorageItem( iface );
    return storage_file_Release( &impl->IStorageFile_iface );
}

static HRESULT WINAPI storage_file_item_GetIids( IStorageItem *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_item_GetRuntimeClassName( IStorageItem *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Storage.IStorageItem", ARRAY_SIZE(L"Windows.Storage.IStorageItem") - 1, class_name );
}

static HRESULT WINAPI storage_file_item_GetTrustLevel( IStorageItem *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_item_RenameAsyncOverloadDefaultOptions( IStorageItem *iface, HSTRING name, IAsyncAction **operation )
{
    FIXME( "iface %p stub!\n", iface );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_item_RenameAsync( IStorageItem *iface, HSTRING name, NameCollisionOption option, IAsyncAction **operation )
{
    FIXME( "iface %p stub!\n", iface );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_item_DeleteAsyncOverloadDefaultOptions( IStorageItem *iface, IAsyncAction **operation )
{
    FIXME( "iface %p stub!\n", iface );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_item_DeleteAsync( IStorageItem *iface, StorageDeleteOption option, IAsyncAction **operation )
{
    FIXME( "iface %p stub!\n", iface );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_item_GetBasicPropertiesAsync( IStorageItem *iface,
    __FIAsyncOperation_1_Windows__CStorage__CFileProperties__CBasicProperties **operation )
{
    FIXME( "iface %p stub!\n", iface );
    if (operation) *operation = NULL;
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_item_get_Name( IStorageItem *iface, HSTRING *value )
{
    struct storage_file *impl = impl_from_IStorageItem( iface );
    TRACE( "iface %p, value %p -> %s\n", iface, value, debugstr_w(impl->name) );
    if (!value) return E_INVALIDARG;
    return WindowsCreateString( impl->name, wcslen(impl->name), value );
}

static HRESULT WINAPI storage_file_item_get_Path( IStorageItem *iface, HSTRING *value )
{
    struct storage_file *impl = impl_from_IStorageItem( iface );
    TRACE( "iface %p, value %p -> %s\n", iface, value, debugstr_w(impl->path) );
    if (!value) return E_INVALIDARG;
    return WindowsCreateString( impl->path, wcslen(impl->path), value );
}

static HRESULT WINAPI storage_file_item_get_Attributes( IStorageItem *iface, FileAttributes *value )
{
    struct storage_file *impl = impl_from_IStorageItem( iface );
    DWORD attr;
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_INVALIDARG;
    attr = GetFileAttributesW( impl->path );
    *value = (attr == INVALID_FILE_ATTRIBUTES) ? FILE_ATTRIBUTE_NORMAL : (FileAttributes)attr;
    return S_OK;
}

static HRESULT WINAPI storage_file_item_get_DateCreated( IStorageItem *iface, DateTime *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_INVALIDARG;
    memset( value, 0, sizeof(*value) );
    return S_OK;
}

static HRESULT WINAPI storage_file_item_IsOfType( IStorageItem *iface, StorageItemTypes type, boolean *value )
{
    TRACE( "iface %p, type %d, value %p\n", iface, type, value );
    if (!value) return E_INVALIDARG;
    *value = (type == StorageItemTypes_File);
    return S_OK;
}

static const struct __x_ABI_CWindows_CStorage_CIStorageItemVtbl storage_file_item_vtbl =
{
    storage_file_item_QueryInterface,
    storage_file_item_AddRef,
    storage_file_item_Release,
    /* IInspectable methods */
    storage_file_item_GetIids,
    storage_file_item_GetRuntimeClassName,
    storage_file_item_GetTrustLevel,
    /* IStorageItem methods */
    storage_file_item_RenameAsyncOverloadDefaultOptions,
    storage_file_item_RenameAsync,
    storage_file_item_DeleteAsyncOverloadDefaultOptions,
    storage_file_item_DeleteAsync,
    storage_file_item_GetBasicPropertiesAsync,
    storage_file_item_get_Name,
    storage_file_item_get_Path,
    storage_file_item_get_Attributes,
    storage_file_item_get_DateCreated,
    storage_file_item_IsOfType,
};

/*
 * RandomAccessStreamReference methods
 */
static HRESULT WINAPI storage_file_stream_ref_QueryInterface( IRandomAccessStreamReference *iface, REFIID iid, void **out )
{
    struct storage_file *impl = impl_from_IRandomAccessStreamReference( iface );
    return storage_file_QueryInterface( &impl->IStorageFile_iface, iid, out );
}

static ULONG WINAPI storage_file_stream_ref_AddRef( IRandomAccessStreamReference *iface )
{
    struct storage_file *impl = impl_from_IRandomAccessStreamReference( iface );
    return storage_file_AddRef( &impl->IStorageFile_iface );
}

static ULONG WINAPI storage_file_stream_ref_Release( IRandomAccessStreamReference *iface )
{
    struct storage_file *impl = impl_from_IRandomAccessStreamReference( iface );
    return storage_file_Release( &impl->IStorageFile_iface );
}

static HRESULT WINAPI storage_file_stream_ref_GetIids( IRandomAccessStreamReference *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_stream_ref_GetRuntimeClassName( IRandomAccessStreamReference *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Storage.Streams.IRandomAccessStreamReference",
                                ARRAY_SIZE(L"Windows.Storage.Streams.IRandomAccessStreamReference") - 1,
                                class_name );
}

static HRESULT WINAPI storage_file_stream_ref_GetTrustLevel( IRandomAccessStreamReference *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_stream_ref_OpenReadAsync( IRandomAccessStreamReference *iface,
    __FIAsyncOperation_1_Windows__CStorage__CStreams__CIRandomAccessStreamWithContentType **operation )
{
    struct storage_file *impl = impl_from_IRandomAccessStreamReference( iface );
    IRandomAccessStream *stream = NULL;
    HRESULT hr;

    TRACE( "iface %p, operation %p.\n", iface, operation );

    if (!operation) return E_INVALIDARG;
    hr = memory_stream_from_file( impl->path, impl->content_type, &stream );
    if (FAILED(hr)) return hr;

    return async_random_access_stream_create( stream, (IAsyncOperation_IRandomAccessStream **)operation );
}

static const struct __x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStreamReferenceVtbl storage_file_stream_ref_vtbl =
{
    storage_file_stream_ref_QueryInterface,
    storage_file_stream_ref_AddRef,
    storage_file_stream_ref_Release,
    /* IInspectable methods */
    storage_file_stream_ref_GetIids,
    storage_file_stream_ref_GetRuntimeClassName,
    storage_file_stream_ref_GetTrustLevel,
    /* IRandomAccessStreamReference methods */
    storage_file_stream_ref_OpenReadAsync,
};

/*
 * InputStreamReference methods
 */
static HRESULT WINAPI storage_file_input_ref_QueryInterface( IInputStreamReference *iface, REFIID iid, void **out )
{
    struct storage_file *impl = impl_from_IInputStreamReference( iface );
    return storage_file_QueryInterface( &impl->IStorageFile_iface, iid, out );
}

static ULONG WINAPI storage_file_input_ref_AddRef( IInputStreamReference *iface )
{
    struct storage_file *impl = impl_from_IInputStreamReference( iface );
    return storage_file_AddRef( &impl->IStorageFile_iface );
}

static ULONG WINAPI storage_file_input_ref_Release( IInputStreamReference *iface )
{
    struct storage_file *impl = impl_from_IInputStreamReference( iface );
    return storage_file_Release( &impl->IStorageFile_iface );
}

static HRESULT WINAPI storage_file_input_ref_GetIids( IInputStreamReference *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_input_ref_GetRuntimeClassName( IInputStreamReference *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Storage.Streams.IInputStreamReference",
                                ARRAY_SIZE(L"Windows.Storage.Streams.IInputStreamReference") - 1,
                                class_name );
}

static HRESULT WINAPI storage_file_input_ref_GetTrustLevel( IInputStreamReference *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_input_ref_OpenSequentialReadAsync( IInputStreamReference *iface,
    __FIAsyncOperation_1_Windows__CStorage__CStreams__CIInputStream **operation )
{
    struct storage_file *impl = impl_from_IInputStreamReference( iface );
    IRandomAccessStream *stream = NULL;
    HRESULT hr;

    TRACE( "iface %p, operation %p.\n", iface, operation );

    if (!operation) return E_INVALIDARG;
    hr = memory_stream_from_file( impl->path, impl->content_type, &stream );
    if (FAILED(hr)) return hr;

    return async_random_access_stream_create( stream, (IAsyncOperation_IRandomAccessStream **)operation );
}

static const struct __x_ABI_CWindows_CStorage_CStreams_CIInputStreamReferenceVtbl storage_file_input_ref_vtbl =
{
    storage_file_input_ref_QueryInterface,
    storage_file_input_ref_AddRef,
    storage_file_input_ref_Release,
    /* IInspectable methods */
    storage_file_input_ref_GetIids,
    storage_file_input_ref_GetRuntimeClassName,
    storage_file_input_ref_GetTrustLevel,
    /* IInputStreamReference methods */
    storage_file_input_ref_OpenSequentialReadAsync,
};

static HRESULT storage_file_create( const WCHAR *path, IStorageFile **out )
{
    struct storage_file *impl;
    WCHAR *p, *ext;

    if (!out) return E_INVALIDARG;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;

    impl->IStorageFile_iface.lpVtbl = &storage_file_vtbl;
    impl->IStorageItem_iface.lpVtbl = &storage_file_item_vtbl;
    impl->IRandomAccessStreamReference_iface.lpVtbl = &storage_file_stream_ref_vtbl;
    impl->IInputStreamReference_iface.lpVtbl = &storage_file_input_ref_vtbl;
    impl->ref = 1;

    lstrcpynW( impl->path, path, ARRAY_SIZE(impl->path) );
    p = wcsrchr( impl->path, '\\' );
    if (p) lstrcpynW( impl->name, p + 1, ARRAY_SIZE(impl->name) );
    else lstrcpynW( impl->name, impl->path, ARRAY_SIZE(impl->name) );

    ext = wcsrchr( impl->name, '.' );
    if (ext) lstrcpynW( impl->file_type, ext, ARRAY_SIZE(impl->file_type) );
    else lstrcpynW( impl->file_type, L"", ARRAY_SIZE(impl->file_type) );

    if (!wcsicmp( impl->file_type, L".ico" ))
        lstrcpynW( impl->content_type, L"image/x-icon", ARRAY_SIZE(impl->content_type) );
    else if (!wcsicmp( impl->file_type, L".png" ))
        lstrcpynW( impl->content_type, L"image/png", ARRAY_SIZE(impl->content_type) );
    else if (!wcsicmp( impl->file_type, L".jpg" ) || !wcsicmp( impl->file_type, L".jpeg" ))
        lstrcpynW( impl->content_type, L"image/jpeg", ARRAY_SIZE(impl->content_type) );
    else
        lstrcpynW( impl->content_type, L"application/octet-stream", ARRAY_SIZE(impl->content_type) );

    *out = &impl->IStorageFile_iface;
    TRACE( "created StorageFile %p: path %s, name %s, type %s\n",
           *out, debugstr_w(impl->path), debugstr_w(impl->name), debugstr_w(impl->file_type) );
    return S_OK;
}

/*
 * StorageFileStatics (Activation Factory)
 */

struct storage_file_statics
{
    IActivationFactory IActivationFactory_iface;
    IStorageFileStatics IStorageFileStatics_iface;
    LONG ref;
};

static inline struct storage_file_statics *impl_from_IActivationFactory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct storage_file_statics, IActivationFactory_iface );
}

static inline struct storage_file_statics *impl_from_IStorageFileStatics( IStorageFileStatics *iface )
{
    return CONTAINING_RECORD( iface, struct storage_file_statics, IStorageFileStatics_iface );
}

static HRESULT WINAPI factory_QueryInterface( IActivationFactory *iface, REFIID iid, void **out )
{
    struct storage_file_statics *impl = impl_from_IActivationFactory( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IActivationFactory ))
    {
        *out = &impl->IActivationFactory_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IStorageFileStatics ))
    {
        *out = &impl->IStorageFileStatics_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI factory_AddRef( IActivationFactory *iface )
{
    struct storage_file_statics *impl = impl_from_IActivationFactory( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI factory_Release( IActivationFactory *iface )
{
    struct storage_file_statics *impl = impl_from_IActivationFactory( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );
    return ref;
}

static HRESULT WINAPI factory_GetIids( IActivationFactory *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI factory_GetRuntimeClassName( IActivationFactory *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI factory_GetTrustLevel( IActivationFactory *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI factory_ActivateInstance( IActivationFactory *iface, IInspectable **instance )
{
    FIXME( "iface %p, instance %p stub!\n", iface, instance );
    return E_NOTIMPL;
}

static const struct IActivationFactoryVtbl factory_vtbl =
{
    factory_QueryInterface,
    factory_AddRef,
    factory_Release,
    /* IInspectable methods */
    factory_GetIids,
    factory_GetRuntimeClassName,
    factory_GetTrustLevel,
    /* IActivationFactory methods */
    factory_ActivateInstance,
};

static HRESULT WINAPI storage_file_statics_QueryInterface( IStorageFileStatics *iface, REFIID iid, void **out )
{
    struct storage_file_statics *impl = impl_from_IStorageFileStatics( iface );
    return factory_QueryInterface( &impl->IActivationFactory_iface, iid, out );
}

static ULONG WINAPI storage_file_statics_AddRef( IStorageFileStatics *iface )
{
    struct storage_file_statics *impl = impl_from_IStorageFileStatics( iface );
    return factory_AddRef( &impl->IActivationFactory_iface );
}

static ULONG WINAPI storage_file_statics_Release( IStorageFileStatics *iface )
{
    struct storage_file_statics *impl = impl_from_IStorageFileStatics( iface );
    return factory_Release( &impl->IActivationFactory_iface );
}

static HRESULT WINAPI storage_file_statics_GetIids( IStorageFileStatics *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_statics_GetRuntimeClassName( IStorageFileStatics *iface, HSTRING *class_name )
{
    return WindowsCreateString( RuntimeClass_Windows_Storage_StorageFile,
                                ARRAY_SIZE(RuntimeClass_Windows_Storage_StorageFile) - 1,
                                class_name );
}

static HRESULT WINAPI storage_file_statics_GetTrustLevel( IStorageFileStatics *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_statics_GetFileFromPathAsync( IStorageFileStatics *iface,
                                                                HSTRING path,
                                                                __FIAsyncOperation_1_Windows__CStorage__CStorageFile **operation )
{
    const WCHAR *file_path = WindowsGetStringRawBuffer( path, NULL );
    IStorageFile *file = NULL;
    HRESULT hr;

    TRACE( "iface %p, path %s, operation %p.\n", iface, debugstr_w(file_path), operation );

    if (!operation) return E_INVALIDARG;
    *operation = NULL;

    if (!file_path) return E_INVALIDARG;

    hr = storage_file_create( file_path, &file );
    if (FAILED(hr)) return hr;

    hr = async_storage_file_create( file, operation );
    IStorageFile_Release( file );
    return hr;
}

static HRESULT WINAPI storage_file_statics_GetFileFromApplicationUriAsync( IStorageFileStatics *iface,
                                                                          IUriRuntimeClass *uri,
                                                                          __FIAsyncOperation_1_Windows__CStorage__CStorageFile **operation )
{
    HSTRING path_hstring = NULL;
    const WCHAR *uri_path;
    WCHAR file_path[MAX_PATH];
    WCHAR *p;
    IStorageFile *file = NULL;
    HRESULT hr;

    TRACE( "iface %p, uri %p, operation %p.\n", iface, uri, operation );

    if (!operation) return E_INVALIDARG;
    *operation = NULL;

    if (!uri) return E_INVALIDARG;

    hr = IUriRuntimeClass_get_Path( uri, &path_hstring );
    if (FAILED(hr) || !path_hstring)
    {
        hr = IUriRuntimeClass_get_RawUri( uri, &path_hstring );
        if (FAILED(hr) || !path_hstring) return hr;
    }

    uri_path = WindowsGetStringRawBuffer( path_hstring, NULL );
    TRACE( "uri path: %s\n", debugstr_w(uri_path) );

    if (!wcsncmp( uri_path, L"ms-appx:///", 11 ))
        uri_path += 11;
    else if (!wcsncmp( uri_path, L"ms-appx://", 10 ))
        uri_path += 10;
    else if (!wcsncmp( uri_path, L"ms-appdata:///", 14 ))
        uri_path += 14;

    while (*uri_path == '/' || *uri_path == '\\')
        uri_path++;

    GetModuleFileNameW( NULL, file_path, ARRAY_SIZE(file_path) );
    p = wcsrchr( file_path, '\\' );
    if (p) *(p + 1) = 0;

    {
        WCHAR rel_path[MAX_PATH];
        int i;
        lstrcpynW( rel_path, uri_path, ARRAY_SIZE(rel_path) );
        for (i = 0; rel_path[i]; i++)
        {
            if (rel_path[i] == '/') rel_path[i] = '\\';
        }
        lstrcatW( file_path, rel_path );
    }

    WindowsDeleteString( path_hstring );

    TRACE( "resolved file path: %s\n", debugstr_w(file_path) );

    hr = storage_file_create( file_path, &file );
    if (FAILED(hr)) return hr;

    hr = async_storage_file_create( file, operation );
    IStorageFile_Release( file );
    return hr;
}

static HRESULT WINAPI storage_file_statics_CreateStreamedFileAsync( IStorageFileStatics *iface,
    HSTRING displayNameWithExtension, void *dataRequested, void *thumbnail,
    __FIAsyncOperation_1_Windows__CStorage__CStorageFile **operation )
{
    FIXME( "iface %p stub!\n", iface );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_statics_ReplaceWithStreamedFileAsync( IStorageFileStatics *iface,
    IStorageFile *fileToReplace, void *dataRequested, void *thumbnail,
    __FIAsyncOperation_1_Windows__CStorage__CStorageFile **operation )
{
    FIXME( "iface %p stub!\n", iface );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_statics_CreateStreamedFileFromUriAsync( IStorageFileStatics *iface,
    HSTRING displayNameWithExtension, IUriRuntimeClass *uri, void *thumbnail,
    __FIAsyncOperation_1_Windows__CStorage__CStorageFile **operation )
{
    FIXME( "iface %p stub!\n", iface );
    return E_NOTIMPL;
}

static const struct IStorageFileStaticsVtbl storage_file_statics_vtbl =
{
    storage_file_statics_QueryInterface,
    storage_file_statics_AddRef,
    storage_file_statics_Release,
    /* IInspectable methods */
    storage_file_statics_GetIids,
    storage_file_statics_GetRuntimeClassName,
    storage_file_statics_GetTrustLevel,
    /* IStorageFileStatics methods */
    storage_file_statics_GetFileFromPathAsync,
    storage_file_statics_GetFileFromApplicationUriAsync,
    storage_file_statics_CreateStreamedFileAsync,
    storage_file_statics_ReplaceWithStreamedFileAsync,
    storage_file_statics_CreateStreamedFileFromUriAsync,
};

static struct storage_file_statics storage_file_statics =
{
    {&factory_vtbl},
    {&storage_file_statics_vtbl},
    1,
};

IActivationFactory *storage_file_factory = &storage_file_statics.IActivationFactory_iface;

/*
 * Windows.Storage.FileIO Implementation
 */

/* IFileIOStatics: GUID {887411eb-7f54-4732-a5f0-5e43e3b8c2f5} */
static const GUID IID_IFileIOStatics = {0x887411eb, 0x7f54, 0x4732, {0xa5, 0xf0, 0x5e, 0x43, 0xe3, 0xb8, 0xc2, 0xf5}};

typedef struct IFileIOStatics IFileIOStatics;

typedef struct IFileIOStaticsVtbl
{
    /* IUnknown */
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IFileIOStatics *This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IFileIOStatics *This);
    ULONG (STDMETHODCALLTYPE *Release)(IFileIOStatics *This);
    /* IInspectable */
    HRESULT (STDMETHODCALLTYPE *GetIids)(IFileIOStatics *This, ULONG *iidCount, IID **iids);
    HRESULT (STDMETHODCALLTYPE *GetRuntimeClassName)(IFileIOStatics *This, HSTRING *className);
    HRESULT (STDMETHODCALLTYPE *GetTrustLevel)(IFileIOStatics *This, TrustLevel *trustLevel);
    /* IFileIOStatics */
    HRESULT (STDMETHODCALLTYPE *ReadTextAsync)(IFileIOStatics *This, IStorageFile *file, IAsyncOperation_HSTRING **operation);
    HRESULT (STDMETHODCALLTYPE *ReadTextWithEncodingAsync)(IFileIOStatics *This, IStorageFile *file, UINT32 encoding, IAsyncOperation_HSTRING **operation);
    HRESULT (STDMETHODCALLTYPE *WriteTextAsync)(IFileIOStatics *This, IStorageFile *file, HSTRING contents, IAsyncAction **operation);
    HRESULT (STDMETHODCALLTYPE *WriteTextWithEncodingAsync)(IFileIOStatics *This, IStorageFile *file, HSTRING contents, UINT32 encoding, IAsyncAction **operation);
    HRESULT (STDMETHODCALLTYPE *AppendTextAsync)(IFileIOStatics *This, IStorageFile *file, HSTRING contents, IAsyncAction **operation);
    HRESULT (STDMETHODCALLTYPE *AppendTextWithEncodingAsync)(IFileIOStatics *This, IStorageFile *file, HSTRING contents, UINT32 encoding, IAsyncAction **operation);
    HRESULT (STDMETHODCALLTYPE *ReadLinesAsync)(IFileIOStatics *This, IStorageFile *file, void **operation);
    HRESULT (STDMETHODCALLTYPE *ReadLinesWithEncodingAsync)(IFileIOStatics *This, IStorageFile *file, UINT32 encoding, void **operation);
    HRESULT (STDMETHODCALLTYPE *WriteLinesAsync)(IFileIOStatics *This, IStorageFile *file, void *lines, IAsyncAction **operation);
    HRESULT (STDMETHODCALLTYPE *WriteLinesWithEncodingAsync)(IFileIOStatics *This, IStorageFile *file, void *lines, UINT32 encoding, IAsyncAction **operation);
    HRESULT (STDMETHODCALLTYPE *AppendLinesAsync)(IFileIOStatics *This, IStorageFile *file, void *lines, IAsyncAction **operation);
    HRESULT (STDMETHODCALLTYPE *AppendLinesWithEncodingAsync)(IFileIOStatics *This, IStorageFile *file, void *lines, UINT32 encoding, IAsyncAction **operation);
    HRESULT (STDMETHODCALLTYPE *ReadBufferAsync)(IFileIOStatics *This, IStorageFile *file, void **operation);
    HRESULT (STDMETHODCALLTYPE *WriteBufferAsync)(IFileIOStatics *This, IStorageFile *file, void *buffer, IAsyncAction **operation);
    HRESULT (STDMETHODCALLTYPE *WriteBytesAsync)(IFileIOStatics *This, IStorageFile *file, UINT32 count, BYTE *buffer, IAsyncAction **operation);
} IFileIOStaticsVtbl;

struct IFileIOStatics
{
    const IFileIOStaticsVtbl *lpVtbl;
};

/*
 * AsyncAction for FileIO
 */
struct async_file_action
{
    IAsyncAction IAsyncAction_iface;
    IAsyncInfo IAsyncInfo_iface;
    LONG ref;
    HRESULT hr;
    AsyncStatus status;
    IAsyncActionCompletedHandler *handler;
};

static inline struct async_file_action *impl_from_async_file_action( IAsyncAction *iface )
{
    return CONTAINING_RECORD( iface, struct async_file_action, IAsyncAction_iface );
}

static inline struct async_file_action *impl_from_async_file_action_info( IAsyncInfo *iface )
{
    return CONTAINING_RECORD( iface, struct async_file_action, IAsyncInfo_iface );
}

static HRESULT WINAPI async_file_action_QueryInterface( IAsyncAction *iface, REFIID iid, void **out )
{
    struct async_file_action *impl = impl_from_async_file_action( iface );
    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (!out) return E_POINTER;
    *out = NULL;

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAsyncAction ))
    {
        *out = &impl->IAsyncAction_iface;
        IAsyncAction_AddRef( iface );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IAsyncInfo ))
    {
        *out = &impl->IAsyncInfo_iface;
        IAsyncAction_AddRef( iface );
        return S_OK;
    }

    WARN( "unsupported iid %s\n", debugstr_guid( iid ) );
    return E_NOINTERFACE;
}

static ULONG WINAPI async_file_action_AddRef( IAsyncAction *iface )
{
    struct async_file_action *impl = impl_from_async_file_action( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI async_file_action_Release( IAsyncAction *iface )
{
    struct async_file_action *impl = impl_from_async_file_action( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    if (!ref)
    {
        if (impl->handler) IAsyncActionCompletedHandler_Release( impl->handler );
        free( impl );
    }
    return ref;
}

static HRESULT WINAPI async_file_action_GetIids( IAsyncAction *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI async_file_action_GetRuntimeClassName( IAsyncAction *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.IAsyncAction", 31, class_name );
}

static HRESULT WINAPI async_file_action_GetTrustLevel( IAsyncAction *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI async_file_action_put_Completed( IAsyncAction *iface, IAsyncActionCompletedHandler *handler )
{
    struct async_file_action *impl = impl_from_async_file_action( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );

    if (impl->handler) IAsyncActionCompletedHandler_Release( impl->handler );
    if ((impl->handler = handler)) IAsyncActionCompletedHandler_AddRef( handler );

    if (handler)
        IAsyncActionCompletedHandler_Invoke( handler, &impl->IAsyncAction_iface, impl->status );

    return S_OK;
}

static HRESULT WINAPI async_file_action_get_Completed( IAsyncAction *iface, IAsyncActionCompletedHandler **handler )
{
    struct async_file_action *impl = impl_from_async_file_action( iface );
    if (!handler) return E_POINTER;
    *handler = impl->handler;
    if (*handler) IAsyncActionCompletedHandler_AddRef( *handler );
    return S_OK;
}

static HRESULT WINAPI async_file_action_GetResults( IAsyncAction *iface )
{
    struct async_file_action *impl = impl_from_async_file_action( iface );
    return impl->hr;
}

static const struct IAsyncActionVtbl async_file_action_vtbl =
{
    async_file_action_QueryInterface,
    async_file_action_AddRef,
    async_file_action_Release,
    async_file_action_GetIids,
    async_file_action_GetRuntimeClassName,
    async_file_action_GetTrustLevel,
    async_file_action_put_Completed,
    async_file_action_get_Completed,
    async_file_action_GetResults,
};

static HRESULT WINAPI async_file_action_info_QueryInterface( IAsyncInfo *iface, REFIID iid, void **out )
{
    struct async_file_action *impl = impl_from_async_file_action_info( iface );
    return async_file_action_QueryInterface( &impl->IAsyncAction_iface, iid, out );
}

static ULONG WINAPI async_file_action_info_AddRef( IAsyncInfo *iface )
{
    struct async_file_action *impl = impl_from_async_file_action_info( iface );
    return async_file_action_AddRef( &impl->IAsyncAction_iface );
}

static ULONG WINAPI async_file_action_info_Release( IAsyncInfo *iface )
{
    struct async_file_action *impl = impl_from_async_file_action_info( iface );
    return async_file_action_Release( &impl->IAsyncAction_iface );
}

static HRESULT WINAPI async_file_action_info_GetIids( IAsyncInfo *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI async_file_action_info_GetRuntimeClassName( IAsyncInfo *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.IAsyncInfo", 27, class_name );
}

static HRESULT WINAPI async_file_action_info_GetTrustLevel( IAsyncInfo *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI async_file_action_info_get_Id( IAsyncInfo *iface, UINT32 *id )
{
    if (!id) return E_INVALIDARG;
    *id = 1;
    return S_OK;
}

static HRESULT WINAPI async_file_action_info_get_Status( IAsyncInfo *iface, AsyncStatus *status )
{
    struct async_file_action *impl = impl_from_async_file_action_info( iface );
    if (!status) return E_INVALIDARG;
    *status = impl->status;
    return S_OK;
}

static HRESULT WINAPI async_file_action_info_get_ErrorCode( IAsyncInfo *iface, HRESULT *error_code )
{
    struct async_file_action *impl = impl_from_async_file_action_info( iface );
    if (!error_code) return E_INVALIDARG;
    *error_code = impl->hr;
    return S_OK;
}

static HRESULT WINAPI async_file_action_info_Cancel( IAsyncInfo *iface )
{
    return S_OK;
}

static HRESULT WINAPI async_file_action_info_Close( IAsyncInfo *iface )
{
    return S_OK;
}

static const struct IAsyncInfoVtbl async_file_action_info_vtbl =
{
    async_file_action_info_QueryInterface,
    async_file_action_info_AddRef,
    async_file_action_info_Release,
    async_file_action_info_GetIids,
    async_file_action_info_GetRuntimeClassName,
    async_file_action_info_GetTrustLevel,
    async_file_action_info_get_Id,
    async_file_action_info_get_Status,
    async_file_action_info_get_ErrorCode,
    async_file_action_info_Cancel,
    async_file_action_info_Close,
};

static HRESULT async_file_action_create( HRESULT hr, AsyncStatus status, IAsyncAction **out )
{
    struct async_file_action *impl;
    if (!out) return E_INVALIDARG;
    *out = NULL;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IAsyncAction_iface.lpVtbl = &async_file_action_vtbl;
    impl->IAsyncInfo_iface.lpVtbl = &async_file_action_info_vtbl;
    impl->ref = 1;
    impl->hr = hr;
    impl->status = status;
    *out = &impl->IAsyncAction_iface;
    TRACE( "created async_file_action %p, hr %#lx, status %d\n", *out, hr, status );
    return S_OK;
}

/*
 * AsyncOperation<HSTRING> for FileIO
 */
struct async_file_hstring
{
    IAsyncOperation_HSTRING IAsyncOperation_HSTRING_iface;
    IAsyncInfo IAsyncInfo_iface;
    LONG ref;
    HRESULT hr;
    AsyncStatus status;
    HSTRING result;
    IAsyncOperationCompletedHandler_HSTRING *handler;
};

static inline struct async_file_hstring *impl_from_IAsyncOperation_HSTRING( IAsyncOperation_HSTRING *iface )
{
    return CONTAINING_RECORD( iface, struct async_file_hstring, IAsyncOperation_HSTRING_iface );
}

static inline struct async_file_hstring *impl_from_async_file_hstring_info( IAsyncInfo *iface )
{
    return CONTAINING_RECORD( iface, struct async_file_hstring, IAsyncInfo_iface );
}

static HRESULT WINAPI async_file_hstring_QueryInterface( IAsyncOperation_HSTRING *iface, REFIID iid, void **out )
{
    struct async_file_hstring *impl = impl_from_IAsyncOperation_HSTRING( iface );
    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (!out) return E_POINTER;
    *out = NULL;

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAsyncOperation_HSTRING ))
    {
        *out = &impl->IAsyncOperation_HSTRING_iface;
        IAsyncOperation_HSTRING_AddRef( iface );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IAsyncInfo ))
    {
        *out = &impl->IAsyncInfo_iface;
        IAsyncOperation_HSTRING_AddRef( iface );
        return S_OK;
    }

    WARN( "unsupported iid %s\n", debugstr_guid( iid ) );
    return E_NOINTERFACE;
}

static ULONG WINAPI async_file_hstring_AddRef( IAsyncOperation_HSTRING *iface )
{
    struct async_file_hstring *impl = impl_from_IAsyncOperation_HSTRING( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI async_file_hstring_Release( IAsyncOperation_HSTRING *iface )
{
    struct async_file_hstring *impl = impl_from_IAsyncOperation_HSTRING( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    if (!ref)
    {
        if (impl->result) WindowsDeleteString( impl->result );
        if (impl->handler) IAsyncOperationCompletedHandler_HSTRING_Release( impl->handler );
        free( impl );
    }
    return ref;
}

static HRESULT WINAPI async_file_hstring_GetIids( IAsyncOperation_HSTRING *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI async_file_hstring_GetRuntimeClassName( IAsyncOperation_HSTRING *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.IAsyncOperation`1<String>", 43, class_name );
}

static HRESULT WINAPI async_file_hstring_GetTrustLevel( IAsyncOperation_HSTRING *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI async_file_hstring_put_Completed( IAsyncOperation_HSTRING *iface, IAsyncOperationCompletedHandler_HSTRING *handler )
{
    struct async_file_hstring *impl = impl_from_IAsyncOperation_HSTRING( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );

    if (impl->handler) IAsyncOperationCompletedHandler_HSTRING_Release( impl->handler );
    if ((impl->handler = handler)) IAsyncOperationCompletedHandler_HSTRING_AddRef( handler );

    if (handler)
        IAsyncOperationCompletedHandler_HSTRING_Invoke( handler, &impl->IAsyncOperation_HSTRING_iface, impl->status );

    return S_OK;
}

static HRESULT WINAPI async_file_hstring_get_Completed( IAsyncOperation_HSTRING *iface, IAsyncOperationCompletedHandler_HSTRING **handler )
{
    struct async_file_hstring *impl = impl_from_IAsyncOperation_HSTRING( iface );
    if (!handler) return E_POINTER;
    *handler = impl->handler;
    if (*handler) IAsyncOperationCompletedHandler_HSTRING_AddRef( *handler );
    return S_OK;
}

static HRESULT WINAPI async_file_hstring_GetResults( IAsyncOperation_HSTRING *iface, HSTRING *results )
{
    struct async_file_hstring *impl = impl_from_IAsyncOperation_HSTRING( iface );
    TRACE( "iface %p, results %p.\n", iface, results );
    if (!results) return E_POINTER;
    if (impl->result)
        return WindowsDuplicateString( impl->result, results );
    *results = NULL;
    return impl->hr;
}

static const struct IAsyncOperation_HSTRINGVtbl async_file_hstring_vtbl =
{
    async_file_hstring_QueryInterface,
    async_file_hstring_AddRef,
    async_file_hstring_Release,
    async_file_hstring_GetIids,
    async_file_hstring_GetRuntimeClassName,
    async_file_hstring_GetTrustLevel,
    async_file_hstring_put_Completed,
    async_file_hstring_get_Completed,
    async_file_hstring_GetResults,
};

static HRESULT WINAPI async_file_hstring_info_QueryInterface( IAsyncInfo *iface, REFIID iid, void **out )
{
    struct async_file_hstring *impl = impl_from_async_file_hstring_info( iface );
    return async_file_hstring_QueryInterface( &impl->IAsyncOperation_HSTRING_iface, iid, out );
}

static ULONG WINAPI async_file_hstring_info_AddRef( IAsyncInfo *iface )
{
    struct async_file_hstring *impl = impl_from_async_file_hstring_info( iface );
    return async_file_hstring_AddRef( &impl->IAsyncOperation_HSTRING_iface );
}

static ULONG WINAPI async_file_hstring_info_Release( IAsyncInfo *iface )
{
    struct async_file_hstring *impl = impl_from_async_file_hstring_info( iface );
    return async_file_hstring_Release( &impl->IAsyncOperation_HSTRING_iface );
}

static HRESULT WINAPI async_file_hstring_info_GetIids( IAsyncInfo *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI async_file_hstring_info_GetRuntimeClassName( IAsyncInfo *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.IAsyncInfo", 27, class_name );
}

static HRESULT WINAPI async_file_hstring_info_GetTrustLevel( IAsyncInfo *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI async_file_hstring_info_get_Id( IAsyncInfo *iface, UINT32 *id )
{
    if (!id) return E_INVALIDARG;
    *id = 1;
    return S_OK;
}

static HRESULT WINAPI async_file_hstring_info_get_Status( IAsyncInfo *iface, AsyncStatus *status )
{
    struct async_file_hstring *impl = impl_from_async_file_hstring_info( iface );
    if (!status) return E_INVALIDARG;
    *status = impl->status;
    return S_OK;
}

static HRESULT WINAPI async_file_hstring_info_get_ErrorCode( IAsyncInfo *iface, HRESULT *error_code )
{
    struct async_file_hstring *impl = impl_from_async_file_hstring_info( iface );
    if (!error_code) return E_INVALIDARG;
    *error_code = impl->hr;
    return S_OK;
}

static HRESULT WINAPI async_file_hstring_info_Cancel( IAsyncInfo *iface )
{
    return S_OK;
}

static HRESULT WINAPI async_file_hstring_info_Close( IAsyncInfo *iface )
{
    return S_OK;
}

static const struct IAsyncInfoVtbl async_file_hstring_info_vtbl =
{
    async_file_hstring_info_QueryInterface,
    async_file_hstring_info_AddRef,
    async_file_hstring_info_Release,
    async_file_hstring_info_GetIids,
    async_file_hstring_info_GetRuntimeClassName,
    async_file_hstring_info_GetTrustLevel,
    async_file_hstring_info_get_Id,
    async_file_hstring_info_get_Status,
    async_file_hstring_info_get_ErrorCode,
    async_file_hstring_info_Cancel,
    async_file_hstring_info_Close,
};

static HRESULT async_file_hstring_create( HSTRING result, HRESULT hr, AsyncStatus status, IAsyncOperation_HSTRING **out )
{
    struct async_file_hstring *impl;
    if (!out) return E_INVALIDARG;
    *out = NULL;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IAsyncOperation_HSTRING_iface.lpVtbl = &async_file_hstring_vtbl;
    impl->IAsyncInfo_iface.lpVtbl = &async_file_hstring_info_vtbl;
    impl->ref = 1;
    impl->hr = hr;
    impl->status = status;
    impl->result = result;
    *out = &impl->IAsyncOperation_HSTRING_iface;
    TRACE( "created async_file_hstring %p, result %s, hr %#lx, status %d\n", *out, debugstr_hstring(result), hr, status );
    return S_OK;
}

/*
 * FileIO helper functions
 */
static HRESULT get_storage_file_path( IStorageFile *file, WCHAR *path, DWORD size )
{
    IStorageItem *item = NULL;
    HSTRING path_hstring = NULL;
    const WCHAR *str;
    HRESULT hr;

    if (!file) return E_INVALIDARG;

    hr = IStorageFile_QueryInterface( file, &IID_IStorageItem, (void **)&item );
    if (FAILED(hr))
    {
        WARN( "Failed to query IStorageItem from file %p: %#lx\n", file, hr );
        return hr;
    }

    hr = IStorageItem_get_Path( item, &path_hstring );
    IStorageItem_Release( item );
    if (FAILED(hr))
    {
        WARN( "Failed to get path from storage item: %#lx\n", hr );
        return hr;
    }

    str = WindowsGetStringRawBuffer( path_hstring, NULL );
    if (!str || !str[0])
    {
        WARN( "Empty path returned for storage file %p\n", file );
        WindowsDeleteString( path_hstring );
        return E_FAIL;
    }

    lstrcpynW( path, str, size );
    WindowsDeleteString( path_hstring );
    return S_OK;
}

static HRESULT file_io_write_text_sync( const WCHAR *path, HSTRING contents, UINT32 encoding )
{
    HANDLE handle;
    DWORD written;
    UINT32 len = 0;
    const WCHAR *str = WindowsGetStringRawBuffer( contents, &len );

    handle = CreateFileW( path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
    if (handle == INVALID_HANDLE_VALUE)
    {
        DWORD err = GetLastError();
        WARN( "CreateFileW(%s) failed: %lu\n", debugstr_w(path), err );
        return HRESULT_FROM_WIN32( err );
    }

    if (len > 0 && str)
    {
        if (encoding == 1 /* Utf16LE */)
        {
            WriteFile( handle, str, len * sizeof(WCHAR), &written, NULL );
        }
        else
        {
            /* Default UTF-8 */
            int utf8_len = WideCharToMultiByte( CP_UTF8, 0, str, len, NULL, 0, NULL, NULL );
            if (utf8_len > 0)
            {
                char *utf8_buf = malloc( utf8_len );
                if (utf8_buf)
                {
                    WideCharToMultiByte( CP_UTF8, 0, str, len, utf8_buf, utf8_len, NULL, NULL );
                    WriteFile( handle, utf8_buf, utf8_len, &written, NULL );
                    free( utf8_buf );
                }
            }
        }
    }

    CloseHandle( handle );
    return S_OK;
}

static HRESULT file_io_read_text_sync( const WCHAR *path, UINT32 encoding, HSTRING *out_str )
{
    HANDLE handle;
    DWORD file_size, read_bytes;
    BYTE *buf;
    HRESULT hr = S_OK;

    if (!out_str) return E_POINTER;
    *out_str = NULL;

    handle = CreateFileW( path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
    if (handle == INVALID_HANDLE_VALUE)
    {
        DWORD err = GetLastError();
        WARN( "CreateFileW(%s) failed: %lu\n", debugstr_w(path), err );
        return HRESULT_FROM_WIN32( err );
    }

    file_size = GetFileSize( handle, NULL );
    if (file_size == INVALID_FILE_SIZE)
    {
        CloseHandle( handle );
        return HRESULT_FROM_WIN32( GetLastError() );
    }

    if (file_size == 0)
    {
        CloseHandle( handle );
        return WindowsCreateString( L"", 0, out_str );
    }

    buf = malloc( file_size + 2 );
    if (!buf)
    {
        CloseHandle( handle );
        return E_OUTOFMEMORY;
    }

    if (!ReadFile( handle, buf, file_size, &read_bytes, NULL ))
    {
        DWORD err = GetLastError();
        free( buf );
        CloseHandle( handle );
        return HRESULT_FROM_WIN32( err );
    }
    CloseHandle( handle );

    /* Check for BOM */
    if (read_bytes >= 3 && buf[0] == 0xef && buf[1] == 0xbb && buf[2] == 0xbf)
    {
        int wlen = MultiByteToWideChar( CP_UTF8, 0, (char *)(buf + 3), read_bytes - 3, NULL, 0 );
        WCHAR *wbuf = malloc( (wlen + 1) * sizeof(WCHAR) );
        if (wbuf)
        {
            MultiByteToWideChar( CP_UTF8, 0, (char *)(buf + 3), read_bytes - 3, wbuf, wlen );
            wbuf[wlen] = 0;
            hr = WindowsCreateString( wbuf, wlen, out_str );
            free( wbuf );
        }
        else hr = E_OUTOFMEMORY;
    }
    else if (read_bytes >= 2 && buf[0] == 0xff && buf[1] == 0xfe)
    {
        int wlen = (read_bytes - 2) / sizeof(WCHAR);
        hr = WindowsCreateString( (WCHAR *)(buf + 2), wlen, out_str );
    }
    else if (encoding == 1 /* Utf16LE */)
    {
        int wlen = read_bytes / sizeof(WCHAR);
        hr = WindowsCreateString( (WCHAR *)buf, wlen, out_str );
    }
    else
    {
        int wlen = MultiByteToWideChar( CP_UTF8, 0, (char *)buf, read_bytes, NULL, 0 );
        WCHAR *wbuf = malloc( (wlen + 1) * sizeof(WCHAR) );
        if (wbuf)
        {
            MultiByteToWideChar( CP_UTF8, 0, (char *)buf, read_bytes, wbuf, wlen );
            wbuf[wlen] = 0;
            hr = WindowsCreateString( wbuf, wlen, out_str );
            free( wbuf );
        }
        else hr = E_OUTOFMEMORY;
    }

    free( buf );
    return hr;
}

/*
 * FileIO Statics implementation
 */
struct file_io_statics
{
    IActivationFactory IActivationFactory_iface;
    IFileIOStatics IFileIOStatics_iface;
    LONG ref;
};

static inline struct file_io_statics *impl_from_file_io_statics_factory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct file_io_statics, IActivationFactory_iface );
}

static inline struct file_io_statics *impl_from_IFileIOStatics( IFileIOStatics *iface )
{
    return CONTAINING_RECORD( iface, struct file_io_statics, IFileIOStatics_iface );
}

static HRESULT WINAPI file_io_factory_QueryInterface( IActivationFactory *iface, REFIID iid, void **out )
{
    struct file_io_statics *impl = impl_from_file_io_statics_factory( iface );
    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (!out) return E_POINTER;
    *out = NULL;

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IActivationFactory ))
    {
        *out = &impl->IActivationFactory_iface;
        IActivationFactory_AddRef( iface );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IFileIOStatics ))
    {
        *out = &impl->IFileIOStatics_iface;
        IActivationFactory_AddRef( iface );
        return S_OK;
    }

    WARN( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    return E_NOINTERFACE;
}

static ULONG WINAPI file_io_factory_AddRef( IActivationFactory *iface )
{
    struct file_io_statics *impl = impl_from_file_io_statics_factory( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI file_io_factory_Release( IActivationFactory *iface )
{
    struct file_io_statics *impl = impl_from_file_io_statics_factory( iface );
    return InterlockedDecrement( &impl->ref );
}

static HRESULT WINAPI file_io_factory_GetIids( IActivationFactory *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI file_io_factory_GetRuntimeClassName( IActivationFactory *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Storage.FileIO", 22, class_name );
}

static HRESULT WINAPI file_io_factory_GetTrustLevel( IActivationFactory *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI file_io_factory_ActivateInstance( IActivationFactory *iface, IInspectable **instance )
{
    if (instance) *instance = NULL;
    return E_NOTIMPL;
}

static const struct IActivationFactoryVtbl file_io_factory_vtbl =
{
    file_io_factory_QueryInterface,
    file_io_factory_AddRef,
    file_io_factory_Release,
    file_io_factory_GetIids,
    file_io_factory_GetRuntimeClassName,
    file_io_factory_GetTrustLevel,
    file_io_factory_ActivateInstance,
};

static HRESULT WINAPI file_io_statics_QueryInterface( IFileIOStatics *iface, REFIID iid, void **out )
{
    struct file_io_statics *impl = impl_from_IFileIOStatics( iface );
    return file_io_factory_QueryInterface( &impl->IActivationFactory_iface, iid, out );
}

static ULONG WINAPI file_io_statics_AddRef( IFileIOStatics *iface )
{
    struct file_io_statics *impl = impl_from_IFileIOStatics( iface );
    return file_io_factory_AddRef( &impl->IActivationFactory_iface );
}

static ULONG WINAPI file_io_statics_Release( IFileIOStatics *iface )
{
    struct file_io_statics *impl = impl_from_IFileIOStatics( iface );
    return file_io_factory_Release( &impl->IActivationFactory_iface );
}

static HRESULT WINAPI file_io_statics_GetIids( IFileIOStatics *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI file_io_statics_GetRuntimeClassName( IFileIOStatics *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Storage.FileIO", 22, class_name );
}

static HRESULT WINAPI file_io_statics_GetTrustLevel( IFileIOStatics *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI file_io_statics_WriteTextWithEncodingAsync( IFileIOStatics *iface, IStorageFile *file,
    HSTRING contents, UINT32 encoding, IAsyncAction **operation )
{
    WCHAR path[MAX_PATH];
    HRESULT hr;

    TRACE( "iface %p, file %p, contents %s, encoding %u, operation %p.\n",
           iface, file, debugstr_hstring( contents ), encoding, operation );

    if (!operation) return E_INVALIDARG;
    *operation = NULL;

    hr = get_storage_file_path( file, path, ARRAY_SIZE(path) );
    if (FAILED(hr))
    {
        WARN( "failed to get path from file %p: %#lx\n", file, hr );
        return async_file_action_create( hr, Error, operation );
    }

    hr = file_io_write_text_sync( path, contents, encoding );
    return async_file_action_create( hr, SUCCEEDED(hr) ? Completed : Error, operation );
}

static HRESULT WINAPI file_io_statics_WriteTextAsync( IFileIOStatics *iface, IStorageFile *file,
    HSTRING contents, IAsyncAction **operation )
{
    return file_io_statics_WriteTextWithEncodingAsync( iface, file, contents, 0 /* Utf8 */, operation );
}

static HRESULT WINAPI file_io_statics_ReadTextWithEncodingAsync( IFileIOStatics *iface, IStorageFile *file,
    UINT32 encoding, IAsyncOperation_HSTRING **operation )
{
    WCHAR path[MAX_PATH];
    HSTRING result = NULL;
    HRESULT hr;

    TRACE( "iface %p, file %p, encoding %u, operation %p.\n",
           iface, file, encoding, operation );

    if (!operation) return E_INVALIDARG;
    *operation = NULL;

    hr = get_storage_file_path( file, path, ARRAY_SIZE(path) );
    if (FAILED(hr))
    {
        WARN( "failed to get path from file %p: %#lx\n", file, hr );
        return async_file_hstring_create( NULL, hr, Error, operation );
    }

    hr = file_io_read_text_sync( path, encoding, &result );
    return async_file_hstring_create( result, hr, SUCCEEDED(hr) ? Completed : Error, operation );
}

static HRESULT WINAPI file_io_statics_ReadTextAsync( IFileIOStatics *iface, IStorageFile *file,
    IAsyncOperation_HSTRING **operation )
{
    return file_io_statics_ReadTextWithEncodingAsync( iface, file, 0 /* Utf8 */, operation );
}

static HRESULT WINAPI file_io_statics_AppendTextWithEncodingAsync( IFileIOStatics *iface, IStorageFile *file,
    HSTRING contents, UINT32 encoding, IAsyncAction **operation )
{
    WCHAR path[MAX_PATH];
    HRESULT hr;
    HANDLE handle;
    DWORD written;
    UINT32 len = 0;
    const WCHAR *str = WindowsGetStringRawBuffer( contents, &len );

    TRACE( "iface %p, file %p, contents %s, encoding %u, operation %p.\n",
           iface, file, debugstr_hstring( contents ), encoding, operation );

    if (!operation) return E_INVALIDARG;
    *operation = NULL;

    hr = get_storage_file_path( file, path, ARRAY_SIZE(path) );
    if (FAILED(hr))
        return async_file_action_create( hr, Error, operation );

    handle = CreateFileW( path, FILE_APPEND_DATA, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
    if (handle == INVALID_HANDLE_VALUE)
        return async_file_action_create( HRESULT_FROM_WIN32(GetLastError()), Error, operation );

    if (len > 0 && str)
    {
        if (encoding == 1 /* Utf16LE */)
        {
            WriteFile( handle, str, len * sizeof(WCHAR), &written, NULL );
        }
        else
        {
            int utf8_len = WideCharToMultiByte( CP_UTF8, 0, str, len, NULL, 0, NULL, NULL );
            if (utf8_len > 0)
            {
                char *utf8_buf = malloc( utf8_len );
                if (utf8_buf)
                {
                    WideCharToMultiByte( CP_UTF8, 0, str, len, utf8_buf, utf8_len, NULL, NULL );
                    WriteFile( handle, utf8_buf, utf8_len, &written, NULL );
                    free( utf8_buf );
                }
            }
        }
    }
    CloseHandle( handle );

    return async_file_action_create( S_OK, Completed, operation );
}

static HRESULT WINAPI file_io_statics_AppendTextAsync( IFileIOStatics *iface, IStorageFile *file,
    HSTRING contents, IAsyncAction **operation )
{
    return file_io_statics_AppendTextWithEncodingAsync( iface, file, contents, 0, operation );
}

static HRESULT WINAPI file_io_statics_ReadLinesAsync( IFileIOStatics *iface, IStorageFile *file, void **operation )
{
    FIXME( "stub!\n" );
    return E_NOTIMPL;
}

static HRESULT WINAPI file_io_statics_ReadLinesWithEncodingAsync( IFileIOStatics *iface, IStorageFile *file, UINT32 encoding, void **operation )
{
    FIXME( "stub!\n" );
    return E_NOTIMPL;
}

static HRESULT WINAPI file_io_statics_WriteLinesAsync( IFileIOStatics *iface, IStorageFile *file, void *lines, IAsyncAction **operation )
{
    FIXME( "stub!\n" );
    return E_NOTIMPL;
}

static HRESULT WINAPI file_io_statics_WriteLinesWithEncodingAsync( IFileIOStatics *iface, IStorageFile *file, void *lines, UINT32 encoding, IAsyncAction **operation )
{
    FIXME( "stub!\n" );
    return E_NOTIMPL;
}

static HRESULT WINAPI file_io_statics_AppendLinesAsync( IFileIOStatics *iface, IStorageFile *file, void *lines, IAsyncAction **operation )
{
    FIXME( "stub!\n" );
    return E_NOTIMPL;
}

static HRESULT WINAPI file_io_statics_AppendLinesWithEncodingAsync( IFileIOStatics *iface, IStorageFile *file, void *lines, UINT32 encoding, IAsyncAction **operation )
{
    FIXME( "stub!\n" );
    return E_NOTIMPL;
}

static HRESULT WINAPI file_io_statics_ReadBufferAsync( IFileIOStatics *iface, IStorageFile *file, void **operation )
{
    FIXME( "stub!\n" );
    return E_NOTIMPL;
}

static HRESULT WINAPI file_io_statics_WriteBufferAsync( IFileIOStatics *iface, IStorageFile *file, void *buffer, IAsyncAction **operation )
{
    FIXME( "stub!\n" );
    return E_NOTIMPL;
}

static HRESULT WINAPI file_io_statics_WriteBytesAsync( IFileIOStatics *iface, IStorageFile *file,
    UINT32 count, BYTE *buffer, IAsyncAction **operation )
{
    WCHAR path[MAX_PATH];
    HRESULT hr;
    HANDLE handle;
    DWORD written;

    TRACE( "iface %p, file %p, count %u, buffer %p, operation %p.\n",
           iface, file, count, buffer, operation );

    if (!operation) return E_INVALIDARG;
    *operation = NULL;

    hr = get_storage_file_path( file, path, ARRAY_SIZE(path) );
    if (FAILED(hr))
        return async_file_action_create( hr, Error, operation );

    handle = CreateFileW( path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
    if (handle == INVALID_HANDLE_VALUE)
        return async_file_action_create( HRESULT_FROM_WIN32(GetLastError()), Error, operation );

    if (count > 0 && buffer)
        WriteFile( handle, buffer, count, &written, NULL );

    CloseHandle( handle );
    return async_file_action_create( S_OK, Completed, operation );
}

static const struct IFileIOStaticsVtbl file_io_statics_vtbl =
{
    file_io_statics_QueryInterface,
    file_io_statics_AddRef,
    file_io_statics_Release,
    /* IInspectable methods */
    file_io_statics_GetIids,
    file_io_statics_GetRuntimeClassName,
    file_io_statics_GetTrustLevel,
    /* IFileIOStatics methods */
    file_io_statics_ReadTextAsync,
    file_io_statics_ReadTextWithEncodingAsync,
    file_io_statics_WriteTextAsync,
    file_io_statics_WriteTextWithEncodingAsync,
    file_io_statics_AppendTextAsync,
    file_io_statics_AppendTextWithEncodingAsync,
    file_io_statics_ReadLinesAsync,
    file_io_statics_ReadLinesWithEncodingAsync,
    file_io_statics_WriteLinesAsync,
    file_io_statics_WriteLinesWithEncodingAsync,
    file_io_statics_AppendLinesAsync,
    file_io_statics_AppendLinesWithEncodingAsync,
    file_io_statics_ReadBufferAsync,
    file_io_statics_WriteBufferAsync,
    file_io_statics_WriteBytesAsync,
};

static struct file_io_statics file_io_statics =
{
    {&file_io_factory_vtbl},
    {&file_io_statics_vtbl},
    1,
};

IActivationFactory *file_io_factory = &file_io_statics.IActivationFactory_iface;
