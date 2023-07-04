#ifndef GAME_PLATFORM
#define GAME_PLATFORM



//
// Platform File API
//
enum pf_file_path_t {
  PF_FILE_PATH_EXE,
  PF_FILE_PATH_USER,
  PF_FILE_PATH_CACHE,

};

enum pf_file_access_t {
  PF_FILE_ACCESS_READ,
  PF_FILE_ACCESS_OVERWRITE,
};

struct pf_file_t {
  void* pf_data; // pointer for platform's usage
};

typedef b32_t pf_open_file_f(pf_file_t* file, const char* filename, pf_file_access_t file_access, pf_file_path_t file_path);
typedef void  pf_close_file_f(pf_file_t* file);
typedef b32_t pf_read_file_f(pf_file_t* file, usz_t size, usz_t offset, void* dest);
typedef b32_t pf_write_file_f(pf_file_t* file, usz_t size, usz_t offset, void* src);


//
// Platform multithreaded work API
//
typedef void pf_task_callback_f(void* data);
typedef void pf_add_task_f(pf_task_callback_f callback, void* data);
typedef void pf_complete_all_tasks_f();

//
// Other Platform API
// 
typedef void  pf_debug_log_f(const char* fmt, ...);

// TODO: remove this?
typedef void  pf_set_dims_f(f32_t width, f32_t height);


struct pf_t {
  // File IO
  pf_open_file_f* open_file;
  pf_read_file_f* read_file;
  pf_write_file_f* write_file;
  pf_close_file_f* close_file;

  // Multithreading API
  pf_add_task_f* add_task;
  pf_complete_all_tasks_f* complete_all_tasks;

  // Logging
  pf_debug_log_f* debug_log;

  // set window dimensions
  // TODO: change name
  pf_set_dims_f* set_design_dims;

};
static pf_t pf;

#endif
