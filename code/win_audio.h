// Audio
struct Win_Audio {
  IMMDeviceEnumerator * device_enum
  IAudioClient2* client;
  IAudioRenderClient* render_client;
  
  // "Secondary" buffer
  U32 buffer_size;
  S16* buffer;
  
  // Other variables for tracking purposes
  U32 latency_sample_count;
  U32 samples_per_second;
  U16 bits_per_sample;
  U16 channels;
    
	B32 is_device_changed;
	B32 is_device_ready;
};

static B32 
_win_audio_set_default_device_as_current_device(Win_Audio* audio) {
  IMMDevice* device;
  HRESULT hr = audio->device_enum->GetDefaultAudioEndpoint(eRender, 
                             eConsole, 
                             &device);
  if (FAILED(hr)) {
    win_log("[Win32::Audio] Failed to get audio endpoint\n");
    return false;
  }
  defer { device->Release(); };
  
  hr = device->Activate(__uuidof(IAudioClient2), 
                        CLSCTX_ALL, 
                        0, 
                        (LPVOID*)&audio->client);
  if(FAILED(hr)) {
    win_log("[Win32::Audio] Failed to create IAudioClient\n");
    return false;
  }
  
  WAVEFORMATEX wave_format = {};
  wave_format.wFormatTag = WAVE_FORMAT_PCM;
  wave_format.wBitsPerSample = audio->bits_per_sample;
  wave_format.nChannels = audio->channels;
  wave_format.nSamplesPerSec = audio->samples_per_second;
  wave_format.nBlockAlign = (wave_format.nChannels * wave_format.wBitsPerSample / 8);
  wave_format.nAvgBytesPerSec = wave_format.nSamplesPerSec * wave_format.nBlockAlign;
  
  REFERENCE_TIME buffer_duration = 0;
  hr = audio->client->GetDevicePeriod(nullptr, &buffer_duration);
  
  DWORD stream_flags = ( AUDCLNT_STREAMFLAGS_RATEADJUST 
                        | AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM
                        | AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY );

  hr = audio->client->Initialize(AUDCLNT_SHAREMODE_SHARED, 
                                 stream_flags, 
                                 buffer_duration,
                                 0, 
                                 &wave_format, 
                                 nullptr);
  if (FAILED(hr))
  {
    win_log("[Win32::Audio] Failed to initialize audio client\n");
    return false;
  }
  
  if (FAILED(audio->client->GetService(__uuidof(IAudioRenderClient),
                                       (LPVOID*)(&audio->render_client))))
  {
    win_log("[Win32::Audio] Failed to create IAudioClient\n");
    return false;
  }
  
  UINT32 sound_frame_count;
  hr = audio->client->GetBufferSize(&sound_frame_count);
  if (FAILED(hr))
  {
    win_log("[Win32::Audio] Failed to get buffer size\n");
    return false;
  }

  audio->buffer_size = sound_frame_count;
  audio->buffer = (S16*)Win32_AllocateMemory(audio->buffer_size);
  if (!audio->buffer) {
    win_log("[Win32::Audio] Failed to allocate secondary buffer\n");
    return false;
  }
	
	
  audio->client->Start();
	audio->is_device_ready = true;
  return true;

}

static B32
win_audio_init(Win_Audio* audio,
               U32 samples_per_second, 
               U16 bits_per_sample,
               U16 channels,
               U32 latency_frames,
               U32 refresh_rate)
{
  audio->channels = channels;
  audio->bits_per_sample = bits_per_sample;
  audio->samples_per_second = samples_per_second;
  audio->latency_sample_count = (samples_per_second / refresh_rate) * latency_frames;
  
  HRESULT hr = CoInitializeEx(0, COINIT_SPEED_OVER_MEMORY);
  if (FAILED(hr)) {
    win_log("[Win32::Audio] Failed CoInitializeEx\n");
    return false;
  }
  
  hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), 
                        0,
                        CLSCTX_ALL, 
                        __uuidof(IMMDeviceEnumerator),
                        (LPVOID*)(&audio->device_enum));
  if (FAILED(hr)) {
    win_log("[Win32::Audio] Failed to create IMMDeviceEnumerator\n");
    goto cleanup_1;
  }
    
	audio->notification_client.ref = 1;
	audio->notification_client.audio = audio;
	hr = audio->device_enum->RegisterEndpointNotificationCallback(&audio->notification_client);
	if(FAILED(hr)) {
		win_log("[Win32::Audio] Failed to register notification callback\n");
		goto cleanup_2;
	}
	
	// NOTE(Momo): Allocate the maximum buffer possible given allowed latency
	audio->buffer_size = audio->latency_sample_count * sizeof(S16);
  audio->buffer = (S16*)Win32_AllocateMemory(audio->buffer_size);
  if (!audio->buffer) {
    win_log("[Win32::Audio] Failed to allocate memory\n");
    goto cleanup_3;
  }
	
	Win32_Audio__InitDefaultDevice(audio);
	return true;
	
	// NOTE(Momo): Cleanup
	cleanup_3: {	
		Win32_FreeMemory(audio->buffer);
	}
	
	cleanup_2: {
		audio->device_enum->UnregisterEndpointNotificationCallback(&audio->notification_client);
	}
	
	cleanup_1: {
		audio->device_enum->Release();
	}
	return false;

}

static inline void 
_win_audio_release_current_device(Win_Audio* audio) {
	if (audio->client) {
		audio->client->Stop();
		audio->client->Release();
		audio->client = nullptr;
	}
	
	if (audio->render_client) {
		audio->render_client->Release();
		audio->render_client = nullptr;
	}
	audio->is_device_ready = false;
}

static void
win_audio_free(Win_Audio* audio) {
  Win32_Audio__FreeCurrentDevice(audio);
	audio->device_enum->UnregisterEndpointNotificationCallback(&audio->notification_client);
	audio->device_enum->Release();
	Win32_FreeMemory(audio->buffer);
}

static void 
win_audio_begin_frame(Win_Audio* audio) {
	if (audio->is_device_changed) {
		win_log("[Win32::Audio] Resetting audio device\n");
		// Attempt to change device
		Win32_Audio__FreeCurrentDevice(audio);
		Win32_Audio__InitDefaultDevice(audio);
		audio->is_device_changed = false;
	}
	
	
  Platform_Audio ret = {};
  
  UINT32 sound_padding_size;
  UINT32 samples_to_write = 0;
    
	if (audio->is_device_ready) {
		// Padding is how much valid data is queued up in the sound buffer
		// if there's enough padding then we could skip writing more data
		HRESULT hr = audio->client->GetCurrentPadding(&sound_padding_size);
		
		if (SUCCEEDED(hr)) {
			samples_to_write = (UINT32)audio->buffer_size - sound_padding_size;
			
			// Cap the samples to write to how much latency is allowed.
			if (samples_to_write > audio->latency_sample_count) {
				samples_to_write = audio->latency_sample_count;
			}
		}
	}
	else {
		// NOTE(Momo): if there is no device avaliable,
		// just write to the whole 'dummy' buffer.
		samples_to_write = audio->buffer_size;
	}
	
  ret.sample_buffer = audio->buffer;
  ret.sample_count = samples_to_write; 
  ret.channels = audio->channels;
  
  return ret;

}
static void
win_audio_end_frame(Win_Audio* audio, 
                    Platform_Audio* output) 
{
	if (!audio->is_device_ready) {
		return;
	}
  // NOTE(Momo): Kinda assumes 16-bit Sound
  BYTE* sound_buffer_data;
  if (SUCCEEDED(audio->render_client->GetBuffer((UINT32)output.sample_count, &sound_buffer_data))) 
  {
    S16* src_sample = output.sample_buffer;
    S16* dest_sample = (S16*)sound_buffer_data;
    // buffer structure for stereo:
    // S16   S16    S16  S16   S16  S16
    // [LEFT RIGHT] LEFT RIGHT LEFT RIGHT....
    for(U32 sample_index = 0; sample_index < output.sample_count; ++sample_index){
      for (U32 channel_index = 0; channel_index < audio->channels; ++channel_index) {
          *dest_sample++ = *src_sample++;
      }
    }
    audio->render_client->ReleaseBuffer((UINT32)output.sample_count, 0);
  }
}
