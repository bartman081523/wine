/* WinRT Windows.UI.Xaml Implementation
 *
 * Copyright (C) 2025 Mohamad Al-Jaf
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

#ifndef __WINE_WINDOWS_UI_XAML_PRIVATE_H
#define __WINE_WINDOWS_UI_XAML_PRIVATE_H

#include <stdarg.h>

#define COBJMACROS
#include "windef.h"
#include "winbase.h"
#include "winstring.h"

#include "activation.h"

#include "wine/debug.h"

#define WIDL_using_Windows_Foundation
#define WIDL_using_Windows_Foundation_Collections
#include "windows.foundation.h"
#define WIDL_using_Windows_UI
#include "windows.ui.h"

extern IActivationFactory *color_helper_factory;
extern IActivationFactory *application_factory;
extern IActivationFactory *window_factory;
extern IActivationFactory *property_metadata_factory;
extern IActivationFactory *dependency_property_factory;
extern IActivationFactory *dependency_object_factory;
extern IActivationFactory *user_factory;
extern IActivationFactory *size_helper_factory;
extern IActivationFactory *frame_factory;
extern IActivationFactory *page_factory;
extern IActivationFactory *grid_factory;
extern IActivationFactory *canvas_factory;
extern IActivationFactory *swapchainpanel_factory;
extern IActivationFactory *control_factory;
extern IActivationFactory *button_factory;
extern IActivationFactory *button_base_factory;
extern IActivationFactory *textblock_factory;
extern IActivationFactory *border_factory;
extern IActivationFactory *stackpanel_factory;
extern IActivationFactory *usercontrol_factory;
extern IActivationFactory *contentcontrol_factory;
extern IActivationFactory *rowdef_factory;
extern IActivationFactory *coldef_factory;
extern IActivationFactory *automation_properties_factory;
extern IActivationFactory *flyout_factory;
extern IActivationFactory *menuflyout_factory;
extern IActivationFactory *menuflyoutitem_factory;
extern IActivationFactory *setter_factory;
extern IActivationFactory *style_factory;
extern IActivationFactory *binding_factory;
extern IActivationFactory *property_path_factory;
extern IActivationFactory *property_changed_event_args_factory;
extern IActivationFactory *core_text_services_manager_factory;
extern IActivationFactory *content_dialog_factory;
extern IActivationFactory *composition_target_factory;

void *create_dependency_property_named(const WCHAR *name);
struct dp_entry
{
    void *dp;
    IInspectable *value;
};
HRESULT dependency_property_get_default_value(void *dp, IInspectable **value);
HRESULT get_global_dispatcher(void **value);
HRESULT xaml_load_component(void *component, void *locator);
void xaml_window_init(HWND hwnd);
void xaml_fire_loaded_events(void);
HRESULT create_resource_dictionary_instance(void **value);
HRESULT create_menu_flyout_instance(void **value);
HRESULT create_menu_flyout_item_instance(const WCHAR *name, void **value);
void xaml_fire_swapchain_size_changed(FLOAT width, FLOAT height);
LRESULT CALLBACK xaml_window_wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

struct typed_event_handler_vtbl
{
    HRESULT (WINAPI *QueryInterface)(void *This, REFIID riid, void **ppv);
    ULONG (WINAPI *AddRef)(void *This);
    ULONG (WINAPI *Release)(void *This);
    HRESULT (WINAPI *Invoke)(void *This, void *sender, void *args);
};

struct typed_event_handler
{
    const struct typed_event_handler_vtbl *lpVtbl;
};

void core_independent_input_source_fire_pointer(UINT msg, WPARAM wparam, LPARAM lparam, void *args);
int core_independent_input_source_has_handlers(UINT msg);

#define DEFINE_IINSPECTABLE_( pfx, iface_type, impl_type, impl_from, iface_mem, expr )             \
    static inline impl_type *impl_from( iface_type *iface )                                        \
    {                                                                                              \
        return CONTAINING_RECORD( iface, impl_type, iface_mem );                                   \
    }                                                                                              \
    static HRESULT WINAPI pfx##_QueryInterface( iface_type *iface, REFIID iid, void **out )        \
    {                                                                                              \
        impl_type *impl = impl_from( iface );                                                      \
        return IInspectable_QueryInterface( (IInspectable *)(expr), iid, out );                    \
    }                                                                                              \
    static ULONG WINAPI pfx##_AddRef( iface_type *iface )                                          \
    {                                                                                              \
        impl_type *impl = impl_from( iface );                                                      \
        return IInspectable_AddRef( (IInspectable *)(expr) );                                      \
    }                                                                                              \
    static ULONG WINAPI pfx##_Release( iface_type *iface )                                         \
    {                                                                                              \
        impl_type *impl = impl_from( iface );                                                      \
        return IInspectable_Release( (IInspectable *)(expr) );                                     \
    }                                                                                              \
    static HRESULT WINAPI pfx##_GetIids( iface_type *iface, ULONG *iid_count, IID **iids )         \
    {                                                                                              \
        impl_type *impl = impl_from( iface );                                                      \
        return IInspectable_GetIids( (IInspectable *)(expr), iid_count, iids );                    \
    }                                                                                              \
    static HRESULT WINAPI pfx##_GetRuntimeClassName( iface_type *iface, HSTRING *class_name )      \
    {                                                                                              \
        impl_type *impl = impl_from( iface );                                                      \
        return IInspectable_GetRuntimeClassName( (IInspectable *)(expr), class_name );             \
    }                                                                                              \
    static HRESULT WINAPI pfx##_GetTrustLevel( iface_type *iface, TrustLevel *trust_level )        \
    {                                                                                              \
        impl_type *impl = impl_from( iface );                                                      \
        return IInspectable_GetTrustLevel( (IInspectable *)(expr), trust_level );                  \
    }
#define DEFINE_IINSPECTABLE( pfx, iface_type, impl_type, base_iface )                              \
    DEFINE_IINSPECTABLE_( pfx, iface_type, impl_type, impl_from_##iface_type, iface_type##_iface, &impl->base_iface )

extern void *global_xaml_metadata_provider;
HRESULT xaml_create_instance_by_name(const WCHAR *class_name, void **instance);

#endif
