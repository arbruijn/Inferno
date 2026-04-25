#include "PsxPlayback.h"

#include <cmath>
#include <utility>

#include "logging.h"

namespace Inferno::Psx {
namespace {
constexpr double kPsxCdDoubleSpeedSectorsPerSecond = 150.0;

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

    if (!sourceFrame.sectorIndices.empty()) {
        const auto currentFrameEndSector = sourceFrame.sectorIndices.back();
        if (previousFrameEndSector_.has_value() && currentFrameEndSector > *previousFrameEndSector_) {
            const auto sectorsBetweenFrames = currentFrameEndSector - *previousFrameEndSector_;
            const double newFrameDurationSeconds =
                static_cast<double>(sectorsBetweenFrames) / kPsxCdDoubleSpeedSectorsPerSecond;
            const bool cadenceChanged =
                !cadence_.usingSectorCadence ||
                std::abs(newFrameDurationSeconds - cadence_.frameDurationSeconds) > 1e-6;

            cadence_.frameDurationSeconds = newFrameDurationSeconds;
            cadence_.usingSectorCadence = true;

            if (!bufferedFrames_.empty()) {
                bufferedFrames_.back().durationSeconds = newFrameDurationSeconds;
            }

            if (cadenceChanged) {
                const double fps = newFrameDurationSeconds > 0.0 ? 1.0 / newFrameDurationSeconds : 0.0;
                SPDLOG_INFO("Detected PSX movie cadence: {:.3f} fps ({:.3f} ms/frame) from {} CD sectors between video frames",
                            fps,
                            newFrameDurationSeconds * 1000.0,
                            sectorsBetweenFrames);
            }
        }

        previousFrameEndSector_ = currentFrameEndSector;
    }

    bufferedFrame.durationSeconds = cadence_.frameDurationSeconds;
    bufferedFrames_.push_back(std::move(bufferedFrame));
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

    if (!loggedFirstAudioPacket_) {
        SPDLOG_INFO("PSX movie audio packet: {} Hz, {} channels, {} sample frames ({:.3f} ms), coding=0x{:02X}",
                    audioPacket.pcm.sampleRate,
                    audioPacket.pcm.channelCount,
                    audioPacket.pcm.sampleFrames,
                    audioPacket.durationSeconds * 1000.0,
                    static_cast<unsigned>(sector.cdxa.codingInfo));
        loggedFirstAudioPacket_ = true;
    }

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
    previousFrameEndSector_.reset();
    loggedFirstAudioPacket_ = false;
    sawAudioPacket_ = false;
    reachedEndOfStream_ = false;
}

} // namespace Inferno::Psx
