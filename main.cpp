#include <Windows.h>
#include <iostream>
#include <string>

BOOL CALLBACK HandleWindow(HWND hwnd, LPARAM)
{
    auto len = GetWindowTextLength(hwnd);
    auto title = static_cast<PSTR>(VirtualAlloc(nullptr, len + 1, MEM_COMMIT, PAGE_READWRITE));
    if (!SUCCEEDED(GetWindowText(hwnd, title, len + 1)))
    {
        std::cout << "Error! Code: " << GetLastError() << std::endl;
        VirtualFree(title, len + 1, MEM_DECOMMIT);
        return TRUE;
    }

    RECT rect{};
    if (!SUCCEEDED(GetWindowRect(hwnd, &rect)))
    {
        std::cout << GetLastError() << std::endl;
        VirtualFree(title, len + 1, MEM_DECOMMIT);
        return TRUE;
    }

    if (rect.top == 0 && rect.left == 0 && rect.bottom == 0 && rect.right == 0)
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

int main()
{
    EnumWindows(&HandleWindow, 0);
    return 0;
}
