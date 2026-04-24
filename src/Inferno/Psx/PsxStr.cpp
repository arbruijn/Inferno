#include "PsxStr.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <istream>
#include <map>
#include <utility>

namespace Inferno::Psx {
namespace {

constexpr std::uint8_t kSubmodeEndOfFile = 0x80;
constexpr std::uint8_t kSubmodeRealTime = 0x40;
constexpr std::uint8_t kSubmodeForm2 = 0x20;
constexpr std::uint8_t kSubmodeData = 0x08;
constexpr std::uint8_t kSubmodeAudio = 0x04;
constexpr std::uint8_t kSubmodeVideo = 0x02;

constexpr std::size_t kCdXaSubheaderSize = 8;
constexpr std::size_t kStrChunkHeaderSize = 32;
constexpr std::size_t kSectorPayloadSize = 2336;
constexpr std::size_t kChunkEccSize = 280;

bool Match4(const std::uint8_t* data, const char* text) {
    return data[0] == static_cast<std::uint8_t>(text[0]) &&
           data[1] == static_cast<std::uint8_t>(text[1]) &&
           data[2] == static_cast<std::uint8_t>(text[2]) &&
           data[3] == static_cast<std::uint8_t>(text[3]);
}

std::uint16_t ReadU16LE(const std::uint8_t* data) {
    return static_cast<std::uint16_t>(data[0] | (static_cast<std::uint16_t>(data[1]) << 8));
}

std::uint32_t ReadU32LE(const std::uint8_t* data) {
    return static_cast<std::uint32_t>(data[0]) |
           (static_cast<std::uint32_t>(data[1]) << 8) |
           (static_cast<std::uint32_t>(data[2]) << 16) |
           (static_cast<std::uint32_t>(data[3]) << 24);
}

bool HasSyncHeader(const std::uint8_t* data) {
    static constexpr std::uint8_t kSync[12] = {
        0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
    };
    return std::memcmp(data, kSync, sizeof(kSync)) == 0;
}

bool HasDuplicatedXaHeader(const std::uint8_t* data) {
    return data[0] == data[4] &&
           data[1] == data[5] &&
           data[2] == data[6] &&
           data[3] == data[7];
}

PsxStrSectorLayout DetectSectorLayoutFromPrefix(const std::uint8_t* data, std::size_t size) {
    if (size >= 12 && HasSyncHeader(data)) {
        return PsxStrSectorLayout::Raw2352;
    }

    if (size >= 8 && HasDuplicatedXaHeader(data)) {
        return PsxStrSectorLayout::Raw2336;
    }

    return PsxStrSectorLayout::Unknown;
}

PsxStrSectorKind ClassifySector(const CdXaSectorHeader& header) {
    if (header.IsVideo()) {
        return PsxStrSectorKind::Video;
    }
    if (header.IsAudio()) {
        return PsxStrSectorKind::Audio;
    }
    if (header.IsData()) {
        return PsxStrSectorKind::Data;
    }
    return PsxStrSectorKind::Unknown;
}

PsxStrPacketKind PacketKindForSector(const PsxStrSector& sector) {
    switch (sector.kind) {
    case PsxStrSectorKind::Audio:
        return PsxStrPacketKind::AudioSector;
    case PsxStrSectorKind::Data:
        return PsxStrPacketKind::DataSector;
    case PsxStrSectorKind::Video:
        return PsxStrPacketKind::UnknownSector;
    case PsxStrSectorKind::Unknown:
        return PsxStrPacketKind::UnknownSector;
    }
    return PsxStrPacketKind::UnknownSector;
}

CdXaSectorHeader ParseCdXaHeader(const std::uint8_t* data) {
    CdXaSectorHeader header;
    header.fileNumber = data[0];
    header.channelNumber = data[1];
    header.submode = data[2];
    header.codingInfo = data[3];
    header.duplicateFileNumber = data[4];
    header.duplicateChannelNumber = data[5];
    header.duplicateSubmode = data[6];
    header.duplicateCodingInfo = data[7];
    return header;
}

StrFrameChunkHeader ParseStrFrameChunkHeader(const std::uint8_t* data) {
    StrFrameChunkHeader header;
    header.magic = ReadU32LE(data + 0);
    header.chunkIndex = ReadU16LE(data + 4);
    header.chunkCount = ReadU16LE(data + 6);
    header.frameNumber = ReadU32LE(data + 8);
    header.usedBytes = ReadU32LE(data + 12);
    header.width = ReadU16LE(data + 16);
    header.height = ReadU16LE(data + 18);
    header.mdecCodeCountHalfRounded = ReadU16LE(data + 20);
    header.constant3800 = ReadU16LE(data + 22);
    header.quantizationScale = ReadU16LE(data + 24);
    header.version = ReadU16LE(data + 26);
    header.reserved = ReadU32LE(data + 28);
    return header;
}

std::vector<std::uint8_t> NormalizeSector(const std::uint8_t* data, PsxStrSectorLayout layout) {
    if (layout == PsxStrSectorLayout::Raw2352) {
        return std::vector<std::uint8_t>(data + 16, data + 16 + kSectorPayloadSize);
    }
    if (layout == PsxStrSectorLayout::Raw2336) {
        return std::vector<std::uint8_t>(data, data + kSectorPayloadSize);
    }
    return {};
}

PsxStrFrameIndexEntry MakeFrameIndexEntry(const PsxStrFrame& frame) {
    PsxStrFrameIndexEntry entry;
    entry.frameNumber = frame.frameNumber;
    entry.width = frame.width;
    entry.height = frame.height;
    entry.quantizationScale = frame.quantizationScale;
    entry.version = frame.version;
    entry.chunkCount = frame.chunkCount;
    entry.usedBytes = frame.usedBytes;
    entry.complete = frame.complete;
    entry.headerConsistent = frame.headerConsistent;
    entry.sectorIndices = frame.sectorIndices;
    entry.chunkIndices = frame.chunkIndices;
    return entry;
}

} // namespace

bool CdXaSectorHeader::IsConsistent() const noexcept {
    return fileNumber == duplicateFileNumber &&
           channelNumber == duplicateChannelNumber &&
           submode == duplicateSubmode &&
           codingInfo == duplicateCodingInfo;
}

bool CdXaSectorHeader::IsEndOfFile() const noexcept {
    return (submode & kSubmodeEndOfFile) != 0;
}

bool CdXaSectorHeader::IsRealTime() const noexcept {
    return (submode & kSubmodeRealTime) != 0;
}

bool CdXaSectorHeader::IsForm2() const noexcept {
    return (submode & kSubmodeForm2) != 0;
}

bool CdXaSectorHeader::IsData() const noexcept {
    return (submode & kSubmodeData) != 0;
}

bool CdXaSectorHeader::IsAudio() const noexcept {
    return (submode & kSubmodeAudio) != 0;
}

bool CdXaSectorHeader::IsVideo() const noexcept {
    return (submode & kSubmodeVideo) != 0;
}

bool StrFrameChunkHeader::LooksLikeStrHeader() const noexcept {
    return magic == 0x80010160 &&
           constant3800 == 0x3800 &&
           width != 0 &&
           height != 0;
}

bool PsxStrSector::HasVideoChunkHeader() const noexcept {
    return kind == PsxStrSectorKind::Video && chunkHeader.has_value();
}

std::vector<std::uint8_t> PsxStrSector::ChunkData() const {
    if (sectorBytes.size() < kCdXaSubheaderSize + kStrChunkHeaderSize + kChunkEccSize) {
        return {};
    }

    const std::size_t dataOffset = kCdXaSubheaderSize + kStrChunkHeaderSize;
    return std::vector<std::uint8_t>(sectorBytes.begin() + static_cast<std::ptrdiff_t>(dataOffset),
                                     sectorBytes.end() - static_cast<std::ptrdiff_t>(kChunkEccSize));
}

void PsxStrReader::Clear() noexcept {
    sourceKind_ = SourceKind::None;
    readMode_ = ReadMode::None;
    stream_ = nullptr;
    customStream_ = nullptr;
    memoryData_ = nullptr;
    memorySize_ = 0;
    memoryOffset_ = 0;
    payloadLayout_ = PsxStrSectorLayout::Unknown;
    payloadBytesRemaining_.reset();
    payloadPaddingBytes_ = 0;
    sourceBytesExhausted_ = false;
    finished_ = false;
    pendingBytes_.clear();
    pendingOffset_ = 0;
    nextSectorIndex_ = 0;
    nextSectorSourceOffset_ = 0;
    bufferedSector_.reset();
    packetFrameActive_ = false;
    packetFrame_ = PsxStrFrame{};
    packetFrameChunks_.clear();
    audioFormatConsistent_ = true;
    previousAudioSectorIndex_.reset();
    audioSpacingCounts_.clear();
    info_ = PsxMovieInfo{};
    frameIndex_.clear();
    error_.clear();
}

void PsxStrReader::SetError(std::string message, std::string* error) const {
    error_ = std::move(message);
    if (error != nullptr) {
        *error = error_;
    }
}

bool PsxStrReader::Load(const void* data, std::size_t size, std::string* error) {
    Clear();

    if (data == nullptr && size != 0) {
        SetError("PsxStrReader::Load received a null data pointer.", error);
        return false;
    }
    if (size == 0) {
        SetError("PsxStrReader::Load requires a non-empty buffer.", error);
        return false;
    }

    sourceKind_ = SourceKind::Memory;
    memoryData_ = static_cast<const std::uint8_t*>(data);
    memorySize_ = size;
    memoryOffset_ = 0;
    return InitializeSource(error);
}

bool PsxStrReader::Load(const std::vector<std::uint8_t>& data, std::string* error) {
    return Load(data.data(), data.size(), error);
}

bool PsxStrReader::Load(std::istream& input, std::string* error) {
    Clear();
    sourceKind_ = SourceKind::Stream;
    stream_ = &input;
    return InitializeSource(error);
}

bool PsxStrReader::Load(PsxReadStream* input, std::string* error) {
    Clear();
    if (input == nullptr) {
        SetError("PsxStrReader::Load requires a non-null PsxReadStream.", error);
        return false;
    }
    sourceKind_ = SourceKind::CustomStream;
    customStream_ = input;
    return InitializeSource(error);
}

bool PsxStrReader::InitializeSource(std::string* error) {
    std::vector<std::uint8_t> header;
    if (!ReadSourceAtMost(12, &header, error)) {
        return false;
    }

    if (header.empty()) {
        SetError("PsxStrReader::Load requires a non-empty buffer.", error);
        return false;
    }

    if (header.size() == 12 && Match4(header.data(), "RIFF")) {
        while (true) {
            std::vector<std::uint8_t> chunkHeader;
            if (!ReadSourceAtMost(8, &chunkHeader, error)) {
                return false;
            }
            if (chunkHeader.empty()) {
                break;
            }
            if (chunkHeader.size() != 8) {
                SetError("PsxStrReader::Load encountered a truncated RIFF chunk header.", error);
                return false;
            }

            const std::size_t chunkSize = static_cast<std::size_t>(ReadU32LE(chunkHeader.data() + 4));
            const std::size_t probeSize = std::min<std::size_t>(chunkSize, 2352);

            std::vector<std::uint8_t> probe;
            if (!ReadSourceExactly(probeSize, &probe, error)) {
                SetError("PsxStrReader::Load encountered a truncated RIFF chunk payload.", error);
                return false;
            }

            const PsxStrSectorLayout layout = DetectSectorLayoutFromPrefix(probe.data(), probe.size());
            if (layout != PsxStrSectorLayout::Unknown) {
                return ConfigurePayload(layout, chunkSize, std::move(probe), true, error);
            }

            const std::size_t trailingBytes = chunkSize - probeSize + (chunkSize & 1U);
            if (!SkipSourceExactly(trailingBytes, error)) {
                SetError("PsxStrReader::Load encountered a truncated RIFF chunk payload.", error);
                return false;
            }
        }

        SetError("PsxStrReader could not recognize a 2336-byte or 2352-byte STR sector layout.", error);
        return false;
    }

    std::vector<std::uint8_t> prefix = std::move(header);
    if (prefix.size() < 2352) {
        if (!ReadSourceAtMost(2352 - prefix.size(), &prefix, error)) {
            return false;
        }
    }

    const PsxStrSectorLayout layout = DetectSectorLayoutFromPrefix(prefix.data(), prefix.size());
    if (layout == PsxStrSectorLayout::Unknown) {
        SetError("PsxStrReader could not recognize a 2336-byte or 2352-byte STR sector layout.", error);
        return false;
    }

    return ConfigurePayload(layout, std::nullopt, std::move(prefix), false, error);
}

bool PsxStrReader::ConfigurePayload(PsxStrSectorLayout layout,
                                    std::optional<std::size_t> payloadSize,
                                    std::vector<std::uint8_t> initialBytes,
                                    bool hasRiffWrapper,
                                    std::string* error) {
    if (payloadSize.has_value() && initialBytes.size() > *payloadSize) {
        SetError("PsxStrReader found a buffer whose size is not a whole number of STR sectors.", error);
        return false;
    }

    info_.hasRiffWrapper = hasRiffWrapper;
    info_.sectorLayout = layout;
    payloadLayout_ = layout;
    payloadBytesRemaining_ = payloadSize;
    if (payloadBytesRemaining_.has_value()) {
        *payloadBytesRemaining_ -= initialBytes.size();
        payloadPaddingBytes_ = *payloadSize & 1U;
    } else {
        payloadPaddingBytes_ = 0;
    }
    sourceBytesExhausted_ = false;
    finished_ = false;
    pendingBytes_ = std::move(initialBytes);
    pendingOffset_ = 0;
    nextSectorIndex_ = 0;
    nextSectorSourceOffset_ = 0;
    bufferedSector_.reset();
    packetFrameActive_ = false;
    packetFrame_ = PsxStrFrame{};
    packetFrameChunks_.clear();
    return true;
}

bool PsxStrReader::ReadSourceAtMost(std::size_t byteCount,
                                    std::vector<std::uint8_t>* output,
                                    std::string* error) {
    if (output == nullptr) {
        SetError("PsxStrReader encountered a null byte buffer.", error);
        return false;
    }
    if (byteCount == 0) {
        return true;
    }

    if (sourceKind_ == SourceKind::Memory) {
        const std::size_t remaining = (memoryOffset_ < memorySize_) ? (memorySize_ - memoryOffset_) : 0;
        const std::size_t bytesToCopy = std::min(byteCount, remaining);
        output->insert(output->end(),
                       memoryData_ + static_cast<std::ptrdiff_t>(memoryOffset_),
                       memoryData_ + static_cast<std::ptrdiff_t>(memoryOffset_ + bytesToCopy));
        memoryOffset_ += bytesToCopy;
        return true;
    }

    if (sourceKind_ == SourceKind::CustomStream && customStream_ != nullptr) {
        const std::size_t originalSize = output->size();
        output->resize(originalSize + byteCount);

        std::size_t bytesRead = 0;
        if (!customStream_->Read(output->data() + originalSize, byteCount, &bytesRead, error)) {
            output->resize(originalSize);
            return false;
        }

        output->resize(originalSize + bytesRead);
        return true;
    }

    if (sourceKind_ != SourceKind::Stream || stream_ == nullptr) {
        SetError("PsxStrReader has no active input source.", error);
        return false;
    }

    const std::size_t originalSize = output->size();
    output->resize(originalSize + byteCount);
    stream_->read(reinterpret_cast<char*>(output->data() + originalSize),
                  static_cast<std::streamsize>(byteCount));
    const std::size_t bytesRead = static_cast<std::size_t>(stream_->gcount());
    output->resize(originalSize + bytesRead);

    if (stream_->fail() && !stream_->eof()) {
        SetError("PsxStrReader::Load failed while reading the input stream.", error);
        return false;
    }
    if (stream_->eof() && stream_->fail()) {
        stream_->clear(stream_->rdstate() & ~std::ios::failbit);
    }
    return true;
}

bool PsxStrReader::ReadSourceExactly(std::size_t byteCount,
                                     std::vector<std::uint8_t>* output,
                                     std::string* error) {
    const std::size_t before = output != nullptr ? output->size() : 0;
    if (!ReadSourceAtMost(byteCount, output, error)) {
        return false;
    }
    return output != nullptr && (output->size() - before) == byteCount;
}

bool PsxStrReader::SkipSourceExactly(std::size_t byteCount, std::string* error) {
    if (byteCount == 0) {
        return true;
    }

    if (sourceKind_ == SourceKind::Memory) {
        const std::size_t remaining = (memoryOffset_ < memorySize_) ? (memorySize_ - memoryOffset_) : 0;
        if (remaining < byteCount) {
            return false;
        }
        memoryOffset_ += byteCount;
        return true;
    }

    if (sourceKind_ == SourceKind::CustomStream && customStream_ != nullptr) {
        return customStream_->Skip(byteCount, error);
    }

    if (sourceKind_ != SourceKind::Stream || stream_ == nullptr) {
        SetError("PsxStrReader has no active input source.", error);
        return false;
    }

    std::array<char, 4096> discardBuffer{};
    std::size_t remaining = byteCount;
    while (remaining > 0) {
        const std::size_t step = std::min(remaining, discardBuffer.size());
        stream_->read(discardBuffer.data(), static_cast<std::streamsize>(step));
        const std::size_t bytesRead = static_cast<std::size_t>(stream_->gcount());
        if (bytesRead != step) {
            return false;
        }
        remaining -= bytesRead;
    }
    return true;
}

const PsxMovieInfo& PsxStrReader::Info() const noexcept {
    return info_;
}

const std::vector<PsxStrFrameIndexEntry>& PsxStrReader::FrameIndex() const noexcept {
    return frameIndex_;
}

std::size_t PsxStrReader::FrameCount() const noexcept {
    return frameIndex_.size();
}

bool PsxStrReader::Finished() const noexcept {
    return finished_;
}

bool PsxStrReader::AudioFormatConsistent() const noexcept {
    return audioFormatConsistent_;
}

std::optional<std::size_t> PsxStrReader::DominantAudioSectorSpacing() const noexcept {
    if (audioSpacingCounts_.empty()) {
        return std::nullopt;
    }

    const auto best = std::max_element(
        audioSpacingCounts_.begin(),
        audioSpacingCounts_.end(),
        [](const auto& lhs, const auto& rhs) {
            if (lhs.second != rhs.second) {
                return lhs.second < rhs.second;
            }
            return lhs.first > rhs.first;
        });
    return best->first;
}

const std::string& PsxStrReader::Error() const noexcept {
    return error_;
}

bool PsxStrReader::FillPendingBytes(std::size_t byteCount, std::string* error) {
    if (pendingOffset_ != 0) {
        pendingBytes_.erase(pendingBytes_.begin(),
                            pendingBytes_.begin() + static_cast<std::ptrdiff_t>(pendingOffset_));
        pendingOffset_ = 0;
    }

    while (pendingBytes_.size() < byteCount && !sourceBytesExhausted_) {
        if (payloadBytesRemaining_.has_value() && *payloadBytesRemaining_ == 0) {
            if (payloadPaddingBytes_ != 0) {
                if (!SkipSourceExactly(payloadPaddingBytes_, error)) {
                    SetError("PsxStrReader::Load encountered a truncated RIFF chunk payload.", error);
                    return false;
                }
                payloadPaddingBytes_ = 0;
            }
            sourceBytesExhausted_ = true;
            break;
        }

        const std::size_t previousSize = pendingBytes_.size();
        std::size_t bytesToRead = byteCount - previousSize;
        if (payloadBytesRemaining_.has_value()) {
            bytesToRead = std::min(bytesToRead, *payloadBytesRemaining_);
        }

        if (!ReadSourceAtMost(bytesToRead, &pendingBytes_, error)) {
            return false;
        }

        const std::size_t bytesRead = pendingBytes_.size() - previousSize;
        if (payloadBytesRemaining_.has_value()) {
            *payloadBytesRemaining_ -= bytesRead;
        }

        if (bytesRead == bytesToRead) {
            continue;
        }

        if (payloadBytesRemaining_.has_value()) {
            SetError("PsxStrReader::Load encountered a truncated RIFF STR payload.", error);
            return false;
        }

        sourceBytesExhausted_ = true;
        break;
    }

    return true;
}

bool PsxStrReader::BuildSector(const std::uint8_t* rawSector,
                               std::size_t sectorIndex,
                               std::size_t sourceOffset,
                               PsxStrSectorLayout layout,
                               PsxStrSector* sector,
                               std::string* error) const {
    if (sector == nullptr) {
        SetError("PsxStrReader::ReadNextSector requires a non-null output sector.", error);
        return false;
    }

    PsxStrSector result;
    result.index = sectorIndex;
    result.sourceOffset = sourceOffset;
    result.layout = layout;
    result.sectorBytes = NormalizeSector(rawSector, layout);
    if (result.sectorBytes.size() != kSectorPayloadSize) {
        SetError("PsxStrReader failed to normalize one or more sectors.", error);
        return false;
    }

    result.cdxa = ParseCdXaHeader(result.sectorBytes.data());
    result.kind = ClassifySector(result.cdxa);
    if (result.kind == PsxStrSectorKind::Video &&
        result.sectorBytes.size() >= kCdXaSubheaderSize + kStrChunkHeaderSize) {
        result.chunkHeader = ParseStrFrameChunkHeader(result.sectorBytes.data() + kCdXaSubheaderSize);
    }

    *sector = std::move(result);
    return true;
}

void PsxStrReader::UpdateInfoFromSector(const PsxStrSector& sector) {
    info_.sectorCount = sector.index + 1;

    switch (sector.kind) {
    case PsxStrSectorKind::Video:
        ++info_.videoSectorCount;
        if (info_.videoSubmode == 0) {
            info_.videoSubmode = sector.cdxa.submode;
        }
        break;
    case PsxStrSectorKind::Audio:
        ++info_.audioSectorCount;
        if (info_.audioSubmode == 0) {
            info_.audioSubmode = sector.cdxa.submode;
            info_.audioCodingInfo = sector.cdxa.codingInfo;
        } else if (sector.cdxa.codingInfo != info_.audioCodingInfo) {
            audioFormatConsistent_ = false;
        }

        if (previousAudioSectorIndex_.has_value() && sector.index > *previousAudioSectorIndex_) {
            ++audioSpacingCounts_[sector.index - *previousAudioSectorIndex_];
        }
        previousAudioSectorIndex_ = sector.index;
        break;
    case PsxStrSectorKind::Data:
        ++info_.dataSectorCount;
        break;
    case PsxStrSectorKind::Unknown:
        ++info_.unknownSectorCount;
        break;
    }
}

void PsxStrReader::UpdateInfoFromFrame(const PsxStrFrame& frame) {
    ++info_.frameCount;
    if (info_.frameCount == 1) {
        info_.firstFrameNumber = frame.frameNumber;
        info_.lastFrameNumber = frame.frameNumber;
        info_.videoWidth = frame.width;
        info_.videoHeight = frame.height;
        info_.videoVersion = frame.version;
        info_.minChunksPerFrame = frame.chunkCount;
        info_.maxChunksPerFrame = frame.chunkCount;
        return;
    }

    if (frame.frameNumber < info_.firstFrameNumber) {
        info_.firstFrameNumber = frame.frameNumber;
    }
    if (frame.frameNumber > info_.lastFrameNumber) {
        info_.lastFrameNumber = frame.frameNumber;
    }

    if (info_.videoWidth == 0 && frame.width != 0 && frame.height != 0) {
        info_.videoWidth = frame.width;
        info_.videoHeight = frame.height;
        info_.videoVersion = frame.version;
    }

    info_.minChunksPerFrame = std::min(info_.minChunksPerFrame, frame.chunkCount);
    info_.maxChunksPerFrame = std::max(info_.maxChunksPerFrame, frame.chunkCount);
}

bool PsxStrReader::PacketFrameIsComplete() const noexcept {
    if (!packetFrameActive_ || packetFrame_.chunkCount == 0) {
        return false;
    }
    if (packetFrameChunks_.size() != packetFrame_.chunkCount) {
        return false;
    }
    std::size_t expectedChunkIndex = 0;
    for (const auto& chunkEntry : packetFrameChunks_) {
        if (chunkEntry.first != expectedChunkIndex) {
            return false;
        }
        ++expectedChunkIndex;
    }
    return true;
}

PsxStrFrame PsxStrReader::FinalizePacketFrame() const {
    PsxStrFrame frame = packetFrame_;
    frame.sectorIndices.clear();
    frame.chunkIndices.clear();
    frame.demuxedData.clear();
    if (frame.usedBytes != 0) {
        frame.demuxedData.reserve(static_cast<std::size_t>(frame.usedBytes));
    }

    for (const auto& chunkEntry : packetFrameChunks_) {
        frame.chunkIndices.push_back(chunkEntry.first);
        frame.sectorIndices.push_back(chunkEntry.second.sectorIndex);
        frame.demuxedData.insert(frame.demuxedData.end(),
                                 chunkEntry.second.chunkData.begin(),
                                 chunkEntry.second.chunkData.end());
    }

    frame.complete = PacketFrameIsComplete();
    if (frame.usedBytes != 0 && frame.usedBytes < frame.demuxedData.size()) {
        frame.demuxedData.resize(static_cast<std::size_t>(frame.usedBytes));
    }

    return frame;
}

PsxStrReadStatus PsxStrReader::ReadNextSectorInternal(PsxStrSector* sector, std::string* error) {
    if (payloadLayout_ == PsxStrSectorLayout::Unknown) {
        SetError("PsxStrReader::Load must succeed before reading sectors.", error);
        return PsxStrReadStatus::Error;
    }

    const std::size_t sectorSize = (payloadLayout_ == PsxStrSectorLayout::Raw2352) ? 2352 : 2336;
    if (!FillPendingBytes(sectorSize, error)) {
        finished_ = true;
        return PsxStrReadStatus::Error;
    }

    const std::size_t available = pendingBytes_.size() - pendingOffset_;
    if (available == 0 && sourceBytesExhausted_) {
        finished_ = true;
        return PsxStrReadStatus::EndOfStream;
    }
    if (available < sectorSize) {
        SetError("PsxStrReader found a buffer whose size is not a whole number of STR sectors.", error);
        finished_ = true;
        return PsxStrReadStatus::Error;
    }

    if (!BuildSector(pendingBytes_.data() + pendingOffset_,
                     nextSectorIndex_,
                     nextSectorSourceOffset_,
                     payloadLayout_,
                     sector,
                     error)) {
        finished_ = true;
        return PsxStrReadStatus::Error;
    }

    pendingOffset_ += sectorSize;
    nextSectorSourceOffset_ += sectorSize;
    ++nextSectorIndex_;
    if (pendingOffset_ == pendingBytes_.size()) {
        pendingBytes_.clear();
        pendingOffset_ = 0;
    }

    UpdateInfoFromSector(*sector);
    return PsxStrReadStatus::Ok;
}

PsxStrReadStatus PsxStrReader::ReadNextSector(PsxStrSector* sector, std::string* error) {
    error_.clear();
    if (error != nullptr) {
        error->clear();
    }

    if (readMode_ == ReadMode::Frame || readMode_ == ReadMode::Packet) {
        SetError("PsxStrReader cannot switch from frame or packet iteration back to sector iteration.", error);
        return PsxStrReadStatus::Error;
    }
    readMode_ = ReadMode::Sector;

    return ReadNextSectorInternal(sector, error);
}

PsxStrReadStatus PsxStrReader::ReadNextPacket(PsxStrPacket* packet, std::string* error) {
    error_.clear();
    if (error != nullptr) {
        error->clear();
    }

    if (packet == nullptr) {
        SetError("PsxStrReader::ReadNextPacket requires a non-null output packet.", error);
        return PsxStrReadStatus::Error;
    }
    if (readMode_ == ReadMode::Sector || readMode_ == ReadMode::Frame) {
        SetError("PsxStrReader cannot switch from sector or frame iteration to packet iteration.", error);
        return PsxStrReadStatus::Error;
    }
    readMode_ = ReadMode::Packet;

    while (true) {
        PsxStrSector sector;
        PsxStrReadStatus status = PsxStrReadStatus::Ok;
        if (bufferedSector_.has_value()) {
            sector = std::move(*bufferedSector_);
            bufferedSector_.reset();
        } else {
            status = ReadNextSectorInternal(&sector, error);
            if (status == PsxStrReadStatus::Error) {
                return status;
            }
            if (status == PsxStrReadStatus::EndOfStream) {
                finished_ = true;
                if (!packetFrameActive_) {
                    return PsxStrReadStatus::EndOfStream;
                }

                PsxStrFrame frameValue = FinalizePacketFrame();
                UpdateInfoFromFrame(frameValue);
                frameIndex_.push_back(MakeFrameIndexEntry(frameValue));
                packetFrameActive_ = false;
                packetFrame_ = PsxStrFrame{};
                packetFrameChunks_.clear();

                *packet = PsxStrPacket{};
                packet->kind = PsxStrPacketKind::VideoFrame;
                packet->frame = std::move(frameValue);
                return PsxStrReadStatus::Ok;
            }
        }

        if (!sector.HasVideoChunkHeader()) {
            *packet = PsxStrPacket{};
            packet->kind = PacketKindForSector(sector);
            packet->sector = std::move(sector);
            return PsxStrReadStatus::Ok;
        }

        const StrFrameChunkHeader& header = *sector.chunkHeader;
        if (!packetFrameActive_) {
            packetFrameActive_ = true;
            packetFrame_ = PsxStrFrame{};
            packetFrameChunks_.clear();
            packetFrame_.frameNumber = header.frameNumber;
            packetFrame_.width = header.width;
            packetFrame_.height = header.height;
            packetFrame_.quantizationScale = header.quantizationScale;
            packetFrame_.version = header.version;
            packetFrame_.chunkCount = header.chunkCount;
            packetFrame_.usedBytes = header.usedBytes;
            packetFrame_.headerConsistent = true;
        } else if (header.frameNumber != packetFrame_.frameNumber) {
            bufferedSector_ = std::move(sector);

            PsxStrFrame frameValue = FinalizePacketFrame();
            UpdateInfoFromFrame(frameValue);
            frameIndex_.push_back(MakeFrameIndexEntry(frameValue));
            packetFrameActive_ = false;
            packetFrame_ = PsxStrFrame{};
            packetFrameChunks_.clear();

            *packet = PsxStrPacket{};
            packet->kind = PsxStrPacketKind::VideoFrame;
            packet->frame = std::move(frameValue);
            return PsxStrReadStatus::Ok;
        } else {
            if (packetFrame_.width != header.width ||
                packetFrame_.height != header.height ||
                packetFrame_.version != header.version ||
                packetFrame_.quantizationScale != header.quantizationScale) {
                packetFrame_.headerConsistent = false;
            }
            if (packetFrame_.usedBytes < header.usedBytes) {
                packetFrame_.usedBytes = header.usedBytes;
            }
            if (packetFrame_.chunkCount == 0) {
                packetFrame_.chunkCount = header.chunkCount;
            }
        }

        packetFrameChunks_.emplace(header.chunkIndex,
                                   PacketFrameChunkRecord{sector.index, sector.ChunkData()});

        if (PacketFrameIsComplete()) {
            PsxStrFrame frameValue = FinalizePacketFrame();
            UpdateInfoFromFrame(frameValue);
            frameIndex_.push_back(MakeFrameIndexEntry(frameValue));
            packetFrameActive_ = false;
            packetFrame_ = PsxStrFrame{};
            packetFrameChunks_.clear();

            *packet = PsxStrPacket{};
            packet->kind = PsxStrPacketKind::VideoFrame;
            packet->frame = std::move(frameValue);
            return PsxStrReadStatus::Ok;
        }
    }
}

} // namespace Inferno::Psx
