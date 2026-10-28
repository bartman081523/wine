/*
 * WinRT Windows.ApplicationModel.Background.BackgroundTaskRegistration Implementation
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
#include <initguid.h>
#include <eventtoken.h>
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(model);

/* GUID definitions */
static const GUID IID_IBackgroundTaskRegistrationStatics = {0x4c542f69, 0xb000, 0x42ba, {0xa0, 0x93, 0x6a, 0x56, 0x3c, 0x65, 0xe3, 0xf8}};
static const GUID IID_IBackgroundTaskRegistration = {0x1062f222, 0xa27e, 0x43cf, {0x8c, 0x12, 0x1f, 0xb0, 0xb3, 0xbf, 0xa0, 0xa0}};

/* IMapView<GUID, IBackgroundTaskRegistration*> */
static const GUID IID_IMapView_GUID_IBackgroundTaskRegistration1 = {0x605a73e8, 0xe975, 0x5efd, {0xb3, 0x87, 0x37, 0x5b, 0x29, 0x5c, 0x30, 0xee}};
static const GUID IID_IMapView_GUID_IBackgroundTaskRegistration2 = {0xb3bd36fa, 0x170b, 0x5131, {0x89, 0xa7, 0x01, 0xbb, 0x06, 0xad, 0x9a, 0x87}};

/* IIterable<IKeyValuePair<GUID, IBackgroundTaskRegistration*>*> */
static const GUID IID_IIterable_IKeyValuePair_GUID_IBackgroundTaskRegistration1 = {0x7189b3e8, 0xafc7, 0x54ae, {0xae, 0xfd, 0xaa, 0x80, 0x55, 0x9d, 0x38, 0x4c}};
static const GUID IID_IIterable_IKeyValuePair_GUID_IBackgroundTaskRegistration2 = {0x93bc8b49, 0x403a, 0x519f, {0xa1, 0xbc, 0xac, 0xcc, 0x81, 0x54, 0x60, 0x8f}};

/* IIterator<IKeyValuePair<GUID, IBackgroundTaskRegistration*>*> */
static const GUID IID_IIterator_IKeyValuePair_GUID_IBackgroundTaskRegistration = {0x6a79e863, 0x4300, 0x459a, {0x99, 0x66, 0xbf, 0x68, 0xfc, 0x96, 0x42, 0x45}};

#define RuntimeClass_Windows_ApplicationModel_Background_BackgroundTaskRegistration \
    L"Windows.ApplicationModel.Background.BackgroundTaskRegistration"

/* IIterator_empty declarations */
typedef struct IIterator_empty IIterator_empty;

typedef struct IIterator_emptyVtbl
{
    /* IUnknown */
    HRESULT (WINAPI *QueryInterface)(IIterator_empty *This, REFIID riid, void **ppvObject);
    ULONG   (WINAPI *AddRef)(IIterator_empty *This);
    ULONG   (WINAPI *Release)(IIterator_empty *This);
    /* IInspectable */
    HRESULT (WINAPI *GetIids)(IIterator_empty *This, ULONG *iidCount, IID **iids);
    HRESULT (WINAPI *GetRuntimeClassName)(IIterator_empty *This, HSTRING *className);
    HRESULT (WINAPI *GetTrustLevel)(IIterator_empty *This, TrustLevel *trustLevel);
    /* IIterator */
    HRESULT (WINAPI *get_Current)(IIterator_empty *This, void **value);
    HRESULT (WINAPI *get_HasCurrent)(IIterator_empty *This, BOOLEAN *value);
    HRESULT (WINAPI *MoveNext)(IIterator_empty *This, BOOLEAN *value);
    HRESULT (WINAPI *GetMany)(IIterator_empty *This, UINT32 items_size, void **items, UINT32 *count);
} IIterator_emptyVtbl;

struct IIterator_empty
{
    const IIterator_emptyVtbl *lpVtbl;
};

struct empty_iterator
{
    IIterator_empty IIterator_iface;
    LONG ref;
};

static inline struct empty_iterator *impl_from_IIterator_empty(IIterator_empty *iface)
{
    return CONTAINING_RECORD(iface, struct empty_iterator, IIterator_iface);
}

static HRESULT WINAPI iterator_QueryInterface(IIterator_empty *iface, REFIID iid, void **out)
{
    struct empty_iterator *impl = impl_from_IIterator_empty(iface);
    static const GUID IID_ComWrappers_internal = {0x11d3b13a, 0x180e, 0x4789, {0xa8, 0xbe, 0x77, 0x12, 0x88, 0x28, 0x93, 0xe6}};
    static const GUID IID_ComWrappers_internal2 = {0x5c13e51c, 0x241d, 0x4859, {0x80, 0x05, 0x8a, 0x8d, 0x8d, 0x98, 0x15, 0xbf}};

    MESSAGE("background_task: iterator_QueryInterface iid %s\n", debugstr_guid(iid));
    if (!out) return E_INVALIDARG;

    if (IsEqualIID(iid, &IID_ComWrappers_internal) || IsEqualIID(iid, &IID_ComWrappers_internal2))
    {
        *out = NULL;
        return E_NOINTERFACE;
    }

    *out = &impl->IIterator_iface;
    IUnknown_AddRef((IUnknown *)*out);
    return S_OK;
}

static ULONG WINAPI iterator_AddRef(IIterator_empty *iface)
{
    struct empty_iterator *impl = impl_from_IIterator_empty(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI iterator_Release(IIterator_empty *iface)
{
    struct empty_iterator *impl = impl_from_IIterator_empty(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI iterator_GetIids(IIterator_empty *iface, ULONG *iid_count, IID **iids)
{
    IID *array;
    if (!iid_count || !iids) return E_INVALIDARG;
    if (!(array = CoTaskMemAlloc(2 * sizeof(IID)))) return E_OUTOFMEMORY;
    array[0] = IID_IAgileObject;
    array[1] = IID_IIterator_IKeyValuePair_GUID_IBackgroundTaskRegistration;
    *iid_count = 2;
    *iids = array;
    return S_OK;
}

static HRESULT WINAPI iterator_GetRuntimeClassName(IIterator_empty *iface, HSTRING *class_name)
{
    static const WCHAR name[] = L"Windows.Foundation.Collections.IIterator`1<Windows.Foundation.Collections.IKeyValuePair`2<Guid, Windows.ApplicationModel.Background.BackgroundTaskRegistration>>";
    return WindowsCreateString(name, ARRAY_SIZE(name) - 1, class_name);
}

static HRESULT WINAPI iterator_GetTrustLevel(IIterator_empty *iface, TrustLevel *trust_level)
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI iterator_get_Current(IIterator_empty *iface, void **value)
{
    if (!value) return E_INVALIDARG;
    *value = NULL;
    return E_BOUNDS;
}

static HRESULT WINAPI iterator_get_HasCurrent(IIterator_empty *iface, BOOLEAN *value)
{
    MESSAGE("background_task: iterator_get_HasCurrent returning FALSE\n");
    if (!value) return E_INVALIDARG;
    *value = FALSE;
    return S_OK;
}

static HRESULT WINAPI iterator_MoveNext(IIterator_empty *iface, BOOLEAN *value)
{
    MESSAGE("background_task: iterator_MoveNext returning FALSE\n");
    if (!value) return E_INVALIDARG;
    *value = FALSE;
    return S_OK;
}

static HRESULT WINAPI iterator_GetMany(IIterator_empty *iface, UINT32 items_size, void **items, UINT32 *count)
{
    MESSAGE("background_task: iterator_GetMany returning 0\n");
    if (!count) return E_INVALIDARG;
    *count = 0;
    return S_OK;
}

static const struct IIterator_emptyVtbl iterator_vtbl =
{
    iterator_QueryInterface,
    iterator_AddRef,
    iterator_Release,
    iterator_GetIids,
    iterator_GetRuntimeClassName,
    iterator_GetTrustLevel,
    iterator_get_Current,
    iterator_get_HasCurrent,
    iterator_MoveNext,
    iterator_GetMany,
};

static HRESULT empty_iterator_create(void **out)
{
    struct empty_iterator *impl;
    if (!out) return E_INVALIDARG;
    if (!(impl = calloc(1, sizeof(*impl)))) return E_OUTOFMEMORY;
    impl->IIterator_iface.lpVtbl = &iterator_vtbl;
    impl->ref = 1;
    *out = &impl->IIterator_iface;
    return S_OK;
}

/* IMapView_empty and IIterable_empty declarations */
typedef struct IMapView_empty IMapView_empty;

typedef struct IMapView_emptyVtbl
{
    /* IUnknown */
    HRESULT (WINAPI *QueryInterface)(IMapView_empty *This, REFIID riid, void **ppvObject);
    ULONG   (WINAPI *AddRef)(IMapView_empty *This);
    ULONG   (WINAPI *Release)(IMapView_empty *This);
    /* IInspectable */
    HRESULT (WINAPI *GetIids)(IMapView_empty *This, ULONG *iidCount, IID **iids);
    HRESULT (WINAPI *GetRuntimeClassName)(IMapView_empty *This, HSTRING *className);
    HRESULT (WINAPI *GetTrustLevel)(IMapView_empty *This, TrustLevel *trustLevel);
    /* IMapView */
    HRESULT (WINAPI *Lookup)(IMapView_empty *This, GUID key, void **value);
    HRESULT (WINAPI *get_Size)(IMapView_empty *This, UINT32 *size);
    HRESULT (WINAPI *HasKey)(IMapView_empty *This, GUID key, BOOLEAN *found);
    HRESULT (WINAPI *Split)(IMapView_empty *This, void **first, void **second);
} IMapView_emptyVtbl;

struct IMapView_empty
{
    const IMapView_emptyVtbl *lpVtbl;
};

typedef struct IIterable_empty IIterable_empty;

typedef struct IIterable_emptyVtbl
{
    /* IUnknown */
    HRESULT (WINAPI *QueryInterface)(IIterable_empty *This, REFIID riid, void **ppvObject);
    ULONG   (WINAPI *AddRef)(IIterable_empty *This);
    ULONG   (WINAPI *Release)(IIterable_empty *This);
    /* IInspectable */
    HRESULT (WINAPI *GetIids)(IIterable_empty *This, ULONG *iidCount, IID **iids);
    HRESULT (WINAPI *GetRuntimeClassName)(IIterable_empty *This, HSTRING *className);
    HRESULT (WINAPI *GetTrustLevel)(IIterable_empty *This, TrustLevel *trustLevel);
    /* IIterable */
    HRESULT (WINAPI *First)(IIterable_empty *This, void **first);
} IIterable_emptyVtbl;

struct IIterable_empty
{
    const IIterable_emptyVtbl *lpVtbl;
};

struct empty_map_view
{
    IMapView_empty IMapView_iface;
    IIterable_empty IIterable_iface;
    LONG ref;
};

static inline struct empty_map_view *impl_from_IMapView_empty(IMapView_empty *iface)
{
    return CONTAINING_RECORD(iface, struct empty_map_view, IMapView_iface);
}

static inline struct empty_map_view *impl_from_IIterable_empty(IIterable_empty *iface)
{
    return CONTAINING_RECORD(iface, struct empty_map_view, IIterable_iface);
}

static HRESULT WINAPI map_view_QueryInterface(IMapView_empty *iface, REFIID iid, void **out)
{
    struct empty_map_view *impl = impl_from_IMapView_empty(iface);
    static const GUID IID_ComWrappers_internal = {0x11d3b13a, 0x180e, 0x4789, {0xa8, 0xbe, 0x77, 0x12, 0x88, 0x28, 0x93, 0xe6}};
    static const GUID IID_ComWrappers_internal2 = {0x5c13e51c, 0x241d, 0x4859, {0x80, 0x05, 0x8a, 0x8d, 0x8d, 0x98, 0x15, 0xbf}};

    MESSAGE("background_task: map_view_QueryInterface iid %s\n", debugstr_guid(iid));
    if (!out) return E_INVALIDARG;

    if (IsEqualIID(iid, &IID_ComWrappers_internal) || IsEqualIID(iid, &IID_ComWrappers_internal2))
    {
        *out = NULL;
        return E_NOINTERFACE;
    }

    if (IsEqualIID(iid, &IID_IIterable_IKeyValuePair_GUID_IBackgroundTaskRegistration1) ||
        IsEqualIID(iid, &IID_IIterable_IKeyValuePair_GUID_IBackgroundTaskRegistration2))
    {
        *out = &impl->IIterable_iface;
        IUnknown_AddRef((IUnknown *)*out);
        return S_OK;
    }

    /* By default return IMapView */
    *out = &impl->IMapView_iface;
    IUnknown_AddRef((IUnknown *)*out);
    return S_OK;
}

static ULONG WINAPI map_view_AddRef(IMapView_empty *iface)
{
    struct empty_map_view *impl = impl_from_IMapView_empty(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI map_view_Release(IMapView_empty *iface)
{
    struct empty_map_view *impl = impl_from_IMapView_empty(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI map_view_GetIids(IMapView_empty *iface, ULONG *iid_count, IID **iids)
{
    IID *array;
    if (!iid_count || !iids) return E_INVALIDARG;
    if (!(array = CoTaskMemAlloc(4 * sizeof(IID)))) return E_OUTOFMEMORY;
    array[0] = IID_IAgileObject;
    array[1] = IID_IMapView_GUID_IBackgroundTaskRegistration1;
    array[2] = IID_IMapView_GUID_IBackgroundTaskRegistration2;
    array[3] = IID_IIterable_IKeyValuePair_GUID_IBackgroundTaskRegistration2;
    *iid_count = 4;
    *iids = array;
    return S_OK;
}

static HRESULT WINAPI map_view_GetRuntimeClassName(IMapView_empty *iface, HSTRING *class_name)
{
    static const WCHAR name[] = L"Windows.Foundation.Collections.IMapView`2<Guid, Windows.ApplicationModel.Background.BackgroundTaskRegistration>";
    return WindowsCreateString(name, ARRAY_SIZE(name) - 1, class_name);
}

static HRESULT WINAPI map_view_GetTrustLevel(IMapView_empty *iface, TrustLevel *trust_level)
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI map_view_Lookup(IMapView_empty *iface, GUID key, void **value)
{
    MESSAGE("background_task: map_view_Lookup returning E_BOUNDS\n");
    if (!value) return E_INVALIDARG;
    *value = NULL;
    return E_BOUNDS;
}

static HRESULT WINAPI map_view_get_Size(IMapView_empty *iface, UINT32 *size)
{
    MESSAGE("background_task: map_view_get_Size returning 0\n");
    if (!size) return E_INVALIDARG;
    *size = 0;
    return S_OK;
}

static HRESULT WINAPI map_view_HasKey(IMapView_empty *iface, GUID key, BOOLEAN *found)
{
    MESSAGE("background_task: map_view_HasKey returning FALSE\n");
    if (!found) return E_INVALIDARG;
    *found = FALSE;
    return S_OK;
}

static HRESULT WINAPI map_view_Split(IMapView_empty *iface, void **first, void **second)
{
    if (!first || !second) return E_INVALIDARG;
    *first = NULL;
    *second = NULL;
    return S_OK;
}

static const struct IMapView_emptyVtbl map_view_vtbl =
{
    map_view_QueryInterface,
    map_view_AddRef,
    map_view_Release,
    map_view_GetIids,
    map_view_GetRuntimeClassName,
    map_view_GetTrustLevel,
    map_view_Lookup,
    map_view_get_Size,
    map_view_HasKey,
    map_view_Split,
};

static HRESULT WINAPI iterable_QueryInterface(IIterable_empty *iface, REFIID iid, void **out)
{
    struct empty_map_view *impl = impl_from_IIterable_empty(iface);
    return map_view_QueryInterface(&impl->IMapView_iface, iid, out);
}

static ULONG WINAPI iterable_AddRef(IIterable_empty *iface)
{
    struct empty_map_view *impl = impl_from_IIterable_empty(iface);
    return map_view_AddRef(&impl->IMapView_iface);
}

static ULONG WINAPI iterable_Release(IIterable_empty *iface)
{
    struct empty_map_view *impl = impl_from_IIterable_empty(iface);
    return map_view_Release(&impl->IMapView_iface);
}

static HRESULT WINAPI iterable_GetIids(IIterable_empty *iface, ULONG *iid_count, IID **iids)
{
    struct empty_map_view *impl = impl_from_IIterable_empty(iface);
    return map_view_GetIids(&impl->IMapView_iface, iid_count, iids);
}

static HRESULT WINAPI iterable_GetRuntimeClassName(IIterable_empty *iface, HSTRING *class_name)
{
    struct empty_map_view *impl = impl_from_IIterable_empty(iface);
    return map_view_GetRuntimeClassName(&impl->IMapView_iface, class_name);
}

static HRESULT WINAPI iterable_GetTrustLevel(IIterable_empty *iface, TrustLevel *trust_level)
{
    struct empty_map_view *impl = impl_from_IIterable_empty(iface);
    return map_view_GetTrustLevel(&impl->IMapView_iface, trust_level);
}

static HRESULT WINAPI iterable_First(IIterable_empty *iface, void **first)
{
    MESSAGE("background_task: iterable_First called, creating empty iterator\n");
    return empty_iterator_create(first);
}

static const struct IIterable_emptyVtbl iterable_vtbl =
{
    iterable_QueryInterface,
    iterable_AddRef,
    iterable_Release,
    iterable_GetIids,
    iterable_GetRuntimeClassName,
    iterable_GetTrustLevel,
    iterable_First,
};

static HRESULT empty_map_view_create(void **out)
{
    struct empty_map_view *impl;
    if (!out) return E_INVALIDARG;
    if (!(impl = calloc(1, sizeof(*impl)))) return E_OUTOFMEMORY;
    impl->IMapView_iface.lpVtbl = &map_view_vtbl;
    impl->IIterable_iface.lpVtbl = &iterable_vtbl;
    impl->ref = 1;
    *out = &impl->IMapView_iface;
    return S_OK;
}

/* IBackgroundTaskRegistrationStatics declarations */
typedef struct IBackgroundTaskRegistrationStatics IBackgroundTaskRegistrationStatics;

typedef struct IBackgroundTaskRegistrationStaticsVtbl
{
    /* IUnknown */
    HRESULT (WINAPI *QueryInterface)(IBackgroundTaskRegistrationStatics *This, REFIID riid, void **ppvObject);
    ULONG   (WINAPI *AddRef)(IBackgroundTaskRegistrationStatics *This);
    ULONG   (WINAPI *Release)(IBackgroundTaskRegistrationStatics *This);
    /* IInspectable */
    HRESULT (WINAPI *GetIids)(IBackgroundTaskRegistrationStatics *This, ULONG *iidCount, IID **iids);
    HRESULT (WINAPI *GetRuntimeClassName)(IBackgroundTaskRegistrationStatics *This, HSTRING *className);
    HRESULT (WINAPI *GetTrustLevel)(IBackgroundTaskRegistrationStatics *This, TrustLevel *trustLevel);
    /* IBackgroundTaskRegistrationStatics */
    HRESULT (WINAPI *get_AllTasks)(IBackgroundTaskRegistrationStatics *This, void **allTasks);
} IBackgroundTaskRegistrationStaticsVtbl;

struct IBackgroundTaskRegistrationStatics
{
    const IBackgroundTaskRegistrationStaticsVtbl *lpVtbl;
};

struct background_task_factory
{
    IActivationFactory IActivationFactory_iface;
    IBackgroundTaskRegistrationStatics IBackgroundTaskRegistrationStatics_iface;
    LONG ref;
};

static inline struct background_task_factory *impl_from_IActivationFactory(IActivationFactory *iface)
{
    return CONTAINING_RECORD(iface, struct background_task_factory, IActivationFactory_iface);
}

static inline struct background_task_factory *impl_from_IBackgroundTaskRegistrationStatics(IBackgroundTaskRegistrationStatics *iface)
{
    return CONTAINING_RECORD(iface, struct background_task_factory, IBackgroundTaskRegistrationStatics_iface);
}

static HRESULT WINAPI factory_QueryInterface(IActivationFactory *iface, REFIID iid, void **out)
{
    struct background_task_factory *factory = impl_from_IActivationFactory(iface);
    static const GUID IID_ComWrappers_internal = {0x11d3b13a, 0x180e, 0x4789, {0xa8, 0xbe, 0x77, 0x12, 0x88, 0x28, 0x93, 0xe6}};
    static const GUID IID_ComWrappers_internal2 = {0x5c13e51c, 0x241d, 0x4859, {0x80, 0x05, 0x8a, 0x8d, 0x8d, 0x98, 0x15, 0xbf}};

    MESSAGE("background_task: factory_QueryInterface iid %s\n", debugstr_guid(iid));
    if (!out) return E_INVALIDARG;

    if (IsEqualIID(iid, &IID_ComWrappers_internal) || IsEqualIID(iid, &IID_ComWrappers_internal2))
    {
        *out = NULL;
        return E_NOINTERFACE;
    }

    if (IsEqualIID(iid, &IID_IUnknown) ||
        IsEqualIID(iid, &IID_IInspectable) ||
        IsEqualIID(iid, &IID_IAgileObject) ||
        IsEqualIID(iid, &IID_IActivationFactory))
    {
        *out = &factory->IActivationFactory_iface;
    }
    else if (IsEqualIID(iid, &IID_IBackgroundTaskRegistrationStatics))
    {
        *out = &factory->IBackgroundTaskRegistrationStatics_iface;
    }
    else
    {
        WARN("unsupported iid %s\n", debugstr_guid(iid));
        *out = NULL;
        return E_NOINTERFACE;
    }

    IUnknown_AddRef((IUnknown *)*out);
    return S_OK;
}

static ULONG WINAPI factory_AddRef(IActivationFactory *iface)
{
    struct background_task_factory *factory = impl_from_IActivationFactory(iface);
    return InterlockedIncrement(&factory->ref);
}

static ULONG WINAPI factory_Release(IActivationFactory *iface)
{
    struct background_task_factory *factory = impl_from_IActivationFactory(iface);
    return InterlockedDecrement(&factory->ref);
}

static HRESULT WINAPI factory_GetIids(IActivationFactory *iface, ULONG *iid_count, IID **iids)
{
    IID *array;
    if (!iid_count || !iids) return E_INVALIDARG;
    if (!(array = CoTaskMemAlloc(3 * sizeof(IID)))) return E_OUTOFMEMORY;
    array[0] = IID_IAgileObject;
    array[1] = IID_IActivationFactory;
    array[2] = IID_IBackgroundTaskRegistrationStatics;
    *iid_count = 3;
    *iids = array;
    return S_OK;
}

static HRESULT WINAPI factory_GetRuntimeClassName(IActivationFactory *iface, HSTRING *class_name)
{
    return WindowsCreateString(RuntimeClass_Windows_ApplicationModel_Background_BackgroundTaskRegistration,
                               wcslen(RuntimeClass_Windows_ApplicationModel_Background_BackgroundTaskRegistration),
                               class_name);
}

static HRESULT WINAPI factory_GetTrustLevel(IActivationFactory *iface, TrustLevel *trust_level)
{
    if (!trust_level) return E_INVALIDARG;
    *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI factory_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    FIXME("iface %p, instance %p stub!\n", iface, instance);
    if (instance) *instance = NULL;
    return E_NOTIMPL;
}

static const struct IActivationFactoryVtbl factory_vtbl =
{
    factory_QueryInterface,
    factory_AddRef,
    factory_Release,
    factory_GetIids,
    factory_GetRuntimeClassName,
    factory_GetTrustLevel,
    factory_ActivateInstance,
};

static HRESULT WINAPI statics_QueryInterface(IBackgroundTaskRegistrationStatics *iface, REFIID iid, void **out)
{
    struct background_task_factory *factory = impl_from_IBackgroundTaskRegistrationStatics(iface);
    return factory_QueryInterface(&factory->IActivationFactory_iface, iid, out);
}

static ULONG WINAPI statics_AddRef(IBackgroundTaskRegistrationStatics *iface)
{
    struct background_task_factory *factory = impl_from_IBackgroundTaskRegistrationStatics(iface);
    return factory_AddRef(&factory->IActivationFactory_iface);
}

static ULONG WINAPI statics_Release(IBackgroundTaskRegistrationStatics *iface)
{
    struct background_task_factory *factory = impl_from_IBackgroundTaskRegistrationStatics(iface);
    return factory_Release(&factory->IActivationFactory_iface);
}

static HRESULT WINAPI statics_GetIids(IBackgroundTaskRegistrationStatics *iface, ULONG *iid_count, IID **iids)
{
    struct background_task_factory *factory = impl_from_IBackgroundTaskRegistrationStatics(iface);
    return factory_GetIids(&factory->IActivationFactory_iface, iid_count, iids);
}

static HRESULT WINAPI statics_GetRuntimeClassName(IBackgroundTaskRegistrationStatics *iface, HSTRING *class_name)
{
    struct background_task_factory *factory = impl_from_IBackgroundTaskRegistrationStatics(iface);
    return factory_GetRuntimeClassName(&factory->IActivationFactory_iface, class_name);
}

static HRESULT WINAPI statics_GetTrustLevel(IBackgroundTaskRegistrationStatics *iface, TrustLevel *trust_level)
{
    struct background_task_factory *factory = impl_from_IBackgroundTaskRegistrationStatics(iface);
    return factory_GetTrustLevel(&factory->IActivationFactory_iface, trust_level);
}

static HRESULT WINAPI statics_get_AllTasks(IBackgroundTaskRegistrationStatics *iface, void **allTasks)
{
    MESSAGE("background_task: statics_get_AllTasks called\n");
    return empty_map_view_create(allTasks);
}

static const struct IBackgroundTaskRegistrationStaticsVtbl statics_vtbl =
{
    statics_QueryInterface,
    statics_AddRef,
    statics_Release,
    statics_GetIids,
    statics_GetRuntimeClassName,
    statics_GetTrustLevel,
    statics_get_AllTasks,
};

static struct background_task_factory background_task_factory_obj =
{
    .IActivationFactory_iface.lpVtbl = &factory_vtbl,
    .IBackgroundTaskRegistrationStatics_iface.lpVtbl = &statics_vtbl,
    .ref = 1,
};

IActivationFactory *background_task_registration_factory = &background_task_factory_obj.IActivationFactory_iface;
