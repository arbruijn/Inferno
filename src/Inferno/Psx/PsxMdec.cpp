#include "PsxMdec.h"

#include <array>
#include <cmath>
#include <cstring>
#include <limits>

namespace Inferno::Psx {
namespace {

constexpr std::uint16_t kMdecHeaderMarker = 0x3800;
constexpr std::size_t kFrameHeaderSize = 8;
constexpr std::size_t kOptionalPrefixSize = 40;
constexpr std::size_t kBlockSize = 8;
constexpr std::size_t kMacroblockSize = 16;
constexpr std::size_t kBlocksPerMacroblock = 6;
constexpr double kPi = 3.141592653589793238462643383279502884;

// Maps each destination matrix slot to the coefficient-list index from the spec's zig-zag table.
constexpr std::array<std::uint8_t, 64> kZigZag = {
    0,  1,  5,  6, 14, 15, 27, 28,
    2,  4,  7, 13, 16, 26, 29, 42,
    3,  8, 12, 17, 25, 30, 41, 43,
    9, 11, 18, 24, 31, 40, 44, 53,
   10, 19, 23, 32, 39, 45, 52, 54,
   20, 22, 33, 38, 46, 51, 55, 60,
   21, 34, 37, 47, 50, 56, 59, 61,
   35, 36, 48, 49, 57, 58, 62, 63,
};

constexpr std::array<std::uint8_t, 64> kQuantMatrix = {
     2, 16, 19, 22, 26, 27, 29, 34,
    16, 16, 22, 24, 27, 29, 34, 37,
    19, 22, 26, 27, 29, 34, 34, 38,
    22, 22, 26, 27, 29, 34, 37, 40,
    22, 26, 27, 29, 32, 35, 40, 48,
    26, 27, 29, 32, 35, 40, 48, 58,
    26, 27, 29, 34, 38, 46, 56, 69,
    27, 29, 35, 38, 46, 56, 69, 83,
};

using BlockMatrix = std::array<float, 64>;
using MacroblockPlane = std::array<float, kMacroblockSize * kMacroblockSize>;

std::array<std::array<float, kBlockSize>, kBlockSize> BuildIdctBasis() {
    std::array<std::array<float, kBlockSize>, kBlockSize> basis{};
    for (std::size_t y = 0; y < kBlockSize; ++y) {
        for (std::size_t u = 0; u < kBlockSize; ++u) {
            const double scale = (u == 0) ? std::sqrt(1.0 / 8.0) : std::sqrt(2.0 / 8.0);
            const double cosine =
                std::cos((static_cast<double>(2 * y + 1) * static_cast<double>(u) * kPi) / 16.0);
            basis[y][u] = static_cast<float>(scale * cosine);
        }
    }
    return basis;
}

const std::array<std::array<float, kBlockSize>, kBlockSize>& IdctBasis() {
    static const auto basis = BuildIdctBasis();
    return basis;
}

struct BitReader {
    explicit BitReader(const std::uint8_t* data, std::size_t size)
        : data(data), size(size), totalBits((size / 2) * 16) {}

    bool ReadBit(unsigned* bit) {
        if (bitOffset >= totalBits) {
            return false;
        }

        const std::size_t wordIndex = bitOffset / 16U;
        const unsigned bitIndex = static_cast<unsigned>(15U - (bitOffset % 16U));
        const std::uint16_t word = static_cast<std::uint16_t>(data[wordIndex * 2] |
            (static_cast<std::uint16_t>(data[wordIndex * 2 + 1]) << 8));
        *bit = static_cast<unsigned>((word >> bitIndex) & 1U);
        ++bitOffset;
        return true;
    }

    bool PeekBits(unsigned count, std::uint32_t* value) const {
        if (count > 32U || bitOffset + count > totalBits) {
            return false;
        }

        std::uint32_t result = 0;
        for (unsigned i = 0; i < count; ++i) {
            const std::size_t pos = bitOffset + i;
            const std::size_t wordIndex = pos / 16U;
            const unsigned bitIndex = static_cast<unsigned>(15U - (pos % 16U));
            const std::uint16_t word = static_cast<std::uint16_t>(data[wordIndex * 2] |
                (static_cast<std::uint16_t>(data[wordIndex * 2 + 1]) << 8));
            result = static_cast<std::uint32_t>((result << 1U) | ((word >> bitIndex) & 1U));
        }

        *value = result;
        return true;
    }

    bool ReadBits(unsigned count, std::uint32_t* value) {
        if (!PeekBits(count, value)) {
            return false;
        }
        bitOffset += count;
        return true;
    }

    bool ReadSignedBits(unsigned count, int* value) {
        std::uint32_t bits = 0;
        if (!ReadBits(count, &bits)) {
            return false;
        }

        const std::uint32_t signBit = 1U << (count - 1U);
        if ((bits & signBit) != 0U) {
            const std::uint32_t extendMask = ~((1U << count) - 1U);
            bits |= extendMask;
        }
        *value = static_cast<int>(static_cast<std::int32_t>(bits));
        return true;
    }

    bool SkipBits(unsigned count) {
        if (bitOffset + count > totalBits) {
            return false;
        }
        bitOffset += count;
        return true;
    }

    std::size_t RemainingBits() const {
        return totalBits - bitOffset;
    }

    const std::uint8_t* data = nullptr;
    std::size_t size = 0;
    std::size_t totalBits = 0;
    std::size_t bitOffset = 0;
};

struct AcVlcEntry {
    std::uint32_t prefix = 0;
    std::uint8_t length = 0;
    std::uint8_t run = 0;
    std::uint8_t level = 0;
    bool escape = false;
    bool endOfBlock = false;
};

constexpr std::array<AcVlcEntry, 113> kAcVlcEntries = {{
    {0b11, 2, 0, 1, false, false},
    {0b011, 3, 1, 1, false, false},
    {0b0100, 4, 0, 2, false, false},
    {0b0101, 4, 2, 1, false, false},
    {0b00101, 5, 0, 3, false, false},
    {0b00110, 5, 4, 1, false, false},
    {0b00111, 5, 3, 1, false, false},
    {0b000100, 6, 7, 1, false, false},
    {0b000101, 6, 6, 1, false, false},
    {0b000110, 6, 1, 2, false, false},
    {0b000111, 6, 5, 1, false, false},
    {0b0000100, 7, 2, 2, false, false},
    {0b0000101, 7, 9, 1, false, false},
    {0b0000110, 7, 0, 4, false, false},
    {0b0000111, 7, 8, 1, false, false},
    {0b00100000, 8, 13, 1, false, false},
    {0b00100001, 8, 0, 6, false, false},
    {0b00100010, 8, 12, 1, false, false},
    {0b00100011, 8, 11, 1, false, false},
    {0b00100100, 8, 3, 2, false, false},
    {0b00100101, 8, 1, 3, false, false},
    {0b00100110, 8, 0, 5, false, false},
    {0b00100111, 8, 10, 1, false, false},
    {0b0000001000, 10, 16, 1, false, false},
    {0b0000001001, 10, 5, 2, false, false},
    {0b0000001010, 10, 0, 7, false, false},
    {0b0000001011, 10, 2, 3, false, false},
    {0b0000001100, 10, 1, 4, false, false},
    {0b0000001101, 10, 15, 1, false, false},
    {0b0000001110, 10, 14, 1, false, false},
    {0b0000001111, 10, 4, 2, false, false},
    {0b000000010000, 12, 0, 11, false, false},
    {0b000000010001, 12, 8, 2, false, false},
    {0b000000010010, 12, 4, 3, false, false},
    {0b000000010011, 12, 0, 10, false, false},
    {0b000000010100, 12, 2, 4, false, false},
    {0b000000010101, 12, 7, 2, false, false},
    {0b000000010110, 12, 21, 1, false, false},
    {0b000000010111, 12, 20, 1, false, false},
    {0b000000011000, 12, 0, 9, false, false},
    {0b000000011001, 12, 19, 1, false, false},
    {0b000000011010, 12, 18, 1, false, false},
    {0b000000011011, 12, 1, 5, false, false},
    {0b000000011100, 12, 3, 3, false, false},
    {0b000000011101, 12, 0, 8, false, false},
    {0b000000011110, 12, 6, 2, false, false},
    {0b000000011111, 12, 17, 1, false, false},
    {0b0000000010000, 13, 10, 2, false, false},
    {0b0000000010001, 13, 9, 2, false, false},
    {0b0000000010010, 13, 5, 3, false, false},
    {0b0000000010011, 13, 3, 4, false, false},
    {0b0000000010100, 13, 2, 5, false, false},
    {0b0000000010101, 13, 1, 7, false, false},
    {0b0000000010110, 13, 1, 6, false, false},
    {0b0000000010111, 13, 0, 15, false, false},
    {0b0000000011000, 13, 0, 14, false, false},
    {0b0000000011001, 13, 0, 13, false, false},
    {0b0000000011010, 13, 0, 12, false, false},
    {0b0000000011011, 13, 26, 1, false, false},
    {0b0000000011100, 13, 25, 1, false, false},
    {0b0000000011101, 13, 24, 1, false, false},
    {0b0000000011110, 13, 23, 1, false, false},
    {0b0000000011111, 13, 22, 1, false, false},
    {0b00000000010000, 14, 0, 31, false, false},
    {0b00000000010001, 14, 0, 30, false, false},
    {0b00000000010010, 14, 0, 29, false, false},
    {0b00000000010011, 14, 0, 28, false, false},
    {0b00000000010100, 14, 0, 27, false, false},
    {0b00000000010101, 14, 0, 26, false, false},
    {0b00000000010110, 14, 0, 25, false, false},
    {0b00000000010111, 14, 0, 24, false, false},
    {0b00000000011000, 14, 0, 23, false, false},
    {0b00000000011001, 14, 0, 22, false, false},
    {0b00000000011010, 14, 0, 21, false, false},
    {0b00000000011011, 14, 0, 20, false, false},
    {0b00000000011100, 14, 0, 19, false, false},
    {0b00000000011101, 14, 0, 18, false, false},
    {0b00000000011110, 14, 0, 17, false, false},
    {0b00000000011111, 14, 0, 16, false, false},
    {0b000000000010000, 15, 0, 40, false, false},
    {0b000000000010001, 15, 0, 39, false, false},
    {0b000000000010010, 15, 0, 38, false, false},
    {0b000000000010011, 15, 0, 37, false, false},
    {0b000000000010100, 15, 0, 36, false, false},
    {0b000000000010101, 15, 0, 35, false, false},
    {0b000000000010110, 15, 0, 34, false, false},
    {0b000000000010111, 15, 0, 33, false, false},
    {0b000000000011000, 15, 0, 32, false, false},
    {0b000000000011001, 15, 1, 14, false, false},
    {0b000000000011010, 15, 1, 13, false, false},
    {0b000000000011011, 15, 1, 12, false, false},
    {0b000000000011100, 15, 1, 11, false, false},
    {0b000000000011101, 15, 1, 10, false, false},
    {0b000000000011110, 15, 1, 9, false, false},
    {0b000000000011111, 15, 1, 8, false, false},
    {0b0000000000010000, 16, 1, 18, false, false},
    {0b0000000000010001, 16, 1, 17, false, false},
    {0b0000000000010010, 16, 1, 16, false, false},
    {0b0000000000010011, 16, 1, 15, false, false},
    {0b0000000000010100, 16, 6, 3, false, false},
    {0b0000000000010101, 16, 16, 2, false, false},
    {0b0000000000010110, 16, 15, 2, false, false},
    {0b0000000000010111, 16, 14, 2, false, false},
    {0b0000000000011000, 16, 13, 2, false, false},
    {0b0000000000011001, 16, 12, 2, false, false},
    {0b0000000000011010, 16, 11, 2, false, false},
    {0b0000000000011011, 16, 31, 1, false, false},
    {0b0000000000011100, 16, 30, 1, false, false},
    {0b0000000000011101, 16, 29, 1, false, false},
    {0b0000000000011110, 16, 28, 1, false, false},
    {0b0000000000011111, 16, 27, 1, false, false},
    {0b000001, 6, 0, 0, true, false},
    {0b10, 2, 0, 0, false, true},
}};

constexpr std::uint8_t kAcVlcLookupBits = 16;

enum class AcVlcLookupKind : std::uint8_t {
    Invalid = 0,
    Coefficient,
    Escape,
    EndOfBlock,
};

struct AcVlcLookupEntry {
    std::uint8_t length = 0;
    std::uint8_t run = 0;
    std::uint8_t level = 0;
    AcVlcLookupKind kind = AcVlcLookupKind::Invalid;
};

constexpr void FillAcVlcLookupRange(
    std::array<AcVlcLookupEntry, 1U << kAcVlcLookupBits>* table,
    std::uint32_t prefix,
    std::uint8_t length,
    const AcVlcLookupEntry& entry) {
    const std::uint32_t suffixBits = static_cast<std::uint32_t>(kAcVlcLookupBits - length);
    const std::uint32_t base = prefix << suffixBits;
    const std::uint32_t count = 1U << suffixBits;
    for (std::uint32_t i = 0; i < count; ++i) {
        (*table)[base + i] = entry;
    }
}

constexpr auto BuildAcVlcLookup() {
    std::array<AcVlcLookupEntry, 1U << kAcVlcLookupBits> table{};
    for (const AcVlcEntry& entry : kAcVlcEntries) {
        AcVlcLookupEntry decoded{};
        decoded.length = entry.length;
        decoded.run = entry.run;
        decoded.level = entry.level;
        if (entry.endOfBlock) {
            decoded.kind = AcVlcLookupKind::EndOfBlock;
        } else if (entry.escape) {
            decoded.kind = AcVlcLookupKind::Escape;
        } else {
            decoded.kind = AcVlcLookupKind::Coefficient;
        }
        FillAcVlcLookupRange(&table, entry.prefix, entry.length, decoded);
    }
    return table;
}

constexpr auto kAcVlcLookup = BuildAcVlcLookup();

std::uint8_t ClampToByte(float value) {
    if (value < 0.0f) {
        return 0;
    }
    if (value > 255.0f) {
        return 255;
    }
    return static_cast<std::uint8_t>(std::lround(value));
}

void SetError(const char* message, std::string* error) {
    if (error != nullptr) {
        *error = message;
    }
}

std::uint16_t ReadU16LE(const std::uint8_t* data) {
    return static_cast<std::uint16_t>(data[0] |
                                      (static_cast<std::uint16_t>(data[1]) << 8));
}

bool IsValidMdecHeader(const std::uint8_t* data, std::size_t size, std::size_t offset) {
    if (offset + kFrameHeaderSize > size) {
        return false;
    }
    const std::uint16_t marker = ReadU16LE(data + offset + 2);
    const std::uint16_t version = ReadU16LE(data + offset + 6);
    return marker == kMdecHeaderMarker && (version == 1 || version == 2);
}

std::size_t FindFramePayloadOffset(const std::uint8_t* data, std::size_t size) {
    if (IsValidMdecHeader(data, size, 0)) {
        return 0;
    }
    if (IsValidMdecHeader(data, size, kOptionalPrefixSize)) {
        return kOptionalPrefixSize;
    }
    return std::numeric_limits<std::size_t>::max();
}

struct AcDecodeResult {
    bool endOfBlock = false;
    int run = 0;
    int value = 0;
};

bool DecodeAcVlc(BitReader& reader, AcDecodeResult* result) {
    const std::size_t remaining = reader.RemainingBits();
    std::uint32_t prefix = 0;
    if (remaining >= kAcVlcLookupBits) {
        if (!reader.PeekBits(kAcVlcLookupBits, &prefix)) {
            return false;
        }
    } else {
        if (!reader.PeekBits(remaining, &prefix))
            return false;
        prefix <<= (kAcVlcLookupBits - remaining);
    }

    const AcVlcLookupEntry& entry = kAcVlcLookup[prefix];
    if (entry.kind == AcVlcLookupKind::Invalid) {
        return false;
    }

    if (!reader.SkipBits(entry.length)) {
        return false;
    }

    if (entry.kind == AcVlcLookupKind::EndOfBlock) {
        result->endOfBlock = true;
        result->run = 0;
        result->value = 0;
        return true;
    }

    if (entry.kind == AcVlcLookupKind::Escape) {
        std::uint32_t run = 0;
        if (!reader.ReadBits(6, &run))
            return false;
        if (!reader.ReadSignedBits(10, &result->value))
            return false;
        result->run = static_cast<int>(run);
        result->endOfBlock = false;
        return true;
    }

    unsigned signBit = 0;
    if (!reader.ReadBit(&signBit))
        return false;

    result->run = static_cast<int>(entry.run);
    result->value = signBit != 0U ? -static_cast<int>(entry.level) : static_cast<int>(entry.level);
    result->endOfBlock = false;
    return true;
}

bool DecodeBlock(BitReader& reader,
                 std::uint16_t expectedQuantScale,
                 BlockMatrix* spatial,
                 std::string* error) {
    std::array<int, 64> coeffs{};
    coeffs.fill(0);

    if (!reader.ReadSignedBits(10, &coeffs[0])) {
        SetError("PsxMdecDecoder ran out of data while reading a block DC coefficient.", error);
        return false;
    }

    std::size_t position = 0;
    while (true) {
        AcDecodeResult ac;
        if (!DecodeAcVlc(reader, &ac)) {
            SetError("DecodeAcVlc failed", error);
            return false;
        }
        if (ac.endOfBlock) {
            break;
        }

        position += static_cast<std::size_t>(ac.run) + 1U;
        if (position >= coeffs.size()) {
            SetError("PsxMdecDecoder decoded an AC coefficient past the end of the 8x8 block.", error);
            return false;
        }
        coeffs[position] = ac.value;
    }

    const float scale = static_cast<float>(expectedQuantScale & 0x003FU);
    BlockMatrix dequantized{};
    for (std::size_t dstIndex = 0; dstIndex < dequantized.size(); ++dstIndex) {
        const std::size_t srcIndex = static_cast<std::size_t>(kZigZag[dstIndex]);
        const float coeff = static_cast<float>(coeffs[srcIndex]);
        const float quant = static_cast<float>(kQuantMatrix[dstIndex]);
        if (srcIndex == 0) {
            dequantized[dstIndex] = coeff * quant;
        } else {
            dequantized[dstIndex] = coeff * scale * quant * 0.125f;
        }
    }

    const auto& basis = IdctBasis();
    BlockMatrix temp{};
    for (std::size_t y = 0; y < kBlockSize; ++y) {
        for (std::size_t x = 0; x < kBlockSize; ++x) {
            float total = 0.0f;
            for (std::size_t u = 0; u < kBlockSize; ++u) {
                total += dequantized[y * 8 + u] * basis[x][u];
            }
            temp[y * 8 + x] = total;
        }
    }

    for (std::size_t y = 0; y < kBlockSize; ++y) {
        for (std::size_t x = 0; x < kBlockSize; ++x) {
            float total = 0.0f;
            for (std::size_t v = 0; v < kBlockSize; ++v) {
                total += basis[y][v] * temp[v * 8 + x];
            }
            (*spatial)[y * 8 + x] = total;
        }
    }

    return true;
}

struct BlockSet {
    BlockMatrix cr{};
    BlockMatrix cb{};
    BlockMatrix y1{};
    BlockMatrix y2{};
    BlockMatrix y3{};
    BlockMatrix y4{};
};

void CopyLumaBlock(const BlockMatrix& block,
                   MacroblockPlane* yPlane,
                   std::size_t baseX,
                   std::size_t baseY) {
    for (std::size_t y = 0; y < 8; ++y) {
        for (std::size_t x = 0; x < 8; ++x) {
            const float value = block[y * 8 + x];
            (*yPlane)[(baseY + y) * kMacroblockSize + (baseX + x)] = value;
        }
    }
}

void CopyChromaBlock(const BlockMatrix& block,
                     MacroblockPlane* plane,
                     std::size_t baseX,
                     std::size_t baseY) {
    for (std::size_t y = 0; y < 8; ++y) {
        for (std::size_t x = 0; x < 8; ++x) {
            const float value = block[y * 8 + x];
            const std::size_t dstY = baseY + y * 2;
            const std::size_t dstX = baseX + x * 2;
            (*plane)[dstY * kMacroblockSize + dstX] = value;
            (*plane)[dstY * kMacroblockSize + dstX + 1] = value;
            (*plane)[(dstY + 1) * kMacroblockSize + dstX] = value;
            (*plane)[(dstY + 1) * kMacroblockSize + dstX + 1] = value;
        }
    }
}

} // namespace

bool PsxMdecDecoder::DecodeVersion2Frame(const PsxStrFrame& frame,
                                         PsxRgbFrame* output,
                                         std::string* error) const {
    return DecodeVersion2Frame(frame.demuxedData, frame.width, frame.height, output, error);
}

bool PsxMdecDecoder::DecodeVersion2Frame(const std::vector<std::uint8_t>& demuxedData,
                                         std::uint16_t width,
                                         std::uint16_t height,
                                         PsxRgbFrame* output,
                                         std::string* error) const {
    if (output == nullptr) {
        SetError("PsxMdecDecoder requires a non-null output frame.", error);
        return false;
    }
    output->width = 0;
    output->height = 0;
    output->pixels.clear();

    if (width == 0 || height == 0) {
        SetError("PsxMdecDecoder requires a non-zero frame size.", error);
        return false;
    }
    if (demuxedData.size() < kFrameHeaderSize) {
        SetError("PsxMdecDecoder received a demuxed frame that is too small.", error);
        return false;
    }

    const std::size_t payloadOffset = FindFramePayloadOffset(demuxedData.data(), demuxedData.size());
    if (payloadOffset == std::numeric_limits<std::size_t>::max()) {
        SetError("PsxMdecDecoder could not find a v2 MDEC frame header.", error);
        return false;
    }

    const std::uint8_t* payload = demuxedData.data() + payloadOffset;
    const std::size_t payloadSize = demuxedData.size() - payloadOffset;

    if (payloadSize < kFrameHeaderSize) {
        SetError("PsxMdecDecoder could not read the v2 MDEC frame header.", error);
        return false;
    }

    const std::uint16_t declaredBlockWords = ReadU16LE(payload + 0);
    const std::uint16_t marker = ReadU16LE(payload + 2);
    const std::uint16_t headerQuantScale = ReadU16LE(payload + 4);
    const std::uint16_t headerVersion = ReadU16LE(payload + 6);

    if (marker != kMdecHeaderMarker) {
        SetError("PsxMdecDecoder found an invalid MDEC frame marker.", error);
        return false;
    }
    if (headerVersion != 1 && headerVersion != 2) {
        SetError("PsxMdecDecoder only supports version-2-compatible MDEC frames.", error);
        return false;
    }

    BitReader reader(payload + kFrameHeaderSize, payloadSize - kFrameHeaderSize);

    const std::size_t blocksX = (static_cast<std::size_t>(width) + 15U) / 16U;
    const std::size_t blocksY = (static_cast<std::size_t>(height) + 15U) / 16U;
    const std::size_t macroblockCount = blocksX * blocksY;
    const std::size_t expectedBlockCount = macroblockCount * kBlocksPerMacroblock;

    output->width = width;
    output->height = height;
    output->pixels.resize(static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 3U);

    std::size_t decodedBlocks = 0;
    for (std::size_t mbX = 0; mbX < blocksX; ++mbX) {
        for (std::size_t mbY = 0; mbY < blocksY; ++mbY) {
            BlockSet blocks;
            const std::array<BlockMatrix*, 6> destinations = {
                &blocks.cr, &blocks.cb, &blocks.y1, &blocks.y2, &blocks.y3, &blocks.y4
            };

            for (BlockMatrix* block : destinations) {
                if (!DecodeBlock(reader, headerQuantScale, block, error)) {
                    return false;
                }
                ++decodedBlocks;
            }

            MacroblockPlane yPlane{};
            MacroblockPlane cbPlane{};
            MacroblockPlane crPlane{};

            CopyChromaBlock(blocks.cr, &crPlane, 0, 0);
            CopyChromaBlock(blocks.cb, &cbPlane, 0, 0);
            CopyLumaBlock(blocks.y1, &yPlane, 0, 0);
            CopyLumaBlock(blocks.y2, &yPlane, 8, 0);
            CopyLumaBlock(blocks.y3, &yPlane, 0, 8);
            CopyLumaBlock(blocks.y4, &yPlane, 8, 8);

            const std::size_t baseX = mbX * 16U;
            const std::size_t baseY = mbY * 16U;
            for (std::size_t y = 0; y < kMacroblockSize; ++y) {
                const std::size_t dstY = baseY + y;
                if (dstY >= height) {
                    continue;
                }
                for (std::size_t x = 0; x < kMacroblockSize; ++x) {
                    const std::size_t dstX = baseX + x;
                    if (dstX >= width) {
                        continue;
                    }

                    const float yValue = yPlane[y * kMacroblockSize + x] + 128.0f;
                    const float cbValue = cbPlane[y * kMacroblockSize + x];
                    const float crValue = crPlane[y * kMacroblockSize + x];

                    const float red = yValue + 1.4020f * crValue;
                    const float green = yValue - 0.3437f * cbValue - 0.7143f * crValue;
                    const float blue = yValue + 1.7720f * cbValue;

                    const std::size_t outIndex = (dstY * width + dstX) * 3U;
                    output->pixels[outIndex + 0] = ClampToByte(red);
                    output->pixels[outIndex + 1] = ClampToByte(green);
                    output->pixels[outIndex + 2] = ClampToByte(blue);
                }
            }
        }
    }

    if (decodedBlocks != expectedBlockCount) {
        SetError("PsxMdecDecoder decoded an unexpected number of macroblock blocks.", error);
        return false;
    }

    (void)declaredBlockWords;
    return true;
}

} // namespace Inferno::Psx
