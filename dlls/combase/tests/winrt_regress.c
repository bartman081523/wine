#define COBJMACROS
#include <windows.h>
#include <initguid.h>
#include <roapi.h>
#include <winstring.h>
#include <stdio.h>

DEFINE_GUID(IID_IMessageWebSocket, 0x33727d08, 0x34d5, 0x4746, 0xad, 0x7b, 0x8d, 0xde, 0x5b, 0xc2, 0xef, 0x88);
DEFINE_GUID(IID_IWebSocket, 0xf877396f, 0x99b1, 0x4e18, 0xbc, 0x08, 0x85, 0x0c, 0x9a, 0xdf, 0x15, 0x6e);
DEFINE_GUID(IID_IClosable, 0x30d5a829, 0x7fa4, 0x4026, 0x83, 0xbb, 0xd7, 0x5b, 0xae, 0x4e, 0xa9, 0x9e);
DEFINE_GUID(IID_IWebSocketControl, 0x2ec4bdc3, 0xd9a5, 0x455a, 0x98, 0x11, 0xde, 0x24, 0xd4, 0x53, 0x37, 0xe9);
DEFINE_GUID(IID_IVector_HSTRING, 0x2af22683, 0x3734, 0x56d0, 0xa6, 0x0e, 0x68, 0x8c, 0xc8, 0x5d, 0x16, 0x19);
DEFINE_GUID(IID_IMouseDeviceStatics, 0x484a9045, 0x6d70, 0x49db, 0x8e, 0x68, 0x46, 0xff, 0xbd, 0x17, 0xd3, 0x8d);
DEFINE_GUID(IID_IMouseDevice, 0x79b9d5f2, 0x879e, 0x4b89, 0xb7, 0x98, 0x79, 0xe4, 0x75, 0x98, 0x03, 0x0c);
DEFINE_GUID(IID_ICoreWindowStatic, 0x4d239005, 0x3c2a, 0x41b1, 0x90, 0x22, 0x53, 0x6b, 0xb9, 0xcf, 0x93, 0xb1);
DEFINE_GUID(IID_ICoreWindow, 0x79bbf6e8, 0x8891, 0x4b66, 0x8e, 0x17, 0x25, 0x0c, 0xa6, 0x81, 0x4b, 0x72);
DEFINE_GUID(IID_ICoreWindowInterop, 0x45d64a29, 0xa63e, 0x4cb6, 0xb4, 0x98, 0x57, 0x81, 0xd2, 0x98, 0xcb, 0x4f);
DEFINE_GUID(IID_ICurrentAppStatics, 0xd5266585, 0x303d, 0x4ecd, 0x8f, 0xac, 0x64, 0xd8, 0xf0, 0xc8, 0x99, 0x0a);

typedef struct ICoreWindowInteropVtbl
{
    HRESULT (WINAPI *QueryInterface)(void *This, REFIID riid, void **ppvObject);
    ULONG (WINAPI *AddRef)(void *This);
    ULONG (WINAPI *Release)(void *This);
    HRESULT (WINAPI *GetWindowHandle)(void *This, HWND *hwnd);
    HRESULT (WINAPI *put_MessageHandled)(void *This, boolean value);
} ICoreWindowInteropVtbl;

typedef struct ICoreWindowInteropTest
{
    const ICoreWindowInteropVtbl *lpVtbl;
} ICoreWindowInteropTest;

typedef struct ICoreWindowStaticVtbl
{
    HRESULT (WINAPI *QueryInterface)(void *This, REFIID riid, void **ppvObject);
    ULONG (WINAPI *AddRef)(void *This);
    ULONG (WINAPI *Release)(void *This);
    HRESULT (WINAPI *GetIids)(void *This, ULONG *iidCount, IID **iids);
    HRESULT (WINAPI *GetRuntimeClassName)(void *This, HSTRING *className);
    HRESULT (WINAPI *GetTrustLevel)(void *This, int *trustLevel);
    HRESULT (WINAPI *GetForCurrentThread)(void *This, void **window);
} ICoreWindowStaticVtbl;

typedef struct ICoreWindowStaticTest
{
    const ICoreWindowStaticVtbl *lpVtbl;
} ICoreWindowStaticTest;

typedef struct ICoreWindowTestVtbl
{
    HRESULT (WINAPI *QueryInterface)(void *This, REFIID riid, void **ppvObject);
    ULONG (WINAPI *AddRef)(void *This);
    ULONG (WINAPI *Release)(void *This);
    HRESULT (WINAPI *GetIids)(void *This, ULONG *iidCount, IID **iids);
    HRESULT (WINAPI *GetRuntimeClassName)(void *This, HSTRING *className);
    HRESULT (WINAPI *GetTrustLevel)(void *This, int *trustLevel);
    /* ICoreWindow slots */
    HRESULT (WINAPI *get_AutomationHostProvider)(void *This, void **value);
    HRESULT (WINAPI *get_Bounds)(void *This, void *value);
    HRESULT (WINAPI *get_CustomProperties)(void *This, void **value);
    HRESULT (WINAPI *get_Dispatcher)(void *This, void **value);
    HRESULT (WINAPI *get_FlowDirection)(void *This, int *value);
    HRESULT (WINAPI *put_FlowDirection)(void *This, int value);
    HRESULT (WINAPI *get_IsInputEnabled)(void *This, boolean *value);
    HRESULT (WINAPI *put_IsInputEnabled)(void *This, boolean value);
    HRESULT (WINAPI *get_PointerCursor)(void *This, void **value);
    HRESULT (WINAPI *put_PointerCursor)(void *This, void *value);
    HRESULT (WINAPI *get_PointerPosition)(void *This, void *value);
    HRESULT (WINAPI *get_Visible)(void *This, boolean *value);
    HRESULT (WINAPI *Activate)(void *This);
    HRESULT (WINAPI *Close)(void *This);
    HRESULT (WINAPI *GetAsyncKeyState)(void *This, int key, int *state);
    HRESULT (WINAPI *GetKeyState)(void *This, int key, int *state);
    HRESULT (WINAPI *ReleasePointerCapture)(void *This);
    HRESULT (WINAPI *SetPointerCapture)(void *This);
} ICoreWindowTestVtbl;

typedef struct ICoreWindowTest
{
    const ICoreWindowTestVtbl *lpVtbl;
} ICoreWindowTest;

int main(void)
{
    HRESULT hr;
    HSTRING str;
    IActivationFactory *factory = NULL;
    IInspectable *instance = NULL;
    IUnknown *unk = NULL;
    int fails = 0;

    printf("=== Starting WinRT Wine Regression Tests ===\n");

    hr = RoInitialize(RO_INIT_MULTITHREADED);
    if (FAILED(hr) && hr != S_FALSE)
    {
        printf("[FAIL] RoInitialize failed: %#lx\n", hr);
        return 1;
    }
    printf("[PASS] RoInitialize\n");

    /* 1. Test Windows.Networking.Sockets.MessageWebSocket */
    {
        const WCHAR ws_name[] = L"Windows.Networking.Sockets.MessageWebSocket";
        hr = WindowsCreateString(ws_name, wcslen(ws_name), &str);
        if (FAILED(hr)) { printf("[FAIL] WindowsCreateString ws: %#lx\n", hr); fails++; }

        hr = RoGetActivationFactory(str, &IID_IActivationFactory, (void **)&factory);
        if (FAILED(hr))
        {
            printf("[FAIL] RoGetActivationFactory MessageWebSocket: %#lx\n", hr);
            fails++;
        }
        else
        {
            printf("[PASS] RoGetActivationFactory(MessageWebSocket)\n");
            hr = IActivationFactory_ActivateInstance(factory, &instance);
            if (FAILED(hr) || !instance)
            {
                printf("[FAIL] ActivateInstance MessageWebSocket: %#lx\n", hr);
                fails++;
            }
            else
            {
                printf("[PASS] ActivateInstance(MessageWebSocket)\n");

                hr = IInspectable_QueryInterface(instance, &IID_IMessageWebSocket, (void **)&unk);
                if (SUCCEEDED(hr) && unk) { printf("[PASS] QI IMessageWebSocket\n"); IUnknown_Release(unk); }
                else { printf("[FAIL] QI IMessageWebSocket: %#lx\n", hr); fails++; }

                hr = IInspectable_QueryInterface(instance, &IID_IWebSocket, (void **)&unk);
                if (SUCCEEDED(hr) && unk) { printf("[PASS] QI IWebSocket\n"); IUnknown_Release(unk); }
                else { printf("[FAIL] QI IWebSocket: %#lx\n", hr); fails++; }

                hr = IInspectable_QueryInterface(instance, &IID_IClosable, (void **)&unk);
                if (SUCCEEDED(hr) && unk) { printf("[PASS] QI IClosable\n"); IUnknown_Release(unk); }
                else { printf("[FAIL] QI IClosable: %#lx\n", hr); fails++; }

                IInspectable_Release(instance);
            }
            IActivationFactory_Release(factory);
        }
        WindowsDeleteString(str);
    }

    /* 2. Test Windows.Devices.Input.MouseDevice */
    {
        const WCHAR mouse_name[] = L"Windows.Devices.Input.MouseDevice";
        hr = WindowsCreateString(mouse_name, wcslen(mouse_name), &str);
        if (FAILED(hr)) { printf("[FAIL] WindowsCreateString mouse: %#lx\n", hr); fails++; }

        factory = NULL;
        hr = RoGetActivationFactory(str, &IID_IActivationFactory, (void **)&factory);
        if (FAILED(hr))
        {
            printf("[FAIL] RoGetActivationFactory MouseDevice: %#lx\n", hr);
            fails++;
        }
        else
        {
            printf("[PASS] RoGetActivationFactory(MouseDevice)\n");

            unk = NULL;
            hr = IActivationFactory_QueryInterface(factory, &IID_IMouseDeviceStatics, (void **)&unk);
            if (SUCCEEDED(hr) && unk)
            {
                typedef HRESULT (WINAPI *p_GetForCurrentView)(void *, void **);
                p_GetForCurrentView get_view = *(p_GetForCurrentView *)(*(void ***)unk + 6);
                IUnknown *mouse_dev = NULL;
                printf("[PASS] QI IMouseDeviceStatics\n");

                hr = get_view(unk, (void **)&mouse_dev);
                if (SUCCEEDED(hr) && mouse_dev)
                {
                    printf("[PASS] IMouseDeviceStatics::GetForCurrentView\n");
                    IUnknown_Release(mouse_dev);
                }
                else
                {
                    printf("[FAIL] GetForCurrentView: %#lx\n", hr);
                    fails++;
                }
                IUnknown_Release(unk);
            }
            else
            {
                printf("[FAIL] QI IMouseDeviceStatics: %#lx\n", hr);
                fails++;
            }
            IActivationFactory_Release(factory);
        }
        WindowsDeleteString(str);
    }

    /* 3. Test Windows.UI.Core.CoreWindow Focus, HWND, and Cursor Invariants */
    {
        const WCHAR cw_name[] = L"Windows.UI.Core.CoreWindow";
        hr = WindowsCreateString(cw_name, wcslen(cw_name), &str);
        if (FAILED(hr)) { printf("[FAIL] WindowsCreateString CoreWindow: %#lx\n", hr); fails++; }

        factory = NULL;
        hr = RoGetActivationFactory(str, &IID_IActivationFactory, (void **)&factory);
        if (FAILED(hr))
        {
            printf("[FAIL] RoGetActivationFactory CoreWindow: %#lx\n", hr);
            fails++;
        }
        else
        {
            ICoreWindowStaticTest *cw_stat = NULL;
            printf("[PASS] RoGetActivationFactory(CoreWindow)\n");

            hr = IActivationFactory_QueryInterface(factory, &IID_ICoreWindowStatic, (void **)&cw_stat);
            if (SUCCEEDED(hr) && cw_stat)
            {
                ICoreWindowTest *core_win = NULL;
                printf("[PASS] QI ICoreWindowStatic\n");

                hr = cw_stat->lpVtbl->GetForCurrentThread(cw_stat, (void **)&core_win);
                if (SUCCEEDED(hr) && core_win)
                {
                    ICoreWindowInteropTest *interop = NULL;
                    HWND hwnd = NULL;
                    printf("[PASS] ICoreWindowStatic::GetForCurrentThread\n");

                    hr = core_win->lpVtbl->QueryInterface(core_win, &IID_ICoreWindowInterop, (void **)&interop);
                    if (SUCCEEDED(hr) && interop)
                    {
                        printf("[PASS] QI ICoreWindowInterop\n");
                        hr = interop->lpVtbl->GetWindowHandle(interop, &hwnd);
                        if (SUCCEEDED(hr) && hwnd && IsWindow(hwnd))
                        {
                            POINT p_before, p_after;
                            RECT clip_before, clip_after;
                            printf("[PASS] ICoreWindowInterop::GetWindowHandle (valid HWND %p)\n", hwnd);

                            /* Test A: Window activation must not freeze or force cursor centering */
                            core_win->lpVtbl->Activate(core_win);

                            /* Set cursor to a known distinct position */
                            SetCursorPos(150, 150);
                            GetCursorPos(&p_before);

                            /* Dispatch WM_MOUSEMOVE - must NOT invoke SetCursorPos centering loop */
                            SendMessageW(hwnd, WM_MOUSEMOVE, 0, MAKELPARAM(160, 160));
                            GetCursorPos(&p_after);

                            /* In the buggy implementation, SetCursorPos snapped back to window center (e.g. 640, 360) */
                            if (p_after.x == p_before.x && p_after.y == p_before.y)
                            {
                                printf("[PASS] WM_MOUSEMOVE does not violently hijack cursor position via SetCursorPos loop\n");
                            }
                            else
                            {
                                printf("[FAIL] WM_MOUSEMOVE hijacked cursor position! (%ld,%ld) -> (%ld,%ld)\n",
                                       p_before.x, p_before.y, p_after.x, p_after.y);
                                fails++;
                            }

                            /* Test B: Key press must not clamp ClipCursor to window client rect */
                            GetClipCursor(&clip_before);
                            SendMessageW(hwnd, WM_KEYDOWN, 'W', 0);
                            GetClipCursor(&clip_after);

                            /* In the buggy implementation, ClipCursor was forced to client rect on every WM_KEYDOWN */
                            if (EqualRect(&clip_before, &clip_after))
                            {
                                printf("[PASS] WM_KEYDOWN does not invasively override ClipCursor\n");
                            }
                            else
                            {
                                printf("[FAIL] WM_KEYDOWN forced ClipCursor: [%ld,%ld - %ld,%ld]\n",
                                       clip_after.left, clip_after.top, clip_after.right, clip_after.bottom);
                                fails++;
                            }

                            /* Test C: PointerCursor hiding */
                            hr = core_win->lpVtbl->put_PointerCursor(core_win, NULL);
                            if (SUCCEEDED(hr))
                            {
                                printf("[PASS] put_PointerCursor(NULL) succeeded\n");
                            }
                            else
                            {
                                printf("[FAIL] put_PointerCursor(NULL): %#lx\n", hr);
                                fails++;
                            }
                        }
                        else
                        {
                            printf("[FAIL] GetWindowHandle failed or invalid HWND: %#lx, hwnd=%p\n", hr, hwnd);
                            fails++;
                        }
                        interop->lpVtbl->Release(interop);
                    }
                    else
                    {
                        printf("[FAIL] QI ICoreWindowInterop: %#lx\n", hr);
                        fails++;
                    }
                    core_win->lpVtbl->Release(core_win);
                }
                else
                {
                    printf("[FAIL] GetForCurrentThread: %#lx\n", hr);
                    fails++;
                }
                cw_stat->lpVtbl->Release(cw_stat);
            }
            else
            {
                printf("[FAIL] QI ICoreWindowStatic: %#lx\n", hr);
                fails++;
            }
            IActivationFactory_Release(factory);
        }
        WindowsDeleteString(str);
    }

    RoUninitialize();

    if (fails == 0)
    {
        printf("=== ALL REGRESSION TESTS PASSED (0 failures) ===\n");
        return 0;
    }
    else
    {
        printf("=== TESTS FAILED: %d failures ===\n", fails);
        return 1;
    }
}
