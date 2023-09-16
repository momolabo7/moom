// 
// DESCRIPTION
//   This is my dear 2D game engine on win32 platform.
//
// FLAGS
//   HOT_RELOADABLE - Enables hot-reload on game.dll when it changes. Default is 1.
// 
// BOOKMARKS
//
//   Memory Management - For managing allocated memory from OS.
//   Work Queue        - Multithreaded work queue system
//   Hot Reload        - Hot reloading system 
//   File              - File system 
//   State             - Main global state
//   Audio             - Audio interfaces
//   Wasapi            - Audio implementation with WASAPI
//   Graphics          - Graphics interfaces
//   Opengl            - Graphics implementation with OGL
//   

// NOTE(momo): For now, we enable these flags
// These macros are in preparation in case we have
// multiple ways to do audio or graphics


#ifndef GAME_USE_WASAPI
# define GAME_USE_WASAPI 1
#endif


#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif

#ifndef HOT_RELOADABLE
# define HOT_RELOADABLE 1
#endif

#include <windows.h>
#include <windowsx.h>
#include <timeapi.h>
#include <imm.h>
#include <initguid.h>
#include <audioclient.h>
#include <mmdeviceapi.h>
#undef near
#undef far

#include "momo.h"
#include "game.h"


//
// MARK:(Memory Management)
//
struct w32_memory_t {
  void* memory;
  usz_t size;
  
  w32_memory_t* prev;
  w32_memory_t* next;
};

//
// MARK:(Work Queue)
//
struct w32_work_t {
  void* data;
  game_task_callback_f* callback;
};

struct w32_work_queue_t {
  w32_work_t entries[256];
  u32_t volatile next_entry_to_read;
  u32_t volatile next_entry_to_write;
  
  u32_t volatile completion_count;
  u32_t volatile completion_goal;
  HANDLE semaphore; 
  
};

//
// MARK:(Hot Reload)
//
struct w32_loaded_code_t {
  // Need to fill these up
  u32_t function_count;
  const char** function_names;
  const char* module_path;
  void** functions;
  
  LARGE_INTEGER module_write_time;
  const char* tmp_path;
  
  b32_t is_valid;
  HMODULE dll; 
};

//
// MARK:(File)
//
struct w32_file_t {
  HANDLE handle;
  u32_t cabinet_index;
};

// TODO(momo): is it possible to use a vector? 
struct w32_file_cabinet_t {
  w32_file_t files[32]; 
  u32_t free_files[32];
  u32_t free_file_count;
};

//
// MARK:(State)
//
struct w32_state_t {
  b32_t is_running;

  // cursor locking system
  b32_t is_cursor_locked;
  POINT cursor_pt_to_lock_to;
  
  f32_t game_width;
  f32_t game_height;
  
  w32_work_queue_t work_queue;
  w32_file_cabinet_t file_cabinet;
  
  HWND window;

  w32_memory_t memory_sentinel;
  game_t* game;
};
static w32_state_t w32_state;

#include <stdio.h>

//
// MARK:(Logging)
//
static 
game_debug_log_sig(w32_log_proc) 
{
  char buffer[256] = {0};
  va_list args;
  va_start(args, fmt);
  vsprintf(buffer, fmt, args);
  va_end(args);
  OutputDebugStringA(buffer);
}
#define w32_log(...) w32_log_proc(__VA_ARGS__)

//
// Mark:(Audio)
//
#define w32_audio_load_sig(name) b32_t name(game_audio_t* game_audio, u32_t samples_per_second, u16_t bits_per_sample, u16_t channels, u32_t latency_frames, u32_t frame_rate, arena_t* allocator)
static w32_audio_load_sig(w32_audio_load);

#define w32_audio_unload_sig(name) void name(game_audio_t* game_audio)
static w32_audio_unload_sig(w32_audio_unload);

#define w32_audio_begin_frame_sig(name) void name(game_audio_t* game_audio)
static w32_audio_begin_frame_sig(w32_audio_begin_frame);

#define w32_audio_end_frame_sig(name) void name(game_audio_t* game_audio)
static w32_audio_end_frame_sig(w32_audio_end_frame);


//
// Mark:(Wasapi)
//
#ifdef GAME_USE_WASAPI 
struct w32_wasapi_t;

struct w32_wasapi_t {
  //w32_wasapi_notif_client_t notifs;
  IMMDevice* mm_device;
  IMMDeviceEnumerator * mm_device_enum;
  IAudioClient* audio_client;
  IAudioRenderClient* render_client;
  
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

#endif // GAME_USE_WASAPI



//
// MARK:(Gfx)
// 
#define w32_gfx_load_sig(name) b32_t  name(game_gfx_t* gfx, HWND window, arena_t* arena, usz_t command_queue_size, usz_t texture_queue_size, usz_t max_textures, usz_t max_payloads, usz_t max_sprites, usz_t max_triangles)
static w32_gfx_load_sig(w32_gfx_load);

#define w32_gfx_begin_frame_sig(name) void name(game_gfx_t* gfx, v2u_t render_wh, u32_t region_x0, u32_t region_y0, u32_t region_x1, u32_t region_y1)
static w32_gfx_begin_frame_sig(w32_gfx_begin_frame);

#define w32_gfx_end_frame_sig(name) void name(game_gfx_t* gfx)
static w32_gfx_end_frame_sig(w32_gfx_end_frame);



//
// MARK:(Opengl)
// 
#ifdef GAME_USE_OPENGL
#define WGL_CONTEXT_MAJOR_VERSION_ARB           0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB           0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB             0x2093
#define WGL_CONTEXT_FLAGS_ARB                   0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB            0x9126

#define WGL_DRAW_TO_WINDOW_ARB                  0x2001
#define WGL_ACCELERATION_ARB                    0x2003
#define WGL_SUPPORT_OPENGL_ARB                  0x2010
#define WGL_DOUBLE_BUFFER_ARB                   0x2011
#define WGL_PIXEL_TYPE_ARB                      0x2013
#define WGL_TYPE_RGBA_ARB                       0x202B
#define WGL_FULL_ACCELERATION_ARB               0x2027
#define WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB        0x20A9
#define WGL_CONTEXT_FLAG_ARB                    0x2094
#define WGL_CONTEXT_DEBUG_BIT_ARB               0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB  0x0002
#define WGL_CONTEXT_MOMO_PROFILE_BIT_ARB        0x00000001

typedef BOOL WINAPI wglChoosePixelFormatARBFn(HDC hdc, const int* piAttribIList, const FLOAT* pfAttribFList, UINT nMaxFormats, int* piFormats, UINT* nNumFormats);

typedef BOOL WINAPI wglSwapIntervalEXTFn(int interval);

typedef HGLRC WINAPI wglCreateContextAttribsARBFn(HDC hdc, HGLRC hShareContext, const int* attribList);

typedef const char* WINAPI wglGetExtensionsStringEXTFn(void);

static wglChoosePixelFormatARBFn* wglChoosePixelFormatARB;
static wglSwapIntervalEXTFn* wglSwapIntervalEXT;
static wglCreateContextAttribsARBFn* wglCreateContextAttribsARB;
static wglGetExtensionsStringEXTFn* wglGetExtensionsStringEXT;



static void* 
_w32_try_get_wgl_function(const char* name, HMODULE fallback_module)
{
  void* p = (void*)wglGetProcAddress(name);
  if ((p == 0) || 
      (p == (void*)0x1) || 
      (p == (void*)0x2) || 
      (p == (void*)0x3) || 
      (p == (void*)-1))
  {
    p = (void*)GetProcAddress(fallback_module, name);
  }
  return p;
  
}

static void
_w32_set_pixel_format(HDC dc) {
  s32_t suggested_pixel_format_index = 0;
  u32_t extended_pick = 0;
  
  if (wglChoosePixelFormatARB) {
    s32_t attrib_list[] = {
      WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
      WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
      WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
      WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
      WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
      WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, GL_TRUE,
      0,
    };
    
    wglChoosePixelFormatARB(dc, attrib_list, 0, 1,
                            &suggested_pixel_format_index, &extended_pick);
    
  }
  
  if (!extended_pick) {
    PIXELFORMATDESCRIPTOR desired_pixel_format = {};
    desired_pixel_format.nSize = sizeof(desired_pixel_format);
    desired_pixel_format.nVersion = 1;
    desired_pixel_format.iPixelType = PFD_TYPE_RGBA;
    desired_pixel_format.dwFlags = 
      PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER; 
    desired_pixel_format.cColorBits = 32;
    desired_pixel_format.cAlphaBits = 8;
    desired_pixel_format.iLayerType = PFD_MAIN_PLANE;
    
    // Here, we ask windows to find the best supported pixel 
    // format based on our desired format.
    suggested_pixel_format_index = 
      ChoosePixelFormat(dc, &desired_pixel_format);
  }
  PIXELFORMATDESCRIPTOR suggested_pixel_format = {};
  
  DescribePixelFormat(dc, suggested_pixel_format_index, 
                      sizeof(suggested_pixel_format), 
                      &suggested_pixel_format);
  SetPixelFormat(dc, 
                 suggested_pixel_format_index, 
                 &suggested_pixel_format);
}
static b32_t
_w32_load_wgl_extentions() {
  WNDCLASSA window_class = {};
  // Er yeah...we have to create a 'fake' opengl_t context 
  // to load the extensions lol.
  window_class.lpfnWndProc = DefWindowProcA;
  window_class.hInstance = GetModuleHandle(0);
  window_class.lpszClassName = "WGLLoader2";
  
  if (RegisterClassA(&window_class)) {
    HWND window = CreateWindowExA( 
                                  0,
                                  window_class.lpszClassName,
                                  "wgl Loader2",
                                  0,
                                  CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  0,
                                  0,
                                  window_class.hInstance,
                                  0);
    
    HDC dc = GetDC(window);
    _w32_set_pixel_format(dc);
    HGLRC opengl_context = wglCreateContext(dc);
    
    b32_t success = true;
    
    if (wglMakeCurrent(dc, opengl_context)) {
      wglChoosePixelFormatARB = (wglChoosePixelFormatARBFn*)wglGetProcAddress("wglChoosePixelFormatARB");
      wglCreateContextAttribsARB = (wglCreateContextAttribsARBFn*)wglGetProcAddress("wglCreateContextAttribsARB");
      
      if (!wglChoosePixelFormatARB || !wglCreateContextAttribsARB) {
        success = false;
      }
      
      DestroyWindow(window);
      ReleaseDC(window, dc);
      wglDeleteContext(opengl_context);
      wglMakeCurrent(0, 0);
      return success;
    }
    else {
      return false;
    }
    
  }
  else {
    DWORD test = GetLastError();
    return false;
  }
}


static 
w32_gfx_load_sig(w32_gfx_load)
{
  HDC dc = GetDC(window); 
  if (!dc) return 0;
  if (!_w32_load_wgl_extentions()) return 0;

  _w32_set_pixel_format(dc);
  
  s32_t opengl_attribs[] {
    WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
    WGL_CONTEXT_MINOR_VERSION_ARB, 5,
    WGL_CONTEXT_FLAG_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
#if 0
    | WGL_CONTEXT_DEBUG_BIT_ARB
#endif
    ,
    WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_MOMO_PROFILE_BIT_ARB,
    0,
  };
  HGLRC opengl_ctx = wglCreateContextAttribsARB(dc, 0, 
                                                opengl_attribs); 
  
  if (!opengl_ctx) {
    return false;
  }

  auto* opengl = arena_push(game_gfx_opengl_t, arena);
  gfx->platform_data = opengl;

  if (!opengl) {
    return false;
  }
  
  
  if(wglMakeCurrent(dc, opengl_ctx)) {
    HMODULE module = LoadLibraryA("opengl32.dll");
#define wgl_set_opengl_function(name) \
opengl->name = (game_gfx_opengl_##name*)_w32_try_get_wgl_function(#name, module); \
if (!opengl->name) { return false; } 
    wgl_set_opengl_function(glEnable);
    wgl_set_opengl_function(glDisable); 
    wgl_set_opengl_function(glViewport);
    wgl_set_opengl_function(glScissor);
    wgl_set_opengl_function(glCreateShader);
    wgl_set_opengl_function(glCompileShader);
    wgl_set_opengl_function(glShaderSource);
    wgl_set_opengl_function(glAttachShader);
    wgl_set_opengl_function(glDeleteShader);
    wgl_set_opengl_function(glClear);
    wgl_set_opengl_function(glClearColor);
    wgl_set_opengl_function(glCreateBuffers);
    wgl_set_opengl_function(glNamedBufferStorage);
    wgl_set_opengl_function(glCreateVertexArrays);
    wgl_set_opengl_function(glVertexArrayVertexBuffer);
    wgl_set_opengl_function(glEnableVertexArrayAttrib);
    wgl_set_opengl_function(glVertexArrayAttribFormat);
    wgl_set_opengl_function(glVertexArrayAttribBinding);
    wgl_set_opengl_function(glVertexArrayBindingDivisor);
    wgl_set_opengl_function(glBlendFunc);
    wgl_set_opengl_function(glBlendFuncSeparate);
    wgl_set_opengl_function(glCreateProgram);
    wgl_set_opengl_function(glLinkProgram);
    wgl_set_opengl_function(glGetProgramiv);
    wgl_set_opengl_function(glGetProgramInfoLog);
    wgl_set_opengl_function(glVertexArrayElementBuffer);
    wgl_set_opengl_function(glCreateTextures);
    wgl_set_opengl_function(glTextureStorage2D);
    wgl_set_opengl_function(glTextureSubImage2D);
    wgl_set_opengl_function(glBindTexture);
    wgl_set_opengl_function(glTexParameteri);
    wgl_set_opengl_function(glBindVertexArray);
    wgl_set_opengl_function(glDrawElementsInstancedBaseInstance);
    wgl_set_opengl_function(glGetUniformLocation);
    wgl_set_opengl_function(glNamedBufferSubData);
    wgl_set_opengl_function(glProgramUniform4fv);
    wgl_set_opengl_function(glProgramUniformMatrix4fv);
    wgl_set_opengl_function(glUseProgram);
    wgl_set_opengl_function(glDeleteTextures);
    wgl_set_opengl_function(glDebugMessageCallbackARB);
    wgl_set_opengl_function(glDrawArrays);
  }
#undef wgl_set_opengl_function
  
  if (!game_gfx_opengl_init(
        gfx, 
        arena,
        command_queue_size,
        texture_queue_size,
        max_textures,
        max_payloads,
        max_sprites,
        max_triangles)) 
  {
    return false;
  }
  
#if 0
  opengl->glEnable(GL_DEBUG_OUTPUT);
  opengl->glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  opengl->glDebugMessageCallbackARB(Win_opengl_t_DebugCallback, nullptr);
#endif
  
  
  // VSync. 
  if (!wglSwapIntervalEXT) {
    wglSwapIntervalEXT = (wglSwapIntervalEXTFn*)wglGetProcAddress("wglSwapIntervalEXT");
  }
  if (wglSwapIntervalEXT) {
    wglSwapIntervalEXT(1);
  }

  return true;
}


static 
w32_gfx_begin_frame_sig(w32_gfx_begin_frame)
{
  game_gfx_opengl_begin_frame(gfx, render_wh, region_x0, region_y0, region_x1, region_y1);
}

static
w32_gfx_end_frame_sig(w32_gfx_end_frame) {
  game_gfx_opengl_end_frame(gfx);
  SwapBuffers(wglGetCurrentDC());
}
#endif // GAME_USE_OPENGL

////////////////////////////////////////////////////////////////////////
//
// Implementations
//


//
// MARK:(Wasapi)
//

//
// This is a thread!
//
static DWORD 
w32_wasapi_run(void* passthrough) {

  while(true) {
    w32_log("Hello\n"); 
  }
  return 0;
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
w32_audio_begin_frame_sig(w32_audio_begin_frame) 
{
  auto* wasapi = (w32_wasapi_t*)(game_audio->platform_data);

  HRESULT hr; 

  // Get the number of audio frames that the buffer can hold.
  UINT32 buffer_frame_count = 0;
  hr = wasapi->audio_client->GetBufferSize(&buffer_frame_count);
  assert(SUCCEEDED(hr));

  // Get the number of frames of padding
  UINT32 padding_frame_count = 0;
  hr = wasapi->audio_client->GetCurrentPadding(&padding_frame_count);
  assert(SUCCEEDED(hr));


  //
  // Thus the total number of frames we can write is:
  // frames_we_can_write = buffer_frame_count - padding_frame_count
  //
  UINT32 sample_count = buffer_frame_count - padding_frame_count; 
  if (sample_count > 0) {
    // Ask for the address of the buffer with the size of sample_count.
    // This could actually fail for a multitude of reasons so it's 
    // probably good to account for that.
    BYTE* data = 0;

    // We should expect GetBuffer to fail.
    // In which we, we should do nothing, but the NEXT time it succees
    // it should continue playing the sound without breaking continuity.
    //
    hr = wasapi->render_client->GetBuffer(sample_count, &data);
    if (SUCCEEDED(hr)) {
      f32_t* samples = (f32_t*)data;
      auto samples_per_second = wasapi->samples_per_second;
      test_square_wave(samples_per_second, sample_count, samples);
      wasapi->render_client->ReleaseBuffer(sample_count, 0);
    }



  }


#if 0
  w32_wasapi_t* wasapi = (w32_wasapi_t*)(game_audio->platform_data);
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

  game_audio->sample_buffer = wasapi->buffer;
  game_audio->sample_count = samples_to_write; 
  game_audio->channels = wasapi->channels;
#endif

}

static 
w32_audio_end_frame_sig(w32_audio_end_frame) 
{
  // Output sound here

  auto* wasapi = (w32_wasapi_t*)(game_audio->platform_data);

  

#if 0
	if (!wasapi->is_device_ready) return;

  // NOTE(Momo): Kinda assumes 16-bit Sound
  BYTE* sound_buffer_data;
  HRESULT hr = IAudioRenderClient_GetBuffer(wasapi->render_client, 
                                            (UINT32)game_audio->sample_count, 
                                            &sound_buffer_data);
  if (FAILED(hr)) return;

  s16_t* src_sample = game_audio->sample_buffer;
  s16_t* dest_sample = (s16_t*)sound_buffer_data;
  // buffer structure for stereo:
  // s16_t   s16_t    s16_t  s16_t   s16_t  s16_t
  // [LEFT RIGHT] LEFT RIGHT LEFT RIGHT....
  for(u32_t sample_index = 0; sample_index < game_audio->sample_count; ++sample_index)
  {
    for (u32_t channel_index = 0; channel_index < wasapi->channels; ++channel_index) {
      *dest_sample++ = *src_sample++;
    }
  }

  IAudioRenderClient_ReleaseBuffer(
      wasapi->render_client, 
      (UINT32)game_audio->sample_count, 
      0);
#endif
}


static 
w32_audio_load_sig(w32_audio_load)
{
  assert(bits_per_sample == 32 || bits_per_sample == 8 || bits_per_sample == 16);
  assert(channels == 1 || channels == 2);

#if 0
  DWORD thread_id = 0;
  HANDLE handle = CreateThread(0, 0, w32_wasapi_run, 0, 0, &thread_id);
  SetThreadPriority(handle, THREAD_PRIORITY_TIME_CRITICAL);
  WaitForSingleObject(handle, INFINITE);
#endif

#if 1

  auto* wasapi = arena_push(w32_wasapi_t, allocator);
  assert(wasapi);

  game_audio->platform_data = wasapi;

  wasapi->channels = channels;
  wasapi->bits_per_sample = bits_per_sample;
  wasapi->samples_per_second = samples_per_second;
  wasapi->latency_sample_count = (samples_per_second / frame_rate) * latency_frames;


  HRESULT hr;
  hr = CoInitializeEx(0, COINIT_SPEED_OVER_MEMORY);
  assert(SUCCEEDED(hr));

  
  //
  // Create the device enumerator.
  //
  // NOTE(momo): The device enumerator is just a thing to enumerates 
  // through a list of devices. Note that this includes ALL devices (not just audio!)
  //
  hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), 
                        0,
                        CLSCTX_INPROC_SERVER,
                        IID_PPV_ARGS(&wasapi->mm_device_enum));
  assert(SUCCEEDED(hr));
   
  //
  // Use the device enumerator to find a default audio device.
  //
  // 'eRender' is a flag to tell it to find an audio rendering device (which are audio
  // output devices like speakers, etc). For capture devices (mics), use 'eCapture'.
  //
  // Not really sure and don't really care what eConsole is for now.
  //
  hr = wasapi->mm_device_enum->GetDefaultAudioEndpoint(eRender, eConsole, &wasapi->mm_device);
  assert(SUCCEEDED(hr));

  // Create the audio client
  hr = wasapi->mm_device->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, NULL, (LPVOID*)(&wasapi->audio_client));
  assert(SUCCEEDED(hr));

                                               
  //
  // Initializes the audio client.
  // 
  // Explanation of flags:
  //
  //   AUDCLNT_STREAMFLAGS_EVENTCALLBACK
  //     Enable events with audio device. Will use this to register a callback 
  //     whenever the audio's buffer need to be refilled.
  //
  //   AUDCLNT_STREAMFLAG_NOPERSIST
  //     Ensures that any thing we do to the device (like volume control) does not persist
  //     upon application restart
  //     
  //   AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM
  //     An audio client typically uses a fixed format (e.g. 32-bit or 16-bit, 48000hz or 44100 hz)
  //     This is obtainable with IAudioClient::GetMixFormat().
  //     This flag means that a mixer will be included that will help convert a given wave format
  //     to the one that the audio client uses. We will use this flag because we will
  //     let the game layer decide what format to use.
  // 
  DWORD flags = 
    (/*AUDCLNT_STREAMFLAGS_EVENTCALLBACK |*/ 
     AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
     AUDCLNT_STREAMFLAGS_NOPERSIST);
  

  WAVEFORMATEX wave_format = {};
  wave_format.wFormatTag = (bits_per_sample == 32) ? WAVE_FORMAT_IEEE_FLOAT : WAVE_FORMAT_PCM;
  wave_format.wBitsPerSample = bits_per_sample;
  wave_format.nChannels = channels;
  wave_format.nSamplesPerSec = samples_per_second;
  // this is always correct for WAVE_FORMAT_CPM and WAVE_FORMAT_IEEE_FLOAT
  wave_format.nBlockAlign = wave_format.nChannels * (wave_format.wBitsPerSample/8);   
  wave_format.nAvgBytesPerSec = wave_format.nSamplesPerSec * wave_format.nBlockAlign; 



  // The buffer duration is a time value expressed in 100-nanosecond units
  // Basically, this is telling the audio engine what buffer size the
  // audio engine should provide. 
  //
  // Note that 1 millisecond = 1,000,000 nanoseconds = 10000 100-nanoseconds
  REFERENCE_TIME buffer_duration = frame_rate * 10000; 
  
  hr = wasapi->audio_client->Initialize(
      AUDCLNT_SHAREMODE_SHARED, 
      flags, 
      buffer_duration, 
      0, 
      &wave_format, 
      NULL);

  assert(SUCCEEDED(hr));


  // Retrieves the render client. The render client is specifically the
  // part of the audio client that plays sound.
  hr = wasapi->audio_client->GetService(IID_PPV_ARGS(&wasapi->render_client));
  assert(SUCCEEDED(hr));

  hr = wasapi->audio_client->Start();
  assert(SUCCEEDED(hr));

  


  //
  // TODO: Enable Refill event here?
  //
#if 0 
  HANDLE hRefillEvent = CreateEventEx(NULL, NULL, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
  hr = audio_client->SetEventHandle(hRefillEvent);
  assert(SUCCEEDED(hr));
#endif

  // Does the success of this matter?
  // Do we even need to return success for this method??
  //_w32_wasapi_set_default_device_as_current_device(wasapi);

#endif
	return true;
	
}



static 
w32_audio_unload_sig(w32_audio_unload) {
  auto* wasapi = (w32_wasapi_t*)(game_audio->platform_data);
  wasapi->audio_client->Release();
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

#define w32_profile_block(...) profiler_block(profiler, __VA_ARGS__)

static inline LONG w32_rect_width(RECT r) { return r.right - r.left; }
static inline LONG w32_rect_height(RECT r) { return r.bottom - r.top; }

static inline v2u_t
w32_get_window_dims(HWND window) {
	RECT rect;
	GetWindowRect(window, &rect);
  
  v2u_t ret;
  ret.w = u32_t(rect.right - rect.left);
  ret.h = u32_t(rect.bottom - rect.top);
  
  return ret;
	
}

static v2u_t
w32_get_client_dims(HWND window) {
	RECT rect;
	GetClientRect(window, &rect);
  
  v2u_t ret;
  ret.w = u32_t(rect.right - rect.left);
  ret.h = u32_t(rect.bottom - rect.top);
  
  return ret;
	
}

static RECT 
w32_calc_render_region(u32_t window_w, 
                       u32_t window_h, 
                       f32_t aspect_ratio)
{
	assert(aspect_ratio > 0.f && window_w > 0 && window_h > 0);
  
	RECT ret;
	
	f32_t optimal_window_w = (f32_t)window_h * aspect_ratio;
	f32_t optimal_window_h = (f32_t)window_w * 1.f/aspect_ratio;
	
	if (optimal_window_w > (f32_t)window_w) {
		// NOTE(Momo): width has priority - top and bottom bars
		ret.left = 0;
		ret.right = window_w;
		
		f32_t empty_height = (f32_t)window_h - optimal_window_h;
		
		ret.bottom = (u32_t)(empty_height * 0.5f);
		ret.top = ret.bottom + (u32_t)optimal_window_h;
	}
	else {
		// NOTE(Momo): height has priority - left and right bars
		ret.bottom = 0;
		ret.top = window_h;
		
		
		f32_t empty_width = (f32_t)window_w - optimal_window_w;
		
		ret.left = (u32_t)(empty_width * 0.5f);
		ret.right = ret.left + (u32_t)optimal_window_w;
	}
	
	return ret;
}

static LARGE_INTEGER
w32_get_performance_counter(void) {
  LARGE_INTEGER result;
  QueryPerformanceCounter(&result);
  return result;
}

static u64_t
w32_get_performance_counter_u64(void) {
  LARGE_INTEGER counter = w32_get_performance_counter();
  u64_t ret = (u64_t)counter.QuadPart;
  return ret;
}
static f32_t
w32_get_secs_elapsed(LARGE_INTEGER start,
                     LARGE_INTEGER end,
                     LARGE_INTEGER performance_frequency) 
{
  return (f32_t(end.QuadPart - start.QuadPart)) / performance_frequency.QuadPart;
}

static inline LARGE_INTEGER
w32_file_time_to_large_integer(FILETIME file_time) {
  LARGE_INTEGER ret = {};
  ret.LowPart = file_time.dwLowDateTime;
  ret.HighPart = file_time.dwHighDateTime;
  
  return ret;
}

static inline LARGE_INTEGER 
w32_get_file_last_write_time(const char* filename) {
  WIN32_FILE_ATTRIBUTE_DATA data;
  FILETIME last_write_time = {};
  
  if(GetFileAttributesEx(filename, GetFileExInfoStandard, &data)) {
    last_write_time = data.ftLastWriteTime;
  }
  return w32_file_time_to_large_integer(last_write_time); 
}

// NOTE(Momo): This function is accessed by multiple threads!
static b32_t
w32_do_next_work_entry(w32_work_queue_t* wq) {
  b32_t should_sleep = false;
  
  // NOTE(Momo): Generally, we want to do: 
  // 1. Get index of next work to do
  // 2. Update index of next work to do
  // 3. Do work
  // 
  // HOWEVER, the thread that is running this
  // function might go into coma at ANY TIME 
  // between these steps, that's why we need to do 
  // the way we are doing below. 
  //
  // ONLY when this thread successfully updated the work 
  // queue's next-work index to the value that this thread
  // THINKS it should be updated to, THEN we do the work.
  
  u32_t old_next_entry_to_read = wq->next_entry_to_read;
  u32_t new_next_entry_to_read = (old_next_entry_to_read + 1) % array_count(wq->entries);
  
  if (old_next_entry_to_read != wq->next_entry_to_write) {
    DWORD initial_value = 
      InterlockedCompareExchange((LONG volatile*)&wq->next_entry_to_read,
                                 new_next_entry_to_read,
                                 old_next_entry_to_read);
    if (initial_value == old_next_entry_to_read) {
      w32_work_t work = wq->entries[old_next_entry_to_read];
      work.callback(work.data);
      InterlockedIncrement((LONG volatile*)&wq->completion_count);
    }
    
  }
  else {
    should_sleep = true;
  }
  return should_sleep;
}

// NOTE(Momo): This makes the main thread
// participate in the work queue.
//
// In some sense, calling this will simulate 'async' 
// feature in modern languages, where we 'wait' until all
// work in the work queue is done!
//
static void
w32_complete_all_tasks_entries(w32_work_queue_t* wq) {
  while(wq->completion_goal != wq->completion_count) {
    w32_do_next_work_entry(wq);
  }
  wq->completion_goal = 0;
  wq->completion_count = 0;
}



static DWORD WINAPI 
w32_worker_func(LPVOID ctx) {
  w32_work_queue_t* wq = (w32_work_queue_t*)ctx;
  
  while(true) {
    if (w32_do_next_work_entry(wq)){
      WaitForSingleObjectEx(wq->semaphore, INFINITE, FALSE);
    }
    
  }
}


static b32_t
w32_init_work_queue(w32_work_queue_t* wq, u32_t thread_count) {
  wq->semaphore = CreateSemaphoreEx(0,
                                    0,                                
                                    thread_count,
                                    0, 0, SEMAPHORE_ALL_ACCESS);
  
  if (wq->semaphore == NULL) return false;
  
  for (u32_t i = 0; i < thread_count; ++i) {
    DWORD thread_id;
    HANDLE thread = CreateThread(NULL, 0, 
                                 w32_worker_func, 
                                 wq, 
                                 0, //CREATE_SUSPENDED, 
                                 &thread_id);
    if (thread == NULL) {
      return false;
    }
    CloseHandle(thread);
  }
  
  return true;
}

// NOTE(Momo): This is not very thread safe. Other threads shouldn't call this.
static void
w32_add_task_entry(w32_work_queue_t* wq, void (*callback)(void* ctx), void *data) {
  u32_t old_next_entry_to_write = wq->next_entry_to_write;
  u32_t new_next_entry_to_write = (old_next_entry_to_write + 1) % array_count(wq->entries);
  assert(wq->next_entry_to_read != new_next_entry_to_write);  
  
  w32_work_t* entry = wq->entries + old_next_entry_to_write;
  entry->callback = callback;
  entry->data = data;
  ++wq->completion_goal;
  
  _ReadWriteBarrier();
  
  wq->next_entry_to_write = new_next_entry_to_write; // this MUST not be reordered
  ReleaseSemaphore(wq->semaphore, 1, 0);
}


static void
w32_init_file_cabinet(w32_file_cabinet_t* c) {
  for(u32_t i = 0; i < array_count(c->files); ++i) {
    c->files[i].cabinet_index = i;
    c->free_files[i] = i;
  }
  c->free_file_count = array_count(c->files);
}

static w32_file_t*
w32_get_next_free_file(w32_file_cabinet_t* c) {
  if (c->free_file_count == 0) {
    return nullptr;
  }
  u32_t free_file_index = c->free_files[c->free_file_count--];
  return c->files + free_file_index; 
  
}

static void
w32_return_file(w32_file_cabinet_t* c, w32_file_t* f) {
  c->free_files[c->free_file_count++] = f->cabinet_index;
}

static void
w32_free_all_memory() {
  w32_memory_t* sentinel = &w32_state.memory_sentinel; 
  w32_memory_t* itr = sentinel->next;
  while(itr != sentinel) {
    w32_memory_t* next = itr->next;
    cll_remove(itr);
    VirtualFree(itr, 0, MEM_RELEASE);
    itr = next;
  }
}


static void 
w32_shutdown() {
  w32_state.is_running = false;
}

static void
w32_unload_code(w32_loaded_code_t* code) {
  if(code->dll) {
    FreeLibrary(code->dll);
    code->dll = 0;
  }
  code->is_valid = false;
  zero_range(code->functions, code->function_count);
}

static void
w32_load_code(w32_loaded_code_t* code) {
  code->is_valid = false;
#if HOT_RELOADABLE 
  b32_t copy_success = false;
  for (u32_t attempt = 0; attempt < 100; ++attempt) {
    if(CopyFile(code->module_path, code->tmp_path, false)) {
      copy_success = true;
      break;
    }
    Sleep(100);
  }
  code->dll = LoadLibraryA(code->tmp_path);
#else // HOT_RELOADABLE
  code->dll = LoadLibraryA(code->module_path);
#endif // HOT_RELOADABLE
  if (code->dll) {
    code->is_valid = true;
    for (u32_t function_index = 0; 
         function_index < code->function_count; 
         ++function_index) 
    {
      void* function = GetProcAddress(code->dll, code->function_names[function_index]);
      if (!function) {
        code->is_valid = false;
        break;
      }
      code->functions[function_index] = function;
    }
    
  }
  
  if(!code->is_valid) {
    w32_unload_code(code);
  }
  
  
  
}

static b32_t
w32_reload_code_if_outdated(w32_loaded_code_t* code) {
  b32_t reloaded = false;
  // Check last modified date
  LARGE_INTEGER last_write_time = w32_get_file_last_write_time(code->module_path);
  if(last_write_time.QuadPart > code->module_write_time.QuadPart) { 
    w32_unload_code(code); 
    for (u32_t i = 0; i < 100; ++i ){
      w32_load_code(code);
      if (code->is_valid) {
        w32_log("[%s] reloaded successfully\n", code->module_path);
        code->module_write_time = w32_get_file_last_write_time(code->module_path);
        reloaded = true;
        break;
      }
      Sleep(100);
    }
  }
  return reloaded;
}

static game_button_code_t
w32_vkeys_to_game_button_code(u32_t code) {

  // A to Z
  if (code >= 0x41 && code <= 0x5A) {
    return game_button_code_t(GAME_BUTTON_CODE_A + code - 0x41);
  }
  
  // 0 to 9
  else if (code >= 0x30 && code <= 0x39) {
    return game_button_code_t(GAME_BUTTON_CODE_0 + code - 0x30);
  }

  // F1 to F12
  // NOTE(momo): there are actually more F-keys??
  else if (code >= 0x70 && code <= 0x7B) {
    return game_button_code_t(GAME_BUTTON_CODE_F1 + code - 0x70);
  }
  else {
    switch(code) {
      case VK_SPACE: return GAME_BUTTON_CODE_SPACE;
    }

  }
  return GAME_BUTTON_CODE_UNKNOWN;
}

// TODO: change 'rr' to 'render_region'
static void
w32_update_input(game_input_t* input, HWND window, f32_t delta_time, RECT rr) 
{
  // Update input
  for (u32_t i = 0; i < array_count(input->buttons); ++i) 
  {
    input->buttons[i].before = input->buttons[i].now;
  }
  input->char_count = 0;
  input->mouse_scroll_delta = 0;
  input->delta_time = delta_time;

  //
  // Process messages
  //
  MSG msg = {};
  while(PeekMessage(&msg, window, 0, 0, PM_REMOVE)) {
    switch(msg.message) {
      case WM_MOUSEWHEEL: {
        s16_t delta = GET_WHEEL_DELTA_WPARAM(msg.wParam)/WHEEL_DELTA;
        //s16_t x_pos = GET_X_LPARAM(msg.lParam); 
        //s16_t y_pos = GET_Y_LPARAM(msg.lParam); 
        input->mouse_scroll_delta = delta;
      } break;
      case WM_CHAR: {
        assert(input->char_count < array_count(input->chars));
        input->chars[input->char_count++] = (u8_t)msg.wParam;
      } break;
      case WM_QUIT:
      case WM_DESTROY:
      case WM_CLOSE: {
        w32_state.is_running = false;
      } break;

      case WM_LBUTTONUP:
      case WM_LBUTTONDOWN: {
        b32_t is_key_down = msg.message == WM_LBUTTONDOWN;
        input->buttons[GAME_BUTTON_CODE_LMB].now = is_key_down;
      } break;

      case WM_MBUTTONUP:
      case WM_MBUTTONDOWN: {
        b32_t is_key_down = msg.message == WM_MBUTTONDOWN;
        input->buttons[GAME_BUTTON_CODE_MMB].now = is_key_down;
      } break;

      case WM_RBUTTONUP:
      case WM_RBUTTONDOWN: {
        b32_t is_key_down = msg.message == WM_RBUTTONDOWN;
        input->buttons[GAME_BUTTON_CODE_RMB].now = is_key_down;
      } break;
      
      case WM_KEYUP:
      case WM_KEYDOWN:
      case WM_SYSKEYDOWN:
      case WM_SYSKEYUP:
      {
        u32_t code = (u32_t)msg.wParam;
        b32_t is_key_down = msg.message == WM_KEYDOWN;
        input->buttons[w32_vkeys_to_game_button_code(code)].now = is_key_down;

        TranslateMessage(&msg);
      } break;
      
      default: {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    }
    
  }

  //
  // Mouse Input
  //
  
  POINT cursor_pos = {0};
  GetCursorPos(&cursor_pos);
  ScreenToClient(window, &cursor_pos);
  
  
  f32_t render_mouse_pos_x = (f32_t)(cursor_pos.x - rr.left);
  f32_t render_mouse_pos_y = (f32_t)(cursor_pos.y - rr.bottom);

  f32_t region_width = (f32_t)(rr.right - rr.left);
  f32_t region_height = (f32_t)(rr.top - rr.bottom);

  f32_t game_to_render_w = w32_state.game_width / region_width;
  f32_t game_to_render_h = w32_state.game_height / region_height;
  
  input->mouse_pos.x = render_mouse_pos_x * game_to_render_w;
  input->mouse_pos.y = render_mouse_pos_y * game_to_render_h;
  
  
  // NOTE(Momo): Flip y
  //game.design_mouse_pos.y = f32_lerp(MOMO_HEIGHT, 0.f, game.design_mouse_pos.y/MOMO_HEIGHT);	
  if (w32_state.is_cursor_locked) {
    SetCursorPos(
        w32_state.cursor_pt_to_lock_to.x,
        w32_state.cursor_pt_to_lock_to.y);
  }
    
}

static void
w32_set_game_dims(f32_t width, f32_t height) {
  assert(width > 0.f && height > 0.f);

  // Ignore if there is no change
  if (width == w32_state.game_width && height == w32_state.game_height) return;

  w32_state.game_width = width;
  w32_state.game_height = height;

  // Get monitor info
  HMONITOR monitor = MonitorFromWindow(0, MONITOR_DEFAULTTONEAREST);
  MONITORINFOEX monitor_info;
  monitor_info.cbSize = sizeof(monitor_info);
  GetMonitorInfo(monitor, &monitor_info); 

  RECT client_dims;
  GetClientRect(w32_state.window, &client_dims); 

  RECT window_dims;
  GetWindowRect(w32_state.window, &window_dims);

  POINT diff;
  diff.x = (window_dims.right - window_dims.left) - client_dims.right;
  diff.y = (window_dims.bottom - window_dims.top) - client_dims.bottom;

  LONG monitor_w = w32_rect_width(monitor_info.rcMonitor);
  LONG monitor_h = w32_rect_height(monitor_info.rcMonitor);
 
  LONG left = monitor_w/2 - (u32_t)width/2;
  LONG top = monitor_h/2 - (u32_t)height/2;

  // Make it right at the center!
  MoveWindow(w32_state.window, left, top, (s32_t)width + diff.x, (s32_t)height + diff.y, TRUE);

}

static 
game_show_cursor_sig(w32_show_cursor)
{
  while(ShowCursor(1) < 0);
}

static  
game_hide_cursor_sig(w32_hide_cursor) {
  while(ShowCursor(0) >= 0);
}

static 
game_lock_cursor_sig(w32_lock_cursor) {
  w32_state.is_cursor_locked = true;
  GetCursorPos(&w32_state.cursor_pt_to_lock_to);
}

static 
game_unlock_cursor_sig(w32_unlock_cursor) {
  w32_state.is_cursor_locked = false;
}

//
// Main functions
//
LRESULT CALLBACK
w32_window_callback(HWND window, 
                    UINT message, 
                    WPARAM w_param,
                    LPARAM l_param) 
{
  LRESULT result = 0;
  switch(message) {
    case WM_CLOSE:  
    case WM_QUIT:
    case WM_DESTROY: {
      w32_state.is_running = false;
    } break;
    default: {
      result = DefWindowProcA(window, message, w_param, l_param);
    };   
  }
  return result;
}

// 
// App function definitions
//

static 
game_open_file_sig(w32_open_file)
{
  // Opening the file
  DWORD access_flag = {};
  DWORD creation_disposition = {};
  switch (file_access) {
    case GAME_FILE_ACCESS_READ: {
      access_flag = GENERIC_READ;
      creation_disposition = OPEN_EXISTING;
    } break;
    case GAME_FILE_ACCESS_OVERWRITE: {
      access_flag = GENERIC_WRITE;
      creation_disposition = CREATE_ALWAYS;
    } break;
    /*
    case Platform_File_Access_Modify: {
      access_flag = GENERIC_READ | GENERIC_WRITE;
      creation_disposition = OPEN_ALWAYS;
    } break;
    */
    
  }
  
  HANDLE handle = CreateFileA(filename,
                              access_flag,
                              FILE_SHARE_READ,
                              0,
                              creation_disposition,
                              0,
                              0) ;
  if (handle == INVALID_HANDLE_VALUE) {
    file->data = nullptr;
    return false;
  }
  else {
    
    w32_file_t* w32_file = w32_get_next_free_file(&w32_state.file_cabinet);
    assert(w32_file);
    w32_file->handle = handle;
    
    file->data = w32_file;
    return true;
  }
}

static 
game_close_file_sig(w32_close_file)
{
  w32_file_t* w32_file = (w32_file_t*)file->data;
  CloseHandle(w32_file->handle);
  
  w32_return_file(&w32_state.file_cabinet, w32_file);
  file->data = nullptr;
}

static 
game_read_file_sig(w32_read_file)
{ 
  w32_file_t* w32_file = (w32_file_t*)file->data;
  
  // Reading the file
  OVERLAPPED overlgameed = {};
  overlgameed.Offset = (u32_t)((offset >> 0) & 0xFFFFFFFF);
  overlgameed.OffsetHigh = (u32_t)((offset >> 32) & 0xFFFFFFFF);
  
  DWORD bytes_read;
  
  if(ReadFile(w32_file->handle, dest, (DWORD)size, &bytes_read, &overlgameed) &&
     (DWORD)size == bytes_read) 
  {
    return true;
  }
  else {
    return false;
  }
}

static 
game_get_file_size_sig(w32_get_file_size)
{ 
  w32_file_t* w32_file = (w32_file_t*)file->data;
 
  LARGE_INTEGER file_size;
  if (!GetFileSizeEx(w32_file->handle, &file_size)) {
    assert(false);
  }
  
  u64_t ret = (u64_t)file_size.QuadPart;
  return ret;

}

static  
game_write_file_sig(w32_write_file)
{
  w32_file_t* w32_file = (w32_file_t*)file->data;
  
  OVERLAPPED overlgameed = {};
  overlgameed.Offset = (u32_t)((offset >> 0) & 0xFFFFFFFF);
  overlgameed.OffsetHigh = (u32_t)((offset >> 32) & 0xFFFFFFFF);
  
  DWORD bytes_wrote;
  if(WriteFile(w32_file->handle, src, (DWORD)size, &bytes_wrote, &overlgameed) &&
     (DWORD)size == bytes_wrote) 
  {
    return true;
  }
  else {
    return false;
  }
}

static 
game_add_task_sig(w32_add_task)
{
  w32_add_task_entry(&w32_state.work_queue, callback, data);
}

static 
game_complete_all_tasks_sig(w32_complete_all_tasks) 
{
  w32_complete_all_tasks_entries(&w32_state.work_queue);
}

static 
game_allocate_memory_sig(w32_allocate_memory)
{
  usz_t aligned_size = align_up_pow2(size, 16);
  usz_t padding_for_alignment = aligned_size - size;
  usz_t total_size = size + padding_for_alignment + sizeof(w32_memory_t);
  usz_t base_offset = sizeof(w32_memory_t);

  auto* block = (w32_memory_t*)
    VirtualAllocEx(GetCurrentProcess(),
                   0, 
                   total_size,
                   MEM_RESERVE | MEM_COMMIT, 
                   PAGE_READWRITE);
  if (!block) return nullptr;

  block->memory = (u8_t*)block + base_offset; 
  block->size = size;

  w32_memory_t* sentinel = &w32_state.memory_sentinel;
  cll_append(sentinel, block);

  return block->memory;

}

static
game_free_memory_sig(w32_free_memory) {
  if (ptr) {
    auto* memory_block = (w32_memory_t*)(ptr);
    cll_remove(memory_block);
    VirtualFree(memory_block, 0, MEM_RELEASE);
  }
}




static b32_t
w32_allocate_memory_into_arena(arena_t* a, usz_t memory_size) {
  void* data = w32_allocate_memory(memory_size);
  if(data == nullptr) return false;
  arena_init(a, data, memory_size);
  return true;
}

//
// Entry Point
//
int CALLBACK
WinMain(HINSTANCE instance, 
        HINSTANCE prev_instance, 
        LPSTR command_line, 
        int show_code) 
{
  SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
  ImmDisableIME((DWORD)-1);

  game_t game = {};

  game.is_running = true;
  game.show_cursor = w32_show_cursor;
  game.lock_cursor = w32_lock_cursor;
  game.hide_cursor = w32_hide_cursor;
  game.unlock_cursor = w32_unlock_cursor;
  game.allocate_memory = w32_allocate_memory;
  game.free_memory = w32_free_memory;
  game.debug_log = w32_log_proc;
  game.add_task = w32_add_task;
  game.complete_all_tasks = w32_complete_all_tasks;
  game.set_design_dimensions = w32_set_game_dims;
  game.open_file = w32_open_file;
  game.read_file = w32_read_file;
  game.write_file = w32_write_file;
  game.get_file_size = w32_get_file_size;
  game.close_file = w32_close_file;

  //
  // Initialize w32 state
  //
  {
    w32_state.is_running = true;

    w32_state.game_width = 1.f;
    w32_state.game_height = 1.f;  

    // initialize the circular linked list
    w32_state.memory_sentinel.next = &w32_state.memory_sentinel;    
    w32_state.memory_sentinel.prev = &w32_state.memory_sentinel;    

    w32_state.game = &game;

    if (!w32_init_work_queue(&w32_state.work_queue, 8)) {
      return 1;
    }
    w32_init_file_cabinet(&w32_state.file_cabinet);
  }
  defer { w32_free_all_memory(); };
  

  //
  // Load game Functions
  //
  game_functions_t game_functions = {};
  w32_loaded_code_t game_code = {};
  game_code.function_count = array_count(game_function_names);
  game_code.function_names = game_function_names;
  game_code.module_path = "game.dll";
  game_code.functions = (void**)&game_functions;
  game_code.tmp_path = "tmp_game.dll";

  w32_load_code(&game_code);
  if (!game_code.is_valid) return 1;
  defer { w32_unload_code(&game_code); };
  game_config_t config = game_functions.get_config();

  //
  //- Create window in the middle of the screen
  //
  HWND window;
  {
    const int window_w = (int)800;
    const int window_h = (int)800;
    const char* title = config.window_title;
    const char* icon_path = "window.ico";
    const int icon_w = 256;
    const int icon_h = 256;
    
    WNDCLASSA w32_class = {};
    w32_class.style = CS_HREDRAW | CS_VREDRAW;
    w32_class.lpfnWndProc = w32_window_callback;
    w32_class.hInstance = instance;
    w32_class.hCursor = LoadCursor(0, IDC_ARROW);
    w32_class.lpszClassName = "MainWindowClass";
    
    w32_class.hIcon = (HICON)LoadImageA(NULL, 
                                        icon_path,
                                        IMAGE_ICON, 
                                        icon_w, 
                                        icon_h,
                                        LR_LOADFROMFILE);
    
    if(!RegisterClassA(&w32_class)) {
      return 1;
    }
    
    RECT window_rect = {0};
    {
      // NOTE(Momo): Monitor dimensions
      HMONITOR monitor = MonitorFromWindow(0, MONITOR_DEFAULTTONEAREST);
      MONITORINFOEX monitor_info;
      monitor_info.cbSize = sizeof(monitor_info);
      GetMonitorInfo(monitor, &monitor_info); 
      
      LONG monitor_w = w32_rect_width(monitor_info.rcMonitor);
      LONG monitor_h = w32_rect_height(monitor_info.rcMonitor);
      
      window_rect.left = monitor_w/2 - window_w/2;
      window_rect.right = monitor_w/2 + window_w/2;
      window_rect.top = monitor_h/2 - window_h/2;
      window_rect.bottom = monitor_h/2 + window_h/2;
    }
    
    DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    
    AdjustWindowRectEx(&window_rect,
                       style,
                       FALSE,
                       0);    
    
    window = CreateWindowExA(0,
                             w32_class.lpszClassName,
                             title,
                             style,
                             window_rect.left,
                             window_rect.top,
                             w32_rect_width(window_rect),
                             w32_rect_height(window_rect),
                             0,
                             0,
                             instance,
                             0);
    
    if (!window) {
      return 1;
    }
    
    
  }
  w32_state.window = window;
  

  //  w32_toggle_fullscreen(window);
  
  
#if 0
  u32_t monitor_frame_rate = 60;
  {
    HDC dc = GetDC(window);
    int w32_frame_rate = GetDeviceCaps(dc, VREFRESH);
    ReleaseDC(window, dc);
    if (w32_frame_rate > 1) {
      monitor_frame_rate = (u32_t)w32_frame_rate;
    }
  }
#endif

  f32_t target_secs_per_frame = 1.f/(f32_t)config.target_frame_rate;
  w32_log("Target Frame Rate: %d Hz\n", config.target_frame_rate);
  
  

  //
  // Gfx
  // 
  arena_t* gfx_arena = &game.gfx_arena;
  if (!w32_allocate_memory_into_arena(gfx_arena, config.gfx_arena_size)) 
    return 1;

  if(!w32_gfx_load(
      &game.gfx,
      window, 
      gfx_arena,
      config.render_command_size, 
      config.texture_queue_size,
      config.max_textures,
      config.max_texture_payloads,
      config.max_sprites,
      config.max_triangles))
    return 1;
 
  // Init Audio
  arena_t* audio_arena = &game.audio_arena;
  if (config.audio_enabled) {
    if (!w32_allocate_memory_into_arena(audio_arena, config.audio_arena_size)) 
      return 1;

    if (!w32_audio_load(
          &game.audio, 
          48000, 32, 2, 1, 
          60, 
          audio_arena)) 
      return 1;
  }
  defer{ 
    if (config.audio_enabled) 
      w32_audio_unload(&game.audio); 
  };



  //
  // Init debug stuff
  //
  arena_t* debug_arena = &game.debug_arena;
  if (!w32_allocate_memory_into_arena(debug_arena, config.debug_arena_size)) return false;

  game_profiler_init(&game.profiler, w32_get_performance_counter_u64, debug_arena, config.max_profiler_entries, config.max_profiler_snapshots);

  game_inspector_init(&game.inspector, debug_arena, config.max_inspector_entries);


  //
  // Game setup
  //

  // Begin game loop
  b32_t is_sleep_granular = timeBeginPeriod(1) == TIMERR_NOERROR;
  
  // Send this to global state
  LARGE_INTEGER performance_frequency;
  QueryPerformanceFrequency(&performance_frequency);
  LARGE_INTEGER last_frame_count = w32_get_performance_counter();

  while (w32_state.is_running && game.is_running) 
  {
#if HOT_RELOADABLE
    // Hot reload game.dll functions
    game.is_dll_reloaded = w32_reload_code_if_outdated(&game_code);
    if (game.is_dll_reloaded) {
      game_profiler_reset(&game.profiler);
    }
#else 
    game_profiler_reset(&game.profiler);
#endif

    // Begin frame
    if (config.audio_enabled) w32_audio_begin_frame(&game.audio);
    v2u_t client_wh = w32_get_client_dims(window);


    f32_t game_aspect = w32_state.game_width / w32_state.game_height;
    RECT rr = w32_calc_render_region(client_wh.w,
                                     client_wh.h,
                                     game_aspect);

    w32_gfx_begin_frame(&game.gfx, client_wh, rr.left, rr.bottom, rr.right, rr.top);
       
    //Process messages and input
    w32_update_input(&game.input, window, target_secs_per_frame, rr);
    
    
    game_functions.update_and_render(&game);


    // End frame
    game_profiler_update_entries(&game.profiler);
    game_inspector_clear(&game.inspector);
    w32_gfx_end_frame(&game.gfx);
    
    

    if (config.audio_enabled) w32_audio_end_frame(&game.audio);
#if 0
    if (w32_state.is_cursor_locked) {
      SetCursorPos(
          w32_state.cursor_pt_to_lock_to.x,
          w32_state.cursor_pt_to_lock_to.y);
    }
#endif

    // Frame-rate control
    //
    // 1. Calculate how much time has passed since the last frame
    // 2. If the time elapsed is greater than the target time elapsed,
    //    sleep/spin-lock until then.    
    //
    // NOTE: We might want to think about VSYNC or getting VBLANK
    // value so that we can figure out how long we *should* sleep
    f32_t secs_elapsed_after_update = 
      w32_get_secs_elapsed(last_frame_count,
                           w32_get_performance_counter(),
                           performance_frequency);
    
    if(target_secs_per_frame > secs_elapsed_after_update) {
      if (is_sleep_granular) {
        DWORD ms_to_sleep 
          = (DWORD)(1000 * (target_secs_per_frame - secs_elapsed_after_update));
        
        // Return control to OS
        if (ms_to_sleep > 1) {
          Sleep(ms_to_sleep - 1);
        }
      }
      
      // Spin lock
      f32_t secs_elapsed_after_sleep = 
        w32_get_secs_elapsed(last_frame_count,
                             w32_get_performance_counter(),
                             performance_frequency);
      if (secs_elapsed_after_sleep > target_secs_per_frame) {
        // log oversleep?
        w32_log("[w32] Overslept! %f vs %f\n", secs_elapsed_after_sleep,
                target_secs_per_frame);
      }
      
      // Spin lock to simulate sleeping more
      while(target_secs_per_frame > secs_elapsed_after_sleep) {
        secs_elapsed_after_sleep = 
          w32_get_secs_elapsed(last_frame_count,
                               w32_get_performance_counter(),
                               performance_frequency);
        
      }
      
    }
    
    
        
    LARGE_INTEGER end_frame_count = w32_get_performance_counter();

    f32_t secs_this_frame =  
      w32_get_secs_elapsed(
          last_frame_count,
          end_frame_count,
          performance_frequency);
    
    // only do this when VSYNC is enabled
    //target_secs_per_frame = secs_this_frame;
#if 0 
    w32_log("target: %f vs %f \n", 
            target_secs_per_frame,
            secs_this_frame);
#endif
    //w32_gfx_swap_buffer(gfx);
    last_frame_count = end_frame_count;
    
    
    
  }
  
  
  
  
  return 0;  
  
  
}
