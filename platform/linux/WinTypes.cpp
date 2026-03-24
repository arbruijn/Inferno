#include "WinTypes.h"
#include <cstdio>
#include <cstdarg>

// Shell implementation using SDL3 for Linux
// This file provides Linux-compatible implementations of Windows shell functions

// Stub implementations for critical Windows functions used by the engine
extern "C" {

void __debugbreak() {
    __builtin_trap();
}

}
