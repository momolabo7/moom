// This file contain the API through which the engine/moe interface with.
// Because it is just the API, it is written in pure C so that it can be used
// in other languages.
//
#ifndef MOE_PF_H
#define MOE_PF_H

#include "momo.h"

//
// Platform Memory API
//
typedef struct {
  void* data;
  usz_t size;
} pf_memory_t;
typedef pf_memory_t* pf_allocate_memory_f(usz_t size);
typedef void  pf_free_memory_f(pf_memory_t* ptr);


//
// Platform File API
//
typedef enum {
  PF_FILE_PATH_EXE,
  PF_FILE_PATH_USER,
  PF_FILE_PATH_CACHE,

} pf_file_path_t;

typedef enum {
  PF_FILE_ACCESS_READ,
  PF_FILE_ACCESS_OVERWRITE,
} pf_file_access_t;

typedef struct {
  void* pf_data; // pointer for platform's usage
} pf_file_t;

typedef b32_t  
pf_open_file_f(
    pf_file_t* file,
    const char* filename,
    pf_file_access_t file_access,
    pf_file_path_t file_path);

typedef void  pf_close_file_f(pf_file_t* file);
typedef b32_t pf_read_file_f(pf_file_t* file, usz_t size, usz_t offset, void* dest);
typedef b32_t pf_write_file_f(pf_file_t* file, usz_t size, usz_t offset, void* src);

//
// Platform multithreaded work API
typedef void pf_task_callback_f(void* data);
typedef void pf_add_task_f(pf_task_callback_f callback, void* data);
typedef void pf_complete_all_tasks_f();

//
// Other Platform API
// 
typedef void  pf_debug_log_f(const char* fmt, ...);
typedef u64_t pf_get_perforance_counter_f();
typedef void  pf_set_moe_dims_f(f32_t width, f32_t height);


//
// Platform Audio API
//
typedef struct {
  s16_t* sample_buffer;
  u32_t sample_count;
  u32_t channels; //TODO: remove this?
} pf_audio_t;


//
// Input related API
//
typedef struct {
  b32_t before;
  b32_t now; 
} input_button_t;

typedef void pf_show_cursor_f();
typedef void pf_hide_cursor_f();
typedef void pf_lock_cursor_f();
typedef void pf_unlock_cursor_f();

// my god
typedef enum {
  // Keyboard keys
  INPUT_BUTTON_CODE_UNKNOWN,
  INPUT_BUTTON_CODE_0,
  INPUT_BUTTON_CODE_1,
  INPUT_BUTTON_CODE_2,
  INPUT_BUTTON_CODE_3,
  INPUT_BUTTON_CODE_4,
  INPUT_BUTTON_CODE_5,
  INPUT_BUTTON_CODE_6,
  INPUT_BUTTON_CODE_7,
  INPUT_BUTTON_CODE_8,
  INPUT_BUTTON_CODE_9,
  INPUT_BUTTON_CODE_F1,
  INPUT_BUTTON_CODE_F2,
  INPUT_BUTTON_CODE_F3,
  INPUT_BUTTON_CODE_F4,
  INPUT_BUTTON_CODE_F5,
  INPUT_BUTTON_CODE_F6,
  INPUT_BUTTON_CODE_F7,
  INPUT_BUTTON_CODE_F8,
  INPUT_BUTTON_CODE_F9,
  INPUT_BUTTON_CODE_F10,
  INPUT_BUTTON_CODE_F11,
  INPUT_BUTTON_CODE_F12,
  INPUT_BUTTON_CODE_BACKSPACE,
  INPUT_BUTTON_CODE_A,
  INPUT_BUTTON_CODE_B,
  INPUT_BUTTON_CODE_C,
  INPUT_BUTTON_CODE_D,
  INPUT_BUTTON_CODE_E,
  INPUT_BUTTON_CODE_F,
  INPUT_BUTTON_CODE_G,
  INPUT_BUTTON_CODE_H,
  INPUT_BUTTON_CODE_I,
  INPUT_BUTTON_CODE_J,
  INPUT_BUTTON_CODE_K,
  INPUT_BUTTON_CODE_L,
  INPUT_BUTTON_CODE_M,
  INPUT_BUTTON_CODE_N,
  INPUT_BUTTON_CODE_O,
  INPUT_BUTTON_CODE_P,
  INPUT_BUTTON_CODE_Q,
  INPUT_BUTTON_CODE_R,
  INPUT_BUTTON_CODE_S,
  INPUT_BUTTON_CODE_T,
  INPUT_BUTTON_CODE_U,
  INPUT_BUTTON_CODE_V,
  INPUT_BUTTON_CODE_W,
  INPUT_BUTTON_CODE_X,
  INPUT_BUTTON_CODE_Y,
  INPUT_BUTTON_CODE_Z,
  INPUT_BUTTON_CODE_SPACE,
  INPUT_BUTTON_CODE_RMB,
  INPUT_BUTTON_CODE_LMB,
  INPUT_BUTTON_CODE_MMB,

  INPUT_BUTTON_CODE_MAX,

} input_button_code_t;

// NOTE(momo): Input is SPECIFICALLY stuff that can be recorded and
// replayed by some kind of system. Other things go to moe_t
typedef struct {
  input_button_t buttons[INPUT_BUTTON_CODE_MAX];
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

} input_t;

// 
// Platform API
//
struct gfx_t;
struct profiler_t;

typedef struct {
  // File IO
  pf_open_file_f* open_file;
  pf_read_file_f* read_file;
  pf_write_file_f* write_file;
  pf_close_file_f* close_file;

  // Multithreading API
  pf_add_task_f* add_task;
  pf_complete_all_tasks_f* complete_all_tasks;

  // Memory allocation
  pf_allocate_memory_f* allocate_memory;
  pf_free_memory_f* free_memory;

  // Mouse 
  pf_show_cursor_f* show_cursor;
  pf_hide_cursor_f* hide_cursor;
  pf_lock_cursor_f* lock_cursor;
  pf_unlock_cursor_f* unlock_cursor;


  // Logging
  pf_debug_log_f* debug_log;

  // set window dimensions
  // TODO: change name
  pf_set_moe_dims_f* set_design_dims;

} pf_t;



typedef struct {
  // For moe to use
  void* game_context;

          
  b32_t is_dll_reloaded;
  b32_t is_running;
} moe_t;

typedef void moe_update_and_render_f(
    moe_t*, 
    pf_t*, 
    gfx_t*, 
    pf_audio_t*, 
    profiler_t*, 
    input_t*);

// To be called by platform
typedef struct moe_functions_t {
  moe_update_and_render_f* update_and_render;
} moe_functions_t;

static const char* moe_function_names[] {
  "moe_update_and_render",
};


// before: 0, now: 1
static b32_t 
input_is_button_poked(input_button_t btn) {
  return !btn.before && btn.now;
}

// before: 1, now: 0
static b32_t
input_is_button_released(input_button_t btn) {
  return btn.before && !btn.now;
}

// before: X, now: 1
static b32_t
input_is_button_down(input_button_t btn){
  return btn.now;
}

// before: 1, now: 1
static b32_t
input_is_button_held(input_button_t btn) {
  return btn.before && btn.now;
}

#endif //MOE_PF_H
