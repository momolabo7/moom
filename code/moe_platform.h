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
} pf_button_t;

// my god
typedef enum {
  // Keyboard keys
  PF_BUTTON_CODE_UNKNOWN,
  PF_BUTTON_CODE_0,
  PF_BUTTON_CODE_1,
  PF_BUTTON_CODE_2,
  PF_BUTTON_CODE_3,
  PF_BUTTON_CODE_4,
  PF_BUTTON_CODE_5,
  PF_BUTTON_CODE_6,
  PF_BUTTON_CODE_7,
  PF_BUTTON_CODE_8,
  PF_BUTTON_CODE_9,
  PF_BUTTON_CODE_F1,
  PF_BUTTON_CODE_F2,
  PF_BUTTON_CODE_F3,
  PF_BUTTON_CODE_F4,
  PF_BUTTON_CODE_F5,
  PF_BUTTON_CODE_F6,
  PF_BUTTON_CODE_F7,
  PF_BUTTON_CODE_F8,
  PF_BUTTON_CODE_F9,
  PF_BUTTON_CODE_F10,
  PF_BUTTON_CODE_F11,
  PF_BUTTON_CODE_F12,
  PF_BUTTON_CODE_BACKSPACE,
  PF_BUTTON_CODE_A,
  PF_BUTTON_CODE_B,
  PF_BUTTON_CODE_C,
  PF_BUTTON_CODE_D,
  PF_BUTTON_CODE_E,
  PF_BUTTON_CODE_F,
  PF_BUTTON_CODE_G,
  PF_BUTTON_CODE_H,
  PF_BUTTON_CODE_I,
  PF_BUTTON_CODE_J,
  PF_BUTTON_CODE_K,
  PF_BUTTON_CODE_L,
  PF_BUTTON_CODE_M,
  PF_BUTTON_CODE_N,
  PF_BUTTON_CODE_O,
  PF_BUTTON_CODE_P,
  PF_BUTTON_CODE_Q,
  PF_BUTTON_CODE_R,
  PF_BUTTON_CODE_S,
  PF_BUTTON_CODE_T,
  PF_BUTTON_CODE_U,
  PF_BUTTON_CODE_V,
  PF_BUTTON_CODE_W,
  PF_BUTTON_CODE_X,
  PF_BUTTON_CODE_Y,
  PF_BUTTON_CODE_Z,
  PF_BUTTON_CODE_SPACE,
  PF_BUTTON_CODE_RMB,
  PF_BUTTON_CODE_LMB,
  PF_BUTTON_CODE_MMB,

  PF_BUTTON_CODE_MAX,

} pf_button_code_t;

typedef struct {
  pf_button_t buttons[PF_BUTTON_CODE_MAX];
  u8_t chars[32];
  u32_t char_count;

  // NOTE(Momo): The mouse position is relative to the moe's dimensions given
  // via set_moe_dims(). It is possible to get back the normalized dimensions
  // by dividing the x/y by the width/height of the moe.
  v2f_t mouse_pos;

} input_t;

// 
// Platform API
//
struct gfx_t;
struct profiler_t;

typedef struct {
  gfx_t* gfx;
  profiler_t* profiler; 
  pf_audio_t* audio;
  input_t* input;

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

  // Logging
  pf_debug_log_f* debug_log;

  // set window dimensions
  // TODO: change name
  pf_set_moe_dims_f* set_moe_dims;

  // Misc
  f32_t seconds_since_last_frame; //aka dt
  b32_t reloaded;

  // For moe to use
  void* game_context;

} moe_t;

typedef b32_t moe_update_and_render_f(moe_t* pf);

// To be called by platform
typedef struct moe_functions_t {
  moe_update_and_render_f* update_and_render;
} moe_functions_t;

static const char* moe_function_names[] {
  "moe_update_and_render",
};

//
// Implementation
//


// before: 0, now: 1
static b32_t 
pf_is_button_poked(pf_button_t btn) {
  return !btn.before && btn.now;
}

// before: 1, now: 0
static b32_t
pf_is_button_released(pf_button_t btn) {
  return btn.before && !btn.now;
}


// before: X, now: 1
static b32_t
pf_is_button_down(pf_button_t btn){
  return btn.now;
}

// before: 1, now: 1
static b32_t
pf_is_button_held(pf_button_t btn) {
  return btn.before && btn.now;
}

#endif //MOE_PF_H
