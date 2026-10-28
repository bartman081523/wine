/* WinRT Windows.UI Notifications Implementation
 *
 * Copyright (C) 2026 Antigravity
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

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(ui);

/* ========================================================================= */
/* BadgeUpdater Implementation                                               */
/* ========================================================================= */

struct badge_updater
{
    IBadgeUpdater IBadgeUpdater_iface;
    LONG ref;
};

static inline struct badge_updater *impl_from_IBadgeUpdater( IBadgeUpdater *iface )
{
    return CONTAINING_RECORD( iface, struct badge_updater, IBadgeUpdater_iface );
}

static HRESULT WINAPI badge_updater_QueryInterface( IBadgeUpdater *iface, REFIID iid, void **out )
{
    struct badge_updater *impl = impl_from_IBadgeUpdater( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IBadgeUpdater ))
    {
        *out = &impl->IBadgeUpdater_iface;
        IInspectable_AddRef( (IInspectable *)*out );
        return S_OK;
    }

    FIXME( "interface %s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI badge_updater_AddRef( IBadgeUpdater *iface )
{
    struct badge_updater *impl = impl_from_IBadgeUpdater( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing ref to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI badge_updater_Release( IBadgeUpdater *iface )
{
    struct badge_updater *impl = impl_from_IBadgeUpdater( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p decreasing ref to %lu.\n", iface, ref );
    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI badge_updater_GetIids( IBadgeUpdater *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI badge_updater_GetRuntimeClassName( IBadgeUpdater *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.UI.Notifications.BadgeUpdater", 37, class_name );
}

static HRESULT WINAPI badge_updater_GetTrustLevel( IBadgeUpdater *iface, TrustLevel *trust_level )
{
    TRACE( "iface %p, trust_level %p.\n", iface, trust_level );
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI badge_updater_Update( IBadgeUpdater *iface, IBadgeNotification *notification )
{
    TRACE( "iface %p, notification %p.\n", iface, notification );
    return S_OK;
}

static HRESULT WINAPI badge_updater_Clear( IBadgeUpdater *iface )
{
    TRACE( "iface %p.\n", iface );
    return S_OK;
}

static HRESULT WINAPI badge_updater_StartPeriodicUpdate( IBadgeUpdater *iface, IUriRuntimeClass *badge_content, PeriodicUpdateRecurrence interval )
{
    FIXME( "iface %p, badge_content %p, interval %d stub!\n", iface, badge_content, interval );
    return S_OK;
}

static HRESULT WINAPI badge_updater_StartPeriodicUpdateAtTime( IBadgeUpdater *iface, IUriRuntimeClass *badge_content, DateTime start_time, PeriodicUpdateRecurrence interval )
{
    FIXME( "iface %p, badge_content %p, interval %d stub!\n", iface, badge_content, interval );
    return S_OK;
}

static HRESULT WINAPI badge_updater_StopPeriodicUpdate( IBadgeUpdater *iface )
{
    TRACE( "iface %p.\n", iface );
    return S_OK;
}

static const struct IBadgeUpdaterVtbl badge_updater_vtbl =
{
    badge_updater_QueryInterface,
    badge_updater_AddRef,
    badge_updater_Release,
    badge_updater_GetIids,
    badge_updater_GetRuntimeClassName,
    badge_updater_GetTrustLevel,
    badge_updater_Update,
    badge_updater_Clear,
    badge_updater_StartPeriodicUpdate,
    badge_updater_StartPeriodicUpdateAtTime,
    badge_updater_StopPeriodicUpdate,
};

static HRESULT badge_updater_create( IBadgeUpdater **out )
{
    struct badge_updater *impl;

    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IBadgeUpdater_iface.lpVtbl = &badge_updater_vtbl;
    impl->ref = 1;
    *out = &impl->IBadgeUpdater_iface;
    return S_OK;
}

/* ========================================================================= */
/* BadgeUpdateManagerStatics Implementation                                  */
/* ========================================================================= */

struct badge_update_manager_statics
{
    IActivationFactory IActivationFactory_iface;
    IBadgeUpdateManagerStatics IBadgeUpdateManagerStatics_iface;
    LONG ref;
};

static inline struct badge_update_manager_statics *impl_from_IActivationFactory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct badge_update_manager_statics, IActivationFactory_iface );
}

static inline struct badge_update_manager_statics *impl_from_IBadgeUpdateManagerStatics( IBadgeUpdateManagerStatics *iface )
{
    return CONTAINING_RECORD( iface, struct badge_update_manager_statics, IBadgeUpdateManagerStatics_iface );
}

static HRESULT WINAPI badge_factory_QueryInterface( IActivationFactory *iface, REFIID iid, void **out )
{
    struct badge_update_manager_statics *impl = impl_from_IActivationFactory( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IActivationFactory ))
    {
        *out = &impl->IActivationFactory_iface;
        IInspectable_AddRef( (IInspectable *)*out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IBadgeUpdateManagerStatics ))
    {
        *out = &impl->IBadgeUpdateManagerStatics_iface;
        IInspectable_AddRef( (IInspectable *)*out );
        return S_OK;
    }

    FIXME( "interface %s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI badge_factory_AddRef( IActivationFactory *iface )
{
    return 2;
}

static ULONG WINAPI badge_factory_Release( IActivationFactory *iface )
{
    return 1;
}

static HRESULT WINAPI badge_factory_GetIids( IActivationFactory *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI badge_factory_GetRuntimeClassName( IActivationFactory *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.UI.Notifications.BadgeUpdateManager", 43, class_name );
}

static HRESULT WINAPI badge_factory_GetTrustLevel( IActivationFactory *iface, TrustLevel *trust_level )
{
    TRACE( "iface %p, trust_level %p.\n", iface, trust_level );
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI badge_factory_ActivateInstance( IActivationFactory *iface, IInspectable **instance )
{
    FIXME( "iface %p, instance %p stub!\n", iface, instance );
    return E_NOTIMPL;
}

static const struct IActivationFactoryVtbl badge_factory_vtbl =
{
    badge_factory_QueryInterface,
    badge_factory_AddRef,
    badge_factory_Release,
    badge_factory_GetIids,
    badge_factory_GetRuntimeClassName,
    badge_factory_GetTrustLevel,
    badge_factory_ActivateInstance,
};

static HRESULT WINAPI badge_statics_QueryInterface( IBadgeUpdateManagerStatics *iface, REFIID iid, void **out )
{
    struct badge_update_manager_statics *impl = impl_from_IBadgeUpdateManagerStatics( iface );
    return badge_factory_QueryInterface( &impl->IActivationFactory_iface, iid, out );
}

static ULONG WINAPI badge_statics_AddRef( IBadgeUpdateManagerStatics *iface )
{
    return 2;
}

static ULONG WINAPI badge_statics_Release( IBadgeUpdateManagerStatics *iface )
{
    return 1;
}

static HRESULT WINAPI badge_statics_GetIids( IBadgeUpdateManagerStatics *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI badge_statics_GetRuntimeClassName( IBadgeUpdateManagerStatics *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.UI.Notifications.BadgeUpdateManager", 43, class_name );
}

static HRESULT WINAPI badge_statics_GetTrustLevel( IBadgeUpdateManagerStatics *iface, TrustLevel *trust_level )
{
    TRACE( "iface %p, trust_level %p.\n", iface, trust_level );
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI badge_statics_CreateBadgeUpdaterForApplication( IBadgeUpdateManagerStatics *iface, IBadgeUpdater **result )
{
    TRACE( "iface %p, result %p.\n", iface, result );
    return badge_updater_create( result );
}

static HRESULT WINAPI badge_statics_CreateBadgeUpdaterForApplicationWithId( IBadgeUpdateManagerStatics *iface, HSTRING application_id, IBadgeUpdater **result )
{
    TRACE( "iface %p, application_id %s, result %p.\n", iface, debugstr_hstring(application_id), result );
    return badge_updater_create( result );
}

static HRESULT WINAPI badge_statics_CreateBadgeUpdaterForSecondaryTile( IBadgeUpdateManagerStatics *iface, HSTRING tile_id, IBadgeUpdater **result )
{
    TRACE( "iface %p, tile_id %s, result %p.\n", iface, debugstr_hstring(tile_id), result );
    return badge_updater_create( result );
}

static HRESULT WINAPI badge_statics_GetTemplateContent( IBadgeUpdateManagerStatics *iface, BadgeTemplateType type, __x_ABI_CWindows_CData_CXml_CDom_CIXmlDocument **result )
{
    FIXME( "iface %p, type %d, result %p stub!\n", iface, type, result );
    if (result) *result = NULL;
    return E_NOTIMPL;
}

static const struct IBadgeUpdateManagerStaticsVtbl badge_statics_vtbl =
{
    badge_statics_QueryInterface,
    badge_statics_AddRef,
    badge_statics_Release,
    badge_statics_GetIids,
    badge_statics_GetRuntimeClassName,
    badge_statics_GetTrustLevel,
    badge_statics_CreateBadgeUpdaterForApplication,
    badge_statics_CreateBadgeUpdaterForApplicationWithId,
    badge_statics_CreateBadgeUpdaterForSecondaryTile,
    badge_statics_GetTemplateContent,
};

static struct badge_update_manager_statics default_badge_update_manager_statics =
{
    {&badge_factory_vtbl},
    {&badge_statics_vtbl},
    1,
};

IActivationFactory *badgeupdatemanager_factory = &default_badge_update_manager_statics.IActivationFactory_iface;

/* ========================================================================= */
/* ToastNotifier Implementation                                              */
/* ========================================================================= */

struct toast_notifier
{
    IToastNotifier IToastNotifier_iface;
    LONG ref;
};

static inline struct toast_notifier *impl_from_IToastNotifier( IToastNotifier *iface )
{
    return CONTAINING_RECORD( iface, struct toast_notifier, IToastNotifier_iface );
}

static HRESULT WINAPI toast_notifier_QueryInterface( IToastNotifier *iface, REFIID iid, void **out )
{
    struct toast_notifier *impl = impl_from_IToastNotifier( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IToastNotifier ))
    {
        *out = &impl->IToastNotifier_iface;
        IInspectable_AddRef( (IInspectable *)*out );
        return S_OK;
    }

    FIXME( "interface %s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI toast_notifier_AddRef( IToastNotifier *iface )
{
    struct toast_notifier *impl = impl_from_IToastNotifier( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing ref to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI toast_notifier_Release( IToastNotifier *iface )
{
    struct toast_notifier *impl = impl_from_IToastNotifier( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p decreasing ref to %lu.\n", iface, ref );
    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI toast_notifier_GetIids( IToastNotifier *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI toast_notifier_GetRuntimeClassName( IToastNotifier *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.UI.Notifications.ToastNotifier", 38, class_name );
}

static HRESULT WINAPI toast_notifier_GetTrustLevel( IToastNotifier *iface, TrustLevel *trust_level )
{
    TRACE( "iface %p, trust_level %p.\n", iface, trust_level );
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI toast_notifier_Show( IToastNotifier *iface, IToastNotification *notification )
{
    TRACE( "iface %p, notification %p.\n", iface, notification );
    return S_OK;
}

static HRESULT WINAPI toast_notifier_Hide( IToastNotifier *iface, IToastNotification *notification )
{
    TRACE( "iface %p, notification %p.\n", iface, notification );
    return S_OK;
}

static HRESULT WINAPI toast_notifier_get_Setting( IToastNotifier *iface, NotificationSetting *value )
{
    TRACE( "iface %p, value %p.\n", iface, value );
    if (!value) return E_POINTER;
    *value = NotificationSetting_Enabled;
    return S_OK;
}

static HRESULT WINAPI toast_notifier_AddToSchedule( IToastNotifier *iface, IScheduledToastNotification *scheduled_toast )
{
    FIXME( "iface %p, scheduled_toast %p stub!\n", iface, scheduled_toast );
    return S_OK;
}

static HRESULT WINAPI toast_notifier_RemoveFromSchedule( IToastNotifier *iface, IScheduledToastNotification *scheduled_toast )
{
    FIXME( "iface %p, scheduled_toast %p stub!\n", iface, scheduled_toast );
    return S_OK;
}

struct empty_scheduled_toast_iterator
{
    __FIIterator_1_Windows__CUI__CNotifications__CScheduledToastNotification __FIIterator_1_Windows__CUI__CNotifications__CScheduledToastNotification_iface;
    LONG ref;
};

static inline struct empty_scheduled_toast_iterator *impl_from_empty_scheduled_toast_iterator( __FIIterator_1_Windows__CUI__CNotifications__CScheduledToastNotification *iface )
{
    return CONTAINING_RECORD( iface, struct empty_scheduled_toast_iterator, __FIIterator_1_Windows__CUI__CNotifications__CScheduledToastNotification_iface );
}

static HRESULT WINAPI empty_scheduled_toast_iterator_QueryInterface( __FIIterator_1_Windows__CUI__CNotifications__CScheduledToastNotification *iface,
                                                                      REFIID iid, void **out )
{
    struct empty_scheduled_toast_iterator *impl = impl_from_empty_scheduled_toast_iterator( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID___FIIterator_1_Windows__CUI__CNotifications__CScheduledToastNotification ))
    {
        *out = &impl->__FIIterator_1_Windows__CUI__CNotifications__CScheduledToastNotification_iface;
        IInspectable_AddRef( (IInspectable *)*out );
        return S_OK;
    }

    WARN( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI empty_scheduled_toast_iterator_AddRef( __FIIterator_1_Windows__CUI__CNotifications__CScheduledToastNotification *iface )
{
    struct empty_scheduled_toast_iterator *impl = impl_from_empty_scheduled_toast_iterator( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI empty_scheduled_toast_iterator_Release( __FIIterator_1_Windows__CUI__CNotifications__CScheduledToastNotification *iface )
{
    struct empty_scheduled_toast_iterator *impl = impl_from_empty_scheduled_toast_iterator( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );
    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI empty_scheduled_toast_iterator_GetIids( __FIIterator_1_Windows__CUI__CNotifications__CScheduledToastNotification *iface,
                                                              ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI empty_scheduled_toast_iterator_GetRuntimeClassName( __FIIterator_1_Windows__CUI__CNotifications__CScheduledToastNotification *iface,
                                                                          HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.Collections.IIterator`1<Windows.UI.Notifications.ScheduledToastNotification>",
                                ARRAY_SIZE(L"Windows.Foundation.Collections.IIterator`1<Windows.UI.Notifications.ScheduledToastNotification>"),
                                class_name );
}

static HRESULT WINAPI empty_scheduled_toast_iterator_GetTrustLevel( __FIIterator_1_Windows__CUI__CNotifications__CScheduledToastNotification *iface,
                                                                    TrustLevel *trust_level )
{
    if (!trust_level) return E_POINTER;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI empty_scheduled_toast_iterator_get_Current( __FIIterator_1_Windows__CUI__CNotifications__CScheduledToastNotification *iface,
                                                                  __x_ABI_CWindows_CUI_CNotifications_CIScheduledToastNotification **value )
{
    TRACE( "iface %p, value %p.\n", iface, value );
    if (value) *value = NULL;
    return 0x8000000b; /* E_BOUNDS */
}

static HRESULT WINAPI empty_scheduled_toast_iterator_get_HasCurrent( __FIIterator_1_Windows__CUI__CNotifications__CScheduledToastNotification *iface,
                                                                     boolean *value )
{
    TRACE( "iface %p, value %p.\n", iface, value );
    if (!value) return E_POINTER;
    *value = FALSE;
    return S_OK;
}

static HRESULT WINAPI empty_scheduled_toast_iterator_MoveNext( __FIIterator_1_Windows__CUI__CNotifications__CScheduledToastNotification *iface,
                                                               boolean *value )
{
    TRACE( "iface %p, value %p.\n", iface, value );
    if (!value) return E_POINTER;
    *value = FALSE;
    return S_OK;
}

static HRESULT WINAPI empty_scheduled_toast_iterator_GetMany( __FIIterator_1_Windows__CUI__CNotifications__CScheduledToastNotification *iface,
                                                              UINT32 items_size,
                                                              __x_ABI_CWindows_CUI_CNotifications_CIScheduledToastNotification **items,
                                                              UINT32 *value )
{
    TRACE( "iface %p, items_size %u, items %p, value %p.\n", iface, items_size, items, value );
    if (value) *value = 0;
    return S_OK;
}

static const struct __FIIterator_1_Windows__CUI__CNotifications__CScheduledToastNotificationVtbl empty_scheduled_toast_iterator_vtbl =
{
    empty_scheduled_toast_iterator_QueryInterface,
    empty_scheduled_toast_iterator_AddRef,
    empty_scheduled_toast_iterator_Release,
    empty_scheduled_toast_iterator_GetIids,
    empty_scheduled_toast_iterator_GetRuntimeClassName,
    empty_scheduled_toast_iterator_GetTrustLevel,
    empty_scheduled_toast_iterator_get_Current,
    empty_scheduled_toast_iterator_get_HasCurrent,
    empty_scheduled_toast_iterator_MoveNext,
    empty_scheduled_toast_iterator_GetMany,
};

static HRESULT empty_scheduled_toast_iterator_create( __FIIterator_1_Windows__CUI__CNotifications__CScheduledToastNotification **out )
{
    struct empty_scheduled_toast_iterator *impl;

    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->__FIIterator_1_Windows__CUI__CNotifications__CScheduledToastNotification_iface.lpVtbl = &empty_scheduled_toast_iterator_vtbl;
    impl->ref = 1;
    *out = &impl->__FIIterator_1_Windows__CUI__CNotifications__CScheduledToastNotification_iface;
    return S_OK;
}

struct empty_scheduled_toast_vector_view
{
    __FIVectorView_1_Windows__CUI__CNotifications__CScheduledToastNotification __FIVectorView_1_Windows__CUI__CNotifications__CScheduledToastNotification_iface;
    __FIIterable_1_Windows__CUI__CNotifications__CScheduledToastNotification __FIIterable_1_Windows__CUI__CNotifications__CScheduledToastNotification_iface;
    LONG ref;
};

static inline struct empty_scheduled_toast_vector_view *impl_from_empty_scheduled_toast_vector_view( __FIVectorView_1_Windows__CUI__CNotifications__CScheduledToastNotification *iface )
{
    return CONTAINING_RECORD( iface, struct empty_scheduled_toast_vector_view, __FIVectorView_1_Windows__CUI__CNotifications__CScheduledToastNotification_iface );
}

static inline struct empty_scheduled_toast_vector_view *impl_from_empty_scheduled_toast_iterable( __FIIterable_1_Windows__CUI__CNotifications__CScheduledToastNotification *iface )
{
    return CONTAINING_RECORD( iface, struct empty_scheduled_toast_vector_view, __FIIterable_1_Windows__CUI__CNotifications__CScheduledToastNotification_iface );
}

static HRESULT WINAPI empty_scheduled_toast_vector_view_QueryInterface( __FIVectorView_1_Windows__CUI__CNotifications__CScheduledToastNotification *iface,
                                                                         REFIID iid, void **out )
{
    struct empty_scheduled_toast_vector_view *impl = impl_from_empty_scheduled_toast_vector_view( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID___FIVectorView_1_Windows__CUI__CNotifications__CScheduledToastNotification ))
    {
        *out = &impl->__FIVectorView_1_Windows__CUI__CNotifications__CScheduledToastNotification_iface;
        IInspectable_AddRef( (IInspectable *)*out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID___FIIterable_1_Windows__CUI__CNotifications__CScheduledToastNotification ))
    {
        *out = &impl->__FIIterable_1_Windows__CUI__CNotifications__CScheduledToastNotification_iface;
        IInspectable_AddRef( (IInspectable *)*out );
        return S_OK;
    }

    WARN( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI empty_scheduled_toast_vector_view_AddRef( __FIVectorView_1_Windows__CUI__CNotifications__CScheduledToastNotification *iface )
{
    struct empty_scheduled_toast_vector_view *impl = impl_from_empty_scheduled_toast_vector_view( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI empty_scheduled_toast_vector_view_Release( __FIVectorView_1_Windows__CUI__CNotifications__CScheduledToastNotification *iface )
{
    struct empty_scheduled_toast_vector_view *impl = impl_from_empty_scheduled_toast_vector_view( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );
    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI empty_scheduled_toast_iterable_QueryInterface( __FIIterable_1_Windows__CUI__CNotifications__CScheduledToastNotification *iface,
                                                                      REFIID iid, void **out )
{
    struct empty_scheduled_toast_vector_view *impl = impl_from_empty_scheduled_toast_iterable( iface );
    return empty_scheduled_toast_vector_view_QueryInterface( &impl->__FIVectorView_1_Windows__CUI__CNotifications__CScheduledToastNotification_iface, iid, out );
}

static ULONG WINAPI empty_scheduled_toast_iterable_AddRef( __FIIterable_1_Windows__CUI__CNotifications__CScheduledToastNotification *iface )
{
    struct empty_scheduled_toast_vector_view *impl = impl_from_empty_scheduled_toast_iterable( iface );
    return empty_scheduled_toast_vector_view_AddRef( &impl->__FIVectorView_1_Windows__CUI__CNotifications__CScheduledToastNotification_iface );
}

static ULONG WINAPI empty_scheduled_toast_iterable_Release( __FIIterable_1_Windows__CUI__CNotifications__CScheduledToastNotification *iface )
{
    struct empty_scheduled_toast_vector_view *impl = impl_from_empty_scheduled_toast_iterable( iface );
    return empty_scheduled_toast_vector_view_Release( &impl->__FIVectorView_1_Windows__CUI__CNotifications__CScheduledToastNotification_iface );
}

static HRESULT WINAPI empty_scheduled_toast_iterable_GetIids( __FIIterable_1_Windows__CUI__CNotifications__CScheduledToastNotification *iface,
                                                              ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI empty_scheduled_toast_iterable_GetRuntimeClassName( __FIIterable_1_Windows__CUI__CNotifications__CScheduledToastNotification *iface,
                                                                          HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.Collections.IIterable`1<Windows.UI.Notifications.ScheduledToastNotification>",
                                ARRAY_SIZE(L"Windows.Foundation.Collections.IIterable`1<Windows.UI.Notifications.ScheduledToastNotification>"),
                                class_name );
}

static HRESULT WINAPI empty_scheduled_toast_iterable_GetTrustLevel( __FIIterable_1_Windows__CUI__CNotifications__CScheduledToastNotification *iface,
                                                                    TrustLevel *trust_level )
{
    if (!trust_level) return E_POINTER;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI empty_scheduled_toast_iterable_First( __FIIterable_1_Windows__CUI__CNotifications__CScheduledToastNotification *iface,
                                                            __FIIterator_1_Windows__CUI__CNotifications__CScheduledToastNotification **value )
{
    TRACE( "iface %p, value %p.\n", iface, value );
    if (!value) return E_POINTER;
    return empty_scheduled_toast_iterator_create( value );
}

static const struct __FIIterable_1_Windows__CUI__CNotifications__CScheduledToastNotificationVtbl empty_scheduled_toast_iterable_vtbl =
{
    empty_scheduled_toast_iterable_QueryInterface,
    empty_scheduled_toast_iterable_AddRef,
    empty_scheduled_toast_iterable_Release,
    empty_scheduled_toast_iterable_GetIids,
    empty_scheduled_toast_iterable_GetRuntimeClassName,
    empty_scheduled_toast_iterable_GetTrustLevel,
    empty_scheduled_toast_iterable_First,
};

static HRESULT WINAPI empty_scheduled_toast_vector_view_GetIids( __FIVectorView_1_Windows__CUI__CNotifications__CScheduledToastNotification *iface,
                                                                 ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI empty_scheduled_toast_vector_view_GetRuntimeClassName( __FIVectorView_1_Windows__CUI__CNotifications__CScheduledToastNotification *iface,
                                                                             HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.Collections.IVectorView`1<Windows.UI.Notifications.ScheduledToastNotification>",
                                ARRAY_SIZE(L"Windows.Foundation.Collections.IVectorView`1<Windows.UI.Notifications.ScheduledToastNotification>"),
                                class_name );
}

static HRESULT WINAPI empty_scheduled_toast_vector_view_GetTrustLevel( __FIVectorView_1_Windows__CUI__CNotifications__CScheduledToastNotification *iface,
                                                                       TrustLevel *trust_level )
{
    if (!trust_level) return E_POINTER;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI empty_scheduled_toast_vector_view_GetAt( __FIVectorView_1_Windows__CUI__CNotifications__CScheduledToastNotification *iface,
                                                               UINT32 index, __x_ABI_CWindows_CUI_CNotifications_CIScheduledToastNotification **value )
{
    TRACE( "iface %p, index %u, value %p.\n", iface, index, value );
    if (value) *value = NULL;
    return 0x8000000b; /* E_BOUNDS */
}

static HRESULT WINAPI empty_scheduled_toast_vector_view_get_Size( __FIVectorView_1_Windows__CUI__CNotifications__CScheduledToastNotification *iface,
                                                                  UINT32 *value )
{
    TRACE( "iface %p, value %p.\n", iface, value );
    if (!value) return E_POINTER;
    *value = 0;
    return S_OK;
}

static HRESULT WINAPI empty_scheduled_toast_vector_view_IndexOf( __FIVectorView_1_Windows__CUI__CNotifications__CScheduledToastNotification *iface,
                                                                 __x_ABI_CWindows_CUI_CNotifications_CIScheduledToastNotification *element,
                                                                 UINT32 *index, BOOLEAN *value )
{
    TRACE( "iface %p, element %p, index %p, value %p.\n", iface, element, index, value );
    if (index) *index = 0;
    if (value) *value = FALSE;
    return S_OK;
}

static HRESULT WINAPI empty_scheduled_toast_vector_view_GetMany( __FIVectorView_1_Windows__CUI__CNotifications__CScheduledToastNotification *iface,
                                                                 UINT32 start_index, UINT32 items_size,
                                                                 __x_ABI_CWindows_CUI_CNotifications_CIScheduledToastNotification **items,
                                                                 UINT32 *value )
{
    TRACE( "iface %p, start_index %u, items_size %u, items %p, value %p.\n", iface, start_index, items_size, items, value );
    if (value) *value = 0;
    return S_OK;
}

static const struct __FIVectorView_1_Windows__CUI__CNotifications__CScheduledToastNotificationVtbl empty_scheduled_toast_vector_view_vtbl =
{
    empty_scheduled_toast_vector_view_QueryInterface,
    empty_scheduled_toast_vector_view_AddRef,
    empty_scheduled_toast_vector_view_Release,
    empty_scheduled_toast_vector_view_GetIids,
    empty_scheduled_toast_vector_view_GetRuntimeClassName,
    empty_scheduled_toast_vector_view_GetTrustLevel,
    empty_scheduled_toast_vector_view_GetAt,
    empty_scheduled_toast_vector_view_get_Size,
    empty_scheduled_toast_vector_view_IndexOf,
    empty_scheduled_toast_vector_view_GetMany,
};

static HRESULT empty_scheduled_toast_vector_view_create( __FIVectorView_1_Windows__CUI__CNotifications__CScheduledToastNotification **out )
{
    struct empty_scheduled_toast_vector_view *impl;

    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->__FIVectorView_1_Windows__CUI__CNotifications__CScheduledToastNotification_iface.lpVtbl = &empty_scheduled_toast_vector_view_vtbl;
    impl->__FIIterable_1_Windows__CUI__CNotifications__CScheduledToastNotification_iface.lpVtbl = &empty_scheduled_toast_iterable_vtbl;
    impl->ref = 1;
    *out = &impl->__FIVectorView_1_Windows__CUI__CNotifications__CScheduledToastNotification_iface;
    return S_OK;
}

static HRESULT WINAPI toast_notifier_GetScheduledToastNotifications( IToastNotifier *iface, __FIVectorView_1_Windows__CUI__CNotifications__CScheduledToastNotification **result )
{
    TRACE( "iface %p, result %p.\n", iface, result );
    if (!result) return E_POINTER;
    return empty_scheduled_toast_vector_view_create( result );
}

static const struct IToastNotifierVtbl toast_notifier_vtbl =
{
    toast_notifier_QueryInterface,
    toast_notifier_AddRef,
    toast_notifier_Release,
    toast_notifier_GetIids,
    toast_notifier_GetRuntimeClassName,
    toast_notifier_GetTrustLevel,
    toast_notifier_Show,
    toast_notifier_Hide,
    toast_notifier_get_Setting,
    toast_notifier_AddToSchedule,
    toast_notifier_RemoveFromSchedule,
    toast_notifier_GetScheduledToastNotifications,
};

static HRESULT toast_notifier_create( IToastNotifier **out )
{
    struct toast_notifier *impl;

    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IToastNotifier_iface.lpVtbl = &toast_notifier_vtbl;
    impl->ref = 1;
    *out = &impl->IToastNotifier_iface;
    return S_OK;
}

/* ========================================================================= */
/* ToastNotificationManagerStatics Implementation                            */
/* ========================================================================= */

struct toast_notification_manager_statics
{
    IActivationFactory IActivationFactory_iface;
    IToastNotificationManagerStatics IToastNotificationManagerStatics_iface;
    LONG ref;
};

static inline struct toast_notification_manager_statics *impl_from_toast_IActivationFactory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct toast_notification_manager_statics, IActivationFactory_iface );
}

static inline struct toast_notification_manager_statics *impl_from_IToastNotificationManagerStatics( IToastNotificationManagerStatics *iface )
{
    return CONTAINING_RECORD( iface, struct toast_notification_manager_statics, IToastNotificationManagerStatics_iface );
}

static HRESULT WINAPI toast_factory_QueryInterface( IActivationFactory *iface, REFIID iid, void **out )
{
    struct toast_notification_manager_statics *impl = impl_from_toast_IActivationFactory( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IActivationFactory ))
    {
        *out = &impl->IActivationFactory_iface;
        IInspectable_AddRef( (IInspectable *)*out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IToastNotificationManagerStatics ))
    {
        *out = &impl->IToastNotificationManagerStatics_iface;
        IInspectable_AddRef( (IInspectable *)*out );
        return S_OK;
    }

    FIXME( "interface %s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI toast_factory_AddRef( IActivationFactory *iface )
{
    return 2;
}

static ULONG WINAPI toast_factory_Release( IActivationFactory *iface )
{
    return 1;
}

static HRESULT WINAPI toast_factory_GetIids( IActivationFactory *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI toast_factory_GetRuntimeClassName( IActivationFactory *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.UI.Notifications.ToastNotificationManager", 50, class_name );
}

static HRESULT WINAPI toast_factory_GetTrustLevel( IActivationFactory *iface, TrustLevel *trust_level )
{
    TRACE( "iface %p, trust_level %p.\n", iface, trust_level );
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI toast_factory_ActivateInstance( IActivationFactory *iface, IInspectable **instance )
{
    FIXME( "iface %p, instance %p stub!\n", iface, instance );
    return E_NOTIMPL;
}

static const struct IActivationFactoryVtbl toast_factory_vtbl =
{
    toast_factory_QueryInterface,
    toast_factory_AddRef,
    toast_factory_Release,
    toast_factory_GetIids,
    toast_factory_GetRuntimeClassName,
    toast_factory_GetTrustLevel,
    toast_factory_ActivateInstance,
};

static HRESULT WINAPI toast_statics_QueryInterface( IToastNotificationManagerStatics *iface, REFIID iid, void **out )
{
    struct toast_notification_manager_statics *impl = impl_from_IToastNotificationManagerStatics( iface );
    return toast_factory_QueryInterface( &impl->IActivationFactory_iface, iid, out );
}

static ULONG WINAPI toast_statics_AddRef( IToastNotificationManagerStatics *iface )
{
    return 2;
}

static ULONG WINAPI toast_statics_Release( IToastNotificationManagerStatics *iface )
{
    return 1;
}

static HRESULT WINAPI toast_statics_GetIids( IToastNotificationManagerStatics *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI toast_statics_GetRuntimeClassName( IToastNotificationManagerStatics *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.UI.Notifications.ToastNotificationManager", 50, class_name );
}

static HRESULT WINAPI toast_statics_GetTrustLevel( IToastNotificationManagerStatics *iface, TrustLevel *trust_level )
{
    TRACE( "iface %p, trust_level %p.\n", iface, trust_level );
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI toast_statics_CreateToastNotifier( IToastNotificationManagerStatics *iface, IToastNotifier **result )
{
    TRACE( "iface %p, result %p.\n", iface, result );
    return toast_notifier_create( result );
}

static HRESULT WINAPI toast_statics_CreateToastNotifierWithId( IToastNotificationManagerStatics *iface, HSTRING application_id, IToastNotifier **result )
{
    TRACE( "iface %p, application_id %s, result %p.\n", iface, debugstr_hstring(application_id), result );
    return toast_notifier_create( result );
}

static HRESULT WINAPI toast_statics_GetTemplateContent( IToastNotificationManagerStatics *iface, ToastTemplateType type, __x_ABI_CWindows_CData_CXml_CDom_CIXmlDocument **result )
{
    FIXME( "iface %p, type %d, result %p stub!\n", iface, type, result );
    if (result) *result = NULL;
    return E_NOTIMPL;
}

static const struct IToastNotificationManagerStaticsVtbl toast_statics_vtbl =
{
    toast_statics_QueryInterface,
    toast_statics_AddRef,
    toast_statics_Release,
    toast_statics_GetIids,
    toast_statics_GetRuntimeClassName,
    toast_statics_GetTrustLevel,
    toast_statics_CreateToastNotifier,
    toast_statics_CreateToastNotifierWithId,
    toast_statics_GetTemplateContent,
};

static struct toast_notification_manager_statics default_toast_notification_manager_statics =
{
    {&toast_factory_vtbl},
    {&toast_statics_vtbl},
    1,
};

IActivationFactory *toastnotificationmanager_factory = &default_toast_notification_manager_statics.IActivationFactory_iface;

/* ========================================================================= */
/* TileUpdater Implementation                                                */
/* ========================================================================= */

struct tile_updater
{
    ITileUpdater ITileUpdater_iface;
    LONG ref;
};

static inline struct tile_updater *impl_from_ITileUpdater( ITileUpdater *iface )
{
    return CONTAINING_RECORD( iface, struct tile_updater, ITileUpdater_iface );
}

static HRESULT WINAPI tile_updater_QueryInterface( ITileUpdater *iface, REFIID iid, void **out )
{
    struct tile_updater *impl = impl_from_ITileUpdater( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_ITileUpdater ))
    {
        *out = &impl->ITileUpdater_iface;
        IInspectable_AddRef( (IInspectable *)*out );
        return S_OK;
    }

    FIXME( "interface %s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI tile_updater_AddRef( ITileUpdater *iface )
{
    struct tile_updater *impl = impl_from_ITileUpdater( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing ref to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI tile_updater_Release( ITileUpdater *iface )
{
    struct tile_updater *impl = impl_from_ITileUpdater( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p decreasing ref to %lu.\n", iface, ref );
    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI tile_updater_GetIids( ITileUpdater *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI tile_updater_GetRuntimeClassName( ITileUpdater *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.UI.Notifications.TileUpdater", 36, class_name );
}

static HRESULT WINAPI tile_updater_GetTrustLevel( ITileUpdater *iface, TrustLevel *trust_level )
{
    TRACE( "iface %p, trust_level %p.\n", iface, trust_level );
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI tile_updater_Update( ITileUpdater *iface, ITileNotification *notification )
{
    TRACE( "iface %p, notification %p.\n", iface, notification );
    return S_OK;
}

static HRESULT WINAPI tile_updater_Clear( ITileUpdater *iface )
{
    TRACE( "iface %p.\n", iface );
    return S_OK;
}

static HRESULT WINAPI tile_updater_EnableNotificationQueue( ITileUpdater *iface, boolean enable )
{
    TRACE( "iface %p, enable %d.\n", iface, enable );
    return S_OK;
}

static HRESULT WINAPI tile_updater_get_Setting( ITileUpdater *iface, NotificationSetting *value )
{
    TRACE( "iface %p, value %p.\n", iface, value );
    if (!value) return E_POINTER;
    *value = NotificationSetting_Enabled;
    return S_OK;
}

static HRESULT WINAPI tile_updater_AddToSchedule( ITileUpdater *iface, IScheduledTileNotification *scheduled_tile )
{
    FIXME( "iface %p, scheduled_tile %p stub!\n", iface, scheduled_tile );
    return S_OK;
}

static HRESULT WINAPI tile_updater_RemoveFromSchedule( ITileUpdater *iface, IScheduledTileNotification *scheduled_tile )
{
    FIXME( "iface %p, scheduled_tile %p stub!\n", iface, scheduled_tile );
    return S_OK;
}

struct empty_scheduled_tile_iterator
{
    __FIIterator_1_Windows__CUI__CNotifications__CScheduledTileNotification __FIIterator_1_Windows__CUI__CNotifications__CScheduledTileNotification_iface;
    LONG ref;
};

static inline struct empty_scheduled_tile_iterator *impl_from_empty_scheduled_tile_iterator( __FIIterator_1_Windows__CUI__CNotifications__CScheduledTileNotification *iface )
{
    return CONTAINING_RECORD( iface, struct empty_scheduled_tile_iterator, __FIIterator_1_Windows__CUI__CNotifications__CScheduledTileNotification_iface );
}

static HRESULT WINAPI empty_scheduled_tile_iterator_QueryInterface( __FIIterator_1_Windows__CUI__CNotifications__CScheduledTileNotification *iface,
                                                                     REFIID iid, void **out )
{
    struct empty_scheduled_tile_iterator *impl = impl_from_empty_scheduled_tile_iterator( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID___FIIterator_1_Windows__CUI__CNotifications__CScheduledTileNotification ))
    {
        *out = &impl->__FIIterator_1_Windows__CUI__CNotifications__CScheduledTileNotification_iface;
        IInspectable_AddRef( (IInspectable *)*out );
        return S_OK;
    }

    WARN( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI empty_scheduled_tile_iterator_AddRef( __FIIterator_1_Windows__CUI__CNotifications__CScheduledTileNotification *iface )
{
    struct empty_scheduled_tile_iterator *impl = impl_from_empty_scheduled_tile_iterator( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI empty_scheduled_tile_iterator_Release( __FIIterator_1_Windows__CUI__CNotifications__CScheduledTileNotification *iface )
{
    struct empty_scheduled_tile_iterator *impl = impl_from_empty_scheduled_tile_iterator( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );
    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI empty_scheduled_tile_iterator_GetIids( __FIIterator_1_Windows__CUI__CNotifications__CScheduledTileNotification *iface,
                                                             ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI empty_scheduled_tile_iterator_GetRuntimeClassName( __FIIterator_1_Windows__CUI__CNotifications__CScheduledTileNotification *iface,
                                                                         HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.Collections.IIterator`1<Windows.UI.Notifications.ScheduledTileNotification>",
                                ARRAY_SIZE(L"Windows.Foundation.Collections.IIterator`1<Windows.UI.Notifications.ScheduledTileNotification>"),
                                class_name );
}

static HRESULT WINAPI empty_scheduled_tile_iterator_GetTrustLevel( __FIIterator_1_Windows__CUI__CNotifications__CScheduledTileNotification *iface,
                                                                   TrustLevel *trust_level )
{
    if (!trust_level) return E_POINTER;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI empty_scheduled_tile_iterator_get_Current( __FIIterator_1_Windows__CUI__CNotifications__CScheduledTileNotification *iface,
                                                                 __x_ABI_CWindows_CUI_CNotifications_CIScheduledTileNotification **value )
{
    TRACE( "iface %p, value %p.\n", iface, value );
    if (value) *value = NULL;
    return 0x8000000b; /* E_BOUNDS */
}

static HRESULT WINAPI empty_scheduled_tile_iterator_get_HasCurrent( __FIIterator_1_Windows__CUI__CNotifications__CScheduledTileNotification *iface,
                                                                    boolean *value )
{
    TRACE( "iface %p, value %p.\n", iface, value );
    if (!value) return E_POINTER;
    *value = FALSE;
    return S_OK;
}

static HRESULT WINAPI empty_scheduled_tile_iterator_MoveNext( __FIIterator_1_Windows__CUI__CNotifications__CScheduledTileNotification *iface,
                                                              boolean *value )
{
    TRACE( "iface %p, value %p.\n", iface, value );
    if (!value) return E_POINTER;
    *value = FALSE;
    return S_OK;
}

static HRESULT WINAPI empty_scheduled_tile_iterator_GetMany( __FIIterator_1_Windows__CUI__CNotifications__CScheduledTileNotification *iface,
                                                             UINT32 items_size,
                                                             __x_ABI_CWindows_CUI_CNotifications_CIScheduledTileNotification **items,
                                                             UINT32 *value )
{
    TRACE( "iface %p, items_size %u, items %p, value %p.\n", iface, items_size, items, value );
    if (value) *value = 0;
    return S_OK;
}

static const struct __FIIterator_1_Windows__CUI__CNotifications__CScheduledTileNotificationVtbl empty_scheduled_tile_iterator_vtbl =
{
    empty_scheduled_tile_iterator_QueryInterface,
    empty_scheduled_tile_iterator_AddRef,
    empty_scheduled_tile_iterator_Release,
    empty_scheduled_tile_iterator_GetIids,
    empty_scheduled_tile_iterator_GetRuntimeClassName,
    empty_scheduled_tile_iterator_GetTrustLevel,
    empty_scheduled_tile_iterator_get_Current,
    empty_scheduled_tile_iterator_get_HasCurrent,
    empty_scheduled_tile_iterator_MoveNext,
    empty_scheduled_tile_iterator_GetMany,
};

static HRESULT empty_scheduled_tile_iterator_create( __FIIterator_1_Windows__CUI__CNotifications__CScheduledTileNotification **out )
{
    struct empty_scheduled_tile_iterator *impl;

    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->__FIIterator_1_Windows__CUI__CNotifications__CScheduledTileNotification_iface.lpVtbl = &empty_scheduled_tile_iterator_vtbl;
    impl->ref = 1;
    *out = &impl->__FIIterator_1_Windows__CUI__CNotifications__CScheduledTileNotification_iface;
    return S_OK;
}

struct empty_scheduled_tile_vector_view
{
    __FIVectorView_1_Windows__CUI__CNotifications__CScheduledTileNotification __FIVectorView_1_Windows__CUI__CNotifications__CScheduledTileNotification_iface;
    __FIIterable_1_Windows__CUI__CNotifications__CScheduledTileNotification __FIIterable_1_Windows__CUI__CNotifications__CScheduledTileNotification_iface;
    LONG ref;
};

static inline struct empty_scheduled_tile_vector_view *impl_from_empty_scheduled_tile_vector_view( __FIVectorView_1_Windows__CUI__CNotifications__CScheduledTileNotification *iface )
{
    return CONTAINING_RECORD( iface, struct empty_scheduled_tile_vector_view, __FIVectorView_1_Windows__CUI__CNotifications__CScheduledTileNotification_iface );
}

static inline struct empty_scheduled_tile_vector_view *impl_from_empty_scheduled_tile_iterable( __FIIterable_1_Windows__CUI__CNotifications__CScheduledTileNotification *iface )
{
    return CONTAINING_RECORD( iface, struct empty_scheduled_tile_vector_view, __FIIterable_1_Windows__CUI__CNotifications__CScheduledTileNotification_iface );
}

static HRESULT WINAPI empty_scheduled_tile_vector_view_QueryInterface( __FIVectorView_1_Windows__CUI__CNotifications__CScheduledTileNotification *iface,
                                                                        REFIID iid, void **out )
{
    struct empty_scheduled_tile_vector_view *impl = impl_from_empty_scheduled_tile_vector_view( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID___FIVectorView_1_Windows__CUI__CNotifications__CScheduledTileNotification ))
    {
        *out = &impl->__FIVectorView_1_Windows__CUI__CNotifications__CScheduledTileNotification_iface;
        IInspectable_AddRef( (IInspectable *)*out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID___FIIterable_1_Windows__CUI__CNotifications__CScheduledTileNotification ))
    {
        *out = &impl->__FIIterable_1_Windows__CUI__CNotifications__CScheduledTileNotification_iface;
        IInspectable_AddRef( (IInspectable *)*out );
        return S_OK;
    }

    WARN( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI empty_scheduled_tile_vector_view_AddRef( __FIVectorView_1_Windows__CUI__CNotifications__CScheduledTileNotification *iface )
{
    struct empty_scheduled_tile_vector_view *impl = impl_from_empty_scheduled_tile_vector_view( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI empty_scheduled_tile_vector_view_Release( __FIVectorView_1_Windows__CUI__CNotifications__CScheduledTileNotification *iface )
{
    struct empty_scheduled_tile_vector_view *impl = impl_from_empty_scheduled_tile_vector_view( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );
    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI empty_scheduled_tile_iterable_QueryInterface( __FIIterable_1_Windows__CUI__CNotifications__CScheduledTileNotification *iface,
                                                                     REFIID iid, void **out )
{
    struct empty_scheduled_tile_vector_view *impl = impl_from_empty_scheduled_tile_iterable( iface );
    return empty_scheduled_tile_vector_view_QueryInterface( &impl->__FIVectorView_1_Windows__CUI__CNotifications__CScheduledTileNotification_iface, iid, out );
}

static ULONG WINAPI empty_scheduled_tile_iterable_AddRef( __FIIterable_1_Windows__CUI__CNotifications__CScheduledTileNotification *iface )
{
    struct empty_scheduled_tile_vector_view *impl = impl_from_empty_scheduled_tile_iterable( iface );
    return empty_scheduled_tile_vector_view_AddRef( &impl->__FIVectorView_1_Windows__CUI__CNotifications__CScheduledTileNotification_iface );
}

static ULONG WINAPI empty_scheduled_tile_iterable_Release( __FIIterable_1_Windows__CUI__CNotifications__CScheduledTileNotification *iface )
{
    struct empty_scheduled_tile_vector_view *impl = impl_from_empty_scheduled_tile_iterable( iface );
    return empty_scheduled_tile_vector_view_Release( &impl->__FIVectorView_1_Windows__CUI__CNotifications__CScheduledTileNotification_iface );
}

static HRESULT WINAPI empty_scheduled_tile_iterable_GetIids( __FIIterable_1_Windows__CUI__CNotifications__CScheduledTileNotification *iface,
                                                             ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI empty_scheduled_tile_iterable_GetRuntimeClassName( __FIIterable_1_Windows__CUI__CNotifications__CScheduledTileNotification *iface,
                                                                         HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.Collections.IIterable`1<Windows.UI.Notifications.ScheduledTileNotification>",
                                ARRAY_SIZE(L"Windows.Foundation.Collections.IIterable`1<Windows.UI.Notifications.ScheduledTileNotification>"),
                                class_name );
}

static HRESULT WINAPI empty_scheduled_tile_iterable_GetTrustLevel( __FIIterable_1_Windows__CUI__CNotifications__CScheduledTileNotification *iface,
                                                                   TrustLevel *trust_level )
{
    if (!trust_level) return E_POINTER;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI empty_scheduled_tile_iterable_First( __FIIterable_1_Windows__CUI__CNotifications__CScheduledTileNotification *iface,
                                                           __FIIterator_1_Windows__CUI__CNotifications__CScheduledTileNotification **value )
{
    TRACE( "iface %p, value %p.\n", iface, value );
    if (!value) return E_POINTER;
    return empty_scheduled_tile_iterator_create( value );
}

static const struct __FIIterable_1_Windows__CUI__CNotifications__CScheduledTileNotificationVtbl empty_scheduled_tile_iterable_vtbl =
{
    empty_scheduled_tile_iterable_QueryInterface,
    empty_scheduled_tile_iterable_AddRef,
    empty_scheduled_tile_iterable_Release,
    empty_scheduled_tile_iterable_GetIids,
    empty_scheduled_tile_iterable_GetRuntimeClassName,
    empty_scheduled_tile_iterable_GetTrustLevel,
    empty_scheduled_tile_iterable_First,
};

static HRESULT WINAPI empty_scheduled_tile_vector_view_GetIids( __FIVectorView_1_Windows__CUI__CNotifications__CScheduledTileNotification *iface,
                                                                ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI empty_scheduled_tile_vector_view_GetRuntimeClassName( __FIVectorView_1_Windows__CUI__CNotifications__CScheduledTileNotification *iface,
                                                                            HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.Collections.IVectorView`1<Windows.UI.Notifications.ScheduledTileNotification>",
                                ARRAY_SIZE(L"Windows.Foundation.Collections.IVectorView`1<Windows.UI.Notifications.ScheduledTileNotification>"),
                                class_name );
}

static HRESULT WINAPI empty_scheduled_tile_vector_view_GetTrustLevel( __FIVectorView_1_Windows__CUI__CNotifications__CScheduledTileNotification *iface,
                                                                      TrustLevel *trust_level )
{
    if (!trust_level) return E_POINTER;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI empty_scheduled_tile_vector_view_GetAt( __FIVectorView_1_Windows__CUI__CNotifications__CScheduledTileNotification *iface,
                                                              UINT32 index, __x_ABI_CWindows_CUI_CNotifications_CIScheduledTileNotification **value )
{
    TRACE( "iface %p, index %u, value %p.\n", iface, index, value );
    if (value) *value = NULL;
    return 0x8000000b; /* E_BOUNDS */
}

static HRESULT WINAPI empty_scheduled_tile_vector_view_get_Size( __FIVectorView_1_Windows__CUI__CNotifications__CScheduledTileNotification *iface,
                                                                 UINT32 *value )
{
    TRACE( "iface %p, value %p.\n", iface, value );
    if (!value) return E_POINTER;
    *value = 0;
    return S_OK;
}

static HRESULT WINAPI empty_scheduled_tile_vector_view_IndexOf( __FIVectorView_1_Windows__CUI__CNotifications__CScheduledTileNotification *iface,
                                                                __x_ABI_CWindows_CUI_CNotifications_CIScheduledTileNotification *element,
                                                                UINT32 *index, BOOLEAN *value )
{
    TRACE( "iface %p, element %p, index %p, value %p.\n", iface, element, index, value );
    if (index) *index = 0;
    if (value) *value = FALSE;
    return S_OK;
}

static HRESULT WINAPI empty_scheduled_tile_vector_view_GetMany( __FIVectorView_1_Windows__CUI__CNotifications__CScheduledTileNotification *iface,
                                                                UINT32 start_index, UINT32 items_size,
                                                                __x_ABI_CWindows_CUI_CNotifications_CIScheduledTileNotification **items,
                                                                UINT32 *value )
{
    TRACE( "iface %p, start_index %u, items_size %u, items %p, value %p.\n", iface, start_index, items_size, items, value );
    if (value) *value = 0;
    return S_OK;
}

static const struct __FIVectorView_1_Windows__CUI__CNotifications__CScheduledTileNotificationVtbl empty_scheduled_tile_vector_view_vtbl =
{
    empty_scheduled_tile_vector_view_QueryInterface,
    empty_scheduled_tile_vector_view_AddRef,
    empty_scheduled_tile_vector_view_Release,
    empty_scheduled_tile_vector_view_GetIids,
    empty_scheduled_tile_vector_view_GetRuntimeClassName,
    empty_scheduled_tile_vector_view_GetTrustLevel,
    empty_scheduled_tile_vector_view_GetAt,
    empty_scheduled_tile_vector_view_get_Size,
    empty_scheduled_tile_vector_view_IndexOf,
    empty_scheduled_tile_vector_view_GetMany,
};

static HRESULT empty_scheduled_tile_vector_view_create( __FIVectorView_1_Windows__CUI__CNotifications__CScheduledTileNotification **out )
{
    struct empty_scheduled_tile_vector_view *impl;

    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->__FIVectorView_1_Windows__CUI__CNotifications__CScheduledTileNotification_iface.lpVtbl = &empty_scheduled_tile_vector_view_vtbl;
    impl->__FIIterable_1_Windows__CUI__CNotifications__CScheduledTileNotification_iface.lpVtbl = &empty_scheduled_tile_iterable_vtbl;
    impl->ref = 1;
    *out = &impl->__FIVectorView_1_Windows__CUI__CNotifications__CScheduledTileNotification_iface;
    return S_OK;
}

static HRESULT WINAPI tile_updater_GetScheduledTileNotifications( ITileUpdater *iface, __FIVectorView_1_Windows__CUI__CNotifications__CScheduledTileNotification **result )
{
    TRACE( "iface %p, result %p.\n", iface, result );
    if (!result) return E_POINTER;
    return empty_scheduled_tile_vector_view_create( result );
}

static HRESULT WINAPI tile_updater_StartPeriodicUpdate( ITileUpdater *iface, IUriRuntimeClass *tile_content, PeriodicUpdateRecurrence requested_interval )
{
    FIXME( "iface %p, tile_content %p, interval %d stub!\n", iface, tile_content, requested_interval );
    return S_OK;
}

static HRESULT WINAPI tile_updater_StartPeriodicUpdateAtTime( ITileUpdater *iface, IUriRuntimeClass *tile_content, DateTime start_time, PeriodicUpdateRecurrence requested_interval )
{
    FIXME( "iface %p, tile_content %p, interval %d stub!\n", iface, tile_content, requested_interval );
    return S_OK;
}

static HRESULT WINAPI tile_updater_StopPeriodicUpdate( ITileUpdater *iface )
{
    TRACE( "iface %p.\n", iface );
    return S_OK;
}

static const struct ITileUpdaterVtbl tile_updater_vtbl =
{
    tile_updater_QueryInterface,
    tile_updater_AddRef,
    tile_updater_Release,
    tile_updater_GetIids,
    tile_updater_GetRuntimeClassName,
    tile_updater_GetTrustLevel,
    tile_updater_Update,
    tile_updater_Clear,
    tile_updater_EnableNotificationQueue,
    tile_updater_get_Setting,
    tile_updater_AddToSchedule,
    tile_updater_RemoveFromSchedule,
    tile_updater_GetScheduledTileNotifications,
    tile_updater_StartPeriodicUpdate,
    tile_updater_StartPeriodicUpdateAtTime,
    tile_updater_StopPeriodicUpdate,
};

static HRESULT tile_updater_create( ITileUpdater **out )
{
    struct tile_updater *impl;

    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->ITileUpdater_iface.lpVtbl = &tile_updater_vtbl;
    impl->ref = 1;
    *out = &impl->ITileUpdater_iface;
    return S_OK;
}

/* ========================================================================= */
/* TileUpdateManagerStatics Implementation                                   */
/* ========================================================================= */

struct tile_update_manager_statics
{
    IActivationFactory IActivationFactory_iface;
    ITileUpdateManagerStatics ITileUpdateManagerStatics_iface;
    LONG ref;
};

static inline struct tile_update_manager_statics *impl_from_tile_IActivationFactory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct tile_update_manager_statics, IActivationFactory_iface );
}

static inline struct tile_update_manager_statics *impl_from_ITileUpdateManagerStatics( ITileUpdateManagerStatics *iface )
{
    return CONTAINING_RECORD( iface, struct tile_update_manager_statics, ITileUpdateManagerStatics_iface );
}

static HRESULT WINAPI tile_factory_QueryInterface( IActivationFactory *iface, REFIID iid, void **out )
{
    struct tile_update_manager_statics *impl = impl_from_tile_IActivationFactory( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IActivationFactory ))
    {
        *out = &impl->IActivationFactory_iface;
        IInspectable_AddRef( (IInspectable *)*out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_ITileUpdateManagerStatics ))
    {
        *out = &impl->ITileUpdateManagerStatics_iface;
        IInspectable_AddRef( (IInspectable *)*out );
        return S_OK;
    }

    FIXME( "interface %s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI tile_factory_AddRef( IActivationFactory *iface )
{
    return 2;
}

static ULONG WINAPI tile_factory_Release( IActivationFactory *iface )
{
    return 1;
}

static HRESULT WINAPI tile_factory_GetIids( IActivationFactory *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI tile_factory_GetRuntimeClassName( IActivationFactory *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.UI.Notifications.TileUpdateManager", 41, class_name );
}

static HRESULT WINAPI tile_factory_GetTrustLevel( IActivationFactory *iface, TrustLevel *trust_level )
{
    TRACE( "iface %p, trust_level %p.\n", iface, trust_level );
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI tile_factory_ActivateInstance( IActivationFactory *iface, IInspectable **instance )
{
    FIXME( "iface %p, instance %p stub!\n", iface, instance );
    return E_NOTIMPL;
}

static const struct IActivationFactoryVtbl tile_factory_vtbl =
{
    tile_factory_QueryInterface,
    tile_factory_AddRef,
    tile_factory_Release,
    tile_factory_GetIids,
    tile_factory_GetRuntimeClassName,
    tile_factory_GetTrustLevel,
    tile_factory_ActivateInstance,
};

static HRESULT WINAPI tile_statics_QueryInterface( ITileUpdateManagerStatics *iface, REFIID iid, void **out )
{
    struct tile_update_manager_statics *impl = impl_from_ITileUpdateManagerStatics( iface );
    return tile_factory_QueryInterface( &impl->IActivationFactory_iface, iid, out );
}

static ULONG WINAPI tile_statics_AddRef( ITileUpdateManagerStatics *iface )
{
    return 2;
}

static ULONG WINAPI tile_statics_Release( ITileUpdateManagerStatics *iface )
{
    return 1;
}

static HRESULT WINAPI tile_statics_GetIids( ITileUpdateManagerStatics *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI tile_statics_GetRuntimeClassName( ITileUpdateManagerStatics *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.UI.Notifications.TileUpdateManager", 41, class_name );
}

static HRESULT WINAPI tile_statics_GetTrustLevel( ITileUpdateManagerStatics *iface, TrustLevel *trust_level )
{
    TRACE( "iface %p, trust_level %p.\n", iface, trust_level );
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI tile_statics_CreateTileUpdaterForApplication( ITileUpdateManagerStatics *iface, ITileUpdater **result )
{
    TRACE( "iface %p, result %p.\n", iface, result );
    return tile_updater_create( result );
}

static HRESULT WINAPI tile_statics_CreateTileUpdaterForApplicationWithId( ITileUpdateManagerStatics *iface, HSTRING application_id, ITileUpdater **result )
{
    TRACE( "iface %p, application_id %s, result %p.\n", iface, debugstr_hstring(application_id), result );
    return tile_updater_create( result );
}

static HRESULT WINAPI tile_statics_CreateTileUpdaterForSecondaryTile( ITileUpdateManagerStatics *iface, HSTRING tile_id, ITileUpdater **result )
{
    TRACE( "iface %p, tile_id %s, result %p.\n", iface, debugstr_hstring(tile_id), result );
    return tile_updater_create( result );
}

static HRESULT WINAPI tile_statics_GetTemplateContent( ITileUpdateManagerStatics *iface, TileTemplateType type, __x_ABI_CWindows_CData_CXml_CDom_CIXmlDocument **result )
{
    FIXME( "iface %p, type %d, result %p stub!\n", iface, type, result );
    if (result) *result = NULL;
    return E_NOTIMPL;
}

static const struct ITileUpdateManagerStaticsVtbl tile_statics_vtbl =
{
    tile_statics_QueryInterface,
    tile_statics_AddRef,
    tile_statics_Release,
    tile_statics_GetIids,
    tile_statics_GetRuntimeClassName,
    tile_statics_GetTrustLevel,
    tile_statics_CreateTileUpdaterForApplication,
    tile_statics_CreateTileUpdaterForApplicationWithId,
    tile_statics_CreateTileUpdaterForSecondaryTile,
    tile_statics_GetTemplateContent,
};

static struct tile_update_manager_statics default_tile_update_manager_statics =
{
    {&tile_factory_vtbl},
    {&tile_statics_vtbl},
    1,
};

IActivationFactory *tileupdatemanager_factory = &default_tile_update_manager_statics.IActivationFactory_iface;
