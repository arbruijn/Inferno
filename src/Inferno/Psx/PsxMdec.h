#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "PsxStr.h"

namespace Inferno::Psx {

struct PsxRgbFrame {
    std::uint16_t width = 0;
    std::uint16_t height = 0;
    std::vector<std::uint8_t> pixels; // RGB24, row-major, three bytes per pixel.
};

class PsxMdecDecoder {
public:
    bool DecodeVersion2Frame(const PsxStrFrame& frame, PsxRgbFrame* output, std::string* error = nullptr) const;
    bool DecodeVersion2Frame(const std::vector<std::uint8_t>& demuxedData,
                             std::uint16_t width,
                             std::uint16_t height,
                             PsxRgbFrame* output,
                             std::string* error = nullptr) const;
};

} // namespace Inferno::Psx
