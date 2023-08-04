#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif

#ifndef HOT_RELOADABLE
# define HOT_RELOADABLE 1
#endif

#define NOMINMAX
#define CINTERFACE
#define COBJMACROS
#define INITGUID
#define COBJMACROS
#define CONST_VTABLE
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
#include "w32_game.h"

// Graphics
#include "w32_game_gfx.h"
#include "game_gfx_opengl.h"
#include "w32_game_gfx_opengl.h"

// Audio
#include "w32_game_audio.h"
#include "w32_game_audio_wasapi.h"

make(profiler_t, profiler);
make(inspector_t, inspector);

#include <stdio.h>

static 
app_debug_log_sig(w32_log_proc) 
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





static app_button_code_t
w32_vkeys_to_app_button_code(u32_t code) {

  // A to Z
  if (code >= 0x41 && code <= 0x5A) {
    return app_button_code_t(APP_BUTTON_CODE_A + code - 0x41);
  }
  
  // 0 to 9
  else if (code >= 0x30 && code <= 0x39) {
    return app_button_code_t(APP_BUTTON_CODE_0 + code - 0x30);
  }

  // F1 to F12
  // NOTE(momo): there are actually more F-keys??
  else if (code >= 0x70 && code <= 0x7B) {
    return app_button_code_t(APP_BUTTON_CODE_F1 + code - 0x70);
  }
  else {
    switch(code) {
      case VK_SPACE: return APP_BUTTON_CODE_SPACE;
    }

  }
  return APP_BUTTON_CODE_UNKNOWN;
}

// TODO: change 'rr' to 'render_region'
static void
w32_update_input(app_input_t* input, HWND window, f32_t delta_time, RECT rr) 
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
        input->buttons[APP_BUTTON_CODE_LMB].now = is_key_down;
      } break;

      case WM_MBUTTONUP:
      case WM_MBUTTONDOWN: {
        b32_t is_key_down = msg.message == WM_MBUTTONDOWN;
        input->buttons[APP_BUTTON_CODE_MMB].now = is_key_down;
      } break;

      case WM_RBUTTONUP:
      case WM_RBUTTONDOWN: {
        b32_t is_key_down = msg.message == WM_RBUTTONDOWN;
        input->buttons[APP_BUTTON_CODE_RMB].now = is_key_down;
      } break;
      
      case WM_KEYUP:
      case WM_KEYDOWN:
      case WM_SYSKEYDOWN:
      case WM_SYSKEYUP:
      {
        u32_t code = (u32_t)msg.wParam;
        b32_t is_key_down = msg.message == WM_KEYDOWN;
        input->buttons[w32_vkeys_to_app_button_code(code)].now = is_key_down;

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
app_show_cursor_sig(w32_show_cursor)
{
  while(ShowCursor(1) < 0);
}

static  
app_hide_cursor_sig(w32_hide_cursor) {
  while(ShowCursor(0) >= 0);
}

static 
app_lock_cursor_sig(w32_lock_cursor) {
  w32_state.is_cursor_locked = true;
  GetCursorPos(&w32_state.cursor_pt_to_lock_to);
}

static 
app_unlock_cursor_sig(w32_unlock_cursor) {
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
app_open_file_sig(w32_open_file)
{
  // Opening the file
  DWORD access_flag = {};
  DWORD creation_disposition = {};
  switch (file_access) {
    case APP_FILE_ACCESS_READ: {
      access_flag = GENERIC_READ;
      creation_disposition = OPEN_EXISTING;
    } break;
    case APP_FILE_ACCESS_OVERWRITE: {
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
app_close_file_sig(w32_close_file)
{
  w32_file_t* w32_file = (w32_file_t*)file->data;
  CloseHandle(w32_file->handle);
  
  w32_return_file(&w32_state.file_cabinet, w32_file);
  file->data = nullptr;
}

static 
app_read_file_sig(w32_read_file)
{ 
  w32_file_t* w32_file = (w32_file_t*)file->data;
  
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

static  
app_write_file_sig(w32_write_file)
{
  w32_file_t* w32_file = (w32_file_t*)file->data;
  
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

static 
app_add_task_sig(w32_add_task)
{
  w32_add_task_entry(&w32_state.work_queue, callback, data);
}

static 
app_complete_all_tasks_sig(w32_complete_all_tasks) 
{
  w32_complete_all_tasks_entries(&w32_state.work_queue);
}

static 
app_allocate_memory_sig(w32_allocate_memory)
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
app_free_memory_sig(w32_free_memory) {
  if (ptr) {
    auto* memory_block = (w32_memory_t*)(ptr);
    cll_remove(memory_block);
    VirtualFree(memory_block, 0, MEM_RELEASE);
  }
}

static
app_set_view_sig(w32_set_view) {
  gfx_t* gfx = w32_state.app->gfx;
  gfx_set_view(gfx, min_x, max_x, min_y, max_y, pos_x, pos_y); 
}

static
app_clear_canvas_sig(w32_clear_canvas) {
  gfx_t* gfx = w32_state.app->gfx;
  gfx_clear_colors(gfx, color); 
}

static
app_draw_sprite_sig(w32_draw_sprite) {
  gfx_t* gfx = w32_state.app->gfx;
  gfx_push_sprite(gfx, color, pos, size, anchor, texture_index, texel_x0, texel_y0, texel_x1, texel_y1 ); 
}

static 
app_draw_rect_sig(w32_draw_rect) {
  gfx_t* gfx = w32_state.app->gfx;
  gfx_draw_filled_rect(gfx,color, pos, rot, scale);
}

static 
app_draw_tri_sig(w32_draw_tri) {
  gfx_t* gfx = w32_state.app->gfx;
  gfx_draw_filled_triangle(gfx,color, p0, p1, p2);
}


static 
app_advance_depth_sig(w32_advance_depth) {
  gfx_t* gfx = w32_state.app->gfx;
  gfx_advance_depth(gfx);
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

  app_t app = {};

  app.show_cursor = w32_show_cursor;
  app.lock_cursor = w32_lock_cursor;
  app.hide_cursor = w32_hide_cursor;
  app.unlock_cursor = w32_unlock_cursor;
  app.allocate_memory = w32_allocate_memory;
  app.free_memory = w32_free_memory;
  app.debug_log = w32_log_proc;
  app.add_task = w32_add_task;
  app.complete_all_tasks = w32_complete_all_tasks;
  app.set_design_dimensions = w32_set_game_dims;
  app.open_file = w32_open_file;
  app.read_file = w32_read_file;
  app.write_file = w32_write_file;
  app.close_file = w32_close_file;
  app.set_view = w32_set_view;
  app.clear_canvas = w32_clear_canvas;
  app.draw_sprite = w32_draw_sprite;
  app.draw_rect =  w32_draw_rect;
  app.draw_tri = w32_draw_tri;
  app.advance_depth = w32_advance_depth;

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

    w32_state.app = &app;

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
  w32_log("Hello");
  defer { w32_unload_code(&game_code); };
  game_init_config_t config = game_functions.init();

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
  
  

  //
  // Gfx
  // 
  make(arena_t, gfx_arena);
  if (!w32_allocate_memory_into_arena(gfx_arena, megabytes(256))) return false;
  defer { w32_free_memory_from_arena(gfx_arena); };
  gfx_t* gfx = w32_gfx_load(
      window, 
      gfx_arena,
      config.render_command_size, 
      config.texture_queue_size,
      256);
  if (!gfx) { return 1; }
 
  // Init Audio
  make(arena_t, audio_arena);
  if (!w32_allocate_memory_into_arena(audio_arena, megabytes(256))) return false;
  defer { w32_free_memory_from_arena(audio_arena); };

  if (!w32_audio_load(&app.audio, 48000, 16, 2, 1, monitor_refresh_rate, audio_arena)) 
    return 1;
  defer{ w32_audio_unload(&app.audio); };


  //
  // Init debug stuff
  //
  make(arena_t, debug_arena);
  if (!w32_allocate_memory_into_arena(debug_arena, megabytes(256))) return false;
  defer { w32_free_memory_from_arena(debug_arena); };

  profiler_init(profiler, w32_get_performance_counter_u64, debug_arena, config.max_profiler_entries, config.max_profiler_snapshots);

  inspector_init(inspector, debug_arena, config.max_inspector_entries);


  //
  // Game setup
  //
  app.is_running = true;
  app.gfx = gfx;
  app.profiler = profiler;
  app.inspector = inspector;

  // Begin game loop
  b32_t is_sleep_granular = timeBeginPeriod(1) == TIMERR_NOERROR;
  
  // Send this to global state
  LARGE_INTEGER performance_frequency;
  QueryPerformanceFrequency(&performance_frequency);
  LARGE_INTEGER last_frame_count = w32_get_performance_counter();

  while (w32_state.is_running && app.is_running) 
  {
#if HOT_RELOADABLE
    // Hot reload game.dll functions
    app.is_dll_reloaded = w32_reload_code_if_outdated(&game_code);
    if (app.is_dll_reloaded) {
      profiler_reset(profiler);
    }
#else 
    profiler_reset(profiler);
#endif

    // Begin frame
    w32_audio_begin_frame(&app.audio);
    v2u_t client_wh = w32_get_client_dims(window);


    f32_t game_aspect = w32_state.game_width / w32_state.game_height;
    RECT rr = w32_calc_render_region(client_wh.w,
                                     client_wh.h,
                                     game_aspect);

    w32_gfx_begin_frame(gfx, client_wh, rr.left, rr.bottom, rr.right, rr.top);
       
    //Process messages and input
    w32_update_input(&app.input, window, target_secs_per_frame, rr);
    
    
    game_functions.update_and_render(&app);


    // End frame
    profiler_update_entries(profiler);
    w32_gfx_end_frame(gfx);
    w32_audio_end_frame(&app.audio);
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
