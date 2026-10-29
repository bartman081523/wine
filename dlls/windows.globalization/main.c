/* WinRT Windows.Globalization implementation
 *
 * Copyright 2021 Rémi Bernon for CodeWeavers
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

WINE_DEFAULT_DEBUG_CHANNEL(locale);

/*
 *
 * IIterator<HSTRING>
 *
 */

struct iterator_hstring
{
   IIterator_HSTRING IIterator_HSTRING_iface;
   LONG ref;

   IVectorView_HSTRING *view;
   UINT32 index;
   UINT32 size;
};

static inline struct iterator_hstring *impl_from_IIterator_HSTRING( IIterator_HSTRING *iface )
{
   return CONTAINING_RECORD(iface, struct iterator_hstring, IIterator_HSTRING_iface);
}

static HRESULT WINAPI iterator_hstring_QueryInterface( IIterator_HSTRING *iface, REFIID iid, void **out )
{
    struct iterator_hstring *impl = impl_from_IIterator_HSTRING(iface);

   TRACE("iface %p, iid %s, out %p.\n", iface, debugstr_guid(iid), out);

   if (IsEqualGUID(iid, &IID_IUnknown) ||
       IsEqualGUID(iid, &IID_IInspectable) ||
       IsEqualGUID(iid, &IID_IAgileObject) ||
       IsEqualGUID(iid, &IID_IIterator_HSTRING))
    {
       IInspectable_AddRef((*out = &impl->IIterator_HSTRING_iface));
       return S_OK;
    }

    WARN("%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid(iid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI iterator_hstring_AddRef( IIterator_HSTRING *iface )
{
    struct iterator_hstring *impl = impl_from_IIterator_HSTRING(iface);
    ULONG ref = InterlockedIncrement(&impl->ref);
    TRACE("iface %p increasing refcount to %lu.\n", iface, ref);
    return ref;
}

static ULONG WINAPI iterator_hstring_Release( IIterator_HSTRING *iface )
{
    struct iterator_hstring *impl = impl_from_IIterator_HSTRING(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);

    TRACE("iface %p decreasing refcount to %lu.\n", iface, ref);

    if (!ref)
    {
        IVectorView_HSTRING_Release(impl->view);
        free(impl);
    }

    return ref;
}

static HRESULT WINAPI iterator_hstring_GetIids( IIterator_HSTRING *iface, ULONG *iid_count, IID **iids )
{
    FIXME("iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids);
    return E_NOTIMPL;
}

static HRESULT WINAPI iterator_hstring_GetRuntimeClassName( IIterator_HSTRING *iface, HSTRING *class_name )
{
    FIXME("iface %p, class_name %p stub!\n", iface, class_name);
    return E_NOTIMPL;
}

static HRESULT WINAPI iterator_hstring_GetTrustLevel( IIterator_HSTRING *iface, TrustLevel *trust_level )
{
    FIXME("iface %p, trust_level %p stub!\n", iface, trust_level);
    return E_NOTIMPL;
}

static HRESULT WINAPI iterator_hstring_get_Current( IIterator_HSTRING *iface, HSTRING *value )
{
    struct iterator_hstring *impl = impl_from_IIterator_HSTRING(iface);
    TRACE("iface %p, value %p.\n", iface, value);
    return IVectorView_HSTRING_GetAt(impl->view, impl->index, value);
}

static HRESULT WINAPI iterator_hstring_get_HasCurrent( IIterator_HSTRING *iface, boolean *value )
{
    struct iterator_hstring *impl = impl_from_IIterator_HSTRING(iface);

    TRACE("iface %p, value %p.\n", iface, value);

    *value = impl->index < impl->size;
    return S_OK;
}

static HRESULT WINAPI iterator_hstring_MoveNext( IIterator_HSTRING *iface, boolean *value )
{
    struct iterator_hstring *impl = impl_from_IIterator_HSTRING(iface);

    TRACE("iface %p, value %p.\n", iface, value);

    if (impl->index < impl->size) impl->index++;
    return IIterator_HSTRING_get_HasCurrent(iface, value);
}

static HRESULT WINAPI iterator_hstring_GetMany( IIterator_HSTRING *iface, UINT32 items_size,
                                                HSTRING *items, UINT *count )
{
    struct iterator_hstring *impl = impl_from_IIterator_HSTRING(iface);
    TRACE("iface %p, items_size %u, items %p, count %p.\n", iface, items_size, items, count);
    return IVectorView_HSTRING_GetMany(impl->view, impl->index, items_size, items, count);
}

static const IIterator_HSTRINGVtbl iterator_hstring_vtbl =
{
    /* IUnknown methods */
    iterator_hstring_QueryInterface,
    iterator_hstring_AddRef,
    iterator_hstring_Release,
    /* IInspectable methods */
    iterator_hstring_GetIids,
    iterator_hstring_GetRuntimeClassName,
    iterator_hstring_GetTrustLevel,
    /* IIterator<HSTRING> methods */
    iterator_hstring_get_Current,
    iterator_hstring_get_HasCurrent,
    iterator_hstring_MoveNext,
    iterator_hstring_GetMany,
};

struct hstring_vector
{
    IVectorView_HSTRING IVectorView_HSTRING_iface;
    IIterable_HSTRING IIterable_HSTRING_iface;
    LONG ref;

    ULONG count;
    HSTRING values[];
};

C_ASSERT(sizeof(struct hstring_vector) == offsetof(struct hstring_vector, values[0]));

static inline struct hstring_vector *impl_from_IVectorView_HSTRING(IVectorView_HSTRING *iface)
{
    return CONTAINING_RECORD(iface, struct hstring_vector, IVectorView_HSTRING_iface);
}

static HRESULT STDMETHODCALLTYPE hstring_vector_QueryInterface(IVectorView_HSTRING *iface,
        REFIID iid, void **out)
{
    struct hstring_vector *impl = impl_from_IVectorView_HSTRING(iface);
    TRACE("iface %p, iid %s, out %p stub!\n", iface, debugstr_guid(iid), out);

    if (IsEqualGUID(iid, &IID_IUnknown) ||
        IsEqualGUID(iid, &IID_IInspectable) ||
        IsEqualGUID(iid, &IID_IAgileObject) ||
        IsEqualGUID(iid, &IID_IVectorView_HSTRING))
    {
        IUnknown_AddRef(iface);
        *out = &impl->IVectorView_HSTRING_iface;
        return S_OK;
    }

    if (IsEqualGUID(iid, &IID_IIterable_HSTRING))
    {
        IUnknown_AddRef(iface);
        *out = &impl->IIterable_HSTRING_iface;
        return S_OK;
    }

    FIXME("%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid(iid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG STDMETHODCALLTYPE hstring_vector_AddRef(IVectorView_HSTRING *iface)
{
    struct hstring_vector *impl = impl_from_IVectorView_HSTRING(iface);
    ULONG ref = InterlockedIncrement(&impl->ref);
    TRACE("iface %p, ref %lu.\n", iface, ref);
    return ref;
}

static ULONG STDMETHODCALLTYPE hstring_vector_Release(IVectorView_HSTRING *iface)
{
    struct hstring_vector *impl = impl_from_IVectorView_HSTRING(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    TRACE("iface %p, ref %lu.\n", iface, ref);
    if (ref == 0)
    {
        while (impl->count--) WindowsDeleteString(impl->values[impl->count]);
        free(impl);
    }
    return ref;
}

static HRESULT STDMETHODCALLTYPE hstring_vector_GetIids(IVectorView_HSTRING *iface,
        ULONG *iid_count, IID **iids)
{
    FIXME("iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids);
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE hstring_vector_GetRuntimeClassName(IVectorView_HSTRING *iface,
        HSTRING *class_name)
{
    FIXME("iface %p, class_name %p stub!\n", iface, class_name);
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE hstring_vector_GetTrustLevel(IVectorView_HSTRING *iface,
        TrustLevel *trust_level)
{
    FIXME("iface %p, trust_level %p stub!\n", iface, trust_level);
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE hstring_vector_GetAt(IVectorView_HSTRING *iface,
        UINT32 index, HSTRING *value)
{
    struct hstring_vector *impl = impl_from_IVectorView_HSTRING(iface);

    TRACE("iface %p, index %#x, value %p.\n", iface, index, value);

    *value = NULL;
    if (index >= impl->count) return E_BOUNDS;
    return WindowsDuplicateString(impl->values[index], value);
}

static HRESULT STDMETHODCALLTYPE hstring_vector_get_Size(IVectorView_HSTRING *iface,
        UINT32 *value)
{
    struct hstring_vector *impl = impl_from_IVectorView_HSTRING(iface);

    TRACE("iface %p, value %p.\n", iface, value);

    *value = impl->count;
    return S_OK;
}

static HRESULT STDMETHODCALLTYPE hstring_vector_IndexOf(IVectorView_HSTRING *iface,
        HSTRING element, UINT32 *index, BOOLEAN *found)
{
    struct hstring_vector *impl = impl_from_IVectorView_HSTRING(iface);
    INT32 i, order;

    TRACE("iface %p, element %p, index %p, found %p.\n", iface, element, index, found);

    for (i = 0; i < impl->count; ++i)
        if (SUCCEEDED(WindowsCompareStringOrdinal(impl->values[i], element, &order)) && order == 0)
            break;

    if (i < impl->count)
    {
        *found = TRUE;
        *index = i;
    }
    else
    {
        *found = FALSE;
        *index = 0;
    }

    return S_OK;
}

static HRESULT STDMETHODCALLTYPE hstring_vector_GetMany(IVectorView_HSTRING *iface,
        UINT32 start_index, UINT32 items_size, HSTRING *items, UINT *count)
{
    struct hstring_vector *impl = impl_from_IVectorView_HSTRING(iface);
    HRESULT hr = S_OK;
    ULONG i;

    TRACE("iface %p, start_index %#x, items %p, count %p.\n", iface, start_index, items, count);

    memset(items, 0, items_size * sizeof(*items));

    for (i = start_index; i < impl->count && i < start_index + items_size; ++i)
        if (FAILED(hr = WindowsDuplicateString(impl->values[i], items + i - start_index)))
            break;

    if (FAILED(hr)) while (i-- > start_index) WindowsDeleteString(items[i - start_index]);
    *count = i - start_index;
    return hr;
}

static const struct IVectorView_HSTRINGVtbl hstring_vector_vtbl =
{
    hstring_vector_QueryInterface,
    hstring_vector_AddRef,
    hstring_vector_Release,
    /* IInspectable methods */
    hstring_vector_GetIids,
    hstring_vector_GetRuntimeClassName,
    hstring_vector_GetTrustLevel,
    /* IVectorView<HSTRING> methods */
    hstring_vector_GetAt,
    hstring_vector_get_Size,
    hstring_vector_IndexOf,
    hstring_vector_GetMany,
};

static const struct IIterable_HSTRINGVtbl iterable_view_hstring_vtbl;

static HRESULT hstring_vector_create(HSTRING *values, SIZE_T count, IVectorView_HSTRING **out)
{
    struct hstring_vector *impl;

    if (!(impl = malloc(offsetof(struct hstring_vector, values[count])))) return E_OUTOFMEMORY;
    impl->ref = 1;

    impl->IVectorView_HSTRING_iface.lpVtbl = &hstring_vector_vtbl;
    impl->IIterable_HSTRING_iface.lpVtbl = &iterable_view_hstring_vtbl;
    impl->count = count;
    memcpy(impl->values, values, count * sizeof(HSTRING));

    *out = &impl->IVectorView_HSTRING_iface;
    return S_OK;
}

/*
 *
 * IIterable<HSTRING>
 *
 */

DEFINE_IINSPECTABLE_(iterable_view_hstring, IIterable_HSTRING, struct hstring_vector, view_impl_from_IIterable_HSTRING,
                     IIterable_HSTRING_iface, &impl->IVectorView_HSTRING_iface)

static HRESULT WINAPI iterable_view_hstring_First( IIterable_HSTRING *iface, IIterator_HSTRING **value )
{
    struct hstring_vector *impl = view_impl_from_IIterable_HSTRING(iface);
    struct iterator_hstring *iter;

    TRACE("iface %p, value %p.\n", iface, value);

    if (!(iter = calloc(1, sizeof(*iter)))) return E_OUTOFMEMORY;
    iter->IIterator_HSTRING_iface.lpVtbl = &iterator_hstring_vtbl;
    iter->ref = 1;

    IVectorView_HSTRING_AddRef((iter->view = &impl->IVectorView_HSTRING_iface));
    iter->size = impl->count;

    *value = &iter->IIterator_HSTRING_iface;
    return S_OK;
}

static const struct IIterable_HSTRINGVtbl iterable_view_hstring_vtbl =
{
    /* IUnknown methods */
    iterable_view_hstring_QueryInterface,
    iterable_view_hstring_AddRef,
    iterable_view_hstring_Release,
    /* IInspectable methods */
    iterable_view_hstring_GetIids,
    iterable_view_hstring_GetRuntimeClassName,
    iterable_view_hstring_GetTrustLevel,
    /* IIterable<HSTRING> methods */
    iterable_view_hstring_First,
};

struct application_languages
{
    IActivationFactory IActivationFactory_iface;
    IApplicationLanguagesStatics IApplicationLanguagesStatics_iface;
    LONG ref;
};

struct windows_globalization
{
    IActivationFactory IActivationFactory_iface;
    IGlobalizationPreferencesStatics IGlobalizationPreferencesStatics_iface;
    LONG ref;
};

struct language_factory
{
    IActivationFactory IActivationFactory_iface;
    ILanguageFactory ILanguageFactory_iface;
    ILanguageStatics ILanguageStatics_iface;
    ILanguageStatics2 ILanguageStatics2_iface;
    LONG ref;
};

struct language
{
    ILanguage ILanguage_iface;
    ILanguage2 ILanguage2_iface;
    LONG ref;
    WCHAR name[LOCALE_NAME_MAX_LENGTH];
};

static inline struct windows_globalization *impl_from_IActivationFactory(IActivationFactory *iface)
{
    return CONTAINING_RECORD(iface, struct windows_globalization, IActivationFactory_iface);
}

static inline struct application_languages *impl_application_languages_from_IActivationFactory(IActivationFactory *iface)
{
    return CONTAINING_RECORD(iface, struct application_languages, IActivationFactory_iface);
}

static inline struct language_factory *impl_language_factory_from_IActivationFactory(IActivationFactory *iface)
{
    return CONTAINING_RECORD(iface, struct language_factory, IActivationFactory_iface);
}

static inline struct windows_globalization *impl_from_IGlobalizationPreferencesStatics(IGlobalizationPreferencesStatics *iface)
{
    return CONTAINING_RECORD(iface, struct windows_globalization, IGlobalizationPreferencesStatics_iface);
}

static inline struct language_factory *impl_from_ILanguageFactory(ILanguageFactory *iface)
{
    return CONTAINING_RECORD(iface, struct language_factory, ILanguageFactory_iface);
}

static inline struct language *impl_from_ILanguage(ILanguage *iface)
{
    return CONTAINING_RECORD(iface, struct language, ILanguage_iface);
}

static HRESULT STDMETHODCALLTYPE language_QueryInterface(
        ILanguage *iface, REFIID iid, void **out)
{
    struct language *language = impl_from_ILanguage(iface);

    TRACE("iface %p, iid %s, out %p.\n", iface, debugstr_guid(iid), out);

    if (IsEqualGUID(iid, &IID_IUnknown) ||
        IsEqualGUID(iid, &IID_IInspectable) ||
        IsEqualGUID(iid, &IID_IAgileObject) ||
        IsEqualGUID(iid, &IID_ILanguage))
    {
        IUnknown_AddRef(iface);
        *out = iface;
        return S_OK;
    }
    else if (IsEqualGUID(iid, &IID_ILanguage2))
    {
        IUnknown_AddRef(&language->ILanguage2_iface);
        *out = &language->ILanguage2_iface;
        return S_OK;
    }

    FIXME("%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid(iid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG STDMETHODCALLTYPE language_AddRef(
        ILanguage *iface)
{
    struct language *language = impl_from_ILanguage(iface);
    ULONG ref = InterlockedIncrement(&language->ref);
    TRACE("iface %p, ref %lu.\n", iface, ref);
    return ref;
}

static ULONG STDMETHODCALLTYPE language_Release(
        ILanguage *iface)
{
    struct language *language = impl_from_ILanguage(iface);
    ULONG ref = InterlockedDecrement(&language->ref);

    TRACE("iface %p, ref %lu.\n", iface, ref);

    if (!ref)
        free(language);

    return ref;
}

static HRESULT STDMETHODCALLTYPE language_GetIids(
        ILanguage *iface, ULONG *iid_count, IID **iids)
{
    FIXME("iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids);
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE language_GetRuntimeClassName(
        ILanguage *iface, HSTRING *class_name)
{
    FIXME("iface %p, class_name %p stub!\n", iface, class_name);
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE language_GetTrustLevel(
        ILanguage *iface, TrustLevel *trust_level)
{
    FIXME("iface %p, trust_level %p stub!\n", iface, trust_level);
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE language_get_LanguageTag(
        ILanguage *iface, HSTRING *value)
{
    struct language *language = impl_from_ILanguage(iface);

    TRACE("iface %p, value %p.\n", iface, value);

    return WindowsCreateString(language->name, wcslen(language->name), value);
}

static HRESULT STDMETHODCALLTYPE language_get_DisplayName(
        ILanguage *iface, HSTRING *value)
{
    FIXME("iface %p, value %p stub!\n", iface, value);
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE language_get_NativeName(
        ILanguage *iface, HSTRING *value)
{
    FIXME("iface %p, value %p stub!\n", iface, value);
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE language_get_Script(
        ILanguage *iface, HSTRING *value)
{
    FIXME("iface %p, value %p stub!\n", iface, value);
    return E_NOTIMPL;
}

static const struct ILanguageVtbl language_vtbl =
{
    language_QueryInterface,
    language_AddRef,
    language_Release,
    /* IInspectable methods */
    language_GetIids,
    language_GetRuntimeClassName,
    language_GetTrustLevel,
    /* ILanguage methods */
    language_get_LanguageTag,
    language_get_DisplayName,
    language_get_NativeName,
    language_get_Script,
};

DEFINE_IINSPECTABLE(language2, ILanguage2, struct language, ILanguage_iface)

static HRESULT STDMETHODCALLTYPE language2_get_LayoutDirection(ILanguage2 *iface, enum LanguageLayoutDirection *value)
{
    struct language *language = impl_from_ILanguage2(iface);
    DWORD reading_layout;

    TRACE("iface %p, value %p.\n", iface, value);

    if (!GetLocaleInfoEx(language->name, LOCALE_IREADINGLAYOUT | LOCALE_RETURN_NUMBER,
            (void *)&reading_layout, sizeof(reading_layout)))
         return E_FAIL;

    *value = (enum LanguageLayoutDirection)reading_layout;
    return S_OK;
}

static const struct ILanguage2Vtbl language2_vtbl =
{
    language2_QueryInterface,
    language2_AddRef,
    language2_Release,
    /* IInspectable methods */
    language2_GetIids,
    language2_GetRuntimeClassName,
    language2_GetTrustLevel,
    /* ILanguage2 methods */
    language2_get_LayoutDirection,
};

static HRESULT STDMETHODCALLTYPE windows_globalization_QueryInterface(
        IActivationFactory *iface, REFIID iid, void **out)
{
    struct windows_globalization *impl = impl_from_IActivationFactory(iface);
    TRACE("iface %p, iid %s, out %p stub!\n", iface, debugstr_guid(iid), out);

    if (IsEqualGUID(iid, &IID_IUnknown) ||
        IsEqualGUID(iid, &IID_IInspectable) ||
        IsEqualGUID(iid, &IID_IAgileObject) ||
        IsEqualGUID(iid, &IID_IActivationFactory))
    {
        IUnknown_AddRef(iface);
        *out = &impl->IActivationFactory_iface;
        return S_OK;
    }

    if (IsEqualGUID(iid, &IID_IGlobalizationPreferencesStatics))
    {
        IUnknown_AddRef(iface);
        *out = &impl->IGlobalizationPreferencesStatics_iface;
        return S_OK;
    }

    FIXME("%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid(iid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG STDMETHODCALLTYPE windows_globalization_AddRef(
        IActivationFactory *iface)
{
    struct windows_globalization *impl = impl_from_IActivationFactory(iface);
    ULONG ref = InterlockedIncrement(&impl->ref);
    TRACE("iface %p, ref %lu.\n", iface, ref);
    return ref;
}

static ULONG STDMETHODCALLTYPE windows_globalization_Release(
        IActivationFactory *iface)
{
    struct windows_globalization *impl = impl_from_IActivationFactory(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    TRACE("iface %p, ref %lu.\n", iface, ref);
    return ref;
}

static HRESULT STDMETHODCALLTYPE windows_globalization_GetIids(
        IActivationFactory *iface, ULONG *iid_count, IID **iids)
{
    FIXME("iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids);
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE windows_globalization_GetRuntimeClassName(
        IActivationFactory *iface, HSTRING *class_name)
{
    FIXME("iface %p, class_name %p stub!\n", iface, class_name);
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE windows_globalization_GetTrustLevel(
        IActivationFactory *iface, TrustLevel *trust_level)
{
    FIXME("iface %p, trust_level %p stub!\n", iface, trust_level);
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE windows_globalization_ActivateInstance(
        IActivationFactory *iface, IInspectable **instance)
{
    FIXME("iface %p, instance %p stub!\n", iface, instance);
    return E_NOTIMPL;
}

static const struct IActivationFactoryVtbl activation_factory_vtbl =
{
    windows_globalization_QueryInterface,
    windows_globalization_AddRef,
    windows_globalization_Release,
    /* IInspectable methods */
    windows_globalization_GetIids,
    windows_globalization_GetRuntimeClassName,
    windows_globalization_GetTrustLevel,
    /* IActivationFactory methods */
    windows_globalization_ActivateInstance,
};

static HRESULT STDMETHODCALLTYPE globalization_preferences_QueryInterface(
        IGlobalizationPreferencesStatics *iface, REFIID iid, void **object)
{
    struct windows_globalization *impl = impl_from_IGlobalizationPreferencesStatics(iface);
    return windows_globalization_QueryInterface(&impl->IActivationFactory_iface, iid, object);
}

static ULONG STDMETHODCALLTYPE globalization_preferences_AddRef(
        IGlobalizationPreferencesStatics *iface)
{
    struct windows_globalization *impl = impl_from_IGlobalizationPreferencesStatics(iface);
    return windows_globalization_AddRef(&impl->IActivationFactory_iface);
}

static ULONG STDMETHODCALLTYPE globalization_preferences_Release(
        IGlobalizationPreferencesStatics *iface)
{
    struct windows_globalization *impl = impl_from_IGlobalizationPreferencesStatics(iface);
    return windows_globalization_Release(&impl->IActivationFactory_iface);
}

static HRESULT STDMETHODCALLTYPE globalization_preferences_GetIids(
        IGlobalizationPreferencesStatics *iface, ULONG *iid_count, IID **iids)
{
    FIXME("iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids);
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE globalization_preferences_GetRuntimeClassName(
        IGlobalizationPreferencesStatics *iface, HSTRING *class_name)
{
    FIXME("iface %p, class_name %p stub!\n", iface, class_name);
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE globalization_preferences_GetTrustLevel(
        IGlobalizationPreferencesStatics *iface, TrustLevel *trust_level)
{
    FIXME("iface %p, trust_level %p stub!\n", iface, trust_level);
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE globalization_preferences_get_Calendars(
        IGlobalizationPreferencesStatics *iface, IVectorView_HSTRING **out)
{
    FIXME("iface %p, out %p stub!\n", iface, out);
    return hstring_vector_create(NULL, 0, out);
}

static HRESULT STDMETHODCALLTYPE globalization_preferences_get_Clocks(
        IGlobalizationPreferencesStatics *iface, IVectorView_HSTRING **out)
{
    FIXME("iface %p, out %p stub!\n", iface, out);
    return hstring_vector_create(NULL, 0, out);
}

static HRESULT STDMETHODCALLTYPE globalization_preferences_get_Currencies(
        IGlobalizationPreferencesStatics *iface, IVectorView_HSTRING **out)
{
    FIXME("iface %p, out %p stub!\n", iface, out);
    return hstring_vector_create(NULL, 0, out);
}

static HRESULT STDMETHODCALLTYPE globalization_preferences_get_Languages(
        IGlobalizationPreferencesStatics *iface, IVectorView_HSTRING **out)
{
    HSTRING hstring;
    HRESULT hr;
    WCHAR locale[LOCALE_NAME_MAX_LENGTH];

    TRACE("iface %p, out %p.\n", iface, out);

    if (!GetUserDefaultLocaleName(locale, LOCALE_NAME_MAX_LENGTH))
        return E_FAIL;

    TRACE("returning language %s\n", debugstr_w(locale));

    if (FAILED(hr = WindowsCreateString(locale, wcslen(locale), &hstring)))
        return hr;

    return hstring_vector_create(&hstring, 1, out);
}

static HRESULT STDMETHODCALLTYPE globalization_preferences_get_HomeGeographicRegion(
        IGlobalizationPreferencesStatics *iface, HSTRING* out)
{
    WCHAR country[16];

    TRACE("iface %p, out %p.\n", iface, out);

    if (!GetUserDefaultGeoName(country, 16))
        return E_FAIL;

    TRACE("returning country %s\n", debugstr_w(country));

    return WindowsCreateString(country, wcslen(country), out);
}

static HRESULT STDMETHODCALLTYPE globalization_preferences_get_WeekStartsOn(
        IGlobalizationPreferencesStatics *iface, enum DayOfWeek* out)
{
    FIXME("iface %p, out %p stub!\n", iface, out);
    return E_NOTIMPL;
}

static const struct IGlobalizationPreferencesStaticsVtbl globalization_preferences_vtbl =
{
    globalization_preferences_QueryInterface,
    globalization_preferences_AddRef,
    globalization_preferences_Release,
    /* IInspectable methods */
    globalization_preferences_GetIids,
    globalization_preferences_GetRuntimeClassName,
    globalization_preferences_GetTrustLevel,
    /* IGlobalizationPreferencesStatics methods */
    globalization_preferences_get_Calendars,
    globalization_preferences_get_Clocks,
    globalization_preferences_get_Currencies,
    globalization_preferences_get_Languages,
    globalization_preferences_get_HomeGeographicRegion,
    globalization_preferences_get_WeekStartsOn,
};

static struct windows_globalization userprofile_preferences =
{
    {&activation_factory_vtbl},
    {&globalization_preferences_vtbl},
    0
};

static HRESULT STDMETHODCALLTYPE windows_globalization_language_factory_QueryInterface(
        IActivationFactory *iface, REFIID iid, void **out)
{
    struct language_factory *factory = impl_language_factory_from_IActivationFactory(iface);

    TRACE("iface %p, iid %s, out %p.\n", iface, debugstr_guid(iid), out);

    if (IsEqualGUID(iid, &IID_IUnknown) ||
        IsEqualGUID(iid, &IID_IInspectable) ||
        IsEqualGUID(iid, &IID_IAgileObject) ||
        IsEqualGUID(iid, &IID_IActivationFactory))
    {
        IUnknown_AddRef(iface);
        *out = iface;
        return S_OK;
    }

    if (IsEqualGUID(iid, &IID_ILanguageFactory))
    {
        IUnknown_AddRef(iface);
        *out = &factory->ILanguageFactory_iface;
        return S_OK;
    }

    if (IsEqualGUID(iid, &IID_ILanguageStatics))
    {
        IUnknown_AddRef(iface);
        *out = &factory->ILanguageStatics_iface;
        return S_OK;
    }

    if (IsEqualGUID(iid, &IID_ILanguageStatics2))
    {
        IUnknown_AddRef(iface);
        *out = &factory->ILanguageStatics2_iface;
        return S_OK;
    }

    FIXME("%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid(iid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG STDMETHODCALLTYPE windows_globalization_language_factory_AddRef(IActivationFactory *iface)
{
    struct language_factory *factory = impl_language_factory_from_IActivationFactory(iface);
    return InterlockedIncrement(&factory->ref);
}

static ULONG STDMETHODCALLTYPE windows_globalization_language_factory_Release(IActivationFactory *iface)
{
    struct language_factory *factory = impl_language_factory_from_IActivationFactory(iface);
    return InterlockedDecrement(&factory->ref);
}

static const struct IActivationFactoryVtbl activation_factory_language_vtbl =
{
    windows_globalization_language_factory_QueryInterface,
    windows_globalization_language_factory_AddRef,
    windows_globalization_language_factory_Release,
    /* IInspectable methods */
    windows_globalization_GetIids,
    windows_globalization_GetRuntimeClassName,
    windows_globalization_GetTrustLevel,
    /* IActivationFactory methods */
    windows_globalization_ActivateInstance,
};

static HRESULT STDMETHODCALLTYPE language_factory_QueryInterface(
        ILanguageFactory *iface, REFIID iid, void **object)
{
    struct language_factory *factory = impl_from_ILanguageFactory(iface);
    return IActivationFactory_QueryInterface(&factory->IActivationFactory_iface, iid, object);
}

static ULONG STDMETHODCALLTYPE language_factory_AddRef(
        ILanguageFactory *iface)
{
    struct language_factory *factory = impl_from_ILanguageFactory(iface);
    return IActivationFactory_AddRef(&factory->IActivationFactory_iface);
}

static ULONG STDMETHODCALLTYPE language_factory_Release(
        ILanguageFactory *iface)
{
    struct language_factory *factory = impl_from_ILanguageFactory(iface);
    return IActivationFactory_Release(&factory->IActivationFactory_iface);
}

static HRESULT STDMETHODCALLTYPE language_factory_GetIids(
        ILanguageFactory *iface, ULONG *iid_count, IID **iids)
{
    FIXME("iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids);
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE language_factory_GetRuntimeClassName(
        ILanguageFactory *iface, HSTRING *class_name)
{
    FIXME("iface %p, class_name %p stub!\n", iface, class_name);
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE language_factory_GetTrustLevel(
        ILanguageFactory *iface, TrustLevel *trust_level)
{
    FIXME("iface %p, trust_level %p stub!\n", iface, trust_level);
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE language_factory_CreateLanguage(
        ILanguageFactory *iface, HSTRING tag, ILanguage **value)
{
    const WCHAR *name = WindowsGetStringRawBuffer(tag, NULL);
    WCHAR buffer[LOCALE_NAME_MAX_LENGTH];
    struct language *language;

    TRACE("iface %p, tag %p, value %p.\n", iface, tag, value);

    if (!GetLocaleInfoEx(name, LOCALE_SNAME, buffer, ARRAY_SIZE(buffer)))
        return E_INVALIDARG;

    if (!(language = calloc(1, sizeof(*language))))
        return E_OUTOFMEMORY;

    language->ILanguage_iface.lpVtbl = &language_vtbl;
    language->ILanguage2_iface.lpVtbl = &language2_vtbl;
    language->ref = 1;
    wcscpy(language->name, buffer);

    *value = &language->ILanguage_iface;

    return S_OK;
}

DEFINE_IINSPECTABLE_(language_statics, ILanguageStatics, struct language_factory, impl_from_ILanguageStatics, ILanguageStatics_iface, &impl->IActivationFactory_iface)
DEFINE_IINSPECTABLE_(language_statics2, ILanguageStatics2, struct language_factory, impl_from_ILanguageStatics2, ILanguageStatics2_iface, &impl->IActivationFactory_iface)

static HRESULT WINAPI language_statics_IsWellFormed(ILanguageStatics *iface, HSTRING tag, BOOLEAN *result)
{
    TRACE("iface %p, tag %s, result %p.\n", iface, debugstr_hstring(tag), result);
    if (!result) return E_INVALIDARG;
    *result = TRUE;
    return S_OK;
}

static HRESULT WINAPI language_statics_get_CurrentInputMethodLanguageTag(ILanguageStatics *iface, HSTRING *value)
{
    TRACE("iface %p, value %p.\n", iface, value);
    if (!value) return E_INVALIDARG;
    return WindowsCreateString(L"en-US", 5, value);
}

static const struct ILanguageStaticsVtbl language_statics_vtbl =
{
    language_statics_QueryInterface,
    language_statics_AddRef,
    language_statics_Release,
    /* IInspectable methods */
    language_statics_GetIids,
    language_statics_GetRuntimeClassName,
    language_statics_GetTrustLevel,
    /* ILanguageStatics methods */
    language_statics_IsWellFormed,
    language_statics_get_CurrentInputMethodLanguageTag,
};

static HRESULT WINAPI language_statics2_TrySetInputMethodLanguageTag(ILanguageStatics2 *iface, HSTRING tag, BOOLEAN *result)
{
    TRACE("iface %p, tag %s, result %p.\n", iface, debugstr_hstring(tag), result);
    if (!result) return E_INVALIDARG;
    *result = TRUE;
    return S_OK;
}

static const struct ILanguageStatics2Vtbl language_statics2_vtbl =
{
    language_statics2_QueryInterface,
    language_statics2_AddRef,
    language_statics2_Release,
    /* IInspectable methods */
    language_statics2_GetIids,
    language_statics2_GetRuntimeClassName,
    language_statics2_GetTrustLevel,
    /* ILanguageStatics2 methods */
    language_statics2_TrySetInputMethodLanguageTag,
};

static const struct ILanguageFactoryVtbl language_factory_vtbl =
{
    language_factory_QueryInterface,
    language_factory_AddRef,
    language_factory_Release,
    /* IInspectable methods */
    language_factory_GetIids,
    language_factory_GetRuntimeClassName,
    language_factory_GetTrustLevel,
    /* ILanguageFactory methods */
    language_factory_CreateLanguage,
};

static struct language_factory language_factory =
{
    {&activation_factory_language_vtbl},
    {&language_factory_vtbl},
    {&language_statics_vtbl},
    {&language_statics2_vtbl},
    0
};

static HRESULT STDMETHODCALLTYPE application_languages_activation_factory_QueryInterface(
        IActivationFactory *iface, REFIID iid, void **out)
{
    struct application_languages *languages = impl_application_languages_from_IActivationFactory(iface);

    TRACE("iface %p, iid %s, out %p.\n", iface, debugstr_guid(iid), out);

    if (IsEqualGUID(iid, &IID_IUnknown) ||
        IsEqualGUID(iid, &IID_IInspectable) ||
        IsEqualGUID(iid, &IID_IAgileObject) ||
        IsEqualGUID(iid, &IID_IActivationFactory))
    {
        IUnknown_AddRef(iface);
        *out = iface;
        return S_OK;
    }
    else if (IsEqualGUID(iid, &IID_IApplicationLanguagesStatics))
    {
        IUnknown_AddRef(&languages->IApplicationLanguagesStatics_iface);
        *out = &languages->IApplicationLanguagesStatics_iface;
        return S_OK;
    }

    FIXME("%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid(iid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG STDMETHODCALLTYPE application_languages_activation_factory_AddRef(IActivationFactory *iface)
{
    struct application_languages *languages = impl_application_languages_from_IActivationFactory(iface);
    ULONG ref = InterlockedIncrement(&languages->ref);
    TRACE("iface %p, ref %lu.\n", iface, ref);
    return ref;
}

static ULONG STDMETHODCALLTYPE application_languages_activation_factory_Release(IActivationFactory *iface)
{
    struct application_languages *languages = impl_application_languages_from_IActivationFactory(iface);
    ULONG ref = InterlockedDecrement(&languages->ref);
    TRACE("iface %p, ref %lu.\n", iface, ref);
    return ref;
}

static HRESULT STDMETHODCALLTYPE application_languages_activation_factory_GetIids(
        IActivationFactory *iface, ULONG *iid_count, IID **iids)
{
    FIXME("iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids);
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE application_languages_activation_factory_GetRuntimeClassName(
        IActivationFactory *iface, HSTRING *class_name)
{
    FIXME("iface %p, class_name %p stub!\n", iface, class_name);
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE application_languages_activation_factory_GetTrustLevel(
        IActivationFactory *iface, TrustLevel *trust_level)
{
    FIXME("iface %p, trust_level %p stub!\n", iface, trust_level);
    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE application_languages_activation_factory_ActivateInstance(
        IActivationFactory *iface, IInspectable **instance)
{
    FIXME("iface %p, instance %p stub!\n", iface, instance);
    return E_NOTIMPL;
}

static const struct IActivationFactoryVtbl application_languages_activation_factory_vtbl =
{
    /* IUnknown methods */
    application_languages_activation_factory_QueryInterface,
    application_languages_activation_factory_AddRef,
    application_languages_activation_factory_Release,
    /* IInspectable methods */
    application_languages_activation_factory_GetIids,
    application_languages_activation_factory_GetRuntimeClassName,
    application_languages_activation_factory_GetTrustLevel,
    /* IActivationFactory methods */
    application_languages_activation_factory_ActivateInstance,
};

DEFINE_IINSPECTABLE(application_languages_statics, IApplicationLanguagesStatics, struct application_languages, IActivationFactory_iface)

static HRESULT STDMETHODCALLTYPE application_languages_statics_get_PrimaryLanguageOverride(
        IApplicationLanguagesStatics *iface, HSTRING *value)
{
    /* Windows returns an empty string when no language override is set.
     * Returning E_NOTIMPL here made CsWinRT raise NotImplementedException at
     * XAML app.xaml load, which WinUI turns into
     * RoFailFastWithErrorContextInternal2(E_NOTIMPL) (run64). */
    TRACE("iface %p, value %p.\n", iface, value);

    if (!value)
        return E_POINTER;

    return WindowsCreateString(L"", 0, value);
}

static HRESULT STDMETHODCALLTYPE application_languages_statics_put_PrimaryLanguageOverride(
        IApplicationLanguagesStatics *iface, HSTRING value)
{
    TRACE("iface %p, value %s.\n", iface, wine_dbgstr_hstring(value));
    return S_OK;
}

static HRESULT STDMETHODCALLTYPE application_languages_statics_get_Languages(
        IApplicationLanguagesStatics *iface, IVectorView_HSTRING **value)
{
    WCHAR locale[LOCALE_NAME_MAX_LENGTH];
    HSTRING hstrings[2];
    UINT32 count = 0;
    HRESULT hr;

    TRACE("iface %p, out %p\n", iface, value);

    if (!value)
        return E_POINTER;

    locale[0] = 0;
    if (!GetUserDefaultLocaleName(locale, LOCALE_NAME_MAX_LENGTH) || !locale[0])
        wcscpy(locale, L"en-US");

    if (FAILED(hr = WindowsCreateString(locale, wcslen(locale), &hstrings[count++])))
        return hr;

    if (_wcsicmp(locale, L"en-US") != 0 && _wcsicmp(locale, L"en") != 0)
    {
        if (FAILED(hr = WindowsCreateString(L"en-US", 5, &hstrings[count++])))
        {
            WindowsDeleteString(hstrings[0]);
            return hr;
        }
    }

    return hstring_vector_create(hstrings, count, value);
}

static HRESULT STDMETHODCALLTYPE application_languages_statics_get_ManifestLanguages(
        IApplicationLanguagesStatics *iface, IVectorView_HSTRING **value)
{
    HSTRING hstring;
    HRESULT hr;

    TRACE("iface %p, out %p\n", iface, value);

    if (!value)
        return E_POINTER;

    if (FAILED(hr = WindowsCreateString(L"en-US", 5, &hstring)))
        return hr;

    return hstring_vector_create(&hstring, 1, value);
}

static const struct IApplicationLanguagesStaticsVtbl application_languages_statics_vtbl =
{
    /* IUnknown methods */
    application_languages_statics_QueryInterface,
    application_languages_statics_AddRef,
    application_languages_statics_Release,
    /* IInspectable methods */
    application_languages_statics_GetIids,
    application_languages_statics_GetRuntimeClassName,
    application_languages_statics_GetTrustLevel,
    /* IApplicationLanguagesStatics methods */
    application_languages_statics_get_PrimaryLanguageOverride,
    application_languages_statics_put_PrimaryLanguageOverride,
    application_languages_statics_get_Languages,
    application_languages_statics_get_ManifestLanguages
};

static struct application_languages application_languages =
{
    {&application_languages_activation_factory_vtbl},
    {&application_languages_statics_vtbl},
    1
};

/* -------------------------------------------------------------------------
 * Windows.Globalization.Fonts.LanguageFontGroup Implementation
 * ------------------------------------------------------------------------- */
static const GUID IID_ILanguageFontGroupFactory = {0xfcaeac67, 0x4e77, 0x49c7, {0xb8, 0x56, 0xdd, 0xe9, 0x34, 0xfc, 0x73, 0x5b}};

struct language_font_impl
{
    struct { const void *lpVtbl; } ILanguageFont_iface;
};

static HRESULT WINAPI font_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = This;
    return S_OK;
}
static ULONG WINAPI font_AddRef(void *This) { return 2; }
static ULONG WINAPI font_Release(void *This) { return 1; }
static HRESULT WINAPI font_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI font_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.Globalization.Fonts.LanguageFont", 40, n); }
static HRESULT WINAPI font_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI font_get_FontFamily(void *This, HSTRING *value) { return WindowsCreateString(L"Segoe UI", 8, value); }
static HRESULT WINAPI font_get_FontWeight(void *This, void *value) { if (value) *(INT32 *)value = 400; return S_OK; }
static HRESULT WINAPI font_get_FontStyle(void *This, void *value) { if (value) *(INT32 *)value = 0; return S_OK; }
static HRESULT WINAPI font_get_FontStretch(void *This, void *value) { if (value) *(INT32 *)value = 5; return S_OK; }
static HRESULT WINAPI font_get_ScaleFactor(void *This, DOUBLE *value) { if (value) *value = 100.0; return S_OK; }

static const void *language_font_vtbl[] = {
    (void *)font_QueryInterface,
    (void *)font_AddRef,
    (void *)font_Release,
    (void *)font_GetIids,
    (void *)font_GetRuntimeClassName,
    (void *)font_GetTrustLevel,
    (void *)font_get_FontFamily,
    (void *)font_get_FontWeight,
    (void *)font_get_FontStyle,
    (void *)font_get_FontStretch,
    (void *)font_get_ScaleFactor
};

static struct language_font_impl default_language_font = { {language_font_vtbl} };

struct font_group_impl
{
    struct { const void *lpVtbl; } ILanguageFontGroup_iface;
};

static HRESULT WINAPI group_QueryInterface(void *This, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = This;
    return S_OK;
}
static ULONG WINAPI group_AddRef(void *This) { return 2; }
static ULONG WINAPI group_Release(void *This) { return 1; }
static HRESULT WINAPI group_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI group_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.Globalization.Fonts.LanguageFontGroup", 45, n); }
static HRESULT WINAPI group_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI group_get_Font(void *This, void **value)
{
    if (!value) return E_POINTER;
    *value = &default_language_font.ILanguageFont_iface;
    return S_OK;
}

static const void *language_font_group_vtbl[32] = {
    (void *)group_QueryInterface,
    (void *)group_AddRef,
    (void *)group_Release,
    (void *)group_GetIids,
    (void *)group_GetRuntimeClassName,
    (void *)group_GetTrustLevel,
    (void *)group_get_Font, (void *)group_get_Font, (void *)group_get_Font, (void *)group_get_Font,
    (void *)group_get_Font, (void *)group_get_Font, (void *)group_get_Font, (void *)group_get_Font,
    (void *)group_get_Font, (void *)group_get_Font, (void *)group_get_Font, (void *)group_get_Font,
    (void *)group_get_Font, (void *)group_get_Font, (void *)group_get_Font, (void *)group_get_Font,
    (void *)group_get_Font, (void *)group_get_Font, (void *)group_get_Font, (void *)group_get_Font,
    (void *)group_get_Font, (void *)group_get_Font, (void *)group_get_Font, (void *)group_get_Font,
    (void *)group_get_Font, (void *)group_get_Font
};

static struct font_group_impl default_font_group = { {language_font_group_vtbl} };

struct font_group_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } ILanguageFontGroupFactory_iface;
    LONG ref;
};

#define impl_from_fg_fac(ptr) ((struct font_group_factory_impl *)((char *)(ptr) - offsetof(struct font_group_factory_impl, IActivationFactory_iface)))
#define impl_from_fg_fac_iface(ptr) ((struct font_group_factory_impl *)((char *)(ptr) - offsetof(struct font_group_factory_impl, ILanguageFontGroupFactory_iface)))

static HRESULT fg_fac_QI_internal(struct font_group_factory_impl *impl, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppvObject = &impl->IActivationFactory_iface;
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ILanguageFontGroupFactory))
    {
        *ppvObject = &impl->ILanguageFontGroupFactory_iface;
        return S_OK;
    }

    TRACE("fg_fac %p: unhandled QI %s\n", impl, debugstr_guid(riid));
    return E_NOINTERFACE;
}

static HRESULT WINAPI fg_fac_QueryInterface(IActivationFactory *iface, REFIID riid, void **ppvObject) { return fg_fac_QI_internal(impl_from_fg_fac(iface), riid, ppvObject); }
static ULONG WINAPI fg_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI fg_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI fg_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI fg_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n) { return WindowsCreateString(L"Windows.Globalization.Fonts.LanguageFontGroup", 45, n); }
static HRESULT WINAPI fg_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI fg_fac_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    if (!instance) return E_POINTER;
    *instance = (IInspectable *)&default_font_group.ILanguageFontGroup_iface;
    return S_OK;
}

static const IActivationFactoryVtbl fg_activation_factory_vtbl = {
    fg_fac_QueryInterface,
    fg_fac_AddRef,
    fg_fac_Release,
    fg_fac_GetIids,
    fg_fac_GetRuntimeClassName,
    fg_fac_GetTrustLevel,
    fg_fac_ActivateInstance
};

static HRESULT WINAPI fg_fact_QI(void *This, REFIID riid, void **ppvObject) { return fg_fac_QI_internal(impl_from_fg_fac_iface(This), riid, ppvObject); }
static ULONG WINAPI fg_fact_AddRef(void *This) { return 2; }
static ULONG WINAPI fg_fact_Release(void *This) { return 1; }
static HRESULT WINAPI fg_fact_Create(void *This, HSTRING tag, void **value)
{
    TRACE("tag %s, value %p\n", debugstr_hstring(tag), value);
    if (!value) return E_POINTER;
    *value = &default_font_group.ILanguageFontGroup_iface;
    return S_OK;
}

static const void *fg_factory_vtbl[] = {
    (void *)fg_fact_QI,
    (void *)fg_fact_AddRef,
    (void *)fg_fact_Release,
    (void *)fg_fac_GetIids,
    (void *)fg_fac_GetRuntimeClassName,
    (void *)fg_fac_GetTrustLevel,
    (void *)fg_fact_Create
};

static struct font_group_factory_impl font_group_factory = {
    {&fg_activation_factory_vtbl},
    {fg_factory_vtbl},
    1
};

/*
 *
 * Windows.Globalization.NumberFormatting.DecimalFormatter
 *
 */

static const GUID IID_IDecimalFormatterFactory = {0x0D018C9A, 0xE393, 0x46B8, {0xB8, 0x30, 0x7A, 0x69, 0xC8, 0xF8, 0x9F, 0xBB}};
static const GUID IID_INumberFormatter = {0xA5007C49, 0x7676, 0x4DB7, {0x86, 0x31, 0x1B, 0x6F, 0xF2, 0x65, 0xCA, 0xA9}};
static const GUID IID_INumberFormatter2 = {0xD4A8C1F0, 0x80D0, 0x4B0D, {0xA8, 0x9E, 0x88, 0x2C, 0x1E, 0x8F, 0x83, 0x10}};
static const GUID IID_INumberFormatterOptions = {0x80332D21, 0xAEE1, 0x4A39, {0xBA, 0xA2, 0x07, 0xED, 0x8C, 0x96, 0xDA, 0xF6}};
static const GUID IID_INumberParser = {0xE6659412, 0x4A13, 0x4A53, {0x83, 0xA1, 0x39, 0x2F, 0xBE, 0x4C, 0xFF, 0x9F}};
static const GUID IID_INumberRounderOption = {0x3B088433, 0x646F, 0x4EFE, {0x8D, 0x48, 0x66, 0xEB, 0x2E, 0x49, 0xE7, 0x36}};
static const GUID IID_ISignificantDigitsOption = {0x1D4DFCDD, 0x2D43, 0x4EE8, {0xBB, 0xF1, 0xC1, 0xB2, 0x6A, 0x71, 0x1A, 0x58}};
static const GUID IID_ISignedZeroOption = {0xFD1CDD31, 0x0A3C, 0x49C4, {0xA6, 0x42, 0x96, 0xA1, 0x56, 0x4F, 0x4F, 0x30}};

struct decimal_formatter
{
    struct { const void *lpVtbl; } INumberFormatter_iface;
    struct { const void *lpVtbl; } INumberFormatter2_iface;
    struct { const void *lpVtbl; } INumberFormatterOptions_iface;
    struct { const void *lpVtbl; } INumberParser_iface;
    struct { const void *lpVtbl; } INumberRounderOption_iface;
    struct { const void *lpVtbl; } ISignificantDigitsOption_iface;
    struct { const void *lpVtbl; } ISignedZeroOption_iface;
    LONG ref;

    INT32 fraction_digits;
    INT32 integer_digits;
    boolean is_grouped;
    boolean is_dp_always_displayed;
    WCHAR resolved_language[64];
    WCHAR geographic_region[64];
};

#define impl_from_df_formatter(ptr) ((struct decimal_formatter *)((char *)(ptr) - offsetof(struct decimal_formatter, INumberFormatter_iface)))
#define impl_from_df_formatter2(ptr) ((struct decimal_formatter *)((char *)(ptr) - offsetof(struct decimal_formatter, INumberFormatter2_iface)))
#define impl_from_df_options(ptr) ((struct decimal_formatter *)((char *)(ptr) - offsetof(struct decimal_formatter, INumberFormatterOptions_iface)))
#define impl_from_df_parser(ptr) ((struct decimal_formatter *)((char *)(ptr) - offsetof(struct decimal_formatter, INumberParser_iface)))
#define impl_from_df_rounder(ptr) ((struct decimal_formatter *)((char *)(ptr) - offsetof(struct decimal_formatter, INumberRounderOption_iface)))
#define impl_from_df_sigdigits(ptr) ((struct decimal_formatter *)((char *)(ptr) - offsetof(struct decimal_formatter, ISignificantDigitsOption_iface)))
#define impl_from_df_signedzero(ptr) ((struct decimal_formatter *)((char *)(ptr) - offsetof(struct decimal_formatter, ISignedZeroOption_iface)))

#define df_from_formatter impl_from_df_formatter
#define df_from_formatter2 impl_from_df_formatter2
#define df_from_options impl_from_df_options
#define df_from_parser impl_from_df_parser
#define df_from_rounder impl_from_df_rounder
#define df_from_sigdigits impl_from_df_sigdigits
#define df_from_signedzero impl_from_df_signedzero

static HRESULT WINAPI df_QI_internal(struct decimal_formatter *impl, REFIID riid, void **out)
{
    if (!out) return E_POINTER;
    *out = NULL;

    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_INumberFormatter))
    {
        *out = &impl->INumberFormatter_iface;
    }
    else if (IsEqualGUID(riid, &IID_INumberFormatter2))
    {
        *out = &impl->INumberFormatter2_iface;
    }
    else if (IsEqualGUID(riid, &IID_INumberFormatterOptions))
    {
        *out = &impl->INumberFormatterOptions_iface;
    }
    else if (IsEqualGUID(riid, &IID_INumberParser))
    {
        *out = &impl->INumberParser_iface;
    }
    else if (IsEqualGUID(riid, &IID_INumberRounderOption))
    {
        *out = &impl->INumberRounderOption_iface;
    }
    else if (IsEqualGUID(riid, &IID_ISignificantDigitsOption))
    {
        *out = &impl->ISignificantDigitsOption_iface;
    }
    else if (IsEqualGUID(riid, &IID_ISignedZeroOption))
    {
        *out = &impl->ISignedZeroOption_iface;
    }
    else
    {
        WARN("unsupported iid %s\n", debugstr_guid(riid));
        return E_NOINTERFACE;
    }

    InterlockedIncrement(&impl->ref);
    return S_OK;
}

static ULONG WINAPI df_AddRef_internal(struct decimal_formatter *impl)
{
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI df_Release_internal(struct decimal_formatter *impl)
{
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI df_GetIids(void *This, ULONG *iid_count, IID **iids)
{
    if (iid_count) *iid_count = 0;
    if (iids) *iids = NULL;
    return S_OK;
}

static HRESULT WINAPI df_GetRuntimeClassName(void *This, HSTRING *class_name)
{
    return WindowsCreateString(L"Windows.Globalization.NumberFormatting.DecimalFormatter", 55, class_name);
}

static HRESULT WINAPI df_GetTrustLevel(void *This, TrustLevel *trust_level)
{
    if (trust_level) *trust_level = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI df_FormatInt(void *This, INT64 value, HSTRING *result)
{
    WCHAR buf[64];
    swprintf(buf, ARRAY_SIZE(buf), L"%I64d", value);
    return WindowsCreateString(buf, wcslen(buf), result);
}

static HRESULT WINAPI df_FormatUInt(void *This, UINT64 value, HSTRING *result)
{
    WCHAR buf[64];
    swprintf(buf, ARRAY_SIZE(buf), L"%I64u", value);
    return WindowsCreateString(buf, wcslen(buf), result);
}

static HRESULT WINAPI df_FormatDouble(void *This, DOUBLE value, HSTRING *result)
{
    struct decimal_formatter *impl = df_from_formatter(This);
    WCHAR buf[64];
    if (impl->fraction_digits == 0)
        swprintf(buf, ARRAY_SIZE(buf), L"%.0f", value);
    else if (impl->fraction_digits > 0)
        swprintf(buf, ARRAY_SIZE(buf), L"%.*f", impl->fraction_digits, value);
    else
        swprintf(buf, ARRAY_SIZE(buf), L"%g", value);
    return WindowsCreateString(buf, wcslen(buf), result);
}

static HRESULT WINAPI df_formatter_QI(void *This, REFIID riid, void **out) { return df_QI_internal(df_from_formatter(This), riid, out); }
static ULONG WINAPI df_formatter_AddRef(void *This) { return df_AddRef_internal(df_from_formatter(This)); }
static ULONG WINAPI df_formatter_Release(void *This) { return df_Release_internal(df_from_formatter(This)); }

static HRESULT WINAPI df_formatter2_QI(void *This, REFIID riid, void **out) { return df_QI_internal(df_from_formatter2(This), riid, out); }
static ULONG WINAPI df_formatter2_AddRef(void *This) { return df_AddRef_internal(df_from_formatter2(This)); }
static ULONG WINAPI df_formatter2_Release(void *This) { return df_Release_internal(df_from_formatter2(This)); }

static HRESULT WINAPI df_options_QI(void *This, REFIID riid, void **out) { return df_QI_internal(df_from_options(This), riid, out); }
static ULONG WINAPI df_options_AddRef(void *This) { return df_AddRef_internal(df_from_options(This)); }
static ULONG WINAPI df_options_Release(void *This) { return df_Release_internal(df_from_options(This)); }

static HRESULT WINAPI df_options_get_Languages(void *This, void **value)
{
    struct decimal_formatter *impl = df_from_options(This);
    HSTRING str;
    if (!value) return E_POINTER;
    WindowsCreateString(impl->resolved_language, wcslen(impl->resolved_language), &str);
    return hstring_vector_create(&str, 1, (IVectorView_HSTRING **)value);
}

static HRESULT WINAPI df_options_get_GeographicRegion(void *This, HSTRING *value)
{
    struct decimal_formatter *impl = df_from_options(This);
    return WindowsCreateString(impl->geographic_region, wcslen(impl->geographic_region), value);
}

static HRESULT WINAPI df_options_get_IntegerDigits(void *This, INT32 *value)
{
    struct decimal_formatter *impl = df_from_options(This);
    if (!value) return E_POINTER;
    *value = impl->integer_digits;
    return S_OK;
}

static HRESULT WINAPI df_options_put_IntegerDigits(void *This, INT32 value)
{
    struct decimal_formatter *impl = df_from_options(This);
    impl->integer_digits = value;
    return S_OK;
}

static HRESULT WINAPI df_options_get_FractionDigits(void *This, INT32 *value)
{
    struct decimal_formatter *impl = df_from_options(This);
    if (!value) return E_POINTER;
    *value = impl->fraction_digits;
    return S_OK;
}

static HRESULT WINAPI df_options_put_FractionDigits(void *This, INT32 value)
{
    struct decimal_formatter *impl = df_from_options(This);
    impl->fraction_digits = value;
    return S_OK;
}

static HRESULT WINAPI df_options_get_IsGrouped(void *This, boolean *value)
{
    struct decimal_formatter *impl = df_from_options(This);
    if (!value) return E_POINTER;
    *value = impl->is_grouped;
    return S_OK;
}

static HRESULT WINAPI df_options_put_IsGrouped(void *This, boolean value)
{
    struct decimal_formatter *impl = df_from_options(This);
    impl->is_grouped = value;
    return S_OK;
}

static HRESULT WINAPI df_options_get_IsDecimalPointAlwaysDisplayed(void *This, boolean *value)
{
    struct decimal_formatter *impl = df_from_options(This);
    if (!value) return E_POINTER;
    *value = impl->is_dp_always_displayed;
    return S_OK;
}

static HRESULT WINAPI df_options_put_IsDecimalPointAlwaysDisplayed(void *This, boolean value)
{
    struct decimal_formatter *impl = df_from_options(This);
    impl->is_dp_always_displayed = value;
    return S_OK;
}

static HRESULT WINAPI df_options_get_NumeralSystem(void *This, HSTRING *value)
{
    return WindowsCreateString(L"Latn", 4, value);
}

static HRESULT WINAPI df_options_put_NumeralSystem(void *This, HSTRING value)
{
    return S_OK;
}

static HRESULT WINAPI df_options_get_ResolvedLanguage(void *This, HSTRING *value)
{
    struct decimal_formatter *impl = df_from_options(This);
    return WindowsCreateString(impl->resolved_language, wcslen(impl->resolved_language), value);
}

static HRESULT WINAPI df_options_get_ResolvedGeographicRegion(void *This, HSTRING *value)
{
    struct decimal_formatter *impl = df_from_options(This);
    return WindowsCreateString(impl->geographic_region, wcslen(impl->geographic_region), value);
}

static HRESULT WINAPI df_parser_QI(void *This, REFIID riid, void **out) { return df_QI_internal(df_from_parser(This), riid, out); }
static ULONG WINAPI df_parser_AddRef(void *This) { return df_AddRef_internal(df_from_parser(This)); }
static ULONG WINAPI df_parser_Release(void *This) { return df_Release_internal(df_from_parser(This)); }

static HRESULT WINAPI df_parser_ParseInt(void *This, HSTRING text, void **result)
{
    if (result) *result = NULL;
    return S_OK;
}

static HRESULT WINAPI df_parser_ParseUInt(void *This, HSTRING text, void **result)
{
    if (result) *result = NULL;
    return S_OK;
}

static HRESULT WINAPI df_parser_ParseDouble(void *This, HSTRING text, void **result)
{
    if (result) *result = NULL;
    return S_OK;
}

static HRESULT WINAPI df_rounder_QI(void *This, REFIID riid, void **out) { return df_QI_internal(df_from_rounder(This), riid, out); }
static ULONG WINAPI df_rounder_AddRef(void *This) { return df_AddRef_internal(df_from_rounder(This)); }
static ULONG WINAPI df_rounder_Release(void *This) { return df_Release_internal(df_from_rounder(This)); }

static HRESULT WINAPI df_rounder_get_NumberRounder(void *This, void **value)
{
    if (value) *value = NULL;
    return S_OK;
}

static HRESULT WINAPI df_rounder_put_NumberRounder(void *This, void *value)
{
    return S_OK;
}

static HRESULT WINAPI df_sigdigits_QI(void *This, REFIID riid, void **out) { return df_QI_internal(df_from_sigdigits(This), riid, out); }
static ULONG WINAPI df_sigdigits_AddRef(void *This) { return df_AddRef_internal(df_from_sigdigits(This)); }
static ULONG WINAPI df_sigdigits_Release(void *This) { return df_Release_internal(df_from_sigdigits(This)); }

static HRESULT WINAPI df_sigdigits_get_SignificantDigits(void *This, INT32 *value)
{
    if (value) *value = 0;
    return S_OK;
}

static HRESULT WINAPI df_sigdigits_put_SignificantDigits(void *This, INT32 value)
{
    return S_OK;
}

static HRESULT WINAPI df_signedzero_QI(void *This, REFIID riid, void **out) { return df_QI_internal(df_from_signedzero(This), riid, out); }
static ULONG WINAPI df_signedzero_AddRef(void *This) { return df_AddRef_internal(df_from_signedzero(This)); }
static ULONG WINAPI df_signedzero_Release(void *This) { return df_Release_internal(df_from_signedzero(This)); }

static HRESULT WINAPI df_signedzero_get_IsZeroSigned(void *This, boolean *value)
{
    if (value) *value = FALSE;
    return S_OK;
}

static HRESULT WINAPI df_signedzero_put_IsZeroSigned(void *This, boolean value)
{
    return S_OK;
}

static const void *decimal_formatter_vtbl[] = {
    (void *)df_formatter_QI,
    (void *)df_formatter_AddRef,
    (void *)df_formatter_Release,
    (void *)df_GetIids,
    (void *)df_GetRuntimeClassName,
    (void *)df_GetTrustLevel,
    (void *)df_FormatInt,
    (void *)df_FormatUInt,
    (void *)df_FormatDouble
};

static const void *decimal_formatter2_vtbl[] = {
    (void *)df_formatter2_QI,
    (void *)df_formatter2_AddRef,
    (void *)df_formatter2_Release,
    (void *)df_GetIids,
    (void *)df_GetRuntimeClassName,
    (void *)df_GetTrustLevel,
    (void *)df_FormatInt,
    (void *)df_FormatUInt,
    (void *)df_FormatDouble
};

static const void *decimal_formatter_options_vtbl[] = {
    (void *)df_options_QI,
    (void *)df_options_AddRef,
    (void *)df_options_Release,
    (void *)df_GetIids,
    (void *)df_GetRuntimeClassName,
    (void *)df_GetTrustLevel,
    (void *)df_options_get_Languages,
    (void *)df_options_get_GeographicRegion,
    (void *)df_options_get_IntegerDigits,
    (void *)df_options_put_IntegerDigits,
    (void *)df_options_get_FractionDigits,
    (void *)df_options_put_FractionDigits,
    (void *)df_options_get_IsGrouped,
    (void *)df_options_put_IsGrouped,
    (void *)df_options_get_IsDecimalPointAlwaysDisplayed,
    (void *)df_options_put_IsDecimalPointAlwaysDisplayed,
    (void *)df_options_get_NumeralSystem,
    (void *)df_options_put_NumeralSystem,
    (void *)df_options_get_ResolvedLanguage,
    (void *)df_options_get_ResolvedGeographicRegion
};

static const void *decimal_formatter_parser_vtbl[] = {
    (void *)df_parser_QI,
    (void *)df_parser_AddRef,
    (void *)df_parser_Release,
    (void *)df_GetIids,
    (void *)df_GetRuntimeClassName,
    (void *)df_GetTrustLevel,
    (void *)df_parser_ParseInt,
    (void *)df_parser_ParseUInt,
    (void *)df_parser_ParseDouble
};

static const void *decimal_formatter_rounder_vtbl[] = {
    (void *)df_rounder_QI,
    (void *)df_rounder_AddRef,
    (void *)df_rounder_Release,
    (void *)df_GetIids,
    (void *)df_GetRuntimeClassName,
    (void *)df_GetTrustLevel,
    (void *)df_rounder_get_NumberRounder,
    (void *)df_rounder_put_NumberRounder
};

static const void *decimal_formatter_sigdigits_vtbl[] = {
    (void *)df_sigdigits_QI,
    (void *)df_sigdigits_AddRef,
    (void *)df_sigdigits_Release,
    (void *)df_GetIids,
    (void *)df_GetRuntimeClassName,
    (void *)df_GetTrustLevel,
    (void *)df_sigdigits_get_SignificantDigits,
    (void *)df_sigdigits_put_SignificantDigits
};

static const void *decimal_formatter_signedzero_vtbl[] = {
    (void *)df_signedzero_QI,
    (void *)df_signedzero_AddRef,
    (void *)df_signedzero_Release,
    (void *)df_GetIids,
    (void *)df_GetRuntimeClassName,
    (void *)df_GetTrustLevel,
    (void *)df_signedzero_get_IsZeroSigned,
    (void *)df_signedzero_put_IsZeroSigned
};

static HRESULT decimal_formatter_create(const WCHAR *lang, const WCHAR *region, struct decimal_formatter **out)
{
    struct decimal_formatter *impl;

    if (!(impl = calloc(1, sizeof(*impl)))) return E_OUTOFMEMORY;
    impl->INumberFormatter_iface.lpVtbl = decimal_formatter_vtbl;
    impl->INumberFormatter2_iface.lpVtbl = decimal_formatter2_vtbl;
    impl->INumberFormatterOptions_iface.lpVtbl = decimal_formatter_options_vtbl;
    impl->INumberParser_iface.lpVtbl = decimal_formatter_parser_vtbl;
    impl->INumberRounderOption_iface.lpVtbl = decimal_formatter_rounder_vtbl;
    impl->ISignificantDigitsOption_iface.lpVtbl = decimal_formatter_sigdigits_vtbl;
    impl->ISignedZeroOption_iface.lpVtbl = decimal_formatter_signedzero_vtbl;
    impl->ref = 1;
    impl->fraction_digits = 2;
    impl->integer_digits = 1;
    impl->is_grouped = FALSE;
    impl->is_dp_always_displayed = FALSE;
    lstrcpynW(impl->resolved_language, lang && *lang ? lang : L"en-US", ARRAY_SIZE(impl->resolved_language));
    lstrcpynW(impl->geographic_region, region && *region ? region : L"US", ARRAY_SIZE(impl->geographic_region));

    *out = impl;
    return S_OK;
}

struct decimal_formatter_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } IDecimalFormatterFactory_iface;
    LONG ref;
};

#define impl_from_df_fac(ptr) ((struct decimal_formatter_factory_impl *)((char *)(ptr) - offsetof(struct decimal_formatter_factory_impl, IActivationFactory_iface)))
#define impl_from_df_fact_iface(ptr) ((struct decimal_formatter_factory_impl *)((char *)(ptr) - offsetof(struct decimal_formatter_factory_impl, IDecimalFormatterFactory_iface)))

static HRESULT df_fac_QI_internal(struct decimal_formatter_factory_impl *impl, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppvObject = &impl->IActivationFactory_iface;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IDecimalFormatterFactory))
    {
        *ppvObject = &impl->IDecimalFormatterFactory_iface;
        return S_OK;
    }

    WARN("df_fac %p: unhandled QI %s\n", impl, debugstr_guid(riid));
    return E_NOINTERFACE;
}

static HRESULT WINAPI df_fac_QueryInterface(IActivationFactory *iface, REFIID riid, void **ppvObject)
{
    return df_fac_QI_internal(impl_from_df_fac(iface), riid, ppvObject);
}
static ULONG WINAPI df_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI df_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI df_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI df_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n)
{
    return WindowsCreateString(L"Windows.Globalization.NumberFormatting.DecimalFormatter", 55, n);
}
static HRESULT WINAPI df_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI df_fac_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    struct decimal_formatter *df;
    HRESULT hr;

    if (!instance) return E_POINTER;
    hr = decimal_formatter_create(L"en-US", L"US", &df);
    if (SUCCEEDED(hr))
        *instance = (IInspectable *)&df->INumberFormatter_iface;
    return hr;
}

static const IActivationFactoryVtbl df_activation_factory_vtbl = {
    df_fac_QueryInterface,
    df_fac_AddRef,
    df_fac_Release,
    df_fac_GetIids,
    df_fac_GetRuntimeClassName,
    df_fac_GetTrustLevel,
    df_fac_ActivateInstance
};

static HRESULT WINAPI df_fact_QI(void *This, REFIID riid, void **ppvObject)
{
    return df_fac_QI_internal(impl_from_df_fact_iface(This), riid, ppvObject);
}
static ULONG WINAPI df_fact_AddRef(void *This) { return 2; }
static ULONG WINAPI df_fact_Release(void *This) { return 1; }
static HRESULT WINAPI df_fact_Create(void *This, void *languages, HSTRING geographicRegion, void **result)
{
    struct decimal_formatter *df;
    const WCHAR *region_w = WindowsGetStringRawBuffer(geographicRegion, NULL);
    const WCHAR *lang_w = NULL;
    IIterable_HSTRING *iterable = (IIterable_HSTRING *)languages;
    IIterator_HSTRING *iterator = NULL;
    HSTRING first_str = NULL;
    boolean has_current = FALSE;
    HRESULT hr;

    TRACE("languages %p, geographicRegion %s, result %p\n", languages, debugstr_hstring(geographicRegion), result);

    if (!result) return E_POINTER;

    if (iterable && SUCCEEDED(IIterable_HSTRING_First(iterable, &iterator)) && iterator)
    {
        if (SUCCEEDED(IIterator_HSTRING_get_HasCurrent(iterator, &has_current)) && has_current)
        {
            if (SUCCEEDED(IIterator_HSTRING_get_Current(iterator, &first_str)) && first_str)
            {
                lang_w = WindowsGetStringRawBuffer(first_str, NULL);
            }
        }
        IIterator_HSTRING_Release(iterator);
    }

    hr = decimal_formatter_create(lang_w, region_w, &df);
    if (SUCCEEDED(hr))
        *result = &df->INumberFormatter_iface;
    return hr;
}

static const void *df_factory_vtbl[] = {
    (void *)df_fact_QI,
    (void *)df_fact_AddRef,
    (void *)df_fact_Release,
    (void *)df_fac_GetIids,
    (void *)df_fac_GetRuntimeClassName,
    (void *)df_fac_GetTrustLevel,
    (void *)df_fact_Create
};

static struct decimal_formatter_factory_impl decimal_formatter_factory = {
    {&df_activation_factory_vtbl},
    {df_factory_vtbl},
    1
};

/*
 *
 * Windows.Globalization.CalendarIdentifiers
 *
 */
static const GUID IID_ICalendarIdentifiersStatics  = {0x80653f68, 0x2cb2, 0x4c1f, {0xb5, 0x90, 0xf0, 0xf5, 0x2b, 0xf4, 0xfd, 0x1a}};
static const GUID IID_ICalendarIdentifiersStatics2 = {0x7df4d488, 0x5fd0, 0x42a7, {0x95, 0xb5, 0x7d, 0x98, 0xd8, 0x23, 0x07, 0x5f}};
static const GUID IID_ICalendarIdentifiersStatics3 = {0x2c225423, 0x1fad, 0x40c0, {0x93, 0x34, 0xa8, 0xeb, 0x90, 0xdb, 0x04, 0xf5}};

struct calendar_identifiers_statics
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } ICalendarIdentifiersStatics_iface;
    struct { const void *lpVtbl; } ICalendarIdentifiersStatics2_iface;
    struct { const void *lpVtbl; } ICalendarIdentifiersStatics3_iface;
};

#define impl_from_cal_factory(ptr) ((struct calendar_identifiers_statics *)((char *)(ptr) - offsetof(struct calendar_identifiers_statics, IActivationFactory_iface)))
#define impl_from_cal_s1(ptr)      ((struct calendar_identifiers_statics *)((char *)(ptr) - offsetof(struct calendar_identifiers_statics, ICalendarIdentifiersStatics_iface)))
#define impl_from_cal_s2(ptr)      ((struct calendar_identifiers_statics *)((char *)(ptr) - offsetof(struct calendar_identifiers_statics, ICalendarIdentifiersStatics2_iface)))
#define impl_from_cal_s3(ptr)      ((struct calendar_identifiers_statics *)((char *)(ptr) - offsetof(struct calendar_identifiers_statics, ICalendarIdentifiersStatics3_iface)))

static HRESULT WINAPI cal_fac_QueryInterface(IActivationFactory *iface, REFIID iid, void **out)
{
    struct calendar_identifiers_statics *impl = impl_from_cal_factory(iface);
    if (!out) return E_POINTER;
    *out = NULL;
    if (IsEqualGUID(iid, &IID_IUnknown) ||
        IsEqualGUID(iid, &IID_IInspectable) ||
        IsEqualGUID(iid, &IID_IAgileObject) ||
        IsEqualGUID(iid, &IID_IActivationFactory))
    {
        *out = &impl->IActivationFactory_iface;
        return S_OK;
    }
    if (IsEqualGUID(iid, &IID_ICalendarIdentifiersStatics))
    {
        *out = &impl->ICalendarIdentifiersStatics_iface;
        return S_OK;
    }
    if (IsEqualGUID(iid, &IID_ICalendarIdentifiersStatics2))
    {
        *out = &impl->ICalendarIdentifiersStatics2_iface;
        return S_OK;
    }
    if (IsEqualGUID(iid, &IID_ICalendarIdentifiersStatics3))
    {
        *out = &impl->ICalendarIdentifiersStatics3_iface;
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI cal_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI cal_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI cal_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI cal_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n) { return WindowsCreateString(L"Windows.Globalization.CalendarIdentifiers", 41, n); }
static HRESULT WINAPI cal_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI cal_fac_ActivateInstance(IActivationFactory *iface, IInspectable **instance) { return CLASS_E_NOTLICENSED; }

static const struct IActivationFactoryVtbl cal_activation_factory_vtbl = {
    cal_fac_QueryInterface,
    cal_fac_AddRef,
    cal_fac_Release,
    cal_fac_GetIids,
    cal_fac_GetRuntimeClassName,
    cal_fac_GetTrustLevel,
    cal_fac_ActivateInstance
};

static HRESULT WINAPI cal_s1_QI(void *This, REFIID iid, void **out) { return cal_fac_QueryInterface(&impl_from_cal_s1(This)->IActivationFactory_iface, iid, out); }
static ULONG WINAPI cal_s1_AddRef(void *This) { return 2; }
static ULONG WINAPI cal_s1_Release(void *This) { return 1; }
static HRESULT WINAPI cal_s1_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI cal_s1_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.Globalization.CalendarIdentifiers", 41, n); }
static HRESULT WINAPI cal_s1_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI cal_s1_get_Gregorian(void *This, HSTRING *val) { return WindowsCreateString(L"GregorianCalendar", 17, val); }
static HRESULT WINAPI cal_s1_get_Hebrew(void *This, HSTRING *val) { return WindowsCreateString(L"HebrewCalendar", 14, val); }
static HRESULT WINAPI cal_s1_get_Hijri(void *This, HSTRING *val) { return WindowsCreateString(L"HijriCalendar", 13, val); }
static HRESULT WINAPI cal_s1_get_Japanese(void *This, HSTRING *val) { return WindowsCreateString(L"JapaneseCalendar", 16, val); }
static HRESULT WINAPI cal_s1_get_Julian(void *This, HSTRING *val) { return WindowsCreateString(L"JulianCalendar", 14, val); }
static HRESULT WINAPI cal_s1_get_Korean(void *This, HSTRING *val) { return WindowsCreateString(L"KoreanCalendar", 14, val); }
static HRESULT WINAPI cal_s1_get_Taiwan(void *This, HSTRING *val) { return WindowsCreateString(L"TaiwanCalendar", 14, val); }
static HRESULT WINAPI cal_s1_get_Thai(void *This, HSTRING *val) { return WindowsCreateString(L"ThaiCalendar", 12, val); }
static HRESULT WINAPI cal_s1_get_UmAlQura(void *This, HSTRING *val) { return WindowsCreateString(L"UmAlQuraCalendar", 16, val); }

static const void *cal_s1_vtbl[] = {
    (void *)cal_s1_QI, (void *)cal_s1_AddRef, (void *)cal_s1_Release,
    (void *)cal_s1_GetIids, (void *)cal_s1_GetRuntimeClassName, (void *)cal_s1_GetTrustLevel,
    (void *)cal_s1_get_Gregorian,
    (void *)cal_s1_get_Hebrew,
    (void *)cal_s1_get_Hijri,
    (void *)cal_s1_get_Japanese,
    (void *)cal_s1_get_Julian,
    (void *)cal_s1_get_Korean,
    (void *)cal_s1_get_Taiwan,
    (void *)cal_s1_get_Thai,
    (void *)cal_s1_get_UmAlQura
};

static HRESULT WINAPI cal_s2_QI(void *This, REFIID iid, void **out) { return cal_fac_QueryInterface(&impl_from_cal_s2(This)->IActivationFactory_iface, iid, out); }
static HRESULT WINAPI cal_s2_get_Persian(void *This, HSTRING *val) { return WindowsCreateString(L"PersianCalendar", 15, val); }

static const void *cal_s2_vtbl[] = {
    (void *)cal_s2_QI, (void *)cal_s1_AddRef, (void *)cal_s1_Release,
    (void *)cal_s1_GetIids, (void *)cal_s1_GetRuntimeClassName, (void *)cal_s1_GetTrustLevel,
    (void *)cal_s2_get_Persian
};

static HRESULT WINAPI cal_s3_QI(void *This, REFIID iid, void **out) { return cal_fac_QueryInterface(&impl_from_cal_s3(This)->IActivationFactory_iface, iid, out); }
static HRESULT WINAPI cal_s3_get_ChineseLunar(void *This, HSTRING *val) { return WindowsCreateString(L"ChineseLunarCalendar", 20, val); }
static HRESULT WINAPI cal_s3_get_JapaneseLunar(void *This, HSTRING *val) { return WindowsCreateString(L"JapaneseLunarCalendar", 21, val); }
static HRESULT WINAPI cal_s3_get_KoreanLunar(void *This, HSTRING *val) { return WindowsCreateString(L"KoreanLunarCalendar", 19, val); }
static HRESULT WINAPI cal_s3_get_TaiwanLunar(void *This, HSTRING *val) { return WindowsCreateString(L"TaiwanLunarCalendar", 19, val); }
static HRESULT WINAPI cal_s3_get_VietnameseLunar(void *This, HSTRING *val) { return WindowsCreateString(L"VietnameseLunarCalendar", 23, val); }

static const void *cal_s3_vtbl[] = {
    (void *)cal_s3_QI, (void *)cal_s1_AddRef, (void *)cal_s1_Release,
    (void *)cal_s1_GetIids, (void *)cal_s1_GetRuntimeClassName, (void *)cal_s1_GetTrustLevel,
    (void *)cal_s3_get_ChineseLunar,
    (void *)cal_s3_get_JapaneseLunar,
    (void *)cal_s3_get_KoreanLunar,
    (void *)cal_s3_get_TaiwanLunar,
    (void *)cal_s3_get_VietnameseLunar
};

static struct calendar_identifiers_statics calendar_identifiers_factory = {
    { &cal_activation_factory_vtbl },
    { cal_s1_vtbl },
    { cal_s2_vtbl },
    { cal_s3_vtbl }
};

HRESULT WINAPI DllGetClassObject(REFCLSID clsid, REFIID riid, void **out)
{
    FIXME("clsid %s, riid %s, out %p stub!\n", debugstr_guid(clsid), debugstr_guid(riid), out);
    return CLASS_E_CLASSNOTAVAILABLE;
}

HRESULT WINAPI DllGetActivationFactory(HSTRING classid, IActivationFactory **factory)
{
    const WCHAR *name = WindowsGetStringRawBuffer(classid, NULL);

    TRACE("classid %s, factory %p.\n", debugstr_hstring(classid), factory);

    *factory = NULL;

    if (!wcscmp(name, RuntimeClass_Windows_System_UserProfile_GlobalizationPreferences))
    {
        *factory = &userprofile_preferences.IActivationFactory_iface;
        IUnknown_AddRef(*factory);
    }
    else if (!wcscmp(name, RuntimeClass_Windows_Globalization_Language))
    {
        *factory = &language_factory.IActivationFactory_iface;
        IUnknown_AddRef(*factory);
    }
    else if (!wcscmp(name, RuntimeClass_Windows_Globalization_GeographicRegion))
    {
        *factory = geographic_region_factory;
        IUnknown_AddRef(*factory);
    }
    else if (!wcscmp(name, RuntimeClass_Windows_Globalization_ApplicationLanguages))
    {
        *factory = &application_languages.IActivationFactory_iface;
        IUnknown_AddRef(*factory);
    }
    else if (!wcscmp(name, L"Windows.Globalization.Fonts.LanguageFontGroup"))
    {
        *factory = &font_group_factory.IActivationFactory_iface;
        IUnknown_AddRef(*factory);
    }
    else if (!wcscmp(name, L"Windows.Globalization.NumberFormatting.DecimalFormatter"))
    {
        *factory = &decimal_formatter_factory.IActivationFactory_iface;
        IUnknown_AddRef(*factory);
    }
    else if (!wcscmp(name, L"Windows.Globalization.CalendarIdentifiers"))
    {
        *factory = &calendar_identifiers_factory.IActivationFactory_iface;
        IUnknown_AddRef(*factory);
    }

    if (*factory) return S_OK;
    return CLASS_E_CLASSNOTAVAILABLE;
}
