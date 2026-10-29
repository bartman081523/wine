/* WinRT Windows.UI.ViewManagement.ApplicationView implementation
 *
 * Copyright 2025 Zhiyi Zhang for CodeWeavers
 * Copyright 2026 Julian / Xodus Team
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

static const GUID IID_ComWrappers_internal = {0x11d3b13a, 0x180e, 0x4789, {0xa8, 0xbe, 0x77, 0x12, 0x88, 0x28, 0x93, 0xe6}};

/* Redhawk safe hooks */
static HRESULT WINAPI redhawk_interop_m3(void *This, void *a1, void *a2, void *a3) { return S_OK; }
static HRESULT WINAPI generic_stub(void *This) { return S_OK; }

/* -------------------------------------------------------------------------
 * ApplicationViewTitleBar Implementation
 * ------------------------------------------------------------------------- */
struct title_bar_impl
{
    struct { const void *lpVtbl; } IApplicationViewTitleBar_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    LONG ref;
};

#define impl_from_ITitleBar(ptr) ((struct title_bar_impl *)((char *)(ptr) - offsetof(struct title_bar_impl, IApplicationViewTitleBar_iface)))

static HRESULT titlebar_QI_internal(struct title_bar_impl *impl, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("titlebar %p, riid %s\n", impl, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IApplicationViewTitleBar))
    {
        *ppvObject = &impl->IApplicationViewTitleBar_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ComWrappers_internal))
    {
        *ppvObject = &impl->IRedhawkInterop_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    TRACE("titlebar %p: unhandled QI %s\n", impl, debugstr_guid(riid));
    return E_NOINTERFACE;
}

static HRESULT WINAPI titlebar_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return titlebar_QI_internal(impl_from_ITitleBar(This), riid, ppvObject);
}
static ULONG WINAPI titlebar_AddRef(void *This)
{
    return InterlockedIncrement(&impl_from_ITitleBar(This)->ref);
}
static ULONG WINAPI titlebar_Release(void *This)
{
    return InterlockedDecrement(&impl_from_ITitleBar(This)->ref);
}
static HRESULT WINAPI titlebar_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI titlebar_GetRuntimeClassName(void *This, HSTRING *n)
{
    return WindowsCreateString(L"Windows.UI.ViewManagement.ApplicationViewTitleBar", 48, n);
}
static HRESULT WINAPI titlebar_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI titlebar_color_stub(void *This, void *v) { return S_OK; }
static HRESULT WINAPI titlebar_get_color_stub(void *This, void **v) { if (v) *v = NULL; return S_OK; }

static const void *title_bar_vtbl[] = {
    (void *)titlebar_QueryInterface,
    (void *)titlebar_AddRef,
    (void *)titlebar_Release,
    (void *)titlebar_GetIids,
    (void *)titlebar_GetRuntimeClassName,
    (void *)titlebar_GetTrustLevel,
    (void *)titlebar_color_stub,
    (void *)titlebar_get_color_stub,
    (void *)titlebar_color_stub,
    (void *)titlebar_get_color_stub,
    (void *)titlebar_color_stub,
    (void *)titlebar_get_color_stub,
    (void *)titlebar_color_stub,
    (void *)titlebar_get_color_stub,
    (void *)titlebar_color_stub,
    (void *)titlebar_get_color_stub,
    (void *)titlebar_color_stub,
    (void *)titlebar_get_color_stub,
    (void *)titlebar_color_stub,
    (void *)titlebar_get_color_stub,
    (void *)titlebar_color_stub,
    (void *)titlebar_get_color_stub
};

static const void *redhawk_titlebar_vtbl[] = {
    (void *)titlebar_QueryInterface,
    (void *)titlebar_AddRef,
    (void *)titlebar_Release,
    (void *)redhawk_interop_m3,
    (void *)titlebar_GetRuntimeClassName,
    (void *)titlebar_GetTrustLevel,
    (void *)titlebar_color_stub,
    (void *)redhawk_interop_m3, /* slot 7 safe hook */
    (void *)titlebar_color_stub, (void *)titlebar_color_stub, (void *)titlebar_color_stub, (void *)titlebar_color_stub
};

static struct title_bar_impl default_title_bar = {
    {title_bar_vtbl},
    {redhawk_titlebar_vtbl},
    1
};

/* -------------------------------------------------------------------------
 * ApplicationView Instance Implementation
 * ------------------------------------------------------------------------- */
struct application_view_impl
{
    struct { const void *lpVtbl; } IApplicationView_iface;
    struct { const void *lpVtbl; } IApplicationView2_iface;
    struct { const void *lpVtbl; } IApplicationView3_iface;
    struct { const void *lpVtbl; } IApplicationView4_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    LONG ref;
};

#define impl_from_IApplicationView(ptr)  ((struct application_view_impl *)((char *)(ptr) - offsetof(struct application_view_impl, IApplicationView_iface)))
#define impl_from_IApplicationView2(ptr) ((struct application_view_impl *)((char *)(ptr) - offsetof(struct application_view_impl, IApplicationView2_iface)))
#define impl_from_IApplicationView3(ptr) ((struct application_view_impl *)((char *)(ptr) - offsetof(struct application_view_impl, IApplicationView3_iface)))
#define impl_from_IApplicationView4(ptr) ((struct application_view_impl *)((char *)(ptr) - offsetof(struct application_view_impl, IApplicationView4_iface)))

static HRESULT view_QI_internal(struct application_view_impl *impl, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("view %p, riid %s\n", impl, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IApplicationView))
    {
        *ppvObject = &impl->IApplicationView_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IApplicationView2))
    {
        *ppvObject = &impl->IApplicationView2_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IApplicationView3))
    {
        *ppvObject = &impl->IApplicationView3_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IApplicationView4))
    {
        *ppvObject = &impl->IApplicationView4_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ComWrappers_internal))
    {
        *ppvObject = &impl->IRedhawkInterop_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    TRACE("view %p: unhandled QI %s\n", impl, debugstr_guid(riid));
    return E_NOINTERFACE;
}

static HRESULT WINAPI view_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return view_QI_internal(impl_from_IApplicationView(This), riid, ppvObject);
}
static ULONG WINAPI view_AddRef(void *This)
{
    return InterlockedIncrement(&impl_from_IApplicationView(This)->ref);
}
static ULONG WINAPI view_Release(void *This)
{
    return InterlockedDecrement(&impl_from_IApplicationView(This)->ref);
}
static HRESULT WINAPI view_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI view_GetRuntimeClassName(void *This, HSTRING *n)
{
    return WindowsCreateString(L"Windows.UI.ViewManagement.ApplicationView", 40, n);
}
static HRESULT WINAPI view_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI view_get_Orientation(void *This, ApplicationViewOrientation *value)
{
    if (!value) return E_POINTER;
    *value = ApplicationViewOrientation_Landscape;
    return S_OK;
}
static HRESULT WINAPI view_get_AdjacentToLeftDisplayEdge(void *This, boolean *value)
{
    if (!value) return E_POINTER;
    *value = TRUE;
    return S_OK;
}
static HRESULT WINAPI view_get_AdjacentToRightDisplayEdge(void *This, boolean *value)
{
    if (!value) return E_POINTER;
    *value = TRUE;
    return S_OK;
}
static HRESULT WINAPI view_get_IsFullScreen(void *This, boolean *value)
{
    if (!value) return E_POINTER;
    *value = FALSE;
    return S_OK;
}
static HRESULT WINAPI view_get_IsOnLockScreen(void *This, boolean *value)
{
    if (!value) return E_POINTER;
    *value = FALSE;
    return S_OK;
}
static HRESULT WINAPI view_get_IsScreenCaptureEnabled(void *This, boolean *value)
{
    if (!value) return E_POINTER;
    *value = TRUE;
    return S_OK;
}
static HRESULT WINAPI view_put_IsScreenCaptureEnabled(void *This, boolean value) { return S_OK; }
static HRESULT WINAPI view_put_Title(void *This, HSTRING value) { return S_OK; }
static HRESULT WINAPI view_get_Title(void *This, HSTRING *value)
{
    if (!value) return E_POINTER;
    return WindowsCreateString(L"Calculator", 10, value);
}
static HRESULT WINAPI view_get_Id(void *This, INT32 *value)
{
    if (!value) return E_POINTER;
    *value = 1;
    return S_OK;
}
static HRESULT WINAPI view_add_Consolidated(void *This, void *handler, EventRegistrationToken *token)
{
    if (token) token->value = 1;
    return S_OK;
}
static HRESULT WINAPI view_remove_Consolidated(void *This, EventRegistrationToken token) { return S_OK; }

static const void *application_view_vtbl[] = {
    (void *)view_QueryInterface,
    (void *)view_AddRef,
    (void *)view_Release,
    (void *)view_GetIids,
    (void *)view_GetRuntimeClassName,
    (void *)view_GetTrustLevel,
    (void *)view_get_Orientation,
    (void *)view_get_AdjacentToLeftDisplayEdge,
    (void *)view_get_AdjacentToRightDisplayEdge,
    (void *)view_get_IsFullScreen,
    (void *)view_get_IsOnLockScreen,
    (void *)view_get_IsScreenCaptureEnabled,
    (void *)view_put_IsScreenCaptureEnabled,
    (void *)view_put_Title,
    (void *)view_get_Title,
    (void *)view_get_Id,
    (void *)view_add_Consolidated,
    (void *)view_remove_Consolidated
};

/* IApplicationView2 methods */
static HRESULT WINAPI view2_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return view_QI_internal(impl_from_IApplicationView2(This), riid, ppvObject);
}
static ULONG WINAPI view2_AddRef(void *This)
{
    return InterlockedIncrement(&impl_from_IApplicationView2(This)->ref);
}
static ULONG WINAPI view2_Release(void *This)
{
    return InterlockedDecrement(&impl_from_IApplicationView2(This)->ref);
}
static HRESULT WINAPI view2_get_SuppressSystemOverlays(void *This, boolean *value)
{
    if (!value) return E_POINTER;
    *value = FALSE;
    return S_OK;
}
static HRESULT WINAPI view2_put_SuppressSystemOverlays(void *This, boolean value) { return S_OK; }
static HRESULT WINAPI view2_get_VisibleBounds(void *This, Rect *value)
{
    if (!value) return E_POINTER;
    value->X = 0; value->Y = 0; value->Width = 1920; value->Height = 1080;
    return S_OK;
}
static HRESULT WINAPI view2_add_VisibleBoundsChanged(void *This, void *handler, EventRegistrationToken *token)
{
    if (token) token->value = 1;
    return S_OK;
}
static HRESULT WINAPI view2_remove_VisibleBoundsChanged(void *This, EventRegistrationToken token) { return S_OK; }
static HRESULT WINAPI view2_SetDesiredBoundsMode(void *This, ApplicationViewBoundsMode mode, boolean *success)
{
    if (success) *success = TRUE;
    return S_OK;
}
static HRESULT WINAPI view2_get_DesiredBoundsMode(void *This, ApplicationViewBoundsMode *value)
{
    if (!value) return E_POINTER;
    *value = ApplicationViewBoundsMode_UseVisible;
    return S_OK;
}

static const void *application_view2_vtbl[] = {
    (void *)view2_QueryInterface,
    (void *)view2_AddRef,
    (void *)view2_Release,
    (void *)view_GetIids,
    (void *)view_GetRuntimeClassName,
    (void *)view_GetTrustLevel,
    (void *)view2_get_SuppressSystemOverlays,
    (void *)view2_put_SuppressSystemOverlays,
    (void *)view2_get_VisibleBounds,
    (void *)view2_add_VisibleBoundsChanged,
    (void *)view2_remove_VisibleBoundsChanged,
    (void *)view2_SetDesiredBoundsMode,
    (void *)view2_get_DesiredBoundsMode
};

/* IApplicationView3 methods */
static HRESULT WINAPI view3_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return view_QI_internal(impl_from_IApplicationView3(This), riid, ppvObject);
}
static ULONG WINAPI view3_AddRef(void *This)
{
    return InterlockedIncrement(&impl_from_IApplicationView3(This)->ref);
}
static ULONG WINAPI view3_Release(void *This)
{
    return InterlockedDecrement(&impl_from_IApplicationView3(This)->ref);
}
static HRESULT WINAPI view3_get_TitleBar(void *This, void **value)
{
    TRACE("view3_get_TitleBar\n");
    if (!value) return E_POINTER;
    *value = &default_title_bar.IApplicationViewTitleBar_iface;
    return S_OK;
}
static HRESULT WINAPI view3_get_FullScreenSystemOverlayMode(void *This, FullScreenSystemOverlayMode *value)
{
    if (!value) return E_POINTER;
    *value = FullScreenSystemOverlayMode_Standard;
    return S_OK;
}
static HRESULT WINAPI view3_put_FullScreenSystemOverlayMode(void *This, FullScreenSystemOverlayMode value) { return S_OK; }
static HRESULT WINAPI view3_get_IsFullScreenMode(void *This, boolean *value)
{
    if (!value) return E_POINTER;
    *value = FALSE;
    return S_OK;
}
static HRESULT WINAPI view3_TryEnterFullScreenMode(void *This, boolean *success)
{
    if (success) *success = FALSE;
    return S_OK;
}
static HRESULT WINAPI view3_ExitFullScreenMode(void *This) { return S_OK; }
static HRESULT WINAPI view3_ShowStandardSystemOverlays(void *This) { return S_OK; }
static HRESULT WINAPI view3_TryResizeView(void *This, Size value, boolean *success)
{
    TRACE("ApplicationView3::TryResizeView (%f x %f)\n", value.Width, value.Height);
    if (success) *success = TRUE;
    return S_OK;
}
static HRESULT WINAPI view3_SetPreferredMinSize(void *This, Size size)
{
    TRACE("ApplicationView3::SetPreferredMinSize (%f x %f)\n", size.Width, size.Height);
    return S_OK;
}

static const void *application_view3_vtbl[] = {
    (void *)view3_QueryInterface,
    (void *)view3_AddRef,
    (void *)view3_Release,
    (void *)view_GetIids,
    (void *)view_GetRuntimeClassName,
    (void *)view_GetTrustLevel,
    (void *)view3_get_TitleBar,
    (void *)view3_get_FullScreenSystemOverlayMode,
    (void *)view3_put_FullScreenSystemOverlayMode,
    (void *)view3_get_IsFullScreenMode,
    (void *)view3_TryEnterFullScreenMode,
    (void *)view3_ExitFullScreenMode,
    (void *)view3_ShowStandardSystemOverlays,
    (void *)view3_TryResizeView,
    (void *)view3_SetPreferredMinSize
};

static HRESULT WINAPI view4_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return view_QI_internal(impl_from_IApplicationView4(This), riid, ppvObject);
}
static ULONG WINAPI view4_AddRef(void *This)
{
    return InterlockedIncrement(&impl_from_IApplicationView4(This)->ref);
}
static ULONG WINAPI view4_Release(void *This)
{
    return InterlockedDecrement(&impl_from_IApplicationView4(This)->ref);
}
static HRESULT WINAPI view4_get_ViewMode(void *This, ApplicationViewMode *value)
{
    TRACE("ApplicationView4::get_ViewMode\n");
    if (!value) return E_POINTER;
    *value = ApplicationViewMode_Default;
    return S_OK;
}
static HRESULT WINAPI view4_IsViewModeSupported(void *This, ApplicationViewMode mode, boolean *is_mode_supported)
{
    TRACE("ApplicationView4::IsViewModeSupported mode=%d\n", mode);
    if (!is_mode_supported) return E_POINTER;
    *is_mode_supported = TRUE;
    return S_OK;
}
static HRESULT WINAPI view4_TryEnterViewModeAsync(void *This, ApplicationViewMode mode, void **operation)
{
    TRACE("ApplicationView4::TryEnterViewModeAsync mode=%d\n", mode);
    if (operation) *operation = NULL;
    return S_OK;
}
static HRESULT WINAPI view4_TryEnterViewModeWithPreferencesAsync(void *This, ApplicationViewMode mode, void *preferences, void **operation)
{
    TRACE("ApplicationView4::TryEnterViewModeWithPreferencesAsync mode=%d\n", mode);
    if (operation) *operation = NULL;
    return S_OK;
}
static HRESULT WINAPI view4_TryConsolidateAsync(void *This, void **operation)
{
    TRACE("ApplicationView4::TryConsolidateAsync\n");
    if (operation) *operation = NULL;
    return S_OK;
}

static const void *application_view4_vtbl[] = {
    (void *)view4_QueryInterface,
    (void *)view4_AddRef,
    (void *)view4_Release,
    (void *)view_GetIids,
    (void *)view_GetRuntimeClassName,
    (void *)view_GetTrustLevel,
    (void *)view4_get_ViewMode,
    (void *)view4_IsViewModeSupported,
    (void *)view4_TryEnterViewModeAsync,
    (void *)view4_TryEnterViewModeWithPreferencesAsync,
    (void *)view4_TryConsolidateAsync
};

static const void *redhawk_view_vtbl[] = {
    (void *)view_QueryInterface,
    (void *)view_AddRef,
    (void *)view_Release,
    (void *)redhawk_interop_m3,
    (void *)view_GetRuntimeClassName,
    (void *)view_GetTrustLevel,
    (void *)view_get_Id,
    (void *)redhawk_interop_m3, /* slot 7 safe hook */
    (void *)redhawk_interop_m3, (void *)redhawk_interop_m3, (void *)redhawk_interop_m3, (void *)redhawk_interop_m3,
    (void *)redhawk_interop_m3, (void *)redhawk_interop_m3, (void *)redhawk_interop_m3, (void *)redhawk_interop_m3,
    (void *)redhawk_interop_m3, (void *)redhawk_interop_m3, (void *)redhawk_interop_m3, (void *)redhawk_interop_m3,
    (void *)redhawk_interop_m3, (void *)redhawk_interop_m3, (void *)redhawk_interop_m3, (void *)redhawk_interop_m3
};

static struct application_view_impl current_view = {
    {application_view_vtbl},
    {application_view2_vtbl},
    {application_view3_vtbl},
    {application_view4_vtbl},
    {redhawk_view_vtbl},
    1
};

/* -------------------------------------------------------------------------
 * Factory Implementation
 * ------------------------------------------------------------------------- */
struct factory
{
    struct { const void *lpVtbl; } IActivationFactory_iface;
    struct { const void *lpVtbl; } IApplicationViewStatics_iface;
    struct { const void *lpVtbl; } IApplicationViewStatics2_iface;
    struct { const void *lpVtbl; } IApplicationViewStatics3_iface;
    struct { const void *lpVtbl; } IApplicationViewInteropStatics_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    LONG ref;
};

#define impl_from_fac(ptr)         ((struct factory *)((char *)(ptr) - offsetof(struct factory, IActivationFactory_iface)))
#define impl_from_fac_s1(ptr)      ((struct factory *)((char *)(ptr) - offsetof(struct factory, IApplicationViewStatics_iface)))
#define impl_from_fac_s2(ptr)      ((struct factory *)((char *)(ptr) - offsetof(struct factory, IApplicationViewStatics2_iface)))
#define impl_from_fac_s3(ptr)      ((struct factory *)((char *)(ptr) - offsetof(struct factory, IApplicationViewStatics3_iface)))
#define impl_from_fac_interop(ptr) ((struct factory *)((char *)(ptr) - offsetof(struct factory, IApplicationViewInteropStatics_iface)))
#define impl_from_fac_red(ptr)     ((struct factory *)((char *)(ptr) - offsetof(struct factory, IRedhawkInterop_iface)))

static HRESULT fac_QI_internal(struct factory *impl, REFIID riid, void **out)
{
    if (!out) return E_POINTER;
    *out = NULL;

    TRACE("impl %p, riid %s\n", impl, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *out = &impl->IActivationFactory_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IApplicationViewStatics))
    {
        *out = &impl->IApplicationViewStatics_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IApplicationViewStatics2))
    {
        *out = &impl->IApplicationViewStatics2_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IApplicationViewStatics3))
    {
        *out = &impl->IApplicationViewStatics3_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IApplicationViewInteropStatics))
    {
        *out = &impl->IApplicationViewInteropStatics_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ComWrappers_internal))
    {
        *out = &impl->IRedhawkInterop_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    FIXME("%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid(riid));
    return E_NOINTERFACE;
}

static HRESULT WINAPI fac_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return fac_QI_internal(impl_from_fac(This), riid, ppvObject);
}
static ULONG WINAPI fac_AddRef(void *This)
{
    return InterlockedIncrement(&impl_from_fac(This)->ref);
}
static ULONG WINAPI fac_Release(void *This)
{
    return InterlockedDecrement(&impl_from_fac(This)->ref);
}
static HRESULT WINAPI fac_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI fac_GetRuntimeClassName(void *This, HSTRING *n)
{
    return WindowsCreateString(L"Windows.UI.ViewManagement.ApplicationView", 40, n);
}
static HRESULT WINAPI fac_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI fac_ActivateInstance(void *This, void **instance)
{
    if (instance) *instance = NULL;
    return E_NOTIMPL;
}

static const void *activation_factory_vtbl[] = {
    (void *)fac_QueryInterface,
    (void *)fac_AddRef,
    (void *)fac_Release,
    (void *)fac_GetIids,
    (void *)fac_GetRuntimeClassName,
    (void *)fac_GetTrustLevel,
    (void *)fac_ActivateInstance
};

static HRESULT WINAPI statics1_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return fac_QI_internal(impl_from_fac_s1(This), riid, ppvObject);
}
static ULONG WINAPI statics1_AddRef(void *This) { return fac_AddRef(impl_from_fac_s1(This)); }
static ULONG WINAPI statics1_Release(void *This) { return fac_Release(impl_from_fac_s1(This)); }
static HRESULT WINAPI statics1_Value(void *This, ApplicationViewState *value)
{
    if (!value) return E_POINTER;
    *value = ApplicationViewState_FullScreenLandscape;
    return S_OK;
}
static HRESULT WINAPI statics1_TryUnsnap(void *This, boolean *success)
{
    if (success) *success = TRUE;
    return S_OK;
}

static const void *statics1_vtbl[] = {
    (void *)statics1_QueryInterface,
    (void *)statics1_AddRef,
    (void *)statics1_Release,
    (void *)fac_GetIids,
    (void *)fac_GetRuntimeClassName,
    (void *)fac_GetTrustLevel,
    (void *)statics1_Value,
    (void *)statics1_TryUnsnap
};

static HRESULT WINAPI statics2_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return fac_QI_internal(impl_from_fac_s2(This), riid, ppvObject);
}
static ULONG WINAPI statics2_AddRef(void *This) { return fac_AddRef(impl_from_fac_s2(This)); }
static ULONG WINAPI statics2_Release(void *This) { return fac_Release(impl_from_fac_s2(This)); }
static HRESULT WINAPI statics2_GetForCurrentView(void *This, void **current)
{
    TRACE("statics2_GetForCurrentView This %p, current %p\n", This, current);
    if (!current) return E_POINTER;
    *current = &current_view.IApplicationView_iface;
    return S_OK;
}
static HRESULT WINAPI statics2_get_TerminateAppOnFinalViewClose(void *This, boolean *value)
{
    if (!value) return E_POINTER;
    *value = FALSE;
    return S_OK;
}
static HRESULT WINAPI statics2_put_TerminateAppOnFinalViewClose(void *This, boolean value) { return S_OK; }

static const void *statics2_vtbl[] = {
    (void *)statics2_QueryInterface,
    (void *)statics2_AddRef,
    (void *)statics2_Release,
    (void *)fac_GetIids,
    (void *)fac_GetRuntimeClassName,
    (void *)fac_GetTrustLevel,
    (void *)statics2_GetForCurrentView,
    (void *)statics2_get_TerminateAppOnFinalViewClose,
    (void *)statics2_put_TerminateAppOnFinalViewClose
};

/* IApplicationViewStatics3 */
static HRESULT WINAPI statics3_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return fac_QI_internal(impl_from_fac_s3(This), riid, ppvObject);
}
static ULONG WINAPI statics3_AddRef(void *This) { return fac_AddRef(impl_from_fac_s3(This)); }
static ULONG WINAPI statics3_Release(void *This) { return fac_Release(impl_from_fac_s3(This)); }
static HRESULT WINAPI statics3_get_PreferredLaunchWindowingMode(void *This, ApplicationViewWindowingMode *value)
{
    if (!value) return E_POINTER;
    *value = ApplicationViewWindowingMode_Auto;
    return S_OK;
}
static HRESULT WINAPI statics3_put_PreferredLaunchWindowingMode(void *This, ApplicationViewWindowingMode value)
{
    TRACE("put_PreferredLaunchWindowingMode: %d\n", value);
    return S_OK;
}
static HRESULT WINAPI statics3_get_PreferredLaunchViewSize(void *This, Size *value)
{
    if (!value) return E_POINTER;
    value->Width = 320.0f;
    value->Height = 500.0f;
    return S_OK;
}
static HRESULT WINAPI statics3_put_PreferredLaunchViewSize(void *This, Size value)
{
    TRACE("put_PreferredLaunchViewSize: %f x %f\n", value.Width, value.Height);
    return S_OK;
}

static const void *statics3_vtbl[] = {
    (void *)statics3_QueryInterface,
    (void *)statics3_AddRef,
    (void *)statics3_Release,
    (void *)fac_GetIids,
    (void *)fac_GetRuntimeClassName,
    (void *)fac_GetTrustLevel,
    (void *)statics3_get_PreferredLaunchWindowingMode,
    (void *)statics3_put_PreferredLaunchWindowingMode,
    (void *)statics3_get_PreferredLaunchViewSize,
    (void *)statics3_put_PreferredLaunchViewSize
};

/* IApplicationViewInteropStatics */
static HRESULT WINAPI statics_interop_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    return fac_QI_internal(impl_from_fac_interop(This), riid, ppvObject);
}
static ULONG WINAPI statics_interop_AddRef(void *This) { return fac_AddRef(impl_from_fac_interop(This)); }
static ULONG WINAPI statics_interop_Release(void *This) { return fac_Release(impl_from_fac_interop(This)); }
static HRESULT WINAPI statics_interop_GetApplicationViewIdForWindow(void *This, void *window, INT32 *id)
{
    TRACE("ApplicationViewInteropStatics::GetApplicationViewIdForWindow window=%p\n", window);
    if (!id) return E_POINTER;
    *id = 1;
    return S_OK;
}

static const void *statics_interop_vtbl[] = {
    (void *)statics_interop_QueryInterface,
    (void *)statics_interop_AddRef,
    (void *)statics_interop_Release,
    (void *)fac_GetIids,
    (void *)fac_GetRuntimeClassName,
    (void *)fac_GetTrustLevel,
    (void *)statics_interop_GetApplicationViewIdForWindow
};

static HRESULT WINAPI redhawk_fac_QI(void *This, REFIID riid, void **ppvObject)
{
    return fac_QI_internal(impl_from_fac_red(This), riid, ppvObject);
}
static ULONG WINAPI redhawk_fac_AddRef(void *This) { return 2; }
static ULONG WINAPI redhawk_fac_Release(void *This) { return 1; }

static const void *redhawk_factory_vtbl[] = {
    (void *)redhawk_fac_QI,
    (void *)redhawk_fac_AddRef,
    (void *)redhawk_fac_Release,
    (void *)redhawk_interop_m3,
    (void *)fac_GetRuntimeClassName,
    (void *)fac_GetTrustLevel,
    (void *)statics2_GetForCurrentView,
    (void *)redhawk_interop_m3, /* slot 7 safe hook */
    (void *)redhawk_interop_m3, (void *)redhawk_interop_m3, (void *)redhawk_interop_m3, (void *)redhawk_interop_m3
};

static struct factory default_application_view_factory =
{
    {activation_factory_vtbl},
    {statics1_vtbl},
    {statics2_vtbl},
    {statics3_vtbl},
    {statics_interop_vtbl},
    {redhawk_factory_vtbl},
    1,
};

IActivationFactory *application_view_factory = (IActivationFactory *)&default_application_view_factory.IActivationFactory_iface;
