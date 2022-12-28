#ifndef WIN_WASAPI
#define WIN_WASAPI

struct Win_Wasapi;
struct Win_Wasapi_Notif_Client {
  IMMNotificationClient imm_notifs;
  Win_Wasapi* wasapi;
  LONG ref;
};

struct Win_Wasapi {
  Platform_Audio platform_audio; // Must be first member
  
  Win_Wasapi_Notif_Client notifs;
  IMMDeviceEnumerator * mm_device_enum;
  IAudioClient2* audio_client;
  IAudioRenderClient* audio_render_client;
  
  // "Secondary" buffer
  u32_t buffer_size;
  s16_t* buffer;
  
  // Other variables for tracking purposes
  u32_t latency_sample_count;
  u32_t samples_per_second;
  u16_t bits_per_sample;
  u16_t channels;
    
	b32_t is_device_changed;
	b32_t is_device_ready;

  arena_t allocator;
};

///////////////////////////////////////////////////////////
// IMPLEMENTATION
//
DEFINE_GUID(CLSID_MMDeviceEnumerator,  0xbcde0395, 0xe52f, 0x467c, 0x8e, 0x3d, 0xc4, 0x57, 0x92, 0x91, 0x69, 0x2e);
DEFINE_GUID(IID_IMMDeviceEnumerator,   0xa95664d2, 0x9614, 0x4f35, 0xa7, 0x46, 0xde, 0x8d, 0xb6, 0x36, 0x17, 0xe6);
DEFINE_GUID(IID_IAudioRenderClient,    0xf294acfc, 0x3146, 0x4483, 0xa7, 0xbf, 0xad, 0xdc, 0xa7, 0xc2, 0x60, 0xe2);
DEFINE_GUID(IID_IAudioClient2,         0x726778cd, 0xf60a, 0x4eda, 0x82, 0xde, 0xe4, 0x76, 0x10, 0xcd,0x78, 0xaa);
DEFINE_GUID(IID_IMMNotificationClient, 0x7991eec9, 0x7e89, 0x4d85, 0x83, 0x90, 0x6c, 0x70, 0x3c, 0xec, 0x60, 0xc0);



//////////////////////////////////////////////////////////
// Win Audio Notifs implementation
static STDMETHODIMP_(ULONG)  
_w32_wasapi_notif_client_AddRef(IMMNotificationClient* mm_notif) {
  Win_Wasapi_Notif_Client* w32_notif = (Win_Wasapi_Notif_Client*)mm_notif;
  return InterlockedIncrement(&w32_notif->ref);
}
static STDMETHODIMP_(ULONG)  
_w32_wasapi_notif_client_Release(IMMNotificationClient* mm_notif) {
  Win_Wasapi_Notif_Client* w32_notif = (Win_Wasapi_Notif_Client*)mm_notif;
  return InterlockedDecrement(&w32_notif->ref);
}
static STDMETHODIMP_(HRESULT)  
_w32_wasapi_notif_client_QueryInterface(IMMNotificationClient* mm_notif,
                                       REFIID riid,
                                       VOID **ppvInterface) 
{
  if (IID_IUnknown == riid)
  {
    _w32_wasapi_notif_client_AddRef(mm_notif);
    *ppvInterface = (IUnknown*)mm_notif;
  }
  else if (IID_IMMNotificationClient == riid)
  {
    _w32_wasapi_notif_client_AddRef(mm_notif);
    *ppvInterface = (IMMNotificationClient*)mm_notif;
  }
  else
  {
    *ppvInterface = 0;
    return E_NOINTERFACE;
  }
  return S_OK;
}
static STDMETHODIMP_(HRESULT) 
_w32_wasapi_notif_client_OnDefaultDeviceChange(IMMNotificationClient* mm_notif,
                                              EDataFlow flow,
                                              ERole role,
                                              LPCWSTR pwstr_device_id)
{
  Win_Wasapi_Notif_Client* w32_notif = (Win_Wasapi_Notif_Client*)mm_notif;
  w32_notif->wasapi->is_device_changed = true;
  return S_OK;
}
static STDMETHODIMP_(HRESULT) 
_w32_wasapi_notif_client_OnDeviceAdded(IMMNotificationClient* client, LPCWSTR pwstr_device_id)
{
  return S_OK;
}
static STDMETHODIMP_(HRESULT) 
_w32_wasapi_notif_client_OnDeviceRemoved(IMMNotificationClient* client, LPCWSTR pwstr_device_id)
{
  return S_OK;
}

static STDMETHODIMP_(HRESULT) 
_w32_wasapi_notif_client_OnDeviceStateChanged(IMMNotificationClient* client,
                                             LPCWSTR pwstr_device_id, 
                                             DWORD dwNewState)
{
  return S_OK;
}

static STDMETHODIMP_(HRESULT) 
_w32_wasapi_notif_client_OnPropertyValueChanged(IMMNotificationClient* client,
                                               LPCWSTR pwstr_device_id, 
                                               const PROPERTYKEY key)
{
  return S_OK;
}


////////////////////////////////////////////////////
// Win Audio implementation
//
static IMMNotificationClientVtbl _w32_wasapi_notifs_vtable {
  _w32_wasapi_notif_client_QueryInterface,
  _w32_wasapi_notif_client_AddRef,
  _w32_wasapi_notif_client_Release,
  _w32_wasapi_notif_client_OnDeviceStateChanged,
  _w32_wasapi_notif_client_OnDeviceAdded,
  _w32_wasapi_notif_client_OnDeviceRemoved,
  _w32_wasapi_notif_client_OnDefaultDeviceChange,
  _w32_wasapi_notif_client_OnPropertyValueChanged,
};

static b32_t 
_w32_wasapi_set_default_device_as_current_device(Win_Wasapi* wasapi) {
  IMMDevice* device;
  HRESULT hr = IMMDeviceEnumerator_GetDefaultAudioEndpoint(wasapi->mm_device_enum, 
                                                           eRender, 
                                                           eConsole, 
                                                           &device);
  if (FAILED(hr)) {
    w32_log("[w32_wasapi] Failed to get wasapi endpoint\n");
    return false;
  }
  defer { IMMDevice_Release(device); };
  
  hr = IMMDevice_Activate(device,
                          IID_IAudioClient2, 
                          CLSCTX_ALL, 
                          0, 
                          (LPVOID*)&wasapi->audio_client);
  if(FAILED(hr)) {
    w32_log("[w32_wasapi] Failed to create IAudioClient\n");
    return false;
  }
  
  WAVEFORMATEX wave_format = {};
  wave_format.wFormatTag = WAVE_FORMAT_PCM;
  wave_format.wBitsPerSample = wasapi->bits_per_sample;
  wave_format.nChannels = wasapi->channels;
  wave_format.nSamplesPerSec = wasapi->samples_per_second;
  wave_format.nBlockAlign = (wave_format.nChannels * wave_format.wBitsPerSample / 8);
  wave_format.nAvgBytesPerSec = wave_format.nSamplesPerSec * wave_format.nBlockAlign;
  
  REFERENCE_TIME buffer_duration = 0;
  hr = IAudioClient2_GetDevicePeriod(wasapi->audio_client, 0, &buffer_duration);
  
  DWORD stream_flags = (AUDCLNT_STREAMFLAGS_RATEADJUST |
                        AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                        AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY);

  hr = IAudioClient2_Initialize(wasapi->audio_client,
                                AUDCLNT_SHAREMODE_SHARED, 
                                stream_flags, 
                                buffer_duration,
                                0, 
                                &wave_format, 
                                0);
  if (FAILED(hr))
  {
    w32_log("[w32_wasapi] Failed to initialize wasapi client\n");
    return false;
  }
  
  hr = IAudioClient2_GetService(wasapi->audio_client, 
                               IID_IAudioRenderClient, 
                               (LPVOID*)&wasapi->audio_render_client);
  if (FAILED(hr))
  {
    w32_log("[w32_wasapi] Failed to create IAudioClient\n");
    return false;
  }
  
  UINT32 sound_frame_count;
  hr = IAudioClient2_GetBufferSize(wasapi->audio_client, &sound_frame_count);
  if (FAILED(hr))
  {
    w32_log("[w32_wasapi] Failed to get buffer size\n");
    return false;
  }

  arena_clear(&wasapi->allocator);
  wasapi->buffer_size = sound_frame_count;
  wasapi->buffer = arena_push_arr(s16_t, &wasapi->allocator, wasapi->buffer_size);
  if (!wasapi->buffer) {
    w32_log("[w32_wasapi] Failed to allocate secondary buffer\n");
    return false;
  }
  IAudioClient2_Start(wasapi->audio_client);
	wasapi->is_device_ready = true;
  return true;
}

static b32_t
w32_wasapi_init(Win_Wasapi* wasapi,
                u32_t samples_per_second, 
                u16_t bits_per_sample,
                u16_t channels,
                u32_t latency_frames,
                u32_t refresh_rate,
                arena_t* allocator)
{
  wasapi->channels = channels;
  wasapi->bits_per_sample = bits_per_sample;
  wasapi->samples_per_second = samples_per_second;
  wasapi->latency_sample_count = (samples_per_second / refresh_rate) * latency_frames;

  if (!arena_partition_with_remaining(allocator, 
                                   &wasapi->allocator, 
                                   16)) 
  {
    w32_log("[w32_wasapi] Failed to partition memory\n");
    return false;
  }
  
  HRESULT hr = CoInitializeEx(0, COINIT_SPEED_OVER_MEMORY);
  if (FAILED(hr)) {
    w32_log("[w32_wasapi] Failed CoInitializeEx\n");
    return false;
  }
  
  hr = CoCreateInstance(CLSID_MMDeviceEnumerator, 
                        0,
                        CLSCTX_ALL, 
                        IID_IMMDeviceEnumerator,
                        (LPVOID*)(&wasapi->mm_device_enum));
  if (FAILED(hr)) {
    w32_log("[w32_wasapi] Failed to create IMMDeviceEnumerator\n");
    goto cleanup_1;
  }
   
  wasapi->notifs.imm_notifs.lpVtbl = &_w32_wasapi_notifs_vtable;
	wasapi->notifs.ref = 1;
	wasapi->notifs.wasapi = wasapi;
  hr = IMMDeviceEnumerator_RegisterEndpointNotificationCallback(wasapi->mm_device_enum, &wasapi->notifs.imm_notifs);

	if(FAILED(hr)) {
		w32_log("[w32_wasapi] Failed to register notification callback\n");
		goto cleanup_2;
	}
	
	// NOTE(Momo): Allocate the maximum buffer possible given allowed latency
	wasapi->buffer_size = wasapi->latency_sample_count * sizeof(s16_t);
  wasapi->buffer = arena_push_arr(s16_t, &wasapi->allocator, wasapi->buffer_size);
  if (!wasapi->buffer) {
    w32_log("[w32_wasapi] Failed to allocate memory\n");
    goto cleanup_3;
  }

  // Does the success of this matter?
  // Do we even need to return success for this method??
  _w32_wasapi_set_default_device_as_current_device(wasapi);

	return true;
	
	// NOTE(Momo): Cleanup
	cleanup_3: 	
    arena_clear(&wasapi->allocator);
	cleanup_2: 
		IMMDeviceEnumerator_UnregisterEndpointNotificationCallback(wasapi->mm_device_enum, &wasapi->notifs.imm_notifs);
	cleanup_1:
		IMMDeviceEnumerator_Release(wasapi->mm_device_enum);

	return false;
}

static inline void 
_w32_wasapi_release_current_device(Win_Wasapi* wasapi) {
	if (wasapi->audio_client) {
		IAudioClient2_Stop(wasapi->audio_client);
		IAudioClient2_Release(wasapi->audio_client);
		wasapi->audio_client = 0;
	}
	
	if (wasapi->audio_render_client) {
		IAudioRenderClient_Release(wasapi->audio_render_client);
		wasapi->audio_render_client = 0;
	}
	wasapi->is_device_ready = false;
}

static void
w32_wasapi_free(Win_Wasapi* wasapi) {
  _w32_wasapi_release_current_device(wasapi);
	IMMDeviceEnumerator_UnregisterEndpointNotificationCallback(wasapi->mm_device_enum, &wasapi->notifs.imm_notifs);
	IMMDeviceEnumerator_Release(wasapi->mm_device_enum);
  arena_clear(&wasapi->allocator);
}

static void 
w32_wasapi_begin_frame(Win_Wasapi* wasapi) {
	if (wasapi->is_device_changed) {
		w32_log("[w32_wasapi] Resetting wasapi device\n");
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
		HRESULT hr = IAudioClient2_GetCurrentPadding(wasapi->audio_client, &sound_padding_size);
		
		if (SUCCEEDED(hr)) {
			samples_to_write = (UINT32)wasapi->buffer_size - sound_padding_size;
			
			// Cap the samples to write to how much latency is allowed.
			if (samples_to_write > wasapi->latency_sample_count) {
				samples_to_write = wasapi->latency_sample_count;
			}
		}
	}
	else {
		// NOTE(Momo): if there is no device avaliable,
		// just write to the whole 'dummy' buffer.
		samples_to_write = wasapi->buffer_size;
	}

  // Get Platform_Audio
  wasapi->platform_audio.sample_buffer = wasapi->buffer;
  wasapi->platform_audio.sample_count = samples_to_write; 
  wasapi->platform_audio.channels = wasapi->channels;

}
static void
w32_wasapi_end_frame(Win_Wasapi* wasapi) 
{
	if (!wasapi->is_device_ready) return;
  Platform_Audio* output = &wasapi->platform_audio;

  // NOTE(Momo): Kinda assumes 16-bit Sound
  BYTE* sound_buffer_data;
  HRESULT hr = IAudioRenderClient_GetBuffer(wasapi->audio_render_client, 
                                            (UINT32)output->sample_count, 
                                            &sound_buffer_data);
  if (FAILED(hr)) return;

  s16_t* src_sample = output->sample_buffer;
  s16_t* dest_sample = (s16_t*)sound_buffer_data;
  // buffer structure for stereo:
  // s16_t   s16_t    s16_t  s16_t   s16_t  s16_t
  // [LEFT RIGHT] LEFT RIGHT LEFT RIGHT....
  for(u32_t sample_index = 0; sample_index < output->sample_count; ++sample_index){
    for (u32_t channel_index = 0; channel_index < wasapi->channels; ++channel_index) {
      *dest_sample++ = *src_sample++;
    }
  }
  IAudioRenderClient_ReleaseBuffer(wasapi->audio_render_client, (UINT32)output->sample_count, 0);
}

/////////////////////////////////////////////////////////
// API Correspondence
//
static Platform_Audio*
w32_audio_load(u32_t samples_per_second, 
               u16_t bits_per_sample,
               u16_t channels,
               u32_t latency_frames,
               u32_t refresh_rate,
               arena_t* allocator) 
{
  // TODO: Ideally, we should give WASAPI a seperate arena 
  Win_Wasapi* wasapi = arena_push(Win_Wasapi, allocator);
  if (!wasapi) return 0;

  b32_t success = w32_wasapi_init(wasapi, samples_per_second, bits_per_sample, channels, latency_frames, refresh_rate, allocator);
  if (!success) return 0;

  return &wasapi->platform_audio;
}

static void
w32_audio_begin_frame(Platform_Audio* audio) {
  w32_wasapi_begin_frame((Win_Wasapi*)audio);
}

static void 
w32_audio_end_frame(Platform_Audio* audio) {
  w32_wasapi_end_frame((Win_Wasapi*)audio);
}

static void
w32_audio_unload(Platform_Audio* audio) {
  // Unused
}

#endif 
