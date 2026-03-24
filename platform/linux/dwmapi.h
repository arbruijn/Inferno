// dwmapi.h stub for Linux - Desktop Window Manager API
#pragma once

// DWM attributes
typedef int DWMWINDOWATTRIBUTE;
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20

// Stub function
inline int DwmSetWindowAttribute(void*, int, const void*, unsigned long) { return 0; }
