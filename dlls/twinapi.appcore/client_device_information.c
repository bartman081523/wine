/*
 * Copyright 2023 Rémi Bernon for CodeWeavers
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

WINE_DEFAULT_DEBUG_CHANNEL(twinapi);

static const GUID IID_IEasClientDeviceInformation2 = {0xffb35923, 0xbb26, 0x4d6a, {0x81, 0xbc, 0x16, 0x5a, 0xee, 0x0a, 0xd7, 0x54}};

typedef struct IEasClientDeviceInformation2 IEasClientDeviceInformation2;

typedef struct IEasClientDeviceInformation2Vtbl
{
    /* IUnknown */
    HRESULT (WINAPI *QueryInterface)(IEasClientDeviceInformation2 *This, REFIID riid, void **ppvObject);
    ULONG   (WINAPI *AddRef)(IEasClientDeviceInformation2 *This);
    ULONG   (WINAPI *Release)(IEasClientDeviceInformation2 *This);
    /* IInspectable */
    HRESULT (WINAPI *GetIids)(IEasClientDeviceInformation2 *This, ULONG *iidCount, IID **iids);
    HRESULT (WINAPI *GetRuntimeClassName)(IEasClientDeviceInformation2 *This, HSTRING *className);
    HRESULT (WINAPI *GetTrustLevel)(IEasClientDeviceInformation2 *This, TrustLevel *trustLevel);
    /* IEasClientDeviceInformation2 */
    HRESULT (WINAPI *get_SystemHardwareVersion)(IEasClientDeviceInformation2 *This, HSTRING *value);
    HRESULT (WINAPI *get_SystemFirmwareVersion)(IEasClientDeviceInformation2 *This, HSTRING *value);
} IEasClientDeviceInformation2Vtbl;

struct IEasClientDeviceInformation2
{
    const IEasClientDeviceInformation2Vtbl *lpVtbl;
};

struct client_device_information
{
    IEasClientDeviceInformation IEasClientDeviceInformation_iface;
    IEasClientDeviceInformation2 IEasClientDeviceInformation2_iface;
    LONG ref;
};

static inline struct client_device_information *impl_from_IEasClientDeviceInformation( IEasClientDeviceInformation *iface )
{
    return CONTAINING_RECORD( iface, struct client_device_information, IEasClientDeviceInformation_iface );
}

static inline struct client_device_information *impl_from_IEasClientDeviceInformation2( IEasClientDeviceInformation2 *iface )
{
    return CONTAINING_RECORD( iface, struct client_device_information, IEasClientDeviceInformation2_iface );
}

static ULONG WINAPI client_device_information_AddRef( IEasClientDeviceInformation *iface );

static HRESULT WINAPI client_device_information_QueryInterface( IEasClientDeviceInformation *iface,
                                                                REFIID iid, void **out )
{
    struct client_device_information *impl = impl_from_IEasClientDeviceInformation( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IEasClientDeviceInformation ))
    {
        *out = &impl->IEasClientDeviceInformation_iface;
        client_device_information_AddRef( iface );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IEasClientDeviceInformation2 ))
    {
        *out = &impl->IEasClientDeviceInformation2_iface;
        client_device_information_AddRef( iface );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI client_device_information_AddRef( IEasClientDeviceInformation *iface )
{
    struct client_device_information *impl = impl_from_IEasClientDeviceInformation( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI client_device_information_Release( IEasClientDeviceInformation *iface )
{
    struct client_device_information *impl = impl_from_IEasClientDeviceInformation( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );
    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI client_device_information_GetIids( IEasClientDeviceInformation *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI client_device_information_GetRuntimeClassName( IEasClientDeviceInformation *iface, HSTRING *class_name )
{
    return WindowsCreateString( RuntimeClass_Windows_Security_ExchangeActiveSyncProvisioning_EasClientDeviceInformation,
                                wcslen( RuntimeClass_Windows_Security_ExchangeActiveSyncProvisioning_EasClientDeviceInformation ),
                                class_name );
}

static HRESULT WINAPI client_device_information_GetTrustLevel( IEasClientDeviceInformation *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI client_device_information_get_Id( IEasClientDeviceInformation *iface, GUID *value )
{
    static const GUID device_id = {0x6bb16afe, 0xe97d, 0x4225, {0xb2, 0xa6, 0xa1, 0xe0, 0x77, 0x8d, 0x1d, 0x8c}};
    TRACE( "iface %p, value %p.\n", iface, value );
    if (!value) return E_POINTER;
    *value = device_id;
    return S_OK;
}

static HRESULT WINAPI client_device_information_get_OperatingSystem( IEasClientDeviceInformation *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p.\n", iface, value );
    return WindowsCreateString( L"WINDOWS", 7, value );
}

static HRESULT WINAPI client_device_information_get_FriendlyName( IEasClientDeviceInformation *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p.\n", iface, value );
    return WindowsCreateString( L"WinePC", 6, value );
}

static HRESULT WINAPI client_device_information_get_SystemManufacturer( IEasClientDeviceInformation *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p.\n", iface, value );
    return WindowsCreateString( L"Microsoft Corporation", 21, value );
}

static HRESULT WINAPI client_device_information_get_SystemProductName( IEasClientDeviceInformation *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p.\n", iface, value );
    return WindowsCreateString( L"System Product Name", 19, value );
}

static HRESULT WINAPI client_device_information_get_SystemSku( IEasClientDeviceInformation *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p.\n", iface, value );
    return WindowsCreateString( L"System SKU", 10, value );
}

static IEasClientDeviceInformationVtbl client_device_information_vtbl =
{
    client_device_information_QueryInterface,
    client_device_information_AddRef,
    client_device_information_Release,
    /* IInspectable methods */
    client_device_information_GetIids,
    client_device_information_GetRuntimeClassName,
    client_device_information_GetTrustLevel,
    /* IEasClientDeviceInformation methods */
    client_device_information_get_Id,
    client_device_information_get_OperatingSystem,
    client_device_information_get_FriendlyName,
    client_device_information_get_SystemManufacturer,
    client_device_information_get_SystemProductName,
    client_device_information_get_SystemSku,
};

static HRESULT WINAPI client_device_information2_QueryInterface( IEasClientDeviceInformation2 *iface, REFIID iid, void **out )
{
    struct client_device_information *impl = impl_from_IEasClientDeviceInformation2( iface );
    return client_device_information_QueryInterface( &impl->IEasClientDeviceInformation_iface, iid, out );
}

static ULONG WINAPI client_device_information2_AddRef( IEasClientDeviceInformation2 *iface )
{
    struct client_device_information *impl = impl_from_IEasClientDeviceInformation2( iface );
    return client_device_information_AddRef( &impl->IEasClientDeviceInformation_iface );
}

static ULONG WINAPI client_device_information2_Release( IEasClientDeviceInformation2 *iface )
{
    struct client_device_information *impl = impl_from_IEasClientDeviceInformation2( iface );
    return client_device_information_Release( &impl->IEasClientDeviceInformation_iface );
}

static HRESULT WINAPI client_device_information2_GetIids( IEasClientDeviceInformation2 *iface, ULONG *iid_count, IID **iids )
{
    struct client_device_information *impl = impl_from_IEasClientDeviceInformation2( iface );
    return client_device_information_GetIids( &impl->IEasClientDeviceInformation_iface, iid_count, iids );
}

static HRESULT WINAPI client_device_information2_GetRuntimeClassName( IEasClientDeviceInformation2 *iface, HSTRING *class_name )
{
    return WindowsCreateString( RuntimeClass_Windows_Security_ExchangeActiveSyncProvisioning_EasClientDeviceInformation,
                                wcslen( RuntimeClass_Windows_Security_ExchangeActiveSyncProvisioning_EasClientDeviceInformation ),
                                class_name );
}

static HRESULT WINAPI client_device_information2_GetTrustLevel( IEasClientDeviceInformation2 *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI client_device_information2_get_SystemHardwareVersion( IEasClientDeviceInformation2 *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p.\n", iface, value );
    return WindowsCreateString( L"1.0.0.0", 7, value );
}

static HRESULT WINAPI client_device_information2_get_SystemFirmwareVersion( IEasClientDeviceInformation2 *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p.\n", iface, value );
    return WindowsCreateString( L"1.0.0.0", 7, value );
}

static const struct IEasClientDeviceInformation2Vtbl client_device_information2_vtbl =
{
    client_device_information2_QueryInterface,
    client_device_information2_AddRef,
    client_device_information2_Release,
    client_device_information2_GetIids,
    client_device_information2_GetRuntimeClassName,
    client_device_information2_GetTrustLevel,
    client_device_information2_get_SystemHardwareVersion,
    client_device_information2_get_SystemFirmwareVersion,
};

struct factory
{
    IActivationFactory IActivationFactory_iface;
    LONG ref;
};

static inline struct factory *impl_from_IActivationFactory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct factory, IActivationFactory_iface );
}

static HRESULT WINAPI factory_QueryInterface( IActivationFactory *iface, REFIID iid, void **out )
{
    struct factory *impl = impl_from_IActivationFactory( iface );

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

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI factory_AddRef( IActivationFactory *iface )
{
    struct factory *impl = impl_from_IActivationFactory( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI factory_Release( IActivationFactory *iface )
{
    struct factory *impl = impl_from_IActivationFactory( iface );
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
    struct client_device_information *impl;

    TRACE( "iface %p, instance %p\n", iface, instance );

    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IEasClientDeviceInformation_iface.lpVtbl = &client_device_information_vtbl;
    impl->IEasClientDeviceInformation2_iface.lpVtbl = &client_device_information2_vtbl;
    impl->ref = 1;

    *instance = (IInspectable *)&impl->IEasClientDeviceInformation_iface;
    return S_OK;
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

static struct factory factory =
{
    {&factory_vtbl},
    1,
};

IActivationFactory *client_device_information_factory = &factory.IActivationFactory_iface;
