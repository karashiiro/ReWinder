#include <Windows.h>
#include <iostream>
#include <string>

LRESULT HandleWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            FillRect(hdc, &ps.rcPaint, reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1));
            EndPaint(hWnd, &ps);
        }
            return 0;
        case WM_CLOSE:
            DestroyWindow(hWnd);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

BOOL HandleMonitor(HMONITOR, HDC, LPRECT pRect, LPARAM param)
{
    const auto &rect = *pRect;
    int *monitor = reinterpret_cast<int *>(param);
    std::cout << "Monitor " << std::to_string(*monitor) << ": (" << std::to_string(rect.top) << ", "
              << std::to_string(rect.left) << ", " << std::to_string(rect.bottom) << ", "
              << std::to_string(rect.right) << ")" << std::endl;
    *monitor++;
    return TRUE;
}

BOOL CALLBACK HandleWindow(HWND hWnd, LPARAM)
{
    auto len = GetWindowTextLength(hWnd);
    auto title = static_cast<PSTR>(VirtualAlloc(nullptr, len + 1, MEM_COMMIT, PAGE_READWRITE));
    if (!SUCCEEDED(GetWindowText(hWnd, title, len + 1)))
    {
        std::cout << "Error! Code: " << GetLastError() << std::endl;
        VirtualFree(title, len + 1, MEM_DECOMMIT);
        return FALSE;
    }

    RECT rect{};
    if (!SUCCEEDED(GetWindowRect(hWnd, &rect)))
    {
        std::cout << GetLastError() << std::endl;
        VirtualFree(title, len + 1, MEM_DECOMMIT);
        return FALSE;
    }

    if (rect.top == rect.bottom && rect.left == rect.right)
    {
        // Window has no size, so we don't care about it
        VirtualFree(title, len + 1, MEM_DECOMMIT);
        return TRUE;
    }

    std::cout << (len == 0 ? "(No Window Title)" : title) << ": (" << std::to_string(rect.top) << ", "
        << std::to_string(rect.left) << ", " << std::to_string(rect.bottom) << ", "
        << std::to_string(rect.right) << ")" << std::endl;

    VirtualFree(title, len + 1, MEM_DECOMMIT);
    return TRUE;
}

int WinMain(HINSTANCE hInst, HINSTANCE, PSTR, int nCmdShow)
{
    const char *CLASS_NAME = "ReWinder";
    WNDCLASS wc{};
    wc.lpfnWndProc = &HandleWindowMessage;
    wc.hInstance = hInst;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    auto hWnd = CreateWindowEx(0, CLASS_NAME, "ReWinder", WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                               nullptr, nullptr, hInst, nullptr);
    if (hWnd == nullptr)
    {
        return static_cast<int>(GetLastError());
    }

    ShowWindow(hWnd, nCmdShow);

    MSG msg{};
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    auto monitor = 0;
    EnumDisplayMonitors(nullptr, nullptr, &HandleMonitor, reinterpret_cast<LPARAM>(&monitor));
    EnumWindows(&HandleWindow, 0);
}
