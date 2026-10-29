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

#include "initguid.h"
#include "private.h"

WINE_DEFAULT_DEBUG_CHANNEL(xaml);

HRESULT WINAPI DllGetActivationFactory( HSTRING classid, IActivationFactory **factory )
{
    const WCHAR *buffer = WindowsGetStringRawBuffer( classid, NULL );

    TRACE( "class %s, factory %p.\n", debugstr_hstring(classid), factory );

    *factory = NULL;

    if (!wcscmp( buffer, RuntimeClass_Windows_UI_ColorHelper ))
        IActivationFactory_QueryInterface( color_helper_factory, &IID_IActivationFactory, (void **)factory );
    else if (!wcscmp( buffer, L"Windows.UI.Xaml.Application" ))
        IActivationFactory_QueryInterface( application_factory, &IID_IActivationFactory, (void **)factory );
    else if (!wcscmp( buffer, L"Windows.UI.Xaml.Window" ))
        IActivationFactory_QueryInterface( window_factory, &IID_IActivationFactory, (void **)factory );
    else if (!wcscmp( buffer, L"Windows.UI.Xaml.PropertyMetadata" ))
        IActivationFactory_QueryInterface( property_metadata_factory, &IID_IActivationFactory, (void **)factory );
    else if (!wcscmp( buffer, L"Windows.UI.Xaml.DependencyProperty" ))
        IActivationFactory_QueryInterface( dependency_property_factory, &IID_IActivationFactory, (void **)factory );
    else if (!wcscmp( buffer, L"Windows.UI.Xaml.DependencyObject" ))
        IActivationFactory_QueryInterface( dependency_object_factory, &IID_IActivationFactory, (void **)factory );
    else if (!wcscmp( buffer, L"Windows.System.User" ))
        IActivationFactory_QueryInterface( user_factory, &IID_IActivationFactory, (void **)factory );
    else if (!wcscmp( buffer, L"Windows.UI.Xaml.SizeHelper" ))
        IActivationFactory_QueryInterface( size_helper_factory, &IID_IActivationFactory, (void **)factory );
    else if (!wcscmp( buffer, L"Windows.UI.Xaml.Controls.Frame" ))
        IActivationFactory_QueryInterface( frame_factory, &IID_IActivationFactory, (void **)factory );
    else if (!wcscmp( buffer, L"Windows.UI.Xaml.Controls.Page" ))
        IActivationFactory_QueryInterface( page_factory, &IID_IActivationFactory, (void **)factory );
    else if (!wcscmp( buffer, L"Windows.UI.Xaml.Controls.Grid" ))
        IActivationFactory_QueryInterface( grid_factory, &IID_IActivationFactory, (void **)factory );
    else if (!wcscmp( buffer, L"Windows.UI.Xaml.Controls.Control" ))
        IActivationFactory_QueryInterface( control_factory, &IID_IActivationFactory, (void **)factory );
    else if (!wcscmp( buffer, L"Windows.UI.Xaml.Controls.Button" ))
        IActivationFactory_QueryInterface( button_factory, &IID_IActivationFactory, (void **)factory );
    else if (!wcscmp( buffer, L"Windows.UI.Xaml.Controls.Primitives.ButtonBase" ))
        IActivationFactory_QueryInterface( button_base_factory, &IID_IActivationFactory, (void **)factory );
    else if (!wcscmp( buffer, L"Windows.UI.Xaml.Controls.TextBlock" ))
        IActivationFactory_QueryInterface( textblock_factory, &IID_IActivationFactory, (void **)factory );
    else if (!wcscmp( buffer, L"Windows.UI.Xaml.Controls.Border" ))
        IActivationFactory_QueryInterface( border_factory, &IID_IActivationFactory, (void **)factory );
    else if (!wcscmp( buffer, L"Windows.UI.Xaml.Controls.StackPanel" ))
        IActivationFactory_QueryInterface( stackpanel_factory, &IID_IActivationFactory, (void **)factory );
    else if (!wcscmp( buffer, L"Windows.UI.Xaml.Controls.UserControl" ))
        IActivationFactory_QueryInterface( usercontrol_factory, &IID_IActivationFactory, (void **)factory );
    else if (!wcscmp( buffer, L"Windows.UI.Xaml.Controls.ContentControl" ))
        IActivationFactory_QueryInterface( contentcontrol_factory, &IID_IActivationFactory, (void **)factory );
    else if (!wcscmp( buffer, L"Windows.UI.Xaml.Controls.RowDefinition" ))
        IActivationFactory_QueryInterface( rowdef_factory, &IID_IActivationFactory, (void **)factory );
    else if (!wcscmp( buffer, L"Windows.UI.Xaml.Controls.ColumnDefinition" ))
        IActivationFactory_QueryInterface( coldef_factory, &IID_IActivationFactory, (void **)factory );
    else if (!wcscmp( buffer, L"Windows.UI.Xaml.Automation.AutomationProperties" ))
        IActivationFactory_QueryInterface( automation_properties_factory, &IID_IActivationFactory, (void **)factory );
    else if (!wcscmp( buffer, L"Windows.UI.Xaml.Controls.Flyout" ))
        IActivationFactory_QueryInterface( flyout_factory, &IID_IActivationFactory, (void **)factory );
    else if (!wcscmp( buffer, L"Windows.UI.Xaml.Controls.MenuFlyout" ))
        IActivationFactory_QueryInterface( menuflyout_factory, &IID_IActivationFactory, (void **)factory );
    else if (!wcscmp( buffer, L"Windows.UI.Xaml.Controls.MenuFlyoutItem" ))
        IActivationFactory_QueryInterface( menuflyoutitem_factory, &IID_IActivationFactory, (void **)factory );
    else if (!wcscmp( buffer, L"Windows.UI.Xaml.Setter" ))
        IActivationFactory_QueryInterface( setter_factory, &IID_IActivationFactory, (void **)factory );
    else if (!wcscmp( buffer, L"Windows.UI.Xaml.Style" ))
        IActivationFactory_QueryInterface( style_factory, &IID_IActivationFactory, (void **)factory );
    else if (!wcscmp( buffer, L"Windows.UI.Xaml.Data.Binding" ))
        IActivationFactory_QueryInterface( binding_factory, &IID_IActivationFactory, (void **)factory );
    else if (!wcscmp( buffer, L"Windows.UI.Xaml.PropertyPath" ))
        IActivationFactory_QueryInterface( property_path_factory, &IID_IActivationFactory, (void **)factory );
    else if (!wcscmp( buffer, L"Windows.UI.Xaml.Data.PropertyChangedEventArgs" ))
        IActivationFactory_QueryInterface( property_changed_event_args_factory, &IID_IActivationFactory, (void **)factory );

    if (*factory) return S_OK;
    return CLASS_E_CLASSNOTAVAILABLE;
}
