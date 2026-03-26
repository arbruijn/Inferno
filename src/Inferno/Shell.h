#pragma once
#include "Types.h"
#ifdef __MINGW32__
#define NOMINMAX
#include <windows.h>
#endif

namespace Inferno {
    class Application;

    static auto WindowClass = L"InfernoWindowClass";

    class Shell {
#ifdef __MINGW32__
        HMODULE _hInstance = GetModuleHandle(nullptr);
#else
        void* _hInstance = nullptr;
#endif
    public:
        Shell() = default;
        ~Shell();
        Shell(const Shell&) = delete;
        Shell(Shell&&) = delete;
        Shell& operator=(const Shell&) = delete;
        Shell& operator=(Shell&&) = delete;

        int Show(uint2 position, uint2 size, int nCmdShow = 3) const;

#ifdef __MINGW32__
        inline static HWND Hwnd = nullptr;
#else
        inline static void* Hwnd = nullptr;
#endif
        inline static float DpiScale = 1;
        inline static bool HasFocus = true;

        static void UpdateWindowTitle(string_view message = {});
        static void Quit();
    };

    void UpdateFullscreen();
}

