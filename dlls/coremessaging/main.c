/* WinRT CoreMessaging Implementation
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

#include "initguid.h"
#include "private.h"
#include "dispatcherqueue.h"

WINE_DEFAULT_DEBUG_CHANNEL(messaging);

struct dispatcher_queue_controller_statics
{
    IActivationFactory IActivationFactory_iface;
    IDispatcherQueueControllerStatics IDispatcherQueueControllerStatics_iface;
    LONG ref;
};

static inline struct dispatcher_queue_controller_statics *impl_from_IActivationFactory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct dispatcher_queue_controller_statics, IActivationFactory_iface );
}

static HRESULT WINAPI factory_QueryInterface( IActivationFactory *iface, REFIID iid, void **out )
{
    struct dispatcher_queue_controller_statics *impl = impl_from_IActivationFactory( iface );

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

    if (IsEqualGUID( iid, &IID_IDispatcherQueueControllerStatics ))
    {
        *out = &impl->IDispatcherQueueControllerStatics_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI factory_AddRef( IActivationFactory *iface )
{
    struct dispatcher_queue_controller_statics *impl = impl_from_IActivationFactory( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing ref to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI factory_Release( IActivationFactory *iface )
{
    struct dispatcher_queue_controller_statics *impl = impl_from_IActivationFactory( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p decreasing ref to %lu.\n", iface, ref );
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

struct dispatcher_queue_controller
{
    IDispatcherQueueController IDispatcherQueueController_iface;
    LONG ref;
};

static inline struct dispatcher_queue_controller *impl_from_IDispatcherQueueController( IDispatcherQueueController *iface )
{
    return CONTAINING_RECORD( iface, struct dispatcher_queue_controller, IDispatcherQueueController_iface );
}

static HRESULT WINAPI dispatcher_queue_controller_QueryInterface( IDispatcherQueueController *iface, REFIID iid, void **out )
{
    struct dispatcher_queue_controller *impl = impl_from_IDispatcherQueueController( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IDispatcherQueueController ))
    {
        *out = &impl->IDispatcherQueueController_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI dispatcher_queue_controller_AddRef( IDispatcherQueueController *iface )
{
    struct dispatcher_queue_controller *impl = impl_from_IDispatcherQueueController( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing ref to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI dispatcher_queue_controller_Release( IDispatcherQueueController *iface )
{
    struct dispatcher_queue_controller *impl = impl_from_IDispatcherQueueController( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing ref to %lu.\n", iface, ref );

    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI dispatcher_queue_controller_GetIids( IDispatcherQueueController *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI dispatcher_queue_controller_GetRuntimeClassName( IDispatcherQueueController *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI dispatcher_queue_controller_GetTrustLevel( IDispatcherQueueController *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI dispatcher_queue_controller_get_DispatcherQueue( IDispatcherQueueController *iface, IDispatcherQueue **value )
{
    FIXME( "iface %p, value %p stub!\n", iface, value );
    return E_NOTIMPL;
}

static HRESULT shutdown_queue_async( IUnknown *invoker, IUnknown *param, PROPVARIANT *result, BOOL called_async )
{
    if (!called_async) return STATUS_PENDING;
    return S_OK;
}

static HRESULT WINAPI dispatcher_queue_controller_ShutdownQueueAsync( IDispatcherQueueController *iface, IAsyncAction **operation )
{
    FIXME( "iface %p, operation %p stub!\n", iface, operation );

    if (!operation) return E_POINTER;
    *operation = NULL;

    return async_action_create( NULL, shutdown_queue_async, operation );
}

static const struct IDispatcherQueueControllerVtbl dispatcher_queue_controller_vtbl =
{
    dispatcher_queue_controller_QueryInterface,
    dispatcher_queue_controller_AddRef,
    dispatcher_queue_controller_Release,
    /* IInspectable methods */
    dispatcher_queue_controller_GetIids,
    dispatcher_queue_controller_GetRuntimeClassName,
    dispatcher_queue_controller_GetTrustLevel,
    /* IDispatcherQueueController methods */
    dispatcher_queue_controller_get_DispatcherQueue,
    dispatcher_queue_controller_ShutdownQueueAsync,
};

DEFINE_IINSPECTABLE( dispatcher_queue_controller_statics, IDispatcherQueueControllerStatics, struct dispatcher_queue_controller_statics, IActivationFactory_iface )

static HRESULT WINAPI dispatcher_queue_controller_statics_CreateOnDedicatedThread( IDispatcherQueueControllerStatics *iface, IDispatcherQueueController **result )
{
    FIXME( "iface %p, result %p stub!\n", iface, result );
    return E_NOTIMPL;
}

static const struct IDispatcherQueueControllerStaticsVtbl dispatcher_queue_controller_statics_vtbl =
{
    dispatcher_queue_controller_statics_QueryInterface,
    dispatcher_queue_controller_statics_AddRef,
    dispatcher_queue_controller_statics_Release,
    /* IInspectable methods */
    dispatcher_queue_controller_statics_GetIids,
    dispatcher_queue_controller_statics_GetRuntimeClassName,
    dispatcher_queue_controller_statics_GetTrustLevel,
    /* IDispatcherQueueControllerStatics methods */
    dispatcher_queue_controller_statics_CreateOnDedicatedThread,
};

static struct dispatcher_queue_controller_statics dispatcher_queue_controller_statics =
{
    {&factory_vtbl},
    {&dispatcher_queue_controller_statics_vtbl},
    1,
};

static IActivationFactory *dispatcher_queue_controller_factory = &dispatcher_queue_controller_statics.IActivationFactory_iface;

HRESULT WINAPI DllGetActivationFactory( HSTRING classid, IActivationFactory **factory )
{
    const WCHAR *name = WindowsGetStringRawBuffer( classid, NULL );

    TRACE( "classid %s, factory %p.\n", debugstr_hstring( classid ), factory );

    *factory = NULL;

    if (!wcscmp( name, RuntimeClass_Windows_System_DispatcherQueueController ))
        IActivationFactory_QueryInterface( dispatcher_queue_controller_factory, &IID_IActivationFactory, (void **)factory );

    if (*factory) return S_OK;
    return CLASS_E_CLASSNOTAVAILABLE;
}

/***********************************************************************
 *           CoreUICreate   (coremessaging.@)
 *
 * Microsoft.Internal.FrameworkUdk (FUN_18007b810 @ +0x7b84c) imports exactly
 * this one function from CoreMessaging.dll, calls it with the address of its
 * own com_ptr wrapper argument and afterwards unwraps it via get() and
 * QueryInterfaces the stored object for IUnknown (IID at DAT_1800abc70).
 * run55 crashed on MOV RAX,[RAX] at +0x7b877 because nothing had put an
 * object into the wrapper. On real Windows this creates the CoreUI messaging
 * session object; here we surface a minimal IUnknown so the caller chain can
 * continue and the next required interfaces show up in the log.
 */
/* The {4b2edab9-129b-4733-be15-356853d55ace} CoreUI session interface:
 * FrameworkUdk FUN_1800775c0 / FUN_1800779b0 / FUN_180077870 QueryInterface
 * the wrapper object for this IID (failure answers 0x80004002 and the caller
 * WIL fail-fasts with c0000602, run56) and then dispatch through jump-only
 * thunks (mov rax,[rcx]; mov rax,<off>(rax); jmp guard_dispatch_icall) back
 * into the SAME base vtable. Observed thunk offsets: 0x18-0x68, 0x90, 0x98,
 * 0xa8, 0xb0, 0xb8, 0xe8, 0x110, 0x128. run57: slot +0x40 (FUN_180079ad0 via
 * FUN_180077870/FUN_18007b9d0) was the first consumed method; E_NOTIMPL in a
 * consumed slot fail-fasts (RoFailFastWithErrorContextInternal2 0x80004001,
 * run57), so every unknown slot answers S_OK and logs its arguments. */
static const IID IID_ICoreUISession =
    {0x4b2edab9, 0x129b, 0x4733, {0xbe,0x15,0x35,0x68,0x53,0xd5,0x5a,0xce}};

/* {fcd9d50d-0902-4a2d-ac94-b95c3316f6a0} (DAT_1800adab0): run60 — after slot 8
 * stored the session into the caller wrapper, FUN_18007b610 gets the session
 * back out, QIs it for this IID, and on success hands the QI'd pointer as 5th
 * arg to the 5-arg adapter FUN_1800775c0, which dispatches thunk
 * FUN_180079b10 = vtable offset +0x60 (slot 12). Answered with the same base
 * iface; if the consumer needs a distinct vtable, revisit. */
static const IID IID_ICoreUISession2 =
    {0xfcd9d50d, 0x0902, 0x4a2d, {0xac,0x94,0xb9,0x5c,0x33,0x16,0xf6,0xa0}};

/* Private CoreUI IID family, consecutive in FrameworkUdk .rdata:
 * 0x1800ad970 {2f292ff4-823c-4a17-9fd5-188c56e252f8}
 * 0x1800ad980 {4ba72ec1-8968-4103-9111-0d70575add0f}
 * 0x1800ad9a0 {37a94c05-1aa6-4cea-9049-083bba055cec} — run62: QI'd by
 *   FUN_180077460 (fn-call idiom like b610), which on success calls
 *   param_2(session, *param_3, *param_4, *(int*)param_5).
 * 0x1800ad9b0 {1bf477b7-489c-4960-b567-25a614eaa81a}
 * All are QI'd on the same session object, and each consumer passes the
 * method function pointer explicitly, so sharing our base vtable is safe.
 * Accepted preemptively to save probe runs. */
static const IID IID_ICoreUISession3 =
    {0x37a94c05, 0x1aa6, 0x4cea, {0x90,0x49,0x08,0x3b,0xba,0x05,0x5c,0xec}};
static const IID IID_ICoreUISession4 =
    {0x2f292ff4, 0x823c, 0x4a17, {0x9f,0xd5,0x18,0x8c,0x56,0xe2,0x52,0xf8}};
static const IID IID_ICoreUISession5 =
    {0x4ba72ec1, 0x8968, 0x4103, {0x91,0x11,0x0d,0x70,0x57,0x5a,0xdd,0x0f}};
static const IID IID_ICoreUISession6 =
    {0x1bf477b7, 0x489c, 0x4960, {0xb5,0x67,0x25,0xa6,0x14,0xea,0xa8,0x1a}};

typedef struct ICoreUISession ICoreUISession;

typedef struct ICoreUISessionVtbl
{
    HRESULT (WINAPI *QueryInterface)(ICoreUISession *iface, REFIID iid, void **out);
    ULONG (WINAPI *AddRef)(ICoreUISession *iface);
    ULONG (WINAPI *Release)(ICoreUISession *iface);
    HRESULT (WINAPI *slot3)(ICoreUISession *iface, void *a1, void *a2, void *a3);
    HRESULT (WINAPI *slot4)(ICoreUISession *iface, void *a1, void *a2, void *a3);
    HRESULT (WINAPI *slot5)(ICoreUISession *iface, void *a1, void *a2, void *a3);
    HRESULT (WINAPI *slot6)(ICoreUISession *iface, void *a1, void *a2, void *a3);
    HRESULT (WINAPI *slot7)(ICoreUISession *iface, void *a1, void *a2, void *a3);
    HRESULT (WINAPI *slot8)(ICoreUISession *iface, void *a1, void *a2, void *a3);
    HRESULT (WINAPI *slot9)(ICoreUISession *iface, void *a1, void *a2, void *a3);
    HRESULT (WINAPI *slot10)(ICoreUISession *iface, void *a1, void *a2, void *a3, void *a4);
    HRESULT (WINAPI *slot11)(ICoreUISession *iface, void *a1, void *a2, void *a3);
    HRESULT (WINAPI *slot12)(ICoreUISession *iface, void *a1, void *a2, void *a3);
    HRESULT (WINAPI *slot13)(ICoreUISession *iface, void *a1, void *a2, void *a3);
    HRESULT (WINAPI *slot14)(ICoreUISession *iface, void *a1, void *a2, void *a3);
    HRESULT (WINAPI *slot15)(ICoreUISession *iface, void *a1, void *a2, void *a3);
    HRESULT (WINAPI *slot16)(ICoreUISession *iface, void *a1, void *a2, void *a3);
    HRESULT (WINAPI *slot17)(ICoreUISession *iface, void *a1, void *a2, void *a3);
    HRESULT (WINAPI *slot18)(ICoreUISession *iface, void *a1);
    HRESULT (WINAPI *slot19)(ICoreUISession *iface, void *a1, void *a2, void *a3);
    HRESULT (WINAPI *slot20)(ICoreUISession *iface, void *a1, void *a2, void *a3);
    HRESULT (WINAPI *slot21)(ICoreUISession *iface, void *a1, void *a2, void *a3);
} ICoreUISessionVtbl;

struct ICoreUISession
{
    const ICoreUISessionVtbl *lpVtbl;
};

struct coreui_session
{
    ICoreUISession ICoreUISession_iface;
    LONG ref;
};

extern void * WINAPI RtlPcToFileHeader(void *pc, void **base);

static BOOL ptr_readable( const void *p, SIZE_T len )
{
    MEMORY_BASIC_INFORMATION mbi;

    if (!p) return FALSE;
    if (!VirtualQuery( p, &mbi, sizeof(mbi) )) return FALSE;
    if (mbi.State != MEM_COMMIT) return FALSE;
    if (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD)) return FALSE;
    return (const BYTE *)p + len <= (const BYTE *)mbi.BaseAddress + mbi.RegionSize;
}

static void dump_arg( const char *tag, const void *p )
{
    if (ptr_readable( p, 16 ))
    {
        const DWORD *v = p;
        TRACE( "  %s %p: %08lx %08lx %08lx %08lx\n", tag, p, (unsigned long)v[0],
               (unsigned long)v[1], (unsigned long)v[2], (unsigned long)v[3] );
    }
    else
        TRACE( "  %s %p: not readable\n", tag, p );
}

static ULONG WINAPI coreui_session_AddRef( ICoreUISession *iface );
static ULONG WINAPI coreui_session_Release( ICoreUISession *iface );

/* __builtin_return_address(0) taken inside the stub itself: that is the call
 * site in the FrameworkUdk adapter, resolvable by RtlPcToFileHeader. A level
 * above cannot be walked because the thunks are jump-only and Wine builds
 * without frame pointers (run57 produced garbage for level 1). */

static HRESULT WINAPI coreui_session_QueryInterface( ICoreUISession *iface, REFIID iid, void **out )
{
    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_ICoreUISession ) ||
        IsEqualGUID( iid, &IID_ICoreUISession2 ) ||
        IsEqualGUID( iid, &IID_ICoreUISession3 ) ||
        IsEqualGUID( iid, &IID_ICoreUISession4 ) ||
        IsEqualGUID( iid, &IID_ICoreUISession5 ) ||
        IsEqualGUID( iid, &IID_ICoreUISession6 ))
    {
        *out = iface;
        coreui_session_AddRef( iface );
        return S_OK;
    }

    {
        void *ret = __builtin_return_address( 0 );
        void *base = NULL;

        RtlPcToFileHeader( ret, &base );
        FIXME( "%s not implemented, returning E_NOINTERFACE, caller ra %p (module base %p).\n",
               debugstr_guid( iid ), ret, base );
    }
    *out = NULL;
    return E_NOINTERFACE;
}

/* QI refusal still keeps E_NOINTERFACE: FrameworkUdk fail-fasts there, so a
 * lying S_OK would crash deeper inside. */

static ULONG WINAPI coreui_session_AddRef( ICoreUISession *iface )
{
    struct coreui_session *impl = CONTAINING_RECORD( iface, struct coreui_session, ICoreUISession_iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p, ref %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI coreui_session_Release( ICoreUISession *iface )
{
    struct coreui_session *impl = CONTAINING_RECORD( iface, struct coreui_session, ICoreUISession_iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p, ref %lu.\n", iface, ref );
    return ref;
}

#define COREUI_STUB(n) \
    static HRESULT WINAPI coreui_session_slot##n( ICoreUISession *iface, void *a1, void *a2, void *a3 ) \
    { \
        void *ret = __builtin_return_address( 0 ); \
        void *base = NULL; \
        RtlPcToFileHeader( ret, &base ); \
        FIXME( "iface %p: vtable slot %d stub, a1 %p, a2 %p, a3 %p, caller ra %p (module base %p).\n", \
               iface, n, a1, a2, a3, ret, base ); \
        dump_arg( "a1", a1 ); \
        dump_arg( "a2", a2 ); \
        dump_arg( "a3", a3 ); \
        return S_OK; \
    }

/* slot 3 is hand-written below: FUN_180077750 passes a void **out wrapper as
 * its second parameter that FUN_18007f420 afterwards unwraps via get() and
 * QueryInterfaces for IID_IUnknown (run61 AV at FrameworkUdk+0x7f491,
 * mov rax,[rax] with rax=0, because the stub left *out untouched). Same
 * out-wrapper contract as slot 8. */
static HRESULT WINAPI coreui_session_slot3( ICoreUISession *iface, void *a1, void *a2, void *a3 )
{
    void *ret = __builtin_return_address( 0 );
    void *base = NULL;

    RtlPcToFileHeader( ret, &base );
    FIXME( "iface %p: vtable slot 3 (+0x18, FUN_180079c50 via FUN_180077750), a1 %p, a2 %p, a3 %p, "
           "caller ra %p (module base %p).\n", iface, a1, a2, a3, ret, base );
    if (a1)
    {
        *(void **)a1 = iface;
        coreui_session_AddRef( iface );
        TRACE( "  stored session %p into out %p.\n", iface, a1 );
    }
    return S_OK;
}

COREUI_STUB(4)
COREUI_STUB(5)
COREUI_STUB(6)
COREUI_STUB(7)
/* slot 8 is hand-written below: FUN_180077870 passes a void **out wrapper as
 * its third parameter that FUN_18007b9d0 afterwards unwraps via get() and
 * QueryInterfaces for IID_IUnknown (run58 AV at FrameworkUdk+0x7ba4e,
 * mov rax,[rax] with rax=0, because the stub left *out untouched). */
COREUI_STUB(9)
COREUI_STUB(11)
COREUI_STUB(12)
COREUI_STUB(13)
COREUI_STUB(14)
COREUI_STUB(15)
COREUI_STUB(16)
COREUI_STUB(17)
COREUI_STUB(19)
COREUI_STUB(20)
COREUI_STUB(21)

#undef COREUI_STUB

/* offset +0x40: (this, arg1, void **out). The adapter FUN_180077870 calls it
 * as methodfn(our_iface, *arg3, &caller_wrapper) and its caller FUN_18007b9d0
 * then get()s that wrapper and QueryInterfaces it for IID_IUnknown. Fill the
 * out slot with our session object so the unwrap succeeds. */
static HRESULT WINAPI coreui_session_slot8( ICoreUISession *iface, void *a1, void *a2, void *a3 )
{
    void *ret = __builtin_return_address( 0 );
    void *base = NULL;

    RtlPcToFileHeader( ret, &base );
    FIXME( "iface %p: vtable slot 8 (+0x40), a1 %p, out %p, a3 %p, caller ra %p (module base %p).\n",
           iface, a1, a2, a3, ret, base );
    dump_arg( "a1", a1 );
    if (a2)
    {
        *(void **)a2 = iface;
        coreui_session_AddRef( iface );
        TRACE( "  stored session %p into out %p.\n", iface, a2 );
    }
    return S_OK;
}

/* offset +0x50, reached via FUN_180079c30 with four extra parameters. */
static HRESULT WINAPI coreui_session_slot10( ICoreUISession *iface, void *a1, void *a2, void *a3, void *a4 )
{
    void *ret = __builtin_return_address( 0 );
    void *base = NULL;

    RtlPcToFileHeader( ret, &base );
    FIXME( "vtable slot +0x50: iface %p, a1 %p, a2 %p, a3 %p, a4 %p, caller ra %p (module base %p).\n",
           iface, a1, a2, a3, a4, ret, base );
    dump_arg( "a1", a1 );
    dump_arg( "a2", a2 );
    dump_arg( "a3", a3 );
    dump_arg( "a4", a4 );
    return S_OK;
}

/* offset +0x90, reached via FUN_180079cf0 with one extra parameter. */
static HRESULT WINAPI coreui_session_slot18( ICoreUISession *iface, void *a1 )
{
    void *ret = __builtin_return_address( 0 );
    void *base = NULL;

    RtlPcToFileHeader( ret, &base );
    FIXME( "vtable slot +0x90: iface %p, a1 %p, caller ra %p (module base %p).\n", iface, a1, ret, base );
    dump_arg( "a1", a1 );
    return S_OK;
}

static const struct ICoreUISessionVtbl coreui_session_vtbl =
{
    coreui_session_QueryInterface,
    coreui_session_AddRef,
    coreui_session_Release,
    /* slots +0x18 - +0x48, unknown semantics */
    coreui_session_slot3,
    coreui_session_slot4,
    coreui_session_slot5,
    coreui_session_slot6,
    coreui_session_slot7,
    coreui_session_slot8,
    coreui_session_slot9,
    /* offset +0x50, four extra parameters */
    coreui_session_slot10,
    coreui_session_slot11,
    coreui_session_slot12,
    coreui_session_slot13,
    coreui_session_slot14,
    coreui_session_slot15,
    coreui_session_slot16,
    coreui_session_slot17,
    /* offset +0x90, one extra parameter */
    coreui_session_slot18,
    coreui_session_slot19,
    coreui_session_slot20,
    coreui_session_slot21,
};

static struct coreui_session coreui_session =
{
    { &coreui_session_vtbl },
    1,
};

HRESULT WINAPI CoreUICreate( void *wrapper )
{
    FIXME( "wrapper %p semi-stub, storing coreui_session %p!\n", wrapper, &coreui_session );
    *(void **)wrapper = &coreui_session.ICoreUISession_iface;
    return S_OK;
}

HRESULT WINAPI CreateDispatcherQueueController( DispatcherQueueOptions options, PDISPATCHERQUEUECONTROLLER *queue_controller )
{
    struct dispatcher_queue_controller *impl;

    FIXME( "options.dwSize = %lu, options.threadType = %d, options.apartmentType = %d, queue_controller %p semi-stub!\n",
            options.dwSize, options.threadType, options.apartmentType, queue_controller );

    if (!queue_controller) return E_POINTER;
    if (options.dwSize != sizeof( DispatcherQueueOptions )) return E_INVALIDARG;
    if (options.threadType != DQTYPE_THREAD_DEDICATED && options.threadType != DQTYPE_THREAD_CURRENT) return E_INVALIDARG;
    if (!(impl = calloc( 1, sizeof( *impl ) ))) return E_OUTOFMEMORY;

    impl->IDispatcherQueueController_iface.lpVtbl = &dispatcher_queue_controller_vtbl;
    impl->ref = 1;

    *queue_controller = &impl->IDispatcherQueueController_iface;
    TRACE( "created IDispatcherQueueController %p.\n", *queue_controller );
    return S_OK;
}
