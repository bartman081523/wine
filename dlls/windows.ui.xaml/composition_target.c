/*
 * WinRT Windows.UI.Xaml.Media.CompositionTarget Implementation
 *
 * Copyright (C) 2026 Julian / Xodus Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "private.h"
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(xaml);

static const GUID IID_ICompositionTargetStatics = {0x2b1af03d, 0x1ed2, 0x4b59, {0xbd, 0x00, 0x75, 0x94, 0xee, 0x92, 0x83, 0x2b}};
static const GUID IID_IRenderingEventArgs        = {0x5bf7d30d, 0x9748, 0x4aed, {0x83, 0x80, 0xd7, 0x89, 0x0e, 0xb7, 0x76, 0xa0}};
static const GUID IID_IAgileObject_local       = {0x94ea2b94, 0xe9cc, 0x49e0, {0xc0, 0xff, 0xee, 0x64, 0xca, 0x8f, 0x5b, 0x90}};

/* -------------------------------------------------------------------------
 * RenderingEventArgs
 * ------------------------------------------------------------------------- */
struct rendering_event_args
{
    IInspectable IInspectable_iface;
    LONG ref;
    TimeSpan time;
};

#define impl_from_IRenderingEventArgs(ptr) ((struct rendering_event_args *)(ptr))

static HRESULT WINAPI args_QueryInterface(IInspectable *iface, REFIID riid, void **ppv)
{
    struct rendering_event_args *impl = impl_from_IRenderingEventArgs(iface);
    if (!ppv) return E_POINTER;
    *ppv = NULL;

    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject_local) ||
        IsEqualGUID(riid, &IID_IRenderingEventArgs))
    {
        *ppv = &impl->IInspectable_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    return E_NOINTERFACE;
}

static ULONG WINAPI args_AddRef(IInspectable *iface)
{
    struct rendering_event_args *impl = impl_from_IRenderingEventArgs(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI args_Release(IInspectable *iface)
{
    struct rendering_event_args *impl = impl_from_IRenderingEventArgs(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI args_GetIids(IInspectable *iface, ULONG *count, IID **iids)
{
    if (count) *count = 0;
    return S_OK;
}

static HRESULT WINAPI args_GetRuntimeClassName(IInspectable *iface, HSTRING *name)
{
    return WindowsCreateString(L"Windows.UI.Xaml.Media.RenderingEventArgs", 39, name);
}

static HRESULT WINAPI args_GetTrustLevel(IInspectable *iface, TrustLevel *level)
{
    if (level) *level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI args_get_RenderingTime(IInspectable *iface, TimeSpan *value)
{
    struct rendering_event_args *impl = impl_from_IRenderingEventArgs(iface);
    if (!value) return E_POINTER;
    *value = impl->time;
    return S_OK;
}

static const void *rendering_args_vtbl[] = {
    (void *)args_QueryInterface,
    (void *)args_AddRef,
    (void *)args_Release,
    (void *)args_GetIids,
    (void *)args_GetRuntimeClassName,
    (void *)args_GetTrustLevel,
    (void *)args_get_RenderingTime
};

/* -------------------------------------------------------------------------
 * Handlers list & 60 FPS Render Tick Thread
 * ------------------------------------------------------------------------- */
struct render_handler_node
{
    EventRegistrationToken token;
    IInspectable *handler;
    struct render_handler_node *next;
};

static CRITICAL_SECTION render_cs;
static CRITICAL_SECTION_DEBUG render_cs_debug = {
    0, 0, &render_cs,
    { &render_cs_debug.ProcessLocksList, &render_cs_debug.ProcessLocksList },
    0, 0, { (DWORD_PTR)(__FILE__ ": render_cs") }
};
static CRITICAL_SECTION render_cs = { &render_cs_debug, -1, 0, 0, 0, 0 };

static struct render_handler_node *render_handlers = NULL;
static INT64 next_render_token = 1;
static BOOL render_thread_active = FALSE;
static HANDLE render_thread_handle = NULL;
static ULONGLONG start_tick = 0;

typedef HRESULT (STDMETHODCALLTYPE *rendering_invoke_fn)(void *This, void *sender, void *args);

static DWORD WINAPI render_thread_proc(void *arg)
{
    TRACE("CompositionTarget rendering thread started\n");
    while (render_thread_active)
    {
        struct render_handler_node *node;
        struct rendering_event_args *args;
        ULONGLONG current_tick = GetTickCount64();

        Sleep(16); /* ~60 FPS */

        if (!render_handlers) continue;

        args = calloc(1, sizeof(*args));
        if (!args) continue;
        args->IInspectable_iface.lpVtbl = (const void *)rendering_args_vtbl;
        args->ref = 1;
        args->time.Duration = (INT64)(current_tick - start_tick) * 10000;

        EnterCriticalSection(&render_cs);
        node = render_handlers;
        while (node)
        {
            if (node->handler)
            {
                void **vtbl = *(void ***)node->handler;
                rendering_invoke_fn invoke = (rendering_invoke_fn)vtbl[3];
                invoke(node->handler, NULL, &args->IInspectable_iface);
            }
            node = node->next;
        }
        LeaveCriticalSection(&render_cs);

        IInspectable_Release(&args->IInspectable_iface);
    }
    TRACE("CompositionTarget rendering thread exiting\n");
    return 0;
}

/* -------------------------------------------------------------------------
 * CompositionTarget Statics & Factory
 * ------------------------------------------------------------------------- */
struct composition_target_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } ICompositionTargetStatics_iface;
};

#define impl_from_ct_factory(ptr) ((struct composition_target_factory_impl *)((char *)(ptr) - offsetof(struct composition_target_factory_impl, IActivationFactory_iface)))
#define impl_from_ct_stat_iface(ptr) ((struct composition_target_factory_impl *)((char *)(ptr) - offsetof(struct composition_target_factory_impl, ICompositionTargetStatics_iface)))

static HRESULT WINAPI ct_fac_QI(IActivationFactory *iface, REFIID riid, void **ppv)
{
    struct composition_target_factory_impl *impl = impl_from_ct_factory(iface);
    if (!ppv) return E_POINTER;
    *ppv = NULL;

    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject_local) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppv = &impl->IActivationFactory_iface;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_ICompositionTargetStatics))
    {
        *ppv = &impl->ICompositionTargetStatics_iface;
        return S_OK;
    }

    return E_NOINTERFACE;
}

static ULONG WINAPI ct_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI ct_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI ct_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI ct_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n)
{
    return WindowsCreateString(L"Windows.UI.Xaml.Media.CompositionTarget", 39, n);
}
static HRESULT WINAPI ct_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI ct_fac_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    return E_NOTIMPL;
}

static const struct IActivationFactoryVtbl ct_activation_factory_vtbl = {
    ct_fac_QI,
    ct_fac_AddRef,
    ct_fac_Release,
    ct_fac_GetIids,
    ct_fac_GetRuntimeClassName,
    ct_fac_GetTrustLevel,
    ct_fac_ActivateInstance
};

static HRESULT WINAPI ct_stat_QI(void *This, REFIID riid, void **ppv)
{
    return ct_fac_QI((IActivationFactory *)impl_from_ct_stat_iface(This), riid, ppv);
}
static ULONG WINAPI ct_stat_AddRef(void *This) { return 2; }
static ULONG WINAPI ct_stat_Release(void *This) { return 1; }
static HRESULT WINAPI ct_stat_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI ct_stat_GetRuntimeClassName(void *This, HSTRING *n)
{
    return WindowsCreateString(L"Windows.UI.Xaml.Media.CompositionTarget", 39, n);
}
static HRESULT WINAPI ct_stat_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI ct_stat_add_Rendering(void *This, void *value, EventRegistrationToken *token)
{
    struct render_handler_node *node;
    TRACE("This %p, value %p, token %p\n", This, value, token);
    if (!value || !token) return E_POINTER;

    EnterCriticalSection(&render_cs);
    node = calloc(1, sizeof(*node));
    if (!node)
    {
        LeaveCriticalSection(&render_cs);
        return E_OUTOFMEMORY;
    }
    node->token.value = next_render_token++;
    node->handler = (IInspectable *)value;
    IInspectable_AddRef((IInspectable *)value);
    node->next = render_handlers;
    render_handlers = node;
    *token = node->token;

    if (!render_thread_active)
    {
        start_tick = GetTickCount64();
        render_thread_active = TRUE;
        render_thread_handle = CreateThread(NULL, 0, render_thread_proc, NULL, 0, NULL);
    }
    LeaveCriticalSection(&render_cs);
    return S_OK;
}

static HRESULT WINAPI ct_stat_remove_Rendering(void *This, EventRegistrationToken token)
{
    struct render_handler_node **curr;
    TRACE("This %p, token %I64d\n", This, token.value);
    EnterCriticalSection(&render_cs);
    curr = &render_handlers;
    while (*curr)
    {
        if ((*curr)->token.value == token.value)
        {
            struct render_handler_node *to_free = *curr;
            *curr = to_free->next;
            if (to_free->handler) IInspectable_Release(to_free->handler);
            free(to_free);
            break;
        }
        curr = &(*curr)->next;
    }
    LeaveCriticalSection(&render_cs);
    return S_OK;
}

static HRESULT WINAPI ct_stat_add_SurfaceContentsLost(void *This, void *value, EventRegistrationToken *token)
{
    TRACE("This %p, value %p, token %p\n", This, value, token);
    if (!token) return E_POINTER;
    token->value = next_render_token++;
    return S_OK;
}

static HRESULT WINAPI ct_stat_remove_SurfaceContentsLost(void *This, EventRegistrationToken token)
{
    TRACE("This %p, token %I64d\n", This, token.value);
    return S_OK;
}

static const void *ct_statics_vtbl[] = {
    (void *)ct_stat_QI,
    (void *)ct_stat_AddRef,
    (void *)ct_stat_Release,
    (void *)ct_stat_GetIids,
    (void *)ct_stat_GetRuntimeClassName,
    (void *)ct_stat_GetTrustLevel,
    (void *)ct_stat_add_Rendering,
    (void *)ct_stat_remove_Rendering,
    (void *)ct_stat_add_SurfaceContentsLost,
    (void *)ct_stat_remove_SurfaceContentsLost
};

static struct composition_target_factory_impl ct_factory_obj = {
    { &ct_activation_factory_vtbl },
    { ct_statics_vtbl }
};

IActivationFactory *composition_target_factory = &ct_factory_obj.IActivationFactory_iface;
