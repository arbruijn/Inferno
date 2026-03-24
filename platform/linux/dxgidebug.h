// dxgidebug.h stub for Linux
#pragma once

#include <vkd3d_windows.h>

// These interfaces don't exist in vkd3d
struct IDXGIInfoQueue;
struct IDXGIDebug1 {};

#ifndef DXGI_DEBUG_ALL
#define DXGI_DEBUG_ALL nullptr
#endif
#ifndef DXGI_DEBUG_RLO_ALL
#define DXGI_DEBUG_RLO_ALL 0
#endif
#ifndef DXGI_DEBUG_RLO_DETAIL
#define DXGI_DEBUG_RLO_DETAIL 1
#endif
#ifndef DXGI_DEBUG_RLO_IGNORE_INTERNAL
#define DXGI_DEBUG_RLO_IGNORE_INTERNAL 2
#endif
