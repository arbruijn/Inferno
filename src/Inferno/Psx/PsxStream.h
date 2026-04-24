#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace Inferno::Psx {

class PsxReadStream {
public:
    virtual ~PsxReadStream() = default;

    virtual bool Read(std::uint8_t* output,
                      std::size_t maxBytes,
                      std::size_t* bytesRead,
                      std::string* error = nullptr) = 0;
    virtual bool Skip(std::size_t byteCount, std::string* error = nullptr);
};

} // namespace Inferno::Psx
