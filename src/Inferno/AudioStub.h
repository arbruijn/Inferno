// AudioStub.h - Minimal audio stubs for MinGW build
#pragma once

#include <string>
#include <vector>

namespace DirectX {
    class AudioEngine {
    public:
        struct RendererDetail {
            std::wstring deviceId;
            std::wstring description;
        };

        static std::vector<RendererDetail> GetRendererDetails() { return {}; }
    };
}
using AudioEngine = DirectX::AudioEngine;
