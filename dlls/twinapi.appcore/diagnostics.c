/* WinRT Windows.Foundation.Diagnostics Implementation
 *
 * Copyright 2026 Xodus / Proton-GE contributors
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include <stdarg.h>
#include <stddef.h>

/* GUIDs */
/* {A93151DA-7FAF-4191-8755-5E86DC65D896} */
static const GUID IID_ILoggingChannelOptionsFactory =
    {0xa93151da, 0x7faf, 0x4191, {0x87, 0x55, 0x5e, 0x86, 0xdc, 0x65, 0xd8, 0x96}};

/* {4C6EF5DD-3B27-4DC9-99F0-299C6E4603A1} */
static const GUID IID_ILoggingChannelFactory2 =
    {0x4c6ef5dd, 0x3b27, 0x4dc9, {0x99, 0xf0, 0x29, 0x9c, 0x6e, 0x46, 0x03, 0xa1}};

/* {D713C6CB-98AB-464B-9F22-A3268478368A} */
static const GUID IID_ILoggingOptionsFactory =
    {0xd713c6cb, 0x98ab, 0x464b, {0x9f, 0x22, 0xa3, 0x26, 0x84, 0x78, 0x36, 0x8a}};

/* Generic stub method returning S_OK */
static HRESULT WINAPI diagnostics_stub_ok( void *iface, ... )
{
    return S_OK;
}

/* =========================================================================
 * LoggingChannelOptions
 * ========================================================================= */
struct logging_channel_options
{
    struct { const void *lpVtbl; } iface;
    LONG ref;
    GUID group;
};

static inline struct logging_channel_options *impl_from_logging_channel_options( void *iface )
{
    return CONTAINING_RECORD( iface, struct logging_channel_options, iface );
}

static HRESULT WINAPI logging_channel_options_QueryInterface( void *iface, REFIID riid, void **out )
{
    struct logging_channel_options *impl = impl_from_logging_channel_options( iface );
    if (!out) return E_POINTER;
    *out = NULL;

    *out = &impl->iface;
    InterlockedIncrement( &impl->ref );
    return S_OK;
}

static ULONG WINAPI logging_channel_options_AddRef( void *iface )
{
    struct logging_channel_options *impl = impl_from_logging_channel_options( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI logging_channel_options_Release( void *iface )
{
    struct logging_channel_options *impl = impl_from_logging_channel_options( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI logging_channel_options_GetIids( void *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI logging_channel_options_GetRuntimeClassName( void *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.Diagnostics.LoggingChannelOptions", 52, class_name );
}

static HRESULT WINAPI logging_channel_options_GetTrustLevel( void *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI logging_channel_options_get_Group( void *iface, GUID *value )
{
    struct logging_channel_options *impl = impl_from_logging_channel_options( iface );
    if (!value) return E_POINTER;
    *value = impl->group;
    return S_OK;
}

static HRESULT WINAPI logging_channel_options_put_Group( void *iface, const GUID *value )
{
    struct logging_channel_options *impl = impl_from_logging_channel_options( iface );
    if (value) impl->group = *value;
    return S_OK;
}

static const void *logging_channel_options_vtbl[] =
{
    logging_channel_options_QueryInterface,
    logging_channel_options_AddRef,
    logging_channel_options_Release,
    logging_channel_options_GetIids,
    logging_channel_options_GetRuntimeClassName,
    logging_channel_options_GetTrustLevel,
    logging_channel_options_get_Group,
    logging_channel_options_put_Group,
};

static HRESULT logging_channel_options_create( const GUID *group, void **out )
{
    struct logging_channel_options *impl;
    if (!out) return E_POINTER;
    impl = calloc( 1, sizeof(*impl) );
    if (!impl) return E_OUTOFMEMORY;
    impl->iface.lpVtbl = logging_channel_options_vtbl;
    impl->ref = 1;
    if (group) impl->group = *group;
    *out = &impl->iface;
    return S_OK;
}

/* LoggingChannelOptions Factory */
struct logging_channel_options_factory
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } ILoggingChannelOptionsFactory_iface;
    LONG ref;
};

static inline struct logging_channel_options_factory *impl_from_logging_channel_options_factory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct logging_channel_options_factory, IActivationFactory_iface );
}

static inline struct logging_channel_options_factory *impl_from_ILoggingChannelOptionsFactory( void *iface )
{
    return CONTAINING_RECORD( iface, struct logging_channel_options_factory, ILoggingChannelOptionsFactory_iface );
}

static HRESULT WINAPI logging_channel_options_factory_QueryInterface( IActivationFactory *iface, REFIID riid, void **out )
{
    struct logging_channel_options_factory *impl = impl_from_logging_channel_options_factory( iface );
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

    if (IsEqualGUID( riid, &IID_ILoggingChannelOptionsFactory ))
    {
        *out = &impl->ILoggingChannelOptionsFactory_iface;
        IActivationFactory_AddRef( iface );
        return S_OK;
    }

    TRACE( "QI for %s\n", debugstr_guid( riid ) );
    *out = &impl->ILoggingChannelOptionsFactory_iface;
    IActivationFactory_AddRef( iface );
    return S_OK;
}

static ULONG WINAPI logging_channel_options_factory_AddRef( IActivationFactory *iface )
{
    struct logging_channel_options_factory *impl = impl_from_logging_channel_options_factory( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI logging_channel_options_factory_Release( IActivationFactory *iface )
{
    struct logging_channel_options_factory *impl = impl_from_logging_channel_options_factory( iface );
    return InterlockedDecrement( &impl->ref );
}

static HRESULT WINAPI logging_channel_options_factory_GetIids( IActivationFactory *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI logging_channel_options_factory_GetRuntimeClassName( IActivationFactory *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.Diagnostics.LoggingChannelOptions", 52, class_name );
}

static HRESULT WINAPI logging_channel_options_factory_GetTrustLevel( IActivationFactory *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI logging_channel_options_factory_ActivateInstance( IActivationFactory *iface, IInspectable **instance )
{
    return logging_channel_options_create( NULL, (void **)instance );
}

static const struct IActivationFactoryVtbl logging_channel_options_activation_factory_vtbl =
{
    logging_channel_options_factory_QueryInterface,
    logging_channel_options_factory_AddRef,
    logging_channel_options_factory_Release,
    logging_channel_options_factory_GetIids,
    logging_channel_options_factory_GetRuntimeClassName,
    logging_channel_options_factory_GetTrustLevel,
    logging_channel_options_factory_ActivateInstance,
};

static HRESULT WINAPI logging_channel_options_factory2_QueryInterface( void *iface, REFIID riid, void **out )
{
    struct logging_channel_options_factory *impl = impl_from_ILoggingChannelOptionsFactory( iface );
    return logging_channel_options_factory_QueryInterface( &impl->IActivationFactory_iface, riid, out );
}

static ULONG WINAPI logging_channel_options_factory2_AddRef( void *iface )
{
    struct logging_channel_options_factory *impl = impl_from_ILoggingChannelOptionsFactory( iface );
    return logging_channel_options_factory_AddRef( &impl->IActivationFactory_iface );
}

static ULONG WINAPI logging_channel_options_factory2_Release( void *iface )
{
    struct logging_channel_options_factory *impl = impl_from_ILoggingChannelOptionsFactory( iface );
    return logging_channel_options_factory_Release( &impl->IActivationFactory_iface );
}

static HRESULT WINAPI logging_channel_options_factory2_GetIids( void *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI logging_channel_options_factory2_GetRuntimeClassName( void *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.Diagnostics.LoggingChannelOptions", 52, class_name );
}

static HRESULT WINAPI logging_channel_options_factory2_GetTrustLevel( void *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI logging_channel_options_factory2_CreateWithGroup( void *iface, const GUID *group, void **out )
{
    TRACE( "group %s, out %p\n", group ? debugstr_guid( group ) : "(null)", out );
    return logging_channel_options_create( group, out );
}

static const void *logging_channel_options_factory2_vtbl[] =
{
    logging_channel_options_factory2_QueryInterface,
    logging_channel_options_factory2_AddRef,
    logging_channel_options_factory2_Release,
    logging_channel_options_factory2_GetIids,
    logging_channel_options_factory2_GetRuntimeClassName,
    logging_channel_options_factory2_GetTrustLevel,
    logging_channel_options_factory2_CreateWithGroup,
};

static struct logging_channel_options_factory default_logging_channel_options_factory =
{
    {&logging_channel_options_activation_factory_vtbl},
    {logging_channel_options_factory2_vtbl},
    1
};

static IActivationFactory *logging_channel_options_factory = &default_logging_channel_options_factory.IActivationFactory_iface;


/* =========================================================================
 * LoggingOptions
 * ========================================================================= */
struct logging_options
{
    struct { const void *lpVtbl; } iface;
    LONG ref;
    INT64 keywords;
    INT32 tags;
    INT16 task;
    INT32 opcode;
    GUID activity_id;
    GUID related_activity_id;
};

static inline struct logging_options *impl_from_logging_options( void *iface )
{
    return CONTAINING_RECORD( iface, struct logging_options, iface );
}

static HRESULT WINAPI logging_options_QueryInterface( void *iface, REFIID riid, void **out )
{
    struct logging_options *impl = impl_from_logging_options( iface );
    if (!out) return E_POINTER;
    *out = &impl->iface;
    InterlockedIncrement( &impl->ref );
    return S_OK;
}

static ULONG WINAPI logging_options_AddRef( void *iface )
{
    struct logging_options *impl = impl_from_logging_options( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI logging_options_Release( void *iface )
{
    struct logging_options *impl = impl_from_logging_options( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI logging_options_GetIids( void *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI logging_options_GetRuntimeClassName( void *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.Diagnostics.LoggingOptions", 44, class_name );
}

static HRESULT WINAPI logging_options_GetTrustLevel( void *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI logging_options_get_Keywords( void *iface, INT64 *value )
{
    struct logging_options *impl = impl_from_logging_options( iface );
    if (!value) return E_POINTER;
    *value = impl->keywords;
    return S_OK;
}

static HRESULT WINAPI logging_options_put_Keywords( void *iface, INT64 value )
{
    struct logging_options *impl = impl_from_logging_options( iface );
    impl->keywords = value;
    return S_OK;
}

static HRESULT WINAPI logging_options_get_Tags( void *iface, INT32 *value )
{
    struct logging_options *impl = impl_from_logging_options( iface );
    if (!value) return E_POINTER;
    *value = impl->tags;
    return S_OK;
}

static HRESULT WINAPI logging_options_put_Tags( void *iface, INT32 value )
{
    struct logging_options *impl = impl_from_logging_options( iface );
    impl->tags = value;
    return S_OK;
}

static HRESULT WINAPI logging_options_get_Task( void *iface, INT16 *value )
{
    struct logging_options *impl = impl_from_logging_options( iface );
    if (!value) return E_POINTER;
    *value = impl->task;
    return S_OK;
}

static HRESULT WINAPI logging_options_put_Task( void *iface, INT16 value )
{
    struct logging_options *impl = impl_from_logging_options( iface );
    impl->task = value;
    return S_OK;
}

static HRESULT WINAPI logging_options_get_Opcode( void *iface, INT32 *value )
{
    struct logging_options *impl = impl_from_logging_options( iface );
    if (!value) return E_POINTER;
    *value = impl->opcode;
    return S_OK;
}

static HRESULT WINAPI logging_options_put_Opcode( void *iface, INT32 value )
{
    struct logging_options *impl = impl_from_logging_options( iface );
    impl->opcode = value;
    return S_OK;
}

static HRESULT WINAPI logging_options_get_ActivityId( void *iface, GUID *value )
{
    struct logging_options *impl = impl_from_logging_options( iface );
    if (!value) return E_POINTER;
    *value = impl->activity_id;
    return S_OK;
}

static HRESULT WINAPI logging_options_put_ActivityId( void *iface, const GUID *value )
{
    struct logging_options *impl = impl_from_logging_options( iface );
    if (value) impl->activity_id = *value;
    return S_OK;
}

static HRESULT WINAPI logging_options_get_RelatedActivityId( void *iface, GUID *value )
{
    struct logging_options *impl = impl_from_logging_options( iface );
    if (!value) return E_POINTER;
    *value = impl->related_activity_id;
    return S_OK;
}

static HRESULT WINAPI logging_options_put_RelatedActivityId( void *iface, const GUID *value )
{
    struct logging_options *impl = impl_from_logging_options( iface );
    if (value) impl->related_activity_id = *value;
    return S_OK;
}

static const void *logging_options_vtbl[] =
{
    logging_options_QueryInterface,
    logging_options_AddRef,
    logging_options_Release,
    logging_options_GetIids,
    logging_options_GetRuntimeClassName,
    logging_options_GetTrustLevel,
    logging_options_get_Keywords,
    logging_options_put_Keywords,
    logging_options_get_Tags,
    logging_options_put_Tags,
    logging_options_get_Task,
    logging_options_put_Task,
    logging_options_get_Opcode,
    logging_options_put_Opcode,
    logging_options_get_ActivityId,
    logging_options_put_ActivityId,
    logging_options_get_RelatedActivityId,
    logging_options_put_RelatedActivityId,
};

static HRESULT logging_options_create( INT64 keywords, void **out )
{
    struct logging_options *impl;
    if (!out) return E_POINTER;
    impl = calloc( 1, sizeof(*impl) );
    if (!impl) return E_OUTOFMEMORY;
    impl->iface.lpVtbl = logging_options_vtbl;
    impl->ref = 1;
    impl->keywords = keywords;
    *out = &impl->iface;
    return S_OK;
}

/* LoggingOptions Factory */
struct logging_options_factory
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } ILoggingOptionsFactory_iface;
    LONG ref;
};

static inline struct logging_options_factory *impl_from_logging_options_factory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct logging_options_factory, IActivationFactory_iface );
}

static inline struct logging_options_factory *impl_from_ILoggingOptionsFactory( void *iface )
{
    return CONTAINING_RECORD( iface, struct logging_options_factory, ILoggingOptionsFactory_iface );
}

static HRESULT WINAPI logging_options_factory_QueryInterface( IActivationFactory *iface, REFIID riid, void **out )
{
    struct logging_options_factory *impl = impl_from_logging_options_factory( iface );
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

    if (IsEqualGUID( riid, &IID_ILoggingOptionsFactory ))
    {
        *out = &impl->ILoggingOptionsFactory_iface;
        IActivationFactory_AddRef( iface );
        return S_OK;
    }

    TRACE( "QI for %s\n", debugstr_guid( riid ) );
    *out = &impl->ILoggingOptionsFactory_iface;
    IActivationFactory_AddRef( iface );
    return S_OK;
}

static ULONG WINAPI logging_options_factory_AddRef( IActivationFactory *iface )
{
    struct logging_options_factory *impl = impl_from_logging_options_factory( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI logging_options_factory_Release( IActivationFactory *iface )
{
    struct logging_options_factory *impl = impl_from_logging_options_factory( iface );
    return InterlockedDecrement( &impl->ref );
}

static HRESULT WINAPI logging_options_factory_GetIids( IActivationFactory *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI logging_options_factory_GetRuntimeClassName( IActivationFactory *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.Diagnostics.LoggingOptions", 44, class_name );
}

static HRESULT WINAPI logging_options_factory_GetTrustLevel( IActivationFactory *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI logging_options_factory_ActivateInstance( IActivationFactory *iface, IInspectable **instance )
{
    return logging_options_create( 0, (void **)instance );
}

static const struct IActivationFactoryVtbl logging_options_activation_factory_vtbl =
{
    logging_options_factory_QueryInterface,
    logging_options_factory_AddRef,
    logging_options_factory_Release,
    logging_options_factory_GetIids,
    logging_options_factory_GetRuntimeClassName,
    logging_options_factory_GetTrustLevel,
    logging_options_factory_ActivateInstance,
};

static HRESULT WINAPI logging_options_factory2_QueryInterface( void *iface, REFIID riid, void **out )
{
    struct logging_options_factory *impl = impl_from_ILoggingOptionsFactory( iface );
    return logging_options_factory_QueryInterface( &impl->IActivationFactory_iface, riid, out );
}

static ULONG WINAPI logging_options_factory2_AddRef( void *iface )
{
    struct logging_options_factory *impl = impl_from_ILoggingOptionsFactory( iface );
    return logging_options_factory_AddRef( &impl->IActivationFactory_iface );
}

static ULONG WINAPI logging_options_factory2_Release( void *iface )
{
    struct logging_options_factory *impl = impl_from_ILoggingOptionsFactory( iface );
    return logging_options_factory_Release( &impl->IActivationFactory_iface );
}

static HRESULT WINAPI logging_options_factory2_GetIids( void *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI logging_options_factory2_GetRuntimeClassName( void *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.Diagnostics.LoggingOptions", 44, class_name );
}

static HRESULT WINAPI logging_options_factory2_GetTrustLevel( void *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI logging_options_factory2_CreateWithKeywords( void *iface, INT64 keywords, void **out )
{
    TRACE( "keywords %lld, out %p\n", (long long)keywords, out );
    return logging_options_create( keywords, out );
}

static const void *logging_options_factory2_vtbl[] =
{
    logging_options_factory2_QueryInterface,
    logging_options_factory2_AddRef,
    logging_options_factory2_Release,
    logging_options_factory2_GetIids,
    logging_options_factory2_GetRuntimeClassName,
    logging_options_factory2_GetTrustLevel,
    logging_options_factory2_CreateWithKeywords,
};

static struct logging_options_factory default_logging_options_factory =
{
    {&logging_options_activation_factory_vtbl},
    {logging_options_factory2_vtbl},
    1
};

static IActivationFactory *logging_options_factory = &default_logging_options_factory.IActivationFactory_iface;


/* =========================================================================
 * LoggingChannel
 * ========================================================================= */
struct logging_channel
{
    struct { const void *lpVtbl; } iface;
    LONG ref;
    WCHAR name[128];
    GUID id;
    GUID group;
};

static inline struct logging_channel *impl_from_logging_channel( void *iface )
{
    return CONTAINING_RECORD( iface, struct logging_channel, iface );
}

static HRESULT WINAPI logging_channel_QueryInterface( void *iface, REFIID riid, void **out )
{
    struct logging_channel *impl = impl_from_logging_channel( iface );
    if (!out) return E_POINTER;
    *out = &impl->iface;
    InterlockedIncrement( &impl->ref );
    return S_OK;
}

static ULONG WINAPI logging_channel_AddRef( void *iface )
{
    struct logging_channel *impl = impl_from_logging_channel( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI logging_channel_Release( void *iface )
{
    struct logging_channel *impl = impl_from_logging_channel( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI logging_channel_GetIids( void *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI logging_channel_GetRuntimeClassName( void *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.Diagnostics.LoggingChannel", 44, class_name );
}

static HRESULT WINAPI logging_channel_GetTrustLevel( void *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI logging_channel_get_Name( void *iface, HSTRING *value )
{
    struct logging_channel *impl = impl_from_logging_channel( iface );
    if (!value) return E_POINTER;
    return WindowsCreateString( impl->name, wcslen(impl->name), value );
}

static HRESULT WINAPI logging_channel_get_Enabled( void *iface, BOOLEAN *value )
{
    if (!value) return E_POINTER;
    *value = TRUE;
    return S_OK;
}

static HRESULT WINAPI logging_channel_get_Level( void *iface, INT32 *value )
{
    if (!value) return E_POINTER;
    *value = 0;
    return S_OK;
}

static HRESULT WINAPI logging_channel_add_LoggingEnabled( void *iface, void *handler, EventRegistrationToken *token )
{
    if (token) token->value = 1;
    return S_OK;
}

static HRESULT WINAPI logging_channel_remove_LoggingEnabled( void *iface, EventRegistrationToken token )
{
    return S_OK;
}

static const void *logging_channel_vtbl[64] =
{
    logging_channel_QueryInterface,
    logging_channel_AddRef,
    logging_channel_Release,
    logging_channel_GetIids,
    logging_channel_GetRuntimeClassName,
    logging_channel_GetTrustLevel,
    logging_channel_get_Name,
    logging_channel_get_Enabled,
    logging_channel_get_Level,
    logging_channel_add_LoggingEnabled,
    logging_channel_remove_LoggingEnabled,
    diagnostics_stub_ok, /* LogMessage */
    diagnostics_stub_ok, /* LogMessageWithLevel */
    diagnostics_stub_ok, /* LogValuePair */
    diagnostics_stub_ok, /* LogValuePairWithLevel */
    diagnostics_stub_ok, /* LogEvent */
    diagnostics_stub_ok, /* LogEventWithFields */
    diagnostics_stub_ok, /* LogEventWithFieldsAndOptions */
    diagnostics_stub_ok, /* LogEventWithFieldsAndOptionsAndId */
    diagnostics_stub_ok, /* Close */
    diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok,
    diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok,
    diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok,
    diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok,
    diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok,
    diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok,
    diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok,
    diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok,
    diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok,
    diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok,
    diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok,
};

static HRESULT logging_channel_create( HSTRING name, void *options, const GUID *id, void **out )
{
    struct logging_channel *impl;
    const WCHAR *str;
    UINT32 len = 0;

    if (!out) return E_POINTER;
    impl = calloc( 1, sizeof(*impl) );
    if (!impl) return E_OUTOFMEMORY;
    impl->iface.lpVtbl = logging_channel_vtbl;
    impl->ref = 1;
    if (name)
    {
        str = WindowsGetStringRawBuffer( name, &len );
        if (str && len < 127) memcpy( impl->name, str, len * sizeof(WCHAR) );
    }
    if (id) impl->id = *id;
    TRACE( "created LoggingChannel name %s, out %p\n", debugstr_w(impl->name), out );
    *out = &impl->iface;
    return S_OK;
}

/* LoggingChannel Factory */
struct logging_channel_factory
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } ILoggingChannelFactory2_iface;
    LONG ref;
};

static inline struct logging_channel_factory *impl_from_logging_channel_factory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct logging_channel_factory, IActivationFactory_iface );
}

static inline struct logging_channel_factory *impl_from_ILoggingChannelFactory2( void *iface )
{
    return CONTAINING_RECORD( iface, struct logging_channel_factory, ILoggingChannelFactory2_iface );
}

static HRESULT WINAPI logging_channel_factory_QueryInterface( IActivationFactory *iface, REFIID riid, void **out )
{
    struct logging_channel_factory *impl = impl_from_logging_channel_factory( iface );
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

    if (IsEqualGUID( riid, &IID_ILoggingChannelFactory2 ))
    {
        *out = &impl->ILoggingChannelFactory2_iface;
        IActivationFactory_AddRef( iface );
        return S_OK;
    }

    TRACE( "QI for %s\n", debugstr_guid( riid ) );
    *out = &impl->ILoggingChannelFactory2_iface;
    IActivationFactory_AddRef( iface );
    return S_OK;
}

static ULONG WINAPI logging_channel_factory_AddRef( IActivationFactory *iface )
{
    struct logging_channel_factory *impl = impl_from_logging_channel_factory( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI logging_channel_factory_Release( IActivationFactory *iface )
{
    struct logging_channel_factory *impl = impl_from_logging_channel_factory( iface );
    return InterlockedDecrement( &impl->ref );
}

static HRESULT WINAPI logging_channel_factory_GetIids( IActivationFactory *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI logging_channel_factory_GetRuntimeClassName( IActivationFactory *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.Diagnostics.LoggingChannel", 44, class_name );
}

static HRESULT WINAPI logging_channel_factory_GetTrustLevel( IActivationFactory *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI logging_channel_factory_ActivateInstance( IActivationFactory *iface, IInspectable **instance )
{
    return logging_channel_create( NULL, NULL, NULL, (void **)instance );
}

static const struct IActivationFactoryVtbl logging_channel_activation_factory_vtbl =
{
    logging_channel_factory_QueryInterface,
    logging_channel_factory_AddRef,
    logging_channel_factory_Release,
    logging_channel_factory_GetIids,
    logging_channel_factory_GetRuntimeClassName,
    logging_channel_factory_GetTrustLevel,
    logging_channel_factory_ActivateInstance,
};

static HRESULT WINAPI logging_channel_factory2_QueryInterface( void *iface, REFIID riid, void **out )
{
    struct logging_channel_factory *impl = impl_from_ILoggingChannelFactory2( iface );
    return logging_channel_factory_QueryInterface( &impl->IActivationFactory_iface, riid, out );
}

static ULONG WINAPI logging_channel_factory2_AddRef( void *iface )
{
    struct logging_channel_factory *impl = impl_from_ILoggingChannelFactory2( iface );
    return logging_channel_factory_AddRef( &impl->IActivationFactory_iface );
}

static ULONG WINAPI logging_channel_factory2_Release( void *iface )
{
    struct logging_channel_factory *impl = impl_from_ILoggingChannelFactory2( iface );
    return logging_channel_factory_Release( &impl->IActivationFactory_iface );
}

static HRESULT WINAPI logging_channel_factory2_GetIids( void *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI logging_channel_factory2_GetRuntimeClassName( void *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.Diagnostics.LoggingChannel", 44, class_name );
}

static HRESULT WINAPI logging_channel_factory2_GetTrustLevel( void *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI logging_channel_factory2_CreateWithOptions( void *iface, HSTRING name, void *options, void **out )
{
    return logging_channel_create( name, options, NULL, out );
}

static HRESULT WINAPI logging_channel_factory2_CreateWithOptionsAndId( void *iface, HSTRING name, void *options, const GUID *id, void **out )
{
    return logging_channel_create( name, options, id, out );
}

static const void *logging_channel_factory2_vtbl[] =
{
    logging_channel_factory2_QueryInterface,
    logging_channel_factory2_AddRef,
    logging_channel_factory2_Release,
    logging_channel_factory2_GetIids,
    logging_channel_factory2_GetRuntimeClassName,
    logging_channel_factory2_GetTrustLevel,
    logging_channel_factory2_CreateWithOptions,
    logging_channel_factory2_CreateWithOptionsAndId,
};

static struct logging_channel_factory default_logging_channel_factory =
{
    {&logging_channel_activation_factory_vtbl},
    {logging_channel_factory2_vtbl},
    1
};

static IActivationFactory *logging_channel_factory = &default_logging_channel_factory.IActivationFactory_iface;


/* =========================================================================
 * LoggingFields
 * ========================================================================= */
struct logging_fields
{
    struct { const void *lpVtbl; } iface;
    LONG ref;
};

static inline struct logging_fields *impl_from_logging_fields( void *iface )
{
    return CONTAINING_RECORD( iface, struct logging_fields, iface );
}

static HRESULT WINAPI logging_fields_QueryInterface( void *iface, REFIID riid, void **out )
{
    struct logging_fields *impl = impl_from_logging_fields( iface );
    if (!out) return E_POINTER;
    *out = &impl->iface;
    InterlockedIncrement( &impl->ref );
    return S_OK;
}

static ULONG WINAPI logging_fields_AddRef( void *iface )
{
    struct logging_fields *impl = impl_from_logging_fields( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI logging_fields_Release( void *iface )
{
    struct logging_fields *impl = impl_from_logging_fields( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI logging_fields_GetIids( void *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI logging_fields_GetRuntimeClassName( void *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.Diagnostics.LoggingFields", 43, class_name );
}

static HRESULT WINAPI logging_fields_GetTrustLevel( void *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static const void *logging_fields_vtbl[64] =
{
    logging_fields_QueryInterface,
    logging_fields_AddRef,
    logging_fields_Release,
    logging_fields_GetIids,
    logging_fields_GetRuntimeClassName,
    logging_fields_GetTrustLevel,
    diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok,
    diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok,
    diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok,
    diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok,
    diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok,
    diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok,
    diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok,
    diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok,
    diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok,
    diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok,
    diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok,
    diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok,
    diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok,
    diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok,
    diagnostics_stub_ok, diagnostics_stub_ok,
};

static HRESULT logging_fields_create( void **out )
{
    struct logging_fields *impl;
    if (!out) return E_POINTER;
    impl = calloc( 1, sizeof(*impl) );
    if (!impl) return E_OUTOFMEMORY;
    impl->iface.lpVtbl = logging_fields_vtbl;
    impl->ref = 1;
    *out = &impl->iface;
    return S_OK;
}

/* LoggingFields Factory */
struct logging_fields_factory
{
    IActivationFactory IActivationFactory_iface;
    LONG ref;
};

static inline struct logging_fields_factory *impl_from_logging_fields_factory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct logging_fields_factory, IActivationFactory_iface );
}

static HRESULT WINAPI logging_fields_factory_QueryInterface( IActivationFactory *iface, REFIID riid, void **out )
{
    struct logging_fields_factory *impl = impl_from_logging_fields_factory( iface );
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

    *out = &impl->IActivationFactory_iface;
    IActivationFactory_AddRef( iface );
    return S_OK;
}

static ULONG WINAPI logging_fields_factory_AddRef( IActivationFactory *iface )
{
    struct logging_fields_factory *impl = impl_from_logging_fields_factory( iface );
    return InterlockedIncrement( &impl->ref );
}

static ULONG WINAPI logging_fields_factory_Release( IActivationFactory *iface )
{
    struct logging_fields_factory *impl = impl_from_logging_fields_factory( iface );
    return InterlockedDecrement( &impl->ref );
}

static HRESULT WINAPI logging_fields_factory_GetIids( IActivationFactory *iface, ULONG *iid_count, IID **iids )
{
    if (iid_count) *iid_count = 0;
    return S_OK;
}

static HRESULT WINAPI logging_fields_factory_GetRuntimeClassName( IActivationFactory *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.Diagnostics.LoggingFields", 43, class_name );
}

static HRESULT WINAPI logging_fields_factory_GetTrustLevel( IActivationFactory *iface, TrustLevel *trust_level )
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI logging_fields_factory_ActivateInstance( IActivationFactory *iface, IInspectable **instance )
{
    return logging_fields_create( (void **)instance );
}

static const struct IActivationFactoryVtbl logging_fields_activation_factory_vtbl =
{
    logging_fields_factory_QueryInterface,
    logging_fields_factory_AddRef,
    logging_fields_factory_Release,
    logging_fields_factory_GetIids,
    logging_fields_factory_GetRuntimeClassName,
    logging_fields_factory_GetTrustLevel,
    logging_fields_factory_ActivateInstance,
};

static struct logging_fields_factory default_logging_fields_factory =
{
    {&logging_fields_activation_factory_vtbl},
    1
};

static IActivationFactory *logging_fields_factory = &default_logging_fields_factory.IActivationFactory_iface;

/* =========================================================================
 * AsyncCausalityTracer
 * ========================================================================= */
static const GUID IID_IAsyncCausalityTracerStatics =
    {0x50850b26, 0x267e, 0x451b, {0xa8, 0x90, 0xab, 0x6a, 0x37, 0x02, 0x45, 0xee}};

static HRESULT WINAPI causality_tracer_QueryInterface( void *iface, REFIID riid, void **out )
{
    if (!out) return E_POINTER;
    *out = iface;
    return S_OK;
}
static ULONG WINAPI causality_tracer_AddRef( void *iface ) { return 2; }
static ULONG WINAPI causality_tracer_Release( void *iface ) { return 1; }
static HRESULT WINAPI causality_tracer_GetIids( void *iface, ULONG *c, IID **i ) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI causality_tracer_GetRuntimeClassName( void *iface, HSTRING *s )
{
    return WindowsCreateString( L"Windows.Foundation.Diagnostics.AsyncCausalityTracer", 51, s );
}
static HRESULT WINAPI causality_tracer_GetTrustLevel( void *iface, TrustLevel *t ) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI causality_tracer_ActivateInstance( void *iface, IInspectable **inst ) { return E_NOTIMPL; }

static const void *causality_tracer_vtbl[16] = {
    causality_tracer_QueryInterface,
    causality_tracer_AddRef,
    causality_tracer_Release,
    causality_tracer_GetIids,
    causality_tracer_GetRuntimeClassName,
    causality_tracer_GetTrustLevel,
    causality_tracer_ActivateInstance,
    diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok,
    diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok,
    diagnostics_stub_ok, diagnostics_stub_ok, diagnostics_stub_ok
};

static struct { const void *lpVtbl; } default_causality_tracer_factory = { causality_tracer_vtbl };
static IActivationFactory *async_causality_tracer_factory = (IActivationFactory *)&default_causality_tracer_factory;
