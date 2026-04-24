#pragma once

#include <cstddef>
#include <cstdint>
#include <istream>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "PsxStream.h"

namespace Inferno::Psx {

enum class PsxStrSectorLayout {
    Unknown = 0,
    Raw2336,
    Raw2352,
};

enum class PsxStrSectorKind {
    Unknown = 0,
    Data,
    Audio,
    Video,
};

enum class PsxStrReadStatus {
    Ok = 0,
    EndOfStream,
    Error,
};

enum class PsxStrPacketKind {
    Unknown = 0,
    VideoFrame,
    AudioSector,
    DataSector,
    UnknownSector,
};

struct CdXaSectorHeader {
    std::uint8_t fileNumber = 0;
    std::uint8_t channelNumber = 0;
    std::uint8_t submode = 0;
    std::uint8_t codingInfo = 0;
    std::uint8_t duplicateFileNumber = 0;
    std::uint8_t duplicateChannelNumber = 0;
    std::uint8_t duplicateSubmode = 0;
    std::uint8_t duplicateCodingInfo = 0;

    bool IsConsistent() const noexcept;
    bool IsEndOfFile() const noexcept;
    bool IsRealTime() const noexcept;
    bool IsForm2() const noexcept;
    bool IsData() const noexcept;
    bool IsAudio() const noexcept;
    bool IsVideo() const noexcept;
};

struct StrFrameChunkHeader {
    std::uint32_t magic = 0;
    std::uint16_t chunkIndex = 0;
    std::uint16_t chunkCount = 0;
    std::uint32_t frameNumber = 0;
    std::uint32_t usedBytes = 0;
    std::uint16_t width = 0;
    std::uint16_t height = 0;
    std::uint16_t mdecCodeCountHalfRounded = 0;
    std::uint16_t constant3800 = 0;
    std::uint16_t quantizationScale = 0;
    std::uint16_t version = 0;
    std::uint32_t reserved = 0;

    bool LooksLikeStrHeader() const noexcept;
};

struct PsxStrSector {
    std::size_t index = 0;
    std::size_t sourceOffset = 0;
    PsxStrSectorLayout layout = PsxStrSectorLayout::Unknown;
    CdXaSectorHeader cdxa;
    PsxStrSectorKind kind = PsxStrSectorKind::Unknown;
    std::vector<std::uint8_t> sectorBytes;
    std::optional<StrFrameChunkHeader> chunkHeader;

    bool HasVideoChunkHeader() const noexcept;
    std::vector<std::uint8_t> ChunkData() const;
};

struct PsxStrFrame {
    std::uint32_t frameNumber = 0;
    std::uint16_t width = 0;
    std::uint16_t height = 0;
    std::uint16_t quantizationScale = 0;
    std::uint16_t version = 0;
    std::uint16_t chunkCount = 0;
    std::uint32_t usedBytes = 0;
    bool complete = false;
    bool headerConsistent = true;
    std::vector<std::size_t> sectorIndices;
    std::vector<std::uint16_t> chunkIndices;
    std::vector<std::uint8_t> demuxedData;
};

struct PsxStrFrameIndexEntry {
    std::uint32_t frameNumber = 0;
    std::uint16_t width = 0;
    std::uint16_t height = 0;
    std::uint16_t quantizationScale = 0;
    std::uint16_t version = 0;
    std::uint16_t chunkCount = 0;
    std::uint32_t usedBytes = 0;
    bool complete = false;
    bool headerConsistent = true;
    std::vector<std::size_t> sectorIndices;
    std::vector<std::uint16_t> chunkIndices;
};

struct PsxMovieInfo {
    bool hasRiffWrapper = false;
    PsxStrSectorLayout sectorLayout = PsxStrSectorLayout::Unknown;
    std::size_t sectorCount = 0;
    std::size_t videoSectorCount = 0;
    std::size_t audioSectorCount = 0;
    std::size_t dataSectorCount = 0;
    std::size_t unknownSectorCount = 0;
    std::size_t frameCount = 0;
    std::uint32_t firstFrameNumber = 0;
    std::uint32_t lastFrameNumber = 0;
    std::uint16_t videoWidth = 0;
    std::uint16_t videoHeight = 0;
    std::uint16_t videoVersion = 0;
    std::uint16_t minChunksPerFrame = 0;
    std::uint16_t maxChunksPerFrame = 0;
    std::uint8_t videoSubmode = 0;
    std::uint8_t audioSubmode = 0;
    std::uint8_t audioCodingInfo = 0;
};

struct PsxStrPacket {
    PsxStrPacketKind kind = PsxStrPacketKind::Unknown;
    std::optional<PsxStrFrame> frame;
    std::optional<PsxStrSector> sector;
};

class PsxStrReader {
public:
    bool Load(const void* data, std::size_t size, std::string* error = nullptr);
    bool Load(const std::vector<std::uint8_t>& data, std::string* error = nullptr);
    bool Load(std::istream& input, std::string* error = nullptr);
    bool Load(PsxReadStream* input, std::string* error = nullptr);

    PsxStrReadStatus ReadNextSector(PsxStrSector* sector, std::string* error = nullptr);
    PsxStrReadStatus ReadNextPacket(PsxStrPacket* packet, std::string* error = nullptr);

    const PsxMovieInfo& Info() const noexcept;
    const std::vector<PsxStrFrameIndexEntry>& FrameIndex() const noexcept;
    std::size_t FrameCount() const noexcept;
    bool Finished() const noexcept;
    bool AudioFormatConsistent() const noexcept;
    std::optional<std::size_t> DominantAudioSectorSpacing() const noexcept;
    const std::string& Error() const noexcept;

    void Clear() noexcept;

private:
    enum class SourceKind {
        None = 0,
        Memory,
        Stream,
        CustomStream,
    };

    enum class ReadMode {
        None = 0,
        Sector,
        Frame,
        Packet,
    };

    struct PacketFrameChunkRecord {
        std::size_t sectorIndex = 0;
        std::vector<std::uint8_t> chunkData;
    };

    bool InitializeSource(std::string* error);
    bool ConfigurePayload(PsxStrSectorLayout layout,
                          std::optional<std::size_t> payloadSize,
                          std::vector<std::uint8_t> initialBytes,
                          bool hasRiffWrapper,
                          std::string* error);
    bool ReadSourceAtMost(std::size_t byteCount,
                          std::vector<std::uint8_t>* output,
                          std::string* error);
    bool ReadSourceExactly(std::size_t byteCount,
                           std::vector<std::uint8_t>* output,
                           std::string* error);
    bool SkipSourceExactly(std::size_t byteCount, std::string* error);
    bool FillPendingBytes(std::size_t byteCount, std::string* error);
    PsxStrReadStatus ReadNextSectorInternal(PsxStrSector* sector, std::string* error);
    bool BuildSector(const std::uint8_t* rawSector,
                     std::size_t sectorIndex,
                     std::size_t sourceOffset,
                     PsxStrSectorLayout layout,
                     PsxStrSector* sector,
                     std::string* error) const;
    void UpdateInfoFromSector(const PsxStrSector& sector);
    void UpdateInfoFromFrame(const PsxStrFrame& frame);
    bool PacketFrameIsComplete() const noexcept;
    PsxStrFrame FinalizePacketFrame() const;
    void SetError(std::string message, std::string* error) const;

    SourceKind sourceKind_ = SourceKind::None;
    ReadMode readMode_ = ReadMode::None;
    std::istream* stream_ = nullptr;
    PsxReadStream* customStream_ = nullptr;
    const std::uint8_t* memoryData_ = nullptr;
    std::size_t memorySize_ = 0;
    std::size_t memoryOffset_ = 0;

    PsxStrSectorLayout payloadLayout_ = PsxStrSectorLayout::Unknown;
    std::optional<std::size_t> payloadBytesRemaining_;
    std::size_t payloadPaddingBytes_ = 0;
    bool sourceBytesExhausted_ = false;
    bool finished_ = false;
    std::vector<std::uint8_t> pendingBytes_;
    std::size_t pendingOffset_ = 0;
    std::size_t nextSectorIndex_ = 0;
    std::size_t nextSectorSourceOffset_ = 0;
    std::optional<PsxStrSector> bufferedSector_;
    bool packetFrameActive_ = false;
    PsxStrFrame packetFrame_;
    std::map<std::uint16_t, PacketFrameChunkRecord> packetFrameChunks_;

    bool audioFormatConsistent_ = true;
    std::optional<std::size_t> previousAudioSectorIndex_;
    std::map<std::size_t, std::size_t> audioSpacingCounts_;

    PsxMovieInfo info_;
    std::vector<PsxStrFrameIndexEntry> frameIndex_;
    mutable std::string error_;
};

} // namespace Inferno::Psx
