/*
 * DirectComposition Device Implementation
 *
 * Copyright 2020 Nikolay Sivov for CodeWeavers
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

#include <stdarg.h>

#define COBJMACROS
#include "windef.h"
#include "winbase.h"
#include "wingdi.h"
#include "winuser.h"
#include "objbase.h"
#include "dxgi.h"
#include "d2dbasetypes.h"
#include "dcomptypes.h"
#include "dcomp.h"
#include "d3d11.h"
#include "wine/debug.h"
#include "initguid.h"

/* {6F15AAF2-D208-4E89-9AB4-489535D34F9C} */
DEFINE_GUID(IID_ID3D11Texture2D, 0x6f15aaf2, 0xd208, 0x4e89, 0x9a, 0xb4, 0x48, 0x95, 0x35, 0xd3, 0x4f, 0x9c);
/* {DB6F6DDB-AC77-4E88-8253-819DF9BBF140} */
DEFINE_GUID(IID_ID3D11Device, 0xdb6f6ddb, 0xac77, 0x4e88, 0x82, 0x53, 0x81, 0x9d, 0xf9, 0xbb, 0xf1, 0x40);

DEFINE_GUID(IID_IDCompositionDevice, 0xc37ea93a, 0xe7aa, 0x450d, 0xb1, 0x6f, 0x97, 0x46, 0xcb, 0x04, 0x07, 0xf3);
DEFINE_GUID(IID_IDCompositionDevice2, 0x75f6468d, 0x1b8e, 0x447c, 0x9b, 0xc6, 0x75, 0xfe, 0xa8, 0x0b, 0x5b, 0x25);
DEFINE_GUID(IID_IDCompositionDesktopDevice, 0x5f4633fe, 0x1e08, 0x4cb8, 0x8c, 0x75, 0xce, 0x24, 0x33, 0x3f, 0x56, 0x02);
DEFINE_GUID(IID_IDCompositionTarget, 0xeacdd04c, 0x117e, 0x4e17, 0x88, 0xf4, 0xd1, 0xb1, 0x2b, 0x0e, 0x3d, 0x89);
DEFINE_GUID(IID_IDCompositionVisual, 0x4d93059d, 0x097b, 0x4651, 0x9a, 0x60, 0xf0, 0xf2, 0x51, 0x16, 0xe2, 0xf3);
DEFINE_GUID(IID_IDCompositionVisual2, 0xe8de1639, 0x4331, 0x4b26, 0xbc, 0x5f, 0x6a, 0x32, 0x1d, 0x34, 0x7a, 0x85);
DEFINE_GUID(IID_IDCompositionSurface, 0xbb8a4953, 0x2c99, 0x4f5a, 0x96, 0xf5, 0x48, 0x19, 0x02, 0x7f, 0xa3, 0xac);
DEFINE_GUID(IID_IDCompositionVirtualSurface, 0xae471c51, 0x5f53, 0x4a24, 0x8d, 0x3e, 0xd0, 0xc3, 0x9c, 0x30, 0xb3, 0xf0);
DEFINE_GUID(IID_IDCompositionSurfaceFactory, 0xe334bc12, 0x3937, 0x4e02, 0x85, 0xeb, 0xfc, 0xf4, 0xeb, 0x30, 0xd2, 0xc8);
DEFINE_GUID(IID_IDCompositionEffect, 0xec81b08f, 0xbfcb, 0x4e8d, 0xb1, 0x93, 0xa9, 0x15, 0x58, 0x79, 0x99, 0xe8);
DEFINE_GUID(IID_IDCompositionEffectGroup, 0xa7929a74, 0xe6b2, 0x4bd6, 0x8b, 0x95, 0x40, 0x40, 0x11, 0x9c, 0xa3, 0x4d);
DEFINE_GUID(IID_IDCompositionTransform3D, 0x71185722, 0x246b, 0x41f2, 0xaa, 0xd1, 0x04, 0x43, 0xf7, 0xf4, 0xbf, 0xc2);
DEFINE_GUID(IID_IDCompositionTranslateTransform3D, 0x91636d4b, 0x9ba1, 0x4532, 0xaa, 0xf7, 0xe3, 0x34, 0x49, 0x94, 0xd7, 0x88);
DEFINE_GUID(IID_IDCompositionScaleTransform3D, 0x2a9e9ead, 0x364b, 0x4b15, 0xa7, 0xc4, 0xa1, 0x99, 0x7f, 0x78, 0xb3, 0x89);
DEFINE_GUID(IID_IDCompositionRotateTransform3D, 0xd8f5b23f, 0xd429, 0x4a91, 0xb5, 0x5a, 0xd2, 0xf4, 0x5f, 0xd7, 0x5b, 0x18);
DEFINE_GUID(IID_IDCompositionMatrixTransform3D, 0x4b3363f0, 0x643b, 0x41b7, 0xb6, 0xe0, 0xcc, 0xf2, 0x2d, 0x34, 0x46, 0x7c);
DEFINE_GUID(IID_IDCompositionTransform, 0xfd55faa7, 0x37e0, 0x4c20, 0x95, 0xd2, 0x9b, 0xe4, 0x5b, 0xc3, 0x3f, 0x55);
DEFINE_GUID(IID_IDCompositionScaleTransform, 0x71fde914, 0x40ef, 0x45ef, 0xbd, 0x51, 0x68, 0xb0, 0x37, 0xc3, 0x39, 0xf9);
DEFINE_GUID(IID_IDCompositionTranslateTransform, 0x06791122, 0xc6f0, 0x417d, 0x83, 0x23, 0x26, 0x9e, 0x98, 0x7f, 0x59, 0x54);
DEFINE_GUID(IID_IDCompositionRotateTransform, 0x641ed83c, 0xae96, 0x46c5, 0x90, 0xdc, 0x32, 0x77, 0x4c, 0xc5, 0xc6, 0xd5);
DEFINE_GUID(IID_IDCompositionSkewTransform, 0xe57aa735, 0xdcdb, 0x4c72, 0x9c, 0x61, 0x05, 0x91, 0xf5, 0x88, 0x89, 0xee);
DEFINE_GUID(IID_IDCompositionMatrixTransform, 0x16cdff07, 0xc503, 0x419c, 0x83, 0xf2, 0x09, 0x65, 0xc7, 0xaf, 0x1f, 0xa6);
DEFINE_GUID(IID_IDCompositionClip, 0x64ac3703, 0x9d3f, 0x45ec, 0xa1, 0x09, 0x7c, 0xac, 0x0e, 0x7a, 0x13, 0xa7);
DEFINE_GUID(IID_IDCompositionRectangleClip, 0x9842ad7d, 0xd9cf, 0x4908, 0xae, 0xd7, 0x48, 0xb5, 0x1d, 0xa5, 0xe7, 0xc2);
DEFINE_GUID(IID_IDCompositionAnimation, 0xcbfd91d9, 0x51b2, 0x45e4, 0xb3, 0xde, 0xd1, 0x9c, 0xcf, 0xb8, 0x63, 0xc5);

WINE_DEFAULT_DEBUG_CHANNEL(dcomp);

/* Internal DCompPrivates interface GUIDs queried by Microsoft.Internal.FrameworkUdk / dwmcorei */
/* {28D6AD3D-EE2A-4BCD-9419-7D54800435B1} */
DEFINE_GUID(IID_IDCompPrivatesBatchQuery, 0x28d6ad3d, 0xee2a, 0x4bcd, 0x94, 0x19, 0x7d, 0x54, 0x80, 0x04, 0x35, 0xb1);

/* {D14B6158-C3FA-4BCE-9C1F-B61D8665EAB0} */
DEFINE_GUID(IID_IDCompPrivatesTarget, 0xd14b6158, 0xc3fa, 0x4bce, 0x9c, 0x1f, 0xb6, 0x1d, 0x86, 0x65, 0xea, 0xb0);

/* {E01EB649-787E-4560-B398-0DE7A2065D8B} */
DEFINE_GUID(IID_IDCompPrivatesExtra1, 0xe01eb649, 0x787e, 0x4560, 0xb3, 0x98, 0x0d, 0xe7, 0xa2, 0x06, 0x5d, 0x8b);

/* {25090604-9C8B-42C9-8A71-BD1B3AD16512} */
DEFINE_GUID(IID_IDCompPrivatesExtra2, 0x25090604, 0x9c8b, 0x42c9, 0x8a, 0x71, 0xbd, 0x1b, 0x3a, 0xd1, 0x65, 0x12);

/* {6B556968-2D25-11E2-99F6-8BC36088709B} */
DEFINE_GUID(IID_IDCompPrivatesCommitCompletion, 0x6b556968, 0x2d25, 0x11e2, 0x99, 0xf6, 0x8b, 0xc3, 0x60, 0x88, 0x70, 0x9b);

/* {9501463E-BE70-420A-A04B-100B5F450480} */
DEFINE_GUID(IID_IDCompositionDevice3, 0x9501463e, 0xbe70, 0x420a, 0xa0, 0x4b, 0x10, 0x0b, 0x5f, 0x45, 0x04, 0x80);
/* {0987CB06-F916-48BF-8D35-CE7641781BD9} Official Windows SDK */
DEFINE_GUID(IID_IDCompositionDevice3_WinSDK, 0x0987cb06, 0xf916, 0x48bf, 0x8d, 0x35, 0xce, 0x76, 0x41, 0x78, 0x1b, 0xd9);

/* {85054009-C786-411C-9CC0-E18DD107F7B3} */
DEFINE_GUID(IID_IDCompositionDevice4, 0x85054009, 0xc786, 0x411c, 0x9c, 0xc0, 0xe1, 0x8d, 0xd1, 0x07, 0xf7, 0xb3);
/* {85FC5CCA-2DA6-494C-86B6-4A775C049B8A} Official Windows SDK */
DEFINE_GUID(IID_IDCompositionDevice4_WinSDK, 0x85fc5cca, 0x2da6, 0x494c, 0x86, 0xb6, 0x4a, 0x77, 0x5c, 0x04, 0x9b, 0x8a);

/* {2C4EEF28-1BC0-4736-B7DD-B62692F9BD67} */
DEFINE_GUID(IID_IDCompositionVisualUnknown, 0x2c4eef28, 0x1bc0, 0x4736, 0xb7, 0xdd, 0xb6, 0x26, 0x92, 0xf9, 0xbd, 0x67);

/* {C12A8AA7-363A-4145-9CC4-B011DA247D4E} */
DEFINE_GUID(IID_IDCompositionVisualExtra, 0xc12a8aa7, 0x363a, 0x4145, 0x9c, 0xc4, 0xb0, 0x11, 0xda, 0x24, 0x7d, 0x4e);

struct dcomp_device
{
    IDCompositionDesktopDevice IDCompositionDesktopDevice_iface;
    IDCompositionDevice IDCompositionDevice_iface;
    void *privates_batch_query_vtbl;
    void *privates_commit_completion_vtbl;
    LONG ref;
    HANDLE commit_event;
    IUnknown *rendering_device;
};

struct dcomp_target
{
    IDCompositionTarget IDCompositionTarget_iface;
    LONG ref;
    HWND hwnd;
};

struct dcomp_visual
{
    IDCompositionVisual2 IDCompositionVisual2_iface;
    LONG ref;
};

struct dcomp_surface
{
    IDCompositionVirtualSurface IDCompositionVirtualSurface_iface;
    LONG ref;
    IUnknown *rendering_device;
    UINT width;
    UINT height;
    DXGI_FORMAT pixel_format;
    DXGI_ALPHA_MODE alpha_mode;
    ID3D11Texture2D *current_texture;
};

struct dcomp_surface_factory
{
    IDCompositionSurfaceFactory IDCompositionSurfaceFactory_iface;
    LONG ref;
    IUnknown *rendering_device;
};

static inline struct dcomp_device *impl_from_IDCompositionDesktopDevice(IDCompositionDesktopDevice *iface)
{
    return CONTAINING_RECORD(iface, struct dcomp_device, IDCompositionDesktopDevice_iface);
}

static inline struct dcomp_device *impl_from_IDCompositionDevice(IDCompositionDevice *iface)
{
    return CONTAINING_RECORD(iface, struct dcomp_device, IDCompositionDevice_iface);
}

static inline struct dcomp_device *impl_from_privates_batch_query(void *iface)
{
    return (struct dcomp_device *)((char *)iface - FIELD_OFFSET(struct dcomp_device, privates_batch_query_vtbl));
}

static inline struct dcomp_device *impl_from_privates_commit_completion(void *iface)
{
    return (struct dcomp_device *)((char *)iface - FIELD_OFFSET(struct dcomp_device, privates_commit_completion_vtbl));
}

static inline struct dcomp_target *impl_from_IDCompositionTarget(IDCompositionTarget *iface)
{
    return CONTAINING_RECORD(iface, struct dcomp_target, IDCompositionTarget_iface);
}

static inline struct dcomp_visual *impl_from_IDCompositionVisual2(IDCompositionVisual2 *iface)
{
    return CONTAINING_RECORD(iface, struct dcomp_visual, IDCompositionVisual2_iface);
}

static inline struct dcomp_surface *impl_from_IDCompositionVirtualSurface(IDCompositionVirtualSurface *iface)
{
    return CONTAINING_RECORD(iface, struct dcomp_surface, IDCompositionVirtualSurface_iface);
}

static inline struct dcomp_surface_factory *impl_from_IDCompositionSurfaceFactory(IDCompositionSurfaceFactory *iface)
{
    return CONTAINING_RECORD(iface, struct dcomp_surface_factory, IDCompositionSurfaceFactory_iface);
}

/* --- IDCompositionTarget --- */
static HRESULT STDMETHODCALLTYPE dcomp_target_QueryInterface(IDCompositionTarget *iface, REFIID riid, void **out)
{
    TRACE("(%p, %s, %p)\n", iface, debugstr_guid(riid), out);
    if (IsEqualIID(riid, &IID_IUnknown) || IsEqualIID(riid, &IID_IDCompositionTarget))
    {
        *out = iface;
        IDCompositionTarget_AddRef(iface);
        return S_OK;
    }
    WARN("Unsupported interface %s\n", debugstr_guid(riid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG STDMETHODCALLTYPE dcomp_target_AddRef(IDCompositionTarget *iface)
{
    struct dcomp_target *target = impl_from_IDCompositionTarget(iface);
    ULONG ref = InterlockedIncrement(&target->ref);
    TRACE("(%p) ref=%lu\n", iface, ref);
    return ref;
}

static ULONG STDMETHODCALLTYPE dcomp_target_Release(IDCompositionTarget *iface)
{
    struct dcomp_target *target = impl_from_IDCompositionTarget(iface);
    ULONG ref = InterlockedDecrement(&target->ref);
    TRACE("(%p) ref=%lu\n", iface, ref);
    if (!ref)
        free(target);
    return ref;
}

static HRESULT STDMETHODCALLTYPE dcomp_target_SetRoot(IDCompositionTarget *iface, IDCompositionVisual *visual)
{
    TRACE("(%p, %p)\n", iface, visual);
    return S_OK;
}

static const IDCompositionTargetVtbl dcomp_target_vtbl =
{
    dcomp_target_QueryInterface,
    dcomp_target_AddRef,
    dcomp_target_Release,
    dcomp_target_SetRoot,
};

static HRESULT dcomp_target_create(HWND hwnd, IDCompositionTarget **out)
{
    struct dcomp_target *target = calloc(1, sizeof(*target));
    if (!target)
        return E_OUTOFMEMORY;
    target->IDCompositionTarget_iface.lpVtbl = &dcomp_target_vtbl;
    target->ref = 1;
    target->hwnd = hwnd;
    *out = &target->IDCompositionTarget_iface;
    return S_OK;
}

/* --- Internal DComp Shared Target / Visual Unknown Objects --- */
struct dcomp_shared_target
{
    void **vtbl;
    LONG ref;
};

static HRESULT WINAPI dcomp_shared_target_QueryInterface(void *iface, REFIID iid, void **out)
{
    TRACE("(%p, %s, %p)\n", iface, debugstr_guid(iid), out);
    if (!out)
        return E_POINTER;
    *out = iface;
    return S_OK;
}

static ULONG WINAPI dcomp_shared_target_AddRef(void *iface)
{
    return 2;
}

static ULONG WINAPI dcomp_shared_target_Release(void *iface)
{
    return 1;
}

static HRESULT WINAPI dcomp_shared_target_Method(void *iface, void *a, void *b, void *c)
{
    TRACE("(%p, %p, %p, %p)\n", iface, a, b, c);
    return S_OK;
}

static void *dcomp_shared_target_vtbl[256];

static void init_dcomp_shared_target_vtbl(void)
{
    static BOOL init = FALSE;
    int i;
    if (init) return;
    init = TRUE;
    for (i = 0; i < 256; ++i)
        dcomp_shared_target_vtbl[i] = (void *)dcomp_shared_target_Method;
    dcomp_shared_target_vtbl[0] = (void *)dcomp_shared_target_QueryInterface;
    dcomp_shared_target_vtbl[1] = (void *)dcomp_shared_target_AddRef;
    dcomp_shared_target_vtbl[2] = (void *)dcomp_shared_target_Release;
}

static struct dcomp_shared_target default_dcomp_shared_target;

/* --- IDCompositionVisual2 --- */
static HRESULT STDMETHODCALLTYPE dcomp_visual_QueryInterface(IDCompositionVisual2 *iface, REFIID riid, void **out)
{
    TRACE("(%p, %s, %p)\n", iface, debugstr_guid(riid), out);
    if (IsEqualIID(riid, &IID_IUnknown) ||
        IsEqualIID(riid, &IID_IDCompositionVisual) ||
        IsEqualIID(riid, &IID_IDCompositionVisual2))
    {
        *out = iface;
        IDCompositionVisual2_AddRef(iface);
        return S_OK;
    }

    if (IsEqualIID(riid, &IID_IDCompositionVisualUnknown) ||
        IsEqualIID(riid, &IID_IDCompositionVisualExtra))
    {
        init_dcomp_shared_target_vtbl();
        default_dcomp_shared_target.vtbl = dcomp_shared_target_vtbl;
        default_dcomp_shared_target.ref = 1;
        *out = &default_dcomp_shared_target;
        return S_OK;
    }

    WARN("Unsupported interface %s\n", debugstr_guid(riid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG STDMETHODCALLTYPE dcomp_visual_AddRef(IDCompositionVisual2 *iface)
{
    struct dcomp_visual *visual = impl_from_IDCompositionVisual2(iface);
    ULONG ref = InterlockedIncrement(&visual->ref);
    TRACE("(%p) ref=%lu\n", iface, ref);
    return ref;
}

static ULONG STDMETHODCALLTYPE dcomp_visual_Release(IDCompositionVisual2 *iface)
{
    struct dcomp_visual *visual = impl_from_IDCompositionVisual2(iface);
    ULONG ref = InterlockedDecrement(&visual->ref);
    TRACE("(%p) ref=%lu\n", iface, ref);
    if (!ref)
        free(visual);
    return ref;
}

static HRESULT STDMETHODCALLTYPE dcomp_visual_SetOffsetXAnimation(IDCompositionVisual2 *iface, IDCompositionAnimation *a) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_visual_SetOffsetX(IDCompositionVisual2 *iface, float x) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_visual_SetOffsetYAnimation(IDCompositionVisual2 *iface, IDCompositionAnimation *a) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_visual_SetOffsetY(IDCompositionVisual2 *iface, float y) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_visual_SetTransformObject(IDCompositionVisual2 *iface, IDCompositionTransform *t) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_visual_SetTransform(IDCompositionVisual2 *iface, const D2D_MATRIX_3X2_F *m) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_visual_SetTransformParent(IDCompositionVisual2 *iface, IDCompositionVisual *v) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_visual_SetEffect(IDCompositionVisual2 *iface, IDCompositionEffect *e) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_visual_SetBitmapInterpolationMode(IDCompositionVisual2 *iface, enum DCOMPOSITION_BITMAP_INTERPOLATION_MODE m) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_visual_SetBorderMode(IDCompositionVisual2 *iface, enum DCOMPOSITION_BORDER_MODE m) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_visual_SetClipObject(IDCompositionVisual2 *iface, IDCompositionClip *c) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_visual_SetClip(IDCompositionVisual2 *iface, const D2D_RECT_F *r) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_visual_SetContent(IDCompositionVisual2 *iface, IUnknown *c) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_visual_AddVisual(IDCompositionVisual2 *iface, IDCompositionVisual *v, BOOL a, IDCompositionVisual *r) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_visual_RemoveVisual(IDCompositionVisual2 *iface, IDCompositionVisual *v) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_visual_RemoveAllVisuals(IDCompositionVisual2 *iface) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_visual_SetCompositeMode(IDCompositionVisual2 *iface, enum DCOMPOSITION_COMPOSITE_MODE m) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_visual_SetOpacityMode(IDCompositionVisual2 *iface, enum DCOMPOSITION_OPACITY_MODE m) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_visual_SetBackFaceVisibility(IDCompositionVisual2 *iface, enum DCOMPOSITION_BACKFACE_VISIBILITY v) { return S_OK; }

static const IDCompositionVisual2Vtbl dcomp_visual_vtbl =
{
    dcomp_visual_QueryInterface,
    dcomp_visual_AddRef,
    dcomp_visual_Release,
    dcomp_visual_SetOffsetXAnimation,
    dcomp_visual_SetOffsetX,
    dcomp_visual_SetOffsetYAnimation,
    dcomp_visual_SetOffsetY,
    dcomp_visual_SetTransformObject,
    dcomp_visual_SetTransform,
    dcomp_visual_SetTransformParent,
    dcomp_visual_SetEffect,
    dcomp_visual_SetBitmapInterpolationMode,
    dcomp_visual_SetBorderMode,
    dcomp_visual_SetClipObject,
    dcomp_visual_SetClip,
    dcomp_visual_SetContent,
    dcomp_visual_AddVisual,
    dcomp_visual_RemoveVisual,
    dcomp_visual_RemoveAllVisuals,
    dcomp_visual_SetCompositeMode,
    dcomp_visual_SetOpacityMode,
    dcomp_visual_SetBackFaceVisibility,
};

static HRESULT dcomp_visual_create(IDCompositionVisual2 **out)
{
    struct dcomp_visual *visual = calloc(1, sizeof(*visual));
    if (!visual)
        return E_OUTOFMEMORY;
    visual->IDCompositionVisual2_iface.lpVtbl = &dcomp_visual_vtbl;
    visual->ref = 1;
    *out = &visual->IDCompositionVisual2_iface;
    return S_OK;
}

/* --- IDCompositionVirtualSurface / IDCompositionSurface --- */
static HRESULT STDMETHODCALLTYPE dcomp_surface_QueryInterface(IDCompositionVirtualSurface *iface, REFIID riid, void **out)
{
    TRACE("(%p, %s, %p)\n", iface, debugstr_guid(riid), out);
    if (IsEqualIID(riid, &IID_IUnknown) ||
        IsEqualIID(riid, &IID_IDCompositionSurface) ||
        IsEqualIID(riid, &IID_IDCompositionVirtualSurface))
    {
        *out = iface;
        IDCompositionVirtualSurface_AddRef(iface);
        return S_OK;
    }
    WARN("Unsupported interface %s\n", debugstr_guid(riid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG STDMETHODCALLTYPE dcomp_surface_AddRef(IDCompositionVirtualSurface *iface)
{
    struct dcomp_surface *surface = impl_from_IDCompositionVirtualSurface(iface);
    ULONG ref = InterlockedIncrement(&surface->ref);
    TRACE("(%p) ref=%lu\n", iface, ref);
    return ref;
}

static ULONG STDMETHODCALLTYPE dcomp_surface_Release(IDCompositionVirtualSurface *iface)
{
    struct dcomp_surface *surface = impl_from_IDCompositionVirtualSurface(iface);
    ULONG ref = InterlockedDecrement(&surface->ref);
    TRACE("(%p) ref=%lu\n", iface, ref);
    if (!ref)
    {
        if (surface->current_texture)
            ID3D11Texture2D_Release(surface->current_texture);
        if (surface->rendering_device)
            IUnknown_Release(surface->rendering_device);
        free(surface);
    }
    return ref;
}

static HRESULT STDMETHODCALLTYPE dcomp_surface_BeginDraw(IDCompositionVirtualSurface *iface,
    const RECT *rect, REFIID iid, void **object, POINT *offset)
{
    struct dcomp_surface *surface = impl_from_IDCompositionVirtualSurface(iface);
    HRESULT hr;

    TRACE("(%p, %p, %s, %p, %p)\n", iface, rect, debugstr_guid(iid), object, offset);

    if (!object)
        return E_POINTER;

    *object = NULL;
    if (offset)
        memset(offset, 0, sizeof(*offset));

    if (surface->rendering_device)
    {
        ID3D11Device *d3d_device = NULL;
        hr = IUnknown_QueryInterface(surface->rendering_device, &IID_ID3D11Device, (void **)&d3d_device);
        if (SUCCEEDED(hr) && d3d_device)
        {
            D3D11_TEXTURE2D_DESC desc = {0};
            desc.Width = surface->width ? surface->width : 1;
            desc.Height = surface->height ? surface->height : 1;
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.Format = surface->pixel_format ? surface->pixel_format : DXGI_FORMAT_B8G8R8A8_UNORM;
            desc.SampleDesc.Count = 1;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
            desc.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE;

            if (surface->current_texture)
            {
                ID3D11Texture2D_Release(surface->current_texture);
                surface->current_texture = NULL;
            }

            hr = ID3D11Device_CreateTexture2D(d3d_device, &desc, NULL, &surface->current_texture);
            if (FAILED(hr))
            {
                desc.MiscFlags = 0;
                hr = ID3D11Device_CreateTexture2D(d3d_device, &desc, NULL, &surface->current_texture);
            }
            if (SUCCEEDED(hr) && surface->current_texture)
            {
                hr = ID3D11Texture2D_QueryInterface(surface->current_texture, iid, object);
            }
            ID3D11Device_Release(d3d_device);

            if (SUCCEEDED(hr) && *object)
            {
                TRACE("BeginDraw successfully created texture object %p\n", *object);
                return S_OK;
            }
        }
    }

    WARN("BeginDraw could not provide texture for interface %s, returning E_NOINTERFACE\n", debugstr_guid(iid));
    return E_NOINTERFACE;
}

static HRESULT STDMETHODCALLTYPE dcomp_surface_EndDraw(IDCompositionVirtualSurface *iface) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_surface_SuspendDraw(IDCompositionVirtualSurface *iface) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_surface_ResumeDraw(IDCompositionVirtualSurface *iface) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_surface_Scroll(IDCompositionVirtualSurface *iface,
    const RECT *scroll, const RECT *clip, int offset_x, int offset_y) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_surface_Resize(IDCompositionVirtualSurface *iface, UINT width, UINT height) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_surface_Trim(IDCompositionVirtualSurface *iface, const RECT *rectangles, UINT count) { return S_OK; }

static const IDCompositionVirtualSurfaceVtbl dcomp_surface_vtbl =
{
    dcomp_surface_QueryInterface,
    dcomp_surface_AddRef,
    dcomp_surface_Release,
    dcomp_surface_BeginDraw,
    dcomp_surface_EndDraw,
    dcomp_surface_SuspendDraw,
    dcomp_surface_ResumeDraw,
    dcomp_surface_Scroll,
    dcomp_surface_Resize,
    dcomp_surface_Trim,
};

static HRESULT dcomp_surface_create(IUnknown *rendering_device, UINT width, UINT height, DXGI_FORMAT pixel_format,
    DXGI_ALPHA_MODE alpha_mode, IDCompositionSurface **out)
{
    struct dcomp_surface *surface = calloc(1, sizeof(*surface));
    if (!surface)
        return E_OUTOFMEMORY;
    surface->IDCompositionVirtualSurface_iface.lpVtbl = &dcomp_surface_vtbl;
    surface->ref = 1;
    surface->rendering_device = rendering_device;
    if (rendering_device)
        IUnknown_AddRef(rendering_device);
    surface->width = width;
    surface->height = height;
    surface->pixel_format = pixel_format;
    surface->alpha_mode = alpha_mode;
    *out = (IDCompositionSurface *)&surface->IDCompositionVirtualSurface_iface;
    return S_OK;
}

static HRESULT dcomp_virtual_surface_create(IUnknown *rendering_device, UINT width, UINT height, DXGI_FORMAT pixel_format,
    DXGI_ALPHA_MODE alpha_mode, IDCompositionVirtualSurface **out)
{
    struct dcomp_surface *surface = calloc(1, sizeof(*surface));
    if (!surface)
        return E_OUTOFMEMORY;
    surface->IDCompositionVirtualSurface_iface.lpVtbl = &dcomp_surface_vtbl;
    surface->ref = 1;
    surface->rendering_device = rendering_device;
    if (rendering_device)
        IUnknown_AddRef(rendering_device);
    surface->width = width;
    surface->height = height;
    surface->pixel_format = pixel_format;
    surface->alpha_mode = alpha_mode;
    *out = &surface->IDCompositionVirtualSurface_iface;
    return S_OK;
}

/* --- IDCompositionSurfaceFactory --- */
static HRESULT STDMETHODCALLTYPE dcomp_surface_factory_QueryInterface(IDCompositionSurfaceFactory *iface, REFIID riid, void **out)
{
    TRACE("(%p, %s, %p)\n", iface, debugstr_guid(riid), out);
    if (IsEqualIID(riid, &IID_IUnknown) || IsEqualIID(riid, &IID_IDCompositionSurfaceFactory))
    {
        *out = iface;
        IDCompositionSurfaceFactory_AddRef(iface);
        return S_OK;
    }
    WARN("Unsupported interface %s\n", debugstr_guid(riid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG STDMETHODCALLTYPE dcomp_surface_factory_AddRef(IDCompositionSurfaceFactory *iface)
{
    struct dcomp_surface_factory *factory = impl_from_IDCompositionSurfaceFactory(iface);
    ULONG ref = InterlockedIncrement(&factory->ref);
    TRACE("(%p) ref=%lu\n", iface, ref);
    return ref;
}

static ULONG STDMETHODCALLTYPE dcomp_surface_factory_Release(IDCompositionSurfaceFactory *iface)
{
    struct dcomp_surface_factory *factory = impl_from_IDCompositionSurfaceFactory(iface);
    ULONG ref = InterlockedDecrement(&factory->ref);
    TRACE("(%p) ref=%lu\n", iface, ref);
    if (!ref)
    {
        if (factory->rendering_device)
            IUnknown_Release(factory->rendering_device);
        free(factory);
    }
    return ref;
}

static HRESULT STDMETHODCALLTYPE dcomp_surface_factory_CreateSurface(IDCompositionSurfaceFactory *iface,
    UINT width, UINT height, DXGI_FORMAT pixel_format, DXGI_ALPHA_MODE alpha_mode, IDCompositionSurface **surface)
{
    struct dcomp_surface_factory *factory = impl_from_IDCompositionSurfaceFactory(iface);
    TRACE("(%p, %u, %u, %d, %d, %p)\n", iface, width, height, pixel_format, alpha_mode, surface);
    return dcomp_surface_create(factory->rendering_device, width, height, pixel_format, alpha_mode, surface);
}

static HRESULT STDMETHODCALLTYPE dcomp_surface_factory_CreateVirtualSurface(IDCompositionSurfaceFactory *iface,
    UINT width, UINT height, DXGI_FORMAT pixel_format, DXGI_ALPHA_MODE alpha_mode, IDCompositionVirtualSurface **surface)
{
    struct dcomp_surface_factory *factory = impl_from_IDCompositionSurfaceFactory(iface);
    TRACE("(%p, %u, %u, %d, %d, %p)\n", iface, width, height, pixel_format, alpha_mode, surface);
    return dcomp_virtual_surface_create(factory->rendering_device, width, height, pixel_format, alpha_mode, surface);
}

static const IDCompositionSurfaceFactoryVtbl dcomp_surface_factory_vtbl =
{
    dcomp_surface_factory_QueryInterface,
    dcomp_surface_factory_AddRef,
    dcomp_surface_factory_Release,
    dcomp_surface_factory_CreateSurface,
    dcomp_surface_factory_CreateVirtualSurface,
};

static HRESULT dcomp_surface_factory_create(IUnknown *rendering_device, IDCompositionSurfaceFactory **out)
{
    struct dcomp_surface_factory *factory = calloc(1, sizeof(*factory));
    if (!factory)
        return E_OUTOFMEMORY;
    factory->IDCompositionSurfaceFactory_iface.lpVtbl = &dcomp_surface_factory_vtbl;
    factory->ref = 1;
    factory->rendering_device = rendering_device;
    if (rendering_device)
        IUnknown_AddRef(rendering_device);
    *out = &factory->IDCompositionSurfaceFactory_iface;
    return S_OK;
}

/* --- Internal DCompPrivates vtables --- */
static HRESULT STDMETHODCALLTYPE privates_generic_stub(void *This, void *a, void *b, void *c)
{
    TRACE("(%p, %p, %p, %p)\n", This, a, b, c);
    return S_OK;
}

static HRESULT STDMETHODCALLTYPE privates_batch_query_QueryInterface(void *This, REFIID riid, void **out)
{
    struct dcomp_device *device = impl_from_privates_batch_query(This);
    return IDCompositionDesktopDevice_QueryInterface(&device->IDCompositionDesktopDevice_iface, riid, out);
}

static ULONG STDMETHODCALLTYPE privates_batch_query_AddRef(void *This)
{
    struct dcomp_device *device = impl_from_privates_batch_query(This);
    return IDCompositionDesktopDevice_AddRef(&device->IDCompositionDesktopDevice_iface);
}

static ULONG STDMETHODCALLTYPE privates_batch_query_Release(void *This)
{
    struct dcomp_device *device = impl_from_privates_batch_query(This);
    return IDCompositionDesktopDevice_Release(&device->IDCompositionDesktopDevice_iface);
}

static HRESULT STDMETHODCALLTYPE privates_batch_query_GetSomething(void *This, DWORD *out_val)
{
    TRACE("(%p, out_val=%p)\n", This, out_val);
    if (out_val)
        *out_val = 0;
    return S_OK;
}

static HRESULT STDMETHODCALLTYPE privates_commit_completion_QueryInterface(void *This, REFIID riid, void **out)
{
    struct dcomp_device *device = impl_from_privates_commit_completion(This);
    return IDCompositionDesktopDevice_QueryInterface(&device->IDCompositionDesktopDevice_iface, riid, out);
}

static ULONG STDMETHODCALLTYPE privates_commit_completion_AddRef(void *This)
{
    struct dcomp_device *device = impl_from_privates_commit_completion(This);
    return IDCompositionDesktopDevice_AddRef(&device->IDCompositionDesktopDevice_iface);
}

static ULONG STDMETHODCALLTYPE privates_commit_completion_Release(void *This)
{
    struct dcomp_device *device = impl_from_privates_commit_completion(This);
    return IDCompositionDesktopDevice_Release(&device->IDCompositionDesktopDevice_iface);
}

static HRESULT STDMETHODCALLTYPE privates_commit_completion_SetCommitCompletionEvent(void *This, HANDLE hEvent)
{
    struct dcomp_device *device = impl_from_privates_commit_completion(This);
    TRACE("(%p, hEvent=%p)\n", This, hEvent);
    device->commit_event = hEvent;
    if (hEvent)
        SetEvent(hEvent);
    return S_OK;
}

static HRESULT STDMETHODCALLTYPE privates_batch_query_OpenSharedResource(void *This, HANDLE handle, REFIID riid, void **out_target)
{
    TRACE("(%p, handle=%p, riid=%s, out_target=%p)\n", This, handle, debugstr_guid(riid), out_target);
    init_dcomp_shared_target_vtbl();
    default_dcomp_shared_target.vtbl = dcomp_shared_target_vtbl;
    default_dcomp_shared_target.ref = 1;
    if (out_target)
        *out_target = &default_dcomp_shared_target;
    return S_OK;
}

/* Static vtable tables for private interfaces */
static void *dcomp_privates_batch_query_vtbl[256];
static void *dcomp_privates_commit_completion_vtbl[32];

static void init_dcomp_privates_vtables(void)
{
    static BOOL initialized = FALSE;
    int i;
    if (initialized)
        return;
    initialized = TRUE;

    for (i = 0; i < 256; ++i)
        dcomp_privates_batch_query_vtbl[i] = (void *)privates_generic_stub;
    dcomp_privates_batch_query_vtbl[0] = (void *)privates_batch_query_QueryInterface;
    dcomp_privates_batch_query_vtbl[1] = (void *)privates_batch_query_AddRef;
    dcomp_privates_batch_query_vtbl[2] = (void *)privates_batch_query_Release;
    /* Slot 29 (0xe8): OpenSharedResource(void *This, HANDLE handle, REFIID riid, void **out_target) */
    dcomp_privates_batch_query_vtbl[29] = (void *)privates_batch_query_OpenSharedResource;
    /* Slot 37 (0x128): OpenSharedTarget(void *This, HANDLE handle, void **out_target, void *arg3) */
    dcomp_privates_batch_query_vtbl[37] = (void *)privates_batch_query_OpenSharedResource;
    /* Slot 46 (0x170): GetSomething(void *This, DWORD *out_val) */
    dcomp_privates_batch_query_vtbl[46] = (void *)privates_batch_query_GetSomething;

    for (i = 0; i < 32; ++i)
        dcomp_privates_commit_completion_vtbl[i] = (void *)privates_generic_stub;
    dcomp_privates_commit_completion_vtbl[0] = (void *)privates_commit_completion_QueryInterface;
    dcomp_privates_commit_completion_vtbl[1] = (void *)privates_commit_completion_AddRef;
    dcomp_privates_commit_completion_vtbl[2] = (void *)privates_commit_completion_Release;
    /* Slots 3..7: SetCommitCompletionEvent(void *This, HANDLE hEvent) */
    dcomp_privates_commit_completion_vtbl[3] = (void *)privates_commit_completion_SetCommitCompletionEvent;
    dcomp_privates_commit_completion_vtbl[4] = (void *)privates_commit_completion_SetCommitCompletionEvent;
    dcomp_privates_commit_completion_vtbl[5] = (void *)privates_commit_completion_SetCommitCompletionEvent;
    dcomp_privates_commit_completion_vtbl[6] = (void *)privates_commit_completion_SetCommitCompletionEvent;
    dcomp_privates_commit_completion_vtbl[7] = (void *)privates_commit_completion_SetCommitCompletionEvent;
}

/* --- IDCompositionDesktopDevice / IDCompositionDevice2 --- */
static HRESULT STDMETHODCALLTYPE dcomp_desktop_device_QueryInterface(IDCompositionDesktopDevice *iface,
    REFIID riid, void **out)
{
    struct dcomp_device *device = impl_from_IDCompositionDesktopDevice(iface);
    TRACE("(%p, %s, %p)\n", iface, debugstr_guid(riid), out);

    if (IsEqualIID(riid, &IID_IUnknown) ||
        IsEqualIID(riid, &IID_IDCompositionDevice2) ||
        IsEqualIID(riid, &IID_IDCompositionDesktopDevice) ||
        IsEqualIID(riid, &IID_IDCompositionDevice3) ||
        IsEqualIID(riid, &IID_IDCompositionDevice3_WinSDK) ||
        IsEqualIID(riid, &IID_IDCompositionDevice4) ||
        IsEqualIID(riid, &IID_IDCompositionDevice4_WinSDK))
    {
        *out = &device->IDCompositionDesktopDevice_iface;
        IDCompositionDesktopDevice_AddRef(iface);
        return S_OK;
    }

    if (IsEqualIID(riid, &IID_IDCompositionDevice))
    {
        *out = &device->IDCompositionDevice_iface;
        IDCompositionDesktopDevice_AddRef(iface);
        return S_OK;
    }

    if (IsEqualIID(riid, &IID_IDCompPrivatesBatchQuery) ||
        IsEqualIID(riid, &IID_IDCompPrivatesTarget) ||
        IsEqualIID(riid, &IID_IDCompPrivatesExtra1) ||
        IsEqualIID(riid, &IID_IDCompPrivatesExtra2))
    {
        *out = &device->privates_batch_query_vtbl;
        IDCompositionDesktopDevice_AddRef(iface);
        return S_OK;
    }

    if (IsEqualIID(riid, &IID_IDCompPrivatesCommitCompletion))
    {
        *out = &device->privates_commit_completion_vtbl;
        IDCompositionDesktopDevice_AddRef(iface);
        return S_OK;
    }

    WARN("Unsupported interface %s\n", debugstr_guid(riid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG STDMETHODCALLTYPE dcomp_desktop_device_AddRef(IDCompositionDesktopDevice *iface)
{
    struct dcomp_device *device = impl_from_IDCompositionDesktopDevice(iface);
    ULONG ref = InterlockedIncrement(&device->ref);
    TRACE("(%p) ref=%lu\n", iface, ref);
    return ref;
}

static ULONG STDMETHODCALLTYPE dcomp_desktop_device_Release(IDCompositionDesktopDevice *iface)
{
    struct dcomp_device *device = impl_from_IDCompositionDesktopDevice(iface);
    ULONG ref = InterlockedDecrement(&device->ref);
    TRACE("(%p) ref=%lu\n", iface, ref);
    if (!ref)
    {
        if (device->rendering_device)
            IUnknown_Release(device->rendering_device);
        free(device);
    }
    return ref;
}

static HRESULT STDMETHODCALLTYPE dcomp_desktop_device_Commit(IDCompositionDesktopDevice *iface)
{
    struct dcomp_device *device = impl_from_IDCompositionDesktopDevice(iface);
    TRACE("(%p, commit_event=%p)\n", iface, device->commit_event);
    if (device->commit_event)
    {
        TRACE("Signaling commit event %p\n", device->commit_event);
        SetEvent(device->commit_event);
    }
    return S_OK;
}

static HRESULT STDMETHODCALLTYPE dcomp_desktop_device_WaitForCommitCompletion(IDCompositionDesktopDevice *iface)
{
    TRACE("(%p)\n", iface);
    return S_OK;
}

static HRESULT STDMETHODCALLTYPE dcomp_desktop_device_GetFrameStatistics(IDCompositionDesktopDevice *iface,
    DCOMPOSITION_FRAME_STATISTICS *statistics)
{
    TRACE("(%p, %p)\n", iface, statistics);
    if (statistics)
    {
        memset(statistics, 0, sizeof(*statistics));
        QueryPerformanceCounter(&statistics->lastFrameTime);
        QueryPerformanceFrequency((LARGE_INTEGER *)&statistics->currentCompositionRate);
    }
    return S_OK;
}

static HRESULT STDMETHODCALLTYPE dcomp_desktop_device_CreateVisual(IDCompositionDesktopDevice *iface,
    IDCompositionVisual2 **visual)
{
    TRACE("(%p, %p)\n", iface, visual);
    if (!visual)
        return E_POINTER;
    return dcomp_visual_create(visual);
}

static HRESULT STDMETHODCALLTYPE dcomp_desktop_device_CreateSurfaceFactory(IDCompositionDesktopDevice *iface,
    IUnknown *rendering_device, IDCompositionSurfaceFactory **surface_factory)
{
    struct dcomp_device *device = impl_from_IDCompositionDesktopDevice(iface);
    TRACE("(%p, %p, %p)\n", iface, rendering_device, surface_factory);
    if (!surface_factory)
        return E_POINTER;
    return dcomp_surface_factory_create(rendering_device ? rendering_device : device->rendering_device, surface_factory);
}

static HRESULT STDMETHODCALLTYPE dcomp_desktop_device_CreateSurface(IDCompositionDesktopDevice *iface,
    UINT width, UINT height, DXGI_FORMAT pixel_format, DXGI_ALPHA_MODE alpha_mode, IDCompositionSurface **surface)
{
    struct dcomp_device *device = impl_from_IDCompositionDesktopDevice(iface);
    TRACE("(%p, %u, %u, %d, %d, %p)\n", iface, width, height, pixel_format, alpha_mode, surface);
    if (!surface)
        return E_POINTER;
    return dcomp_surface_create(device->rendering_device, width, height, pixel_format, alpha_mode, surface);
}

static HRESULT STDMETHODCALLTYPE dcomp_desktop_device_CreateVirtualSurface(IDCompositionDesktopDevice *iface,
    UINT width, UINT height, DXGI_FORMAT pixel_format, DXGI_ALPHA_MODE alpha_mode, IDCompositionVirtualSurface **surface)
{
    struct dcomp_device *device = impl_from_IDCompositionDesktopDevice(iface);
    TRACE("(%p, %u, %u, %d, %d, %p)\n", iface, width, height, pixel_format, alpha_mode, surface);
    if (!surface)
        return E_POINTER;
    return dcomp_virtual_surface_create(device->rendering_device, width, height, pixel_format, alpha_mode, surface);
}

/* --- IDCompositionEffectGroup --- */
struct dcomp_effect_group
{
    IDCompositionEffectGroup IDCompositionEffectGroup_iface;
    LONG ref;
};

static inline struct dcomp_effect_group *impl_from_IDCompositionEffectGroup(IDCompositionEffectGroup *iface)
{
    return CONTAINING_RECORD(iface, struct dcomp_effect_group, IDCompositionEffectGroup_iface);
}

static HRESULT STDMETHODCALLTYPE dcomp_effect_group_QueryInterface(IDCompositionEffectGroup *iface,
    REFIID riid, void **out)
{
    TRACE("(%p, %s, %p)\n", iface, debugstr_guid(riid), out);
    if (IsEqualIID(riid, &IID_IUnknown) ||
        IsEqualIID(riid, &IID_IDCompositionEffect) ||
        IsEqualIID(riid, &IID_IDCompositionEffectGroup))
    {
        *out = iface;
        IDCompositionEffectGroup_AddRef(iface);
        return S_OK;
    }
    WARN("Unsupported interface %s\n", debugstr_guid(riid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG STDMETHODCALLTYPE dcomp_effect_group_AddRef(IDCompositionEffectGroup *iface)
{
    struct dcomp_effect_group *group = impl_from_IDCompositionEffectGroup(iface);
    ULONG ref = InterlockedIncrement(&group->ref);
    TRACE("(%p) ref=%lu\n", iface, ref);
    return ref;
}

static ULONG STDMETHODCALLTYPE dcomp_effect_group_Release(IDCompositionEffectGroup *iface)
{
    struct dcomp_effect_group *group = impl_from_IDCompositionEffectGroup(iface);
    ULONG ref = InterlockedDecrement(&group->ref);
    TRACE("(%p) ref=%lu\n", iface, ref);
    if (!ref)
        free(group);
    return ref;
}

static HRESULT STDMETHODCALLTYPE dcomp_effect_group_SetOpacityAnimation(IDCompositionEffectGroup *iface,
    IDCompositionAnimation *animation) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_effect_group_SetOpacity(IDCompositionEffectGroup *iface,
    float opacity) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_effect_group_SetTransform3D(IDCompositionEffectGroup *iface,
    IDCompositionTransform3D *transform) { return S_OK; }

static const IDCompositionEffectGroupVtbl dcomp_effect_group_vtbl =
{
    dcomp_effect_group_QueryInterface,
    dcomp_effect_group_AddRef,
    dcomp_effect_group_Release,
    dcomp_effect_group_SetOpacityAnimation,
    dcomp_effect_group_SetOpacity,
    dcomp_effect_group_SetTransform3D,
};

static HRESULT dcomp_effect_group_create(IDCompositionEffectGroup **out)
{
    struct dcomp_effect_group *group = calloc(1, sizeof(*group));
    if (!group)
        return E_OUTOFMEMORY;
    group->IDCompositionEffectGroup_iface.lpVtbl = &dcomp_effect_group_vtbl;
    group->ref = 1;
    *out = &group->IDCompositionEffectGroup_iface;
    return S_OK;
}

/* --- IDCompositionTranslateTransform --- */
struct dcomp_translate_transform
{
    IDCompositionTranslateTransform IDCompositionTranslateTransform_iface;
    LONG ref;
};

static inline struct dcomp_translate_transform *impl_from_IDCompositionTranslateTransform(IDCompositionTranslateTransform *iface)
{
    return CONTAINING_RECORD(iface, struct dcomp_translate_transform, IDCompositionTranslateTransform_iface);
}

static HRESULT STDMETHODCALLTYPE dcomp_translate_transform_QueryInterface(IDCompositionTranslateTransform *iface,
    REFIID riid, void **out)
{
    TRACE("(%p, %s, %p)\n", iface, debugstr_guid(riid), out);
    if (IsEqualIID(riid, &IID_IUnknown) ||
        IsEqualIID(riid, &IID_IDCompositionEffect) ||
        IsEqualIID(riid, &IID_IDCompositionTransform3D) ||
        IsEqualIID(riid, &IID_IDCompositionTransform) ||
        IsEqualIID(riid, &IID_IDCompositionTranslateTransform))
    {
        *out = iface;
        IDCompositionTranslateTransform_AddRef(iface);
        return S_OK;
    }
    WARN("Unsupported interface %s\n", debugstr_guid(riid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG STDMETHODCALLTYPE dcomp_translate_transform_AddRef(IDCompositionTranslateTransform *iface)
{
    struct dcomp_translate_transform *t = impl_from_IDCompositionTranslateTransform(iface);
    ULONG ref = InterlockedIncrement(&t->ref);
    TRACE("(%p) ref=%lu\n", iface, ref);
    return ref;
}

static ULONG STDMETHODCALLTYPE dcomp_translate_transform_Release(IDCompositionTranslateTransform *iface)
{
    struct dcomp_translate_transform *t = impl_from_IDCompositionTranslateTransform(iface);
    ULONG ref = InterlockedDecrement(&t->ref);
    TRACE("(%p) ref=%lu\n", iface, ref);
    if (!ref)
        free(t);
    return ref;
}

static HRESULT STDMETHODCALLTYPE dcomp_translate_transform_SetOffsetXAnimation(IDCompositionTranslateTransform *iface, IDCompositionAnimation *a) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_translate_transform_SetOffsetX(IDCompositionTranslateTransform *iface, float o) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_translate_transform_SetOffsetYAnimation(IDCompositionTranslateTransform *iface, IDCompositionAnimation *a) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_translate_transform_SetOffsetY(IDCompositionTranslateTransform *iface, float o) { return S_OK; }

static const IDCompositionTranslateTransformVtbl dcomp_translate_transform_vtbl =
{
    dcomp_translate_transform_QueryInterface,
    dcomp_translate_transform_AddRef,
    dcomp_translate_transform_Release,
    dcomp_translate_transform_SetOffsetXAnimation,
    dcomp_translate_transform_SetOffsetX,
    dcomp_translate_transform_SetOffsetYAnimation,
    dcomp_translate_transform_SetOffsetY,
};

static HRESULT dcomp_translate_transform_create(IDCompositionTranslateTransform **out)
{
    struct dcomp_translate_transform *t = calloc(1, sizeof(*t));
    if (!t)
        return E_OUTOFMEMORY;
    t->IDCompositionTranslateTransform_iface.lpVtbl = &dcomp_translate_transform_vtbl;
    t->ref = 1;
    *out = &t->IDCompositionTranslateTransform_iface;
    return S_OK;
}

/* --- IDCompositionScaleTransform --- */
struct dcomp_scale_transform
{
    IDCompositionScaleTransform IDCompositionScaleTransform_iface;
    LONG ref;
};

static inline struct dcomp_scale_transform *impl_from_IDCompositionScaleTransform(IDCompositionScaleTransform *iface)
{
    return CONTAINING_RECORD(iface, struct dcomp_scale_transform, IDCompositionScaleTransform_iface);
}

static HRESULT STDMETHODCALLTYPE dcomp_scale_transform_QueryInterface(IDCompositionScaleTransform *iface,
    REFIID riid, void **out)
{
    TRACE("(%p, %s, %p)\n", iface, debugstr_guid(riid), out);
    if (IsEqualIID(riid, &IID_IUnknown) ||
        IsEqualIID(riid, &IID_IDCompositionEffect) ||
        IsEqualIID(riid, &IID_IDCompositionTransform3D) ||
        IsEqualIID(riid, &IID_IDCompositionTransform) ||
        IsEqualIID(riid, &IID_IDCompositionScaleTransform))
    {
        *out = iface;
        IDCompositionScaleTransform_AddRef(iface);
        return S_OK;
    }
    WARN("Unsupported interface %s\n", debugstr_guid(riid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG STDMETHODCALLTYPE dcomp_scale_transform_AddRef(IDCompositionScaleTransform *iface)
{
    struct dcomp_scale_transform *t = impl_from_IDCompositionScaleTransform(iface);
    ULONG ref = InterlockedIncrement(&t->ref);
    TRACE("(%p) ref=%lu\n", iface, ref);
    return ref;
}

static ULONG STDMETHODCALLTYPE dcomp_scale_transform_Release(IDCompositionScaleTransform *iface)
{
    struct dcomp_scale_transform *t = impl_from_IDCompositionScaleTransform(iface);
    ULONG ref = InterlockedDecrement(&t->ref);
    TRACE("(%p) ref=%lu\n", iface, ref);
    if (!ref)
        free(t);
    return ref;
}

static HRESULT STDMETHODCALLTYPE dcomp_scale_transform_SetScaleXAnimation(IDCompositionScaleTransform *iface, IDCompositionAnimation *a) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_scale_transform_SetScaleX(IDCompositionScaleTransform *iface, float s) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_scale_transform_SetScaleYAnimation(IDCompositionScaleTransform *iface, IDCompositionAnimation *a) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_scale_transform_SetScaleY(IDCompositionScaleTransform *iface, float s) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_scale_transform_SetCenterXAnimation(IDCompositionScaleTransform *iface, IDCompositionAnimation *a) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_scale_transform_SetCenterX(IDCompositionScaleTransform *iface, float c) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_scale_transform_SetCenterYAnimation(IDCompositionScaleTransform *iface, IDCompositionAnimation *a) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_scale_transform_SetCenterY(IDCompositionScaleTransform *iface, float c) { return S_OK; }

static const IDCompositionScaleTransformVtbl dcomp_scale_transform_vtbl =
{
    dcomp_scale_transform_QueryInterface,
    dcomp_scale_transform_AddRef,
    dcomp_scale_transform_Release,
    dcomp_scale_transform_SetScaleXAnimation,
    dcomp_scale_transform_SetScaleX,
    dcomp_scale_transform_SetScaleYAnimation,
    dcomp_scale_transform_SetScaleY,
    dcomp_scale_transform_SetCenterXAnimation,
    dcomp_scale_transform_SetCenterX,
    dcomp_scale_transform_SetCenterYAnimation,
    dcomp_scale_transform_SetCenterY,
};

static HRESULT dcomp_scale_transform_create(IDCompositionScaleTransform **out)
{
    struct dcomp_scale_transform *t = calloc(1, sizeof(*t));
    if (!t)
        return E_OUTOFMEMORY;
    t->IDCompositionScaleTransform_iface.lpVtbl = &dcomp_scale_transform_vtbl;
    t->ref = 1;
    *out = &t->IDCompositionScaleTransform_iface;
    return S_OK;
}

/* --- IDCompositionRotateTransform --- */
struct dcomp_rotate_transform
{
    IDCompositionRotateTransform IDCompositionRotateTransform_iface;
    LONG ref;
};

static inline struct dcomp_rotate_transform *impl_from_IDCompositionRotateTransform(IDCompositionRotateTransform *iface)
{
    return CONTAINING_RECORD(iface, struct dcomp_rotate_transform, IDCompositionRotateTransform_iface);
}

static HRESULT STDMETHODCALLTYPE dcomp_rotate_transform_QueryInterface(IDCompositionRotateTransform *iface,
    REFIID riid, void **out)
{
    TRACE("(%p, %s, %p)\n", iface, debugstr_guid(riid), out);
    if (IsEqualIID(riid, &IID_IUnknown) ||
        IsEqualIID(riid, &IID_IDCompositionEffect) ||
        IsEqualIID(riid, &IID_IDCompositionTransform3D) ||
        IsEqualIID(riid, &IID_IDCompositionTransform) ||
        IsEqualIID(riid, &IID_IDCompositionRotateTransform))
    {
        *out = iface;
        IDCompositionRotateTransform_AddRef(iface);
        return S_OK;
    }
    WARN("Unsupported interface %s\n", debugstr_guid(riid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG STDMETHODCALLTYPE dcomp_rotate_transform_AddRef(IDCompositionRotateTransform *iface)
{
    struct dcomp_rotate_transform *t = impl_from_IDCompositionRotateTransform(iface);
    ULONG ref = InterlockedIncrement(&t->ref);
    TRACE("(%p) ref=%lu\n", iface, ref);
    return ref;
}

static ULONG STDMETHODCALLTYPE dcomp_rotate_transform_Release(IDCompositionRotateTransform *iface)
{
    struct dcomp_rotate_transform *t = impl_from_IDCompositionRotateTransform(iface);
    ULONG ref = InterlockedDecrement(&t->ref);
    TRACE("(%p) ref=%lu\n", iface, ref);
    if (!ref)
        free(t);
    return ref;
}

static HRESULT STDMETHODCALLTYPE dcomp_rotate_transform_SetAngleAnimation(IDCompositionRotateTransform *iface, IDCompositionAnimation *a) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_rotate_transform_SetAngle(IDCompositionRotateTransform *iface, float a) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_rotate_transform_SetCenterXAnimation(IDCompositionRotateTransform *iface, IDCompositionAnimation *a) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_rotate_transform_SetCenterX(IDCompositionRotateTransform *iface, float c) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_rotate_transform_SetCenterYAnimation(IDCompositionRotateTransform *iface, IDCompositionAnimation *a) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_rotate_transform_SetCenterY(IDCompositionRotateTransform *iface, float c) { return S_OK; }

static const IDCompositionRotateTransformVtbl dcomp_rotate_transform_vtbl =
{
    dcomp_rotate_transform_QueryInterface,
    dcomp_rotate_transform_AddRef,
    dcomp_rotate_transform_Release,
    dcomp_rotate_transform_SetAngleAnimation,
    dcomp_rotate_transform_SetAngle,
    dcomp_rotate_transform_SetCenterXAnimation,
    dcomp_rotate_transform_SetCenterX,
    dcomp_rotate_transform_SetCenterYAnimation,
    dcomp_rotate_transform_SetCenterY,
};

static HRESULT dcomp_rotate_transform_create(IDCompositionRotateTransform **out)
{
    struct dcomp_rotate_transform *t = calloc(1, sizeof(*t));
    if (!t)
        return E_OUTOFMEMORY;
    t->IDCompositionRotateTransform_iface.lpVtbl = &dcomp_rotate_transform_vtbl;
    t->ref = 1;
    *out = &t->IDCompositionRotateTransform_iface;
    return S_OK;
}

/* --- IDCompositionSkewTransform --- */
struct dcomp_skew_transform
{
    IDCompositionSkewTransform IDCompositionSkewTransform_iface;
    LONG ref;
};

static inline struct dcomp_skew_transform *impl_from_IDCompositionSkewTransform(IDCompositionSkewTransform *iface)
{
    return CONTAINING_RECORD(iface, struct dcomp_skew_transform, IDCompositionSkewTransform_iface);
}

static HRESULT STDMETHODCALLTYPE dcomp_skew_transform_QueryInterface(IDCompositionSkewTransform *iface,
    REFIID riid, void **out)
{
    TRACE("(%p, %s, %p)\n", iface, debugstr_guid(riid), out);
    if (IsEqualIID(riid, &IID_IUnknown) ||
        IsEqualIID(riid, &IID_IDCompositionEffect) ||
        IsEqualIID(riid, &IID_IDCompositionTransform3D) ||
        IsEqualIID(riid, &IID_IDCompositionTransform) ||
        IsEqualIID(riid, &IID_IDCompositionSkewTransform))
    {
        *out = iface;
        IDCompositionSkewTransform_AddRef(iface);
        return S_OK;
    }
    WARN("Unsupported interface %s\n", debugstr_guid(riid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG STDMETHODCALLTYPE dcomp_skew_transform_AddRef(IDCompositionSkewTransform *iface)
{
    struct dcomp_skew_transform *t = impl_from_IDCompositionSkewTransform(iface);
    ULONG ref = InterlockedIncrement(&t->ref);
    TRACE("(%p) ref=%lu\n", iface, ref);
    return ref;
}

static ULONG STDMETHODCALLTYPE dcomp_skew_transform_Release(IDCompositionSkewTransform *iface)
{
    struct dcomp_skew_transform *t = impl_from_IDCompositionSkewTransform(iface);
    ULONG ref = InterlockedDecrement(&t->ref);
    TRACE("(%p) ref=%lu\n", iface, ref);
    if (!ref)
        free(t);
    return ref;
}

static HRESULT STDMETHODCALLTYPE dcomp_skew_transform_SetAngleXAnimation(IDCompositionSkewTransform *iface, IDCompositionAnimation *a) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_skew_transform_SetAngleX(IDCompositionSkewTransform *iface, float a) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_skew_transform_SetAngleYAnimation(IDCompositionSkewTransform *iface, IDCompositionAnimation *a) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_skew_transform_SetAngleY(IDCompositionSkewTransform *iface, float a) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_skew_transform_SetCenterXAnimation(IDCompositionSkewTransform *iface, IDCompositionAnimation *a) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_skew_transform_SetCenterX(IDCompositionSkewTransform *iface, float c) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_skew_transform_SetCenterYAnimation(IDCompositionSkewTransform *iface, IDCompositionAnimation *a) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_skew_transform_SetCenterY(IDCompositionSkewTransform *iface, float c) { return S_OK; }

static const IDCompositionSkewTransformVtbl dcomp_skew_transform_vtbl =
{
    dcomp_skew_transform_QueryInterface,
    dcomp_skew_transform_AddRef,
    dcomp_skew_transform_Release,
    dcomp_skew_transform_SetAngleXAnimation,
    dcomp_skew_transform_SetAngleX,
    dcomp_skew_transform_SetAngleYAnimation,
    dcomp_skew_transform_SetAngleY,
    dcomp_skew_transform_SetCenterXAnimation,
    dcomp_skew_transform_SetCenterX,
    dcomp_skew_transform_SetCenterYAnimation,
    dcomp_skew_transform_SetCenterY,
};

static HRESULT dcomp_skew_transform_create(IDCompositionSkewTransform **out)
{
    struct dcomp_skew_transform *t = calloc(1, sizeof(*t));
    if (!t)
        return E_OUTOFMEMORY;
    t->IDCompositionSkewTransform_iface.lpVtbl = &dcomp_skew_transform_vtbl;
    t->ref = 1;
    *out = &t->IDCompositionSkewTransform_iface;
    return S_OK;
}

/* --- IDCompositionMatrixTransform --- */
struct dcomp_matrix_transform
{
    IDCompositionMatrixTransform IDCompositionMatrixTransform_iface;
    LONG ref;
};

static inline struct dcomp_matrix_transform *impl_from_IDCompositionMatrixTransform(IDCompositionMatrixTransform *iface)
{
    return CONTAINING_RECORD(iface, struct dcomp_matrix_transform, IDCompositionMatrixTransform_iface);
}

static HRESULT STDMETHODCALLTYPE dcomp_matrix_transform_QueryInterface(IDCompositionMatrixTransform *iface,
    REFIID riid, void **out)
{
    TRACE("(%p, %s, %p)\n", iface, debugstr_guid(riid), out);
    if (IsEqualIID(riid, &IID_IUnknown) ||
        IsEqualIID(riid, &IID_IDCompositionEffect) ||
        IsEqualIID(riid, &IID_IDCompositionTransform3D) ||
        IsEqualIID(riid, &IID_IDCompositionTransform) ||
        IsEqualIID(riid, &IID_IDCompositionMatrixTransform))
    {
        *out = iface;
        IDCompositionMatrixTransform_AddRef(iface);
        return S_OK;
    }
    WARN("Unsupported interface %s\n", debugstr_guid(riid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG STDMETHODCALLTYPE dcomp_matrix_transform_AddRef(IDCompositionMatrixTransform *iface)
{
    struct dcomp_matrix_transform *t = impl_from_IDCompositionMatrixTransform(iface);
    ULONG ref = InterlockedIncrement(&t->ref);
    TRACE("(%p) ref=%lu\n", iface, ref);
    return ref;
}

static ULONG STDMETHODCALLTYPE dcomp_matrix_transform_Release(IDCompositionMatrixTransform *iface)
{
    struct dcomp_matrix_transform *t = impl_from_IDCompositionMatrixTransform(iface);
    ULONG ref = InterlockedDecrement(&t->ref);
    TRACE("(%p) ref=%lu\n", iface, ref);
    if (!ref)
        free(t);
    return ref;
}

static HRESULT STDMETHODCALLTYPE dcomp_matrix_transform_SetMatrix(IDCompositionMatrixTransform *iface, const D2D_MATRIX_3X2_F *m) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_matrix_transform_SetMatrixElementAnimation(IDCompositionMatrixTransform *iface, int r, int c, IDCompositionAnimation *a) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_matrix_transform_SetMatrixElement(IDCompositionMatrixTransform *iface, int r, int c, float v) { return S_OK; }

static const IDCompositionMatrixTransformVtbl dcomp_matrix_transform_vtbl =
{
    dcomp_matrix_transform_QueryInterface,
    dcomp_matrix_transform_AddRef,
    dcomp_matrix_transform_Release,
    dcomp_matrix_transform_SetMatrix,
    dcomp_matrix_transform_SetMatrixElementAnimation,
    dcomp_matrix_transform_SetMatrixElement,
};

static HRESULT dcomp_matrix_transform_create(IDCompositionMatrixTransform **out)
{
    struct dcomp_matrix_transform *t = calloc(1, sizeof(*t));
    if (!t)
        return E_OUTOFMEMORY;
    t->IDCompositionMatrixTransform_iface.lpVtbl = &dcomp_matrix_transform_vtbl;
    t->ref = 1;
    *out = &t->IDCompositionMatrixTransform_iface;
    return S_OK;
}

/* --- IDCompositionRectangleClip --- */
struct dcomp_rectangle_clip
{
    IDCompositionRectangleClip IDCompositionRectangleClip_iface;
    LONG ref;
};

static inline struct dcomp_rectangle_clip *impl_from_IDCompositionRectangleClip(IDCompositionRectangleClip *iface)
{
    return CONTAINING_RECORD(iface, struct dcomp_rectangle_clip, IDCompositionRectangleClip_iface);
}

static HRESULT STDMETHODCALLTYPE dcomp_rectangle_clip_QueryInterface(IDCompositionRectangleClip *iface,
    REFIID riid, void **out)
{
    TRACE("(%p, %s, %p)\n", iface, debugstr_guid(riid), out);
    if (IsEqualIID(riid, &IID_IUnknown) ||
        IsEqualIID(riid, &IID_IDCompositionClip) ||
        IsEqualIID(riid, &IID_IDCompositionRectangleClip))
    {
        *out = iface;
        IDCompositionRectangleClip_AddRef(iface);
        return S_OK;
    }
    WARN("Unsupported interface %s\n", debugstr_guid(riid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG STDMETHODCALLTYPE dcomp_rectangle_clip_AddRef(IDCompositionRectangleClip *iface)
{
    struct dcomp_rectangle_clip *c = impl_from_IDCompositionRectangleClip(iface);
    ULONG ref = InterlockedIncrement(&c->ref);
    TRACE("(%p) ref=%lu\n", iface, ref);
    return ref;
}

static ULONG STDMETHODCALLTYPE dcomp_rectangle_clip_Release(IDCompositionRectangleClip *iface)
{
    struct dcomp_rectangle_clip *c = impl_from_IDCompositionRectangleClip(iface);
    ULONG ref = InterlockedDecrement(&c->ref);
    TRACE("(%p) ref=%lu\n", iface, ref);
    if (!ref)
        free(c);
    return ref;
}

static HRESULT STDMETHODCALLTYPE dcomp_rectangle_clip_SetLeftAnimation(IDCompositionRectangleClip *iface, IDCompositionAnimation *a) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_rectangle_clip_SetLeft(IDCompositionRectangleClip *iface, float v) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_rectangle_clip_SetTopAnimation(IDCompositionRectangleClip *iface, IDCompositionAnimation *a) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_rectangle_clip_SetTop(IDCompositionRectangleClip *iface, float v) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_rectangle_clip_SetRightAnimation(IDCompositionRectangleClip *iface, IDCompositionAnimation *a) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_rectangle_clip_SetRight(IDCompositionRectangleClip *iface, float v) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_rectangle_clip_SetBottomAnimation(IDCompositionRectangleClip *iface, IDCompositionAnimation *a) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_rectangle_clip_SetBottom(IDCompositionRectangleClip *iface, float v) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_rectangle_clip_SetTopLeftRadiusXAnimation(IDCompositionRectangleClip *iface, IDCompositionAnimation *a) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_rectangle_clip_SetTopLeftRadiusX(IDCompositionRectangleClip *iface, float v) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_rectangle_clip_SetTopLeftRadiusYAnimation(IDCompositionRectangleClip *iface, IDCompositionAnimation *a) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_rectangle_clip_SetTopLeftRadiusY(IDCompositionRectangleClip *iface, float v) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_rectangle_clip_SetTopRightRadiusXAnimation(IDCompositionRectangleClip *iface, IDCompositionAnimation *a) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_rectangle_clip_SetTopRightRadiusX(IDCompositionRectangleClip *iface, float v) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_rectangle_clip_SetTopRightRadiusYAnimation(IDCompositionRectangleClip *iface, IDCompositionAnimation *a) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_rectangle_clip_SetTopRightRadiusY(IDCompositionRectangleClip *iface, float v) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_rectangle_clip_SetBottomLeftRadiusXAnimation(IDCompositionRectangleClip *iface, IDCompositionAnimation *a) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_rectangle_clip_SetBottomLeftRadiusX(IDCompositionRectangleClip *iface, float v) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_rectangle_clip_SetBottomLeftRadiusYAnimation(IDCompositionRectangleClip *iface, IDCompositionAnimation *a) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_rectangle_clip_SetBottomLeftRadiusY(IDCompositionRectangleClip *iface, float v) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_rectangle_clip_SetBottomRightRadiusXAnimation(IDCompositionRectangleClip *iface, IDCompositionAnimation *a) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_rectangle_clip_SetBottomRightRadiusX(IDCompositionRectangleClip *iface, float v) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_rectangle_clip_SetBottomRightRadiusYAnimation(IDCompositionRectangleClip *iface, IDCompositionAnimation *a) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_rectangle_clip_SetBottomRightRadiusY(IDCompositionRectangleClip *iface, float v) { return S_OK; }

static const IDCompositionRectangleClipVtbl dcomp_rectangle_clip_vtbl =
{
    dcomp_rectangle_clip_QueryInterface,
    dcomp_rectangle_clip_AddRef,
    dcomp_rectangle_clip_Release,
    dcomp_rectangle_clip_SetLeftAnimation,
    dcomp_rectangle_clip_SetLeft,
    dcomp_rectangle_clip_SetTopAnimation,
    dcomp_rectangle_clip_SetTop,
    dcomp_rectangle_clip_SetRightAnimation,
    dcomp_rectangle_clip_SetRight,
    dcomp_rectangle_clip_SetBottomAnimation,
    dcomp_rectangle_clip_SetBottom,
    dcomp_rectangle_clip_SetTopLeftRadiusXAnimation,
    dcomp_rectangle_clip_SetTopLeftRadiusX,
    dcomp_rectangle_clip_SetTopLeftRadiusYAnimation,
    dcomp_rectangle_clip_SetTopLeftRadiusY,
    dcomp_rectangle_clip_SetTopRightRadiusXAnimation,
    dcomp_rectangle_clip_SetTopRightRadiusX,
    dcomp_rectangle_clip_SetTopRightRadiusYAnimation,
    dcomp_rectangle_clip_SetTopRightRadiusY,
    dcomp_rectangle_clip_SetBottomLeftRadiusXAnimation,
    dcomp_rectangle_clip_SetBottomLeftRadiusX,
    dcomp_rectangle_clip_SetBottomLeftRadiusYAnimation,
    dcomp_rectangle_clip_SetBottomLeftRadiusY,
    dcomp_rectangle_clip_SetBottomRightRadiusXAnimation,
    dcomp_rectangle_clip_SetBottomRightRadiusX,
    dcomp_rectangle_clip_SetBottomRightRadiusYAnimation,
    dcomp_rectangle_clip_SetBottomRightRadiusY,
};

static HRESULT dcomp_rectangle_clip_create(IDCompositionRectangleClip **out)
{
    struct dcomp_rectangle_clip *clip = calloc(1, sizeof(*clip));
    if (!clip)
        return E_OUTOFMEMORY;
    clip->IDCompositionRectangleClip_iface.lpVtbl = &dcomp_rectangle_clip_vtbl;
    clip->ref = 1;
    *out = &clip->IDCompositionRectangleClip_iface;
    return S_OK;
}

/* --- IDCompositionAnimation --- */
struct dcomp_animation
{
    IDCompositionAnimation IDCompositionAnimation_iface;
    LONG ref;
};

static inline struct dcomp_animation *impl_from_IDCompositionAnimation(IDCompositionAnimation *iface)
{
    return CONTAINING_RECORD(iface, struct dcomp_animation, IDCompositionAnimation_iface);
}

static HRESULT STDMETHODCALLTYPE dcomp_animation_QueryInterface(IDCompositionAnimation *iface,
    REFIID riid, void **out)
{
    TRACE("(%p, %s, %p)\n", iface, debugstr_guid(riid), out);
    if (IsEqualIID(riid, &IID_IUnknown) ||
        IsEqualIID(riid, &IID_IDCompositionAnimation))
    {
        *out = iface;
        IDCompositionAnimation_AddRef(iface);
        return S_OK;
    }
    WARN("Unsupported interface %s\n", debugstr_guid(riid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG STDMETHODCALLTYPE dcomp_animation_AddRef(IDCompositionAnimation *iface)
{
    struct dcomp_animation *a = impl_from_IDCompositionAnimation(iface);
    ULONG ref = InterlockedIncrement(&a->ref);
    TRACE("(%p) ref=%lu\n", iface, ref);
    return ref;
}

static ULONG STDMETHODCALLTYPE dcomp_animation_Release(IDCompositionAnimation *iface)
{
    struct dcomp_animation *a = impl_from_IDCompositionAnimation(iface);
    ULONG ref = InterlockedDecrement(&a->ref);
    TRACE("(%p) ref=%lu\n", iface, ref);
    if (!ref)
        free(a);
    return ref;
}

static HRESULT STDMETHODCALLTYPE dcomp_animation_Reset(IDCompositionAnimation *iface) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_animation_SetAbsoluteBeginTime(IDCompositionAnimation *iface, LARGE_INTEGER time) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_animation_AddCubic(IDCompositionAnimation *iface, double offset, float c0, float c1, float c2, float c3) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_animation_AddSinusoidal(IDCompositionAnimation *iface, double offset, float bias, float amp, float freq, float phase) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_animation_AddRepeat(IDCompositionAnimation *iface, double offset, double duration) { return S_OK; }
static HRESULT STDMETHODCALLTYPE dcomp_animation_End(IDCompositionAnimation *iface, double offset, float value) { return S_OK; }

static const IDCompositionAnimationVtbl dcomp_animation_vtbl =
{
    dcomp_animation_QueryInterface,
    dcomp_animation_AddRef,
    dcomp_animation_Release,
    dcomp_animation_Reset,
    dcomp_animation_SetAbsoluteBeginTime,
    dcomp_animation_AddCubic,
    dcomp_animation_AddSinusoidal,
    dcomp_animation_AddRepeat,
    dcomp_animation_End,
};

static HRESULT dcomp_animation_create(IDCompositionAnimation **out)
{
    struct dcomp_animation *a = calloc(1, sizeof(*a));
    if (!a)
        return E_OUTOFMEMORY;
    a->IDCompositionAnimation_iface.lpVtbl = &dcomp_animation_vtbl;
    a->ref = 1;
    *out = &a->IDCompositionAnimation_iface;
    return S_OK;
}

/* --- IDCompositionTransform (Generic/Group) --- */
struct dcomp_transform
{
    IDCompositionTransform IDCompositionTransform_iface;
    LONG ref;
};

static inline struct dcomp_transform *impl_from_IDCompositionTransform(IDCompositionTransform *iface)
{
    return CONTAINING_RECORD(iface, struct dcomp_transform, IDCompositionTransform_iface);
}

static HRESULT STDMETHODCALLTYPE dcomp_transform_QueryInterface(IDCompositionTransform *iface,
    REFIID riid, void **out)
{
    TRACE("(%p, %s, %p)\n", iface, debugstr_guid(riid), out);
    if (IsEqualIID(riid, &IID_IUnknown) ||
        IsEqualIID(riid, &IID_IDCompositionEffect) ||
        IsEqualIID(riid, &IID_IDCompositionTransform3D) ||
        IsEqualIID(riid, &IID_IDCompositionTransform))
    {
        *out = iface;
        IDCompositionTransform_AddRef(iface);
        return S_OK;
    }
    WARN("Unsupported interface %s\n", debugstr_guid(riid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG STDMETHODCALLTYPE dcomp_transform_AddRef(IDCompositionTransform *iface)
{
    struct dcomp_transform *t = impl_from_IDCompositionTransform(iface);
    ULONG ref = InterlockedIncrement(&t->ref);
    TRACE("(%p) ref=%lu\n", iface, ref);
    return ref;
}

static ULONG STDMETHODCALLTYPE dcomp_transform_Release(IDCompositionTransform *iface)
{
    struct dcomp_transform *t = impl_from_IDCompositionTransform(iface);
    ULONG ref = InterlockedDecrement(&t->ref);
    TRACE("(%p) ref=%lu\n", iface, ref);
    if (!ref)
        free(t);
    return ref;
}

static const IDCompositionTransformVtbl dcomp_transform_vtbl =
{
    dcomp_transform_QueryInterface,
    dcomp_transform_AddRef,
    dcomp_transform_Release,
};

static HRESULT dcomp_transform_create(IDCompositionTransform **out)
{
    struct dcomp_transform *t = calloc(1, sizeof(*t));
    if (!t)
        return E_OUTOFMEMORY;
    t->IDCompositionTransform_iface.lpVtbl = &dcomp_transform_vtbl;
    t->ref = 1;
    *out = &t->IDCompositionTransform_iface;
    return S_OK;
}

static HRESULT STDMETHODCALLTYPE dcomp_desktop_device_CreateTranslateTransform(IDCompositionDesktopDevice *iface,
    IDCompositionTranslateTransform **transform)
{
    TRACE("(%p, %p)\n", iface, transform);
    if (!transform)
        return E_POINTER;
    return dcomp_translate_transform_create(transform);
}

static HRESULT STDMETHODCALLTYPE dcomp_desktop_device_CreateScaleTransform(IDCompositionDesktopDevice *iface,
    IDCompositionScaleTransform **transform)
{
    TRACE("(%p, %p)\n", iface, transform);
    if (!transform)
        return E_POINTER;
    return dcomp_scale_transform_create(transform);
}

static HRESULT STDMETHODCALLTYPE dcomp_desktop_device_CreateRotateTransform(IDCompositionDesktopDevice *iface,
    IDCompositionRotateTransform **transform)
{
    TRACE("(%p, %p)\n", iface, transform);
    if (!transform)
        return E_POINTER;
    return dcomp_rotate_transform_create(transform);
}

static HRESULT STDMETHODCALLTYPE dcomp_desktop_device_CreateSkewTransform(IDCompositionDesktopDevice *iface,
    IDCompositionSkewTransform **transform)
{
    TRACE("(%p, %p)\n", iface, transform);
    if (!transform)
        return E_POINTER;
    return dcomp_skew_transform_create(transform);
}

static HRESULT STDMETHODCALLTYPE dcomp_desktop_device_CreateMatrixTransform(IDCompositionDesktopDevice *iface,
    IDCompositionMatrixTransform **transform)
{
    TRACE("(%p, %p)\n", iface, transform);
    if (!transform)
        return E_POINTER;
    return dcomp_matrix_transform_create(transform);
}

static HRESULT STDMETHODCALLTYPE dcomp_desktop_device_CreateTransformGroup(IDCompositionDesktopDevice *iface,
    IDCompositionTransform **transforms, UINT elements, IDCompositionTransform **transform_group)
{
    TRACE("(%p, %p, %u, %p)\n", iface, transforms, elements, transform_group);
    if (!transform_group)
        return E_POINTER;
    return dcomp_transform_create(transform_group);
}

static HRESULT STDMETHODCALLTYPE dcomp_desktop_device_CreateTranslateTransform3D(IDCompositionDesktopDevice *iface,
    IDCompositionTranslateTransform3D **transform_3d)
{
    TRACE("(%p, %p)\n", iface, transform_3d);
    if (!transform_3d)
        return E_POINTER;
    return dcomp_transform_create((IDCompositionTransform **)transform_3d);
}

static HRESULT STDMETHODCALLTYPE dcomp_desktop_device_CreateScaleTransform3D(IDCompositionDesktopDevice *iface,
    IDCompositionScaleTransform3D **transform_3d)
{
    TRACE("(%p, %p)\n", iface, transform_3d);
    if (!transform_3d)
        return E_POINTER;
    return dcomp_transform_create((IDCompositionTransform **)transform_3d);
}

static HRESULT STDMETHODCALLTYPE dcomp_desktop_device_CreateRotateTransform3D(IDCompositionDesktopDevice *iface,
    IDCompositionRotateTransform3D **transform_3d)
{
    TRACE("(%p, %p)\n", iface, transform_3d);
    if (!transform_3d)
        return E_POINTER;
    return dcomp_transform_create((IDCompositionTransform **)transform_3d);
}

static HRESULT STDMETHODCALLTYPE dcomp_desktop_device_CreateMatrixTransform3D(IDCompositionDesktopDevice *iface,
    IDCompositionMatrixTransform3D **transform_3d)
{
    TRACE("(%p, %p)\n", iface, transform_3d);
    if (!transform_3d)
        return E_POINTER;
    return dcomp_transform_create((IDCompositionTransform **)transform_3d);
}

static HRESULT STDMETHODCALLTYPE dcomp_desktop_device_CreateTransform3DGroup(IDCompositionDesktopDevice *iface,
    IDCompositionTransform3D **transforms_3d, UINT elements, IDCompositionTransform3D **transform_3d_group)
{
    TRACE("(%p, %p, %u, %p)\n", iface, transforms_3d, elements, transform_3d_group);
    if (!transform_3d_group)
        return E_POINTER;
    return dcomp_transform_create((IDCompositionTransform **)transform_3d_group);
}

static HRESULT STDMETHODCALLTYPE dcomp_desktop_device_CreateEffectGroup(IDCompositionDesktopDevice *iface,
    IDCompositionEffectGroup **effect_group)
{
    TRACE("(%p, %p)\n", iface, effect_group);
    if (!effect_group)
        return E_POINTER;
    return dcomp_effect_group_create(effect_group);
}

static HRESULT STDMETHODCALLTYPE dcomp_desktop_device_CreateRectangleClip(IDCompositionDesktopDevice *iface,
    IDCompositionRectangleClip **clip)
{
    TRACE("(%p, %p)\n", iface, clip);
    if (!clip)
        return E_POINTER;
    return dcomp_rectangle_clip_create(clip);
}

static HRESULT STDMETHODCALLTYPE dcomp_desktop_device_CreateAnimation(IDCompositionDesktopDevice *iface,
    IDCompositionAnimation **animation)
{
    TRACE("(%p, %p)\n", iface, animation);
    if (!animation)
        return E_POINTER;
    return dcomp_animation_create(animation);
}

static HRESULT STDMETHODCALLTYPE dcomp_desktop_device_CreateTargetForHwnd(IDCompositionDesktopDevice *iface,
    HWND hwnd, BOOL topmost, IDCompositionTarget **target)
{
    TRACE("(%p, %p, %d, %p)\n", iface, hwnd, topmost, target);
    if (!target)
        return E_POINTER;
    if (hwnd && IsWindow(hwnd))
    {
        SetPropW(GetDesktopWindow(), L"__WineDCompTarget", hwnd);
        TRACE("Stored __WineDCompTarget = %p\n", hwnd);
    }
    return dcomp_target_create(hwnd, target);
}

static HRESULT STDMETHODCALLTYPE dcomp_desktop_device_CreateSurfaceFromHandle(IDCompositionDesktopDevice *iface,
    HANDLE handle, IUnknown **surface)
{
    struct dcomp_device *device = impl_from_IDCompositionDesktopDevice(iface);
    TRACE("(%p, %p, %p)\n", iface, handle, surface);
    if (!surface)
        return E_POINTER;
    return dcomp_surface_create(device->rendering_device, 0, 0, DXGI_FORMAT_UNKNOWN, DXGI_ALPHA_MODE_UNSPECIFIED, (IDCompositionSurface **)surface);
}

static HRESULT STDMETHODCALLTYPE dcomp_desktop_device_CreateSurfaceFromHwnd(IDCompositionDesktopDevice *iface,
    HWND hwnd, IUnknown **surface)
{
    struct dcomp_device *device = impl_from_IDCompositionDesktopDevice(iface);
    TRACE("(%p, %p, %p)\n", iface, hwnd, surface);
    if (!surface)
        return E_POINTER;
    return dcomp_surface_create(device->rendering_device, 0, 0, DXGI_FORMAT_UNKNOWN, DXGI_ALPHA_MODE_UNSPECIFIED, (IDCompositionSurface **)surface);
}

static const IDCompositionDesktopDeviceVtbl dcomp_desktop_device_vtbl =
{
    dcomp_desktop_device_QueryInterface,
    dcomp_desktop_device_AddRef,
    dcomp_desktop_device_Release,
    dcomp_desktop_device_Commit,
    dcomp_desktop_device_WaitForCommitCompletion,
    dcomp_desktop_device_GetFrameStatistics,
    dcomp_desktop_device_CreateVisual,
    dcomp_desktop_device_CreateSurfaceFactory,
    dcomp_desktop_device_CreateSurface,
    dcomp_desktop_device_CreateVirtualSurface,
    dcomp_desktop_device_CreateTranslateTransform,
    dcomp_desktop_device_CreateScaleTransform,
    dcomp_desktop_device_CreateRotateTransform,
    dcomp_desktop_device_CreateSkewTransform,
    dcomp_desktop_device_CreateMatrixTransform,
    dcomp_desktop_device_CreateTransformGroup,
    dcomp_desktop_device_CreateTranslateTransform3D,
    dcomp_desktop_device_CreateScaleTransform3D,
    dcomp_desktop_device_CreateRotateTransform3D,
    dcomp_desktop_device_CreateMatrixTransform3D,
    dcomp_desktop_device_CreateTransform3DGroup,
    dcomp_desktop_device_CreateEffectGroup,
    dcomp_desktop_device_CreateRectangleClip,
    dcomp_desktop_device_CreateAnimation,
    dcomp_desktop_device_CreateTargetForHwnd,
    dcomp_desktop_device_CreateSurfaceFromHandle,
    dcomp_desktop_device_CreateSurfaceFromHwnd,
};

/* --- IDCompositionDevice (v1 wrapper forwarding to desktop device) --- */
static HRESULT STDMETHODCALLTYPE dcomp_device_v1_QueryInterface(IDCompositionDevice *iface, REFIID riid, void **out)
{
    struct dcomp_device *device = impl_from_IDCompositionDevice(iface);
    return IDCompositionDesktopDevice_QueryInterface(&device->IDCompositionDesktopDevice_iface, riid, out);
}

static ULONG STDMETHODCALLTYPE dcomp_device_v1_AddRef(IDCompositionDevice *iface)
{
    struct dcomp_device *device = impl_from_IDCompositionDevice(iface);
    return IDCompositionDesktopDevice_AddRef(&device->IDCompositionDesktopDevice_iface);
}

static ULONG STDMETHODCALLTYPE dcomp_device_v1_Release(IDCompositionDevice *iface)
{
    struct dcomp_device *device = impl_from_IDCompositionDevice(iface);
    return IDCompositionDesktopDevice_Release(&device->IDCompositionDesktopDevice_iface);
}

static HRESULT STDMETHODCALLTYPE dcomp_device_v1_Commit(IDCompositionDevice *iface)
{
    struct dcomp_device *device = impl_from_IDCompositionDevice(iface);
    return IDCompositionDesktopDevice_Commit(&device->IDCompositionDesktopDevice_iface);
}

static HRESULT STDMETHODCALLTYPE dcomp_device_v1_WaitForCommitCompletion(IDCompositionDevice *iface)
{
    struct dcomp_device *device = impl_from_IDCompositionDevice(iface);
    return IDCompositionDesktopDevice_WaitForCommitCompletion(&device->IDCompositionDesktopDevice_iface);
}

static HRESULT STDMETHODCALLTYPE dcomp_device_v1_GetFrameStatistics(IDCompositionDevice *iface, DCOMPOSITION_FRAME_STATISTICS *s)
{
    struct dcomp_device *device = impl_from_IDCompositionDevice(iface);
    return IDCompositionDesktopDevice_GetFrameStatistics(&device->IDCompositionDesktopDevice_iface, s);
}

static HRESULT STDMETHODCALLTYPE dcomp_device_v1_CreateTargetForHwnd(IDCompositionDevice *iface, HWND hwnd, BOOL topmost, IDCompositionTarget **target)
{
    struct dcomp_device *device = impl_from_IDCompositionDevice(iface);
    return IDCompositionDesktopDevice_CreateTargetForHwnd(&device->IDCompositionDesktopDevice_iface, hwnd, topmost, target);
}

static HRESULT STDMETHODCALLTYPE dcomp_device_v1_CreateVisual(IDCompositionDevice *iface, IDCompositionVisual **visual)
{
    struct dcomp_device *device = impl_from_IDCompositionDevice(iface);
    return IDCompositionDesktopDevice_CreateVisual(&device->IDCompositionDesktopDevice_iface, (IDCompositionVisual2 **)visual);
}

static HRESULT STDMETHODCALLTYPE dcomp_device_v1_CreateSurface(IDCompositionDevice *iface,
    UINT width, UINT height, DXGI_FORMAT pixel_format, DXGI_ALPHA_MODE alpha_mode, IDCompositionSurface **surface)
{
    struct dcomp_device *device = impl_from_IDCompositionDevice(iface);
    return IDCompositionDesktopDevice_CreateSurface(&device->IDCompositionDesktopDevice_iface, width, height, pixel_format, alpha_mode, surface);
}

static HRESULT STDMETHODCALLTYPE dcomp_device_v1_CreateVirtualSurface(IDCompositionDevice *iface,
    UINT width, UINT height, DXGI_FORMAT pixel_format, DXGI_ALPHA_MODE alpha_mode, IDCompositionVirtualSurface **surface)
{
    struct dcomp_device *device = impl_from_IDCompositionDevice(iface);
    return IDCompositionDesktopDevice_CreateVirtualSurface(&device->IDCompositionDesktopDevice_iface, width, height, pixel_format, alpha_mode, surface);
}

static HRESULT STDMETHODCALLTYPE dcomp_device_v1_CreateSurfaceFromHandle(IDCompositionDevice *iface, HANDLE handle, IUnknown **surface)
{
    struct dcomp_device *device = impl_from_IDCompositionDevice(iface);
    return IDCompositionDesktopDevice_CreateSurfaceFromHandle(&device->IDCompositionDesktopDevice_iface, handle, surface);
}

static HRESULT STDMETHODCALLTYPE dcomp_device_v1_CreateSurfaceFromHwnd(IDCompositionDevice *iface, HWND hwnd, IUnknown **surface)
{
    struct dcomp_device *device = impl_from_IDCompositionDevice(iface);
    return IDCompositionDesktopDevice_CreateSurfaceFromHwnd(&device->IDCompositionDesktopDevice_iface, hwnd, surface);
}

static HRESULT STDMETHODCALLTYPE dcomp_device_v1_CreateTranslateTransform(IDCompositionDevice *iface, IDCompositionTranslateTransform **t)
{
    struct dcomp_device *device = impl_from_IDCompositionDevice(iface);
    return IDCompositionDesktopDevice_CreateTranslateTransform(&device->IDCompositionDesktopDevice_iface, t);
}

static HRESULT STDMETHODCALLTYPE dcomp_device_v1_CreateScaleTransform(IDCompositionDevice *iface, IDCompositionScaleTransform **t)
{
    struct dcomp_device *device = impl_from_IDCompositionDevice(iface);
    return IDCompositionDesktopDevice_CreateScaleTransform(&device->IDCompositionDesktopDevice_iface, t);
}

static HRESULT STDMETHODCALLTYPE dcomp_device_v1_CreateRotateTransform(IDCompositionDevice *iface, IDCompositionRotateTransform **t)
{
    struct dcomp_device *device = impl_from_IDCompositionDevice(iface);
    return IDCompositionDesktopDevice_CreateRotateTransform(&device->IDCompositionDesktopDevice_iface, t);
}

static HRESULT STDMETHODCALLTYPE dcomp_device_v1_CreateSkewTransform(IDCompositionDevice *iface, IDCompositionSkewTransform **t)
{
    struct dcomp_device *device = impl_from_IDCompositionDevice(iface);
    return IDCompositionDesktopDevice_CreateSkewTransform(&device->IDCompositionDesktopDevice_iface, t);
}

static HRESULT STDMETHODCALLTYPE dcomp_device_v1_CreateMatrixTransform(IDCompositionDevice *iface, IDCompositionMatrixTransform **t)
{
    struct dcomp_device *device = impl_from_IDCompositionDevice(iface);
    return IDCompositionDesktopDevice_CreateMatrixTransform(&device->IDCompositionDesktopDevice_iface, t);
}

static HRESULT STDMETHODCALLTYPE dcomp_device_v1_CreateTransformGroup(IDCompositionDevice *iface, IDCompositionTransform **t, UINT count, IDCompositionTransform **out)
{
    struct dcomp_device *device = impl_from_IDCompositionDevice(iface);
    return IDCompositionDesktopDevice_CreateTransformGroup(&device->IDCompositionDesktopDevice_iface, t, count, out);
}

static HRESULT STDMETHODCALLTYPE dcomp_device_v1_CreateTranslateTransform3D(IDCompositionDevice *iface, IDCompositionTranslateTransform3D **t)
{
    struct dcomp_device *device = impl_from_IDCompositionDevice(iface);
    return IDCompositionDesktopDevice_CreateTranslateTransform3D(&device->IDCompositionDesktopDevice_iface, t);
}

static HRESULT STDMETHODCALLTYPE dcomp_device_v1_CreateScaleTransform3D(IDCompositionDevice *iface, IDCompositionScaleTransform3D **t)
{
    struct dcomp_device *device = impl_from_IDCompositionDevice(iface);
    return IDCompositionDesktopDevice_CreateScaleTransform3D(&device->IDCompositionDesktopDevice_iface, t);
}

static HRESULT STDMETHODCALLTYPE dcomp_device_v1_CreateRotateTransform3D(IDCompositionDevice *iface, IDCompositionRotateTransform3D **t)
{
    struct dcomp_device *device = impl_from_IDCompositionDevice(iface);
    return IDCompositionDesktopDevice_CreateRotateTransform3D(&device->IDCompositionDesktopDevice_iface, t);
}

static HRESULT STDMETHODCALLTYPE dcomp_device_v1_CreateMatrixTransform3D(IDCompositionDevice *iface, IDCompositionMatrixTransform3D **t)
{
    struct dcomp_device *device = impl_from_IDCompositionDevice(iface);
    return IDCompositionDesktopDevice_CreateMatrixTransform3D(&device->IDCompositionDesktopDevice_iface, t);
}

static HRESULT STDMETHODCALLTYPE dcomp_device_v1_CreateTransform3DGroup(IDCompositionDevice *iface, IDCompositionTransform3D **t, UINT count, IDCompositionTransform3D **out)
{
    struct dcomp_device *device = impl_from_IDCompositionDevice(iface);
    return IDCompositionDesktopDevice_CreateTransform3DGroup(&device->IDCompositionDesktopDevice_iface, t, count, out);
}

static HRESULT STDMETHODCALLTYPE dcomp_device_v1_CreateEffectGroup(IDCompositionDevice *iface, IDCompositionEffectGroup **out)
{
    struct dcomp_device *device = impl_from_IDCompositionDevice(iface);
    return IDCompositionDesktopDevice_CreateEffectGroup(&device->IDCompositionDesktopDevice_iface, out);
}

static HRESULT STDMETHODCALLTYPE dcomp_device_v1_CreateRectangleClip(IDCompositionDevice *iface, IDCompositionRectangleClip **out)
{
    struct dcomp_device *device = impl_from_IDCompositionDevice(iface);
    return IDCompositionDesktopDevice_CreateRectangleClip(&device->IDCompositionDesktopDevice_iface, out);
}

static HRESULT STDMETHODCALLTYPE dcomp_device_v1_CreateAnimation(IDCompositionDevice *iface, IDCompositionAnimation **out)
{
    struct dcomp_device *device = impl_from_IDCompositionDevice(iface);
    return IDCompositionDesktopDevice_CreateAnimation(&device->IDCompositionDesktopDevice_iface, out);
}

static HRESULT STDMETHODCALLTYPE dcomp_device_v1_CheckDeviceState(IDCompositionDevice *iface, BOOL *valid)
{
    TRACE("(%p, %p)\n", iface, valid);
    if (valid)
        *valid = TRUE;
    return S_OK;
}

static const IDCompositionDeviceVtbl dcomp_device_v1_vtbl =
{
    dcomp_device_v1_QueryInterface,
    dcomp_device_v1_AddRef,
    dcomp_device_v1_Release,
    dcomp_device_v1_Commit,
    dcomp_device_v1_WaitForCommitCompletion,
    dcomp_device_v1_GetFrameStatistics,
    dcomp_device_v1_CreateTargetForHwnd,
    dcomp_device_v1_CreateVisual,
    dcomp_device_v1_CreateSurface,
    dcomp_device_v1_CreateVirtualSurface,
    dcomp_device_v1_CreateSurfaceFromHandle,
    dcomp_device_v1_CreateSurfaceFromHwnd,
    dcomp_device_v1_CreateTranslateTransform,
    dcomp_device_v1_CreateScaleTransform,
    dcomp_device_v1_CreateRotateTransform,
    dcomp_device_v1_CreateSkewTransform,
    dcomp_device_v1_CreateMatrixTransform,
    dcomp_device_v1_CreateTransformGroup,
    dcomp_device_v1_CreateTranslateTransform3D,
    dcomp_device_v1_CreateScaleTransform3D,
    dcomp_device_v1_CreateRotateTransform3D,
    dcomp_device_v1_CreateMatrixTransform3D,
    dcomp_device_v1_CreateTransform3DGroup,
    dcomp_device_v1_CreateEffectGroup,
    dcomp_device_v1_CreateRectangleClip,
    dcomp_device_v1_CreateAnimation,
    dcomp_device_v1_CheckDeviceState,
};

static HRESULT dcomp_device_create(IUnknown *rendering_device, REFIID iid, void **device_out)
{
    struct dcomp_device *device;
    HRESULT hr;

    init_dcomp_privates_vtables();

    device = calloc(1, sizeof(*device));
    if (!device)
        return E_OUTOFMEMORY;

    device->IDCompositionDesktopDevice_iface.lpVtbl = &dcomp_desktop_device_vtbl;
    device->IDCompositionDevice_iface.lpVtbl = &dcomp_device_v1_vtbl;
    device->privates_batch_query_vtbl = dcomp_privates_batch_query_vtbl;
    device->privates_commit_completion_vtbl = dcomp_privates_commit_completion_vtbl;
    device->ref = 1;
    device->commit_event = NULL;

    if (rendering_device)
    {
        device->rendering_device = rendering_device;
        IUnknown_AddRef(rendering_device);
    }

    hr = IDCompositionDesktopDevice_QueryInterface(&device->IDCompositionDesktopDevice_iface, iid, device_out);
    IDCompositionDesktopDevice_Release(&device->IDCompositionDesktopDevice_iface);
    return hr;
}

/* --- Public DComposition Exports --- */
HRESULT WINAPI DCompositionCreateDevice(IDXGIDevice *dxgi_device, REFIID iid, void **device)
{
    TRACE("%p, %s, %p.\n", dxgi_device, debugstr_guid(iid), device);
    if (!device)
        return E_POINTER;
    return dcomp_device_create((IUnknown *)dxgi_device, iid, device);
}

HRESULT WINAPI DCompositionCreateDevice2(IUnknown *rendering_device, REFIID iid, void **device)
{
    TRACE("%p, %s, %p.\n", rendering_device, debugstr_guid(iid), device);
    if (!device)
        return E_POINTER;
    return dcomp_device_create(rendering_device, iid, device);
}

HRESULT WINAPI DCompositionCreateDevice3(IUnknown *rendering_device, REFIID iid, void **device)
{
    TRACE("%p, %s, %p.\n", rendering_device, debugstr_guid(iid), device);
    if (!device)
        return E_POINTER;
    return dcomp_device_create(rendering_device, iid, device);
}

/* Ordinal 1040: DCompositionCreateSharedVisualHandle
 * DirectComposition visual handle creator forwarding to win32k / dwmcore.
 * Consumed by Microsoft.Internal.FrameworkUdk.dll and dcompi.dll. */
HRESULT WINAPI DCompositionCreateSharedVisualHandle(HANDLE *out_handle)
{
    HANDLE handle;

    TRACE("(%p)\n", out_handle);
    if (!out_handle)
        return E_POINTER;

    handle = CreateEventW(NULL, FALSE, FALSE, NULL);
    if (!handle)
    {
        WARN("failed to create visual handle event\n");
        return E_FAIL;
    }
    *out_handle = handle;
    TRACE("created shared visual handle %p\n", handle);
    return S_OK;
}

/* Ordinal 1038: DComposition input site / external output site helper
 * Called by Microsoft.UI.Input.dll to hook ContentExternalOutputSite into the visual tree. */
struct dcomp_input_site
{
    void **vtbl;
    LONG ref;
};

static HRESULT WINAPI dcomp_input_site_QueryInterface(void *iface, REFIID iid, void **out)
{
    TRACE("(%p, %s, %p)\n", iface, debugstr_guid(iid), out);
    if (!out)
        return E_POINTER;
    *out = iface;
    return S_OK;
}

static ULONG WINAPI dcomp_input_site_AddRef(void *iface)
{
    return 2;
}

static ULONG WINAPI dcomp_input_site_Release(void *iface)
{
    return 1;
}

static HRESULT WINAPI dcomp_input_site_Method(void *iface, void *arg1, void *arg2, void *arg3, void *arg4)
{
    TRACE("(%p, %p, %p, %p, %p)\n", iface, arg1, arg2, arg3, arg4);
    return S_OK;
}

static void *dcomp_input_site_methods[32] =
{
    dcomp_input_site_QueryInterface,
    dcomp_input_site_AddRef,
    dcomp_input_site_Release,
    dcomp_input_site_Method, /* method 3 (called at 0x1800d1b42) */
    dcomp_input_site_Method,
    dcomp_input_site_Method,
    dcomp_input_site_Method,
    dcomp_input_site_Method,
    dcomp_input_site_Method,
    dcomp_input_site_Method,
    dcomp_input_site_Method,
    dcomp_input_site_Method,
    dcomp_input_site_Method,
    dcomp_input_site_Method,
    dcomp_input_site_Method,
    dcomp_input_site_Method,
    dcomp_input_site_Method,
    dcomp_input_site_Method,
    dcomp_input_site_Method,
    dcomp_input_site_Method,
    dcomp_input_site_Method,
    dcomp_input_site_Method,
    dcomp_input_site_Method,
    dcomp_input_site_Method,
    dcomp_input_site_Method,
    dcomp_input_site_Method,
    dcomp_input_site_Method,
    dcomp_input_site_Method,
    dcomp_input_site_Method,
    dcomp_input_site_Method,
    dcomp_input_site_Method,
    dcomp_input_site_Method,
};

static struct dcomp_input_site default_dcomp_input_site = { dcomp_input_site_methods, 1 };

HRESULT WINAPI DCompositionOrdinal1038(void *arg1, void **out_obj)
{
    TRACE("(%p, %p)\n", arg1, out_obj);
    if (!out_obj)
        return E_POINTER;
    *out_obj = &default_dcomp_input_site;
    return S_OK;
}
