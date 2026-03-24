// WinCompat.h - Force-included Windows compatibility header for Linux
// This header provides all the Windows types needed by DirectXTK12 and the engine

#pragma once

// Calling conventions - not applicable on Linux
#ifndef __cdecl
#define __cdecl
#endif
#ifndef __stdcall
#define __stdcall
#endif
#ifndef WINAPI
#define WINAPI
#endif

// SAL (Source Annotation Language) annotations - stub for Linux
#define _In_
#define _Out_
#define _Inout_
#define _In_z_
#define _In_opt_
#define _In_opt_z_
#define _Out_opt_
#define _Out_writes_(x)
#define _Out_writes_bytes_(x)
#define _Out_writes_z_(x)
#define _Out_writes_opt_(x)
#define _Out_writes_to_(s,c)
#define _Out_writes_bytes_to_(s,c)
#define _In_reads_(x)
#define _In_reads_bytes_(x)
#define _In_reads_z_(x)
#define _In_reads_opt_(x)
#define _In_reads_opt_z_(x)
#define _In_reads_to_(x,y)
#define _Inout_updates_(x)
#define _Inout_updates_bytes_(x)
#define _Inout_updates_z_(x)
#define _Outptr_
#define _Outptr_result_maybenull_
#define _Outptr_opt_
#define _Outptr_opt_result_maybenull_
#define _COM_Outptr_
#define _COM_Outptr_result_maybenull_
#define _Outptr_result_buffer_(x)
#define _Outptr_result_bytebuffer_(x)
#define _Printf_format_string_
#define _Success_(x)
#define _Field_size_(x)
#define _Field_size_bytes_(x)
#define _Field_size_opt_(x)
#define _Field_size_part_(x,y)
#define _Analysis_assume_(x)
#define _Ret_maybenull_
#define _Ret_notnull_
#define _Ret_z_
#define _Post_writable_byte_size_(x)
#define _Post_z_
#define DECLSPEC_SELECTANY
#define _Use_decl_annotations_
#define _Check_return_
#define _IRQL_requires_same_
#define _Post_equal_to_(x)
#define _Pre_defensive_
#define _Post_defensive_
#define _On_failure_(x)
#define _Always_(x)
#define _Maybenull_
#define _Notnull_
#define _Null_terminated_
#define _Pre_maybenull_
#define _Pre_notnull_
#define _Pre_valid_
#define _Post_maybenull_
#define _Post_notnull_
#define _Post_valid_
#define _Pre_z_
#define _Pre_writable_size_(x)
#define _Pre_readable_size_(x)
#define _Benign_race_begin_
#define _Benign_race_end_
#define _No_competing_thread_
#define _Function_class_(x)
#define _Raises_SEH_exception_
#define _Literal_
#define _Notliteral_
#define _Pre_satisfies_(x)
#define _Post_satisfies_(x)
#define _When_(x)
#define _At_(x,y)
#define _Analysis_noreturn_
#define _Post_
#define _Pre_
#define _Outptr_result_nullonfailure_
#define _Outptr_result_buffer_maybenull_(x)
#define _In_range_(lb,ub)
#define _Out_range_(lb,ub)
#define _Ret_range_(lb,ub)
#define _Pre_equal_to_(x)
#define _Struct_size_bytes_(x)
#define _Readable_bytes_(x)
#define _Writable_bytes_(x)
#define _Post_bytecap_(x)
#define _Post_bytecount_(x)
#define _Strict_type_match_
#define _Enum_is_bitflag_
#define _Points_to_data_
#define _In_opt_count_(x)
#define _In_count_(x)
#define _In_opt_bytecount_(x)
#define _In_bytecount_(x)

// Common Win32 macros
#ifndef IID_PPV_ARGS
#define IID_PPV_ARGS(ppType) __uuidof(**(ppType)), reinterpret_cast<void**>(ppType)
#endif
#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(x) (void)(x)
#endif

// Include vkd3d's Windows compatibility layer first
#include <vkd3d_windows.h>
#include <vkd3d_d3dcommon.h>

// Additional Windows types not provided by vkd3d
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cwchar>

typedef void* HWND;
typedef void* HINSTANCE;
typedef void* HMODULE;
typedef void* HBRUSH;
typedef void* HCURSOR;
typedef void* HICON;
typedef void* HMENU;
typedef void* HDC;
typedef wchar_t WCHAR;
typedef WCHAR* LPWSTR;
typedef const WCHAR* LPCWSTR;
typedef char* LPSTR;
typedef const char* LPCSTR;
typedef void* LPVOID;
typedef const void* LPCVOID;
typedef uintptr_t WPARAM;
typedef intptr_t LPARAM;
typedef long LRESULT;
typedef intptr_t LONG_PTR;
typedef uintptr_t ULONG_PTR;
typedef uintptr_t UINT_PTR;
typedef uintptr_t DWORD_PTR;
typedef int errno_t;

#define TRUE 1
#define FALSE 0
#define INFINITE 0xFFFFFFFF
#define CP_UTF8 65001

// Window styles
#define WS_OVERLAPPEDWINDOW 0x00CF0000L
#define SW_SHOW 5
#define SW_SHOWMAXIMIZED 3
#define SW_RESTORE 9
#define SWP_NOZORDER 0x0004
#define SWP_FRAMECHANGED 0x0020
#define SWP_NOMOVE 0x0002
#define SWP_NOSIZE 0x0001
#define HWND_TOP ((HWND)0)
#define GWLP_USERDATA (-21)
#define GWL_STYLE (-16)
#define GWL_EXSTYLE (-20)
#define WS_EX_TOPMOST 0x00000008L

// Messages
#define WM_CLOSE 0x0010
#define WM_DESTROY 0x0002
#define WM_SIZE 0x0005
#define WM_MOVE 0x0003
#define WM_ACTIVATEAPP 0x001C
#define WM_SYSKEYDOWN 0x0104
#define WM_CHAR 0x0102
#define WM_GETMINMAXINFO 0x0024
#define WM_ENTERSIZEMOVE 0x0231
#define WM_EXITSIZEMOVE 0x0232
#define WM_POWERBROADCAST 0x0218
#define WM_DISPLAYCHANGE 0x007E
#define WM_MENUCHAR 0x0120
#define WM_SYSCOMMAND 0x0112
#define WM_QUIT 0x0012
#define SIZE_MINIMIZED 1
#define SC_KEYMENU 0xF100
#define PBT_APMQUERYSUSPEND 0x0000
#define PBT_APMRESUMESUSPEND 0x0007
#define VK_RETURN 0x0D
#define PM_REMOVE 0x0001
#define SM_CXSCREEN 0
#define SM_CYSCREEN 1
#define MB_OK 0x00000000L
#define MB_ICONWARNING 0x00000030L
#define MB_ICONERROR 0x00000010L
#define CS_HREDRAW 0x0002
#define CS_VREDRAW 0x0001
#define IDC_ARROW ((LPCWSTR)32512)

// Structs
struct POINT { long x, y; };
struct MSG { HWND hwnd; UINT message; WPARAM wParam; LPARAM lParam; DWORD time; POINT pt; };
struct WNDCLASSEX {
    UINT cbSize; UINT style; void* lpfnWndProc; int cbClsExtra; int cbWndExtra;
    HINSTANCE hInstance; HICON hIcon; HCURSOR hCursor; HBRUSH hbrBackground;
    LPCWSTR lpszMenuName; LPCWSTR lpszClassName; HICON hIconSm;
};
struct WINDOWPLACEMENT {
    UINT length; UINT flags; UINT showCmd;
    POINT ptMinPosition; POINT ptMaxPosition; RECT rcNormalPosition;
};
struct MINMAXINFO {
    POINT ptReserved; POINT ptMaxSize; POINT ptMaxPosition;
    POINT ptMinTrackSize; POINT ptMaxTrackSize;
};

// Win32 stubs
inline BOOL AllocConsole() { return TRUE; }
inline HMODULE GetModuleHandle(const void*) { return nullptr; }
inline DWORD GetModuleFileName(HMODULE, LPWSTR, DWORD) { return 0; }
inline DWORD GetCurrentDirectory(DWORD, LPWSTR) { return 0; }
inline int GetSystemMetrics(int) { return 0; }
inline DWORD GetDpiForWindow(HWND) { return 96; }
inline BOOL ShowWindow(HWND, int) { return TRUE; }
inline BOOL DestroyWindow(HWND) { return TRUE; }
inline BOOL SetWindowTextW(HWND, LPCWSTR) { return TRUE; }
inline LONG SetWindowLongPtr(HWND, int, LONG_PTR) { return 0; }
inline LONG_PTR GetWindowLongPtr(HWND, int) { return 0; }
inline BOOL SetWindowPos(HWND, HWND, int, int, int, int, UINT) { return TRUE; }
inline BOOL GetClientRect(HWND, RECT*) { return TRUE; }
inline BOOL GetWindowRect(HWND, RECT*) { return TRUE; }
inline BOOL GetWindowPlacement(HWND, WINDOWPLACEMENT*) { return TRUE; }
inline BOOL AdjustWindowRectEx(RECT*, DWORD, BOOL, DWORD) { return TRUE; }
inline BOOL PeekMessage(MSG*, HWND, UINT, UINT, UINT) { return FALSE; }
inline BOOL TranslateMessage(const MSG*) { return TRUE; }
inline LRESULT DispatchMessage(const MSG*) { return 0; }
inline void PostQuitMessage(int) {}
inline LRESULT DefWindowProc(HWND, UINT, WPARAM, LPARAM) { return 0; }
inline BOOL PostMessage(HWND, UINT, WPARAM, LPARAM) { return TRUE; }
inline BOOL RegisterClassEx(const WNDCLASSEX*) { return TRUE; }
inline BOOL UnregisterClass(LPCWSTR, HMODULE) { return TRUE; }
inline HBRUSH CreateSolidBrush(DWORD) { return nullptr; }
inline BOOL DeleteObject(HBRUSH) { return TRUE; }
inline HICON LoadIcon(HINSTANCE, LPCWSTR) { return nullptr; }
inline HCURSOR LoadCursor(HINSTANCE, LPCWSTR) { return nullptr; }
inline HWND CreateWindowEx(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, void*) { return nullptr; }
inline int DwmSetWindowAttribute(HWND, int, const void*, DWORD) { return 0; }
inline void OutputDebugStringA(const char*) {}
inline errno_t freopen_s(FILE**, const char*, const char*, FILE*) { return 0; }
inline LPWSTR* CommandLineToArgvW(LPCWSTR, int*) { return nullptr; }
inline void* LocalFree(void*) { return nullptr; }

// String helpers
#include <string>
#include <locale>
#include <codecvt>
namespace Inferno {
    inline std::wstring Widen(std::string_view str) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> c;
        return c.from_bytes(str.data(), str.data() + str.size());
    }
    inline std::string Narrow(const std::wstring& str) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> c;
        return c.to_bytes(str);
    }
}

// ThrowIfFailed
#include <stdexcept>
inline void ThrowIfFailed(HRESULT hr) { if (FAILED(hr)) throw std::runtime_error("HRESULT failed"); }

// WRL::ComPtr
namespace Microsoft::WRL {
    template<typename T> class ComPtr {
        T* p_ = nullptr;
        void AddRef() const { if (p_) p_->AddRef(); }
        unsigned long Release() { unsigned long r = 0; if (p_) { r = p_->Release(); p_ = nullptr; } return r; }
    public:
        ComPtr() = default;
        ComPtr(std::nullptr_t) {}
        explicit ComPtr(T* p) : p_(p) { AddRef(); }
        ComPtr(const ComPtr& o) : p_(o.p_) { AddRef(); }
        ComPtr(ComPtr&& o) : p_(o.p_) { o.p_ = nullptr; }
        ~ComPtr() { Release(); }
        ComPtr& operator=(std::nullptr_t) { Release(); return *this; }
        ComPtr& operator=(T* p) { if (p_ != p) { ComPtr t(p); Swap(t); } return *this; }
        ComPtr& operator=(const ComPtr& o) { if (p_ != o.p_) { ComPtr t(o); Swap(t); } return *this; }
        ComPtr& operator=(ComPtr&& o) { if (p_ != o.p_) { Release(); p_ = o.p_; o.p_ = nullptr; } return *this; }
        void Swap(ComPtr& o) { T* t = p_; p_ = o.p_; o.p_ = t; }
        T* Get() const { return p_; }
        T** GetAddressOf() { return &p_; }
        T** ReleaseAndGetAddressOf() { Release(); return &p_; }
        T* Detach() { T* t = p_; p_ = nullptr; return t; }
        void Attach(T* p) { if (p_) Release(); p_ = p; }
        unsigned long Reset() { return Release(); }
        template<typename U> HRESULT As(ComPtr<U>* o) const {
            if (!p_) { *o = nullptr; return 0; }
            return p_->QueryInterface(__uuidof(U), reinterpret_cast<void**>(o->ReleaseAndGetAddressOf()));
        }
        T* operator->() const { return p_; }
        explicit operator bool() const { return p_ != nullptr; }
        bool operator==(std::nullptr_t) const { return p_ == nullptr; }
        bool operator!=(std::nullptr_t) const { return p_ != nullptr; }
        bool operator==(const ComPtr& o) const { return p_ == o.p_; }
        bool operator!=(const ComPtr& o) const { return p_ != o.p_; }
    };
}

// Time period stub
struct SetWindowsTimePeriod { SetWindowsTimePeriod() {} ~SetWindowsTimePeriod() {} };

// sal.h annotations
#ifndef _Use_decl_annotations_
#define _Use_decl_annotations_
#endif
#ifndef _Check_return_
#define _Check_return_
#endif

// MSVC 'byte' type (unsigned char on MSVC, std::byte in C++17 on GCC)
using byte = unsigned char;

// Windows string comparison functions
#include <strings.h>
inline int _strnicmp(const char* s1, const char* s2, size_t n) { return strncasecmp(s1, s2, n); }
inline int _wcsnicmp(const wchar_t* s1, const wchar_t* s2, size_t n) { return wcsncasecmp(s1, s2, n); }

// Windows case conversion functions
#include <algorithm>
#include <cctype>
inline char* CharUpperA(char* s) { for (char* p = s; *p; ++p) *p = (char)std::toupper((unsigned char)*p); return s; }
inline char* CharLowerA(char* s) { for (char* p = s; *p; ++p) *p = (char)std::tolower((unsigned char)*p); return s; }
