
#ifndef APP_H
#define APP_H

#include "game_gfx.h"
#include "game_console.h"
#include "game_asset_file.h"

//
// Button API
// 

struct app_button_t {
  b32_t before : 1;
  b32_t now: 1; 
};


// my god
enum app_button_code_t {
  // Keyboard keys
  APP_BUTTON_CODE_UNKNOWN,
  APP_BUTTON_CODE_0,
  APP_BUTTON_CODE_1,
  APP_BUTTON_CODE_2,
  APP_BUTTON_CODE_3,
  APP_BUTTON_CODE_4,
  APP_BUTTON_CODE_5,
  APP_BUTTON_CODE_6,
  APP_BUTTON_CODE_7,
  APP_BUTTON_CODE_8,
  APP_BUTTON_CODE_9,
  APP_BUTTON_CODE_F1,
  APP_BUTTON_CODE_F2,
  APP_BUTTON_CODE_F3,
  APP_BUTTON_CODE_F4,
  APP_BUTTON_CODE_F5,
  APP_BUTTON_CODE_F6,
  APP_BUTTON_CODE_F7,
  APP_BUTTON_CODE_F8,
  APP_BUTTON_CODE_F9,
  APP_BUTTON_CODE_F10,
  APP_BUTTON_CODE_F11,
  APP_BUTTON_CODE_F12,
  APP_BUTTON_CODE_BACKSPACE,
  APP_BUTTON_CODE_A,
  APP_BUTTON_CODE_B,
  APP_BUTTON_CODE_C,
  APP_BUTTON_CODE_D,
  APP_BUTTON_CODE_E,
  APP_BUTTON_CODE_F,
  APP_BUTTON_CODE_G,
  APP_BUTTON_CODE_H,
  APP_BUTTON_CODE_I,
  APP_BUTTON_CODE_J,
  APP_BUTTON_CODE_K,
  APP_BUTTON_CODE_L,
  APP_BUTTON_CODE_M,
  APP_BUTTON_CODE_N,
  APP_BUTTON_CODE_O,
  APP_BUTTON_CODE_P,
  APP_BUTTON_CODE_Q,
  APP_BUTTON_CODE_R,
  APP_BUTTON_CODE_S,
  APP_BUTTON_CODE_T,
  APP_BUTTON_CODE_U,
  APP_BUTTON_CODE_V,
  APP_BUTTON_CODE_W,
  APP_BUTTON_CODE_X,
  APP_BUTTON_CODE_Y,
  APP_BUTTON_CODE_Z,
  APP_BUTTON_CODE_SPACE,
  APP_BUTTON_CODE_RMB,
  APP_BUTTON_CODE_LMB,
  APP_BUTTON_CODE_MMB,

  APP_BUTTON_CODE_MAX,

};

//
// Input API
//
// NOTE(momo): Input is SPECIFICALLY stuff that can be recorded and
// replayed by some kind of system. Other things go to game_t
struct app_input_t {
  app_button_t buttons[APP_BUTTON_CODE_MAX];
  u8_t chars[32];
  u32_t char_count;

  // NOTE(Momo): The mouse position is relative to the moe's dimensions given
  // via set_design_dims(). It is possible to get back the normalized dimensions
  // by dividing the x/y by the width/height of the moe.
  v2f_t mouse_pos;

  // NOTE(Momo): Mouse wheels values are not normally analog!
  // +ve is forwards
  // -ve is backwards
  s32_t mouse_scroll_delta;

  // TODO(Momo): not sure if this should even be here
  f32_t delta_time; //aka dt
};

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
  void* data; // pointer for platform's usage
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

#define app_complete_all_tasks_i(name) void name(void)
typedef app_complete_all_tasks_i(app_complete_all_tasks_f);

// 
// Window/Graphics related
//
#define app_set_design_dimensions_i(name) void name(f32_t width, f32_t height);
typedef app_set_design_dimensions_i(app_set_design_dimensions_f);


//
// Input API
//


struct audio_buffer_t {
  s16_t* sample_buffer;
  u32_t sample_count;
  u32_t channels; //TODO: remove this?
};

struct game_init_config_t {
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

  app_input_t input;

  gfx_t* gfx;
  audio_buffer_t* audio; 
  profiler_t* profiler;
  
          
  b32_t is_dll_reloaded;
  b32_t is_running;

  void* game;
};


#define game_init_i(name) game_init_config_t name(void)
typedef game_init_i(game_init_f);

#define game_update_and_render_i(name) void name(app_t* a)
typedef game_update_and_render_i(game_update_and_render_f);

// To be called by platform
struct game_functions_t {
  game_init_f* init;
  game_update_and_render_f* update_and_render;
};

static const char* game_function_names[] {
  "game_init",
  "game_update_and_render",
};


//
// App function implementation
//

// before: 0, now: 1
static b32_t
app_is_button_poked(app_t* app, app_button_code_t code) {
  app_input_t* in = &app->input;
  auto btn = in->buttons[code];
  return !btn.before && btn.now;
}

// before: 1, now: 0
static b32_t
app_is_button_released(app_t* app, app_button_code_t code) {
  app_input_t* in = &app->input;
  auto btn = in->buttons[code];
  return btn.before && !btn.now;
}

// before: X, now: 1
static b32_t
app_is_button_down(app_t* app, app_button_code_t code){
  app_input_t* in = &app->input;
  return in->buttons[code].now;
}


// before: 1, now: 1
static b32_t
app_is_button_held(app_t* app, app_button_code_t code) {
  app_input_t* in = &app->input;
  auto btn = in->buttons[code];
  return btn.before && btn.now;
}


#include "game_assets.h"
#include "game_asset_rendering.h"

#endif //GAME_H
