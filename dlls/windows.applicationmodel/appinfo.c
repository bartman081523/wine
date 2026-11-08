/*
 * WinRT Windows.ApplicationModel.AppInfo Implementation
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
#include "appmodel.h"

WINE_DEFAULT_DEBUG_CHANNEL(model);

/* AppDisplayInfo */
struct app_display_info
{
    IAppDisplayInfo IAppDisplayInfo_iface;
    LONG ref;
    WCHAR display_name[128];
    WCHAR description[128];
};

static inline struct app_display_info *impl_from_IAppDisplayInfo( IAppDisplayInfo *iface )
{
    return CONTAINING_RECORD( iface, struct app_display_info, IAppDisplayInfo_iface );
}

static HRESULT WINAPI app_display_info_QueryInterface( IAppDisplayInfo *iface, REFIID iid, void **out )
{
    struct app_display_info *impl = impl_from_IAppDisplayInfo( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAppDisplayInfo ))
    {
        *out = &impl->IAppDisplayInfo_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI app_display_info_AddRef( IAppDisplayInfo *iface )
{
    struct app_display_info *impl = impl_from_IAppDisplayInfo( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI app_display_info_Release( IAppDisplayInfo *iface )
{
    struct app_display_info *impl = impl_from_IAppDisplayInfo( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );
    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI app_display_info_GetIids( IAppDisplayInfo *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI app_display_info_GetRuntimeClassName( IAppDisplayInfo *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.ApplicationModel.AppDisplayInfo",
                                ARRAY_SIZE(L"Windows.ApplicationModel.AppDisplayInfo") - 1,
                                class_name );
}

static HRESULT WINAPI app_display_info_GetTrustLevel( IAppDisplayInfo *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI app_display_info_get_DisplayName( IAppDisplayInfo *iface, HSTRING *value )
{
    struct app_display_info *impl = impl_from_IAppDisplayInfo( iface );
    TRACE( "iface %p, value %p -> %s\n", iface, value, debugstr_w(impl->display_name) );
    if (!value) return E_INVALIDARG;
    return WindowsCreateString( impl->display_name, wcslen(impl->display_name), value );
}

static HRESULT WINAPI app_display_info_get_Description( IAppDisplayInfo *iface, HSTRING *value )
{
    struct app_display_info *impl = impl_from_IAppDisplayInfo( iface );
    TRACE( "iface %p, value %p -> %s\n", iface, value, debugstr_w(impl->description) );
    if (!value) return E_INVALIDARG;
    return WindowsCreateString( impl->description, wcslen(impl->description), value );
}

static HRESULT WINAPI app_display_info_GetLogo( IAppDisplayInfo *iface, __x_ABI_CWindows_CFoundation_CSize size,
                                                __x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStreamReference **value )
{
    FIXME( "iface %p stub!\n", iface );
    if (!value) return E_INVALIDARG;
    *value = NULL;
    return S_OK;
}

static const struct IAppDisplayInfoVtbl app_display_info_vtbl =
{
    app_display_info_QueryInterface,
    app_display_info_AddRef,
    app_display_info_Release,
    /* IInspectable methods */
    app_display_info_GetIids,
    app_display_info_GetRuntimeClassName,
    app_display_info_GetTrustLevel,
    /* IAppDisplayInfo methods */
    app_display_info_get_DisplayName,
    app_display_info_get_Description,
    app_display_info_GetLogo,
};

static void get_fallback_app_identity( WCHAR *name, DWORD name_len, WCHAR *pfn, DWORD pfn_len )
{
    WCHAR path[MAX_PATH], *p;
    GetModuleFileNameW( NULL, path, ARRAY_SIZE(path) );
    p = wcsrchr( path, '\\' );
    if (p) p++; else p = path;
    if (name)
    {
        lstrcpynW( name, p, name_len );
        p = wcsrchr( name, '.' );
        if (p) *p = 0;
    }
    if (pfn)
    {
        WCHAR base[64];
        lstrcpynW( base, (name && *name) ? name : L"Application", ARRAY_SIZE(base) );
        swprintf( pfn, pfn_len, L"%s_App", base );
    }
}

static HRESULT create_app_display_info( const WCHAR *display_name, const WCHAR *description, IAppDisplayInfo **out )
{
    struct app_display_info *impl;
    WCHAR fallback_name[64];
    if (!out) return E_INVALIDARG;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IAppDisplayInfo_iface.lpVtbl = &app_display_info_vtbl;
    impl->ref = 1;
    get_fallback_app_identity( fallback_name, ARRAY_SIZE(fallback_name), NULL, 0 );
    lstrcpynW( impl->display_name, display_name ? display_name : fallback_name, ARRAY_SIZE(impl->display_name) );
    lstrcpynW( impl->description, description ? description : fallback_name, ARRAY_SIZE(impl->description) );
    *out = &impl->IAppDisplayInfo_iface;
    return S_OK;
}

/* AppInfo */
struct app_info
{
    IAppInfo IAppInfo_iface;
    IAppInfo2 IAppInfo2_iface;
    IAppInfo3 IAppInfo3_iface;
    IAppInfo4 IAppInfo4_iface;
    LONG ref;
    WCHAR id[64];
    WCHAR app_user_model_id[256];
    WCHAR package_family_name[128];
    WCHAR display_name[128];
};

static inline struct app_info *impl_from_IAppInfo( IAppInfo *iface )
{
    return CONTAINING_RECORD( iface, struct app_info, IAppInfo_iface );
}

static HRESULT WINAPI app_info_QueryInterface( IAppInfo *iface, REFIID iid, void **out )
{
    struct app_info *impl = impl_from_IAppInfo( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAppInfo ))
    {
        *out = &impl->IAppInfo_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IAppInfo2 ))
    {
        *out = &impl->IAppInfo2_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IAppInfo3 ))
    {
        *out = &impl->IAppInfo3_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IAppInfo4 ))
    {
        *out = &impl->IAppInfo4_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI app_info_AddRef( IAppInfo *iface )
{
    struct app_info *impl = impl_from_IAppInfo( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI app_info_Release( IAppInfo *iface )
{
    struct app_info *impl = impl_from_IAppInfo( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );
    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI app_info_GetIids( IAppInfo *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI app_info_GetRuntimeClassName( IAppInfo *iface, HSTRING *class_name )
{
    return WindowsCreateString( RuntimeClass_Windows_ApplicationModel_AppInfo,
                                ARRAY_SIZE(RuntimeClass_Windows_ApplicationModel_AppInfo) - 1,
                                class_name );
}

static HRESULT WINAPI app_info_GetTrustLevel( IAppInfo *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI app_info_get_Id( IAppInfo *iface, HSTRING *value )
{
    struct app_info *impl = impl_from_IAppInfo( iface );
    TRACE( "iface %p, value %p -> %s\n", iface, value, debugstr_w(impl->id) );
    if (!value) return E_INVALIDARG;
    return WindowsCreateString( impl->id, wcslen(impl->id), value );
}

static HRESULT WINAPI app_info_get_AppUserModelId( IAppInfo *iface, HSTRING *value )
{
    struct app_info *impl = impl_from_IAppInfo( iface );
    TRACE( "iface %p, value %p -> %s\n", iface, value, debugstr_w(impl->app_user_model_id) );
    if (!value) return E_INVALIDARG;
    return WindowsCreateString( impl->app_user_model_id, wcslen(impl->app_user_model_id), value );
}

static HRESULT WINAPI app_info_get_DisplayInfo( IAppInfo *iface, IAppDisplayInfo **value )
{
    struct app_info *impl = impl_from_IAppInfo( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_INVALIDARG;
    return create_app_display_info( impl->display_name, impl->display_name, value );
}

static HRESULT WINAPI app_info_get_PackageFamilyName( IAppInfo *iface, HSTRING *value )
{
    struct app_info *impl = impl_from_IAppInfo( iface );
    TRACE( "iface %p, value %p -> %s\n", iface, value, debugstr_w(impl->package_family_name) );
    if (!value) return E_INVALIDARG;
    return WindowsCreateString( impl->package_family_name, wcslen(impl->package_family_name), value );
}

static const struct IAppInfoVtbl app_info_vtbl =
{
    app_info_QueryInterface,
    app_info_AddRef,
    app_info_Release,
    /* IInspectable methods */
    app_info_GetIids,
    app_info_GetRuntimeClassName,
    app_info_GetTrustLevel,
    /* IAppInfo methods */
    app_info_get_Id,
    app_info_get_AppUserModelId,
    app_info_get_DisplayInfo,
    app_info_get_PackageFamilyName,
};

DEFINE_IINSPECTABLE( app_info2, IAppInfo2, struct app_info, IAppInfo_iface )

static HRESULT WINAPI app_info2_get_Package( IAppInfo2 *iface, IPackage **value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_INVALIDARG;
    return create_package( value );
}

static const struct IAppInfo2Vtbl app_info2_vtbl =
{
    app_info2_QueryInterface,
    app_info2_AddRef,
    app_info2_Release,
    /* IInspectable methods */
    app_info2_GetIids,
    app_info2_GetRuntimeClassName,
    app_info2_GetTrustLevel,
    /* IAppInfo2 methods */
    app_info2_get_Package,
};

DEFINE_IINSPECTABLE( app_info3, IAppInfo3, struct app_info, IAppInfo_iface )

static HRESULT WINAPI app_info3_get_ExecutionContext( IAppInfo3 *iface, AppExecutionContext *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if (!value) return E_INVALIDARG;
    *value = AppExecutionContext_Host;
    return S_OK;
}

static const struct IAppInfo3Vtbl app_info3_vtbl =
{
    app_info3_QueryInterface,
    app_info3_AddRef,
    app_info3_Release,
    /* IInspectable methods */
    app_info3_GetIids,
    app_info3_GetRuntimeClassName,
    app_info3_GetTrustLevel,
    /* IAppInfo3 methods */
    app_info3_get_ExecutionContext,
};

DEFINE_IINSPECTABLE( app_info4, IAppInfo4, struct app_info, IAppInfo_iface )

static HRESULT WINAPI app_info4_get_SupportedFileExtensions( IAppInfo4 *iface, UINT32 *value_size, HSTRING **value )
{
    TRACE( "iface %p, value_size %p, value %p\n", iface, value_size, value );
    if (!value_size || !value) return E_INVALIDARG;
    *value_size = 0;
    *value = NULL;
    return S_OK;
}

static const struct IAppInfo4Vtbl app_info4_vtbl =
{
    app_info4_QueryInterface,
    app_info4_AddRef,
    app_info4_Release,
    /* IInspectable methods */
    app_info4_GetIids,
    app_info4_GetRuntimeClassName,
    app_info4_GetTrustLevel,
    /* IAppInfo4 methods */
    app_info4_get_SupportedFileExtensions,
};

static HRESULT create_app_info( const WCHAR *aumid, IAppInfo **out )
{
    struct app_info *impl;
    WCHAR fallback_name[64];
    const WINE_APPMODEL_ENTRY *entry = WineAppModelGetCurrentEntry();

    if (!out) return E_INVALIDARG;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;

    impl->IAppInfo_iface.lpVtbl = &app_info_vtbl;
    impl->IAppInfo2_iface.lpVtbl = &app_info2_vtbl;
    impl->IAppInfo3_iface.lpVtbl = &app_info3_vtbl;
    impl->IAppInfo4_iface.lpVtbl = &app_info4_vtbl;
    impl->ref = 1;

    if (aumid && *aumid)
    {
        const WCHAR *bang = wcschr( aumid, '!' );
        lstrcpynW( impl->app_user_model_id, aumid, ARRAY_SIZE(impl->app_user_model_id) );
        if (bang)
        {
            size_t pfn_len = bang - aumid;
            if (pfn_len >= ARRAY_SIZE(impl->package_family_name))
                pfn_len = ARRAY_SIZE(impl->package_family_name) - 1;
            memcpy( impl->package_family_name, aumid, pfn_len * sizeof(WCHAR) );
            impl->package_family_name[pfn_len] = 0;
            lstrcpynW( impl->id, bang + 1, ARRAY_SIZE(impl->id) );
        }
        else
        {
            lstrcpynW( impl->package_family_name, aumid, ARRAY_SIZE(impl->package_family_name) );
            lstrcpynW( impl->id, L"App", ARRAY_SIZE(impl->id) );
        }
    }
    else if (entry)
    {
        WCHAR fallback_pfn[128];
        const WCHAR *id, *pfn;
        id = (entry->app_id && *entry->app_id) ? entry->app_id : L"App";
        get_fallback_app_identity( NULL, 0, fallback_pfn, ARRAY_SIZE(fallback_pfn) );
        pfn = (entry->family_name && *entry->family_name) ? entry->family_name : fallback_pfn;
        lstrcpynW( impl->id, id, ARRAY_SIZE(impl->id) );
        lstrcpynW( impl->package_family_name, pfn, ARRAY_SIZE(impl->package_family_name) );
        swprintf( impl->app_user_model_id, ARRAY_SIZE(impl->app_user_model_id), L"%s!%s", pfn, id );
    }
    else
    {
        WCHAR fallback_pfn[128];
        get_fallback_app_identity( fallback_name, ARRAY_SIZE(fallback_name), fallback_pfn, ARRAY_SIZE(fallback_pfn) );
        lstrcpynW( impl->id, L"App", ARRAY_SIZE(impl->id) );
        lstrcpynW( impl->package_family_name, fallback_pfn, ARRAY_SIZE(impl->package_family_name) );
        swprintf( impl->app_user_model_id, ARRAY_SIZE(impl->app_user_model_id), L"%s!App", fallback_pfn );
    }

    get_fallback_app_identity( fallback_name, ARRAY_SIZE(fallback_name), NULL, 0 );
    lstrcpynW( impl->display_name, (entry && entry->name && *entry->name) ? entry->name : fallback_name, ARRAY_SIZE(impl->display_name) );

    *out = &impl->IAppInfo_iface;
    TRACE( "created IAppInfo %p: aumid %s, pfn %s, id %s\n",
           *out, debugstr_w(impl->app_user_model_id), debugstr_w(impl->package_family_name), debugstr_w(impl->id) );
    return S_OK;
}

/* AppInfoStatics (Activation Factory) */
struct app_info_statics
{
    IActivationFactory IActivationFactory_iface;
    IAppInfoStatics IAppInfoStatics_iface;
    LONG ref;
};

static inline struct app_info_statics *impl_from_IActivationFactory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct app_info_statics, IActivationFactory_iface );
}

static HRESULT WINAPI factory_QueryInterface( IActivationFactory *iface, REFIID iid, void **out )
{
    struct app_info_statics *impl = impl_from_IActivationFactory( iface );

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

    if (IsEqualGUID( iid, &IID_IAppInfoStatics ))
    {
        *out = &impl->IAppInfoStatics_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI factory_AddRef( IActivationFactory *iface )
{
    struct app_info_statics *impl = impl_from_IActivationFactory( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI factory_Release( IActivationFactory *iface )
{
    struct app_info_statics *impl = impl_from_IActivationFactory( iface );
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
    TRACE( "iface %p, instance %p\n", iface, instance );
    return create_app_info( NULL, (IAppInfo **)instance );
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

DEFINE_IINSPECTABLE( app_info_statics, IAppInfoStatics, struct app_info_statics, IActivationFactory_iface )

static HRESULT WINAPI app_info_statics_get_Current( IAppInfoStatics *iface, IAppInfo **value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    return create_app_info( NULL, value );
}

static HRESULT WINAPI app_info_statics_GetFromAppUserModelId( IAppInfoStatics *iface, HSTRING app_user_model_id, IAppInfo **result )
{
    const WCHAR *str = WindowsGetStringRawBuffer( app_user_model_id, NULL );
    TRACE( "iface %p, app_user_model_id %s, result %p\n", iface, debugstr_w(str), result );
    return create_app_info( str, result );
}

static HRESULT WINAPI app_info_statics_GetFromAppUserModelIdForUser( IAppInfoStatics *iface, __x_ABI_CWindows_CSystem_CIUser *user,
                                                                    HSTRING app_user_model_id, IAppInfo **result )
{
    const WCHAR *str = WindowsGetStringRawBuffer( app_user_model_id, NULL );
    TRACE( "iface %p, user %p, app_user_model_id %s, result %p\n", iface, user, debugstr_w(str), result );
    return create_app_info( str, result );
}

static const struct IAppInfoStaticsVtbl app_info_statics_vtbl =
{
    app_info_statics_QueryInterface,
    app_info_statics_AddRef,
    app_info_statics_Release,
    /* IInspectable methods */
    app_info_statics_GetIids,
    app_info_statics_GetRuntimeClassName,
    app_info_statics_GetTrustLevel,
    /* IAppInfoStatics methods */
    app_info_statics_get_Current,
    app_info_statics_GetFromAppUserModelId,
    app_info_statics_GetFromAppUserModelIdForUser,
};

static struct app_info_statics app_info_statics =
{
    {&factory_vtbl},
    {&app_info_statics_vtbl},
    1,
};

IActivationFactory *app_info_factory = &app_info_statics.IActivationFactory_iface;
