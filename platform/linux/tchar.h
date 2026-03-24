// tchar.h stub for Linux
#pragma once

// TCHAR macro mappings
#ifdef _UNICODE
typedef wchar_t TCHAR;
#define _T(x) L##x
#define _tcslen wcslen
#define _tcscpy wcscpy
#define _tcscmp wcscmp
#define _tcsncpy wcsncpy
#define _tprintf wprintf
#define _stprintf swprintf
#else
typedef char TCHAR;
#define _T(x) x
#define _tcslen strlen
#define _tcscpy strcpy
#define _tcscmp strcmp
#define _tcsncpy strncpy
#define _tprintf printf
#define _stprintf snprintf
#endif
