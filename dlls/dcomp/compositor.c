/*
 * DirectComposition WinRT Compositor Implementation
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

#include <stdarg.h>

#define COBJMACROS
#include "windef.h"
#include "winbase.h"
#include "winstring.h"
#include "wine/debug.h"
#include "objbase.h"
#include "initguid.h"
#include "activation.h"

#define WIDL_using_Windows_Foundation_Numerics
#include "windows.foundation.numerics.h"

#define WIDL_using_Windows_UI
#include "windows.ui.h"

#define WIDL_using_Windows_UI_Composition
#include "windows.ui.composition.h"

WINE_DEFAULT_DEBUG_CHANNEL(dcomp);

/* Desktop interop interface GUIDs */
/* {29E691FA-4567-4DCA-B319-D0F207EB6807} */
DEFINE_GUID(IID_ICompositorDesktopInterop, 0x29e691fa, 0x4567, 0x4dca, 0xb3, 0x19, 0xd0, 0xf2, 0x07, 0xeb, 0x68, 0x07);

/* {6329D6CA-3366-490E-9DB3-25312929AC51} */
DEFINE_GUID(IID_IDesktopWindowTarget, 0x6329d6ca, 0x3366, 0x490e, 0x9d, 0xb3, 0x25, 0x31, 0x29, 0x29, 0xac, 0x51);

/* {25297D5C-3AD4-4C9C-B5CF-E36A38512330} */
DEFINE_GUID(IID_ICompositorInterop, 0x25297d5c, 0x3ad4, 0x4c9c, 0xb5, 0xcf, 0xe3, 0x6a, 0x38, 0x51, 0x23, 0x30);

/* Windows.Foundation.IClosable: {30d5a762-e565-4f40-81a0-eb33ed6123ab} */
DEFINE_GUID(IID_IClosable, 0x30d5a762, 0xe565, 0x4f40, 0x81, 0xa0, 0xeb, 0x33, 0xed, 0x61, 0x23, 0xab);

/* ICompositor Desktop Interop declaration */
typedef struct ICompositorDesktopInterop ICompositorDesktopInterop;
typedef struct ICompositorDesktopInteropVtbl
{
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(ICompositorDesktopInterop *This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(ICompositorDesktopInterop *This);
    ULONG (STDMETHODCALLTYPE *Release)(ICompositorDesktopInterop *This);
    HRESULT (STDMETHODCALLTYPE *CreateDesktopWindowTarget)(ICompositorDesktopInterop *This, HWND hwndTarget, BOOL isTopmost, ICompositionTarget **result);
} ICompositorDesktopInteropVtbl;

struct ICompositorDesktopInterop
{
    const ICompositorDesktopInteropVtbl *lpVtbl;
};

/* IDesktopWindowTarget declaration */
typedef struct IDesktopWindowTarget IDesktopWindowTarget;
typedef struct IDesktopWindowTargetVtbl
{
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IDesktopWindowTarget *This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IDesktopWindowTarget *This);
    ULONG (STDMETHODCALLTYPE *Release)(IDesktopWindowTarget *This);
    HRESULT (STDMETHODCALLTYPE *get_IsTopmost)(IDesktopWindowTarget *This, boolean *value);
} IDesktopWindowTargetVtbl;

struct IDesktopWindowTarget
{
    const IDesktopWindowTargetVtbl *lpVtbl;
};

/* ICompositorInterop declaration */
typedef struct ICompositorInterop ICompositorInterop;
typedef struct ICompositorInteropVtbl
{
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(ICompositorInterop *This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(ICompositorInterop *This);
    ULONG (STDMETHODCALLTYPE *Release)(ICompositorInterop *This);
    HRESULT (STDMETHODCALLTYPE *CreateCompositionSurfaceForHandle)(ICompositorInterop *This, HANDLE swapchain, ICompositionSurface **result);
    HRESULT (STDMETHODCALLTYPE *CreateCompositionSurfaceForSwapChain)(ICompositorInterop *This, IUnknown *swapchain, ICompositionSurface **result);
    HRESULT (STDMETHODCALLTYPE *CreateGraphicsDevice)(ICompositorInterop *This, IUnknown *device, ICompositionGraphicsDevice **result);
} ICompositorInteropVtbl;

struct ICompositorInterop
{
    const ICompositorInteropVtbl *lpVtbl;
};

/* --- Forward Declarations --- */
struct compositor;
struct desktop_window_target;
struct visual;
struct visual_collection;
struct color_brush;
struct property_set;
struct scoped_batch;
struct commit_batch;

static HRESULT create_visual(struct compositor *compositor, IVisual **out);
static HRESULT create_color_brush(Color color, ICompositionColorBrush **out);
static HRESULT create_property_set(ICompositionPropertySet **out);
static HRESULT create_scoped_batch(CompositionBatchTypes type, ICompositionScopedBatch **out);
static HRESULT create_commit_batch(CompositionBatchTypes type, ICompositionCommitBatch **out);

/* ========================================================================= */
/* --- DesktopWindowTarget Implementation --- */
/* ========================================================================= */

struct desktop_window_target
{
    ICompositionTarget ICompositionTarget_iface;
    IDesktopWindowTarget IDesktopWindowTarget_iface;
    LONG ref;
    HWND hwnd;
    BOOL is_topmost;
    IVisual *root;
};

static inline struct desktop_window_target *impl_from_ICompositionTarget(ICompositionTarget *iface)
{
    return CONTAINING_RECORD(iface, struct desktop_window_target, ICompositionTarget_iface);
}

static inline struct desktop_window_target *impl_from_IDesktopWindowTarget(IDesktopWindowTarget *iface)
{
    return CONTAINING_RECORD(iface, struct desktop_window_target, IDesktopWindowTarget_iface);
}

static HRESULT WINAPI desktop_window_target_QueryInterface(ICompositionTarget *iface, REFIID iid, void **out)
{
    struct desktop_window_target *impl = impl_from_ICompositionTarget(iface);

    TRACE("iface %p, iid %s, out %p.\n", iface, debugstr_guid(iid), out);

    if (!out) return E_POINTER;

    if (IsEqualGUID(iid, &IID_IUnknown) ||
        IsEqualGUID(iid, &IID_IInspectable) ||
        IsEqualGUID(iid, &IID_IAgileObject) ||
        IsEqualGUID(iid, &IID_ICompositionTarget))
    {
        *out = &impl->ICompositionTarget_iface;
        ICompositionTarget_AddRef(iface);
        return S_OK;
    }
    if (IsEqualGUID(iid, &IID_IDesktopWindowTarget))
    {
        *out = &impl->IDesktopWindowTarget_iface;
        ICompositionTarget_AddRef(iface);
        return S_OK;
    }

    WARN("unsupported iid %s.\n", debugstr_guid(iid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI desktop_window_target_AddRef(ICompositionTarget *iface)
{
    struct desktop_window_target *impl = impl_from_ICompositionTarget(iface);
    ULONG ref = InterlockedIncrement(&impl->ref);
    TRACE("iface %p, ref %lu.\n", iface, ref);
    return ref;
}

static ULONG WINAPI desktop_window_target_Release(ICompositionTarget *iface)
{
    struct desktop_window_target *impl = impl_from_ICompositionTarget(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    TRACE("iface %p, ref %lu.\n", iface, ref);
    if (!ref)
    {
        if (impl->root) IVisual_Release(impl->root);
        free(impl);
    }
    return ref;
}

static HRESULT WINAPI desktop_window_target_GetIids(ICompositionTarget *iface, ULONG *iidCount, IID **iids)
{
    FIXME("iface %p, iidCount %p, iids %p stub!\n", iface, iidCount, iids);
    return E_NOTIMPL;
}

static HRESULT WINAPI desktop_window_target_GetRuntimeClassName(ICompositionTarget *iface, HSTRING *className)
{
    TRACE("iface %p, className %p.\n", iface, className);
    return WindowsCreateString(L"Windows.UI.Composition.Desktop.DesktopWindowTarget",
                               wcslen(L"Windows.UI.Composition.Desktop.DesktopWindowTarget"), className);
}

static HRESULT WINAPI desktop_window_target_GetTrustLevel(ICompositionTarget *iface, TrustLevel *trustLevel)
{
    TRACE("iface %p, trustLevel %p.\n", iface, trustLevel);
    *trustLevel = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI desktop_window_target_get_Root(ICompositionTarget *iface, IVisual **value)
{
    struct desktop_window_target *impl = impl_from_ICompositionTarget(iface);
    TRACE("iface %p, value %p.\n", iface, value);
    if (!value) return E_POINTER;
    if (impl->root) IVisual_AddRef(impl->root);
    *value = impl->root;
    return S_OK;
}

static HRESULT WINAPI desktop_window_target_put_Root(ICompositionTarget *iface, IVisual *value)
{
    struct desktop_window_target *impl = impl_from_ICompositionTarget(iface);
    TRACE("iface %p, value %p.\n", iface, value);
    if (impl->root) IVisual_Release(impl->root);
    impl->root = value;
    if (impl->root) IVisual_AddRef(impl->root);
    return S_OK;
}

static const struct ICompositionTargetVtbl desktop_window_target_vtbl =
{
    desktop_window_target_QueryInterface,
    desktop_window_target_AddRef,
    desktop_window_target_Release,
    desktop_window_target_GetIids,
    desktop_window_target_GetRuntimeClassName,
    desktop_window_target_GetTrustLevel,
    desktop_window_target_get_Root,
    desktop_window_target_put_Root
};

static HRESULT WINAPI desktop_window_target_interop_QueryInterface(IDesktopWindowTarget *iface, REFIID iid, void **out)
{
    struct desktop_window_target *impl = impl_from_IDesktopWindowTarget(iface);
    return desktop_window_target_QueryInterface(&impl->ICompositionTarget_iface, iid, out);
}

static ULONG WINAPI desktop_window_target_interop_AddRef(IDesktopWindowTarget *iface)
{
    struct desktop_window_target *impl = impl_from_IDesktopWindowTarget(iface);
    return desktop_window_target_AddRef(&impl->ICompositionTarget_iface);
}

static ULONG WINAPI desktop_window_target_interop_Release(IDesktopWindowTarget *iface)
{
    struct desktop_window_target *impl = impl_from_IDesktopWindowTarget(iface);
    return desktop_window_target_Release(&impl->ICompositionTarget_iface);
}

static HRESULT WINAPI desktop_window_target_interop_get_IsTopmost(IDesktopWindowTarget *iface, boolean *value)
{
    struct desktop_window_target *impl = impl_from_IDesktopWindowTarget(iface);
    TRACE("iface %p, value %p.\n", iface, value);
    if (!value) return E_POINTER;
    *value = impl->is_topmost;
    return S_OK;
}

static const struct IDesktopWindowTargetVtbl desktop_window_target_interop_vtbl =
{
    desktop_window_target_interop_QueryInterface,
    desktop_window_target_interop_AddRef,
    desktop_window_target_interop_Release,
    desktop_window_target_interop_get_IsTopmost
};

/* ========================================================================= */
/* --- VisualCollection Implementation --- */
/* ========================================================================= */

struct visual_collection
{
    IVisualCollection IVisualCollection_iface;
    LONG ref;
};

static inline struct visual_collection *impl_from_IVisualCollection(IVisualCollection *iface)
{
    return CONTAINING_RECORD(iface, struct visual_collection, IVisualCollection_iface);
}

static HRESULT WINAPI visual_collection_QueryInterface(IVisualCollection *iface, REFIID iid, void **out)
{
    struct visual_collection *impl = impl_from_IVisualCollection(iface);

    if (!out) return E_POINTER;

    if (IsEqualGUID(iid, &IID_IUnknown) ||
        IsEqualGUID(iid, &IID_IInspectable) ||
        IsEqualGUID(iid, &IID_IAgileObject) ||
        IsEqualGUID(iid, &IID_IVisualCollection))
    {
        *out = &impl->IVisualCollection_iface;
        IVisualCollection_AddRef(iface);
        return S_OK;
    }

    WARN("unsupported iid %s.\n", debugstr_guid(iid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI visual_collection_AddRef(IVisualCollection *iface)
{
    struct visual_collection *impl = impl_from_IVisualCollection(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI visual_collection_Release(IVisualCollection *iface)
{
    struct visual_collection *impl = impl_from_IVisualCollection(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI visual_collection_GetIids(IVisualCollection *iface, ULONG *iidCount, IID **iids)
{
    return E_NOTIMPL;
}

static HRESULT WINAPI visual_collection_GetRuntimeClassName(IVisualCollection *iface, HSTRING *className)
{
    return WindowsCreateString(L"Windows.UI.Composition.VisualCollection",
                               wcslen(L"Windows.UI.Composition.VisualCollection"), className);
}

static HRESULT WINAPI visual_collection_GetTrustLevel(IVisualCollection *iface, TrustLevel *trustLevel)
{
    *trustLevel = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI visual_collection_get_Count(IVisualCollection *iface, INT32 *value)
{
    TRACE("iface %p, value %p.\n", iface, value);
    if (!value) return E_POINTER;
    *value = 0;
    return S_OK;
}

static HRESULT WINAPI visual_collection_InsertAbove(IVisualCollection *iface, IVisual *child, IVisual *sibling)
{
    TRACE("iface %p, child %p, sibling %p.\n", iface, child, sibling);
    return S_OK;
}

static HRESULT WINAPI visual_collection_InsertAtBottom(IVisualCollection *iface, IVisual *child)
{
    TRACE("iface %p, child %p.\n", iface, child);
    return S_OK;
}

static HRESULT WINAPI visual_collection_InsertAtTop(IVisualCollection *iface, IVisual *child)
{
    TRACE("iface %p, child %p.\n", iface, child);
    return S_OK;
}

static HRESULT WINAPI visual_collection_InsertBelow(IVisualCollection *iface, IVisual *child, IVisual *sibling)
{
    TRACE("iface %p, child %p, sibling %p.\n", iface, child, sibling);
    return S_OK;
}

static HRESULT WINAPI visual_collection_Remove(IVisualCollection *iface, IVisual *child)
{
    TRACE("iface %p, child %p.\n", iface, child);
    return S_OK;
}

static HRESULT WINAPI visual_collection_RemoveAll(IVisualCollection *iface)
{
    TRACE("iface %p.\n", iface);
    return S_OK;
}

static const struct IVisualCollectionVtbl visual_collection_vtbl =
{
    visual_collection_QueryInterface,
    visual_collection_AddRef,
    visual_collection_Release,
    visual_collection_GetIids,
    visual_collection_GetRuntimeClassName,
    visual_collection_GetTrustLevel,
    visual_collection_get_Count,
    visual_collection_InsertAbove,
    visual_collection_InsertAtBottom,
    visual_collection_InsertAtTop,
    visual_collection_InsertBelow,
    visual_collection_Remove,
    visual_collection_RemoveAll
};

/* ========================================================================= */
/* --- Visual / ContainerVisual / SpriteVisual Implementation --- */
/* ========================================================================= */

struct visual
{
    IVisual IVisual_iface;
    IContainerVisual IContainerVisual_iface;
    ISpriteVisual ISpriteVisual_iface;
    ICompositionObject ICompositionObject_iface;
    LONG ref;
    struct compositor *compositor;
    struct visual_collection *children;
    IVisual *parent;
    FLOAT opacity;
    boolean is_visible;
};

static inline struct visual *impl_from_IVisual(IVisual *iface)
{
    return CONTAINING_RECORD(iface, struct visual, IVisual_iface);
}

static inline struct visual *impl_from_IContainerVisual(IContainerVisual *iface)
{
    return CONTAINING_RECORD(iface, struct visual, IContainerVisual_iface);
}

static inline struct visual *impl_from_ISpriteVisual(ISpriteVisual *iface)
{
    return CONTAINING_RECORD(iface, struct visual, ISpriteVisual_iface);
}

static inline struct visual *impl_from_ICompositionObject(ICompositionObject *iface)
{
    return CONTAINING_RECORD(iface, struct visual, ICompositionObject_iface);
}

static HRESULT WINAPI visual_QueryInterface(IVisual *iface, REFIID iid, void **out)
{
    struct visual *impl = impl_from_IVisual(iface);

    if (!out) return E_POINTER;

    if (IsEqualGUID(iid, &IID_IUnknown) ||
        IsEqualGUID(iid, &IID_IInspectable) ||
        IsEqualGUID(iid, &IID_IAgileObject) ||
        IsEqualGUID(iid, &IID_IVisual))
    {
        *out = &impl->IVisual_iface;
        IVisual_AddRef(iface);
        return S_OK;
    }
    if (IsEqualGUID(iid, &IID_IContainerVisual))
    {
        *out = &impl->IContainerVisual_iface;
        IVisual_AddRef(iface);
        return S_OK;
    }
    if (IsEqualGUID(iid, &IID_ISpriteVisual))
    {
        *out = &impl->ISpriteVisual_iface;
        IVisual_AddRef(iface);
        return S_OK;
    }
    if (IsEqualGUID(iid, &IID_ICompositionObject))
    {
        *out = &impl->ICompositionObject_iface;
        IVisual_AddRef(iface);
        return S_OK;
    }

    WARN("unsupported iid %s.\n", debugstr_guid(iid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI visual_AddRef(IVisual *iface)
{
    struct visual *impl = impl_from_IVisual(iface);
    ULONG ref = InterlockedIncrement(&impl->ref);
    TRACE("iface %p, ref %lu.\n", iface, ref);
    return ref;
}

static ULONG WINAPI visual_Release(IVisual *iface)
{
    struct visual *impl = impl_from_IVisual(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    TRACE("iface %p, ref %lu.\n", iface, ref);
    if (!ref)
    {
        if (impl->children) IVisualCollection_Release(&impl->children->IVisualCollection_iface);
        free(impl);
    }
    return ref;
}

static HRESULT WINAPI visual_GetIids(IVisual *iface, ULONG *iidCount, IID **iids)
{
    return E_NOTIMPL;
}

static HRESULT WINAPI visual_GetRuntimeClassName(IVisual *iface, HSTRING *className)
{
    return WindowsCreateString(L"Windows.UI.Composition.SpriteVisual",
                               wcslen(L"Windows.UI.Composition.SpriteVisual"), className);
}

static HRESULT WINAPI visual_GetTrustLevel(IVisual *iface, TrustLevel *trustLevel)
{
    *trustLevel = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI visual_get_AnchorPoint(IVisual *iface, Vector2 *value)
{
    if (value) memset(value, 0, sizeof(*value));
    return S_OK;
}

static HRESULT WINAPI visual_put_AnchorPoint(IVisual *iface, Vector2 value)
{
    return S_OK;
}

static HRESULT WINAPI visual_get_BackfaceVisibility(IVisual *iface, CompositionBackfaceVisibility *value)
{
    if (value) *value = CompositionBackfaceVisibility_Inherit;
    return S_OK;
}

static HRESULT WINAPI visual_put_BackfaceVisibility(IVisual *iface, CompositionBackfaceVisibility value)
{
    return S_OK;
}

static HRESULT WINAPI visual_get_BorderMode(IVisual *iface, CompositionBorderMode *value)
{
    if (value) *value = CompositionBorderMode_Inherit;
    return S_OK;
}

static HRESULT WINAPI visual_put_BorderMode(IVisual *iface, CompositionBorderMode value)
{
    return S_OK;
}

static HRESULT WINAPI visual_get_CenterPoint(IVisual *iface, Vector3 *value)
{
    if (value) memset(value, 0, sizeof(*value));
    return S_OK;
}

static HRESULT WINAPI visual_put_CenterPoint(IVisual *iface, Vector3 value)
{
    return S_OK;
}

static HRESULT WINAPI visual_get_Clip(IVisual *iface, ICompositionClip **value)
{
    if (value) *value = NULL;
    return S_OK;
}

static HRESULT WINAPI visual_put_Clip(IVisual *iface, ICompositionClip *value)
{
    return S_OK;
}

static HRESULT WINAPI visual_get_CompositeMode(IVisual *iface, CompositionCompositeMode *value)
{
    if (value) *value = CompositionCompositeMode_Inherit;
    return S_OK;
}

static HRESULT WINAPI visual_put_CompositeMode(IVisual *iface, CompositionCompositeMode value)
{
    return S_OK;
}

static HRESULT WINAPI visual_get_IsVisible(IVisual *iface, boolean *value)
{
    struct visual *impl = impl_from_IVisual(iface);
    if (value) *value = impl->is_visible;
    return S_OK;
}

static HRESULT WINAPI visual_put_IsVisible(IVisual *iface, boolean value)
{
    struct visual *impl = impl_from_IVisual(iface);
    impl->is_visible = value;
    return S_OK;
}

static HRESULT WINAPI visual_get_Offset(IVisual *iface, Vector3 *value)
{
    if (value) memset(value, 0, sizeof(*value));
    return S_OK;
}

static HRESULT WINAPI visual_put_Offset(IVisual *iface, Vector3 value)
{
    return S_OK;
}

static HRESULT WINAPI visual_get_Opacity(IVisual *iface, FLOAT *value)
{
    struct visual *impl = impl_from_IVisual(iface);
    if (value) *value = impl->opacity;
    return S_OK;
}

static HRESULT WINAPI visual_put_Opacity(IVisual *iface, FLOAT value)
{
    struct visual *impl = impl_from_IVisual(iface);
    impl->opacity = value;
    return S_OK;
}

static HRESULT WINAPI visual_get_Orientation(IVisual *iface, Quaternion *value)
{
    if (value) memset(value, 0, sizeof(*value));
    return S_OK;
}

static HRESULT WINAPI visual_put_Orientation(IVisual *iface, Quaternion value)
{
    return S_OK;
}

static HRESULT WINAPI visual_get_Parent(IVisual *iface, IContainerVisual **value)
{
    if (value) *value = NULL;
    return S_OK;
}

static HRESULT WINAPI visual_get_RotationAngle(IVisual *iface, FLOAT *value)
{
    if (value) *value = 0.0f;
    return S_OK;
}

static HRESULT WINAPI visual_put_RotationAngle(IVisual *iface, FLOAT value)
{
    return S_OK;
}

static HRESULT WINAPI visual_get_RotationAngleInDegrees(IVisual *iface, FLOAT *value)
{
    if (value) *value = 0.0f;
    return S_OK;
}

static HRESULT WINAPI visual_put_RotationAngleInDegrees(IVisual *iface, FLOAT value)
{
    return S_OK;
}

static HRESULT WINAPI visual_get_RotationAxis(IVisual *iface, Vector3 *value)
{
    if (value) memset(value, 0, sizeof(*value));
    return S_OK;
}

static HRESULT WINAPI visual_put_RotationAxis(IVisual *iface, Vector3 value)
{
    return S_OK;
}

static HRESULT WINAPI visual_get_Scale(IVisual *iface, Vector3 *value)
{
    if (value) { value->X = 1.0f; value->Y = 1.0f; value->Z = 1.0f; }
    return S_OK;
}

static HRESULT WINAPI visual_put_Scale(IVisual *iface, Vector3 value)
{
    return S_OK;
}

static HRESULT WINAPI visual_get_Size(IVisual *iface, Vector2 *value)
{
    if (value) memset(value, 0, sizeof(*value));
    return S_OK;
}

static HRESULT WINAPI visual_put_Size(IVisual *iface, Vector2 value)
{
    return S_OK;
}

static HRESULT WINAPI visual_get_TransformMatrix(IVisual *iface, Matrix4x4 *value)
{
    if (value) memset(value, 0, sizeof(*value));
    return S_OK;
}

static HRESULT WINAPI visual_put_TransformMatrix(IVisual *iface, Matrix4x4 value)
{
    return S_OK;
}

static const struct IVisualVtbl visual_vtbl =
{
    visual_QueryInterface,
    visual_AddRef,
    visual_Release,
    visual_GetIids,
    visual_GetRuntimeClassName,
    visual_GetTrustLevel,
    visual_get_AnchorPoint,
    visual_put_AnchorPoint,
    visual_get_BackfaceVisibility,
    visual_put_BackfaceVisibility,
    visual_get_BorderMode,
    visual_put_BorderMode,
    visual_get_CenterPoint,
    visual_put_CenterPoint,
    visual_get_Clip,
    visual_put_Clip,
    visual_get_CompositeMode,
    visual_put_CompositeMode,
    visual_get_IsVisible,
    visual_put_IsVisible,
    visual_get_Offset,
    visual_put_Offset,
    visual_get_Opacity,
    visual_put_Opacity,
    visual_get_Orientation,
    visual_put_Orientation,
    visual_get_Parent,
    visual_get_RotationAngle,
    visual_put_RotationAngle,
    visual_get_RotationAngleInDegrees,
    visual_put_RotationAngleInDegrees,
    visual_get_RotationAxis,
    visual_put_RotationAxis,
    visual_get_Scale,
    visual_put_Scale,
    visual_get_Size,
    visual_put_Size,
    visual_get_TransformMatrix,
    visual_put_TransformMatrix
};

/* IContainerVisual methods */
static HRESULT WINAPI container_visual_QueryInterface(IContainerVisual *iface, REFIID iid, void **out)
{
    struct visual *impl = impl_from_IContainerVisual(iface);
    return visual_QueryInterface(&impl->IVisual_iface, iid, out);
}

static ULONG WINAPI container_visual_AddRef(IContainerVisual *iface)
{
    struct visual *impl = impl_from_IContainerVisual(iface);
    return visual_AddRef(&impl->IVisual_iface);
}

static ULONG WINAPI container_visual_Release(IContainerVisual *iface)
{
    struct visual *impl = impl_from_IContainerVisual(iface);
    return visual_Release(&impl->IVisual_iface);
}

static HRESULT WINAPI container_visual_GetIids(IContainerVisual *iface, ULONG *iidCount, IID **iids)
{
    return E_NOTIMPL;
}

static HRESULT WINAPI container_visual_GetRuntimeClassName(IContainerVisual *iface, HSTRING *className)
{
    return WindowsCreateString(L"Windows.UI.Composition.ContainerVisual",
                               wcslen(L"Windows.UI.Composition.ContainerVisual"), className);
}

static HRESULT WINAPI container_visual_GetTrustLevel(IContainerVisual *iface, TrustLevel *trustLevel)
{
    *trustLevel = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI container_visual_get_Children(IContainerVisual *iface, IVisualCollection **value)
{
    struct visual *impl = impl_from_IContainerVisual(iface);

    TRACE("iface %p, value %p.\n", iface, value);
    if (!value) return E_POINTER;

    if (!impl->children)
    {
        struct visual_collection *col = calloc(1, sizeof(*col));
        if (!col) return E_OUTOFMEMORY;
        col->IVisualCollection_iface.lpVtbl = &visual_collection_vtbl;
        col->ref = 1;
        impl->children = col;
    }

    IVisualCollection_AddRef(&impl->children->IVisualCollection_iface);
    *value = &impl->children->IVisualCollection_iface;
    return S_OK;
}

static const struct IContainerVisualVtbl container_visual_vtbl =
{
    container_visual_QueryInterface,
    container_visual_AddRef,
    container_visual_Release,
    container_visual_GetIids,
    container_visual_GetRuntimeClassName,
    container_visual_GetTrustLevel,
    container_visual_get_Children
};

/* ISpriteVisual methods */
static HRESULT WINAPI sprite_visual_QueryInterface(ISpriteVisual *iface, REFIID iid, void **out)
{
    struct visual *impl = impl_from_ISpriteVisual(iface);
    return visual_QueryInterface(&impl->IVisual_iface, iid, out);
}

static ULONG WINAPI sprite_visual_AddRef(ISpriteVisual *iface)
{
    struct visual *impl = impl_from_ISpriteVisual(iface);
    return visual_AddRef(&impl->IVisual_iface);
}

static ULONG WINAPI sprite_visual_Release(ISpriteVisual *iface)
{
    struct visual *impl = impl_from_ISpriteVisual(iface);
    return visual_Release(&impl->IVisual_iface);
}

static HRESULT WINAPI sprite_visual_GetIids(ISpriteVisual *iface, ULONG *iidCount, IID **iids)
{
    return E_NOTIMPL;
}

static HRESULT WINAPI sprite_visual_GetRuntimeClassName(ISpriteVisual *iface, HSTRING *className)
{
    return WindowsCreateString(L"Windows.UI.Composition.SpriteVisual",
                               wcslen(L"Windows.UI.Composition.SpriteVisual"), className);
}

static HRESULT WINAPI sprite_visual_GetTrustLevel(ISpriteVisual *iface, TrustLevel *trustLevel)
{
    *trustLevel = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI sprite_visual_get_Brush(ISpriteVisual *iface, ICompositionBrush **value)
{
    if (value) *value = NULL;
    return S_OK;
}

static HRESULT WINAPI sprite_visual_put_Brush(ISpriteVisual *iface, ICompositionBrush *value)
{
    return S_OK;
}

static const struct ISpriteVisualVtbl sprite_visual_vtbl =
{
    sprite_visual_QueryInterface,
    sprite_visual_AddRef,
    sprite_visual_Release,
    sprite_visual_GetIids,
    sprite_visual_GetRuntimeClassName,
    sprite_visual_GetTrustLevel,
    sprite_visual_get_Brush,
    sprite_visual_put_Brush
};

/* ICompositionObject methods */
static HRESULT WINAPI composition_object_QueryInterface(ICompositionObject *iface, REFIID iid, void **out)
{
    struct visual *impl = impl_from_ICompositionObject(iface);
    return visual_QueryInterface(&impl->IVisual_iface, iid, out);
}

static ULONG WINAPI composition_object_AddRef(ICompositionObject *iface)
{
    struct visual *impl = impl_from_ICompositionObject(iface);
    return visual_AddRef(&impl->IVisual_iface);
}

static ULONG WINAPI composition_object_Release(ICompositionObject *iface)
{
    struct visual *impl = impl_from_ICompositionObject(iface);
    return visual_Release(&impl->IVisual_iface);
}

static HRESULT WINAPI composition_object_GetIids(ICompositionObject *iface, ULONG *iidCount, IID **iids)
{
    return E_NOTIMPL;
}

static HRESULT WINAPI composition_object_GetRuntimeClassName(ICompositionObject *iface, HSTRING *className)
{
    return WindowsCreateString(L"Windows.UI.Composition.CompositionObject",
                               wcslen(L"Windows.UI.Composition.CompositionObject"), className);
}

static HRESULT WINAPI composition_object_GetTrustLevel(ICompositionObject *iface, TrustLevel *trustLevel)
{
    *trustLevel = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI composition_object_get_Compositor(ICompositionObject *iface, ICompositor **value)
{
    struct visual *impl = impl_from_ICompositionObject(iface);
    TRACE("iface %p, value %p.\n", iface, value);
    if (!value) return E_POINTER;
    if (impl->compositor)
    {
        ICompositor_AddRef((ICompositor *)impl->compositor);
        *value = (ICompositor *)impl->compositor;
    }
    else
    {
        *value = NULL;
    }
    return S_OK;
}

static HRESULT WINAPI composition_object_get_Dispatcher(ICompositionObject *iface, struct __x_ABI_CWindows_CUI_CCore_CICoreDispatcher **value)
{
    if (value) *value = NULL;
    return S_OK;
}

static HRESULT WINAPI composition_object_get_Properties(ICompositionObject *iface, ICompositionPropertySet **value)
{
    TRACE("iface %p, value %p.\n", iface, value);
    return create_property_set(value);
}

static HRESULT WINAPI composition_object_StartAnimation(ICompositionObject *iface, HSTRING name, ICompositionAnimation *animation)
{
    TRACE("iface %p, name %s, animation %p.\n", iface, debugstr_hstring(name), animation);
    return S_OK;
}

static HRESULT WINAPI composition_object_StopAnimation(ICompositionObject *iface, HSTRING name)
{
    TRACE("iface %p, name %s.\n", iface, debugstr_hstring(name));
    return S_OK;
}

static const struct ICompositionObjectVtbl composition_object_vtbl =
{
    composition_object_QueryInterface,
    composition_object_AddRef,
    composition_object_Release,
    composition_object_GetIids,
    composition_object_GetRuntimeClassName,
    composition_object_GetTrustLevel,
    composition_object_get_Compositor,
    composition_object_get_Dispatcher,
    composition_object_get_Properties,
    composition_object_StartAnimation,
    composition_object_StopAnimation
};

static HRESULT create_visual(struct compositor *compositor, IVisual **out)
{
    struct visual *v;

    if (!out) return E_POINTER;
    *out = NULL;

    v = calloc(1, sizeof(*v));
    if (!v) return E_OUTOFMEMORY;

    v->IVisual_iface.lpVtbl = &visual_vtbl;
    v->IContainerVisual_iface.lpVtbl = &container_visual_vtbl;
    v->ISpriteVisual_iface.lpVtbl = &sprite_visual_vtbl;
    v->ICompositionObject_iface.lpVtbl = &composition_object_vtbl;
    v->ref = 1;
    v->compositor = compositor;
    v->is_visible = TRUE;
    v->opacity = 1.0f;

    *out = &v->IVisual_iface;
    return S_OK;
}

/* ========================================================================= */
/* --- ColorBrush Implementation --- */
/* ========================================================================= */

struct color_brush
{
    ICompositionColorBrush ICompositionColorBrush_iface;
    ICompositionBrush ICompositionBrush_iface;
    LONG ref;
    Color color;
};

static inline struct color_brush *impl_from_ICompositionColorBrush(ICompositionColorBrush *iface)
{
    return CONTAINING_RECORD(iface, struct color_brush, ICompositionColorBrush_iface);
}

static inline struct color_brush *impl_from_ICompositionBrush(ICompositionBrush *iface)
{
    return CONTAINING_RECORD(iface, struct color_brush, ICompositionBrush_iface);
}

static HRESULT WINAPI color_brush_QueryInterface(ICompositionColorBrush *iface, REFIID iid, void **out)
{
    struct color_brush *impl = impl_from_ICompositionColorBrush(iface);

    if (!out) return E_POINTER;

    if (IsEqualGUID(iid, &IID_IUnknown) ||
        IsEqualGUID(iid, &IID_IInspectable) ||
        IsEqualGUID(iid, &IID_IAgileObject) ||
        IsEqualGUID(iid, &IID_ICompositionColorBrush))
    {
        *out = &impl->ICompositionColorBrush_iface;
        ICompositionColorBrush_AddRef(iface);
        return S_OK;
    }
    if (IsEqualGUID(iid, &IID_ICompositionBrush))
    {
        *out = &impl->ICompositionBrush_iface;
        ICompositionColorBrush_AddRef(iface);
        return S_OK;
    }

    WARN("unsupported iid %s.\n", debugstr_guid(iid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI color_brush_AddRef(ICompositionColorBrush *iface)
{
    struct color_brush *impl = impl_from_ICompositionColorBrush(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI color_brush_Release(ICompositionColorBrush *iface)
{
    struct color_brush *impl = impl_from_ICompositionColorBrush(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI color_brush_GetIids(ICompositionColorBrush *iface, ULONG *iidCount, IID **iids)
{
    return E_NOTIMPL;
}

static HRESULT WINAPI color_brush_GetRuntimeClassName(ICompositionColorBrush *iface, HSTRING *className)
{
    return WindowsCreateString(L"Windows.UI.Composition.CompositionColorBrush",
                               wcslen(L"Windows.UI.Composition.CompositionColorBrush"), className);
}

static HRESULT WINAPI color_brush_GetTrustLevel(ICompositionColorBrush *iface, TrustLevel *trustLevel)
{
    *trustLevel = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI color_brush_get_Color(ICompositionColorBrush *iface, Color *value)
{
    struct color_brush *impl = impl_from_ICompositionColorBrush(iface);
    if (value) *value = impl->color;
    return S_OK;
}

static HRESULT WINAPI color_brush_put_Color(ICompositionColorBrush *iface, Color value)
{
    struct color_brush *impl = impl_from_ICompositionColorBrush(iface);
    impl->color = value;
    return S_OK;
}

static const struct ICompositionColorBrushVtbl color_brush_vtbl =
{
    color_brush_QueryInterface,
    color_brush_AddRef,
    color_brush_Release,
    color_brush_GetIids,
    color_brush_GetRuntimeClassName,
    color_brush_GetTrustLevel,
    color_brush_get_Color,
    color_brush_put_Color
};

static HRESULT create_color_brush(Color color, ICompositionColorBrush **out)
{
    struct color_brush *brush;

    if (!out) return E_POINTER;
    *out = NULL;

    brush = calloc(1, sizeof(*brush));
    if (!brush) return E_OUTOFMEMORY;

    brush->ICompositionColorBrush_iface.lpVtbl = &color_brush_vtbl;
    brush->ref = 1;
    brush->color = color;

    *out = &brush->ICompositionColorBrush_iface;
    return S_OK;
}

/* ========================================================================= */
/* --- PropertySet Implementation --- */
/* ========================================================================= */

struct property_set
{
    ICompositionPropertySet ICompositionPropertySet_iface;
    LONG ref;
};

static inline struct property_set *impl_from_ICompositionPropertySet(ICompositionPropertySet *iface)
{
    return CONTAINING_RECORD(iface, struct property_set, ICompositionPropertySet_iface);
}

static HRESULT WINAPI property_set_QueryInterface(ICompositionPropertySet *iface, REFIID iid, void **out)
{
    struct property_set *impl = impl_from_ICompositionPropertySet(iface);

    if (!out) return E_POINTER;

    if (IsEqualGUID(iid, &IID_IUnknown) ||
        IsEqualGUID(iid, &IID_IInspectable) ||
        IsEqualGUID(iid, &IID_IAgileObject) ||
        IsEqualGUID(iid, &IID_ICompositionPropertySet))
    {
        *out = &impl->ICompositionPropertySet_iface;
        ICompositionPropertySet_AddRef(iface);
        return S_OK;
    }

    WARN("unsupported iid %s.\n", debugstr_guid(iid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI property_set_AddRef(ICompositionPropertySet *iface)
{
    struct property_set *impl = impl_from_ICompositionPropertySet(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI property_set_Release(ICompositionPropertySet *iface)
{
    struct property_set *impl = impl_from_ICompositionPropertySet(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI property_set_GetIids(ICompositionPropertySet *iface, ULONG *iidCount, IID **iids)
{
    return E_NOTIMPL;
}

static HRESULT WINAPI property_set_GetRuntimeClassName(ICompositionPropertySet *iface, HSTRING *className)
{
    return WindowsCreateString(L"Windows.UI.Composition.CompositionPropertySet",
                               wcslen(L"Windows.UI.Composition.CompositionPropertySet"), className);
}

static HRESULT WINAPI property_set_GetTrustLevel(ICompositionPropertySet *iface, TrustLevel *trustLevel)
{
    *trustLevel = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI property_set_InsertColor(ICompositionPropertySet *iface, HSTRING name, Color value)
{
    return S_OK;
}

static HRESULT WINAPI property_set_InsertMatrix3x2(ICompositionPropertySet *iface, HSTRING name, Matrix3x2 value)
{
    return S_OK;
}

static HRESULT WINAPI property_set_InsertMatrix4x4(ICompositionPropertySet *iface, HSTRING name, Matrix4x4 value)
{
    return S_OK;
}

static HRESULT WINAPI property_set_InsertQuaternion(ICompositionPropertySet *iface, HSTRING name, Quaternion value)
{
    return S_OK;
}

static HRESULT WINAPI property_set_InsertScalar(ICompositionPropertySet *iface, HSTRING name, FLOAT value)
{
    return S_OK;
}

static HRESULT WINAPI property_set_InsertVector2(ICompositionPropertySet *iface, HSTRING name, Vector2 value)
{
    return S_OK;
}

static HRESULT WINAPI property_set_InsertVector3(ICompositionPropertySet *iface, HSTRING name, Vector3 value)
{
    return S_OK;
}

static HRESULT WINAPI property_set_InsertVector4(ICompositionPropertySet *iface, HSTRING name, Vector4 value)
{
    return S_OK;
}

static HRESULT WINAPI property_set_TryGetColor(ICompositionPropertySet *iface, HSTRING name, Color *value, CompositionGetValueStatus *result)
{
    if (result) *result = CompositionGetValueStatus_NotFound;
    return S_OK;
}

static HRESULT WINAPI property_set_TryGetMatrix3x2(ICompositionPropertySet *iface, HSTRING name, Matrix3x2 *value, CompositionGetValueStatus *result)
{
    if (result) *result = CompositionGetValueStatus_NotFound;
    return S_OK;
}

static HRESULT WINAPI property_set_TryGetMatrix4x4(ICompositionPropertySet *iface, HSTRING name, Matrix4x4 *value, CompositionGetValueStatus *result)
{
    if (result) *result = CompositionGetValueStatus_NotFound;
    return S_OK;
}

static HRESULT WINAPI property_set_TryGetQuaternion(ICompositionPropertySet *iface, HSTRING name, Quaternion *value, CompositionGetValueStatus *result)
{
    if (result) *result = CompositionGetValueStatus_NotFound;
    return S_OK;
}

static HRESULT WINAPI property_set_TryGetScalar(ICompositionPropertySet *iface, HSTRING name, FLOAT *value, CompositionGetValueStatus *result)
{
    if (result) *result = CompositionGetValueStatus_NotFound;
    return S_OK;
}

static HRESULT WINAPI property_set_TryGetVector2(ICompositionPropertySet *iface, HSTRING name, Vector2 *value, CompositionGetValueStatus *result)
{
    if (result) *result = CompositionGetValueStatus_NotFound;
    return S_OK;
}

static HRESULT WINAPI property_set_TryGetVector3(ICompositionPropertySet *iface, HSTRING name, Vector3 *value, CompositionGetValueStatus *result)
{
    if (result) *result = CompositionGetValueStatus_NotFound;
    return S_OK;
}

static HRESULT WINAPI property_set_TryGetVector4(ICompositionPropertySet *iface, HSTRING name, Vector4 *value, CompositionGetValueStatus *result)
{
    if (result) *result = CompositionGetValueStatus_NotFound;
    return S_OK;
}

static const struct ICompositionPropertySetVtbl property_set_vtbl =
{
    property_set_QueryInterface,
    property_set_AddRef,
    property_set_Release,
    property_set_GetIids,
    property_set_GetRuntimeClassName,
    property_set_GetTrustLevel,
    property_set_InsertColor,
    property_set_InsertMatrix3x2,
    property_set_InsertMatrix4x4,
    property_set_InsertQuaternion,
    property_set_InsertScalar,
    property_set_InsertVector2,
    property_set_InsertVector3,
    property_set_InsertVector4,
    property_set_TryGetColor,
    property_set_TryGetMatrix3x2,
    property_set_TryGetMatrix4x4,
    property_set_TryGetQuaternion,
    property_set_TryGetScalar,
    property_set_TryGetVector2,
    property_set_TryGetVector3,
    property_set_TryGetVector4
};

static HRESULT create_property_set(ICompositionPropertySet **out)
{
    struct property_set *ps;

    if (!out) return E_POINTER;
    *out = NULL;

    ps = calloc(1, sizeof(*ps));
    if (!ps) return E_OUTOFMEMORY;

    ps->ICompositionPropertySet_iface.lpVtbl = &property_set_vtbl;
    ps->ref = 1;

    *out = &ps->ICompositionPropertySet_iface;
    return S_OK;
}

/* ========================================================================= */
/* --- CompositionBatch Implementation --- */
/* ========================================================================= */

struct scoped_batch
{
    ICompositionScopedBatch ICompositionScopedBatch_iface;
    LONG ref;
};

static inline struct scoped_batch *impl_from_ICompositionScopedBatch(ICompositionScopedBatch *iface)
{
    return CONTAINING_RECORD(iface, struct scoped_batch, ICompositionScopedBatch_iface);
}

static HRESULT WINAPI scoped_batch_QueryInterface(ICompositionScopedBatch *iface, REFIID iid, void **out)
{
    struct scoped_batch *impl = impl_from_ICompositionScopedBatch(iface);

    if (!out) return E_POINTER;

    if (IsEqualGUID(iid, &IID_IUnknown) ||
        IsEqualGUID(iid, &IID_IInspectable) ||
        IsEqualGUID(iid, &IID_IAgileObject) ||
        IsEqualGUID(iid, &IID_ICompositionScopedBatch))
    {
        *out = &impl->ICompositionScopedBatch_iface;
        ICompositionScopedBatch_AddRef(iface);
        return S_OK;
    }

    WARN("unsupported iid %s.\n", debugstr_guid(iid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI scoped_batch_AddRef(ICompositionScopedBatch *iface)
{
    struct scoped_batch *impl = impl_from_ICompositionScopedBatch(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI scoped_batch_Release(ICompositionScopedBatch *iface)
{
    struct scoped_batch *impl = impl_from_ICompositionScopedBatch(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI scoped_batch_GetIids(ICompositionScopedBatch *iface, ULONG *iidCount, IID **iids)
{
    return E_NOTIMPL;
}

static HRESULT WINAPI scoped_batch_GetRuntimeClassName(ICompositionScopedBatch *iface, HSTRING *className)
{
    return WindowsCreateString(L"Windows.UI.Composition.CompositionScopedBatch",
                               wcslen(L"Windows.UI.Composition.CompositionScopedBatch"), className);
}

static HRESULT WINAPI scoped_batch_GetTrustLevel(ICompositionScopedBatch *iface, TrustLevel *trustLevel)
{
    *trustLevel = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI scoped_batch_get_IsActive(ICompositionScopedBatch *iface, boolean *value)
{
    if (value) *value = TRUE;
    return S_OK;
}

static HRESULT WINAPI scoped_batch_get_IsEnded(ICompositionScopedBatch *iface, boolean *value)
{
    if (value) *value = FALSE;
    return S_OK;
}

static HRESULT WINAPI scoped_batch_End(ICompositionScopedBatch *iface)
{
    return S_OK;
}

static HRESULT WINAPI scoped_batch_Resume(ICompositionScopedBatch *iface)
{
    return S_OK;
}

static HRESULT WINAPI scoped_batch_Suspend(ICompositionScopedBatch *iface)
{
    return S_OK;
}

static HRESULT WINAPI scoped_batch_add_Completed(ICompositionScopedBatch *iface,
    __FITypedEventHandler_2_IInspectable_Windows__CUI__CComposition__CCompositionBatchCompletedEventArgs *handler,
    EventRegistrationToken *token)
{
    if (token) token->value = 1;
    return S_OK;
}

static HRESULT WINAPI scoped_batch_remove_Completed(ICompositionScopedBatch *iface, EventRegistrationToken token)
{
    return S_OK;
}

static const struct ICompositionScopedBatchVtbl scoped_batch_vtbl =
{
    scoped_batch_QueryInterface,
    scoped_batch_AddRef,
    scoped_batch_Release,
    scoped_batch_GetIids,
    scoped_batch_GetRuntimeClassName,
    scoped_batch_GetTrustLevel,
    scoped_batch_get_IsActive,
    scoped_batch_get_IsEnded,
    scoped_batch_End,
    scoped_batch_Resume,
    scoped_batch_Suspend,
    scoped_batch_add_Completed,
    scoped_batch_remove_Completed
};

static HRESULT create_scoped_batch(CompositionBatchTypes type, ICompositionScopedBatch **out)
{
    struct scoped_batch *b;

    if (!out) return E_POINTER;
    *out = NULL;

    b = calloc(1, sizeof(*b));
    if (!b) return E_OUTOFMEMORY;

    b->ICompositionScopedBatch_iface.lpVtbl = &scoped_batch_vtbl;
    b->ref = 1;

    *out = &b->ICompositionScopedBatch_iface;
    return S_OK;
}

struct commit_batch
{
    ICompositionCommitBatch ICompositionCommitBatch_iface;
    LONG ref;
};

static inline struct commit_batch *impl_from_ICompositionCommitBatch(ICompositionCommitBatch *iface)
{
    return CONTAINING_RECORD(iface, struct commit_batch, ICompositionCommitBatch_iface);
}

static HRESULT WINAPI commit_batch_QueryInterface(ICompositionCommitBatch *iface, REFIID iid, void **out)
{
    struct commit_batch *impl = impl_from_ICompositionCommitBatch(iface);

    if (!out) return E_POINTER;

    if (IsEqualGUID(iid, &IID_IUnknown) ||
        IsEqualGUID(iid, &IID_IInspectable) ||
        IsEqualGUID(iid, &IID_IAgileObject) ||
        IsEqualGUID(iid, &IID_ICompositionCommitBatch))
    {
        *out = &impl->ICompositionCommitBatch_iface;
        ICompositionCommitBatch_AddRef(iface);
        return S_OK;
    }

    WARN("unsupported iid %s.\n", debugstr_guid(iid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI commit_batch_AddRef(ICompositionCommitBatch *iface)
{
    struct commit_batch *impl = impl_from_ICompositionCommitBatch(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI commit_batch_Release(ICompositionCommitBatch *iface)
{
    struct commit_batch *impl = impl_from_ICompositionCommitBatch(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI commit_batch_GetIids(ICompositionCommitBatch *iface, ULONG *iidCount, IID **iids)
{
    return E_NOTIMPL;
}

static HRESULT WINAPI commit_batch_GetRuntimeClassName(ICompositionCommitBatch *iface, HSTRING *className)
{
    return WindowsCreateString(L"Windows.UI.Composition.CompositionCommitBatch",
                               wcslen(L"Windows.UI.Composition.CompositionCommitBatch"), className);
}

static HRESULT WINAPI commit_batch_GetTrustLevel(ICompositionCommitBatch *iface, TrustLevel *trustLevel)
{
    *trustLevel = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI commit_batch_get_IsActive(ICompositionCommitBatch *iface, boolean *value)
{
    if (value) *value = TRUE;
    return S_OK;
}

static HRESULT WINAPI commit_batch_get_IsEnded(ICompositionCommitBatch *iface, boolean *value)
{
    if (value) *value = FALSE;
    return S_OK;
}

static HRESULT WINAPI commit_batch_add_Completed(ICompositionCommitBatch *iface,
    __FITypedEventHandler_2_IInspectable_Windows__CUI__CComposition__CCompositionBatchCompletedEventArgs *handler,
    EventRegistrationToken *token)
{
    if (token) token->value = 1;
    return S_OK;
}

static HRESULT WINAPI commit_batch_remove_Completed(ICompositionCommitBatch *iface, EventRegistrationToken token)
{
    return S_OK;
}

static const struct ICompositionCommitBatchVtbl commit_batch_vtbl =
{
    commit_batch_QueryInterface,
    commit_batch_AddRef,
    commit_batch_Release,
    commit_batch_GetIids,
    commit_batch_GetRuntimeClassName,
    commit_batch_GetTrustLevel,
    commit_batch_get_IsActive,
    commit_batch_get_IsEnded,
    commit_batch_add_Completed,
    commit_batch_remove_Completed
};

static HRESULT create_commit_batch(CompositionBatchTypes type, ICompositionCommitBatch **out)
{
    struct commit_batch *b;

    if (!out) return E_POINTER;
    *out = NULL;

    b = calloc(1, sizeof(*b));
    if (!b) return E_OUTOFMEMORY;

    b->ICompositionCommitBatch_iface.lpVtbl = &commit_batch_vtbl;
    b->ref = 1;

    *out = &b->ICompositionCommitBatch_iface;
    return S_OK;
}

/* ========================================================================= */
/* --- Compositor Implementation --- */
/* ========================================================================= */

struct compositor
{
    ICompositor ICompositor_iface;
    ICompositorDesktopInterop ICompositorDesktopInterop_iface;
    ICompositorInterop ICompositorInterop_iface;
    LONG ref;
};

static inline struct compositor *impl_from_ICompositor(ICompositor *iface)
{
    return CONTAINING_RECORD(iface, struct compositor, ICompositor_iface);
}

static inline struct compositor *impl_from_ICompositorDesktopInterop(ICompositorDesktopInterop *iface)
{
    return CONTAINING_RECORD(iface, struct compositor, ICompositorDesktopInterop_iface);
}

static inline struct compositor *impl_from_ICompositorInterop(ICompositorInterop *iface)
{
    return CONTAINING_RECORD(iface, struct compositor, ICompositorInterop_iface);
}

static HRESULT WINAPI compositor_QueryInterface(ICompositor *iface, REFIID iid, void **out)
{
    struct compositor *impl = impl_from_ICompositor(iface);

    TRACE("iface %p, iid %s, out %p.\n", iface, debugstr_guid(iid), out);

    if (!out) return E_POINTER;

    if (IsEqualGUID(iid, &IID_IUnknown) ||
        IsEqualGUID(iid, &IID_IInspectable) ||
        IsEqualGUID(iid, &IID_IAgileObject) ||
        IsEqualGUID(iid, &IID_ICompositor) ||
        IsEqualGUID(iid, &IID_IClosable))
    {
        *out = &impl->ICompositor_iface;
        ICompositor_AddRef(iface);
        return S_OK;
    }
    if (IsEqualGUID(iid, &IID_ICompositorDesktopInterop))
    {
        *out = &impl->ICompositorDesktopInterop_iface;
        ICompositor_AddRef(iface);
        return S_OK;
    }
    if (IsEqualGUID(iid, &IID_ICompositorInterop))
    {
        *out = &impl->ICompositorInterop_iface;
        ICompositor_AddRef(iface);
        return S_OK;
    }

    WARN("unsupported iid %s.\n", debugstr_guid(iid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI compositor_AddRef(ICompositor *iface)
{
    struct compositor *impl = impl_from_ICompositor(iface);
    ULONG ref = InterlockedIncrement(&impl->ref);
    TRACE("iface %p, ref %lu.\n", iface, ref);
    return ref;
}

static ULONG WINAPI compositor_Release(ICompositor *iface)
{
    struct compositor *impl = impl_from_ICompositor(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    TRACE("iface %p, ref %lu.\n", iface, ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI compositor_GetIids(ICompositor *iface, ULONG *iidCount, IID **iids)
{
    FIXME("iface %p, iidCount %p, iids %p stub!\n", iface, iidCount, iids);
    return E_NOTIMPL;
}

static HRESULT WINAPI compositor_GetRuntimeClassName(ICompositor *iface, HSTRING *className)
{
    TRACE("iface %p, className %p.\n", iface, className);
    return WindowsCreateString(L"Windows.UI.Composition.Compositor",
                               wcslen(L"Windows.UI.Composition.Compositor"), className);
}

static HRESULT WINAPI compositor_GetTrustLevel(ICompositor *iface, TrustLevel *trustLevel)
{
    TRACE("iface %p, trustLevel %p.\n", iface, trustLevel);
    *trustLevel = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI compositor_CreateColorKeyFrameAnimation(ICompositor *iface, IColorKeyFrameAnimation **result)
{
    FIXME("iface %p, result %p stub!\n", iface, result);
    if (result) *result = NULL;
    return S_OK;
}

static HRESULT WINAPI compositor_CreateColorBrush(ICompositor *iface, ICompositionColorBrush **result)
{
    Color c = {255, 255, 255, 255};
    TRACE("iface %p, result %p.\n", iface, result);
    return create_color_brush(c, result);
}

static HRESULT WINAPI compositor_CreateColorBrushWithColor(ICompositor *iface, Color color, ICompositionColorBrush **result)
{
    TRACE("iface %p, color (%u,%u,%u,%u), result %p.\n", iface, color.A, color.R, color.G, color.B, result);
    return create_color_brush(color, result);
}

static HRESULT WINAPI compositor_CreateContainerVisual(ICompositor *iface, IContainerVisual **result)
{
    struct compositor *impl = impl_from_ICompositor(iface);
    IVisual *v = NULL;
    HRESULT hr;

    TRACE("iface %p, result %p.\n", iface, result);
    if (!result) return E_POINTER;

    hr = create_visual(impl, &v);
    if (SUCCEEDED(hr))
    {
        hr = IVisual_QueryInterface(v, &IID_IContainerVisual, (void **)result);
        IVisual_Release(v);
    }
    return hr;
}

static HRESULT WINAPI compositor_CreateCubicBezierEasingFunction(ICompositor *iface,
    Vector2 point1, Vector2 point2, ICubicBezierEasingFunction **result)
{
    FIXME("stub!\n");
    if (result) *result = NULL;
    return S_OK;
}

static HRESULT WINAPI compositor_CreateEffectFactory(ICompositor *iface,
    struct __x_ABI_CWindows_CGraphics_CEffects_CIGraphicsEffect *effect, ICompositionEffectFactory **result)
{
    FIXME("stub!\n");
    if (result) *result = NULL;
    return S_OK;
}

static HRESULT WINAPI compositor_CreateEffectFactoryWithProperties(ICompositor *iface,
    struct __x_ABI_CWindows_CGraphics_CEffects_CIGraphicsEffect *effect,
    __FIIterable_1_HSTRING *animatable, ICompositionEffectFactory **result)
{
    FIXME("stub!\n");
    if (result) *result = NULL;
    return S_OK;
}

static HRESULT WINAPI compositor_CreateExpressionAnimation(ICompositor *iface, IExpressionAnimation **result)
{
    FIXME("stub!\n");
    if (result) *result = NULL;
    return S_OK;
}

static HRESULT WINAPI compositor_CreateExpressionAnimationWithExpression(ICompositor *iface,
    HSTRING expression, IExpressionAnimation **result)
{
    FIXME("stub!\n");
    if (result) *result = NULL;
    return S_OK;
}

static HRESULT WINAPI compositor_CreateInsetClip(ICompositor *iface, IInsetClip **result)
{
    FIXME("stub!\n");
    if (result) *result = NULL;
    return S_OK;
}

static HRESULT WINAPI compositor_CreateInsetClipWithInsets(ICompositor *iface,
    FLOAT left, FLOAT top, FLOAT right, FLOAT bottom, IInsetClip **result)
{
    FIXME("stub!\n");
    if (result) *result = NULL;
    return S_OK;
}

static HRESULT WINAPI compositor_CreateLinearEasingFunction(ICompositor *iface, ILinearEasingFunction **result)
{
    FIXME("stub!\n");
    if (result) *result = NULL;
    return S_OK;
}

static HRESULT WINAPI compositor_CreatePropertySet(ICompositor *iface, ICompositionPropertySet **result)
{
    TRACE("iface %p, result %p.\n", iface, result);
    return create_property_set(result);
}

static HRESULT WINAPI compositor_CreateQuaternionKeyFrameAnimation(ICompositor *iface, IQuaternionKeyFrameAnimation **result)
{
    FIXME("stub!\n");
    if (result) *result = NULL;
    return S_OK;
}

static HRESULT WINAPI compositor_CreateScalarKeyFrameAnimation(ICompositor *iface, IScalarKeyFrameAnimation **result)
{
    FIXME("stub!\n");
    if (result) *result = NULL;
    return S_OK;
}

static HRESULT WINAPI compositor_CreateScopedBatch(ICompositor *iface, CompositionBatchTypes type, ICompositionScopedBatch **result)
{
    TRACE("iface %p, type %d, result %p.\n", iface, type, result);
    return create_scoped_batch(type, result);
}

static HRESULT WINAPI compositor_CreateSpriteVisual(ICompositor *iface, ISpriteVisual **result)
{
    struct compositor *impl = impl_from_ICompositor(iface);
    IVisual *v = NULL;
    HRESULT hr;

    TRACE("iface %p, result %p.\n", iface, result);
    if (!result) return E_POINTER;

    hr = create_visual(impl, &v);
    if (SUCCEEDED(hr))
    {
        hr = IVisual_QueryInterface(v, &IID_ISpriteVisual, (void **)result);
        IVisual_Release(v);
    }
    return hr;
}

static HRESULT WINAPI compositor_CreateSurfaceBrush(ICompositor *iface, ICompositionSurfaceBrush **result)
{
    FIXME("stub!\n");
    if (result) *result = NULL;
    return S_OK;
}

static HRESULT WINAPI compositor_CreateSurfaceBrushWithSurface(ICompositor *iface,
    ICompositionSurface *surface, ICompositionSurfaceBrush **result)
{
    FIXME("stub!\n");
    if (result) *result = NULL;
    return S_OK;
}

static HRESULT WINAPI compositor_CreateTargetForCurrentView(ICompositor *iface, ICompositionTarget **result)
{
    FIXME("stub!\n");
    if (result) *result = NULL;
    return S_OK;
}

static HRESULT WINAPI compositor_CreateVector2KeyFrameAnimation(ICompositor *iface, IVector2KeyFrameAnimation **result)
{
    FIXME("stub!\n");
    if (result) *result = NULL;
    return S_OK;
}

static HRESULT WINAPI compositor_CreateVector3KeyFrameAnimation(ICompositor *iface, IVector3KeyFrameAnimation **result)
{
    FIXME("stub!\n");
    if (result) *result = NULL;
    return S_OK;
}

static HRESULT WINAPI compositor_CreateVector4KeyFrameAnimation(ICompositor *iface, IVector4KeyFrameAnimation **result)
{
    FIXME("stub!\n");
    if (result) *result = NULL;
    return S_OK;
}

static HRESULT WINAPI compositor_GetCommitBatch(ICompositor *iface, CompositionBatchTypes type, ICompositionCommitBatch **result)
{
    TRACE("iface %p, type %d, result %p.\n", iface, type, result);
    return create_commit_batch(type, result);
}

static const struct ICompositorVtbl compositor_vtbl =
{
    compositor_QueryInterface,
    compositor_AddRef,
    compositor_Release,
    compositor_GetIids,
    compositor_GetRuntimeClassName,
    compositor_GetTrustLevel,
    compositor_CreateColorKeyFrameAnimation,
    compositor_CreateColorBrush,
    compositor_CreateColorBrushWithColor,
    compositor_CreateContainerVisual,
    compositor_CreateCubicBezierEasingFunction,
    compositor_CreateEffectFactory,
    compositor_CreateEffectFactoryWithProperties,
    compositor_CreateExpressionAnimation,
    compositor_CreateExpressionAnimationWithExpression,
    compositor_CreateInsetClip,
    compositor_CreateInsetClipWithInsets,
    compositor_CreateLinearEasingFunction,
    compositor_CreatePropertySet,
    compositor_CreateQuaternionKeyFrameAnimation,
    compositor_CreateScalarKeyFrameAnimation,
    compositor_CreateScopedBatch,
    compositor_CreateSpriteVisual,
    compositor_CreateSurfaceBrush,
    compositor_CreateSurfaceBrushWithSurface,
    compositor_CreateTargetForCurrentView,
    compositor_CreateVector2KeyFrameAnimation,
    compositor_CreateVector3KeyFrameAnimation,
    compositor_CreateVector4KeyFrameAnimation,
    compositor_GetCommitBatch
};

/* ICompositorDesktopInterop methods */
static HRESULT WINAPI compositor_desktop_interop_QueryInterface(ICompositorDesktopInterop *iface, REFIID iid, void **out)
{
    struct compositor *impl = impl_from_ICompositorDesktopInterop(iface);
    return compositor_QueryInterface(&impl->ICompositor_iface, iid, out);
}

static ULONG WINAPI compositor_desktop_interop_AddRef(ICompositorDesktopInterop *iface)
{
    struct compositor *impl = impl_from_ICompositorDesktopInterop(iface);
    return compositor_AddRef(&impl->ICompositor_iface);
}

static ULONG WINAPI compositor_desktop_interop_Release(ICompositorDesktopInterop *iface)
{
    struct compositor *impl = impl_from_ICompositorDesktopInterop(iface);
    return compositor_Release(&impl->ICompositor_iface);
}

static HRESULT WINAPI compositor_desktop_interop_CreateDesktopWindowTarget(
    ICompositorDesktopInterop *iface, HWND hwndTarget, BOOL isTopmost, ICompositionTarget **result)
{
    struct desktop_window_target *target;

    TRACE("iface %p, hwndTarget %p, isTopmost %d, result %p.\n", iface, hwndTarget, isTopmost, result);

    if (!result) return E_POINTER;
    *result = NULL;

    target = calloc(1, sizeof(*target));
    if (!target) return E_OUTOFMEMORY;

    target->ICompositionTarget_iface.lpVtbl = &desktop_window_target_vtbl;
    target->IDesktopWindowTarget_iface.lpVtbl = &desktop_window_target_interop_vtbl;
    target->ref = 1;
    target->hwnd = hwndTarget;
    target->is_topmost = isTopmost;

    *result = &target->ICompositionTarget_iface;
    TRACE("Created DesktopWindowTarget %p for hwnd %p\n", target, hwndTarget);
    return S_OK;
}

static const struct ICompositorDesktopInteropVtbl compositor_desktop_interop_vtbl =
{
    compositor_desktop_interop_QueryInterface,
    compositor_desktop_interop_AddRef,
    compositor_desktop_interop_Release,
    compositor_desktop_interop_CreateDesktopWindowTarget
};

/* ICompositorInterop methods */
static HRESULT WINAPI compositor_interop_QueryInterface(ICompositorInterop *iface, REFIID iid, void **out)
{
    struct compositor *impl = impl_from_ICompositorInterop(iface);
    return compositor_QueryInterface(&impl->ICompositor_iface, iid, out);
}

static ULONG WINAPI compositor_interop_AddRef(ICompositorInterop *iface)
{
    struct compositor *impl = impl_from_ICompositorInterop(iface);
    return compositor_AddRef(&impl->ICompositor_iface);
}

static ULONG WINAPI compositor_interop_Release(ICompositorInterop *iface)
{
    struct compositor *impl = impl_from_ICompositorInterop(iface);
    return compositor_Release(&impl->ICompositor_iface);
}

static HRESULT WINAPI compositor_interop_CreateCompositionSurfaceForHandle(
    ICompositorInterop *iface, HANDLE swapchain, ICompositionSurface **result)
{
    FIXME("stub!\n");
    if (result) *result = NULL;
    return S_OK;
}

static HRESULT WINAPI compositor_interop_CreateCompositionSurfaceForSwapChain(
    ICompositorInterop *iface, IUnknown *swapchain, ICompositionSurface **result)
{
    FIXME("stub!\n");
    if (result) *result = NULL;
    return S_OK;
}

static HRESULT WINAPI compositor_interop_CreateGraphicsDevice(
    ICompositorInterop *iface, IUnknown *device, ICompositionGraphicsDevice **result)
{
    FIXME("stub!\n");
    if (result) *result = NULL;
    return S_OK;
}

static const struct ICompositorInteropVtbl compositor_interop_vtbl =
{
    compositor_interop_QueryInterface,
    compositor_interop_AddRef,
    compositor_interop_Release,
    compositor_interop_CreateCompositionSurfaceForHandle,
    compositor_interop_CreateCompositionSurfaceForSwapChain,
    compositor_interop_CreateGraphicsDevice
};

/* ========================================================================= */
/* --- Activation Factory Implementation --- */
/* ========================================================================= */

struct compositor_factory
{
    IActivationFactory IActivationFactory_iface;
    LONG ref;
};

static inline struct compositor_factory *impl_from_IActivationFactory(IActivationFactory *iface)
{
    return CONTAINING_RECORD(iface, struct compositor_factory, IActivationFactory_iface);
}

static HRESULT WINAPI compositor_factory_QueryInterface(IActivationFactory *iface, REFIID iid, void **out)
{
    struct compositor_factory *impl = impl_from_IActivationFactory(iface);

    TRACE("iface %p, iid %s, out %p.\n", iface, debugstr_guid(iid), out);

    if (!out) return E_POINTER;

    if (IsEqualGUID(iid, &IID_IUnknown) ||
        IsEqualGUID(iid, &IID_IInspectable) ||
        IsEqualGUID(iid, &IID_IAgileObject) ||
        IsEqualGUID(iid, &IID_IActivationFactory))
    {
        *out = &impl->IActivationFactory_iface;
        IActivationFactory_AddRef(iface);
        return S_OK;
    }

    WARN("unsupported iid %s.\n", debugstr_guid(iid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI compositor_factory_AddRef(IActivationFactory *iface)
{
    struct compositor_factory *impl = impl_from_IActivationFactory(iface);
    ULONG ref = InterlockedIncrement(&impl->ref);
    TRACE("iface %p, ref %lu.\n", iface, ref);
    return ref;
}

static ULONG WINAPI compositor_factory_Release(IActivationFactory *iface)
{
    struct compositor_factory *impl = impl_from_IActivationFactory(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    TRACE("iface %p, ref %lu.\n", iface, ref);
    return ref;
}

static HRESULT WINAPI compositor_factory_GetIids(IActivationFactory *iface, ULONG *iidCount, IID **iids)
{
    FIXME("iface %p, iidCount %p, iids %p stub!\n", iface, iidCount, iids);
    return E_NOTIMPL;
}

static HRESULT WINAPI compositor_factory_GetRuntimeClassName(IActivationFactory *iface, HSTRING *className)
{
    TRACE("iface %p, className %p.\n", iface, className);
    return WindowsCreateString(L"Windows.UI.Composition.Compositor",
                               wcslen(L"Windows.UI.Composition.Compositor"), className);
}

static HRESULT WINAPI compositor_factory_GetTrustLevel(IActivationFactory *iface, TrustLevel *trustLevel)
{
    TRACE("iface %p, trustLevel %p.\n", iface, trustLevel);
    *trustLevel = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI compositor_factory_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    struct compositor *comp;

    TRACE("iface %p, instance %p.\n", iface, instance);

    if (!instance) return E_POINTER;
    *instance = NULL;

    comp = calloc(1, sizeof(*comp));
    if (!comp) return E_OUTOFMEMORY;

    comp->ICompositor_iface.lpVtbl = &compositor_vtbl;
    comp->ICompositorDesktopInterop_iface.lpVtbl = &compositor_desktop_interop_vtbl;
    comp->ICompositorInterop_iface.lpVtbl = &compositor_interop_vtbl;
    comp->ref = 1;

    *instance = (IInspectable *)&comp->ICompositor_iface;
    TRACE("Created Compositor instance %p\n", comp);
    return S_OK;
}

static const struct IActivationFactoryVtbl compositor_factory_vtbl =
{
    compositor_factory_QueryInterface,
    compositor_factory_AddRef,
    compositor_factory_Release,
    compositor_factory_GetIids,
    compositor_factory_GetRuntimeClassName,
    compositor_factory_GetTrustLevel,
    compositor_factory_ActivateInstance
};

static struct compositor_factory default_compositor_factory =
{
    {&compositor_factory_vtbl},
    1
};

/* ========================================================================= */
/* --- Exported DLL Functions --- */
/* ========================================================================= */

HRESULT WINAPI DllGetActivationFactory(HSTRING classid, IActivationFactory **factory)
{
    const WCHAR *buffer = WindowsGetStringRawBuffer(classid, NULL);

    TRACE("class %s, factory %p.\n", debugstr_hstring(classid), factory);

    if (!factory) return E_INVALIDARG;
    *factory = NULL;

    if (!buffer) return E_INVALIDARG;

    if (!wcscmp(buffer, L"Windows.UI.Composition.Compositor"))
    {
        *factory = &default_compositor_factory.IActivationFactory_iface;
        IActivationFactory_AddRef(*factory);
        return S_OK;
    }

    WARN("class %s not supported.\n", debugstr_hstring(classid));
    return CLASS_E_CLASSNOTAVAILABLE;
}

HRESULT WINAPI DllGetClassObject(REFCLSID clsid, REFIID riid, void **out)
{
    FIXME("clsid %s, riid %s, out %p stub!\n", debugstr_guid(clsid), debugstr_guid(riid), out);
    return CLASS_E_CLASSNOTAVAILABLE;
}
