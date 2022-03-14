

#include "momo.h"
#include "win_renderer.h"
#include "game_platform.h"


#define NOMINMAX
#include <windows.h>
#undef near
#undef far

//~Helper Window functions
static inline LONG width_of(RECT r) { return r.right - r.left; }
static inline LONG height_of(RECT r) { return r.bottom - r.top; }

//#define WIN_LOG_ENABLED

#ifdef WIN_LOG_ENABLED
#include <stdio.h>
static void
win_log_proc(const char* fmt, ...) {
  char buffer[256] = {0};
  va_list args;
  va_start(args, fmt);
  vsprintf(buffer, fmt, args);
  va_end(args);
  OutputDebugStringA(buffer);
}
#define win_log(...) win_log_proc(__VA_ARGS__)
#else
#define win_log(...)
#endif // INTERNAL

#if 0
static void
win_toggle_fullscreen(HWND Window)
{
  // NOTE(casey): This follows Raymond Chen's prescription
  // for fullscreen toggling, see:
  // http://blogs.msdn.com/b/oldnewthing/archive/2010/04/12/9994016.aspx
  static WINDOWPLACEMENT GlobalWindowPosition = {sizeof(GlobalWindowPosition)};
  
  DWORD Style = GetWindowLong(Window, GWL_STYLE);
  if(Style & WS_OVERLAPPEDWINDOW)
  {
    MONITORINFO MonitorInfo = {sizeof(MonitorInfo)};
    if(GetWindowPlacement(Window, &GlobalWindowPosition) &&
       GetMonitorInfo(MonitorFromWindow(Window, MONITOR_DEFAULTTOPRIMARY), &MonitorInfo))
    {
      SetWindowLong(Window, GWL_STYLE, Style & ~WS_OVERLAPPEDWINDOW);
      SetWindowPos(Window, HWND_TOP,
                   MonitorInfo.rcMonitor.left, MonitorInfo.rcMonitor.top,
                   MonitorInfo.rcMonitor.right - MonitorInfo.rcMonitor.left,
                   MonitorInfo.rcMonitor.bottom - MonitorInfo.rcMonitor.top,
                   SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
  }
  else
  {
    SetWindowLong(Window, GWL_STYLE, Style | WS_OVERLAPPEDWINDOW);
    SetWindowPlacement(Window, &GlobalWindowPosition);
    SetWindowPos(Window, 0, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                 SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
  }
}
#endif


static inline V2U
win_get_window_dims(HWND window) {
	RECT rect;
	GetWindowRect(window, &rect);
  
  V2U ret;
  ret.w = U32(rect.right - rect.left);
  ret.h = U32(rect.bottom - rect.top);
  
  return ret;
	
}

static V2U
win_get_client_dims(HWND window) {
	RECT rect;
	GetClientRect(window, &rect);
  
  V2U ret;
  ret.w = U32(rect.right - rect.left);
  ret.h = U32(rect.bottom - rect.top);
  
  return ret;
	
}

static Rect2U 
win_calc_render_region(U32 window_w, 
                       U32 window_h, 
                       U32 render_w, 
                       U32 render_h) 
{
	assert(render_w > 0 && render_h > 0 && window_w > 0 && window_h > 0);
  
	Rect2U ret;
	
	F32 optimal_window_w = (F32)window_h * ((F32)render_w / (F32)render_h);
	F32 optimal_window_h = (F32)window_w * ((F32)render_h / (F32)render_w);
	
	if (optimal_window_w > (F32)window_w) {
		// NOTE(Momo): width has priority - top and bottom bars
		ret.min.x = 0;
		ret.max.x = window_w;
		
		F32 empty_height = (F32)window_h - optimal_window_h;
		
		ret.min.y = (U32)(empty_height * 0.5f);
		ret.max.y = ret.min.y + (U32)optimal_window_h;
	}
	else {
		// NOTE(Momo): height has priority - left and right bars
		ret.min.y = 0;
		ret.max.y = window_h;
		
		
		F32 empty_width = (F32)window_w - optimal_window_w;
		
		ret.min.x = (U32)(empty_width * 0.5f);
		ret.max.x = ret.min.x + (U32)optimal_window_w;
	}
	
	return ret;
}




static LARGE_INTEGER
win_get_performance_counter(void) {
  LARGE_INTEGER result;
  QueryPerformanceCounter(&result);
  return result;
}

static F32
win_get_secs_elapsed(LARGE_INTEGER start,
                     LARGE_INTEGER end,
                     LARGE_INTEGER performance_frequency) 
{
  
  return (F32(end.QuadPart - start.QuadPart)) / performance_frequency.QuadPart;
}


//~DLL loading
struct Win_Loaded_Code {
  // Need to fill these up
  U32 function_count;
  const char** function_names;
  const char* module_path;
  void** functions;
#if INTERNAL
  const char* tmp_path;
#endif  
  
  B32 is_valid;
  HMODULE dll; 
};

static void
win_unload_code(Win_Loaded_Code* code) {
  if(code->dll) {
    FreeLibrary(code->dll);
    code->dll = 0;
  }
  code->is_valid = false;
  zero_range(code->functions, code->function_count);
}

static void
win_load_code(Win_Loaded_Code* code) {
  code->is_valid = false;
  
#if INTERNAL
  B32 copy_success = false;
  for (U32 attempt = 0; attempt < 100; ++attempt) {
    if(CopyFile(code->module_path, code->tmp_path, false)) {
      copy_success = true;
      break;
    }
    Sleep(100);
  }
  code->dll = LoadLibraryA(code->tmp_path);
#else //INTERNAL
  code->dll = LoadLibraryA(code->module_path);
#endif //INTERNAL
  
  
  
  if (code->dll) {
    code->is_valid = true;
    for (U32 function_index = 0; 
         function_index < code->function_count; 
         ++function_index) 
    {
      void* function = GetProcAddress(code->dll, code->function_names[function_index]);
      if (!function) {
        code->is_valid = true;
        break;
      }
      code->functions[function_index] = function;
    }
    
  }
  
  if(!code->is_valid) {
    win_unload_code(code);
  }
}

static void
win_reload_code(Win_Loaded_Code* code) {
  win_unload_code(code); 
  for (U32 i = 0; i < 100; ++i ){
    win_load_code(code);
    if (code->is_valid) {
      break;
    }
    Sleep(100);
  }
}



//~Worker/Producer  functionality
struct Win_Work {
  void* data;
  Platform_Task_Callback* callback;
};

struct Win_Work_Queue {
  Win_Work entries[256];
  U32 volatile next_entry_to_read;
  U32 volatile next_entry_to_write;
  
  U32 volatile completion_count;
  U32 volatile completion_goal;
  HANDLE semaphore; 
  
};

// NOTE(Momo): This function is accessed by multiple threads!
static B32
win_do_next_work_entry(Win_Work_Queue* wq) {
  B32 should_sleep = false;
  
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
  
  U32 old_next_entry_to_read = wq->next_entry_to_read;
  U32 new_next_entry_to_read = (old_next_entry_to_read + 1) % array_count(wq->entries);
  
  if (old_next_entry_to_read != wq->next_entry_to_write) {
    DWORD initial_value = 
      InterlockedCompareExchange((LONG volatile*)&wq->next_entry_to_read,
                                 new_next_entry_to_read,
                                 old_next_entry_to_read);
    if (initial_value == old_next_entry_to_read) {
      Win_Work work = wq->entries[old_next_entry_to_read];
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
win_complete_all_tasks_entries(Win_Work_Queue* wq) {
  while(wq->completion_goal != wq->completion_count) {
    win_do_next_work_entry(wq);
  }
  wq->completion_goal = 0;
  wq->completion_count = 0;
}



static DWORD WINAPI 
win_worker_func(LPVOID ctx) {
  auto* wq = (Win_Work_Queue*)ctx;
  
  while(true) {
    if (win_do_next_work_entry(wq)){
      WaitForSingleObjectEx(wq->semaphore, INFINITE, FALSE);
    }
    
  }
}

static B32
win_init_work_queue(Win_Work_Queue* wq, U32 thread_count) {
  wq->semaphore = CreateSemaphoreEx(0,
                                    0,                                
                                    thread_count,
                                    0, 0, SEMAPHORE_ALL_ACCESS);
  
  if (wq->semaphore == NULL) return false;
  
  for (U32 i = 0; i < thread_count; ++i) {
    DWORD thread_id;
    HANDLE thread = CreateThread(NULL, 0, 
                                 win_worker_func, 
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
// TODO(Momo): Make it so that other threads can call this?
static void
win_add_task_entry(Win_Work_Queue* wq, void (*callback)(void* ctx), void *data) {
  U32 old_next_entry_to_write = wq->next_entry_to_write;
  U32 new_next_entry_to_write = (old_next_entry_to_write + 1) % array_count(wq->entries);
  assert(wq->next_entry_to_read != new_next_entry_to_write);  
  
  auto* entry = wq->entries + old_next_entry_to_write;
  entry->callback = callback;
  entry->data = data;
  ++wq->completion_goal;
  
  _ReadWriteBarrier();
  
  wq->next_entry_to_write = new_next_entry_to_write; // this MUST not be reordered
  ReleaseSemaphore(wq->semaphore, 1, 0);
}



//~Global variables
struct Win_State{
  B32 is_running;
  B32 is_hot_reloading;
  
  U32 aspect_ratio_width;
  U32 aspect_ratio_height;
  
  Win_Work_Queue work_queue;
};
static Win_State win_global_state;


struct Win_File {
  HANDLE handle;
};

//~ For Platform API
static void 
win_hot_reload() {
  win_global_state.is_hot_reloading = true;
}

static void 
win_shutdown() {
  win_global_state.is_running = false;
}
static void 
win_set_aspect_ratio(U32 width, U32 height) {
  win_global_state.aspect_ratio_width = width;
  win_global_state.aspect_ratio_height = height;
}


static void*
win_allocate_memory(UMI memory_size) {
  return (U8*)VirtualAllocEx(GetCurrentProcess(),
                             0, 
                             memory_size,
                             MEM_RESERVE | MEM_COMMIT, 
                             PAGE_READWRITE);
}


static void
win_free_memory(void* memory) {
  VirtualFreeEx(GetCurrentProcess(), 
                memory,    
                0, 
                MEM_RELEASE); 
}



static Platform_File
win_open_file(const char* filename, 
              Platform_File_Access access,
              Platform_File_Path path) 
{
  // Opening the file
  Platform_File ret = {};
  
  DWORD access_flag = {};
  DWORD creation_disposition = {};
  switch (access) {
    case PLATFORM_FILE_ACCESS_READ: {
      access_flag = GENERIC_READ;
      creation_disposition = OPEN_EXISTING;
    } break;
    case PLATFORM_FILE_ACCESS_OVERWRITE: {
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
    ret.platform_data = nullptr;
    ret.error = false;
    return ret;
  }
  else {
    // TODO(Momo): We should definitely use an arena for this
    auto* win_file = (Win_File*)win_allocate_memory(sizeof(Win_File));
    win_file->handle = handle;
    
    ret.platform_data = win_file;
    ret.error = false;
    return ret;
  }
}

static void
win_close_file(Platform_File* file) {
  auto* win_file = (Win_File*)file->platform_data;
  CloseHandle(win_file->handle);
  
  win_free_memory(file->platform_data);
  file->platform_data = nullptr;
}

static void
win_read_file(Platform_File* file, UMI size, UMI offset, void* dest) 
{ 
  if (!is_ok(file)) return;
  
  auto* win_file = (Win_File*)file->platform_data;
  
  // Reading the file
  OVERLAPPED overlapped = {};
  overlapped.Offset = (U32)((offset >> 0) & 0xFFFFFFFF);
  overlapped.OffsetHigh = (U32)((offset >> 32) & 0xFFFFFFFF);
  
  DWORD bytes_read;
  
  if(ReadFile(win_file->handle, dest, (DWORD)size, &bytes_read, &overlapped) &&
     (DWORD)size == bytes_read) 
  {
    // success;
  }
  else {
    file->error = true;
  }
}

static void 
win_write_file(Platform_File* file, UMI size, UMI offset, void* src)
{
  if (!is_ok(file)) return;
  
  auto* win_file = (Win_File*)file->platform_data;
  
  OVERLAPPED overlapped = {};
  overlapped.Offset = (U32)((offset >> 0) & 0xFFFFFFFF);
  overlapped.OffsetHigh = (U32)((offset >> 32) & 0xFFFFFFFF);
  
  DWORD bytes_wrote;
  if(WriteFile(win_file->handle, src, (DWORD)size, &bytes_wrote, &overlapped) &&
     (DWORD)size == bytes_wrote) 
  {
    // success
  }
  else {
    file->error = true;
  }
}

static void
win_add_task(Platform_Task_Callback callback, void* data) {
  win_add_task_entry(&win_global_state.work_queue, callback, data);
}

static void
win_complete_all_tasks() {
  win_complete_all_tasks_entries(&win_global_state.work_queue);
}


static Platform_API
win_create_platform_api()
{
  Platform_API pf_api;
  pf_api.hot_reload = win_hot_reload;
  pf_api.alloc = win_allocate_memory;
  pf_api.free = win_free_memory;
  pf_api.shutdown = win_shutdown;
  pf_api.open_file = win_open_file;
  pf_api.read_file = win_read_file;
  pf_api.write_file = win_write_file;
  pf_api.close_file = win_close_file;
  pf_api.set_aspect_ratio = win_set_aspect_ratio;
  pf_api.add_task = win_add_task;
  pf_api.complete_all_tasks = win_complete_all_tasks;
  return pf_api;
}

//~ Main functions
LRESULT CALLBACK
win_window_callback(HWND window, 
                    UINT message, 
                    WPARAM w_param,
                    LPARAM l_param) 
{
  LRESULT result = 0;
  switch(message) {
    case WM_CLOSE:  
    case WM_QUIT:
    case WM_DESTROY: {
      win_global_state.is_running = false;
    } break;
    default: {
      result = DefWindowProcA(window, message, w_param, l_param);
    };   
  }
  return result;
}



int CALLBACK
WinMain(HINSTANCE instance, 
        HINSTANCE prev_instance, 
        LPSTR command_line, 
        int show_code) 
{
  
  
  SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
  ImmDisableIME((DWORD)-1);
  
  //- Initialize window state
  {
    win_global_state.is_running = true;
    win_global_state.is_hot_reloading = true;
    win_global_state.aspect_ratio_width = 16;
    win_global_state.aspect_ratio_height = 9;
    
    if (!win_init_work_queue(&win_global_state.work_queue, 8)) {
      return 1;
    }
  }
  
  
  
  //- Create window in the middle of the screen
  HWND window;
  {
    const int win_w = 1600;
    const int win_h = 900;
    const char* title = "Momodevelop: TXT";
    const char* icon_path = "window.ico";
    const int icon_w = 256;
    const int icon_h = 256;
    
    
    WNDCLASSA win_class = {};
    win_class.style = CS_HREDRAW | CS_VREDRAW;
    win_class.lpfnWndProc = win_window_callback;
    win_class.hInstance = instance;
    win_class.hCursor = LoadCursor(0, IDC_ARROW);
    win_class.lpszClassName = "MainWindowClass";
    
    win_class.hIcon = (HICON)LoadImageA(NULL, 
                                        icon_path,
                                        IMAGE_ICON, 
                                        icon_w, 
                                        icon_h,
                                        LR_LOADFROMFILE);
    
    if(!RegisterClassA(&win_class)) {
      return 1;
    }
    
    RECT win_rect = {};
    {
      // NOTE(Momo): Monitor dimensions
      HMONITOR monitor = MonitorFromWindow(0, MONITOR_DEFAULTTONEAREST);
      MONITORINFOEX monitor_info;
      monitor_info.cbSize = sizeof(monitor_info);
      GetMonitorInfo(monitor, &monitor_info); 
      
      LONG monitor_w = width_of(monitor_info.rcMonitor);
      LONG monitor_h = height_of(monitor_info.rcMonitor);
      
      win_rect.left = monitor_w/2 - win_w/2;
      win_rect.right = monitor_w/2 + win_w/2;
      win_rect.top = monitor_h/2 - win_h/2;
      win_rect.bottom = monitor_h/2 + win_h/2;
    }
    
    DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    
    AdjustWindowRectEx(&win_rect,
                       style,
                       FALSE,
                       0);    
    
    window = CreateWindowExA(0,
                             win_class.lpszClassName,
                             title,
                             style,
                             win_rect.left,
                             win_rect.top,
                             width_of(win_rect),
                             height_of(win_rect),
                             0,
                             0,
                             instance,
                             0);
    
    if (!window) {
      return 1;
    }
    
    
  }
  //  win_toggle_fullscreen(window);
  
  //-Determine refresh rate
  // NOTE(Momo): For now we will adjust according to user's monitor...?
  // We might want to fuck care and just stick to 60 though.
  U32 monitor_refresh_rate = 60;
  {
    HDC dc = GetDC(window);
    int win_refresh_rate = GetDeviceCaps(dc, VREFRESH);
    ReleaseDC(window, dc);
    if (win_refresh_rate > 1) {
      monitor_refresh_rate = (U32)win_refresh_rate;
    }
  }
  F32 target_secs_per_frame = 1.f/(F32)monitor_refresh_rate;
  win_log("Monitor Refresh Rate: %d\n", monitor_refresh_rate);
  
  
  //-Load Renderer functions
  Win_Renderer_Functions renderer_functions = {};
  Win_Loaded_Code renderer_code = {};
  renderer_code.function_count = array_count(win_renderer_function_names);
  renderer_code.function_names = win_renderer_function_names;
  renderer_code.module_path = "renderer.dll";
  renderer_code.functions = (void**)&renderer_functions;
#if INTERNAL
  renderer_code.tmp_path = "tmp_renderer.dll";
#endif // INTERNAL
  win_load_code(&renderer_code);
  if (!renderer_code.is_valid) return 1;
  defer { win_unload_code(&renderer_code); };
  
  //-Load Game Functions
  Game_Functions game_functions = {};
  Win_Loaded_Code game_code = {};
  game_code.function_count = array_count(game_function_names);
  game_code.function_names = game_function_names;
  game_code.module_path = "game.dll";
  game_code.functions = (void**)&game_functions;
#if INTERNAL
  game_code.tmp_path = "tmp_game.dll";
#endif // INTERNAL
  win_load_code(&game_code);
  if (!game_code.is_valid) return 1;
  defer { win_unload_code(&game_code); };
  
  
  //-NOTE(Momo): Init renderer
  Renderer* renderer = renderer_functions.load(window, MB(128), MB(128));
  if (!renderer) { return 1; }
  defer { renderer_functions.unload(renderer); };
  
  
  //-Game Memory setup
  Game_Memory game = {};
  game.platform_api = win_create_platform_api();
  game.texture_queue = &renderer->texture_queue;
  
  //-Init input
  
  Game_Input input = {};
  
  //- Begin game loop
  B32 is_sleep_granular = timeBeginPeriod(1) == TIMERR_NOERROR;
  
  // TODO(Momo): send this to global state
  LARGE_INTEGER performance_frequency;
  QueryPerformanceFrequency(&performance_frequency);
  
  LARGE_INTEGER last_frame_count = win_get_performance_counter();
  
  while (win_global_state.is_running) {
    
    //- Begin render frame
    V2U render_wh = win_get_client_dims(window);
    Rect2U render_region = win_calc_render_region(render_wh.w,
                                                  render_wh.h,
                                                  win_global_state.aspect_ratio_width,
                                                  win_global_state.aspect_ratio_height);
    Game_Render_Commands* render_commands = nullptr;
    if (renderer_code.is_valid) {
      render_commands = renderer_functions.begin_frame(renderer, 
                                                       render_wh, 
                                                       render_region);
    }
    
    
    //-NOTE(Momo): Hot reload game.dll functions
    if (win_global_state.is_hot_reloading){
      win_reload_code(&game_code);
      win_global_state.is_hot_reloading = false;
    }
    
    //-Process messages and input
    input.seconds_since_last_frame = target_secs_per_frame;
    update(&input);
    {
      MSG msg = {};
      while(PeekMessage(&msg, window, 0, 0, PM_REMOVE)) {
        switch(msg.message) {
          case WM_QUIT:
          case WM_DESTROY:
          case WM_CLOSE: {
            win_global_state.is_running = false;
          } break;
          case WM_KEYUP:
          case WM_KEYDOWN:
          case WM_SYSKEYDOWN:
          case WM_SYSKEYUP:
          {
            U32 code = (U32)msg.wParam;
            B32 is_key_down = msg.message == WM_KEYDOWN;
            switch(code) {
              case 0x57: /* W  */ 
              {
                input.button_up.now = is_key_down;
              } break;
            }
            
          } break;
          
          default: {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
          }
        }
        
      }
    }
    
    
    
    //-Game logic
    
    if(game_code.is_valid && render_commands) { 
      game_functions.update(&game, &input, render_commands);
    }
    
    
#if 1   
    //-Frame-rate control
    // 1. Calculate how much time has passed since the last frame
    // 2. If the time elapsed is greater than the target time elapsed,
    //    sleep/spin-lock until then.    
    // NOTE(Momo): We might want to think about VSYNC or getting VBLANK
    // value so that we can figure out how long we *should* sleep
    F32 secs_elapsed_after_update = 
      win_get_secs_elapsed(last_frame_count,
                           win_get_performance_counter(),
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
      F32 secs_elapsed_after_sleep = 
        win_get_secs_elapsed(last_frame_count,
                             win_get_performance_counter(),
                             performance_frequency);
      if (secs_elapsed_after_sleep > target_secs_per_frame) {
        // log oversleep?
        win_log("[Win] Overslept! %f vs %f\n", secs_elapsed_after_sleep,
                target_secs_per_frame);
      }
      
      // Spin lock to simulate sleeping more
      while(target_secs_per_frame > secs_elapsed_after_sleep) {
        secs_elapsed_after_sleep = 
          win_get_secs_elapsed(last_frame_count,
                               win_get_performance_counter(),
                               performance_frequency);
        
      }
      
    }
#endif
    
    
    //- End render frame
    if (renderer_code.is_valid) {
      renderer_functions.end_frame(renderer, render_commands);
    }
    
    LARGE_INTEGER end_frame_count = win_get_performance_counter();
    F32 secs_this_frame =  win_get_secs_elapsed(last_frame_count,
                                                end_frame_count,
                                                performance_frequency);
    
    // only do this when VSYNC is enabled
    //target_secs_per_frame = secs_this_frame;
#if 0
    win_log("target: %f vs %f \n", 
            target_secs_per_frame,
            secs_this_frame);
#endif
    last_frame_count = end_frame_count;
    
    
  }
  
  
  
  
  return 0;  
  
  
}