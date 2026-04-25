#pragma once

#include <cstddef>
#include <cstdint>
#include <istream>
#include <memory>
#include <string>
#include <vector>

#include "PsxStream.h"

namespace Inferno::Psx {

class PsxIsoFileStream;

struct PsxCdSectorLayout {
    std::size_t rawSectorSize = 2352;
    std::size_t payloadOffset = 24;
    std::size_t payloadSize = 2048;
};

inline constexpr PsxCdSectorLayout kPsxMode2_2048 = {2352, 24, 2048};
inline constexpr PsxCdSectorLayout kPsxMode2_2336 = {2352, 16, 2336};
inline constexpr PsxCdSectorLayout kPsxMode2_2352 = {2352, 0, 2352};

struct PsxIsoDirectoryEntry {
    std::string name;
    std::uint32_t extentLba = 0;
    std::uint32_t size = 0;
    std::uint8_t flags = 0;
    bool isDirectory = false;
    std::vector<std::uint8_t> rawIdentifier;
};

class PsxIso9660Reader {
public:
    bool Load(const void* data, std::size_t size, std::string* error = nullptr);
    bool Load(const std::vector<std::uint8_t>& data, std::string* error = nullptr);
    bool Load(std::istream& input, std::string* error = nullptr);

    bool GetEntry(const std::string& path, PsxIsoDirectoryEntry* entry, std::string* error = nullptr) const;
    bool ListDirectory(const std::string& path,
                       std::vector<PsxIsoDirectoryEntry>* entries,
                       std::string* error = nullptr) const;
    bool ReadExtent(std::uint32_t extentLba,
                    std::size_t size,
                    std::vector<std::uint8_t>* output,
                    const PsxCdSectorLayout& layout = kPsxMode2_2048,
                    std::string* error = nullptr) const;
    bool ReadFile(const std::string& path,
                  std::vector<std::uint8_t>* output,
                  const PsxCdSectorLayout& layout = kPsxMode2_2048,
                  std::string* error = nullptr) const;
    std::unique_ptr<PsxReadStream> OpenFile(const std::string& path,
                                            const PsxCdSectorLayout& layout = kPsxMode2_2048,
                                            std::string* error = nullptr) const;

    const PsxIsoDirectoryEntry& RootEntry() const noexcept;
    const std::string& SystemId() const noexcept;
    const std::string& VolumeId() const noexcept;
    std::uint32_t VolumeSpaceSize() const noexcept;
    const std::string& Error() const noexcept;

    void Clear() noexcept;

private:
    enum class SourceKind {
        None = 0,
        Memory,
        Stream,
    };

    bool Initialize(std::string* error);
    bool ReadBytes(std::uint64_t absoluteOffset,
                   std::size_t size,
                   std::vector<std::uint8_t>* output,
                   std::string* error) const;
    bool ReadLogicalSector(std::uint32_t lba,
                           std::vector<std::uint8_t>* output,
                           std::string* error) const;
    bool ParseDirectoryRecord(const std::uint8_t* record,
                              std::size_t size,
                              PsxIsoDirectoryEntry* entry) const;
    bool ReadDirectoryEntries(const PsxIsoDirectoryEntry& entry,
                              std::vector<PsxIsoDirectoryEntry>* entries,
                              std::string* error) const;
    bool ResolveEntry(const std::string& path,
                      PsxIsoDirectoryEntry* entry,
                      std::string* error) const;
    void SetError(std::string message, std::string* error) const;

    friend class PsxIsoFileStream;

    SourceKind sourceKind_ = SourceKind::None;
    const std::uint8_t* memoryData_ = nullptr;
    std::size_t memorySize_ = 0;
    std::istream* stream_ = nullptr;
    std::uint64_t streamBaseOffset_ = 0;
    std::uint64_t streamSize_ = 0;

    std::string systemId_;
    std::string volumeId_;
    std::uint32_t volumeSpaceSize_ = 0;
    PsxIsoDirectoryEntry rootEntry_;
    mutable std::string error_;
};

} // namespace Inferno::Psx
