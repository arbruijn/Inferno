// PlatformHelpers.h - Linux compatible version
#pragma once

#include <exception>
#include <memory>
#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <cassert>

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3) \
    (static_cast<uint32_t>(static_cast<uint8_t>(ch0)) \
    | (static_cast<uint32_t>(static_cast<uint8_t>(ch1)) << 8) \
    | (static_cast<uint32_t>(static_cast<uint8_t>(ch2)) << 16) \
    | (static_cast<uint32_t>(static_cast<uint8_t>(ch3)) << 24))
#endif

// Linux stubs for missing Windows types
#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE ((void*)(intptr_t)-1)
#endif

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(x) (void)(x)
#endif

#ifndef _WIN32
inline void CloseHandle(void*) {}
#endif

class com_exception : public std::exception {
public:
    com_exception(HRESULT hr) noexcept : result(hr) {}
    const char* what() const noexcept override {
        static char s_str[64] = {};
        snprintf(s_str, sizeof(s_str), "Failure with HRESULT of %08X", static_cast<unsigned int>(result));
        return s_str;
    }
private:
    HRESULT result;
};

inline void ThrowIfFailed(HRESULT hr) noexcept(false) {
    if (FAILED(hr)) throw com_exception(hr);
}

inline void DebugTrace(const char* format, ...) noexcept {
#ifdef _DEBUG
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
#else
    (void)format;
#endif
}

struct aligned_deleter { void operator()(void* p) noexcept { if (p) free(p); } };
struct handle_closer { void operator()(void* h) noexcept { (void)h; } };
using ScopedHandle = std::unique_ptr<void, handle_closer>;
inline void* safe_handle(void* h) noexcept { return (h == INVALID_HANDLE_VALUE) ? nullptr : h; }

template<typename T>
constexpr bool IsPowerOf2(T x) noexcept { return ((x != 0) && !(x & (x - 1))); }

template<typename T>
T AlignDown(T size, size_t alignment) noexcept {
    if (alignment > 0) {
        assert(((alignment - 1) & alignment) == 0);
        auto mask = static_cast<T>(alignment - 1);
        return size & ~mask;
    }
    return size;
}

template<typename T>
T AlignUp(T size, size_t alignment) noexcept {
    if (alignment > 0) {
        assert(((alignment - 1) & alignment) == 0);
        auto mask = static_cast<T>(alignment - 1);
        return (size + mask) & ~mask;
    }
    return size;
}
