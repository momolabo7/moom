// 
// DESCRIPTION
//   This is my dear 2D eden engine on win32 platform.
//

#ifndef EDEN_USE_WASAPI
# define EDEN_USE_WASAPI 1
#endif

#ifndef EDEN_USE_OPENGL
# define EDEN_USE_OPENGL 1
#endif

#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif

#ifndef HOT_RELOAD
# define HOT_RELOAD 1
#endif

#include <windows.h>
#include <windowsx.h>
#include <timeapi.h>
#include <stdio.h>

#include "eden.h"

#ifdef EDEN_USE_WASAPI 
# include "w32_eden_audio_wasapi.h"
#endif // EDEN_USE_WASAPI

#if EDEN_USE_OPENGL
# include "w32_eden_gfx_opengl.h"
#endif // EDEN_USE_OPENGL

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "imm32.lib")

//
// MARK:(Work Queue)
//
struct w32_work_t {
  void* data;
  eden_task_callback_f* callback;
};

struct w32_work_queue_t {
  u32_t entry_cap;
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


//
// MARK:(State)
//
struct w32_state_t {
  b32_t is_running;

  // cursor locking system
  b32_t is_cursor_locked;
  POINT cursor_pt_to_lock_to;
  
  f32_t eden_width;
  f32_t eden_height;
  
  w32_work_queue_t work_queue;
  
  HWND window;

  arena_t arena;
};
// @todo: can we remove this global shit somehow?
static w32_state_t* w32_state;


static 
eden_debug_log_sig(w32_log_proc) 
{
  char buffer[256] = {0};
  va_list args;
  va_start(args, fmt);
  vsprintf(buffer, fmt, args);
  va_end(args);
  OutputDebugStringA(buffer);
}
#define w32_log(...) w32_log_proc(__VA_ARGS__)


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
	if(aspect_ratio <= 0.f || window_w == 0 || window_h == 0) 
    return {};
  
	RECT ret;
	
	f32_t optimal_window_w = (f32_t)window_h * aspect_ratio;
	f32_t optimal_window_h = (f32_t)window_w * 1.f/aspect_ratio;
	
	if (optimal_window_w > (f32_t)window_w) {
		// @note: width has priority - top and bottom bars
		ret.left = 0;
		ret.right = window_w;
		
		f32_t empty_height = (f32_t)window_h - optimal_window_h;
		
		ret.bottom = (u32_t)(empty_height * 0.5f);
		ret.top = ret.bottom + (u32_t)optimal_window_h;
	}
	else {
		// @note: height has priority - left and right bars
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

// @note: This function is accessed by multiple threads!
static b32_t
w32_do_next_work_entry(w32_work_queue_t* wq) {
  b32_t should_sleep = false;
  
  // @note: Generally, we want to do: 
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

// @note: This makes the main thread
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

// @note: This is not very thread safe. Other threads shouldn't call this.
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
w32_shutdown() {
  w32_state->is_running = false;
}

static void
w32_unload_code(w32_loaded_code_t* code) {
  if(code->dll) {
    FreeLibrary(code->dll);
    code->dll = 0;
  }
  code->is_valid = false;
  memory_zero_range(code->functions, code->function_count);
}

static void
w32_load_code(w32_loaded_code_t* code) {
  code->is_valid = false;
  //b32_t copy_success = false;
  for (u32_t attempt = 0; attempt < 100; ++attempt) {
    if(CopyFile(code->module_path, code->tmp_path, false)) {
      //copy_success = true;
      break;
    }
    Sleep(100);
  }
  code->dll = LoadLibraryA(code->tmp_path);
  if (code->dll) {
    code->is_valid = true;
    for (u32_t function_index = 0; 
         function_index < code->function_count; 
         ++function_index) 
    {
      void* function = (void*)GetProcAddress(code->dll, code->function_names[function_index]);
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

static eden_button_code_t
w32_vkeys_to_eden_button_code(u32_t code) {

  // A to Z
  if (code >= 0x41 && code <= 0x5A) {
    return eden_button_code_t(EDEN_BUTTON_CODE_A + code - 0x41);
  }
  
  // 0 to 9
  else if (code >= 0x30 && code <= 0x39) {
    return eden_button_code_t(EDEN_BUTTON_CODE_0 + code - 0x30);
  }

  // F1 to F12
  // @note: there are actually more F-keys??
  else if (code >= 0x70 && code <= 0x7B) {
    return eden_button_code_t(EDEN_BUTTON_CODE_F1 + code - 0x70);
  }
  else {
    switch(code) {
      case VK_SPACE: return EDEN_BUTTON_CODE_SPACE;
    }

  }
  return EDEN_BUTTON_CODE_UNKNOWN;
}

static void
w32_update_input(eden_input_t* input, HWND window, f32_t delta_time, RECT rr) 
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
        w32_state->is_running = false;
      } break;

      case WM_LBUTTONUP:
      case WM_LBUTTONDOWN: {
        b32_t is_key_down = msg.message == WM_LBUTTONDOWN;
        input->buttons[EDEN_BUTTON_CODE_LMB].now = is_key_down;
      } break;

      case WM_MBUTTONUP:
      case WM_MBUTTONDOWN: {
        b32_t is_key_down = msg.message == WM_MBUTTONDOWN;
        input->buttons[EDEN_BUTTON_CODE_MMB].now = is_key_down;
      } break;

      case WM_RBUTTONUP:
      case WM_RBUTTONDOWN: {
        b32_t is_key_down = msg.message == WM_RBUTTONDOWN;
        input->buttons[EDEN_BUTTON_CODE_RMB].now = is_key_down;
      } break;
      
      case WM_KEYUP:
      case WM_KEYDOWN:
      case WM_SYSKEYDOWN:
      case WM_SYSKEYUP:
      {
        u32_t code = (u32_t)msg.wParam;
        b32_t is_key_down = msg.message == WM_KEYDOWN;
        input->buttons[w32_vkeys_to_eden_button_code(code)].now = is_key_down;

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

  f32_t eden_to_render_w = w32_state->eden_width / region_width;
  f32_t eden_to_render_h = w32_state->eden_height / region_height;
  
  input->mouse_pos.x = render_mouse_pos_x * eden_to_render_w;
  input->mouse_pos.y = render_mouse_pos_y * eden_to_render_h;
  
  
  if (w32_state->is_cursor_locked) {
    SetCursorPos(
        w32_state->cursor_pt_to_lock_to.x,
        w32_state->cursor_pt_to_lock_to.y);
  }
    
}

static void
w32_set_eden_dims(f32_t width, f32_t height) {
  assert(width > 0.f && height > 0.f);

  // Ignore if there is no change
  if (width == w32_state->eden_width && height == w32_state->eden_height) return;

  w32_state->eden_width = width;
  w32_state->eden_height = height;

  // Get monitor info
  HMONITOR monitor = MonitorFromWindow(0, MONITOR_DEFAULTTONEAREST);
  MONITORINFOEX monitor_info;
  monitor_info.cbSize = sizeof(monitor_info);
  GetMonitorInfo(monitor, &monitor_info); 

  RECT client_dims;
  GetClientRect(w32_state->window, &client_dims); 

  RECT window_dims;
  GetWindowRect(w32_state->window, &window_dims);

  POINT diff;
  diff.x = (window_dims.right - window_dims.left) - client_dims.right;
  diff.y = (window_dims.bottom - window_dims.top) - client_dims.bottom;

  LONG monitor_w = w32_rect_width(monitor_info.rcMonitor);
  LONG monitor_h = w32_rect_height(monitor_info.rcMonitor);
 
  LONG left = monitor_w/2 - (u32_t)width/2;
  LONG top = monitor_h/2 - (u32_t)height/2;

  // Make it right at the center!
  MoveWindow(w32_state->window, left, top, (s32_t)width + diff.x, (s32_t)height + diff.y, TRUE);

}

static 
eden_show_cursor_sig(w32_show_cursor)
{
  while(ShowCursor(1) < 0);
}

static  
eden_hide_cursor_sig(w32_hide_cursor) {
  while(ShowCursor(0) >= 0);
}

static 
eden_lock_cursor_sig(w32_lock_cursor) {
  w32_state->is_cursor_locked = true;
  GetCursorPos(&w32_state->cursor_pt_to_lock_to);
}

static 
eden_unlock_cursor_sig(w32_unlock_cursor) {
  w32_state->is_cursor_locked = false;
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
      w32_state->is_running = false;
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
eden_add_task_sig(w32_add_task)
{
  w32_add_task_entry(&w32_state->work_queue, callback, data);
}

static 
eden_complete_all_tasks_sig(w32_complete_all_tasks) 
{
  w32_complete_all_tasks_entries(&w32_state->work_queue);
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

  //
  // Initialize w32 state
  //
  
  w32_state = arena_alloc_bootstrap(w32_state_t, arena, gigabytes(1));
  {
    w32_state->is_running = true;

    w32_state->eden_width = 1.f;
    w32_state->eden_height = 1.f;  

    if (!w32_init_work_queue(&w32_state->work_queue, 8)) {
      return 1;
    }
  }
  arena_t* platform_arena = &w32_state->arena;
  

  //
  // Load eden Functions
  //
  eden_functions_t eden_functions = {};
#if HOT_RELOAD 
  w32_loaded_code_t eden_code = {};
  eden_code.function_count = array_count(eden_function_names);
  eden_code.function_names = eden_function_names;
  eden_code.module_path = "eden.dll";
  eden_code.functions = (void**)&eden_functions;
  eden_code.tmp_path = "tmp_eden.dll";
  w32_load_code(&eden_code);
  if (!eden_code.is_valid) return 1;
  defer { w32_unload_code(&eden_code); };
#else  // HOT_RELOAD 
  eden_functions.get_config = eden_get_config;
  eden_functions.update_and_render = eden_update_and_render;
#endif // HOT_RELOAD
  
  eden_config_t config = eden_functions.get_config();

  eden_t* eden = arena_push(eden_t, &w32_state->arena);
  eden->is_running = true;
  eden->show_cursor = w32_show_cursor;
  eden->lock_cursor = w32_lock_cursor;
  eden->hide_cursor = w32_hide_cursor;
  eden->unlock_cursor = w32_unlock_cursor;
  eden->debug_log = w32_log_proc;
  eden->add_task = w32_add_task;
  eden->complete_all_tasks = w32_complete_all_tasks;
  eden->set_design_dimensions = w32_set_eden_dims;


  //
  // Create window in the middle of the screen
  //
  HWND window;
  {
    const int window_w = (int)config.window_initial_width;
    const int window_h = (int)config.window_initial_height;
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
      // Monitor dimensions
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
  w32_state->window = window;
  
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
  

  if(!w32_gfx_load(
      eden,
      window, 
      config.texture_queue_size,
      config.max_commands,
      config.max_textures,
      config.max_texture_payloads,
      config.max_elements))
    return 1;
 
  // Init Audio
  if (config.speaker_enabled) {

    if (!w32_speaker_load(
          &eden->speaker, 
          config.speaker_samples_per_second, 
          config.speaker_bitrate_type,
          config.speaker_channels, 
          1, 
          config.target_frame_rate, 
          config.speaker_max_sounds, 
          platform_arena)) 
    {
      return 1;
    }

  }
  defer{ if (config.speaker_enabled) w32_speaker_unload(&eden->speaker); };



  //
  // Init debug stuff
  //
  if (config.profiler_enabled) 
  {
    if (!eden_profiler_init(
          &eden->profiler, 
          platform_arena, 
          config.profiler_max_entries, 
          config.profiler_max_snapshots_per_entry))
    {
      return 1;
    }
  }

  if (config.inspector_enabled) 
  {
    if (!eden_inspector_init(
          &eden->inspector, 
          platform_arena, 
          config.inspector_max_entries))
    {
      return 1;
    }
  }


  //
  // Game setup
  //

  // Begin eden loop
  b32_t is_sleep_granular = timeBeginPeriod(1) == TIMERR_NOERROR;
  
  LARGE_INTEGER performance_frequency;
  QueryPerformanceFrequency(&performance_frequency);
  LARGE_INTEGER last_frame_count = w32_get_performance_counter();

  while (w32_state->is_running && eden->is_running) 
  {
#if HOT_RELOAD
    // Hot reload eden->dll functions
    eden->is_dll_reloaded = w32_reload_code_if_outdated(&eden_code);
    if (eden->is_dll_reloaded) {
      eden_profiler_reset(&eden->profiler);
    }
#else  // HOT_RELOAD
    eden_profiler_reset(&eden->profiler);
#endif // HOT_RELOAD

    // Begin frame
    if (config.speaker_enabled) w32_speaker_begin_frame(&eden->speaker);
    v2u_t client_wh = w32_get_client_dims(window);


    f32_t eden_aspect = w32_state->eden_width / w32_state->eden_height;
    RECT rr = w32_calc_render_region(client_wh.w,
                                     client_wh.h,
                                     eden_aspect);

    w32_gfx_begin_frame(&eden->gfx, client_wh, rr.left, rr.bottom, rr.right, rr.top);
       
    //Process messages and input
    eden_profile_begin(eden, input);
    w32_update_input(&eden->input, window, target_secs_per_frame, rr);
    eden_profile_end(eden, input);
    
    eden_functions.update_and_render(eden);


    // End frame
    if (config.speaker_enabled) 
      eden_speaker_update(eden);
    

    if (config.profiler_enabled)
      eden_profiler_update_entries(&eden->profiler);


    if (config.inspector_enabled) 
      eden_inspector_clear(&eden->inspector);

    w32_gfx_end_frame(&eden->gfx);
    
    

    if (config.speaker_enabled) w32_speaker_end_frame(&eden->speaker);

    // Frame-rate control
    //
    // 1. Calculate how much time has passed since the last frame
    // 2. If the time elapsed is greater than the target time elapsed,
    //    sleep/spin-lock until then.    
    //
    // @note: We might want to think about VSYNC or getting VBLANK
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

#if 0 
    f32_t secs_this_frame =  
      w32_get_secs_elapsed(
          last_frame_count,
          end_frame_count,
          performance_frequency);
    
    // only do this when VSYNC is enabled
    //target_secs_per_frame = secs_this_frame;
    w32_log("target: %f vs %f \n", 
            target_secs_per_frame,
            secs_this_frame);
#endif
    //w32_gfx_swap_buffer(gfx);
    last_frame_count = end_frame_count;
  }


  return 0;  
  
}
