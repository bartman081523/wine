/*
 * WinRT Windows.UI.Xaml Generic Controls & Visual Tree Implementation
 *
 * Copyright (C) 2026 Julian / Xodus Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "private.h"
#include "winuser.h"
#include "wingdi.h"
#include <math.h>
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(xaml);

/* Core WinUI Interface GUIDs */
static const GUID IID_IUIElement             = {0x676d0be9, 0xb65c, 0x41c6, {0xba, 0x40, 0x58, 0xcf, 0x87, 0xf2, 0x01, 0xc1}};
static const GUID IID_IUIElement2            = {0x676d0bf9, 0xb66c, 0x41d6, {0xba, 0x50, 0x58, 0xcf, 0x87, 0xf2, 0x01, 0xd1}};
static const GUID IID_IUIElement3            = {0xbc2b28f1, 0x26f2, 0x4aab, {0xb2, 0x56, 0x3b, 0x53, 0x50, 0x88, 0x1e, 0x37}};
static const GUID IID_IUIElement4            = {0x69145cd4, 0x199a, 0x4657, {0x9e, 0x57, 0xe9, 0x9e, 0x8f, 0x13, 0x67, 0x12}};
static const GUID IID_IUIElement5            = {0x8eed9bc2, 0xa58c, 0x4453, {0xaf, 0x0f, 0xa9, 0x2e, 0xe0, 0x6d, 0x03, 0x17}};
static const GUID IID_IUIElement7            = {0xcafc4968, 0x6369, 0x4249, {0x80, 0xf9, 0x3d, 0x65, 0x63, 0x19, 0xe8, 0x11}};
static const GUID IID_IUIElementOverrides    = {0x608d2f1d, 0x7858, 0x4aeb, {0x89, 0xe4, 0xb5, 0x4e, 0x2c, 0x7e, 0xd3, 0xd3}};
static const GUID IID_IUIElementOverrides7   = {0xb97f7f68, 0xc29b, 0x4c99, {0xa1, 0xc3, 0x95, 0x26, 0x19, 0xd6, 0xe7, 0x20}};
static const GUID IID_IPointer               = {0x5ee8f39f, 0x747d, 0x4171, {0x90, 0xe6, 0xcd, 0x37, 0xa9, 0xdf, 0xfb, 0x11}};
static const GUID IID_IPointerRoutedEventArgs = {0xda628f0a, 0x9752, 0x49e2, {0xbd, 0xe2, 0x49, 0xec, 0xca, 0xb9, 0x19, 0x4d}};
static const GUID IID_IPointerPoint          = {0xe995317d, 0x7296, 0x42d9, {0x82, 0x33, 0xc5, 0xbe, 0x73, 0xb7, 0x4a, 0x4a}};
static const GUID IID_IPointerPointProperties = {0xc79d8a4b, 0xc163, 0x4ee7, {0x80, 0x3f, 0x67, 0xce, 0x79, 0xf9, 0x97, 0x2d}};
static const GUID IID_IFrameworkElement      = {0xa391d09b, 0x4a99, 0x4b7c, {0x9d, 0x8d, 0x6f, 0xa5, 0xd0, 0x1f, 0x6f, 0xbf}};
static const GUID IID_IFrameworkElement2     = {0xf19104be, 0x422a, 0x4904, {0xa5, 0x2f, 0xee, 0x72, 0x01, 0x04, 0x29, 0xe5}};
static const GUID IID_IFrameworkElement3     = {0xc81c2720, 0x5c52, 0x4bbe, {0xa1, 0x99, 0x2b, 0x1e, 0x34, 0xf0, 0x0f, 0x70}};
static const GUID IID_IFrameworkElement4     = {0x6b765bb3, 0xfba3, 0x4404, {0xbd, 0xee, 0x1a, 0x45, 0xd1, 0xca, 0x5f, 0x21}};
static const GUID IID_IFrameworkElement6     = {0x792a5d91, 0x62a1, 0x40bf, {0xa0, 0xce, 0xf9, 0xc1, 0x31, 0xfc, 0xb7, 0xa7}};
static const GUID IID_IFrameworkElementOverrides  = {0xda007e54, 0xb3c2, 0x4b9a, {0xaa, 0x8e, 0xd3, 0xf0, 0x71, 0x26, 0x2b, 0x97}};
static const GUID IID_IFrameworkElementOverrides2 = {0xcb5cd2b9, 0xe3b4, 0x458c, {0xb6, 0x4e, 0x14, 0x34, 0xfd, 0x1b, 0xd8, 0x8a}};
static const GUID IID_IControl               = {0xb4a04776, 0x4e88, 0x50ca, {0x8f, 0x2b, 0x08, 0x94, 0x0d, 0x6c, 0x5f, 0x94}};
static const GUID IID_IControl_sdk           = {0xa8912263, 0x2951, 0x4f58, {0xa9, 0xc5, 0x5a, 0x13, 0x4e, 0xaa, 0x7f, 0x07}};
static const GUID IID_IControlProtected      = {0x1791587a, 0x2a7c, 0x4981, {0xa2, 0xd8, 0xab, 0x99, 0xff, 0xfc, 0x65, 0xb9}};
static const GUID IID_IControl2              = {0x43e0fe10, 0x14ec, 0x427e, {0x8c, 0x57, 0xde, 0xe6, 0x0d, 0xf6, 0x0a, 0xa8}};
static const GUID IID_IControl3              = {0x077b29e4, 0xdade, 0x4f55, {0x9b, 0x96, 0x09, 0xe2, 0x1b, 0x28, 0xeb, 0x1c}};
static const GUID IID_IControl4              = {0x0e18aeee, 0x5f2e, 0x44ea, {0x85, 0x13, 0xd3, 0x87, 0x5c, 0x0a, 0x75, 0x13}};
static const GUID IID_IControl5              = {0x5ebb51d5, 0x9e58, 0x49b7, {0xbc, 0x2d, 0x01, 0x55, 0xff, 0x11, 0x86, 0x64}};
static const GUID IID_IControlOverrides      = {0xa09691df, 0x9824, 0x41fe, {0xb5, 0x30, 0xb0, 0xd8, 0x99, 0x0e, 0x64, 0xc1}};
static const GUID IID_IControlOverrides6     = {0xe194c96a, 0xd8cc, 0x4339, {0xa0, 0xe1, 0x2e, 0x9c, 0x42, 0x08, 0x79, 0xec}};
static const GUID IID_IContentControl        = {0xa26dd1dc, 0xcd44, 0x435c, {0xbe, 0x94, 0x01, 0xd6, 0x24, 0x1c, 0x23, 0x1c}};
static const GUID IID_IContentControl2       = {0x652c208c, 0x8ccf, 0x4399, {0xbd, 0x3e, 0x5a, 0x01, 0x5a, 0xa1, 0xbc, 0x03}};
static const GUID IID_IContentControlOverrides = {0x5f4c0b10, 0xe38e, 0x4b5d, {0xbe, 0x1a, 0x5e, 0xd0, 0x42, 0x46, 0xa6, 0x35}};
static const GUID IID_IUserControl           = {0xa7a69ec9, 0xea35, 0x4679, {0xbf, 0x29, 0xf4, 0xf0, 0x92, 0x86, 0xd3, 0x14}};
static const GUID IID_IUserControlStatics    = {0xe8887976, 0x2c5c, 0x41cf, {0xbe, 0x6a, 0x9e, 0x44, 0xbe, 0xfd, 0xf6, 0x55}};
static const GUID IID_IPanel                 = {0xa50a4bbd, 0x8361, 0x469c, {0x90, 0xda, 0xe9, 0xa4, 0x0c, 0x74, 0x74, 0xdf}};
static const GUID IID_IGrid                  = {0xfd104460, 0x2e15, 0x4ba3, {0x8b, 0x8f, 0xfa, 0x69, 0x3a, 0x41, 0x61, 0xe9}};
static const GUID IID_IGridStatics           = {0x64fe2e9f, 0xf951, 0x42b6, {0xa9, 0xce, 0xbb, 0x17, 0x9a, 0xf1, 0x15, 0x95}};
static const GUID IID_ICanvas                = {0x79190e19, 0xcd38, 0x4823, {0xae, 0xae, 0x64, 0xa7, 0x71, 0x32, 0xf5, 0x19}};
static const GUID IID_ICanvasFactory         = {0x1b328bd1, 0xb400, 0x4a8e, {0x94, 0x3b, 0x5a, 0xd2, 0xc4, 0x5b, 0xe0, 0xdf}};
static const GUID IID_ICanvasStatics         = {0x0e2bf0f9, 0xb041, 0x4497, {0xb0, 0xf6, 0x36, 0x59, 0x22, 0x9f, 0x61, 0x6b}};
static const GUID IID_ISwapChainPanel        = {0xc589644f, 0xeba8, 0x427a, {0xb7, 0x5a, 0x9f, 0x1f, 0x93, 0xa1, 0x1a, 0xe9}};
static const GUID IID_ISwapChainPanelFactory = {0xf38f8d7f, 0x1a48, 0x49cb, {0x86, 0xd2, 0x10, 0xea, 0xaa, 0xf6, 0xfd, 0x70}};
static const GUID IID_ISwapChainPanelStatics = {0xf89c0297, 0xeea8, 0x498c, {0x99, 0xc4, 0x25, 0x7e, 0x36, 0x60, 0xdf, 0x7e}};
static const GUID IID_ISwapChainPanelNative  = {0xf92f19d2, 0x3ade, 0x45a6, {0xa2, 0x0c, 0xf6, 0xf1, 0xea, 0x90, 0x55, 0x4b}};
static const GUID IID_ISwapChainPanelNative2 = {0xd5a2f60c, 0x37b2, 0x44a2, {0x93, 0x7b, 0x8d, 0x8e, 0xb9, 0x72, 0x68, 0x21}};
static const GUID IID_IBorder                = {0x797c4539, 0x45bd, 0x4633, {0xa0, 0x44, 0xbf, 0xb0, 0x2e, 0xf5, 0x17, 0x0f}};
static const GUID IID_IButton                = {0x280335ae, 0x5570, 0x46c7, {0x8e, 0x0b, 0x60, 0x2b, 0xe7, 0x12, 0x29, 0xa2}};
static const GUID IID_IButtonBase            = {0xfa002c1a, 0x494e, 0x46cf, {0x91, 0xd4, 0xe1, 0x4a, 0x8d, 0x79, 0x86, 0x74}};
static const GUID IID_ITextBlock             = {0xae2d9271, 0x3b4a, 0x45fc, {0x84, 0x68, 0xf7, 0x94, 0x95, 0x48, 0xf4, 0xd5}};
static const GUID IID_IComponentConnector    = {0xf6790987, 0xe6e5, 0x47f2, {0x92, 0xc6, 0xec, 0xcc, 0xe4, 0xba, 0x15, 0x9a}};
static const GUID IID_IComponentConnector2   = {0xdc8f368b, 0xeccc, 0x498e, {0xb1, 0x39, 0x91, 0x14, 0x22, 0x54, 0xd7, 0xae}};
static const GUID IID_IVector_UIElement      = {0xb4c1e3ac, 0x8768, 0x5b9d, {0xa6, 0x61, 0xf6, 0x33, 0x30, 0xb8, 0x50, 0x7b}};
static const GUID IID_IDependencyObject_local  = {0x5c526665, 0xf60e, 0x4912, {0xaf, 0x59, 0x5f, 0xe0, 0x68, 0x0f, 0x08, 0x9d}};
static const GUID IID_IDependencyObject2_local = {0x29fed85d, 0x3d22, 0x43a1, {0xad, 0xd0, 0x17, 0x02, 0x7c, 0x08, 0xb2, 0x12}};
static const GUID IID_ComWrappers_internal     = {0x11d3b13a, 0x180e, 0x4789, {0xa8, 0xbe, 0x77, 0x12, 0x88, 0x28, 0x93, 0xe6}};
static const GUID IID_ComWrappers_internal2    = {0x5c13e51c, 0x241d, 0x4859, {0x80, 0x05, 0x8a, 0x8d, 0x8d, 0x98, 0x15, 0xbf}};
static const GUID IID_ITrackerOwner            = {0xeb24c20b, 0x9816, 0x4ac7, {0x8c, 0xff, 0x36, 0xf6, 0x7a, 0x11, 0x8f, 0x4e}};
static const GUID IID_IAgileObject_local       = {0x94ea2b94, 0xe9cc, 0x49e0, {0xc0, 0xff, 0xee, 0x64, 0xca, 0x8f, 0x5b, 0x90}};
static const GUID IID_INavigationView_local    = {0xb00eb54c, 0x9174, 0x5d84, {0x96, 0x78, 0x56, 0xc9, 0x80, 0x16, 0xe6, 0x89}};
static const GUID IID_INavigationView2_local   = {0x5ba2eefc, 0x3736, 0x5e42, {0xac, 0x56, 0x9d, 0x0b, 0xe5, 0x52, 0x3d, 0x40}};
static const GUID IID_IPage                     = {0xdfa149ac, 0x1849, 0x445e, {0x93, 0x7c, 0x40, 0xa9, 0x59, 0x0c, 0xc0, 0x76}};
static const GUID IID_IPageOverrides            = {0x74b861a1, 0x7487, 0x46a9, {0x9a, 0x6e, 0xc7, 0x8b, 0x51, 0x27, 0x26, 0xc5}};
static const GUID IID_IFlyout                   = {0x6584ebdf, 0xcbc7, 0x4e19, {0x84, 0x3a, 0xa1, 0x4f, 0x24, 0xa7, 0x93, 0xea}};
static const GUID IID_IFlyoutFactory            = {0x4bed4933, 0x008e, 0x4b03, {0xa3, 0x85, 0x79, 0xfe, 0x52, 0x66, 0xdd, 0xba}};
static const GUID IID_IFlyoutBase               = {0x723eea0b, 0xd12e, 0x430d, {0xa9, 0xf0, 0x9b, 0xb3, 0x2b, 0xbf, 0x99, 0x13}};
static const GUID IID_IFlyoutBase2              = {0xf82b435e, 0x65b3, 0x41c6, {0xa9, 0xe2, 0x77, 0xb6, 0x7b, 0xc4, 0xc0, 0x0c}};
static const GUID IID_IMenuFlyout               = {0xad3210f1, 0x985b, 0x4171, {0xbb, 0xba, 0x67, 0xbc, 0xc7, 0x29, 0x64, 0x9b}};
static const GUID IID_IMenuFlyout2              = {0x507c7bad, 0xd9d1, 0x4c05, {0x9d, 0x4b, 0xcd, 0xa8, 0xde, 0x9a, 0xb2, 0x42}};
static const GUID IID_IMenuFlyoutFactory         = {0x644533ab, 0xbbb9, 0x499b, {0x9f, 0xf3, 0x30, 0xd4, 0x0e, 0x2c, 0x56, 0xd8}};
static const GUID IID_IMenuFlyoutItem           = {0x160775cf, 0xb598, 0x44af, {0xa4, 0x88, 0xc3, 0xce, 0x2a, 0xf6, 0xd3, 0xf0}};
static const GUID IID_IMenuFlyoutItem2          = {0x0ae266eb, 0x028c, 0x4423, {0xa8, 0xe3, 0x98, 0x9f, 0xd9, 0xdd, 0x71, 0x26}};
static const GUID IID_IMenuFlyoutItemBase       = {0xf9b56047, 0x8da6, 0x4730, {0x92, 0x08, 0x14, 0x41, 0x25, 0xc6, 0xb3, 0x96}};
static const GUID IID_IMenuFlyoutItemFactory    = {0xc5aa39e2, 0x44f3, 0x455e, {0x9c, 0x5b, 0x62, 0xbb, 0x6a, 0xfc, 0xe3, 0xad}};
static const GUID IID_IVector_MenuFlyoutItemBase = {0xf7f1c37b, 0xb4df, 0x5128, {0xa4, 0x76, 0xfc, 0x83, 0x99, 0xbc, 0x10, 0xaf}};
static const GUID IID_IIterable_MenuFlyoutItemBase = {0x0bb14d64, 0xcc9a, 0x58ae, {0xb8, 0x81, 0x73, 0xfe, 0x0e, 0xcb, 0x99, 0xeb}};
static const GUID IID_IStyle                    = {0xc4a9f225, 0x9db7, 0x4a7d, {0xb6, 0xd1, 0xf7, 0x4e, 0xdb, 0x92, 0x93, 0xc2}};
static const GUID IID_IStyleFactory             = {0xa36824e3, 0x3d81, 0x4ce5, {0xaa, 0x51, 0x8b, 0x41, 0x0f, 0x60, 0x2f, 0xcd}};
static const GUID IID_ISetterBaseCollection     = {0x03c40ca8, 0x909e, 0x4117, {0x81, 0x1c, 0xa4, 0x52, 0x94, 0x96, 0xbd, 0xf1}};
static const GUID IID_IVector_SetterBase        = {0xbd55a7fd, 0x7501, 0x5519, {0x9f, 0xfb, 0xd0, 0x07, 0x42, 0x40, 0xdb, 0x61}};
static const GUID IID_IIterable_SetterBase      = {0xaafe70cb, 0x90a8, 0x5719, {0xbd, 0x3e, 0xf0, 0xb0, 0x66, 0xed, 0x16, 0x26}};
static const GUID IID_ISetter                   = {0xa73ded29, 0xb4ae, 0x4a81, {0xbe, 0x85, 0xe6, 0x90, 0xba, 0x0d, 0x3b, 0x6e}};
static const GUID IID_ISetter2                  = {0x70169561, 0x05b1, 0x4fa3, {0x9d, 0x53, 0x8e, 0x0c, 0x8c, 0x74, 0x7a, 0xfc}};
static const GUID IID_ISetterBase               = {0x418be27c, 0x2ac4, 0x4f22, {0x80, 0x97, 0xde, 0xa3, 0xae, 0xeb, 0x2f, 0xb3}};
static const GUID IID_ISetterFactory            = {0xd3ca3d42, 0x09b1, 0x49d5, {0x88, 0x91, 0xe7, 0xb5, 0x64, 0x8e, 0x02, 0xa2}};
static const GUID IID_IBinding                  = {0x3f7a0c6b, 0xd00f, 0x4730, {0x8c, 0x1d, 0x48, 0xe1, 0x6c, 0x46, 0xf9, 0xca}};
static const GUID IID_IBinding2                 = {0x34f96fcb, 0x0406, 0x48b3, {0x9e, 0x82, 0xf3, 0x33, 0xec, 0x4c, 0x69, 0x10}};
static const GUID IID_IBindingBase              = {0x1589a2ab, 0x3d15, 0x49bc, {0xa4, 0x47, 0x8a, 0x54, 0x48, 0xe5, 0x88, 0x70}};
static const GUID IID_IBindingFactory           = {0xff42bb08, 0xc39e, 0x4f7e, {0x84, 0x34, 0xa1, 0x56, 0x90, 0x83, 0x88, 0x3c}};
static const GUID IID_IPropertyPath             = {0x300e5d8a, 0x1ff3, 0x4d2c, {0x95, 0xec, 0x27, 0xf8, 0x1d, 0xeb, 0xac, 0xb8}};
static const GUID IID_IPropertyPathFactory      = {0x4e4cdf99, 0x9826, 0x4e56, {0x84, 0x7c, 0xca, 0x05, 0x5f, 0x16, 0x29, 0x05}};
static const GUID IID_IPropertyChangedEventArgs = {0x4f33a9a0, 0x5cf4, 0x47a4, {0xb1, 0x6f, 0xd7, 0xfa, 0xaf, 0x17, 0x45, 0x7e}};
static const GUID IID_IPropertyChangedEventArgsFactory = {0x6dcc9c03, 0xe0c7, 0x4eee, {0x8e, 0xa9, 0x37, 0xe3, 0x40, 0x6e, 0xeb, 0x1c}};
static const GUID IID_IWeakReferenceSource      = {0x00000038, 0x0000, 0x0000, {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
static const GUID IID_IWeakReference            = {0x00000037, 0x0000, 0x0000, {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
static const GUID IID_IRowDefinition            = {0x4abae829, 0xd80c, 0x4a5e, {0xa4, 0x8c, 0xf8, 0xb3, 0xd3, 0xb6, 0x53, 0x3d}};
static const GUID IID_IRowDefinitionStatics     = {0x5adf3fe5, 0x2056, 0x4724, {0x94, 0xd6, 0xe4, 0x81, 0x2b, 0x02, 0x2e, 0xc8}};
static const GUID IID_IColumnDefinition         = {0xf7f1b229, 0xf024, 0x467f, {0x97, 0x0a, 0x7e, 0x70, 0x56, 0x15, 0xdb, 0x7b}};
static const GUID IID_IColumnDefinitionStatics  = {0x06b0d728, 0xd044, 0x40c6, {0x94, 0x2e, 0xae, 0x60, 0xea, 0xc7, 0x48, 0x51}};
static const GUID IID_IAdaptiveTrigger          = {0xa5f04119, 0x0cd9, 0x49f1, {0xa2, 0x3f, 0x44, 0xe5, 0x47, 0xab, 0x9f, 0x1a}};
static const GUID IID_IStateTriggerBase         = {0x48b20698, 0xaf06, 0x466c, {0x80, 0x52, 0x93, 0x66, 0x6d, 0xde, 0x0e, 0x49}};
static const GUID IID_IAdaptiveTriggerFactory   = {0xc966d482, 0x5aeb, 0x4841, {0x92, 0x47, 0xc1, 0xa0, 0xbd, 0xd6, 0xf5, 0x9f}};
static const GUID IID_IContentDialog            = {0x38dc4404, 0xd24e, 0x40d8, {0x94, 0x15, 0x34, 0x94, 0x64, 0xc1, 0xaf, 0xdc}};
static const GUID IID_IContentDialog2           = {0x2f93eb45, 0xee43, 0x4303, {0x9b, 0x38, 0x3f, 0xe1, 0xa1, 0x11, 0xec, 0xbf}};
static const GUID IID_IContentDialogFactory     = {0x05557178, 0x9d8e, 0x4315, {0xb3, 0x7d, 0x68, 0x0c, 0x14, 0x01, 0x2c, 0x35}};
static const GUID IID_IContentDialogStatics     = {0x23ab47a6, 0x508a, 0x4f33, {0xb7, 0x86, 0xf2, 0xfa, 0x96, 0xd8, 0x69, 0xaf}};
static const GUID IID_IContentDialogStatics2    = {0x329825b2, 0x8fb0, 0x4125, {0xbb, 0x07, 0xe7, 0x9a, 0x5a, 0x03, 0x81, 0x9f}};
static const GUID IID_IAsyncInfo_local          = {0x00000036, 0x0000, 0x0000, {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};

struct winrt_grid_length
{
    DOUBLE value;
    INT32 grid_unit_type; /* 0 = Auto, 1 = Pixel, 2 = Star */
};

struct xaml_style;

enum xaml_control_type
{
    CONTROL_TYPE_ELEMENT,
    CONTROL_TYPE_GRID,
    CONTROL_TYPE_CANVAS,
    CONTROL_TYPE_SWAPCHAINPANEL,
    CONTROL_TYPE_STACKPANEL,
    CONTROL_TYPE_BORDER,
    CONTROL_TYPE_BUTTON,
    CONTROL_TYPE_TEXTBLOCK,
    CONTROL_TYPE_CONTENTCONTROL,
    CONTROL_TYPE_USERCONTROL,
    CONTROL_TYPE_PAGE,
    CONTROL_TYPE_ROWDEF,
    CONTROL_TYPE_COLDEF,
    CONTROL_TYPE_FLYOUT,
    CONTROL_TYPE_IMAGE,
    CONTROL_TYPE_PIVOT,
    CONTROL_TYPE_PIVOTITEM,
    CONTROL_TYPE_MENUFLYOUT,
    CONTROL_TYPE_MENUFLYOUTITEM,
    CONTROL_TYPE_ADAPTIVETRIGGER,
    CONTROL_TYPE_CONTENTDIALOG
};

struct xaml_element
{
    /* Interfaces */
    struct { const void *lpVtbl; } IUIElement_iface;
    struct { const void *lpVtbl; } IFrameworkElement_iface;
    struct { const void *lpVtbl; } IControl_iface;
    struct { const void *lpVtbl; } IControlProtected_iface;
    struct { const void *lpVtbl; } IContentControl_iface;
    struct { const void *lpVtbl; } IUserControl_iface;
    struct { const void *lpVtbl; } IPanel_iface;
    struct { const void *lpVtbl; } IGrid_iface;
    struct { const void *lpVtbl; } ICanvas_iface;
    struct { const void *lpVtbl; } ISwapChainPanel_iface;
    struct { const void *lpVtbl; } ISwapChainPanelNative_iface;
    struct { const void *lpVtbl; } IBorder_iface;
    struct { const void *lpVtbl; } IButton_iface;
    struct { const void *lpVtbl; } IButtonBase_iface;
    struct { const void *lpVtbl; } ITextBlock_iface;
    struct { const void *lpVtbl; } IPage_iface;
    struct { const void *lpVtbl; } IDependencyObject_iface;
    struct { const void *lpVtbl; } IDependencyObject2_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    struct { const void *lpVtbl; } IFlyout_iface;
    struct { const void *lpVtbl; } IFlyoutBase_iface;
    struct { const void *lpVtbl; } IMenuFlyout_iface;
    struct { const void *lpVtbl; } IMenuFlyout2_iface;
    struct { const void *lpVtbl; } IMenuFlyoutItem_iface;
    struct { const void *lpVtbl; } IMenuFlyoutItem2_iface;
    struct { const void *lpVtbl; } IMenuFlyoutItemBase_iface;
    struct { const void *lpVtbl; } IRowDefinition_iface;
    struct { const void *lpVtbl; } IColumnDefinition_iface;
    struct { const void *lpVtbl; } IWeakReferenceSource_iface;
    struct { const void *lpVtbl; } IAdaptiveTrigger_iface;
    struct { const void *lpVtbl; } IContentDialog_iface;
    struct { const void *lpVtbl; } IContentDialog2_iface;

    LONG ref;
    enum xaml_control_type type;

    /* Properties */
    WCHAR name[64];
    WCHAR custom_class_name[128];
    WCHAR automation_id[64];
    WCHAR automation_name[128];
    INT32 grid_row;
    INT32 grid_column;
    INT32 grid_rowspan;
    INT32 grid_colspan;
    DOUBLE width;
    DOUBLE height;
    INT32 visibility; /* 0 = Visible, 1 = Collapsed */
    boolean is_enabled;

    /* Computed layout bounds */
    RECT bounds;
    boolean is_pressed;
    boolean is_hovered;

    /* Content & Styling */
    WCHAR text[256];
    DOUBLE font_size;
    COLORREF foreground_color;
    COLORREF background_color;
    COLORREF border_color;
    INT32 border_thickness;

    /* Child for Border / ContentControl */
    struct xaml_element *child_content;
    void *swapchain;

    /* Children for Panel / Grid */
    struct xaml_element **children;
    UINT32 children_count;
    UINT32 children_capacity;

    /* Grid Row/Column definitions */
    DOUBLE row_heights[16];
    INT32 row_unit_types[16]; /* 0 = Auto, 1 = Pixel, 2 = Star */
    UINT32 row_count;

    DOUBLE col_widths[16];
    INT32 col_unit_types[16];
    UINT32 col_count;

    /* AdaptiveTrigger */
    DOUBLE min_window_width;
    DOUBLE min_window_height;

    /* Events */
    void *click_handler;
    void *loaded_handler;
    void *size_changed_handler;
    void *command;
    void *command_param;

#define MAX_ELEM_HANDLERS 4
    /* Pointer & Input Events */
    void *pointer_pressed_handlers[MAX_ELEM_HANDLERS];
    UINT pointer_pressed_count;
    void *pointer_released_handlers[MAX_ELEM_HANDLERS];
    UINT pointer_released_count;
    void *pointer_moved_handlers[MAX_ELEM_HANDLERS];
    UINT pointer_moved_count;
    void *pointer_wheel_handlers[MAX_ELEM_HANDLERS];
    UINT pointer_wheel_count;
    void *pointer_entered_handlers[MAX_ELEM_HANDLERS];
    UINT pointer_entered_count;
    void *pointer_exited_handlers[MAX_ELEM_HANDLERS];
    UINT pointer_exited_count;
    void *pointer_capture_lost_handlers[MAX_ELEM_HANDLERS];
    UINT pointer_capture_lost_count;
    void *pointer_canceled_handlers[MAX_ELEM_HANDLERS];
    UINT pointer_canceled_count;
    void *tapped_handlers[MAX_ELEM_HANDLERS];
    UINT tapped_count;
    void *double_tapped_handlers[MAX_ELEM_HANDLERS];
    UINT double_tapped_count;
    void *holding_handlers[MAX_ELEM_HANDLERS];
    UINT holding_count;
    void *right_tapped_handlers[MAX_ELEM_HANDLERS];
    UINT right_tapped_count;
    void *keydown_handlers[MAX_ELEM_HANDLERS];
    UINT keydown_count;
    void *keyup_handlers[MAX_ELEM_HANDLERS];
    UINT keyup_count;
    void *got_focus_handlers[MAX_ELEM_HANDLERS];
    UINT got_focus_count;
    void *lost_focus_handlers[MAX_ELEM_HANDLERS];
    UINT lost_focus_count;

    void *resources;
    void *data_context;
    struct xaml_element *parent;

    void *outer;
    struct xaml_style *flyout_style;

    struct dp_entry dp_entries[64];
    UINT32 dp_count;
};

#define impl_from_IUIElement(ptr)             ((struct xaml_element *)((char *)(ptr) - offsetof(struct xaml_element, IUIElement_iface)))
#define impl_from_IFrameworkElement(ptr)      ((struct xaml_element *)((char *)(ptr) - offsetof(struct xaml_element, IFrameworkElement_iface)))
#define impl_from_IControl(ptr)               ((struct xaml_element *)((char *)(ptr) - offsetof(struct xaml_element, IControl_iface)))
#define impl_from_IControlProtected(ptr)      ((struct xaml_element *)((char *)(ptr) - offsetof(struct xaml_element, IControlProtected_iface)))
#define impl_from_IContentControl(ptr)        ((struct xaml_element *)((char *)(ptr) - offsetof(struct xaml_element, IContentControl_iface)))
#define impl_from_IUserControl(ptr)           ((struct xaml_element *)((char *)(ptr) - offsetof(struct xaml_element, IUserControl_iface)))
#define impl_from_IPanel(ptr)                 ((struct xaml_element *)((char *)(ptr) - offsetof(struct xaml_element, IPanel_iface)))
#define impl_from_IGrid(ptr)                  ((struct xaml_element *)((char *)(ptr) - offsetof(struct xaml_element, IGrid_iface)))
#define impl_from_ICanvas(ptr)                ((struct xaml_element *)((char *)(ptr) - offsetof(struct xaml_element, ICanvas_iface)))
#define impl_from_ISwapChainPanel(ptr)        ((struct xaml_element *)((char *)(ptr) - offsetof(struct xaml_element, ISwapChainPanel_iface)))
#define impl_from_ISwapChainPanelNative(ptr)  ((struct xaml_element *)((char *)(ptr) - offsetof(struct xaml_element, ISwapChainPanelNative_iface)))
#define impl_from_IBorder(ptr)                ((struct xaml_element *)((char *)(ptr) - offsetof(struct xaml_element, IBorder_iface)))
#define impl_from_IButton(ptr)                ((struct xaml_element *)((char *)(ptr) - offsetof(struct xaml_element, IButton_iface)))
#define impl_from_IButtonBase(ptr)            ((struct xaml_element *)((char *)(ptr) - offsetof(struct xaml_element, IButtonBase_iface)))
#define impl_from_ITextBlock(ptr)             ((struct xaml_element *)((char *)(ptr) - offsetof(struct xaml_element, ITextBlock_iface)))
#define impl_from_IPage(ptr)                  ((struct xaml_element *)((char *)(ptr) - offsetof(struct xaml_element, IPage_iface)))
#define impl_from_IDependencyObject(ptr)      ((struct xaml_element *)((char *)(ptr) - offsetof(struct xaml_element, IDependencyObject_iface)))
#define impl_from_IDependencyObject2(ptr)     ((struct xaml_element *)((char *)(ptr) - offsetof(struct xaml_element, IDependencyObject2_iface)))
#define impl_from_IRedhawkInterop(ptr)        ((struct xaml_element *)((char *)(ptr) - offsetof(struct xaml_element, IRedhawkInterop_iface)))
#define impl_from_IFlyout(ptr)                ((struct xaml_element *)((char *)(ptr) - offsetof(struct xaml_element, IFlyout_iface)))
#define impl_from_IFlyoutBase(ptr)            ((struct xaml_element *)((char *)(ptr) - offsetof(struct xaml_element, IFlyoutBase_iface)))
#define impl_from_IMenuFlyout(ptr)            ((struct xaml_element *)((char *)(ptr) - offsetof(struct xaml_element, IMenuFlyout_iface)))
#define impl_from_IMenuFlyout2(ptr)           ((struct xaml_element *)((char *)(ptr) - offsetof(struct xaml_element, IMenuFlyout2_iface)))
#define impl_from_IMenuFlyoutItem(ptr)        ((struct xaml_element *)((char *)(ptr) - offsetof(struct xaml_element, IMenuFlyoutItem_iface)))
#define impl_from_IMenuFlyoutItem2(ptr)       ((struct xaml_element *)((char *)(ptr) - offsetof(struct xaml_element, IMenuFlyoutItem2_iface)))
#define impl_from_IMenuFlyoutItemBase(ptr)    ((struct xaml_element *)((char *)(ptr) - offsetof(struct xaml_element, IMenuFlyoutItemBase_iface)))
#define impl_from_IRowDefinition(ptr)         ((struct xaml_element *)((char *)(ptr) - offsetof(struct xaml_element, IRowDefinition_iface)))
#define impl_from_IColumnDefinition(ptr)      ((struct xaml_element *)((char *)(ptr) - offsetof(struct xaml_element, IColumnDefinition_iface)))
#define impl_from_elem_IWRS(ptr)              ((struct xaml_element *)((char *)(ptr) - offsetof(struct xaml_element, IWeakReferenceSource_iface)))
#define impl_from_IContentDialog(ptr)         ((struct xaml_element *)((char *)(ptr) - offsetof(struct xaml_element, IContentDialog_iface)))
#define impl_from_IContentDialog2(ptr)        ((struct xaml_element *)((char *)(ptr) - offsetof(struct xaml_element, IContentDialog2_iface)))

static HRESULT elem_QI_internal(struct xaml_element *impl, REFIID riid, void **ppvObject);
static ULONG elem_AddRef_internal(struct xaml_element *impl);
static ULONG elem_Release_internal(struct xaml_element *impl);
static struct xaml_style *xaml_style_create(void);
static void xaml_style_release(struct xaml_style *s);

/* Forward declarations */
static const void *elem_ui_vtbl[128];
static const void *elem_fe_vtbl[64];
static const void *elem_ctrl_vtbl[64];
static const void *elem_ctrl_prot_vtbl[16];
static const void *elem_cc_vtbl[32];
static const void *elem_uc_vtbl[32];
static const void *elem_page_vtbl[16];
static const void *elem_panel_vtbl[32];
static const void *elem_grid_vtbl[32];
static const void *elem_canvas_vtbl[16];
static const void *elem_scp_vtbl[16];
static const void *elem_scp_native_vtbl[8];
static const void *elem_border_vtbl[32];
static const void *elem_btn_vtbl[32];
static const void *elem_btnbase_vtbl[32];
static const void *elem_tb_vtbl[32];
static const void *elem_do_vtbl[32];
static const void *elem_do2_vtbl[16];
static const void *elem_redhawk_vtbl[32];
static const void *elem_flyout_vtbl[32];
static const void *elem_flyout_base_vtbl[32];
static const void *elem_menuflyout_vtbl[32];
static const void *elem_menuflyout2_vtbl[32];
static const void *elem_menuflyoutitem_vtbl[32];
static const void *elem_menuflyoutitem2_vtbl[16];
static const void *elem_menuflyoutitembase_vtbl[16];
static const void *elem_rowdef_vtbl[16];
static const void *elem_coldef_vtbl[16];
static const void *elem_wrs_vtbl[4];
static const void *elem_adapt_vtbl[16];
static const void *elem_cd_vtbl[64];
static const void *elem_cd2_vtbl[32];
static void init_elem_vtables(void);

/* Global active root visual tree */
static struct xaml_element *global_root_element = NULL;
static struct xaml_element *global_swapchain_panel = NULL;
static struct xaml_element *global_page_element = NULL;
static void *global_page_component = NULL;
HWND global_xaml_hwnd = NULL;
static BOOL global_swapchain_active = FALSE;

static BOOL is_calculator_app(void)
{
    static int cached = -1;
    if (cached == -1)
    {
        WCHAR modpath[MAX_PATH];
        if (GetModuleFileNameW(NULL, modpath, MAX_PATH))
        {
            WCHAR *p = wcsrchr(modpath, L'\\');
            if (!p) p = modpath;
            else p++;
            cached = (wcsicmp(p, L"CalculatorApp.exe") == 0 ||
                      wcsicmp(p, L"Calculator.exe") == 0 ||
                      wcsstr(modpath, L"Calculator") != NULL);
        }
        else cached = 0;
    }
    return cached != 0;
}

/* -------------------------------------------------------------------------
 * Element Constructor
 * ------------------------------------------------------------------------- */
static struct xaml_element *xaml_element_create(enum xaml_control_type type, const WCHAR *name)
{
    struct xaml_element *elem;
    init_elem_vtables();
    elem = calloc(1, sizeof(*elem));
    if (!elem) return NULL;

    elem->IUIElement_iface.lpVtbl        = elem_ui_vtbl;
    elem->IFrameworkElement_iface.lpVtbl = elem_fe_vtbl;
    elem->IControl_iface.lpVtbl          = elem_ctrl_vtbl;
    elem->IControlProtected_iface.lpVtbl = elem_ctrl_prot_vtbl;
    elem->IContentControl_iface.lpVtbl   = elem_cc_vtbl;
    elem->IUserControl_iface.lpVtbl      = elem_uc_vtbl;
    elem->IPanel_iface.lpVtbl            = elem_panel_vtbl;
    elem->IGrid_iface.lpVtbl             = elem_grid_vtbl;
    elem->ICanvas_iface.lpVtbl           = elem_canvas_vtbl;
    elem->ISwapChainPanel_iface.lpVtbl   = elem_scp_vtbl;
    elem->ISwapChainPanelNative_iface.lpVtbl = elem_scp_native_vtbl;
    elem->IBorder_iface.lpVtbl           = elem_border_vtbl;
    elem->IButton_iface.lpVtbl           = elem_btn_vtbl;
    elem->IButtonBase_iface.lpVtbl       = elem_btnbase_vtbl;
    elem->ITextBlock_iface.lpVtbl        = elem_tb_vtbl;
    elem->IPage_iface.lpVtbl             = elem_page_vtbl;
    elem->IDependencyObject_iface.lpVtbl = elem_do_vtbl;
    elem->IDependencyObject2_iface.lpVtbl = elem_do2_vtbl;
    elem->IRedhawkInterop_iface.lpVtbl   = elem_redhawk_vtbl;
    elem->IFlyout_iface.lpVtbl           = elem_flyout_vtbl;
    elem->IFlyoutBase_iface.lpVtbl       = elem_flyout_base_vtbl;
    elem->IMenuFlyout_iface.lpVtbl       = elem_menuflyout_vtbl;
    elem->IMenuFlyout2_iface.lpVtbl      = elem_menuflyout2_vtbl;
    elem->IMenuFlyoutItem_iface.lpVtbl   = elem_menuflyoutitem_vtbl;
    elem->IMenuFlyoutItem2_iface.lpVtbl  = elem_menuflyoutitem2_vtbl;
    elem->IMenuFlyoutItemBase_iface.lpVtbl = elem_menuflyoutitembase_vtbl;
    elem->IRowDefinition_iface.lpVtbl   = elem_rowdef_vtbl;
    elem->IColumnDefinition_iface.lpVtbl = elem_coldef_vtbl;
    elem->IWeakReferenceSource_iface.lpVtbl = elem_wrs_vtbl;
    elem->IAdaptiveTrigger_iface.lpVtbl = elem_adapt_vtbl;
    elem->IContentDialog_iface.lpVtbl   = elem_cd_vtbl;
    elem->IContentDialog2_iface.lpVtbl  = elem_cd2_vtbl;

    elem->ref = 1;
    elem->type = type;
    elem->is_enabled = TRUE;
    elem->grid_rowspan = 1;
    elem->grid_colspan = 1;
    elem->foreground_color = RGB(255, 255, 255);
    elem->background_color = RGB(45, 45, 45);
    elem->border_color     = RGB(58, 58, 58);
    elem->border_thickness = 1;
    elem->font_size = 14.0;

    if (name) lstrcpynW(elem->name, name, ARRAY_SIZE(elem->name));
    return elem;
}

static void xaml_element_add_child(struct xaml_element *parent, struct xaml_element *child)
{
    if (!parent || !child) return;
    if (parent->children_count >= parent->children_capacity)
    {
        UINT32 new_cap = parent->children_capacity ? parent->children_capacity * 2 : 16;
        struct xaml_element **new_arr = realloc(parent->children, new_cap * sizeof(*new_arr));
        if (!new_arr) return;
        parent->children = new_arr;
        parent->children_capacity = new_cap;
    }
    child->parent = parent;
    parent->children[parent->children_count++] = child;
    elem_AddRef_internal(child);
}

/* -------------------------------------------------------------------------
 * IUnknown & IInspectable Methods
 * ------------------------------------------------------------------------- */
static void *elem_get_iunknown(struct xaml_element *impl)
{
    switch (impl->type)
    {
    case CONTROL_TYPE_ROWDEF:
        return &impl->IRowDefinition_iface;
    case CONTROL_TYPE_COLDEF:
        return &impl->IColumnDefinition_iface;
    case CONTROL_TYPE_MENUFLYOUT:
        return &impl->IMenuFlyout_iface;
    case CONTROL_TYPE_MENUFLYOUTITEM:
        return &impl->IMenuFlyoutItem_iface;
    case CONTROL_TYPE_FLYOUT:
        return &impl->IFlyout_iface;
    case CONTROL_TYPE_ADAPTIVETRIGGER:
        return &impl->IAdaptiveTrigger_iface;
    default:
        return &impl->IUIElement_iface;
    }
}

static BOOL elem_is_uielement(struct xaml_element *impl)
{
    return (impl->type != CONTROL_TYPE_ROWDEF &&
            impl->type != CONTROL_TYPE_COLDEF &&
            impl->type != CONTROL_TYPE_MENUFLYOUT &&
            impl->type != CONTROL_TYPE_FLYOUT &&
            impl->type != CONTROL_TYPE_ADAPTIVETRIGGER);
}

static HRESULT elem_QI_internal(struct xaml_element *impl, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("elem %p (%s, type=%d) QI riid %s\n", impl, debugstr_w(impl->name), impl->type, debugstr_guid(riid));

    if (IsEqualGUID(riid, &IID_IUnknown))
    {
        *ppvObject = elem_get_iunknown(impl);
        elem_AddRef_internal(impl);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject_local))
    {
        *ppvObject = elem_get_iunknown(impl);
        elem_AddRef_internal(impl);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IUIElement) ||
        IsEqualGUID(riid, &IID_IUIElement2) ||
        IsEqualGUID(riid, &IID_IUIElement3) ||
        IsEqualGUID(riid, &IID_IUIElement4) ||
        IsEqualGUID(riid, &IID_IUIElement5) ||
        IsEqualGUID(riid, &IID_IUIElement7) ||
        IsEqualGUID(riid, &IID_IUIElementOverrides) ||
        IsEqualGUID(riid, &IID_IUIElementOverrides7))
    {
        if (!elem_is_uielement(impl))
            return E_NOINTERFACE;
        *ppvObject = &impl->IUIElement_iface;
        elem_AddRef_internal(impl);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IFrameworkElement) ||
        IsEqualGUID(riid, &IID_IFrameworkElement2) ||
        IsEqualGUID(riid, &IID_IFrameworkElement3) ||
        IsEqualGUID(riid, &IID_IFrameworkElement4) ||
        IsEqualGUID(riid, &IID_IFrameworkElement6) ||
        IsEqualGUID(riid, &IID_IFrameworkElementOverrides) ||
        IsEqualGUID(riid, &IID_IFrameworkElementOverrides2))
    {
        if (!elem_is_uielement(impl))
            return E_NOINTERFACE;
        *ppvObject = &impl->IFrameworkElement_iface;
        elem_AddRef_internal(impl);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IControl) ||
        IsEqualGUID(riid, &IID_IControl_sdk) ||
        IsEqualGUID(riid, &IID_IControl2) ||
        IsEqualGUID(riid, &IID_IControl3) ||
        IsEqualGUID(riid, &IID_IControl4) ||
        IsEqualGUID(riid, &IID_IControl5) ||
        IsEqualGUID(riid, &IID_IControlOverrides) ||
        IsEqualGUID(riid, &IID_IControlOverrides6) ||
        IsEqualGUID(riid, &IID_INavigationView_local) ||
        IsEqualGUID(riid, &IID_INavigationView2_local))
    {
        if (!elem_is_uielement(impl))
            return E_NOINTERFACE;
        *ppvObject = &impl->IControl_iface;
        elem_AddRef_internal(impl);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IControlProtected))
    {
        if (!elem_is_uielement(impl))
            return E_NOINTERFACE;
        *ppvObject = &impl->IControlProtected_iface;
        elem_AddRef_internal(impl);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IContentControl) ||
        IsEqualGUID(riid, &IID_IContentControl2) ||
        IsEqualGUID(riid, &IID_IContentControlOverrides))
    {
        if (!elem_is_uielement(impl))
            return E_NOINTERFACE;
        *ppvObject = &impl->IContentControl_iface;
        elem_AddRef_internal(impl);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IContentDialog))
    {
        if (!elem_is_uielement(impl))
            return E_NOINTERFACE;
        *ppvObject = &impl->IContentDialog_iface;
        elem_AddRef_internal(impl);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IContentDialog2))
    {
        if (!elem_is_uielement(impl))
            return E_NOINTERFACE;
        *ppvObject = &impl->IContentDialog2_iface;
        elem_AddRef_internal(impl);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IUserControl))
    {
        if (!elem_is_uielement(impl))
            return E_NOINTERFACE;
        *ppvObject = &impl->IUserControl_iface;
        elem_AddRef_internal(impl);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IPage) ||
        IsEqualGUID(riid, &IID_IPageOverrides))
    {
        if (!elem_is_uielement(impl))
            return E_NOINTERFACE;
        *ppvObject = &impl->IPage_iface;
        elem_AddRef_internal(impl);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IPanel))
    {
        if (!elem_is_uielement(impl))
            return E_NOINTERFACE;
        *ppvObject = &impl->IPanel_iface;
        elem_AddRef_internal(impl);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IGrid))
    {
        if (!elem_is_uielement(impl))
            return E_NOINTERFACE;
        *ppvObject = &impl->IGrid_iface;
        elem_AddRef_internal(impl);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ICanvas))
    {
        if (impl->type != CONTROL_TYPE_CANVAS)
            return E_NOINTERFACE;
        *ppvObject = &impl->ICanvas_iface;
        elem_AddRef_internal(impl);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ISwapChainPanel))
    {
        if (impl->type != CONTROL_TYPE_SWAPCHAINPANEL)
            return E_NOINTERFACE;
        *ppvObject = &impl->ISwapChainPanel_iface;
        elem_AddRef_internal(impl);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ISwapChainPanelNative) ||
        IsEqualGUID(riid, &IID_ISwapChainPanelNative2))
    {
        if (impl->type != CONTROL_TYPE_SWAPCHAINPANEL)
            return E_NOINTERFACE;
        *ppvObject = &impl->ISwapChainPanelNative_iface;
        elem_AddRef_internal(impl);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IBorder))
    {
        if (!elem_is_uielement(impl))
            return E_NOINTERFACE;
        *ppvObject = &impl->IBorder_iface;
        elem_AddRef_internal(impl);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IButton))
    {
        if (!elem_is_uielement(impl))
            return E_NOINTERFACE;
        *ppvObject = &impl->IButton_iface;
        elem_AddRef_internal(impl);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IButtonBase))
    {
        if (!elem_is_uielement(impl))
            return E_NOINTERFACE;
        *ppvObject = &impl->IButtonBase_iface;
        elem_AddRef_internal(impl);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ITextBlock))
    {
        if (!elem_is_uielement(impl))
            return E_NOINTERFACE;
        *ppvObject = &impl->ITextBlock_iface;
        elem_AddRef_internal(impl);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IDependencyObject_local))
    {
        *ppvObject = &impl->IDependencyObject_iface;
        elem_AddRef_internal(impl);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IDependencyObject2_local))
    {
        *ppvObject = &impl->IDependencyObject2_iface;
        elem_AddRef_internal(impl);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ComWrappers_internal) ||
        IsEqualGUID(riid, &IID_ComWrappers_internal2) ||
        IsEqualGUID(riid, &IID_ITrackerOwner))
    {
        TRACE("elem %p (%s, type=%d) returning IRedhawkInterop_iface\n", impl, debugstr_w(impl->name), impl->type);
        *ppvObject = &impl->IRedhawkInterop_iface;
        elem_AddRef_internal(impl);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IFlyout))
    {
        if (impl->type != CONTROL_TYPE_FLYOUT)
            return E_NOINTERFACE;
        *ppvObject = &impl->IFlyout_iface;
        elem_AddRef_internal(impl);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IFlyoutBase) ||
        IsEqualGUID(riid, &IID_IFlyoutBase2))
    {
        if (impl->type != CONTROL_TYPE_FLYOUT && impl->type != CONTROL_TYPE_MENUFLYOUT)
            return E_NOINTERFACE;
        *ppvObject = &impl->IFlyoutBase_iface;
        elem_AddRef_internal(impl);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IMenuFlyout))
    {
        if (impl->type != CONTROL_TYPE_MENUFLYOUT)
            return E_NOINTERFACE;
        *ppvObject = &impl->IMenuFlyout_iface;
        elem_AddRef_internal(impl);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IMenuFlyout2))
    {
        if (impl->type != CONTROL_TYPE_MENUFLYOUT)
            return E_NOINTERFACE;
        *ppvObject = &impl->IMenuFlyout2_iface;
        elem_AddRef_internal(impl);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IMenuFlyoutItem))
    {
        if (impl->type != CONTROL_TYPE_MENUFLYOUTITEM)
            return E_NOINTERFACE;
        *ppvObject = &impl->IMenuFlyoutItem_iface;
        elem_AddRef_internal(impl);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IMenuFlyoutItem2))
    {
        if (impl->type != CONTROL_TYPE_MENUFLYOUTITEM)
            return E_NOINTERFACE;
        *ppvObject = &impl->IMenuFlyoutItem2_iface;
        elem_AddRef_internal(impl);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IMenuFlyoutItemBase))
    {
        if (impl->type != CONTROL_TYPE_MENUFLYOUTITEM)
            return E_NOINTERFACE;
        *ppvObject = &impl->IMenuFlyoutItemBase_iface;
        elem_AddRef_internal(impl);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IRowDefinition))
    {
        if (impl->type != CONTROL_TYPE_ROWDEF)
            return E_NOINTERFACE;
        *ppvObject = &impl->IRowDefinition_iface;
        elem_AddRef_internal(impl);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IColumnDefinition))
    {
        if (impl->type != CONTROL_TYPE_COLDEF)
            return E_NOINTERFACE;
        *ppvObject = &impl->IColumnDefinition_iface;
        elem_AddRef_internal(impl);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IAdaptiveTrigger) ||
        IsEqualGUID(riid, &IID_IStateTriggerBase))
    {
        if (impl->type != CONTROL_TYPE_ADAPTIVETRIGGER)
            return E_NOINTERFACE;
        *ppvObject = &impl->IAdaptiveTrigger_iface;
        elem_AddRef_internal(impl);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IWeakReferenceSource))
    {
        *ppvObject = &impl->IWeakReferenceSource_iface;
        elem_AddRef_internal(impl);
        return S_OK;
    }

    /* Prevent ping-pong recursion if outer is queried */
    if (impl->outer && (ULONG_PTR)impl->outer > 0x10000)
    {
        if (IsEqualGUID(riid, &IID_IComponentConnector) ||
            IsEqualGUID(riid, &IID_IComponentConnector2))
        {
            return E_NOINTERFACE;
        }
    }

    return E_NOINTERFACE;
}

static ULONG elem_AddRef_internal(struct xaml_element *impl)
{
    return InterlockedIncrement(&impl->ref);
}

static ULONG elem_Release_internal(struct xaml_element *impl)
{
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref)
    {
        UINT32 i;
        if (impl->child_content) elem_Release_internal(impl->child_content);
        if (impl->flyout_style) xaml_style_release(impl->flyout_style);
        if (impl->resources) IUnknown_Release((IUnknown *)impl->resources);
        if (impl->data_context) IUnknown_Release((IUnknown *)impl->data_context);
        if (impl->size_changed_handler) IUnknown_Release((IUnknown *)impl->size_changed_handler);
        for (i = 0; i < impl->dp_count; i++)
        {
            if (impl->dp_entries[i].value) IInspectable_Release(impl->dp_entries[i].value);
        }
        for (i = 0; i < impl->children_count; i++)
        {
            if (impl->children[i]) elem_Release_internal(impl->children[i]);
        }
        free(impl->children);
        free(impl);
    }
    return ref;
}

static HRESULT WINAPI elem_stub_ret_ok(void *This) { return S_OK; }
static HRESULT WINAPI elem_stub_ret_zero(void *This, void *val) { if (val) *(ULONG_PTR *)val = 0; return S_OK; }

/* -------------------------------------------------------------------------
 * IUIElement VTable
 * ------------------------------------------------------------------------- */
static HRESULT WINAPI elem_ui_QI(void *This, REFIID riid, void **ppv) { return elem_QI_internal(impl_from_IUIElement(This), riid, ppv); }
static ULONG WINAPI elem_ui_AddRef(void *This) { return elem_AddRef_internal(impl_from_IUIElement(This)); }
static ULONG WINAPI elem_ui_Release(void *This) { return elem_Release_internal(impl_from_IUIElement(This)); }
static HRESULT WINAPI elem_ui_GetIids(void *This, ULONG *c, IID **i)
{
    if (i) *i = NULL;
    if (c) *c = 0;
    return S_OK;
}
static HRESULT WINAPI elem_ui_GetRuntimeClassName(void *This, HSTRING *n)
{
    struct xaml_element *impl = impl_from_IUIElement(This);
    const WCHAR *name = L"Windows.UI.Xaml.UIElement";
    if (impl->custom_class_name[0])
    {
        name = impl->custom_class_name;
    }
    else
    {
        switch (impl->type)
        {
        case CONTROL_TYPE_GRID:           name = L"Windows.UI.Xaml.Controls.Grid"; break;
        case CONTROL_TYPE_CANVAS:         name = L"Windows.UI.Xaml.Controls.Canvas"; break;
        case CONTROL_TYPE_BORDER:         name = L"Windows.UI.Xaml.Controls.Border"; break;
        case CONTROL_TYPE_BUTTON:         name = L"Windows.UI.Xaml.Controls.Button"; break;
        case CONTROL_TYPE_TEXTBLOCK:      name = L"Windows.UI.Xaml.Controls.TextBlock"; break;
        case CONTROL_TYPE_USERCONTROL:    name = L"Windows.UI.Xaml.Controls.UserControl"; break;
        case CONTROL_TYPE_CONTENTCONTROL: name = L"Windows.UI.Xaml.Controls.ContentControl"; break;
        case CONTROL_TYPE_PAGE:           name = L"Windows.UI.Xaml.Controls.Page"; break;
        case CONTROL_TYPE_ROWDEF:         name = L"Windows.UI.Xaml.Controls.RowDefinition"; break;
        case CONTROL_TYPE_COLDEF:         name = L"Windows.UI.Xaml.Controls.ColumnDefinition"; break;
        case CONTROL_TYPE_FLYOUT:         name = L"Windows.UI.Xaml.Controls.Flyout"; break;
        case CONTROL_TYPE_IMAGE:          name = L"Windows.UI.Xaml.Controls.Image"; break;
        case CONTROL_TYPE_PIVOT:          name = L"Windows.UI.Xaml.Controls.Pivot"; break;
        case CONTROL_TYPE_PIVOTITEM:      name = L"Windows.UI.Xaml.Controls.PivotItem"; break;
        case CONTROL_TYPE_MENUFLYOUT:     name = L"Windows.UI.Xaml.Controls.MenuFlyout"; break;
        case CONTROL_TYPE_MENUFLYOUTITEM: name = L"Windows.UI.Xaml.Controls.MenuFlyoutItem"; break;
        default: break;
        }
    }
    TRACE("elem %p (%s) GetRuntimeClassName -> %s\n", impl, debugstr_w(impl->name), debugstr_w(name));
    return WindowsCreateString(name, lstrlenW(name), n);
}
static HRESULT WINAPI elem_ui_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI elem_page_QI(void *This, REFIID riid, void **ppv) { return elem_QI_internal(impl_from_IPage(This), riid, ppv); }
static ULONG WINAPI elem_page_AddRef(void *This) { return elem_AddRef_internal(impl_from_IPage(This)); }
static ULONG WINAPI elem_page_Release(void *This) { return elem_Release_internal(impl_from_IPage(This)); }

static HRESULT WINAPI elem_ui_get_Visibility(void *This, INT32 *value)
{
    struct xaml_element *impl = impl_from_IUIElement(This);
    if (value) *value = impl->visibility;
    return S_OK;
}
static HRESULT WINAPI elem_ui_put_Visibility(void *This, INT32 value)
{
    struct xaml_element *impl = impl_from_IUIElement(This);
    impl->visibility = value;
    if (global_xaml_hwnd) InvalidateRect(global_xaml_hwnd, NULL, FALSE);
    return S_OK;
}

static INT64 next_event_token = 1;

/* IPointerPointProperties implementation */
struct elem_pointer_props_impl
{
    IInspectable IInspectable_iface;
    LONG ref;
    FLOAT x, y;
    boolean left_pressed;
    boolean right_pressed;
    boolean middle_pressed;
    boolean in_contact;
    UINT32 update_kind;
    INT32 wheel_delta;
};

static inline struct elem_pointer_props_impl *impl_from_elem_IPointerPointProperties(void *iface)
{
    return (struct elem_pointer_props_impl *)iface;
}

static HRESULT WINAPI eprops_QI(void *This, REFIID riid, void **ppv)
{
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IPointerPointProperties) ||
        IsEqualGUID(riid, &IID_IAgileObject))
    {
        *ppv = This;
        InterlockedIncrement(&impl_from_elem_IPointerPointProperties(This)->ref);
        return S_OK;
    }
    *ppv = NULL;
    return E_NOINTERFACE;
}
static ULONG WINAPI eprops_AddRef(void *This) { return InterlockedIncrement(&impl_from_elem_IPointerPointProperties(This)->ref); }
static ULONG WINAPI eprops_Release(void *This)
{
    struct elem_pointer_props_impl *impl = impl_from_elem_IPointerPointProperties(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}
static HRESULT WINAPI eprops_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI eprops_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Input.PointerPointProperties", 39, n); }
static HRESULT WINAPI eprops_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI eprops_get_Pressure(void *This, FLOAT *val) { if (val) *val = 0.5f; return S_OK; }
static HRESULT WINAPI eprops_get_IsInverted(void *This, boolean *val) { if (val) *val = FALSE; return S_OK; }
static HRESULT WINAPI eprops_get_IsEraser(void *This, boolean *val) { if (val) *val = FALSE; return S_OK; }
static HRESULT WINAPI eprops_get_Orientation(void *This, FLOAT *val) { if (val) *val = 0.0f; return S_OK; }
static HRESULT WINAPI eprops_get_XTilt(void *This, FLOAT *val) { if (val) *val = 0.0f; return S_OK; }
static HRESULT WINAPI eprops_get_YTilt(void *This, FLOAT *val) { if (val) *val = 0.0f; return S_OK; }
static HRESULT WINAPI eprops_get_Twist(void *This, FLOAT *val) { if (val) *val = 0.0f; return S_OK; }
static HRESULT WINAPI eprops_get_ContactRect(void *This, void *val) { return S_OK; }
static HRESULT WINAPI eprops_get_ContactRectRaw(void *This, void *val) { return S_OK; }
static HRESULT WINAPI eprops_get_TouchConfidence(void *This, boolean *val) { if (val) *val = TRUE; return S_OK; }
static HRESULT WINAPI eprops_get_IsLeftButtonPressed(void *This, boolean *val)
{
    if (val) *val = impl_from_elem_IPointerPointProperties(This)->left_pressed;
    return S_OK;
}
static HRESULT WINAPI eprops_get_IsRightButtonPressed(void *This, boolean *val)
{
    if (val) *val = impl_from_elem_IPointerPointProperties(This)->right_pressed;
    return S_OK;
}
static HRESULT WINAPI eprops_get_IsMiddleButtonPressed(void *This, boolean *val)
{
    if (val) *val = impl_from_elem_IPointerPointProperties(This)->middle_pressed;
    return S_OK;
}
static HRESULT WINAPI eprops_get_MouseWheelDelta(void *This, INT32 *val)
{
    if (val) *val = impl_from_elem_IPointerPointProperties(This)->wheel_delta;
    return S_OK;
}
static HRESULT WINAPI eprops_get_IsHorizontalMouseWheel(void *This, boolean *val) { if (val) *val = FALSE; return S_OK; }
static HRESULT WINAPI eprops_get_IsPrimary(void *This, boolean *val) { if (val) *val = TRUE; return S_OK; }
static HRESULT WINAPI eprops_get_IsInRange(void *This, boolean *val) { if (val) *val = TRUE; return S_OK; }
static HRESULT WINAPI eprops_get_IsCanceled(void *This, boolean *val) { if (val) *val = FALSE; return S_OK; }
static HRESULT WINAPI eprops_get_IsBarrelButtonPressed(void *This, boolean *val) { if (val) *val = FALSE; return S_OK; }
static HRESULT WINAPI eprops_get_IsXButton1Pressed(void *This, boolean *val) { if (val) *val = FALSE; return S_OK; }
static HRESULT WINAPI eprops_get_IsXButton2Pressed(void *This, boolean *val) { if (val) *val = FALSE; return S_OK; }
static HRESULT WINAPI eprops_get_PointerUpdateKind(void *This, UINT32 *val)
{
    if (val) *val = impl_from_elem_IPointerPointProperties(This)->update_kind;
    return S_OK;
}
static HRESULT WINAPI eprops_HasUsage(void *This, UINT32 uPage, UINT32 uId, boolean *val) { if (val) *val = FALSE; return S_OK; }
static HRESULT WINAPI eprops_GetUsageValue(void *This, UINT32 uPage, UINT32 uId, INT32 *val) { if (val) *val = 0; return S_OK; }

static const void *elem_pointer_props_vtbl[] = {
    (void *)eprops_QI, (void *)eprops_AddRef, (void *)eprops_Release,
    (void *)eprops_GetIids, (void *)eprops_GetRuntimeClassName, (void *)eprops_GetTrustLevel,
    (void *)eprops_get_Pressure, (void *)eprops_get_IsInverted, (void *)eprops_get_IsEraser,
    (void *)eprops_get_Orientation, (void *)eprops_get_XTilt, (void *)eprops_get_YTilt,
    (void *)eprops_get_Twist, (void *)eprops_get_ContactRect, (void *)eprops_get_ContactRectRaw,
    (void *)eprops_get_TouchConfidence, (void *)eprops_get_IsLeftButtonPressed,
    (void *)eprops_get_IsRightButtonPressed, (void *)eprops_get_IsMiddleButtonPressed,
    (void *)eprops_get_MouseWheelDelta, (void *)eprops_get_IsHorizontalMouseWheel,
    (void *)eprops_get_IsPrimary, (void *)eprops_get_IsInRange, (void *)eprops_get_IsCanceled,
    (void *)eprops_get_IsBarrelButtonPressed, (void *)eprops_get_IsXButton1Pressed,
    (void *)eprops_get_IsXButton2Pressed, (void *)eprops_get_PointerUpdateKind,
    (void *)eprops_HasUsage, (void *)eprops_GetUsageValue
};

/* IPointerPoint implementation */
struct elem_pointer_point_impl
{
    IInspectable IInspectable_iface;
    LONG ref;
    FLOAT x, y;
    boolean in_contact;
    struct elem_pointer_props_impl *props;
};

static inline struct elem_pointer_point_impl *impl_from_elem_IPointerPoint(void *iface)
{
    return (struct elem_pointer_point_impl *)iface;
}

static HRESULT WINAPI ept_QI(void *This, REFIID riid, void **ppv)
{
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IPointerPoint) ||
        IsEqualGUID(riid, &IID_IAgileObject))
    {
        *ppv = This;
        InterlockedIncrement(&impl_from_elem_IPointerPoint(This)->ref);
        return S_OK;
    }
    *ppv = NULL;
    return E_NOINTERFACE;
}
static ULONG WINAPI ept_AddRef(void *This) { return InterlockedIncrement(&impl_from_elem_IPointerPoint(This)->ref); }
static ULONG WINAPI ept_Release(void *This)
{
    struct elem_pointer_point_impl *impl = impl_from_elem_IPointerPoint(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref)
    {
        if (impl->props) eprops_Release(&impl->props->IInspectable_iface);
        free(impl);
    }
    return ref;
}
static HRESULT WINAPI ept_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI ept_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Input.PointerPoint", 30, n); }
static HRESULT WINAPI ept_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI ept_get_PointerDevice(void *This, void **value) { if (value) *value = NULL; return S_OK; }
static HRESULT WINAPI ept_get_Position(void *This, void *value)
{
    struct elem_pointer_point_impl *impl = impl_from_elem_IPointerPoint(This);
    struct { FLOAT X; FLOAT Y; } *pt = value;
    if (pt) { pt->X = impl->x; pt->Y = impl->y; }
    return S_OK;
}
static HRESULT WINAPI ept_get_RawPosition(void *This, void *value) { return ept_get_Position(This, value); }
static HRESULT WINAPI ept_get_PointerId(void *This, UINT32 *value) { if (value) *value = 1; return S_OK; }
static HRESULT WINAPI ept_get_FrameId(void *This, UINT32 *value) { if (value) *value = 1; return S_OK; }
static HRESULT WINAPI ept_get_Timestamp(void *This, UINT64 *value) { if (value) *value = (UINT64)GetTickCount64() * 1000; return S_OK; }
static HRESULT WINAPI ept_get_IsInContact(void *This, boolean *value)
{
    struct elem_pointer_point_impl *impl = impl_from_elem_IPointerPoint(This);
    if (value) *value = impl->in_contact;
    return S_OK;
}
static HRESULT WINAPI ept_get_Properties(void *This, void **value)
{
    struct elem_pointer_point_impl *impl = impl_from_elem_IPointerPoint(This);
    if (!value) return E_POINTER;
    *value = &impl->props->IInspectable_iface;
    eprops_AddRef(&impl->props->IInspectable_iface);
    return S_OK;
}

static const void *elem_pointer_point_vtbl[] = {
    (void *)ept_QI, (void *)ept_AddRef, (void *)ept_Release,
    (void *)ept_GetIids, (void *)ept_GetRuntimeClassName, (void *)ept_GetTrustLevel,
    (void *)ept_get_PointerDevice, (void *)ept_get_Position, (void *)ept_get_RawPosition,
    (void *)ept_get_PointerId, (void *)ept_get_FrameId, (void *)ept_get_Timestamp,
    (void *)ept_get_IsInContact, (void *)ept_get_Properties
};

/* IPointer implementation */
struct elem_pointer_impl
{
    IInspectable IInspectable_iface;
    LONG ref;
    UINT32 id;
    UINT32 device_type; /* 0=touch, 1=pen, 2=mouse */
    boolean in_contact;
    boolean in_range;
};

static inline struct elem_pointer_impl *impl_from_elem_IPointer(void *iface)
{
    return (struct elem_pointer_impl *)iface;
}

static HRESULT WINAPI eptr_QI(void *This, REFIID riid, void **ppv)
{
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IPointer) ||
        IsEqualGUID(riid, &IID_IAgileObject))
    {
        *ppv = This;
        InterlockedIncrement(&impl_from_elem_IPointer(This)->ref);
        return S_OK;
    }
    *ppv = NULL;
    return E_NOINTERFACE;
}
static ULONG WINAPI eptr_AddRef(void *This) { return InterlockedIncrement(&impl_from_elem_IPointer(This)->ref); }
static ULONG WINAPI eptr_Release(void *This)
{
    struct elem_pointer_impl *impl = impl_from_elem_IPointer(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}
static HRESULT WINAPI eptr_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI eptr_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.Input.Pointer", 29, n); }
static HRESULT WINAPI eptr_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI eptr_get_PointerId(void *This, UINT32 *val)
{
    if (val) *val = impl_from_elem_IPointer(This)->id;
    return S_OK;
}
static HRESULT WINAPI eptr_get_PointerDeviceType(void *This, UINT32 *val)
{
    if (val) *val = impl_from_elem_IPointer(This)->device_type;
    return S_OK;
}
static HRESULT WINAPI eptr_get_IsInContact(void *This, boolean *val)
{
    if (val) *val = impl_from_elem_IPointer(This)->in_contact;
    return S_OK;
}
static HRESULT WINAPI eptr_get_IsInRange(void *This, boolean *val)
{
    if (val) *val = impl_from_elem_IPointer(This)->in_range;
    return S_OK;
}

static const void *elem_pointer_vtbl[] = {
    (void *)eptr_QI, (void *)eptr_AddRef, (void *)eptr_Release,
    (void *)eptr_GetIids, (void *)eptr_GetRuntimeClassName, (void *)eptr_GetTrustLevel,
    (void *)eptr_get_PointerId, (void *)eptr_get_PointerDeviceType,
    (void *)eptr_get_IsInContact, (void *)eptr_get_IsInRange
};

/* IPointerRoutedEventArgs implementation */
struct elem_pointer_routed_args_impl
{
    IInspectable IInspectable_iface;
    LONG ref;
    struct elem_pointer_point_impl *point;
    struct elem_pointer_impl *pointer;
    boolean handled;
    UINT32 key_modifiers;
};

static inline struct elem_pointer_routed_args_impl *impl_from_elem_IPointerRoutedEventArgs(void *iface)
{
    return (struct elem_pointer_routed_args_impl *)iface;
}

static HRESULT WINAPI epra_QI(void *This, REFIID riid, void **ppv)
{
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IPointerRoutedEventArgs) ||
        IsEqualGUID(riid, &IID_IAgileObject))
    {
        *ppv = This;
        InterlockedIncrement(&impl_from_elem_IPointerRoutedEventArgs(This)->ref);
        return S_OK;
    }
    *ppv = NULL;
    return E_NOINTERFACE;
}
static ULONG WINAPI epra_AddRef(void *This) { return InterlockedIncrement(&impl_from_elem_IPointerRoutedEventArgs(This)->ref); }
static ULONG WINAPI epra_Release(void *This)
{
    struct elem_pointer_routed_args_impl *impl = impl_from_elem_IPointerRoutedEventArgs(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref)
    {
        if (impl->point) ept_Release(&impl->point->IInspectable_iface);
        if (impl->pointer) eptr_Release(&impl->pointer->IInspectable_iface);
        free(impl);
    }
    return ref;
}
static HRESULT WINAPI epra_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI epra_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.Input.PointerRoutedEventArgs", 44, n); }
static HRESULT WINAPI epra_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI epra_get_Pointer(void *This, void **val)
{
    struct elem_pointer_routed_args_impl *impl = impl_from_elem_IPointerRoutedEventArgs(This);
    if (!val) return E_POINTER;
    *val = &impl->pointer->IInspectable_iface;
    eptr_AddRef(&impl->pointer->IInspectable_iface);
    return S_OK;
}
static HRESULT WINAPI epra_get_KeyModifiers(void *This, UINT32 *val)
{
    if (val) *val = impl_from_elem_IPointerRoutedEventArgs(This)->key_modifiers;
    return S_OK;
}
static HRESULT WINAPI epra_get_Handled(void *This, boolean *val)
{
    if (val) *val = impl_from_elem_IPointerRoutedEventArgs(This)->handled;
    return S_OK;
}
static HRESULT WINAPI epra_put_Handled(void *This, boolean val)
{
    impl_from_elem_IPointerRoutedEventArgs(This)->handled = val;
    return S_OK;
}
static HRESULT WINAPI epra_GetCurrentPoint(void *This, void *relativeTo, void **val)
{
    struct elem_pointer_routed_args_impl *impl = impl_from_elem_IPointerRoutedEventArgs(This);
    if (!val) return E_POINTER;
    *val = &impl->point->IInspectable_iface;
    ept_AddRef(&impl->point->IInspectable_iface);
    return S_OK;
}
static HRESULT WINAPI epra_GetIntermediatePoints(void *This, void *relativeTo, void **val)
{
    if (val) *val = NULL;
    return S_OK;
}

static const void *elem_pointer_routed_args_vtbl[] = {
    (void *)epra_QI, (void *)epra_AddRef, (void *)epra_Release,
    (void *)epra_GetIids, (void *)epra_GetRuntimeClassName, (void *)epra_GetTrustLevel,
    (void *)epra_get_Pointer, (void *)epra_get_KeyModifiers,
    (void *)epra_get_Handled, (void *)epra_put_Handled,
    (void *)epra_GetCurrentPoint, (void *)epra_GetIntermediatePoints
};

static HRESULT WINAPI elem_ui_add_PointerPressed(void *This, void *handler, EventRegistrationToken *token)
{
    struct xaml_element *elem = impl_from_IUIElement(This);
    TRACE("elem %p (%s) add_PointerPressed handler %p\n", elem, debugstr_w(elem->name), handler);
    if (!token) return E_POINTER;
    token->value = ++next_event_token;
    if (handler && elem->pointer_pressed_count < MAX_ELEM_HANDLERS)
    {
        IUnknown_AddRef((IUnknown *)handler);
        elem->pointer_pressed_handlers[elem->pointer_pressed_count++] = handler;
    }
    return S_OK;
}
static HRESULT WINAPI elem_ui_remove_PointerPressed(void *This, EventRegistrationToken token) { return S_OK; }

static HRESULT WINAPI elem_ui_add_PointerMoved(void *This, void *handler, EventRegistrationToken *token)
{
    struct xaml_element *elem = impl_from_IUIElement(This);
    TRACE("elem %p (%s) add_PointerMoved handler %p\n", elem, debugstr_w(elem->name), handler);
    if (!token) return E_POINTER;
    token->value = ++next_event_token;
    if (handler && elem->pointer_moved_count < MAX_ELEM_HANDLERS)
    {
        IUnknown_AddRef((IUnknown *)handler);
        elem->pointer_moved_handlers[elem->pointer_moved_count++] = handler;
    }
    return S_OK;
}
static HRESULT WINAPI elem_ui_remove_PointerMoved(void *This, EventRegistrationToken token) { return S_OK; }

static HRESULT WINAPI elem_ui_add_PointerReleased(void *This, void *handler, EventRegistrationToken *token)
{
    struct xaml_element *elem = impl_from_IUIElement(This);
    TRACE("elem %p (%s) add_PointerReleased handler %p\n", elem, debugstr_w(elem->name), handler);
    if (!token) return E_POINTER;
    token->value = ++next_event_token;
    if (handler && elem->pointer_released_count < MAX_ELEM_HANDLERS)
    {
        IUnknown_AddRef((IUnknown *)handler);
        elem->pointer_released_handlers[elem->pointer_released_count++] = handler;
    }
    return S_OK;
}
static HRESULT WINAPI elem_ui_remove_PointerReleased(void *This, EventRegistrationToken token) { return S_OK; }

static HRESULT WINAPI elem_ui_add_PointerEntered(void *This, void *handler, EventRegistrationToken *token)
{
    struct xaml_element *elem = impl_from_IUIElement(This);
    TRACE("elem %p (%s) add_PointerEntered handler %p\n", elem, debugstr_w(elem->name), handler);
    if (!token) return E_POINTER;
    token->value = ++next_event_token;
    if (handler && elem->pointer_entered_count < MAX_ELEM_HANDLERS)
    {
        IUnknown_AddRef((IUnknown *)handler);
        elem->pointer_entered_handlers[elem->pointer_entered_count++] = handler;
    }
    return S_OK;
}
static HRESULT WINAPI elem_ui_remove_PointerEntered(void *This, EventRegistrationToken token) { return S_OK; }

static HRESULT WINAPI elem_ui_add_PointerExited(void *This, void *handler, EventRegistrationToken *token)
{
    struct xaml_element *elem = impl_from_IUIElement(This);
    TRACE("elem %p (%s) add_PointerExited handler %p\n", elem, debugstr_w(elem->name), handler);
    if (!token) return E_POINTER;
    token->value = ++next_event_token;
    if (handler && elem->pointer_exited_count < MAX_ELEM_HANDLERS)
    {
        IUnknown_AddRef((IUnknown *)handler);
        elem->pointer_exited_handlers[elem->pointer_exited_count++] = handler;
    }
    return S_OK;
}
static HRESULT WINAPI elem_ui_remove_PointerExited(void *This, EventRegistrationToken token) { return S_OK; }

static HRESULT WINAPI elem_ui_add_PointerCaptureLost(void *This, void *handler, EventRegistrationToken *token)
{
    if (token) token->value = ++next_event_token;
    return S_OK;
}
static HRESULT WINAPI elem_ui_remove_PointerCaptureLost(void *This, EventRegistrationToken token) { return S_OK; }

static HRESULT WINAPI elem_ui_add_PointerCanceled(void *This, void *handler, EventRegistrationToken *token)
{
    if (token) token->value = ++next_event_token;
    return S_OK;
}
static HRESULT WINAPI elem_ui_remove_PointerCanceled(void *This, EventRegistrationToken token) { return S_OK; }

static HRESULT WINAPI elem_ui_add_PointerWheelChanged(void *This, void *handler, EventRegistrationToken *token)
{
    struct xaml_element *elem = impl_from_IUIElement(This);
    TRACE("elem %p (%s) add_PointerWheelChanged handler %p\n", elem, debugstr_w(elem->name), handler);
    if (!token) return E_POINTER;
    token->value = ++next_event_token;
    if (handler && elem->pointer_wheel_count < MAX_ELEM_HANDLERS)
    {
        IUnknown_AddRef((IUnknown *)handler);
        elem->pointer_wheel_handlers[elem->pointer_wheel_count++] = handler;
    }
    return S_OK;
}
static HRESULT WINAPI elem_ui_remove_PointerWheelChanged(void *This, EventRegistrationToken token) { return S_OK; }

static HRESULT WINAPI elem_ui_add_Tapped(void *This, void *handler, EventRegistrationToken *token)
{
    struct xaml_element *elem = impl_from_IUIElement(This);
    TRACE("elem %p (%s) add_Tapped handler %p\n", elem, debugstr_w(elem->name), handler);
    if (!token) return E_POINTER;
    token->value = ++next_event_token;
    if (handler && elem->tapped_count < MAX_ELEM_HANDLERS)
    {
        IUnknown_AddRef((IUnknown *)handler);
        elem->tapped_handlers[elem->tapped_count++] = handler;
    }
    return S_OK;
}
static HRESULT WINAPI elem_ui_remove_Tapped(void *This, EventRegistrationToken token) { return S_OK; }

static HRESULT WINAPI elem_ui_add_DoubleTapped(void *This, void *handler, EventRegistrationToken *token)
{
    if (token) token->value = ++next_event_token;
    return S_OK;
}
static HRESULT WINAPI elem_ui_remove_DoubleTapped(void *This, EventRegistrationToken token) { return S_OK; }

static HRESULT WINAPI elem_ui_add_Holding(void *This, void *handler, EventRegistrationToken *token)
{
    if (token) token->value = ++next_event_token;
    return S_OK;
}
static HRESULT WINAPI elem_ui_remove_Holding(void *This, EventRegistrationToken token) { return S_OK; }

static HRESULT WINAPI elem_ui_add_RightTapped(void *This, void *handler, EventRegistrationToken *token)
{
    if (token) token->value = ++next_event_token;
    return S_OK;
}
static HRESULT WINAPI elem_ui_remove_RightTapped(void *This, EventRegistrationToken token) { return S_OK; }

static HRESULT WINAPI elem_ui_add_KeyDown(void *This, void *handler, EventRegistrationToken *token)
{
    struct xaml_element *elem = impl_from_IUIElement(This);
    if (!token) return E_POINTER;
    token->value = ++next_event_token;
    if (handler && elem->keydown_count < MAX_ELEM_HANDLERS)
    {
        IUnknown_AddRef((IUnknown *)handler);
        elem->keydown_handlers[elem->keydown_count++] = handler;
    }
    return S_OK;
}
static HRESULT WINAPI elem_ui_remove_KeyDown(void *This, EventRegistrationToken token) { return S_OK; }

static HRESULT WINAPI elem_ui_add_KeyUp(void *This, void *handler, EventRegistrationToken *token)
{
    struct xaml_element *elem = impl_from_IUIElement(This);
    if (!token) return E_POINTER;
    token->value = ++next_event_token;
    if (handler && elem->keyup_count < MAX_ELEM_HANDLERS)
    {
        IUnknown_AddRef((IUnknown *)handler);
        elem->keyup_handlers[elem->keyup_count++] = handler;
    }
    return S_OK;
}
static HRESULT WINAPI elem_ui_remove_KeyUp(void *This, EventRegistrationToken token) { return S_OK; }

static HRESULT WINAPI elem_ui_add_GotFocus(void *This, void *handler, EventRegistrationToken *token)
{
    if (token) token->value = ++next_event_token;
    return S_OK;
}
static HRESULT WINAPI elem_ui_remove_GotFocus(void *This, EventRegistrationToken token) { return S_OK; }

static HRESULT WINAPI elem_ui_add_LostFocus(void *This, void *handler, EventRegistrationToken *token)
{
    if (token) token->value = ++next_event_token;
    return S_OK;
}
static HRESULT WINAPI elem_ui_remove_LostFocus(void *This, EventRegistrationToken token) { return S_OK; }

static void elem_fire_pointer(struct xaml_element *elem, UINT msg, WPARAM wparam, LPARAM lparam)
{
    struct elem_pointer_routed_args_impl *args;
    struct elem_pointer_point_impl *point;
    struct elem_pointer_props_impl *props;
    struct elem_pointer_impl *ptr;
    void **handlers = NULL;
    UINT count = 0, i;
    FLOAT x = (FLOAT)(short)LOWORD(lparam);
    FLOAT y = (FLOAT)(short)HIWORD(lparam);
    boolean left_pressed = (wparam & MK_LBUTTON) != 0;
    boolean right_pressed = (wparam & MK_RBUTTON) != 0;
    boolean middle_pressed = (wparam & MK_MBUTTON) != 0;
    boolean in_contact = FALSE;
    UINT32 update_kind = 0;
    INT32 wheel_delta = 0;

    if (!elem) return;

    switch (msg)
    {
        case WM_LBUTTONDOWN:
            in_contact = TRUE; left_pressed = TRUE; update_kind = 1;
            handlers = elem->pointer_pressed_handlers; count = elem->pointer_pressed_count;
            break;
        case WM_LBUTTONUP:
            in_contact = FALSE; left_pressed = FALSE; update_kind = 2;
            handlers = elem->pointer_released_handlers; count = elem->pointer_released_count;
            break;
        case WM_RBUTTONDOWN:
            in_contact = TRUE; right_pressed = TRUE; update_kind = 3;
            handlers = elem->pointer_pressed_handlers; count = elem->pointer_pressed_count;
            break;
        case WM_RBUTTONUP:
            in_contact = FALSE; right_pressed = FALSE; update_kind = 4;
            handlers = elem->pointer_released_handlers; count = elem->pointer_released_count;
            break;
        case WM_MBUTTONDOWN:
            in_contact = TRUE; middle_pressed = TRUE; update_kind = 5;
            handlers = elem->pointer_pressed_handlers; count = elem->pointer_pressed_count;
            break;
        case WM_MBUTTONUP:
            in_contact = FALSE; middle_pressed = FALSE; update_kind = 6;
            handlers = elem->pointer_released_handlers; count = elem->pointer_released_count;
            break;
        case WM_MOUSEMOVE:
            in_contact = left_pressed || right_pressed || middle_pressed;
            update_kind = 0;
            handlers = elem->pointer_moved_handlers; count = elem->pointer_moved_count;
            break;
        case WM_MOUSEWHEEL:
            wheel_delta = GET_WHEEL_DELTA_WPARAM(wparam);
            handlers = elem->pointer_wheel_handlers; count = elem->pointer_wheel_count;
            break;
        default:
            return;
    }

    if (count == 0) return;

    props = calloc(1, sizeof(*props));
    point = calloc(1, sizeof(*point));
    ptr = calloc(1, sizeof(*ptr));
    args = calloc(1, sizeof(*args));
    if (!props || !point || !ptr || !args)
    {
        free(props); free(point); free(ptr); free(args);
        return;
    }

    props->IInspectable_iface.lpVtbl = (void *)elem_pointer_props_vtbl;
    props->ref = 1;
    props->x = x; props->y = y;
    props->left_pressed = left_pressed; props->right_pressed = right_pressed; props->middle_pressed = middle_pressed;
    props->in_contact = in_contact; props->update_kind = update_kind; props->wheel_delta = wheel_delta;

    point->IInspectable_iface.lpVtbl = (void *)elem_pointer_point_vtbl;
    point->ref = 1;
    point->x = x; point->y = y;
    point->in_contact = in_contact;
    point->props = props;

    ptr->IInspectable_iface.lpVtbl = (void *)elem_pointer_vtbl;
    ptr->ref = 1;
    ptr->id = 1;
    ptr->device_type = 2; /* PointerDeviceType_Mouse */
    ptr->in_contact = in_contact;
    ptr->in_range = TRUE;

    args->IInspectable_iface.lpVtbl = (void *)elem_pointer_routed_args_vtbl;
    args->ref = 1;
    args->point = point;
    args->pointer = ptr;

    for (i = 0; i < count; i++)
    {
        void *h = handlers[i];
        void **vtbl = h ? *(void ***)h : NULL;
        if (vtbl && (uintptr_t)vtbl[3] > 0x10000)
        {
            HRESULT (STDMETHODCALLTYPE *invoke_fn)(void *, void *, void *) = (void *)vtbl[3];
            TRACE("Invoking UIElement pointer handler[%d] on %p (%s) msg=0x%x (%.0f,%.0f)\n",
                  i, elem, debugstr_w(elem->name), msg, x, y);
            invoke_fn(h, (void *)&elem->IUIElement_iface, (void *)args);
        }
    }

    epra_Release(&args->IInspectable_iface);
}

static void elem_fire_pointer_all(UINT msg, WPARAM wparam, LPARAM lparam)
{
    if (global_swapchain_panel)
        elem_fire_pointer(global_swapchain_panel, msg, wparam, lparam);
    if (global_page_element && global_page_element != global_swapchain_panel)
        elem_fire_pointer(global_page_element, msg, wparam, lparam);
    if (global_root_element && global_root_element != global_swapchain_panel && global_root_element != global_page_element)
        elem_fire_pointer(global_root_element, msg, wparam, lparam);
}

/* -------------------------------------------------------------------------
 * IFrameworkElement VTable
 * ------------------------------------------------------------------------- */
static HRESULT WINAPI elem_fe_QI(void *This, REFIID riid, void **ppv) { return elem_QI_internal(impl_from_IFrameworkElement(This), riid, ppv); }
static ULONG WINAPI elem_fe_AddRef(void *This) { return elem_AddRef_internal(impl_from_IFrameworkElement(This)); }
static ULONG WINAPI elem_fe_Release(void *This) { return elem_Release_internal(impl_from_IFrameworkElement(This)); }
static HRESULT WINAPI elem_fe_get_ActualWidth(void *This, DOUBLE *val)
{
    struct xaml_element *impl = impl_from_IFrameworkElement(This);
    DOUBLE res = (DOUBLE)(impl->bounds.right - impl->bounds.left);
    if (res <= 0.0)
    {
        if (global_xaml_hwnd)
        {
            RECT r;
            if (GetClientRect(global_xaml_hwnd, &r) && (r.right - r.left > 0))
                res = (DOUBLE)(r.right - r.left);
        }
        if (res <= 0.0) res = 1024.0;
    }
    if (val) *val = res;
    return S_OK;
}
static HRESULT WINAPI elem_fe_get_ActualHeight(void *This, DOUBLE *val)
{
    struct xaml_element *impl = impl_from_IFrameworkElement(This);
    DOUBLE res = (DOUBLE)(impl->bounds.bottom - impl->bounds.top);
    if (res <= 0.0)
    {
        if (global_xaml_hwnd)
        {
            RECT r;
            if (GetClientRect(global_xaml_hwnd, &r) && (r.bottom - r.top > 0))
                res = (DOUBLE)(r.bottom - r.top);
        }
        if (res <= 0.0) res = 768.0;
    }
    if (val) *val = res;
    return S_OK;
}
static HRESULT WINAPI elem_fe_get_Width(void *This, DOUBLE *val) { if (val) *val = impl_from_IFrameworkElement(This)->width; return S_OK; }
static HRESULT WINAPI elem_fe_put_Width(void *This, DOUBLE val) { impl_from_IFrameworkElement(This)->width = val; return S_OK; }
static HRESULT WINAPI elem_fe_get_Height(void *This, DOUBLE *val) { if (val) *val = impl_from_IFrameworkElement(This)->height; return S_OK; }
static HRESULT WINAPI elem_fe_put_Height(void *This, DOUBLE val) { impl_from_IFrameworkElement(This)->height = val; return S_OK; }
static HRESULT WINAPI elem_fe_get_Name(void *This, HSTRING *val) { return WindowsCreateString(impl_from_IFrameworkElement(This)->name, wcslen(impl_from_IFrameworkElement(This)->name), val); }
static HRESULT WINAPI elem_fe_put_Name(void *This, HSTRING val)
{
    const WCHAR *str = WindowsGetStringRawBuffer(val, NULL);
    if (str) lstrcpynW(impl_from_IFrameworkElement(This)->name, str, 64);
    return S_OK;
}
static HRESULT WINAPI elem_fe_add_Loaded(void *This, void *handler, EventRegistrationToken *token)
{
    struct xaml_element *impl = impl_from_IFrameworkElement(This);
    void **hvtbl = handler ? *(void ***)handler : NULL;
    TRACE("element %p (%s) add_Loaded handler %p (vtbl=%p)\n", impl, debugstr_w(impl->name), handler, hvtbl);
    if (handler)
    {
        IUnknown_AddRef((IUnknown *)handler);
        impl->loaded_handler = handler;
    }
    if (token) token->value = 1;
    return S_OK;
}
static HRESULT WINAPI elem_fe_remove_Loaded(void *This, EventRegistrationToken token) { return S_OK; }
static HRESULT WINAPI elem_fe_SetBinding(void *This, void *dp, void *binding) { return S_OK; }

static struct xaml_element *xaml_find_element_by_name(struct xaml_element *elem, const WCHAR *name)
{
    UINT32 i;
    if (!elem || !name) return NULL;
    if (!wcscmp(elem->name, name)) return elem;

    if (elem->child_content)
    {
        struct xaml_element *res = xaml_find_element_by_name(elem->child_content, name);
        if (res) return res;
    }

    for (i = 0; i < elem->children_count; i++)
    {
        struct xaml_element *res = xaml_find_element_by_name(elem->children[i], name);
        if (res) return res;
    }
    return NULL;
}

static HRESULT WINAPI elem_fe_get_Resources(void *This, void **value)
{
    struct xaml_element *elem = impl_from_IFrameworkElement(This);
    TRACE("This %p (%s), value %p\n", elem, debugstr_w(elem->name), value);
    if (!value) return E_POINTER;
    if (!elem->resources)
    {
        HRESULT hr = create_resource_dictionary_instance(&elem->resources);
        if (FAILED(hr)) return hr;
    }
    *value = elem->resources;
    IUnknown_AddRef((IUnknown *)*value);
    return S_OK;
}

static HRESULT WINAPI elem_fe_put_Resources(void *This, void *value)
{
    struct xaml_element *elem = impl_from_IFrameworkElement(This);
    TRACE("This %p (%s), value %p\n", elem, debugstr_w(elem->name), value);
    if (elem->resources) IUnknown_Release((IUnknown *)elem->resources);
    elem->resources = value;
    if (elem->resources) IUnknown_AddRef((IUnknown *)elem->resources);
    return S_OK;
}

static HRESULT WINAPI elem_fe_get_DataContext(void *This, void **value)
{
    struct xaml_element *elem = impl_from_IFrameworkElement(This);
    TRACE("This %p (%s), value %p\n", elem, debugstr_w(elem->name), value);
    if (!value) return E_POINTER;
    *value = elem->data_context;
    if (*value) IUnknown_AddRef((IUnknown *)*value);
    return S_OK;
}

static HRESULT WINAPI elem_fe_put_DataContext(void *This, void *value)
{
    struct xaml_element *elem = impl_from_IFrameworkElement(This);
    TRACE("This %p (%s), value %p\n", elem, debugstr_w(elem->name), value);
    if (elem->data_context) IUnknown_Release((IUnknown *)elem->data_context);
    elem->data_context = value;
    if (elem->data_context) IUnknown_AddRef((IUnknown *)elem->data_context);
    return S_OK;
}

static HRESULT WINAPI elem_fe_get_Parent(void *This, void **value)
{
    struct xaml_element *elem = impl_from_IFrameworkElement(This);
    TRACE("This %p (%s), value %p\n", elem, debugstr_w(elem->name), value);
    if (!value) return E_POINTER;
    *value = elem->parent ? &elem->parent->IFrameworkElement_iface : NULL;
    if (*value) IUnknown_AddRef((IUnknown *)*value);
    return S_OK;
}

static const GUID IID_ISizeChangedEventArgs = {0xd5312e60, 0x5cc1, 0x42a1, {0x92, 0x0c, 0x1a, 0xf4, 0x6b, 0xe2, 0xf9, 0x86}};

struct size_changed_args_impl
{
    IInspectable IInspectable_iface;
    LONG ref;
    struct { FLOAT Width; FLOAT Height; } prev_size;
    struct { FLOAT Width; FLOAT Height; } new_size;
};

static inline struct size_changed_args_impl *impl_from_ISizeChangedEventArgs(void *iface)
{
    return (struct size_changed_args_impl *)iface;
}

static HRESULT WINAPI sca_QueryInterface(void *This, REFIID riid, void **ppv)
{
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_ISizeChangedEventArgs) ||
        IsEqualGUID(riid, &IID_IAgileObject))
    {
        *ppv = This;
        InterlockedIncrement(&impl_from_ISizeChangedEventArgs(This)->ref);
        return S_OK;
    }
    *ppv = NULL;
    return E_NOINTERFACE;
}
static ULONG WINAPI sca_AddRef(void *This) { return InterlockedIncrement(&impl_from_ISizeChangedEventArgs(This)->ref); }
static ULONG WINAPI sca_Release(void *This)
{
    struct size_changed_args_impl *impl = impl_from_ISizeChangedEventArgs(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}
static HRESULT WINAPI sca_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI sca_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.SizeChangedEventArgs", 36, n); }
static HRESULT WINAPI sca_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI sca_get_PreviousSize(void *This, void *val)
{
    struct size_changed_args_impl *impl = impl_from_ISizeChangedEventArgs(This);
    if (val) memcpy(val, &impl->prev_size, sizeof(impl->prev_size));
    return S_OK;
}
static HRESULT WINAPI sca_get_NewSize(void *This, void *val)
{
    struct size_changed_args_impl *impl = impl_from_ISizeChangedEventArgs(This);
    if (val) memcpy(val, &impl->new_size, sizeof(impl->new_size));
    return S_OK;
}

static const void *size_changed_args_vtbl[] = {
    (void *)sca_QueryInterface,
    (void *)sca_AddRef,
    (void *)sca_Release,
    (void *)sca_GetIids,
    (void *)sca_GetRuntimeClassName,
    (void *)sca_GetTrustLevel,
    (void *)sca_get_PreviousSize,
    (void *)sca_get_NewSize
};

static void elem_fire_size_changed(struct xaml_element *elem, float pw, float ph, float nw, float nh)
{
    struct size_changed_args_impl *args;
    HRESULT (STDMETHODCALLTYPE *invoke_fn)(void *This, void *sender, void *e);
    void **vtbl;

    if (!elem || !elem->size_changed_handler) return;

    args = calloc(1, sizeof(*args));
    if (!args) return;
    args->IInspectable_iface.lpVtbl = (void *)size_changed_args_vtbl;
    args->ref = 1;
    args->prev_size.Width = pw;
    args->prev_size.Height = ph;
    args->new_size.Width = nw;
    args->new_size.Height = nh;

    vtbl = *(void ***)elem->size_changed_handler;
    invoke_fn = (HRESULT (STDMETHODCALLTYPE *)(void *, void *, void *))vtbl[3];
    TRACE("Firing SizeChanged on elem %p (%s): %f x %f -> %f x %f\n",
          elem, debugstr_w(elem->name), pw, ph, nw, nh);
    invoke_fn(elem->size_changed_handler, (void *)&elem->IUIElement_iface, (void *)args);

    sca_Release(&args->IInspectable_iface);
}

static HRESULT WINAPI elem_fe_add_SizeChanged(void *This, void *handler, EventRegistrationToken *token)
{
    struct xaml_element *elem = impl_from_IFrameworkElement(This);
    DOUBLE w = 1024.0, h = 768.0;
    TRACE("This %p (%s), handler %p, token %p\n", elem, debugstr_w(elem->name), handler, token);
    if (token) token->value = 1;
    if (handler)
    {
        elem->size_changed_handler = handler;
        IUnknown_AddRef((IUnknown *)handler);
        elem_fe_get_ActualWidth(This, &w);
        elem_fe_get_ActualHeight(This, &h);
        elem_fire_size_changed(elem, 0.0f, 0.0f, (float)w, (float)h);
    }
    return S_OK;
}

static HRESULT WINAPI elem_fe_remove_SizeChanged(void *This, EventRegistrationToken token)
{
    struct xaml_element *elem = impl_from_IFrameworkElement(This);
    if (elem->size_changed_handler)
    {
        IUnknown_Release((IUnknown *)elem->size_changed_handler);
        elem->size_changed_handler = NULL;
    }
    return S_OK;
}

static HRESULT WINAPI elem_fe_FindName(void *This, HSTRING name, void **returnValue)
{
    struct xaml_element *elem = impl_from_IFrameworkElement(This);
    const WCHAR *name_w = WindowsGetStringRawBuffer(name, NULL);
    struct xaml_element *found;

    TRACE("This %p (%s), FindName %s\n", elem, debugstr_w(elem->name), debugstr_w(name_w));
    if (!returnValue) return E_POINTER;
    *returnValue = NULL;

    found = xaml_find_element_by_name(elem, name_w);
    if (!found && global_root_element)
        found = xaml_find_element_by_name(global_root_element, name_w);

    if (found)
    {
        *returnValue = &found->IFrameworkElement_iface;
        elem_AddRef_internal(found);
    }
    return S_OK;
}

/* -------------------------------------------------------------------------
 * IButtonBase & IButton VTable
 * ------------------------------------------------------------------------- */
static HRESULT WINAPI elem_btn_QI(void *This, REFIID riid, void **ppv) { return elem_QI_internal(impl_from_IButton(This), riid, ppv); }
static ULONG WINAPI elem_btn_AddRef(void *This) { return elem_AddRef_internal(impl_from_IButton(This)); }
static ULONG WINAPI elem_btn_Release(void *This) { return elem_Release_internal(impl_from_IButton(This)); }
static HRESULT WINAPI elem_btn_GetRuntimeClassName(void *This, HSTRING *n) { return elem_ui_GetRuntimeClassName(&impl_from_IButton(This)->IUIElement_iface, n); }

static HRESULT WINAPI elem_btnbase_QI(void *This, REFIID riid, void **ppv) { return elem_QI_internal(impl_from_IButtonBase(This), riid, ppv); }
static ULONG WINAPI elem_btnbase_AddRef(void *This) { return elem_AddRef_internal(impl_from_IButtonBase(This)); }
static ULONG WINAPI elem_btnbase_Release(void *This) { return elem_Release_internal(impl_from_IButtonBase(This)); }
static HRESULT WINAPI elem_btnbase_GetRuntimeClassName(void *This, HSTRING *n) { return elem_ui_GetRuntimeClassName(&impl_from_IButtonBase(This)->IUIElement_iface, n); }
static HRESULT WINAPI elem_btn_get_Command(void *This, void **val) { if (val) *val = impl_from_IButtonBase(This)->command; return S_OK; }
static HRESULT WINAPI elem_btn_put_Command(void *This, void *val) { impl_from_IButtonBase(This)->command = val; return S_OK; }
static HRESULT WINAPI elem_btn_get_CommandParam(void *This, void **val) { if (val) *val = impl_from_IButtonBase(This)->command_param; return S_OK; }
static HRESULT WINAPI elem_btn_put_CommandParam(void *This, void *val) { impl_from_IButtonBase(This)->command_param = val; return S_OK; }
static HRESULT WINAPI elem_btn_add_Click(void *This, void *handler, EventRegistrationToken *token)
{
    struct xaml_element *impl = impl_from_IButtonBase(This);
    TRACE("Button %p (%s) add_Click handler %p\n", impl, debugstr_w(impl->name), handler);
    if (handler)
    {
        IUnknown_AddRef((IUnknown *)handler);
        impl->click_handler = handler;
    }
    if (token) token->value = 1;
    return S_OK;
}
static HRESULT WINAPI elem_btn_remove_Click(void *This, EventRegistrationToken token) { return S_OK; }

/* -------------------------------------------------------------------------
 * ITextBlock VTable
 * ------------------------------------------------------------------------- */
static HRESULT WINAPI elem_tb_QI(void *This, REFIID riid, void **ppv) { return elem_QI_internal(impl_from_ITextBlock(This), riid, ppv); }
static ULONG WINAPI elem_tb_AddRef(void *This) { return elem_AddRef_internal(impl_from_ITextBlock(This)); }
static ULONG WINAPI elem_tb_Release(void *This) { return elem_Release_internal(impl_from_ITextBlock(This)); }
static HRESULT WINAPI elem_tb_get_FontSize(void *This, DOUBLE *val) { if (val) *val = impl_from_ITextBlock(This)->font_size; return S_OK; }
static HRESULT WINAPI elem_tb_put_FontSize(void *This, DOUBLE val) { impl_from_ITextBlock(This)->font_size = val; return S_OK; }
static HRESULT WINAPI elem_tb_get_Text(void *This, HSTRING *val) { return WindowsCreateString(impl_from_ITextBlock(This)->text, wcslen(impl_from_ITextBlock(This)->text), val); }
static HRESULT WINAPI elem_tb_put_Text(void *This, HSTRING val)
{
    struct xaml_element *impl = impl_from_ITextBlock(This);
    const WCHAR *str = WindowsGetStringRawBuffer(val, NULL);
    TRACE("TextBlock %p (%s) put_Text: %s\n", impl, debugstr_w(impl->name), debugstr_w(str));
    if (str) lstrcpynW(impl->text, str, ARRAY_SIZE(impl->text));
    else impl->text[0] = 0;
    if (global_xaml_hwnd) InvalidateRect(global_xaml_hwnd, NULL, FALSE);
    return S_OK;
}

/* -------------------------------------------------------------------------
 * IBorder VTable
 * ------------------------------------------------------------------------- */
static HRESULT WINAPI elem_border_QI(void *This, REFIID riid, void **ppv) { return elem_QI_internal(impl_from_IBorder(This), riid, ppv); }
static ULONG WINAPI elem_border_AddRef(void *This) { return elem_AddRef_internal(impl_from_IBorder(This)); }
static ULONG WINAPI elem_border_Release(void *This) { return elem_Release_internal(impl_from_IBorder(This)); }
static HRESULT WINAPI elem_border_get_Child(void *This, void **val)
{
    struct xaml_element *impl = impl_from_IBorder(This);
    if (!val) return E_POINTER;
    *val = impl->child_content ? &impl->child_content->IUIElement_iface : NULL;
    if (*val) elem_AddRef_internal(impl->child_content);
    return S_OK;
}
static HRESULT WINAPI elem_border_put_Child(void *This, void *val)
{
    struct xaml_element *impl = impl_from_IBorder(This);
    TRACE("Border %p (%s) put_Child: %p\n", impl, debugstr_w(impl->name), val);
    if (val)
    {
        struct xaml_element *child = impl_from_IUIElement(val);
        elem_AddRef_internal(child);
        if (impl->child_content) elem_Release_internal(impl->child_content);
        impl->child_content = child;
    }
    else
    {
        if (impl->child_content) elem_Release_internal(impl->child_content);
        impl->child_content = NULL;
    }
    if (global_xaml_hwnd) InvalidateRect(global_xaml_hwnd, NULL, FALSE);
    return S_OK;
}

/* -------------------------------------------------------------------------
 * IContentControl & IUserControl VTable
 * ------------------------------------------------------------------------- */
static HRESULT WINAPI elem_cc_QI(void *This, REFIID riid, void **ppv) { return elem_QI_internal(impl_from_IContentControl(This), riid, ppv); }
static ULONG WINAPI elem_cc_AddRef(void *This) { return elem_AddRef_internal(impl_from_IContentControl(This)); }
static ULONG WINAPI elem_cc_Release(void *This) { return elem_Release_internal(impl_from_IContentControl(This)); }
static HRESULT WINAPI elem_cc_get_Content(void *This, void **val)
{
    struct xaml_element *impl = impl_from_IContentControl(This);
    if (!val) return E_POINTER;
    *val = impl->child_content ? &impl->child_content->IUIElement_iface : NULL;
    if (*val) elem_AddRef_internal(impl->child_content);
    return S_OK;
}
static HRESULT WINAPI elem_cc_put_Content(void *This, void *val)
{
    struct xaml_element *impl = impl_from_IContentControl(This);
    TRACE("ContentControl %p (%s) put_Content: %p\n", impl, debugstr_w(impl->name), val);
    if (val)
    {
        struct xaml_element *child = impl_from_IUIElement(val);
        elem_AddRef_internal(child);
        if (impl->child_content) elem_Release_internal(impl->child_content);
        impl->child_content = child;
    }
    if (global_xaml_hwnd) InvalidateRect(global_xaml_hwnd, NULL, FALSE);
    return S_OK;
}

/* -------------------------------------------------------------------------
 * UIElementCollection (IVector<UIElement*>)
 * ------------------------------------------------------------------------- */
struct elem_collection_impl
{
    struct { const void *lpVtbl; } IVector_iface;
    LONG ref;
    struct xaml_element *owner;
};

#define impl_from_IVector(ptr) ((struct elem_collection_impl *)((char *)(ptr) - offsetof(struct elem_collection_impl, IVector_iface)))

static HRESULT WINAPI vec_QI(void *This, REFIID riid, void **ppv)
{
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IVector_UIElement))
    {
        *ppv = This;
        InterlockedIncrement(&impl_from_IVector(This)->ref);
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI vec_AddRef(void *This) { return InterlockedIncrement(&impl_from_IVector(This)->ref); }
static ULONG WINAPI vec_Release(void *This)
{
    struct elem_collection_impl *impl = impl_from_IVector(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}
static HRESULT WINAPI vec_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI vec_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.Foundation.Collections.IVector`1", 39, n); }
static HRESULT WINAPI vec_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI vec_GetAt(void *This, UINT32 index, void **item)
{
    struct elem_collection_impl *impl = impl_from_IVector(This);
    if (!item) return E_POINTER;
    if (index >= impl->owner->children_count) return E_BOUNDS;
    *item = &impl->owner->children[index]->IUIElement_iface;
    elem_AddRef_internal(impl->owner->children[index]);
    return S_OK;
}
static HRESULT WINAPI vec_get_Size(void *This, UINT32 *size)
{
    struct elem_collection_impl *impl = impl_from_IVector(This);
    if (size) *size = impl->owner->children_count;
    return S_OK;
}
static HRESULT WINAPI vec_Append(void *This, void *item)
{
    struct elem_collection_impl *impl = impl_from_IVector(This);
    if (item)
    {
        struct xaml_element *child = impl_from_IUIElement(item);
        xaml_element_add_child(impl->owner, child);
    }
    return S_OK;
}

static const void *elem_vec_vtbl[16] = {
    (void *)vec_QI,
    (void *)vec_AddRef,
    (void *)vec_Release,
    (void *)vec_GetIids,
    (void *)vec_GetRuntimeClassName,
    (void *)vec_GetTrustLevel,
    (void *)vec_GetAt,            /* 6 */
    (void *)vec_get_Size,         /* 7 */
    (void *)elem_stub_ret_zero,   /* 8 GetView */
    (void *)elem_stub_ret_zero,   /* 9 IndexOf */
    (void *)elem_stub_ret_ok,     /* 10 SetAt */
    (void *)elem_stub_ret_ok,     /* 11 InsertAt */
    (void *)elem_stub_ret_ok,     /* 12 RemoveAt */
    (void *)vec_Append,           /* 13 Append */
    (void *)elem_stub_ret_ok,     /* 14 RemoveAtEnd */
    (void *)elem_stub_ret_ok      /* 15 Clear */
};

static void *create_elem_collection(struct xaml_element *owner)
{
    struct elem_collection_impl *impl = calloc(1, sizeof(*impl));
    if (!impl) return NULL;
    impl->IVector_iface.lpVtbl = elem_vec_vtbl;
    impl->ref = 1;
    impl->owner = owner;
    return &impl->IVector_iface;
}

/* -------------------------------------------------------------------------
 * IPanel & IGrid VTable
 * ------------------------------------------------------------------------- */
static HRESULT WINAPI elem_panel_QI(void *This, REFIID riid, void **ppv) { return elem_QI_internal(impl_from_IPanel(This), riid, ppv); }
static ULONG WINAPI elem_panel_AddRef(void *This) { return elem_AddRef_internal(impl_from_IPanel(This)); }
static ULONG WINAPI elem_panel_Release(void *This) { return elem_Release_internal(impl_from_IPanel(This)); }
static HRESULT WINAPI elem_panel_get_Children(void *This, void **val)
{
    struct xaml_element *impl = impl_from_IPanel(This);
    if (!val) return E_POINTER;
    *val = create_elem_collection(impl);
    return S_OK;
}

static HRESULT WINAPI elem_grid_QI(void *This, REFIID riid, void **ppv) { return elem_QI_internal(impl_from_IGrid(This), riid, ppv); }
static ULONG WINAPI elem_grid_AddRef(void *This) { return elem_AddRef_internal(impl_from_IGrid(This)); }
static ULONG WINAPI elem_grid_Release(void *This) { return elem_Release_internal(impl_from_IGrid(This)); }

static HRESULT WINAPI elem_canvas_QI(void *This, REFIID riid, void **ppv) { return elem_QI_internal(impl_from_ICanvas(This), riid, ppv); }
static ULONG WINAPI elem_canvas_AddRef(void *This) { return elem_AddRef_internal(impl_from_ICanvas(This)); }
static ULONG WINAPI elem_canvas_Release(void *This) { return elem_Release_internal(impl_from_ICanvas(This)); }
static HRESULT WINAPI elem_canvas_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.Controls.Canvas", 31, n); }

static HRESULT WINAPI elem_scp_QI(void *This, REFIID riid, void **ppv) { return elem_QI_internal(impl_from_ISwapChainPanel(This), riid, ppv); }
static ULONG WINAPI elem_scp_AddRef(void *This) { return elem_AddRef_internal(impl_from_ISwapChainPanel(This)); }
static ULONG WINAPI elem_scp_Release(void *This) { return elem_Release_internal(impl_from_ISwapChainPanel(This)); }
static HRESULT WINAPI elem_scp_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.Controls.SwapChainPanel", 39, n); }
static HRESULT WINAPI elem_scp_get_CompositionScaleX(void *This, float *val) { if (val) *val = 1.0f; return S_OK; }
static HRESULT WINAPI elem_scp_get_CompositionScaleY(void *This, float *val) { if (val) *val = 1.0f; return S_OK; }
static HRESULT WINAPI elem_scp_add_CompositionScaleChanged(void *This, void *handler, EventRegistrationToken *token) { if (token) token->value = 1; return S_OK; }
static HRESULT WINAPI elem_scp_remove_CompositionScaleChanged(void *This, EventRegistrationToken token) { return S_OK; }
/* -------------------------------------------------------------------------
 * CoreIndependentInputSource Implementation
 * ------------------------------------------------------------------------- */
static const GUID IID_ICoreInputSourceBase        = {0x9f488807, 0x4580, 0x4be8, {0xbe, 0x68, 0x92, 0xa9, 0x31, 0x17, 0x13, 0xbb}};
static const GUID IID_ICorePointerInputSource     = {0xbbf1bb18, 0xe47a, 0x48eb, {0x88, 0x07, 0xf8, 0xf8, 0xd3, 0xea, 0x45, 0x51}};
static const GUID IID_ICorePointerInputSource_Alt = {0xbcf7b858, 0xe4b2, 0x48fb, {0x88, 0x07, 0xf8, 0xf8, 0xd3, 0xea, 0x45, 0x51}};

struct input_handler_entry
{
    void *handler;
    INT64 token;
};

struct core_independent_input_source
{
    struct { const void *lpVtbl; } ICoreInputSourceBase_iface;
    struct { const void *lpVtbl; } ICorePointerInputSource_iface;
    LONG ref;
    struct input_handler_entry pointer_pressed[16];
    int pointer_pressed_count;
    struct input_handler_entry pointer_released[16];
    int pointer_released_count;
    struct input_handler_entry pointer_moved[16];
    int pointer_moved_count;
    struct input_handler_entry pointer_wheel[16];
    int pointer_wheel_count;
    INT64 next_token;
};

#define impl_from_ICoreInputSourceBase(ptr) \
    ((struct core_independent_input_source *)((char *)(ptr) - offsetof(struct core_independent_input_source, ICoreInputSourceBase_iface)))
#define impl_from_ICorePointerInputSource(ptr) \
    ((struct core_independent_input_source *)((char *)(ptr) - offsetof(struct core_independent_input_source, ICorePointerInputSource_iface)))

static struct core_independent_input_source *global_independent_input_source = NULL;

static HRESULT WINAPI input_source_QI(void *This, REFIID riid, void **ppv)
{
    struct core_independent_input_source *impl = impl_from_ICoreInputSourceBase(This);
    if (!ppv) return E_POINTER;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_ICoreInputSourceBase))
    {
        *ppv = &impl->ICoreInputSourceBase_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_ICorePointerInputSource) ||
        IsEqualGUID(riid, &IID_ICorePointerInputSource_Alt))
    {
        *ppv = &impl->ICorePointerInputSource_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    TRACE("input_source %p unhandled QI %s\n", impl, debugstr_guid(riid));
    *ppv = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI input_source_AddRef(void *This)
{
    return InterlockedIncrement(&impl_from_ICoreInputSourceBase(This)->ref);
}

static ULONG WINAPI input_source_Release(void *This)
{
    return InterlockedDecrement(&impl_from_ICoreInputSourceBase(This)->ref);
}

static HRESULT WINAPI input_source_GetIids(void *This, ULONG *count, IID **iids)
{
    if (count) *count = 0;
    return S_OK;
}

static HRESULT WINAPI input_source_GetRuntimeClassName(void *This, HSTRING *className)
{
    return WindowsCreateString(L"Windows.UI.Core.CoreIndependentInputSource", 42, className);
}

static HRESULT WINAPI input_source_GetTrustLevel(void *This, TrustLevel *trustLevel)
{
    if (trustLevel) *trustLevel = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI input_source_get_Dispatcher(void *This, void **value)
{
    TRACE("CoreIndependentInputSource get_Dispatcher\n");
    return get_global_dispatcher(value);
}

static HRESULT WINAPI input_source_get_IsInputEnabled(void *This, boolean *value)
{
    if (value) *value = TRUE;
    return S_OK;
}

static HRESULT WINAPI input_source_put_IsInputEnabled(void *This, boolean value)
{
    return S_OK;
}

static HRESULT WINAPI input_source_add_InputEnabled(void *This, void *handler, EventRegistrationToken *token)
{
    if (token) token->value = 1;
    return S_OK;
}

static HRESULT WINAPI input_source_remove_InputEnabled(void *This, EventRegistrationToken token)
{
    return S_OK;
}

static const void *core_input_source_base_vtbl[] = {
    (void *)input_source_QI,
    (void *)input_source_AddRef,
    (void *)input_source_Release,
    (void *)input_source_GetIids,
    (void *)input_source_GetRuntimeClassName,
    (void *)input_source_GetTrustLevel,
    (void *)input_source_get_Dispatcher,
    (void *)input_source_get_IsInputEnabled,
    (void *)input_source_put_IsInputEnabled,
    (void *)input_source_add_InputEnabled,
    (void *)input_source_remove_InputEnabled
};

/* ICorePointerInputSource implementation */
static HRESULT WINAPI ptr_input_QI(void *This, REFIID riid, void **ppv)
{
    return input_source_QI(&impl_from_ICorePointerInputSource(This)->ICoreInputSourceBase_iface, riid, ppv);
}

static ULONG WINAPI ptr_input_AddRef(void *This)
{
    return input_source_AddRef(&impl_from_ICorePointerInputSource(This)->ICoreInputSourceBase_iface);
}

static ULONG WINAPI ptr_input_Release(void *This)
{
    return input_source_Release(&impl_from_ICorePointerInputSource(This)->ICoreInputSourceBase_iface);
}

static HRESULT WINAPI ptr_input_get_HasCapture(void *This, boolean *value)
{
    if (value) *value = FALSE;
    return S_OK;
}

static HRESULT WINAPI ptr_input_get_PointerPosition(void *This, void *value)
{
    FLOAT *coords = (FLOAT *)value;
    if (coords) { coords[0] = 0.0f; coords[1] = 0.0f; }
    return S_OK;
}

static HRESULT WINAPI ptr_input_get_PointerCursor(void *This, void **value)
{
    if (value) *value = NULL;
    return S_OK;
}

static HRESULT WINAPI ptr_input_put_PointerCursor(void *This, void *value)
{
    return S_OK;
}

static HRESULT WINAPI ptr_input_ReleasePointerCapture(void *This)
{
    return S_OK;
}

static HRESULT WINAPI ptr_input_SetPointerCapture(void *This)
{
    return S_OK;
}

static HRESULT WINAPI ptr_input_add_PointerCaptureLost(void *This, void *handler, EventRegistrationToken *token)
{
    if (token) token->value = 1;
    return S_OK;
}

static HRESULT WINAPI ptr_input_remove_PointerCaptureLost(void *This, EventRegistrationToken token)
{
    return S_OK;
}

static HRESULT WINAPI ptr_input_add_PointerEntered(void *This, void *handler, EventRegistrationToken *token)
{
    if (token) token->value = 1;
    return S_OK;
}

static HRESULT WINAPI ptr_input_remove_PointerEntered(void *This, EventRegistrationToken token)
{
    return S_OK;
}

static HRESULT WINAPI ptr_input_add_PointerExited(void *This, void *handler, EventRegistrationToken *token)
{
    if (token) token->value = 1;
    return S_OK;
}

static HRESULT WINAPI ptr_input_remove_PointerExited(void *This, EventRegistrationToken token)
{
    return S_OK;
}

static HRESULT WINAPI ptr_input_add_PointerMoved(void *This, void *handler, EventRegistrationToken *token)
{
    struct core_independent_input_source *impl = impl_from_ICorePointerInputSource(This);
    TRACE("ICorePointerInputSource add_PointerMoved handler=%p\n", handler);
    if (!handler) return E_POINTER;
    if (impl->pointer_moved_count < 16)
    {
        INT64 tok = ++impl->next_token;
        impl->pointer_moved[impl->pointer_moved_count].handler = handler;
        impl->pointer_moved[impl->pointer_moved_count].token = tok;
        impl->pointer_moved_count++;
        IUnknown_AddRef((IUnknown *)handler);
        if (token) token->value = tok;
    }
    return S_OK;
}

static HRESULT WINAPI ptr_input_remove_PointerMoved(void *This, EventRegistrationToken token)
{
    return S_OK;
}

static HRESULT WINAPI ptr_input_add_PointerPressed(void *This, void *handler, EventRegistrationToken *token)
{
    struct core_independent_input_source *impl = impl_from_ICorePointerInputSource(This);
    TRACE("ICorePointerInputSource add_PointerPressed handler=%p\n", handler);
    if (!handler) return E_POINTER;
    if (impl->pointer_pressed_count < 16)
    {
        INT64 tok = ++impl->next_token;
        impl->pointer_pressed[impl->pointer_pressed_count].handler = handler;
        impl->pointer_pressed[impl->pointer_pressed_count].token = tok;
        impl->pointer_pressed_count++;
        IUnknown_AddRef((IUnknown *)handler);
        if (token) token->value = tok;
    }
    return S_OK;
}

static HRESULT WINAPI ptr_input_remove_PointerPressed(void *This, EventRegistrationToken token)
{
    return S_OK;
}

static HRESULT WINAPI ptr_input_add_PointerReleased(void *This, void *handler, EventRegistrationToken *token)
{
    struct core_independent_input_source *impl = impl_from_ICorePointerInputSource(This);
    TRACE("ICorePointerInputSource add_PointerReleased handler=%p\n", handler);
    if (!handler) return E_POINTER;
    if (impl->pointer_released_count < 16)
    {
        INT64 tok = ++impl->next_token;
        impl->pointer_released[impl->pointer_released_count].handler = handler;
        impl->pointer_released[impl->pointer_released_count].token = tok;
        impl->pointer_released_count++;
        IUnknown_AddRef((IUnknown *)handler);
        if (token) token->value = tok;
    }
    return S_OK;
}

static HRESULT WINAPI ptr_input_remove_PointerReleased(void *This, EventRegistrationToken token)
{
    return S_OK;
}

static HRESULT WINAPI ptr_input_add_PointerWheelChanged(void *This, void *handler, EventRegistrationToken *token)
{
    struct core_independent_input_source *impl = impl_from_ICorePointerInputSource(This);
    TRACE("ICorePointerInputSource add_PointerWheelChanged handler=%p\n", handler);
    if (!handler) return E_POINTER;
    if (impl->pointer_wheel_count < 16)
    {
        INT64 tok = ++impl->next_token;
        impl->pointer_wheel[impl->pointer_wheel_count].handler = handler;
        impl->pointer_wheel[impl->pointer_wheel_count].token = tok;
        impl->pointer_wheel_count++;
        IUnknown_AddRef((IUnknown *)handler);
        if (token) token->value = tok;
    }
    return S_OK;
}

static HRESULT WINAPI ptr_input_remove_PointerWheelChanged(void *This, EventRegistrationToken token)
{
    return S_OK;
}

static const void *core_pointer_input_source_vtbl[] = {
    (void *)ptr_input_QI,                         /* 0 */
    (void *)ptr_input_AddRef,                     /* 1 */
    (void *)ptr_input_Release,                    /* 2 */
    (void *)input_source_GetIids,                 /* 3 */
    (void *)input_source_GetRuntimeClassName,     /* 4 */
    (void *)input_source_GetTrustLevel,           /* 5 */
    (void *)ptr_input_get_HasCapture,             /* 6 */
    (void *)ptr_input_get_PointerPosition,        /* 7 */
    (void *)ptr_input_get_PointerCursor,          /* 8 */
    (void *)ptr_input_put_PointerCursor,          /* 9 */
    (void *)ptr_input_ReleasePointerCapture,      /* 10 */
    (void *)ptr_input_SetPointerCapture,          /* 11 */
    (void *)ptr_input_add_PointerCaptureLost,     /* 12 */
    (void *)ptr_input_remove_PointerCaptureLost,  /* 13 */
    (void *)ptr_input_add_PointerEntered,         /* 14 */
    (void *)ptr_input_remove_PointerEntered,      /* 15 */
    (void *)ptr_input_add_PointerExited,          /* 16 */
    (void *)ptr_input_remove_PointerExited,       /* 17 */
    (void *)ptr_input_add_PointerMoved,           /* 18 */
    (void *)ptr_input_remove_PointerMoved,        /* 19 */
    (void *)ptr_input_add_PointerPressed,         /* 20 */
    (void *)ptr_input_remove_PointerPressed,      /* 21 */
    (void *)ptr_input_add_PointerReleased,        /* 22 */
    (void *)ptr_input_remove_PointerReleased,     /* 23 */
    (void *)ptr_input_add_PointerWheelChanged,    /* 24 */
    (void *)ptr_input_remove_PointerWheelChanged  /* 25 */
};

int core_independent_input_source_has_handlers(UINT msg)
{
    struct core_independent_input_source *impl = global_independent_input_source;
    if (!impl) return 0;
    switch (msg)
    {
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
            return impl->pointer_pressed_count > 0;
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
            return impl->pointer_released_count > 0;
        case WM_MOUSEMOVE:
            return impl->pointer_moved_count > 0;
        case WM_MOUSEWHEEL:
            return impl->pointer_wheel_count > 0;
        default:
            return 0;
    }
}

void core_independent_input_source_fire_pointer(UINT msg, WPARAM wparam, LPARAM lparam, void *args)
{
    struct core_independent_input_source *impl = global_independent_input_source;
    struct input_handler_entry *handlers = NULL;
    int count = 0, i;

    if (!impl || !args) return;

    switch (msg)
    {
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
            handlers = impl->pointer_pressed;
            count = impl->pointer_pressed_count;
            break;
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
            handlers = impl->pointer_released;
            count = impl->pointer_released_count;
            break;
        case WM_MOUSEMOVE:
            handlers = impl->pointer_moved;
            count = impl->pointer_moved_count;
            break;
        case WM_MOUSEWHEEL:
            handlers = impl->pointer_wheel;
            count = impl->pointer_wheel_count;
            break;
        default:
            return;
    }

    for (i = 0; i < count; i++)
    {
        struct typed_event_handler *h = (struct typed_event_handler *)handlers[i].handler;
        if (h && h->lpVtbl && h->lpVtbl->Invoke)
        {
            TRACE("Invoking independent input handler[%d] %p (msg=0x%x)\n", i, h, msg);
            h->lpVtbl->Invoke(h, &impl->ICorePointerInputSource_iface, args);
        }
    }
}

static HRESULT create_core_independent_input_source(void **ret)
{
    struct core_independent_input_source *impl;
    if (!ret) return E_POINTER;
    if (global_independent_input_source)
    {
        InterlockedIncrement(&global_independent_input_source->ref);
        *ret = &global_independent_input_source->ICoreInputSourceBase_iface;
        return S_OK;
    }
    impl = calloc(1, sizeof(*impl));
    if (!impl) return E_OUTOFMEMORY;
    impl->ICoreInputSourceBase_iface.lpVtbl = core_input_source_base_vtbl;
    impl->ICorePointerInputSource_iface.lpVtbl = core_pointer_input_source_vtbl;
    impl->ref = 10000; /* Persistent singleton */
    global_independent_input_source = impl;
    *ret = &impl->ICoreInputSourceBase_iface;
    return S_OK;
}

static HRESULT WINAPI elem_scp_CreateCoreIndependentInputSource(void *This, int deviceTypes, void **ret)
{
    TRACE("SwapChainPanel %p CreateCoreIndependentInputSource deviceTypes=0x%x, ret=%p\n", This, deviceTypes, ret);
    return create_core_independent_input_source(ret);
}

static HRESULT WINAPI elem_scp_native_QI(void *This, REFIID riid, void **ppv) { return elem_QI_internal(impl_from_ISwapChainPanelNative(This), riid, ppv); }
static ULONG WINAPI elem_scp_native_AddRef(void *This) { return elem_AddRef_internal(impl_from_ISwapChainPanelNative(This)); }
static ULONG WINAPI elem_scp_native_Release(void *This) { return elem_Release_internal(impl_from_ISwapChainPanelNative(This)); }
static HRESULT WINAPI elem_scp_native_SetSwapChain(void *This, void *swapchain)
{
    struct xaml_element *impl = impl_from_ISwapChainPanelNative(This);
    RECT rect;
    FLOAT width = 1024.0f, height = 768.0f;

    TRACE("SwapChainPanel %p SetSwapChain(%p)\n", impl, swapchain);
    impl->swapchain = swapchain;
    if (swapchain) global_swapchain_active = TRUE;
    global_swapchain_panel = impl;
    if (!global_root_element) global_root_element = impl;

    if (global_xaml_hwnd && GetClientRect(global_xaml_hwnd, &rect))
    {
        width = (FLOAT)(rect.right - rect.left);
        height = (FLOAT)(rect.bottom - rect.top);
    }
    if (width > 0.0f && height > 0.0f)
    {
        TRACE("Firing SizeChanged on SwapChainPanel %p upon SetSwapChain: %.0f x %.0f\n", impl, width, height);
        elem_fire_size_changed(impl, 0.0f, 0.0f, width, height);
    }
    return S_OK;
}
static HRESULT WINAPI elem_scp_native_SetSwapChainHandle(void *This, HANDLE handle)
{
    struct xaml_element *impl = impl_from_ISwapChainPanelNative(This);
    TRACE("SwapChainPanel %p SetSwapChainHandle(%p)\n", impl, handle);
    return S_OK;
}

static HRESULT WINAPI elem_ctrl_QI(void *This, REFIID riid, void **ppv) { return elem_QI_internal(impl_from_IControl(This), riid, ppv); }
static ULONG WINAPI elem_ctrl_AddRef(void *This) { return elem_AddRef_internal(impl_from_IControl(This)); }
static ULONG WINAPI elem_ctrl_Release(void *This) { return elem_Release_internal(impl_from_IControl(This)); }
static HRESULT WINAPI elem_ctrl_GetRuntimeClassName(void *This, HSTRING *n) { return elem_ui_GetRuntimeClassName(&impl_from_IControl(This)->IUIElement_iface, n); }
static HRESULT WINAPI elem_ctrl_Focus(void *This, int value, boolean *returnValue)
{
    struct xaml_element *impl = impl_from_IControl(This);
    TRACE("elem %p (%s) Focus(state=%d)\n", impl, debugstr_w(impl->name), value);
    if (returnValue) *returnValue = TRUE;
    return S_OK;
}

/* -------------------------------------------------------------------------
 * IControlProtected VTable
 * ------------------------------------------------------------------------- */
static HRESULT WINAPI elem_ctrl_prot_QI(void *This, REFIID riid, void **ppv) { return elem_QI_internal(impl_from_IControlProtected(This), riid, ppv); }
static ULONG WINAPI elem_ctrl_prot_AddRef(void *This) { return elem_AddRef_internal(impl_from_IControlProtected(This)); }
static ULONG WINAPI elem_ctrl_prot_Release(void *This) { return elem_Release_internal(impl_from_IControlProtected(This)); }
static HRESULT WINAPI elem_ctrl_prot_GetRuntimeClassName(void *This, HSTRING *n) { return elem_ui_GetRuntimeClassName(&impl_from_IControlProtected(This)->IUIElement_iface, n); }

static HRESULT WINAPI elem_ctrl_prot_get_DefaultStyleKey(void *This, void **value)
{
    struct xaml_element *elem = impl_from_IControlProtected(This);
    TRACE("elem %p (%s) get_DefaultStyleKey\n", elem, debugstr_w(elem->name));
    if (value) *value = NULL;
    return S_OK;
}

static HRESULT WINAPI elem_ctrl_prot_put_DefaultStyleKey(void *This, void *value)
{
    struct xaml_element *elem = impl_from_IControlProtected(This);
    TRACE("elem %p (%s) put_DefaultStyleKey %p\n", elem, debugstr_w(elem->name), value);
    return S_OK;
}

static HRESULT WINAPI elem_ctrl_prot_GetTemplateChild(void *This, HSTRING childName, void **returnValue)
{
    struct xaml_element *elem = impl_from_IControlProtected(This);
    const WCHAR *str = childName ? WindowsGetStringRawBuffer(childName, NULL) : NULL;
    struct xaml_element *found = NULL;

    TRACE("elem %p (%s), GetTemplateChild %s\n", elem, debugstr_w(elem->name), debugstr_w(str));
    if (!returnValue) return E_POINTER;
    *returnValue = NULL;

    if (str && str[0])
    {
        found = xaml_find_element_by_name(elem, str);
        if (!found && global_root_element)
            found = xaml_find_element_by_name(global_root_element, str);
    }

    if (found)
    {
        *returnValue = &found->IDependencyObject_iface;
        elem_AddRef_internal(found);
    }
    return S_OK;
}

static HRESULT WINAPI elem_cc_GetRuntimeClassName(void *This, HSTRING *n) { return elem_ui_GetRuntimeClassName(&impl_from_IContentControl(This)->IUIElement_iface, n); }

static HRESULT WINAPI elem_uc_QI(void *This, REFIID riid, void **ppv) { return elem_QI_internal(impl_from_IUserControl(This), riid, ppv); }
static ULONG WINAPI elem_uc_AddRef(void *This) { return elem_AddRef_internal(impl_from_IUserControl(This)); }
static ULONG WINAPI elem_uc_Release(void *This) { return elem_Release_internal(impl_from_IUserControl(This)); }
static HRESULT WINAPI elem_uc_GetRuntimeClassName(void *This, HSTRING *n) { return elem_ui_GetRuntimeClassName(&impl_from_IUserControl(This)->IUIElement_iface, n); }

static HRESULT WINAPI elem_panel_GetRuntimeClassName(void *This, HSTRING *n) { return elem_ui_GetRuntimeClassName(&impl_from_IPanel(This)->IUIElement_iface, n); }
static HRESULT WINAPI elem_grid_GetRuntimeClassName(void *This, HSTRING *n) { return elem_ui_GetRuntimeClassName(&impl_from_IGrid(This)->IUIElement_iface, n); }
static HRESULT WINAPI elem_border_GetRuntimeClassName(void *This, HSTRING *n) { return elem_ui_GetRuntimeClassName(&impl_from_IBorder(This)->IUIElement_iface, n); }
static HRESULT WINAPI elem_tb_GetRuntimeClassName(void *This, HSTRING *n) { return elem_ui_GetRuntimeClassName(&impl_from_ITextBlock(This)->IUIElement_iface, n); }

static HRESULT WINAPI elem_do_QI(void *This, REFIID riid, void **ppv) { return elem_QI_internal(impl_from_IDependencyObject(This), riid, ppv); }
static ULONG WINAPI elem_do_AddRef(void *This) { return elem_AddRef_internal(impl_from_IDependencyObject(This)); }
static ULONG WINAPI elem_do_Release(void *This) { return elem_Release_internal(impl_from_IDependencyObject(This)); }
static HRESULT WINAPI elem_do_GetRuntimeClassName(void *This, HSTRING *n) { return elem_ui_GetRuntimeClassName(&impl_from_IDependencyObject(This)->IUIElement_iface, n); }
static HRESULT WINAPI elem_do_GetValue(void *This, void *dp, IInspectable **result)
{
    struct xaml_element *impl = impl_from_IDependencyObject(This);
    UINT32 i;
    TRACE("elem_do %p (elem %p %s), dp %p, result %p\n", This, impl, debugstr_w(impl->name), dp, result);
    if (!result) return E_POINTER;
    *result = NULL;
    for (i = 0; i < impl->dp_count; i++)
    {
        if (impl->dp_entries[i].dp == dp)
        {
            *result = impl->dp_entries[i].value;
            if (*result) IInspectable_AddRef(*result);
            TRACE("elem_do found stored value %p for dp %p\n", *result, dp);
            return S_OK;
        }
    }
    return dependency_property_get_default_value(dp, result);
}
static HRESULT WINAPI elem_do_SetValue(void *This, void *dp, IInspectable *value)
{
    struct xaml_element *impl = impl_from_IDependencyObject(This);
    UINT32 i;
    TRACE("elem_do %p (elem %p %s), dp %p, value %p\n", This, impl, debugstr_w(impl->name), dp, value);
    if (!dp) return E_POINTER;
    for (i = 0; i < impl->dp_count; i++)
    {
        if (impl->dp_entries[i].dp == dp)
        {
            if (impl->dp_entries[i].value) IInspectable_Release(impl->dp_entries[i].value);
            impl->dp_entries[i].value = value;
            if (value) IInspectable_AddRef(value);
            return S_OK;
        }
    }
    if (impl->dp_count < ARRAY_SIZE(impl->dp_entries))
    {
        impl->dp_entries[impl->dp_count].dp = dp;
        impl->dp_entries[impl->dp_count].value = value;
        if (value) IInspectable_AddRef(value);
        impl->dp_count++;
    }
    return S_OK;
}
static HRESULT WINAPI elem_do_ClearValue(void *This, void *dp)
{
    struct xaml_element *impl = impl_from_IDependencyObject(This);
    UINT32 i;
    TRACE("elem_do %p, dp %p\n", This, dp);
    for (i = 0; i < impl->dp_count; i++)
    {
        if (impl->dp_entries[i].dp == dp)
        {
            if (impl->dp_entries[i].value) IInspectable_Release(impl->dp_entries[i].value);
            for (; i + 1 < impl->dp_count; i++)
                impl->dp_entries[i] = impl->dp_entries[i + 1];
            impl->dp_count--;
            break;
        }
    }
    return S_OK;
}
static HRESULT WINAPI elem_do_get_Dispatcher(void *This, void **result)
{
    TRACE("elem_do %p, dispatcher %p\n", This, result);
    if (!result) return E_POINTER;
    return get_global_dispatcher(result);
}

static HRESULT WINAPI elem_do2_QI(void *This, REFIID riid, void **ppv) { return elem_QI_internal(impl_from_IDependencyObject2(This), riid, ppv); }
static ULONG WINAPI elem_do2_AddRef(void *This) { return elem_AddRef_internal(impl_from_IDependencyObject2(This)); }
static ULONG WINAPI elem_do2_Release(void *This) { return elem_Release_internal(impl_from_IDependencyObject2(This)); }
static HRESULT WINAPI elem_do2_RegisterPropertyChangedCallback(void *This, void *dp, void *callback, INT64 *result)
{
    TRACE("elem_do2 %p, dp %p, callback %p\n", This, dp, callback);
    if (!result) return E_POINTER;
    *result = 1;
    return S_OK;
}
static HRESULT WINAPI elem_do2_UnregisterPropertyChangedCallback(void *This, void *dp, INT64 token)
{
    TRACE("elem_do2 %p, dp %p, token %lld\n", This, dp, token);
    return S_OK;
}

static HRESULT WINAPI elem_redhawk_QI(void *This, REFIID riid, void **ppv)
{
    TRACE("elem_redhawk_QI This %p, riid %s\n", This, debugstr_guid(riid));
    return elem_QI_internal(impl_from_IRedhawkInterop(This), riid, ppv);
}
static ULONG WINAPI elem_redhawk_AddRef(void *This)
{
    TRACE("elem_redhawk_AddRef This %p\n", This);
    return elem_AddRef_internal(impl_from_IRedhawkInterop(This));
}
static ULONG WINAPI elem_redhawk_Release(void *This)
{
    TRACE("elem_redhawk_Release This %p\n", This);
    return elem_Release_internal(impl_from_IRedhawkInterop(This));
}
static HRESULT WINAPI elem_redhawk_GetIids(void *This, ULONG *c, IID **i)
{
    TRACE("elem_redhawk_GetIids This %p\n", This);
    if (i) *i = NULL; if (c) *c = 0; return S_OK;
}
static HRESULT WINAPI elem_redhawk_GetRuntimeClassName(void *This, HSTRING *n)
{
    TRACE("elem_redhawk_GetRuntimeClassName This %p\n", This);
    return elem_ui_GetRuntimeClassName(&impl_from_IRedhawkInterop(This)->IUIElement_iface, n);
}
static HRESULT WINAPI elem_redhawk_GetTrustLevel(void *This, TrustLevel *t)
{
    TRACE("elem_redhawk_GetTrustLevel This %p\n", This);
    if (t) *t = BaseTrust; return S_OK;
}

#define DEFINE_REDHAWK_SLOT(slot) \
static HRESULT WINAPI elem_redhawk_slot_##slot(void *This, void *a1, void *a2, void *a3) { \
    TRACE("elem_redhawk slot " #slot " This %p, args (%p, %p, %p), ret %p\n", This, a1, a2, a3, __builtin_return_address(0)); \
    return S_OK; \
}

DEFINE_REDHAWK_SLOT(3)
DEFINE_REDHAWK_SLOT(4)
DEFINE_REDHAWK_SLOT(5)
DEFINE_REDHAWK_SLOT(6)
DEFINE_REDHAWK_SLOT(7)
DEFINE_REDHAWK_SLOT(8)
DEFINE_REDHAWK_SLOT(9)
DEFINE_REDHAWK_SLOT(10)
DEFINE_REDHAWK_SLOT(11)
DEFINE_REDHAWK_SLOT(12)
DEFINE_REDHAWK_SLOT(13)
DEFINE_REDHAWK_SLOT(14)
DEFINE_REDHAWK_SLOT(15)

static HRESULT WINAPI elem_flyout_QI(void *This, REFIID riid, void **ppv) { return elem_QI_internal(impl_from_IFlyout(This), riid, ppv); }
static ULONG WINAPI elem_flyout_AddRef(void *This) { return elem_AddRef_internal(impl_from_IFlyout(This)); }
static ULONG WINAPI elem_flyout_Release(void *This) { return elem_Release_internal(impl_from_IFlyout(This)); }
static HRESULT WINAPI elem_flyout_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.Controls.Flyout", 31, n); }

static HRESULT WINAPI elem_flyout_get_Content(void *This, void **v)
{
    struct xaml_element *impl = impl_from_IFlyout(This);
    if (!v) return E_POINTER;
    *v = impl->child_content ? (void *)&impl->child_content->IUIElement_iface : NULL;
    if (*v) elem_AddRef_internal(impl->child_content);
    return S_OK;
}

static HRESULT WINAPI elem_flyout_put_Content(void *This, void *v)
{
    struct xaml_element *impl = impl_from_IFlyout(This);
    if (impl->child_content) elem_Release_internal(impl->child_content);
    impl->child_content = (struct xaml_element *)v;
    if (v) elem_AddRef_internal(impl->child_content);
    return S_OK;
}

static HRESULT WINAPI elem_flyout_get_FlyoutPresenterStyle(void *This, void **v)
{
    struct xaml_element *impl = impl_from_IFlyout(This);
    if (!v) return E_POINTER;
    if (!impl->flyout_style)
        impl->flyout_style = xaml_style_create();
    *v = impl->flyout_style;
    if (*v) IUnknown_AddRef((IUnknown *)*v);
    TRACE("Flyout %p: get_FlyoutPresenterStyle -> %p\n", impl, *v);
    return S_OK;
}

static HRESULT WINAPI elem_flyout_put_FlyoutPresenterStyle(void *This, void *v)
{
    struct xaml_element *impl = impl_from_IFlyout(This);
    if (impl->flyout_style) xaml_style_release(impl->flyout_style);
    impl->flyout_style = (struct xaml_style *)v;
    if (v) IUnknown_AddRef((IUnknown *)v);
    return S_OK;
}

static HRESULT WINAPI elem_flyout_base_QI(void *This, REFIID riid, void **ppv) { return elem_QI_internal(impl_from_IFlyoutBase(This), riid, ppv); }
static ULONG WINAPI elem_flyout_base_AddRef(void *This) { return elem_AddRef_internal(impl_from_IFlyoutBase(This)); }
static ULONG WINAPI elem_flyout_base_Release(void *This) { return elem_Release_internal(impl_from_IFlyoutBase(This)); }
static HRESULT WINAPI elem_flyout_base_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.Controls.Primitives.FlyoutBase", 46, n); }

static HRESULT WINAPI elem_flyout_base_get_Placement(void *This, INT32 *v) { if (v) *v = 0; return S_OK; }
static HRESULT WINAPI elem_flyout_base_put_Placement(void *This, INT32 v) { return S_OK; }
static HRESULT WINAPI elem_flyout_base_add_evt(void *This, void *handler, UINT64 *token) { if (token) *token = 1; return S_OK; }
static HRESULT WINAPI elem_flyout_base_rem_evt(void *This, UINT64 token) { return S_OK; }
static HRESULT WINAPI elem_flyout_base_ShowAt(void *This, void *target) { return S_OK; }
static HRESULT WINAPI elem_flyout_base_Hide(void *This) { return S_OK; }

/* -------------------------------------------------------------------------
 * MenuFlyout Item Collection (IVector<MenuFlyoutItemBase*>)
 * ------------------------------------------------------------------------- */
struct menuflyout_vec_impl
{
    struct { const void *lpVtbl; } IVector_iface;
    LONG ref;
    struct xaml_element *owner;
};

#define impl_from_menuflyout_IVector(ptr) ((struct menuflyout_vec_impl *)((char *)(ptr) - offsetof(struct menuflyout_vec_impl, IVector_iface)))

static HRESULT WINAPI menuflyout_vec_QI(void *This, REFIID riid, void **ppv)
{
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject_local) ||
        IsEqualGUID(riid, &IID_IVector_MenuFlyoutItemBase) ||
        IsEqualGUID(riid, &IID_IIterable_MenuFlyoutItemBase))
    {
        *ppv = This;
        InterlockedIncrement(&impl_from_menuflyout_IVector(This)->ref);
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI menuflyout_vec_AddRef(void *This) { return InterlockedIncrement(&impl_from_menuflyout_IVector(This)->ref); }
static ULONG WINAPI menuflyout_vec_Release(void *This)
{
    struct menuflyout_vec_impl *impl = impl_from_menuflyout_IVector(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}
static HRESULT WINAPI menuflyout_vec_GetRuntimeClassName(void *This, HSTRING *n)
{
    return WindowsCreateString(L"Windows.Foundation.Collections.IVector`1<Windows.UI.Xaml.Controls.MenuFlyoutItemBase>", 80, n);
}
static HRESULT WINAPI menuflyout_vec_get_Size(void *This, UINT32 *size)
{
    struct menuflyout_vec_impl *impl = impl_from_menuflyout_IVector(This);
    if (!size) return E_POINTER;
    *size = impl->owner ? impl->owner->children_count : 0;
    return S_OK;
}
static HRESULT WINAPI menuflyout_vec_GetAt(void *This, UINT32 index, void **item)
{
    struct menuflyout_vec_impl *impl = impl_from_menuflyout_IVector(This);
    if (!item) return E_POINTER;
    *item = NULL;
    if (!impl->owner || index >= impl->owner->children_count) return E_BOUNDS;
    *item = &impl->owner->children[index]->IMenuFlyoutItemBase_iface;
    elem_AddRef_internal(impl->owner->children[index]);
    return S_OK;
}

static const void *menuflyout_vec_vtbl[16] = {
    (void *)menuflyout_vec_QI,
    (void *)menuflyout_vec_AddRef,
    (void *)menuflyout_vec_Release,
    (void *)vec_GetIids,
    (void *)menuflyout_vec_GetRuntimeClassName,
    (void *)vec_GetTrustLevel,
    (void *)menuflyout_vec_GetAt,        /* 6 */
    (void *)menuflyout_vec_get_Size,     /* 7 */
    (void *)elem_stub_ret_zero,          /* 8 GetView */
    (void *)elem_stub_ret_zero,          /* 9 IndexOf */
    (void *)elem_stub_ret_ok,            /* 10 SetAt */
    (void *)elem_stub_ret_ok,            /* 11 InsertAt */
    (void *)elem_stub_ret_ok,            /* 12 RemoveAt */
    (void *)elem_stub_ret_ok,            /* 13 Append */
    (void *)elem_stub_ret_ok,            /* 14 RemoveAtEnd */
    (void *)elem_stub_ret_ok             /* 15 Clear */
};

static void *create_menuflyout_collection(struct xaml_element *owner)
{
    struct menuflyout_vec_impl *impl = calloc(1, sizeof(*impl));
    if (!impl) return NULL;
    impl->IVector_iface.lpVtbl = menuflyout_vec_vtbl;
    impl->ref = 1;
    impl->owner = owner;
    return &impl->IVector_iface;
}

static HRESULT WINAPI elem_menuflyout_QI(void *This, REFIID riid, void **ppv) { return elem_QI_internal(impl_from_IMenuFlyout(This), riid, ppv); }
static ULONG WINAPI elem_menuflyout_AddRef(void *This) { return elem_AddRef_internal(impl_from_IMenuFlyout(This)); }
static ULONG WINAPI elem_menuflyout_Release(void *This) { return elem_Release_internal(impl_from_IMenuFlyout(This)); }
static HRESULT WINAPI elem_menuflyout_GetRuntimeClassName(void *This, HSTRING *n)
{
    struct xaml_element *elem = impl_from_IMenuFlyout(This);
    TRACE("MenuFlyout %p (%s) GetRuntimeClassName\n", elem, debugstr_w(elem->name));
    return WindowsCreateString(L"Windows.UI.Xaml.Controls.MenuFlyout", 35, n);
}

static HRESULT WINAPI elem_menuflyout_get_Items(void *This, void **v)
{
    struct xaml_element *impl = impl_from_IMenuFlyout(This);
    if (!v) return E_POINTER;
    *v = create_menuflyout_collection(impl);
    return *v ? S_OK : E_OUTOFMEMORY;
}

static HRESULT WINAPI elem_menuflyout_get_MenuFlyoutPresenterStyle(void *This, void **v)
{
    struct xaml_element *impl = impl_from_IMenuFlyout(This);
    if (!v) return E_POINTER;
    if (!impl->flyout_style)
        impl->flyout_style = xaml_style_create();
    *v = impl->flyout_style;
    if (*v) IUnknown_AddRef((IUnknown *)*v);
    TRACE("MenuFlyout %p: get_MenuFlyoutPresenterStyle -> %p\n", impl, *v);
    return S_OK;
}

static HRESULT WINAPI elem_menuflyout_put_MenuFlyoutPresenterStyle(void *This, void *v)
{
    return S_OK;
}

static HRESULT WINAPI elem_menuflyout2_QI(void *This, REFIID riid, void **ppv) { return elem_QI_internal(impl_from_IMenuFlyout2(This), riid, ppv); }
static ULONG WINAPI elem_menuflyout2_AddRef(void *This) { return elem_AddRef_internal(impl_from_IMenuFlyout2(This)); }
static ULONG WINAPI elem_menuflyout2_Release(void *This) { return elem_Release_internal(impl_from_IMenuFlyout2(This)); }
static HRESULT WINAPI elem_menuflyout2_ShowAt(void *This, void *target, double pt) { return S_OK; }

/* -------------------------------------------------------------------------
 * MenuFlyoutItem Implementation
 * ------------------------------------------------------------------------- */
static HRESULT WINAPI elem_menuflyoutitem_QI(void *This, REFIID riid, void **ppv) { return elem_QI_internal(impl_from_IMenuFlyoutItem(This), riid, ppv); }
static ULONG WINAPI elem_menuflyoutitem_AddRef(void *This) { return elem_AddRef_internal(impl_from_IMenuFlyoutItem(This)); }
static ULONG WINAPI elem_menuflyoutitem_Release(void *This) { return elem_Release_internal(impl_from_IMenuFlyoutItem(This)); }
static HRESULT WINAPI elem_menuflyoutitem_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.Controls.MenuFlyoutItem", 39, n); }

static HRESULT WINAPI elem_menuflyoutitem_get_Text(void *This, HSTRING *value)
{
    struct xaml_element *elem = impl_from_IMenuFlyoutItem(This);
    if (!value) return E_POINTER;
    return WindowsCreateString(elem->text, lstrlenW(elem->text), value);
}

static HRESULT WINAPI elem_menuflyoutitem_put_Text(void *This, HSTRING value)
{
    struct xaml_element *elem = impl_from_IMenuFlyoutItem(This);
    const WCHAR *str = WindowsGetStringRawBuffer(value, NULL);
    TRACE("(%p)->put_Text(%s)\n", elem, debugstr_w(str));
    if (str) lstrcpynW(elem->text, str, ARRAY_SIZE(elem->text));
    return S_OK;
}

static HRESULT WINAPI elem_menuflyoutitem_get_Command(void *This, void **value)
{
    if (value) *value = NULL;
    return S_OK;
}

static HRESULT WINAPI elem_menuflyoutitem_put_Command(void *This, void *value)
{
    return S_OK;
}

static HRESULT WINAPI elem_menuflyoutitem_get_CommandParameter(void *This, void **value)
{
    if (value) *value = NULL;
    return S_OK;
}

static HRESULT WINAPI elem_menuflyoutitem_put_CommandParameter(void *This, void *value)
{
    return S_OK;
}

static HRESULT WINAPI elem_menuflyoutitem_add_Click(void *This, void *handler, UINT64 *token)
{
    if (token) *token = 1;
    return S_OK;
}

static HRESULT WINAPI elem_menuflyoutitem_remove_Click(void *This, UINT64 token)
{
    return S_OK;
}

static HRESULT WINAPI elem_menuflyoutitem2_QI(void *This, REFIID riid, void **ppv) { return elem_QI_internal(impl_from_IMenuFlyoutItem2(This), riid, ppv); }
static ULONG WINAPI elem_menuflyoutitem2_AddRef(void *This) { return elem_AddRef_internal(impl_from_IMenuFlyoutItem2(This)); }
static ULONG WINAPI elem_menuflyoutitem2_Release(void *This) { return elem_Release_internal(impl_from_IMenuFlyoutItem2(This)); }

static HRESULT WINAPI elem_menuflyoutitembase_QI(void *This, REFIID riid, void **ppv) { return elem_QI_internal(impl_from_IMenuFlyoutItemBase(This), riid, ppv); }
static ULONG WINAPI elem_menuflyoutitembase_AddRef(void *This) { return elem_AddRef_internal(impl_from_IMenuFlyoutItemBase(This)); }
static ULONG WINAPI elem_menuflyoutitembase_Release(void *This) { return elem_Release_internal(impl_from_IMenuFlyoutItemBase(This)); }

static HRESULT WINAPI elem_rowdef_QI(void *This, REFIID riid, void **ppv) { return elem_QI_internal(impl_from_IRowDefinition(This), riid, ppv); }
static ULONG WINAPI elem_rowdef_AddRef(void *This) { return elem_AddRef_internal(impl_from_IRowDefinition(This)); }
static ULONG WINAPI elem_rowdef_Release(void *This) { return elem_Release_internal(impl_from_IRowDefinition(This)); }
static HRESULT WINAPI elem_rowdef_GetRuntimeClassName(void *This, HSTRING *name)
{
    if (!name) return E_POINTER;
    return WindowsCreateString(L"Windows.UI.Xaml.Controls.RowDefinition", 38, name);
}

static HRESULT WINAPI elem_rowdef_get_Height(void *This, struct winrt_grid_length *val)
{
    struct xaml_element *elem = impl_from_IRowDefinition(This);
    if (!val) return E_POINTER;
    val->value = elem->height;
    val->grid_unit_type = elem->row_unit_types[0];
    return S_OK;
}
static HRESULT WINAPI elem_rowdef_put_Height(void *This, struct winrt_grid_length val)
{
    struct xaml_element *elem = impl_from_IRowDefinition(This);
    elem->height = val.value;
    elem->row_unit_types[0] = val.grid_unit_type;
    return S_OK;
}
static HRESULT WINAPI elem_rowdef_get_MaxHeight(void *This, DOUBLE *val)
{
    if (val) *val = 1e9;
    return S_OK;
}
static HRESULT WINAPI elem_rowdef_put_MaxHeight(void *This, DOUBLE val)
{
    return S_OK;
}
static HRESULT WINAPI elem_rowdef_get_MinHeight(void *This, DOUBLE *val)
{
    if (val) *val = 0;
    return S_OK;
}
static HRESULT WINAPI elem_rowdef_put_MinHeight(void *This, DOUBLE val)
{
    return S_OK;
}
static HRESULT WINAPI elem_rowdef_get_ActualHeight(void *This, DOUBLE *val)
{
    struct xaml_element *elem = impl_from_IRowDefinition(This);
    if (val) *val = elem->height;
    return S_OK;
}

static HRESULT WINAPI elem_coldef_QI(void *This, REFIID riid, void **ppv) { return elem_QI_internal(impl_from_IColumnDefinition(This), riid, ppv); }
static ULONG WINAPI elem_coldef_AddRef(void *This) { return elem_AddRef_internal(impl_from_IColumnDefinition(This)); }
static ULONG WINAPI elem_coldef_Release(void *This) { return elem_Release_internal(impl_from_IColumnDefinition(This)); }
static HRESULT WINAPI elem_coldef_GetRuntimeClassName(void *This, HSTRING *name)
{
    if (!name) return E_POINTER;
    return WindowsCreateString(L"Windows.UI.Xaml.Controls.ColumnDefinition", 41, name);
}

static HRESULT WINAPI elem_coldef_get_Width(void *This, struct winrt_grid_length *val)
{
    struct xaml_element *elem = impl_from_IColumnDefinition(This);
    if (!val) return E_POINTER;
    val->value = elem->width;
    val->grid_unit_type = elem->col_unit_types[0];
    return S_OK;
}
static HRESULT WINAPI elem_coldef_put_Width(void *This, struct winrt_grid_length val)
{
    struct xaml_element *elem = impl_from_IColumnDefinition(This);
    elem->width = val.value;
    elem->col_unit_types[0] = val.grid_unit_type;
    return S_OK;
}
static HRESULT WINAPI elem_coldef_get_MaxWidth(void *This, DOUBLE *val)
{
    if (val) *val = 1e9;
    return S_OK;
}
static HRESULT WINAPI elem_coldef_put_MaxWidth(void *This, DOUBLE val)
{
    return S_OK;
}
static HRESULT WINAPI elem_coldef_get_MinWidth(void *This, DOUBLE *val)
{
    if (val) *val = 0;
    return S_OK;
}
static HRESULT WINAPI elem_coldef_put_MinWidth(void *This, DOUBLE val)
{
    return S_OK;
}
static HRESULT WINAPI elem_coldef_get_ActualWidth(void *This, DOUBLE *val)
{
    struct xaml_element *elem = impl_from_IColumnDefinition(This);
    if (val) *val = elem->width;
    return S_OK;
}

/* -------------------------------------------------------------------------
 * AdaptiveTrigger Implementation
 * ------------------------------------------------------------------------- */
#define impl_from_IAdaptiveTrigger(ptr) ((struct xaml_element *)((char *)(ptr) - offsetof(struct xaml_element, IAdaptiveTrigger_iface)))

static HRESULT WINAPI elem_adapt_QI(void *This, REFIID riid, void **ppv) { return elem_QI_internal(impl_from_IAdaptiveTrigger(This), riid, ppv); }
static ULONG WINAPI elem_adapt_AddRef(void *This) { return elem_AddRef_internal(impl_from_IAdaptiveTrigger(This)); }
static ULONG WINAPI elem_adapt_Release(void *This) { return elem_Release_internal(impl_from_IAdaptiveTrigger(This)); }
static HRESULT WINAPI elem_adapt_GetRuntimeClassName(void *This, HSTRING *name)
{
    if (!name) return E_POINTER;
    return WindowsCreateString(L"Windows.UI.Xaml.AdaptiveTrigger", 31, name);
}

static HRESULT WINAPI elem_adapt_get_MinWindowWidth(void *This, DOUBLE *val)
{
    struct xaml_element *elem = impl_from_IAdaptiveTrigger(This);
    if (!val) return E_POINTER;
    *val = elem->min_window_width;
    return S_OK;
}

static HRESULT WINAPI elem_adapt_put_MinWindowWidth(void *This, DOUBLE val)
{
    struct xaml_element *elem = impl_from_IAdaptiveTrigger(This);
    elem->min_window_width = val;
    return S_OK;
}

static HRESULT WINAPI elem_adapt_get_MinWindowHeight(void *This, DOUBLE *val)
{
    struct xaml_element *elem = impl_from_IAdaptiveTrigger(This);
    if (!val) return E_POINTER;
    *val = elem->min_window_height;
    return S_OK;
}

static HRESULT WINAPI elem_adapt_put_MinWindowHeight(void *This, DOUBLE val)
{
    struct xaml_element *elem = impl_from_IAdaptiveTrigger(This);
    TRACE("AdaptiveTrigger %p (%s) put_MinWindowHeight: %f\n", elem, debugstr_w(elem->name), val);
    elem->min_window_height = val;
    return S_OK;
}

/* -------------------------------------------------------------------------
 * Element Weak Reference Implementation
 * ------------------------------------------------------------------------- */
struct elem_weakref
{
    struct { const void *lpVtbl; } IWeakReference_iface;
    LONG ref;
    struct xaml_element *target;
};

#define impl_from_elem_WeakRef(ptr) ((struct elem_weakref *)((char *)(ptr) - offsetof(struct elem_weakref, IWeakReference_iface)))

static HRESULT WINAPI elem_weakref_QI(void *This, REFIID riid, void **ppv)
{
    struct elem_weakref *impl = impl_from_elem_WeakRef(This);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) || IsEqualGUID(riid, &IID_IWeakReference))
    {
        *ppv = &impl->IWeakReference_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI elem_weakref_AddRef(void *This)
{
    struct elem_weakref *impl = impl_from_elem_WeakRef(This);
    return InterlockedIncrement(&impl->ref);
}
static ULONG WINAPI elem_weakref_Release(void *This)
{
    struct elem_weakref *impl = impl_from_elem_WeakRef(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}
static HRESULT WINAPI elem_weakref_Resolve(void *This, REFIID riid, void **ppv)
{
    struct elem_weakref *impl = impl_from_elem_WeakRef(This);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (!impl->target) return S_OK;
    return elem_QI_internal(impl->target, riid, ppv);
}
static const void *elem_weakref_vtbl[4] = {
    elem_weakref_QI, elem_weakref_AddRef, elem_weakref_Release, elem_weakref_Resolve
};

static HRESULT elem_weakref_create(struct xaml_element *target, void **out)
{
    struct elem_weakref *w = calloc(1, sizeof(*w));
    if (!w) return E_OUTOFMEMORY;
    w->IWeakReference_iface.lpVtbl = elem_weakref_vtbl;
    w->ref = 1;
    w->target = target;
    *out = &w->IWeakReference_iface;
    return S_OK;
}

static HRESULT WINAPI elem_wrs_QI(void *This, REFIID riid, void **ppv) { return elem_QI_internal(impl_from_elem_IWRS(This), riid, ppv); }
static ULONG WINAPI elem_wrs_AddRef(void *This) { return elem_AddRef_internal(impl_from_elem_IWRS(This)); }
static ULONG WINAPI elem_wrs_Release(void *This) { return elem_Release_internal(impl_from_elem_IWRS(This)); }
static HRESULT WINAPI elem_wrs_GetWeakReference(void *This, void **weakReference)
{
    struct xaml_element *impl = impl_from_elem_IWRS(This);
    if (!weakReference) return E_POINTER;
    return elem_weakref_create(impl, weakReference);
}
static const void *elem_wrs_vtbl[4] = {
    elem_wrs_QI, elem_wrs_AddRef, elem_wrs_Release, elem_wrs_GetWeakReference
};

/* -------------------------------------------------------------------------
 * ContentDialog Async Result (IAsyncOperation<ContentDialogResult> & IAsyncInfo)
 * ------------------------------------------------------------------------- */
struct async_content_dialog_result
{
    IInspectable IInspectable_iface;
    struct { const void *lpVtbl; } IAsyncInfo_iface;
    LONG ref;
    AsyncStatus status;
    HRESULT hr;
    INT32 result;
};

#define impl_from_async_cdr(ptr) ((struct async_content_dialog_result *)(ptr))
#define impl_from_async_cdr_info(ptr) ((struct async_content_dialog_result *)((char *)(ptr) - offsetof(struct async_content_dialog_result, IAsyncInfo_iface)))

static HRESULT WINAPI async_cdr_QI(IInspectable *iface, REFIID riid, void **ppv)
{
    struct async_content_dialog_result *impl = impl_from_async_cdr(iface);
    if (!ppv) return E_POINTER;
    *ppv = NULL;

    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject_local))
    {
        *ppv = &impl->IInspectable_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IAsyncInfo_local))
    {
        *ppv = &impl->IAsyncInfo_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    /* Fallback: accept any other interface queried as the parameterized IAsyncOperation<ContentDialogResult> */
    *ppv = &impl->IInspectable_iface;
    InterlockedIncrement(&impl->ref);
    return S_OK;
}

static ULONG WINAPI async_cdr_AddRef(IInspectable *iface)
{
    struct async_content_dialog_result *impl = impl_from_async_cdr(iface);
    return InterlockedIncrement(&impl->ref);
}

static ULONG WINAPI async_cdr_Release(IInspectable *iface)
{
    struct async_content_dialog_result *impl = impl_from_async_cdr(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}

static HRESULT WINAPI async_cdr_GetIids(IInspectable *iface, ULONG *c, IID **i)
{
    if (c) *c = 0;
    return S_OK;
}

static HRESULT WINAPI async_cdr_GetRuntimeClassName(IInspectable *iface, HSTRING *n)
{
    return WindowsCreateString(L"Windows.Foundation.IAsyncOperation`1<Windows.UI.Xaml.Controls.ContentDialogResult>", 78, n);
}

static HRESULT WINAPI async_cdr_GetTrustLevel(IInspectable *iface, TrustLevel *t)
{
    if (t) *t = BaseTrust;
    return S_OK;
}

static HRESULT WINAPI async_cdr_put_Completed(IInspectable *iface, void *handler)
{
    struct async_content_dialog_result *impl = impl_from_async_cdr(iface);
    TRACE("async_cdr %p put_Completed handler %p\n", impl, handler);
    if (handler)
    {
        typedef HRESULT (STDMETHODCALLTYPE *invoke_fn)(void *This, void *asyncInfo, AsyncStatus status);
        void **vtbl = *(void ***)handler;
        invoke_fn invoke = (invoke_fn)vtbl[3];
        invoke(handler, &impl->IInspectable_iface, impl->status);
    }
    return S_OK;
}

static HRESULT WINAPI async_cdr_get_Completed(IInspectable *iface, void **handler)
{
    if (!handler) return E_POINTER;
    *handler = NULL;
    return S_OK;
}

static HRESULT WINAPI async_cdr_GetResults(IInspectable *iface, INT32 *results)
{
    struct async_content_dialog_result *impl = impl_from_async_cdr(iface);
    if (!results) return E_POINTER;
    *results = impl->result;
    return impl->hr;
}

static const void *async_cdr_vtbl[] = {
    (void *)async_cdr_QI,
    (void *)async_cdr_AddRef,
    (void *)async_cdr_Release,
    (void *)async_cdr_GetIids,
    (void *)async_cdr_GetRuntimeClassName,
    (void *)async_cdr_GetTrustLevel,
    (void *)async_cdr_put_Completed,
    (void *)async_cdr_get_Completed,
    (void *)async_cdr_GetResults
};

static HRESULT WINAPI async_cdr_info_QI(void *This, REFIID riid, void **ppv)
{
    return async_cdr_QI(&impl_from_async_cdr_info(This)->IInspectable_iface, riid, ppv);
}
static ULONG WINAPI async_cdr_info_AddRef(void *This)
{
    return async_cdr_AddRef(&impl_from_async_cdr_info(This)->IInspectable_iface);
}
static ULONG WINAPI async_cdr_info_Release(void *This)
{
    return async_cdr_Release(&impl_from_async_cdr_info(This)->IInspectable_iface);
}
static HRESULT WINAPI async_cdr_info_GetIids(void *This, ULONG *c, IID **i)
{
    if (c) *c = 0;
    return S_OK;
}
static HRESULT WINAPI async_cdr_info_GetRuntimeClassName(void *This, HSTRING *n)
{
    return WindowsCreateString(L"Windows.Foundation.IAsyncInfo", 29, n);
}
static HRESULT WINAPI async_cdr_info_GetTrustLevel(void *This, TrustLevel *t)
{
    if (t) *t = BaseTrust;
    return S_OK;
}
static HRESULT WINAPI async_cdr_info_get_Id(void *This, UINT32 *id)
{
    if (!id) return E_POINTER;
    *id = 1;
    return S_OK;
}
static HRESULT WINAPI async_cdr_info_get_Status(void *This, AsyncStatus *status)
{
    struct async_content_dialog_result *impl = impl_from_async_cdr_info(This);
    if (!status) return E_POINTER;
    *status = impl->status;
    return S_OK;
}
static HRESULT WINAPI async_cdr_info_get_ErrorCode(void *This, HRESULT *errorCode)
{
    struct async_content_dialog_result *impl = impl_from_async_cdr_info(This);
    if (!errorCode) return E_POINTER;
    *errorCode = impl->hr;
    return S_OK;
}
static HRESULT WINAPI async_cdr_info_Cancel(void *This) { return S_OK; }
static HRESULT WINAPI async_cdr_info_Close(void *This) { return S_OK; }

static const void *async_cdr_info_vtbl[] = {
    (void *)async_cdr_info_QI,
    (void *)async_cdr_info_AddRef,
    (void *)async_cdr_info_Release,
    (void *)async_cdr_info_GetIids,
    (void *)async_cdr_info_GetRuntimeClassName,
    (void *)async_cdr_info_GetTrustLevel,
    (void *)async_cdr_info_get_Id,
    (void *)async_cdr_info_get_Status,
    (void *)async_cdr_info_get_ErrorCode,
    (void *)async_cdr_info_Cancel,
    (void *)async_cdr_info_Close
};

static HRESULT async_content_dialog_result_create(INT32 result, HRESULT hr, IInspectable **out)
{
    struct async_content_dialog_result *impl = calloc(1, sizeof(*impl));
    if (!impl) return E_OUTOFMEMORY;
    impl->IInspectable_iface.lpVtbl = (const void *)async_cdr_vtbl;
    impl->IAsyncInfo_iface.lpVtbl = (const void *)async_cdr_info_vtbl;
    impl->ref = 1;
    impl->status = 1; /* Completed */
    impl->hr = hr;
    impl->result = result;
    *out = &impl->IInspectable_iface;
    return S_OK;
}

/* -------------------------------------------------------------------------
 * ContentDialog Instance Implementation
 * ------------------------------------------------------------------------- */
static HRESULT WINAPI elem_cd_QI(void *This, REFIID riid, void **ppv) { return elem_QI_internal(impl_from_IContentDialog(This), riid, ppv); }
static ULONG WINAPI elem_cd_AddRef(void *This) { return elem_AddRef_internal(impl_from_IContentDialog(This)); }
static ULONG WINAPI elem_cd_Release(void *This) { return elem_Release_internal(impl_from_IContentDialog(This)); }
static HRESULT WINAPI elem_cd_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.Controls.ContentDialog", 38, n); }

static HRESULT WINAPI elem_cd_ShowAsync(void *This, void **returnValue)
{
    TRACE("ContentDialog %p ShowAsync\n", This);
    if (!returnValue) return E_POINTER;
    return async_content_dialog_result_create(1 /* Primary */, S_OK, (IInspectable **)returnValue);
}

static HRESULT WINAPI elem_cd2_QI(void *This, REFIID riid, void **ppv) { return elem_QI_internal(impl_from_IContentDialog2(This), riid, ppv); }
static ULONG WINAPI elem_cd2_AddRef(void *This) { return elem_AddRef_internal(impl_from_IContentDialog2(This)); }
static ULONG WINAPI elem_cd2_Release(void *This) { return elem_Release_internal(impl_from_IContentDialog2(This)); }
static HRESULT WINAPI elem_cd2_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.Controls.ContentDialog", 38, n); }

/* Initialize VTables */
static void init_elem_vtables(void)
{
    static int initialized = 0;
    int i;
    if (initialized) return;
    initialized = 1;

    for (i = 0; i < 128; i++) elem_ui_vtbl[i] = (void *)elem_stub_ret_ok;
    elem_ui_vtbl[0]  = (void *)elem_ui_QI;
    elem_ui_vtbl[1]  = (void *)elem_ui_AddRef;
    elem_ui_vtbl[2]  = (void *)elem_ui_Release;
    elem_ui_vtbl[3]  = (void *)elem_ui_GetIids;
    elem_ui_vtbl[4]  = (void *)elem_ui_GetRuntimeClassName;
    elem_ui_vtbl[5]  = (void *)elem_ui_GetTrustLevel;
    elem_ui_vtbl[14] = (void *)elem_ui_get_Visibility;
    elem_ui_vtbl[15] = (void *)elem_ui_put_Visibility;
    elem_ui_vtbl[21] = (void *)elem_ui_get_Visibility;
    elem_ui_vtbl[22] = (void *)elem_ui_put_Visibility;

    elem_ui_vtbl[41] = (void *)elem_ui_add_KeyUp;
    elem_ui_vtbl[42] = (void *)elem_ui_remove_KeyUp;
    elem_ui_vtbl[43] = (void *)elem_ui_add_KeyDown;
    elem_ui_vtbl[44] = (void *)elem_ui_remove_KeyDown;
    elem_ui_vtbl[45] = (void *)elem_ui_add_GotFocus;
    elem_ui_vtbl[46] = (void *)elem_ui_remove_GotFocus;
    elem_ui_vtbl[47] = (void *)elem_ui_add_LostFocus;
    elem_ui_vtbl[48] = (void *)elem_ui_remove_LostFocus;

    elem_ui_vtbl[57] = (void *)elem_ui_add_PointerPressed;
    elem_ui_vtbl[58] = (void *)elem_ui_remove_PointerPressed;
    elem_ui_vtbl[59] = (void *)elem_ui_add_PointerMoved;
    elem_ui_vtbl[60] = (void *)elem_ui_remove_PointerMoved;
    elem_ui_vtbl[61] = (void *)elem_ui_add_PointerReleased;
    elem_ui_vtbl[62] = (void *)elem_ui_remove_PointerReleased;
    elem_ui_vtbl[63] = (void *)elem_ui_add_PointerEntered;
    elem_ui_vtbl[64] = (void *)elem_ui_remove_PointerEntered;
    elem_ui_vtbl[65] = (void *)elem_ui_add_PointerExited;
    elem_ui_vtbl[66] = (void *)elem_ui_remove_PointerExited;
    elem_ui_vtbl[67] = (void *)elem_ui_add_PointerCaptureLost;
    elem_ui_vtbl[68] = (void *)elem_ui_remove_PointerCaptureLost;
    elem_ui_vtbl[69] = (void *)elem_ui_add_PointerCanceled;
    elem_ui_vtbl[70] = (void *)elem_ui_remove_PointerCanceled;
    elem_ui_vtbl[71] = (void *)elem_ui_add_PointerWheelChanged;
    elem_ui_vtbl[72] = (void *)elem_ui_remove_PointerWheelChanged;

    elem_ui_vtbl[73] = (void *)elem_ui_add_Tapped;
    elem_ui_vtbl[74] = (void *)elem_ui_remove_Tapped;
    elem_ui_vtbl[75] = (void *)elem_ui_add_DoubleTapped;
    elem_ui_vtbl[76] = (void *)elem_ui_remove_DoubleTapped;
    elem_ui_vtbl[77] = (void *)elem_ui_add_Holding;
    elem_ui_vtbl[78] = (void *)elem_ui_remove_Holding;
    elem_ui_vtbl[79] = (void *)elem_ui_add_RightTapped;
    elem_ui_vtbl[80] = (void *)elem_ui_remove_RightTapped;

    for (i = 0; i < 64; i++) elem_fe_vtbl[i] = (void *)elem_stub_ret_ok;
    elem_fe_vtbl[0]  = (void *)elem_fe_QI;
    elem_fe_vtbl[1]  = (void *)elem_fe_AddRef;
    elem_fe_vtbl[2]  = (void *)elem_fe_Release;
    elem_fe_vtbl[3]  = (void *)elem_ui_GetIids;
    elem_fe_vtbl[4]  = (void *)elem_ui_GetRuntimeClassName;
    elem_fe_vtbl[5]  = (void *)elem_ui_GetTrustLevel;
    elem_fe_vtbl[7]  = (void *)elem_fe_get_Resources;
    elem_fe_vtbl[8]  = (void *)elem_fe_put_Resources;
    elem_fe_vtbl[13] = (void *)elem_fe_get_ActualWidth;
    elem_fe_vtbl[14] = (void *)elem_fe_get_ActualHeight;
    elem_fe_vtbl[15] = (void *)elem_fe_get_Width;
    elem_fe_vtbl[16] = (void *)elem_fe_put_Width;
    elem_fe_vtbl[17] = (void *)elem_fe_get_Height;
    elem_fe_vtbl[18] = (void *)elem_fe_put_Height;
    elem_fe_vtbl[33] = (void *)elem_fe_get_Name;
    elem_fe_vtbl[34] = (void *)elem_fe_put_Name;
    elem_fe_vtbl[36] = (void *)elem_fe_get_DataContext;
    elem_fe_vtbl[37] = (void *)elem_fe_put_DataContext;
    elem_fe_vtbl[40] = (void *)elem_fe_get_Parent;
    elem_fe_vtbl[43] = (void *)elem_fe_add_Loaded;
    elem_fe_vtbl[44] = (void *)elem_fe_remove_Loaded;
    elem_fe_vtbl[47] = (void *)elem_fe_add_SizeChanged;
    elem_fe_vtbl[48] = (void *)elem_fe_remove_SizeChanged;
    elem_fe_vtbl[51] = (void *)elem_fe_FindName;
    elem_fe_vtbl[52] = (void *)elem_fe_SetBinding;

    for (i = 0; i < 64; i++) elem_ctrl_vtbl[i] = (void *)elem_stub_ret_ok;
    elem_ctrl_vtbl[0] = (void *)elem_ctrl_QI;
    elem_ctrl_vtbl[1] = (void *)elem_ctrl_AddRef;
    elem_ctrl_vtbl[2] = (void *)elem_ctrl_Release;
    elem_ctrl_vtbl[3] = (void *)elem_ui_GetIids;
    elem_ctrl_vtbl[4] = (void *)elem_ctrl_GetRuntimeClassName;
    elem_ctrl_vtbl[5] = (void *)elem_ui_GetTrustLevel;
    elem_ctrl_vtbl[46] = (void *)elem_ctrl_Focus;

    for (i = 0; i < 16; i++) elem_ctrl_prot_vtbl[i] = (void *)elem_stub_ret_ok;
    elem_ctrl_prot_vtbl[0] = (void *)elem_ctrl_prot_QI;
    elem_ctrl_prot_vtbl[1] = (void *)elem_ctrl_prot_AddRef;
    elem_ctrl_prot_vtbl[2] = (void *)elem_ctrl_prot_Release;
    elem_ctrl_prot_vtbl[3] = (void *)elem_ui_GetIids;
    elem_ctrl_prot_vtbl[4] = (void *)elem_ctrl_prot_GetRuntimeClassName;
    elem_ctrl_prot_vtbl[5] = (void *)elem_ui_GetTrustLevel;
    elem_ctrl_prot_vtbl[6] = (void *)elem_ctrl_prot_get_DefaultStyleKey;
    elem_ctrl_prot_vtbl[7] = (void *)elem_ctrl_prot_put_DefaultStyleKey;
    elem_ctrl_prot_vtbl[8] = (void *)elem_ctrl_prot_GetTemplateChild;

    for (i = 0; i < 32; i++) elem_cc_vtbl[i] = (void *)elem_stub_ret_ok;
    elem_cc_vtbl[0] = (void *)elem_cc_QI;
    elem_cc_vtbl[1] = (void *)elem_cc_AddRef;
    elem_cc_vtbl[2] = (void *)elem_cc_Release;
    elem_cc_vtbl[3] = (void *)elem_ui_GetIids;
    elem_cc_vtbl[4] = (void *)elem_cc_GetRuntimeClassName;
    elem_cc_vtbl[5] = (void *)elem_ui_GetTrustLevel;
    elem_cc_vtbl[6] = (void *)elem_cc_get_Content;
    elem_cc_vtbl[7] = (void *)elem_cc_put_Content;

    for (i = 0; i < 32; i++) elem_uc_vtbl[i] = (void *)elem_stub_ret_ok;
    elem_uc_vtbl[0] = (void *)elem_uc_QI;
    elem_uc_vtbl[1] = (void *)elem_uc_AddRef;
    elem_uc_vtbl[2] = (void *)elem_uc_Release;
    elem_uc_vtbl[3] = (void *)elem_ui_GetIids;
    elem_uc_vtbl[4] = (void *)elem_uc_GetRuntimeClassName;
    elem_uc_vtbl[5] = (void *)elem_ui_GetTrustLevel;
    elem_uc_vtbl[6] = (void *)elem_cc_get_Content;
    elem_uc_vtbl[7] = (void *)elem_cc_put_Content;

    for (i = 0; i < 16; i++) elem_page_vtbl[i] = (void *)elem_stub_ret_ok;
    elem_page_vtbl[0] = (void *)elem_page_QI;
    elem_page_vtbl[1] = (void *)elem_page_AddRef;
    elem_page_vtbl[2] = (void *)elem_page_Release;
    elem_page_vtbl[3] = (void *)elem_ui_GetIids;
    elem_page_vtbl[4] = (void *)elem_ui_GetRuntimeClassName;
    elem_page_vtbl[5] = (void *)elem_ui_GetTrustLevel;

    for (i = 0; i < 32; i++) elem_panel_vtbl[i] = (void *)elem_stub_ret_ok;
    elem_panel_vtbl[0] = (void *)elem_panel_QI;
    elem_panel_vtbl[1] = (void *)elem_panel_AddRef;
    elem_panel_vtbl[2] = (void *)elem_panel_Release;
    elem_panel_vtbl[3] = (void *)elem_ui_GetIids;
    elem_panel_vtbl[4] = (void *)elem_panel_GetRuntimeClassName;
    elem_panel_vtbl[5] = (void *)elem_ui_GetTrustLevel;
    elem_panel_vtbl[6] = (void *)elem_panel_get_Children;

    for (i = 0; i < 32; i++) elem_grid_vtbl[i] = (void *)elem_stub_ret_ok;
    elem_grid_vtbl[0] = (void *)elem_grid_QI;
    elem_grid_vtbl[1] = (void *)elem_grid_AddRef;
    elem_grid_vtbl[2] = (void *)elem_grid_Release;
    elem_grid_vtbl[3] = (void *)elem_ui_GetIids;
    elem_grid_vtbl[4] = (void *)elem_grid_GetRuntimeClassName;
    elem_grid_vtbl[5] = (void *)elem_ui_GetTrustLevel;
    elem_grid_vtbl[6] = (void *)elem_panel_get_Children;

    for (i = 0; i < 16; i++) elem_canvas_vtbl[i] = (void *)elem_stub_ret_ok;
    elem_canvas_vtbl[0] = (void *)elem_canvas_QI;
    elem_canvas_vtbl[1] = (void *)elem_canvas_AddRef;
    elem_canvas_vtbl[2] = (void *)elem_canvas_Release;
    elem_canvas_vtbl[3] = (void *)elem_ui_GetIids;
    elem_canvas_vtbl[4] = (void *)elem_canvas_GetRuntimeClassName;
    elem_canvas_vtbl[5] = (void *)elem_ui_GetTrustLevel;

    for (i = 0; i < 16; i++) elem_scp_vtbl[i] = (void *)elem_stub_ret_ok;
    elem_scp_vtbl[0]  = (void *)elem_scp_QI;
    elem_scp_vtbl[1]  = (void *)elem_scp_AddRef;
    elem_scp_vtbl[2]  = (void *)elem_scp_Release;
    elem_scp_vtbl[3]  = (void *)elem_ui_GetIids;
    elem_scp_vtbl[4]  = (void *)elem_scp_GetRuntimeClassName;
    elem_scp_vtbl[5]  = (void *)elem_ui_GetTrustLevel;
    elem_scp_vtbl[6]  = (void *)elem_scp_get_CompositionScaleX;
    elem_scp_vtbl[7]  = (void *)elem_scp_get_CompositionScaleY;
    elem_scp_vtbl[8]  = (void *)elem_scp_add_CompositionScaleChanged;
    elem_scp_vtbl[9]  = (void *)elem_scp_remove_CompositionScaleChanged;
    elem_scp_vtbl[10] = (void *)elem_scp_CreateCoreIndependentInputSource;

    for (i = 0; i < 8; i++) elem_scp_native_vtbl[i] = (void *)elem_stub_ret_ok;
    elem_scp_native_vtbl[0] = (void *)elem_scp_native_QI;
    elem_scp_native_vtbl[1] = (void *)elem_scp_native_AddRef;
    elem_scp_native_vtbl[2] = (void *)elem_scp_native_Release;
    elem_scp_native_vtbl[3] = (void *)elem_scp_native_SetSwapChain;
    elem_scp_native_vtbl[4] = (void *)elem_scp_native_SetSwapChainHandle;

    for (i = 0; i < 32; i++) elem_border_vtbl[i] = (void *)elem_stub_ret_ok;
    elem_border_vtbl[0]  = (void *)elem_border_QI;
    elem_border_vtbl[1]  = (void *)elem_border_AddRef;
    elem_border_vtbl[2]  = (void *)elem_border_Release;
    elem_border_vtbl[3]  = (void *)elem_ui_GetIids;
    elem_border_vtbl[4]  = (void *)elem_border_GetRuntimeClassName;
    elem_border_vtbl[5]  = (void *)elem_ui_GetTrustLevel;
    elem_border_vtbl[16] = (void *)elem_border_get_Child;
    elem_border_vtbl[17] = (void *)elem_border_put_Child;

    for (i = 0; i < 32; i++) elem_btn_vtbl[i] = (void *)elem_stub_ret_ok;
    elem_btn_vtbl[0] = (void *)elem_btn_QI;
    elem_btn_vtbl[1] = (void *)elem_btn_AddRef;
    elem_btn_vtbl[2] = (void *)elem_btn_Release;
    elem_btn_vtbl[3] = (void *)elem_ui_GetIids;
    elem_btn_vtbl[4] = (void *)elem_btn_GetRuntimeClassName;
    elem_btn_vtbl[5] = (void *)elem_ui_GetTrustLevel;

    for (i = 0; i < 32; i++) elem_btnbase_vtbl[i] = (void *)elem_stub_ret_ok;
    elem_btnbase_vtbl[0]  = (void *)elem_btnbase_QI;
    elem_btnbase_vtbl[1]  = (void *)elem_btnbase_AddRef;
    elem_btnbase_vtbl[2]  = (void *)elem_btnbase_Release;
    elem_btnbase_vtbl[3]  = (void *)elem_ui_GetIids;
    elem_btnbase_vtbl[4]  = (void *)elem_btnbase_GetRuntimeClassName;
    elem_btnbase_vtbl[5]  = (void *)elem_ui_GetTrustLevel;
    elem_btnbase_vtbl[10] = (void *)elem_btn_get_Command;
    elem_btnbase_vtbl[11] = (void *)elem_btn_put_Command;
    elem_btnbase_vtbl[12] = (void *)elem_btn_get_CommandParam;
    elem_btnbase_vtbl[13] = (void *)elem_btn_put_CommandParam;
    elem_btnbase_vtbl[14] = (void *)elem_btn_add_Click;
    elem_btnbase_vtbl[15] = (void *)elem_btn_remove_Click;

    for (i = 0; i < 32; i++) elem_tb_vtbl[i] = (void *)elem_stub_ret_ok;
    elem_tb_vtbl[0]  = (void *)elem_tb_QI;
    elem_tb_vtbl[1]  = (void *)elem_tb_AddRef;
    elem_tb_vtbl[2]  = (void *)elem_tb_Release;
    elem_tb_vtbl[3]  = (void *)elem_ui_GetIids;
    elem_tb_vtbl[4]  = (void *)elem_tb_GetRuntimeClassName;
    elem_tb_vtbl[5]  = (void *)elem_ui_GetTrustLevel;
    elem_tb_vtbl[6]  = (void *)elem_tb_get_FontSize;
    elem_tb_vtbl[7]  = (void *)elem_tb_put_FontSize;
    elem_tb_vtbl[26] = (void *)elem_tb_get_Text;
    elem_tb_vtbl[27] = (void *)elem_tb_put_Text;

    for (i = 0; i < 32; i++) elem_do_vtbl[i] = (void *)elem_stub_ret_ok;
    elem_do_vtbl[0] = (void *)elem_do_QI;
    elem_do_vtbl[1] = (void *)elem_do_AddRef;
    elem_do_vtbl[2] = (void *)elem_do_Release;
    elem_do_vtbl[3] = (void *)elem_ui_GetIids;
    elem_do_vtbl[4] = (void *)elem_do_GetRuntimeClassName;
    elem_do_vtbl[5] = (void *)elem_ui_GetTrustLevel;
    elem_do_vtbl[6] = (void *)elem_do_GetValue;
    elem_do_vtbl[7] = (void *)elem_do_SetValue;
    elem_do_vtbl[8] = (void *)elem_do_ClearValue;
    elem_do_vtbl[9] = (void *)elem_do_GetValue; /* ReadLocalValue */
    elem_do_vtbl[10] = (void *)elem_do_GetValue; /* GetAnimationBaseValue */
    elem_do_vtbl[11] = (void *)elem_do_get_Dispatcher;

    for (i = 0; i < 16; i++) elem_do2_vtbl[i] = (void *)elem_stub_ret_ok;
    elem_do2_vtbl[0] = (void *)elem_do2_QI;
    elem_do2_vtbl[1] = (void *)elem_do2_AddRef;
    elem_do2_vtbl[2] = (void *)elem_do2_Release;
    elem_do2_vtbl[3] = (void *)elem_ui_GetIids;
    elem_do2_vtbl[4] = (void *)elem_do_GetRuntimeClassName;
    elem_do2_vtbl[5] = (void *)elem_ui_GetTrustLevel;
    elem_do2_vtbl[6] = (void *)elem_do2_RegisterPropertyChangedCallback;
    elem_do2_vtbl[7] = (void *)elem_do2_UnregisterPropertyChangedCallback;

    for (i = 0; i < 32; i++) elem_redhawk_vtbl[i] = (void *)elem_stub_ret_ok;
    elem_redhawk_vtbl[0] = (void *)elem_redhawk_QI;
    elem_redhawk_vtbl[1] = (void *)elem_redhawk_AddRef;
    elem_redhawk_vtbl[2] = (void *)elem_redhawk_Release;
    elem_redhawk_vtbl[3] = (void *)elem_redhawk_slot_3;
    elem_redhawk_vtbl[4] = (void *)elem_redhawk_GetRuntimeClassName;
    elem_redhawk_vtbl[5] = (void *)elem_redhawk_GetTrustLevel;
    elem_redhawk_vtbl[6] = (void *)elem_redhawk_slot_6;
    elem_redhawk_vtbl[7] = (void *)elem_redhawk_slot_7;
    elem_redhawk_vtbl[8] = (void *)elem_redhawk_slot_8;
    elem_redhawk_vtbl[9] = (void *)elem_redhawk_slot_9;
    elem_redhawk_vtbl[10] = (void *)elem_redhawk_slot_10;
    elem_redhawk_vtbl[11] = (void *)elem_redhawk_slot_11;
    elem_redhawk_vtbl[12] = (void *)elem_redhawk_slot_12;
    elem_redhawk_vtbl[13] = (void *)elem_redhawk_slot_13;
    elem_redhawk_vtbl[14] = (void *)elem_redhawk_slot_14;
    elem_redhawk_vtbl[15] = (void *)elem_redhawk_slot_15;

    for (i = 0; i < 32; i++) elem_flyout_vtbl[i] = (void *)elem_stub_ret_ok;
    elem_flyout_vtbl[0] = (void *)elem_flyout_QI;
    elem_flyout_vtbl[1] = (void *)elem_flyout_AddRef;
    elem_flyout_vtbl[2] = (void *)elem_flyout_Release;
    elem_flyout_vtbl[3] = (void *)elem_ui_GetIids;
    elem_flyout_vtbl[4] = (void *)elem_flyout_GetRuntimeClassName;
    elem_flyout_vtbl[5] = (void *)elem_ui_GetTrustLevel;
    elem_flyout_vtbl[6] = (void *)elem_flyout_get_Content;
    elem_flyout_vtbl[7] = (void *)elem_flyout_put_Content;
    elem_flyout_vtbl[8] = (void *)elem_flyout_get_FlyoutPresenterStyle;
    elem_flyout_vtbl[9] = (void *)elem_flyout_put_FlyoutPresenterStyle;

    for (i = 0; i < 32; i++) elem_flyout_base_vtbl[i] = (void *)elem_stub_ret_ok;
    elem_flyout_base_vtbl[0]  = (void *)elem_flyout_base_QI;
    elem_flyout_base_vtbl[1]  = (void *)elem_flyout_base_AddRef;
    elem_flyout_base_vtbl[2]  = (void *)elem_flyout_base_Release;
    elem_flyout_base_vtbl[3]  = (void *)elem_ui_GetIids;
    elem_flyout_base_vtbl[4]  = (void *)elem_flyout_base_GetRuntimeClassName;
    elem_flyout_base_vtbl[5]  = (void *)elem_ui_GetTrustLevel;
    elem_flyout_base_vtbl[6]  = (void *)elem_flyout_base_get_Placement;
    elem_flyout_base_vtbl[7]  = (void *)elem_flyout_base_put_Placement;
    elem_flyout_base_vtbl[8]  = (void *)elem_flyout_base_add_evt; /* add_Opened */
    elem_flyout_base_vtbl[9]  = (void *)elem_flyout_base_rem_evt; /* remove_Opened */
    elem_flyout_base_vtbl[10] = (void *)elem_flyout_base_add_evt; /* add_Closed */
    elem_flyout_base_vtbl[11] = (void *)elem_flyout_base_rem_evt; /* remove_Closed */
    elem_flyout_base_vtbl[12] = (void *)elem_flyout_base_add_evt; /* add_Opening */
    elem_flyout_base_vtbl[13] = (void *)elem_flyout_base_rem_evt; /* remove_Opening */
    elem_flyout_base_vtbl[14] = (void *)elem_flyout_base_ShowAt;
    elem_flyout_base_vtbl[15] = (void *)elem_flyout_base_Hide;

    for (i = 0; i < 32; i++) elem_menuflyout_vtbl[i] = (void *)elem_stub_ret_ok;
    elem_menuflyout_vtbl[0] = (void *)elem_menuflyout_QI;
    elem_menuflyout_vtbl[1] = (void *)elem_menuflyout_AddRef;
    elem_menuflyout_vtbl[2] = (void *)elem_menuflyout_Release;
    elem_menuflyout_vtbl[3] = (void *)elem_ui_GetIids;
    elem_menuflyout_vtbl[4] = (void *)elem_menuflyout_GetRuntimeClassName;
    elem_menuflyout_vtbl[5] = (void *)elem_ui_GetTrustLevel;
    elem_menuflyout_vtbl[6] = (void *)elem_menuflyout_get_Items;
    elem_menuflyout_vtbl[7] = (void *)elem_menuflyout_get_MenuFlyoutPresenterStyle;
    elem_menuflyout_vtbl[8] = (void *)elem_menuflyout_put_MenuFlyoutPresenterStyle;

    for (i = 0; i < 32; i++) elem_menuflyout2_vtbl[i] = (void *)elem_stub_ret_ok;
    elem_menuflyout2_vtbl[0] = (void *)elem_menuflyout2_QI;
    elem_menuflyout2_vtbl[1] = (void *)elem_menuflyout2_AddRef;
    elem_menuflyout2_vtbl[2] = (void *)elem_menuflyout2_Release;
    elem_menuflyout2_vtbl[3] = (void *)elem_ui_GetIids;
    elem_menuflyout2_vtbl[4] = (void *)elem_menuflyout_GetRuntimeClassName;
    elem_menuflyout2_vtbl[5] = (void *)elem_ui_GetTrustLevel;
    elem_menuflyout2_vtbl[6] = (void *)elem_menuflyout2_ShowAt;

    for (i = 0; i < 32; i++) elem_menuflyoutitem_vtbl[i] = (void *)elem_stub_ret_ok;
    elem_menuflyoutitem_vtbl[0]  = (void *)elem_menuflyoutitem_QI;
    elem_menuflyoutitem_vtbl[1]  = (void *)elem_menuflyoutitem_AddRef;
    elem_menuflyoutitem_vtbl[2]  = (void *)elem_menuflyoutitem_Release;
    elem_menuflyoutitem_vtbl[3]  = (void *)elem_ui_GetIids;
    elem_menuflyoutitem_vtbl[4]  = (void *)elem_menuflyoutitem_GetRuntimeClassName;
    elem_menuflyoutitem_vtbl[5]  = (void *)elem_ui_GetTrustLevel;
    elem_menuflyoutitem_vtbl[6]  = (void *)elem_menuflyoutitem_get_Text;
    elem_menuflyoutitem_vtbl[7]  = (void *)elem_menuflyoutitem_put_Text;
    elem_menuflyoutitem_vtbl[8]  = (void *)elem_menuflyoutitem_get_Command;
    elem_menuflyoutitem_vtbl[9]  = (void *)elem_menuflyoutitem_put_Command;
    elem_menuflyoutitem_vtbl[10] = (void *)elem_menuflyoutitem_get_CommandParameter;
    elem_menuflyoutitem_vtbl[11] = (void *)elem_menuflyoutitem_put_CommandParameter;
    elem_menuflyoutitem_vtbl[12] = (void *)elem_menuflyoutitem_add_Click;
    elem_menuflyoutitem_vtbl[13] = (void *)elem_menuflyoutitem_remove_Click;

    for (i = 0; i < 16; i++) elem_menuflyoutitem2_vtbl[i] = (void *)elem_stub_ret_ok;
    elem_menuflyoutitem2_vtbl[0] = (void *)elem_menuflyoutitem2_QI;
    elem_menuflyoutitem2_vtbl[1] = (void *)elem_menuflyoutitem2_AddRef;
    elem_menuflyoutitem2_vtbl[2] = (void *)elem_menuflyoutitem2_Release;
    elem_menuflyoutitem2_vtbl[3] = (void *)elem_ui_GetIids;
    elem_menuflyoutitem2_vtbl[4] = (void *)elem_menuflyoutitem_GetRuntimeClassName;
    elem_menuflyoutitem2_vtbl[5] = (void *)elem_ui_GetTrustLevel;
    elem_menuflyoutitem2_vtbl[6] = (void *)elem_stub_ret_zero; /* get_Icon */
    elem_menuflyoutitem2_vtbl[7] = (void *)elem_stub_ret_ok;   /* put_Icon */

    for (i = 0; i < 16; i++) elem_menuflyoutitembase_vtbl[i] = (void *)elem_stub_ret_ok;
    elem_menuflyoutitembase_vtbl[0] = (void *)elem_menuflyoutitembase_QI;
    elem_menuflyoutitembase_vtbl[1] = (void *)elem_menuflyoutitembase_AddRef;
    elem_menuflyoutitembase_vtbl[2] = (void *)elem_menuflyoutitembase_Release;
    elem_menuflyoutitembase_vtbl[3] = (void *)elem_ui_GetIids;
    elem_menuflyoutitembase_vtbl[4] = (void *)elem_menuflyoutitem_GetRuntimeClassName;
    elem_menuflyoutitembase_vtbl[5] = (void *)elem_ui_GetTrustLevel;

    for (i = 0; i < 16; i++) elem_rowdef_vtbl[i] = (void *)elem_stub_ret_ok;
    elem_rowdef_vtbl[0] = (void *)elem_rowdef_QI;
    elem_rowdef_vtbl[1] = (void *)elem_rowdef_AddRef;
    elem_rowdef_vtbl[2] = (void *)elem_rowdef_Release;
    elem_rowdef_vtbl[3] = (void *)elem_ui_GetIids;
    elem_rowdef_vtbl[4] = (void *)elem_rowdef_GetRuntimeClassName;
    elem_rowdef_vtbl[5] = (void *)elem_ui_GetTrustLevel;
    elem_rowdef_vtbl[6] = (void *)elem_rowdef_get_Height;
    elem_rowdef_vtbl[7] = (void *)elem_rowdef_put_Height;
    elem_rowdef_vtbl[8] = (void *)elem_rowdef_get_MaxHeight;
    elem_rowdef_vtbl[9] = (void *)elem_rowdef_put_MaxHeight;
    elem_rowdef_vtbl[10] = (void *)elem_rowdef_get_MinHeight;
    elem_rowdef_vtbl[11] = (void *)elem_rowdef_put_MinHeight;
    elem_rowdef_vtbl[12] = (void *)elem_rowdef_get_ActualHeight;

    for (i = 0; i < 16; i++) elem_coldef_vtbl[i] = (void *)elem_stub_ret_ok;
    elem_coldef_vtbl[0] = (void *)elem_coldef_QI;
    elem_coldef_vtbl[1] = (void *)elem_coldef_AddRef;
    elem_coldef_vtbl[2] = (void *)elem_coldef_Release;
    elem_coldef_vtbl[3] = (void *)elem_ui_GetIids;
    elem_coldef_vtbl[4] = (void *)elem_coldef_GetRuntimeClassName;
    elem_coldef_vtbl[5] = (void *)elem_ui_GetTrustLevel;
    elem_coldef_vtbl[6] = (void *)elem_coldef_get_Width;
    elem_coldef_vtbl[7] = (void *)elem_coldef_put_Width;
    elem_coldef_vtbl[8] = (void *)elem_coldef_get_MaxWidth;
    elem_coldef_vtbl[9] = (void *)elem_coldef_put_MaxWidth;
    elem_coldef_vtbl[10] = (void *)elem_coldef_get_MinWidth;
    elem_coldef_vtbl[11] = (void *)elem_coldef_put_MinWidth;
    elem_coldef_vtbl[12] = (void *)elem_coldef_get_ActualWidth;

    for (i = 0; i < 16; i++) elem_adapt_vtbl[i] = (void *)elem_stub_ret_ok;
    elem_adapt_vtbl[0] = (void *)elem_adapt_QI;
    elem_adapt_vtbl[1] = (void *)elem_adapt_AddRef;
    elem_adapt_vtbl[2] = (void *)elem_adapt_Release;
    elem_adapt_vtbl[3] = (void *)elem_ui_GetIids;
    elem_adapt_vtbl[4] = (void *)elem_adapt_GetRuntimeClassName;
    elem_adapt_vtbl[5] = (void *)elem_ui_GetTrustLevel;
    elem_adapt_vtbl[6] = (void *)elem_adapt_get_MinWindowWidth;
    elem_adapt_vtbl[7] = (void *)elem_adapt_put_MinWindowWidth;
    elem_adapt_vtbl[8] = (void *)elem_adapt_get_MinWindowHeight;
    elem_adapt_vtbl[9] = (void *)elem_adapt_put_MinWindowHeight;

    for (i = 0; i < 64; i++) elem_cd_vtbl[i] = (void *)elem_stub_ret_ok;
    elem_cd_vtbl[0]  = (void *)elem_cd_QI;
    elem_cd_vtbl[1]  = (void *)elem_cd_AddRef;
    elem_cd_vtbl[2]  = (void *)elem_cd_Release;
    elem_cd_vtbl[3]  = (void *)elem_ui_GetIids;
    elem_cd_vtbl[4]  = (void *)elem_cd_GetRuntimeClassName;
    elem_cd_vtbl[5]  = (void *)elem_ui_GetTrustLevel;
    elem_cd_vtbl[39] = (void *)elem_cd_ShowAsync;

    for (i = 0; i < 32; i++) elem_cd2_vtbl[i] = (void *)elem_stub_ret_ok;
    elem_cd2_vtbl[0] = (void *)elem_cd2_QI;
    elem_cd2_vtbl[1] = (void *)elem_cd2_AddRef;
    elem_cd2_vtbl[2] = (void *)elem_cd2_Release;
    elem_cd2_vtbl[3] = (void *)elem_ui_GetIids;
    elem_cd2_vtbl[4] = (void *)elem_cd2_GetRuntimeClassName;
    elem_cd2_vtbl[5] = (void *)elem_ui_GetTrustLevel;
}

/* -------------------------------------------------------------------------
 * IGridStatics Implementation
 * ------------------------------------------------------------------------- */
static HRESULT WINAPI grid_s_QI(void *This, REFIID riid, void **ppv)
{
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IGridStatics))
    {
        *ppv = This;
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI grid_s_AddRef(void *This) { return 2; }
static ULONG WINAPI grid_s_Release(void *This) { return 1; }
static HRESULT WINAPI grid_s_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI grid_s_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.Controls.Grid", 28, n); }
static HRESULT WINAPI grid_s_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI grid_s_GetRow(void *This, void *element, INT32 *value)
{
    struct xaml_element *elem = impl_from_IFrameworkElement(element);
    if (value) *value = elem ? elem->grid_row : 0;
    return S_OK;
}
static HRESULT WINAPI grid_s_SetRow(void *This, void *element, INT32 value)
{
    struct xaml_element *elem = impl_from_IFrameworkElement(element);
    if (elem) elem->grid_row = value;
    return S_OK;
}
static HRESULT WINAPI grid_s_GetColumn(void *This, void *element, INT32 *value)
{
    struct xaml_element *elem = impl_from_IFrameworkElement(element);
    if (value) *value = elem ? elem->grid_column : 0;
    return S_OK;
}
static HRESULT WINAPI grid_s_SetColumn(void *This, void *element, INT32 value)
{
    struct xaml_element *elem = impl_from_IFrameworkElement(element);
    if (elem) elem->grid_column = value;
    return S_OK;
}
static HRESULT WINAPI grid_s_GetRowSpan(void *This, void *element, INT32 *value)
{
    struct xaml_element *elem = impl_from_IFrameworkElement(element);
    if (value) *value = elem ? elem->grid_rowspan : 1;
    return S_OK;
}
static HRESULT WINAPI grid_s_SetRowSpan(void *This, void *element, INT32 value)
{
    struct xaml_element *elem = impl_from_IFrameworkElement(element);
    if (elem) elem->grid_rowspan = value;
    return S_OK;
}
static HRESULT WINAPI grid_s_GetColumnSpan(void *This, void *element, INT32 *value)
{
    struct xaml_element *elem = impl_from_IFrameworkElement(element);
    if (value) *value = elem ? elem->grid_colspan : 1;
    return S_OK;
}
static HRESULT WINAPI grid_s_SetColumnSpan(void *This, void *element, INT32 value)
{
    struct xaml_element *elem = impl_from_IFrameworkElement(element);
    if (elem) elem->grid_colspan = value;
    return S_OK;
}

static const void *grid_statics_vtbl[32] = {
    (void *)grid_s_QI,
    (void *)grid_s_AddRef,
    (void *)grid_s_Release,
    (void *)grid_s_GetIids,
    (void *)grid_s_GetRuntimeClassName,
    (void *)grid_s_GetTrustLevel,
    (void *)elem_stub_ret_zero, /* RowProperty */
    (void *)grid_s_GetRow,      /* 7 */
    (void *)grid_s_SetRow,      /* 8 */
    (void *)elem_stub_ret_zero, /* ColumnProperty */
    (void *)grid_s_GetColumn,   /* 10 */
    (void *)grid_s_SetColumn,   /* 11 */
    (void *)elem_stub_ret_zero, /* RowSpanProperty */
    (void *)grid_s_GetRowSpan,  /* 13 */
    (void *)grid_s_SetRowSpan,  /* 14 */
    (void *)elem_stub_ret_zero, /* ColumnSpanProperty */
    (void *)grid_s_GetColumnSpan, /* 16 */
    (void *)grid_s_SetColumnSpan  /* 17 */
};

/* -------------------------------------------------------------------------
 * Control Activation Factories
 * ------------------------------------------------------------------------- */
static HRESULT WINAPI grid_fac_QI(void *This, REFIID riid, void **ppv)
{
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppv = This;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IGridStatics))
    {
        static const struct { const void *lpVtbl; } statics = { grid_statics_vtbl };
        *ppv = (void *)&statics;
        return S_OK;
    }
    return E_NOINTERFACE;
}
static HRESULT WINAPI grid_fac_Activate(void *This, IInspectable **instance)
{
    struct xaml_element *elem;
    init_elem_vtables();
    elem = xaml_element_create(CONTROL_TYPE_GRID, L"Grid");
    if (!elem) return E_OUTOFMEMORY;
    *instance = (IInspectable *)&elem->IUIElement_iface;
    return S_OK;
}

static const struct { const void *lpVtbl; } grid_factory_obj = {
    (const void *[]){ (void *)grid_fac_QI, (void *)grid_s_AddRef, (void *)grid_s_Release,
                      (void *)grid_s_GetIids, (void *)grid_s_GetRuntimeClassName, (void *)grid_s_GetTrustLevel,
                      (void *)grid_fac_Activate }
};
IActivationFactory *grid_factory = (IActivationFactory *)&grid_factory_obj;

/* -------------------------------------------------------------------------
 * Canvas Implementation & Factory (with ICanvasFactory and ICanvasStatics)
 * ------------------------------------------------------------------------- */
struct canvas_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } ICanvasFactory_iface;
    struct { const void *lpVtbl; } ICanvasStatics_iface;
};

#define impl_from_canvas_factory(ptr) ((struct canvas_factory_impl *)((char *)(ptr) - offsetof(struct canvas_factory_impl, IActivationFactory_iface)))
#define impl_from_canvas_fac_iface(ptr) ((struct canvas_factory_impl *)((char *)(ptr) - offsetof(struct canvas_factory_impl, ICanvasFactory_iface)))
#define impl_from_canvas_stat_iface(ptr) ((struct canvas_factory_impl *)((char *)(ptr) - offsetof(struct canvas_factory_impl, ICanvasStatics_iface)))

static HRESULT WINAPI canvas_fac_QI(IActivationFactory *iface, REFIID riid, void **ppv)
{
    struct canvas_factory_impl *impl = impl_from_canvas_factory(iface);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject_local) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppv = &impl->IActivationFactory_iface;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_ICanvasFactory))
    {
        *ppv = &impl->ICanvasFactory_iface;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_ICanvasStatics))
    {
        *ppv = &impl->ICanvasStatics_iface;
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI canvas_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI canvas_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI canvas_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI canvas_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.Controls.Canvas", 31, n); }
static HRESULT WINAPI canvas_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI canvas_fac_CreateInstance(void *This, void *outer, void **inner, void **instance)
{
    struct xaml_element *elem;
    init_elem_vtables();
    elem = xaml_element_create(CONTROL_TYPE_CANVAS, L"Canvas");
    if (!elem) return E_OUTOFMEMORY;
    elem->outer = outer;
    if (inner)
    {
        *inner = &elem->IUIElement_iface;
        InterlockedIncrement(&elem->ref);
    }
    if (instance) *instance = (void *)&elem->ICanvas_iface;
    TRACE("canvas_fac_CreateInstance outer %p -> inner %p, instance %p\n", outer, inner ? *inner : NULL, instance ? *instance : NULL);
    return S_OK;
}

static HRESULT WINAPI canvas_fac_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    return canvas_fac_CreateInstance(iface, NULL, NULL, (void **)instance);
}

static HRESULT WINAPI canvas_fac_iface_QI(void *This, REFIID riid, void **ppv)
{
    return canvas_fac_QI((IActivationFactory *)impl_from_canvas_fac_iface(This), riid, ppv);
}
static ULONG WINAPI canvas_fac_iface_AddRef(void *This) { return 2; }
static ULONG WINAPI canvas_fac_iface_Release(void *This) { return 1; }

static const void *canvas_factory_vtbl[] = {
    (void *)canvas_fac_iface_QI,
    (void *)canvas_fac_iface_AddRef,
    (void *)canvas_fac_iface_Release,
    (void *)canvas_fac_GetIids,
    (void *)canvas_fac_GetRuntimeClassName,
    (void *)canvas_fac_GetTrustLevel,
    (void *)canvas_fac_CreateInstance
};

static HRESULT WINAPI canvas_stat_iface_QI(void *This, REFIID riid, void **ppv)
{
    return canvas_fac_QI((IActivationFactory *)impl_from_canvas_stat_iface(This), riid, ppv);
}
static ULONG WINAPI canvas_stat_iface_AddRef(void *This) { return 2; }
static ULONG WINAPI canvas_stat_iface_Release(void *This) { return 1; }

static HRESULT WINAPI canvas_dp_Left(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"Left"); return S_OK; }
static HRESULT WINAPI canvas_GetLeft(void *This, void *e, double *v) { if (v) *v = 0.0; return S_OK; }
static HRESULT WINAPI canvas_SetLeft(void *This, void *e, double v) { return S_OK; }
static HRESULT WINAPI canvas_dp_Top(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"Top"); return S_OK; }
static HRESULT WINAPI canvas_GetTop(void *This, void *e, double *v) { if (v) *v = 0.0; return S_OK; }
static HRESULT WINAPI canvas_SetTop(void *This, void *e, double v) { return S_OK; }
static HRESULT WINAPI canvas_dp_ZIndex(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"ZIndex"); return S_OK; }
static HRESULT WINAPI canvas_GetZIndex(void *This, void *e, int *v) { if (v) *v = 0; return S_OK; }
static HRESULT WINAPI canvas_SetZIndex(void *This, void *e, int v) { return S_OK; }

static const void *canvas_statics_vtbl[] = {
    (void *)canvas_stat_iface_QI,
    (void *)canvas_stat_iface_AddRef,
    (void *)canvas_stat_iface_Release,
    (void *)canvas_fac_GetIids,
    (void *)canvas_fac_GetRuntimeClassName,
    (void *)canvas_fac_GetTrustLevel,
    (void *)canvas_dp_Left,   /* 6 */
    (void *)canvas_GetLeft,   /* 7 */
    (void *)canvas_SetLeft,   /* 8 */
    (void *)canvas_dp_Top,    /* 9 */
    (void *)canvas_GetTop,    /* 10 */
    (void *)canvas_SetTop,    /* 11 */
    (void *)canvas_dp_ZIndex, /* 12 */
    (void *)canvas_GetZIndex, /* 13 */
    (void *)canvas_SetZIndex  /* 14 */
};

static const IActivationFactoryVtbl canvas_activation_factory_vtbl = {
    canvas_fac_QI,
    canvas_fac_AddRef,
    canvas_fac_Release,
    canvas_fac_GetIids,
    canvas_fac_GetRuntimeClassName,
    canvas_fac_GetTrustLevel,
    canvas_fac_ActivateInstance
};

static struct canvas_factory_impl canvas_factory_obj = {
    { &canvas_activation_factory_vtbl },
    { canvas_factory_vtbl },
    { canvas_statics_vtbl }
};
IActivationFactory *canvas_factory = &canvas_factory_obj.IActivationFactory_iface;

/* -------------------------------------------------------------------------
 * SwapChainPanel Implementation & Factory (with ISwapChainPanelFactory and ISwapChainPanelStatics)
 * ------------------------------------------------------------------------- */
struct swapchainpanel_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } ISwapChainPanelFactory_iface;
    struct { const void *lpVtbl; } ISwapChainPanelStatics_iface;
};

#define impl_from_scp_factory(ptr) ((struct swapchainpanel_factory_impl *)((char *)(ptr) - offsetof(struct swapchainpanel_factory_impl, IActivationFactory_iface)))
#define impl_from_scp_fac_iface(ptr) ((struct swapchainpanel_factory_impl *)((char *)(ptr) - offsetof(struct swapchainpanel_factory_impl, ISwapChainPanelFactory_iface)))
#define impl_from_scp_stat_iface(ptr) ((struct swapchainpanel_factory_impl *)((char *)(ptr) - offsetof(struct swapchainpanel_factory_impl, ISwapChainPanelStatics_iface)))

static HRESULT WINAPI scp_fac_QI(IActivationFactory *iface, REFIID riid, void **ppv)
{
    struct swapchainpanel_factory_impl *impl = impl_from_scp_factory(iface);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject_local) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppv = &impl->IActivationFactory_iface;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_ISwapChainPanelFactory))
    {
        *ppv = &impl->ISwapChainPanelFactory_iface;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_ISwapChainPanelStatics))
    {
        *ppv = &impl->ISwapChainPanelStatics_iface;
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI scp_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI scp_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI scp_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI scp_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.Controls.SwapChainPanel", 39, n); }
static HRESULT WINAPI scp_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI scp_fac_CreateInstance(void *This, void *outer, void **inner, void **instance)
{
    struct xaml_element *elem;
    init_elem_vtables();
    elem = xaml_element_create(CONTROL_TYPE_SWAPCHAINPANEL, L"SwapChainPanel");
    if (!elem) return E_OUTOFMEMORY;
    elem->outer = outer;
    if (inner)
    {
        *inner = &elem->IUIElement_iface;
        InterlockedIncrement(&elem->ref);
    }
    if (instance) *instance = (void *)&elem->ISwapChainPanel_iface;
    global_swapchain_panel = elem;
    if (!global_root_element) global_root_element = elem;
    TRACE("scp_fac_CreateInstance outer %p -> inner %p, instance %p\n", outer, inner ? *inner : NULL, instance ? *instance : NULL);
    return S_OK;
}

static HRESULT WINAPI scp_fac_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    return scp_fac_CreateInstance(iface, NULL, NULL, (void **)instance);
}

static HRESULT WINAPI scp_fac_iface_QI(void *This, REFIID riid, void **ppv)
{
    return scp_fac_QI((IActivationFactory *)impl_from_scp_fac_iface(This), riid, ppv);
}
static ULONG WINAPI scp_fac_iface_AddRef(void *This) { return 2; }
static ULONG WINAPI scp_fac_iface_Release(void *This) { return 1; }

static const void *swapchainpanel_factory_vtbl[] = {
    (void *)scp_fac_iface_QI,
    (void *)scp_fac_iface_AddRef,
    (void *)scp_fac_iface_Release,
    (void *)scp_fac_GetIids,
    (void *)scp_fac_GetRuntimeClassName,
    (void *)scp_fac_GetTrustLevel,
    (void *)scp_fac_CreateInstance
};

static HRESULT WINAPI scp_stat_iface_QI(void *This, REFIID riid, void **ppv)
{
    return scp_fac_QI((IActivationFactory *)impl_from_scp_stat_iface(This), riid, ppv);
}
static ULONG WINAPI scp_stat_iface_AddRef(void *This) { return 2; }
static ULONG WINAPI scp_stat_iface_Release(void *This) { return 1; }

static HRESULT WINAPI scp_dp_CompositionScaleX(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"CompositionScaleX"); return S_OK; }
static HRESULT WINAPI scp_dp_CompositionScaleY(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"CompositionScaleY"); return S_OK; }

static const void *swapchainpanel_statics_vtbl[] = {
    (void *)scp_stat_iface_QI,
    (void *)scp_stat_iface_AddRef,
    (void *)scp_stat_iface_Release,
    (void *)scp_fac_GetIids,
    (void *)scp_fac_GetRuntimeClassName,
    (void *)scp_fac_GetTrustLevel,
    (void *)scp_dp_CompositionScaleX, /* 6 */
    (void *)scp_dp_CompositionScaleY  /* 7 */
};

static const IActivationFactoryVtbl swapchainpanel_activation_factory_vtbl = {
    scp_fac_QI,
    scp_fac_AddRef,
    scp_fac_Release,
    scp_fac_GetIids,
    scp_fac_GetRuntimeClassName,
    scp_fac_GetTrustLevel,
    scp_fac_ActivateInstance
};

static struct swapchainpanel_factory_impl swapchainpanel_factory_obj = {
    { &swapchainpanel_activation_factory_vtbl },
    { swapchainpanel_factory_vtbl },
    { swapchainpanel_statics_vtbl }
};
IActivationFactory *swapchainpanel_factory = &swapchainpanel_factory_obj.IActivationFactory_iface;

/* -------------------------------------------------------------------------
 * Control Factory (with IControlFactory WinRT aggregation)
 * ------------------------------------------------------------------------- */
static const GUID IID_IControlFactory = {0xb12b1d5a, 0x6419, 0x4e16, {0xb2, 0x12, 0xa4, 0x53, 0x36, 0xb7, 0x57, 0x78}};

struct control_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } IControlFactory_iface;
};

#define impl_from_ctrl_factory(ptr) ((struct control_factory_impl *)((char *)(ptr) - offsetof(struct control_factory_impl, IActivationFactory_iface)))
#define impl_from_ctrl_fac_iface(ptr) ((struct control_factory_impl *)((char *)(ptr) - offsetof(struct control_factory_impl, IControlFactory_iface)))

static HRESULT WINAPI control_fac_QI(IActivationFactory *iface, REFIID riid, void **ppv)
{
    struct control_factory_impl *impl = impl_from_ctrl_factory(iface);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject_local) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppv = &impl->IActivationFactory_iface;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IControlFactory))
    {
        *ppv = &impl->IControlFactory_iface;
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI control_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI control_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI control_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI control_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.Controls.Control", 32, n); }
static HRESULT WINAPI control_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI control_fac_CreateInstance(void *This, void *outer, void **inner, void **instance)
{
    struct xaml_element *elem;
    init_elem_vtables();
    elem = xaml_element_create(CONTROL_TYPE_ELEMENT, L"Control");
    if (!elem) return E_OUTOFMEMORY;
    elem->outer = outer;
    if (inner)
    {
        *inner = &elem->IUIElement_iface;
        InterlockedIncrement(&elem->ref);
    }
    if (instance) *instance = (void *)&elem->IControl_iface;
    TRACE("control_fac_CreateInstance outer %p -> inner %p, instance %p\n", outer, inner ? *inner : NULL, instance ? *instance : NULL);
    return S_OK;
}

static HRESULT WINAPI control_fac_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    return control_fac_CreateInstance(iface, NULL, NULL, (void **)instance);
}

static HRESULT WINAPI control_fac_iface_QI(void *This, REFIID riid, void **ppv)
{
    return control_fac_QI((IActivationFactory *)impl_from_ctrl_fac_iface(This), riid, ppv);
}
static ULONG WINAPI control_fac_iface_AddRef(void *This) { return 2; }
static ULONG WINAPI control_fac_iface_Release(void *This) { return 1; }

static const IActivationFactoryVtbl control_activation_factory_vtbl = {
    control_fac_QI,
    control_fac_AddRef,
    control_fac_Release,
    control_fac_GetIids,
    control_fac_GetRuntimeClassName,
    control_fac_GetTrustLevel,
    control_fac_ActivateInstance
};

static const void *control_factory_vtbl[] = {
    (void *)control_fac_iface_QI,
    (void *)control_fac_iface_AddRef,
    (void *)control_fac_iface_Release,
    (void *)control_fac_GetIids,
    (void *)control_fac_GetRuntimeClassName,
    (void *)control_fac_GetTrustLevel,
    (void *)control_fac_CreateInstance
};

static struct control_factory_impl control_factory_impl_obj = {
    { &control_activation_factory_vtbl },
    { control_factory_vtbl }
};
IActivationFactory *control_factory = &control_factory_impl_obj.IActivationFactory_iface;

/* -------------------------------------------------------------------------
 * Button Factory (with IButtonFactory / IButtonBaseFactory WinRT aggregation)
 * ------------------------------------------------------------------------- */
static const GUID IID_IButtonFactory = {0x80a13c19, 0x843a, 0x451c, {0x8c, 0xf5, 0x44, 0xc7, 0x01, 0xb0, 0xe2, 0x16}};
static const GUID IID_IButtonBaseFactory = {0x389b7c71, 0x5220, 0x42b2, {0x99, 0x92, 0x26, 0x90, 0xc1, 0xa6, 0x70, 0x2f}};
static const GUID IID_IButtonBaseStatics = {0x67ef17e1, 0xfe37, 0x474f, {0x9e, 0x97, 0x3b, 0x5e, 0x0b, 0x30, 0xf2, 0xdf}};

struct button_base_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } IButtonBaseStatics_iface;
    struct { const void *lpVtbl; } IButtonBaseFactory_iface;
};
extern struct button_base_factory_impl button_base_factory_obj;

struct button_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } IButtonFactory_iface;
};

#define impl_from_btn_factory(ptr) ((struct button_factory_impl *)((char *)(ptr) - offsetof(struct button_factory_impl, IActivationFactory_iface)))
#define impl_from_btn_fac_iface(ptr) ((struct button_factory_impl *)((char *)(ptr) - offsetof(struct button_factory_impl, IButtonFactory_iface)))

static HRESULT WINAPI button_fac_QI(IActivationFactory *iface, REFIID riid, void **ppv)
{
    struct button_factory_impl *impl = impl_from_btn_factory(iface);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject_local) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppv = &impl->IActivationFactory_iface;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IButtonFactory) ||
        IsEqualGUID(riid, &IID_IButtonBaseFactory))
    {
        *ppv = &impl->IButtonFactory_iface;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IButtonBaseStatics))
    {
        *ppv = &button_base_factory_obj.IButtonBaseStatics_iface;
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI button_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI button_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI button_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI button_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.Controls.Button", 31, n); }
static HRESULT WINAPI button_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI button_fac_CreateInstance(void *This, void *outer, void **inner, void **instance)
{
    struct xaml_element *elem;
    init_elem_vtables();
    elem = xaml_element_create(CONTROL_TYPE_BUTTON, L"Button");
    if (!elem) return E_OUTOFMEMORY;
    elem->outer = outer;
    if (inner)
    {
        *inner = &elem->IUIElement_iface;
        InterlockedIncrement(&elem->ref);
    }
    if (instance) *instance = (void *)&elem->IButton_iface;
    TRACE("button_fac_CreateInstance outer %p -> inner %p, instance %p\n", outer, inner ? *inner : NULL, instance ? *instance : NULL);
    return S_OK;
}

static HRESULT WINAPI button_fac_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    return button_fac_CreateInstance(iface, NULL, NULL, (void **)instance);
}

static HRESULT WINAPI button_fac_iface_QI(void *This, REFIID riid, void **ppv)
{
    return button_fac_QI((IActivationFactory *)impl_from_btn_fac_iface(This), riid, ppv);
}
static ULONG WINAPI button_fac_iface_AddRef(void *This) { return 2; }
static ULONG WINAPI button_fac_iface_Release(void *This) { return 1; }

static const IActivationFactoryVtbl button_activation_factory_vtbl = {
    button_fac_QI,
    button_fac_AddRef,
    button_fac_Release,
    button_fac_GetIids,
    button_fac_GetRuntimeClassName,
    button_fac_GetTrustLevel,
    button_fac_ActivateInstance
};

static const void *button_factory_vtbl[] = {
    (void *)button_fac_iface_QI,
    (void *)button_fac_iface_AddRef,
    (void *)button_fac_iface_Release,
    (void *)button_fac_GetIids,
    (void *)button_fac_GetRuntimeClassName,
    (void *)button_fac_GetTrustLevel,
    (void *)button_fac_CreateInstance
};

static struct button_factory_impl button_factory_obj = {
    { &button_activation_factory_vtbl },
    { button_factory_vtbl }
};
IActivationFactory *button_factory = &button_factory_obj.IActivationFactory_iface;

/* -------------------------------------------------------------------------
 * ButtonBase Factory & Statics (Windows.UI.Xaml.Controls.Primitives.ButtonBase)
 * ------------------------------------------------------------------------- */
#define impl_from_btn_base_factory(ptr) ((struct button_base_factory_impl *)((char *)(ptr) - offsetof(struct button_base_factory_impl, IActivationFactory_iface)))
#define impl_from_btn_base_s_iface(ptr) ((struct button_base_factory_impl *)((char *)(ptr) - offsetof(struct button_base_factory_impl, IButtonBaseStatics_iface)))
#define impl_from_btn_base_fac_iface(ptr) ((struct button_base_factory_impl *)((char *)(ptr) - offsetof(struct button_base_factory_impl, IButtonBaseFactory_iface)))

static HRESULT WINAPI button_base_fac_QI(IActivationFactory *iface, REFIID riid, void **ppv)
{
    struct button_base_factory_impl *impl = impl_from_btn_base_factory(iface);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject_local) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppv = &impl->IActivationFactory_iface;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IButtonBaseStatics))
    {
        *ppv = &impl->IButtonBaseStatics_iface;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IButtonBaseFactory))
    {
        *ppv = &impl->IButtonBaseFactory_iface;
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI button_base_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI button_base_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI button_base_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI button_base_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.Controls.Primitives.ButtonBase", 46, n); }
static HRESULT WINAPI button_base_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI button_base_s_QI(void *This, REFIID riid, void **ppv)
{
    return button_base_fac_QI(&impl_from_btn_base_s_iface(This)->IActivationFactory_iface, riid, ppv);
}
static ULONG WINAPI button_base_s_AddRef(void *This) { return 2; }
static ULONG WINAPI button_base_s_Release(void *This) { return 1; }
static HRESULT WINAPI button_base_s_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI button_base_s_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.Controls.Primitives.ButtonBase", 46, n); }
static HRESULT WINAPI button_base_s_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI button_base_s_get_ClickModeProperty(void *This, void **v)
{
    if (!v) return E_POINTER;
    *v = create_dependency_property_named(L"ClickMode");
    return S_OK;
}
static HRESULT WINAPI button_base_s_get_IsPointerOverProperty(void *This, void **v)
{
    if (!v) return E_POINTER;
    *v = create_dependency_property_named(L"IsPointerOver");
    return S_OK;
}
static HRESULT WINAPI button_base_s_get_IsPressedProperty(void *This, void **v)
{
    if (!v) return E_POINTER;
    *v = create_dependency_property_named(L"IsPressed");
    return S_OK;
}
static HRESULT WINAPI button_base_s_get_CommandProperty(void *This, void **v)
{
    if (!v) return E_POINTER;
    *v = create_dependency_property_named(L"Command");
    return S_OK;
}
static HRESULT WINAPI button_base_s_get_CommandParameterProperty(void *This, void **v)
{
    if (!v) return E_POINTER;
    *v = create_dependency_property_named(L"CommandParameter");
    return S_OK;
}

static const void *button_base_statics_vtbl[] = {
    (void *)button_base_s_QI,
    (void *)button_base_s_AddRef,
    (void *)button_base_s_Release,
    (void *)button_base_s_GetIids,
    (void *)button_base_s_GetRuntimeClassName,
    (void *)button_base_s_GetTrustLevel,
    (void *)button_base_s_get_ClickModeProperty,
    (void *)button_base_s_get_IsPointerOverProperty,
    (void *)button_base_s_get_IsPressedProperty,
    (void *)button_base_s_get_CommandProperty,
    (void *)button_base_s_get_CommandParameterProperty
};

static HRESULT WINAPI button_base_fac_iface_QI(void *This, REFIID riid, void **ppv)
{
    return button_base_fac_QI(&impl_from_btn_base_fac_iface(This)->IActivationFactory_iface, riid, ppv);
}
static ULONG WINAPI button_base_fac_iface_AddRef(void *This) { return 2; }
static ULONG WINAPI button_base_fac_iface_Release(void *This) { return 1; }

static HRESULT WINAPI button_base_fac_CreateInstance(void *This, void *outer, void **inner, void **instance)
{
    struct xaml_element *elem;
    init_elem_vtables();
    elem = xaml_element_create(CONTROL_TYPE_BUTTON, L"ButtonBase");
    if (!elem) return E_OUTOFMEMORY;
    elem->outer = outer;
    if (inner)
    {
        *inner = &elem->IUIElement_iface;
        InterlockedIncrement(&elem->ref);
    }
    if (instance) *instance = (void *)&elem->IButtonBase_iface;
    TRACE("button_base_fac_CreateInstance outer %p -> inner %p, instance %p\n", outer, inner ? *inner : NULL, instance ? *instance : NULL);
    return S_OK;
}

static const void *button_base_factory_vtbl[] = {
    (void *)button_base_fac_iface_QI,
    (void *)button_base_fac_iface_AddRef,
    (void *)button_base_fac_iface_Release,
    (void *)button_base_s_GetIids,
    (void *)button_base_s_GetRuntimeClassName,
    (void *)button_base_s_GetTrustLevel,
    (void *)button_base_fac_CreateInstance
};

static HRESULT WINAPI button_base_fac_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    return button_base_fac_CreateInstance(iface, NULL, NULL, (void **)instance);
}

static const IActivationFactoryVtbl button_base_activation_factory_vtbl = {
    button_base_fac_QI,
    button_base_fac_AddRef,
    button_base_fac_Release,
    button_base_fac_GetIids,
    button_base_fac_GetRuntimeClassName,
    button_base_fac_GetTrustLevel,
    button_base_fac_ActivateInstance
};

struct button_base_factory_impl button_base_factory_obj = {
    { &button_base_activation_factory_vtbl },
    { button_base_statics_vtbl },
    { button_base_factory_vtbl }
};
IActivationFactory *button_base_factory = &button_base_factory_obj.IActivationFactory_iface;

static HRESULT WINAPI textblock_fac_Activate(void *This, IInspectable **instance)
{
    struct xaml_element *elem;
    init_elem_vtables();
    elem = xaml_element_create(CONTROL_TYPE_TEXTBLOCK, L"TextBlock");
    if (!elem) return E_OUTOFMEMORY;
    *instance = (IInspectable *)&elem->IUIElement_iface;
    return S_OK;
}
static const struct { const void *lpVtbl; } textblock_factory_obj = {
    (const void *[]){ (void *)grid_s_QI, (void *)grid_s_AddRef, (void *)grid_s_Release,
                      (void *)grid_s_GetIids, (void *)grid_s_GetRuntimeClassName, (void *)grid_s_GetTrustLevel,
                      (void *)textblock_fac_Activate }
};
IActivationFactory *textblock_factory = (IActivationFactory *)&textblock_factory_obj;

static HRESULT WINAPI border_fac_Activate(void *This, IInspectable **instance)
{
    struct xaml_element *elem;
    init_elem_vtables();
    elem = xaml_element_create(CONTROL_TYPE_BORDER, L"Border");
    if (!elem) return E_OUTOFMEMORY;
    *instance = (IInspectable *)&elem->IUIElement_iface;
    return S_OK;
}
static const struct { const void *lpVtbl; } border_factory_obj = {
    (const void *[]){ (void *)grid_s_QI, (void *)grid_s_AddRef, (void *)grid_s_Release,
                      (void *)grid_s_GetIids, (void *)grid_s_GetRuntimeClassName, (void *)grid_s_GetTrustLevel,
                      (void *)border_fac_Activate }
};
IActivationFactory *border_factory = (IActivationFactory *)&border_factory_obj;

static HRESULT WINAPI stackpanel_fac_Activate(void *This, IInspectable **instance)
{
    struct xaml_element *elem;
    init_elem_vtables();
    elem = xaml_element_create(CONTROL_TYPE_STACKPANEL, L"StackPanel");
    if (!elem) return E_OUTOFMEMORY;
    *instance = (IInspectable *)&elem->IUIElement_iface;
    return S_OK;
}
static const struct { const void *lpVtbl; } stackpanel_factory_obj = {
    (const void *[]){ (void *)grid_s_QI, (void *)grid_s_AddRef, (void *)grid_s_Release,
                      (void *)grid_s_GetIids, (void *)grid_s_GetRuntimeClassName, (void *)grid_s_GetTrustLevel,
                      (void *)stackpanel_fac_Activate }
};
IActivationFactory *stackpanel_factory = (IActivationFactory *)&stackpanel_factory_obj;

static const GUID IID_IUserControlFactory = {0x38b1ed92, 0xa28a, 0x4972, {0x93, 0xdf, 0xf4, 0xf7, 0x59, 0xb8, 0xaf, 0xd2}};

struct usercontrol_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } IUserControlFactory_iface;
    struct { const void *lpVtbl; } IUserControlStatics_iface;
};

#define impl_from_uc_factory(ptr) ((struct usercontrol_factory_impl *)((char *)(ptr) - offsetof(struct usercontrol_factory_impl, IActivationFactory_iface)))
#define impl_from_uc_fac_iface(ptr) ((struct usercontrol_factory_impl *)((char *)(ptr) - offsetof(struct usercontrol_factory_impl, IUserControlFactory_iface)))
#define impl_from_uc_stat_iface(ptr) ((struct usercontrol_factory_impl *)((char *)(ptr) - offsetof(struct usercontrol_factory_impl, IUserControlStatics_iface)))

static HRESULT WINAPI usercontrol_fac_QI(IActivationFactory *iface, REFIID riid, void **ppv)
{
    struct usercontrol_factory_impl *impl = impl_from_uc_factory(iface);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject_local) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppv = &impl->IActivationFactory_iface;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IUserControlFactory))
    {
        *ppv = &impl->IUserControlFactory_iface;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IUserControlStatics))
    {
        *ppv = &impl->IUserControlStatics_iface;
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI usercontrol_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI usercontrol_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI usercontrol_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI usercontrol_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.Controls.UserControl", 36, n); }
static HRESULT WINAPI usercontrol_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI usercontrol_fac_CreateInstance(void *This, void *outer, void **inner, void **instance)
{
    struct xaml_element *elem;
    init_elem_vtables();
    elem = xaml_element_create(CONTROL_TYPE_USERCONTROL, L"UserControl");
    if (!elem) return E_OUTOFMEMORY;
    elem->outer = outer;
    if (inner)
    {
        *inner = &elem->IUIElement_iface;
        InterlockedIncrement(&elem->ref);
    }
    if (instance) *instance = (void *)&elem->IUserControl_iface;
    TRACE("usercontrol_fac_CreateInstance outer %p -> inner %p, instance %p\n", outer, inner ? *inner : NULL, instance ? *instance : NULL);
    return S_OK;
}

static HRESULT WINAPI usercontrol_fac_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    return usercontrol_fac_CreateInstance(iface, NULL, NULL, (void **)instance);
}

static HRESULT WINAPI usercontrol_fac_iface_QI(void *This, REFIID riid, void **ppv)
{
    return usercontrol_fac_QI((IActivationFactory *)impl_from_uc_fac_iface(This), riid, ppv);
}
static ULONG WINAPI usercontrol_fac_iface_AddRef(void *This) { return 2; }
static ULONG WINAPI usercontrol_fac_iface_Release(void *This) { return 1; }

static HRESULT WINAPI usercontrol_stat_iface_QI(void *This, REFIID riid, void **ppv)
{
    return usercontrol_fac_QI((IActivationFactory *)impl_from_uc_stat_iface(This), riid, ppv);
}
static ULONG WINAPI usercontrol_stat_iface_AddRef(void *This) { return 2; }
static ULONG WINAPI usercontrol_stat_iface_Release(void *This) { return 1; }

static HRESULT WINAPI usercontrol_dp_Content(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"Content"); return S_OK; }

static const void *usercontrol_statics_vtbl[] = {
    (void *)usercontrol_stat_iface_QI,
    (void *)usercontrol_stat_iface_AddRef,
    (void *)usercontrol_stat_iface_Release,
    (void *)usercontrol_fac_GetIids,
    (void *)usercontrol_fac_GetRuntimeClassName,
    (void *)usercontrol_fac_GetTrustLevel,
    (void *)usercontrol_dp_Content /* 6 */
};

static const IActivationFactoryVtbl usercontrol_activation_factory_vtbl = {
    usercontrol_fac_QI,
    usercontrol_fac_AddRef,
    usercontrol_fac_Release,
    usercontrol_fac_GetIids,
    usercontrol_fac_GetRuntimeClassName,
    usercontrol_fac_GetTrustLevel,
    usercontrol_fac_ActivateInstance
};

static const void *usercontrol_factory_vtbl[] = {
    (void *)usercontrol_fac_iface_QI,
    (void *)usercontrol_fac_iface_AddRef,
    (void *)usercontrol_fac_iface_Release,
    (void *)usercontrol_fac_GetIids,
    (void *)usercontrol_fac_GetRuntimeClassName,
    (void *)usercontrol_fac_GetTrustLevel,
    (void *)usercontrol_fac_CreateInstance
};

static struct usercontrol_factory_impl usercontrol_factory_obj = {
    { &usercontrol_activation_factory_vtbl },
    { usercontrol_factory_vtbl },
    { usercontrol_statics_vtbl }
};
IActivationFactory *usercontrol_factory = &usercontrol_factory_obj.IActivationFactory_iface;

/* -------------------------------------------------------------------------
 * ContentControl Factory (with IContentControlFactory WinRT aggregation)
 * ------------------------------------------------------------------------- */
static const GUID IID_IContentControlFactory = {0xab017b19, 0x196e, 0x4ed5, {0xa2, 0x7f, 0x55, 0xc5, 0x8f, 0xb1, 0x67, 0x3f}};

struct contentcontrol_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } IContentControlFactory_iface;
};

#define impl_from_cc_factory(ptr) ((struct contentcontrol_factory_impl *)((char *)(ptr) - offsetof(struct contentcontrol_factory_impl, IActivationFactory_iface)))
#define impl_from_cc_fac_iface(ptr) ((struct contentcontrol_factory_impl *)((char *)(ptr) - offsetof(struct contentcontrol_factory_impl, IContentControlFactory_iface)))

static HRESULT WINAPI contentcontrol_fac_QI(IActivationFactory *iface, REFIID riid, void **ppv)
{
    struct contentcontrol_factory_impl *impl = impl_from_cc_factory(iface);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject_local) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppv = &impl->IActivationFactory_iface;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IContentControlFactory))
    {
        *ppv = &impl->IContentControlFactory_iface;
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI contentcontrol_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI contentcontrol_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI contentcontrol_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI contentcontrol_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.Controls.ContentControl", 39, n); }
static HRESULT WINAPI contentcontrol_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI contentcontrol_fac_CreateInstance(void *This, void *outer, void **inner, void **instance)
{
    struct xaml_element *elem;
    init_elem_vtables();
    elem = xaml_element_create(CONTROL_TYPE_CONTENTCONTROL, L"ContentControl");
    if (!elem) return E_OUTOFMEMORY;
    elem->outer = outer;
    if (inner)
    {
        *inner = &elem->IUIElement_iface;
        InterlockedIncrement(&elem->ref);
    }
    if (instance) *instance = (void *)&elem->IContentControl_iface;
    TRACE("contentcontrol_fac_CreateInstance outer %p -> inner %p, instance %p\n", outer, inner ? *inner : NULL, instance ? *instance : NULL);
    return S_OK;
}

static HRESULT WINAPI contentcontrol_fac_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    return contentcontrol_fac_CreateInstance(iface, NULL, NULL, (void **)instance);
}

static HRESULT WINAPI contentcontrol_fac_iface_QI(void *This, REFIID riid, void **ppv)
{
    return contentcontrol_fac_QI((IActivationFactory *)impl_from_cc_fac_iface(This), riid, ppv);
}
static ULONG WINAPI contentcontrol_fac_iface_AddRef(void *This) { return 2; }
static ULONG WINAPI contentcontrol_fac_iface_Release(void *This) { return 1; }

static const IActivationFactoryVtbl contentcontrol_activation_factory_vtbl = {
    contentcontrol_fac_QI,
    contentcontrol_fac_AddRef,
    contentcontrol_fac_Release,
    contentcontrol_fac_GetIids,
    contentcontrol_fac_GetRuntimeClassName,
    contentcontrol_fac_GetTrustLevel,
    contentcontrol_fac_ActivateInstance
};

static const void *contentcontrol_factory_vtbl[] = {
    (void *)contentcontrol_fac_iface_QI,
    (void *)contentcontrol_fac_iface_AddRef,
    (void *)contentcontrol_fac_iface_Release,
    (void *)contentcontrol_fac_GetIids,
    (void *)contentcontrol_fac_GetRuntimeClassName,
    (void *)contentcontrol_fac_GetTrustLevel,
    (void *)contentcontrol_fac_CreateInstance
};

static struct contentcontrol_factory_impl contentcontrol_factory_impl_obj = {
    { &contentcontrol_activation_factory_vtbl },
    { contentcontrol_factory_vtbl }
};
IActivationFactory *contentcontrol_factory = &contentcontrol_factory_impl_obj.IActivationFactory_iface;

/* -------------------------------------------------------------------------
 * ContentDialog Factory & Statics Implementation
 * ------------------------------------------------------------------------- */
struct content_dialog_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } IContentDialogFactory_iface;
    struct { const void *lpVtbl; } IContentDialogStatics_iface;
    struct { const void *lpVtbl; } IContentDialogStatics2_iface;
};

#define impl_from_cd_factory(ptr) ((struct content_dialog_factory_impl *)((char *)(ptr) - offsetof(struct content_dialog_factory_impl, IActivationFactory_iface)))
#define impl_from_cd_fac_iface(ptr) ((struct content_dialog_factory_impl *)((char *)(ptr) - offsetof(struct content_dialog_factory_impl, IContentDialogFactory_iface)))
#define impl_from_cd_stat_iface(ptr) ((struct content_dialog_factory_impl *)((char *)(ptr) - offsetof(struct content_dialog_factory_impl, IContentDialogStatics_iface)))
#define impl_from_cd_stat2_iface(ptr) ((struct content_dialog_factory_impl *)((char *)(ptr) - offsetof(struct content_dialog_factory_impl, IContentDialogStatics2_iface)))

static HRESULT WINAPI cd_fac_QI(IActivationFactory *iface, REFIID riid, void **ppv)
{
    struct content_dialog_factory_impl *impl = impl_from_cd_factory(iface);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject_local) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppv = &impl->IActivationFactory_iface;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IContentDialogFactory))
    {
        *ppv = &impl->IContentDialogFactory_iface;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IContentDialogStatics))
    {
        *ppv = &impl->IContentDialogStatics_iface;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IContentDialogStatics2))
    {
        *ppv = &impl->IContentDialogStatics2_iface;
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI cd_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI cd_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI cd_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI cd_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.Controls.ContentDialog", 38, n); }
static HRESULT WINAPI cd_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI cd_fac_CreateInstance(void *This, void *outer, void **inner, void **instance)
{
    struct xaml_element *elem;
    init_elem_vtables();
    elem = xaml_element_create(CONTROL_TYPE_CONTENTDIALOG, L"ContentDialog");
    if (!elem) return E_OUTOFMEMORY;
    elem->outer = outer;
    if (inner)
    {
        *inner = &elem->IUIElement_iface;
        InterlockedIncrement(&elem->ref);
    }
    if (instance) *instance = (void *)&elem->IContentDialog_iface;
    TRACE("cd_fac_CreateInstance outer %p -> inner %p, instance %p\n", outer, inner ? *inner : NULL, instance ? *instance : NULL);
    return S_OK;
}

static HRESULT WINAPI cd_fac_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    return cd_fac_CreateInstance(iface, NULL, NULL, (void **)instance);
}

static HRESULT WINAPI cd_fac_iface_QI(void *This, REFIID riid, void **ppv)
{
    return cd_fac_QI((IActivationFactory *)impl_from_cd_fac_iface(This), riid, ppv);
}
static ULONG WINAPI cd_fac_iface_AddRef(void *This) { return 2; }
static ULONG WINAPI cd_fac_iface_Release(void *This) { return 1; }

static const void *content_dialog_fac_vtbl[] = {
    (void *)cd_fac_iface_QI,
    (void *)cd_fac_iface_AddRef,
    (void *)cd_fac_iface_Release,
    (void *)cd_fac_GetIids,
    (void *)cd_fac_GetRuntimeClassName,
    (void *)cd_fac_GetTrustLevel,
    (void *)cd_fac_CreateInstance
};

static HRESULT WINAPI cd_stat_iface_QI(void *This, REFIID riid, void **ppv)
{
    return cd_fac_QI((IActivationFactory *)impl_from_cd_stat_iface(This), riid, ppv);
}
static ULONG WINAPI cd_stat_iface_AddRef(void *This) { return 2; }
static ULONG WINAPI cd_stat_iface_Release(void *This) { return 1; }

static HRESULT WINAPI cd_dp_Title(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"Title"); return S_OK; }
static HRESULT WINAPI cd_dp_TitleTemplate(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"TitleTemplate"); return S_OK; }
static HRESULT WINAPI cd_dp_FullSizeDesired(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"FullSizeDesired"); return S_OK; }
static HRESULT WINAPI cd_dp_PrimaryButtonText(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"PrimaryButtonText"); return S_OK; }
static HRESULT WINAPI cd_dp_SecondaryButtonText(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"SecondaryButtonText"); return S_OK; }
static HRESULT WINAPI cd_dp_PrimaryButtonCommand(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"PrimaryButtonCommand"); return S_OK; }
static HRESULT WINAPI cd_dp_SecondaryButtonCommand(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"SecondaryButtonCommand"); return S_OK; }
static HRESULT WINAPI cd_dp_PrimaryButtonCommandParameter(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"PrimaryButtonCommandParameter"); return S_OK; }
static HRESULT WINAPI cd_dp_SecondaryButtonCommandParameter(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"SecondaryButtonCommandParameter"); return S_OK; }
static HRESULT WINAPI cd_dp_IsPrimaryButtonEnabled(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"IsPrimaryButtonEnabled"); return S_OK; }
static HRESULT WINAPI cd_dp_IsSecondaryButtonEnabled(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"IsSecondaryButtonEnabled"); return S_OK; }

static const void *content_dialog_stat_vtbl[] = {
    (void *)cd_stat_iface_QI,
    (void *)cd_stat_iface_AddRef,
    (void *)cd_stat_iface_Release,
    (void *)cd_fac_GetIids,
    (void *)cd_fac_GetRuntimeClassName,
    (void *)cd_fac_GetTrustLevel,
    (void *)cd_dp_Title,
    (void *)cd_dp_TitleTemplate,
    (void *)cd_dp_FullSizeDesired,
    (void *)cd_dp_PrimaryButtonText,
    (void *)cd_dp_SecondaryButtonText,
    (void *)cd_dp_PrimaryButtonCommand,
    (void *)cd_dp_SecondaryButtonCommand,
    (void *)cd_dp_PrimaryButtonCommandParameter,
    (void *)cd_dp_SecondaryButtonCommandParameter,
    (void *)cd_dp_IsPrimaryButtonEnabled,
    (void *)cd_dp_IsSecondaryButtonEnabled
};

static HRESULT WINAPI cd_stat2_iface_QI(void *This, REFIID riid, void **ppv)
{
    return cd_fac_QI((IActivationFactory *)impl_from_cd_stat2_iface(This), riid, ppv);
}
static ULONG WINAPI cd_stat2_iface_AddRef(void *This) { return 2; }
static ULONG WINAPI cd_stat2_iface_Release(void *This) { return 1; }

static HRESULT WINAPI cd_dp_CloseButtonText(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"CloseButtonText"); return S_OK; }
static HRESULT WINAPI cd_dp_CloseButtonCommand(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"CloseButtonCommand"); return S_OK; }
static HRESULT WINAPI cd_dp_CloseButtonCommandParameter(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"CloseButtonCommandParameter"); return S_OK; }
static HRESULT WINAPI cd_dp_PrimaryButtonStyle(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"PrimaryButtonStyle"); return S_OK; }
static HRESULT WINAPI cd_dp_SecondaryButtonStyle(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"SecondaryButtonStyle"); return S_OK; }
static HRESULT WINAPI cd_dp_CloseButtonStyle(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"CloseButtonStyle"); return S_OK; }
static HRESULT WINAPI cd_dp_DefaultButton(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"DefaultButton"); return S_OK; }

static const void *content_dialog_stat2_vtbl[] = {
    (void *)cd_stat2_iface_QI,
    (void *)cd_stat2_iface_AddRef,
    (void *)cd_stat2_iface_Release,
    (void *)cd_fac_GetIids,
    (void *)cd_fac_GetRuntimeClassName,
    (void *)cd_fac_GetTrustLevel,
    (void *)cd_dp_CloseButtonText,
    (void *)cd_dp_CloseButtonCommand,
    (void *)cd_dp_CloseButtonCommandParameter,
    (void *)cd_dp_PrimaryButtonStyle,
    (void *)cd_dp_SecondaryButtonStyle,
    (void *)cd_dp_CloseButtonStyle,
    (void *)cd_dp_DefaultButton
};

static const IActivationFactoryVtbl content_dialog_activation_factory_vtbl = {
    cd_fac_QI,
    cd_fac_AddRef,
    cd_fac_Release,
    cd_fac_GetIids,
    cd_fac_GetRuntimeClassName,
    cd_fac_GetTrustLevel,
    cd_fac_ActivateInstance
};

static struct content_dialog_factory_impl content_dialog_factory_impl_obj = {
    { &content_dialog_activation_factory_vtbl },
    { content_dialog_fac_vtbl },
    { content_dialog_stat_vtbl },
    { content_dialog_stat2_vtbl }
};
IActivationFactory *content_dialog_factory = &content_dialog_factory_impl_obj.IActivationFactory_iface;

static HRESULT WINAPI rowdef_fac_Activate(void *This, IInspectable **instance)
{
    struct xaml_element *elem;
    init_elem_vtables();
    elem = xaml_element_create(CONTROL_TYPE_ROWDEF, L"RowDefinition");
    if (!elem) return E_OUTOFMEMORY;
    *instance = (IInspectable *)&elem->IRowDefinition_iface;
    return S_OK;
}

static HRESULT WINAPI rowdef_fac_QI(void *This, REFIID riid, void **ppv)
{
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppv = This;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IRowDefinitionStatics))
    {
        static const void *rowdef_s_vtbl[16] = {
            (void *)rowdef_fac_QI, (void *)grid_s_AddRef, (void *)grid_s_Release,
            (void *)grid_s_GetIids, (void *)grid_s_GetRuntimeClassName, (void *)grid_s_GetTrustLevel,
            (void *)elem_stub_ret_zero, /* HeightProperty */
            (void *)elem_stub_ret_zero, /* MaxHeightProperty */
            (void *)elem_stub_ret_zero  /* MinHeightProperty */
        };
        static const struct { const void *lpVtbl; } statics = { rowdef_s_vtbl };
        *ppv = (void *)&statics;
        return S_OK;
    }
    return E_NOINTERFACE;
}

static const struct { const void *lpVtbl; } rowdef_factory_obj = {
    (const void *[]){ (void *)rowdef_fac_QI, (void *)grid_s_AddRef, (void *)grid_s_Release,
                      (void *)grid_s_GetIids, (void *)grid_s_GetRuntimeClassName, (void *)grid_s_GetTrustLevel,
                      (void *)rowdef_fac_Activate }
};
IActivationFactory *rowdef_factory = (IActivationFactory *)&rowdef_factory_obj;

static HRESULT WINAPI coldef_fac_Activate(void *This, IInspectable **instance)
{
    struct xaml_element *elem;
    init_elem_vtables();
    elem = xaml_element_create(CONTROL_TYPE_COLDEF, L"ColumnDefinition");
    if (!elem) return E_OUTOFMEMORY;
    *instance = (IInspectable *)&elem->IColumnDefinition_iface;
    return S_OK;
}

static HRESULT WINAPI coldef_fac_QI(void *This, REFIID riid, void **ppv)
{
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppv = This;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IColumnDefinitionStatics))
    {
        static const void *coldef_s_vtbl[16] = {
            (void *)coldef_fac_QI, (void *)grid_s_AddRef, (void *)grid_s_Release,
            (void *)grid_s_GetIids, (void *)grid_s_GetRuntimeClassName, (void *)grid_s_GetTrustLevel,
            (void *)elem_stub_ret_zero, /* WidthProperty */
            (void *)elem_stub_ret_zero, /* MaxWidthProperty */
            (void *)elem_stub_ret_zero  /* MinWidthProperty */
        };
        static const struct { const void *lpVtbl; } statics = { coldef_s_vtbl };
        *ppv = (void *)&statics;
        return S_OK;
    }
    return E_NOINTERFACE;
}

static const struct { const void *lpVtbl; } coldef_factory_obj = {
    (const void *[]){ (void *)coldef_fac_QI, (void *)grid_s_AddRef, (void *)grid_s_Release,
                      (void *)grid_s_GetIids, (void *)grid_s_GetRuntimeClassName, (void *)grid_s_GetTrustLevel,
                      (void *)coldef_fac_Activate }
};
IActivationFactory *coldef_factory = (IActivationFactory *)&coldef_factory_obj;

/* -------------------------------------------------------------------------
 * Flyout Factory
 * ------------------------------------------------------------------------- */
struct flyout_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } IFlyoutFactory_iface;
};

#define impl_from_flyout_factory(ptr) ((struct flyout_factory_impl *)((char *)(ptr) - offsetof(struct flyout_factory_impl, IActivationFactory_iface)))
#define impl_from_flyout_fac_iface(ptr) ((struct flyout_factory_impl *)((char *)(ptr) - offsetof(struct flyout_factory_impl, IFlyoutFactory_iface)))

static HRESULT WINAPI flyout_fac_QI(IActivationFactory *iface, REFIID riid, void **ppv)
{
    struct flyout_factory_impl *impl = impl_from_flyout_factory(iface);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject_local) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppv = &impl->IActivationFactory_iface;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IFlyoutFactory))
    {
        *ppv = &impl->IFlyoutFactory_iface;
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI flyout_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI flyout_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI flyout_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI flyout_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.Controls.Flyout", 31, n); }
static HRESULT WINAPI flyout_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI flyout_fac_Activate(IActivationFactory *iface, IInspectable **instance)
{
    struct xaml_element *elem;
    init_elem_vtables();
    elem = xaml_element_create(CONTROL_TYPE_FLYOUT, L"Flyout");
    if (!elem) return E_OUTOFMEMORY;
    *instance = (IInspectable *)&elem->IFlyout_iface;
    return S_OK;
}

static const struct IActivationFactoryVtbl flyout_activation_factory_vtbl = {
    flyout_fac_QI,
    flyout_fac_AddRef,
    flyout_fac_Release,
    flyout_fac_GetIids,
    flyout_fac_GetRuntimeClassName,
    flyout_fac_GetTrustLevel,
    flyout_fac_Activate
};

static HRESULT WINAPI flyout_fac_iface_QI(void *This, REFIID riid, void **ppv) { return flyout_fac_QI(&impl_from_flyout_fac_iface(This)->IActivationFactory_iface, riid, ppv); }
static ULONG WINAPI flyout_fac_iface_AddRef(void *This) { return 2; }
static ULONG WINAPI flyout_fac_iface_Release(void *This) { return 1; }
static HRESULT WINAPI flyout_fac_CreateInstance(void *This, void *outer, void **inner, void **instance)
{
    struct xaml_element *elem;
    init_elem_vtables();
    elem = xaml_element_create(CONTROL_TYPE_FLYOUT, L"Flyout");
    if (!elem) return E_OUTOFMEMORY;
    elem->outer = outer;
    if (inner) *inner = (void *)&elem->IFlyout_iface;
    if (instance) *instance = (void *)&elem->IFlyout_iface;
    return S_OK;
}

static const void *flyout_factory_vtbl[] = {
    (void *)flyout_fac_iface_QI,
    (void *)flyout_fac_iface_AddRef,
    (void *)flyout_fac_iface_Release,
    (void *)flyout_fac_GetIids,
    (void *)flyout_fac_GetRuntimeClassName,
    (void *)flyout_fac_GetTrustLevel,
    (void *)flyout_fac_CreateInstance
};

static struct flyout_factory_impl flyout_factory_obj = {
    { &flyout_activation_factory_vtbl },
    { flyout_factory_vtbl }
};
IActivationFactory *flyout_factory = &flyout_factory_obj.IActivationFactory_iface;

/* -------------------------------------------------------------------------
 * MenuFlyout Factory & Instance Creator
 * ------------------------------------------------------------------------- */
struct menuflyout_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } IMenuFlyoutFactory_iface;
};

#define impl_from_menuflyout_factory(ptr) ((struct menuflyout_factory_impl *)((char *)(ptr) - offsetof(struct menuflyout_factory_impl, IActivationFactory_iface)))
#define impl_from_menuflyout_fac_iface(ptr) ((struct menuflyout_factory_impl *)((char *)(ptr) - offsetof(struct menuflyout_factory_impl, IMenuFlyoutFactory_iface)))

static HRESULT WINAPI menuflyout_fac_QI(IActivationFactory *iface, REFIID riid, void **ppv)
{
    struct menuflyout_factory_impl *impl = impl_from_menuflyout_factory(iface);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject_local) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppv = &impl->IActivationFactory_iface;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IMenuFlyoutFactory))
    {
        *ppv = &impl->IMenuFlyoutFactory_iface;
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI menuflyout_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI menuflyout_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI menuflyout_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI menuflyout_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.Controls.MenuFlyout", 35, n); }
static HRESULT WINAPI menuflyout_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI menuflyout_fac_Activate(IActivationFactory *iface, IInspectable **instance)
{
    struct xaml_element *elem;
    init_elem_vtables();
    elem = xaml_element_create(CONTROL_TYPE_MENUFLYOUT, L"MenuFlyout");
    if (!elem) return E_OUTOFMEMORY;
    *instance = (IInspectable *)&elem->IMenuFlyout_iface;
    return S_OK;
}

static const struct IActivationFactoryVtbl menuflyout_activation_factory_vtbl = {
    menuflyout_fac_QI,
    menuflyout_fac_AddRef,
    menuflyout_fac_Release,
    menuflyout_fac_GetIids,
    menuflyout_fac_GetRuntimeClassName,
    menuflyout_fac_GetTrustLevel,
    menuflyout_fac_Activate
};

static HRESULT WINAPI menuflyout_fac_iface_QI(void *This, REFIID riid, void **ppv) { return menuflyout_fac_QI(&impl_from_menuflyout_fac_iface(This)->IActivationFactory_iface, riid, ppv); }
static ULONG WINAPI menuflyout_fac_iface_AddRef(void *This) { return 2; }
static ULONG WINAPI menuflyout_fac_iface_Release(void *This) { return 1; }
static HRESULT WINAPI menuflyout_fac_CreateInstance(void *This, void *outer, void **inner, void **instance)
{
    struct xaml_element *elem;
    init_elem_vtables();
    elem = xaml_element_create(CONTROL_TYPE_MENUFLYOUT, L"MenuFlyout");
    if (!elem) return E_OUTOFMEMORY;
    elem->outer = outer;
    if (inner) *inner = (void *)&elem->IMenuFlyout_iface;
    if (instance) *instance = (void *)&elem->IMenuFlyout_iface;
    return S_OK;
}

static const void *menuflyout_factory_vtbl[] = {
    (void *)menuflyout_fac_iface_QI,
    (void *)menuflyout_fac_iface_AddRef,
    (void *)menuflyout_fac_iface_Release,
    (void *)flyout_fac_GetIids,
    (void *)flyout_fac_GetRuntimeClassName,
    (void *)flyout_fac_GetTrustLevel,
    (void *)menuflyout_fac_CreateInstance
};

static struct menuflyout_factory_impl menuflyout_factory_obj = {
    { &menuflyout_activation_factory_vtbl },
    { menuflyout_factory_vtbl }
};
IActivationFactory *menuflyout_factory = &menuflyout_factory_obj.IActivationFactory_iface;

static struct xaml_element *cached_display_menu_flyout = NULL;

HRESULT create_menu_flyout_instance(void **value)
{
    if (!value) return E_POINTER;
    init_elem_vtables();
    if (!cached_display_menu_flyout)
    {
        struct xaml_element *elem, *copy_item, *paste_item;
        elem = xaml_element_create(CONTROL_TYPE_MENUFLYOUT, L"DisplayContextMenu");
        if (!elem) return E_OUTOFMEMORY;
        copy_item = xaml_element_create(CONTROL_TYPE_MENUFLYOUTITEM, L"CopyMenuItem");
        paste_item = xaml_element_create(CONTROL_TYPE_MENUFLYOUTITEM, L"PasteMenuItem");
        if (copy_item) xaml_element_add_child(elem, copy_item);
        if (paste_item) xaml_element_add_child(elem, paste_item);
        cached_display_menu_flyout = elem;
    }
    elem_AddRef_internal(cached_display_menu_flyout);
    *value = &cached_display_menu_flyout->IMenuFlyout_iface;
    return S_OK;
}

/* -------------------------------------------------------------------------
 * MenuFlyoutItem Factory & Instance Creator
 * ------------------------------------------------------------------------- */
struct menuflyoutitem_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } IMenuFlyoutItemFactory_iface;
};

#define impl_from_menuflyoutitem_factory(ptr) ((struct menuflyoutitem_factory_impl *)((char *)(ptr) - offsetof(struct menuflyoutitem_factory_impl, IActivationFactory_iface)))
#define impl_from_menuflyoutitem_fac_iface(ptr) ((struct menuflyoutitem_factory_impl *)((char *)(ptr) - offsetof(struct menuflyoutitem_factory_impl, IMenuFlyoutItemFactory_iface)))

static HRESULT WINAPI menuflyoutitem_fac_QI(IActivationFactory *iface, REFIID riid, void **ppv)
{
    struct menuflyoutitem_factory_impl *impl = impl_from_menuflyoutitem_factory(iface);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject_local) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppv = &impl->IActivationFactory_iface;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IMenuFlyoutItemFactory))
    {
        *ppv = &impl->IMenuFlyoutItemFactory_iface;
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI menuflyoutitem_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI menuflyoutitem_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI menuflyoutitem_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI menuflyoutitem_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.Controls.MenuFlyoutItem", 39, n); }
static HRESULT WINAPI menuflyoutitem_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI menuflyoutitem_fac_Activate(IActivationFactory *iface, IInspectable **instance)
{
    struct xaml_element *elem;
    init_elem_vtables();
    elem = xaml_element_create(CONTROL_TYPE_MENUFLYOUTITEM, L"MenuFlyoutItem");
    if (!elem) return E_OUTOFMEMORY;
    *instance = (IInspectable *)&elem->IMenuFlyoutItem_iface;
    return S_OK;
}

static const struct IActivationFactoryVtbl menuflyoutitem_activation_factory_vtbl = {
    menuflyoutitem_fac_QI,
    menuflyoutitem_fac_AddRef,
    menuflyoutitem_fac_Release,
    menuflyoutitem_fac_GetIids,
    menuflyoutitem_fac_GetRuntimeClassName,
    menuflyoutitem_fac_GetTrustLevel,
    menuflyoutitem_fac_Activate
};

static HRESULT WINAPI menuflyoutitem_fac_iface_QI(void *This, REFIID riid, void **ppv) { return menuflyoutitem_fac_QI(&impl_from_menuflyoutitem_fac_iface(This)->IActivationFactory_iface, riid, ppv); }
static ULONG WINAPI menuflyoutitem_fac_iface_AddRef(void *This) { return 2; }
static ULONG WINAPI menuflyoutitem_fac_iface_Release(void *This) { return 1; }
static HRESULT WINAPI menuflyoutitem_fac_CreateInstance(void *This, void *outer, void **inner, void **instance)
{
    struct xaml_element *elem;
    init_elem_vtables();
    elem = xaml_element_create(CONTROL_TYPE_MENUFLYOUTITEM, L"MenuFlyoutItem");
    if (!elem) return E_OUTOFMEMORY;
    elem->outer = outer;
    if (inner) *inner = (void *)&elem->IMenuFlyoutItem_iface;
    if (instance) *instance = (void *)&elem->IMenuFlyoutItem_iface;
    return S_OK;
}

static const void *menuflyoutitem_factory_vtbl[] = {
    (void *)menuflyoutitem_fac_iface_QI,
    (void *)menuflyoutitem_fac_iface_AddRef,
    (void *)menuflyoutitem_fac_iface_Release,
    (void *)flyout_fac_GetIids,
    (void *)flyout_fac_GetRuntimeClassName,
    (void *)flyout_fac_GetTrustLevel,
    (void *)menuflyoutitem_fac_CreateInstance
};

static struct menuflyoutitem_factory_impl menuflyoutitem_factory_obj = {
    { &menuflyoutitem_activation_factory_vtbl },
    { menuflyoutitem_factory_vtbl }
};
IActivationFactory *menuflyoutitem_factory = &menuflyoutitem_factory_obj.IActivationFactory_iface;

HRESULT create_menu_flyout_item_instance(const WCHAR *name, void **value)
{
    struct xaml_element *elem;
    if (!value) return E_POINTER;
    init_elem_vtables();
    elem = xaml_element_create(CONTROL_TYPE_MENUFLYOUTITEM, name ? name : L"MenuFlyoutItem");
    if (!elem) return E_OUTOFMEMORY;
    *value = &elem->IMenuFlyoutItem_iface;
    return S_OK;
}

/* -------------------------------------------------------------------------
 * Setter Implementation & Factory
 * ------------------------------------------------------------------------- */
struct xaml_setter
{
    struct { const void *lpVtbl; } ISetter_iface;
    struct { const void *lpVtbl; } ISetterBase_iface;
    struct { const void *lpVtbl; } IDependencyObject_iface;
    LONG ref;
    void *property;
    IInspectable *value;
    boolean is_sealed;
};

#define impl_from_ISetter(ptr)     ((struct xaml_setter *)((char *)(ptr) - offsetof(struct xaml_setter, ISetter_iface)))
#define impl_from_ISetterBase(ptr) ((struct xaml_setter *)((char *)(ptr) - offsetof(struct xaml_setter, ISetterBase_iface)))
#define impl_from_setter_do(ptr)   ((struct xaml_setter *)((char *)(ptr) - offsetof(struct xaml_setter, IDependencyObject_iface)))

static HRESULT WINAPI setter_QI(void *This, REFIID riid, void **ppv)
{
    struct xaml_setter *impl = impl_from_ISetter(This);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject_local) ||
        IsEqualGUID(riid, &IID_ISetter))
    {
        *ppv = &impl->ISetter_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_ISetterBase))
    {
        *ppv = &impl->ISetterBase_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IDependencyObject_local) ||
        IsEqualGUID(riid, &IID_IDependencyObject2_local))
    {
        *ppv = &impl->IDependencyObject_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI setter_AddRef(void *This) { return InterlockedIncrement(&impl_from_ISetter(This)->ref); }
static ULONG WINAPI setter_Release(void *This)
{
    struct xaml_setter *impl = impl_from_ISetter(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref)
    {
        if (impl->property) IUnknown_Release((IUnknown *)impl->property);
        if (impl->value) IUnknown_Release((IUnknown *)impl->value);
        free(impl);
    }
    return ref;
}
static HRESULT WINAPI setter_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI setter_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.Setter", 23, n); }
static HRESULT WINAPI setter_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI setter_get_Property(void *This, void **v)
{
    struct xaml_setter *impl = impl_from_ISetter(This);
    if (!v) return E_POINTER;
    *v = impl->property;
    if (impl->property) IUnknown_AddRef((IUnknown *)impl->property);
    return S_OK;
}
static HRESULT WINAPI setter_put_Property(void *This, void *v)
{
    struct xaml_setter *impl = impl_from_ISetter(This);
    if (impl->property) IUnknown_Release((IUnknown *)impl->property);
    impl->property = v;
    if (v) IUnknown_AddRef((IUnknown *)v);
    return S_OK;
}
static HRESULT WINAPI setter_get_Value(void *This, IInspectable **v)
{
    struct xaml_setter *impl = impl_from_ISetter(This);
    if (!v) return E_POINTER;
    *v = impl->value;
    if (impl->value) IUnknown_AddRef((IUnknown *)impl->value);
    return S_OK;
}
static HRESULT WINAPI setter_put_Value(void *This, IInspectable *v)
{
    struct xaml_setter *impl = impl_from_ISetter(This);
    if (impl->value) IUnknown_Release((IUnknown *)impl->value);
    impl->value = v;
    if (v) IUnknown_AddRef((IUnknown *)v);
    return S_OK;
}

static const void *setter_vtbl[] = {
    (void *)setter_QI,
    (void *)setter_AddRef,
    (void *)setter_Release,
    (void *)setter_GetIids,
    (void *)setter_GetRuntimeClassName,
    (void *)setter_GetTrustLevel,
    (void *)setter_get_Property,
    (void *)setter_put_Property,
    (void *)setter_get_Value,
    (void *)setter_put_Value
};

static HRESULT WINAPI setter_base_QI(void *This, REFIID riid, void **ppv) { return setter_QI(impl_from_ISetterBase(This), riid, ppv); }
static ULONG WINAPI setter_base_AddRef(void *This) { return InterlockedIncrement(&impl_from_ISetterBase(This)->ref); }
static ULONG WINAPI setter_base_Release(void *This) { return setter_Release(impl_from_ISetterBase(This)); }
static HRESULT WINAPI setter_base_get_IsSealed(void *This, boolean *v)
{
    struct xaml_setter *impl = impl_from_ISetterBase(This);
    if (v) *v = impl->is_sealed;
    return S_OK;
}

static const void *setter_base_vtbl[] = {
    (void *)setter_base_QI,
    (void *)setter_base_AddRef,
    (void *)setter_base_Release,
    (void *)setter_GetIids,
    (void *)setter_GetRuntimeClassName,
    (void *)setter_GetTrustLevel,
    (void *)setter_base_get_IsSealed
};

static HRESULT WINAPI setter_do_QI(void *This, REFIID riid, void **ppv) { return setter_QI(impl_from_setter_do(This), riid, ppv); }
static ULONG WINAPI setter_do_AddRef(void *This) { return setter_AddRef(impl_from_setter_do(This)); }
static ULONG WINAPI setter_do_Release(void *This) { return setter_Release(impl_from_setter_do(This)); }
static const void *setter_do_vtbl[] = {
    (void *)setter_do_QI,
    (void *)setter_do_AddRef,
    (void *)setter_do_Release,
    (void *)setter_GetIids,
    (void *)setter_GetRuntimeClassName,
    (void *)setter_GetTrustLevel
};

static struct xaml_setter *xaml_setter_create(void *prop, IInspectable *val)
{
    struct xaml_setter *s = calloc(1, sizeof(*s));
    if (!s) return NULL;
    s->ISetter_iface.lpVtbl = setter_vtbl;
    s->ISetterBase_iface.lpVtbl = setter_base_vtbl;
    s->IDependencyObject_iface.lpVtbl = setter_do_vtbl;
    s->ref = 1;
    s->property = prop;
    s->value = val;
    if (prop) IUnknown_AddRef((IUnknown *)prop);
    if (val) IUnknown_AddRef((IUnknown *)val);
    return s;
}

struct setter_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } ISetterFactory_iface;
};

#define impl_from_setter_factory(ptr) ((struct setter_factory_impl *)((char *)(ptr) - offsetof(struct setter_factory_impl, IActivationFactory_iface)))
#define impl_from_setter_fac_iface(ptr) ((struct setter_factory_impl *)((char *)(ptr) - offsetof(struct setter_factory_impl, ISetterFactory_iface)))

static HRESULT WINAPI setter_fac_QI(IActivationFactory *iface, REFIID riid, void **ppv)
{
    struct setter_factory_impl *impl = impl_from_setter_factory(iface);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject_local) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppv = &impl->IActivationFactory_iface;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_ISetterFactory))
    {
        *ppv = &impl->ISetterFactory_iface;
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI setter_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI setter_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI setter_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI setter_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.Setter", 23, n); }
static HRESULT WINAPI setter_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI setter_fac_Activate(IActivationFactory *iface, IInspectable **instance)
{
    struct xaml_setter *s = xaml_setter_create(NULL, NULL);
    if (!s) return E_OUTOFMEMORY;
    *instance = (IInspectable *)&s->ISetter_iface;
    return S_OK;
}

static const struct IActivationFactoryVtbl setter_activation_factory_vtbl = {
    setter_fac_QI,
    setter_fac_AddRef,
    setter_fac_Release,
    setter_fac_GetIids,
    setter_fac_GetRuntimeClassName,
    setter_fac_GetTrustLevel,
    setter_fac_Activate
};

static HRESULT WINAPI setter_fac_iface_QI(void *This, REFIID riid, void **ppv) { return setter_fac_QI(&impl_from_setter_fac_iface(This)->IActivationFactory_iface, riid, ppv); }
static ULONG WINAPI setter_fac_iface_AddRef(void *This) { return 2; }
static ULONG WINAPI setter_fac_iface_Release(void *This) { return 1; }
static HRESULT WINAPI setter_fac_CreateInstance(void *This, void *targetProperty, IInspectable *value, void **instance)
{
    struct xaml_setter *s;
    if (!instance) return E_POINTER;
    s = xaml_setter_create(targetProperty, value);
    if (!s) return E_OUTOFMEMORY;
    *instance = (void *)&s->ISetter_iface;
    TRACE("setter_fac_CreateInstance prop=%p, val=%p -> %p\n", targetProperty, value, *instance);
    return S_OK;
}

static const void *setter_factory_vtbl[] = {
    (void *)setter_fac_iface_QI,
    (void *)setter_fac_iface_AddRef,
    (void *)setter_fac_iface_Release,
    (void *)setter_fac_GetIids,
    (void *)setter_fac_GetRuntimeClassName,
    (void *)setter_fac_GetTrustLevel,
    (void *)setter_fac_CreateInstance
};

static struct setter_factory_impl setter_factory_obj = {
    { &setter_activation_factory_vtbl },
    { setter_factory_vtbl }
};
IActivationFactory *setter_factory = &setter_factory_obj.IActivationFactory_iface;

/* -------------------------------------------------------------------------
 * Style & SetterBaseCollection Implementation & Factory
 * ------------------------------------------------------------------------- */
struct xaml_style
{
    struct { const void *lpVtbl; } IStyle_iface;
    struct { const void *lpVtbl; } ISetterBaseCollection_iface;
    struct { const void *lpVtbl; } IVector_SetterBase_iface;
    struct { const void *lpVtbl; } IDependencyObject_iface;
    LONG ref;

    void **setters;
    UINT32 setters_count;
    UINT32 setters_capacity;

    boolean is_sealed;
    void *based_on;
};

#define impl_from_IStyle(ptr)                 ((struct xaml_style *)((char *)(ptr) - offsetof(struct xaml_style, IStyle_iface)))
#define impl_from_ISetterBaseCollection(ptr)  ((struct xaml_style *)((char *)(ptr) - offsetof(struct xaml_style, ISetterBaseCollection_iface)))
#define impl_from_IVector_SetterBase(ptr)     ((struct xaml_style *)((char *)(ptr) - offsetof(struct xaml_style, IVector_SetterBase_iface)))
#define impl_from_style_do(ptr)               ((struct xaml_style *)((char *)(ptr) - offsetof(struct xaml_style, IDependencyObject_iface)))

static HRESULT WINAPI style_QI(void *This, REFIID riid, void **ppv)
{
    struct xaml_style *impl = impl_from_IStyle(This);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject_local) ||
        IsEqualGUID(riid, &IID_IStyle))
    {
        *ppv = &impl->IStyle_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IDependencyObject_local) ||
        IsEqualGUID(riid, &IID_IDependencyObject2_local))
    {
        *ppv = &impl->IDependencyObject_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI style_AddRef(void *This) { return InterlockedIncrement(&impl_from_IStyle(This)->ref); }
static ULONG WINAPI style_Release(void *This)
{
    struct xaml_style *impl = impl_from_IStyle(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref)
    {
        UINT32 i;
        for (i = 0; i < impl->setters_count; i++)
        {
            if (impl->setters[i]) IUnknown_Release((IUnknown *)impl->setters[i]);
        }
        free(impl->setters);
        if (impl->based_on) IUnknown_Release((IUnknown *)impl->based_on);
        free(impl);
    }
    return ref;
}
static HRESULT WINAPI style_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI style_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.Style", 22, n); }
static HRESULT WINAPI style_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI style_get_IsSealed(void *This, boolean *v)
{
    struct xaml_style *impl = impl_from_IStyle(This);
    if (v) *v = impl->is_sealed;
    return S_OK;
}
static HRESULT WINAPI style_get_Setters(void *This, void **v)
{
    struct xaml_style *impl = impl_from_IStyle(This);
    if (!v) return E_POINTER;
    *v = &impl->ISetterBaseCollection_iface;
    InterlockedIncrement(&impl->ref);
    return S_OK;
}
static HRESULT WINAPI style_get_TargetType(void *This, void *v) { return S_OK; }
static HRESULT WINAPI style_put_TargetType(void *This, void *v) { return S_OK; }
static HRESULT WINAPI style_get_BasedOn(void *This, void **v)
{
    struct xaml_style *impl = impl_from_IStyle(This);
    if (!v) return E_POINTER;
    *v = impl->based_on;
    if (impl->based_on) IUnknown_AddRef((IUnknown *)impl->based_on);
    return S_OK;
}
static HRESULT WINAPI style_put_BasedOn(void *This, void *v)
{
    struct xaml_style *impl = impl_from_IStyle(This);
    if (impl->based_on) IUnknown_Release((IUnknown *)impl->based_on);
    impl->based_on = v;
    if (v) IUnknown_AddRef((IUnknown *)v);
    return S_OK;
}
static HRESULT WINAPI style_Seal(void *This)
{
    struct xaml_style *impl = impl_from_IStyle(This);
    impl->is_sealed = TRUE;
    return S_OK;
}

static const void *style_vtbl[] = {
    (void *)style_QI,
    (void *)style_AddRef,
    (void *)style_Release,
    (void *)style_GetIids,
    (void *)style_GetRuntimeClassName,
    (void *)style_GetTrustLevel,
    (void *)style_get_IsSealed,
    (void *)style_get_Setters,
    (void *)style_get_TargetType,
    (void *)style_put_TargetType,
    (void *)style_get_BasedOn,
    (void *)style_put_BasedOn,
    (void *)style_Seal
};

static HRESULT WINAPI style_col_QI(void *This, REFIID riid, void **ppv)
{
    struct xaml_style *impl = impl_from_ISetterBaseCollection(This);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject_local) ||
        IsEqualGUID(riid, &IID_ISetterBaseCollection))
    {
        *ppv = &impl->ISetterBaseCollection_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IVector_SetterBase) ||
        IsEqualGUID(riid, &IID_IIterable_SetterBase))
    {
        *ppv = &impl->IVector_SetterBase_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IDependencyObject_local) ||
        IsEqualGUID(riid, &IID_IDependencyObject2_local))
    {
        *ppv = &impl->IDependencyObject_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI style_col_AddRef(void *This) { return InterlockedIncrement(&impl_from_ISetterBaseCollection(This)->ref); }
static ULONG WINAPI style_col_Release(void *This) { return style_Release(&impl_from_ISetterBaseCollection(This)->IStyle_iface); }
static HRESULT WINAPI style_col_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.SetterBaseCollection", 36, n); }
static HRESULT WINAPI style_col_get_IsSealed(void *This, boolean *v)
{
    struct xaml_style *impl = impl_from_ISetterBaseCollection(This);
    if (v) *v = impl->is_sealed;
    return S_OK;
}

static const void *setter_collection_vtbl[] = {
    (void *)style_col_QI,
    (void *)style_col_AddRef,
    (void *)style_col_Release,
    (void *)style_GetIids,
    (void *)style_col_GetRuntimeClassName,
    (void *)style_GetTrustLevel,
    (void *)style_col_get_IsSealed
};

static HRESULT WINAPI style_vec_QI(void *This, REFIID riid, void **ppv) { return style_col_QI(impl_from_IVector_SetterBase(This), riid, ppv); }
static ULONG WINAPI style_vec_AddRef(void *This) { return InterlockedIncrement(&impl_from_IVector_SetterBase(This)->ref); }
static ULONG WINAPI style_vec_Release(void *This) { return style_Release(&impl_from_IVector_SetterBase(This)->IStyle_iface); }
static HRESULT WINAPI style_vec_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.Foundation.Collections.IVector`1<Windows.UI.Xaml.SetterBase>", 66, n); }

static HRESULT WINAPI style_vec_GetAt(void *This, unsigned int index, void **item)
{
    struct xaml_style *impl = impl_from_IVector_SetterBase(This);
    if (!item) return E_POINTER;
    if (index >= impl->setters_count) return E_BOUNDS;
    *item = impl->setters[index];
    if (*item) IUnknown_AddRef((IUnknown *)*item);
    return S_OK;
}
static HRESULT WINAPI style_vec_get_Size(void *This, unsigned int *size)
{
    struct xaml_style *impl = impl_from_IVector_SetterBase(This);
    if (!size) return E_POINTER;
    *size = impl->setters_count;
    return S_OK;
}
static HRESULT WINAPI style_vec_GetView(void *This, void **view) { if (view) *view = NULL; return E_NOTIMPL; }
static HRESULT WINAPI style_vec_IndexOf(void *This, void *item, unsigned int *index, boolean *found)
{
    struct xaml_style *impl = impl_from_IVector_SetterBase(This);
    UINT32 i;
    for (i = 0; i < impl->setters_count; i++)
    {
        if (impl->setters[i] == item)
        {
            if (index) *index = i;
            if (found) *found = TRUE;
            return S_OK;
        }
    }
    if (found) *found = FALSE;
    return S_OK;
}
static HRESULT WINAPI style_vec_SetAt(void *This, unsigned int index, void *item) { return S_OK; }
static HRESULT WINAPI style_vec_InsertAt(void *This, unsigned int index, void *item) { return S_OK; }
static HRESULT WINAPI style_vec_RemoveAt(void *This, unsigned int index) { return S_OK; }
static HRESULT WINAPI style_vec_Append(void *This, void *item)
{
    struct xaml_style *impl = impl_from_IVector_SetterBase(This);
    if (impl->setters_count >= impl->setters_capacity)
    {
        UINT32 new_cap = impl->setters_capacity ? impl->setters_capacity * 2 : 8;
        void **new_arr = realloc(impl->setters, new_cap * sizeof(*new_arr));
        if (!new_arr) return E_OUTOFMEMORY;
        impl->setters = new_arr;
        impl->setters_capacity = new_cap;
    }
    if (item) IUnknown_AddRef((IUnknown *)item);
    impl->setters[impl->setters_count++] = item;
    TRACE("Style %p setters appended item %p, count=%u\n", impl, item, impl->setters_count);
    return S_OK;
}
static HRESULT WINAPI style_vec_RemoveAtEnd(void *This) { return S_OK; }
static HRESULT WINAPI style_vec_Clear(void *This) { return S_OK; }
static HRESULT WINAPI style_vec_GetMany(void *This, unsigned int startIndex, unsigned int capacity, void **items, unsigned int *actual) { if (actual) *actual = 0; return S_OK; }
static HRESULT WINAPI style_vec_ReplaceAll(void *This, unsigned int count, void **value) { return S_OK; }

static const void *vector_setter_vtbl[] = {
    (void *)style_vec_QI,
    (void *)style_vec_AddRef,
    (void *)style_vec_Release,
    (void *)style_GetIids,
    (void *)style_vec_GetRuntimeClassName,
    (void *)style_GetTrustLevel,
    (void *)style_vec_GetAt,
    (void *)style_vec_get_Size,
    (void *)style_vec_GetView,
    (void *)style_vec_IndexOf,
    (void *)style_vec_SetAt,
    (void *)style_vec_InsertAt,
    (void *)style_vec_RemoveAt,
    (void *)style_vec_Append,
    (void *)style_vec_RemoveAtEnd,
    (void *)style_vec_Clear,
    (void *)style_vec_GetMany,
    (void *)style_vec_ReplaceAll
};

static HRESULT WINAPI style_do_QI(void *This, REFIID riid, void **ppv) { return style_QI(impl_from_style_do(This), riid, ppv); }
static ULONG WINAPI style_do_AddRef(void *This) { return style_AddRef(impl_from_style_do(This)); }
static ULONG WINAPI style_do_Release(void *This) { return style_Release(&impl_from_style_do(This)->IStyle_iface); }
static const void *style_do_vtbl[] = {
    (void *)style_do_QI,
    (void *)style_do_AddRef,
    (void *)style_do_Release,
    (void *)style_GetIids,
    (void *)style_GetRuntimeClassName,
    (void *)style_GetTrustLevel
};

static struct xaml_style *xaml_style_create(void)
{
    struct xaml_style *s = calloc(1, sizeof(*s));
    if (!s) return NULL;
    s->IStyle_iface.lpVtbl = style_vtbl;
    s->ISetterBaseCollection_iface.lpVtbl = setter_collection_vtbl;
    s->IVector_SetterBase_iface.lpVtbl = vector_setter_vtbl;
    s->IDependencyObject_iface.lpVtbl = style_do_vtbl;
    s->ref = 1;
    return s;
}

static void xaml_style_release(struct xaml_style *s)
{
    if (s) style_Release(&s->IStyle_iface);
}

struct style_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } IStyleFactory_iface;
};

#define impl_from_style_factory(ptr) ((struct style_factory_impl *)((char *)(ptr) - offsetof(struct style_factory_impl, IActivationFactory_iface)))
#define impl_from_style_fac_iface(ptr) ((struct style_factory_impl *)((char *)(ptr) - offsetof(struct style_factory_impl, IStyleFactory_iface)))

static HRESULT WINAPI style_fac_QI(IActivationFactory *iface, REFIID riid, void **ppv)
{
    struct style_factory_impl *impl = impl_from_style_factory(iface);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject_local) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppv = &impl->IActivationFactory_iface;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IStyleFactory))
    {
        *ppv = &impl->IStyleFactory_iface;
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI style_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI style_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI style_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI style_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.Style", 22, n); }
static HRESULT WINAPI style_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI style_fac_Activate(IActivationFactory *iface, IInspectable **instance)
{
    struct xaml_style *s = xaml_style_create();
    if (!s) return E_OUTOFMEMORY;
    *instance = (IInspectable *)&s->IStyle_iface;
    return S_OK;
}

static const struct IActivationFactoryVtbl style_activation_factory_vtbl = {
    style_fac_QI,
    style_fac_AddRef,
    style_fac_Release,
    style_fac_GetIids,
    style_fac_GetRuntimeClassName,
    style_fac_GetTrustLevel,
    style_fac_Activate
};

static HRESULT WINAPI style_fac_iface_QI(void *This, REFIID riid, void **ppv) { return style_fac_QI(&impl_from_style_fac_iface(This)->IActivationFactory_iface, riid, ppv); }
static ULONG WINAPI style_fac_iface_AddRef(void *This) { return 2; }
static ULONG WINAPI style_fac_iface_Release(void *This) { return 1; }
static HRESULT WINAPI style_fac_CreateInstance(void *This, void *targetType, void **instance)
{
    struct xaml_style *s;
    if (!instance) return E_POINTER;
    s = xaml_style_create();
    if (!s) return E_OUTOFMEMORY;
    *instance = (void *)&s->IStyle_iface;
    TRACE("style_fac_CreateInstance -> %p\n", *instance);
    return S_OK;
}

static const void *style_factory_vtbl[] = {
    (void *)style_fac_iface_QI,
    (void *)style_fac_iface_AddRef,
    (void *)style_fac_iface_Release,
    (void *)style_fac_GetIids,
    (void *)style_fac_GetRuntimeClassName,
    (void *)style_fac_GetTrustLevel,
    (void *)style_fac_CreateInstance
};

static struct style_factory_impl style_factory_obj = {
    { &style_activation_factory_vtbl },
    { style_factory_vtbl }
};
IActivationFactory *style_factory = &style_factory_obj.IActivationFactory_iface;

/* -------------------------------------------------------------------------
 * Binding Implementation & Factory
 * ------------------------------------------------------------------------- */
struct xaml_binding
{
    struct { const void *lpVtbl; } IBinding_iface;
    struct { const void *lpVtbl; } IBindingBase_iface;
    struct { const void *lpVtbl; } IDependencyObject_iface;
    LONG ref;
    void *outer;
    void *path;
    INT32 mode;
    void *source;
    HSTRING element_name;
};

#define impl_from_IBinding(ptr)     ((struct xaml_binding *)((char *)(ptr) - offsetof(struct xaml_binding, IBinding_iface)))
#define impl_from_IBindingBase(ptr) ((struct xaml_binding *)((char *)(ptr) - offsetof(struct xaml_binding, IBindingBase_iface)))
#define impl_from_binding_do(ptr)   ((struct xaml_binding *)((char *)(ptr) - offsetof(struct xaml_binding, IDependencyObject_iface)))

static HRESULT WINAPI binding_QI(void *This, REFIID riid, void **ppv)
{
    struct xaml_binding *impl = impl_from_IBinding(This);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject_local) ||
        IsEqualGUID(riid, &IID_IBinding) ||
        IsEqualGUID(riid, &IID_IBinding2))
    {
        *ppv = &impl->IBinding_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IBindingBase))
    {
        *ppv = &impl->IBindingBase_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IDependencyObject_local) ||
        IsEqualGUID(riid, &IID_IDependencyObject2_local))
    {
        *ppv = &impl->IDependencyObject_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI binding_AddRef(void *This) { return InterlockedIncrement(&impl_from_IBinding(This)->ref); }
static ULONG WINAPI binding_Release(void *This)
{
    struct xaml_binding *impl = impl_from_IBinding(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref)
    {
        if (impl->path) IUnknown_Release((IUnknown *)impl->path);
        if (impl->source) IUnknown_Release((IUnknown *)impl->source);
        if (impl->element_name) WindowsDeleteString(impl->element_name);
        free(impl);
    }
    return ref;
}
static HRESULT WINAPI binding_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI binding_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.Data.Binding", 29, n); }
static HRESULT WINAPI binding_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI binding_get_Path(void *This, void **v)
{
    struct xaml_binding *impl = impl_from_IBinding(This);
    if (!v) return E_POINTER;
    *v = impl->path;
    if (impl->path) IUnknown_AddRef((IUnknown *)impl->path);
    return S_OK;
}
static HRESULT WINAPI binding_put_Path(void *This, void *v)
{
    struct xaml_binding *impl = impl_from_IBinding(This);
    if (impl->path) IUnknown_Release((IUnknown *)impl->path);
    impl->path = v;
    if (v) IUnknown_AddRef((IUnknown *)v);
    return S_OK;
}
static HRESULT WINAPI binding_get_Mode(void *This, INT32 *v) { struct xaml_binding *impl = impl_from_IBinding(This); if (v) *v = impl->mode; return S_OK; }
static HRESULT WINAPI binding_put_Mode(void *This, INT32 v) { struct xaml_binding *impl = impl_from_IBinding(This); impl->mode = v; return S_OK; }
static HRESULT WINAPI binding_get_Source(void *This, void **v)
{
    struct xaml_binding *impl = impl_from_IBinding(This);
    if (!v) return E_POINTER;
    *v = impl->source;
    if (impl->source) IUnknown_AddRef((IUnknown *)impl->source);
    return S_OK;
}
static HRESULT WINAPI binding_put_Source(void *This, void *v)
{
    struct xaml_binding *impl = impl_from_IBinding(This);
    if (impl->source) IUnknown_Release((IUnknown *)impl->source);
    impl->source = v;
    if (v) IUnknown_AddRef((IUnknown *)v);
    return S_OK;
}

static const void *binding_vtbl[32] = {
    (void *)binding_QI,
    (void *)binding_AddRef,
    (void *)binding_Release,
    (void *)binding_GetIids,
    (void *)binding_GetRuntimeClassName,
    (void *)binding_GetTrustLevel,
    (void *)binding_get_Path,
    (void *)binding_put_Path,
    (void *)binding_get_Mode,
    (void *)binding_put_Mode,
    (void *)binding_get_Source,
    (void *)binding_put_Source
};

static HRESULT WINAPI binding_base_QI(void *This, REFIID riid, void **ppv) { return binding_QI(impl_from_IBindingBase(This), riid, ppv); }
static ULONG WINAPI binding_base_AddRef(void *This) { return InterlockedIncrement(&impl_from_IBindingBase(This)->ref); }
static ULONG WINAPI binding_base_Release(void *This) { return binding_Release(impl_from_IBindingBase(This)); }
static HRESULT WINAPI binding_base_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.Data.BindingBase", 33, n); }

static const void *binding_base_vtbl[] = {
    (void *)binding_base_QI,
    (void *)binding_base_AddRef,
    (void *)binding_base_Release,
    (void *)binding_GetIids,
    (void *)binding_base_GetRuntimeClassName,
    (void *)binding_GetTrustLevel
};

static HRESULT WINAPI binding_do_QI(void *This, REFIID riid, void **ppv) { return binding_QI(impl_from_binding_do(This), riid, ppv); }
static ULONG WINAPI binding_do_AddRef(void *This) { return binding_AddRef(impl_from_binding_do(This)); }
static ULONG WINAPI binding_do_Release(void *This) { return binding_Release(impl_from_binding_do(This)); }
static const void *binding_do_vtbl[] = {
    (void *)binding_do_QI,
    (void *)binding_do_AddRef,
    (void *)binding_do_Release,
    (void *)binding_GetIids,
    (void *)binding_GetRuntimeClassName,
    (void *)binding_GetTrustLevel
};

static struct xaml_binding *xaml_binding_create(void)
{
    struct xaml_binding *b = calloc(1, sizeof(*b));
    if (!b) return NULL;
    b->IBinding_iface.lpVtbl = binding_vtbl;
    b->IBindingBase_iface.lpVtbl = binding_base_vtbl;
    b->IDependencyObject_iface.lpVtbl = binding_do_vtbl;
    b->ref = 1;
    return b;
}

struct binding_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } IBindingFactory_iface;
};

#define impl_from_binding_factory(ptr) ((struct binding_factory_impl *)((char *)(ptr) - offsetof(struct binding_factory_impl, IActivationFactory_iface)))
#define impl_from_binding_fac_iface(ptr) ((struct binding_factory_impl *)((char *)(ptr) - offsetof(struct binding_factory_impl, IBindingFactory_iface)))

static HRESULT WINAPI binding_fac_QI(IActivationFactory *iface, REFIID riid, void **ppv)
{
    struct binding_factory_impl *impl = impl_from_binding_factory(iface);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject_local) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppv = &impl->IActivationFactory_iface;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IBindingFactory))
    {
        *ppv = &impl->IBindingFactory_iface;
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI binding_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI binding_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI binding_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI binding_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.Data.Binding", 29, n); }
static HRESULT WINAPI binding_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI binding_fac_Activate(IActivationFactory *iface, IInspectable **instance)
{
    struct xaml_binding *b = xaml_binding_create();
    if (!b) return E_OUTOFMEMORY;
    *instance = (IInspectable *)&b->IBinding_iface;
    return S_OK;
}

static const struct IActivationFactoryVtbl binding_activation_factory_vtbl = {
    binding_fac_QI,
    binding_fac_AddRef,
    binding_fac_Release,
    binding_fac_GetIids,
    binding_fac_GetRuntimeClassName,
    binding_fac_GetTrustLevel,
    binding_fac_Activate
};

static HRESULT WINAPI binding_fac_iface_QI(void *This, REFIID riid, void **ppv) { return binding_fac_QI(&impl_from_binding_fac_iface(This)->IActivationFactory_iface, riid, ppv); }
static ULONG WINAPI binding_fac_iface_AddRef(void *This) { return 2; }
static ULONG WINAPI binding_fac_iface_Release(void *This) { return 1; }
static HRESULT WINAPI binding_fac_CreateInstance(void *This, void *outer, void **inner, void **instance)
{
    struct xaml_binding *b;
    if (!instance) return E_POINTER;
    b = xaml_binding_create();
    if (!b) return E_OUTOFMEMORY;
    b->outer = outer;
    if (inner) *inner = (void *)&b->IBinding_iface;
    *instance = (void *)&b->IBinding_iface;
    TRACE("binding_fac_CreateInstance -> %p\n", *instance);
    return S_OK;
}

static const void *binding_factory_vtbl[] = {
    (void *)binding_fac_iface_QI,
    (void *)binding_fac_iface_AddRef,
    (void *)binding_fac_iface_Release,
    (void *)binding_fac_GetIids,
    (void *)binding_fac_GetRuntimeClassName,
    (void *)binding_fac_GetTrustLevel,
    (void *)binding_fac_CreateInstance
};

static struct binding_factory_impl binding_factory_obj = {
    { &binding_activation_factory_vtbl },
    { binding_factory_vtbl }
};
IActivationFactory *binding_factory = &binding_factory_obj.IActivationFactory_iface;

/* -------------------------------------------------------------------------
 * PropertyPath Implementation & Factory
 * ------------------------------------------------------------------------- */
struct xaml_property_path
{
    struct { const void *lpVtbl; } IPropertyPath_iface;
    LONG ref;
    HSTRING path;
};

#define impl_from_IPropertyPath(ptr) ((struct xaml_property_path *)((char *)(ptr) - offsetof(struct xaml_property_path, IPropertyPath_iface)))

static HRESULT WINAPI prop_path_QI(void *This, REFIID riid, void **ppv)
{
    struct xaml_property_path *impl = impl_from_IPropertyPath(This);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject_local) ||
        IsEqualGUID(riid, &IID_IPropertyPath))
    {
        *ppv = &impl->IPropertyPath_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI prop_path_AddRef(void *This) { return InterlockedIncrement(&impl_from_IPropertyPath(This)->ref); }
static ULONG WINAPI prop_path_Release(void *This)
{
    struct xaml_property_path *impl = impl_from_IPropertyPath(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref)
    {
        if (impl->path) WindowsDeleteString(impl->path);
        free(impl);
    }
    return ref;
}
static HRESULT WINAPI prop_path_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI prop_path_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.PropertyPath", 29, n); }
static HRESULT WINAPI prop_path_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI prop_path_get_Path(void *This, HSTRING *v)
{
    struct xaml_property_path *impl = impl_from_IPropertyPath(This);
    if (!v) return E_POINTER;
    return WindowsDuplicateString(impl->path, v);
}

static const void *prop_path_vtbl[] = {
    (void *)prop_path_QI,
    (void *)prop_path_AddRef,
    (void *)prop_path_Release,
    (void *)prop_path_GetIids,
    (void *)prop_path_GetRuntimeClassName,
    (void *)prop_path_GetTrustLevel,
    (void *)prop_path_get_Path
};

struct prop_path_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } IPropertyPathFactory_iface;
};

#define impl_from_prop_path_factory(ptr) ((struct prop_path_factory_impl *)((char *)(ptr) - offsetof(struct prop_path_factory_impl, IActivationFactory_iface)))
#define impl_from_prop_path_fac_iface(ptr) ((struct prop_path_factory_impl *)((char *)(ptr) - offsetof(struct prop_path_factory_impl, IPropertyPathFactory_iface)))

static HRESULT WINAPI prop_path_fac_QI(IActivationFactory *iface, REFIID riid, void **ppv)
{
    struct prop_path_factory_impl *impl = impl_from_prop_path_factory(iface);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject_local) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppv = &impl->IActivationFactory_iface;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IPropertyPathFactory))
    {
        *ppv = &impl->IPropertyPathFactory_iface;
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI prop_path_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI prop_path_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI prop_path_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI prop_path_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.PropertyPath", 29, n); }
static HRESULT WINAPI prop_path_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI prop_path_fac_Activate(IActivationFactory *iface, IInspectable **instance)
{
    struct xaml_property_path *p = calloc(1, sizeof(*p));
    if (!p) return E_OUTOFMEMORY;
    p->IPropertyPath_iface.lpVtbl = prop_path_vtbl;
    p->ref = 1;
    *instance = (IInspectable *)&p->IPropertyPath_iface;
    return S_OK;
}

static const struct IActivationFactoryVtbl prop_path_activation_factory_vtbl = {
    prop_path_fac_QI,
    prop_path_fac_AddRef,
    prop_path_fac_Release,
    prop_path_fac_GetIids,
    prop_path_fac_GetRuntimeClassName,
    prop_path_fac_GetTrustLevel,
    prop_path_fac_Activate
};

static HRESULT WINAPI prop_path_fac_iface_QI(void *This, REFIID riid, void **ppv) { return prop_path_fac_QI(&impl_from_prop_path_fac_iface(This)->IActivationFactory_iface, riid, ppv); }
static ULONG WINAPI prop_path_fac_iface_AddRef(void *This) { return 2; }
static ULONG WINAPI prop_path_fac_iface_Release(void *This) { return 1; }
static HRESULT WINAPI prop_path_fac_CreateInstance(void *This, HSTRING path, void **instance)
{
    struct xaml_property_path *p;
    if (!instance) return E_POINTER;
    p = calloc(1, sizeof(*p));
    if (!p) return E_OUTOFMEMORY;
    p->IPropertyPath_iface.lpVtbl = prop_path_vtbl;
    p->ref = 1;
    if (path) WindowsDuplicateString(path, &p->path);
    *instance = (void *)&p->IPropertyPath_iface;
    TRACE("prop_path_fac_CreateInstance path %s -> %p\n", debugstr_hstring(path), *instance);
    return S_OK;
}

static const void *prop_path_factory_vtbl[] = {
    (void *)prop_path_fac_iface_QI,
    (void *)prop_path_fac_iface_AddRef,
    (void *)prop_path_fac_iface_Release,
    (void *)prop_path_fac_GetIids,
    (void *)prop_path_fac_GetRuntimeClassName,
    (void *)prop_path_fac_GetTrustLevel,
    (void *)prop_path_fac_CreateInstance
};

static struct prop_path_factory_impl prop_path_factory_obj = {
    { &prop_path_activation_factory_vtbl },
    { prop_path_factory_vtbl }
};
IActivationFactory *property_path_factory = &prop_path_factory_obj.IActivationFactory_iface;

/* -------------------------------------------------------------------------
 * PropertyChangedEventArgs Implementation & Factory
 * ------------------------------------------------------------------------- */
struct xaml_property_changed_args
{
    struct { const void *lpVtbl; } IPropertyChangedEventArgs_iface;
    struct { const void *lpVtbl; } IWeakReferenceSource_iface;
    LONG ref;
    void *outer;
    HSTRING property_name;
};

#define impl_from_IPropertyChangedEventArgs(ptr) ((struct xaml_property_changed_args *)((char *)(ptr) - offsetof(struct xaml_property_changed_args, IPropertyChangedEventArgs_iface)))
#define impl_from_pca_IWRS(ptr)                  ((struct xaml_property_changed_args *)((char *)(ptr) - offsetof(struct xaml_property_changed_args, IWeakReferenceSource_iface)))

static HRESULT WINAPI pca_QI(void *This, REFIID riid, void **ppv);
static ULONG WINAPI pca_AddRef(void *This);
static ULONG WINAPI pca_Release(void *This);

struct pca_weakref
{
    struct { const void *lpVtbl; } IWeakReference_iface;
    LONG ref;
    struct xaml_property_changed_args *target;
};

#define impl_from_pca_WeakRef(ptr) ((struct pca_weakref *)((char *)(ptr) - offsetof(struct pca_weakref, IWeakReference_iface)))

static HRESULT WINAPI pca_weakref_QI(void *This, REFIID riid, void **ppv)
{
    struct pca_weakref *impl = impl_from_pca_WeakRef(This);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) || IsEqualGUID(riid, &IID_IWeakReference))
    {
        *ppv = &impl->IWeakReference_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI pca_weakref_AddRef(void *This)
{
    struct pca_weakref *impl = impl_from_pca_WeakRef(This);
    return InterlockedIncrement(&impl->ref);
}
static ULONG WINAPI pca_weakref_Release(void *This)
{
    struct pca_weakref *impl = impl_from_pca_WeakRef(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref) free(impl);
    return ref;
}
static HRESULT WINAPI pca_weakref_Resolve(void *This, REFIID riid, void **ppv)
{
    struct pca_weakref *impl = impl_from_pca_WeakRef(This);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (!impl->target) return S_OK;
    return pca_QI(&impl->target->IPropertyChangedEventArgs_iface, riid, ppv);
}
static const void *pca_weakref_vtbl[4] = {
    pca_weakref_QI, pca_weakref_AddRef, pca_weakref_Release, pca_weakref_Resolve
};

static HRESULT pca_weakref_create(struct xaml_property_changed_args *target, void **out)
{
    struct pca_weakref *w = calloc(1, sizeof(*w));
    if (!w) return E_OUTOFMEMORY;
    w->IWeakReference_iface.lpVtbl = pca_weakref_vtbl;
    w->ref = 1;
    w->target = target;
    *out = &w->IWeakReference_iface;
    return S_OK;
}

static HRESULT WINAPI pca_wrs_QI(void *This, REFIID riid, void **ppv) { return pca_QI(&impl_from_pca_IWRS(This)->IPropertyChangedEventArgs_iface, riid, ppv); }
static ULONG WINAPI pca_wrs_AddRef(void *This) { return pca_AddRef(&impl_from_pca_IWRS(This)->IPropertyChangedEventArgs_iface); }
static ULONG WINAPI pca_wrs_Release(void *This) { return pca_Release(&impl_from_pca_IWRS(This)->IPropertyChangedEventArgs_iface); }
static HRESULT WINAPI pca_wrs_GetWeakReference(void *This, void **weakReference)
{
    struct xaml_property_changed_args *impl = impl_from_pca_IWRS(This);
    if (!weakReference) return E_POINTER;
    return pca_weakref_create(impl, weakReference);
}
static const void *pca_wrs_vtbl[4] = {
    pca_wrs_QI, pca_wrs_AddRef, pca_wrs_Release, pca_wrs_GetWeakReference
};

static HRESULT WINAPI pca_QI(void *This, REFIID riid, void **ppv)
{
    struct xaml_property_changed_args *impl = impl_from_IPropertyChangedEventArgs(This);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject_local) ||
        IsEqualGUID(riid, &IID_IPropertyChangedEventArgs))
    {
        *ppv = &impl->IPropertyChangedEventArgs_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IWeakReferenceSource))
    {
        *ppv = &impl->IWeakReferenceSource_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI pca_AddRef(void *This) { return InterlockedIncrement(&impl_from_IPropertyChangedEventArgs(This)->ref); }
static ULONG WINAPI pca_Release(void *This)
{
    struct xaml_property_changed_args *impl = impl_from_IPropertyChangedEventArgs(This);
    ULONG ref = InterlockedDecrement(&impl->ref);
    if (!ref)
    {
        if (impl->property_name) WindowsDeleteString(impl->property_name);
        free(impl);
    }
    return ref;
}
static HRESULT WINAPI pca_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI pca_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.Data.PropertyChangedEventArgs", 44, n); }
static HRESULT WINAPI pca_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI pca_get_PropertyName(void *This, HSTRING *v)
{
    struct xaml_property_changed_args *impl = impl_from_IPropertyChangedEventArgs(This);
    if (!v) return E_POINTER;
    return WindowsDuplicateString(impl->property_name, v);
}

static const void *pca_vtbl[] = {
    (void *)pca_QI,
    (void *)pca_AddRef,
    (void *)pca_Release,
    (void *)pca_GetIids,
    (void *)pca_GetRuntimeClassName,
    (void *)pca_GetTrustLevel,
    (void *)pca_get_PropertyName
};

struct pca_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } IPropertyChangedEventArgsFactory_iface;
};

#define impl_from_pca_factory(ptr) ((struct pca_factory_impl *)((char *)(ptr) - offsetof(struct pca_factory_impl, IActivationFactory_iface)))
#define impl_from_pca_fac_iface(ptr) ((struct pca_factory_impl *)((char *)(ptr) - offsetof(struct pca_factory_impl, IPropertyChangedEventArgsFactory_iface)))

static HRESULT WINAPI pca_fac_QI(IActivationFactory *iface, REFIID riid, void **ppv)
{
    struct pca_factory_impl *impl = impl_from_pca_factory(iface);
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject_local) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppv = &impl->IActivationFactory_iface;
        return S_OK;
    }
    if (IsEqualGUID(riid, &IID_IPropertyChangedEventArgsFactory))
    {
        *ppv = &impl->IPropertyChangedEventArgsFactory_iface;
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI pca_fac_AddRef(IActivationFactory *iface) { return 2; }
static ULONG WINAPI pca_fac_Release(IActivationFactory *iface) { return 1; }
static HRESULT WINAPI pca_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI pca_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.Data.PropertyChangedEventArgs", 44, n); }
static HRESULT WINAPI pca_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }

static HRESULT WINAPI pca_fac_Activate(IActivationFactory *iface, IInspectable **instance)
{
    struct xaml_property_changed_args *p = calloc(1, sizeof(*p));
    if (!p) return E_OUTOFMEMORY;
    p->IPropertyChangedEventArgs_iface.lpVtbl = pca_vtbl;
    p->IWeakReferenceSource_iface.lpVtbl = pca_wrs_vtbl;
    p->ref = 1;
    *instance = (IInspectable *)&p->IPropertyChangedEventArgs_iface;
    return S_OK;
}

static const struct IActivationFactoryVtbl pca_activation_factory_vtbl = {
    pca_fac_QI,
    pca_fac_AddRef,
    pca_fac_Release,
    pca_fac_GetIids,
    pca_fac_GetRuntimeClassName,
    pca_fac_GetTrustLevel,
    pca_fac_Activate
};

static HRESULT WINAPI pca_fac_iface_QI(void *This, REFIID riid, void **ppv) { return pca_fac_QI(&impl_from_pca_fac_iface(This)->IActivationFactory_iface, riid, ppv); }
static ULONG WINAPI pca_fac_iface_AddRef(void *This) { return 2; }
static ULONG WINAPI pca_fac_iface_Release(void *This) { return 1; }
static HRESULT WINAPI pca_fac_CreateInstance(void *This, HSTRING name, void *outer, void **inner, void **instance)
{
    struct xaml_property_changed_args *p;
    if (!instance) return E_POINTER;
    p = calloc(1, sizeof(*p));
    if (!p) return E_OUTOFMEMORY;
    p->IPropertyChangedEventArgs_iface.lpVtbl = pca_vtbl;
    p->IWeakReferenceSource_iface.lpVtbl = pca_wrs_vtbl;
    p->ref = 1;
    p->outer = outer;
    if (name) WindowsDuplicateString(name, &p->property_name);
    if (inner) *inner = (void *)&p->IPropertyChangedEventArgs_iface;
    *instance = (void *)&p->IPropertyChangedEventArgs_iface;
    TRACE("pca_fac_CreateInstance name %s -> %p\n", debugstr_hstring(name), *instance);
    return S_OK;
}

static const void *pca_factory_vtbl[] = {
    (void *)pca_fac_iface_QI,
    (void *)pca_fac_iface_AddRef,
    (void *)pca_fac_iface_Release,
    (void *)pca_fac_GetIids,
    (void *)pca_fac_GetRuntimeClassName,
    (void *)pca_fac_GetTrustLevel,
    (void *)pca_fac_CreateInstance
};

static struct pca_factory_impl pca_factory_obj = {
    { &pca_activation_factory_vtbl },
    { pca_factory_vtbl }
};
IActivationFactory *property_changed_event_args_factory = &pca_factory_obj.IActivationFactory_iface;

/* -------------------------------------------------------------------------
 * AutomationProperties Implementation
 * ------------------------------------------------------------------------- */
static const GUID IID_IAutomationPropertiesStatics  = {0xb618fd7b, 0x32d0, 0x4970, {0x9c, 0x42, 0x7c, 0x03, 0x9a, 0xc7, 0xbe, 0x78}};
static const GUID IID_IAutomationPropertiesStatics2 = {0x3976547f, 0x7089, 0x4801, {0x8f, 0x1d, 0xaa, 0xb7, 0x80, 0x90, 0xd1, 0xa0}};
static const GUID IID_IAutomationPropertiesStatics3 = {0x7b75d735, 0x5cb1, 0x42ad, {0x9b, 0x57, 0x5f, 0xab, 0xa8, 0xc1, 0x86, 0x7f}};
static const GUID IID_IAutomationPropertiesStatics4 = {0xf7d62655, 0x311a, 0x4b7c, {0xa1, 0x31, 0x52, 0x4e, 0x89, 0xcd, 0x3c, 0xf9}};
static const GUID IID_IAutomationPropertiesStatics5 = {0x0be35b26, 0xc8f9, 0x41a2, {0xb4, 0xdb, 0xe6, 0xa7, 0xa3, 0x2b, 0x0c, 0x34}};
static const GUID IID_IAutomationPropertiesStatics6 = {0xc61e030f, 0xeb49, 0x4e5d, {0xb0, 0x12, 0x4c, 0x1c, 0x96, 0xc3, 0x90, 0x1b}};

static struct xaml_element *elem_from_inspectable(void *obj)
{
    void *ui = NULL;
    if (!obj) return NULL;
    if (SUCCEEDED(IUnknown_QueryInterface((IUnknown *)obj, &IID_IUIElement, &ui)) && ui)
    {
        IUnknown_Release((IUnknown *)ui);
        return (struct xaml_element *)ui;
    }
    if (SUCCEEDED(IUnknown_QueryInterface((IUnknown *)obj, &IID_IDependencyObject_local, &ui)) && ui)
    {
        IUnknown_Release((IUnknown *)ui);
        return impl_from_IDependencyObject(ui);
    }
    return NULL;
}

static HRESULT WINAPI auto_s_GetName(void *This, void *element, HSTRING *value)
{
    struct xaml_element *elem = elem_from_inspectable(element);
    TRACE("auto_s_GetName This %p, element %p, elem %p\n", This, element, elem);
    if (!value) return E_POINTER;
    if (elem && elem->automation_name[0])
        return WindowsCreateString(elem->automation_name, wcslen(elem->automation_name), value);
    if (elem && elem->name[0])
        return WindowsCreateString(elem->name, wcslen(elem->name), value);
    return WindowsCreateString(L"", 0, value);
}

static HRESULT WINAPI auto_s_SetName(void *This, void *element, HSTRING value)
{
    struct xaml_element *elem = elem_from_inspectable(element);
    TRACE("auto_s_SetName This %p, element %p, elem %p, value %s\n", This, element, elem, debugstr_hstring(value));
    if (elem && value)
    {
        const WCHAR *str = WindowsGetStringRawBuffer(value, NULL);
        if (str) lstrcpynW(elem->automation_name, str, ARRAY_SIZE(elem->automation_name));
    }
    return S_OK;
}

static HRESULT WINAPI auto_s_GetAutomationId(void *This, void *element, HSTRING *value)
{
    struct xaml_element *elem = elem_from_inspectable(element);
    TRACE("auto_s_GetAutomationId This %p, element %p, elem %p\n", This, element, elem);
    if (!value) return E_POINTER;
    if (elem && elem->automation_id[0])
        return WindowsCreateString(elem->automation_id, wcslen(elem->automation_id), value);
    if (elem && elem->name[0])
        return WindowsCreateString(elem->name, wcslen(elem->name), value);
    return WindowsCreateString(L"", 0, value);
}

static HRESULT WINAPI auto_s_SetAutomationId(void *This, void *element, HSTRING value)
{
    struct xaml_element *elem = elem_from_inspectable(element);
    TRACE("auto_s_SetAutomationId This %p, element %p, elem %p, value %s\n", This, element, elem, debugstr_hstring(value));
    if (elem && value)
    {
        const WCHAR *str = WindowsGetStringRawBuffer(value, NULL);
        if (str) lstrcpynW(elem->automation_id, str, ARRAY_SIZE(elem->automation_id));
    }
    return S_OK;
}

static HRESULT WINAPI auto_dp_AcceleratorKey(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"AcceleratorKey"); return S_OK; }
static HRESULT WINAPI auto_dp_AccessKey(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"AccessKey"); return S_OK; }
static HRESULT WINAPI auto_dp_AutomationId(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"AutomationId"); return S_OK; }
static HRESULT WINAPI auto_dp_HelpText(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"HelpText"); return S_OK; }
static HRESULT WINAPI auto_dp_IsRequiredForForm(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"IsRequiredForForm"); return S_OK; }
static HRESULT WINAPI auto_dp_ItemStatus(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"ItemStatus"); return S_OK; }
static HRESULT WINAPI auto_dp_ItemType(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"ItemType"); return S_OK; }
static HRESULT WINAPI auto_dp_LabeledBy(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"LabeledBy"); return S_OK; }
static HRESULT WINAPI auto_dp_Name(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"Name"); return S_OK; }
static HRESULT WINAPI auto_dp_LiveSetting(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"LiveSetting"); return S_OK; }

static HRESULT WINAPI auto_dp_AccessibilityView(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"AccessibilityView"); return S_OK; }
static HRESULT WINAPI auto_dp_ControlledPeers(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"ControlledPeers"); return S_OK; }

static HRESULT WINAPI auto_dp_PositionInSet(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"PositionInSet"); return S_OK; }
static HRESULT WINAPI auto_dp_SizeOfSet(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"SizeOfSet"); return S_OK; }
static HRESULT WINAPI auto_dp_Level(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"Level"); return S_OK; }
static HRESULT WINAPI auto_dp_Annotations(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"Annotations"); return S_OK; }

static HRESULT WINAPI auto_dp_LandmarkType(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"LandmarkType"); return S_OK; }
static HRESULT WINAPI auto_dp_LocalizedLandmarkType(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"LocalizedLandmarkType"); return S_OK; }

static HRESULT WINAPI auto_dp_IsPeripheral(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"IsPeripheral"); return S_OK; }
static HRESULT WINAPI auto_dp_IsDataValidForForm(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"IsDataValidForForm"); return S_OK; }
static HRESULT WINAPI auto_dp_FullDescription(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"FullDescription"); return S_OK; }
static HRESULT WINAPI auto_dp_LocalizedControlType(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"LocalizedControlType"); return S_OK; }
static HRESULT WINAPI auto_dp_DescribedBy(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"DescribedBy"); return S_OK; }
static HRESULT WINAPI auto_dp_FlowsTo(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"FlowsTo"); return S_OK; }
static HRESULT WINAPI auto_dp_FlowsFrom(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"FlowsFrom"); return S_OK; }

static HRESULT WINAPI auto_dp_Culture(void *This, void **v) { if (!v) return E_POINTER; *v = create_dependency_property_named(L"Culture"); return S_OK; }

static HRESULT WINAPI auto_stub_hstring_empty(void *This, void *elem, HSTRING *val)
{
    if (!val) return E_POINTER;
    return WindowsCreateString(L"", 0, val);
}

static HRESULT WINAPI auto_stub_zero2(void *This, void *elem, void *val)
{
    if (val) *(ULONG_PTR *)val = 0;
    return S_OK;
}

static HRESULT WINAPI auto_stub_ok2(void *This, void *elem, ...)
{
    return S_OK;
}

/* -------------------------------------------------------------------------
 * AutomationProperties Factory & Unified Statics Implementation
 * ------------------------------------------------------------------------- */
struct auto_properties_factory_impl
{
    IActivationFactory IActivationFactory_iface;
    struct { const void *lpVtbl; } IAutomationPropertiesStatics_iface;
    struct { const void *lpVtbl; } IAutomationPropertiesStatics2_iface;
    struct { const void *lpVtbl; } IAutomationPropertiesStatics3_iface;
    struct { const void *lpVtbl; } IAutomationPropertiesStatics4_iface;
    struct { const void *lpVtbl; } IAutomationPropertiesStatics5_iface;
    struct { const void *lpVtbl; } IAutomationPropertiesStatics6_iface;
    struct { const void *lpVtbl; } IRedhawkInterop_iface;
    LONG ref;
};

#define impl_from_auto_fac(ptr)       ((struct auto_properties_factory_impl *)((char *)(ptr) - offsetof(struct auto_properties_factory_impl, IActivationFactory_iface)))
#define impl_from_auto_statics(ptr)   ((struct auto_properties_factory_impl *)((char *)(ptr) - offsetof(struct auto_properties_factory_impl, IAutomationPropertiesStatics_iface)))
#define impl_from_auto_statics2(ptr)  ((struct auto_properties_factory_impl *)((char *)(ptr) - offsetof(struct auto_properties_factory_impl, IAutomationPropertiesStatics2_iface)))
#define impl_from_auto_statics3(ptr)  ((struct auto_properties_factory_impl *)((char *)(ptr) - offsetof(struct auto_properties_factory_impl, IAutomationPropertiesStatics3_iface)))
#define impl_from_auto_statics4(ptr)  ((struct auto_properties_factory_impl *)((char *)(ptr) - offsetof(struct auto_properties_factory_impl, IAutomationPropertiesStatics4_iface)))
#define impl_from_auto_statics5(ptr)  ((struct auto_properties_factory_impl *)((char *)(ptr) - offsetof(struct auto_properties_factory_impl, IAutomationPropertiesStatics5_iface)))
#define impl_from_auto_statics6(ptr)  ((struct auto_properties_factory_impl *)((char *)(ptr) - offsetof(struct auto_properties_factory_impl, IAutomationPropertiesStatics6_iface)))
#define impl_from_auto_redfac(ptr)    ((struct auto_properties_factory_impl *)((char *)(ptr) - offsetof(struct auto_properties_factory_impl, IRedhawkInterop_iface)))

static HRESULT auto_fac_QI_internal(struct auto_properties_factory_impl *impl, REFIID riid, void **ppvObject)
{
    if (!ppvObject) return E_POINTER;
    *ppvObject = NULL;

    TRACE("auto_fac %p, riid %s\n", impl, debugstr_guid(riid));
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IAgileObject) ||
        IsEqualGUID(riid, &IID_IActivationFactory))
    {
        *ppvObject = &impl->IActivationFactory_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IAutomationPropertiesStatics))
    {
        *ppvObject = &impl->IAutomationPropertiesStatics_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IAutomationPropertiesStatics2))
    {
        *ppvObject = &impl->IAutomationPropertiesStatics2_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IAutomationPropertiesStatics3))
    {
        *ppvObject = &impl->IAutomationPropertiesStatics3_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IAutomationPropertiesStatics4))
    {
        *ppvObject = &impl->IAutomationPropertiesStatics4_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IAutomationPropertiesStatics5))
    {
        *ppvObject = &impl->IAutomationPropertiesStatics5_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_IAutomationPropertiesStatics6))
    {
        *ppvObject = &impl->IAutomationPropertiesStatics6_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    if (IsEqualGUID(riid, &IID_ComWrappers_internal) ||
        IsEqualGUID(riid, &IID_ComWrappers_internal2) ||
        IsEqualGUID(riid, &IID_ITrackerOwner))
    {
        *ppvObject = &impl->IRedhawkInterop_iface;
        InterlockedIncrement(&impl->ref);
        return S_OK;
    }

    TRACE("auto_fac %p: unhandled QI %s\n", impl, debugstr_guid(riid));
    return E_NOINTERFACE;
}

static HRESULT WINAPI auto_fac_QueryInterface(IActivationFactory *iface, REFIID riid, void **ppvObject)
{
    return auto_fac_QI_internal(impl_from_auto_fac(iface), riid, ppvObject);
}
static ULONG WINAPI auto_fac_AddRef(IActivationFactory *iface)
{
    return InterlockedIncrement(&impl_from_auto_fac(iface)->ref);
}
static ULONG WINAPI auto_fac_Release(IActivationFactory *iface)
{
    return InterlockedDecrement(&impl_from_auto_fac(iface)->ref);
}
static HRESULT WINAPI auto_fac_GetIids(IActivationFactory *iface, ULONG *c, IID **i)
{
    if (c) *c = 0;
    return S_OK;
}
static HRESULT WINAPI auto_fac_GetRuntimeClassName(IActivationFactory *iface, HSTRING *n)
{
    return WindowsCreateString(L"Windows.UI.Xaml.Automation.AutomationProperties", 47, n);
}
static HRESULT WINAPI auto_fac_GetTrustLevel(IActivationFactory *iface, TrustLevel *t)
{
    if (t) *t = BaseTrust;
    return S_OK;
}
static HRESULT WINAPI auto_fac_ActivateInstance(IActivationFactory *iface, IInspectable **instance)
{
    if (instance) *instance = NULL;
    return E_NOTIMPL;
}

static const struct IActivationFactoryVtbl auto_activation_factory_vtbl = {
    auto_fac_QueryInterface,
    auto_fac_AddRef,
    auto_fac_Release,
    auto_fac_GetIids,
    auto_fac_GetRuntimeClassName,
    auto_fac_GetTrustLevel,
    auto_fac_ActivateInstance
};

static HRESULT WINAPI auto_s1_QI(void *This, REFIID riid, void **ppv) { return auto_fac_QI_internal(impl_from_auto_statics(This), riid, ppv); }
static ULONG WINAPI auto_s1_AddRef(void *This) { return InterlockedIncrement(&impl_from_auto_statics(This)->ref); }
static ULONG WINAPI auto_s1_Release(void *This) { return InterlockedDecrement(&impl_from_auto_statics(This)->ref); }

static HRESULT WINAPI auto_s2_QI(void *This, REFIID riid, void **ppv) { return auto_fac_QI_internal(impl_from_auto_statics2(This), riid, ppv); }
static ULONG WINAPI auto_s2_AddRef(void *This) { return InterlockedIncrement(&impl_from_auto_statics2(This)->ref); }
static ULONG WINAPI auto_s2_Release(void *This) { return InterlockedDecrement(&impl_from_auto_statics2(This)->ref); }

static HRESULT WINAPI auto_s3_QI(void *This, REFIID riid, void **ppv) { return auto_fac_QI_internal(impl_from_auto_statics3(This), riid, ppv); }
static ULONG WINAPI auto_s3_AddRef(void *This) { return InterlockedIncrement(&impl_from_auto_statics3(This)->ref); }
static ULONG WINAPI auto_s3_Release(void *This) { return InterlockedDecrement(&impl_from_auto_statics3(This)->ref); }

static HRESULT WINAPI auto_s4_QI(void *This, REFIID riid, void **ppv) { return auto_fac_QI_internal(impl_from_auto_statics4(This), riid, ppv); }
static ULONG WINAPI auto_s4_AddRef(void *This) { return InterlockedIncrement(&impl_from_auto_statics4(This)->ref); }
static ULONG WINAPI auto_s4_Release(void *This) { return InterlockedDecrement(&impl_from_auto_statics4(This)->ref); }

static HRESULT WINAPI auto_s5_QI(void *This, REFIID riid, void **ppv) { return auto_fac_QI_internal(impl_from_auto_statics5(This), riid, ppv); }
static ULONG WINAPI auto_s5_AddRef(void *This) { return InterlockedIncrement(&impl_from_auto_statics5(This)->ref); }
static ULONG WINAPI auto_s5_Release(void *This) { return InterlockedDecrement(&impl_from_auto_statics5(This)->ref); }

static HRESULT WINAPI auto_s6_QI(void *This, REFIID riid, void **ppv) { return auto_fac_QI_internal(impl_from_auto_statics6(This), riid, ppv); }
static ULONG WINAPI auto_s6_AddRef(void *This) { return InterlockedIncrement(&impl_from_auto_statics6(This)->ref); }
static ULONG WINAPI auto_s6_Release(void *This) { return InterlockedDecrement(&impl_from_auto_statics6(This)->ref); }

static const void *auto_statics_vtbl[36] = {
    (void *)auto_s1_QI,
    (void *)auto_s1_AddRef,
    (void *)auto_s1_Release,
    (void *)auto_fac_GetIids,
    (void *)auto_fac_GetRuntimeClassName,
    (void *)auto_fac_GetTrustLevel,
    (void *)auto_dp_AcceleratorKey,      /* 6: get_AcceleratorKeyProperty */
    (void *)auto_stub_hstring_empty,     /* 7: GetAcceleratorKey */
    (void *)auto_stub_ok2,               /* 8: SetAcceleratorKey */
    (void *)auto_dp_AccessKey,           /* 9: get_AccessKeyProperty */
    (void *)auto_stub_hstring_empty,     /* 10: GetAccessKey */
    (void *)auto_stub_ok2,               /* 11: SetAccessKey */
    (void *)auto_dp_AutomationId,        /* 12: get_AutomationIdProperty */
    (void *)auto_s_GetAutomationId,      /* 13: GetAutomationId */
    (void *)auto_s_SetAutomationId,      /* 14: SetAutomationId */
    (void *)auto_dp_HelpText,            /* 15: get_HelpTextProperty */
    (void *)auto_stub_hstring_empty,     /* 16: GetHelpText */
    (void *)auto_stub_ok2,               /* 17: SetHelpText */
    (void *)auto_dp_IsRequiredForForm,   /* 18: get_IsRequiredForFormProperty */
    (void *)auto_stub_zero2,             /* 19: GetIsRequiredForForm */
    (void *)auto_stub_ok2,               /* 20: SetIsRequiredForForm */
    (void *)auto_dp_ItemStatus,          /* 21: get_ItemStatusProperty */
    (void *)auto_stub_hstring_empty,     /* 22: GetItemStatus */
    (void *)auto_stub_ok2,               /* 23: SetItemStatus */
    (void *)auto_dp_ItemType,            /* 24: get_ItemTypeProperty */
    (void *)auto_stub_hstring_empty,     /* 25: GetItemType */
    (void *)auto_stub_ok2,               /* 26: SetItemType */
    (void *)auto_dp_LabeledBy,           /* 27: get_LabeledByProperty */
    (void *)auto_stub_zero2,             /* 28: GetLabeledBy */
    (void *)auto_stub_ok2,               /* 29: SetLabeledBy */
    (void *)auto_dp_Name,                /* 30: get_NameProperty */
    (void *)auto_s_GetName,              /* 31: GetName */
    (void *)auto_s_SetName,              /* 32: SetName */
    (void *)auto_dp_LiveSetting,         /* 33: get_LiveSettingProperty */
    (void *)auto_stub_zero2,             /* 34: GetLiveSetting */
    (void *)auto_stub_ok2                /* 35: SetLiveSetting */
};

static const void *auto_statics2_vtbl[11] = {
    (void *)auto_s2_QI,
    (void *)auto_s2_AddRef,
    (void *)auto_s2_Release,
    (void *)auto_fac_GetIids,
    (void *)auto_fac_GetRuntimeClassName,
    (void *)auto_fac_GetTrustLevel,
    (void *)auto_dp_AccessibilityView,   /* 6: get_AccessibilityViewProperty */
    (void *)auto_stub_zero2,             /* 7: GetAccessibilityView */
    (void *)auto_stub_ok2,               /* 8: SetAccessibilityView */
    (void *)auto_dp_ControlledPeers,     /* 9: get_ControlledPeersProperty */
    (void *)auto_stub_zero2              /* 10: GetControlledPeers */
};

static const void *auto_statics3_vtbl[17] = {
    (void *)auto_s3_QI,
    (void *)auto_s3_AddRef,
    (void *)auto_s3_Release,
    (void *)auto_fac_GetIids,
    (void *)auto_fac_GetRuntimeClassName,
    (void *)auto_fac_GetTrustLevel,
    (void *)auto_dp_PositionInSet,       /* 6 */
    (void *)auto_stub_zero2,             /* 7 */
    (void *)auto_stub_ok2,               /* 8 */
    (void *)auto_dp_SizeOfSet,           /* 9 */
    (void *)auto_stub_zero2,             /* 10 */
    (void *)auto_stub_ok2,               /* 11 */
    (void *)auto_dp_Level,               /* 12 */
    (void *)auto_stub_zero2,             /* 13 */
    (void *)auto_stub_ok2,               /* 14 */
    (void *)auto_dp_Annotations,         /* 15 */
    (void *)auto_stub_zero2              /* 16 */
};

static const void *auto_statics4_vtbl[12] = {
    (void *)auto_s4_QI,
    (void *)auto_s4_AddRef,
    (void *)auto_s4_Release,
    (void *)auto_fac_GetIids,
    (void *)auto_fac_GetRuntimeClassName,
    (void *)auto_fac_GetTrustLevel,
    (void *)auto_dp_LandmarkType,        /* 6 */
    (void *)auto_stub_zero2,             /* 7 */
    (void *)auto_stub_ok2,               /* 8 */
    (void *)auto_dp_LocalizedLandmarkType, /* 9 */
    (void *)auto_stub_hstring_empty,     /* 10 */
    (void *)auto_stub_ok2                /* 11 */
};

static const void *auto_statics5_vtbl[24] = {
    (void *)auto_s5_QI,
    (void *)auto_s5_AddRef,
    (void *)auto_s5_Release,
    (void *)auto_fac_GetIids,
    (void *)auto_fac_GetRuntimeClassName,
    (void *)auto_fac_GetTrustLevel,
    (void *)auto_dp_IsPeripheral,        /* 6 */
    (void *)auto_stub_zero2,             /* 7 */
    (void *)auto_stub_ok2,               /* 8 */
    (void *)auto_dp_IsDataValidForForm,  /* 9 */
    (void *)auto_stub_zero2,             /* 10 */
    (void *)auto_stub_ok2,               /* 11 */
    (void *)auto_dp_FullDescription,     /* 12 */
    (void *)auto_stub_hstring_empty,     /* 13 */
    (void *)auto_stub_ok2,               /* 14 */
    (void *)auto_dp_LocalizedControlType, /* 15 */
    (void *)auto_stub_hstring_empty,     /* 16 */
    (void *)auto_stub_ok2,               /* 17 */
    (void *)auto_dp_DescribedBy,         /* 18 */
    (void *)auto_stub_zero2,             /* 19 */
    (void *)auto_dp_FlowsTo,             /* 20 */
    (void *)auto_stub_zero2,             /* 21 */
    (void *)auto_dp_FlowsFrom,           /* 22 */
    (void *)auto_stub_zero2              /* 23 */
};

static const void *auto_statics6_vtbl[9] = {
    (void *)auto_s6_QI,
    (void *)auto_s6_AddRef,
    (void *)auto_s6_Release,
    (void *)auto_fac_GetIids,
    (void *)auto_fac_GetRuntimeClassName,
    (void *)auto_fac_GetTrustLevel,
    (void *)auto_dp_Culture,             /* 6 */
    (void *)auto_stub_zero2,             /* 7 */
    (void *)auto_stub_ok2                /* 8 */
};

static HRESULT WINAPI redhawk_auto_fac_QI(void *This, REFIID riid, void **ppvObject)
{
    return auto_fac_QI_internal(impl_from_auto_redfac(This), riid, ppvObject);
}
static ULONG WINAPI redhawk_auto_fac_AddRef(void *This) { return 2; }
static ULONG WINAPI redhawk_auto_fac_Release(void *This) { return 1; }

static const void *redhawk_auto_factory_vtbl[16] = {
    (void *)redhawk_auto_fac_QI,
    (void *)redhawk_auto_fac_AddRef,
    (void *)redhawk_auto_fac_Release,
    (void *)elem_stub_ret_ok, (void *)elem_stub_ret_ok, (void *)elem_stub_ret_ok,
    (void *)elem_stub_ret_ok, (void *)elem_stub_ret_ok, (void *)elem_stub_ret_ok,
    (void *)elem_stub_ret_ok, (void *)elem_stub_ret_ok, (void *)elem_stub_ret_ok,
    (void *)elem_stub_ret_ok, (void *)elem_stub_ret_ok, (void *)elem_stub_ret_ok,
    (void *)elem_stub_ret_ok
};

static struct auto_properties_factory_impl default_auto_factory = {
    { &auto_activation_factory_vtbl },
    { auto_statics_vtbl },
    { auto_statics2_vtbl },
    { auto_statics3_vtbl },
    { auto_statics4_vtbl },
    { auto_statics5_vtbl },
    { auto_statics6_vtbl },
    { redhawk_auto_factory_vtbl },
    1
};

IActivationFactory *automation_properties_factory = &default_auto_factory.IActivationFactory_iface;

/* -------------------------------------------------------------------------
 * Visual Tree Layout: Measure & Arrange
 * ------------------------------------------------------------------------- */
static void xaml_arrange(struct xaml_element *elem, RECT rect)
{
    UINT32 i;
    if (!elem || elem->visibility != 0) return;
    elem->bounds = rect;

    if (elem->child_content)
    {
        xaml_arrange(elem->child_content, rect);
    }

    if (elem->type == CONTROL_TYPE_GRID && elem->children_count > 0)
    {
        INT32 max_row = 0, max_col = 0;
        int num_rows, num_cols, total_w, total_h;
        int row_pos[17] = {0};
        int col_pos[17] = {0};
        double fixed_h = 0, total_stars_h = 0;
        double fixed_w = 0, total_stars_w = 0;
        double avail_h, avail_w;

        for (i = 0; i < elem->children_count; i++)
        {
            if (elem->children[i]->grid_row > max_row) max_row = elem->children[i]->grid_row;
            if (elem->children[i]->grid_column > max_col) max_col = elem->children[i]->grid_column;
        }
        num_rows = (elem->row_count > (UINT32)(max_row + 1)) ? elem->row_count : (max_row + 1);
        num_cols = (elem->col_count > (UINT32)(max_col + 1)) ? elem->col_count : (max_col + 1);
        if (num_rows > 16) num_rows = 16;
        if (num_cols > 16) num_cols = 16;

        total_w = rect.right - rect.left;
        total_h = rect.bottom - rect.top;

        /* Calculate rows */
        for (i = 0; i < (UINT32)num_rows; i++)
        {
            if (elem->row_unit_types[i] == 1) /* Pixel */
                fixed_h += elem->row_heights[i];
            else if (elem->row_unit_types[i] == 2) /* Star */
                total_stars_h += (elem->row_heights[i] > 0 ? elem->row_heights[i] : 1.0);
            else /* Auto or default */
                total_stars_h += 1.0;
        }
        avail_h = (double)total_h - fixed_h;
        if (avail_h < 0) avail_h = 0;

        row_pos[0] = rect.top;
        for (i = 0; i < (UINT32)num_rows; i++)
        {
            double h;
            if (elem->row_unit_types[i] == 1)
                h = elem->row_heights[i];
            else if (total_stars_h > 0)
            {
                double weight = (elem->row_unit_types[i] == 2 && elem->row_heights[i] > 0) ? elem->row_heights[i] : 1.0;
                h = (avail_h * weight) / total_stars_h;
            }
            else
                h = (double)total_h / num_rows;
            row_pos[i + 1] = row_pos[i] + (int)h;
        }

        /* Calculate columns */
        for (i = 0; i < (UINT32)num_cols; i++)
        {
            if (elem->col_unit_types[i] == 1)
                fixed_w += elem->col_widths[i];
            else if (elem->col_unit_types[i] == 2)
                total_stars_w += (elem->col_widths[i] > 0 ? elem->col_widths[i] : 1.0);
            else
                total_stars_w += 1.0;
        }
        avail_w = (double)total_w - fixed_w;
        if (avail_w < 0) avail_w = 0;

        col_pos[0] = rect.left;
        for (i = 0; i < (UINT32)num_cols; i++)
        {
            double w;
            if (elem->col_unit_types[i] == 1)
                w = elem->col_widths[i];
            else if (total_stars_w > 0)
            {
                double weight = (elem->col_unit_types[i] == 2 && elem->col_widths[i] > 0) ? elem->col_widths[i] : 1.0;
                w = (avail_w * weight) / total_stars_w;
            }
            else
                w = (double)total_w / num_cols;
            col_pos[i + 1] = col_pos[i] + (int)w;
        }

        for (i = 0; i < elem->children_count; i++)
        {
            struct xaml_element *child = elem->children[i];
            RECT child_rect;
            int r = child->grid_row < num_rows ? child->grid_row : 0;
            int c = child->grid_column < num_cols ? child->grid_column : 0;
            int r_span = child->grid_rowspan > 0 ? child->grid_rowspan : 1;
            int c_span = child->grid_colspan > 0 ? child->grid_colspan : 1;
            if (r + r_span > num_rows) r_span = num_rows - r;
            if (c + c_span > num_cols) c_span = num_cols - c;

            child_rect.left   = col_pos[c];
            child_rect.right  = col_pos[c + c_span];
            child_rect.top    = row_pos[r];
            child_rect.bottom = row_pos[r + r_span];

            if (child->type == CONTROL_TYPE_BUTTON)
            {
                child_rect.left   += 2;
                child_rect.right  -= 2;
                child_rect.top    += 2;
                child_rect.bottom -= 2;
            }
            xaml_arrange(child, child_rect);
        }
    }
    else if (elem->children_count > 0)
    {
        for (i = 0; i < elem->children_count; i++)
        {
            xaml_arrange(elem->children[i], rect);
        }
    }
}

/* -------------------------------------------------------------------------
 * Double-Buffered GDI Surface Rasterizer
 * ------------------------------------------------------------------------- */
static void xaml_render_element(HDC hdc, struct xaml_element *elem)
{
    int w, h;
    UINT32 i;
    if (!elem || elem->visibility != 0) return;

    w = elem->bounds.right - elem->bounds.left;
    h = elem->bounds.bottom - elem->bounds.top;
    if (w <= 0 || h <= 0) return;

    if (elem->type == CONTROL_TYPE_BUTTON)
    {
        COLORREF bg = elem->is_pressed ? RGB(34, 34, 34) : (elem->is_hovered ? RGB(56, 56, 56) : elem->background_color);
        HBRUSH br, old_br;
        HPEN pen, old_pen;

        if (!wcscmp(elem->text, L"=") || !wcscmp(elem->name, L"EqualButton"))
        {
            bg = elem->is_pressed ? RGB(0, 100, 180) : RGB(0, 120, 215);
        }
        br = CreateSolidBrush(bg);
        pen = CreatePen(PS_SOLID, 1, elem->border_color);
        old_br = SelectObject(hdc, br);
        old_pen = SelectObject(hdc, pen);

        RoundRect(hdc, elem->bounds.left, elem->bounds.top, elem->bounds.right, elem->bounds.bottom, 6, 6);

        SelectObject(hdc, old_br);
        SelectObject(hdc, old_pen);
        DeleteObject(br);
        DeleteObject(pen);

        if (elem->text[0])
        {
            if (!wcscmp(elem->name, L"BackSpaceButton") || !wcscmp(elem->text, L"\x232B") || !wcscmp(elem->text, L"⌫"))
            {
                int cx = (elem->bounds.left + elem->bounds.right) / 2;
                int cy = (elem->bounds.top + elem->bounds.bottom) / 2;
                HPEN bpen = CreatePen(PS_SOLID, 2, elem->foreground_color);
                HPEN bold_pen = SelectObject(hdc, bpen);
                MoveToEx(hdc, cx - 7, cy, NULL); LineTo(hdc, cx + 7, cy);
                MoveToEx(hdc, cx - 7, cy, NULL); LineTo(hdc, cx - 2, cy - 5);
                MoveToEx(hdc, cx - 7, cy, NULL); LineTo(hdc, cx - 2, cy + 5);
                SelectObject(hdc, bold_pen);
                DeleteObject(bpen);
            }
            else
            {
                int pt_size = (elem->font_size > 0) ? (int)elem->font_size : 14;
                int font_h = -MulDiv(pt_size, GetDeviceCaps(hdc, LOGPIXELSY), 72);
                HFONT font = CreateFontW(font_h, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                         DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                         CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
                HFONT old_font = SelectObject(hdc, font);
                RECT text_rect = elem->bounds;

                SetBkMode(hdc, TRANSPARENT);
                SetTextColor(hdc, elem->foreground_color);
                DrawTextW(hdc, elem->text, -1, &text_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

                SelectObject(hdc, old_font);
                DeleteObject(font);
            }
        }
    }
    else if (elem->type == CONTROL_TYPE_TEXTBLOCK)
    {
        if (!wcscmp(elem->name, L"Header"))
            return;

        if (elem->text[0])
        {
            int pt_size = (elem->font_size > 0) ? (int)elem->font_size : 16;
            int weight, font_h;
            HFONT font, old_font;
            RECT text_rect;
            UINT fmt;

            if (!wcscmp(elem->name, L"Results"))
            {
                int len = wcslen(elem->text);
                if (len > 14) pt_size = 20;
                else if (len > 11) pt_size = 26;
                else if (len > 8) pt_size = 34;
                else pt_size = 46;
            }
            weight = (!wcscmp(elem->name, L"Results") || elem->font_size >= 30) ? FW_BOLD : FW_NORMAL;
            font_h = -MulDiv(pt_size, GetDeviceCaps(hdc, LOGPIXELSY), 72);
            font = CreateFontW(font_h, 0, 0, 0, weight, FALSE, FALSE, FALSE,
                               DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                               CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
            old_font = SelectObject(hdc, font);
            text_rect = elem->bounds;
            fmt = DT_VCENTER | DT_SINGLELINE;

            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, elem->foreground_color);

            text_rect.right -= 12;
            text_rect.left += 12;
            if (!wcscmp(elem->name, L"Results") || !wcscmp(elem->name, L"ExpressionText"))
                fmt |= DT_RIGHT;
            else
                fmt |= DT_LEFT;

            DrawTextW(hdc, elem->text, -1, &text_rect, fmt);

            SelectObject(hdc, old_font);
            DeleteObject(font);
        }
    }
    else if (elem->type == CONTROL_TYPE_BORDER)
    {
        if (elem->border_thickness > 0)
        {
            HPEN pen = CreatePen(PS_SOLID, elem->border_thickness, elem->border_color);
            HPEN old_pen = SelectObject(hdc, pen);
            HBRUSH old_br = SelectObject(hdc, GetStockObject(NULL_BRUSH));
            Rectangle(hdc, elem->bounds.left, elem->bounds.top, elem->bounds.right, elem->bounds.bottom);
            SelectObject(hdc, old_br);
            SelectObject(hdc, old_pen);
            DeleteObject(pen);
        }
    }

    if (elem->child_content)
    {
        xaml_render_element(hdc, elem->child_content);
    }

    for (i = 0; i < elem->children_count; i++)
    {
        xaml_render_element(hdc, elem->children[i]);
    }
}

/* -------------------------------------------------------------------------
 * Microsoft Calculator Navigation, Modes, Flyouts & Calculation Engine
 * ------------------------------------------------------------------------- */

enum calc_mode {
    CALC_MODE_STANDARD = 0,
    CALC_MODE_SCIENTIFIC = 1,
    CALC_MODE_PROGRAMMER = 2,
    CALC_MODE_DATE = 3,
    CALC_MODE_CURRENCY = 4,
    CALC_MODE_VOLUME = 5,
    CALC_MODE_LENGTH = 6,
    CALC_MODE_WEIGHT = 7,
    CALC_MODE_TEMPERATURE = 8,
    CALC_MODE_ENERGY = 9,
    CALC_MODE_AREA = 10,
    CALC_MODE_SPEED = 11,
    CALC_MODE_TIME = 12,
    CALC_MODE_POWER = 13,
    CALC_MODE_DATA = 14,
    CALC_MODE_PRESSURE = 15,
    CALC_MODE_ANGLE = 16,
    CALC_MODE_SETTINGS = 17,
    CALC_MODE_COUNT
};

struct nav_item_desc {
    const WCHAR *name;
    const WCHAR *glyph;
    enum calc_mode mode;
    boolean is_header;
};

static const struct nav_item_desc nav_items[] = {
    { L"Calculator", NULL, 0, TRUE },
    { L"Standard", L"Standard", CALC_MODE_STANDARD, FALSE },
    { L"Scientific", L"Scientific", CALC_MODE_SCIENTIFIC, FALSE },
    { L"Programmer", L"Programmer", CALC_MODE_PROGRAMMER, FALSE },
    { L"Date Calculation", L"Date", CALC_MODE_DATE, FALSE },
    { L"Converter", NULL, 0, TRUE },
    { L"Currency", L"$", CALC_MODE_CURRENCY, FALSE },
    { L"Volume", L"Vol", CALC_MODE_VOLUME, FALSE },
    { L"Length", L"Len", CALC_MODE_LENGTH, FALSE },
    { L"Weight and Mass", L"Wt", CALC_MODE_WEIGHT, FALSE },
    { L"Temperature", L"Temp", CALC_MODE_TEMPERATURE, FALSE },
    { L"Energy", L"NRG", CALC_MODE_ENERGY, FALSE },
    { L"Area", L"Area", CALC_MODE_AREA, FALSE },
    { L"Speed", L"Spd", CALC_MODE_SPEED, FALSE },
    { L"Time", L"Time", CALC_MODE_TIME, FALSE },
    { L"Power", L"Pwr", CALC_MODE_POWER, FALSE },
    { L"Data", L"Data", CALC_MODE_DATA, FALSE },
    { L"Pressure", L"Pres", CALC_MODE_PRESSURE, FALSE },
    { L"Angle", L"Ang", CALC_MODE_ANGLE, FALSE },
    { L"Settings", L"Set", CALC_MODE_SETTINGS, FALSE }
};
#define NAV_ITEM_COUNT (sizeof(nav_items) / sizeof(nav_items[0]))

static enum calc_mode current_calc_mode = CALC_MODE_STANDARD;
static boolean is_nav_pane_open = FALSE;
static boolean is_history_open = FALSE;

/* Memory storage */
static double calc_memory = 0.0;
static boolean calc_has_memory = FALSE;

/* History list */
#define MAX_CALC_HISTORY 16
static WCHAR calc_history_expr[MAX_CALC_HISTORY][64];
static WCHAR calc_history_res[MAX_CALC_HISTORY][32];
static int calc_history_count = 0;

/* Programmer mode state */
static UINT64 prog_val = 0;
static int prog_radix = 10; /* 16=HEX, 10=DEC, 8=OCT, 2=BIN */

/* Converter state */
static double conv_val1 = 1.0;
static double conv_val2 = 3.28084;
static int conv_unit1 = 0;
static int conv_unit2 = 1;

static double calc_accumulator = 0;
static WCHAR calc_pending_op = 0;
static boolean calc_has_decimal = FALSE;
static boolean calc_new_number = TRUE;

/* Forward declarations */
static void xaml_do_calc_action(const WCHAR *btn_name);
static void xaml_fire_click(struct xaml_element *btn);
static struct xaml_element *xaml_find_button_by_name(struct xaml_element *elem, const WCHAR *name);
static void xaml_handle_keydown(WPARAM key);

static void draw_rounded_button(HDC hdc, RECT rect, COLORREF bg, COLORREF border, const WCHAR *text, int font_size, COLORREF text_col, boolean pressed)
{
    HBRUSH br, old_br;
    HPEN pen, old_pen;
    HFONT font, old_font;
    RECT text_r = rect;

    if (pressed) bg = RGB(50, 50, 50);

    br = CreateSolidBrush(bg);
    pen = CreatePen(PS_SOLID, 1, border);
    old_br = SelectObject(hdc, br);
    old_pen = SelectObject(hdc, pen);

    RoundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, 6, 6);

    SelectObject(hdc, old_br);
    SelectObject(hdc, old_pen);
    DeleteObject(br);
    DeleteObject(pen);

    if (text && text[0])
    {
        int font_h = -MulDiv(font_size, GetDeviceCaps(hdc, LOGPIXELSY), 72);
        font = CreateFontW(font_h, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                           DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                           CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
        old_font = SelectObject(hdc, font);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, text_col);
        DrawTextW(hdc, text, -1, &text_r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdc, old_font);
        DeleteObject(font);
    }
}

static void xaml_render_header_bar(HDC hdc, int width, int height)
{
    RECT r;
    HPEN pen, old_pen;
    HFONT font, old_font;
    const WCHAR *title = (current_calc_mode < CALC_MODE_COUNT) ? nav_items[current_calc_mode == CALC_MODE_STANDARD ? 1 : (current_calc_mode == CALC_MODE_SCIENTIFIC ? 2 : (current_calc_mode == CALC_MODE_PROGRAMMER ? 3 : (current_calc_mode == CALC_MODE_DATE ? 4 : (current_calc_mode == CALC_MODE_SETTINGS ? 19 : 5 + current_calc_mode - 4))))].name : L"Calculator";

    /* Hamburger button at (4, 4) to (40, 36) */
    pen = CreatePen(PS_SOLID, 2, RGB(240, 240, 240));
    old_pen = SelectObject(hdc, pen);
    MoveToEx(hdc, 12, 14, NULL); LineTo(hdc, 30, 14);
    MoveToEx(hdc, 12, 20, NULL); LineTo(hdc, 30, 20);
    MoveToEx(hdc, 12, 26, NULL); LineTo(hdc, 30, 26);
    SelectObject(hdc, old_pen);
    DeleteObject(pen);

    /* Header Title at (46, 6) */
    r.left = 46; r.top = 6; r.right = width - 80; r.bottom = 38;
    font = CreateFontW(-MulDiv(18, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    old_font = SelectObject(hdc, font);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));
    DrawTextW(hdc, title, -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, old_font);
    DeleteObject(font);

    /* History Button at (width - 76, 4) */
    r.left = width - 76; r.top = 4; r.right = width - 42; r.bottom = 36;
    draw_rounded_button(hdc, r, RGB(32, 32, 32), RGB(50, 50, 50), L"", 14, RGB(220, 220, 220), is_history_open);
    {
        int cx = (r.left + r.right) / 2;
        int cy = (r.top + r.bottom) / 2;
        HPEN hpen = CreatePen(PS_SOLID, 2, is_history_open ? RGB(0, 120, 215) : RGB(220, 220, 220));
        HPEN hold_pen = SelectObject(hdc, hpen);
        HBRUSH hold_br = SelectObject(hdc, GetStockObject(NULL_BRUSH));
        Ellipse(hdc, cx - 7, cy - 7, cx + 8, cy + 8);
        MoveToEx(hdc, cx, cy, NULL); LineTo(hdc, cx, cy - 4);
        MoveToEx(hdc, cx, cy, NULL); LineTo(hdc, cx + 3, cy);
        SelectObject(hdc, hold_br);
        SelectObject(hdc, hold_pen);
        DeleteObject(hpen);
    }

    /* Always on top button at (width - 38, 4) */
    r.left = width - 38; r.top = 4; r.right = width - 6; r.bottom = 36;
    draw_rounded_button(hdc, r, RGB(32, 32, 32), RGB(50, 50, 50), L"", 12, RGB(220, 220, 220), FALSE);
    {
        int cx = (r.left + r.right) / 2;
        int cy = (r.top + r.bottom) / 2;
        HPEN hpen = CreatePen(PS_SOLID, 1, RGB(220, 220, 220));
        HPEN hold_pen = SelectObject(hdc, hpen);
        HBRUSH hold_br = SelectObject(hdc, GetStockObject(NULL_BRUSH));
        Rectangle(hdc, cx - 6, cy - 6, cx + 1, cy + 1);
        Rectangle(hdc, cx - 2, cy - 2, cx + 6, cy + 6);
        SelectObject(hdc, hold_br);
        SelectObject(hdc, hold_pen);
        DeleteObject(hpen);
    }
}

static void xaml_render_memory_bar(HDC hdc, int width)
{
    RECT r;
    int col_w = (width - 16) / 6;
    int y = 138;
    int h = 26;
    COLORREF dis_col = RGB(110, 110, 110);
    COLORREF en_col = RGB(240, 240, 240);

    /* MC */
    r.left = 8; r.top = y; r.right = 8 + col_w; r.bottom = y + h;
    draw_rounded_button(hdc, r, RGB(32, 32, 32), RGB(32, 32, 32), L"MC", 11, calc_has_memory ? en_col : dis_col, FALSE);

    /* MR */
    r.left += col_w; r.right += col_w;
    draw_rounded_button(hdc, r, RGB(32, 32, 32), RGB(32, 32, 32), L"MR", 11, calc_has_memory ? en_col : dis_col, FALSE);

    /* M+ */
    r.left += col_w; r.right += col_w;
    draw_rounded_button(hdc, r, RGB(32, 32, 32), RGB(32, 32, 32), L"M+", 11, en_col, FALSE);

    /* M- */
    r.left += col_w; r.right += col_w;
    draw_rounded_button(hdc, r, RGB(32, 32, 32), RGB(32, 32, 32), L"M-", 11, en_col, FALSE);

    /* MS */
    r.left += col_w; r.right += col_w;
    draw_rounded_button(hdc, r, RGB(32, 32, 32), RGB(32, 32, 32), L"MS", 11, en_col, FALSE);

    /* M▾ */
    r.left += col_w; r.right += col_w;
    draw_rounded_button(hdc, r, RGB(32, 32, 32), RGB(32, 32, 32), L"Mv", 11, calc_has_memory ? en_col : dis_col, FALSE);
}

static void xaml_render_scientific_keys(HDC hdc, int width, int height)
{
    static const WCHAR *sci_labels[4][5] = {
        { L"2nd", L"\x03C0", L"e", L"C", L"DEL" },
        { L"x\x00B2", L"1/x", L"|x|", L"exp", L"mod" },
        { L"\x221Ax", L"(", L")", L"n!", L"\x00F7" },
        { L"x^y", L"10^x", L"log", L"ln", L"\x00D7" }
    };
    int r, c;
    int col_w = (width - 16) / 5;
    int row_h = 32;
    int start_y = 166;
    RECT rect, cov;
    HBRUSH bg_br;

    cov.left = 0; cov.top = 165; cov.right = width; cov.bottom = start_y + 4 * row_h;
    bg_br = CreateSolidBrush(RGB(32, 32, 32));
    FillRect(hdc, &cov, bg_br);
    DeleteObject(bg_br);

    for (r = 0; r < 4; r++)
    {
        for (c = 0; c < 5; c++)
        {
            rect.left = 8 + c * col_w;
            rect.top = start_y + r * row_h;
            rect.right = rect.left + col_w - 2;
            rect.bottom = rect.top + row_h - 2;
            draw_rounded_button(hdc, rect, RGB(45, 45, 45), RGB(58, 58, 58), sci_labels[r][c], 11, RGB(255, 255, 255), FALSE);
        }
    }
}

static void xaml_render_programmer_panel(HDC hdc, int width, int height)
{
    RECT r, cov;
    HFONT font, old_font;
    WCHAR hex_str[32], dec_str[32], oct_str[32], bin_str[64];
    struct xaml_element *res_elem = global_root_element ? xaml_find_element_by_name(global_root_element, L"Results") : NULL;
    UINT64 val = 0;
    int c;
    int col_w;
    RECT rect;
    HBRUSH bg_br;

    cov.left = 0; cov.top = 135; cov.right = width; cov.bottom = 252;
    bg_br = CreateSolidBrush(RGB(32, 32, 32));
    FillRect(hdc, &cov, bg_br);
    DeleteObject(bg_br);

    if (res_elem) val = (UINT64)_wtoi64(res_elem->text);

    swprintf(hex_str, ARRAY_SIZE(hex_str), L"HEX  %llX", val);
    swprintf(dec_str, ARRAY_SIZE(dec_str), L"DEC  %llu", val);
    swprintf(oct_str, ARRAY_SIZE(oct_str), L"OCT  %llo", val);
    {
        WCHAR bin_digits[17];
        int b;
        for (b = 0; b < 16; b++) bin_digits[15 - b] = ((val >> b) & 1) ? '1' : '0';
        bin_digits[16] = 0;
        swprintf(bin_str, ARRAY_SIZE(bin_str), L"BIN  %.4s %.4s %.4s %.4s", bin_digits, bin_digits + 4, bin_digits + 8, bin_digits + 12);
    }

    font = CreateFontW(-MulDiv(12, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Consolas");
    old_font = SelectObject(hdc, font);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(200, 200, 200));

    r.left = 16; r.top = 138; r.right = width - 16; r.bottom = 156;
    DrawTextW(hdc, hex_str, -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    r.top += 18; r.bottom += 18;
    DrawTextW(hdc, dec_str, -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    r.top += 18; r.bottom += 18;
    DrawTextW(hdc, oct_str, -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    r.top += 18; r.bottom += 18;
    DrawTextW(hdc, bin_str, -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    SelectObject(hdc, old_font);
    DeleteObject(font);

    /* Hex row A..F */
    col_w = (width - 16) / 6;
    for (c = 0; c < 6; c++)
    {
        WCHAR letter[2] = { (WCHAR)('A' + c), 0 };
        rect.left = 8 + c * col_w;
        rect.top = 216;
        rect.right = rect.left + col_w - 2;
        rect.bottom = 246;
        draw_rounded_button(hdc, rect, RGB(45, 45, 45), RGB(58, 58, 58), letter, 12, RGB(255, 255, 255), FALSE);
    }
}

static void xaml_render_date_calc(HDC hdc, int width, int height)
{
    RECT r;
    HFONT font, old_font;

    r.left = 20; r.top = 80; r.right = width - 20; r.bottom = 110;
    font = CreateFontW(-MulDiv(16, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    old_font = SelectObject(hdc, font);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));
    DrawTextW(hdc, L"Difference between dates", -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, old_font);
    DeleteObject(font);

    font = CreateFontW(-MulDiv(13, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    old_font = SelectObject(hdc, font);
    SetTextColor(hdc, RGB(180, 180, 180));

    r.top = 130; r.bottom = 150;
    DrawTextW(hdc, L"From", -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    r.top = 152; r.bottom = 180;
    SetTextColor(hdc, RGB(255, 255, 255));
    DrawTextW(hdc, L"Thursday, October 29, 2026", -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    r.top = 200; r.bottom = 220;
    SetTextColor(hdc, RGB(180, 180, 180));
    DrawTextW(hdc, L"To", -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    r.top = 222; r.bottom = 250;
    SetTextColor(hdc, RGB(255, 255, 255));
    DrawTextW(hdc, L"Thursday, October 29, 2026", -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    r.top = 290; r.bottom = 310;
    SetTextColor(hdc, RGB(180, 180, 180));
    DrawTextW(hdc, L"Difference", -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    r.top = 315; r.bottom = 350;
    SelectObject(hdc, old_font);
    DeleteObject(font);

    font = CreateFontW(-MulDiv(22, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    old_font = SelectObject(hdc, font);
    SetTextColor(hdc, RGB(255, 255, 255));
    DrawTextW(hdc, L"Same day", -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, old_font);
    DeleteObject(font);
}

static void xaml_render_converter(HDC hdc, int width, int height)
{
    RECT r;
    HFONT font, old_font;
    WCHAR val1_str[32], val2_str[32], info_str[64];
    struct xaml_element *res_elem = global_root_element ? xaml_find_element_by_name(global_root_element, L"Results") : NULL;
    double v = 1.0;
    const WCHAR *u1 = L"Meters", *u2 = L"Feet";

    if (res_elem)
    {
        WCHAR buf[64];
        int j;
        lstrcpynW(buf, res_elem->text, ARRAY_SIZE(buf));
        for (j = 0; buf[j]; j++) if (buf[j] == ',') buf[j] = '.';
        v = wcstod(buf, NULL);
    }
    if (v == 0) v = 1.0;

    if (current_calc_mode == CALC_MODE_LENGTH) { u1 = L"Meters"; u2 = L"Feet"; swprintf(val2_str, ARRAY_SIZE(val2_str), L"%g", v * 3.28084); swprintf(info_str, ARRAY_SIZE(info_str), L"1 m = 3.28084 ft"); }
    else if (current_calc_mode == CALC_MODE_VOLUME) { u1 = L"Liters"; u2 = L"Gallons (US)"; swprintf(val2_str, ARRAY_SIZE(val2_str), L"%g", v * 0.264172); swprintf(info_str, ARRAY_SIZE(info_str), L"1 L = 0.264172 gal"); }
    else if (current_calc_mode == CALC_MODE_TEMPERATURE) { u1 = L"Celsius"; u2 = L"Fahrenheit"; swprintf(val2_str, ARRAY_SIZE(val2_str), L"%g", v * 9.0 / 5.0 + 32.0); swprintf(info_str, ARRAY_SIZE(info_str), L"0 \x00B0C = 32 \x00B0F"); }
    else if (current_calc_mode == CALC_MODE_WEIGHT) { u1 = L"Kilograms"; u2 = L"Pounds"; swprintf(val2_str, ARRAY_SIZE(val2_str), L"%g", v * 2.20462); swprintf(info_str, ARRAY_SIZE(info_str), L"1 kg = 2.20462 lbs"); }
    else if (current_calc_mode == CALC_MODE_SPEED) { u1 = L"Kilometers/h"; u2 = L"Miles/h"; swprintf(val2_str, ARRAY_SIZE(val2_str), L"%g", v * 0.621371); swprintf(info_str, ARRAY_SIZE(info_str), L"1 km/h = 0.621371 mph"); }
    else if (current_calc_mode == CALC_MODE_DATA) { u1 = L"Gigabytes"; u2 = L"Megabytes"; swprintf(val2_str, ARRAY_SIZE(val2_str), L"%g", v * 1024.0); swprintf(info_str, ARRAY_SIZE(info_str), L"1 GB = 1024 MB"); }
    else { u1 = L"Units"; u2 = L"Converted"; swprintf(val2_str, ARRAY_SIZE(val2_str), L"%g", v * 1.5); swprintf(info_str, ARRAY_SIZE(info_str), L"1 = 1.5"); }

    swprintf(val1_str, ARRAY_SIZE(val1_str), L"%g", v);

    /* Unit 1 display */
    r.left = 20; r.top = 60; r.right = width - 20; r.bottom = 95;
    font = CreateFontW(-MulDiv(26, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    old_font = SelectObject(hdc, font);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));
    DrawTextW(hdc, val1_str, -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, old_font);
    DeleteObject(font);

    r.top = 95; r.bottom = 115;
    font = CreateFontW(-MulDiv(13, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    old_font = SelectObject(hdc, font);
    SetTextColor(hdc, RGB(180, 180, 180));
    DrawTextW(hdc, u1, -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    /* Unit 2 display */
    r.top = 125; r.bottom = 160;
    SelectObject(hdc, old_font);
    DeleteObject(font);
    font = CreateFontW(-MulDiv(26, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    old_font = SelectObject(hdc, font);
    SetTextColor(hdc, RGB(255, 255, 255));
    DrawTextW(hdc, val2_str, -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, old_font);
    DeleteObject(font);

    r.top = 160; r.bottom = 180;
    font = CreateFontW(-MulDiv(13, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    old_font = SelectObject(hdc, font);
    SetTextColor(hdc, RGB(180, 180, 180));
    DrawTextW(hdc, u2, -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    r.top = 185; r.bottom = 205;
    DrawTextW(hdc, info_str, -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, old_font);
    DeleteObject(font);
}

static void xaml_render_settings(HDC hdc, int width, int height)
{
    RECT r;
    HFONT font, old_font;

    /* Back Button at (10, 48) */
    r.left = 10; r.top = 48; r.right = 80; r.bottom = 78;
    draw_rounded_button(hdc, r, RGB(45, 45, 45), RGB(60, 60, 60), L"\x2190 Back", 11, RGB(240, 240, 240), FALSE);

    /* Theme Section */
    r.left = 20; r.top = 95; r.right = width - 20; r.bottom = 120;
    font = CreateFontW(-MulDiv(15, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    old_font = SelectObject(hdc, font);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));
    DrawTextW(hdc, L"App theme", -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, old_font);
    DeleteObject(font);

    font = CreateFontW(-MulDiv(13, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    old_font = SelectObject(hdc, font);
    SetTextColor(hdc, RGB(220, 220, 220));

    {
        int opt_y = 126;
        int k;
        const WCHAR *opts[] = { L"Dark theme", L"Light theme", L"Use system setting" };
        for (k = 0; k < 3; k++)
        {
            boolean checked = (k == 0);
            HPEN rpen = CreatePen(PS_SOLID, 2, checked ? RGB(0, 120, 215) : RGB(140, 140, 140));
            HPEN rold_pen = SelectObject(hdc, rpen);
            HBRUSH rold_br = SelectObject(hdc, GetStockObject(NULL_BRUSH));
            Ellipse(hdc, 20, opt_y + 1, 36, opt_y + 17);
            if (checked)
            {
                HBRUSH dot_br = CreateSolidBrush(RGB(0, 120, 215));
                SelectObject(hdc, dot_br);
                Ellipse(hdc, 24, opt_y + 5, 32, opt_y + 13);
                DeleteObject(dot_br);
            }
            SelectObject(hdc, rold_br);
            SelectObject(hdc, rold_pen);
            DeleteObject(rpen);

            r.left = 44; r.top = opt_y; r.right = width - 20; r.bottom = opt_y + 18;
            DrawTextW(hdc, opts[k], -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
            opt_y += 24;
        }
    }

    /* About Section */
    r.top = 220; r.bottom = 245;
    SelectObject(hdc, old_font);
    DeleteObject(font);
    font = CreateFontW(-MulDiv(15, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    old_font = SelectObject(hdc, font);
    SetTextColor(hdc, RGB(255, 255, 255));
    DrawTextW(hdc, L"About", -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, old_font);
    DeleteObject(font);

    font = CreateFontW(-MulDiv(12, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    old_font = SelectObject(hdc, font);
    SetTextColor(hdc, RGB(180, 180, 180));

    r.top = 250; r.bottom = 270;
    DrawTextW(hdc, L"Calculator (Microsoft Windows Store App)", -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    r.top = 272; r.bottom = 292;
    DrawTextW(hdc, L"Version 11.2405.0.0", -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    r.top = 294; r.bottom = 314;
    DrawTextW(hdc, L"Wine / Proton-GE / Xodus WinUI Runtime", -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    r.top = 316; r.bottom = 336;
    DrawTextW(hdc, L"\x00A9 Microsoft Corporation. All rights reserved.", -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    SelectObject(hdc, old_font);
    DeleteObject(font);
}

static void xaml_render_nav_drawer(HDC hdc, int width, int height)
{
    HBRUSH br;
    HPEN pen;
    RECT r;
    UINT i;
    int y;
    HFONT font, old_font;

    if (!is_nav_pane_open) return;

    r.left = 0; r.top = 0; r.right = 260; r.bottom = height;
    br = CreateSolidBrush(RGB(37, 37, 37));
    FillRect(hdc, &r, br);
    DeleteObject(br);

    pen = CreatePen(PS_SOLID, 1, RGB(55, 55, 55));
    SelectObject(hdc, pen);
    MoveToEx(hdc, 260, 0, NULL);
    LineTo(hdc, 260, height);
    DeleteObject(pen);

    /* Drawer Header Hamburger */
    pen = CreatePen(PS_SOLID, 2, RGB(240, 240, 240));
    SelectObject(hdc, pen);
    MoveToEx(hdc, 12, 14, NULL); LineTo(hdc, 30, 14);
    MoveToEx(hdc, 12, 20, NULL); LineTo(hdc, 30, 20);
    MoveToEx(hdc, 12, 26, NULL); LineTo(hdc, 30, 26);
    DeleteObject(pen);

    y = 44;
    for (i = 0; i < NAV_ITEM_COUNT; i++)
    {
        r.left = 12; r.top = y; r.right = 248; r.bottom = y + 26;
        if (nav_items[i].is_header)
        {
            font = CreateFontW(-MulDiv(11, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                               DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                               CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
            old_font = SelectObject(hdc, font);
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(160, 160, 160));
            DrawTextW(hdc, nav_items[i].name, -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
            SelectObject(hdc, old_font);
            DeleteObject(font);
            y += 28;
        }
        else
        {
            RECT acc_r;
            boolean is_curr = (current_calc_mode == nav_items[i].mode);
            if (is_curr)
            {
                HBRUSH sel_br = CreateSolidBrush(RGB(50, 50, 50));
                FillRect(hdc, &r, sel_br);
                DeleteObject(sel_br);

                /* Blue accent bar */
                acc_r = r;
                acc_r.right = acc_r.left + 3;
                sel_br = CreateSolidBrush(RGB(0, 120, 215));
                FillRect(hdc, &acc_r, sel_br);
                DeleteObject(sel_br);
            }

            font = CreateFontW(-MulDiv(13, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, is_curr ? FW_BOLD : FW_NORMAL, FALSE, FALSE, FALSE,
                               DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                               CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
            old_font = SelectObject(hdc, font);
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, is_curr ? RGB(255, 255, 255) : RGB(220, 220, 220));
            r.left += 8;
            DrawTextW(hdc, nav_items[i].name, -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
            SelectObject(hdc, old_font);
            DeleteObject(font);
            y += 26;
        }
    }
}

static void xaml_render_history_drawer(HDC hdc, int width, int height)
{
    HBRUSH br;
    HPEN pen;
    RECT r;
    int i, y;
    HFONT font, old_font;

    if (!is_history_open) return;

    r.left = width - 260; r.top = 40; r.right = width; r.bottom = height;
    br = CreateSolidBrush(RGB(37, 37, 37));
    FillRect(hdc, &r, br);
    DeleteObject(br);

    pen = CreatePen(PS_SOLID, 1, RGB(55, 55, 55));
    SelectObject(hdc, pen);
    MoveToEx(hdc, width - 260, 40, NULL);
    LineTo(hdc, width - 260, height);
    DeleteObject(pen);

    /* Title */
    r.left += 16; r.top += 12; r.right -= 16; r.bottom = r.top + 24;
    font = CreateFontW(-MulDiv(14, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    old_font = SelectObject(hdc, font);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));
    DrawTextW(hdc, L"History", -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, old_font);
    DeleteObject(font);

    if (calc_history_count == 0)
    {
        r.top += 40; r.bottom = r.top + 30;
        font = CreateFontW(-MulDiv(12, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                           DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                           CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
        old_font = SelectObject(hdc, font);
        SetTextColor(hdc, RGB(160, 160, 160));
        DrawTextW(hdc, L"There's no history yet", -1, &r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdc, old_font);
        DeleteObject(font);
    }
    else
    {
        y = 80;
        for (i = calc_history_count - 1; i >= 0 && y < height - 60; i--)
        {
            r.top = y; r.bottom = y + 18;
            font = CreateFontW(-MulDiv(11, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                               DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                               CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
            old_font = SelectObject(hdc, font);
            SetTextColor(hdc, RGB(180, 180, 180));
            DrawTextW(hdc, calc_history_expr[i], -1, &r, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

            r.top = y + 18; r.bottom = y + 42;
            SelectObject(hdc, old_font);
            DeleteObject(font);

            font = CreateFontW(-MulDiv(16, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                               DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                               CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
            old_font = SelectObject(hdc, font);
            SetTextColor(hdc, RGB(255, 255, 255));
            DrawTextW(hdc, calc_history_res[i], -1, &r, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
            SelectObject(hdc, old_font);
            DeleteObject(font);

            y += 50;
        }

        /* Clear History button */
        r.left = width - 120; r.top = height - 42; r.right = width - 16; r.bottom = height - 12;
        draw_rounded_button(hdc, r, RGB(45, 45, 45), RGB(60, 60, 60), L"Clear", 11, RGB(240, 240, 240), FALSE);
    }
}

static void xaml_render_window(HWND hwnd, HDC hdc)
{
    RECT client;
    int width, height;
    HDC mem_dc;
    HBITMAP mem_bmp, old_bmp;
    HBRUSH bg_br;

    GetClientRect(hwnd, &client);
    width = client.right - client.left;
    height = client.bottom - client.top;
    if (width <= 0 || height <= 0) return;

    if (!is_calculator_app())
    {
        if (global_swapchain_active || (global_root_element && (global_root_element->type == CONTROL_TYPE_SWAPCHAINPANEL || global_root_element->swapchain)))
            return;
    }

    mem_dc = CreateCompatibleDC(hdc);
    mem_bmp = CreateCompatibleBitmap(hdc, width, height);
    old_bmp = SelectObject(mem_dc, mem_bmp);

    bg_br = CreateSolidBrush(RGB(32, 32, 32));
    FillRect(mem_dc, &client, bg_br);
    DeleteObject(bg_br);

    if (global_root_element)
    {
        xaml_arrange(global_root_element, client);
        xaml_render_element(mem_dc, global_root_element);
    }

    /* Top Navigation / Title Bar */
    xaml_render_header_bar(mem_dc, width, height);

    /* Render Active Mode components */
    if (current_calc_mode == CALC_MODE_STANDARD)
    {
        xaml_render_memory_bar(mem_dc, width);
    }
    else if (current_calc_mode == CALC_MODE_SCIENTIFIC)
    {
        xaml_render_memory_bar(mem_dc, width);
        xaml_render_scientific_keys(mem_dc, width, height);
    }
    else if (current_calc_mode == CALC_MODE_PROGRAMMER)
    {
        xaml_render_programmer_panel(mem_dc, width, height);
    }
    else if (current_calc_mode == CALC_MODE_DATE)
    {
        RECT cov = client;
        cov.top = 40;
        bg_br = CreateSolidBrush(RGB(32, 32, 32));
        FillRect(mem_dc, &cov, bg_br);
        DeleteObject(bg_br);
        xaml_render_date_calc(mem_dc, width, height);
    }
    else if (current_calc_mode >= CALC_MODE_CURRENCY && current_calc_mode <= CALC_MODE_ANGLE)
    {
        RECT cov = client;
        cov.top = 40;
        cov.bottom = 210;
        bg_br = CreateSolidBrush(RGB(32, 32, 32));
        FillRect(mem_dc, &cov, bg_br);
        DeleteObject(bg_br);
        xaml_render_converter(mem_dc, width, height);
    }
    else if (current_calc_mode == CALC_MODE_SETTINGS)
    {
        RECT cov = client;
        cov.top = 40;
        bg_br = CreateSolidBrush(RGB(32, 32, 32));
        FillRect(mem_dc, &cov, bg_br);
        DeleteObject(bg_br);
        xaml_render_settings(mem_dc, width, height);
    }

    /* Flyout Drawers */
    if (is_history_open)
    {
        xaml_render_history_drawer(mem_dc, width, height);
    }
    if (is_nav_pane_open)
    {
        xaml_render_nav_drawer(mem_dc, width, height);
    }

    BitBlt(hdc, 0, 0, width, height, mem_dc, 0, 0, SRCCOPY);

    SelectObject(mem_dc, old_bmp);
    DeleteObject(mem_bmp);
    DeleteDC(mem_dc);
}

/* -------------------------------------------------------------------------
 * Hit-Testing & Event Routing
 * ------------------------------------------------------------------------- */
static struct xaml_element *xaml_hittest(struct xaml_element *elem, POINT pt)
{
    int i;
    if (!elem || elem->visibility != 0) return NULL;
    if (!PtInRect(&elem->bounds, pt)) return NULL;

    if (elem->child_content)
    {
        struct xaml_element *hit = xaml_hittest(elem->child_content, pt);
        if (hit) return hit;
    }

    if (elem->children_count > 0)
    {
        for (i = (int)elem->children_count - 1; i >= 0; i--)
        {
            struct xaml_element *hit = xaml_hittest(elem->children[i], pt);
            if (hit) return hit;
        }
    }

    if (elem->type == CONTROL_TYPE_BUTTON) return elem;
    return NULL;
}

static void xaml_do_calc_action(const WCHAR *btn_name)
{
    struct xaml_element *res_elem = global_root_element ? xaml_find_element_by_name(global_root_element, L"Results") : NULL;
    struct xaml_element *expr_elem = global_root_element ? xaml_find_element_by_name(global_root_element, L"ExpressionText") : NULL;
    if (!res_elem || !btn_name) return;

    if (!wcsncmp(btn_name, L"Num", 3))
    {
        WCHAR digit = btn_name[3];
        if (calc_new_number)
        {
            res_elem->text[0] = digit;
            res_elem->text[1] = 0;
            calc_new_number = FALSE;
            calc_has_decimal = FALSE;
        }
        else
        {
            int len = wcslen(res_elem->text);
            if (len < 16)
            {
                if (len == 1 && res_elem->text[0] == '0')
                    res_elem->text[0] = digit;
                else
                {
                    res_elem->text[len] = digit;
                    res_elem->text[len + 1] = 0;
                }
            }
        }
    }
    else if (!wcscmp(btn_name, L"DecimalSeparatorButton"))
    {
        if (!calc_has_decimal)
        {
            int len = wcslen(res_elem->text);
            if (calc_new_number)
            {
                lstrcpyW(res_elem->text, L"0,");
                calc_new_number = FALSE;
            }
            else if (len < 15)
            {
                res_elem->text[len] = ',';
                res_elem->text[len + 1] = 0;
            }
            calc_has_decimal = TRUE;
        }
    }
    else if (!wcscmp(btn_name, L"ClearButton"))
    {
        lstrcpyW(res_elem->text, L"0");
        if (expr_elem) expr_elem->text[0] = 0;
        calc_accumulator = 0;
        calc_pending_op = 0;
        calc_has_decimal = FALSE;
        calc_new_number = TRUE;
    }
    else if (!wcscmp(btn_name, L"ClearEntryButton"))
    {
        lstrcpyW(res_elem->text, L"0");
        calc_has_decimal = FALSE;
        calc_new_number = TRUE;
    }
    else if (!wcscmp(btn_name, L"BackSpaceButton"))
    {
        int len = wcslen(res_elem->text);
        if (len > 1 && !calc_new_number)
        {
            if (res_elem->text[len - 1] == ',') calc_has_decimal = FALSE;
            res_elem->text[len - 1] = 0;
        }
        else
        {
            lstrcpyW(res_elem->text, L"0");
            calc_new_number = TRUE;
        }
    }
    else if (!wcscmp(btn_name, L"NegateButton"))
    {
        if (wcscmp(res_elem->text, L"0"))
        {
            if (res_elem->text[0] == '-')
                memmove(res_elem->text, res_elem->text + 1, (wcslen(res_elem->text)) * sizeof(WCHAR));
            else
            {
                memmove(res_elem->text + 1, res_elem->text, (wcslen(res_elem->text) + 1) * sizeof(WCHAR));
                res_elem->text[0] = '-';
            }
        }
    }
    else if (!wcscmp(btn_name, L"InvertButton") || !wcscmp(btn_name, L"1/x"))
    {
        WCHAR buf[64];
        double val;
        int j;
        lstrcpynW(buf, res_elem->text, ARRAY_SIZE(buf));
        for (j = 0; buf[j]; j++) if (buf[j] == ',') buf[j] = '.';
        val = wcstod(buf, NULL);
        if (val != 0) val = 1.0 / val;
        swprintf(res_elem->text, ARRAY_SIZE(res_elem->text), L"%g", val);
        for (j = 0; res_elem->text[j]; j++) if (res_elem->text[j] == '.') res_elem->text[j] = ',';
        calc_new_number = TRUE;
    }
    else if (!wcscmp(btn_name, L"XPower2Button") || !wcscmp(btn_name, L"x\x00B2"))
    {
        WCHAR buf[64];
        double val;
        int j;
        lstrcpynW(buf, res_elem->text, ARRAY_SIZE(buf));
        for (j = 0; buf[j]; j++) if (buf[j] == ',') buf[j] = '.';
        val = wcstod(buf, NULL);
        val = val * val;
        swprintf(res_elem->text, ARRAY_SIZE(res_elem->text), L"%g", val);
        for (j = 0; res_elem->text[j]; j++) if (res_elem->text[j] == '.') res_elem->text[j] = ',';
        calc_new_number = TRUE;
    }
    else if (!wcscmp(btn_name, L"SquareRootButton") || !wcscmp(btn_name, L"\x221Ax"))
    {
        WCHAR buf[64];
        double val;
        int j;
        lstrcpynW(buf, res_elem->text, ARRAY_SIZE(buf));
        for (j = 0; buf[j]; j++) if (buf[j] == ',') buf[j] = '.';
        val = wcstod(buf, NULL);
        if (val >= 0) val = sqrt(val);
        swprintf(res_elem->text, ARRAY_SIZE(res_elem->text), L"%g", val);
        for (j = 0; res_elem->text[j]; j++) if (res_elem->text[j] == '.') res_elem->text[j] = ',';
        calc_new_number = TRUE;
    }
    else if (!wcscmp(btn_name, L"PercentButton") || !wcscmp(btn_name, L"%"))
    {
        WCHAR buf[64];
        double val;
        int j;
        lstrcpynW(buf, res_elem->text, ARRAY_SIZE(buf));
        for (j = 0; buf[j]; j++) if (buf[j] == ',') buf[j] = '.';
        val = wcstod(buf, NULL);
        val = (calc_accumulator != 0) ? (calc_accumulator * val / 100.0) : (val / 100.0);
        swprintf(res_elem->text, ARRAY_SIZE(res_elem->text), L"%g", val);
        for (j = 0; res_elem->text[j]; j++) if (res_elem->text[j] == '.') res_elem->text[j] = ',';
        calc_new_number = TRUE;
    }
    else if (!wcscmp(btn_name, L"\x03C0") || !wcscmp(btn_name, L"PiButton"))
    {
        swprintf(res_elem->text, ARRAY_SIZE(res_elem->text), L"3,141592653589793");
        calc_new_number = TRUE;
    }
    else if (!wcscmp(btn_name, L"e") || !wcscmp(btn_name, L"EulerButton"))
    {
        swprintf(res_elem->text, ARRAY_SIZE(res_elem->text), L"2,718281828459045");
        calc_new_number = TRUE;
    }
    else if (!wcscmp(btn_name, L"log"))
    {
        WCHAR buf[64];
        double val;
        int j;
        lstrcpynW(buf, res_elem->text, ARRAY_SIZE(buf));
        for (j = 0; buf[j]; j++) if (buf[j] == ',') buf[j] = '.';
        val = wcstod(buf, NULL);
        if (val > 0) val = log10(val);
        swprintf(res_elem->text, ARRAY_SIZE(res_elem->text), L"%g", val);
        for (j = 0; res_elem->text[j]; j++) if (res_elem->text[j] == '.') res_elem->text[j] = ',';
        calc_new_number = TRUE;
    }
    else if (!wcscmp(btn_name, L"ln"))
    {
        WCHAR buf[64];
        double val;
        int j;
        lstrcpynW(buf, res_elem->text, ARRAY_SIZE(buf));
        for (j = 0; buf[j]; j++) if (buf[j] == ',') buf[j] = '.';
        val = wcstod(buf, NULL);
        if (val > 0) val = log(val);
        swprintf(res_elem->text, ARRAY_SIZE(res_elem->text), L"%g", val);
        for (j = 0; res_elem->text[j]; j++) if (res_elem->text[j] == '.') res_elem->text[j] = ',';
        calc_new_number = TRUE;
    }
    else if (!wcscmp(btn_name, L"n!"))
    {
        WCHAR buf[64];
        double val, f = 1.0;
        int j, k, n;
        lstrcpynW(buf, res_elem->text, ARRAY_SIZE(buf));
        for (j = 0; buf[j]; j++) if (buf[j] == ',') buf[j] = '.';
        val = wcstod(buf, NULL);
        n = (int)val;
        if (n >= 0 && n <= 170)
        {
            for (k = 2; k <= n; k++) f *= k;
            swprintf(res_elem->text, ARRAY_SIZE(res_elem->text), L"%g", f);
            for (j = 0; res_elem->text[j]; j++) if (res_elem->text[j] == '.') res_elem->text[j] = ',';
        }
        calc_new_number = TRUE;
    }
    else if (!wcscmp(btn_name, L"|x|"))
    {
        WCHAR buf[64];
        double val;
        int j;
        lstrcpynW(buf, res_elem->text, ARRAY_SIZE(buf));
        for (j = 0; buf[j]; j++) if (buf[j] == ',') buf[j] = '.';
        val = fabs(wcstod(buf, NULL));
        swprintf(res_elem->text, ARRAY_SIZE(res_elem->text), L"%g", val);
        for (j = 0; res_elem->text[j]; j++) if (res_elem->text[j] == '.') res_elem->text[j] = ',';
        calc_new_number = TRUE;
    }
    else if (!wcscmp(btn_name, L"PlusButton") || !wcscmp(btn_name, L"MinusButton") ||
             !wcscmp(btn_name, L"MultiplyButton") || !wcscmp(btn_name, L"DivideButton"))
    {
        WCHAR buf[64];
        double val;
        int j;
        lstrcpynW(buf, res_elem->text, ARRAY_SIZE(buf));
        for (j = 0; buf[j]; j++) if (buf[j] == ',') buf[j] = '.';
        val = wcstod(buf, NULL);

        if (calc_pending_op && !calc_new_number)
        {
            if (calc_pending_op == '+') calc_accumulator += val;
            else if (calc_pending_op == '-') calc_accumulator -= val;
            else if (calc_pending_op == '*') calc_accumulator *= val;
            else if (calc_pending_op == '/' && val != 0) calc_accumulator /= val;
            swprintf(res_elem->text, ARRAY_SIZE(res_elem->text), L"%g", calc_accumulator);
            for (j = 0; res_elem->text[j]; j++) if (res_elem->text[j] == '.') res_elem->text[j] = ',';
        }
        else
        {
            calc_accumulator = val;
        }
        if (!wcscmp(btn_name, L"PlusButton")) calc_pending_op = '+';
        else if (!wcscmp(btn_name, L"MinusButton")) calc_pending_op = '-';
        else if (!wcscmp(btn_name, L"MultiplyButton")) calc_pending_op = '*';
        else if (!wcscmp(btn_name, L"DivideButton")) calc_pending_op = '/';

        if (expr_elem)
            swprintf(expr_elem->text, ARRAY_SIZE(expr_elem->text), L"%g %c", calc_accumulator,
                     calc_pending_op == '*' ? 0x00D7 : (calc_pending_op == '/' ? 0x00F7 : calc_pending_op));

        calc_new_number = TRUE;
        calc_has_decimal = FALSE;
    }
    else if (!wcscmp(btn_name, L"EqualButton"))
    {
        WCHAR buf[64];
        double val;
        int j;
        lstrcpynW(buf, res_elem->text, ARRAY_SIZE(buf));
        for (j = 0; buf[j]; j++) if (buf[j] == ',') buf[j] = '.';
        val = wcstod(buf, NULL);

        if (calc_pending_op)
        {
            WCHAR expr_buf[64];
            swprintf(expr_buf, ARRAY_SIZE(expr_buf), L"%g %c %g =", calc_accumulator,
                     calc_pending_op == '*' ? 0x00D7 : (calc_pending_op == '/' ? 0x00F7 : calc_pending_op), val);

            if (expr_elem) lstrcpynW(expr_elem->text, expr_buf, ARRAY_SIZE(expr_elem->text));

            if (calc_pending_op == '+') calc_accumulator += val;
            else if (calc_pending_op == '-') calc_accumulator -= val;
            else if (calc_pending_op == '*') calc_accumulator *= val;
            else if (calc_pending_op == '/' && val != 0) calc_accumulator /= val;

            swprintf(res_elem->text, ARRAY_SIZE(res_elem->text), L"%g", calc_accumulator);
            for (j = 0; res_elem->text[j]; j++) if (res_elem->text[j] == '.') res_elem->text[j] = ',';

            /* Record into History */
            if (calc_history_count < MAX_CALC_HISTORY)
            {
                lstrcpynW(calc_history_expr[calc_history_count], expr_buf, 64);
                lstrcpynW(calc_history_res[calc_history_count], res_elem->text, 32);
                calc_history_count++;
            }

            calc_pending_op = 0;
            calc_new_number = TRUE;
            calc_has_decimal = FALSE;
        }
    }
    else if (!wcscmp(btn_name, L"ClearMemoryButton") || !wcscmp(btn_name, L"MC"))
    {
        calc_memory = 0.0;
        calc_has_memory = FALSE;
    }
    else if (!wcscmp(btn_name, L"MemRecall") || !wcscmp(btn_name, L"MR"))
    {
        if (calc_has_memory)
        {
            int j;
            swprintf(res_elem->text, ARRAY_SIZE(res_elem->text), L"%g", calc_memory);
            for (j = 0; res_elem->text[j]; j++) if (res_elem->text[j] == '.') res_elem->text[j] = ',';
            calc_new_number = TRUE;
        }
    }
    else if (!wcscmp(btn_name, L"MemPlus") || !wcscmp(btn_name, L"M+"))
    {
        WCHAR buf[64];
        double val;
        int j;
        lstrcpynW(buf, res_elem->text, ARRAY_SIZE(buf));
        for (j = 0; buf[j]; j++) if (buf[j] == ',') buf[j] = '.';
        val = wcstod(buf, NULL);
        calc_memory += val;
        calc_has_memory = TRUE;
        calc_new_number = TRUE;
    }
    else if (!wcscmp(btn_name, L"MemMinus") || !wcscmp(btn_name, L"M-"))
    {
        WCHAR buf[64];
        double val;
        int j;
        lstrcpynW(buf, res_elem->text, ARRAY_SIZE(buf));
        for (j = 0; buf[j]; j++) if (buf[j] == ',') buf[j] = '.';
        val = wcstod(buf, NULL);
        calc_memory -= val;
        calc_has_memory = TRUE;
        calc_new_number = TRUE;
    }
    else if (!wcscmp(btn_name, L"MemButton") || !wcscmp(btn_name, L"MS"))
    {
        WCHAR buf[64];
        double val;
        int j;
        lstrcpynW(buf, res_elem->text, ARRAY_SIZE(buf));
        for (j = 0; buf[j]; j++) if (buf[j] == ',') buf[j] = '.';
        val = wcstod(buf, NULL);
        calc_memory = val;
        calc_has_memory = TRUE;
        calc_new_number = TRUE;
    }

    if (global_xaml_hwnd)
    {
        InvalidateRect(global_xaml_hwnd, NULL, FALSE);
    }
}

static void xaml_fire_click(struct xaml_element *btn)
{
    void **vtbl;
    if (!btn) return;
    TRACE("Firing Click on button %s (handler %p)\n", debugstr_w(btn->name), btn->click_handler);

    if (btn->command)
    {
        void **cmd_vtbl = *(void ***)btn->command;
        if (cmd_vtbl && (ULONG_PTR)cmd_vtbl[9] > 0x10000)
        {
            HRESULT (WINAPI *pExecute)(void *, void *) = (void *)cmd_vtbl[9];
            HRESULT hr = pExecute(btn->command, btn->command_param);
            TRACE("Command Execute returned hr=0x%08lx\n", hr);
        }
    }

    if (btn->click_handler)
    {
        vtbl = *(void ***)btn->click_handler;
        if (vtbl && (ULONG_PTR)vtbl[3] > 0x10000)
        {
            HRESULT (WINAPI *pInvoke)(void *, void *, void *) = (void *)vtbl[3];
            HRESULT hr = pInvoke(btn->click_handler, &btn->IUIElement_iface, NULL);
            TRACE("Click handler Invoke returned hr=0x%08lx\n", hr);
        }
    }
    xaml_do_calc_action(btn->name);
}

static struct xaml_element *xaml_find_button_by_name(struct xaml_element *elem, const WCHAR *name)
{
    UINT32 i;
    if (!elem) return NULL;
    if (elem->type == CONTROL_TYPE_BUTTON && !wcscmp(elem->name, name)) return elem;

    if (elem->child_content)
    {
        struct xaml_element *res = xaml_find_button_by_name(elem->child_content, name);
        if (res) return res;
    }

    for (i = 0; i < elem->children_count; i++)
    {
        struct xaml_element *res = xaml_find_button_by_name(elem->children[i], name);
        if (res) return res;
    }
    return NULL;
}

static void xaml_handle_keydown(WPARAM key)
{
    const WCHAR *target_btn = NULL;

    /* Check Alt key chords (mode switching) */
    if (GetKeyState(VK_MENU) < 0)
    {
        if (key == '1') current_calc_mode = CALC_MODE_STANDARD;
        else if (key == '2') current_calc_mode = CALC_MODE_SCIENTIFIC;
        else if (key == '3') current_calc_mode = CALC_MODE_PROGRAMMER;
        else if (key == '4') current_calc_mode = CALC_MODE_DATE;

        if (global_xaml_hwnd) InvalidateRect(global_xaml_hwnd, NULL, FALSE);
        return;
    }

    /* Check Ctrl key chords (history, memory) */
    if (GetKeyState(VK_CONTROL) < 0)
    {
        if (key == 'H' || key == 'h')
        {
            is_history_open = !is_history_open;
            if (global_xaml_hwnd) InvalidateRect(global_xaml_hwnd, NULL, FALSE);
            return;
        }
    }

    /* Programmer mode hex keys */
    if (current_calc_mode == CALC_MODE_PROGRAMMER)
    {
        if (key >= 'A' && key <= 'F')
        {
            struct xaml_element *res = global_root_element ? xaml_find_element_by_name(global_root_element, L"Results") : NULL;
            if (res)
            {
                int len = wcslen(res->text);
                if (calc_new_number || !wcscmp(res->text, L"0"))
                {
                    res->text[0] = (WCHAR)key;
                    res->text[1] = 0;
                    calc_new_number = FALSE;
                }
                else if (len < 16)
                {
                    res->text[len] = (WCHAR)key;
                    res->text[len + 1] = 0;
                }
                if (global_xaml_hwnd) InvalidateRect(global_xaml_hwnd, NULL, FALSE);
                return;
            }
        }
    }

    switch (key)
    {
        case '0': case VK_NUMPAD0: target_btn = L"Num0Button"; break;
        case '1': case VK_NUMPAD1: target_btn = L"Num1Button"; break;
        case '2': case VK_NUMPAD2: target_btn = L"Num2Button"; break;
        case '3': case VK_NUMPAD3: target_btn = L"Num3Button"; break;
        case '4': case VK_NUMPAD4: target_btn = L"Num4Button"; break;
        case '5': case VK_NUMPAD5: target_btn = L"Num5Button"; break;
        case '6': case VK_NUMPAD6: target_btn = L"Num6Button"; break;
        case '7': case VK_NUMPAD7: target_btn = L"Num7Button"; break;
        case '8': case VK_NUMPAD8: target_btn = L"Num8Button"; break;
        case '9': case VK_NUMPAD9: target_btn = L"Num9Button"; break;
        case VK_ADD: case 0xBB: case '+': target_btn = L"PlusButton"; break;
        case VK_SUBTRACT: case 0xBD: case '-': target_btn = L"MinusButton"; break;
        case VK_MULTIPLY: case '*': case 'x': case 'X': target_btn = L"MultiplyButton"; break;
        case VK_DIVIDE: case 0xBF: case '/': target_btn = L"DivideButton"; break;
        case VK_RETURN: case '=': target_btn = L"EqualButton"; break;
        case VK_BACK: target_btn = L"BackSpaceButton"; break;
        case VK_ESCAPE: target_btn = L"ClearButton"; break;
        case VK_DECIMAL: case VK_OEM_PERIOD: case VK_OEM_COMMA: case ',': case '.': target_btn = L"DecimalSeparatorButton"; break;
    }
    if (target_btn && global_root_element)
    {
        struct xaml_element *btn = xaml_find_button_by_name(global_root_element, target_btn);
        if (btn)
        {
            btn->is_pressed = TRUE;
            InvalidateRect(global_xaml_hwnd, NULL, FALSE);
            UpdateWindow(global_xaml_hwnd);
            xaml_fire_click(btn);
            btn->is_pressed = FALSE;
            InvalidateRect(global_xaml_hwnd, NULL, FALSE);
        }
    }
}

/* -------------------------------------------------------------------------
 * Window Procedure for Windows.UI.Xaml.Window
 * ------------------------------------------------------------------------- */
static struct xaml_element *active_pressed_button = NULL;

extern void core_window_fire_size_changed(FLOAT width, FLOAT height);
extern void core_window_fire_activated(int state);
extern void core_window_fire_pointer(UINT msg, WPARAM wparam, LPARAM lparam);
extern void core_window_fire_key(UINT msg, WPARAM wparam, LPARAM lparam);
extern void core_window_fire_char(WPARAM wparam, LPARAM lparam);

LRESULT CALLBACK xaml_window_wndproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            xaml_fire_loaded_events();
            if (is_calculator_app() || (!global_swapchain_active && (!global_root_element || (!global_root_element->swapchain && global_root_element->type != CONTROL_TYPE_SWAPCHAINPANEL))))
            {
                xaml_render_window(hwnd, hdc);
            }
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_ERASEBKGND:
            return 1;
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        {
            POINT pt;
            RECT client;
            core_window_fire_pointer(msg, wparam, lparam);
            elem_fire_pointer_all(msg, wparam, lparam);
            if (!is_calculator_app())
            {
                pt.x = (short)LOWORD(lparam);
                pt.y = (short)HIWORD(lparam);
                if (global_root_element && msg == WM_LBUTTONDOWN)
                {
                    struct xaml_element *hit = xaml_hittest(global_root_element, pt);
                    if (hit)
                    {
                        active_pressed_button = hit;
                        hit->is_pressed = TRUE;
                        SetCapture(hwnd);
                        InvalidateRect(hwnd, NULL, FALSE);
                        return 0;
                    }
                }
                return DefWindowProcW(hwnd, msg, wparam, lparam);
            }
            pt.x = (short)LOWORD(lparam);
            pt.y = (short)HIWORD(lparam);
            GetClientRect(hwnd, &client);

            /* 1. If Navigation Drawer is open */
            if (is_nav_pane_open)
            {
                if (pt.x <= 260)
                {
                    int y = 44;
                    UINT i;
                    /* Hamburger button inside drawer */
                    if (pt.x <= 40 && pt.y <= 36)
                    {
                        is_nav_pane_open = FALSE;
                        InvalidateRect(hwnd, NULL, FALSE);
                        return 0;
                    }
                    /* Check nav items */
                    for (i = 0; i < NAV_ITEM_COUNT; i++)
                    {
                        if (nav_items[i].is_header)
                        {
                            y += 28;
                        }
                        else
                        {
                            if (pt.y >= y && pt.y < y + 26)
                            {
                                current_calc_mode = nav_items[i].mode;
                                is_nav_pane_open = FALSE;
                                InvalidateRect(hwnd, NULL, FALSE);
                                return 0;
                            }
                            y += 26;
                        }
                    }
                }
                else
                {
                    /* Clicked outside drawer: light dismiss */
                    is_nav_pane_open = FALSE;
                    InvalidateRect(hwnd, NULL, FALSE);
                    return 0;
                }
            }

            /* 2. If History Drawer is open */
            if (is_history_open)
            {
                if (pt.x >= client.right - 260)
                {
                    /* Clear history button */
                    if (pt.y >= client.bottom - 42 && pt.y <= client.bottom - 12 &&
                        pt.x >= client.right - 120 && pt.x <= client.right - 16)
                    {
                        calc_history_count = 0;
                        InvalidateRect(hwnd, NULL, FALSE);
                        return 0;
                    }
                }
                else
                {
                    /* Clicked outside history drawer: light dismiss */
                    is_history_open = FALSE;
                    InvalidateRect(hwnd, NULL, FALSE);
                    return 0;
                }
            }

            /* 3. Top Header Bar clicks */
            if (pt.y <= 40)
            {
                /* Hamburger button */
                if (pt.x <= 42)
                {
                    is_nav_pane_open = !is_nav_pane_open;
                    if (is_nav_pane_open) is_history_open = FALSE;
                    InvalidateRect(hwnd, NULL, FALSE);
                    return 0;
                }
                /* History button */
                if (pt.x >= client.right - 76 && pt.x <= client.right - 42)
                {
                    is_history_open = !is_history_open;
                    if (is_history_open) is_nav_pane_open = FALSE;
                    InvalidateRect(hwnd, NULL, FALSE);
                    return 0;
                }
            }

            /* 4. Settings Back button */
            if (current_calc_mode == CALC_MODE_SETTINGS && pt.y >= 48 && pt.y <= 78 && pt.x >= 10 && pt.x <= 80)
            {
                current_calc_mode = CALC_MODE_STANDARD;
                InvalidateRect(hwnd, NULL, FALSE);
                return 0;
            }

            /* 5. Memory Bar clicks (y: 135 to 165) */
            if ((current_calc_mode == CALC_MODE_STANDARD || current_calc_mode == CALC_MODE_SCIENTIFIC) &&
                pt.y >= 135 && pt.y <= 165)
            {
                int col_w = (client.right - 16) / 6;
                int mem_idx = (pt.x - 8) / col_w;
                if (mem_idx == 0) xaml_do_calc_action(L"ClearMemoryButton");
                else if (mem_idx == 1) xaml_do_calc_action(L"MemRecall");
                else if (mem_idx == 2) xaml_do_calc_action(L"MemPlus");
                else if (mem_idx == 3) xaml_do_calc_action(L"MemMinus");
                else if (mem_idx == 4) xaml_do_calc_action(L"MemButton");
                else if (mem_idx == 5) { is_history_open = !is_history_open; }
                InvalidateRect(hwnd, NULL, FALSE);
                return 0;
            }

            /* 6. Scientific operators row clicks */
            if (current_calc_mode == CALC_MODE_SCIENTIFIC && pt.y >= 166 && pt.y < 166 + 4 * 32)
            {
                static const WCHAR *sci_labels[4][5] = {
                    { L"2nd", L"\x03C0", L"e", L"ClearButton", L"BackSpaceButton" },
                    { L"XPower2Button", L"InvertButton", L"|x|", L"exp", L"mod" },
                    { L"SquareRootButton", L"(", L")", L"n!", L"DivideButton" },
                    { L"x\x02B8", L"10\x02E3", L"log", L"ln", L"MultiplyButton" }
                };
                int col_w = (client.right - 16) / 5;
                int r_idx = (pt.y - 166) / 32;
                int c_idx = (pt.x - 8) / col_w;
                if (r_idx >= 0 && r_idx < 4 && c_idx >= 0 && c_idx < 5)
                {
                    xaml_do_calc_action(sci_labels[r_idx][c_idx]);
                    InvalidateRect(hwnd, NULL, FALSE);
                    return 0;
                }
            }

            /* 7. Programmer Hex keys (y: 216 to 246) */
            if (current_calc_mode == CALC_MODE_PROGRAMMER && pt.y >= 216 && pt.y <= 246)
            {
                int col_w = (client.right - 16) / 6;
                int hex_idx = (pt.x - 8) / col_w;
                if (hex_idx >= 0 && hex_idx < 6)
                {
                    struct xaml_element *res = global_root_element ? xaml_find_element_by_name(global_root_element, L"Results") : NULL;
                    if (res)
                    {
                        int len = wcslen(res->text);
                        if (calc_new_number || !wcscmp(res->text, L"0"))
                        {
                            res->text[0] = (WCHAR)('A' + hex_idx);
                            res->text[1] = 0;
                            calc_new_number = FALSE;
                        }
                        else if (len < 16)
                        {
                            res->text[len] = (WCHAR)('A' + hex_idx);
                            res->text[len + 1] = 0;
                        }
                        InvalidateRect(hwnd, NULL, FALSE);
                        return 0;
                    }
                }
            }

            /* 8. Standard button grid hit testing */
            {
                struct xaml_element *hit = xaml_hittest(global_root_element, pt);
                if (hit)
                {
                    active_pressed_button = hit;
                    hit->is_pressed = TRUE;
                    SetCapture(hwnd);
                    InvalidateRect(hwnd, NULL, FALSE);
                }
            }
            return 0;
        }
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        {
            core_window_fire_pointer(msg, wparam, lparam);
            elem_fire_pointer_all(msg, wparam, lparam);
            if (!is_calculator_app())
            {
                if (active_pressed_button)
                {
                    POINT pt;
                    struct xaml_element *hit;
                    pt.x = (short)LOWORD(lparam);
                    pt.y = (short)HIWORD(lparam);
                    hit = xaml_hittest(global_root_element, pt);
                    active_pressed_button->is_pressed = FALSE;
                    ReleaseCapture();
                    InvalidateRect(hwnd, NULL, FALSE);
                    if (hit == active_pressed_button)
                    {
                        xaml_fire_click(active_pressed_button);
                    }
                    active_pressed_button = NULL;
                    return 0;
                }
                return DefWindowProcW(hwnd, msg, wparam, lparam);
            }
            if (active_pressed_button)
            {
                POINT pt;
                struct xaml_element *hit;
                pt.x = (short)LOWORD(lparam);
                pt.y = (short)HIWORD(lparam);
                hit = xaml_hittest(global_root_element, pt);
                active_pressed_button->is_pressed = FALSE;
                ReleaseCapture();
                InvalidateRect(hwnd, NULL, FALSE);
                if (hit == active_pressed_button)
                {
                    xaml_fire_click(active_pressed_button);
                }
                active_pressed_button = NULL;
            }
            return 0;
        }
        case WM_MOUSEMOVE:
        {
            core_window_fire_pointer(msg, wparam, lparam);
            elem_fire_pointer_all(msg, wparam, lparam);
            break;
        }
        case WM_MOUSEWHEEL:
        {
            core_window_fire_pointer(msg, wparam, lparam);
            elem_fire_pointer_all(msg, wparam, lparam);
            return DefWindowProcW(hwnd, msg, wparam, lparam);
        }
        case WM_KEYDOWN:
            core_window_fire_key(msg, wparam, lparam);
            if (is_calculator_app())
            {
                xaml_handle_keydown(wparam);
                return 0;
            }
            return 0;
        case WM_KEYUP:
            core_window_fire_key(msg, wparam, lparam);
            return 0;
        case WM_CHAR:
            core_window_fire_char(wparam, lparam);
            return 0;
            return DefWindowProcW(hwnd, msg, wparam, lparam);
        case WM_SIZE:
        {
            int w = LOWORD(lparam);
            int h = HIWORD(lparam);
            if (global_swapchain_panel)
            {
                elem_fire_size_changed(global_swapchain_panel, 0.0f, 0.0f, (float)w, (float)h);
            }
            if (global_root_element && global_root_element != global_swapchain_panel)
            {
                elem_fire_size_changed(global_root_element, 0.0f, 0.0f, (float)w, (float)h);
            }
            if (w > 0 && h > 0)
                core_window_fire_size_changed((FLOAT)w, (FLOAT)h);
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }
        case WM_ACTIVATE:
        {
            TRACE("xaml_window_wndproc: WM_ACTIVATE wparam=%lx\n", (unsigned long)wparam);
            if (wparam != WA_INACTIVE)
                core_window_fire_activated(0 /* CodeActivated */);
            else
                core_window_fire_activated(1 /* Deactivated */);
            break;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wparam, lparam);
}

void xaml_window_init(HWND hwnd)
{
    global_xaml_hwnd = hwnd;
}

void xaml_fire_swapchain_size_changed(FLOAT width, FLOAT height)
{
    if (global_swapchain_panel)
    {
        TRACE("Firing SizeChanged on global_swapchain_panel %p: %.0f x %.0f\n", global_swapchain_panel, width, height);
        elem_fire_size_changed(global_swapchain_panel, 0.0f, 0.0f, width, height);
    }
    if (global_root_element && global_root_element != global_swapchain_panel)
    {
        elem_fire_size_changed(global_root_element, 0.0f, 0.0f, width, height);
    }
}

static const GUID IID_IRoutedEventArgs = {0x5c985ac6, 0xd802, 0x4b38, {0xa2, 0x23, 0xbf, 0x07, 0x0c, 0x43, 0xfe, 0xdf}};
static const GUID IID_IRoutedEventHandler = {0xa856e674, 0xb0b6, 0x4bc3, {0xbb, 0xa8, 0x1b, 0xa0, 0x6e, 0x40, 0xd4, 0xb5}};

struct dummy_routed_args_t
{
    const void *lpVtbl;
    LONG ref;
};

static HRESULT WINAPI routed_args_QI(void *This, REFIID riid, void **ppv)
{
    if (!ppv) return E_POINTER;
    *ppv = NULL;
    if (IsEqualGUID(riid, &IID_IUnknown) ||
        IsEqualGUID(riid, &IID_IInspectable) ||
        IsEqualGUID(riid, &IID_IRoutedEventArgs))
    {
        *ppv = This;
        return S_OK;
    }
    return E_NOINTERFACE;
}
static ULONG WINAPI routed_args_AddRef(void *This)
{
    struct dummy_routed_args_t *args = This;
    return InterlockedIncrement(&args->ref);
}
static ULONG WINAPI routed_args_Release(void *This)
{
    struct dummy_routed_args_t *args = This;
    return InterlockedDecrement(&args->ref);
}
static HRESULT WINAPI routed_args_GetIids(void *This, ULONG *c, IID **i) { if (c) *c = 0; return S_OK; }
static HRESULT WINAPI routed_args_GetRuntimeClassName(void *This, HSTRING *n) { return WindowsCreateString(L"Windows.UI.Xaml.RoutedEventArgs", 31, n); }
static HRESULT WINAPI routed_args_GetTrustLevel(void *This, TrustLevel *t) { if (t) *t = BaseTrust; return S_OK; }
static HRESULT WINAPI routed_args_get_OriginalSource(void *This, void **val)
{
    if (val) *val = global_page_element ? (void *)&global_page_element->IUIElement_iface : NULL;
    return S_OK;
}

static const void *routed_args_vtbl[] = {
    (void *)routed_args_QI,
    (void *)routed_args_AddRef,
    (void *)routed_args_Release,
    (void *)routed_args_GetIids,
    (void *)routed_args_GetRuntimeClassName,
    (void *)routed_args_GetTrustLevel,
    (void *)routed_args_get_OriginalSource
};
static struct dummy_routed_args_t dummy_routed_args = { routed_args_vtbl, 1000 };

void xaml_fire_loaded_events(void)
{
    if (global_page_element && global_page_element->loaded_handler)
    {
        void *handler = global_page_element->loaded_handler;
        void *sender = &global_page_element->IUIElement_iface;
        void **vtbl = *(void ***)handler;
        HRESULT hr;

        TRACE("xaml_fire_loaded_events: global_page_element %p, handler %p, sender %p\n", global_page_element, handler, sender);
        global_page_element->loaded_handler = NULL;

        if (vtbl && (ULONG_PTR)vtbl[3] > 0x10000)
        {
            HRESULT (WINAPI *pInvoke)(void *, void *, void *) = (void *)vtbl[3];
            TRACE("Firing delayed MainPage OnPageLoaded delegate %p via slot 3 (%p)\n", handler, pInvoke);
            hr = pInvoke(handler, sender, (void *)&dummy_routed_args);
            TRACE("OnPageLoaded delegate returned hr=0x%08lx\n", hr);
        }
    }
}

/* -------------------------------------------------------------------------
 * XAML Visual Tree Builder & IComponentConnector Dispatcher
 * ------------------------------------------------------------------------- */
static void connect_control_iface(void *connector, int connection_id, void *iface, const WCHAR *name)
{
    void **vtbl;
    if (!connector || !iface) return;
    vtbl = *(void ***)connector;
    if (vtbl && (ULONG_PTR)vtbl[6] > 0x10000)
    {
        HRESULT (WINAPI *pConnect)(void *, int, void *) = (void *)vtbl[6];
        HRESULT hr;
        TRACE("Calling pConnect=%p (slot 6) on connector=%p, id=%d, iface=%p (%s)\n",
              pConnect, connector, connection_id, iface, debugstr_w(name));
        hr = pConnect(connector, connection_id, iface);
        TRACE("Connect ID %d (%s) returned hr=0x%08lx\n", connection_id, debugstr_w(name), hr);
    }
}

static void connect_control(void *connector, int connection_id, struct xaml_element *elem)
{
    void *iface;
    if (!elem) return;
    if (elem->type == CONTROL_TYPE_ROWDEF)
        iface = &elem->IRowDefinition_iface;
    else if (elem->type == CONTROL_TYPE_COLDEF)
        iface = &elem->IColumnDefinition_iface;
    else if (elem->type == CONTROL_TYPE_MENUFLYOUTITEM)
        iface = &elem->IMenuFlyoutItem_iface;
    else if (elem->type == CONTROL_TYPE_MENUFLYOUT)
        iface = &elem->IMenuFlyout_iface;
    else if (elem->type == CONTROL_TYPE_FLYOUT)
        iface = &elem->IFlyout_iface;
    else if (elem->type == CONTROL_TYPE_GRID)
        iface = &elem->IGrid_iface;
    else if (elem->type == CONTROL_TYPE_ADAPTIVETRIGGER)
        iface = &elem->IAdaptiveTrigger_iface;
    else
        iface = &elem->IUIElement_iface;

    connect_control_iface(connector, connection_id, iface, elem->name);
}

static struct xaml_element *xaml_element_from_object(void *obj)
{
    HRESULT (WINAPI *pQI)(void *, REFIID, void **);
    void *ui_iface = NULL;
    void **vtbl;
    if (!obj) return NULL;
    vtbl = *(void ***)obj;
    if (vtbl && (ULONG_PTR)vtbl[0] > 0x10000)
    {
        if (vtbl == (void **)elem_ui_vtbl)
            return impl_from_IUIElement(obj);
        if (vtbl == (void **)elem_uc_vtbl)
            return impl_from_IUserControl(obj);
        if (vtbl == (void **)elem_ctrl_vtbl)
            return impl_from_IControl(obj);
        if (vtbl == (void **)elem_grid_vtbl)
            return impl_from_IGrid(obj);
        if (vtbl == (void **)elem_btn_vtbl)
            return impl_from_IButton(obj);
        if (vtbl == (void **)elem_btnbase_vtbl)
            return impl_from_IButtonBase(obj);
        if (vtbl == (void **)elem_tb_vtbl)
            return impl_from_ITextBlock(obj);
        if (vtbl == (void **)elem_border_vtbl)
            return impl_from_IBorder(obj);

        pQI = (void *)vtbl[0];
        if (SUCCEEDED(pQI(obj, &IID_IUIElement, &ui_iface)) && ui_iface)
        {
            struct xaml_element *elem = impl_from_IUIElement(ui_iface);
            elem_Release_internal(elem);
            return elem;
        }
    }
    return NULL;
}

HRESULT xaml_load_component(void *component, void *locator)
{
    void *connector = NULL;
    void **comp_vtbl;
    WCHAR uri_buf[256];

    init_elem_vtables();
    TRACE("xaml_load_component component=%p, locator=%p\n", component, locator);
    if (!component) return S_OK;

    comp_vtbl = *(void ***)component;
    if (comp_vtbl && (ULONG_PTR)comp_vtbl[0] > 0x10000)
    {
        HRESULT (WINAPI *pQI)(void *, REFIID, void **) = (void *)comp_vtbl[0];
        pQI(component, &IID_IComponentConnector, &connector);
    }
    TRACE("IComponentConnector=%p\n", connector);

    uri_buf[0] = 0;
    if (locator && (ULONG_PTR)locator > 0x10000)
    {
        void **loc_vtbl = *(void ***)locator;
        if (loc_vtbl && (ULONG_PTR)loc_vtbl[0] > 0x10000)
        {
            void *uri_class = NULL;
            HRESULT (WINAPI *pQI)(void *, REFIID, void **) = (void *)loc_vtbl[0];
            if (SUCCEEDED(pQI(locator, &IID_IUriRuntimeClass, &uri_class)) && uri_class)
            {
                void **uri_vtbl = *(void ***)uri_class;
                HRESULT (WINAPI *pGetAbsoluteUri)(void *, HSTRING *) = (void *)uri_vtbl[6];
                HSTRING hstr = NULL;
                if (SUCCEEDED(pGetAbsoluteUri(uri_class, &hstr)) && hstr)
                {
                    const WCHAR *raw = WindowsGetStringRawBuffer(hstr, NULL);
                    if (raw) lstrcpynW(uri_buf, raw, ARRAY_SIZE(uri_buf));
                    WindowsDeleteString(hstr);
                }
            }
        }
    }
    TRACE("Loading component URI: %s\n", debugstr_w(uri_buf));

    /* -------------------------------------------------------------
     * 1. MainPage.xaml
     * ------------------------------------------------------------- */
    if (!uri_buf[0] || wcsstr(uri_buf, L"MainPage.xaml"))
    {
        struct xaml_element *root_grid, *titlebar, *navview, *calc_holder, *header, *copy_btn, *paste_btn, *page_elem;
        void *tb_inst = NULL;
        void *nv_inst = NULL;

        root_grid = xaml_element_create(CONTROL_TYPE_GRID, L"MainPageRootGrid");
        root_grid->row_count = 2;
        root_grid->col_count = 1;
        root_grid->row_heights[0] = 40.0; root_grid->row_unit_types[0] = 1; /* 40px fixed */
        root_grid->row_heights[1] = 1.0;  root_grid->row_unit_types[1] = 2; /* 1* remainder */

        titlebar = xaml_element_create(CONTROL_TYPE_ELEMENT, L"AppTitleBar");
        lstrcpyW(titlebar->custom_class_name, L"CalculatorApp.TitleBar");
        titlebar->grid_row = 0;
        titlebar->height = 32.0;
        if (SUCCEEDED(xaml_create_instance_by_name(L"CalculatorApp.TitleBar", &tb_inst)) && tb_inst)
            connect_control_iface(connector, 15, tb_inst, L"AppTitleBar");
        else
            connect_control(connector, 15, titlebar);
        xaml_element_add_child(root_grid, titlebar);

        navview = xaml_element_create(CONTROL_TYPE_ELEMENT, L"NavView");
        lstrcpyW(navview->custom_class_name, L"Microsoft.UI.Xaml.Controls.NavigationView");
        navview->grid_row = 1;
        if (SUCCEEDED(xaml_create_instance_by_name(L"Microsoft.UI.Xaml.Controls.NavigationView", &nv_inst)) && nv_inst)
            connect_control_iface(connector, 16, nv_inst, L"NavView");
        else
            connect_control(connector, 16, navview);
        xaml_element_add_child(root_grid, navview);

        calc_holder = xaml_element_create(CONTROL_TYPE_BORDER, L"CalcHolder");
        calc_holder->grid_row = 1;
        connect_control(connector, 22, calc_holder);
        xaml_element_add_child(root_grid, calc_holder);

        header = xaml_element_create(CONTROL_TYPE_TEXTBLOCK, L"Header");
        lstrcpyW(header->text, L"Standard");
        header->font_size = 20.0;
        header->grid_row = 0;
        connect_control(connector, 20, header);
        xaml_element_add_child(root_grid, header);

        copy_btn = xaml_element_create(CONTROL_TYPE_BUTTON, L"CopyButton");
        connect_control(connector, 26, copy_btn);
        paste_btn = xaml_element_create(CONTROL_TYPE_BUTTON, L"PasteButton");
        connect_control(connector, 27, paste_btn);

        page_elem = xaml_element_create(CONTROL_TYPE_PAGE, L"PageRoot");
        lstrcpyW(page_elem->custom_class_name, L"Windows.UI.Xaml.Controls.Page");
        page_elem->child_content = root_grid;
        connect_control(connector, 1, page_elem);

        global_root_element = root_grid;
        global_page_element = page_elem;
        global_page_component = component;
    }
    /* -------------------------------------------------------------
     * 1b. TitleBar.xaml
     * ------------------------------------------------------------- */
    else if (wcsstr(uri_buf, L"TitleBar.xaml"))
    {
        struct xaml_element *layout_root, *bg_elem, *title_holder, *app_icon, *app_name, *exit_btn;
        struct xaml_element *tb_comp = xaml_element_from_object(component);

        layout_root = xaml_element_create(CONTROL_TYPE_GRID, L"LayoutRoot");
        bg_elem = xaml_element_create(CONTROL_TYPE_GRID, L"BackgroundElement");
        title_holder = xaml_element_create(CONTROL_TYPE_GRID, L"TitleHolder");
        app_icon = xaml_element_create(CONTROL_TYPE_IMAGE, L"AppIcon");
        app_name = xaml_element_create(CONTROL_TYPE_TEXTBLOCK, L"AppName");
        lstrcpyW(app_name->text, L"Calculator");
        exit_btn = xaml_element_create(CONTROL_TYPE_BUTTON, L"ExitAlwaysOnTopButton");

        connect_control_iface(connector, 1, component, L"TitleBarUserControl");
        connect_control(connector, 2, layout_root);
        connect_control(connector, 12, exit_btn);
        connect_control(connector, 13, bg_elem);
        connect_control(connector, 14, title_holder);
        connect_control(connector, 15, app_icon);
        connect_control(connector, 16, app_name);

        xaml_element_add_child(layout_root, bg_elem);
        xaml_element_add_child(layout_root, title_holder);
        xaml_element_add_child(title_holder, app_icon);
        xaml_element_add_child(title_holder, app_name);

        if (tb_comp) tb_comp->child_content = layout_root;
        if (global_root_element)
        {
            struct xaml_element *tb = xaml_find_element_by_name(global_root_element, L"AppTitleBar");
            if (tb) tb->child_content = tb_comp ? tb_comp : layout_root;
        }
    }
    /* -------------------------------------------------------------
     * 2. Calculator.xaml
     * ------------------------------------------------------------- */
    else if (wcsstr(uri_buf, L"Calculator.xaml"))
    {
        struct xaml_element *calc_grid, *expr_text, *results, *aot_results, *numpad_panel, *ops_panel;
        struct xaml_element *dock_panel, *dock_pivot, *dock_mem, *dock_hist, *hist_pivot, *mem_pivot;
        struct xaml_element *mem_panel, *mem_btn, *hist_btn, *mem_flyout, *hist_flyout;
        struct xaml_element *calc_elem = xaml_element_from_object(component);
        void *expr_inst = NULL;
        void *results_inst = NULL;
        void *aot_inst = NULL;
        void *ops_inst = NULL;

        /* ID 1: Root UserControl (component) */
        connect_control_iface(connector, 1, component, L"CalculatorUserControl");

        /* ID 5: CopyMenuItem, ID 6: PasteMenuItem */
        {
            void *flyout_val = NULL;
            if (SUCCEEDED(create_menu_flyout_instance(&flyout_val)) && cached_display_menu_flyout)
            {
                if (cached_display_menu_flyout->children_count >= 1 && cached_display_menu_flyout->children[0])
                {
                    connect_control(connector, 5, cached_display_menu_flyout->children[0]);
                }
                if (cached_display_menu_flyout->children_count >= 2 && cached_display_menu_flyout->children[1])
                {
                    connect_control(connector, 6, cached_display_menu_flyout->children[1]);
                }
                if (flyout_val) IUnknown_Release((IUnknown *)flyout_val);
            }
        }

        /* ID 31: ColumnMain, ID 32: ColumnHistory */
        {
            struct xaml_element *col_main = xaml_element_create(CONTROL_TYPE_COLDEF, L"ColumnMain");
            struct xaml_element *col_hist = xaml_element_create(CONTROL_TYPE_COLDEF, L"ColumnHistory");
            connect_control(connector, 31, col_main);
            connect_control(connector, 32, col_hist);
        }

        /* RowDefinitions: ID 137..142 */
        {
            struct xaml_element *r;
            r = xaml_element_create(CONTROL_TYPE_ROWDEF, L"RowHamburger");
            connect_control(connector, 137, r);
            r = xaml_element_create(CONTROL_TYPE_ROWDEF, L"RowExpression");
            connect_control(connector, 138, r);
            r = xaml_element_create(CONTROL_TYPE_ROWDEF, L"RowResult");
            connect_control(connector, 139, r);
            r = xaml_element_create(CONTROL_TYPE_ROWDEF, L"RowDisplayControls");
            connect_control(connector, 140, r);
            r = xaml_element_create(CONTROL_TYPE_ROWDEF, L"RowMemoryControls");
            connect_control(connector, 141, r);
            r = xaml_element_create(CONTROL_TYPE_ROWDEF, L"RowNumPad");
            connect_control(connector, 142, r);
        }

        /* ID 40: ResultsMVisualStateTrigger */
        {
            struct xaml_element *trig = xaml_element_create(CONTROL_TYPE_ADAPTIVETRIGGER, L"ResultsMVisualStateTrigger");
            connect_control(connector, 40, trig);
        }

        /* Inner Grid layout container */
        calc_grid = xaml_element_create(CONTROL_TYPE_GRID, L"CalculatorGrid");
        calc_grid->row_count = 6;
        calc_grid->col_count = 1;
        calc_grid->row_heights[0] = 0;   calc_grid->row_unit_types[0] = 1; /* 0px */
        calc_grid->row_heights[1] = 28;  calc_grid->row_unit_types[1] = 2; /* 28* Expression */
        calc_grid->row_heights[2] = 64;  calc_grid->row_unit_types[2] = 2; /* 64* Results */
        calc_grid->row_heights[3] = 0;   calc_grid->row_unit_types[3] = 1; /* 0px */
        calc_grid->row_heights[4] = 0;   calc_grid->row_unit_types[4] = 1; /* 0px */
        calc_grid->row_heights[5] = 310; calc_grid->row_unit_types[5] = 2; /* 310* Numpad */

        /* ID 66: DockPanel */
        dock_panel = xaml_element_create(CONTROL_TYPE_BORDER, L"DockPanel");
        connect_control(connector, 66, dock_panel);

        /* ID 116: DockPivot */
        dock_pivot = xaml_element_create(CONTROL_TYPE_PIVOT, L"DockPivot");
        connect_control(connector, 116, dock_pivot);

        /* ID 131: HistoryPivotItem, ID 132: MemoryPivotItem */
        hist_pivot = xaml_element_create(CONTROL_TYPE_PIVOTITEM, L"HistoryPivotItem");
        connect_control(connector, 131, hist_pivot);
        mem_pivot = xaml_element_create(CONTROL_TYPE_PIVOTITEM, L"MemoryPivotItem");
        connect_control(connector, 132, mem_pivot);

        /* ID 134: DockMemoryHolder, ID 136: DockHistoryHolder */
        dock_mem = xaml_element_create(CONTROL_TYPE_BORDER, L"DockMemoryHolder");
        connect_control(connector, 134, dock_mem);
        dock_hist = xaml_element_create(CONTROL_TYPE_BORDER, L"DockHistoryHolder");
        connect_control(connector, 136, dock_hist);

        /* ID 143: ExpressionText */
        expr_text = xaml_element_create(CONTROL_TYPE_TEXTBLOCK, L"ExpressionText");
        lstrcpyW(expr_text->custom_class_name, L"CalculatorApp.Controls.OverflowTextBlock");
        expr_text->grid_row = 1;
        expr_text->font_size = 14.0;
        expr_text->foreground_color = RGB(160, 160, 160);
        if (SUCCEEDED(xaml_create_instance_by_name(L"CalculatorApp.Controls.OverflowTextBlock", &expr_inst)) && expr_inst)
            connect_control_iface(connector, 143, expr_inst, L"ExpressionText");
        else
            connect_control(connector, 143, expr_text);
        xaml_element_add_child(calc_grid, expr_text);

        /* ID 144: Results */
        results = xaml_element_create(CONTROL_TYPE_TEXTBLOCK, L"Results");
        lstrcpyW(results->custom_class_name, L"CalculatorApp.Controls.CalculationResult");
        results->grid_row = 2;
        results->font_size = 46.0;
        lstrcpyW(results->text, L"0");
        if (SUCCEEDED(xaml_create_instance_by_name(L"CalculatorApp.Controls.CalculationResult", &results_inst)) && results_inst)
            connect_control_iface(connector, 144, results_inst, L"Results");
        else
            connect_control(connector, 144, results);
        xaml_element_add_child(calc_grid, results);

        /* ID 145: AlwaysOnTopResults */
        aot_results = xaml_element_create(CONTROL_TYPE_TEXTBLOCK, L"AlwaysOnTopResults");
        lstrcpyW(aot_results->custom_class_name, L"CalculatorApp.Controls.OverflowTextBlock");
        aot_results->grid_row = 2;
        if (SUCCEEDED(xaml_create_instance_by_name(L"CalculatorApp.Controls.OverflowTextBlock", &aot_inst)) && aot_inst)
            connect_control_iface(connector, 145, aot_inst, L"AlwaysOnTopResults");
        else
            connect_control(connector, 145, aot_results);

        /* ID 149: MemoryPanel */
        mem_panel = xaml_element_create(CONTROL_TYPE_GRID, L"MemoryPanel");
        connect_control(connector, 149, mem_panel);

        /* ID 150: NumpadPanel */
        numpad_panel = xaml_element_create(CONTROL_TYPE_GRID, L"NumpadPanel");
        numpad_panel->grid_row = 5;
        connect_control(connector, 150, numpad_panel);
        xaml_element_add_child(calc_grid, numpad_panel);

        /* ID 151: OpsPanel */
        ops_panel = xaml_element_create(CONTROL_TYPE_USERCONTROL, L"OpsPanel");
        lstrcpyW(ops_panel->custom_class_name, L"CalculatorApp.OperatorsPanel");
        if (SUCCEEDED(xaml_create_instance_by_name(L"CalculatorApp.OperatorsPanel", &ops_inst)) && ops_inst)
        {
            struct xaml_element *ops_e = xaml_element_from_object(ops_inst);
            connect_control_iface(connector, 151, ops_inst, L"OpsPanel");
            if (ops_e) xaml_element_add_child(numpad_panel, ops_e);
            else xaml_element_add_child(numpad_panel, ops_panel);
        }
        else
        {
            connect_control(connector, 151, ops_panel);
            xaml_element_add_child(numpad_panel, ops_panel);
        }

        /* ColumnDefinitions M4..M6: 152..154 */
        {
            struct xaml_element *c;
            c = xaml_element_create(CONTROL_TYPE_COLDEF, L"M4");
            connect_control(connector, 152, c);
            c = xaml_element_create(CONTROL_TYPE_COLDEF, L"M5");
            connect_control(connector, 153, c);
            c = xaml_element_create(CONTROL_TYPE_COLDEF, L"M6");
            connect_control(connector, 154, c);
        }

        /* Memory Buttons: 155..159 */
        {
            #define ADD_MEM_BTN(id, name_str) do { \
                struct xaml_element *b = xaml_element_create(CONTROL_TYPE_BUTTON, name_str); \
                void *b_inst = NULL; \
                lstrcpyW(b->custom_class_name, L"CalculatorApp.Controls.CalculatorButton"); \
                if (SUCCEEDED(xaml_create_instance_by_name(L"CalculatorApp.Controls.CalculatorButton", &b_inst)) && b_inst) \
                    connect_control_iface(connector, id, b_inst, name_str); \
                else \
                    connect_control(connector, id, b); \
            } while (0)

            ADD_MEM_BTN(155, L"ClearMemoryButton");
            ADD_MEM_BTN(156, L"MemRecall");
            ADD_MEM_BTN(157, L"MemPlus");
            ADD_MEM_BTN(158, L"MemMinus");
            ADD_MEM_BTN(159, L"MemButton");

            #undef ADD_MEM_BTN
        }

        /* ID 160: MemoryButton */
        mem_btn = xaml_element_create(CONTROL_TYPE_BUTTON, L"MemoryButton");
        connect_control(connector, 160, mem_btn);

        /* ID 161: MemoryFlyout */
        mem_flyout = xaml_element_create(CONTROL_TYPE_FLYOUT, L"MemoryFlyout");
        connect_control(connector, 161, mem_flyout);

        /* ID 162: HistoryButton */
        hist_btn = xaml_element_create(CONTROL_TYPE_BUTTON, L"HistoryButton");
        connect_control(connector, 162, hist_btn);

        /* ID 163: HistoryFlyout */
        hist_flyout = xaml_element_create(CONTROL_TYPE_FLYOUT, L"HistoryFlyout");
        connect_control(connector, 163, hist_flyout);

        if (calc_elem)
        {
            calc_elem->child_content = calc_grid;
            lstrcpyW(calc_elem->name, L"Calculator");
        }

        if (global_root_element)
        {
            struct xaml_element *holder = xaml_find_element_by_name(global_root_element, L"CalcHolder");
            if (holder) holder->child_content = calc_elem ? calc_elem : calc_grid;
        }
    }
    /* -------------------------------------------------------------
     * 3. OperatorsPanel.xaml
     * ------------------------------------------------------------- */
    else if (wcsstr(uri_buf, L"OperatorsPanel.xaml"))
    {
        struct xaml_element *ops_root;
        struct xaml_element *ops_comp = xaml_element_from_object(component);
        void *std_ops_inst = NULL;

        connect_control_iface(connector, 1, component, L"OperatorsPanelUserControl");

        ops_root = xaml_element_create(CONTROL_TYPE_GRID, L"OperatorsPanelGrid");

        if (SUCCEEDED(xaml_create_instance_by_name(L"CalculatorApp.CalculatorStandardOperators", &std_ops_inst)) && std_ops_inst)
        {
            struct xaml_element *std_elem = xaml_element_from_object(std_ops_inst);
            connect_control_iface(connector, 2, std_ops_inst, L"StandardOperators");
            if (std_elem) ops_root->child_content = std_elem;
        }
        else
            connect_control(connector, 2, ops_root);

        if (ops_comp) ops_comp->child_content = ops_root;

        if (global_root_element)
        {
            struct xaml_element *ops_p = xaml_find_element_by_name(global_root_element, L"OpsPanel");
            if (ops_p) ops_p->child_content = ops_comp ? ops_comp : ops_root;
        }
    }
    /* -------------------------------------------------------------
     * 4. CalculatorStandardOperators.xaml
     * ------------------------------------------------------------- */
    else if (wcsstr(uri_buf, L"CalculatorStandardOperators.xaml"))
    {
        struct xaml_element *ops_grid, *numpad_host;
        struct xaml_element *std_comp = xaml_element_from_object(component);
        void *np_inst = NULL;

        connect_control_iface(connector, 1, component, L"StandardOperatorsUserControl");

        ops_grid = xaml_element_create(CONTROL_TYPE_GRID, L"StandardOperatorsGrid");
        ops_grid->row_count = 6;
        ops_grid->col_count = 4;
        connect_control(connector, 2, ops_grid);

        #define ADD_BTN(id, name_str, row_idx, col_idx, text_str) do { \
            struct xaml_element *b = xaml_element_create(CONTROL_TYPE_BUTTON, name_str); \
            void *b_inst = NULL; \
            lstrcpyW(b->custom_class_name, L"CalculatorApp.Controls.CalculatorButton"); \
            b->grid_row = row_idx; b->grid_column = col_idx; \
            lstrcpyW(b->text, text_str); \
            if (SUCCEEDED(xaml_create_instance_by_name(L"CalculatorApp.Controls.CalculatorButton", &b_inst)) && b_inst) \
            { \
                struct xaml_element *be = xaml_element_from_object(b_inst); \
                if (be) { \
                    lstrcpyW(be->name, name_str); \
                    lstrcpyW(be->text, text_str); \
                    be->grid_row = row_idx; be->grid_column = col_idx; \
                    connect_control_iface(connector, id, b_inst, name_str); \
                    xaml_element_add_child(ops_grid, be); \
                } else { \
                    connect_control_iface(connector, id, b_inst, name_str); \
                    xaml_element_add_child(ops_grid, b); \
                } \
            } \
            else { \
                connect_control(connector, id, b); \
                xaml_element_add_child(ops_grid, b); \
            } \
        } while (0)

        ADD_BTN(48, L"PercentButton",    0, 0, L"%");
        ADD_BTN(49, L"ClearEntryButton", 0, 1, L"CE");
        ADD_BTN(50, L"ClearButton",      0, 2, L"C");
        ADD_BTN(51, L"BackSpaceButton",  0, 3, L"⌫");

        ADD_BTN(44, L"InvertButton",     1, 0, L"1/x");
        ADD_BTN(45, L"XPower2Button",    1, 1, L"x²");
        ADD_BTN(46, L"SquareRootButton", 1, 2, L"√x");
        ADD_BTN(32, L"DivideButton",     1, 3, L"÷");

        ADD_BTN(33, L"MultiplyButton",   2, 3, L"×");
        ADD_BTN(34, L"MinusButton",      3, 3, L"-");
        ADD_BTN(35, L"PlusButton",       4, 3, L"+");
        ADD_BTN(36, L"EqualButton",      5, 3, L"=");

        ADD_BTN(31, L"NegateButton",     5, 0, L"±");

        #undef ADD_BTN

        numpad_host = xaml_element_create(CONTROL_TYPE_USERCONTROL, L"NumberPad");
        lstrcpyW(numpad_host->custom_class_name, L"CalculatorApp.NumberPad");
        numpad_host->grid_row = 2;
        numpad_host->grid_column = 0;
        numpad_host->grid_rowspan = 4;
        numpad_host->grid_colspan = 3;
        if (SUCCEEDED(xaml_create_instance_by_name(L"CalculatorApp.NumberPad", &np_inst)) && np_inst)
        {
            struct xaml_element *np_elem = xaml_element_from_object(np_inst);
            connect_control_iface(connector, 30, np_inst, L"NumberPad");
            if (np_elem)
            {
                np_elem->grid_row = 2;
                np_elem->grid_column = 0;
                np_elem->grid_rowspan = 4;
                np_elem->grid_colspan = 3;
                xaml_element_add_child(ops_grid, np_elem);
            }
            else
            {
                xaml_element_add_child(ops_grid, numpad_host);
            }
        }
        else
        {
            connect_control(connector, 30, numpad_host);
            xaml_element_add_child(ops_grid, numpad_host);
        }

        if (std_comp) std_comp->child_content = ops_grid;

        if (global_root_element)
        {
            struct xaml_element *ops_p = xaml_find_element_by_name(global_root_element, L"OpsPanel");
            if (ops_p) ops_p->child_content = std_comp ? std_comp : ops_grid;
        }
    }
    /* -------------------------------------------------------------
     * 5. NumberPad.xaml
     * ------------------------------------------------------------- */
    else if (wcsstr(uri_buf, L"NumberPad.xaml"))
    {
        struct xaml_element *np_grid;
        struct xaml_element *np_comp = xaml_element_from_object(component);

        connect_control_iface(connector, 1, component, L"NumberPadUserControl");

        np_grid = xaml_element_create(CONTROL_TYPE_GRID, L"NumberPadGrid");
        np_grid->row_count = 4;
        np_grid->col_count = 3;
        connect_control(connector, 2, np_grid);

        #define ADD_NUM(id, name_str, row_idx, col_idx, text_str) do { \
            struct xaml_element *b = xaml_element_create(CONTROL_TYPE_BUTTON, name_str); \
            void *b_inst = NULL; \
            lstrcpyW(b->custom_class_name, L"CalculatorApp.Controls.CalculatorButton"); \
            b->grid_row = row_idx; b->grid_column = col_idx; \
            lstrcpyW(b->text, text_str); \
            if (SUCCEEDED(xaml_create_instance_by_name(L"CalculatorApp.Controls.CalculatorButton", &b_inst)) && b_inst) \
            { \
                struct xaml_element *be = xaml_element_from_object(b_inst); \
                if (be) { \
                    lstrcpyW(be->name, name_str); \
                    lstrcpyW(be->text, text_str); \
                    be->grid_row = row_idx; be->grid_column = col_idx; \
                    connect_control_iface(connector, id, b_inst, name_str); \
                    xaml_element_add_child(np_grid, be); \
                } else { \
                    connect_control_iface(connector, id, b_inst, name_str); \
                    xaml_element_add_child(np_grid, b); \
                } \
            } \
            else { \
                connect_control(connector, id, b); \
                xaml_element_add_child(np_grid, b); \
            } \
        } while (0)

        ADD_NUM(13, L"Num7Button", 0, 0, L"7");
        ADD_NUM(14, L"Num8Button", 0, 1, L"8");
        ADD_NUM(15, L"Num9Button", 0, 2, L"9");

        ADD_NUM(10, L"Num4Button", 1, 0, L"4");
        ADD_NUM(11, L"Num5Button", 1, 1, L"5");
        ADD_NUM(12, L"Num6Button", 1, 2, L"6");

        ADD_NUM(7,  L"Num1Button", 2, 0, L"1");
        ADD_NUM(8,  L"Num2Button", 2, 1, L"2");
        ADD_NUM(9,  L"Num3Button", 2, 2, L"3");

        ADD_NUM(6,  L"Num0Button", 3, 1, L"0");
        ADD_NUM(16, L"DecimalSeparatorButton", 3, 2, L",");

        #undef ADD_NUM

        if (np_comp) np_comp->child_content = np_grid;

        if (global_root_element)
        {
            struct xaml_element *np_p = xaml_find_element_by_name(global_root_element, L"NumberPad");
            if (np_p) np_p->child_content = np_comp ? np_comp : np_grid;
        }
    }

    return S_OK;
}
