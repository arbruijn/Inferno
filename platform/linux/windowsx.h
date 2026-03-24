// windowsx.h stub for Linux
#pragma once

// Macro helpers from Windows windowsx.h
#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif
#ifndef LOWORD
#define LOWORD(l) ((unsigned short)(((uintptr_t)(l)) & 0xFFFF))
#endif
#ifndef HIWORD
#define HIWORD(l) ((unsigned short)((((uintptr_t)(l)) >> 16) & 0xFFFF))
#endif
