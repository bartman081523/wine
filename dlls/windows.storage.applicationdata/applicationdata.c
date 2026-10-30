/* WinRT Windows.Storage.ApplicationData ApplicationData Implementation
 *
 * Copyright (C) 2023 Mohamad Al-Jaf
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

WINE_DEFAULT_DEBUG_CHANNEL(data);

/* kernelbase: package identity provides the Windows per-package folder layout. */
LONG WINAPI GetCurrentPackageFamilyName( UINT32 *length, WCHAR *name );

static HRESULT WINAPI application_data_folder_get( IApplicationData *iface, const WCHAR *kind_name,
        IStorageFolder **value );

static const GUID IID_ComWrappers_internal = {0x11d3b13a, 0x180e, 0x4789, {0xa8, 0xbe, 0x77, 0x12, 0x88, 0x28, 0x93, 0xe6}};
static HRESULT WINAPI redhawk_interop_stub(void *This, ...) { return S_OK; }

struct application_data_statics
{
    IActivationFactory IActivationFactory_iface;
    IApplicationDataStatics IApplicationDataStatics_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    LONG ref;
};

static inline struct application_data_statics *impl_from_IActivationFactory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct application_data_statics, IActivationFactory_iface );
}

static HRESULT WINAPI factory_QueryInterface( IActivationFactory *iface, REFIID iid, void **out );
static ULONG WINAPI factory_AddRef( IActivationFactory *iface );
static ULONG WINAPI factory_Release( IActivationFactory *iface );
static HRESULT WINAPI factory_GetRuntimeClassName( IActivationFactory *iface, HSTRING *class_name );
static HRESULT WINAPI factory_GetTrustLevel( IActivationFactory *iface, TrustLevel *trust_level );

static HRESULT WINAPI redhawk_statics_QI(void *This, REFIID riid, void **out)
{
    struct application_data_statics *impl = (struct application_data_statics *)((char *)This - offsetof(struct application_data_statics, IRedhawkInterop_iface));
    return factory_QueryInterface(&impl->IActivationFactory_iface, riid, out);
}
static ULONG WINAPI redhawk_statics_AddRef(void *This)
{
    struct application_data_statics *impl = (struct application_data_statics *)((char *)This - offsetof(struct application_data_statics, IRedhawkInterop_iface));
    return factory_AddRef(&impl->IActivationFactory_iface);
}
static ULONG WINAPI redhawk_statics_Release(void *This)
{
    struct application_data_statics *impl = (struct application_data_statics *)((char *)This - offsetof(struct application_data_statics, IRedhawkInterop_iface));
    return factory_Release(&impl->IActivationFactory_iface);
}

static const void *redhawk_statics_vtbl[32] = {
    (void *)redhawk_statics_QI,
    (void *)redhawk_statics_AddRef,
    (void *)redhawk_statics_Release,
    (void *)redhawk_interop_stub,
    (void *)factory_GetRuntimeClassName,
    (void *)factory_GetTrustLevel,
    (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub,
    (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub,
    (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub,
    (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub,
    (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub,
    (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub,
    (void *)redhawk_interop_stub, (void *)redhawk_interop_stub
};

static HRESULT WINAPI factory_QueryInterface( IActivationFactory *iface, REFIID iid, void **out )
{
    struct application_data_statics *impl = impl_from_IActivationFactory( iface );

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

    if (IsEqualGUID( iid, &IID_IApplicationDataStatics ))
    {
        *out = &impl->IApplicationDataStatics_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_ComWrappers_internal ))
    {
        *out = &impl->IRedhawkInterop_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI factory_AddRef( IActivationFactory *iface )
{
    struct application_data_statics *impl = impl_from_IActivationFactory( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI factory_Release( IActivationFactory *iface )
{
    struct application_data_statics *impl = impl_from_IActivationFactory( iface );
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
    static const WCHAR name[] = L"Windows.Storage.ApplicationData";
    return WindowsCreateString( name, ARRAY_SIZE(name) - 1, class_name );
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

struct application_data
{
    IApplicationData IApplicationData_iface;
    IApplicationData2 IApplicationData2_iface;
    IApplicationData3 IApplicationData3_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    LONG ref;
    IApplicationDataContainer *local_settings;
    IApplicationDataContainer *roaming_settings;
};

static inline struct application_data *impl_from_IApplicationData( IApplicationData *iface )
{
    return CONTAINING_RECORD( iface, struct application_data, IApplicationData_iface );
}

static HRESULT WINAPI application_data_QueryInterface( IApplicationData *iface, REFIID iid, void **out );
static ULONG WINAPI application_data_AddRef( IApplicationData *iface );
static ULONG WINAPI application_data_Release( IApplicationData *iface );
static HRESULT WINAPI application_data_GetRuntimeClassName( IApplicationData *iface, HSTRING *class_name );
static HRESULT WINAPI application_data_GetTrustLevel( IApplicationData *iface, TrustLevel *trust_level );

static HRESULT WINAPI redhawk_appdata_QI(void *This, REFIID riid, void **out)
{
    struct application_data *impl = (struct application_data *)((char *)This - offsetof(struct application_data, IRedhawkInterop_iface));
    return application_data_QueryInterface(&impl->IApplicationData_iface, riid, out);
}
static ULONG WINAPI redhawk_appdata_AddRef(void *This)
{
    struct application_data *impl = (struct application_data *)((char *)This - offsetof(struct application_data, IRedhawkInterop_iface));
    return application_data_AddRef(&impl->IApplicationData_iface);
}
static ULONG WINAPI redhawk_appdata_Release(void *This)
{
    struct application_data *impl = (struct application_data *)((char *)This - offsetof(struct application_data, IRedhawkInterop_iface));
    return application_data_Release(&impl->IApplicationData_iface);
}

static const void *redhawk_appdata_vtbl[32] = {
    (void *)redhawk_appdata_QI,
    (void *)redhawk_appdata_AddRef,
    (void *)redhawk_appdata_Release,
    (void *)redhawk_interop_stub,
    (void *)application_data_GetRuntimeClassName,
    (void *)application_data_GetTrustLevel,
    (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub,
    (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub,
    (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub,
    (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub,
    (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub,
    (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub,
    (void *)redhawk_interop_stub, (void *)redhawk_interop_stub
};

static HRESULT WINAPI application_data_QueryInterface( IApplicationData *iface, REFIID iid, void **out )
{
    struct application_data *impl = impl_from_IApplicationData( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IApplicationData ))
    {
        *out = &impl->IApplicationData_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IApplicationData2 ))
    {
        *out = &impl->IApplicationData2_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IApplicationData3 ))
    {
        *out = &impl->IApplicationData3_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_ComWrappers_internal ))
    {
        *out = &impl->IRedhawkInterop_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI application_data_AddRef( IApplicationData *iface )
{
    struct application_data *impl = impl_from_IApplicationData( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI application_data_Release( IApplicationData *iface )
{
    struct application_data *impl = impl_from_IApplicationData( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref)
    {
        if (impl->local_settings) IApplicationDataContainer_Release( impl->local_settings );
        if (impl->roaming_settings) IApplicationDataContainer_Release( impl->roaming_settings );
        free( impl );
    }
    return ref;
}

static HRESULT WINAPI application_data_GetIids( IApplicationData *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI application_data_GetRuntimeClassName( IApplicationData *iface, HSTRING *class_name )
{
    static const WCHAR name[] = L"Windows.Storage.ApplicationData";
    return WindowsCreateString( name, ARRAY_SIZE(name) - 1, class_name );
}

static HRESULT WINAPI application_data_GetTrustLevel( IApplicationData *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI application_data_get_Version( IApplicationData *iface, UINT32 *value )
{
    FIXME( "iface %p, value %p stub!\n", iface, value );
    return E_NOTIMPL;
}

static HRESULT WINAPI application_data_SetVersionAsync( IApplicationData *iface, UINT32 version, IApplicationDataSetVersionHandler *handler,
                                                        IAsyncAction **operation )
{
    FIXME( "iface %p, version %d, handler %p, operation %p stub!\n", iface, version, handler, operation );
    return E_NOTIMPL;
}

static HRESULT WINAPI application_data_ClearAllAsync( IApplicationData *iface, IAsyncAction **operation )
{
    FIXME( "iface %p, operation %p stub!\n", iface, operation );
    return E_NOTIMPL;
}

static HRESULT WINAPI application_data_ClearAsync( IApplicationData *iface, ApplicationDataLocality locality, IAsyncAction **operation )
{
    FIXME( "iface %p, %d locality, operation %p stub!\n", iface, locality, operation );
    return E_NOTIMPL;
}

struct application_data_container
{
    IApplicationDataContainer IApplicationDataContainer_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    LONG ref;
    WCHAR name[64];
    ApplicationDataLocality locality;
    IPropertySet *values;
};

static inline struct application_data_container *impl_from_IApplicationDataContainer( IApplicationDataContainer *iface )
{
    return CONTAINING_RECORD( iface, struct application_data_container, IApplicationDataContainer_iface );
}

static HRESULT WINAPI application_data_container_QueryInterface( IApplicationDataContainer *iface, REFIID iid, void **out );
static ULONG WINAPI application_data_container_AddRef( IApplicationDataContainer *iface );
static ULONG WINAPI application_data_container_Release( IApplicationDataContainer *iface );
static HRESULT WINAPI application_data_container_GetRuntimeClassName( IApplicationDataContainer *iface, HSTRING *class_name );
static HRESULT WINAPI application_data_container_GetTrustLevel( IApplicationDataContainer *iface, TrustLevel *trust_level );

static HRESULT WINAPI redhawk_container_QI(void *This, REFIID riid, void **out)
{
    struct application_data_container *impl = (struct application_data_container *)((char *)This - offsetof(struct application_data_container, IRedhawkInterop_iface));
    return application_data_container_QueryInterface(&impl->IApplicationDataContainer_iface, riid, out);
}
static ULONG WINAPI redhawk_container_AddRef(void *This)
{
    struct application_data_container *impl = (struct application_data_container *)((char *)This - offsetof(struct application_data_container, IRedhawkInterop_iface));
    return application_data_container_AddRef(&impl->IApplicationDataContainer_iface);
}
static ULONG WINAPI redhawk_container_Release(void *This)
{
    struct application_data_container *impl = (struct application_data_container *)((char *)This - offsetof(struct application_data_container, IRedhawkInterop_iface));
    return application_data_container_Release(&impl->IApplicationDataContainer_iface);
}

static const void *redhawk_container_vtbl[32] = {
    (void *)redhawk_container_QI,
    (void *)redhawk_container_AddRef,
    (void *)redhawk_container_Release,
    (void *)redhawk_interop_stub,
    (void *)application_data_container_GetRuntimeClassName,
    (void *)application_data_container_GetTrustLevel,
    (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub,
    (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub,
    (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub,
    (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub,
    (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub,
    (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub, (void *)redhawk_interop_stub,
    (void *)redhawk_interop_stub, (void *)redhawk_interop_stub
};

static HRESULT WINAPI application_data_container_QueryInterface( IApplicationDataContainer *iface, REFIID iid, void **out )
{
    struct application_data_container *impl = impl_from_IApplicationDataContainer( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IApplicationDataContainer ))
    {
        *out = &impl->IApplicationDataContainer_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_ComWrappers_internal ))
    {
        *out = &impl->IRedhawkInterop_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI application_data_container_AddRef( IApplicationDataContainer *iface )
{
    struct application_data_container *impl = impl_from_IApplicationDataContainer( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI application_data_container_Release( IApplicationDataContainer *iface )
{
    struct application_data_container *impl = impl_from_IApplicationDataContainer( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref)
    {
        if (impl->values) IPropertySet_Release( impl->values );
        free( impl );
    }
    return ref;
}

static HRESULT application_data_container_create( const WCHAR *name, ApplicationDataLocality locality, IApplicationDataContainer **out );

static HRESULT WINAPI application_data_container_GetIids( IApplicationDataContainer *iface, ULONG *iid_count, IID **iids )
{
    IID *array;

    TRACE( "iface %p, iid_count %p, iids %p.\n", iface, iid_count, iids );
    if (!iid_count || !iids) return E_INVALIDARG;

    if (!(array = CoTaskMemAlloc( 2 * sizeof(IID) ))) return E_OUTOFMEMORY;
    array[0] = IID_IApplicationDataContainer;
    array[1] = IID_IAgileObject;
    *iid_count = 2;
    *iids = array;
    return S_OK;
}

static HRESULT WINAPI application_data_container_GetRuntimeClassName( IApplicationDataContainer *iface, HSTRING *class_name )
{
    static const WCHAR name[] = L"Windows.Storage.ApplicationDataContainer";
    return WindowsCreateString( name, ARRAY_SIZE(name) - 1, class_name );
}

static HRESULT WINAPI application_data_container_GetTrustLevel( IApplicationDataContainer *iface, TrustLevel *trust_level )
{
    TRACE( "iface %p, trust_level %p.\n", iface, trust_level );
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI application_data_container_get_Name( IApplicationDataContainer *iface, HSTRING *value )
{
    struct application_data_container *impl = impl_from_IApplicationDataContainer( iface );
    return WindowsCreateString( impl->name, wcslen( impl->name ), value );
}

static HRESULT WINAPI application_data_container_get_Locality( IApplicationDataContainer *iface, ApplicationDataLocality *value )
{
    struct application_data_container *impl = impl_from_IApplicationDataContainer( iface );
    *value = impl->locality;
    return S_OK;
}

static HRESULT WINAPI application_data_container_get_Values( IApplicationDataContainer *iface, IPropertySet **value )
{
    struct application_data_container *impl = impl_from_IApplicationDataContainer( iface );

    TRACE( "iface %p, value %p.\n", iface, value );

    if (!value) return E_INVALIDARG;

    if (!impl->values)
    {
        static const WCHAR name[] = L"Windows.Foundation.Collections.PropertySet";
        HSTRING class_name;
        IInspectable *instance;
        HRESULT hr;

        hr = WindowsCreateString( name, ARRAY_SIZE(name) - 1, &class_name );
        if (FAILED(hr)) return hr;

        hr = RoActivateInstance( class_name, &instance );
        WindowsDeleteString( class_name );
        if (FAILED(hr))
        {
            ERR( "failed to activate PropertySet: %#lx\n", hr );
            return hr;
        }

        hr = IInspectable_QueryInterface( instance, &IID_IPropertySet, (void **)&impl->values );
        IInspectable_Release( instance );
        if (FAILED(hr))
        {
            ERR( "failed to query IPropertySet: %#lx\n", hr );
            return hr;
        }
    }

    *value = impl->values;
    IPropertySet_AddRef( *value );
    return S_OK;
}

static HRESULT WINAPI application_data_container_get_Containers( IApplicationDataContainer *iface,
                                                                 IMapView_HSTRING_ApplicationDataContainer **value )
{
    FIXME( "iface %p, value %p stub!\n", iface, value );
    if (value) *value = NULL;
    return E_NOTIMPL;
}

static HRESULT WINAPI application_data_container_CreateContainer( IApplicationDataContainer *iface, HSTRING name,
                                                                 ApplicationDataCreateDisposition disposition,
                                                                 IApplicationDataContainer **container )
{
    struct application_data_container *impl = impl_from_IApplicationDataContainer( iface );
    const WCHAR *str = WindowsGetStringRawBuffer( name, NULL );

    TRACE( "iface %p, name %s, disposition %d, container %p.\n", iface, debugstr_w(str), disposition, container );
    if (!container) return E_INVALIDARG;

    return application_data_container_create( str, impl->locality, container );
}

static HRESULT WINAPI application_data_container_DeleteContainer( IApplicationDataContainer *iface, HSTRING name )
{
    FIXME( "iface %p, name %p stub!\n", iface, name );
    return S_OK;
}

static const struct IApplicationDataContainerVtbl application_data_container_vtbl =
{
    /* IUnknown methods */
    application_data_container_QueryInterface,
    application_data_container_AddRef,
    application_data_container_Release,
    /* IInspectable methods */
    application_data_container_GetIids,
    application_data_container_GetRuntimeClassName,
    application_data_container_GetTrustLevel,
    /* IApplicationDataContainer methods */
    application_data_container_get_Name,
    application_data_container_get_Locality,
    application_data_container_get_Values,
    application_data_container_get_Containers,
    application_data_container_CreateContainer,
    application_data_container_DeleteContainer,
};

static HRESULT application_data_container_create( const WCHAR *name, ApplicationDataLocality locality, IApplicationDataContainer **out )
{
    struct application_data_container *impl;

    if (!out) return E_INVALIDARG;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;

    impl->IApplicationDataContainer_iface.lpVtbl = &application_data_container_vtbl;
    impl->IRedhawkInterop_iface.lpVtbl = redhawk_container_vtbl;
    impl->ref = 1;
    lstrcpynW( impl->name, name, ARRAY_SIZE(impl->name) );
    impl->locality = locality;

    *out = &impl->IApplicationDataContainer_iface;
    return S_OK;
}

static HRESULT WINAPI application_data_get_LocalSettings( IApplicationData *iface, IApplicationDataContainer **value )
{
    struct application_data *impl = impl_from_IApplicationData( iface );
    HRESULT hr;

    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_INVALIDARG;

    if (!impl->local_settings)
    {
        hr = application_data_container_create( L"LocalSettings", ApplicationDataLocality_Local, &impl->local_settings );
        if (FAILED(hr)) return hr;
    }

    *value = impl->local_settings;
    IApplicationDataContainer_AddRef( *value );
    return S_OK;
}

static HRESULT WINAPI application_data_get_RoamingSettings( IApplicationData *iface, IApplicationDataContainer **value )
{
    struct application_data *impl = impl_from_IApplicationData( iface );
    HRESULT hr;

    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_INVALIDARG;

    if (!impl->roaming_settings)
    {
        hr = application_data_container_create( L"RoamingSettings", ApplicationDataLocality_Roaming, &impl->roaming_settings );
        if (FAILED(hr)) return hr;
    }

    *value = impl->roaming_settings;
    IApplicationDataContainer_AddRef( *value );
    return S_OK;
}

static HRESULT WINAPI application_data_get_LocalFolder( IApplicationData *iface, IStorageFolder **value )
{
    return application_data_folder_get( iface, L"LocalState", value );
}

static HRESULT WINAPI application_data_get_RoamingFolder( IApplicationData *iface, IStorageFolder **value )
{
    return application_data_folder_get( iface, L"RoamingState", value );
}

static HRESULT WINAPI application_data_get_TemporaryFolder( IApplicationData *iface, IStorageFolder **value )
{
    return application_data_folder_get( iface, L"TemporaryState", value );
}

static HRESULT WINAPI application_data_add_DataChanged( IApplicationData *iface, ITypedEventHandler_ApplicationData_IInspectable *handler,
                                                        EventRegistrationToken *token )
{
    FIXME( "iface %p, handler %p, token %p stub!\n", iface, handler, token );
    return E_NOTIMPL;
}

static HRESULT WINAPI application_data_remove_DataChanged( IApplicationData *iface, EventRegistrationToken token )
{
    FIXME( "iface %p, token %#I64x stub!\n", iface, token.value );
    return E_NOTIMPL;
}

static HRESULT WINAPI application_data_SignalDataChanged( IApplicationData *iface )
{
    FIXME( "iface %p stub!\n", iface );
    return E_NOTIMPL;
}

static HRESULT WINAPI application_data_get_RoamingStorageQuota( IApplicationData *iface, UINT64 *value )
{
    FIXME( "iface %p, value %p stub!\n", iface, value );
    return E_NOTIMPL;
}

static const struct IApplicationDataVtbl application_data_vtbl =
{
    application_data_QueryInterface,
    application_data_AddRef,
    application_data_Release,
    /* IInspectable methods */
    application_data_GetIids,
    application_data_GetRuntimeClassName,
    application_data_GetTrustLevel,
    /* IApplicationData methods */
    application_data_get_Version,
    application_data_SetVersionAsync,
    application_data_ClearAllAsync,
    application_data_ClearAsync,
    application_data_get_LocalSettings,
    application_data_get_RoamingSettings,
    application_data_get_LocalFolder,
    application_data_get_RoamingFolder,
    application_data_get_TemporaryFolder,
    application_data_add_DataChanged,
    application_data_remove_DataChanged,
    application_data_SignalDataChanged,
    application_data_get_RoamingStorageQuota,
};

/* StorageFolder stand-in for the ApplicationData folder getters. Windows maps
 * LocalFolder/RoamingFolder/TemporaryFolder onto %LOCALAPPDATA%\Packages\
 * <family>\<LocalState|RoamingState|TemporaryState>; without package identity
 * the fallback is %LOCALAPPDATA%\<kind>. The folder is created on demand since
 * CsWinRT callers expect get_Path/get_Name (run66: get_LocalFolder
 * E_NOTIMPL -> NotImplementedException -> fail-fast). Async folder operations
 * stay stubs until a run shows the app calling them. */

static void append_path_component( WCHAR *buffer, const WCHAR *component, size_t buffer_size )
{
    size_t len = wcslen( buffer );

    if (len && buffer[len - 1] != '\\') buffer[len++] = '\\';
    lstrcpynW( buffer + len, component, buffer_size - len );
}

struct storage_file
{
    IStorageFile IStorageFile_iface;
    IStorageItem IStorageItem_iface;
    LONG ref;
    WCHAR path[MAX_PATH];
    WCHAR name[MAX_PATH];
    WCHAR file_type[32];
    WCHAR content_type[64];
};

static inline struct storage_file *storage_file_from_IStorageFile( IStorageFile *iface )
{
    return CONTAINING_RECORD( iface, struct storage_file, IStorageFile_iface );
}

static inline struct storage_file *storage_file_from_IStorageItem( IStorageItem *iface )
{
    return CONTAINING_RECORD( iface, struct storage_file, IStorageItem_iface );
}

static HRESULT WINAPI storage_file_QueryInterface( IStorageFile *iface, REFIID iid, void **out )
{
    struct storage_file *impl = storage_file_from_IStorageFile( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (!out) return E_POINTER;
    *out = NULL;

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IStorageFile ))
    {
        *out = &impl->IStorageFile_iface;
        IStorageFile_AddRef( iface );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IStorageItem ))
    {
        *out = &impl->IStorageItem_iface;
        IStorageFile_AddRef( iface );
        return S_OK;
    }

    TRACE( "iface %p, iid %s not implemented, returning E_NOINTERFACE.\n", iface, debugstr_guid( iid ) );
    return E_NOINTERFACE;
}

static ULONG WINAPI storage_file_AddRef( IStorageFile *iface )
{
    struct storage_file *impl = storage_file_from_IStorageFile( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI storage_file_Release( IStorageFile *iface )
{
    struct storage_file *impl = storage_file_from_IStorageFile( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );
    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI storage_file_GetIids( IStorageFile *iface, ULONG *iid_count, IID **iids )
{
    TRACE( "iface %p, iid_count %p, iids %p.\n", iface, iid_count, iids );
    if (iid_count) *iid_count = 0;
    if (iids) *iids = NULL;
    return S_OK;
}

static HRESULT WINAPI storage_file_GetRuntimeClassName( IStorageFile *iface, HSTRING *class_name )
{
    TRACE( "iface %p, class_name %p.\n", iface, class_name );
    return WindowsCreateString( L"Windows.Storage.StorageFile", ARRAY_SIZE(L"Windows.Storage.StorageFile") - 1, class_name );
}

static HRESULT WINAPI storage_file_GetTrustLevel( IStorageFile *iface, TrustLevel *trust_level )
{
    TRACE( "iface %p, trust_level %p.\n", iface, trust_level );
    if (trust_level) *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI storage_file_get_FileType( IStorageFile *iface, HSTRING *value )
{
    struct storage_file *impl = storage_file_from_IStorageFile( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_INVALIDARG;
    return WindowsCreateString( impl->file_type, wcslen(impl->file_type), value );
}

static HRESULT WINAPI storage_file_get_ContentType( IStorageFile *iface, HSTRING *value )
{
    struct storage_file *impl = storage_file_from_IStorageFile( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_INVALIDARG;
    return WindowsCreateString( impl->content_type, wcslen(impl->content_type), value );
}

static HRESULT WINAPI storage_file_OpenAsync( IStorageFile *iface, FileAccessMode mode,
    IAsyncOperation_IRandomAccessStream **operation )
{
    FIXME( "iface %p, mode %d, operation %p stub!\n", iface, mode, operation );
    if (operation) *operation = NULL;
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_OpenTransactedWriteAsync( IStorageFile *iface,
    IAsyncOperation_StorageStreamTransaction **operation )
{
    FIXME( "iface %p, operation %p stub!\n", iface, operation );
    if (operation) *operation = NULL;
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_CopyOverloadDefaultNameAndOptions( IStorageFile *iface, IStorageFolder *folder,
    IAsyncOperation_StorageFile **operation )
{
    FIXME( "iface %p, folder %p, operation %p stub!\n", iface, folder, operation );
    if (operation) *operation = NULL;
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_CopyOverloadDefaultOptions( IStorageFile *iface, IStorageFolder *folder,
    HSTRING name, IAsyncOperation_StorageFile **operation )
{
    FIXME( "iface %p, folder %p, name %s, operation %p stub!\n", iface, folder, debugstr_hstring(name), operation );
    if (operation) *operation = NULL;
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_CopyOverload( IStorageFile *iface, IStorageFolder *folder,
    HSTRING name, NameCollisionOption option, IAsyncOperation_StorageFile **operation )
{
    FIXME( "iface %p, folder %p, name %s, option %d, operation %p stub!\n", iface, folder, debugstr_hstring(name), option, operation );
    if (operation) *operation = NULL;
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_CopyAndReplaceAsync( IStorageFile *iface, IStorageFile *file,
    IAsyncAction **operation )
{
    FIXME( "iface %p, file %p, operation %p stub!\n", iface, file, operation );
    if (operation) *operation = NULL;
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_MoveOverloadDefaultNameAndOptions( IStorageFile *iface, IStorageFolder *folder,
    IAsyncAction **operation )
{
    FIXME( "iface %p, folder %p, operation %p stub!\n", iface, folder, operation );
    if (operation) *operation = NULL;
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_MoveOverloadDefaultOptions( IStorageFile *iface, IStorageFolder *folder,
    HSTRING name, IAsyncAction **operation )
{
    FIXME( "iface %p, folder %p, name %s, operation %p stub!\n", iface, folder, debugstr_hstring(name), operation );
    if (operation) *operation = NULL;
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_MoveOverload( IStorageFile *iface, IStorageFolder *folder,
    HSTRING name, NameCollisionOption option, IAsyncAction **operation )
{
    FIXME( "iface %p, folder %p, name %s, option %d, operation %p stub!\n", iface, folder, debugstr_hstring(name), option, operation );
    if (operation) *operation = NULL;
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_MoveAndReplaceAsync( IStorageFile *iface, IStorageFile *file,
    IAsyncAction **operation )
{
    FIXME( "iface %p, file %p, operation %p stub!\n", iface, file, operation );
    if (operation) *operation = NULL;
    return E_NOTIMPL;
}

static const struct IStorageFileVtbl storage_file_vtbl =
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

static HRESULT WINAPI storage_file_item_QueryInterface( IStorageItem *iface, REFIID iid, void **out )
{
    struct storage_file *impl = storage_file_from_IStorageItem( iface );
    return storage_file_QueryInterface( &impl->IStorageFile_iface, iid, out );
}

static ULONG WINAPI storage_file_item_AddRef( IStorageItem *iface )
{
    struct storage_file *impl = storage_file_from_IStorageItem( iface );
    return storage_file_AddRef( &impl->IStorageFile_iface );
}

static ULONG WINAPI storage_file_item_Release( IStorageItem *iface )
{
    struct storage_file *impl = storage_file_from_IStorageItem( iface );
    return storage_file_Release( &impl->IStorageFile_iface );
}

static HRESULT WINAPI storage_file_item_GetIids( IStorageItem *iface, ULONG *iid_count, IID **iids )
{
    return storage_file_GetIids( NULL, iid_count, iids );
}

static HRESULT WINAPI storage_file_item_GetRuntimeClassName( IStorageItem *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Storage.IStorageItem", ARRAY_SIZE(L"Windows.Storage.IStorageItem") - 1, class_name );
}

static HRESULT WINAPI storage_file_item_GetTrustLevel( IStorageItem *iface, TrustLevel *trust_level )
{
    if (trust_level) *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI storage_file_item_RenameAsyncOverloadDefaultOptions( IStorageItem *iface, HSTRING name, IAsyncAction **operation )
{
    FIXME( "iface %p, name %s, operation %p stub!\n", iface, debugstr_hstring(name), operation );
    if (operation) *operation = NULL;
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_item_RenameAsync( IStorageItem *iface, HSTRING name, NameCollisionOption option, IAsyncAction **operation )
{
    FIXME( "iface %p, name %s, option %d, operation %p stub!\n", iface, debugstr_hstring(name), option, operation );
    if (operation) *operation = NULL;
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_item_DeleteAsyncOverloadDefaultOptions( IStorageItem *iface, IAsyncAction **operation )
{
    FIXME( "iface %p, operation %p stub!\n", iface, operation );
    if (operation) *operation = NULL;
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_item_DeleteAsync( IStorageItem *iface, StorageDeleteOption option, IAsyncAction **operation )
{
    FIXME( "iface %p, option %d, operation %p stub!\n", iface, option, operation );
    if (operation) *operation = NULL;
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_item_GetBasicPropertiesAsync( IStorageItem *iface,
    IAsyncOperation_BasicProperties **operation )
{
    FIXME( "iface %p, operation %p stub!\n", iface, operation );
    if (operation) *operation = NULL;
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_item_get_Name( IStorageItem *iface, HSTRING *value )
{
    struct storage_file *impl = storage_file_from_IStorageItem( iface );
    TRACE( "iface %p, value %p -> %s\n", iface, value, debugstr_w(impl->name) );
    if (!value) return E_INVALIDARG;
    return WindowsCreateString( impl->name, wcslen(impl->name), value );
}

static HRESULT WINAPI storage_file_item_get_Path( IStorageItem *iface, HSTRING *value )
{
    struct storage_file *impl = storage_file_from_IStorageItem( iface );
    TRACE( "iface %p, value %p -> %s\n", iface, value, debugstr_w(impl->path) );
    if (!value) return E_INVALIDARG;
    return WindowsCreateString( impl->path, wcslen(impl->path), value );
}

static HRESULT WINAPI storage_file_item_get_Attributes( IStorageItem *iface, FileAttributes *value )
{
    struct storage_file *impl = storage_file_from_IStorageItem( iface );
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

static const struct IStorageItemVtbl storage_file_item_vtbl =
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

static HRESULT storage_file_create( const WCHAR *path, IStorageFile **out )
{
    struct storage_file *impl;
    const WCHAR *p, *ext;

    if (!out) return E_INVALIDARG;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;

    impl->IStorageFile_iface.lpVtbl = &storage_file_vtbl;
    impl->IStorageItem_iface.lpVtbl = &storage_file_item_vtbl;
    impl->ref = 1;

    lstrcpynW( impl->path, path, ARRAY_SIZE(impl->path) );
    p = wcsrchr( impl->path, '\\' );
    if (p) lstrcpynW( impl->name, p + 1, ARRAY_SIZE(impl->name) );
    else lstrcpynW( impl->name, impl->path, ARRAY_SIZE(impl->name) );

    ext = wcsrchr( impl->name, '.' );
    if (ext) lstrcpynW( impl->file_type, ext, ARRAY_SIZE(impl->file_type) );
    else lstrcpynW( impl->file_type, L"", ARRAY_SIZE(impl->file_type) );

    if (!wcsicmp( impl->file_type, L".png" ))
        lstrcpynW( impl->content_type, L"image/png", ARRAY_SIZE(impl->content_type) );
    else if (!wcsicmp( impl->file_type, L".jpg" ) || !wcsicmp( impl->file_type, L".jpeg" ))
        lstrcpynW( impl->content_type, L"image/jpeg", ARRAY_SIZE(impl->content_type) );
    else if (!wcsicmp( impl->file_type, L".json" ))
        lstrcpynW( impl->content_type, L"application/json", ARRAY_SIZE(impl->content_type) );
    else
        lstrcpynW( impl->content_type, L"application/octet-stream", ARRAY_SIZE(impl->content_type) );

    *out = &impl->IStorageFile_iface;
    TRACE( "created StorageFile %p: path %s, name %s\n", *out, debugstr_w(impl->path), debugstr_w(impl->name) );
    return S_OK;
}

struct async_storage_file
{
    IAsyncOperation_StorageFile IAsyncOperation_StorageFile_iface;
    IAsyncInfo IAsyncInfo_iface;
    LONG ref;
    IStorageFile *result;
    HRESULT hr;
    AsyncStatus status;
    IAsyncOperationCompletedHandler_StorageFile *handler;
};

static inline struct async_storage_file *impl_from_IAsyncOperation_StorageFile( IAsyncOperation_StorageFile *iface )
{
    return CONTAINING_RECORD( iface, struct async_storage_file, IAsyncOperation_StorageFile_iface );
}

static inline struct async_storage_file *impl_from_async_storage_file_IAsyncInfo( IAsyncInfo *iface )
{
    return CONTAINING_RECORD( iface, struct async_storage_file, IAsyncInfo_iface );
}

static HRESULT WINAPI async_storage_file_QueryInterface( IAsyncOperation_StorageFile *iface, REFIID iid, void **out )
{
    struct async_storage_file *impl = impl_from_IAsyncOperation_StorageFile( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );
    if (!out) return E_POINTER;
    *out = NULL;

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAsyncOperation_StorageFile ))
    {
        *out = &impl->IAsyncOperation_StorageFile_iface;
        IAsyncOperation_StorageFile_AddRef( iface );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IAsyncInfo ))
    {
        *out = &impl->IAsyncInfo_iface;
        IAsyncOperation_StorageFile_AddRef( iface );
        return S_OK;
    }

    TRACE( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    return E_NOINTERFACE;
}

static ULONG WINAPI async_storage_file_AddRef( IAsyncOperation_StorageFile *iface )
{
    struct async_storage_file *impl = impl_from_IAsyncOperation_StorageFile( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI async_storage_file_Release( IAsyncOperation_StorageFile *iface )
{
    struct async_storage_file *impl = impl_from_IAsyncOperation_StorageFile( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );
    if (!ref)
    {
        if (impl->result) IStorageFile_Release( impl->result );
        if (impl->handler) IAsyncOperationCompletedHandler_StorageFile_Release( impl->handler );
        free( impl );
    }
    return ref;
}

static HRESULT WINAPI async_storage_file_GetIids( IAsyncOperation_StorageFile *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    if (iids) *iids = NULL;
    return S_OK;
}

static HRESULT WINAPI async_storage_file_GetRuntimeClassName( IAsyncOperation_StorageFile *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.IAsyncOperation`1<Windows.Storage.StorageFile>",
                                ARRAY_SIZE(L"Windows.Foundation.IAsyncOperation`1<Windows.Storage.StorageFile>") - 1,
                                class_name );
}

static HRESULT WINAPI async_storage_file_GetTrustLevel( IAsyncOperation_StorageFile *iface, TrustLevel *trust_level )
{
    if (trust_level) *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI async_storage_file_put_Completed( IAsyncOperation_StorageFile *iface,
    IAsyncOperationCompletedHandler_StorageFile *handler )
{
    struct async_storage_file *impl = impl_from_IAsyncOperation_StorageFile( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );
    impl->handler = handler;
    if (handler)
    {
        IAsyncOperationCompletedHandler_StorageFile_AddRef( handler );
        IAsyncOperationCompletedHandler_StorageFile_Invoke( handler, iface, impl->status );
    }
    return S_OK;
}

static HRESULT WINAPI async_storage_file_get_Completed( IAsyncOperation_StorageFile *iface,
    IAsyncOperationCompletedHandler_StorageFile **handler )
{
    struct async_storage_file *impl = impl_from_IAsyncOperation_StorageFile( iface );
    if (!handler) return E_INVALIDARG;
    *handler = impl->handler;
    if (*handler) IAsyncOperationCompletedHandler_StorageFile_AddRef( *handler );
    return S_OK;
}

static HRESULT WINAPI async_storage_file_GetResults( IAsyncOperation_StorageFile *iface,
    IStorageFile **results )
{
    struct async_storage_file *impl = impl_from_IAsyncOperation_StorageFile( iface );
    TRACE( "iface %p, results %p, hr %#lx, status %d.\n", iface, results, impl->hr, impl->status );
    if (FAILED(impl->hr))
    {
        if (results) *results = NULL;
        return impl->hr;
    }
    if (!results) return E_INVALIDARG;
    *results = impl->result;
    if (*results) IStorageFile_AddRef( *results );
    return S_OK;
}

static const struct IAsyncOperation_StorageFileVtbl async_storage_file_vtbl =
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
    return async_storage_file_GetIids( NULL, iid_count, iids );
}

static HRESULT WINAPI async_storage_file_info_GetRuntimeClassName( IAsyncInfo *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.IAsyncInfo", ARRAY_SIZE(L"Windows.Foundation.IAsyncInfo") - 1, class_name );
}

static HRESULT WINAPI async_storage_file_info_GetTrustLevel( IAsyncInfo *iface, TrustLevel *trust_level )
{
    if (trust_level) *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI async_storage_file_info_get_Id( IAsyncInfo *iface, UINT32 *id )
{
    if (!id) return E_INVALIDARG;
    *id = 1;
    return S_OK;
}

static HRESULT WINAPI async_storage_file_info_get_Status( IAsyncInfo *iface, AsyncStatus *status )
{
    struct async_storage_file *impl = impl_from_async_storage_file_IAsyncInfo( iface );
    if (!status) return E_INVALIDARG;
    *status = impl->status;
    return S_OK;
}

static HRESULT WINAPI async_storage_file_info_get_ErrorCode( IAsyncInfo *iface, HRESULT *error_code )
{
    struct async_storage_file *impl = impl_from_async_storage_file_IAsyncInfo( iface );
    if (!error_code) return E_INVALIDARG;
    *error_code = impl->hr;
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

static HRESULT async_storage_file_create( IStorageFile *file, HRESULT hr, AsyncStatus status, IAsyncOperation_StorageFile **out )
{
    struct async_storage_file *impl;

    if (!out) return E_INVALIDARG;
    *out = NULL;

    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;

    impl->IAsyncOperation_StorageFile_iface.lpVtbl = &async_storage_file_vtbl;
    impl->IAsyncInfo_iface.lpVtbl = &async_storage_file_info_vtbl;
    impl->ref = 1;
    impl->result = file;
    if (file) IStorageFile_AddRef( file );
    impl->hr = hr;
    impl->status = status;

    *out = &impl->IAsyncOperation_StorageFile_iface;
    TRACE( "created async_storage_file %p: file %p, hr %#lx, status %d\n", *out, file, hr, status );
    return S_OK;
}

struct storage_folder
{
    IStorageFolder IStorageFolder_iface;
    IStorageItem IStorageItem_iface;
    LONG ref;
    WCHAR name[64];
    WCHAR path[MAX_PATH];
};

static inline struct storage_folder *impl_from_IStorageFolder( IStorageFolder *iface )
{
    return CONTAINING_RECORD( iface, struct storage_folder, IStorageFolder_iface );
}

static HRESULT WINAPI storage_folder_QueryInterface( IStorageFolder *iface, REFIID iid, void **out )
{
    struct storage_folder *impl = impl_from_IStorageFolder( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IStorageFolder ))
    {
        *out = &impl->IStorageFolder_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IStorageItem ))
    {
        *out = &impl->IStorageItem_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "iface %p, iid %s not implemented, returning E_NOINTERFACE.\n", iface, debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI storage_folder_AddRef( IStorageFolder *iface )
{
    struct storage_folder *impl = impl_from_IStorageFolder( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI storage_folder_Release( IStorageFolder *iface )
{
    struct storage_folder *impl = impl_from_IStorageFolder( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );
    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI storage_folder_GetIids( IStorageFolder *iface, ULONG *iid_count, IID **iids )
{
    TRACE( "iface %p, iid_count %p, iids %p.\n", iface, iid_count, iids );
   *iid_count = 0;
    *iids = NULL;
    return S_OK;
}

static HRESULT WINAPI storage_folder_GetRuntimeClassName( IStorageFolder *iface, HSTRING *class_name )
{
    TRACE( "iface %p, class_name %p.\n", iface, class_name );
    return WindowsCreateString( RuntimeClass_Windows_Storage_StorageFolder,
                                wcslen( RuntimeClass_Windows_Storage_StorageFolder ), class_name );
}

static HRESULT WINAPI storage_folder_GetTrustLevel( IStorageFolder *iface, TrustLevel *trust_level )
{
    TRACE( "iface %p, trust_level %p.\n", iface, trust_level );
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI storage_folder_CreateFileAsync(
        IStorageFolder *iface, HSTRING name, CreationCollisionOption options,
        IAsyncOperation_StorageFile **operation )
{
    struct storage_folder *impl = impl_from_IStorageFolder( iface );
    const WCHAR *filename = WindowsGetStringRawBuffer( name, NULL );
    WCHAR full_path[MAX_PATH];
    DWORD disposition;
    HANDLE hfile;
    IStorageFile *file = NULL;
    HRESULT hr;

    TRACE( "iface %p, name %s, options %d, operation %p.\n", iface, debugstr_hstring( name ), options, operation );
    if (!operation) return E_INVALIDARG;
    *operation = NULL;

    if (!filename || !*filename) return E_INVALIDARG;

    lstrcpynW( full_path, impl->path, ARRAY_SIZE(full_path) );
    append_path_component( full_path, filename, ARRAY_SIZE(full_path) );

    switch (options)
    {
    case CreationCollisionOption_ReplaceExisting:
        disposition = CREATE_ALWAYS;
        break;
    case CreationCollisionOption_FailIfExists:
        disposition = CREATE_NEW;
        break;
    case CreationCollisionOption_OpenIfExists:
    default:
        disposition = OPEN_ALWAYS;
        break;
    }

    hfile = CreateFileW( full_path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
                         NULL, disposition, FILE_ATTRIBUTE_NORMAL, NULL );
    if (hfile == INVALID_HANDLE_VALUE)
    {
        DWORD err = GetLastError();
        TRACE( "failed to create/open file %s: %lu\n", debugstr_w(full_path), err );
        return async_storage_file_create( NULL, HRESULT_FROM_WIN32(err), Error, operation );
    }
    CloseHandle( hfile );

    hr = storage_file_create( full_path, &file );
    if (FAILED(hr)) return hr;
    hr = async_storage_file_create( file, S_OK, Completed, operation );
    IStorageFile_Release( file );
    return hr;
}

static HRESULT WINAPI storage_folder_CreateFileAsyncOverloadDefaultOptions(
        IStorageFolder *iface, HSTRING name, IAsyncOperation_StorageFile **operation )
{
    return storage_folder_CreateFileAsync( iface, name, CreationCollisionOption_FailIfExists, operation );
}

static HRESULT WINAPI storage_folder_CreateFolderAsyncOverloadDefaultOptions(
        IStorageFolder *iface, HSTRING name, IAsyncOperation_StorageFolder **operation )
{
    FIXME( "iface %p, name %s, operation %p stub!\n", iface, debugstr_hstring( name ), operation );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_folder_CreateFolderAsync(
        IStorageFolder *iface, HSTRING name, CreationCollisionOption options,
        IAsyncOperation_StorageFolder **operation )
{
    FIXME( "iface %p, name %s, options %d, operation %p stub!\n", iface, debugstr_hstring( name ), options, operation );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_folder_GetFileAsync( IStorageFolder *iface, HSTRING name,
        IAsyncOperation_StorageFile **operation )
{
    struct storage_folder *impl = impl_from_IStorageFolder( iface );
    const WCHAR *filename = WindowsGetStringRawBuffer( name, NULL );
    WCHAR full_path[MAX_PATH];
    DWORD attr;

    TRACE( "iface %p, name %s, operation %p.\n", iface, debugstr_hstring( name ), operation );
    if (!operation) return E_INVALIDARG;
    *operation = NULL;

    if (!filename || !*filename) return E_INVALIDARG;

    lstrcpynW( full_path, impl->path, ARRAY_SIZE(full_path) );
    append_path_component( full_path, filename, ARRAY_SIZE(full_path) );

    attr = GetFileAttributesW( full_path );
    if (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY))
    {
        IStorageFile *file = NULL;
        HRESULT hr = storage_file_create( full_path, &file );
        if (FAILED(hr)) return hr;
        hr = async_storage_file_create( file, S_OK, Completed, operation );
        IStorageFile_Release( file );
        return hr;
    }

    TRACE( "file %s not found at %s, returning async error 0x80070002.\n", debugstr_w(filename), debugstr_w(full_path) );
    return async_storage_file_create( NULL, HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND), Error, operation );
}

static HRESULT WINAPI storage_folder_GetFolderAsync( IStorageFolder *iface, HSTRING name,
        IAsyncOperation_StorageFolder **operation )
{
    FIXME( "iface %p, name %s, operation %p stub!\n", iface, debugstr_hstring( name ), operation );
    return async_storage_file_create( NULL, HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND), Error, (IAsyncOperation_StorageFile **)operation );
}

static HRESULT WINAPI storage_folder_GetItemAsync( IStorageFolder *iface, HSTRING name,
        IAsyncOperation_IStorageItem **operation )
{
    FIXME( "iface %p, name %s, operation %p stub!\n", iface, debugstr_hstring( name ), operation );
    return async_storage_file_create( NULL, HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND), Error, (IAsyncOperation_StorageFile **)operation );
}

struct empty_vector_view
{
    IInspectable IInspectable_iface;
    LONG ref;
};

static inline struct empty_vector_view *impl_from_empty_vector_view( void *iface )
{
    return CONTAINING_RECORD( iface, struct empty_vector_view, IInspectable_iface );
}

static HRESULT WINAPI empty_vector_view_QueryInterface( void *iface, REFIID iid, void **out )
{
    struct empty_vector_view *impl = impl_from_empty_vector_view( iface );
    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (!out) return E_POINTER;
    *out = NULL;

    *out = &impl->IInspectable_iface;
    InterlockedIncrement( &impl->ref );
    return S_OK;
}

static ULONG WINAPI empty_vector_view_AddRef( void *iface )
{
    struct empty_vector_view *impl = impl_from_empty_vector_view( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI empty_vector_view_Release( void *iface )
{
    struct empty_vector_view *impl = impl_from_empty_vector_view( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI empty_vector_view_GetIids( void *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI empty_vector_view_GetRuntimeClassName( void *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.Collections.IVectorView`1<IInspectable>", 55, class_name );
}

static HRESULT WINAPI empty_vector_view_GetTrustLevel( void *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI empty_vector_view_GetAt( void *This, UINT32 index, void **item )
{
    if (item) *item = NULL;
    return E_BOUNDS;
}

static HRESULT WINAPI empty_vector_view_get_Size( void *This, UINT32 *size )
{
    if (!size) return E_POINTER;
    *size = 0;
    return S_OK;
}

static HRESULT WINAPI empty_vector_view_IndexOf( void *This, void *item, UINT32 *index, boolean *found )
{
    if (index) *index = 0;
    if (found) *found = FALSE;
    return S_OK;
}

static HRESULT WINAPI empty_vector_view_GetMany( void *This, UINT32 startIndex, UINT32 capacity, void **items, UINT32 *actual )
{
    if (actual) *actual = 0;
    return S_OK;
}

static const void *empty_vector_view_vtbl[] =
{
    (void *)empty_vector_view_QueryInterface,
    (void *)empty_vector_view_AddRef,
    (void *)empty_vector_view_Release,
    (void *)empty_vector_view_GetIids,
    (void *)empty_vector_view_GetRuntimeClassName,
    (void *)empty_vector_view_GetTrustLevel,
    (void *)empty_vector_view_GetAt,
    (void *)empty_vector_view_get_Size,
    (void *)empty_vector_view_IndexOf,
    (void *)empty_vector_view_GetMany
};

static HRESULT empty_vector_view_create( void **out )
{
    struct empty_vector_view *impl;
    if (!out) return E_POINTER;
    *out = NULL;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IInspectable_iface.lpVtbl = (const struct IInspectableVtbl *)empty_vector_view_vtbl;
    impl->ref = 1;
    *out = &impl->IInspectable_iface;
    return S_OK;
}

struct async_vector_view
{
    IInspectable IInspectable_iface;
    IAsyncInfo IAsyncInfo_iface;
    LONG ref;
    void *view;
    IInspectable *handler;
};

static inline struct async_vector_view *impl_from_async_vector_view( void *iface )
{
    return CONTAINING_RECORD( iface, struct async_vector_view, IInspectable_iface );
}

static inline struct async_vector_view *impl_from_async_vector_view_IAsyncInfo( IAsyncInfo *iface )
{
    return CONTAINING_RECORD( iface, struct async_vector_view, IAsyncInfo_iface );
}

static HRESULT WINAPI async_vector_view_QueryInterface( void *iface, REFIID iid, void **out )
{
    struct async_vector_view *impl = impl_from_async_vector_view( iface );
    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (!out) return E_POINTER;
    *out = NULL;

    if (IsEqualGUID( iid, &IID_IAsyncInfo ))
    {
        *out = &impl->IAsyncInfo_iface;
        InterlockedIncrement( &impl->ref );
        return S_OK;
    }

    *out = &impl->IInspectable_iface;
    InterlockedIncrement( &impl->ref );
    return S_OK;
}

static ULONG WINAPI async_vector_view_AddRef( void *iface )
{
    struct async_vector_view *impl = impl_from_async_vector_view( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI async_vector_view_Release( void *iface )
{
    struct async_vector_view *impl = impl_from_async_vector_view( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    if (!ref)
    {
        if (impl->handler) IInspectable_Release( impl->handler );
        if (impl->view) IInspectable_Release( (IInspectable *)impl->view );
        free( impl );
    }
    return ref;
}

static HRESULT WINAPI async_vector_view_GetIids( void *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI async_vector_view_GetRuntimeClassName( void *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.IAsyncOperation`1<IVectorView>", 48, class_name );
}

static HRESULT WINAPI async_vector_view_GetTrustLevel( void *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI async_vector_view_put_Completed( void *iface, void *handler )
{
    struct async_vector_view *impl = impl_from_async_vector_view( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );

    if (impl->handler) IInspectable_Release( impl->handler );
    if ((impl->handler = handler)) IInspectable_AddRef( (IInspectable *)handler );

    if (handler)
    {
        HRESULT (STDMETHODCALLTYPE *invoke)(void *This, void *info, AsyncStatus status) =
            ((void **)*(void **)handler)[3];
        invoke( handler, &impl->IInspectable_iface, Completed );
    }

    return S_OK;
}

static HRESULT WINAPI async_vector_view_get_Completed( void *iface, void **handler )
{
    struct async_vector_view *impl = impl_from_async_vector_view( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );
    if (!handler) return E_POINTER;
    *handler = impl->handler;
    if (*handler) IInspectable_AddRef( (IInspectable *)*handler );
    return S_OK;
}

static HRESULT WINAPI async_vector_view_GetResults( void *This, void **results )
{
    struct async_vector_view *impl = impl_from_async_vector_view( This );
    TRACE( "This %p, results %p.\n", This, results );
    if (results)
    {
        *results = impl->view;
        if (*results) IInspectable_AddRef( (IInspectable *)*results );
    }
    return S_OK;
}

static const void *async_vector_view_vtbl[] =
{
    (void *)async_vector_view_QueryInterface,
    (void *)async_vector_view_AddRef,
    (void *)async_vector_view_Release,
    (void *)async_vector_view_GetIids,
    (void *)async_vector_view_GetRuntimeClassName,
    (void *)async_vector_view_GetTrustLevel,
    (void *)async_vector_view_put_Completed,
    (void *)async_vector_view_get_Completed,
    (void *)async_vector_view_GetResults
};

static HRESULT WINAPI async_vector_view_info_QueryInterface( IAsyncInfo *iface, REFIID iid, void **out )
{
    struct async_vector_view *impl = impl_from_async_vector_view_IAsyncInfo( iface );
    return async_vector_view_QueryInterface( &impl->IInspectable_iface, iid, out );
}

static ULONG WINAPI async_vector_view_info_AddRef( IAsyncInfo *iface )
{
    struct async_vector_view *impl = impl_from_async_vector_view_IAsyncInfo( iface );
    return async_vector_view_AddRef( &impl->IInspectable_iface );
}

static ULONG WINAPI async_vector_view_info_Release( IAsyncInfo *iface )
{
    struct async_vector_view *impl = impl_from_async_vector_view_IAsyncInfo( iface );
    return async_vector_view_Release( &impl->IInspectable_iface );
}

static HRESULT WINAPI async_vector_view_info_GetIids( IAsyncInfo *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI async_vector_view_info_GetRuntimeClassName( IAsyncInfo *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.IAsyncInfo", 27, class_name );
}

static HRESULT WINAPI async_vector_view_info_GetTrustLevel( IAsyncInfo *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI async_vector_view_info_get_Id( IAsyncInfo *iface, UINT32 *id )
{
    if (!id) return E_POINTER;
    *id = 1;
    return S_OK;
}

static HRESULT WINAPI async_vector_view_info_get_Status( IAsyncInfo *iface, AsyncStatus *status )
{
    if (!status) return E_POINTER;
    *status = Completed;
    return S_OK;
}

static HRESULT WINAPI async_vector_view_info_get_ErrorCode( IAsyncInfo *iface, HRESULT *error_code )
{
    if (!error_code) return E_POINTER;
    *error_code = S_OK;
    return S_OK;
}

static HRESULT WINAPI async_vector_view_info_Cancel( IAsyncInfo *iface )
{
    return S_OK;
}

static HRESULT WINAPI async_vector_view_info_Close( IAsyncInfo *iface )
{
    return S_OK;
}

static const struct IAsyncInfoVtbl async_vector_view_info_vtbl =
{
    async_vector_view_info_QueryInterface,
    async_vector_view_info_AddRef,
    async_vector_view_info_Release,
    async_vector_view_info_GetIids,
    async_vector_view_info_GetRuntimeClassName,
    async_vector_view_info_GetTrustLevel,
    async_vector_view_info_get_Id,
    async_vector_view_info_get_Status,
    async_vector_view_info_get_ErrorCode,
    async_vector_view_info_Cancel,
    async_vector_view_info_Close
};

static HRESULT async_vector_view_create( void **out )
{
    struct async_vector_view *impl;
    if (!out) return E_POINTER;
    *out = NULL;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IInspectable_iface.lpVtbl = (const struct IInspectableVtbl *)async_vector_view_vtbl;
    impl->IAsyncInfo_iface.lpVtbl = &async_vector_view_info_vtbl;
    impl->ref = 1;
    empty_vector_view_create( &impl->view );
    *out = &impl->IInspectable_iface;
    TRACE( "created async_vector_view %p\n", *out );
    return S_OK;
}

static HRESULT WINAPI storage_folder_GetFilesAsyncOverloadDefaultOptionsStartAndCount(
        IStorageFolder *iface, IAsyncOperation_IVectorView_StorageFile **operation )
{
    TRACE( "iface %p, operation %p\n", iface, operation );
    return async_vector_view_create( (void **)operation );
}

static HRESULT WINAPI storage_folder_GetFoldersAsyncOverloadDefaultOptionsStartAndCount(
        IStorageFolder *iface, IAsyncOperation_IVectorView_StorageFolder **operation )
{
    TRACE( "iface %p, operation %p\n", iface, operation );
    return async_vector_view_create( (void **)operation );
}

static HRESULT WINAPI storage_folder_GetItemsAsyncOverloadDefaultStartAndCount(
        IStorageFolder *iface, IAsyncOperation_IVectorView_IStorageItem **operation )
{
    TRACE( "iface %p, operation %p\n", iface, operation );
    return async_vector_view_create( (void **)operation );
}

static const struct IStorageFolderVtbl storage_folder_vtbl =
{
    storage_folder_QueryInterface,
    storage_folder_AddRef,
    storage_folder_Release,
    /* IInspectable methods */
    storage_folder_GetIids,
    storage_folder_GetRuntimeClassName,
    storage_folder_GetTrustLevel,
    /* IStorageFolder methods */
    storage_folder_CreateFileAsyncOverloadDefaultOptions,
    storage_folder_CreateFileAsync,
    storage_folder_CreateFolderAsyncOverloadDefaultOptions,
    storage_folder_CreateFolderAsync,
    storage_folder_GetFileAsync,
    storage_folder_GetFolderAsync,
    storage_folder_GetItemAsync,
    storage_folder_GetFilesAsyncOverloadDefaultOptionsStartAndCount,
    storage_folder_GetFoldersAsyncOverloadDefaultOptionsStartAndCount,
    storage_folder_GetItemsAsyncOverloadDefaultStartAndCount,
};

struct async_action
{
    IAsyncAction IAsyncAction_iface;
    IAsyncInfo IAsyncInfo_iface;
    LONG ref;
    HRESULT hr;
    AsyncStatus status;
    IAsyncActionCompletedHandler *handler;
};

static inline struct async_action *impl_from_IAsyncAction( IAsyncAction *iface )
{
    return CONTAINING_RECORD( iface, struct async_action, IAsyncAction_iface );
}

static inline struct async_action *impl_from_async_action_IAsyncInfo( IAsyncInfo *iface )
{
    return CONTAINING_RECORD( iface, struct async_action, IAsyncInfo_iface );
}

static HRESULT WINAPI async_action_QueryInterface( IAsyncAction *iface, REFIID iid, void **out )
{
    struct async_action *impl = impl_from_IAsyncAction( iface );
    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (!out) return E_POINTER;
    *out = NULL;

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAsyncAction ))
    {
        *out = &impl->IAsyncAction_iface;
        InterlockedIncrement( &impl->ref );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IAsyncInfo ))
    {
        *out = &impl->IAsyncInfo_iface;
        InterlockedIncrement( &impl->ref );
        return S_OK;
    }

    WARN( "unsupported iid %s\n", debugstr_guid( iid ) );
    return E_NOINTERFACE;
}

static ULONG WINAPI async_action_AddRef( IAsyncAction *iface )
{
    struct async_action *impl = impl_from_IAsyncAction( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI async_action_Release( IAsyncAction *iface )
{
    struct async_action *impl = impl_from_IAsyncAction( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    if (!ref)
    {
        if (impl->handler) IAsyncActionCompletedHandler_Release( impl->handler );
        free( impl );
    }
    return ref;
}

static HRESULT WINAPI async_action_GetIids( IAsyncAction *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI async_action_GetRuntimeClassName( IAsyncAction *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.IAsyncAction", 31, class_name );
}

static HRESULT WINAPI async_action_GetTrustLevel( IAsyncAction *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI async_action_put_Completed( IAsyncAction *iface, IAsyncActionCompletedHandler *handler )
{
    struct async_action *impl = impl_from_IAsyncAction( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );

    if (impl->handler) IAsyncActionCompletedHandler_Release( impl->handler );
    if ((impl->handler = handler)) IAsyncActionCompletedHandler_AddRef( handler );

    if (handler)
        IAsyncActionCompletedHandler_Invoke( handler, &impl->IAsyncAction_iface, impl->status );

    return S_OK;
}

static HRESULT WINAPI async_action_get_Completed( IAsyncAction *iface, IAsyncActionCompletedHandler **handler )
{
    struct async_action *impl = impl_from_IAsyncAction( iface );
    if (!handler) return E_POINTER;
    *handler = impl->handler;
    if (*handler) IAsyncActionCompletedHandler_AddRef( *handler );
    return S_OK;
}

static HRESULT WINAPI async_action_GetResults( IAsyncAction *iface )
{
    struct async_action *impl = impl_from_IAsyncAction( iface );
    return impl->hr;
}

static const struct IAsyncActionVtbl async_action_vtbl =
{
    async_action_QueryInterface,
    async_action_AddRef,
    async_action_Release,
    async_action_GetIids,
    async_action_GetRuntimeClassName,
    async_action_GetTrustLevel,
    async_action_put_Completed,
    async_action_get_Completed,
    async_action_GetResults,
};

static HRESULT WINAPI async_action_info_QueryInterface( IAsyncInfo *iface, REFIID iid, void **out )
{
    struct async_action *impl = impl_from_async_action_IAsyncInfo( iface );
    return async_action_QueryInterface( &impl->IAsyncAction_iface, iid, out );
}

static ULONG WINAPI async_action_info_AddRef( IAsyncInfo *iface )
{
    struct async_action *impl = impl_from_async_action_IAsyncInfo( iface );
    return async_action_AddRef( &impl->IAsyncAction_iface );
}

static ULONG WINAPI async_action_info_Release( IAsyncInfo *iface )
{
    struct async_action *impl = impl_from_async_action_IAsyncInfo( iface );
    return async_action_Release( &impl->IAsyncAction_iface );
}

static HRESULT WINAPI async_action_info_GetIids( IAsyncInfo *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI async_action_info_GetRuntimeClassName( IAsyncInfo *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.IAsyncInfo", 27, class_name );
}

static HRESULT WINAPI async_action_info_GetTrustLevel( IAsyncInfo *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI async_action_info_get_Id( IAsyncInfo *iface, UINT32 *id )
{
    if (!id) return E_INVALIDARG;
    *id = 1;
    return S_OK;
}

static HRESULT WINAPI async_action_info_get_Status( IAsyncInfo *iface, AsyncStatus *status )
{
    struct async_action *impl = impl_from_async_action_IAsyncInfo( iface );
    if (!status) return E_INVALIDARG;
    *status = impl->status;
    return S_OK;
}

static HRESULT WINAPI async_action_info_get_ErrorCode( IAsyncInfo *iface, HRESULT *error_code )
{
    struct async_action *impl = impl_from_async_action_IAsyncInfo( iface );
    if (!error_code) return E_INVALIDARG;
    *error_code = impl->hr;
    return S_OK;
}

static HRESULT WINAPI async_action_info_Cancel( IAsyncInfo *iface )
{
    return S_OK;
}

static HRESULT WINAPI async_action_info_Close( IAsyncInfo *iface )
{
    return S_OK;
}

static const struct IAsyncInfoVtbl async_action_info_vtbl =
{
    async_action_info_QueryInterface,
    async_action_info_AddRef,
    async_action_info_Release,
    async_action_info_GetIids,
    async_action_info_GetRuntimeClassName,
    async_action_info_GetTrustLevel,
    async_action_info_get_Id,
    async_action_info_get_Status,
    async_action_info_get_ErrorCode,
    async_action_info_Cancel,
    async_action_info_Close,
};

static HRESULT async_action_create( HRESULT hr, AsyncStatus status, IAsyncAction **out )
{
    struct async_action *impl;
    if (!out) return E_INVALIDARG;
    *out = NULL;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IAsyncAction_iface.lpVtbl = &async_action_vtbl;
    impl->IAsyncInfo_iface.lpVtbl = &async_action_info_vtbl;
    impl->ref = 1;
    impl->hr = hr;
    impl->status = status;
    *out = &impl->IAsyncAction_iface;
    TRACE( "created async_action %p, hr %#lx, status %d\n", *out, hr, status );
    return S_OK;
}

/* IStorageItem view of the same folder: Name and Path answer, the rest stubs. */
DEFINE_IINSPECTABLE( storage_item, IStorageItem, struct storage_folder, IStorageFolder_iface )

static HRESULT WINAPI storage_item_RenameAsyncOverloadDefaultOptions( IStorageItem *iface, HSTRING name,
        IAsyncAction **operation )
{
    TRACE( "iface %p, name %s, operation %p\n", iface, debugstr_hstring( name ), operation );
    return async_action_create( S_OK, Completed, operation );
}

static HRESULT WINAPI storage_item_RenameAsync( IStorageItem *iface, HSTRING name, NameCollisionOption option,
        IAsyncAction **operation )
{
    TRACE( "iface %p, name %s, option %d, operation %p\n", iface, debugstr_hstring( name ), option, operation );
    return async_action_create( S_OK, Completed, operation );
}

static HRESULT WINAPI storage_item_DeleteAsyncOverloadDefaultOptions( IStorageItem *iface, IAsyncAction **operation )
{
    struct storage_folder *impl = impl_from_IStorageItem( iface );
    TRACE( "iface %p, operation %p, path %s\n", iface, operation, debugstr_w(impl->path) );
    if (impl->path[0])
    {
        DWORD attr = GetFileAttributesW( impl->path );
        if (attr != INVALID_FILE_ATTRIBUTES)
        {
            if (attr & FILE_ATTRIBUTE_DIRECTORY)
                RemoveDirectoryW( impl->path );
            else
                DeleteFileW( impl->path );
        }
    }
    return async_action_create( S_OK, Completed, operation );
}

static HRESULT WINAPI storage_item_DeleteAsync( IStorageItem *iface, StorageDeleteOption option,
        IAsyncAction **operation )
{
    TRACE( "iface %p, option %d, operation %p\n", iface, option, operation );
    return storage_item_DeleteAsyncOverloadDefaultOptions( iface, operation );
}

static HRESULT WINAPI storage_item_GetBasicPropertiesAsync( IStorageItem *iface,
        IAsyncOperation_BasicProperties **operation )
{
    FIXME( "iface %p, operation %p stub!\n", iface, operation );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_item_get_Name( IStorageItem *iface, HSTRING *value )
{
    struct storage_folder *impl = impl_from_IStorageItem( iface );

    TRACE( "iface %p, value %p: %s.\n", iface, value, debugstr_w( impl->name ) );
    return WindowsCreateString( impl->name, wcslen( impl->name ), value );
}

static HRESULT WINAPI storage_item_get_Path( IStorageItem *iface, HSTRING *value )
{
    struct storage_folder *impl = impl_from_IStorageItem( iface );

    TRACE( "iface %p, value %p: %s.\n", iface, value, debugstr_w( impl->path ) );
    return WindowsCreateString( impl->path, wcslen( impl->path ), value );
}

static HRESULT WINAPI storage_item_get_Attributes( IStorageItem *iface, FileAttributes *value )
{
    TRACE( "iface %p, value %p.\n", iface, value );
    *value = FileAttributes_Directory;
    return S_OK;
}

static HRESULT WINAPI storage_item_get_DateCreated( IStorageItem *iface, DateTime *value )
{
    TRACE( "iface %p, value %p.\n", iface, value );
    memset( value, 0, sizeof(*value) );
    return S_OK;
}

static HRESULT WINAPI storage_item_IsOfType( IStorageItem *iface, StorageItemTypes type, boolean *value )
{
    TRACE( "iface %p, type %d, value %p.\n", iface, type, value );
    *value = (type == StorageItemTypes_Folder);
    return S_OK;
}

static const struct IStorageItemVtbl storage_item_vtbl =
{
    storage_item_QueryInterface,
    storage_item_AddRef,
    storage_item_Release,
    /* IInspectable methods */
    storage_item_GetIids,
    storage_item_GetRuntimeClassName,
    storage_item_GetTrustLevel,
    /* IStorageItem methods */
    storage_item_RenameAsyncOverloadDefaultOptions,
    storage_item_RenameAsync,
    storage_item_DeleteAsyncOverloadDefaultOptions,
    storage_item_DeleteAsync,
    storage_item_GetBasicPropertiesAsync,
    storage_item_get_Name,
    storage_item_get_Path,
    storage_item_get_Attributes,
    storage_item_get_DateCreated,
    storage_item_IsOfType,
};

static void create_directory_chain( const WCHAR *path )
{
    WCHAR *backslash;

    if (CreateDirectoryW( path, NULL ) || GetLastError() == ERROR_ALREADY_EXISTS) return;
    /* Create the chain one level at a time; exists-always wins. */
    for (backslash = wcschr( path + 3, '\\' ); backslash; backslash = wcschr( backslash + 1, '\\' ))
    {
        *backslash = 0;
        CreateDirectoryW( path, NULL );
        *backslash = '\\';
    }
    CreateDirectoryW( path, NULL );
}

static HRESULT storage_folder_create( const WCHAR *kind_name, IStorageFolder **value )
{
    struct storage_folder *impl;
    WCHAR family[128];
    UINT32 family_len = ARRAY_SIZE( family );
    LONG family_ret;

    if (!value) return E_INVALIDARG;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;

    family_ret = GetCurrentPackageFamilyName( &family_len, family );
    if (GetEnvironmentVariableW( L"LOCALAPPDATA", impl->path, MAX_PATH ) && *impl->path)
    {
        if (family_ret == ERROR_SUCCESS)
        {
            append_path_component( impl->path, L"Packages", MAX_PATH );
            append_path_component( impl->path, family, MAX_PATH );
        }
        else
        {
            WARN( "no package identity (hr %#x); using bare %s directory.\n", (UINT32)family_ret, debugstr_w( kind_name ) );
        }
        append_path_component( impl->path, kind_name, MAX_PATH );
        create_directory_chain( impl->path );
    }
    else
    {
        ERR( "LOCALAPPDATA not set; %s has no path (folder object stays name-only).\n", debugstr_w( kind_name ) );
    }

    lstrcpynW( impl->name, kind_name, ARRAY_SIZE( impl->name ) );
    impl->IStorageFolder_iface.lpVtbl = &storage_folder_vtbl;
    impl->IStorageItem_iface.lpVtbl = &storage_item_vtbl;
    impl->ref = 1;

    TRACE( "created IStorageFolder %p for %s at %s.\n", &impl->IStorageFolder_iface,
           debugstr_w( kind_name ), debugstr_w( impl->path ) );
    *value = &impl->IStorageFolder_iface;
    return S_OK;
}

static HRESULT WINAPI application_data_folder_get( IApplicationData *iface, const WCHAR *kind_name,
        IStorageFolder **value )
{
    TRACE( "iface %p, kind_name %s, value %p.\n", iface, debugstr_w( kind_name ), value );
    if (!value) return E_INVALIDARG;
    return storage_folder_create( kind_name, value );
}

DEFINE_IINSPECTABLE( application_data2, IApplicationData2, struct application_data, IApplicationData_iface )

static HRESULT WINAPI application_data2_get_LocalCacheFolder( IApplicationData2 *iface, IStorageFolder **value )
{
    struct application_data *impl = impl_from_IApplicationData2( iface );
    return application_data_folder_get( &impl->IApplicationData_iface, L"LocalCache", value );
}

static const struct IApplicationData2Vtbl application_data2_vtbl =
{
    application_data2_QueryInterface,
    application_data2_AddRef,
    application_data2_Release,
    /* IInspectable methods */
    application_data2_GetIids,
    application_data2_GetRuntimeClassName,
    application_data2_GetTrustLevel,
    /* IApplicationData2 methods */
    application_data2_get_LocalCacheFolder,
};

DEFINE_IINSPECTABLE( application_data3, IApplicationData3, struct application_data, IApplicationData_iface )

static HRESULT WINAPI application_data3_GetPublisherCacheFolder( IApplicationData3 *iface, HSTRING folder_name, IStorageFolder **value )
{
    struct application_data *impl = impl_from_IApplicationData3( iface );
    return application_data_folder_get( &impl->IApplicationData_iface, L"PublisherCache", value );
}

static HRESULT WINAPI application_data3_ClearPublisherCacheFolderAsync( IApplicationData3 *iface, HSTRING folder_name, IAsyncAction **operation )
{
    FIXME( "iface %p, folder_name %p, operation %p stub!\n", iface, folder_name, operation );
    return E_NOTIMPL;
}

static HRESULT WINAPI application_data3_get_SharedLocalFolder( IApplicationData3 *iface, IStorageFolder **value )
{
    struct application_data *impl = impl_from_IApplicationData3( iface );
    return application_data_folder_get( &impl->IApplicationData_iface, L"SharedLocal", value );
}

static const struct IApplicationData3Vtbl application_data3_vtbl =
{
    application_data3_QueryInterface,
    application_data3_AddRef,
    application_data3_Release,
    /* IInspectable methods */
    application_data3_GetIids,
    application_data3_GetRuntimeClassName,
    application_data3_GetTrustLevel,
    /* IApplicationData3 methods */
    application_data3_GetPublisherCacheFolder,
    application_data3_ClearPublisherCacheFolderAsync,
    application_data3_get_SharedLocalFolder,
};

DEFINE_IINSPECTABLE( application_data_statics, IApplicationDataStatics, struct application_data_statics, IActivationFactory_iface )

static HRESULT WINAPI application_data_statics_get_Current( IApplicationDataStatics *iface, IApplicationData **value )
{
    struct application_data *impl;

    TRACE( "iface %p, value %p\n", iface, value );

    if (!value) return E_INVALIDARG;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;

    impl->IApplicationData_iface.lpVtbl = &application_data_vtbl;
    impl->IApplicationData2_iface.lpVtbl = &application_data2_vtbl;
    impl->IApplicationData3_iface.lpVtbl = &application_data3_vtbl;
    impl->IRedhawkInterop_iface.lpVtbl = redhawk_appdata_vtbl;
    impl->ref = 1;

    *value = &impl->IApplicationData_iface;
    TRACE( "created IApplicationData %p.\n", *value );
    return S_OK;
}

static const struct IApplicationDataStaticsVtbl application_data_statics_vtbl =
{
    application_data_statics_QueryInterface,
    application_data_statics_AddRef,
    application_data_statics_Release,
    /* IInspectable methods */
    application_data_statics_GetIids,
    application_data_statics_GetRuntimeClassName,
    application_data_statics_GetTrustLevel,
    /* IApplicationDataStatics methods */
    application_data_statics_get_Current,
};

static struct application_data_statics application_data_statics =
{
    {&factory_vtbl},
    {&application_data_statics_vtbl},
    {redhawk_statics_vtbl},
    1,
};

IActivationFactory *application_data_factory = &application_data_statics.IActivationFactory_iface;

/* --- Windows.Management.Core.ApplicationDataManager Implementation --- */
/* {1e1862e3-698e-49a1-9752-dee94925b9b3} */
static const GUID IID_IApplicationDataManagerStatics = { 0x1e1862e3, 0x698e, 0x49a1, { 0x97, 0x52, 0xde, 0xe9, 0x49, 0x25, 0xb9, 0xb3 } };

struct app_data_manager_statics
{
    IActivationFactory IActivationFactory_iface;
    IApplicationDataManagerStatics IApplicationDataManagerStatics_iface;
    LONG ref;
};

static inline struct app_data_manager_statics *impl_from_app_data_manager_factory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct app_data_manager_statics, IActivationFactory_iface );
}

static inline struct app_data_manager_statics *impl_from_IApplicationDataManagerStatics( IApplicationDataManagerStatics *iface )
{
    return CONTAINING_RECORD( iface, struct app_data_manager_statics, IApplicationDataManagerStatics_iface );
}

static HRESULT WINAPI app_data_manager_factory_QueryInterface( IActivationFactory *iface, REFIID iid, void **out )
{
    struct app_data_manager_statics *impl = impl_from_app_data_manager_factory( iface );

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

    if (IsEqualGUID( iid, &IID_IApplicationDataManagerStatics ))
    {
        *out = &impl->IApplicationDataManagerStatics_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI app_data_manager_factory_AddRef( IActivationFactory *iface )
{
    struct app_data_manager_statics *impl = impl_from_app_data_manager_factory( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI app_data_manager_factory_Release( IActivationFactory *iface )
{
    struct app_data_manager_statics *impl = impl_from_app_data_manager_factory( iface );
    return InterlockedDecrement( &impl->ref );
}

static HRESULT WINAPI app_data_manager_factory_GetIids( IActivationFactory *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI app_data_manager_factory_GetRuntimeClassName( IActivationFactory *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Management.Core.ApplicationDataManager", 45, class_name );
}

static HRESULT WINAPI app_data_manager_factory_GetTrustLevel( IActivationFactory *iface, TrustLevel *trust_level )
{
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI app_data_manager_factory_ActivateInstance( IActivationFactory *iface, IInspectable **instance )
{
    FIXME( "iface %p, instance %p stub!\n", iface, instance );
    return E_NOTIMPL;
}

static const struct IActivationFactoryVtbl app_data_manager_factory_vtbl =
{
    app_data_manager_factory_QueryInterface,
    app_data_manager_factory_AddRef,
    app_data_manager_factory_Release,
    /* IInspectable methods */
    app_data_manager_factory_GetIids,
    app_data_manager_factory_GetRuntimeClassName,
    app_data_manager_factory_GetTrustLevel,
    /* IActivationFactory methods */
    app_data_manager_factory_ActivateInstance,
};

static HRESULT WINAPI app_data_manager_statics_QueryInterface( IApplicationDataManagerStatics *iface, REFIID iid, void **out )
{
    struct app_data_manager_statics *impl = impl_from_IApplicationDataManagerStatics( iface );
    return IActivationFactory_QueryInterface( &impl->IActivationFactory_iface, iid, out );
}

static ULONG WINAPI app_data_manager_statics_AddRef( IApplicationDataManagerStatics *iface )
{
    struct app_data_manager_statics *impl = impl_from_IApplicationDataManagerStatics( iface );
    return IActivationFactory_AddRef( &impl->IActivationFactory_iface );
}

static ULONG WINAPI app_data_manager_statics_Release( IApplicationDataManagerStatics *iface )
{
    struct app_data_manager_statics *impl = impl_from_IApplicationDataManagerStatics( iface );
    return IActivationFactory_Release( &impl->IActivationFactory_iface );
}

static HRESULT WINAPI app_data_manager_statics_GetIids( IApplicationDataManagerStatics *iface, ULONG *iid_count, IID **iids )
{
    struct app_data_manager_statics *impl = impl_from_IApplicationDataManagerStatics( iface );
    return IActivationFactory_GetIids( &impl->IActivationFactory_iface, iid_count, iids );
}

static HRESULT WINAPI app_data_manager_statics_GetRuntimeClassName( IApplicationDataManagerStatics *iface, HSTRING *class_name )
{
    struct app_data_manager_statics *impl = impl_from_IApplicationDataManagerStatics( iface );
    return IActivationFactory_GetRuntimeClassName( &impl->IActivationFactory_iface, class_name );
}

static HRESULT WINAPI app_data_manager_statics_GetTrustLevel( IApplicationDataManagerStatics *iface, TrustLevel *trust_level )
{
    struct app_data_manager_statics *impl = impl_from_IApplicationDataManagerStatics( iface );
    return IActivationFactory_GetTrustLevel( &impl->IActivationFactory_iface, trust_level );
}

static HRESULT WINAPI app_data_manager_statics_CreateForPackageFamily(
    IApplicationDataManagerStatics *iface, HSTRING package_family, IApplicationData **result )
{
    TRACE( "iface %p, package_family %s, result %p\n", iface, debugstr_hstring( package_family ), result );
    if (!result) return E_INVALIDARG;
    return application_data_statics_get_Current( &application_data_statics.IApplicationDataStatics_iface, result );
}

static const struct IApplicationDataManagerStaticsVtbl app_data_manager_statics_vtbl =
{
    app_data_manager_statics_QueryInterface,
    app_data_manager_statics_AddRef,
    app_data_manager_statics_Release,
    /* IInspectable methods */
    app_data_manager_statics_GetIids,
    app_data_manager_statics_GetRuntimeClassName,
    app_data_manager_statics_GetTrustLevel,
    /* IApplicationDataManagerStatics methods */
    app_data_manager_statics_CreateForPackageFamily,
};

static struct app_data_manager_statics app_data_manager_statics =
{
    {&app_data_manager_factory_vtbl},
    {&app_data_manager_statics_vtbl},
    1,
};

IActivationFactory *app_data_manager_factory = &app_data_manager_statics.IActivationFactory_iface;
