// imgui_impl_sdl3.cpp - SDL3 platform backend for ImGui on Linux
#include "imgui.h"
#include "imgui_impl.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_mouse.h>
#include <chrono>

static SDL_Window* g_Window = nullptr;
static Uint64 g_Time = 0;
static Uint64 g_TicksPerSecond = 0;

bool ImGui_ImplSDL3_Init(SDL_Window* window) {
    g_Window = window;
    g_TicksPerSecond = SDL_GetPerformanceFrequency();
    g_Time = SDL_GetPerformanceCounter();

    ImGuiIO& io = ImGui::GetIO();
    io.BackendPlatformName = "imgui_impl_sdl3";
    io.SetClipboardTextFn = [](void*, const char* text) { SDL_SetClipboardText(text); };
    io.GetClipboardTextFn = [](void*) -> const char* { return SDL_GetClipboardText(); };
    return true;
}

void ImGui_ImplSDL3_Shutdown() {
    g_Window = nullptr;
}

static ImGuiKey SDLKeyToImGuiKey(SDL_Keycode key) {
    switch (key) {
        case SDLK_TAB: return ImGuiKey_Tab;
        case SDLK_LEFT: return ImGuiKey_LeftArrow;
        case SDLK_RIGHT: return ImGuiKey_RightArrow;
        case SDLK_UP: return ImGuiKey_UpArrow;
        case SDLK_DOWN: return ImGuiKey_DownArrow;
        case SDLK_PAGEUP: return ImGuiKey_PageUp;
        case SDLK_PAGEDOWN: return ImGuiKey_PageDown;
        case SDLK_HOME: return ImGuiKey_Home;
        case SDLK_END: return ImGuiKey_End;
        case SDLK_INSERT: return ImGuiKey_Insert;
        case SDLK_DELETE: return ImGuiKey_Delete;
        case SDLK_BACKSPACE: return ImGuiKey_Backspace;
        case SDLK_SPACE: return ImGuiKey_Space;
        case SDLK_RETURN: return ImGuiKey_Enter;
        case SDLK_ESCAPE: return ImGuiKey_Escape;
        case SDLK_APOSTROPHE: return ImGuiKey_Apostrophe;
        case SDLK_COMMA: return ImGuiKey_Comma;
        case SDLK_MINUS: return ImGuiKey_Minus;
        case SDLK_PERIOD: return ImGuiKey_Period;
        case SDLK_SLASH: return ImGuiKey_Slash;
        case SDLK_SEMICOLON: return ImGuiKey_Semicolon;
        case SDLK_EQUALS: return ImGuiKey_Equal;
        case SDLK_LEFTBRACKET: return ImGuiKey_LeftBracket;
        case SDLK_BACKSLASH: return ImGuiKey_Backslash;
        case SDLK_RIGHTBRACKET: return ImGuiKey_RightBracket;
        case SDLK_GRAVE: return ImGuiKey_GraveAccent;
        case SDLK_CAPSLOCK: return ImGuiKey_CapsLock;
        case SDLK_SCROLLLOCK: return ImGuiKey_ScrollLock;
        case SDLK_NUMLOCKCLEAR: return ImGuiKey_NumLock;
        case SDLK_PRINTSCREEN: return ImGuiKey_PrintScreen;
        case SDLK_PAUSE: return ImGuiKey_Pause;
        case SDLK_KP_0: return ImGuiKey_Keypad0;
        case SDLK_KP_1: return ImGuiKey_Keypad1;
        case SDLK_KP_2: return ImGuiKey_Keypad2;
        case SDLK_KP_3: return ImGuiKey_Keypad3;
        case SDLK_KP_4: return ImGuiKey_Keypad4;
        case SDLK_KP_5: return ImGuiKey_Keypad5;
        case SDLK_KP_6: return ImGuiKey_Keypad6;
        case SDLK_KP_7: return ImGuiKey_Keypad7;
        case SDLK_KP_8: return ImGuiKey_Keypad8;
        case SDLK_KP_9: return ImGuiKey_Keypad9;
        case SDLK_KP_PERIOD: return ImGuiKey_KeypadDecimal;
        case SDLK_KP_DIVIDE: return ImGuiKey_KeypadDivide;
        case SDLK_KP_MULTIPLY: return ImGuiKey_KeypadMultiply;
        case SDLK_KP_MINUS: return ImGuiKey_KeypadSubtract;
        case SDLK_KP_PLUS: return ImGuiKey_KeypadAdd;
        case SDLK_KP_ENTER: return ImGuiKey_KeypadEnter;
        case SDLK_KP_EQUALS: return ImGuiKey_KeypadEqual;
        case SDLK_LCTRL: return ImGuiKey_LeftCtrl;
        case SDLK_LSHIFT: return ImGuiKey_LeftShift;
        case SDLK_LALT: return ImGuiKey_LeftAlt;
        case SDLK_LGUI: return ImGuiKey_LeftSuper;
        case SDLK_RCTRL: return ImGuiKey_RightCtrl;
        case SDLK_RSHIFT: return ImGuiKey_RightShift;
        case SDLK_RALT: return ImGuiKey_RightAlt;
        case SDLK_RGUI: return ImGuiKey_RightSuper;
        case SDLK_APPLICATION: return ImGuiKey_Menu;
        case SDLK_0: return ImGuiKey_0;
        case SDLK_1: return ImGuiKey_1;
        case SDLK_2: return ImGuiKey_2;
        case SDLK_3: return ImGuiKey_3;
        case SDLK_4: return ImGuiKey_4;
        case SDLK_5: return ImGuiKey_5;
        case SDLK_6: return ImGuiKey_6;
        case SDLK_7: return ImGuiKey_7;
        case SDLK_8: return ImGuiKey_8;
        case SDLK_9: return ImGuiKey_9;
        case SDLK_A: return ImGuiKey_A;
        case SDLK_B: return ImGuiKey_B;
        case SDLK_C: return ImGuiKey_C;
        case SDLK_D: return ImGuiKey_D;
        case SDLK_E: return ImGuiKey_E;
        case SDLK_F: return ImGuiKey_F;
        case SDLK_G: return ImGuiKey_G;
        case SDLK_H: return ImGuiKey_H;
        case SDLK_I: return ImGuiKey_I;
        case SDLK_J: return ImGuiKey_J;
        case SDLK_K: return ImGuiKey_K;
        case SDLK_L: return ImGuiKey_L;
        case SDLK_M: return ImGuiKey_M;
        case SDLK_N: return ImGuiKey_N;
        case SDLK_O: return ImGuiKey_O;
        case SDLK_P: return ImGuiKey_P;
        case SDLK_Q: return ImGuiKey_Q;
        case SDLK_R: return ImGuiKey_R;
        case SDLK_S: return ImGuiKey_S;
        case SDLK_T: return ImGuiKey_T;
        case SDLK_U: return ImGuiKey_U;
        case SDLK_V: return ImGuiKey_V;
        case SDLK_W: return ImGuiKey_W;
        case SDLK_X: return ImGuiKey_X;
        case SDLK_Y: return ImGuiKey_Y;
        case SDLK_Z: return ImGuiKey_Z;
        case SDLK_F1: return ImGuiKey_F1;
        case SDLK_F2: return ImGuiKey_F2;
        case SDLK_F3: return ImGuiKey_F3;
        case SDLK_F4: return ImGuiKey_F4;
        case SDLK_F5: return ImGuiKey_F5;
        case SDLK_F6: return ImGuiKey_F6;
        case SDLK_F7: return ImGuiKey_F7;
        case SDLK_F8: return ImGuiKey_F8;
        case SDLK_F9: return ImGuiKey_F9;
        case SDLK_F10: return ImGuiKey_F10;
        case SDLK_F11: return ImGuiKey_F11;
        case SDLK_F12: return ImGuiKey_F12;
        default: return ImGuiKey_None;
    }
}

void ImGui_ImplSDL3_NewFrame() {
    ImGuiIO& io = ImGui::GetIO();

    int w, h;
    SDL_GetWindowSize(g_Window, &w, &h);
    io.DisplaySize = ImVec2((float)w, (float)h);

    Uint64 currentTime = SDL_GetPerformanceCounter();
    io.DeltaTime = (float)(currentTime - g_Time) / (float)g_TicksPerSecond;
    if (io.DeltaTime <= 0.0f) io.DeltaTime = 1.0f / 60.0f;
    g_Time = currentTime;
}

bool ImGui_ImplSDL3_ProcessEvent(const SDL_Event* event) {
    ImGuiIO& io = ImGui::GetIO();
    switch (event->type) {
        case SDL_EVENT_MOUSE_MOTION:
            io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
            io.AddMousePosEvent((float)event->motion.x, (float)event->motion.y);
            return true;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP: {
            int button = -1;
            if (event->button.button == SDL_BUTTON_LEFT) button = 0;
            else if (event->button.button == SDL_BUTTON_RIGHT) button = 1;
            else if (event->button.button == SDL_BUTTON_MIDDLE) button = 2;
            if (button != -1) {
                io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
                io.AddMouseButtonEvent(button, event->type == SDL_EVENT_MOUSE_BUTTON_DOWN);
            }
            return true;
        }
        case SDL_EVENT_MOUSE_WHEEL:
            io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
            io.AddMouseWheelEvent(event->wheel.x, event->wheel.y);
            return true;
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP: {
            ImGuiKey key = SDLKeyToImGuiKey(event->key.key);
            io.AddKeyEvent(key, event->type == SDL_EVENT_KEY_DOWN);
            io.AddKeyEvent(ImGuiMod_Ctrl, (event->key.mod & SDL_KMOD_CTRL) != 0);
            io.AddKeyEvent(ImGuiMod_Shift, (event->key.mod & SDL_KMOD_SHIFT) != 0);
            io.AddKeyEvent(ImGuiMod_Alt, (event->key.mod & SDL_KMOD_ALT) != 0);
            io.AddKeyEvent(ImGuiMod_Super, (event->key.mod & SDL_KMOD_GUI) != 0);
            return true;
        }
        case SDL_EVENT_TEXT_INPUT:
            io.AddInputCharactersUTF8(event->text.text);
            return true;
        case SDL_EVENT_WINDOW_RESIZED:
            return true;
    }
    return false;
}

// Stub implementations for the Win32 API used by the engine
// These are called from imgui_local.h but won't work properly on Linux
bool ImGui_ImplWin32_Init(void*) { return true; }
void ImGui_ImplWin32_Shutdown() {}
void ImGui_ImplWin32_NewFrame() { ImGui_ImplSDL3_NewFrame(); }
bool ImGui_ImplWin32_WndProcHandler(void*, unsigned int, uintptr_t, intptr_t) { return false; }
