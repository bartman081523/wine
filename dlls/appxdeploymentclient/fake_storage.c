/* Fake IStorageFolder + IStorageItem for FakePackage
 * Based on dlls/windows.applicationmodel/package.c
 * Modified to return current directory for bootstrapper
 */

#include "private.h"
#define WIDL_using_Windows_Storage
#include "wine/debug.h"
#include <windows.storage.h>

WINE_DEFAULT_DEBUG_CHANNEL(appx);

struct storage_folder {
  IStorageFolder IStorageFolder_iface;
  IStorageItem IStorageItem_iface;
  LONG ref;
};

static inline struct storage_folder *
impl_from_IStorageFolder(IStorageFolder *iface) {
  return CONTAINING_RECORD(iface, struct storage_folder, IStorageFolder_iface);
}

// IStorageFolder implementation
static HRESULT WINAPI storage_folder_QueryInterface(IStorageFolder *iface,
                                                    REFIID iid, void **out) {
  struct storage_folder *impl = impl_from_IStorageFolder(iface);

  TRACE("iface %p, iid %s, out %p.\n", iface, debugstr_guid(iid), out);

  if (IsEqualGUID(iid, &IID_IUnknown) || IsEqualGUID(iid, &IID_IInspectable) ||
      IsEqualGUID(iid, &IID_IStorageFolder)) {
    *out = &impl->IStorageFolder_iface;
    IInspectable_AddRef(*out);
    return S_OK;
  }

  if (IsEqualGUID(iid, &IID_IStorageItem)) {
    *out = &impl->IStorageItem_iface;
    IInspectable_AddRef(*out);
    return S_OK;
  }

  FIXME("%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid(iid));
  *out = NULL;
  return E_NOINTERFACE;
}

static ULONG WINAPI storage_folder_AddRef(IStorageFolder *iface) {
  struct storage_folder *impl = impl_from_IStorageFolder(iface);
  ULONG ref = InterlockedIncrement(&impl->ref);
  TRACE("iface %p increasing refcount to %lu.\n", iface, ref);
  return ref;
}

static ULONG WINAPI storage_folder_Release(IStorageFolder *iface) {
  struct storage_folder *impl = impl_from_IStorageFolder(iface);
  ULONG ref = InterlockedDecrement(&impl->ref);

  TRACE("iface %p decreasing refcount to %lu.\n", iface, ref);

  if (!ref)
    free(impl);
  return ref;
}

static HRESULT WINAPI storage_folder_GetIids(IStorageFolder *iface,
                                             ULONG *iid_count, IID **iids) {
  FIXME("iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids);
  return E_NOTIMPL;
}

static HRESULT WINAPI storage_folder_GetRuntimeClassName(IStorageFolder *iface,
                                                         HSTRING *class_name) {
  FIXME("iface %p, class_name %p stub!\n", iface, class_name);
  return E_NOTIMPL;
}

static HRESULT WINAPI storage_folder_GetTrustLevel(IStorageFolder *iface,
                                                   TrustLevel *trust_level) {
  FIXME("iface %p, trust_level %p stub!\n", iface, trust_level);
  return E_NOTIMPL;
}

static HRESULT WINAPI storage_folder_CreateFileAsyncOverloadDefaultOptions(
    IStorageFolder *iface, HSTRING name,
    IAsyncOperation_StorageFile **operation) {
  FIXME("iface %p, name %s, operation %p stub!\n", iface,
        debugstr_hstring(name), operation);
  return E_NOTIMPL;
}

static HRESULT WINAPI storage_folder_CreateFileAsync(
    IStorageFolder *iface, HSTRING name, CreationCollisionOption options,
    IAsyncOperation_StorageFile **operation) {
  FIXME("iface %p, name %s, options %d, operation %p stub!\n", iface,
        debugstr_hstring(name), options, operation);
  return E_NOTIMPL;
}

static HRESULT WINAPI storage_folder_CreateFolderAsyncOverloadDefaultOptions(
    IStorageFolder *iface, HSTRING name,
    IAsyncOperation_StorageFolder **operation) {
  FIXME("iface %p, name %s, operation %p stub!\n", iface,
        debugstr_hstring(name), operation);
  return E_NOTIMPL;
}

static HRESULT WINAPI storage_folder_CreateFolderAsync(
    IStorageFolder *iface, HSTRING name, CreationCollisionOption options,
    IAsyncOperation_StorageFolder **operation) {
  FIXME("iface %p, name %s, options %d, operation %p stub!\n", iface,
        debugstr_hstring(name), options, operation);
  return E_NOTIMPL;
}

static HRESULT WINAPI
storage_folder_GetFileAsync(IStorageFolder *iface, HSTRING name,
                            IAsyncOperation_StorageFile **operation) {
  FIXME("iface %p, name %s, operation %p stub!\n", iface,
        debugstr_hstring(name), operation);
  return E_NOTIMPL;
}

static HRESULT WINAPI
storage_folder_GetFolderAsync(IStorageFolder *iface, HSTRING name,
                              IAsyncOperation_StorageFolder **operation) {
  FIXME("iface %p, name %s, operation %p stub!\n", iface,
        debugstr_hstring(name), operation);
  return E_NOTIMPL;
}

static HRESULT WINAPI
storage_folder_GetItemAsync(IStorageFolder *iface, HSTRING name,
                            IAsyncOperation_IStorageItem **operation) {
  FIXME("iface %p, name %s, operation %p stub!\n", iface,
        debugstr_hstring(name), operation);
  return E_NOTIMPL;
}

static HRESULT WINAPI
storage_folder_GetFilesAsyncOverloadDefaultOptionsStartAndCount(
    IStorageFolder *iface,
    IAsyncOperation_IVectorView_StorageFile **operation) {
  FIXME("iface %p, operation %p stub!\n", iface, operation);
  return E_NOTIMPL;
}

static HRESULT WINAPI
storage_folder_GetFoldersAsyncOverloadDefaultOptionsStartAndCount(
    IStorageFolder *iface,
    IAsyncOperation_IVectorView_StorageFolder **operation) {
  FIXME("iface %p, operation %p stub!\n", iface, operation);
  return E_NOTIMPL;
}

static HRESULT WINAPI storage_folder_GetItemsAsyncOverloadDefaultStartAndCount(
    IStorageFolder *iface,
    IAsyncOperation_IVectorView_IStorageItem **operation) {
  FIXME("iface %p, operation %p stub!\n", iface, operation);
  return E_NOTIMPL;
}

static const struct IStorageFolderVtbl storage_folder_vtbl = {
    storage_folder_QueryInterface,
    storage_folder_AddRef,
    storage_folder_Release,
    /* IInspectable methods */
    storage_folder_GetIids,
    storage_folder_GetRuntimeClassName,
    storage_folder_GetTrustLevel,
    /* IStorageFolder methods */
    storage_folder_CreateFileAsyncOverloadDefaultOptions,
    storage_folder_CreateFileAsync,
    storage_folder_CreateFolderAsyncOverloadDefaultOptions,
    storage_folder_CreateFolderAsync,
    storage_folder_GetFileAsync,
    storage_folder_GetFolderAsync,
    storage_folder_GetItemAsync,
    storage_folder_GetFilesAsyncOverloadDefaultOptionsStartAndCount,
    storage_folder_GetFoldersAsyncOverloadDefaultOptionsStartAndCount,
    storage_folder_GetItemsAsyncOverloadDefaultStartAndCount,
};

// IStorageItem implementation
DEFINE_IINSPECTABLE(storage_item, IStorageItem, struct storage_folder,
                    IStorageFolder_iface)

static HRESULT WINAPI storage_item_RenameAsyncOverloadDefaultOptions(
    IStorageItem *iface, HSTRING name, IAsyncAction **operation) {
  FIXME("iface %p, name %s, operation %p stub!\n", iface,
        debugstr_hstring(name), operation);
  return E_NOTIMPL;
}

static HRESULT WINAPI storage_item_RenameAsync(IStorageItem *iface,
                                               HSTRING name,
                                               NameCollisionOption option,
                                               IAsyncAction **operation) {
  FIXME("iface %p, name %s, option %d, operation %p stub!\n", iface,
        debugstr_hstring(name), option, operation);
  return E_NOTIMPL;
}

static HRESULT WINAPI storage_item_DeleteAsyncOverloadDefaultOptions(
    IStorageItem *iface, IAsyncAction **operation) {
  FIXME("iface %p, operation %p stub!\n", iface, operation);
  return E_NOTIMPL;
}

static HRESULT WINAPI storage_item_DeleteAsync(IStorageItem *iface,
                                               StorageDeleteOption option,
                                               IAsyncAction **operation) {
  FIXME("iface %p, option %d, operation %p stub!\n", iface, option, operation);
  return E_NOTIMPL;
}

static HRESULT WINAPI storage_item_GetBasicPropertiesAsync(
    IStorageItem *iface, IAsyncOperation_BasicProperties **operation) {
  FIXME("iface %p, operation %p stub!\n", iface, operation);
  return E_NOTIMPL;
}

static HRESULT WINAPI storage_item_get_Name(IStorageItem *iface,
                                            HSTRING *value) {
  FIXME("iface %p, value %p stub!\n", iface, value);
  return E_NOTIMPL;
}

// *** CRITICAL MODIFICATION: Return current directory instead of module path
// ***
static HRESULT WINAPI storage_item_get_Path(IStorageItem *iface,
                                            HSTRING *value) {
  WCHAR path[MAX_PATH];

  TRACE("iface %p, value %p\n", iface, value);

  // MODIFIED: Return current working directory where DLLs are located
  // Original implementation used GetModuleFileNameW which won't work for fake
  // package
  GetCurrentDirectoryW(MAX_PATH, path);

  TRACE("FakeStorage returning CWD: %s\n", debugstr_w(path));
  return WindowsCreateString(path, wcslen(path), value);
}

static HRESULT WINAPI storage_item_get_Attributes(IStorageItem *iface,
                                                  FileAttributes *value) {
  FIXME("iface %p, value %p stub!\n", iface, value);
  return E_NOTIMPL;
}

static HRESULT WINAPI storage_item_get_DateCreated(IStorageItem *iface,
                                                   DateTime *value) {
  FIXME("iface %p, value %p stub!\n", iface, value);
  return E_NOTIMPL;
}

static HRESULT WINAPI storage_item_IsOfType(IStorageItem *iface,
                                            StorageItemTypes type,
                                            boolean *value) {
  FIXME("iface %p, type %d, value %p stub!\n", iface, type, value);
  return E_NOTIMPL;
}

static const struct IStorageItemVtbl storage_item_vtbl = {
    storage_item_QueryInterface,
    storage_item_AddRef,
    storage_item_Release,
    /* IInspectable methods */
    storage_item_GetIids,
    storage_item_GetRuntimeClassName,
    storage_item_GetTrustLevel,
    /* IStorageItem methods */
    storage_item_RenameAsyncOverloadDefaultOptions,
    storage_item_RenameAsync,
    storage_item_DeleteAsyncOverloadDefaultOptions,
    storage_item_DeleteAsync,
    storage_item_GetBasicPropertiesAsync,
    storage_item_get_Name,
    storage_item_get_Path, // *** THE CRITICAL METHOD ***
    storage_item_get_Attributes,
    storage_item_get_DateCreated,
    storage_item_IsOfType,
};

// Export function for use in package.c
HRESULT create_fake_storage_folder(IStorageFolder **value) {
  struct storage_folder *impl;

  TRACE("Creating fake storage folder\n");

  if (!value)
    return E_INVALIDARG;
  if (!(impl = calloc(1, sizeof(*impl))))
    return E_OUTOFMEMORY;

  impl->IStorageFolder_iface.lpVtbl = &storage_folder_vtbl;
  impl->IStorageItem_iface.lpVtbl = &storage_item_vtbl;
  impl->ref = 1;

  *value = &impl->IStorageFolder_iface;
  TRACE("created IStorageFolder %p.\n", *value);
  return S_OK;
}
