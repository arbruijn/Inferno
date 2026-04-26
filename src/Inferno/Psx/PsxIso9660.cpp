#include "PsxIso9660.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <limits>
#include <memory>
#include <utility>

namespace Inferno::Psx {
namespace {

std::uint32_t ReadU32LE(const std::uint8_t* data) {
    return static_cast<std::uint32_t>(data[0]) |
           (static_cast<std::uint32_t>(data[1]) << 8U) |
           (static_cast<std::uint32_t>(data[2]) << 16U) |
           (static_cast<std::uint32_t>(data[3]) << 24U);
}

std::string DecodeAsciiField(const std::uint8_t* data, std::size_t size) {
    std::string text(reinterpret_cast<const char*>(data), size);
    while (!text.empty() && text.back() == ' ') {
        text.pop_back();
    }
    return text;
}

std::vector<std::string> SplitIsoPath(const std::string& path) {
    std::vector<std::string> parts;
    std::size_t position = 0;
    while (position < path.size()) {
        while (position < path.size() && path[position] == '/') {
            ++position;
        }
        if (position >= path.size()) {
            break;
        }

        const std::size_t nextSlash = path.find('/', position);
        if (nextSlash == std::string::npos) {
            parts.push_back(path.substr(position));
            break;
        }

        parts.push_back(path.substr(position, nextSlash - position));
        position = nextSlash + 1;
    }
    return parts;
}

std::string NormalizeIsoName(std::string name) {
    while (!name.empty() && name.back() == '.') {
        name.pop_back();
    }
    const std::size_t versionMarker = name.find(';');
    if (versionMarker != std::string::npos) {
        name.erase(versionMarker);
    }
    std::transform(name.begin(), name.end(), name.begin(), [](unsigned char ch) {
        return static_cast<char>(std::toupper(ch));
    });
    return name;
}

bool IsoNamesMatch(const std::string& recordName, const std::string& requestedName) {
    return NormalizeIsoName(recordName) == NormalizeIsoName(requestedName);
}

std::size_t SectorCountForIsoFileSize(std::size_t size) {
    return (size + kPsxMode2_2048.payloadSize - 1) / kPsxMode2_2048.payloadSize;
}

std::size_t ExpandedReadSize(std::size_t fileSize, const PsxCdSectorLayout& layout) {
    if (layout.payloadSize == kPsxMode2_2048.payloadSize &&
        layout.rawSectorSize == kPsxMode2_2048.rawSectorSize &&
        layout.payloadOffset == kPsxMode2_2048.payloadOffset) {
        return fileSize;
    }

    if ((layout.rawSectorSize == kPsxMode2_2336.rawSectorSize &&
         layout.payloadSize == kPsxMode2_2336.payloadSize &&
         layout.payloadOffset == kPsxMode2_2336.payloadOffset) ||
        (layout.rawSectorSize == kPsxMode2_2352.rawSectorSize &&
         layout.payloadSize == kPsxMode2_2352.payloadSize &&
         layout.payloadOffset == kPsxMode2_2352.payloadOffset)) {
        return SectorCountForIsoFileSize(fileSize) * layout.payloadSize;
    }

    return fileSize;
}

} // namespace

class PsxIsoFileStream final : public PsxReadStream {
public:
    PsxIsoFileStream(PsxIso9660Reader* owner,
                     std::uint32_t extentLba,
                     std::size_t size,
                     PsxCdSectorLayout layout)
        : owner_(owner), extentLba_(extentLba), size_(size), layout_(layout) {}

    bool Read(std::uint8_t* output,
              std::size_t maxBytes,
              std::size_t* bytesRead,
              std::string* error) override {
        if (bytesRead == nullptr) {
            if (owner_ != nullptr) {
                owner_->SetError("PsxIsoFileStream::Read requires a non-null bytesRead output.", error);
            }
            return false;
        }
        *bytesRead = 0;

        if (output == nullptr && maxBytes != 0) {
            if (owner_ != nullptr) {
                owner_->SetError("PsxIsoFileStream::Read requires a non-null output buffer.", error);
            }
            return false;
        }
        if (maxBytes == 0 || position_ >= size_) {
            return true;
        }

        const std::size_t targetBytes = std::min(maxBytes, size_ - position_);
        std::size_t produced = 0;
        while (produced < targetBytes) {
            const std::size_t sectorIndex = position_ / layout_.payloadSize;
            const std::size_t inSectorOffset = position_ % layout_.payloadSize;
            const std::size_t chunkSize = std::min(targetBytes - produced, layout_.payloadSize - inSectorOffset);
            const std::uint64_t absoluteOffset =
                static_cast<std::uint64_t>(extentLba_ + sectorIndex) * layout_.rawSectorSize +
                layout_.payloadOffset + inSectorOffset;

            std::vector<std::uint8_t> chunk;
            if (!owner_->ReadBytes(absoluteOffset, chunkSize, &chunk, error)) {
                return false;
            }
            if (chunk.size() != chunkSize) {
                owner_->SetError("PsxIsoFileStream encountered a truncated image while reading.", error);
                return false;
            }

            std::copy(chunk.begin(), chunk.end(), output + static_cast<std::ptrdiff_t>(produced));
            produced += chunkSize;
            position_ += chunkSize;
        }

        *bytesRead = produced;
        return true;
    }

    bool Skip(std::size_t byteCount, std::string* error) override {
        if (byteCount > (size_ - position_)) {
            if (owner_ != nullptr) {
                owner_->SetError("PsxIsoFileStream attempted to skip past the end of the opened file.", error);
            }
            return false;
        }
        position_ += byteCount;
        return true;
    }

private:
    PsxIso9660Reader* owner_ = nullptr;
    std::uint32_t extentLba_ = 0;
    std::size_t size_ = 0;
    std::size_t position_ = 0;
    PsxCdSectorLayout layout_{};
};

void PsxIso9660Reader::Clear() noexcept {
    sourceKind_ = SourceKind::None;
    memoryData_ = nullptr;
    memorySize_ = 0;
    stream_ = nullptr;
    streamBaseOffset_ = 0;
    streamSize_ = 0;
    systemId_.clear();
    volumeId_.clear();
    volumeSpaceSize_ = 0;
    rootEntry_ = PsxIsoDirectoryEntry{};
    error_.clear();
}

void PsxIso9660Reader::SetError(std::string message, std::string* error) const {
    error_ = std::move(message);
    if (error != nullptr) {
        *error = error_;
    }
}

bool PsxIso9660Reader::Load(const void* data, std::size_t size, std::string* error) {
    Clear();

    if (data == nullptr && size != 0) {
        SetError("PsxIso9660Reader::Load received a null data pointer.", error);
        return false;
    }
    if (size == 0) {
        SetError("PsxIso9660Reader::Load requires a non-empty image.", error);
        return false;
    }
    if ((size % kPsxMode2_2048.rawSectorSize) != 0) {
        SetError("PsxIso9660Reader::Load requires an image with a whole number of 2352-byte sectors.", error);
        return false;
    }

    sourceKind_ = SourceKind::Memory;
    memoryData_ = static_cast<const std::uint8_t*>(data);
    memorySize_ = size;
    return Initialize(error);
}

bool PsxIso9660Reader::Load(const std::vector<std::uint8_t>& data, std::string* error) {
    return Load(data.data(), data.size(), error);
}

bool PsxIso9660Reader::Load(std::istream& input, std::string* error) {
    Clear();

    const std::istream::pos_type originalPosition = input.tellg();
    if (originalPosition == std::istream::pos_type(-1)) {
        SetError("PsxIso9660Reader::Load requires a seekable input stream.", error);
        return false;
    }

    input.seekg(0, std::ios::end);
    const std::istream::pos_type endPosition = input.tellg();
    if (endPosition == std::istream::pos_type(-1)) {
        SetError("PsxIso9660Reader::Load could not determine the image size.", error);
        return false;
    }
    input.seekg(originalPosition);

    const std::uint64_t imageSize = static_cast<std::uint64_t>(endPosition - originalPosition);
    if (imageSize == 0) {
        SetError("PsxIso9660Reader::Load requires a non-empty image.", error);
        return false;
    }
    if ((imageSize % kPsxMode2_2048.rawSectorSize) != 0) {
        SetError("PsxIso9660Reader::Load requires an image with a whole number of 2352-byte sectors.", error);
        return false;
    }

    sourceKind_ = SourceKind::Stream;
    stream_ = &input;
    streamBaseOffset_ = static_cast<std::uint64_t>(originalPosition);
    streamSize_ = imageSize;
    return Initialize(error);
}

bool PsxIso9660Reader::Initialize(std::string* error) {
    std::vector<std::uint8_t> pvd;
    if (!ReadLogicalSector(16, &pvd, error)) {
        return false;
    }
    if (pvd.size() != kPsxMode2_2048.payloadSize) {
        SetError("PsxIso9660Reader::Load could not read a full primary volume descriptor sector.", error);
        return false;
    }
    if (pvd[0] != 1 || std::memcmp(pvd.data() + 1, "CD001", 5) != 0 || pvd[6] != 1) {
        SetError("PsxIso9660Reader::Load could not find the primary volume descriptor at sector 16.", error);
        return false;
    }

    systemId_ = DecodeAsciiField(pvd.data() + 8, 32);
    volumeId_ = DecodeAsciiField(pvd.data() + 40, 32);
    volumeSpaceSize_ = ReadU32LE(pvd.data() + 80);

    const std::size_t rootLength = pvd[156];
    if (rootLength == 0 || (156 + rootLength) > pvd.size()) {
        SetError("PsxIso9660Reader::Load found an invalid root directory record.", error);
        return false;
    }
    if (!ParseDirectoryRecord(pvd.data() + 156, rootLength, &rootEntry_) || !rootEntry_.isDirectory) {
        SetError("PsxIso9660Reader::Load found an invalid root directory record.", error);
        return false;
    }

    return true;
}

bool PsxIso9660Reader::ReadBytes(std::uint64_t absoluteOffset,
                                 std::size_t size,
                                 std::vector<std::uint8_t>* output,
                                 std::string* error) {
    if (output == nullptr) {
        SetError("PsxIso9660Reader requires a non-null output buffer.", error);
        return false;
    }

    if (sourceKind_ == SourceKind::Memory) {
        const std::uint64_t endOffset = absoluteOffset + static_cast<std::uint64_t>(size);
        if (endOffset > memorySize_) {
            SetError("PsxIso9660Reader attempted to read past the end of the image.", error);
            return false;
        }
        output->assign(memoryData_ + absoluteOffset, memoryData_ + endOffset);
        return true;
    }

    if (sourceKind_ == SourceKind::Stream) {
        const std::uint64_t endOffset = absoluteOffset + static_cast<std::uint64_t>(size);
        if (endOffset > streamSize_) {
            SetError("PsxIso9660Reader attempted to read past the end of the image.", error);
            return false;
        }
        if (absoluteOffset > static_cast<std::uint64_t>(std::numeric_limits<std::streamoff>::max()) ||
            size > static_cast<std::size_t>(std::numeric_limits<std::streamsize>::max())) {
            SetError("PsxIso9660Reader cannot address this image offset on the current platform.", error);
            return false;
        }

        output->assign(size, 0);

        stream_->clear();
        if (streamBaseOffset_ + absoluteOffset != streamLastPos_) {
            stream_->seekg(static_cast<std::streamoff>(streamBaseOffset_ + absoluteOffset), std::ios::beg);
            if (!stream_->good()) {
                SetError("PsxIso9660Reader failed to seek within the image.", error);
                return false;
            }
            streamLastPos_ = streamBaseOffset_ + absoluteOffset;
        }

        if (size != 0) {
            stream_->read(reinterpret_cast<char*>(output->data()), static_cast<std::streamsize>(size));
            if (stream_->gcount() != static_cast<std::streamsize>(size)) {
                SetError("PsxIso9660Reader encountered a truncated image while reading.", error);
                return false;
            }
            streamLastPos_ += size;
        }

        return true;
    }

    SetError("PsxIso9660Reader has no loaded image.", error);
    return false;
}

bool PsxIso9660Reader::ReadLogicalSector(std::uint32_t lba,
                                         std::vector<std::uint8_t>* output,
                                         std::string* error) {
    const std::uint64_t offset =
        static_cast<std::uint64_t>(lba) * kPsxMode2_2048.rawSectorSize + kPsxMode2_2048.payloadOffset;
    return ReadBytes(offset, kPsxMode2_2048.payloadSize, output, error);
}

bool PsxIso9660Reader::ParseDirectoryRecord(const std::uint8_t* record,
                                            std::size_t size,
                                            PsxIsoDirectoryEntry* entry) const {
    if (record == nullptr || entry == nullptr || size < 34) {
        return false;
    }

    const std::size_t recordLength = record[0];
    if (recordLength == 0 || recordLength > size) {
        return false;
    }

    const std::size_t nameLength = record[32];
    if ((33 + nameLength) > recordLength) {
        return false;
    }

    entry->extentLba = ReadU32LE(record + 2);
    entry->size = ReadU32LE(record + 10);
    entry->flags = record[25];
    entry->isDirectory = (entry->flags & 0x02U) != 0;
    entry->rawIdentifier.assign(record + 33, record + 33 + nameLength);

    if (entry->rawIdentifier.size() == 1 && entry->rawIdentifier[0] == 0x00) {
        entry->name = ".";
    } else if (entry->rawIdentifier.size() == 1 && entry->rawIdentifier[0] == 0x01) {
        entry->name = "..";
    } else {
        entry->name.assign(entry->rawIdentifier.begin(), entry->rawIdentifier.end());
    }

    return true;
}

bool PsxIso9660Reader::ReadDirectoryEntries(const PsxIsoDirectoryEntry& entry,
                                            std::vector<PsxIsoDirectoryEntry>* entries,
                                            std::string* error) {
    if (entries == nullptr) {
        SetError("PsxIso9660Reader::ReadDirectoryEntries requires a non-null output vector.", error);
        return false;
    }
    if (!entry.isDirectory) {
        SetError("PsxIso9660Reader cannot list a non-directory entry.", error);
        return false;
    }

    std::vector<std::uint8_t> bytes;
    if (!ReadExtent(entry.extentLba, entry.size, &bytes, kPsxMode2_2048, error)) {
        return false;
    }

    entries->clear();
    std::size_t position = 0;
    while (position < bytes.size()) {
        const std::uint8_t recordLength = bytes[position];
        if (recordLength == 0) {
            position = ((position / kPsxMode2_2048.payloadSize) + 1) * kPsxMode2_2048.payloadSize;
            continue;
        }
        if ((position + recordLength) > bytes.size()) {
            SetError("PsxIso9660Reader encountered a truncated directory record.", error);
            return false;
        }

        PsxIsoDirectoryEntry child;
        if (!ParseDirectoryRecord(bytes.data() + position, recordLength, &child)) {
            SetError("PsxIso9660Reader encountered an invalid directory record.", error);
            return false;
        }
        entries->push_back(std::move(child));
        position += recordLength;
    }

    return true;
}

bool PsxIso9660Reader::ResolveEntry(const std::string& path,
                                    PsxIsoDirectoryEntry* entry,
                                    std::string* error) {
    if (sourceKind_ == SourceKind::None) {
        SetError("PsxIso9660Reader has no loaded image.", error);
        return false;
    }
    if (entry == nullptr) {
        SetError("PsxIso9660Reader::GetEntry requires a non-null output entry.", error);
        return false;
    }

    if (path.empty() || path == "/") {
        *entry = rootEntry_;
        return true;
    }

    PsxIsoDirectoryEntry current = rootEntry_;
    for (const std::string& part : SplitIsoPath(path)) {
        if (!current.isDirectory) {
            SetError("PsxIso9660Reader path component is not a directory: " + path, error);
            return false;
        }

        std::vector<PsxIsoDirectoryEntry> children;
        if (!ReadDirectoryEntries(current, &children, error)) {
            return false;
        }

        bool found = false;
        for (const PsxIsoDirectoryEntry& child : children) {
            if (child.name == "." || child.name == "..") {
                continue;
            }
            if (IsoNamesMatch(child.name, part)) {
                current = child;
                found = true;
                break;
            }
        }

        if (!found) {
            SetError("PsxIso9660Reader could not find path: " + path, error);
            return false;
        }
    }

    *entry = std::move(current);
    return true;
}

bool PsxIso9660Reader::GetEntry(const std::string& path,
                                PsxIsoDirectoryEntry* entry,
                                std::string* error) {
    return ResolveEntry(path, entry, error);
}

bool PsxIso9660Reader::ListDirectory(const std::string& path,
                                     std::vector<PsxIsoDirectoryEntry>* entries,
                                     std::string* error) {
    PsxIsoDirectoryEntry directory;
    if (!ResolveEntry(path, &directory, error)) {
        return false;
    }
    return ReadDirectoryEntries(directory, entries, error);
}

bool PsxIso9660Reader::ReadExtent(std::uint32_t extentLba,
                                  std::size_t size,
                                  std::vector<std::uint8_t>* output,
                                  const PsxCdSectorLayout& layout,
                                  std::string* error) {
    if (output == nullptr) {
        SetError("PsxIso9660Reader::ReadExtent requires a non-null output buffer.", error);
        return false;
    }
    if (layout.rawSectorSize == 0 || layout.payloadSize == 0 || layout.payloadOffset >= layout.rawSectorSize ||
        (layout.payloadOffset + layout.payloadSize) > layout.rawSectorSize) {
        SetError("PsxIso9660Reader::ReadExtent received an invalid sector layout.", error);
        return false;
    }

    output->clear();
    output->reserve(size);

    std::size_t remaining = size;
    std::uint32_t currentLba = extentLba;
    while (remaining > 0) {
        const std::uint64_t sectorOffset =
            static_cast<std::uint64_t>(currentLba) * layout.rawSectorSize + layout.payloadOffset;
        const std::size_t chunkSize = std::min(remaining, layout.payloadSize);

        std::vector<std::uint8_t> chunk;
        if (!ReadBytes(sectorOffset, chunkSize, &chunk, error)) {
            return false;
        }

        output->insert(output->end(), chunk.begin(), chunk.end());
        remaining -= chunkSize;
        ++currentLba;
    }

    return true;
}

bool PsxIso9660Reader::ReadFile(const std::string& path,
                                std::vector<std::uint8_t>* output,
                                const PsxCdSectorLayout& layout,
                                std::string* error) {
    PsxIsoDirectoryEntry entry;
    if (!ResolveEntry(path, &entry, error)) {
        return false;
    }
    if (entry.isDirectory) {
        SetError("PsxIso9660Reader::ReadFile cannot read a directory: " + path, error);
        return false;
    }

    return ReadExtent(entry.extentLba, ExpandedReadSize(entry.size, layout), output, layout, error);
}

std::unique_ptr<PsxReadStream> PsxIso9660Reader::OpenFile(const std::string& path,
                                                          const PsxCdSectorLayout& layout,
                                                          std::string* error) {
    PsxIsoDirectoryEntry entry;
    if (!ResolveEntry(path, &entry, error)) {
        return nullptr;
    }
    if (entry.isDirectory) {
        SetError("PsxIso9660Reader::OpenFile cannot open a directory: " + path, error);
        return nullptr;
    }
    if (layout.rawSectorSize == 0 || layout.payloadSize == 0 || layout.payloadOffset >= layout.rawSectorSize ||
        (layout.payloadOffset + layout.payloadSize) > layout.rawSectorSize) {
        SetError("PsxIso9660Reader::OpenFile received an invalid sector layout.", error);
        return nullptr;
    }

    return std::make_unique<PsxIsoFileStream>(this,
                                              entry.extentLba,
                                              ExpandedReadSize(entry.size, layout),
                                              layout);
}

const PsxIsoDirectoryEntry& PsxIso9660Reader::RootEntry() const noexcept {
    return rootEntry_;
}

const std::string& PsxIso9660Reader::SystemId() const noexcept {
    return systemId_;
}

const std::string& PsxIso9660Reader::VolumeId() const noexcept {
    return volumeId_;
}

std::uint32_t PsxIso9660Reader::VolumeSpaceSize() const noexcept {
    return volumeSpaceSize_;
}

const std::string& PsxIso9660Reader::Error() const noexcept {
    return error_;
}

} // namespace Inferno::Psx
