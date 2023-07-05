
#ifndef APP_H
#define APP_H

#include "game_gfx.h"
#include "game_input.h"
#include "game_console.h"
#include "game_asset_file.h"

//
// File IO API
// 
enum app_file_path_t {
  APP_FILE_PATH_EXE,
  APP_FILE_PATH_USER,
  APP_FILE_PATH_CACHE,

};

enum app_file_access_t {
  APP_FILE_ACCESS_READ,
  APP_FILE_ACCESS_OVERWRITE,
};

struct app_file_t {
  void* pf_data; // pointer for platform's usage
};

#define app_open_file_i(name) b32_t name(app_file_t* file, const char* filename, app_file_access_t file_access, app_file_path_t file_path)
typedef app_open_file_i(app_open_file_f);

#define app_close_file_i(name) void  name(app_file_t* file)
typedef app_close_file_i(app_close_file_f);

#define app_read_file_i(name) b32_t name(app_file_t* file, usz_t size, usz_t offset, void* dest)
typedef app_read_file_i(app_read_file_f);

#define app_write_file_i(name) b32_t name(app_file_t* file, usz_t size, usz_t offset, void* src)
typedef app_write_file_i(app_write_file_f);

//
// Logging API
// 
#define app_debug_log_i(name) void name(const char* fmt, ...)
typedef app_debug_log_i(app_debug_log_f);

//
// Cursor API
//
#define app_show_cursor_i(name) void name()
typedef app_show_cursor_i(app_show_cursor_f);


#define app_hide_cursor_i(name) void name()
typedef app_hide_cursor_i(app_hide_cursor_f);

#define app_lock_cursor_i(name) void name()
typedef app_lock_cursor_i(app_lock_cursor_f);

#define app_unlock_cursor_i(name) void name()
typedef app_unlock_cursor_i(app_unlock_cursor_f);


//
// Memory Allocation API
//
#define app_allocate_memory_i(name) void* name(usz_t size)
typedef app_allocate_memory_i(app_allocate_memory_f);

#define app_free_memory_i(name) void name(void* ptr)
typedef app_free_memory_i(app_free_memory_f);

//
// Multithreaded work API
//
typedef void app_task_callback_f(void* data);

#define app_add_task_i(name) void name(app_task_callback_f callback, void* data)
typedef app_add_task_i(app_add_task_f);

#define app_complete_all_tasks_i(name) void name()
typedef app_complete_all_tasks_i(app_complete_all_tasks_f);

// 
// Window/Graphics related
//
#define app_set_design_dimensions_i(name) void name(f32_t width, f32_t height);
typedef app_set_design_dimensions_i(app_set_design_dimensions_f);


//
struct audio_buffer_t {
  s16_t* sample_buffer;
  u32_t sample_count;
  u32_t channels; //TODO: remove this?
};

struct game_platform_config_t {
  usz_t texture_queue_size;
  usz_t render_command_size;

  // must be null terminated
  const char* window_title; // TODO(game): change to str8_t?
};

struct app_t {
  app_show_cursor_f* show_cursor;
  app_hide_cursor_f* hide_cursor;
  app_lock_cursor_f* lock_cursor;
  app_unlock_cursor_f* unlock_cursor;
  app_allocate_memory_f* allocate_memory;
  app_free_memory_f* free_memory;
  app_debug_log_f* debug_log;
  app_add_task_f* add_task;
  app_complete_all_tasks_f* complete_all_tasks;
  app_set_design_dimensions_f* set_design_dimensions;
  app_open_file_f* open_file;
  app_close_file_f* close_file;
  app_write_file_f* write_file;
  app_read_file_f* read_file;


  gfx_t* gfx;
  audio_buffer_t* audio; 
  profiler_t* profiler;
  input_t* input;
          
  b32_t is_dll_reloaded;
  b32_t is_running;

  void* game;
};

#define game_update_and_render_i(name) void name(app_t* a)
typedef game_update_and_render_i(game_update_and_render_f);

// TODO(Momo): change to interface
typedef game_platform_config_t game_get_platform_config_f(void); 

// To be called by platform
struct game_functions_t {
  game_get_platform_config_f* get_platform_config;
  game_update_and_render_f* update_and_render;
};

static const char* game_function_names[] {
  "game_get_platform_config",
  "game_update_and_render",
};


#include "game_assets.h"
#include "game_asset_rendering.h"

#endif //GAME_H
