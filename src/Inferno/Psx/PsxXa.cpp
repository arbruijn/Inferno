#include "PsxXa.h"

#include <algorithm>
#include <array>
#include <utility>

namespace Inferno::Psx {
namespace {

constexpr std::size_t kCdSyncHeaderSize = 12;
constexpr std::size_t kCdHeaderSize = 4;
constexpr std::size_t kCdXaSubheaderSize = 8;
constexpr std::size_t kCdXaAudioPayloadSize = 2304;
constexpr std::size_t kCdXaSectorUserDataSize = 2324;
constexpr std::size_t kNormalizedSectorSize = 2336;
constexpr std::size_t kRawSectorSize = 2352;
constexpr std::size_t kSoundGroupSize = 128;
constexpr std::size_t kSoundGroupCountPerSector = 18;
constexpr std::size_t kSoundGroupDataOffset = 16;
constexpr std::size_t kSamplesPerBlock = 28;

constexpr std::uint8_t kCodingInfoReservedMask = 0xAA;
constexpr std::uint8_t kCodingInfoEmphasis = 0x40;
constexpr std::uint8_t kCodingInfoBits8 = 0x10;
constexpr std::uint8_t kCodingInfoSampleRate18900 = 0x04;
constexpr std::uint8_t kCodingInfoStereo = 0x01;

constexpr std::array<int, 4> kPositiveFilter = {0, 60, 115, 98};
constexpr std::array<int, 4> kNegativeFilter = {0, 0, -52, -55};

void SetError(std::string message, std::string* error) {
    if (error != nullptr) {
        *error = std::move(message);
    }
}

bool HasSyncHeader(const std::uint8_t* data) {
    static constexpr std::uint8_t kSync[12] = {
        0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
    };
    for (std::size_t i = 0; i < sizeof(kSync); ++i) {
        if (data[i] != kSync[i]) {
            return false;
        }
    }
    return true;
}

const std::uint8_t* ResolveAudioPayload(const void* sectorBytes,
                                        std::size_t size,
                                        std::string* error) {
    if (sectorBytes == nullptr) {
        SetError("PsxXaDecoder received a null sector pointer.", error);
        return nullptr;
    }

    const auto* bytes = static_cast<const std::uint8_t*>(sectorBytes);
    if (size == kCdXaAudioPayloadSize || size == kCdXaSectorUserDataSize) {
        return bytes;
    }
    if (size == kNormalizedSectorSize) {
        return bytes + kCdXaSubheaderSize;
    }
    if (size == kRawSectorSize && HasSyncHeader(bytes)) {
        return bytes + kCdSyncHeaderSize + kCdHeaderSize + kCdXaSubheaderSize;
    }

    SetError("PsxXaDecoder expected a 2304-byte XA payload, 2324-byte XA user-data buffer, "
             "2336-byte normalized sector, or 2352-byte raw sector.",
             error);
    return nullptr;
}

int ClampSample(std::int32_t sample) {
    return std::clamp(sample, static_cast<std::int32_t>(-32768), static_cast<std::int32_t>(32767));
}

int DecodeShift(std::uint8_t headerByte, PsxXaBitsPerSample bitsPerSample) {
    int shift = static_cast<int>(headerByte & 0x0F);
    if (shift > 12) {
        shift = 9;
    }

    const int baseShift = (bitsPerSample == PsxXaBitsPerSample::Bits8) ? 8 : 12;
    return baseShift - shift;
}

std::int32_t ScaleAdpcmSample(int sample, int shift) {
    if (shift >= 0) {
        return static_cast<std::int32_t>(sample) * static_cast<std::int32_t>(1U << shift);
    }
    return sample >> -shift;
}

int DecodeFilter(std::uint8_t headerByte) {
    return static_cast<int>((headerByte >> 4) & 0x03);
}

int SignExtend4(std::uint8_t value) {
    value &= 0x0F;
    return ((value & 0x08) != 0) ? static_cast<int>(value) - 16 : static_cast<int>(value);
}

std::int32_t Predictor(const PsxXaDecodeState& state, int channel, int filter) {
    const std::int32_t sum = state.previousSample[channel] * kPositiveFilter[filter] +
                             state.olderSample[channel] * kNegativeFilter[filter];
    return (sum + 32) >> 6;
}

void Decode4BitBlock(const std::uint8_t* group,
                     int blockIndex,
                     int nibbleIndex,
                     int channel,
                     std::vector<std::int16_t>* output,
                     PsxXaDecodeState* state) {
    const std::uint8_t headerByte =
        group[4 + static_cast<std::size_t>(blockIndex) * 2 + static_cast<std::size_t>(nibbleIndex)];
    const int shift = DecodeShift(headerByte, PsxXaBitsPerSample::Bits4);
    const int filter = DecodeFilter(headerByte);

    for (std::size_t sampleIndex = 0; sampleIndex < kSamplesPerBlock; ++sampleIndex) {
        const std::uint8_t packed = group[kSoundGroupDataOffset +
                                          static_cast<std::size_t>(blockIndex) +
                                          sampleIndex * 4];
        const int code = (nibbleIndex == 0) ? SignExtend4(static_cast<std::uint8_t>(packed & 0x0F))
                                            : SignExtend4(static_cast<std::uint8_t>(packed >> 4));
        const std::int32_t sample = ScaleAdpcmSample(code, shift) + Predictor(*state, channel, filter);
        const std::int32_t clamped = ClampSample(sample);
        output->push_back(static_cast<std::int16_t>(clamped));
        state->olderSample[channel] = state->previousSample[channel];
        state->previousSample[channel] = clamped;
    }
}

void Decode8BitBlock(const std::uint8_t* group,
                     int blockPairIndex,
                     int laneIndex,
                     int channel,
                     std::vector<std::int16_t>* output,
                     PsxXaDecodeState* state) {
    const std::uint8_t headerByte =
        group[4 + static_cast<std::size_t>(blockPairIndex) * 2 + static_cast<std::size_t>(laneIndex)];
    const int shift = DecodeShift(headerByte, PsxXaBitsPerSample::Bits8);
    const int filter = DecodeFilter(headerByte);

    for (std::size_t sampleIndex = 0; sampleIndex < kSamplesPerBlock; ++sampleIndex) {
        const std::uint8_t packed = group[kSoundGroupDataOffset +
                                          static_cast<std::size_t>(blockPairIndex) * 2 +
                                          static_cast<std::size_t>(laneIndex) +
                                          sampleIndex * 4];
        const int code = static_cast<std::int8_t>(packed);
        const std::int32_t sample = ScaleAdpcmSample(code, shift) + Predictor(*state, channel, filter);
        const std::int32_t clamped = ClampSample(sample);
        output->push_back(static_cast<std::int16_t>(clamped));
        state->olderSample[channel] = state->previousSample[channel];
        state->previousSample[channel] = clamped;
    }
}

bool DecodeSectorPcm16(const std::uint8_t* payload,
                       const PsxXaAudioFormat& format,
                       PsxXaDecodeState* state,
                       PsxXaPcm16* output) {
    if (payload == nullptr || state == nullptr || output == nullptr) {
        return false;
    }

    std::array<std::vector<std::int16_t>, 2> channelSamples;
    channelSamples[0].reserve(format.sampleFramesPerSector);
    if (format.channelCount > 1) {
        channelSamples[1].reserve(format.sampleFramesPerSector);
    }

    for (std::size_t groupIndex = 0; groupIndex < kSoundGroupCountPerSector; ++groupIndex) {
        const std::uint8_t* group = payload + groupIndex * kSoundGroupSize;
        if (format.bitsPerSample == PsxXaBitsPerSample::Bits4) {
            for (int blockIndex = 0; blockIndex < 4; ++blockIndex) {
                if (format.stereo) {
                    Decode4BitBlock(group, blockIndex, 0, 0, &channelSamples[0], state);
                    Decode4BitBlock(group, blockIndex, 1, 1, &channelSamples[1], state);
                } else {
                    Decode4BitBlock(group, blockIndex, 0, 0, &channelSamples[0], state);
                    Decode4BitBlock(group, blockIndex, 1, 0, &channelSamples[0], state);
                }
            }
        } else {
            for (int blockPairIndex = 0; blockPairIndex < 2; ++blockPairIndex) {
                if (format.stereo) {
                    Decode8BitBlock(group, blockPairIndex, 0, 0, &channelSamples[0], state);
                    Decode8BitBlock(group, blockPairIndex, 1, 1, &channelSamples[1], state);
                } else {
                    Decode8BitBlock(group, blockPairIndex, 0, 0, &channelSamples[0], state);
                    Decode8BitBlock(group, blockPairIndex, 1, 0, &channelSamples[0], state);
                }
            }
        }
    }

    output->sampleRate = format.sampleRate;
    output->channelCount = format.channelCount;
    output->sampleFrames = format.sampleFramesPerSector;
    output->samples.clear();
    output->samples.resize(format.samplesPerSector);

    if (format.channelCount == 1) {
        output->samples = std::move(channelSamples[0]);
        return true;
    }

    for (std::size_t frameIndex = 0; frameIndex < format.sampleFramesPerSector; ++frameIndex) {
        output->samples[frameIndex * 2] = channelSamples[0][frameIndex];
        output->samples[frameIndex * 2 + 1] = channelSamples[1][frameIndex];
    }
    return true;
}

bool DecodeSectorPcmFloat(const std::uint8_t* payload,
                         const PsxXaAudioFormat& format,
                         PsxXaDecodeState* state,
                         PsxXaPcmFloat* output) {
    if (payload == nullptr || state == nullptr || output == nullptr) {
        return false;
    }

    PsxXaPcm16 pcm16;
    if (!DecodeSectorPcm16(payload, format, state, &pcm16)) {
        return false;
    }

    output->sampleRate = pcm16.sampleRate;
    output->channelCount = pcm16.channelCount;
    output->sampleFrames = pcm16.sampleFrames;
    output->samples.resize(pcm16.samples.size());
    for (std::size_t i = 0; i < pcm16.samples.size(); ++i) {
        output->samples[i] = static_cast<float>(pcm16.samples[i]) / 32768.0f;
    }
    return true;
}

} // namespace

bool ParsePsxXaAudioFormat(std::uint8_t codingInfo, PsxXaAudioFormat* format) noexcept {
    if (format == nullptr) {
        return false;
    }

    PsxXaAudioFormat parsed;
    parsed.codingInfo = codingInfo;
    parsed.reservedBits = static_cast<std::uint8_t>(codingInfo & kCodingInfoReservedMask);
    parsed.emphasis = (codingInfo & kCodingInfoEmphasis) != 0;
    parsed.stereo = (codingInfo & kCodingInfoStereo) != 0;
    parsed.channelCount = parsed.stereo ? 2 : 1;
    parsed.sampleRate = (codingInfo & kCodingInfoSampleRate18900) != 0 ? 18900 : 37800;
    parsed.bitsPerSample = (codingInfo & kCodingInfoBits8) != 0 ? PsxXaBitsPerSample::Bits8
                                                                : PsxXaBitsPerSample::Bits4;

    parsed.samplesPerSector = (parsed.bitsPerSample == PsxXaBitsPerSample::Bits8) ? 2016 : 4032;
    parsed.sampleFramesPerSector = parsed.samplesPerSector / parsed.channelCount;

    *format = parsed;
    return true;
}

bool ParsePsxXaAudioFormat(const CdXaSectorHeader& header, PsxXaAudioFormat* format) noexcept {
    if (!header.IsAudio()) {
        return false;
    }
    return ParsePsxXaAudioFormat(header.codingInfo, format);
}

void PsxXaDecodeState::Reset() noexcept {
    previousSample[0] = 0;
    previousSample[1] = 0;
    olderSample[0] = 0;
    olderSample[1] = 0;
}

void PsxXaDecoder::Reset() noexcept {
    state_.Reset();
}

const PsxXaDecodeState& PsxXaDecoder::State() const noexcept {
    return state_;
}

bool PsxXaDecoder::DecodeSector(const PsxStrSector& sector, PsxXaPcm16* output, std::string* error) {
    if (!sector.cdxa.IsAudio()) {
        SetError("PsxXaDecoder::DecodeSector expected an audio sector.", error);
        return false;
    }
    return DecodeSector(sector.sectorBytes.data(), sector.sectorBytes.size(), sector.cdxa.codingInfo, output, error);
}

bool PsxXaDecoder::DecodeSector(const PsxStrSector& sector, PsxXaPcmFloat* output, std::string* error) {
    if (!sector.cdxa.IsAudio()) {
        SetError("PsxXaDecoder::DecodeSector expected an audio sector.", error);
        return false;
    }
    return DecodeSector(sector.sectorBytes.data(), sector.sectorBytes.size(), sector.cdxa.codingInfo, output, error);
}

bool PsxXaDecoder::DecodeSector(const void* sectorBytes,
                                std::size_t size,
                                std::uint8_t codingInfo,
                                PsxXaPcm16* output,
                                std::string* error) {
    if (output == nullptr) {
        SetError("PsxXaDecoder::DecodeSector requires a non-null PCM16 output buffer.", error);
        return false;
    }

    PsxXaAudioFormat format;
    if (!ParsePsxXaAudioFormat(codingInfo, &format)) {
        SetError("PsxXaDecoder could not parse XA audio coding info.", error);
        return false;
    }

    const std::uint8_t* payload = ResolveAudioPayload(sectorBytes, size, error);
    if (payload == nullptr) {
        return false;
    }

    return DecodeSectorPcm16(payload, format, &state_, output);
}

bool PsxXaDecoder::DecodeSector(const void* sectorBytes,
                                std::size_t size,
                                std::uint8_t codingInfo,
                                PsxXaPcmFloat* output,
                                std::string* error) {
    if (output == nullptr) {
        SetError("PsxXaDecoder::DecodeSector requires a non-null float output buffer.", error);
        return false;
    }

    PsxXaAudioFormat format;
    if (!ParsePsxXaAudioFormat(codingInfo, &format)) {
        SetError("PsxXaDecoder could not parse XA audio coding info.", error);
        return false;
    }

    const std::uint8_t* payload = ResolveAudioPayload(sectorBytes, size, error);
    if (payload == nullptr) {
        return false;
    }

    return DecodeSectorPcmFloat(payload, format, &state_, output);
}

} // namespace Inferno::Psx
