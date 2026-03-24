// stringapiset.h stub for Linux - Windows string conversion APIs
#pragma once

#include <string>
#include <locale>
#include <codecvt>
#include <cstring>

#define CP_UTF8 65001

// Stub implementations of Windows string conversion functions
inline int MultiByteToWideChar(unsigned int, unsigned long, const char* str, int len, wchar_t* out, int outLen) {
    if (!str) return 0;
    if (len == -1) len = (int)strlen(str);
    if (!out) return len + 1;
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    auto wstr = conv.from_bytes(str, str + len);
    int copyLen = std::min((int)wstr.size(), outLen - 1);
    if (copyLen > 0) {
        wmemcpy(out, wstr.c_str(), copyLen);
        out[copyLen] = 0;
    }
    return (int)wstr.size();
}

inline int WideCharToMultiByte(unsigned int, unsigned long, const wchar_t* str, int len, char* out, int outLen, void*, void*) {
    if (!str) return 0;
    if (len == -1) len = (int)wcslen(str);
    if (!out) return len + 1;
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    auto sstr = conv.to_bytes(str, str + len);
    int copyLen = std::min((int)sstr.size(), outLen - 1);
    if (copyLen > 0) {
        memcpy(out, sstr.c_str(), copyLen);
        out[copyLen] = 0;
    }
    return (int)sstr.size();
}
