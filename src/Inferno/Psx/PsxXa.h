#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "PsxStr.h"

namespace Inferno::Psx {

enum class PsxXaBitsPerSample : std::uint8_t {
    Bits4 = 4,
    Bits8 = 8,
};

struct PsxXaAudioFormat {
    std::uint8_t codingInfo = 0;
    std::uint8_t reservedBits = 0;
    bool emphasis = false;
    bool stereo = false;
    std::uint8_t channelCount = 0;
    int sampleRate = 0;
    PsxXaBitsPerSample bitsPerSample = PsxXaBitsPerSample::Bits4;
    std::size_t sampleFramesPerSector = 0;
    std::size_t samplesPerSector = 0;
};

bool ParsePsxXaAudioFormat(std::uint8_t codingInfo, PsxXaAudioFormat* format) noexcept;
bool ParsePsxXaAudioFormat(const CdXaSectorHeader& header, PsxXaAudioFormat* format) noexcept;

struct PsxXaDecodeState {
    std::int32_t previousSample[2] = {0, 0};
    std::int32_t olderSample[2] = {0, 0};

    void Reset() noexcept;
};

struct PsxXaPcm16 {
    int sampleRate = 0;
    std::uint8_t channelCount = 0;
    std::size_t sampleFrames = 0;
    std::vector<std::int16_t> samples; // Interleaved PCM16.
};

struct PsxXaPcmFloat {
    int sampleRate = 0;
    std::uint8_t channelCount = 0;
    std::size_t sampleFrames = 0;
    std::vector<float> samples; // Interleaved PCM, normalized to [-1.0, 1.0).
};

class PsxXaDecoder {
public:
    // XA ADPCM prediction history carries across sectors in a stream.
    void Reset() noexcept;
    const PsxXaDecodeState& State() const noexcept;

    bool DecodeSector(const PsxStrSector& sector, PsxXaPcm16* output, std::string* error = nullptr);
    bool DecodeSector(const PsxStrSector& sector, PsxXaPcmFloat* output, std::string* error = nullptr);

    bool DecodeSector(const void* sectorBytes,
                      std::size_t size,
                      std::uint8_t codingInfo,
                      PsxXaPcm16* output,
                      std::string* error = nullptr);

    bool DecodeSector(const void* sectorBytes,
                      std::size_t size,
                      std::uint8_t codingInfo,
                      PsxXaPcmFloat* output,
                      std::string* error = nullptr);

private:
    PsxXaDecodeState state_;
};

} // namespace Inferno::Psx
