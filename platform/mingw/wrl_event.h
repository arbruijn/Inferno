// WRL Event wrapper stub for MinGW
// Provides Microsoft::WRL::Wrappers::Event equivalent
#pragma once

#include <windows.h>
#include <utility>

namespace Microsoft::WRL::Wrappers {
    class Event {
        HANDLE _handle = INVALID_HANDLE_VALUE;
    public:
        Event() = default;
        explicit Event(HANDLE h) : _handle(h) {}
        ~Event() { if (_handle != INVALID_HANDLE_VALUE) CloseHandle(_handle); }

        Event(const Event&) = delete;
        Event& operator=(const Event&) = delete;

        Event(Event&& other) noexcept : _handle(std::exchange(other._handle, INVALID_HANDLE_VALUE)) {}
        Event& operator=(Event&& other) noexcept {
            if (this != &other) {
                if (_handle != INVALID_HANDLE_VALUE) CloseHandle(_handle);
                _handle = std::exchange(other._handle, INVALID_HANDLE_VALUE);
            }
            return *this;
        }

        void Attach(HANDLE h) noexcept {
            if (_handle != INVALID_HANDLE_VALUE) CloseHandle(_handle);
            _handle = h;
        }

        HANDLE Get() const noexcept { return _handle; }
        bool IsValid() const noexcept { return _handle != INVALID_HANDLE_VALUE && _handle != nullptr; }
    };
}
