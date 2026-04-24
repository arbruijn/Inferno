#include "PsxPlayback.h"

#include <utility>

namespace Inferno::Psx {
namespace {

void SetError(std::string message, std::string* error) {
    if (error != nullptr) {
        *error = std::move(message);
    }
}

} // namespace

bool PsxPlaybackSession::Start(PsxStrReader* reader, std::string* error) {
    if (reader == nullptr) {
        SetError("PsxPlaybackSession::Start requires a non-null PsxStrReader.", error);
        return false;
    }

    Reset();
    reader_ = reader;
    return ReadPackets(0, true, error);
}

bool PsxPlaybackSession::FillVideoBuffer(std::size_t targetBufferedFrames, std::string* error) {
    if (reader_ == nullptr) {
        SetError("PsxPlaybackSession::FillVideoBuffer requires Start() to be called first.", error);
        return false;
    }
    return ReadPackets(targetBufferedFrames, false, error);
}

bool PsxPlaybackSession::TakeFrontVideoFrame(PsxPlaybackBufferedFrame* frame, std::string* error) {
    if (frame == nullptr) {
        SetError("PsxPlaybackSession::TakeFrontVideoFrame requires a non-null output frame.", error);
        return false;
    }
    if (bufferedFrames_.empty()) {
        SetError("PsxPlaybackSession::TakeFrontVideoFrame was called with no buffered video frames.", error);
        return false;
    }

    *frame = std::move(bufferedFrames_.front());
    bufferedFrames_.pop_front();
    return true;
}

std::vector<PsxPlaybackAudioPacket> PsxPlaybackSession::TakeQueuedAudioPackets() {
    std::vector<PsxPlaybackAudioPacket> audioPackets = std::move(queuedAudioPackets_);
    queuedAudioPackets_.clear();
    return audioPackets;
}

const PsxPlaybackCadence& PsxPlaybackSession::Cadence() const noexcept {
    return cadence_;
}

std::size_t PsxPlaybackSession::BufferedFrameCount() const noexcept {
    return bufferedFrames_.size();
}

bool PsxPlaybackSession::HasBufferedFrames() const noexcept {
    return !bufferedFrames_.empty();
}

bool PsxPlaybackSession::ReachedEndOfStream() const noexcept {
    return reachedEndOfStream_;
}

bool PsxPlaybackSession::SawAudioPacket() const noexcept {
    return sawAudioPacket_;
}

bool PsxPlaybackSession::ReadPackets(std::size_t targetBufferedFrames,
                                     bool stopAfterFirstAudioPacket,
                                     std::string* error) {
    while (true) {
        if (!stopAfterFirstAudioPacket && bufferedFrames_.size() >= targetBufferedFrames) {
            return true;
        }

        PsxStrPacket packet;
        const PsxStrReadStatus status = reader_->ReadNextPacket(&packet, error);
        if (status == PsxStrReadStatus::Error) {
            return false;
        }
        if (status == PsxStrReadStatus::EndOfStream) {
            reachedEndOfStream_ = true;
            return true;
        }

        if (packet.kind == PsxStrPacketKind::AudioSector && packet.sector.has_value()) {
            if (!BufferAudioSector(*packet.sector, error)) {
                return false;
            }
            if (stopAfterFirstAudioPacket) {
                return true;
            }
            continue;
        }

        if (packet.kind != PsxStrPacketKind::VideoFrame || !packet.frame.has_value()) {
            continue;
        }

        if (!BufferDecodedFrame(*packet.frame, error)) {
            return false;
        }
    }
}

bool PsxPlaybackSession::BufferDecodedFrame(const PsxStrFrame& sourceFrame, std::string* error) {
    if (!sourceFrame.complete) {
        SetError("Encountered an incomplete video frame.", error);
        return false;
    }
    if (sourceFrame.version != 2) {
        SetError("This playback session only handles version 2 frames.", error);
        return false;
    }

    PsxPlaybackBufferedFrame bufferedFrame;
    bufferedFrame.frameNumber = sourceFrame.frameNumber;
    if (!videoDecoder_.DecodeVersion2Frame(sourceFrame, &bufferedFrame.frame, error)) {
        return false;
    }

    bufferedFrames_.push_back(std::move(bufferedFrame));
    ++cadence_.framesSinceLastAudioPacket;
    return true;
}

bool PsxPlaybackSession::BufferAudioSector(const PsxStrSector& sector, std::string* error) {
    PsxPlaybackAudioPacket audioPacket;
    if (!audioDecoder_.DecodeSector(sector, &audioPacket.pcm, error)) {
        return false;
    }

    if (audioPacket.pcm.sampleRate > 0) {
        audioPacket.durationSeconds =
            static_cast<double>(audioPacket.pcm.sampleFrames) / static_cast<double>(audioPacket.pcm.sampleRate);
    }

    if (cadence_.framesSinceLastAudioPacket > 0 && audioPacket.durationSeconds > 0.0) {
        cadence_.frameDurationSeconds =
            audioPacket.durationSeconds / static_cast<double>(cadence_.framesSinceLastAudioPacket);
        cadence_.usingAudioCadence = true;
    }

    cadence_.framesSinceLastAudioPacket = 0;
    queuedAudioPackets_.push_back(std::move(audioPacket));
    sawAudioPacket_ = true;
    return true;
}

void PsxPlaybackSession::Reset() noexcept {
    reader_ = nullptr;
    audioDecoder_.Reset();
    bufferedFrames_.clear();
    queuedAudioPackets_.clear();
    cadence_ = PsxPlaybackCadence{};
    sawAudioPacket_ = false;
    reachedEndOfStream_ = false;
}

} // namespace Inferno::Psx
