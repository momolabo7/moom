#ifndef WIN_H
#define WIN_H

struct w32_memory_t {
  void* memory;
  usz_t size;
  
  w32_memory_t* prev;
  w32_memory_t* next;
};

struct w32_work_t {
  void* data;
  app_task_callback_f* callback;
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
  
  LARGE_INTEGER module_write_time;
  const char* tmp_path;
  
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



#endif //WIN_H
