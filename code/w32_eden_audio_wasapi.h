#ifndef __W32_EDEN_AUDIO_WASAPI_H__
#define __W32_EDEN_AUDIO_WASAPI_H__

#include <imm.h>
#include <initguid.h>
#include <audioclient.h>
#include <mmdeviceapi.h>

#include "w32_eden_audio.h"

struct w32_wasapi_t {
  //w32_wasapi_notif_client_t notifs;
  IMMDevice* mm_device;
  IMMDeviceEnumerator * mm_device_enum;
  IAudioClient* speaker_client;
  IAudioRenderClient* render_client;
  
  // Intermediate ring buffer for eden to write speaker to.
  u32_t buf_size;
  void* buffer;
  
  // Other variables for tracking purposes
  u32_t samples_per_second;
  u16_t bits_per_sample;
  u16_t channels;
  u32_t frame_rate;
    
	b32_t is_device_changed;
	b32_t is_device_ready;

  arena_t allocator;
};



static b32_t
_w32_wasapi_init_default_speaker_output_device(w32_wasapi_t* wasapi) {
  HRESULT hr;
  //
  // Use the device enumerator to find a default speaker device.
  //
  // 'eRender' is a flag to tell it to find an speaker rendering device (which are speaker
  // output devices like speakers, etc). For capture devices (mics), use 'eCapture'.
  //
  // Not really sure and don't really care what eConsole is for now.
  //
  hr = wasapi->mm_device_enum->GetDefaultAudioEndpoint(eRender, eConsole, &wasapi->mm_device);
  if (!SUCCEEDED(hr)) return false;

  // Create the speaker client
  hr = wasapi->mm_device->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, (LPVOID*)(&wasapi->speaker_client));
  if (!SUCCEEDED(hr)) return false;

                                               
  //
  // Initializes the speaker client.
  // 
  // Explanation of flags:
  //
  //   AUDCLNT_STREAMFLAGS_EVENTCALLBACK
  //     Enable events with speaker device. Will use this to register a callback 
  //     whenever the speaker's buffer need to be refilled.
  //
  //   AUDCLNT_STREAMFLAG_NOPERSIST
  //     Ensures that any thing we do to the device (like volume control) does not persist
  //     upon application restart
  //     
  //   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM
  //     An speaker client typically uses a fixed format (e.g. 32-bit or 16-bit, 48000hz or 44100 hz)
  //     This is obtainable with IAudioClient::GetMixFormat().
  //     This flag means that a mixer will be included that will help convert a given wave format
  //     to the one that the speaker client uses. We will use this flag because we will
  //     let the eden layer decide what format to use.
  //
  //  
  // 
  //
  DWORD flags = 
    (/*AUDCLNT_STREAMFLAGS_EVENTCALLBACK |*/ 
     AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
     AUDCLNT_STREAMFLAGS_NOPERSIST);
  

  WAVEFORMATEX wave_format = {};
  wave_format.wFormatTag = (wasapi->bits_per_sample == 32) ? WAVE_FORMAT_IEEE_FLOAT : WAVE_FORMAT_PCM;
  wave_format.wBitsPerSample = (WORD)wasapi->bits_per_sample;
  wave_format.nChannels = (WORD)wasapi->channels;
  wave_format.nSamplesPerSec = wasapi->samples_per_second;
  // this is always correct for WAVE_FORMAT_CPM and WAVE_FORMAT_IEEE_FLOAT
  wave_format.nBlockAlign = wave_format.nChannels * (wave_format.wBitsPerSample/8);   
  wave_format.nAvgBytesPerSec = wave_format.nSamplesPerSec * wave_format.nBlockAlign; 


  //
  // The buffer duration is a time value expressed in 100-nanosecond units
  // Basically, this is telling the speaker engine our refresh rate, which in turns
  // allows the speaker engine to know what is the minimum buffer size to provide
  // when our engine wants to write to the speaker buffer.
  //
  // Note that 1 millisecond = 1,000,000 nanoseconds = 10000 100-nanoseconds
  //
  REFERENCE_TIME buf_duration = wasapi->frame_rate * 10000; 
  
  hr = wasapi->speaker_client->Initialize(
      AUDCLNT_SHAREMODE_SHARED, 
      flags, 
      buf_duration, 
      0, 
      &wave_format, 
      NULL);

  if (!SUCCEEDED(hr)) return false;


  // Retrieves the render client. The render client is specifically the
  // part of the speaker client that plays sound. One render client represents
  // one speaker device (which can be an engine like NVdia Broadcast or a hardware).
  hr = wasapi->speaker_client->GetService(IID_PPV_ARGS(&wasapi->render_client));
  if (!SUCCEEDED(hr)) return false;




  // @todo: Do we just the the real buffer size and just use that value
  // to initialize our sound buffer size that our eden layer will write to?


  // Get the number of speaker frames that the buffer can hold.
  // Note that 1 'speaker frame' == 1 sample per second
  UINT32 buf_frame_count = 0;
  hr = wasapi->speaker_client->GetBufferSize(&buf_frame_count);
  if (!SUCCEEDED(hr)) return false;

  // Get the number of frames of padding
  UINT32 padding_frame_count = 0;
  hr = wasapi->speaker_client->GetCurrentPadding(&padding_frame_count);
  if (!SUCCEEDED(hr)) return false;

  // Initialize the secondary buffer.
  // UINT32 writable_frames = buf_frame_count - padding_frame_count;

  hr = wasapi->speaker_client->Start();
  if (!SUCCEEDED(hr)) return false;

  return true;
}

//
// API Correspondence
//
static void test_sine_wave(s32_t samples_per_second, s32_t sample_count, f32_t *samples, f32_t tone_hz = 440, f32_t tone_volume = 0.1) {
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

static void test_square_wave(s32_t samples_per_second, s32_t sample_count, f32_t *samples, f32_t tone_hz = 440, f32_t tone_volume = 0.1) {
  static f32_t t_sine = 0;
  s32_t wave_period = s32_t(samples_per_second / tone_hz);

  f32_t *sample_out = samples;
  for (int sample_index = 0; sample_index < sample_count; sample_index++) {
    f32_t sine_value = f32_sin(t_sine);
    if (sine_value > 0) sine_value = 1.f;
    else sine_value = -1.f;
    f32_t sample_value = (f32_t)(sine_value * tone_volume);
    *sample_out++ = sample_value;
    *sample_out++ = sample_value;

    t_sine += TAU_32 / (f32_t)wave_period;
    if (t_sine >= TAU_32) {
      t_sine -= TAU_32;
    }
  }
}


static 
w32_speaker_begin_frame_sig(w32_speaker_begin_frame) 
{
  auto* wasapi = (w32_wasapi_t*)(eden_speaker->platform_data);

  HRESULT hr; 
  
  // Check if device changed
  // @todo: Do we want to do the event method...?
  b32_t default_device_changed = false;
  {
    IMMDevice* current_default_device = nullptr;
    wasapi->mm_device_enum->GetDefaultAudioEndpoint(eRender, eConsole, &current_default_device);

    LPWSTR id1;
    LPWSTR id2;
    wasapi->mm_device->GetId(&id1);
    current_default_device->GetId(&id2);

    default_device_changed  = wcscmp(id1, id2) != 0;

    CoTaskMemFree(id1);
    CoTaskMemFree(id2);
    current_default_device->Release();
  }

  if (default_device_changed) {
    wasapi->speaker_client->Release();
    wasapi->render_client->Release();
    wasapi->mm_device->Release();
    if (!_w32_wasapi_init_default_speaker_output_device(wasapi)) {
      return false;
    }
  }


  // Get the number of speaker frames that the buffer can hold.
  UINT32 buf_frame_count = 0;
  hr = wasapi->speaker_client->GetBufferSize(&buf_frame_count);
  if (FAILED(hr)) return false;

  // Get the number of frames of padding
  UINT32 padding_frame_count = 0;
  hr = wasapi->speaker_client->GetCurrentPadding(&padding_frame_count);
  if (FAILED(hr)) return false;

  UINT32 samples_to_write = buf_frame_count - padding_frame_count; 

  // Setup for the eden layer
  eden_speaker->sample_count = samples_to_write; 
  eden_speaker->samples = nullptr;
  eden_speaker->device_bits_per_sample = wasapi->bits_per_sample;
  eden_speaker->device_channels = wasapi->channels;
  eden_speaker->device_samples_per_second = wasapi->samples_per_second;

  // Get the buffer 
  if (eden_speaker->sample_count > 0) 
  {
    // Ask for the address of the buffer with the size of sample_count.
    // This could actually fail for a multitude of reasons so it's 
    // probably good to account for that.
    BYTE* data = 0;

    // We should expect GetBuffer to fail.
    // In which we, we should do nothing, but the NEXT time it succees
    // it should continue playing the sound without breaking continuity.
    hr = wasapi->render_client->GetBuffer(eden_speaker->sample_count, &data);
    if (SUCCEEDED(hr)) {
      eden_speaker->samples = data;
    }

  }

#if 0
  w32_wasapi_t* wasapi = (w32_wasapi_t*)(eden_speaker->platform_data);
	if (wasapi->is_device_changed) {
		//w32_log("[w32_wasapi] Resetting wasapi device\n");
		// Attempt to change device
		_w32_wasapi_release_current_device(wasapi);
		_w32_wasapi_set_default_device_as_current_device(wasapi);
		wasapi->is_device_changed = false;
	}
	
  UINT32 sound_padding_size;
  UINT32 samples_to_write = 0;
    
	if (wasapi->is_device_ready) {
		// Padding is how much valid data is queued up in the sound buffer
		// if there's enough padding then we could skip writing more data
		HRESULT hr = IAudioClient2_GetCurrentPadding(wasapi->speaker_client, &sound_padding_size);
		
		if (SUCCEEDED(hr)) {
			samples_to_write = (UINT32)wasapi->buf_size - sound_padding_size;
			
			// Cap the samples to write to how much latency is allowed.
			if (samples_to_write > wasapi->latency_sample_count) {
				samples_to_write = wasapi->latency_sample_count;
			}
		}
	}
	else {
		// if there is no device avaliable,
		// just write to the whole 'dummy' buffer.
		samples_to_write = wasapi->buf_size;
	}

  eden_speaker->sample_buffer = wasapi->buffer;
  eden_speaker->sample_count = samples_to_write; 
  eden_speaker->channels = wasapi->channels;
#endif

  return true;

}

static 
w32_speaker_end_frame_sig(w32_speaker_end_frame) 
{
  auto* wasapi = (w32_wasapi_t*)(eden_speaker->platform_data);

  if (eden_speaker->sample_count > 0) {
    wasapi->render_client->ReleaseBuffer(eden_speaker->sample_count, 0);
  }
  

#if 0
	if (!wasapi->is_device_ready) return;

  // @note: Kinda assumes 16-bit Sound
  BYTE* sound_buf_data;
  HRESULT hr = IAudioRenderClient_GetBuffer(wasapi->render_client, 
                                            (UINT32)eden_speaker->sample_count, 
                                            &sound_buf_data);
  if (FAILED(hr)) return;

  s16_t* src_sample = eden_speaker->sample_buffer;
  s16_t* dest_sample = (s16_t*)sound_buf_data;
  // buffer structure for stereo:
  // s16_t   s16_t    s16_t  s16_t   s16_t  s16_t
  // [LEFT RIGHT] LEFT RIGHT LEFT RIGHT....
  for(u32_t sample_index = 0; sample_index < eden_speaker->sample_count; ++sample_index)
  {
    for (u32_t channel_index = 0; channel_index < wasapi->channels; ++channel_index) {
      *dest_sample++ = *src_sample++;
    }
  }

  IAudioRenderClient_ReleaseBuffer(
      wasapi->render_client, 
      (UINT32)eden_speaker->sample_count, 
      0);
#endif

  return true;
}


static 
w32_speaker_load_sig(w32_speaker_load)
{
  assert(channels == 1 || channels == 2);
  u32_t bits_per_sample = 0;
  switch(bitrate_type)
  {
    case EDEN_SPEAKER_BITRATE_TYPE_S16:
      bits_per_sample = 16;

  };


#if 0
  DWORD thread_id = 0;
  HANDLE handle = CreateThread(0, 0, w32_wasapi_run, 0, 0, &thread_id);
  SetThreadPriority(handle, THREAD_PRIORITY_TIME_CRITICAL);
  WaitForSingleObject(handle, INFINITE);
#endif


  auto* wasapi = arena_push(w32_wasapi_t, allocator);
  if (!wasapi) return false;

  eden_speaker->platform_data = wasapi;

  wasapi->frame_rate = frame_rate;
  wasapi->channels = channels;
  wasapi->bits_per_sample = bits_per_sample;
  wasapi->samples_per_second = samples_per_second;
#if 0
  //
  // Setup the intermediate buffer for eden to write speaker to.
  //
  // I don't really know the best buffer size to use AND I don't want to keep changing buffer
  // size when the device changes, so I'm just going to allocate 1 second worth of samples
  // This means that we are taking the strategy of allocating so much that we should not worry 
  // about having too little. 
  //
  wasapi->buf_size = samples_per_second * channels * bits_per_sample/8; // 1 second worth of samples
  wasapi->buffer = arena_push_size(allocator, wasapi->buf_size, 16);
  assert(wasapi->buffer);
#endif

  HRESULT hr;
  hr = CoInitializeEx(0, COINIT_SPEED_OVER_MEMORY);
  if (!SUCCEEDED(hr)) return false;

  //
  // Create the device enumerator.
  //
  // @note: The device enumerator is just a thing to enumerates 
  // through a list of devices. Note that this includes ALL devices (not just speaker!)
  //
  hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), 
                        0,
                        CLSCTX_INPROC_SERVER,
                        IID_PPV_ARGS(&wasapi->mm_device_enum));
  if (!SUCCEEDED(hr)) return false;
  

  if (!_w32_wasapi_init_default_speaker_output_device(wasapi)) {
    return false;
  }

  //
  // @todo: Enable Refill event here?
  //
#if 0 
  HANDLE hRefillEvent = CreateEventEx(NULL, NULL, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
  hr = speaker_client->SetEventHandle(hRefillEvent);
  assert(SUCCEEDED(hr));
#endif

  // Does the success of this matter?
  // Do we even need to return success for this method??
  //_w32_wasapi_set_default_device_as_current_device(wasapi);


  // Initialize mixer
  if(!eden_speaker_init(eden_speaker, bitrate_type, max_sounds, allocator))
    return false;


	return true;
	
}



static 
w32_speaker_unload_sig(w32_speaker_unload) {
  auto* wasapi = (w32_wasapi_t*)(eden_speaker->platform_data);
  wasapi->speaker_client->Release();
  wasapi->render_client->Release();
  wasapi->mm_device->Release();
  wasapi->mm_device_enum->Release();


#if 0
  _w32_wasapi_release_current_device(wasapi);
	IMMDeviceEnumerator_UnregisterEndpointNotificationCallback(wasapi->mm_device_enum, &wasapi->notifs.imm_notifs);
	IMMDeviceEnumerator_Release(wasapi->mm_device_enum);
  arena_clear(&wasapi->allocator);
#endif
}

#endif
