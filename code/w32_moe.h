
#ifndef WIN_H
#define WIN_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
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
#include "game_platform.h"

struct w32_memory_t {
  pf_memory_t pf_memory;
  
  w32_memory_t* prev;
  w32_memory_t* next;
};

struct w32_work_t {
  void* data;
  pf_task_callback_f* callback;
};

// TODO(momo): Is it possible to use a vector?
struct w32_work_queue_t {
  w32_work_t entries[256];
  u32_t volatile next_entry_to_read;
  u32_t volatile next_entry_to_write;
  
  u32_t volatile completion_count;
  u32_t volatile completion_goal;
  HANDLE semaphore; 
  
};

struct w32_loaded_code_t {
  // Need to fill these up
  u32_t function_count;
  const char** function_names;
  const char* module_path;
  void** functions;
  
#if INTERNAL
  LARGE_INTEGER module_write_time;
  const char* tmp_path;
#endif  
  
  b32_t is_valid;
  HMODULE dll; 
};

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
};
static w32_state_t w32_state;

#if INTERNAL
#include <stdio.h>
static void
w32_log_proc(const char* fmt, ...) {
  char buffer[256] = {0};
  va_list args;
  va_start(args, fmt);
  vsprintf(buffer, fmt, args);
  va_end(args);
  OutputDebugStringA(buffer);
}

profiler_t _profiler = {0};
profiler_t* profiler = &_profiler;

#define w32_log(...) w32_log_proc(__VA_ARGS__)
#define w32_profile_block(...) profiler_block(profiler, __VA_ARGS__)
#else
#define w32_log(...)
#define w32_profiler_block(...)
#endif // INTERNAL

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

static pf_memory_t*
w32_allocate_memory(umi_t size)
{
  const auto alignment = 16;
  umi_t aligned_size = align_up_pow2(size, alignment);
  umi_t padding_for_alignment = aligned_size - size;
  umi_t total_size = size + padding_for_alignment + sizeof(w32_memory_t);
  umi_t base_offset = sizeof(w32_memory_t);

  auto* block = (w32_memory_t*)
    VirtualAllocEx(GetCurrentProcess(),
                   0, 
                   total_size,
                   MEM_RESERVE | MEM_COMMIT, 
                   PAGE_READWRITE);
  if (!block) return nullptr;


  block->pf_memory.data = (u8_t*)block + base_offset; 
  block->pf_memory.size = size;

  w32_memory_t* sentinel = &w32_state.memory_sentinel;
  cll_append(sentinel, block);

  return &block->pf_memory;

}

static void
w32_free_memory(pf_memory_t* block) {
  if (block) {
    auto* memory_block = (w32_memory_t*)(block);
    cll_remove(memory_block);
    VirtualFree(memory_block, 0, MEM_RELEASE);
  }
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
  
#if INTERNAL
  b32_t copy_success = false;
  for (u32_t attempt = 0; attempt < 100; ++attempt) {
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

#if INTERNAL
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
#endif // INTERNAL 




#if 1 // OPENGL
#include "w32_gfx_opengl.h"
#else // DX etc 
#endif 

#include "w32_audio_wasapi.h"

#endif //WIN_H
