

#include "momo.h"
#include "win_gfx.h"

#include "game_pf.h"


#define NOMINMAX
#include <windows.h>
#undef near
#undef far

//~Helper Window functions
static inline LONG width_of(RECT r) { return r.right - r.left; }
static inline LONG height_of(RECT r) { return r.bottom - r.top; }


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

static F64
win_get_secs_elapsed(LARGE_INTEGER start,
                     LARGE_INTEGER end,
                     LARGE_INTEGER performance_frequency) 
{
  
  return (F64(end.QuadPart - start.QuadPart)) / performance_frequency.QuadPart;
}


//~Win_Work queue functionality
struct Win_Work {
  void* data;
  PF_Work_Callback_Fn* callback;
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
win_complete_all_work_entries(Win_Work_Queue* wq) {
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
win_add_work_entry(Win_Work_Queue* wq, void (*callback)(void* ctx), void *data) {
  U32 old_next_entry_to_write = wq->next_entry_to_write;
  U32 new_next_entry_to_write = (old_next_entry_to_write + 1) % array_count(wq->entries);
  assert(wq->next_entry_to_read != new_next_entry_to_write);  
  
  auto* entry = wq->entries + old_next_entry_to_write;
  entry->callback = callback;
  entry->data = data;
  ++wq->completion_goal;
  
  //_ReadWriteBarrier();
  
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



static PF_File
win_open_file(const char* filename, 
              PF_File_Access access,
              PF_File_Path path) 
{
  // Opening the file
  PF_File ret = {};
  
  DWORD access_flag = {};
  DWORD creation_disposition = {};
  switch (access) {
    case PF_FILE_ACCESS_READ: {
      access_flag = GENERIC_READ;
      creation_disposition = OPEN_EXISTING;
    } break;
    case PF_FILE_ACCESS_OVERWRITE: {
      access_flag = GENERIC_WRITE;
      creation_disposition = CREATE_ALWAYS;
    } break;
    /*
    case PF_File_Access_Modify: {
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
win_close_file(PF_File* file) {
  auto* win_file = (Win_File*)file->platform_data;
  CloseHandle(win_file->handle);
  
  win_free_memory(file->platform_data);
  file->platform_data = nullptr;
}

static void
win_read_file(PF_File* file, UMI size, UMI offset, void* dest) 
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
win_write_file(PF_File* file, UMI size, UMI offset, void* src)
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
win_add_work(PF_Work_Callback_Fn callback, void* data) {
  win_add_work_entry(&win_global_state.work_queue, callback, data);
}

static void
win_complete_all_work() {
  win_complete_all_work_entries(&win_global_state.work_queue);
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
  pf_api.add_work = win_add_work;
  pf_api.complete_all_work = win_complete_all_work;
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
    win_global_state.aspect_ratio_width = 1;
    win_global_state.aspect_ratio_height = 1;
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
  
  //-NOTE(Momo): Game Memory setup
  Game_Memory game = {};
  game.platform_api = win_create_platform_api();
  
  //-NOTE(Momo): Load Gfx functions
  Win_Gfx_API gfx_api;
  HMODULE gfx_dll;
  {
    gfx_dll =  LoadLibraryA("gfx.dll");
    if (gfx_dll) {
      gfx_api.init = (Win_Gfx_Init_Fn*)GetProcAddress(gfx_dll, "win_gfx_init");
      if(!gfx_api.init) return 1;
      
      gfx_api.free = (Win_Gfx_Free_Fn*)GetProcAddress(gfx_dll, "win_gfx_free");
      if(!gfx_api.free) return 1;
      
      gfx_api.render = (Win_Gfx_Render_Fn*)GetProcAddress(gfx_dll, "win_gfx_render");
      if(!gfx_api.render) return 1;
      
    }
    else {
      return 1;
    }
  }
  defer { FreeLibrary(gfx_dll); };
  
  
  //-NOTE(Momo): Init gfx
  Game_Gfx* gfx = gfx_api.init(window);
  if (!gfx) {
    return 1;
  }
  defer { gfx_api.free(gfx); };
  
  
  //- NOTE(Momo): Init input
  
  Game_Input input = {};
  
  //- Begin game loop
  
  B32 is_sleep_granular = timeBeginPeriod(1) == TIMERR_NOERROR;
  LARGE_INTEGER performance_frequency;
  QueryPerformanceFrequency(&performance_frequency);
  
  LARGE_INTEGER last_count = win_get_performance_counter();
  
  Game_API game_api = {}; 
  HMODULE game_dll = NULL; 
  
  while (win_global_state.is_running) {
    //-NOTE(Momo): Hot reload game.dll functions
    if (win_global_state.is_hot_reloading){
      static constexpr char* running_game_dll = "running_game.dll";
      static constexpr char* compiled_game_dll = "game.dll";
      
      // Release the current game dll
      if (game_dll) {
        FreeLibrary(game_dll);
        game_dll = NULL;
      }
      
      // Copy the compiled game dll
      if(!CopyFile(compiled_game_dll, running_game_dll, false)) {
        return 1;
      }
      
      game_dll = LoadLibraryA(running_game_dll);
      if (game_dll) {
        game_api.update = (Game_Update_Fn*)GetProcAddress(game_dll, "game_update");
        if(!game_api.update) {
          FreeLibrary(game_dll);
          return 1;
        }          
      }
      else {
        return 1;
      }
      win_global_state.is_hot_reloading = false;
    }
    
    //-Process messages and input
    // TODO(Momo): figure out target secs per frame
    const F64 target_dt = 1/60.0;
    input.seconds_since_last_frame = (F32)target_dt;
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
    
    //-Game logic here 
    
    game_api.update(&game, &input, gfx);
    
    //-Game render here
    // NOTE(Momo): Resize if needed. 
    V2U render_wh = win_get_client_dims(window);
    Rect2U render_region = win_calc_render_region(render_wh.w,
                                                  render_wh.h,
                                                  win_global_state.aspect_ratio_width,
                                                  win_global_state.aspect_ratio_height);
    
    
    
    gfx_api.render(gfx, render_wh, render_region);
    
    //-Frame-rate control
    // 1. Calculate how much time has passed since the last frame
    // 2. If the time elapsed is greater than the target time elapsed,
    //    sleep/spin-lock until then.    
    F64 secs_elapsed = 
      win_get_secs_elapsed(last_count,
                           win_get_performance_counter(),
                           performance_frequency);
    
    
    
    if(target_dt > secs_elapsed) {
      if (is_sleep_granular) {
        DWORD ms_to_sleep 
          = (DWORD)(1000 * (target_dt - secs_elapsed));
        
        // NOTE(Momo): Return control to OS
        if (ms_to_sleep > 1) {
          Sleep(ms_to_sleep - 1);
        }
        
        // NOTE(Momo): Spin lock
        while(target_dt > secs_elapsed) {
          secs_elapsed = 
            win_get_secs_elapsed(last_count,
                                 win_get_performance_counter(),
                                 performance_frequency);
        }
        
      }
    }
    else {
      // NOTE(Momo): At this point, we basically missed a frame :(
      // TODO(Momo): Do we need to do anything about missing a frame?
    }
    
    
    last_count = win_get_performance_counter();
    
    
    //-NOTE(Momo): swap buffers
    {
      HDC dc = GetDC(window);
      SwapBuffers(dc);
      ReleaseDC(window, dc);
    }
    
    
  }
  
  
  
  
  return 0;  
  
  
}