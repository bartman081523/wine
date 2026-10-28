/* WinRT Windows.ApplicationModel.Core.CoreApplication factory
 *
 * Copyright 2026 J. Wirth for the Xodus project
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

#define WIDL_using_Windows_ApplicationModel_Core
#include "windows.applicationmodel.core.h"

/* ntdll RtlPcToFileHeader (rtlsupportapi.h does not compile standalone here) */
extern void * WINAPI RtlPcToFileHeader(void *pc, void **base);

WINE_DEFAULT_DEBUG_CHANNEL(twinapi);

/* singleton defined below, handed out by the factory for IID_ICoreApplication */
static HRESULT core_application_instance_get( ICoreApplication **out );

/* defined in the PropertySet block below */
static HRESULT property_set_create( IPropertySet **out );

/* The generated header only declares this IID; no wine module links a
 * definition, so provide ours. The IID_ICoreApplication alias macro from
 * WIDL_using_Windows_ApplicationModel_Core expands to the same long name. */
const IID IID___x_ABI_CWindows_CApplicationModel_CCore_CICoreApplication =
    {0x0aacf7a4, 0x5e1d, 0x49df, {0x80,0x34,0xfb,0x6a,0x68,0xbc,0x5e,0xd1}};

/* Private statics interface activated by Microsoft.Internal.FrameworkUdk.dll
 * for "Windows.ApplicationModel.Core.CoreApplication" with IID
 * {17b0e613-942a-422d-904c-f90dc71a7dae}. The IID is not part of any public
 * contract and absent from windows.applicationmodel.core.idl; its vtable
 * layout was recovered from the two FrameworkUdk call sites: slots 0-5 are
 * IInspectable, slot 0x58/8 forwards Application.Start with two parameters,
 * slot 0x60/8 is a zero-parameter call whose E_ILLEGAL_METHOD_CALL answer is
 * explicitly swallowed by FrameworkUdk. */
static const IID IID_ICoreApplicationStatics =
    {0x17b0e613, 0x942a, 0x422d, {0x90,0x4c,0xf9,0x0d,0xc7,0x1a,0x7d,0xae}};

typedef struct ICoreApplicationStaticsCore ICoreApplicationStaticsCore;

typedef struct ICoreApplicationStaticsCoreVtbl
{
    HRESULT (WINAPI *QueryInterface)(ICoreApplicationStaticsCore *iface, REFIID iid, void **out);
    ULONG (WINAPI *AddRef)(ICoreApplicationStaticsCore *iface);
    ULONG (WINAPI *Release)(ICoreApplicationStaticsCore *iface);
    HRESULT (WINAPI *GetIids)(ICoreApplicationStaticsCore *iface, ULONG *iid_count, IID **iids);
    HRESULT (WINAPI *GetRuntimeClassName)(ICoreApplicationStaticsCore *iface, HSTRING *class_name);
    HRESULT (WINAPI *GetTrustLevel)(ICoreApplicationStaticsCore *iface, TrustLevel *trust_level);
    HRESULT (WINAPI *slot6)(ICoreApplicationStaticsCore *iface);
    HRESULT (WINAPI *slot7)(ICoreApplicationStaticsCore *iface);
    HRESULT (WINAPI *slot8)(ICoreApplicationStaticsCore *iface);
    HRESULT (WINAPI *slot9)(ICoreApplicationStaticsCore *iface);
    HRESULT (WINAPI *slot10)(ICoreApplicationStaticsCore *iface);
    HRESULT (WINAPI *Run)(ICoreApplicationStaticsCore *iface, void *param1, void *param2);
    HRESULT (WINAPI *GetState)(ICoreApplicationStaticsCore *iface);
} ICoreApplicationStaticsCoreVtbl;

struct ICoreApplicationStaticsCore
{
    const ICoreApplicationStaticsCoreVtbl *lpVtbl;
};

struct core_application_factory
{
    IActivationFactory IActivationFactory_iface;
    ICoreApplicationStaticsCore ICoreApplicationStatics_iface;
    LONG ref;
};

static inline struct core_application_factory *impl_from_IActivationFactory(IActivationFactory *iface)
{
    return CONTAINING_RECORD(iface, struct core_application_factory, IActivationFactory_iface);
}

static inline struct core_application_factory *impl_from_statics(ICoreApplicationStaticsCore *iface)
{
    return CONTAINING_RECORD(iface, struct core_application_factory, ICoreApplicationStatics_iface);
}

static ULONG WINAPI statics_AddRef(ICoreApplicationStaticsCore *iface);
static ULONG WINAPI statics_Release(ICoreApplicationStaticsCore *iface);

static HRESULT WINAPI activation_factory_QueryInterface(IActivationFactory *iface, REFIID iid,
                                                        void **out)
{
    struct core_application_factory *impl = impl_from_IActivationFactory(iface);

    TRACE("iface %p, iid %s, out %p.\n", iface, debugstr_guid(iid), out);

    if (IsEqualGUID(iid, &IID_IUnknown)
        || IsEqualGUID(iid, &IID_IInspectable)
        || IsEqualGUID(iid, &IID_IAgileObject)
        || IsEqualGUID(iid, &IID_IActivationFactory))
    {
        IActivationFactory_AddRef(&impl->IActivationFactory_iface);
        *out = &impl->IActivationFactory_iface;
        return S_OK;
    }
    else if (IsEqualGUID(iid, &IID_ICoreApplicationStatics))
    {
        statics_AddRef(&impl->ICoreApplicationStatics_iface);
        *out = &impl->ICoreApplicationStatics_iface;
        return S_OK;
    }
    else if (IsEqualGUID(iid, &IID_ICoreApplication))
    {
        /* Windows returns an instance-like singleton for the public interface;
         * run68 died exactly here (E_NOINTERFACE -> CsWinRT fail-fast). */
        return core_application_instance_get( (ICoreApplication **)out );
    }

    FIXME("%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid(iid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI activation_factory_AddRef(IActivationFactory *iface)
{
    struct core_application_factory *impl = impl_from_IActivationFactory(iface);
    ULONG ref = InterlockedIncrement(&impl->ref);
    TRACE("iface %p, ref %lu.\n", iface, ref);
    return ref;
}

static ULONG WINAPI activation_factory_Release(IActivationFactory *iface)
{
    struct core_application_factory *impl = impl_from_IActivationFactory(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    TRACE("iface %p, ref %lu.\n", iface, ref);
    return ref;
}

static HRESULT WINAPI activation_factory_GetIids(IActivationFactory *iface, ULONG *iid_count,
                                                 IID **iids)
{
    FIXME("iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids);
    return E_NOTIMPL;
}

static HRESULT WINAPI activation_factory_GetRuntimeClassName(IActivationFactory *iface,
                                                             HSTRING *class_name)
{
    FIXME("iface %p, class_name %p stub!\n", iface, class_name);
    return E_NOTIMPL;
}

static HRESULT WINAPI activation_factory_GetTrustLevel(IActivationFactory *iface,
                                                       TrustLevel *trust_level)
{
    FIXME("iface %p, trust_level %p stub!\n", iface, trust_level);
    return E_NOTIMPL;
}

static HRESULT WINAPI activation_factory_ActivateInstance(IActivationFactory *iface,
                                                          IInspectable **instance)
{
    FIXME("iface %p, instance %p stub!\n", iface, instance);
    return E_NOTIMPL;
}

static const struct IActivationFactoryVtbl activation_factory_vtbl =
{
    activation_factory_QueryInterface,
    activation_factory_AddRef,
    activation_factory_Release,
    /* IInspectable methods */
    activation_factory_GetIids,
    activation_factory_GetRuntimeClassName,
    activation_factory_GetTrustLevel,
    activation_factory_ActivateInstance,
};

static BOOL ptr_readable(const void *p, SIZE_T len)
{
    MEMORY_BASIC_INFORMATION mbi;

    if (!p) return FALSE;
    if (!VirtualQuery(p, &mbi, sizeof(mbi))) return FALSE;
    if (mbi.State != MEM_COMMIT) return FALSE;
    if (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD)) return FALSE;
    return (const BYTE *)p + len <= (const BYTE *)mbi.BaseAddress + mbi.RegionSize;
}

static void dump_arg(const char *tag, const void *p)
{
    if (ptr_readable(p, 16))
    {
        const DWORD *v = p;
        TRACE("  %s %p: %08lx %08lx %08lx %08lx\n", tag, p, (unsigned long)v[0],
              (unsigned long)v[1], (unsigned long)v[2], (unsigned long)v[3]);
    }
    else
        TRACE("  %s %p: not readable\n", tag, p);
}

static HRESULT WINAPI statics_QueryInterface(ICoreApplicationStaticsCore *iface, REFIID iid,
                                             void **out)
{
    TRACE("iface %p, iid %s, out %p.\n", iface, debugstr_guid(iid), out);

    if (IsEqualGUID(iid, &IID_IUnknown)
        || IsEqualGUID(iid, &IID_IInspectable)
        || IsEqualGUID(iid, &IID_IAgileObject)
        || IsEqualGUID(iid, &IID_ICoreApplicationStatics))
    {
        statics_AddRef(iface);
        *out = iface;
        return S_OK;
    }

    FIXME("%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid(iid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI statics_AddRef(ICoreApplicationStaticsCore *iface)
{
    struct core_application_factory *impl = impl_from_statics(iface);
    ULONG ref = InterlockedIncrement(&impl->ref);
    TRACE("iface %p, ref %lu.\n", iface, ref);
    return ref;
}

static ULONG WINAPI statics_Release(ICoreApplicationStaticsCore *iface)
{
    struct core_application_factory *impl = impl_from_statics(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    TRACE("iface %p, ref %lu.\n", iface, ref);
    return ref;
}

static HRESULT WINAPI statics_GetIids(ICoreApplicationStaticsCore *iface, ULONG *iid_count,
                                      IID **iids)
{
    FIXME("iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids);
    return E_NOTIMPL;
}

static HRESULT WINAPI statics_GetRuntimeClassName(ICoreApplicationStaticsCore *iface,
                                                  HSTRING *class_name)
{
    FIXME("iface %p, class_name %p stub!\n", iface, class_name);
    return E_NOTIMPL;
}

static HRESULT WINAPI statics_GetTrustLevel(ICoreApplicationStaticsCore *iface,
                                            TrustLevel *trust_level)
{
    FIXME("iface %p, trust_level %p stub!\n", iface, trust_level);
    return E_NOTIMPL;
}

static HRESULT WINAPI statics_slot6(ICoreApplicationStaticsCore *iface)
{
    FIXME("iface %p: vtable slot 6 stub!\n", iface);
    return E_NOTIMPL;
}

static HRESULT WINAPI statics_slot7(ICoreApplicationStaticsCore *iface)
{
    FIXME("iface %p: vtable slot 7 stub!\n", iface);
    return E_NOTIMPL;
}

static HRESULT WINAPI statics_slot8(ICoreApplicationStaticsCore *iface)
{
    FIXME("iface %p: vtable slot 8 stub!\n", iface);
    return E_NOTIMPL;
}

static HRESULT WINAPI statics_slot9(ICoreApplicationStaticsCore *iface)
{
    FIXME("iface %p: vtable slot 9 stub!\n", iface);
    return E_NOTIMPL;
}

static HRESULT WINAPI statics_slot10(ICoreApplicationStaticsCore *iface)
{
    FIXME("iface %p: vtable slot 10 stub!\n", iface);
    return E_NOTIMPL;
}

/* FrameworkUdk FUN_1800253a0: activates CoreApplication with the private
 * statics IID and forwards both of its own parameters unchanged. Without the
 * real CoreApplication there is nothing to run; report success and probe the
 * arguments so the caller chain can be identified from the log. */
static HRESULT WINAPI statics_Run(ICoreApplicationStaticsCore *iface, void *param1, void *param2)
{
    void *ret = __builtin_return_address(0);
    void *base = NULL;

    RtlPcToFileHeader(ret, &base);
    FIXME("Application.Start forward: iface %p, p1 %p, p2 %p, caller ra %p (module base %p).\n",
          iface, param1, param2, ret, base);
    dump_arg("p1", param1);
    dump_arg("p2", param2);

    return S_OK;
}

/* FrameworkUdk FUN_180025580 calls this with no parameters and swallows
 * exactly E_ILLEGAL_METHOD_CALL (0x8000000e), treating it as "no dispatcher".
 * Mimic the real CoreApplication behaviour of answering that when no
 * application run loop is active. */
static HRESULT WINAPI statics_GetState(ICoreApplicationStaticsCore *iface)
{
    TRACE("iface %p: reporting no active CoreApplication.\n", iface);
    return E_ILLEGAL_METHOD_CALL; /* 0x8000000e, swallowed by FrameworkUdk */
}

static const struct ICoreApplicationStaticsCoreVtbl statics_vtbl =
{
    statics_QueryInterface,
    statics_AddRef,
    statics_Release,
    /* IInspectable methods */
    statics_GetIids,
    statics_GetRuntimeClassName,
    statics_GetTrustLevel,
    /* unused slots, offsets 0x30-0x50 */
    statics_slot6,
    statics_slot7,
    statics_slot8,
    statics_slot9,
    statics_slot10,
    /* offset 0x58: Application.Start forward */
    statics_Run,
    /* offset 0x60: zero-arg state call, E_ILLEGAL_METHOD_CALL tolerated */
    statics_GetState,
};

/* Public ICoreApplication singleton ({0aacf7a4-5e1d-49df-8034-fb6a68bc5ed1}).
 * run68: RoGetActivationFactory("Windows.ApplicationModel.Core.CoreApplication",
 * IID_ICoreApplication) returned E_NOINTERFACE because the factory only knew
 * its private statics IID; CsWinRT turned that into InvalidCastException ->
 * RoFailFastWithErrorContextInternal2 (hr 0x80004002). Windows hands out an
 * instance-like singleton here, so the factory answers IID_ICoreApplication. */
struct core_application_instance
{
    ICoreApplication ICoreApplication_iface;
    LONG ref;
};

static inline struct core_application_instance *impl_from_ICoreApplication( ICoreApplication *iface )
{
    return CONTAINING_RECORD( iface, struct core_application_instance, ICoreApplication_iface );
}

static ULONG WINAPI core_application_instance_AddRef( ICoreApplication *iface )
{
    struct core_application_instance *impl = impl_from_ICoreApplication( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI core_application_instance_Release( ICoreApplication *iface )
{
    struct core_application_instance *impl = impl_from_ICoreApplication( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );
    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI core_application_instance_QueryInterface( ICoreApplication *iface, REFIID iid, void **out )
{
    struct core_application_instance *impl = impl_from_ICoreApplication( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_ICoreApplication ))
    {
        *out = &impl->ICoreApplication_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static HRESULT WINAPI core_application_instance_GetIids( ICoreApplication *iface, ULONG *iid_count, IID **iids )
{
    TRACE( "iface %p, iid_count %p, iids %p.\n", iface, iid_count, iids );
    *iid_count = 0;
    *iids = NULL;
    return S_OK;
}

static HRESULT WINAPI core_application_instance_GetRuntimeClassName( ICoreApplication *iface, HSTRING *class_name )
{
    TRACE( "iface %p, class_name %p.\n", iface, class_name );
    return WindowsCreateString( RuntimeClass_Windows_ApplicationModel_Core_CoreApplication,
                                wcslen( RuntimeClass_Windows_ApplicationModel_Core_CoreApplication ), class_name );
}

static HRESULT WINAPI core_application_instance_GetTrustLevel( ICoreApplication *iface, TrustLevel *trust_level )
{
    TRACE( "iface %p, trust_level %p.\n", iface, trust_level );
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI core_application_instance_get_Id( ICoreApplication *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p.\n", iface, value );
    return WindowsCreateString( L"CoreApplication", 14, value );
}

static HRESULT WINAPI core_application_instance_add_Suspending( ICoreApplication *iface,
        __FIEventHandler_1_Windows__CApplicationModel__CSuspendingEventArgs *handler,
        EventRegistrationToken *token )
{
    TRACE( "iface %p, handler %p, token %p: recording no-op registration.\n", iface, handler, token );
    token->value = 1;
    return S_OK;
}

static HRESULT WINAPI core_application_instance_remove_Suspending( ICoreApplication *iface,
        EventRegistrationToken token )
{
    TRACE( "iface %p, token %#I64x: removing no-op registration.\n", iface, token.value );
    return S_OK;
}

static HRESULT WINAPI core_application_instance_add_Resuming( ICoreApplication *iface,
        __FIEventHandler_1_IInspectable *handler, EventRegistrationToken *token )
{
    TRACE( "iface %p, handler %p, token %p: recording no-op registration.\n", iface, handler, token );
    token->value = 1;
    return S_OK;
}

static HRESULT WINAPI core_application_instance_remove_Resuming( ICoreApplication *iface,
        EventRegistrationToken token )
{
    TRACE( "iface %p, token %#I64x: removing no-op registration.\n", iface, token.value );
    return S_OK;
}

static HRESULT WINAPI core_application_instance_get_Properties( ICoreApplication *iface,
        IPropertySet **value )
{
    HRESULT hr = property_set_create( value );
    TRACE( "iface %p -> value %p (hr %#lx).\n", iface,
           SUCCEEDED( hr ) ? (void *)*value : NULL, (unsigned long)hr );
    return hr;
}

static HRESULT WINAPI core_application_instance_GetCurrentView( ICoreApplication *iface,
        ICoreApplicationView **value )
{
    FIXME( "iface %p, value %p stub!\n", iface, value );
    return E_NOTIMPL;
}

static HRESULT WINAPI core_application_instance_Run( ICoreApplication *iface,
        IFrameworkViewSource *view_source )
{
    FIXME( "iface %p, view_source %p stub!\n", iface, view_source );
    return E_NOTIMPL;
}

static HRESULT WINAPI core_application_instance_RunWithActivationFactories( ICoreApplication *iface,
        IGetActivationFactory *factory )
{
    FIXME( "iface %p, factory %p stub!\n", iface, factory );
    return E_NOTIMPL;
}

/* ===== minimal Windows.Foundation.Collections.PropertySet =====
 * run69: get_Properties FIXME E_NOTIMPL -> fail-fast (hr 0x80004001). There is
 * no PropertySet implementation anywhere in wine, so provide one here. vtable
 * layouts follow wine's windows.foundation.collections.idl order, which
 * matches the real ABI slot order confirmed against CsWinRT projections
 * (IMap: Lookup, get_Size, HasKey, GetView, Insert, Remove, Clear;
 * IMapView: Lookup, get_Size, HasKey, Split; GUIDs computed according to the
 * WinRT pinterface hash and cross-checked against winsdk headers). */
#define PROPERTY_SET_MAX_ENTRIES 16

static const IID property_set_IID_IMap =
    {0x1b0d3570,0x0877,0x5ec2,{0x8a,0x2c,0x3b,0x95,0x39,0x50,0x6a,0xca}};
static const IID property_set_IID_IMapView =
    {0xbb78502a,0xf79d,0x54fa,{0x92,0xc9,0x90,0xc5,0x03,0x9f,0xdf,0x7e}};
static const IID property_set_IID_IObservableMap =
    {0x236aac9d,0xfb12,0x5c4d,{0xa4,0x1c,0x9e,0x44,0x5f,0xb4,0xd7,0xec}};
static const IID property_set_IID_IIterable =
    {0xfe2f3d47,0x5d47,0x5499,{0x83,0x74,0x43,0x0c,0x7c,0xda,0x02,0x04}};
static const IID property_set_IID_IIterator =
    {0x5db5fa32,0x707c,0x5849,{0xa0,0x6b,0x91,0xc8,0xeb,0x9d,0x10,0xe8}};
static const IID property_set_IID_IKeyValuePair =
    {0x09335560,0x6c6b,0x5a26,{0x93,0x48,0x97,0xb7,0x81,0x13,0x2b,0x20}};

typedef struct property_map_iface property_map_iface;
typedef struct property_map_view_iface property_map_view_iface;
typedef struct property_observable_map_iface property_observable_map_iface;
typedef struct property_iterable_iface property_iterable_iface;
typedef struct property_iterator_iface property_iterator_iface;
typedef struct property_pair_iface property_pair_iface;

typedef struct property_map_vtbl
{
    HRESULT (WINAPI *QueryInterface)(property_map_iface *This, REFIID iid, void **out);
    ULONG (WINAPI *AddRef)(property_map_iface *This);
    ULONG (WINAPI *Release)(property_map_iface *This);
    HRESULT (WINAPI *GetIids)(property_map_iface *This, ULONG *iid_count, IID **iids);
    HRESULT (WINAPI *GetRuntimeClassName)(property_map_iface *This, HSTRING *class_name);
    HRESULT (WINAPI *GetTrustLevel)(property_map_iface *This, TrustLevel *trust_level);
    HRESULT (WINAPI *Lookup)(property_map_iface *This, HSTRING key, IInspectable **value);
    HRESULT (WINAPI *get_Size)(property_map_iface *This, UINT32 *size);
    HRESULT (WINAPI *HasKey)(property_map_iface *This, HSTRING key, BOOLEAN *found);
    HRESULT (WINAPI *GetView)(property_map_iface *This, property_map_view_iface **view);
    HRESULT (WINAPI *Insert)(property_map_iface *This, HSTRING key, IInspectable *value,
                             BOOLEAN *replaced);
    HRESULT (WINAPI *Remove)(property_map_iface *This, HSTRING key);
    HRESULT (WINAPI *Clear)(property_map_iface *This);
} property_map_vtbl;

typedef struct property_map_view_vtbl
{
    HRESULT (WINAPI *QueryInterface)(property_map_view_iface *This, REFIID iid, void **out);
    ULONG (WINAPI *AddRef)(property_map_view_iface *This);
    ULONG (WINAPI *Release)(property_map_view_iface *This);
    HRESULT (WINAPI *GetIids)(property_map_view_iface *This, ULONG *iid_count, IID **iids);
    HRESULT (WINAPI *GetRuntimeClassName)(property_map_view_iface *This, HSTRING *class_name);
    HRESULT (WINAPI *GetTrustLevel)(property_map_view_iface *This, TrustLevel *trust_level);
    HRESULT (WINAPI *Lookup)(property_map_view_iface *This, HSTRING key, IInspectable **value);
    HRESULT (WINAPI *get_Size)(property_map_view_iface *This, UINT32 *size);
    HRESULT (WINAPI *HasKey)(property_map_view_iface *This, HSTRING key, BOOLEAN *found);
    HRESULT (WINAPI *Split)(property_map_view_iface *This, property_map_view_iface **first,
                            property_map_view_iface **second);
} property_map_view_vtbl;

typedef struct property_observable_map_vtbl
{
    HRESULT (WINAPI *QueryInterface)(property_observable_map_iface *This, REFIID iid, void **out);
    ULONG (WINAPI *AddRef)(property_observable_map_iface *This);
    ULONG (WINAPI *Release)(property_observable_map_iface *This);
    HRESULT (WINAPI *GetIids)(property_observable_map_iface *This, ULONG *iid_count, IID **iids);
    HRESULT (WINAPI *GetRuntimeClassName)(property_observable_map_iface *This, HSTRING *class_name);
    HRESULT (WINAPI *GetTrustLevel)(property_observable_map_iface *This, TrustLevel *trust_level);
    HRESULT (WINAPI *add_MapChanged)(property_observable_map_iface *This, void *handler,
                                     EventRegistrationToken *token);
    HRESULT (WINAPI *remove_MapChanged)(property_observable_map_iface *This,
                                        EventRegistrationToken token);
} property_observable_map_vtbl;

typedef struct property_iterable_vtbl
{
    HRESULT (WINAPI *QueryInterface)(property_iterable_iface *This, REFIID iid, void **out);
    ULONG (WINAPI *AddRef)(property_iterable_iface *This);
    ULONG (WINAPI *Release)(property_iterable_iface *This);
    HRESULT (WINAPI *GetIids)(property_iterable_iface *This, ULONG *iid_count, IID **iids);
    HRESULT (WINAPI *GetRuntimeClassName)(property_iterable_iface *This, HSTRING *class_name);
    HRESULT (WINAPI *GetTrustLevel)(property_iterable_iface *This, TrustLevel *trust_level);
    HRESULT (WINAPI *First)(property_iterable_iface *This, property_iterator_iface **iterator);
} property_iterable_vtbl;

typedef struct property_iterator_vtbl
{
    HRESULT (WINAPI *QueryInterface)(property_iterator_iface *This, REFIID iid, void **out);
    ULONG (WINAPI *AddRef)(property_iterator_iface *This);
    ULONG (WINAPI *Release)(property_iterator_iface *This);
    HRESULT (WINAPI *GetIids)(property_iterator_iface *This, ULONG *iid_count, IID **iids);
    HRESULT (WINAPI *GetRuntimeClassName)(property_iterator_iface *This, HSTRING *class_name);
    HRESULT (WINAPI *GetTrustLevel)(property_iterator_iface *This, TrustLevel *trust_level);
    HRESULT (WINAPI *get_Current)(property_iterator_iface *This, property_pair_iface **pair);
    HRESULT (WINAPI *get_HasCurrent)(property_iterator_iface *This, BOOLEAN *has_current);
    HRESULT (WINAPI *MoveNext)(property_iterator_iface *This, BOOLEAN *moved);
    HRESULT (WINAPI *GetMany)(property_iterator_iface *This, UINT32 items_size,
                              property_pair_iface **items, UINT32 *count);
} property_iterator_vtbl;

typedef struct property_pair_vtbl
{
    HRESULT (WINAPI *QueryInterface)(property_pair_iface *This, REFIID iid, void **out);
    ULONG (WINAPI *AddRef)(property_pair_iface *This);
    ULONG (WINAPI *Release)(property_pair_iface *This);
    HRESULT (WINAPI *GetIids)(property_pair_iface *This, ULONG *iid_count, IID **iids);
    HRESULT (WINAPI *GetRuntimeClassName)(property_pair_iface *This, HSTRING *class_name);
    HRESULT (WINAPI *GetTrustLevel)(property_pair_iface *This, TrustLevel *trust_level);
    HRESULT (WINAPI *get_Key)(property_pair_iface *This, HSTRING *key);
    HRESULT (WINAPI *get_Value)(property_pair_iface *This, IInspectable **value);
} property_pair_vtbl;

/* vtbl instances defined further down; needed before their first users */
extern const property_iterator_vtbl property_set_IIterator_vtbl;
extern const property_pair_vtbl property_set_IKeyValuePair_vtbl;

struct property_map_iface
{
    CONST_VTBL const property_map_vtbl *lpVtbl;
};
struct property_map_view_iface
{
    CONST_VTBL const property_map_view_vtbl *lpVtbl;
};
struct property_observable_map_iface
{
    CONST_VTBL const property_observable_map_vtbl *lpVtbl;
};
struct property_iterable_iface
{
    CONST_VTBL const property_iterable_vtbl *lpVtbl;
};
struct property_iterator_iface
{
    CONST_VTBL const property_iterator_vtbl *lpVtbl;
};
struct property_pair_iface
{
    CONST_VTBL const property_pair_vtbl *lpVtbl;
};

struct property_set_entry
{
    HSTRING key;
    IInspectable *value;
};

struct property_set
{
    IPropertySet IPropertySet_iface;
    struct property_map_iface IMap_iface;
    struct property_map_view_iface IMapView_iface;
    struct property_observable_map_iface IObservableMap_iface;
    struct property_iterable_iface IIterable_iface;
    struct property_set_entry entries[PROPERTY_SET_MAX_ENTRIES];
    UINT32 count;
    LONG token;
    LONG ref;
};

struct property_iterator
{
    struct property_iterator_iface IIterator_iface;
    struct property_set *set;
    UINT32 index;
    LONG ref;
};

struct property_pair
{
    struct property_pair_iface IKeyValuePair_iface;
    HSTRING key;
    IInspectable *value;
    LONG ref;
};

static HRESULT property_set_find_index( struct property_set *impl, HSTRING key, UINT32 *index )
{
    UINT32 len;
    const WCHAR *raw = WindowsGetStringRawBuffer( key, &len );
    UINT32 i;

    for (i = 0; i < impl->count; ++i)
    {
        UINT32 elen;
        const WCHAR *eraw = WindowsGetStringRawBuffer( impl->entries[i].key, &elen );
        if (elen == len && len == memcmp( eraw, raw, len * sizeof(WCHAR) ))
        {
            *index = i;
            return S_OK;
        }
    }
    return E_BOUNDS;
}

static HRESULT WINAPI property_set_QueryInterface( IPropertySet *iface, REFIID iid, void **out )
{
    struct property_set *impl = CONTAINING_RECORD( iface, struct property_set, IPropertySet_iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IPropertySet ))
    {
        IInspectable_AddRef( (IInspectable *)&impl->IPropertySet_iface );
        *out = &impl->IPropertySet_iface;
        return S_OK;
    }
    if (IsEqualGUID( iid, &property_set_IID_IMap )) *out = &impl->IMap_iface;
    else if (IsEqualGUID( iid, &property_set_IID_IMapView )) *out = &impl->IMapView_iface;
    else if (IsEqualGUID( iid, &property_set_IID_IObservableMap )) *out = &impl->IObservableMap_iface;
    else if (IsEqualGUID( iid, &property_set_IID_IIterable )) *out = &impl->IIterable_iface;
    else
    {
        FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
        *out = NULL;
        return E_NOINTERFACE;
    }

    IInspectable_AddRef( (IInspectable *)&impl->IPropertySet_iface );
    return S_OK;
}

static ULONG WINAPI property_set_AddRef( IPropertySet *iface )
{
    struct property_set *impl = CONTAINING_RECORD( iface, struct property_set, IPropertySet_iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p ref %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI property_set_Release( IPropertySet *iface )
{
    struct property_set *impl = CONTAINING_RECORD( iface, struct property_set, IPropertySet_iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    UINT32 i;

    TRACE( "iface %p ref %lu.\n", iface, ref );
    if (ref) return ref;
    for (i = 0; i < impl->count; ++i)
    {
        WindowsDeleteString( impl->entries[i].key );
        IInspectable_Release( impl->entries[i].value );
    }
    free( impl );
    return 0;
}

static HRESULT WINAPI property_set_GetIids( IPropertySet *iface, ULONG *iid_count, IID **iids )
{
    struct property_set *impl = CONTAINING_RECORD( iface, struct property_set, IPropertySet_iface );
    TRACE( "iface %p.\n", impl );
    *iid_count = 0;
    *iids = NULL;
    return S_OK;
}

static HRESULT WINAPI property_set_GetRuntimeClassName( IPropertySet *iface, HSTRING *class_name )
{
    struct property_set *impl = CONTAINING_RECORD( iface, struct property_set, IPropertySet_iface );
    TRACE( "iface %p.\n", impl );
    return WindowsCreateString( RuntimeClass_Windows_Foundation_Collections_PropertySet,
                                wcslen( RuntimeClass_Windows_Foundation_Collections_PropertySet ),
                                class_name );
}

static HRESULT WINAPI property_set_GetTrustLevel( IPropertySet *iface, TrustLevel *trust_level )
{
    struct property_set *impl = CONTAINING_RECORD( iface, struct property_set, IPropertySet_iface );
    TRACE( "iface %p.\n", impl );
    *trust_level = BaseTrust;
    return S_OK;
}

DEFINE_IINSPECTABLE_( property_map, struct property_map_iface, struct property_set,
                      property_map_impl_from, IMap_iface, &impl->IPropertySet_iface )
DEFINE_IINSPECTABLE_( property_map_view, struct property_map_view_iface, struct property_set,
                      property_map_view_impl_from, IMapView_iface, &impl->IPropertySet_iface )
DEFINE_IINSPECTABLE_( property_observable_map, struct property_observable_map_iface, struct property_set,
                      property_observable_map_impl_from, IObservableMap_iface, &impl->IPropertySet_iface )
DEFINE_IINSPECTABLE_( property_iterable, struct property_iterable_iface, struct property_set,
                      property_iterable_impl_from, IIterable_iface, &impl->IPropertySet_iface )

static HRESULT WINAPI property_map_Lookup( property_map_iface *This, HSTRING key, IInspectable **value )
{
    struct property_set *impl = property_map_impl_from( This );
    UINT32 index;
    HRESULT hr = property_set_find_index( impl, key, &index );

    if (FAILED( hr ))
    {
        *value = NULL;
        return E_BOUNDS;
    }
    *value = impl->entries[index].value;
    IInspectable_AddRef( *value );
    return S_OK;
}

static HRESULT WINAPI property_map_get_Size( property_map_iface *This, UINT32 *size )
{
    struct property_set *impl = property_map_impl_from( This );
    *size = impl->count;
    return S_OK;
}

static HRESULT WINAPI property_map_HasKey( property_map_iface *This, HSTRING key, BOOLEAN *found )
{
    struct property_set *impl = property_map_impl_from( This );
    UINT32 index;

    *found = SUCCEEDED( property_set_find_index( impl, key, &index ) );
    return S_OK;
}

static HRESULT WINAPI property_map_GetView( property_map_iface *This, property_map_view_iface **view )
{
    struct property_set *impl = property_map_impl_from( This );
    TRACE( "-> view %p.\n", &impl->IMapView_iface );
    *view = &impl->IMapView_iface;
    IInspectable_AddRef( (IInspectable *)&impl->IPropertySet_iface );
    return S_OK;
}

static HRESULT WINAPI property_map_Insert( property_map_iface *This, HSTRING key, IInspectable *value,
                                           BOOLEAN *replaced )
{
    struct property_set *impl = property_map_impl_from( This );
    UINT32 len;
    const WCHAR *raw = WindowsGetStringRawBuffer( key, &len );
    UINT32 index;

    if (SUCCEEDED( property_set_find_index( impl, key, &index ) ))
    {
        *replaced = TRUE;
        IInspectable_Release( impl->entries[index].value );
        impl->entries[index].value = value;
        IInspectable_AddRef( value );
        return S_OK;
    }
    *replaced = FALSE;
    if (impl->count >= PROPERTY_SET_MAX_ENTRIES)
    {
        FIXME( "property set %p full (%u entries), refusing insert.\n", impl,
               (unsigned int)impl->count );
        return E_OUTOFMEMORY;
    }
    WindowsCreateString( raw, len, &impl->entries[impl->count].key );
    impl->entries[impl->count].value = value;
    IInspectable_AddRef( value );
    impl->count++;
    return S_OK;
}

static HRESULT WINAPI property_map_Remove( property_map_iface *This, HSTRING key )
{
    struct property_set *impl = property_map_impl_from( This );
    UINT32 index;

    if (FAILED( property_set_find_index( impl, key, &index ) )) return E_BOUNDS;
    WindowsDeleteString( impl->entries[index].key );
    IInspectable_Release( impl->entries[index].value );
    memmove( &impl->entries[index], &impl->entries[index + 1],
             (impl->count - index - 1) * sizeof( struct property_set_entry ) );
    impl->count--;
    return S_OK;
}

static HRESULT WINAPI property_map_Clear( property_map_iface *This )
{
    struct property_set *impl = property_map_impl_from( This );
    UINT32 i;

    for (i = 0; i < impl->count; ++i)
    {
        WindowsDeleteString( impl->entries[i].key );
        IInspectable_Release( impl->entries[i].value );
    }
    impl->count = 0;
    return S_OK;
}

static HRESULT WINAPI property_map_view_Lookup( property_map_view_iface *This, HSTRING key, IInspectable **value )
{
    struct property_set *impl = property_map_view_impl_from( This );
    UINT32 index;
    HRESULT hr = property_set_find_index( impl, key, &index );

    if (FAILED( hr ))
    {
        *value = NULL;
        return E_BOUNDS;
    }
    *value = impl->entries[index].value;
    IInspectable_AddRef( *value );
    return S_OK;
}

static HRESULT WINAPI property_map_view_get_Size( property_map_view_iface *This, UINT32 *size )
{
    struct property_set *impl = property_map_view_impl_from( This );
    *size = impl->count;
    return S_OK;
}

static HRESULT WINAPI property_map_view_HasKey( property_map_view_iface *This, HSTRING key, BOOLEAN *found )
{
    struct property_set *impl = property_map_view_impl_from( This );
    UINT32 index;

    *found = SUCCEEDED( property_set_find_index( impl, key, &index ) );
    return S_OK;
}

static HRESULT WINAPI property_map_view_Split( property_map_view_iface *This, property_map_view_iface **first,
                                               property_map_view_iface **second )
{
    struct property_set *impl = property_map_view_impl_from( This );
    TRACE( "iface %p: single view, returning itself, NULL.\n", impl );
    *first = &impl->IMapView_iface;
    IInspectable_AddRef( (IInspectable *)&impl->IPropertySet_iface );
    *second = NULL;
    return S_OK;
}

static HRESULT WINAPI property_observable_map_add_MapChanged( property_observable_map_iface *This,
                                                              void *handler, EventRegistrationToken *token )
{
    struct property_set *impl = property_observable_map_impl_from( This );
    TRACE( "iface %p, handler %p: recording no-op registration.\n", impl, handler );
    token->value = InterlockedIncrement( &impl->token );
    return S_OK;
}

static HRESULT WINAPI property_observable_map_remove_MapChanged( property_observable_map_iface *This,
                                                                 EventRegistrationToken token )
{
    struct property_set *impl = property_observable_map_impl_from( This );
    TRACE( "iface %p, token %#I64x: removing no-op registration.\n", impl, token.value );
    return S_OK;
}

static inline struct property_iterator *property_iterator_impl_from( property_iterator_iface *iface )
{
    return CONTAINING_RECORD( iface, struct property_iterator, IIterator_iface );
}

static HRESULT WINAPI property_iterator_QueryInterface( property_iterator_iface *This, REFIID iid, void **out )
{
    struct property_iterator *iter = property_iterator_impl_from( This );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &property_set_IID_IIterator ))
    {
        IInspectable_AddRef( (IInspectable *)&iter->IIterator_iface );
        *out = &iter->IIterator_iface;
        return S_OK;
    }
    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI property_iterator_AddRef( property_iterator_iface *This )
{
    struct property_iterator *iter = property_iterator_impl_from( This );
    return InterlockedIncrement( &iter->ref );
}

static HRESULT WINAPI property_iterator_GetIids( property_iterator_iface *This, ULONG *iid_count, IID **iids )
{
    struct property_iterator *iter = property_iterator_impl_from( This );
    TRACE( "iface %p.\n", iter );
    *iid_count = 0;
    *iids = NULL;
    return S_OK;
}

static HRESULT WINAPI property_iterator_GetRuntimeClassName( property_iterator_iface *This, HSTRING *class_name )
{
    static const WCHAR name[] =
        L"Windows.Foundation.Collections.IIterator`2<Windows.Foundation.String, Windows.Foundation.Object>";
    struct property_iterator *iter = property_iterator_impl_from( This );
    TRACE( "iface %p.\n", iter );
    return WindowsCreateString( name, wcslen( name ), class_name );
}

static HRESULT WINAPI property_iterator_GetTrustLevel( property_iterator_iface *This, TrustLevel *trust_level )
{
    struct property_iterator *iter = property_iterator_impl_from( This );
    TRACE( "iface %p.\n", iter );
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI property_iterable_First( property_iterable_iface *This,
                                               property_iterator_iface **iterator )
{
    struct property_set *impl = property_iterable_impl_from( This );
    struct property_iterator *iter;

    if (!(iter = calloc( 1, sizeof(*iter) ))) return E_OUTOFMEMORY;
    iter->IIterator_iface.lpVtbl = &property_set_IIterator_vtbl;
    iter->set = impl;
    IInspectable_AddRef( (IInspectable *)&impl->IPropertySet_iface );
    iter->index = 0;
    iter->ref = 1;
    *iterator = &iter->IIterator_iface;
    return S_OK;
}

static HRESULT WINAPI property_iterator_get_HasCurrent( property_iterator_iface *This, BOOLEAN *has_current )
{
    struct property_iterator *iter = property_iterator_impl_from( This );
    *has_current = iter->index < iter->set->count;
    return S_OK;
}

static HRESULT WINAPI property_iterator_get_Current( property_iterator_iface *This, property_pair_iface **pair )
{
    struct property_iterator *iter = property_iterator_impl_from( This );
    struct property_pair *p;

    if (iter->index >= iter->set->count) return E_BOUNDS;
    if (!(p = calloc( 1, sizeof(*p) ))) return E_OUTOFMEMORY;
    p->IKeyValuePair_iface.lpVtbl = &property_set_IKeyValuePair_vtbl;
    WindowsDuplicateString( iter->set->entries[iter->index].key, &p->key );
    p->value = iter->set->entries[iter->index].value;
    IInspectable_AddRef( p->value );
    p->ref = 1;
    *pair = &p->IKeyValuePair_iface;
    return S_OK;
}

static HRESULT WINAPI property_iterator_MoveNext( property_iterator_iface *This, BOOLEAN *moved )
{
    struct property_iterator *iter = property_iterator_impl_from( This );
    *moved = iter->index + 1 < iter->set->count;
    if (*moved) iter->index++;
    return S_OK;
}

static HRESULT WINAPI property_iterator_GetMany( property_iterator_iface *This, UINT32 items_size,
                                                 property_pair_iface **items, UINT32 *count )
{
    struct property_iterator *iter = property_iterator_impl_from( This );
    FIXME( "iface %p: GetMany stub!\n", iter );
    *count = 0;
    return S_OK;
}

static ULONG WINAPI property_iterator_Release( property_iterator_iface *This )
{
    struct property_iterator *iter = property_iterator_impl_from( This );
    ULONG ref = InterlockedDecrement( &iter->ref );

    if (!ref)
    {
        IInspectable_Release( (IInspectable *)&iter->set->IPropertySet_iface );
        free( iter );
    }
    return ref;
}

static inline struct property_pair *property_pair_impl_from( property_pair_iface *iface )
{
    return CONTAINING_RECORD( iface, struct property_pair, IKeyValuePair_iface );
}

static HRESULT WINAPI property_pair_QueryInterface( property_pair_iface *This, REFIID iid, void **out )
{
    struct property_pair *p = property_pair_impl_from( This );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &property_set_IID_IKeyValuePair ))
    {
        IInspectable_AddRef( (IInspectable *)&p->IKeyValuePair_iface );
        *out = &p->IKeyValuePair_iface;
        return S_OK;
    }
    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI property_pair_AddRef( property_pair_iface *This )
{
    struct property_pair *p = property_pair_impl_from( This );
    return InterlockedIncrement( &p->ref );
}

static ULONG WINAPI property_pair_Release( property_pair_iface *This )
{
    struct property_pair *p = property_pair_impl_from( This );
    ULONG ref = InterlockedDecrement( &p->ref );

    if (!ref)
    {
        WindowsDeleteString( p->key );
        IInspectable_Release( p->value );
        free( p );
    }
    return ref;
}

static HRESULT WINAPI property_pair_GetIids( property_pair_iface *This, ULONG *iid_count, IID **iids )
{
    struct property_pair *p = property_pair_impl_from( This );
    TRACE( "iface %p.\n", p );
    *iid_count = 0;
    *iids = NULL;
    return S_OK;
}

static HRESULT WINAPI property_pair_GetRuntimeClassName( property_pair_iface *This, HSTRING *class_name )
{
    struct property_pair *p = property_pair_impl_from( This );
    static const WCHAR name[] = L"Windows.Foundation.Collections.IKeyValuePair`2<Windows.Foundation.String, Windows.Foundation.Object>";
    TRACE( "iface %p.\n", p );
    return WindowsCreateString( name, wcslen( name ), class_name );
}

static HRESULT WINAPI property_pair_GetTrustLevel( property_pair_iface *This, TrustLevel *trust_level )
{
    struct property_pair *p = property_pair_impl_from( This );
    TRACE( "iface %p.\n", p );
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI property_pair_get_Key( property_pair_iface *This, HSTRING *key )
{
    struct property_pair *p = property_pair_impl_from( This );
    return WindowsDuplicateString( p->key, key );
}

static HRESULT WINAPI property_pair_get_Value( property_pair_iface *This, IInspectable **value )
{
    struct property_pair *p = property_pair_impl_from( This );
    *value = p->value;
    IInspectable_AddRef( *value );
    return S_OK;
}

static const property_map_vtbl property_set_IMap_vtbl =
{
    property_map_QueryInterface,
    property_map_AddRef,
    property_map_Release,
    property_map_GetIids,
    property_map_GetRuntimeClassName,
    property_map_GetTrustLevel,
    property_map_Lookup,
    property_map_get_Size,
    property_map_HasKey,
    property_map_GetView,
    property_map_Insert,
    property_map_Remove,
    property_map_Clear,
};

static const property_map_view_vtbl property_set_IMapView_vtbl =
{
    property_map_view_QueryInterface,
    property_map_view_AddRef,
    property_map_view_Release,
    property_map_view_GetIids,
    property_map_view_GetRuntimeClassName,
    property_map_view_GetTrustLevel,
    property_map_view_Lookup,
    property_map_view_get_Size,
    property_map_view_HasKey,
    property_map_view_Split,
};

static const property_observable_map_vtbl property_set_IObservableMap_vtbl =
{
    property_observable_map_QueryInterface,
    property_observable_map_AddRef,
    property_observable_map_Release,
    property_observable_map_GetIids,
    property_observable_map_GetRuntimeClassName,
    property_observable_map_GetTrustLevel,
    property_observable_map_add_MapChanged,
    property_observable_map_remove_MapChanged,
};

static const property_iterable_vtbl property_set_IIterable_vtbl =
{
    property_iterable_QueryInterface,
    property_iterable_AddRef,
    property_iterable_Release,
    property_iterable_GetIids,
    property_iterable_GetRuntimeClassName,
    property_iterable_GetTrustLevel,
    property_iterable_First,
};

const property_iterator_vtbl property_set_IIterator_vtbl =
{
    property_iterator_QueryInterface,
    property_iterator_AddRef,
    property_iterator_Release,
    property_iterator_GetIids,
    property_iterator_GetRuntimeClassName,
    property_iterator_GetTrustLevel,
    property_iterator_get_Current,
    property_iterator_get_HasCurrent,
    property_iterator_MoveNext,
    property_iterator_GetMany,
};

const property_pair_vtbl property_set_IKeyValuePair_vtbl =
{
    property_pair_QueryInterface,
    property_pair_AddRef,
    property_pair_Release,
    property_pair_GetIids,
    property_pair_GetRuntimeClassName,
    property_pair_GetTrustLevel,
    property_pair_get_Key,
    property_pair_get_Value,
};

static const IPropertySetVtbl property_set_vtbl =
{
    property_set_QueryInterface,
    property_set_AddRef,
    property_set_Release,
    property_set_GetIids,
    property_set_GetRuntimeClassName,
    property_set_GetTrustLevel,
};

static HRESULT property_set_create( IPropertySet **out )
{
    struct property_set *impl;

    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IPropertySet_iface.lpVtbl = &property_set_vtbl;
    impl->IMap_iface.lpVtbl = &property_set_IMap_vtbl;
    impl->IMapView_iface.lpVtbl = &property_set_IMapView_vtbl;
    impl->IObservableMap_iface.lpVtbl = &property_set_IObservableMap_vtbl;
    impl->IIterable_iface.lpVtbl = &property_set_IIterable_vtbl;
    impl->ref = 1;
    *out = &impl->IPropertySet_iface;
    return S_OK;
}

static const struct ICoreApplicationVtbl core_application_instance_vtbl =
{
    core_application_instance_QueryInterface,
    core_application_instance_AddRef,
    core_application_instance_Release,
    /* IInspectable methods */
    core_application_instance_GetIids,
    core_application_instance_GetRuntimeClassName,
    core_application_instance_GetTrustLevel,
    /* ICoreApplication methods */
    core_application_instance_get_Id,
    core_application_instance_add_Suspending,
    core_application_instance_remove_Suspending,
    core_application_instance_add_Resuming,
    core_application_instance_remove_Resuming,
    core_application_instance_get_Properties,
    core_application_instance_GetCurrentView,
    core_application_instance_Run,
    core_application_instance_RunWithActivationFactories,
};

static HRESULT core_application_instance_get( ICoreApplication **out )
{
    struct core_application_instance *impl;

    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;

    impl->ICoreApplication_iface.lpVtbl = &core_application_instance_vtbl;
    impl->ref = 1;
    *out = &impl->ICoreApplication_iface;
    TRACE( "created ICoreApplication %p.\n", *out );
    return S_OK;
}

static struct core_application_factory core_application =
{
    {&activation_factory_vtbl},
    {&statics_vtbl},
    1,
};

IActivationFactory *core_application_factory = &core_application.IActivationFactory_iface;