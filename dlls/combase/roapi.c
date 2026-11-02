/*
 * Copyright 2014 Martin Storsjo
 * Copyright 2016 Michael Müller
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
#define COBJMACROS
#include "objbase.h"
#include "initguid.h"
#include "roapi.h"
#include "roparameterizediid.h"
#include "roerrorapi.h"
#include "restrictederrorinfo.h"
#include "winstring.h"
#include "errhandlingapi.h"
#include "oleauto.h"

#include "combase_private.h"

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(combase);

struct activatable_class_data
{
    ULONG size;
    DWORD unk;
    DWORD module_len;
    DWORD module_offset;
    DWORD threading_model;
};

static HRESULT get_library_for_classid(const WCHAR *classid, WCHAR **out)
{
    ACTCTX_SECTION_KEYED_DATA data;
    HKEY hkey_root, hkey_class;
    DWORD type, size;
    HRESULT hr;
    WCHAR *buf = NULL;

    *out = NULL;

    /* search activation context first */
    data.cbSize = sizeof(data);
    if (FindActCtxSectionStringW(FIND_ACTCTX_SECTION_KEY_RETURN_HACTCTX, NULL,
            ACTIVATION_CONTEXT_SECTION_WINRT_ACTIVATABLE_CLASSES, classid, &data))
    {
        struct activatable_class_data *activatable_class = (struct activatable_class_data *)data.lpData;
        void *ptr = (BYTE *)data.lpSectionBase + activatable_class->module_offset;
        *out = wcsdup(ptr);
        return S_OK;
    }

    /* load class registry key */
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\WindowsRuntime\\ActivatableClassId",
                      0, KEY_READ, &hkey_root))
        return REGDB_E_READREGDB;
    if (RegOpenKeyExW(hkey_root, classid, 0, KEY_READ, &hkey_class))
    {
        RegCloseKey(hkey_root);
        if (!wcscmp(classid, L"Windows.Security.Cryptography.DataProtection.DataProtectionProvider"))
        {
            *out = wcsdup(L"cryptowinrt.dll");
            return S_OK;
        }
        if (!wcsncmp(classid, L"Windows.ApplicationModel.", 25))
        {
            *out = wcsdup(L"windows.applicationmodel.dll");
            return S_OK;
        }
        if (!wcsncmp(classid, L"Windows.UI.Composition.", 23))
        {
            *out = wcsdup(L"dcomp.dll");
            return S_OK;
        }
        if (!wcsncmp(classid, L"Windows.UI.Core.", 16))
        {
            *out = wcsdup(L"windows.ui.dll");
            return S_OK;
        }
        if (!wcsncmp(classid, L"Windows.UI.Shell.", 17))
        {
            *out = wcsdup(L"windows.ui.dll");
            return S_OK;
        }
        if (!wcsncmp(classid, L"Windows.UI.Notifications.", 25))
        {
            *out = wcsdup(L"windows.ui.dll");
            return S_OK;
        }
        if (!wcsncmp(classid, L"Windows.UI.ViewManagement.", 26))
        {
            *out = wcsdup(L"windows.ui.dll");
            return S_OK;
        }
        if (!wcsncmp(classid, L"Windows.UI.Input.", 17))
        {
            *out = wcsdup(L"windows.ui.dll");
            return S_OK;
        }
        if (!wcsncmp(classid, L"Windows.Graphics.Display.", 25))
        {
            *out = wcsdup(L"windows.ui.dll");
            return S_OK;
        }
        if (!wcsncmp(classid, L"Windows.Devices.Input.", 22))
        {
            *out = wcsdup(L"windows.ui.dll");
            return S_OK;
        }
        if (!wcsncmp(classid, L"Windows.Devices.Lights.", 23))
        {
            *out = wcsdup(L"windows.devices.enumeration.dll");
            return S_OK;
        }
        if (!wcsncmp(classid, L"Windows.System.Profile.", 23))
        {
            *out = wcsdup(L"twinapi.appcore.dll");
            return S_OK;
        }
        if (!wcsncmp(classid, L"Windows.Web.Http.", 17))
        {
            *out = wcsdup(L"windows.web.dll");
            return S_OK;
        }
        if (!wcsncmp(classid, L"Windows.Security.Authentication.Web.Core.", 41))
        {
            *out = wcsdup(L"twinapi.appcore.dll");
            return S_OK;
        }
        if (!wcsncmp(classid, L"Windows.Networking.", 19))
        {
            *out = wcsdup(L"windows.networking.dll");
            return S_OK;
        }
        if (!wcsncmp(classid, L"Windows.System.Threading.", 25))
        {
            *out = wcsdup(L"threadpoolwinrt.dll");
            return S_OK;
        }
        if (!wcsncmp(classid, L"Windows.Services.Store.", 23))
        {
            *out = wcsdup(L"windows.applicationmodel.dll");
            return S_OK;
        }
        if (!wcsncmp(classid, L"Windows.Management.Core.", 24))
        {
            *out = wcsdup(L"windows.storage.applicationdata.dll");
            return S_OK;
        }
        if (!wcsncmp(classid, L"Windows.Storage.ApplicationData", 30))
        {
            *out = wcsdup(L"windows.storage.applicationdata.dll");
            return S_OK;
        }
        if (!wcsncmp(classid, L"Windows.Storage.", 16))
        {
            *out = wcsdup(L"windows.storage.dll");
            return S_OK;
        }
        if (!wcsncmp(classid, L"Windows.Globalization.", 22))
        {
            *out = wcsdup(L"windows.globalization.dll");
            return S_OK;
        }
        if (!wcsncmp(classid, L"Windows.Media.Audio.", 20) ||
            !wcsncmp(classid, L"Windows.Media.Devices.", 22))
        {
            *out = wcsdup(L"windows.media.devices.dll");
            return S_OK;
        }
        if (!wcsncmp(classid, L"Windows.UI.Xaml.", 16))
        {
            *out = wcsdup(L"windows.ui.xaml.dll");
            return S_OK;
        }
        if (!wcsncmp(classid, L"Microsoft.UI.Xaml.", 18))
        {
            *out = wcsdup(L"Microsoft.UI.Xaml.dll");
            return S_OK;
        }
        if (!wcsncmp(classid, L"Windows.System.MemoryManager", 28))
        {
            *out = wcsdup(L"windows.applicationmodel.dll");
            return S_OK;
        }
        if (!wcsncmp(classid, L"Windows.System.User", 19))
        {
            *out = wcsdup(L"windows.ui.xaml.dll");
            return S_OK;
        }
        if (!wcsncmp(classid, L"CalculatorApp.ViewModel.", 24) ||
            !wcsncmp(classid, L"CalculatorApp.", 14))
        {
            *out = wcsdup(L"CalcViewModel.dll");
            return S_OK;
        }
        if (!wcsncmp(classid, L"GraphControl.", 13))
        {
            *out = wcsdup(L"GraphControl.dll");
            return S_OK;
        }
        if (!wcsncmp(classid, L"TraceLogging.", 13))
        {
            *out = wcsdup(L"TraceLogging.dll");
            return S_OK;
        }
        if (!wcsncmp(classid, L"Microsoft.Web.WebView2.Core.", 29))
        {
            *out = wcsdup(L"Microsoft.Web.WebView2.Core.dll");
            return S_OK;
        }
        if (!wcsncmp(classid, L"Microsoft.Toolkit.Uwp.Notifications.", 36) ||
            !wcsncmp(classid, L"McgBridge.", 10) ||
            !wcsncmp(classid, L"Microsoft.AdSdk.", 16) ||
            !wcsncmp(classid, L"OM.SDKWindows.", 14))
        {
            *out = wcsdup(L"Microsoft.MicrosoftSolitaireCollection.dll");
            return S_OK;
        }
        ERR("Class %s not found in registry\n", debugstr_w(classid));
        return REGDB_E_CLASSNOTREG;
    }
    RegCloseKey(hkey_root);

    /* load (and expand) DllPath registry value */
    if (RegQueryValueExW(hkey_class, L"DllPath", NULL, &type, NULL, &size))
    {
        hr = REGDB_E_READREGDB;
        goto done;
    }
    if (type != REG_SZ && type != REG_EXPAND_SZ)
    {
        hr = REGDB_E_READREGDB;
        goto done;
    }
    if (!(buf = malloc(size)))
    {
        hr = E_OUTOFMEMORY;
        goto done;
    }
    if (RegQueryValueExW(hkey_class, L"DllPath", NULL, NULL, (BYTE *)buf, &size))
    {
        hr = REGDB_E_READREGDB;
        goto done;
    }
    if (type == REG_EXPAND_SZ)
    {
        WCHAR *expanded;
        DWORD len = ExpandEnvironmentStringsW(buf, NULL, 0);
        if (!(expanded = malloc(len * sizeof(WCHAR))))
        {
            hr = E_OUTOFMEMORY;
            goto done;
        }
        ExpandEnvironmentStringsW(buf, expanded, len);
        free(buf);
        buf = expanded;
    }

    *out = buf;
    return S_OK;

done:
    free(buf);
    RegCloseKey(hkey_class);
    return hr;
}


/***********************************************************************
 *      RoInitialize (combase.@)
 */
HRESULT WINAPI RoInitialize(RO_INIT_TYPE type)
{
    switch (type) {
    case RO_INIT_SINGLETHREADED:
        return CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    default:
        FIXME("type %d\n", type);
    case RO_INIT_MULTITHREADED:
        return CoInitializeEx(NULL, COINIT_MULTITHREADED);
    }
}

/***********************************************************************
 *      RoUninitialize (combase.@)
 */
void WINAPI RoUninitialize(void)
{
    CoUninitialize();
}

/* Stub implementation for McgBridge.Uwp.Consent.ConsentBridge and ConsentBridgeSettings */
struct stub_consent_async_op
{
    void *vtbl;
    LONG ref;
};

static HRESULT WINAPI consent_async_QueryInterface(void *this, REFIID riid, void **ppv)
{
    if (!ppv) return E_POINTER;
    *ppv = this;
    return S_OK;
}
static ULONG WINAPI consent_async_AddRef(void *this) { return 2; }
static ULONG WINAPI consent_async_Release(void *this) { return 1; }
static HRESULT WINAPI consent_async_GetIids(void *this, ULONG *count, IID **iids) { if (count) *count = 0; return S_OK; }
static HRESULT WINAPI consent_async_GetRuntimeClassName(void *this, HSTRING *name) { return S_OK; }
static HRESULT WINAPI consent_async_GetTrustLevel(void *this, TrustLevel *trust) { if (trust) *trust = BaseTrust; return S_OK; }
static HRESULT WINAPI consent_async_put_Completed(void *this, void *handler)
{
    if (handler)
    {
        void **vtbl = *(void ***)handler;
        HRESULT (WINAPI *invoke)(void *, void *, int) = (void *)vtbl[3];
        invoke(handler, this, 1 /* Completed */);
    }
    return S_OK;
}
static HRESULT WINAPI consent_async_get_Completed(void *this, void **handler)
{
    if (handler) *handler = NULL;
    return S_OK;
}
static HRESULT WINAPI consent_async_GetResults(void *this, BOOLEAN *results)
{
    if (results) *results = FALSE;
    return S_OK;
}
static HRESULT WINAPI consent_async_get_Id(void *this, unsigned int *id) { if (id) *id = 1; return S_OK; }
static HRESULT WINAPI consent_async_get_Status(void *this, int *status) { if (status) *status = 1; return S_OK; }
static HRESULT WINAPI consent_async_get_ErrorCode(void *this, HRESULT *ec) { if (ec) *ec = S_OK; return S_OK; }
static HRESULT WINAPI consent_async_Cancel(void *this) { return S_OK; }
static HRESULT WINAPI consent_async_Close(void *this) { return S_OK; }

static void *stub_consent_async_vtbl[] = {
    consent_async_QueryInterface,
    consent_async_AddRef,
    consent_async_Release,
    consent_async_GetIids,
    consent_async_GetRuntimeClassName,
    consent_async_GetTrustLevel,
    consent_async_put_Completed,
    consent_async_get_Completed,
    consent_async_GetResults,
    consent_async_get_Id,
    consent_async_get_Status,
    consent_async_get_ErrorCode,
    consent_async_Cancel,
    consent_async_Close,
    consent_async_GetResults, consent_async_GetResults,
};
static struct stub_consent_async_op global_consent_async_op = { stub_consent_async_vtbl, 1 };

struct stub_consent_bridge
{
    void *vtbl;
    LONG ref;
};

static HRESULT WINAPI consent_bridge_QueryInterface(void *this, REFIID riid, void **ppv)
{
    if (!ppv) return E_POINTER;
    *ppv = this;
    return S_OK;
}
static ULONG WINAPI consent_bridge_AddRef(void *this) { return 2; }
static ULONG WINAPI consent_bridge_Release(void *this) { return 1; }
static HRESULT WINAPI consent_bridge_GetIids(void *this, ULONG *count, IID **iids) { if (count) *count = 0; return S_OK; }
static HRESULT WINAPI consent_bridge_GetRuntimeClassName(void *this, HSTRING *name) { return S_OK; }
static HRESULT WINAPI consent_bridge_GetTrustLevel(void *this, TrustLevel *trust) { if (trust) *trust = BaseTrust; return S_OK; }
static HRESULT WINAPI consent_bridge_nop(void *this) { return S_OK; }
static HRESULT WINAPI consent_bridge_CheckConsentRequired(void *this, void **out_op)
{
    if (out_op) *out_op = &global_consent_async_op;
    return S_OK;
}

static void *stub_consent_bridge_vtbl[] = {
    consent_bridge_QueryInterface,
    consent_bridge_AddRef,
    consent_bridge_Release,
    consent_bridge_GetIids,
    consent_bridge_GetRuntimeClassName,
    consent_bridge_GetTrustLevel,
    consent_bridge_nop,
    consent_bridge_nop,
    consent_bridge_CheckConsentRequired,
    consent_bridge_nop,
    consent_bridge_nop,
    consent_bridge_nop,
    consent_bridge_nop,
    consent_bridge_nop,
    consent_bridge_nop,
    consent_bridge_nop,
};
static struct stub_consent_bridge global_consent_bridge = { stub_consent_bridge_vtbl, 1 };

static HRESULT WINAPI consent_factory_QueryInterface(void *this, REFIID riid, void **ppv)
{
    if (!ppv) return E_POINTER;
    *ppv = this;
    return S_OK;
}
static ULONG WINAPI consent_factory_AddRef(void *this) { return 2; }
static ULONG WINAPI consent_factory_Release(void *this) { return 1; }
static HRESULT WINAPI consent_factory_GetIids(void *this, ULONG *count, IID **iids) { if (count) *count = 0; return S_OK; }
static HRESULT WINAPI consent_factory_GetRuntimeClassName(void *this, HSTRING *name) { return S_OK; }
static HRESULT WINAPI consent_factory_GetTrustLevel(void *this, TrustLevel *trust) { if (trust) *trust = BaseTrust; return S_OK; }
static HRESULT WINAPI consent_factory_CreateInstance(void *this, void *settings, void **out_instance)
{
    if (out_instance) *out_instance = &global_consent_bridge;
    return S_OK;
}

static void *stub_consent_factory_vtbl[] = {
    consent_factory_QueryInterface,
    consent_factory_AddRef,
    consent_factory_Release,
    consent_factory_GetIids,
    consent_factory_GetRuntimeClassName,
    consent_factory_GetTrustLevel,
    consent_factory_CreateInstance,
    consent_factory_CreateInstance,
    consent_factory_CreateInstance,
    consent_factory_CreateInstance,
};
static struct { void *vtbl; LONG ref; } global_consent_factory = { stub_consent_factory_vtbl, 1 };

/* Stub for ConsentBridgeSettings */
static HRESULT WINAPI consent_settings_QueryInterface(void *this, REFIID riid, void **ppv)
{
    if (!ppv) return E_POINTER;
    *ppv = this;
    return S_OK;
}
static ULONG WINAPI consent_settings_AddRef(void *this) { return 2; }
static ULONG WINAPI consent_settings_Release(void *this) { return 1; }
static HRESULT WINAPI consent_settings_GetIids(void *this, ULONG *count, IID **iids) { if (count) *count = 0; return S_OK; }
static HRESULT WINAPI consent_settings_GetRuntimeClassName(void *this, HSTRING *name) { return S_OK; }
static HRESULT WINAPI consent_settings_GetTrustLevel(void *this, TrustLevel *trust) { if (trust) *trust = BaseTrust; return S_OK; }
static HRESULT WINAPI consent_settings_slot(void *this) { return S_OK; }

static void *stub_consent_settings_vtbl[50];
static struct { void *vtbl; LONG ref; } global_consent_settings;

static void init_consent_settings_vtbl(void)
{
    int i;
    if (stub_consent_settings_vtbl[0]) return;
    stub_consent_settings_vtbl[0] = consent_settings_QueryInterface;
    stub_consent_settings_vtbl[1] = consent_settings_AddRef;
    stub_consent_settings_vtbl[2] = consent_settings_Release;
    stub_consent_settings_vtbl[3] = consent_settings_GetIids;
    stub_consent_settings_vtbl[4] = consent_settings_GetRuntimeClassName;
    stub_consent_settings_vtbl[5] = consent_settings_GetTrustLevel;
    for (i = 6; i < 50; i++)
        stub_consent_settings_vtbl[i] = consent_settings_slot;
    global_consent_settings.vtbl = stub_consent_settings_vtbl;
    global_consent_settings.ref = 1;
}

static HRESULT WINAPI consent_settings_factory_ActivateInstance(void *this, IInspectable **instance)
{
    init_consent_settings_vtbl();
    if (instance) *instance = (IInspectable *)&global_consent_settings;
    return S_OK;
}

static void *stub_consent_settings_factory_vtbl[] = {
    consent_factory_QueryInterface,
    consent_factory_AddRef,
    consent_factory_Release,
    consent_factory_GetIids,
    consent_factory_GetRuntimeClassName,
    consent_factory_GetTrustLevel,
    consent_settings_factory_ActivateInstance,
};
static struct { void *vtbl; LONG ref; } global_consent_settings_factory = { stub_consent_settings_factory_vtbl, 1 };

/***********************************************************************
 *      RoGetActivationFactory (combase.@)
 */
HRESULT WINAPI DECLSPEC_HOTPATCH RoGetActivationFactory(HSTRING classid, REFIID iid, void **class_factory)
{
    PFNGETACTIVATIONFACTORY pDllGetActivationFactory;
    IActivationFactory *factory;
    const WCHAR *class_str;
    WCHAR *library;
    HMODULE module;
    HRESULT hr;

    FIXME("(%s, %s, %p): semi-stub\n", debugstr_hstring(classid), debugstr_guid(iid), class_factory);

    if (!iid || !class_factory)
        return E_INVALIDARG;

    if (FAILED(hr = ensure_mta()))
        return hr;

    class_str = WindowsGetStringRawBuffer(classid, NULL);
    if (class_str)
    {
        if (!wcscmp(class_str, L"McgBridge.Uwp.Consent.ConsentBridge"))
        {
            TRACE("Returning stub factory for %s\n", debugstr_w(class_str));
            *class_factory = &global_consent_factory;
            return S_OK;
        }
        if (!wcscmp(class_str, L"McgBridge.Uwp.Consent.ConsentBridgeSettings"))
        {
            TRACE("Returning stub settings factory for %s\n", debugstr_w(class_str));
            init_consent_settings_vtbl();
            *class_factory = &global_consent_settings_factory;
            return S_OK;
        }
    }

    hr = get_library_for_classid(WindowsGetStringRawBuffer(classid, NULL), &library);
    if (FAILED(hr))
    {
        ERR("Failed to find library for %s\n", debugstr_hstring(classid));
        return hr;
    }

    if (!(module = LoadLibraryW(library)))
    {
        ERR("Failed to load module %s\n", debugstr_w(library));
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto done;
    }

    if (!(pDllGetActivationFactory = (void *)GetProcAddress(module, "DllGetActivationFactory")))
    {
        ERR("Module %s does not implement DllGetActivationFactory\n", debugstr_w(library));
        hr = E_FAIL;
        goto done;
    }

    TRACE("Found library %s for class %s\n", debugstr_w(library), debugstr_hstring(classid));

    hr = pDllGetActivationFactory(classid, &factory);
    if (SUCCEEDED(hr))
    {
        hr = IActivationFactory_QueryInterface(factory, iid, class_factory);
        if (SUCCEEDED(hr))
        {
            TRACE("Created interface %p\n", *class_factory);
            module = NULL;
        }
        IActivationFactory_Release(factory);
    }
    else
    {
        ERR("Class %s not found in %s, hr %#lx.\n", wine_dbgstr_hstring(classid), debugstr_w(library), hr);
    }

done:
    free(library);
    if (module) FreeLibrary(module);
    return hr;
}

/***********************************************************************
 *      RoGetParameterizedTypeInstanceIID (combase.@)
 */
HRESULT WINAPI RoGetParameterizedTypeInstanceIID(UINT32 name_element_count, const WCHAR **name_elements,
                                                 const IRoMetaDataLocator *meta_data_locator, GUID *iid,
                                                 ROPARAMIIDHANDLE *hiid)
{
    FIXME("stub: %d %p %p %p %p\n", name_element_count, name_elements, meta_data_locator, iid, hiid);
    if (iid) *iid = GUID_NULL;
    if (hiid) *hiid = INVALID_HANDLE_VALUE;
    return E_NOTIMPL;
}

/***********************************************************************
 *      RoActivateInstance (combase.@)
 */
HRESULT WINAPI RoActivateInstance(HSTRING classid, IInspectable **instance)
{
    IActivationFactory *factory;
    HRESULT hr;

    FIXME("(%p, %p): semi-stub\n", classid, instance);

    hr = RoGetActivationFactory(classid, &IID_IActivationFactory, (void **)&factory);
    if (SUCCEEDED(hr))
    {
        hr = IActivationFactory_ActivateInstance(factory, instance);
        IActivationFactory_Release(factory);
    }

    return hr;
}

struct agile_reference
{
    IAgileReference IAgileReference_iface;
    enum AgileReferenceOptions option;
    IStream *marshal_stream;
    CRITICAL_SECTION cs;
    IUnknown *obj;
    BOOLEAN is_agile;
    LONG ref;
};

static HRESULT marshal_object_in_agile_reference(struct agile_reference *ref, REFIID riid, IUnknown *obj)
{
    HRESULT hr;

    hr = CreateStreamOnHGlobal(0, TRUE, &ref->marshal_stream);
    if (FAILED(hr))
        return hr;

    hr = CoMarshalInterface(ref->marshal_stream, riid, obj, MSHCTX_INPROC, NULL, MSHLFLAGS_TABLESTRONG);
    if (FAILED(hr))
    {
        IStream_Release(ref->marshal_stream);
        ref->marshal_stream = NULL;
    }
    return hr;
}

static inline struct agile_reference *impl_from_IAgileReference(IAgileReference *iface)
{
    return CONTAINING_RECORD(iface, struct agile_reference, IAgileReference_iface);
}

static HRESULT WINAPI agile_ref_QueryInterface(IAgileReference *iface, REFIID riid, void **obj)
{
    TRACE("(%p, %s, %p)\n", iface, debugstr_guid(riid), obj);

    if (!riid || !obj) return E_INVALIDARG;

    if (IsEqualGUID(riid, &IID_IUnknown)
        || IsEqualGUID(riid, &IID_IAgileObject)
        || IsEqualGUID(riid, &IID_IAgileReference))
    {
        IUnknown_AddRef(iface);
        *obj = iface;
        return S_OK;
    }

    *obj = NULL;
    FIXME("interface %s is not implemented\n", debugstr_guid(riid));
    return E_NOINTERFACE;
}

static ULONG WINAPI agile_ref_AddRef(IAgileReference *iface)
{
    struct agile_reference *impl = impl_from_IAgileReference(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI agile_ref_Release(IAgileReference *iface)
{
    struct agile_reference *impl = impl_from_IAgileReference(iface);
    LONG ref = InterlockedDecrement(&impl->ref);

    if (!ref)
    {
        TRACE("destroying %p\n", iface);

        if (impl->obj)
            IUnknown_Release(impl->obj);

        if (impl->marshal_stream)
        {
            LARGE_INTEGER zero = {0};

            IStream_Seek(impl->marshal_stream, zero, STREAM_SEEK_SET, NULL);
            CoReleaseMarshalData(impl->marshal_stream);
            IStream_Release(impl->marshal_stream);
        }
        DeleteCriticalSection(&impl->cs);
        free(impl);
    }

    return ref;
}

static HRESULT WINAPI agile_ref_Resolve(IAgileReference *iface, REFIID riid, void **obj)
{
    struct agile_reference *impl = impl_from_IAgileReference(iface);
    LARGE_INTEGER zero = {0};
    HRESULT hr;

    TRACE("(%p, %s, %p)\n", iface, debugstr_guid(riid), obj);

    if (impl->is_agile)
        return IUnknown_QueryInterface(impl->obj, riid, obj);

    EnterCriticalSection(&impl->cs);
    if (impl->option == AGILEREFERENCE_DELAYEDMARSHAL && impl->marshal_stream == NULL)
    {
        if (FAILED(hr = marshal_object_in_agile_reference(impl, riid, impl->obj)))
        {
            LeaveCriticalSection(&impl->cs);
            return hr;
        }

        IUnknown_Release(impl->obj);
        impl->obj = NULL;
    }

    if (SUCCEEDED(hr = IStream_Seek(impl->marshal_stream, zero, STREAM_SEEK_SET, NULL)))
        hr = CoUnmarshalInterface(impl->marshal_stream, riid, obj);

    LeaveCriticalSection(&impl->cs);
    return hr;
}

static const IAgileReferenceVtbl agile_ref_vtbl =
{
    agile_ref_QueryInterface,
    agile_ref_AddRef,
    agile_ref_Release,
    agile_ref_Resolve,
};

static BOOL object_has_interface(IUnknown *obj, REFIID iid)
{
    IUnknown *unk;
    HRESULT hr;

    hr = IUnknown_QueryInterface(obj, iid, (void **)&unk);
    if (SUCCEEDED(hr))
        IUnknown_Release(unk);
    return SUCCEEDED(hr);
}

/***********************************************************************
 *      RoGetAgileReference (combase.@)
 */
HRESULT WINAPI RoGetAgileReference(enum AgileReferenceOptions option, REFIID riid, IUnknown *obj,
                                   IAgileReference **agile_reference)
{
    struct agile_reference *impl;
    HRESULT hr;

    TRACE("(%d, %s, %p, %p).\n", option, debugstr_guid(riid), obj, agile_reference);

    if (option != AGILEREFERENCE_DEFAULT && option != AGILEREFERENCE_DELAYEDMARSHAL)
        return E_INVALIDARG;

    if (!InternalIsProcessInitialized())
    {
        ERR("Apartment not initialized\n");
        return CO_E_NOTINITIALIZED;
    }

    if (!object_has_interface(obj, riid))
        return E_NOINTERFACE;
    if (object_has_interface(obj, &IID_INoMarshal))
        return CO_E_NOT_SUPPORTED;

    impl = calloc(1, sizeof(*impl));
    if (!impl)
        return E_OUTOFMEMORY;

    impl->IAgileReference_iface.lpVtbl = &agile_ref_vtbl;
    impl->option = option;
    impl->is_agile = object_has_interface(obj, &IID_IAgileObject);
    impl->ref = 1;

    if (option == AGILEREFERENCE_DELAYEDMARSHAL || impl->is_agile)
    {
        impl->obj = obj;
        IUnknown_AddRef(impl->obj);
    }
    else if (option == AGILEREFERENCE_DEFAULT)
    {
        if (FAILED(hr = marshal_object_in_agile_reference(impl, riid, obj)))
        {
            free(impl);
            return hr;
        }
    }

    InitializeCriticalSection(&impl->cs);

    *agile_reference = &impl->IAgileReference_iface;
    return S_OK;
}


/***********************************************************************
 *      RoGetApartmentIdentifier (combase.@)
 */
HRESULT WINAPI RoGetApartmentIdentifier(UINT64 *identifier)
{
    FIXME("(%p): stub\n", identifier);

    if (!identifier)
        return E_INVALIDARG;

    *identifier = 0xdeadbeef;
    return S_OK;
}

/***********************************************************************
 *      RoRegisterForApartmentShutdown (combase.@)
 */
HRESULT WINAPI RoRegisterForApartmentShutdown(IApartmentShutdown *callback,
        UINT64 *identifier, APARTMENT_SHUTDOWN_REGISTRATION_COOKIE *cookie)
{
    HRESULT hr;

    FIXME("(%p, %p, %p): stub\n", callback, identifier, cookie);

    hr = RoGetApartmentIdentifier(identifier);
    if (FAILED(hr))
        return hr;

    if (cookie)
        *cookie = (void *)0xcafecafe;
    return S_OK;
}

/***********************************************************************
 *      RoGetServerActivatableClasses (combase.@)
 */
HRESULT WINAPI RoGetServerActivatableClasses(HSTRING name, HSTRING **classes, DWORD *count)
{
    FIXME("(%p, %p, %p): stub\n", name, classes, count);

    if (count)
        *count = 0;
    return S_OK;
}

/***********************************************************************
 *      RoRegisterActivationFactories (combase.@)
 */
HRESULT WINAPI RoRegisterActivationFactories(HSTRING *classes, PFNGETACTIVATIONFACTORY *callbacks,
                                             UINT32 count, RO_REGISTRATION_COOKIE *cookie)
{
    FIXME("(%p, %p, %d, %p): stub\n", classes, callbacks, count, cookie);

    return S_OK;
}

/* Restricted error info: Windows stores one IRestrictedErrorInfo per thread
 * (per RoOriginateError / SetRestrictedErrorInfo) and hands it back through
 * GetRestrictedErrorInfo. Callers rely on the S_OK/S_FALSE split: E_NOTIMPL
 * (the old stub behaviour) made CsWinRT turn every failure round-trip into an
 * extra unimplemented-exception path (run65: ApplicationData.LocalFolder
 * E_NOTIMPL was followed by GetRestrictedErrorInfo E_NOTIMPL and surfaced as
 * NotImplementedException -> RoFailFastWithErrorContextInternal2 -> c0000602).
 * A process-global single slot is enough for our current consumers; a real
 * per-thread slot can be added when cross-thread flows show up. */

struct restricted_error_info
{
    IRestrictedErrorInfo IRestrictedErrorInfo_iface;
    LONG ref;
    BSTR description;
    BSTR restricted_description;
    HRESULT error;
};

static void restricted_error_info_store( IRestrictedErrorInfo *new_iface )
{
    struct tlsdata *tlsdata;
    IRestrictedErrorInfo *old;

    if (FAILED(com_get_tlsdata(&tlsdata))) return;

    old = tlsdata->restricted_error_info;
    if (new_iface) IRestrictedErrorInfo_AddRef( new_iface );
    tlsdata->restricted_error_info = new_iface;
    if (old) IRestrictedErrorInfo_Release( old );
}

static struct restricted_error_info *impl_from_IRestrictedErrorInfo( IRestrictedErrorInfo *iface )
{
    return CONTAINING_RECORD( iface, struct restricted_error_info, IRestrictedErrorInfo_iface );
}

static HRESULT WINAPI restricted_error_info_QueryInterface( IRestrictedErrorInfo *iface, REFIID iid, void **out )
{
    struct restricted_error_info *impl = impl_from_IRestrictedErrorInfo( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) || IsEqualGUID( iid, &IID_IRestrictedErrorInfo ))
    {
        IRestrictedErrorInfo_AddRef( (*out = &impl->IRestrictedErrorInfo_iface) );
        return S_OK;
    }

    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI restricted_error_info_AddRef( IRestrictedErrorInfo *iface )
{
    struct restricted_error_info *impl = impl_from_IRestrictedErrorInfo( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI restricted_error_info_Release( IRestrictedErrorInfo *iface )
{
    struct restricted_error_info *impl = impl_from_IRestrictedErrorInfo( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref)
    {
        SysFreeString( impl->description );
        SysFreeString( impl->restricted_description );
        free( impl );
    }
    return ref;
}

static HRESULT WINAPI restricted_error_info_GetErrorDetails( IRestrictedErrorInfo *iface, BSTR *desc,
        HRESULT *error, BSTR *restricted_desc, BSTR *sid )
{
    struct restricted_error_info *impl = impl_from_IRestrictedErrorInfo( iface );

    TRACE( "iface %p, desc %p, error %p, restricted_desc %p, sid %p.\n",
           iface, desc, error, restricted_desc, sid );

    if (desc)
    {
        *desc = NULL;
        if (impl->description && !(*desc = SysAllocStringLen( impl->description,
                SysStringLen( impl->description ) )))
            return E_OUTOFMEMORY;
    }
    if (error) *error = impl->error;
    if (restricted_desc)
    {
        *restricted_desc = NULL;
        if (impl->restricted_description && !(*restricted_desc = SysAllocStringLen(
                impl->restricted_description, SysStringLen( impl->restricted_description ))))
            return E_OUTOFMEMORY;
    }
    if (sid) *sid = NULL;

    return S_OK;
}

static HRESULT WINAPI restricted_error_info_GetReference( IRestrictedErrorInfo *iface, BSTR *reference )
{
    TRACE( "iface %p, reference %p.\n", iface, reference );

    if (!reference) return E_POINTER;
    *reference = NULL;
    return S_OK;
}

static const struct IRestrictedErrorInfoVtbl restricted_error_info_vtbl =
{
    restricted_error_info_QueryInterface,
    restricted_error_info_AddRef,
    restricted_error_info_Release,
    restricted_error_info_GetErrorDetails,
    restricted_error_info_GetReference,
};

/* Windows semantics: PrimaryLanguageOverride empty string etc. — restricted
 * error descriptions mirror the originating message. */
static void restricted_error_info_create( HRESULT error, const WCHAR *str, ULONG str_len )
{
    struct restricted_error_info *impl;

    if (!(impl = calloc( 1, sizeof(*impl) ))) return;
    impl->IRestrictedErrorInfo_iface.lpVtbl = &restricted_error_info_vtbl;
    impl->ref = 1;
    impl->error = error;
    if (str_len && str && !(impl->description = SysAllocStringLen( str, str_len )))
    {
        free( impl );
        return;
    }

    restricted_error_info_store( &impl->IRestrictedErrorInfo_iface );
    IRestrictedErrorInfo_Release( &impl->IRestrictedErrorInfo_iface );
}

/***********************************************************************
 *      GetRestrictedErrorInfo (combase.@)
 */
HRESULT WINAPI GetRestrictedErrorInfo( IRestrictedErrorInfo **info )
{
    struct tlsdata *tlsdata;
    IRestrictedErrorInfo *current;
    HRESULT hr;

    TRACE( "(%p)\n", info );

    if (!info) return E_POINTER;
    *info = NULL;

    if (FAILED(hr = com_get_tlsdata(&tlsdata))) return hr;

    current = tlsdata->restricted_error_info;
    tlsdata->restricted_error_info = NULL;

    if (!current) return S_FALSE;
    *info = current;
    return S_OK;
}

/***********************************************************************
 *      SetRestrictedErrorInfo (combase.@)
 */
HRESULT WINAPI SetRestrictedErrorInfo( IRestrictedErrorInfo *info )
{
    TRACE( "(%p)\n", info );

    if (info)
    {
        /* Windows also QIs the object to validate it; our callers hand us
         * the object they just created (CsWinRT path in run65). */
        restricted_error_info_store( info );
    }
    else
    {
        restricted_error_info_store( NULL );
    }

    return S_OK;
}

/* RoOriginate* only records the slot; the full context chain (stowed exception
 * object) is out of scope for now and documented as a limitation. */
static BOOL ro_originate_error_common( HRESULT error, const WCHAR *message, UINT32 message_len )
{
    FIXME( "error %#lx, message %s, caller %p.\n",
           error, message ? debugstr_wn( message, message_len ) : "(null)", __builtin_return_address(0) );
    restricted_error_info_create( error, message, message_len );
    return TRUE;
}

/***********************************************************************
 *      RoOriginateError (combase.@)
 */
BOOL WINAPI RoOriginateError( HRESULT error, HSTRING message )
{
    const WCHAR *buffer = WindowsGetStringRawBuffer( message, NULL );

    return ro_originate_error_common( error, buffer, message ? WindowsGetStringLen( message ) : 0 );
}

/***********************************************************************
 *      RoOriginateErrorW (combase.@)
 */
BOOL WINAPI RoOriginateErrorW( HRESULT error, UINT max_len, const WCHAR *message )
{
    return ro_originate_error_common( error, message, max_len );
}

/***********************************************************************
 *      RoTransformError (combase.@)
 */
BOOL WINAPI RoTransformError( HRESULT old_error, HRESULT new_error, HSTRING message )
{
    const WCHAR *buffer = WindowsGetStringRawBuffer( message, NULL );
    TRACE( "old_error %#lx, new_error %#lx, message %s.\n", old_error, new_error, debugstr_hstring( message ) );
    return ro_originate_error_common( new_error, buffer, message ? WindowsGetStringLen( message ) : 0 );
}

/***********************************************************************
 *      RoTransformErrorW (combase.@)
 */
BOOL WINAPI RoTransformErrorW( HRESULT old_error, HRESULT new_error, UINT max_len, const WCHAR *message )
{
    TRACE( "old_error %#lx, new_error %#lx, message %s.\n", old_error, new_error, message ? debugstr_wn( message, max_len ) : "(null)" );
    return ro_originate_error_common( new_error, message, max_len );
}

/***********************************************************************
 *      RoOriginateLanguageException (combase.@)
 */
BOOL WINAPI RoOriginateLanguageException( HRESULT error, HSTRING message, IUnknown *language_exception )
{
    const WCHAR *buffer = WindowsGetStringRawBuffer( message, NULL );
    void *frames[16];
    USHORT count = RtlCaptureStackBackTrace( 0, 16, frames, NULL );
    USHORT i;

    WARN( "error %#lx, message %s, language_exception %p, caller %p (not stowed).\n",
          error, message ? debugstr_wn( buffer, WindowsGetStringLen( message ) ) : "(null)",
          language_exception, __builtin_return_address(0) );
    for (i = 0; i < count; i++)
        WARN( "  frame[%02u]: %p\n", i, frames[i] );
    restricted_error_info_create( error, buffer, message ? WindowsGetStringLen( message ) : 0 );
    return TRUE;
}

/***********************************************************************
 *      RoFailFastWithErrorContextInternal2 (combase.@)
 */
void WINAPI RoFailFastWithErrorContextInternal2( HRESULT hrError, ULONG cbStowedException, void *pvStowedException )
{
    IRestrictedErrorInfo *info = NULL;
    HRESULT error = hrError;
    BSTR desc = NULL;
    void *frames[32];
    USHORT frame_count, i;

    /* Dump the failing context before terminating so run logs answer
     * "which stub returned this error" without a fresh +relay autopsy. */
    ERR( "fail-fast: hrError %#08lx, cbStowedException %lu, pvStowedException %p.\n", hrError, cbStowedException, pvStowedException );

    if (SUCCEEDED( GetRestrictedErrorInfo( &info ) ) && info)
    {
        if (SUCCEEDED( IRestrictedErrorInfo_GetErrorDetails( info, &desc, &error, NULL, NULL ) ) && desc)
            ERR( "fail-fast: restricted error hr %#08lx, details: %s.\n", error, debugstr_w( desc ) );
        else
            ERR( "fail-fast: restricted error info %p unreadable or empty (GetErrorDetails hr %#08lx).\n", info, error );
        if (desc) SysFreeString( desc );
        IRestrictedErrorInfo_Release( info );
    }
    else
    {
        ERR( "fail-fast: no restricted error info recorded (GetRestrictedErrorInfo hr %#08lx).\n", hrError );
    }

    if (pvStowedException && cbStowedException)
    {
        ERR( "fail-fast: stowed exception context present at %p (%#lx bytes), first bytes %#08x.\n",
             pvStowedException, (ULONG)cbStowedException, *(UINT32 *)pvStowedException );
    }

    frame_count = RtlCaptureStackBackTrace( 0, ARRAY_SIZE( frames ), frames, NULL );
    for (i = 0; i < frame_count; ++i)
        ERR( "fail-fast: stack[%02u] %p.\n", i, frames[i] );

    RaiseFailFastException( NULL, NULL, 0 );
}


/***********************************************************************
 *      RoReportUnhandledError (combase.@)
 */
HRESULT WINAPI RoReportUnhandledError(IRestrictedErrorInfo *info)
{
    BSTR desc = NULL, restricted_desc = NULL, sid = NULL;
    HRESULT error = 0;
    BOOL release_info = FALSE;

    ERR("(%p): unhandled error reported!\n", info);
    if (!info)
    {
        if (SUCCEEDED(GetRestrictedErrorInfo(&info)) && info)
            release_info = TRUE;
    }

    if (info && SUCCEEDED(IRestrictedErrorInfo_GetErrorDetails(info, &desc, &error, &restricted_desc, &sid)))
    {
        ERR("  hr %#08lx, details: %s, restricted: %s, sid: %s\n",
            error, debugstr_w(desc), debugstr_w(restricted_desc), debugstr_w(sid));
        if (desc) SysFreeString(desc);
        if (restricted_desc) SysFreeString(restricted_desc);
        if (sid) SysFreeString(sid);
    }
    if (release_info && info)
        IRestrictedErrorInfo_Release(info);

    {
        void *frames[32];
        USHORT frame_count, i;
        frame_count = RtlCaptureStackBackTrace( 0, ARRAY_SIZE( frames ), frames, NULL );
        for (i = 0; i < frame_count; ++i)
            ERR("  unhandled-error: stack[%02u] %p.\n", i, frames[i]);
    }

    return S_OK;
}

/***********************************************************************
 *      RoSetErrorReportingFlags (combase.@)
 */
HRESULT WINAPI RoSetErrorReportingFlags(UINT32 flags)
{
    FIXME("(%08x): stub\n", flags);
    return S_OK;
}

/***********************************************************************
 *      RoGetErrorReportingFlags (combase.@)
 */
HRESULT WINAPI RoGetErrorReportingFlags(UINT32 *flags)
{
    FIXME("(%p): stub\n", flags);

    if (!flags)
        return E_POINTER;

    *flags = RO_ERROR_REPORTING_USESETERRORINFO;
    return S_OK;
}


/***********************************************************************
 *      CleanupTlsOleState (combase.@)
 */
void WINAPI CleanupTlsOleState(void *unknown)
{
    FIXME("(%p): stub\n", unknown);
}

/***********************************************************************
 *      DllGetActivationFactory (combase.@)
 */
HRESULT WINAPI DllGetActivationFactory(HSTRING classid, IActivationFactory **factory)
{
    FIXME("(%s, %p): stub\n", debugstr_hstring(classid), factory);

    return REGDB_E_CLASSNOTREG;
}
