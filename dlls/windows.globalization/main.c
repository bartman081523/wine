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

    if (*factory) return S_OK;
    return CLASS_E_CLASSNOTAVAILABLE;
}
