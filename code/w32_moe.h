
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
#include <timeapi.h>
#include <imm.h>
#include <initguid.h>
#include <audioclient.h>
#include <mmdeviceapi.h>
#undef near
#undef far

#include "momo.h"
#include "moe_platform.h"
#include "moe_profiler.h"

struct W32_Memory_Block {
  Platform_Memory_Block platform_block;

  W32_Memory_Block* prev;
  W32_Memory_Block* next;
};

struct W32_Work {
  void* data;
  Platform_Task_Callback* callback;
};

// TODO(momo): Is it possible to use a vector?
struct W32_Work_Queue {
  W32_Work entries[256];
  U32 volatile next_entry_to_read;
  U32 volatile next_entry_to_write;
  
  U32 volatile completion_count;
  U32 volatile completion_goal;
  HANDLE semaphore; 
  
};

struct W32_Loaded_Code {
  // Need to fill these up
  U32 function_count;
  const char** function_names;
  const char* module_path;
  void** functions;
  
#if INTERNAL
  LARGE_INTEGER module_write_time;
  const char* tmp_path;
#endif  
  
  B32 is_valid;
  HMODULE dll; 
};

struct W32_File {
  HANDLE handle;
  U32 cabinet_index;
};

// TODO(momo): is it possible to use a vector? 
struct W32_File_Cabinet {
  W32_File files[32]; 
  U32 free_files[32];
  U32 free_file_count;
};

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

Profiler _profiler = {0};
Profiler* profiler = &_profiler;

#define w32_log(...) w32_log_proc(__VA_ARGS__)
#define w32_profile_block(...) prf_block(profiler, __VA_ARGS__)
#else
#define w32_log(...)
#define w32_profiler_block(...)
#endif // INTERNAL

static inline LONG w32_rect_width(RECT r) { return r.right - r.left; }
static inline LONG w32_rect_height(RECT r) { return r.bottom - r.top; }

static inline V2U
w32_get_window_dims(HWND window) {
	RECT rect;
	GetWindowRect(window, &rect);
  
  V2U ret;
  ret.w = U32(rect.right - rect.left);
  ret.h = U32(rect.bottom - rect.top);
  
  return ret;
	
}

static V2U
w32_get_client_dims(HWND window) {
	RECT rect;
	GetClientRect(window, &rect);
  
  V2U ret;
  ret.w = U32(rect.right - rect.left);
  ret.h = U32(rect.bottom - rect.top);
  
  return ret;
	
}

static RECT 
w32_calc_render_region(U32 window_w, 
                       U32 window_h, 
                       F32 aspect_ratio)
{
	assert(aspect_ratio > 0.f && window_w > 0 && window_h > 0);
  
	RECT ret;
	
	F32 optimal_window_w = (F32)window_h * aspect_ratio;
	F32 optimal_window_h = (F32)window_w * 1.f/aspect_ratio;
	
	if (optimal_window_w > (F32)window_w) {
		// NOTE(Momo): width has priority - top and bottom bars
		ret.left = 0;
		ret.right = window_w;
		
		F32 empty_height = (F32)window_h - optimal_window_h;
		
		ret.bottom = (U32)(empty_height * 0.5f);
		ret.top = ret.bottom + (U32)optimal_window_h;
	}
	else {
		// NOTE(Momo): height has priority - left and right bars
		ret.bottom = 0;
		ret.top = window_h;
		
		
		F32 empty_width = (F32)window_w - optimal_window_w;
		
		ret.left = (U32)(empty_width * 0.5f);
		ret.right = ret.left + (U32)optimal_window_w;
	}
	
	return ret;
}

static LARGE_INTEGER
w32_get_performance_counter(void) {
  LARGE_INTEGER result;
  QueryPerformanceCounter(&result);
  return result;
}

static U64
w32_get_performance_counter_u64(void) {
  LARGE_INTEGER counter = w32_get_performance_counter();
  U64 ret = (U64)counter.QuadPart;
  return ret;
}
static F32
w32_get_secs_elapsed(LARGE_INTEGER start,
                     LARGE_INTEGER end,
                     LARGE_INTEGER performance_frequency) 
{
  return (F32(end.QuadPart - start.QuadPart)) / performance_frequency.QuadPart;
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





static void
w32_unload_code(W32_Loaded_Code* code) {
  if(code->dll) {
    FreeLibrary(code->dll);
    code->dll = 0;
  }
  code->is_valid = false;
  zero_range(code->functions, code->function_count);
}

static void
w32_load_code(W32_Loaded_Code* code) {
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
static B32
w32_reload_code_if_outdated(W32_Loaded_Code* code) {
  B32 reloaded = false;
  // Check last modified date
  LARGE_INTEGER last_write_time = w32_get_file_last_write_time(code->module_path);
  if(last_write_time.QuadPart > code->module_write_time.QuadPart) { 
    w32_unload_code(code); 
    for (U32 i = 0; i < 100; ++i ){
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
