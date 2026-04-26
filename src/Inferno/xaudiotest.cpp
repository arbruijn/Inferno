#include <windows.h>
#include <mmreg.h>
#include <xaudio2.h>

#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>

namespace {

double qpc_to_ms(LARGE_INTEGER delta, LARGE_INTEGER freq) {
  return (static_cast<double>(delta.QuadPart) * 1000.0) /
         static_cast<double>(freq.QuadPart);
}

struct TimingCallback final : IXAudio2VoiceCallback {
  explicit TimingCallback(size_t expected)
      : expected_count(expected),
        event(CreateEventW(nullptr, TRUE, FALSE, nullptr)),
        buffer_end_times(expected) {}

  ~TimingCallback() {
    if (event) {
      CloseHandle(event);
    }
  }

  void OnBufferStart(void*) override {}

  void OnBufferEnd(void*) override {
    LARGE_INTEGER now{};
    QueryPerformanceCounter(&now);

    const long index = buffer_end_count.fetch_add(1, std::memory_order_relaxed);
    if (static_cast<size_t>(index) < buffer_end_times.size()) {
      buffer_end_times[static_cast<size_t>(index)] = now;
    }

    if (static_cast<size_t>(index + 1) >= expected_count) {
      SetEvent(event);
    }
  }

  void OnLoopEnd(void*) override {}
  void OnStreamEnd() override {}
  void OnVoiceError(void*, HRESULT) override {
    SetEvent(event);
  }
  void OnVoiceProcessingPassStart(UINT32) override {}
  void OnVoiceProcessingPassEnd() override {}

  void wait_for_completion(DWORD timeout_ms) {
    WaitForSingleObject(event, timeout_ms);
  }

  const size_t expected_count;
  HANDLE event;
  std::vector<LARGE_INTEGER> buffer_end_times;
  std::atomic<long> buffer_end_count{0};
};

struct TestCase {
  UINT32 sample_rate;
  UINT32 frames_per_buffer;
  UINT32 buffer_count;
};

std::vector<std::uint8_t> make_pcm_buffer(UINT32 frames, UINT32 sample_rate) {
  constexpr UINT32 channels = 1;
  constexpr UINT32 bits_per_sample = 16;
  const UINT32 bytes_per_sample = bits_per_sample / 8;
  const size_t sample_count = static_cast<size_t>(frames) * channels;
  std::vector<std::uint8_t> bytes(sample_count * bytes_per_sample);

  const double frequency = 220.0;
  const double amplitude = 0.15 * 32767.0;
  for (UINT32 i = 0; i < frames; ++i) {
    const double t = static_cast<double>(i) / static_cast<double>(sample_rate);
    const double value = std::sin(2.0 * 3.14159265358979323846 * frequency * t);
    const auto sample = static_cast<std::int16_t>(std::lround(value * amplitude));
    bytes[static_cast<size_t>(i) * 2 + 0] = static_cast<std::uint8_t>(sample & 0xff);
    bytes[static_cast<size_t>(i) * 2 + 1] =
        static_cast<std::uint8_t>((static_cast<unsigned>(sample) >> 8) & 0xff);
  }

  return bytes;
}

void print_case_header(const TestCase& tc, double expected_ms) {
  std::printf(
      "\ncase: sample_rate=%u frames_per_buffer=%u buffer_count=%u expected_buffer_ms=%.3f\n",
      tc.sample_rate, tc.frames_per_buffer, tc.buffer_count, expected_ms);
}

void print_intervals(const TimingCallback& cb, LARGE_INTEGER freq) {
  if (cb.buffer_end_count.load(std::memory_order_relaxed) < 2) {
    std::printf("  insufficient callbacks captured\n");
    return;
  }

  std::vector<double> intervals;
  intervals.reserve(cb.buffer_end_times.size() - 1);
  for (size_t i = 1; i < cb.buffer_end_times.size(); ++i) {
    if (cb.buffer_end_times[i].QuadPart == 0 || cb.buffer_end_times[i - 1].QuadPart == 0) {
      break;
    }

    LARGE_INTEGER delta{};
    delta.QuadPart = cb.buffer_end_times[i].QuadPart - cb.buffer_end_times[i - 1].QuadPart;
    intervals.push_back(qpc_to_ms(delta, freq));
  }

  if (intervals.empty()) {
    std::printf("  insufficient interval data\n");
    return;
  }

  double sum = 0.0;
  double min_v = intervals.front();
  double max_v = intervals.front();
  for (double v : intervals) {
    sum += v;
    if (v < min_v) min_v = v;
    if (v > max_v) max_v = v;
  }

  std::printf("  intervals_ms:");
  for (double v : intervals) {
    std::printf(" %.3f", v);
  }
  std::printf("\n  summary_ms: avg=%.3f min=%.3f max=%.3f\n", sum / intervals.size(), min_v, max_v);
}

bool run_case(IXAudio2* engine, const TestCase& tc, LARGE_INTEGER freq) {
  const UINT32 channels = 1;
  const UINT32 bits_per_sample = 16;
  const UINT32 block_align = channels * (bits_per_sample / 8);
  const UINT32 avg_bytes_per_sec = tc.sample_rate * block_align;

  WAVEFORMATEX format{};
  format.wFormatTag = WAVE_FORMAT_PCM;
  format.nChannels = static_cast<WORD>(channels);
  format.nSamplesPerSec = tc.sample_rate;
  format.wBitsPerSample = static_cast<WORD>(bits_per_sample);
  format.nBlockAlign = static_cast<WORD>(block_align);
  format.nAvgBytesPerSec = avg_bytes_per_sec;
  format.cbSize = 0;

  TimingCallback callback(tc.buffer_count);
  IXAudio2SourceVoice* source_voice = nullptr;
  HRESULT hr = engine->CreateSourceVoice(&source_voice, &format, 0, XAUDIO2_DEFAULT_FREQ_RATIO,
                                         &callback, nullptr, nullptr);
  if (FAILED(hr)) {
    std::printf("  CreateSourceVoice failed: 0x%08lx\n", static_cast<unsigned long>(hr));
    return false;
  }

  std::vector<std::vector<std::uint8_t>> buffers;
  buffers.reserve(tc.buffer_count);
  for (UINT32 i = 0; i < tc.buffer_count; ++i) {
    buffers.push_back(make_pcm_buffer(tc.frames_per_buffer, tc.sample_rate));
  }

  for (UINT32 i = 0; i < tc.buffer_count; ++i) {
    XAUDIO2_BUFFER buffer{};
    buffer.AudioBytes = static_cast<UINT32>(buffers[i].size());
    buffer.pAudioData = buffers[i].data();
    buffer.Flags = (i + 1 == tc.buffer_count) ? XAUDIO2_END_OF_STREAM : 0;
    hr = source_voice->SubmitSourceBuffer(&buffer);
    if (FAILED(hr)) {
      std::printf("  SubmitSourceBuffer failed: 0x%08lx\n", static_cast<unsigned long>(hr));
      source_voice->DestroyVoice();
      return false;
    }
  }

  LARGE_INTEGER start_qpc{};
  QueryPerformanceCounter(&start_qpc);
  hr = source_voice->Start(0);
  if (FAILED(hr)) {
    std::printf("  Start failed: 0x%08lx\n", static_cast<unsigned long>(hr));
    source_voice->DestroyVoice();
    return false;
  }

  const DWORD wait_result = WaitForSingleObject(callback.event, 10000);
  if (wait_result == WAIT_TIMEOUT) {
    std::printf("  wait timeout after 10000 ms\n");
  } else if (wait_result != WAIT_OBJECT_0) {
    std::printf("  wait failed: %lu\n", static_cast<unsigned long>(wait_result));
  }

  LARGE_INTEGER end_qpc{};
  QueryPerformanceCounter(&end_qpc);
  LARGE_INTEGER total_delta{};
  total_delta.QuadPart = end_qpc.QuadPart - start_qpc.QuadPart;

  std::printf("  run_time_ms: %.3f callback_count=%ld\n", qpc_to_ms(total_delta, freq),
              callback.buffer_end_count.load(std::memory_order_relaxed));

  print_intervals(callback, freq);

  source_voice->Stop(0);
  source_voice->DestroyVoice();
  return true;
}

}  // namespace

int xaudiotest(IXAudio2 *engine) {
  LARGE_INTEGER freq{};
  if (!QueryPerformanceFrequency(&freq)) {
    std::printf("QueryPerformanceFrequency failed\n");
    return 1;
  }

#if 0
  HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
  if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
    std::printf("CoInitializeEx failed: 0x%08lx\n", static_cast<unsigned long>(hr));
    return 1;
  }

  IXAudio2* engine = nullptr;
  hr = XAudio2Create(&engine, 0, XAUDIO2_DEFAULT_PROCESSOR);
  if (FAILED(hr)) {
    std::printf("XAudio2Create failed: 0x%08lx\n", static_cast<unsigned long>(hr));
    if (SUCCEEDED(hr)) {
      CoUninitialize();
    }
    return 1;
  }

  IXAudio2MasteringVoice* mastering_voice = nullptr;
  hr = engine->CreateMasteringVoice(&mastering_voice);
  if (FAILED(hr)) {
    std::printf("CreateMasteringVoice failed: 0x%08lx\n", static_cast<unsigned long>(hr));
    engine->Release();
    CoUninitialize();
    return 1;
  }

  hr = engine->StartEngine();
  if (FAILED(hr)) {
    std::printf("StartEngine failed: 0x%08lx\n", static_cast<unsigned long>(hr));
    mastering_voice->DestroyVoice();
    engine->Release();
    CoUninitialize();
    return 1;
  }
#endif  

  std::array<TestCase, 9> cases = {{
      {37800, 1024, 4},
      {37800, 1500, 4},
      {37800, 2048, 4},
      {44100, 1024, 4},
      {44100, 1500, 4},
      {44100, 2048, 4},
      {48000, 1024, 4},
      {48000, 1500, 4},
      {48000, 2048, 4},
  }};

  bool all_ok = true;
  for (TestCase& tc : cases) {
    tc.buffer_count *= 4;
    const double expected_ms = (static_cast<double>(tc.frames_per_buffer) * 1000.0) /
                               static_cast<double>(tc.sample_rate);
    print_case_header(tc, expected_ms);
    if (!run_case(engine, tc, freq)) {
      all_ok = false;
    }
  }

  #if 0
  engine->StopEngine();
  mastering_voice->DestroyVoice();
  engine->Release();
  CoUninitialize();
  #endif

  return all_ok ? 0 : 1;
}
