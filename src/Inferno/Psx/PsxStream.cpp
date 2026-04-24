#include "PsxStream.h"

#include <algorithm>
#include <array>

namespace Inferno::Psx {

bool PsxReadStream::Skip(std::size_t byteCount, std::string* error) {
    std::array<std::uint8_t, 4096> discard{};
    std::size_t remaining = byteCount;
    while (remaining > 0) {
        const std::size_t step = std::min(remaining, discard.size());
        std::size_t bytesRead = 0;
        if (!Read(discard.data(), step, &bytesRead, error)) {
            return false;
        }
        if (bytesRead != step) {
            return false;
        }
        remaining -= bytesRead;
    }
    return true;
}

} // namespace Inferno::Psx
