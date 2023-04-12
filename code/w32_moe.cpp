#include "w32_moe.h"




#if 0
static void*
w32_allocate(usz_t memory_size) {
  return (u8_t*)VirtualAllocEx(GetCurrentProcess(),
                             0, 
                             memory_size,
                             MEM_RESERVE | MEM_COMMIT, 
                             PAGE_READWRITE);
}


static void
w32_free(void* memory) {
  VirtualFreeEx(GetCurrentProcess(), 
                memory,    
                0, 
                MEM_RELEASE); 
}

#endif

static b32_t
w32_allocate_memory_into_arena(arena_t* a, usz_t memory_size) {
  void* data = VirtualAllocEx(GetCurrentProcess(),
                              0, 
                              memory_size,
                              MEM_RESERVE | MEM_COMMIT, 
                              PAGE_READWRITE);
  if(data == nullptr) return false;
  arena_init(a, data, memory_size);
  return true;
}




static void
w32_free_memory_from_arena(arena_t* a) {
  VirtualFreeEx(GetCurrentProcess(), 
                a->memory,    
                0, 
                MEM_RELEASE); 
}


static b32_t
w32_open_file(pf_file_t* file,
              const char* filename, 
              pf_file_access_t access,
              pf_file_path_t path) 
{
  // Opening the file
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
    file->pf_data = nullptr;
    return false;
  }
  else {
    
    w32_file_t* w32_file = w32_get_next_free_file(&w32_state.file_cabinet);
    assert(w32_file);
    w32_file->handle = handle;
    
    file->pf_data = w32_file;
    return true;
  }
}

static void
w32_close_file(pf_file_t* file) {
  w32_file_t* w32_file = (w32_file_t*)file->pf_data;
  CloseHandle(w32_file->handle);
  
  w32_return_file(&w32_state.file_cabinet, w32_file);
  file->pf_data = nullptr;
}

static b32_t
w32_read_file(pf_file_t* file, usz_t size, usz_t offset, void* dest) 
{ 
  w32_file_t* w32_file = (w32_file_t*)file->pf_data;
  
  // Reading the file
  OVERLAPPED overlapped = {};
  overlapped.Offset = (u32_t)((offset >> 0) & 0xFFFFFFFF);
  overlapped.OffsetHigh = (u32_t)((offset >> 32) & 0xFFFFFFFF);
  
  DWORD bytes_read;
  
  if(ReadFile(w32_file->handle, dest, (DWORD)size, &bytes_read, &overlapped) &&
     (DWORD)size == bytes_read) 
  {
    return true;
  }
  else {
    return false;
  }
}

static b32_t 
w32_write_file(pf_file_t* file, usz_t size, usz_t offset, void* src)
{
  w32_file_t* w32_file = (w32_file_t*)file->pf_data;
  
  OVERLAPPED overlapped = {};
  overlapped.Offset = (u32_t)((offset >> 0) & 0xFFFFFFFF);
  overlapped.OffsetHigh = (u32_t)((offset >> 32) & 0xFFFFFFFF);
  
  DWORD bytes_wrote;
  if(WriteFile(w32_file->handle, src, (DWORD)size, &bytes_wrote, &overlapped) &&
     (DWORD)size == bytes_wrote) 
  {
    return true;
  }
  else {
    return false;
  }
}

static void
w32_add_task(pf_task_callback_f callback, void* data) {
  w32_add_task_entry(&w32_state.work_queue, callback, data);
}

static void
w32_complete_all_tasks() {
  w32_complete_all_tasks_entries(&w32_state.work_queue);
}



static input_button_code_t
w32_vkeys_to_input_button_code(u32_t code) {

  // A to Z
  if (code >= 0x41 && code <= 0x5A) {
    return input_button_code_t(INPUT_BUTTON_CODE_A + code - 0x41);
  }
  
  // 0 to 9
  else if (code >= 0x30 && code <= 0x39) {
    return input_button_code_t(INPUT_BUTTON_CODE_0 + code - 0x30);
  }

  // F1 to F12
  // NOTE(momo): there are actually more F-keys??
  else if (code >= 0x70 && code <= 0x7B) {
    return input_button_code_t(INPUT_BUTTON_CODE_F1 + code - 0x70);
  }
  else {
    switch(code) {
      case VK_SPACE: return INPUT_BUTTON_CODE_SPACE;
    }

  }
  
  return INPUT_BUTTON_CODE_UNKNOWN;
}

static void
w32_process_input(HWND window, input_t* input) 
{
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
        input->buttons[INPUT_BUTTON_CODE_LMB].now = is_key_down;
      } break;

      case WM_MBUTTONUP:
      case WM_MBUTTONDOWN: {
        b32_t is_key_down = msg.message == WM_MBUTTONDOWN;
        input->buttons[INPUT_BUTTON_CODE_MMB].now = is_key_down;
      } break;

      case WM_RBUTTONUP:
      case WM_RBUTTONDOWN: {
        b32_t is_key_down = msg.message == WM_RBUTTONDOWN;
        input->buttons[INPUT_BUTTON_CODE_RMB].now = is_key_down;
      } break;
      
      case WM_KEYUP:
      case WM_KEYDOWN:
      case WM_SYSKEYDOWN:
      case WM_SYSKEYUP:
      {
        u32_t code = (u32_t)msg.wParam;
        b32_t is_key_down = msg.message == WM_KEYDOWN;
        input->buttons[w32_vkeys_to_input_button_code(code)].now = is_key_down;

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
w32_set_moe_dims(f32_t width, f32_t height) {
  assert(width > 0.f && height > 0.f);

  // Ignore if there is no change
  if (width == w32_state.moe_width && height == w32_state.moe_height) return;

  w32_state.moe_width = width;
  w32_state.moe_height = height;

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

static void 
w32_show_cursor() {
  while(ShowCursor(1) < 0);
}

static void 
w32_hide_cursor() {
  while(ShowCursor(0) >= 0);
}

static void
w32_lock_cursor() {
  w32_state.is_cursor_locked = true;
  GetCursorPos(&w32_state.cursor_pt_to_lock_to);
}

static void
w32_unlock_cursor() {
  w32_state.is_cursor_locked = false;
}

static void
w32_set_platform_api(moe_t* moe)
{
  //moe->hot_reload = w32_hot_reload;
  //moe->shutdown = w32_shutdown;
  //
  //moe->set_render_region = w32_set_render_region;
  //moe->set_window_size = w32_set_window_size;
  //moe->get_window_size = w32_get_window_size;
  moe->pf.set_moe_dims = w32_set_moe_dims;
  moe->pf.open_file = w32_open_file;
  moe->pf.read_file = w32_read_file;
  moe->pf.write_file = w32_write_file;
  moe->pf.close_file = w32_close_file;
  moe->pf.add_task = w32_add_task;
  moe->pf.complete_all_tasks = w32_complete_all_tasks;
  moe->pf.debug_log = w32_log_proc;
  moe->pf.allocate_memory = w32_allocate_memory;
  moe->pf.free_memory = w32_free_memory;
  moe->pf.show_cursor = w32_show_cursor;
  moe->pf.hide_cursor = w32_hide_cursor;
  moe->pf.lock_cursor = w32_lock_cursor;
  moe->pf.unlock_cursor = w32_unlock_cursor;
}

//~ Main functions
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

static void 
w32_update_input(input_t* input) {
  for (u32_t i = 0; i < array_count(input->buttons); ++i) {
    input->buttons[i].before = input->buttons[i].now;
  }
  input->char_count = 0;
  input->mouse_scroll_delta = 0;
}

int CALLBACK
WinMain(HINSTANCE instance, 
        HINSTANCE prev_instance, 
        LPSTR command_line, 
        int show_code) 
{

  SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
  ImmDisableIME((DWORD)-1);
  
  //- Initialize w32 state
  {
    w32_state.is_running = true;

    w32_state.moe_width = 1.f;
    w32_state.moe_height = 1.f;  

    // initialize the circular linked list
    w32_state.memory_sentinel.next = &w32_state.memory_sentinel;    
    w32_state.memory_sentinel.prev = &w32_state.memory_sentinel;    

    if (!w32_init_work_queue(&w32_state.work_queue, 8)) {
      return 1;
    }
    w32_init_file_cabinet(&w32_state.file_cabinet);
  }
  defer { w32_free_all_memory(); };
  
  
  
  //- Create window in the middle of the screen
  HWND window;
  {
    const int window_w = (int)800;
    const int window_h = (int)800;
    const char* title = "Momodevelop: TXT";
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
  
  //-Determine refresh rate
  // NOTE(Momo): For now we will adjust according to user's monitor...?
  // We might want to fuck care and just stick to 60 though.
  u32_t monitor_refresh_rate = 60;
  {
    HDC dc = GetDC(window);
    int w32_refresh_rate = GetDeviceCaps(dc, VREFRESH);
    ReleaseDC(window, dc);
    if (w32_refresh_rate > 1) {
      monitor_refresh_rate = (u32_t)w32_refresh_rate;
    }
  }
  f32_t target_secs_per_frame = 1.f/(f32_t)monitor_refresh_rate;
  w32_log("Monitor Refresh Rate: %d Hz\n", monitor_refresh_rate);
  w32_log("Target Secs per Frame: %.2f\n", target_secs_per_frame);
  
  

  // Load moe Functions
  moe_functions_t moe_functions = {};
  w32_loaded_code_t moe_code = {};
  moe_code.function_count = array_count(moe_function_names);
  moe_code.function_names = moe_function_names;
  moe_code.module_path = "moe.dll";
  moe_code.functions = (void**)&moe_functions;
#if INTERNAL
  moe_code.tmp_path = "tmp_moe.dll";
#endif // INTERNAL
  w32_load_code(&moe_code);
  if (!moe_code.is_valid) return 1;
  defer { w32_unload_code(&moe_code); };
  
  
  //
  // Gfx
  // 

  gfx_t* gfx = w32_gfx_load(window, megabytes(100), megabytes(100));
  if (!gfx) { return 1; }
  defer { w32_gfx_unload(gfx); };
 
  // Init Audio
  make(arena_t, audio_arena);
  if (!w32_allocate_memory_into_arena(audio_arena, megabytes(256))) return false;
  defer { w32_free_memory_from_arena(audio_arena); };

  pf_audio_t* audio = w32_audio_load(48000, 16, 2, 1, monitor_refresh_rate, audio_arena);
  if (!audio) return false;
  defer{ w32_audio_unload(audio); };


  //
  // Init profiler
  //
  profiler_init(profiler, w32_get_performance_counter_u64);

  //
  // Init input
  // 
  input_t input = {};

  //
  // MOE setup
  //
  make(moe_t, moe);
  w32_set_platform_api(moe);
  moe->gfx = gfx;
  moe->profiler = profiler;
  moe->audio = audio;
  moe->input = &input;
  
  
  //- Begin moe loop
  b32_t is_sleep_granular = timeBeginPeriod(1) == TIMERR_NOERROR;
  
  //- Send this to global state
  LARGE_INTEGER performance_frequency;
  QueryPerformanceFrequency(&performance_frequency);
  LARGE_INTEGER last_frame_count = w32_get_performance_counter();

  b32_t game_is_running = true;
  while (w32_state.is_running && game_is_running) 
  {
#if 1
    // Hot reload moe.dll functions
    moe->reloaded = w32_reload_code_if_outdated(&moe_code);
    if (moe->reloaded) {
      profiler_reset(profiler);
    }
#endif
   
    w32_profile_block(moe_loop);

    // Begin frame
    w32_audio_begin_frame(audio);
    v2u_t client_wh = w32_get_client_dims(window);


    f32_t moe_aspect = w32_state.moe_width / w32_state.moe_height;
    RECT rr = w32_calc_render_region(client_wh.w,
                                     client_wh.h,
                                     moe_aspect);
    w32_gfx_begin_frame(gfx, client_wh, rr.left, rr.bottom, rr.right, rr.top);
       
    //-Process messages and input
    moe->delta_time = target_secs_per_frame;
    w32_update_input(&input);
    w32_process_input(window, &input); 
    
    //- Mouse input 
    {
      POINT cursor_pos = {0};
      GetCursorPos(&cursor_pos);
      ScreenToClient(window, &cursor_pos);
      
      
      f32_t render_mouse_pos_x = (f32_t)(cursor_pos.x - rr.left);
      f32_t render_mouse_pos_y = (f32_t)(cursor_pos.y - rr.bottom);

      f32_t region_width = (f32_t)(rr.right - rr.left);
      f32_t region_height = (f32_t)(rr.top - rr.bottom);

      f32_t moe_to_render_w = w32_state.moe_width / region_width;
      f32_t moe_to_render_h = w32_state.moe_height / region_height;
      
      input.mouse_pos.x = render_mouse_pos_x * moe_to_render_w;
      input.mouse_pos.y = render_mouse_pos_y * moe_to_render_h;
      
      
      // NOTE(Momo): Flip y
      //moe->design_mouse_pos.y = f32_lerp(MOE_HEIGHT, 0.f, moe->design_mouse_pos.y/MOE_HEIGHT);	
      if (w32_state.is_cursor_locked) {
        SetCursorPos(
            w32_state.cursor_pt_to_lock_to.x,
            w32_state.cursor_pt_to_lock_to.y);
      }
    }
    
    
    //-moe_t logic
    if(moe_code.is_valid) { 
      game_is_running = moe_functions.update_and_render(moe);
    }

    // End  frame
    profiler_update_entries(profiler);
    w32_gfx_end_frame(gfx);
    w32_audio_end_frame(audio);
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
    f32_t secs_this_frame =  w32_get_secs_elapsed(last_frame_count,
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
