/* WinRT Windows.Networking.Connectivity.NetworkInformation Implementation
 *
 * Copyright (C) 2024 Mohamad Al-Jaf
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
#include "initguid.h"
#include "netlistmgr.h"

WINE_DEFAULT_DEBUG_CHANNEL(connectivity);

static EventRegistrationToken dummy_cookie = {.value = 0xdeadbeef};

static const GUID IID_IWeakReferenceSource = {0x00000038, 0x0000, 0x0000, {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
static const GUID IID_ComWrappers_internal = {0x11d3b13a, 0x180e, 0x4789, {0xa8, 0xbe, 0x77, 0x12, 0x88, 0x28, 0x93, 0xe6}};
static const GUID IID_IManagedObjectTracker1 = {0x5c13e51c, 0x4f32, 0x4726, {0xa3, 0xfd, 0xf3, 0xed, 0xd6, 0x3d, 0xa3, 0xa0}};
static const GUID IID_IManagedObjectTracker2 = {0x5c13e51c, 0xc760, 0x496f, {0xa6, 0x5a, 0x09, 0x65, 0xd1, 0xd6, 0x04, 0xa8}};

struct network_information_statics
{
    IActivationFactory IActivationFactory_iface;
    INetworkInformationStatics INetworkInformationStatics_iface;
    LONG ref;
};

static inline struct network_information_statics *impl_from_IActivationFactory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct network_information_statics, IActivationFactory_iface );
}

static HRESULT WINAPI factory_QueryInterface( IActivationFactory *iface, REFIID iid, void **out )
{
    struct network_information_statics *impl = impl_from_IActivationFactory( iface );

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

    if (IsEqualGUID( iid, &IID_INetworkInformationStatics ))
    {
        *out = &impl->INetworkInformationStatics_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IWeakReferenceSource ) ||
        IsEqualGUID( iid, &IID_ComWrappers_internal ) ||
        IsEqualGUID( iid, &IID_IManagedObjectTracker1 ) ||
        IsEqualGUID( iid, &IID_IManagedObjectTracker2 ))
    {
        *out = NULL;
        return E_NOINTERFACE;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

/* ConnectionCost implementation */
struct connection_cost
{
    IConnectionCost IConnectionCost_iface;
    LONG ref;
};

static inline struct connection_cost *impl_from_IConnectionCost( IConnectionCost *iface )
{
    return CONTAINING_RECORD( iface, struct connection_cost, IConnectionCost_iface );
}

static HRESULT WINAPI connection_cost_QueryInterface( IConnectionCost *iface, REFIID iid, void **out )
{
    struct connection_cost *impl = impl_from_IConnectionCost( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IConnectionCost ))
    {
        *out = &impl->IConnectionCost_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IWeakReferenceSource ) ||
        IsEqualGUID( iid, &IID_ComWrappers_internal ) ||
        IsEqualGUID( iid, &IID_IManagedObjectTracker1 ) ||
        IsEqualGUID( iid, &IID_IManagedObjectTracker2 ))
    {
        *out = NULL;
        return E_NOINTERFACE;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI connection_cost_AddRef( IConnectionCost *iface )
{
    struct connection_cost *impl = impl_from_IConnectionCost( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI connection_cost_Release( IConnectionCost *iface )
{
    struct connection_cost *impl = impl_from_IConnectionCost( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );
    if (!ref)
        free( impl );
    return ref;
}

static HRESULT WINAPI connection_cost_GetIids( IConnectionCost *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI connection_cost_GetRuntimeClassName( IConnectionCost *iface, HSTRING *class_name )
{
    return WindowsCreateString( RuntimeClass_Windows_Networking_Connectivity_ConnectionCost,
                                ARRAY_SIZE(RuntimeClass_Windows_Networking_Connectivity_ConnectionCost) - 1,
                                class_name );
}

static HRESULT WINAPI connection_cost_GetTrustLevel( IConnectionCost *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_POINTER;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI connection_cost_get_NetworkCostType( IConnectionCost *iface, NetworkCostType *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    *value = NetworkCostType_Unrestricted;
    return S_OK;
}

static HRESULT WINAPI connection_cost_get_Roaming( IConnectionCost *iface, boolean *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    *value = FALSE;
    return S_OK;
}

static HRESULT WINAPI connection_cost_get_OverDataLimit( IConnectionCost *iface, boolean *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    *value = FALSE;
    return S_OK;
}

static HRESULT WINAPI connection_cost_get_ApproachingDataLimit( IConnectionCost *iface, boolean *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    *value = FALSE;
    return S_OK;
}

static const struct IConnectionCostVtbl connection_cost_vtbl =
{
    connection_cost_QueryInterface,
    connection_cost_AddRef,
    connection_cost_Release,
    /* IInspectable methods */
    connection_cost_GetIids,
    connection_cost_GetRuntimeClassName,
    connection_cost_GetTrustLevel,
    /* IConnectionCost methods */
    connection_cost_get_NetworkCostType,
    connection_cost_get_Roaming,
    connection_cost_get_OverDataLimit,
    connection_cost_get_ApproachingDataLimit,
};

static HRESULT connection_cost_create( IConnectionCost **cost )
{
    struct connection_cost *impl;

    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IConnectionCost_iface.lpVtbl = &connection_cost_vtbl;
    impl->ref = 1;
    *cost = &impl->IConnectionCost_iface;
    return S_OK;
}

/* NetworkAdapter implementation */
struct network_adapter
{
    INetworkAdapter INetworkAdapter_iface;
    LONG ref;
};

static inline struct network_adapter *impl_from_INetworkAdapter( INetworkAdapter *iface )
{
    return CONTAINING_RECORD( iface, struct network_adapter, INetworkAdapter_iface );
}

static HRESULT WINAPI network_adapter_QueryInterface( INetworkAdapter *iface, REFIID iid, void **out )
{
    struct network_adapter *impl = impl_from_INetworkAdapter( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_INetworkAdapter ))
    {
        *out = &impl->INetworkAdapter_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IWeakReferenceSource ) ||
        IsEqualGUID( iid, &IID_ComWrappers_internal ) ||
        IsEqualGUID( iid, &IID_IManagedObjectTracker1 ) ||
        IsEqualGUID( iid, &IID_IManagedObjectTracker2 ))
    {
        *out = NULL;
        return E_NOINTERFACE;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI network_adapter_AddRef( INetworkAdapter *iface )
{
    struct network_adapter *impl = impl_from_INetworkAdapter( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI network_adapter_Release( INetworkAdapter *iface )
{
    struct network_adapter *impl = impl_from_INetworkAdapter( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );
    if (!ref)
        free( impl );
    return ref;
}

static HRESULT WINAPI network_adapter_GetIids( INetworkAdapter *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI network_adapter_GetRuntimeClassName( INetworkAdapter *iface, HSTRING *class_name )
{
    return WindowsCreateString( RuntimeClass_Windows_Networking_Connectivity_NetworkAdapter,
                                ARRAY_SIZE(RuntimeClass_Windows_Networking_Connectivity_NetworkAdapter) - 1,
                                class_name );
}

static HRESULT WINAPI network_adapter_GetTrustLevel( INetworkAdapter *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_POINTER;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI network_adapter_get_OutboundMaxBitsPerSecond( INetworkAdapter *iface, UINT64 *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    *value = 1000000000ULL;
    return S_OK;
}

static HRESULT WINAPI network_adapter_get_InboundMaxBitsPerSecond( INetworkAdapter *iface, UINT64 *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    *value = 1000000000ULL;
    return S_OK;
}

static HRESULT WINAPI network_adapter_get_IanaInterfaceType( INetworkAdapter *iface, UINT32 *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    *value = 6; /* MIB_IF_TYPE_ETHERNET */
    return S_OK;
}

static HRESULT WINAPI network_adapter_get_NetworkItem( INetworkAdapter *iface, INetworkItem **value )
{
    FIXME( "iface %p, value %p stub!\n", iface, value );
    if (!value) return E_POINTER;
    *value = NULL;
    return S_OK;
}

static HRESULT WINAPI network_adapter_get_NetworkAdapterId( INetworkAdapter *iface, GUID *value )
{
    static const GUID adapter_id = {0x11223344, 0x5566, 0x7788, {0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00}};
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    *value = adapter_id;
    return S_OK;
}

static HRESULT WINAPI network_adapter_GetConnectedProfileAsync( INetworkAdapter *iface,
    __FIAsyncOperation_1_Windows__CNetworking__CConnectivity__CConnectionProfile **value )
{
    FIXME( "iface %p, value %p stub!\n", iface, value );
    if (!value) return E_POINTER;
    *value = NULL;
    return E_NOTIMPL;
}

static const struct INetworkAdapterVtbl network_adapter_vtbl =
{
    network_adapter_QueryInterface,
    network_adapter_AddRef,
    network_adapter_Release,
    /* IInspectable methods */
    network_adapter_GetIids,
    network_adapter_GetRuntimeClassName,
    network_adapter_GetTrustLevel,
    /* INetworkAdapter methods */
    network_adapter_get_OutboundMaxBitsPerSecond,
    network_adapter_get_InboundMaxBitsPerSecond,
    network_adapter_get_IanaInterfaceType,
    network_adapter_get_NetworkItem,
    network_adapter_get_NetworkAdapterId,
    network_adapter_GetConnectedProfileAsync,
};

static HRESULT network_adapter_create( INetworkAdapter **adapter )
{
    struct network_adapter *impl;

    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->INetworkAdapter_iface.lpVtbl = &network_adapter_vtbl;
    impl->ref = 1;
    *adapter = &impl->INetworkAdapter_iface;
    return S_OK;
}

/* HostNames empty VectorView / Iterable / Iterator implementation */
struct host_names_iterator
{
    IIterator_HostName IIterator_HostName_iface;
    LONG ref;
};

static inline struct host_names_iterator *impl_from_IIterator_HostName( IIterator_HostName *iface )
{
    return CONTAINING_RECORD( iface, struct host_names_iterator, IIterator_HostName_iface );
}

static HRESULT WINAPI host_names_iterator_QueryInterface( IIterator_HostName *iface, REFIID iid, void **out )
{
    struct host_names_iterator *impl = impl_from_IIterator_HostName( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IIterator_HostName ))
    {
        *out = &impl->IIterator_HostName_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IWeakReferenceSource ) ||
        IsEqualGUID( iid, &IID_ComWrappers_internal ) ||
        IsEqualGUID( iid, &IID_IManagedObjectTracker1 ) ||
        IsEqualGUID( iid, &IID_IManagedObjectTracker2 ))
    {
        *out = NULL;
        return E_NOINTERFACE;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI host_names_iterator_AddRef( IIterator_HostName *iface )
{
    struct host_names_iterator *impl = impl_from_IIterator_HostName( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI host_names_iterator_Release( IIterator_HostName *iface )
{
    struct host_names_iterator *impl = impl_from_IIterator_HostName( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );
    if (!ref)
        free( impl );
    return ref;
}

static HRESULT WINAPI host_names_iterator_GetIids( IIterator_HostName *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI host_names_iterator_GetRuntimeClassName( IIterator_HostName *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI host_names_iterator_GetTrustLevel( IIterator_HostName *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_POINTER;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI host_names_iterator_get_Current( IIterator_HostName *iface, IHostName **value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    *value = NULL;
    return 0x8000000b; /* E_BOUNDS */
}

static HRESULT WINAPI host_names_iterator_get_HasCurrent( IIterator_HostName *iface, boolean *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    *value = FALSE;
    return S_OK;
}

static HRESULT WINAPI host_names_iterator_MoveNext( IIterator_HostName *iface, boolean *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    *value = FALSE;
    return S_OK;
}

static HRESULT WINAPI host_names_iterator_GetMany( IIterator_HostName *iface, UINT32 items_size, IHostName **items, UINT32 *value )
{
    TRACE( "iface %p, items_size %u, items %p, value %p\n", iface, items_size, items, value );
    if (!value) return E_POINTER;
    *value = 0;
    return S_OK;
}

static const struct IIterator_HostNameVtbl host_names_iterator_vtbl =
{
    host_names_iterator_QueryInterface,
    host_names_iterator_AddRef,
    host_names_iterator_Release,
    /* IInspectable methods */
    host_names_iterator_GetIids,
    host_names_iterator_GetRuntimeClassName,
    host_names_iterator_GetTrustLevel,
    /* IIterator_HostName methods */
    host_names_iterator_get_Current,
    host_names_iterator_get_HasCurrent,
    host_names_iterator_MoveNext,
    host_names_iterator_GetMany,
};

static HRESULT host_names_iterator_create( IIterator_HostName **iterator )
{
    struct host_names_iterator *impl;

    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IIterator_HostName_iface.lpVtbl = &host_names_iterator_vtbl;
    impl->ref = 1;
    *iterator = &impl->IIterator_HostName_iface;
    return S_OK;
}

struct host_names_vector_view
{
    IVectorView_HostName IVectorView_HostName_iface;
    IIterable_HostName IIterable_HostName_iface;
    LONG ref;
};

static inline struct host_names_vector_view *impl_from_IVectorView_HostName( IVectorView_HostName *iface )
{
    return CONTAINING_RECORD( iface, struct host_names_vector_view, IVectorView_HostName_iface );
}

static inline struct host_names_vector_view *impl_from_IIterable_HostName( IIterable_HostName *iface )
{
    return CONTAINING_RECORD( iface, struct host_names_vector_view, IIterable_HostName_iface );
}

static HRESULT WINAPI host_names_vector_view_QueryInterface( IVectorView_HostName *iface, REFIID iid, void **out )
{
    struct host_names_vector_view *impl = impl_from_IVectorView_HostName( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IVectorView_HostName ))
    {
        *out = &impl->IVectorView_HostName_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IIterable_HostName ))
    {
        *out = &impl->IIterable_HostName_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IWeakReferenceSource ) ||
        IsEqualGUID( iid, &IID_ComWrappers_internal ) ||
        IsEqualGUID( iid, &IID_IManagedObjectTracker1 ) ||
        IsEqualGUID( iid, &IID_IManagedObjectTracker2 ))
    {
        *out = NULL;
        return E_NOINTERFACE;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI host_names_vector_view_AddRef( IVectorView_HostName *iface )
{
    struct host_names_vector_view *impl = impl_from_IVectorView_HostName( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI host_names_vector_view_Release( IVectorView_HostName *iface )
{
    struct host_names_vector_view *impl = impl_from_IVectorView_HostName( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );
    if (!ref)
        free( impl );
    return ref;
}

static HRESULT WINAPI host_names_vector_view_GetIids( IVectorView_HostName *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI host_names_vector_view_GetRuntimeClassName( IVectorView_HostName *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI host_names_vector_view_GetTrustLevel( IVectorView_HostName *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_POINTER;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI host_names_vector_view_GetAt( IVectorView_HostName *iface, UINT32 index, IHostName **value )
{
    TRACE( "iface %p, index %u, value %p\n", iface, index, value );
    if (!value) return E_POINTER;
    *value = NULL;
    return 0x8000000b; /* E_BOUNDS */
}

static HRESULT WINAPI host_names_vector_view_get_Size( IVectorView_HostName *iface, UINT32 *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    *value = 0;
    return S_OK;
}

static HRESULT WINAPI host_names_vector_view_IndexOf( IVectorView_HostName *iface, IHostName *element, UINT32 *index, BOOLEAN *value )
{
    TRACE( "iface %p, element %p, index %p, value %p\n", iface, element, index, value );
    if (!index || !value) return E_POINTER;
    *index = 0;
    *value = FALSE;
    return S_OK;
}

static HRESULT WINAPI host_names_vector_view_GetMany( IVectorView_HostName *iface, UINT32 start_index, UINT32 items_size, IHostName **items, UINT32 *value )
{
    TRACE( "iface %p, start_index %u, items_size %u, items %p, value %p\n", iface, start_index, items_size, items, value );
    if (!value) return E_POINTER;
    *value = 0;
    return S_OK;
}

static const struct IVectorView_HostNameVtbl host_names_vector_view_vtbl =
{
    host_names_vector_view_QueryInterface,
    host_names_vector_view_AddRef,
    host_names_vector_view_Release,
    /* IInspectable methods */
    host_names_vector_view_GetIids,
    host_names_vector_view_GetRuntimeClassName,
    host_names_vector_view_GetTrustLevel,
    /* IVectorView_HostName methods */
    host_names_vector_view_GetAt,
    host_names_vector_view_get_Size,
    host_names_vector_view_IndexOf,
    host_names_vector_view_GetMany,
};

static HRESULT WINAPI host_names_iterable_QueryInterface( IIterable_HostName *iface, REFIID iid, void **out )
{
    struct host_names_vector_view *impl = impl_from_IIterable_HostName( iface );
    return host_names_vector_view_QueryInterface( &impl->IVectorView_HostName_iface, iid, out );
}

static ULONG WINAPI host_names_iterable_AddRef( IIterable_HostName *iface )
{
    struct host_names_vector_view *impl = impl_from_IIterable_HostName( iface );
    return host_names_vector_view_AddRef( &impl->IVectorView_HostName_iface );
}

static ULONG WINAPI host_names_iterable_Release( IIterable_HostName *iface )
{
    struct host_names_vector_view *impl = impl_from_IIterable_HostName( iface );
    return host_names_vector_view_Release( &impl->IVectorView_HostName_iface );
}

static HRESULT WINAPI host_names_iterable_GetIids( IIterable_HostName *iface, ULONG *iid_count, IID **iids )
{
    struct host_names_vector_view *impl = impl_from_IIterable_HostName( iface );
    return host_names_vector_view_GetIids( &impl->IVectorView_HostName_iface, iid_count, iids );
}

static HRESULT WINAPI host_names_iterable_GetRuntimeClassName( IIterable_HostName *iface, HSTRING *class_name )
{
    struct host_names_vector_view *impl = impl_from_IIterable_HostName( iface );
    return host_names_vector_view_GetRuntimeClassName( &impl->IVectorView_HostName_iface, class_name );
}

static HRESULT WINAPI host_names_iterable_GetTrustLevel( IIterable_HostName *iface, TrustLevel *trust_level )
{
    struct host_names_vector_view *impl = impl_from_IIterable_HostName( iface );
    return host_names_vector_view_GetTrustLevel( &impl->IVectorView_HostName_iface, trust_level );
}

static HRESULT WINAPI host_names_iterable_First( IIterable_HostName *iface, IIterator_HostName **value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    return host_names_iterator_create( value );
}

static const struct IIterable_HostNameVtbl host_names_iterable_vtbl =
{
    host_names_iterable_QueryInterface,
    host_names_iterable_AddRef,
    host_names_iterable_Release,
    /* IInspectable methods */
    host_names_iterable_GetIids,
    host_names_iterable_GetRuntimeClassName,
    host_names_iterable_GetTrustLevel,
    /* IIterable_HostName methods */
    host_names_iterable_First,
};

static HRESULT host_names_vector_view_create( IVectorView_HostName **view )
{
    struct host_names_vector_view *impl;

    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IVectorView_HostName_iface.lpVtbl = &host_names_vector_view_vtbl;
    impl->IIterable_HostName_iface.lpVtbl = &host_names_iterable_vtbl;
    impl->ref = 1;
    *view = &impl->IVectorView_HostName_iface;
    return S_OK;
}

static ULONG WINAPI factory_AddRef( IActivationFactory *iface )
{
    struct network_information_statics *impl = impl_from_IActivationFactory( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI factory_Release( IActivationFactory *iface )
{
    struct network_information_statics *impl = impl_from_IActivationFactory( iface );
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

struct connection_profile
{
    IConnectionProfile IConnectionProfile_iface;
    IConnectionProfile2 IConnectionProfile2_iface;
    LONG ref;

    INetworkListManager *network_list_manager;
};

static inline struct connection_profile *impl_from_IConnectionProfile( IConnectionProfile *iface )
{
    return CONTAINING_RECORD( iface, struct connection_profile, IConnectionProfile_iface );
}

static HRESULT WINAPI connection_profile_QueryInterface( IConnectionProfile *iface, REFIID iid, void **out )
{
    struct connection_profile *impl = impl_from_IConnectionProfile( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IConnectionProfile ))
    {
        *out = &impl->IConnectionProfile_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IConnectionProfile2 ))
    {
        *out = &impl->IConnectionProfile2_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IWeakReferenceSource ) ||
        IsEqualGUID( iid, &IID_ComWrappers_internal ) ||
        IsEqualGUID( iid, &IID_IManagedObjectTracker1 ) ||
        IsEqualGUID( iid, &IID_IManagedObjectTracker2 ))
    {
        *out = NULL;
        return E_NOINTERFACE;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI connection_profile_AddRef( IConnectionProfile *iface )
{
    struct connection_profile *impl = impl_from_IConnectionProfile( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI connection_profile_Release( IConnectionProfile *iface )
{
    struct connection_profile *impl = impl_from_IConnectionProfile( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref)
    {
        INetworkListManager_Release( impl->network_list_manager );
        free( impl );
    }
    return ref;
}

static HRESULT WINAPI connection_profile_GetIids( IConnectionProfile *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI connection_profile_GetRuntimeClassName( IConnectionProfile *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI connection_profile_GetTrustLevel( IConnectionProfile *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI connection_profile_get_ProfileName( IConnectionProfile *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    return WindowsCreateString( L"Wine Network", 12, value );
}

static HRESULT WINAPI connection_profile_GetNetworkConnectivityLevel( IConnectionProfile *iface, NetworkConnectivityLevel *value )
{
    struct connection_profile *impl = impl_from_IConnectionProfile( iface );
    NetworkConnectivityLevel network_connectivity_level;
    NLM_CONNECTIVITY connectivity = 0xdeadbeef;
    const char *offline_env;
    HRESULT hr;

    TRACE( "iface %p, value %p\n", iface, value );

    if (!value) return E_POINTER;

    offline_env = getenv( "WINE_UWP_OFFLINE" );
    if (offline_env && *offline_env && strcmp( offline_env, "0" ) != 0)
    {
        TRACE( "WINE_UWP_OFFLINE is set, returning NetworkConnectivityLevel_None\n" );
        *value = NetworkConnectivityLevel_None;
        return S_OK;
    }

    if (FAILED(hr = INetworkListManager_GetConnectivity( impl->network_list_manager, &connectivity ))) return hr;

    if (connectivity == NLM_CONNECTIVITY_DISCONNECTED)
        network_connectivity_level = NetworkConnectivityLevel_None;
    else if (connectivity & ( NLM_CONNECTIVITY_IPV4_INTERNET | NLM_CONNECTIVITY_IPV6_INTERNET ))
        network_connectivity_level = NetworkConnectivityLevel_InternetAccess;
    else if (connectivity & ( NLM_CONNECTIVITY_IPV4_LOCALNETWORK | NLM_CONNECTIVITY_IPV6_LOCALNETWORK | NLM_CONNECTIVITY_IPV4_NOTRAFFIC | NLM_CONNECTIVITY_IPV6_NOTRAFFIC ))
        network_connectivity_level = NetworkConnectivityLevel_LocalAccess;
    else
        network_connectivity_level = NetworkConnectivityLevel_ConstrainedInternetAccess;

    *value = network_connectivity_level;
    return S_OK;
}

static HRESULT WINAPI connection_profile_GetNetworkNames( IConnectionProfile *iface, IVectorView_HSTRING **value )
{
    FIXME( "iface %p, value %p stub!\n", iface, value );
    return E_NOTIMPL;
}

static HRESULT WINAPI connection_profile_GetConnectionCost( IConnectionProfile *iface, IConnectionCost **value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    return connection_cost_create( value );
}

static HRESULT WINAPI connection_profile_GetDataPlanStatus( IConnectionProfile *iface, IDataPlanStatus **value )
{
    FIXME( "iface %p, value %p stub!\n", iface, value );
    return E_NOTIMPL;
}

static HRESULT WINAPI connection_profile_get_NetworkAdapter( IConnectionProfile *iface, INetworkAdapter **value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    return network_adapter_create( value );
}

static HRESULT WINAPI connection_profile_GetLocalUsage( IConnectionProfile *iface, DateTime start, DateTime end, IDataUsage **value )
{
    FIXME( "iface %p, start %I64d, end %I64d, value %p stub!\n", iface, start.UniversalTime, end.UniversalTime, value );
    return E_NOTIMPL;
}

static HRESULT WINAPI connection_profile_GetLocalUsagePerRoamingStates( IConnectionProfile *iface, DateTime start, DateTime end, RoamingStates states, IDataUsage **value )
{
    FIXME( "iface %p, start %I64d, end %I64d, states %u, value %p stub!\n", iface, start.UniversalTime, end.UniversalTime, states, value );
    return E_NOTIMPL;
}

static HRESULT WINAPI connection_profile_get_NetworkSecuritySettings( IConnectionProfile *iface, INetworkSecuritySettings **value )
{
    FIXME( "iface %p, value %p stub!\n", iface, value );
    return E_NOTIMPL;
}

static const struct IConnectionProfileVtbl connection_profile_vtbl =
{
    connection_profile_QueryInterface,
    connection_profile_AddRef,
    connection_profile_Release,
    /* IInspectable methods */
    connection_profile_GetIids,
    connection_profile_GetRuntimeClassName,
    connection_profile_GetTrustLevel,
    /* IConnectionProfile methods */
    connection_profile_get_ProfileName,
    connection_profile_GetNetworkConnectivityLevel,
    connection_profile_GetNetworkNames,
    connection_profile_GetConnectionCost,
    connection_profile_GetDataPlanStatus,
    connection_profile_get_NetworkAdapter,
    connection_profile_GetLocalUsage,
    connection_profile_GetLocalUsagePerRoamingStates,
    connection_profile_get_NetworkSecuritySettings,
};

static inline struct connection_profile *impl_from_IConnectionProfile2( IConnectionProfile2 *iface )
{
    return CONTAINING_RECORD( iface, struct connection_profile, IConnectionProfile2_iface );
}

static HRESULT WINAPI connection_profile2_QueryInterface( IConnectionProfile2 *iface, REFIID iid, void **out )
{
    struct connection_profile *impl = impl_from_IConnectionProfile2( iface );
    return IConnectionProfile_QueryInterface(&impl->IConnectionProfile_iface, iid, out);
}

static ULONG WINAPI connection_profile2_AddRef( IConnectionProfile2 *iface )
{
    struct connection_profile *impl = impl_from_IConnectionProfile2( iface );
    return IConnectionProfile_AddRef(&impl->IConnectionProfile_iface);
}

static ULONG WINAPI connection_profile2_Release( IConnectionProfile2 *iface )
{
    struct connection_profile *impl = impl_from_IConnectionProfile2( iface );
    return IConnectionProfile_Release(&impl->IConnectionProfile_iface);
}

static HRESULT WINAPI connection_profile2_GetIids( IConnectionProfile2 *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI connection_profile2_GetRuntimeClassName( IConnectionProfile2 *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI connection_profile2_GetTrustLevel( IConnectionProfile2 *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

HRESULT WINAPI connection_profile2_get_IsWwanConnectionProfile( IConnectionProfile2 *iface, boolean *value )
{
    FIXME( "iface %p, value %p stub!\n", iface, value );
    *value = 0;
    return S_OK;
}

HRESULT WINAPI connection_profile2_get_IsWlanConnectionProfile( IConnectionProfile2 *iface, boolean *value )
{
    FIXME( "iface %p, value %p stub!\n", iface, value );
    *value = 0;
    return S_OK;
}

HRESULT WINAPI connection_profile2_get_WwanConnectionProfileDetails( IConnectionProfile2 *iface, IWwanConnectionProfileDetails **value )
{
    FIXME( "iface %p, value %p stub!\n", iface, value );
    return E_NOTIMPL;
}

HRESULT WINAPI connection_profile2_get_WlanConnectionProfileDetails( IConnectionProfile2 *iface, IWlanConnectionProfileDetails **value )
{
    FIXME( "iface %p, value %p stub!\n", iface, value );
    return E_NOTIMPL;
}

HRESULT WINAPI connection_profile2_get_ServiceProviderGuid( IConnectionProfile2 *iface, IReference_GUID **value )
{
    FIXME( "iface %p, value %p stub!\n", iface, value );
    return E_NOTIMPL;
}

HRESULT WINAPI connection_profile2_GetSignalBars( IConnectionProfile2 *iface, IReference_BYTE **value )
{
    FIXME( "iface %p, value %p stub!\n", iface, value );
    return E_NOTIMPL;
}

HRESULT WINAPI connection_profile2_GetDomainConnectivityLevel( IConnectionProfile2 *iface, DomainConnectivityLevel *value )
{
    FIXME( "iface %p, value %p stub!\n", iface, value );
    return E_NOTIMPL;
}

HRESULT WINAPI connection_profile2_GetNetworkUsageAsync( IConnectionProfile2 *iface, DateTime time_start, DateTime time_end,
        DataUsageGranularity granularity, NetworkUsageStates states, IAsyncOperation_IVectorView_NetworkUsage **value )
{
    FIXME( "iface %p, time_start %I64d, time_end %I64d, granularity %d, states %d-%d, value %p stub!\n",
            iface, time_start.UniversalTime, time_end.UniversalTime, granularity, states.Roaming, states.Shared, value );
    return E_NOTIMPL;
}

HRESULT WINAPI connection_profile2_GetConnectivityIntervalsAsync( IConnectionProfile2 *iface, DateTime time_start, DateTime time_end,
        NetworkUsageStates states, IAsyncOperation_IVectorView_ConnectivityInterval **value )
{
    FIXME( "iface %p, time_start %I64d, time_end %I64d, states %d-%d, value %p stub!\n",
            iface, time_start.UniversalTime, time_end.UniversalTime, states.Roaming, states.Shared, value );
    return E_NOTIMPL;
}

static const struct IConnectionProfile2Vtbl connection_profile2_vtbl =
{
    connection_profile2_QueryInterface,
    connection_profile2_AddRef,
    connection_profile2_Release,
    /* IInspectable methods */
    connection_profile2_GetIids,
    connection_profile2_GetRuntimeClassName,
    connection_profile2_GetTrustLevel,
    /* IConnectionProfile2 methods */
    connection_profile2_get_IsWwanConnectionProfile,
    connection_profile2_get_IsWlanConnectionProfile,
    connection_profile2_get_WwanConnectionProfileDetails,
    connection_profile2_get_WlanConnectionProfileDetails,
    connection_profile2_get_ServiceProviderGuid,
    connection_profile2_GetSignalBars,
    connection_profile2_GetDomainConnectivityLevel,
    connection_profile2_GetNetworkUsageAsync,
    connection_profile2_GetConnectivityIntervalsAsync,
};

DEFINE_IINSPECTABLE( network_information_statics, INetworkInformationStatics, struct network_information_statics, IActivationFactory_iface )

static HRESULT WINAPI network_information_statics_GetConnectionProfiles( INetworkInformationStatics *iface, IVectorView_ConnectionProfile **value )
{
    FIXME( "iface %p, value %p stub!\n", iface, value );
    return E_NOTIMPL;
}

static HRESULT WINAPI network_information_statics_GetInternetConnectionProfile( INetworkInformationStatics *iface, IConnectionProfile **value )
{
    NetworkConnectivityLevel network_connectivity_level = 0xdeadbeef;
    INetworkListManager *network_list_manager = NULL;
    struct connection_profile *impl;
    HRESULT hr;

    TRACE( "iface %p, value %p\n", iface, value );

    *value = NULL;
    if (FAILED(hr = CoCreateInstance( &CLSID_NetworkListManager, NULL, CLSCTX_INPROC_SERVER, &IID_INetworkListManager, (void **)&network_list_manager ))) return hr;
    if (!(impl = calloc( 1, sizeof( *impl ) )))
    {
        INetworkListManager_Release( network_list_manager );
        return E_OUTOFMEMORY;
    }

    impl->IConnectionProfile_iface.lpVtbl = &connection_profile_vtbl;
    impl->IConnectionProfile2_iface.lpVtbl = &connection_profile2_vtbl;
    impl->ref = 1;
    impl->network_list_manager = network_list_manager;

    hr = IConnectionProfile_GetNetworkConnectivityLevel( &impl->IConnectionProfile_iface, &network_connectivity_level );
    if (FAILED(hr) || network_connectivity_level == NetworkConnectivityLevel_None)
    {
        free( impl );
        INetworkListManager_Release( network_list_manager );
        return hr;
    }

    *value = &impl->IConnectionProfile_iface;
    TRACE( "created IConnectionProfile %p.\n", *value );
    return hr;
}

static HRESULT WINAPI network_information_statics_GetLanIdentifiers( INetworkInformationStatics *iface, IVectorView_LanIdentifier **value )
{
    FIXME( "iface %p, value %p stub!\n", iface, value );
    return E_NOTIMPL;
}

static HRESULT WINAPI network_information_statics_GetHostNames( INetworkInformationStatics *iface, IVectorView_HostName **value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_POINTER;
    return host_names_vector_view_create( value );
}

static HRESULT WINAPI network_information_statics_GetProxyConfigurationAsync( INetworkInformationStatics *iface, IUriRuntimeClass *uri, IAsyncOperation_ProxyConfiguration **value )
{
    FIXME( "iface %p, uri %p, value %p stub!\n", iface, uri, value );
    return E_NOTIMPL;
}

static HRESULT WINAPI network_information_statics_GetSortedEndpointPairs( INetworkInformationStatics *iface, IIterable_EndpointPair *endpoint,
                                                                          HostNameSortOptions options, IVectorView_EndpointPair **value )
{
    FIXME( "iface %p, endpoint %p, options %d, value %p stub!\n", iface, endpoint, options, value );
    return E_NOTIMPL;
}

static HRESULT WINAPI network_information_statics_add_NetworkStatusChanged( INetworkInformationStatics *iface, INetworkStatusChangedEventHandler *handler,
                                                                            EventRegistrationToken *cookie )
{
    FIXME( "iface %p, handler %p, cookie %p stub!\n", iface, handler, cookie );
    *cookie = dummy_cookie;
    return S_OK;
}

static HRESULT WINAPI network_information_statics_remove_NetworkStatusChanged( INetworkInformationStatics *iface, EventRegistrationToken cookie )
{
    FIXME( "iface %p, cookie %#I64x stub!\n", iface, cookie.value );
    return S_OK;
}

static const struct INetworkInformationStaticsVtbl network_information_statics_vtbl =
{
    network_information_statics_QueryInterface,
    network_information_statics_AddRef,
    network_information_statics_Release,
    /* IInspectable methods */
    network_information_statics_GetIids,
    network_information_statics_GetRuntimeClassName,
    network_information_statics_GetTrustLevel,
    /* INetworkInformationStatics methods */
    network_information_statics_GetConnectionProfiles,
    network_information_statics_GetInternetConnectionProfile,
    network_information_statics_GetLanIdentifiers,
    network_information_statics_GetHostNames,
    network_information_statics_GetProxyConfigurationAsync,
    network_information_statics_GetSortedEndpointPairs,
    network_information_statics_add_NetworkStatusChanged,
    network_information_statics_remove_NetworkStatusChanged,
};

static struct network_information_statics network_information_statics =
{
    {&factory_vtbl},
    {&network_information_statics_vtbl},
    1,
};

IActivationFactory *network_information_factory = &network_information_statics.IActivationFactory_iface;
