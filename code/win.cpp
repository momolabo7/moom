#include "win.h"

Profiler _g_profiler = {0};
Profiler* g_profiler = &_g_profiler; 

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

//~File cabinet

struct Win_File {
  HANDLE handle;
  U32 cabinet_index;
};

struct Win_File_Cabinet {
  Win_File files[32]; 
  U32 free_files[32];
  U32 free_file_count;
};

static void
win_init_file_cabinet(Win_File_Cabinet* c) {
  for(U32 i = 0; i < array_count(c->files); ++i) {
    c->files[i].cabinet_index = i;
    c->free_files[i] = i;
  }
  c->free_file_count = array_count(c->files);
}

static Win_File*
win_get_next_free_file(Win_File_Cabinet* c) {
  if (c->free_file_count == 0) {
    return nullptr;
  }
  U32 free_file_index = c->free_files[c->free_file_count--];
  return c->files + free_file_index; 
  
}

static void
win_return_file(Win_File_Cabinet* c, Win_File* f) {
  c->free_files[c->free_file_count++] = f->cabinet_index;
}

//~Global variables
struct Win_State{
  B32 is_running;
  
  U32 aspect_ratio_width;
  U32 aspect_ratio_height;
  
  Win_Work_Queue work_queue;
  Win_File_Cabinet file_cabinet;
  
};
static Win_State g_win_state;


//~ For Platform API

static void 
win_shutdown() {
  g_win_state.is_running = false;
}

#if 0
static void*
win_allocate(UMI memory_size) {
  return (U8*)VirtualAllocEx(GetCurrentProcess(),
                             0, 
                             memory_size,
                             MEM_RESERVE | MEM_COMMIT, 
                             PAGE_READWRITE);
}


static void
win_free(void* memory) {
  VirtualFreeEx(GetCurrentProcess(), 
                memory,    
                0, 
                MEM_RELEASE); 
}

#endif

static B32
win_allocate_memory_into_arena(Bump_Allocator* a, UMI memory_size) {
  void* data = VirtualAllocEx(GetCurrentProcess(),
                              0, 
                              memory_size,
                              MEM_RESERVE | MEM_COMMIT, 
                              PAGE_READWRITE);
  if(data == nullptr) return false;
  ba_init(a, data, memory_size);
  return true;
}




static void
win_free_memory_from_arena(Bump_Allocator* a) {
  VirtualFreeEx(GetCurrentProcess(), 
                a->memory,    
                0, 
                MEM_RELEASE); 
}


static B32
win_open_file(Platform_File* file,
              const char* filename, 
              Platform_File_Access access,
              Platform_File_Path path) 
{
  // Opening the file
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
    file->platform_data = nullptr;
    return false;
  }
  else {
    
    Win_File* win_file = win_get_next_free_file(&g_win_state.file_cabinet);
    assert(win_file);
    win_file->handle = handle;
    
    file->platform_data = win_file;
    return true;
  }
}

static void
win_close_file(Platform_File* file) {
  auto* win_file = (Win_File*)file->platform_data;
  CloseHandle(win_file->handle);
  
  win_return_file(&g_win_state.file_cabinet, win_file);
  file->platform_data = nullptr;
}

static B32
win_read_file(Platform_File* file, UMI size, UMI offset, void* dest) 
{ 
  auto* win_file = (Win_File*)file->platform_data;
  
  // Reading the file
  OVERLAPPED overlapped = {};
  overlapped.Offset = (U32)((offset >> 0) & 0xFFFFFFFF);
  overlapped.OffsetHigh = (U32)((offset >> 32) & 0xFFFFFFFF);
  
  DWORD bytes_read;
  
  if(ReadFile(win_file->handle, dest, (DWORD)size, &bytes_read, &overlapped) &&
     (DWORD)size == bytes_read) 
  {
    return true;
  }
  else {
    return false;
  }
}

static B32 
win_write_file(Platform_File* file, UMI size, UMI offset, void* src)
{
  auto* win_file = (Win_File*)file->platform_data;
  
  OVERLAPPED overlapped = {};
  overlapped.Offset = (U32)((offset >> 0) & 0xFFFFFFFF);
  overlapped.OffsetHigh = (U32)((offset >> 32) & 0xFFFFFFFF);
  
  DWORD bytes_wrote;
  if(WriteFile(win_file->handle, src, (DWORD)size, &bytes_wrote, &overlapped) &&
     (DWORD)size == bytes_wrote) 
  {
    return true;
  }
  else {
    return false;
  }
}

static void
win_add_task(Platform_Task_Callback callback, void* data) {
  win_add_task_entry(&g_win_state.work_queue, callback, data);
}

static void
win_complete_all_tasks() {
  win_complete_all_tasks_entries(&g_win_state.work_queue);
}


static void
win_process_input(HWND window, Platform* pf) 
{
  MSG msg = {};
  while(PeekMessage(&msg, window, 0, 0, PM_REMOVE)) {
    switch(msg.message) {
      case WM_CHAR: {
        assert(pf->char_count < array_count(pf->chars));
        pf->chars[pf->char_count++] = (U8)msg.wParam;
      } break;
      case WM_QUIT:
      case WM_DESTROY:
      case WM_CLOSE: {
        g_win_state.is_running = false;
      } break;
      case WM_LBUTTONUP:
      case WM_LBUTTONDOWN: {
        U32 code = (U32)msg.wParam;
        B32 is_key_down = msg.message == WM_LBUTTONDOWN;
        pf->button_editor0.now = is_key_down;
      } break;
      case WM_RBUTTONUP:
      case WM_RBUTTONDOWN: {
        U32 code = (U32)msg.wParam;
        B32 is_key_down = msg.message == WM_RBUTTONDOWN;
        pf->button_editor1.now = is_key_down;
      } break;
      
      case WM_KEYUP:
      case WM_KEYDOWN:
      case WM_SYSKEYDOWN:
      case WM_SYSKEYUP:
      {
        U32 code = (U32)msg.wParam;
        B32 is_key_down = msg.message == WM_KEYDOWN;
        switch(code) {
          case 0x51: /* Q */ {
            pf->button_rotate_left.now = is_key_down;
          } break;
          case 0x45: /* E */ {
            pf->button_rotate_right.now = is_key_down;
          } break;
          //case 0x46: /* F */ {
          case 0x20: /* space */ { 
            pf->button_use.now = is_key_down;
          } break;
          case 0x57: /* W */ {
            pf->button_up.now = is_key_down;
          } break;
          case 0x41: /* A */ {
            pf->button_left.now = is_key_down;
          } break;
          case 0x53: /* S */ {
            pf->button_down.now = is_key_down;
          } break;
          case 0x44: /* D */ {
            pf->button_right.now = is_key_down;
          } break;
          case 0x70: /* F1 */{
            pf->button_console.now = is_key_down;
          } break;
          case 0x71: /* F2 */{
            pf->button_editor_on.now = is_key_down;
          } break;
          case 0xDB: /* [ */{
            pf->button_editor2.now = is_key_down;
          } break;
          case 0xDD: /* ] */{
            pf->button_editor3.now = is_key_down;
          } break;
        }
        TranslateMessage(&msg);
      } break;
      
      default: {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    }
    
  }
  
}

static void
win_setup_platform_functions(Platform* pf)
{
  //pf->hot_reload = win_hot_reload;
  //pf->shutdown = win_shutdown;
  pf->open_file = win_open_file;
  pf->read_file = win_read_file;
  pf->write_file = win_write_file;
  pf->close_file = win_close_file;
  pf->add_task = win_add_task;
  pf->complete_all_tasks = win_complete_all_tasks;
  pf->debug_log = win_log_proc;
  pf->get_performance_counter = win_get_performance_counter_u64;
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
      g_win_state.is_running = false;
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
    g_win_state.is_running = true;
    g_win_state.aspect_ratio_width = 16;
    g_win_state.aspect_ratio_height = 9;
    
    if (!win_init_work_queue(&g_win_state.work_queue, 8)) {
      return 1;
    }
    win_init_file_cabinet(&g_win_state.file_cabinet);
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
  Win_Gfx_Functions renderer_functions = {};
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
  
  
  //-Init renderer
  declare_and_pointerize(Bump_Allocator, renderer_arena);
  if (!win_allocate_memory_into_arena(renderer_arena, MB(256))) return false;
  defer { win_free_memory_from_arena(renderer_arena); };
  
  Gfx* renderer = 
    renderer_functions.load(window, 
                            MB(100),
                            MB(100), 
                            renderer_arena);
  if (!renderer) { return 1; }
  defer { renderer_functions.unload(renderer); };
 
  // Init Audio
  declare_and_pointerize(Bump_Allocator, audio_arena);
  if (!win_allocate_memory_into_arena(audio_arena, MB(256))) return false;
  defer { win_free_memory_from_arena(audio_arena); };

  Platform_Audio* audio = win_audio_load(48000, 16, 2, 1, monitor_refresh_rate, audio_arena);
  if (!audio) return false;
  defer{ win_audio_unload(audio); };


  // Init profiler
  prf_init(g_profiler, win_get_performance_counter_u64);
  
  // Platform setup
  declare_and_pointerize(Platform, pf);
  
  declare_and_pointerize(Bump_Allocator, game_arena);
  if (!win_allocate_memory_into_arena(game_arena, MB(32))) return false;
  defer { win_free_memory_from_arena(game_arena); };
  
  win_setup_platform_functions(pf);
  pf->gfx = renderer;
  pf->profiler = g_profiler;
  pf->game_arena = game_arena;
  pf->audio = audio;
  
  
  //- Begin game loop
  B32 is_sleep_granular = timeBeginPeriod(1) == TIMERR_NOERROR;
  
  //- Send this to global state
  LARGE_INTEGER performance_frequency;
  QueryPerformanceFrequency(&performance_frequency);
  LARGE_INTEGER last_frame_count = win_get_performance_counter();
  
  while (g_win_state.is_running) {
    win_profile_block("game loop");
    
    // Begin frame
    win_audio_begin_frame(audio);
    V2U render_wh = win_get_client_dims(window);
    Rect2U render_region = win_calc_render_region(render_wh.w,
                                                  render_wh.h,
                                                  game_aspect_ratio);
    if (renderer_code.is_valid) {
      renderer_functions.begin_frame(renderer, 
                                     render_wh, 
                                     render_region);
    }
    
#if INTERNAL
    //-Hot reload game.dll functions
    pf->reloaded = win_reload_code_if_outdated(&game_code);
    if (pf->reloaded) {
      prf_reset(g_profiler);
    }
#endif
    
    //-Process messages and input
    pf->seconds_since_last_frame = target_secs_per_frame;
    pf_update_input(pf);
    win_process_input(window, pf); 
    
    //- Mouse input 
    {
      POINT cursor_pos = {};
      GetCursorPos(&cursor_pos);
      ScreenToClient(window, &cursor_pos);
      
      pf->screen_mouse_pos.x = cursor_pos.x;
      pf->screen_mouse_pos.y = cursor_pos.y;
      
      pf->render_mouse_pos = pf->screen_mouse_pos - render_region.min;
      
      F32 design_to_render_w = game_width / width_of(render_region);
      F32 design_to_render_h = game_height / height_of(render_region);
      
      pf->design_mouse_pos.x = F32(pf->render_mouse_pos.x) * design_to_render_w;
      pf->design_mouse_pos.y = F32(pf->render_mouse_pos.y) * design_to_render_h;
      
      
      // NOTE(Momo): Flip y
      // TODO(Momo): should this really be here?
      // Maybe we should really make y-axis downwards...
      // since this is a 2D engine.
      pf->design_mouse_pos.y = lerp(game_height, 
                                       0.f, 
                                       pf->design_mouse_pos.y/game_height);	
    }
    
    
    //-Game logic
    if(game_code.is_valid) { 
      game_functions.update_and_render(pf);
    }

    // End  frame
    prf_update_entries(g_profiler);
    if (renderer_code.is_valid) {
      renderer_functions.end_frame(renderer);
    }
    win_audio_end_frame(audio);

    
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
