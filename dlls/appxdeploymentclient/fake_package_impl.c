/* Package and iteration objects for the PackageManager WinRT surface,
 * projected from the seeded package table (HKCU\Software\Wine\AppModel,
 * served by kernelbase's appmodel_registry.c) through the
 * WineAppModelGetEntry* exports. Every object is a thin projection over
 * one WINE_APPMODEL_ENTRY; on an unseeded prefix the collections are
 * simply empty. */

#define COBJMACROS
#include "objbase.h"
#include "winbase.h"
#include "windef.h"
#include "winstring.h"
#include <stdarg.h>

#define WIDL_using_Windows_Foundation
#define WIDL_using_Windows_Foundation_Collections
#define WIDL_using_Windows_ApplicationModel
#define WIDL_using_Windows_Storage
#include "windows.applicationmodel.h"
#include "windows.foundation.h"
#include "windows.storage.h"
#include "appmodel.h"

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(appx);

typedef struct FakePackage FakePackage;

/* The whole projected package set, built once from the table. Objects
 * live for the process duration, so the interface stubs keep the static
 * refcount treatment. */
static FakePackage *table_packages;      /* one per entry, table order */
static FakePackage **all_packages;       /* iterable input: every entry */
static FakePackage **framework_packages; /* iterable input: IsFramework */
static UINT32 table_count;
static UINT32 framework_count;
static SRWLOCK packages_lock = SRWLOCK_INIT;
static BOOL packages_loaded;

typedef struct {
  IVectorView_Package IVectorView_Package_iface;
  IIterable_Package IIterable_Package_iface;
  LONG ref;
} FakeDependencyVector;

/* shared by every non-framework package's Dependencies view */
static FakeDependencyVector global_dependency_vector;

// 1. FakeStorageFolder Stub
typedef struct {
  IStorageFolder IStorageFolder_iface;
  IStorageItem IStorageItem_iface;
  LONG ref;
  const WCHAR *path;
} FakeStorageFolder;

static inline FakeStorageFolder *
impl_from_IStorageFolder(IStorageFolder *iface) {
  return CONTAINING_RECORD(iface, FakeStorageFolder, IStorageFolder_iface);
}
static inline FakeStorageFolder *impl_from_IStorageItem(IStorageItem *iface) {
  return CONTAINING_RECORD(iface, FakeStorageFolder, IStorageItem_iface);
}

static HRESULT WINAPI Storage_QueryInterface(IStorageFolder *iface, REFIID riid,
                                             void **ppv) {
  FakeStorageFolder *This = impl_from_IStorageFolder(iface);
  if (IsEqualGUID(riid, &IID_IUnknown) ||
      IsEqualGUID(riid, &IID_IInspectable) ||
      IsEqualGUID(riid, &IID_IStorageFolder)) {
    *ppv = &This->IStorageFolder_iface;
  } else if (IsEqualGUID(riid, &IID_IStorageItem)) {
    *ppv = &This->IStorageItem_iface;
  } else {
    *ppv = NULL;
    return E_NOINTERFACE;
  }
  IUnknown_AddRef((IUnknown *)*ppv);
  return S_OK;
}
static ULONG WINAPI Storage_AddRef(IStorageFolder *iface) { return 2; }
static ULONG WINAPI Storage_Release(IStorageFolder *iface) { return 1; }

static HRESULT WINAPI Item_QueryInterface(IStorageItem *iface, REFIID riid,
                                          void **ppv) {
  FakeStorageFolder *This = impl_from_IStorageItem(iface);
  return Storage_QueryInterface(&This->IStorageFolder_iface, riid, ppv);
}
static ULONG WINAPI Item_AddRef(IStorageItem *iface) { return 2; }
static ULONG WINAPI Item_Release(IStorageItem *iface) { return 1; }
static HRESULT WINAPI Item_get_Path(IStorageItem *iface, HSTRING *value) {
  FakeStorageFolder *This = impl_from_IStorageItem(iface);
  const WCHAR *path = This->path;
  WCHAR cwd[MAX_PATH];
  if (!path) {
    /* no seeded install location serves the working directory instead */
    GetCurrentDirectoryW(MAX_PATH, cwd);
    path = cwd;
  }
  TRACE("InstalledLocation path %s\n", debugstr_w(path));
  return WindowsCreateString(path, wcslen(path), value);
}
static HRESULT WINAPI Item_Stub(IStorageItem *iface) { return E_NOTIMPL; }
static const IStorageItemVtbl FakeStorageItem_Vtbl = {
    Item_QueryInterface, Item_AddRef,       Item_Release,
    (void *)Item_Stub,   (void *)Item_Stub, (void *)Item_Stub,
    (void *)Item_Stub,   (void *)Item_Stub, (void *)Item_Stub,
    (void *)Item_Stub,   (void *)Item_Stub, (void *)Item_Stub,
    Item_get_Path,       (void *)Item_Stub, (void *)Item_Stub,
    (void *)Item_Stub};
static HRESULT WINAPI Folder_Stub(IStorageFolder *iface) { return E_NOTIMPL; }
static const IStorageFolderVtbl FakeStorageFolder_Vtbl = {
    Storage_QueryInterface, Storage_AddRef,      Storage_Release,
    (void *)Folder_Stub,    (void *)Folder_Stub, (void *)Folder_Stub,
    (void *)Folder_Stub,    (void *)Folder_Stub, (void *)Folder_Stub,
    (void *)Folder_Stub,    (void *)Folder_Stub, (void *)Folder_Stub,
    (void *)Folder_Stub,    (void *)Folder_Stub, (void *)Folder_Stub,
    (void *)Folder_Stub};

// 2. FakePackage & FakePackageId
typedef struct {
  IPackageId IPackageId_iface;
  const WCHAR *name;
  const WCHAR *family_name;
  const WCHAR *full_name;
  const WCHAR *publisher;
  const WCHAR *publisher_id;
  const WCHAR *resource_id;
  PackageVersion version;
  UINT32 architecture;
} FakePackageIdObj;

struct FakePackage {
  IPackage IPackage_iface;
  FakePackageIdObj id;
  FakeStorageFolder folder;
  boolean is_framework;
  LONG ref;
};

static inline FakePackageIdObj *impl_from_IPackageId(IPackageId *iface) {
  return CONTAINING_RECORD(iface, FakePackageIdObj, IPackageId_iface);
}
static inline FakePackage *impl_from_IPackage(IPackage *iface) {
  return CONTAINING_RECORD(iface, FakePackage, IPackage_iface);
}
static HRESULT WINAPI Id_QueryInterface(IPackageId *iface, REFIID riid,
                                        void **ppv) {
  TRACE("Id_QueryInterface %s\n", debugstr_guid(riid));
  if (IsEqualGUID(riid, &IID_IUnknown) || IsEqualGUID(riid, &IID_IPackageId) ||
      IsEqualGUID(riid, &IID_IInspectable)) {
    *ppv = iface;
    IUnknown_AddRef((IUnknown *)*ppv);
    return S_OK;
  }
  return E_NOINTERFACE;
}
static ULONG WINAPI Id_AddRef(IPackageId *iface) { return 2; }
static ULONG WINAPI Id_Release(IPackageId *iface) { return 1; }

static HRESULT WINAPI Id_get_Name(IPackageId *iface, HSTRING *v) {
  FakePackageIdObj *this = impl_from_IPackageId(iface);
  HRESULT hr = WindowsCreateString(this->name, wcslen(this->name), v);
  TRACE("Id_get_Name: %s, hr=%x, v=%p\n", debugstr_w(this->name), hr, *v);
  return hr;
}
static HRESULT WINAPI Id_get_Version(IPackageId *iface, PackageVersion *v) {
  FakePackageIdObj *this = impl_from_IPackageId(iface);
  TRACE("Id_get_Version\n");
  *v = this->version;
  return S_OK;
}
static HRESULT WINAPI
Id_get_Architecture(IPackageId *iface,
                    enum __x_ABI_CWindows_CSystem_CProcessorArchitecture *v) {
  FakePackageIdObj *this = impl_from_IPackageId(iface);
  TRACE("Id_get_Architecture: %u\n", this->architecture);
  *v = (enum __x_ABI_CWindows_CSystem_CProcessorArchitecture)this->architecture;
  return S_OK;
}
static HRESULT WINAPI Id_get_PublisherId(IPackageId *iface, HSTRING *v) {
  FakePackageIdObj *this = impl_from_IPackageId(iface);
  TRACE("Id_get_PublisherId\n");
  return WindowsCreateString(this->publisher_id,
                             wcslen(this->publisher_id), v);
}
static HRESULT WINAPI Id_get_FullName(IPackageId *iface, HSTRING *v) {
  FakePackageIdObj *this = impl_from_IPackageId(iface);
  TRACE("Id_get_FullName: %s\n", debugstr_w(this->full_name));
  return WindowsCreateString(this->full_name, wcslen(this->full_name), v);
}
static HRESULT WINAPI Id_get_FamilyName(IPackageId *iface, HSTRING *v) {
  FakePackageIdObj *this = impl_from_IPackageId(iface);
  TRACE("Id_get_FamilyName: %s\n", debugstr_w(this->family_name));
  return WindowsCreateString(this->family_name, wcslen(this->family_name), v);
}
static HRESULT WINAPI Id_get_ResourceId(IPackageId *iface, HSTRING *v) {
  FakePackageIdObj *this = impl_from_IPackageId(iface);
  return WindowsCreateString(this->resource_id, wcslen(this->resource_id), v);
}
static HRESULT WINAPI Id_get_Publisher(IPackageId *iface, HSTRING *v) {
  FakePackageIdObj *this = impl_from_IPackageId(iface);
  TRACE("Id_get_Publisher: %s\n", debugstr_w(this->publisher));
  return WindowsCreateString(this->publisher, wcslen(this->publisher), v);
}
static HRESULT WINAPI Id_Stub(IPackageId *iface) { return E_NOTIMPL; }
static const IPackageIdVtbl FakePackageId_Vtbl = {
    Id_QueryInterface, Id_AddRef,        Id_Release,
    (void *)Id_Stub,   (void *)Id_Stub,  (void *)Id_Stub,
    Id_get_Name,       Id_get_Version,   Id_get_Architecture,
    Id_get_ResourceId, Id_get_Publisher, Id_get_PublisherId,
    Id_get_FullName,   Id_get_FamilyName};

static HRESULT WINAPI Pkg_QueryInterface(IPackage *iface, REFIID riid,
                                         void **ppv) {
  TRACE("Pkg_QueryInterface %s\n", debugstr_guid(riid));
  if (IsEqualGUID(riid, &IID_IUnknown) || IsEqualGUID(riid, &IID_IPackage) ||
      IsEqualGUID(riid, &IID_IInspectable)) {
    *ppv = iface;
    IUnknown_AddRef((IUnknown *)*ppv);
    return S_OK;
  }
  return E_NOINTERFACE;
}
static ULONG WINAPI Pkg_AddRef(IPackage *iface) { return 2; }
static ULONG WINAPI Pkg_Release(IPackage *iface) { return 1; }

static HRESULT WINAPI Pkg_get_Id(IPackage *iface, IPackageId **v) {
  FakePackage *this = impl_from_IPackage(iface);
  TRACE("Pkg_get_Id for %p\n", this);
  *v = &this->id.IPackageId_iface;
  IPackageId_AddRef(*v);
  return S_OK;
}
static HRESULT WINAPI Pkg_get_InstalledLocation(IPackage *iface,
                                                IStorageFolder **v) {
  FakePackage *this = impl_from_IPackage(iface);
  TRACE("Pkg_get_InstalledLocation\n");
  *v = &this->folder.IStorageFolder_iface;
  IStorageFolder_AddRef(*v);
  return S_OK;
}
static HRESULT WINAPI Pkg_get_IsFramework(IPackage *iface, boolean *v) {
  FakePackage *this = impl_from_IPackage(iface);
  TRACE("Pkg_get_IsFramework: %d\n", this->is_framework);
  *v = this->is_framework;
  return S_OK;
}
static HRESULT WINAPI Pkg_get_Dependencies(IPackage *iface,
                                           IVectorView_Package **v) {
  FakePackage *this = impl_from_IPackage(iface);
  TRACE("Pkg_get_Dependencies for %p, is_framework=%d\n", this,
        this->is_framework);
  if (!this->is_framework) {
    *v = &global_dependency_vector.IVectorView_Package_iface;
    IVectorView_Package_AddRef(*v);
    return S_OK;
  }
  return E_NOTIMPL;
}

static HRESULT WINAPI Pkg_Stub(IPackage *iface) { return E_NOTIMPL; }

static const IPackageVtbl FakePackage_Vtbl = {
    Pkg_QueryInterface,  Pkg_AddRef,
    Pkg_Release,         (void *)Pkg_Stub,
    (void *)Pkg_Stub,    (void *)Pkg_Stub,
    Pkg_get_Id,          Pkg_get_InstalledLocation,
    Pkg_get_IsFramework, Pkg_get_Dependencies};

/* A package object projects one table entry; the strings are owned by
 * the kernelbase reader and outlive every consumer here. */
static void package_from_entry(const WINE_APPMODEL_ENTRY *entry,
                               FakePackage *package) {
  package->IPackage_iface.lpVtbl = &FakePackage_Vtbl;
  package->id.IPackageId_iface.lpVtbl = &FakePackageId_Vtbl;
  package->id.name = entry->name ? entry->name : L"";
  package->id.family_name = entry->family_name ? entry->family_name : L"";
  package->id.full_name = entry->full_name ? entry->full_name : L"";
  package->id.publisher = entry->publisher ? entry->publisher : L"";
  package->id.publisher_id = entry->publisher_id ? entry->publisher_id : L"";
  package->id.resource_id = entry->resource_id ? entry->resource_id : L"";
  package->id.version.Major = entry->package_version.Major;
  package->id.version.Minor = entry->package_version.Minor;
  package->id.version.Build = entry->package_version.Build;
  package->id.version.Revision = entry->package_version.Revision;
  package->id.architecture = entry->processor_architecture;
  package->folder.IStorageFolder_iface.lpVtbl = &FakeStorageFolder_Vtbl;
  package->folder.IStorageItem_iface.lpVtbl = &FakeStorageItem_Vtbl;
  package->folder.ref = 1;
  package->folder.path = entry->path;
  package->is_framework = entry->is_framework;
  package->ref = 1;
}

static void build_table_packages(void) {
  UINT32 count = WineAppModelGetEntryCount(), i, fw;

  table_count = count;
  if (!count) return;

  table_packages = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                             count * sizeof(*table_packages));
  all_packages = HeapAlloc(GetProcessHeap(), 0, count * sizeof(*all_packages));
  framework_packages =
      HeapAlloc(GetProcessHeap(), 0, count * sizeof(*framework_packages));
  if (!table_packages || !all_packages || !framework_packages) {
    HeapFree(GetProcessHeap(), 0, table_packages);
    HeapFree(GetProcessHeap(), 0, all_packages);
    HeapFree(GetProcessHeap(), 0, framework_packages);
    table_packages = NULL;
    all_packages = NULL;
    framework_packages = NULL;
    table_count = 0;
    return;
  }

  fw = 0;
  for (i = 0; i < count; i++) {
    package_from_entry(WineAppModelGetEntry(i), &table_packages[i]);
    all_packages[i] = &table_packages[i];
    if (table_packages[i].is_framework)
      framework_packages[fw++] = &table_packages[i];
  }
  framework_count = fw;
  TRACE("built %u package objects (%u frameworks) from the seeded table\n",
        table_count, framework_count);
}

static void ensure_packages(void) {
  /* The exclusive lock is only held for the build pass; the heap objects
   * it creates are complete before the readers see them. */
  AcquireSRWLockExclusive(&packages_lock);
  if (!packages_loaded) {
    build_table_packages();
    packages_loaded = TRUE;
  }
  ReleaseSRWLockExclusive(&packages_lock);
}

// 3. FakeVectorView
static inline FakeDependencyVector *
impl_from_IVectorView_Package(IVectorView_Package *iface) {
  return CONTAINING_RECORD(iface, FakeDependencyVector,
                           IVectorView_Package_iface);
}
HRESULT create_dependency_iterator(IIterator_Package **out);

static HRESULT WINAPI Vector_QueryInterface(IVectorView_Package *iface,
                                            REFIID riid, void **ppv) {
  FakeDependencyVector *This = impl_from_IVectorView_Package(iface);
  if (IsEqualGUID(riid, &IID_IUnknown) ||
      IsEqualGUID(riid, &IID_IInspectable) ||
      IsEqualGUID(riid, &IID_IVectorView_Package)) {
    *ppv = &This->IVectorView_Package_iface;
  } else if (IsEqualGUID(riid, &IID_IIterable_Package)) {
    *ppv = &This->IIterable_Package_iface;
  } else {
    *ppv = NULL;
    return E_NOINTERFACE;
  }
  IUnknown_AddRef((IUnknown *)*ppv);
  return S_OK;
}
static ULONG WINAPI Vector_AddRef(IVectorView_Package *iface) { return 2; }
static ULONG WINAPI Vector_Release(IVectorView_Package *iface) { return 1; }
static HRESULT WINAPI Vector_GetIids(IVectorView_Package *iface, ULONG *c,
                                     IID **i) {
  return E_NOTIMPL;
}
static HRESULT WINAPI Vector_GetRuntimeClassName(IVectorView_Package *iface,
                                                 HSTRING *s) {
  return E_NOTIMPL;
}
static HRESULT WINAPI Vector_GetTrustLevel(IVectorView_Package *iface,
                                           TrustLevel *t) {
  return E_NOTIMPL;
}

static HRESULT WINAPI Vector_GetAt(IVectorView_Package *iface, UINT32 index,
                                   IPackage **item) {
  FakePackage *package;
  TRACE("Vector_GetAt index %u\n", index);
  if (index >= framework_count) return E_BOUNDS;
  package = framework_packages[index];
  *item = &package->IPackage_iface;
  IPackage_AddRef(*item);
  return S_OK;
}
static HRESULT WINAPI Vector_get_Size(IVectorView_Package *iface,
                                      UINT32 *size) {
  *size = framework_count;
  TRACE("Vector_get_Size: %u\n", framework_count);
  return S_OK;
}
static HRESULT WINAPI Vector_IndexOf(IVectorView_Package *iface, IPackage *item,
                                     UINT32 *index, boolean *found) {
  *found = FALSE;
  return S_OK;
}
static HRESULT WINAPI Vector_GetMany(IVectorView_Package *iface, UINT32 first,
                                     UINT32 count, IPackage **items,
                                     UINT32 *actual) {
  return E_NOTIMPL;
}
static const IVectorView_PackageVtbl FakeVectorView_Vtbl = {
    Vector_QueryInterface,
    Vector_AddRef,
    Vector_Release,
    Vector_GetIids,
    Vector_GetRuntimeClassName,
    Vector_GetTrustLevel,
    Vector_GetAt,
    Vector_get_Size,
    Vector_IndexOf,
    Vector_GetMany};

static HRESULT WINAPI VectorIterable_QueryInterface(IIterable_Package *iface,
                                                    REFIID riid, void **ppv) {
  FakeDependencyVector *This =
      CONTAINING_RECORD(iface, FakeDependencyVector, IIterable_Package_iface);
  return Vector_QueryInterface(&This->IVectorView_Package_iface, riid, ppv);
}
static ULONG WINAPI VectorIterable_AddRef(IIterable_Package *iface) {
  return 2;
}
static ULONG WINAPI VectorIterable_Release(IIterable_Package *iface) {
  return 1;
}
static HRESULT WINAPI VectorIterable_First(IIterable_Package *iface,
                                           IIterator_Package **iter) {
  return create_dependency_iterator(iter);
}
static HRESULT WINAPI Iterable_Stub(IIterable_Package *iface) {
  return E_NOTIMPL;
}
static const IIterable_PackageVtbl FakeVectorIterable_Vtbl = {
    VectorIterable_QueryInterface, VectorIterable_AddRef,
    VectorIterable_Release,        (void *)Iterable_Stub,
    (void *)Iterable_Stub,         (void *)Iterable_Stub,
    VectorIterable_First};
static FakeDependencyVector global_dependency_vector = {
    {&FakeVectorView_Vtbl}, {&FakeVectorIterable_Vtbl}, 1};

// 4. FakeIterator
typedef struct {
  IIterator_Package IIterator_Package_iface;
  LONG ref;
  int index;
  int count;
  FakePackage **packages;
} FakeIterator;

static inline FakeIterator *
impl_from_IIterator_Package(IIterator_Package *iface) {
  return CONTAINING_RECORD(iface, FakeIterator, IIterator_Package_iface);
}
static HRESULT WINAPI Iter_QueryInterface(IIterator_Package *iface, REFIID riid,
                                          void **ppv) {
  if (IsEqualGUID(riid, &IID_IUnknown) ||
      IsEqualGUID(riid, &IID_IInspectable) ||
      IsEqualGUID(riid, &IID_IIterator_Package)) {
    *ppv = iface;
    IUnknown_AddRef((IUnknown *)*ppv);
    return S_OK;
  }
  return E_NOINTERFACE;
}
static ULONG WINAPI Iter_AddRef(IIterator_Package *iface) { return 2; }
static ULONG WINAPI Iter_Release(IIterator_Package *iface) { return 1; }

static HRESULT WINAPI Iter_get_Current(IIterator_Package *iface,
                                       IPackage **current) {
  FakeIterator *this = impl_from_IIterator_Package(iface);
  TRACE("Iter_get_Current index %d\n", this->index);
  if (this->index < this->count) {
    *current = &this->packages[this->index]->IPackage_iface;
    IPackage_AddRef(*current);
    return S_OK;
  }
  return E_BOUNDS;
}
static HRESULT WINAPI Iter_get_HasCurrent(IIterator_Package *iface,
                                          boolean *has) {
  FakeIterator *this = impl_from_IIterator_Package(iface);
  *has = (this->index < this->count);
  TRACE("Iter_get_HasCurrent: %d\n", *has);
  return S_OK;
}
static HRESULT WINAPI Iter_MoveNext(IIterator_Package *iface, boolean *has) {
  FakeIterator *this = impl_from_IIterator_Package(iface);
  TRACE("Iter_MoveNext: %d -> %d\n", this->index, this->index + 1);
  if (this->index < this->count)
    this->index++;
  *has = (this->index < this->count);
  return S_OK;
}
static HRESULT WINAPI Iter_Stub(IIterator_Package *iface) { return E_NOTIMPL; }
static const IIterator_PackageVtbl FakeIterator_Vtbl = {
    Iter_QueryInterface, Iter_AddRef,         Iter_Release,
    (void *)Iter_Stub,   (void *)Iter_Stub,   (void *)Iter_Stub,
    Iter_get_Current,    Iter_get_HasCurrent, Iter_MoveNext,
    (void *)Iter_Stub};
HRESULT create_package_iterator(FakePackage **pkgs, int count,
                                IIterator_Package **out) {
  FakeIterator *impl = HeapAlloc(GetProcessHeap(), 0, sizeof(FakeIterator));
  if (!impl) {
    *out = NULL;
    return E_OUTOFMEMORY;
  }
  impl->IIterator_Package_iface.lpVtbl = &FakeIterator_Vtbl;
  impl->ref = 1;
  impl->index = 0;
  impl->count = count;
  impl->packages = pkgs;
  *out = &impl->IIterator_Package_iface;
  return S_OK;
}
HRESULT create_dependency_iterator(IIterator_Package **out) {
  ensure_packages();
  return create_package_iterator(framework_packages, framework_count, out);
}

// Iterable
typedef struct {
  IIterable_Package IIterable_Package_iface;
  LONG ref;
  FakePackage **packages;
  int count;
} FakeIterable;

static inline FakeIterable *
impl_from_IIterable_Package(IIterable_Package *iface) {
  return CONTAINING_RECORD(iface, FakeIterable, IIterable_Package_iface);
}
static HRESULT WINAPI Iterable_QueryInterface(IIterable_Package *iface,
                                              REFIID riid, void **ppv) {
  if (IsEqualGUID(riid, &IID_IUnknown) ||
      IsEqualGUID(riid, &IID_IInspectable) ||
      IsEqualGUID(riid, &IID_IIterable_Package)) {
    *ppv = iface;
    IUnknown_AddRef((IUnknown *)*ppv);
    return S_OK;
  }
  return E_NOINTERFACE;
}
static ULONG WINAPI Iterable_AddRef(IIterable_Package *iface) { return 2; }
static ULONG WINAPI Iterable_Release(IIterable_Package *iface) { return 1; }
static HRESULT WINAPI Iterable_First(IIterable_Package *iface,
                                     IIterator_Package **iter) {
  FakeIterable *this = impl_from_IIterable_Package(iface);
  TRACE("Iterable_First for count %d\n", this->count);
  return create_package_iterator(this->packages, this->count, iter);
}
static HRESULT WINAPI Iterable_Stub_Pkg(IIterable_Package *iface) {
  return E_NOTIMPL;
}
static const IIterable_PackageVtbl FakeIterable_Vtbl = {
    Iterable_QueryInterface,
    Iterable_AddRef,
    Iterable_Release,
    (void *)Iterable_Stub_Pkg,
    (void *)Iterable_Stub_Pkg,
    (void *)Iterable_Stub_Pkg,
    Iterable_First};
HRESULT get_fake_package_iterable(IIterable_Package **out) {
  FakeIterable *impl;

  ensure_packages();
  impl = HeapAlloc(GetProcessHeap(), 0, sizeof(FakeIterable));
  if (!impl) {
    *out = NULL;
    return E_OUTOFMEMORY;
  }
  impl->IIterable_Package_iface.lpVtbl = &FakeIterable_Vtbl;
  impl->ref = 1;
  impl->packages = all_packages;
  impl->count = table_count;
  *out = &impl->IIterable_Package_iface;
  return S_OK;
}