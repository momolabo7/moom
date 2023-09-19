#include "momo.h"

#include <windows.h>
#include <stdio.h>

#include <mmsystem.h>
#include <mmdeviceapi.h>
#include <audioclient.h>

#define SafeRelease(ppT) do { if (*ppT) { (*ppT)->Release(); *ppT = NULL; } } while(0)

static void output_test_sine_wave_f32(s32_t samples_per_second, s32_t sample_count, f32_t *samples, f32_t tone_hz = 440, f32_t tone_volume = 0.5) {
  static f32_t t_sine = 0;
  s32_t wave_period = s32_t(samples_per_second / tone_hz);

  f32_t *sample_out = samples;
  for (int sample_index = 0; sample_index < sample_count; sample_index++) {
    f32_t sine_value = f32_sin(t_sine);
    f32_t sample_value = (f32_t)(sine_value * tone_volume);
    *sample_out++ = sample_value;
    *sample_out++ = sample_value;

    t_sine += TAU_32 / (f32_t)wave_period;
    if (t_sine >= TAU_32) {
      t_sine -= TAU_32;
    }
  }
}

static DWORD
wasapi__run(void* passthrough) {
  //
  // Bunch of COM code to initialize WASAPI
  //
  HRESULT hr;
  hr = CoInitializeEx(0, COINIT_SPEED_OVER_MEMORY);
  assert(SUCCEEDED(hr));

  IMMDevice* mm_device = NULL;
  IMMDeviceEnumerator* mm_device_enumerator = NULL;

  hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&mm_device_enumerator));
  assert(SUCCEEDED(hr));

  hr = mm_device_enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &mm_device);
  assert(SUCCEEDED(hr));

  IAudioClient* audio_client = NULL;
  hr = mm_device->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, (void**)(&audio_client));
  assert(SUCCEEDED(hr));


  WAVEFORMATEX* mix_format = NULL;
  audio_client->GetMixFormat(&mix_format);
  assert(mix_format->nChannels == 2);
  assert(mix_format->wBitsPerSample == 32);

  //
  // TODO: Do we need to change this according to what 
  // frame rate our app is on?
  //
  REFERENCE_TIME buffer_duration = 60 * 10000; //  60ms
  
  
  DWORD flags = AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST;
  hr = audio_client->Initialize(AUDCLNT_SHAREMODE_SHARED, flags, buffer_duration, 0, mix_format, NULL);
  assert(SUCCEEDED(hr));

  IAudioRenderClient* render_client = NULL;
  hr = audio_client->GetService(IID_PPV_ARGS(&render_client));
  assert(SUCCEEDED(hr));

  UINT32 bufferFrameCount = 0;
  hr = audio_client->GetBufferSize(&bufferFrameCount);
  assert(SUCCEEDED(hr));

  HANDLE hRefillEvent = CreateEventEx(NULL, NULL, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
  hr = audio_client->SetEventHandle(hRefillEvent);
  assert(SUCCEEDED(hr));

  {
    BYTE *data = NULL;
    hr = render_client->GetBuffer(bufferFrameCount, &data);
    assert(SUCCEEDED(hr));

    hr = render_client->ReleaseBuffer(bufferFrameCount, AUDCLNT_BUFFERFLAGS_SILENT);
    assert(SUCCEEDED(hr));
  }

  u64_t total_frames_written = 0;

  hr = audio_client->Start();
  while (true) {
    auto res = WaitForSingleObject(hRefillEvent, INFINITE);

    if (res == WAIT_OBJECT_0)
    {
#if 1
      {
        IAudioClock* audio_clock = NULL;
        audio_client->GetService(IID_PPV_ARGS(&audio_clock));
        if (audio_clock)
        {
          UINT64 freq = 0;
          audio_clock->GetFrequency(&freq);
          UINT64 position = 0;
          audio_clock->GetPosition(&position, NULL);
          f64_t sec = (f64_t)(position) / (f64_t)(freq);

          u64_t total_bytes_written = total_frames_written * mix_format->nBlockAlign;
          printf("AudioClock: freq=%lldHz, pos=%10lld, %10.5fsec -- Total Bytes Written: %10lld \n", freq, position, sec, total_bytes_written);
          //assert(total_bytes_written >= position);
          audio_clock->Release();
        }
      }
#endif

      // Check if default device has changed
      bool default_device_changed = false;
      {
        IMMDevice* current_default_device = NULL;
        mm_device_enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &current_default_device);

        //
        LPWSTR id1;
        LPWSTR id2;
        mm_device->GetId(&id1);
        current_default_device->GetId(&id2);

        default_device_changed = wcscmp(id1, id2) != 0;

        CoTaskMemFree(id1);
        CoTaskMemFree(id2);
        current_default_device->Release();
      }

      // See how much buffer space is available.
      UINT32 numFramesPadding = 0;
      hr = audio_client->GetCurrentPadding(&numFramesPadding);

      // NOTE(nick): check for device change
      if (hr == AUDCLNT_E_DEVICE_INVALIDATED || default_device_changed)
      {
        hr = audio_client->Stop();
        assert(SUCCEEDED(hr));

        SafeRelease(&render_client);
        SafeRelease(&audio_client);
        SafeRelease(&mm_device);

        {
          hr = mm_device_enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &mm_device);
          assert(SUCCEEDED(hr));

          hr = mm_device->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, (void**)(&audio_client));
          assert(SUCCEEDED(hr));

          audio_client->GetMixFormat(&mix_format);
          assert(mix_format->nChannels == 2);
          assert(mix_format->wBitsPerSample == 32);

          hr = audio_client->Initialize(AUDCLNT_SHAREMODE_SHARED, flags, buffer_duration, 0, mix_format, NULL);
          assert(SUCCEEDED(hr));

          hr = audio_client->GetService(IID_PPV_ARGS(&render_client));
          assert(SUCCEEDED(hr));

          hr = audio_client->GetBufferSize(&bufferFrameCount);
          assert(SUCCEEDED(hr));

          hr = audio_client->SetEventHandle(hRefillEvent);
          assert(SUCCEEDED(hr));
        } 

        hr = audio_client->Start();
        continue;
      }

      // NOTE(nick): output sound
      UINT32 SampleCount = bufferFrameCount - numFramesPadding;
      if (SampleCount > 0)
      {
        // Grab all the available space in the shared buffer.
        BYTE *data = NULL;
        hr = render_client->GetBuffer(SampleCount, &data);

        f32_t* fData = (f32_t *)data;

        auto SamplesPerSecond = mix_format->nSamplesPerSec;
        f32_t *samples = (f32_t *)data;
        {
          output_test_sine_wave_f32(SamplesPerSecond, SampleCount, samples, 440, 0.5);
        }

        hr = render_client->ReleaseBuffer(SampleCount, 0);
        total_frames_written += SampleCount;
      }
    }
  }
}

int APIENTRY CALLBACK
WinMain(HINSTANCE instance, 
    HINSTANCE prev_instance, 
    LPSTR argv, 
    int argc) 
{
  HANDLE stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
  if (!stdout_handle) {
    AttachConsole(ATTACH_PARENT_PROCESS);
  }
  DWORD thread_id = 0;
  HANDLE handle = CreateThread(0, 0, wasapi__run, 0, 0, &thread_id);
  SetThreadPriority(handle, THREAD_PRIORITY_TIME_CRITICAL);
  WaitForSingleObject(handle, INFINITE);
}
