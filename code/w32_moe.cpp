#include "w32_moe.h"


//////////////////////////////////////////////////////////////////
// Memory allocation
struct W32_Memory_Block {
  Platform_Memory_Block platform_block;
  W32_Memory_Block* prev;
  W32_Memory_Block* next;
};


//////////////////////////////////////////////////////////////////
// Worker/Producer  functionality
struct W32_Work {
  void* data;
  Platform_Task_Callback* callback;
};

struct W32_Work_Queue {
  W32_Work entries[256];
  U32 volatile next_entry_to_read;
  U32 volatile next_entry_to_write;
  
  U32 volatile completion_count;
  U32 volatile completion_goal;
  HANDLE semaphore; 
  
};


// NOTE(Momo): This function is accessed by multiple threads!
static B32
w32_do_next_work_entry(W32_Work_Queue* wq) {
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
      W32_Work work = wq->entries[old_next_entry_to_read];
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
w32_complete_all_tasks_entries(W32_Work_Queue* wq) {
  while(wq->completion_goal != wq->completion_count) {
    w32_do_next_work_entry(wq);
  }
  wq->completion_goal = 0;
  wq->completion_count = 0;
}



static DWORD WINAPI 
w32_worker_func(LPVOID ctx) {
  W32_Work_Queue* wq = (W32_Work_Queue*)ctx;
  
  while(true) {
    if (w32_do_next_work_entry(wq)){
      WaitForSingleObjectEx(wq->semaphore, INFINITE, FALSE);
    }
    
  }
}


static B32
w32_init_work_queue(W32_Work_Queue* wq, U32 thread_count) {
  wq->semaphore = CreateSemaphoreEx(0,
                                    0,                                
                                    thread_count,
                                    0, 0, SEMAPHORE_ALL_ACCESS);
  
  if (wq->semaphore == NULL) return false;
  
  for (U32 i = 0; i < thread_count; ++i) {
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
w32_add_task_entry(W32_Work_Queue* wq, void (*callback)(void* ctx), void *data) {
  U32 old_next_entry_to_write = wq->next_entry_to_write;
  U32 new_next_entry_to_write = (old_next_entry_to_write + 1) % array_count(wq->entries);
  assert(wq->next_entry_to_read != new_next_entry_to_write);  
  
  W32_Work* entry = wq->entries + old_next_entry_to_write;
  entry->callback = callback;
  entry->data = data;
  ++wq->completion_goal;
  
  _ReadWriteBarrier();
  
  wq->next_entry_to_write = new_next_entry_to_write; // this MUST not be reordered
  ReleaseSemaphore(wq->semaphore, 1, 0);
}

///////////////////////////////////////////////////////
//~File cabinet

struct W32_File {
  HANDLE handle;
  U32 cabinet_index;
};

struct W32_File_Cabinet {
  W32_File files[32]; 
  U32 free_files[32];
  U32 free_file_count;
};

static void
w32_init_file_cabinet(W32_File_Cabinet* c) {
  for(U32 i = 0; i < array_count(c->files); ++i) {
    c->files[i].cabinet_index = i;
    c->free_files[i] = i;
  }
  c->free_file_count = array_count(c->files);
}

static W32_File*
w32_get_next_free_file(W32_File_Cabinet* c) {
  if (c->free_file_count == 0) {
    return nullptr;
  }
  U32 free_file_index = c->free_files[c->free_file_count--];
  return c->files + free_file_index; 
  
}

static void
w32_return_file(W32_File_Cabinet* c, W32_File* f) {
  c->free_files[c->free_file_count++] = f->cabinet_index;
}


struct W32_State {
  B32 is_running;
  
  F32 moe_width;
  F32 moe_height;
  
  W32_Work_Queue work_queue;
  W32_File_Cabinet file_cabinet;
  
  HWND window;

  W32_Memory_Block memory_sentinel;
};
static W32_State w32_state;



static Platform_Memory_Block*
w32_allocate_memory(UMI size)
{
  UMI total_size = size + sizeof(W32_Memory_Block);
  UMI base_offset = sizeof(W32_Memory_Block);


  W32_Memory_Block* block = (W32_Memory_Block*)
    VirtualAllocEx(GetCurrentProcess(),
                   0, 
                   total_size,
                   MEM_RESERVE | MEM_COMMIT, 
                   PAGE_READWRITE);
  if (!block) return null;


  block->platform_block.data = (U8*)block + base_offset; 
  block->platform_block.size = size;

  W32_Memory_Block* sentinel = &w32_state.memory_sentinel;

  cll_append(sentinel, block);

#if 0
  block->next = sentinel;
  block->prev = sentinel->prev;
  block->prev->next = block;
  block->next->prev = block;
#endif


  return &block->platform_block;

}

static void
w32_free_memory(Platform_Memory_Block* platform_block) {
  if (platform_block) {
    W32_Memory_Block* block = (W32_Memory_Block*)platform_block;
    cll_remove(block);
    VirtualFree(block, 0, MEM_RELEASE);
  }
}

static void
w32_free_all_memory() {
  W32_Memory_Block* sentinel = &w32_state.memory_sentinel; 
  W32_Memory_Block* itr = sentinel->next;
  while(itr != sentinel) {
    VirtualFree(itr, 0, MEM_RELEASE);
    itr = itr->next;
  }
}



static void 
w32_shutdown() {
  w32_state.is_running = false;
}

#if 0
static void*
w32_allocate(UMI memory_size) {
  return (U8*)VirtualAllocEx(GetCurrentProcess(),
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

static B32
w32_allocate_memory_into_arena(Arena* a, UMI memory_size) {
  void* data = VirtualAllocEx(GetCurrentProcess(),
                              0, 
                              memory_size,
                              MEM_RESERVE | MEM_COMMIT, 
                              PAGE_READWRITE);
  if(data == null) return false;
  arn_init(a, data, memory_size);
  return true;
}




static void
w32_free_memory_from_arena(Arena* a) {
  VirtualFreeEx(GetCurrentProcess(), 
                a->memory,    
                0, 
                MEM_RELEASE); 
}


static B32
w32_open_file(Platform_File* file,
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
    
    W32_File* w32_file = w32_get_next_free_file(&w32_state.file_cabinet);
    assert(w32_file);
    w32_file->handle = handle;
    
    file->platform_data = w32_file;
    return true;
  }
}

static void
w32_close_file(Platform_File* file) {
  W32_File* w32_file = (W32_File*)file->platform_data;
  CloseHandle(w32_file->handle);
  
  w32_return_file(&w32_state.file_cabinet, w32_file);
  file->platform_data = nullptr;
}

static B32
w32_read_file(Platform_File* file, UMI size, UMI offset, void* dest) 
{ 
  W32_File* w32_file = (W32_File*)file->platform_data;
  
  // Reading the file
  OVERLAPPED overlapped = {};
  overlapped.Offset = (U32)((offset >> 0) & 0xFFFFFFFF);
  overlapped.OffsetHigh = (U32)((offset >> 32) & 0xFFFFFFFF);
  
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

static B32 
w32_write_file(Platform_File* file, UMI size, UMI offset, void* src)
{
  W32_File* w32_file = (W32_File*)file->platform_data;
  
  OVERLAPPED overlapped = {};
  overlapped.Offset = (U32)((offset >> 0) & 0xFFFFFFFF);
  overlapped.OffsetHigh = (U32)((offset >> 32) & 0xFFFFFFFF);
  
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
w32_add_task(Platform_Task_Callback callback, void* data) {
  w32_add_task_entry(&w32_state.work_queue, callback, data);
}

static void
w32_complete_all_tasks() {
  w32_complete_all_tasks_entries(&w32_state.work_queue);
}


static void
w32_process_input(HWND window, Platform* pf) 
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
        w32_state.is_running = false;
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
w32_set_moe_dims(F32 width, F32 height) {
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
 
  LONG left = monitor_w/2 - (U32)width/2;
  LONG top = monitor_h/2 - (U32)height/2;

  // Make it right at the center!
  MoveWindow(w32_state.window, left, top, (S32)width + diff.x, (S32)height + diff.y, TRUE);

  }

static void
w32_setup_platform_functions(Platform* pf)
{
  //pf->hot_reload = w32_hot_reload;
  //pf->shutdown = w32_shutdown;
  //
  //pf->set_render_region = w32_set_render_region;
  //pf->set_window_size = w32_set_window_size;
  //pf->get_window_size = w32_get_window_size;
  pf->set_moe_dims = w32_set_moe_dims;
  pf->open_file = w32_open_file;
  pf->read_file = w32_read_file;
  pf->write_file = w32_write_file;
  pf->close_file = w32_close_file;
  pf->add_task = w32_add_task;
  pf->complete_all_tasks = w32_complete_all_tasks;
  pf->debug_log = w32_log_proc;
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
  U32 monitor_refresh_rate = 60;
  {
    HDC dc = GetDC(window);
    int w32_refresh_rate = GetDeviceCaps(dc, VREFRESH);
    ReleaseDC(window, dc);
    if (w32_refresh_rate > 1) {
      monitor_refresh_rate = (U32)w32_refresh_rate;
    }
  }
  F32 target_secs_per_frame = 1.f/(F32)monitor_refresh_rate;
  w32_log("Monitor Refresh Rate: %d Hz\n", monitor_refresh_rate);
  w32_log("Target Secs per Frame: %.2f\n", target_secs_per_frame);
  
  
  //-Load Renderer functions
#if 0
  W32_Gfx_Functions gfx_functions = {};
  W32_Loaded_Code gfx_code = {};
  gfx_code.function_count = array_count(w32_gfx_function_names);
  gfx_code.function_names = w32_gfx_function_names;
  gfx_code.module_path = "gfx.dll";
  gfx_code.functions = (void**)&gfx_functions;
#if INTERNAL
  gfx_code.tmp_path = "tmp_gfx.dll";
#endif // INTERNAL
  w32_load_code(&gfx_code);
  if (!gfx_code.is_valid) return 1;
  defer { w32_unload_code(&gfx_code); };
#endif  

  //-Load Moe Functions
  Moe_Functions moe_functions = {};
  W32_Loaded_Code moe_code = {};
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
  
  
  //-Init gfx
  make(Arena, gfx_arena);
  if (!w32_allocate_memory_into_arena(gfx_arena, MB(256))) return false;
  defer { w32_free_memory_from_arena(gfx_arena); };
 
    
  Gfx* gfx = w32_gfx_load(window, MB(100), MB(100), gfx_arena);
  if (!gfx) { return 1; }
  defer { w32_gfx_unload(gfx); };
 
  // Init Audio
  make(Arena, audio_arena);
  if (!w32_allocate_memory_into_arena(audio_arena, MB(256))) return false;
  defer { w32_free_memory_from_arena(audio_arena); };

  Platform_Audio* audio = w32_audio_load(48000, 16, 2, 1, monitor_refresh_rate, audio_arena);
  if (!audio) return false;
  defer{ w32_audio_unload(audio); };


  // Init profiler
  prf_init(profiler, w32_get_performance_counter_u64);
  
  // Platform setup
  make(Platform, pf);
 
  // Moe memory set up
  make(Arena, moe_arena);
  if (!w32_allocate_memory_into_arena(moe_arena, MB(32))) return false;
  defer { w32_free_memory_from_arena(moe_arena); };
  
  w32_setup_platform_functions(pf);
  pf->gfx = gfx;
  pf->profiler = profiler;
  pf->moe_arena = moe_arena;
  pf->audio = audio;
  
  
  //- Begin moe loop
  B32 is_sleep_granular = timeBeginPeriod(1) == TIMERR_NOERROR;
  
  //- Send this to global state
  LARGE_INTEGER performance_frequency;
  QueryPerformanceFrequency(&performance_frequency);
  LARGE_INTEGER last_frame_count = w32_get_performance_counter();
 
  while (w32_state.is_running) {
  
#if 1
    // Hot reload moe.dll functions
    pf->reloaded = w32_reload_code_if_outdated(&moe_code);
    if (pf->reloaded) {
      prf_reset(profiler);
    }
#endif
   
    w32_profile_block(moe_loop);

    // Begin frame
    w32_audio_begin_frame(audio);
    V2U client_wh = w32_get_client_dims(window);


    F32 moe_aspect = w32_state.moe_width / w32_state.moe_height;
    RECT rr = w32_calc_render_region(client_wh.w,
                                     client_wh.h,
                                     moe_aspect);
    w32_gfx_begin_frame(gfx, client_wh, rr.left, rr.bottom, rr.right, rr.top);
       
    //-Process messages and input
    pf->seconds_since_last_frame = target_secs_per_frame;
    pf_update_input(pf);
    w32_process_input(window, pf); 
    
    //- Mouse input 
    {
      POINT cursor_pos = {0};
      GetCursorPos(&cursor_pos);
      ScreenToClient(window, &cursor_pos);
      
      
      F32 render_mouse_pos_x = (F32)(cursor_pos.x - rr.left);
      F32 render_mouse_pos_y = (F32)(cursor_pos.y - rr.bottom);

      F32 region_width = (F32)(rr.right - rr.left);
      F32 region_height = (F32)(rr.top - rr.bottom);

      F32 moe_to_render_w = w32_state.moe_width / region_width;
      F32 moe_to_render_h = w32_state.moe_height / region_height;
      
      pf->mouse_pos.x = render_mouse_pos_x * moe_to_render_w;
      pf->mouse_pos.y = render_mouse_pos_y * moe_to_render_h;
      
      
      // NOTE(Momo): Flip y
      // TODO(Momo): should this really be here?
      //pf->design_mouse_pos.y = lerp_f32(MOE_HEIGHT, 0.f, pf->design_mouse_pos.y/MOE_HEIGHT);	

    }
    
    
    //-Moe logic
    if(moe_code.is_valid) { 
      moe_functions.update_and_render(pf);
    }

    // End  frame
    prf_update_entries(profiler);
    w32_gfx_end_frame(gfx);
    w32_audio_end_frame(audio);

    // Frame-rate control
    //
    // 1. Calculate how much time has passed since the last frame
    // 2. If the time elapsed is greater than the target time elapsed,
    //    sleep/spin-lock until then.    
    //
    // NOTE: We might want to think about VSYNC or getting VBLANK
    // value so that we can figure out how long we *should* sleep
    F32 secs_elapsed_after_update = 
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
      F32 secs_elapsed_after_sleep = 
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
    F32 secs_this_frame =  w32_get_secs_elapsed(last_frame_count,
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