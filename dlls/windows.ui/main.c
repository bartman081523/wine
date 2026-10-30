/* WinRT Windows.UI Implementation
 *
 * Copyright (C) 2023 Mohamad Al-Jaf
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

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(ui);

#include "systemnavigationmanager.c"
#include "displayinformation.c"
#include "inputcapabilities.c"
#include "taskbarmanager.c"

HRESULT WINAPI DllGetClassObject( REFCLSID clsid, REFIID riid, void **out )
{
    FIXME( "clsid %s, riid %s, out %p stub!\n", debugstr_guid(clsid), debugstr_guid(riid), out );
    return CLASS_E_CLASSNOTAVAILABLE;
}

HRESULT WINAPI DllGetActivationFactory( HSTRING classid, IActivationFactory **factory )
{
    const WCHAR *buffer = WindowsGetStringRawBuffer( classid, NULL );

    TRACE( "class %s, factory %p.\n", debugstr_hstring(classid), factory );

    *factory = NULL;

    if (!wcscmp( buffer, RuntimeClass_Windows_UI_ViewManagement_AccessibilitySettings ))
        IActivationFactory_QueryInterface( accessibilitysettings_factory, &IID_IActivationFactory, (void **)factory );

    if (!wcscmp( buffer, RuntimeClass_Windows_UI_ViewManagement_UISettings ))
        IActivationFactory_QueryInterface( uisettings_factory, &IID_IActivationFactory, (void **)factory );

    if (!wcscmp( buffer, RuntimeClass_Windows_UI_ViewManagement_UIViewSettings ))
        IActivationFactory_QueryInterface( uiviewsettings_factory, &IID_IActivationFactory, (void **)factory );

    if (!wcscmp( buffer, RuntimeClass_Windows_UI_ViewManagement_InputPane ))
        IActivationFactory_QueryInterface( inputpane_factory, &IID_IActivationFactory, (void **)factory );

    if (!wcscmp( buffer, RuntimeClass_Windows_UI_Core_CoreWindow ))
        IActivationFactory_QueryInterface( corewindow_factory, &IID_IActivationFactory, (void **)factory );

    if (!wcscmp( buffer, L"Windows.UI.Core.SystemNavigationManager" ))
        IActivationFactory_QueryInterface( system_navigation_manager_factory, &IID_IActivationFactory, (void **)factory );

    if (!wcscmp( buffer, L"Windows.UI.Core.Preview.SystemNavigationManagerPreview" ))
        IActivationFactory_QueryInterface( system_navigation_manager_preview_factory, &IID_IActivationFactory, (void **)factory );

    if (!wcscmp( buffer, L"Windows.Graphics.Display.DisplayInformation" ))
        IActivationFactory_QueryInterface( display_information_factory, &IID_IActivationFactory, (void **)factory );

    if (!wcscmp( buffer, L"Windows.Devices.Input.KeyboardCapabilities" ))
        IActivationFactory_QueryInterface( keyboard_capabilities_factory, &IID_IActivationFactory, (void **)factory );

    if (!wcscmp( buffer, L"Windows.Devices.Input.MouseCapabilities" ))
        IActivationFactory_QueryInterface( mouse_capabilities_factory, &IID_IActivationFactory, (void **)factory );

    if (!wcscmp( buffer, L"Windows.Devices.Input.TouchCapabilities" ))
        IActivationFactory_QueryInterface( touch_capabilities_factory, &IID_IActivationFactory, (void **)factory );

    if (!wcscmp( buffer, L"Windows.UI.Notifications.BadgeUpdateManager" ))
        IActivationFactory_QueryInterface( badgeupdatemanager_factory, &IID_IActivationFactory, (void **)factory );

    if (!wcscmp( buffer, L"Windows.UI.Notifications.ToastNotificationManager" ))
        IActivationFactory_QueryInterface( toastnotificationmanager_factory, &IID_IActivationFactory, (void **)factory );

    if (!wcscmp( buffer, L"Windows.UI.Notifications.TileUpdateManager" ))
        IActivationFactory_QueryInterface( tileupdatemanager_factory, &IID_IActivationFactory, (void **)factory );

    if (!wcscmp( buffer, L"Windows.UI.Shell.TaskbarManager" ))
        IActivationFactory_QueryInterface( taskbar_manager_factory, &IID_IActivationFactory, (void **)factory );

    if (!wcsncmp( buffer, L"Windows.UI.Composition.", 23 ))
    {
        HMODULE dcomp = LoadLibraryW( L"dcomp.dll" );
        if (dcomp)
        {
            HRESULT (WINAPI *pDllGetActivationFactory)(HSTRING, IActivationFactory **);
            pDllGetActivationFactory = (void *)GetProcAddress( dcomp, "DllGetActivationFactory" );
            if (pDllGetActivationFactory)
                return pDllGetActivationFactory( classid, factory );
        }
    }

    if (*factory) return S_OK;
    return CLASS_E_CLASSNOTAVAILABLE;
}
