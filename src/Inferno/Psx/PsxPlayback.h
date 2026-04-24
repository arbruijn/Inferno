#pragma once

#include <cstddef>
#include <cstdint>
#include <deque>
#include <string>
#include <vector>

#include "PsxMdec.h"
#include "PsxStr.h"
#include "PsxXa.h"

namespace Inferno::Psx {

struct PsxPlaybackCadence {
    double frameDurationSeconds = 1.0 / 30.0;
    std::size_t framesSinceLastAudioPacket = 0;
    bool usingAudioCadence = false;
};

struct PsxPlaybackBufferedFrame {
    std::uint32_t frameNumber = 0;
    PsxRgbFrame frame;
};

struct PsxPlaybackAudioPacket {
    PsxXaPcm16 pcm;
    double durationSeconds = 0.0;
};

class PsxPlaybackSession {
public:
    bool Start(PsxStrReader* reader, std::string* error = nullptr);
    bool FillVideoBuffer(std::size_t targetBufferedFrames, std::string* error = nullptr);
    bool TakeFrontVideoFrame(PsxPlaybackBufferedFrame* frame, std::string* error = nullptr);

    std::vector<PsxPlaybackAudioPacket> TakeQueuedAudioPackets();

    const PsxPlaybackCadence& Cadence() const noexcept;
    std::size_t BufferedFrameCount() const noexcept;
    bool HasBufferedFrames() const noexcept;
    bool ReachedEndOfStream() const noexcept;
    bool SawAudioPacket() const noexcept;

private:
    bool ReadPackets(std::size_t targetBufferedFrames,
                     bool stopAfterFirstAudioPacket,
                     std::string* error);
    bool BufferDecodedFrame(const PsxStrFrame& sourceFrame, std::string* error);
    bool BufferAudioSector(const PsxStrSector& sector, std::string* error);
    void Reset() noexcept;

    PsxStrReader* reader_ = nullptr;
    PsxMdecDecoder videoDecoder_;
    PsxXaDecoder audioDecoder_;
    std::deque<PsxPlaybackBufferedFrame> bufferedFrames_;
    std::vector<PsxPlaybackAudioPacket> queuedAudioPackets_;
    PsxPlaybackCadence cadence_;
    bool sawAudioPacket_ = false;
    bool reachedEndOfStream_ = false;
};

} // namespace Inferno::Psx
