#pragma once

#include <set>
#include <queue>
#include <optional>
#include <unordered_map>
#include <cassert>
#include <array>
#include <filesystem>

#define NOMINMAX
#include <windows.h>
#if 0
// Copied from <windows.h> to make SimpleMath happy
using UINT = unsigned int;

typedef struct tagRECT {
    long left;
    long top;
    long right;
    long bottom;
} RECT, * PRECT, * NPRECT, * LPRECT;
#endif

#include <DirectXTK12/SimpleMath.h>

#define FMT_HEADER_ONLY
#include <fmt/core.h>

#undef min
#undef max
