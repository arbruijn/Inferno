#include "pch.h"

#include "PsxXaAudio.h"

#include <fstream>
#include <mutex>

#include "Audio/Audio.h"
#include "FileSystem.h"
#include "PsxIso9660.h"
#include "PsxStr.h"
#include "PsxXa.h"
#include "Resources.Common.h"
#include "Settings.h"
#include "SoundSystem.h"

namespace Inferno::Psx {
namespace {
    constexpr int XA_MUSIC_SILENCE = -30;
    constexpr std::size_t XA_BUFFER_FRAMES = 1024;

    float VolumeToAmplitudeRatio(float volume, float silence = XA_MUSIC_SILENCE) {
        if (volume <= 0.0001f) {
            return 0.0f;
        }
        return XAudio2DecibelsToAmplitudeRatio(silence * (1.0f - volume));
    }

    struct XaTrack {
        std::vector<std::int16_t> samples;
        int sampleRate = 0;
        uint8 channelCount = 0;
    };

    class XaAudioStream final {
    public:
        bool Play(string_view filename, uint8 channel) {
            XaTrack track;
            if (!LoadTrack(filename, channel, &track)) {
                return false;
            }

            auto* engine = Sound::GetEngine();
            if (!engine) {
                SPDLOG_WARN("Unable to play XA audio `{}`: audio engine is not initialized", filename);
                return false;
            }

            std::unique_ptr<DynamicSoundEffectInstance> voice;
            try {
                voice = std::make_unique<DynamicSoundEffectInstance>(engine, nullptr, track.sampleRate,
                                                                    track.channelCount, 16);
            } catch (const std::exception& e) {
                SPDLOG_WARN("Unable to create XA audio voice for `{}`: {}", filename, e.what());
                return false;
            }

            Sound::StopMusic();

            std::unique_ptr<DynamicSoundEffectInstance> voiceToStop;
            {
                std::scoped_lock lock(_mutex);
                voiceToStop = std::move(_voice);
                _samples = std::move(track.samples);
                _sampleRate = track.sampleRate;
                _channelCount = track.channelCount;
                _voice = std::move(voice);
            }

            if (voiceToStop) {
                voiceToStop->Stop(true);
            }

            //_voice->SetVolume(VolumeToAmplitudeRatio(Settings::Inferno.MusicVolume));
            SubmitAllBuffers(*_voice);

            try {
                _voice->Play();
            } catch (const std::exception& e) {
                SPDLOG_WARN("Unable to start XA audio playback for `{}`: {}", filename, e.what());
                Stop();
                return false;
            }

            SPDLOG_INFO("Started XA audio `{}` on channel {}", filename, static_cast<unsigned>(channel));
            return true;
        }

        void Stop() noexcept {
            std::unique_ptr<DynamicSoundEffectInstance> voiceToStop;
            {
                std::scoped_lock lock(_mutex);
                voiceToStop = std::move(_voice);
                _samples.clear();
                _sampleRate = 0;
                _channelCount = 0;
            }

            if (voiceToStop) {
                voiceToStop->Stop(true);
            }
        }

    private:
        bool LoadTrack(string_view filename, uint8 channel, XaTrack* track) {
            if (track == nullptr) {
                return false;
            }

            const auto isoPath = D1_FOLDER / "psx.bin";
            std::ifstream image(isoPath, std::ios::binary);
            if (!image) {
                SPDLOG_WARN("Unable to open PSX image `{}`", isoPath.string());
                return false;
            }

            PsxIso9660Reader iso;
            std::string error;
            if (!iso.Load(image, &error)) {
                SPDLOG_WARN("Unable to load PSX image `{}`: {}", isoPath.string(), error);
                return false;
            }

            std::vector<std::uint8_t> bytes;
            if (!iso.ReadFile(std::string(filename), &bytes, kPsxMode2_2352, &error)) {
                SPDLOG_WARN("Unable to read XA file `{}` from `{}`: {}", filename, isoPath.string(), error);
                return false;
            }

            PsxStrReader reader;
            if (!reader.Load(bytes, &error)) {
                SPDLOG_WARN("Unable to parse XA file `{}`: {}", filename, error);
                return false;
            }

            PsxXaDecoder decoder;
            PsxStrSector sector;
            PsxXaAudioFormat format{};
            bool foundAudio = false;
            bool formatLocked = false;

            while (true) {
                error.clear();
                const auto status = reader.ReadNextSector(&sector, &error);
                if (status == PsxStrReadStatus::EndOfStream) {
                    break;
                }
                if (status == PsxStrReadStatus::Error) {
                    SPDLOG_WARN("Unable to read XA sectors from `{}`: {}", filename, error);
                    return false;
                }

                if (!sector.cdxa.IsAudio() || sector.cdxa.channelNumber != channel) {
                    continue;
                }

                PsxXaAudioFormat sectorFormat{};
                if (!ParsePsxXaAudioFormat(sector.cdxa, &sectorFormat)) {
                    SPDLOG_WARN("XA file `{}` contains an unsupported audio sector", filename);
                    return false;
                }

                if (!formatLocked) {
                    format = sectorFormat;
                    formatLocked = true;
                } else if (sectorFormat.codingInfo != format.codingInfo) {
                    SPDLOG_WARN("XA file `{}` changes audio format mid-stream", filename);
                    return false;
                }

                PsxXaPcm16 pcm;
                if (!decoder.DecodeSector(sector, &pcm, &error)) {
                    SPDLOG_WARN("Unable to decode XA sector from `{}`: {}", filename, error);
                    return false;
                }

                if (pcm.samples.empty()) {
                    continue;
                }

                track->samples.insert(track->samples.end(), pcm.samples.begin(), pcm.samples.end());
                foundAudio = true;
            }

            if (!foundAudio || track->samples.empty()) {
                SPDLOG_WARN("XA file `{}` did not contain any audio sectors for channel {}", filename,
                            static_cast<unsigned>(channel));
                return false;
            }

            track->sampleRate = format.sampleRate;
            track->channelCount = format.channelCount;
            return true;
        }

        void SubmitAllBuffers(DynamicSoundEffectInstance& voice) {
            if (_samples.empty() || _channelCount == 0) {
                return;
            }

            const std::size_t frameCount = _samples.size() / _channelCount;
            std::size_t frameOffset = 0;
            while (frameOffset < frameCount) {
                const std::size_t frames = std::min(XA_BUFFER_FRAMES, frameCount - frameOffset);
                const auto* data = reinterpret_cast<const std::uint8_t*>(
                    _samples.data() + frameOffset * _channelCount);
                const std::size_t bytes = frames * static_cast<std::size_t>(_channelCount) * sizeof(std::int16_t);
                voice.SubmitBuffer(data, bytes);
                frameOffset += frames;
            }
        }

        std::mutex _mutex;
        std::unique_ptr<DynamicSoundEffectInstance> _voice;
        std::vector<std::int16_t> _samples;
        int _sampleRate = 0;
        uint8 _channelCount = 0;
    };

    XaAudioStream& GetXaAudioStream() {
        static XaAudioStream stream;
        return stream;
    }
} // namespace

bool PlayXaAudio(string_view filename, uint8 channel) {
    return GetXaAudioStream().Play(filename, channel);
}

void StopXaAudio() {
    GetXaAudioStream().Stop();
}

} // namespace Inferno::Psx
