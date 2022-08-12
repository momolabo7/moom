
DEFINE_GUID(CLSID_MMDeviceEnumerator,  0xbcde0395, 0xe52f, 0x467c, 0x8e, 0x3d, 0xc4, 0x57, 0x92, 0x91, 0x69, 0x2e);
DEFINE_GUID(IID_IMMDeviceEnumerator,   0xa95664d2, 0x9614, 0x4f35, 0xa7, 0x46, 0xde, 0x8d, 0xb6, 0x36, 0x17, 0xe6);
DEFINE_GUID(IID_IAudioRenderClient,    0xf294acfc, 0x3146, 0x4483, 0xa7, 0xbf, 0xad, 0xdc, 0xa7, 0xc2, 0x60, 0xe2);
DEFINE_GUID(IID_IAudioClient2,         0x726778cd, 0xf60a, 0x4eda, 0x82, 0xde, 0xe4, 0x76, 0x10, 0xcd,0x78, 0xaa);
DEFINE_GUID(IID_IMMNotificationClient, 0x7991eec9, 0x7e89, 0x4d85, 0x83, 0x90, 0x6c, 0x70, 0x3c, 0xec, 0x60, 0xc0);




struct Win_Audio;
struct Win_Audio_Notif_Client {
  IMMNotificationClient imm_notif_client;
  Win_Audio* audio;
  LONG ref;
};

struct Win_Audio {
  Win_Audio_Notif_Client notif_client;
  IMMDeviceEnumerator * device_enum;
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

  Bump_Allocator* allocator;
};

///////////////////////////////////////////////////////////
// IMPLEMENTATION
//

//////////////////////////////////////////////////////////
// Win Audio Notifs implementation
static STDMETHODIMP_(ULONG)  
_win_audio_notif_client_AddRef(IMMNotificationClient* client) {
  Win_Audio_Notif_Client* my_client = (Win_Audio_Notif_Client*)client;
  return InterlockedIncrement(&my_client->ref);
}
static STDMETHODIMP_(ULONG)  
_win_audio_notif_client_Release(IMMNotificationClient* client) {
  Win_Audio_Notif_Client* my_client = (Win_Audio_Notif_Client*)client;
  return InterlockedDecrement(&my_client->ref);
}
static STDMETHODIMP_(HRESULT)  
_win_audio_notif_client_QueryInterface(IMMNotificationClient* client,
                                       REFIID riid,
                                       VOID **ppvInterface) 
{
  Win_Audio_Notif_Client* my_client = (Win_Audio_Notif_Client*)client;
  if (IID_IUnknown == riid)
  {
    _win_audio_notif_client_AddRef(client);
    *ppvInterface = (IUnknown*)my_client;
  }
  else if (IID_IMMNotificationClient == riid)
  {
    _win_audio_notif_client_AddRef(client);
    *ppvInterface = (IMMNotificationClient*)my_client;
  }
  else
  {
    *ppvInterface = 0;
    return E_NOINTERFACE;
  }
  return S_OK;
}
static STDMETHODIMP_(HRESULT) 
_win_audio_notif_client_OnDefaultDeviceChange(IMMNotificationClient* client,
                                              EDataFlow flow,
                                              ERole role,
                                              LPCWSTR pwstr_device_id)
{
  return S_OK;
}
static STDMETHODIMP_(HRESULT) 
_win_audio_notif_client_OnDeviceAdded(IMMNotificationClient* client, LPCWSTR pwstr_device_id)
{
  return S_OK;
}
static STDMETHODIMP_(HRESULT) 
_win_audio_notif_client_OnDeviceRemoved(IMMNotificationClient* client, LPCWSTR pwstr_device_id)
{
  return S_OK;
}

static STDMETHODIMP_(HRESULT) 
_win_audio_notif_client_OnDeviceStateChanged(IMMNotificationClient* client,
                                             LPCWSTR pwstr_device_id, 
                                             DWORD dwNewState)
{
  return S_OK;
}

static STDMETHODIMP_(HRESULT) 
_win_audio_notif_client_OnPropertyValueChanged(IMMNotificationClient* client,
                                               LPCWSTR pwstr_device_id, 
                                               const PROPERTYKEY key)
{
  return S_OK;
}


////////////////////////////////////////////////////
// Win Audio implementation
//
static IMMNotificationClientVtbl _win_audio_notif_client {
  _win_audio_notif_client_QueryInterface,
  _win_audio_notif_client_AddRef,
  _win_audio_notif_client_Release,
  _win_audio_notif_client_OnDeviceStateChanged,
  _win_audio_notif_client_OnDeviceAdded,
  _win_audio_notif_client_OnDeviceRemoved,
  _win_audio_notif_client_OnDefaultDeviceChange,
  _win_audio_notif_client_OnPropertyValueChanged,
};

static B32 
_win_audio_set_default_device_as_current_device(Win_Audio* audio) {
  IMMDevice* device;
  HRESULT hr = IMMDeviceEnumerator_GetDefaultAudioEndpoint(audio->device_enum, 
                                                           eRender, 
                                                           eConsole, 
                                                           &device);
  if (FAILED(hr)) {
    win_log("[win_audio] Failed to get audio endpoint\n");
    return false;
  }
  defer { IMMDevice_Release(device); };
  
  hr = IMMDevice_Activate(device,
                          IID_IAudioClient2, 
                          CLSCTX_ALL, 
                          0, 
                          (LPVOID*)&audio->client);
  if(FAILED(hr)) {
    win_log("[win_audio] Failed to create IAudioClient\n");
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
  hr = IAudioClient2_GetDevicePeriod(audio->client, 0, &buffer_duration);
  
  DWORD stream_flags = (AUDCLNT_STREAMFLAGS_RATEADJUST |
                        AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                        AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY);

  hr = IAudioClient2_Initialize(audio->client,
                                AUDCLNT_SHAREMODE_SHARED, 
                                stream_flags, 
                                buffer_duration,
                                0, 
                                &wave_format, 
                                0);
  if (FAILED(hr))
  {
    win_log("[win_audio] Failed to initialize audio client\n");
    return false;
  }
  
  hr = IAudioClient2_GetService(audio->client, 
                               IID_IAudioRenderClient, 
                               (LPVOID*)&audio->render_client);
  if (FAILED(hr))
  {
    win_log("[win_audio] Failed to create IAudioClient\n");
    return false;
  }
  
  UINT32 sound_frame_count;
  hr = IAudioClient2_GetBufferSize(audio->client, &sound_frame_count);
  if (FAILED(hr))
  {
    win_log("[win_audio] Failed to get buffer size\n");
    return false;
  }

  audio->buffer_size = sound_frame_count;
  audio->buffer = ba_push<S16>(audio->allocator, audio->buffer_size);
  if (!audio->buffer) {
    win_log("[win_audio] Failed to allocate secondary buffer\n");
    return false;
  }
	
  IAudioClient2_Start(audio->client);
	audio->is_device_ready = true;
  return true;

}
static B32
win_audio_init(Win_Audio* audio,
               U32 samples_per_second, 
               U16 bits_per_sample,
               U16 channels,
               U32 latency_frames,
               U32 refresh_rate,
               Bump_Allocator* allocator)
{
  audio->channels = channels;
  audio->bits_per_sample = bits_per_sample;
  audio->samples_per_second = samples_per_second;
  audio->latency_sample_count = (samples_per_second / refresh_rate) * latency_frames;
  audio->allocator = allocator;
  
  HRESULT hr = CoInitializeEx(0, COINIT_SPEED_OVER_MEMORY);
  if (FAILED(hr)) {
    win_log("[win_audio] Failed CoInitializeEx\n");
    return false;
  }
  
  hr = CoCreateInstance(CLSID_MMDeviceEnumerator, 
                        0,
                        CLSCTX_ALL, 
                        IID_IMMDeviceEnumerator,
                        (LPVOID*)(&audio->device_enum));
  if (FAILED(hr)) {
    win_log("[win_audio] Failed to create IMMDeviceEnumerator\n");
    goto cleanup_1;
  }
   
  audio->notif_client.imm_notif_client.lpVtbl = &_win_audio_notif_client;
	audio->notif_client.ref = 1;
	audio->notif_client.audio = audio;
  hr = IMMDeviceEnumerator_RegisterEndpointNotificationCallback(audio->device_enum, &audio->notif_client.imm_notif_client);

	if(FAILED(hr)) {
		win_log("[win_audio] Failed to register notification callback\n");
		goto cleanup_2;
	}
	
	// NOTE(Momo): Allocate the maximum buffer possible given allowed latency
	audio->buffer_size = audio->latency_sample_count * sizeof(S16);
  audio->buffer = ba_push<S16>(audio->allocator, audio->buffer_size);
  if (!audio->buffer) {
    win_log("[win_audio] Failed to allocate memory\n");
    goto cleanup_3;
  }
	
	return _win_audio_set_default_device_as_current_device(audio);
	
	// NOTE(Momo): Cleanup
	cleanup_3: 	
    ba_clear(audio->allocator);
	cleanup_2: 
		IMMDeviceEnumerator_UnregisterEndpointNotificationCallback(audio->device_enum, &audio->notif_client.imm_notif_client);
	cleanup_1: 
		IMMDeviceEnumerator_Release(audio->device_enum);

	return false;

}

static inline void 
_win_audio_release_current_device(Win_Audio* audio) {
	if (audio->client) {
		IAudioClient2_Stop(audio->client);
		IAudioClient2_Release(audio->client);
		audio->client = 0;
	}
	
	if (audio->render_client) {
		IAudioRenderClient_Release(audio->render_client);
		audio->render_client = 0;
	}
	audio->is_device_ready = false;
}

static void
win_audio_free(Win_Audio* audio) {
  _win_audio_release_current_device(audio);
	IMMDeviceEnumerator_UnregisterEndpointNotificationCallback(audio->device_enum, &audio->notif_client.imm_notif_client);
	IMMDeviceEnumerator_Release(audio->device_enum);
  ba_clear(audio->allocator);
}

static Platform_Audio 
win_audio_begin_frame(Win_Audio* audio) {
	if (audio->is_device_changed) {
		win_log("[win_audio] Resetting audio device\n");
		// Attempt to change device
		_win_audio_release_current_device(audio);
		_win_audio_set_default_device_as_current_device(audio);
		audio->is_device_changed = false;
	}
	
	
  Platform_Audio ret = {};
  
  UINT32 sound_padding_size;
  UINT32 samples_to_write = 0;
    
	if (audio->is_device_ready) {
		// Padding is how much valid data is queued up in the sound buffer
		// if there's enough padding then we could skip writing more data
		HRESULT hr = IAudioClient2_GetCurrentPadding(audio->client, &sound_padding_size);
		
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
                    Platform_Audio output) 
{
	if (!audio->is_device_ready) return;

  // NOTE(Momo): Kinda assumes 16-bit Sound
  BYTE* sound_buffer_data;
  HRESULT hr = IAudioRenderClient_GetBuffer(audio->render_client, 
                                            (UINT32)output.sample_count, 
                                            &sound_buffer_data);
  if (FAILED(hr)) return;

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
  IAudioRenderClient_ReleaseBuffer(audio->render_client, (UINT32)output.sample_count, 0);
}
