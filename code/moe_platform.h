// This file contain the API through which the engine/moe interface with.
// Because it is just the API, it is written in pure C so that it can be used
// in other languages.
//
#ifndef MOE_PLATFORM_H
#define MOE_PLATFORM_H

#include "momo.h"

/////////////////////////////////////////////////////////////////////
// Platform Memory API
//
typedef struct {
  void* data;
} platform_memory_block_t;

typedef platform_memory_block_t* platform_allocate_memory_f(umi_t size);
typedef void  platform_free_memory_f(platform_memory_block_t* ptr);

//////////////////////////////////////////////////////////////////////
// Platform File API
typedef enum {
  PLATFORM_FILE_PATH_EXE,
  PLATFORM_FILE_PATH_USER,
  PLATFORM_FILE_PATH_CACHE,
  
} platform_file_path_t;

typedef enum {
  PLATFORM_FILE_ACCESS_READ,
  PLATFORM_FILE_ACCESS_OVERWRITE,
} platform_file_access_t;

typedef struct {
  void* platform_data; // pointer for platform's usage
} platform_file_t;

typedef b32_t  
platform_open_file_f(platform_file_t* file,
                     const char* filename,
                     platform_file_access_t file_access,
                     platform_file_path_t file_path);

typedef void  platform_close_file_f(platform_file_t* file);
typedef b32_t platform_read_file_f(platform_file_t* file, umi_t size, umi_t offset, void* dest);
typedef b32_t platform_write_file_f(platform_file_t* file, umi_t size, umi_t offset, void* src);

//
// platform_t multithreaded work API
typedef void platform_task_callback_f(void* data);
typedef void platform_add_task_f(platform_task_callback_f callback, void* data);
typedef void platform_complete_all_tasks_f();

/////////////////////////////////////////////////////////////////////////
// Other platform API
typedef void  platform_debug_log_f(const char* fmt, ...);
typedef u64_t platform_get_perforance_counter_f();
typedef void  platform_set_moe_dims_f(f32_t width, f32_t height);


//////////////////////////////////////////////////////////////////////////
// Platform Audio API
//
typedef struct {
  s16_t* sample_buffer;
  u32_t sample_count;
  u32_t channels; //TODO: remove this?
} platform_audio_t;

/////////////////////////////////////////////////////////////////////////
// Input related API
//
typedef struct {
  b32_t before;
  b32_t now; 
} platform_button_t;


struct gfx_t;
struct profiler_t;



// These could really all be functions on the platform side
typedef struct {

  //arena_t* moe_arena; // Require 32MB
  gfx_t* gfx;
  profiler_t* profiler; 
  platform_audio_t* audio;

  // Input API
  // TODO Maybe this should be a seperate struct
  // Or maybe this should be handled on the platform side.
  union {
    struct {
      platform_button_t button_up;
      platform_button_t button_down;
      platform_button_t button_left;
      platform_button_t button_right;
      platform_button_t button_console;
      
      platform_button_t button_rotate_left;
      platform_button_t button_rotate_right;
      
      platform_button_t button_use;
      
      platform_button_t button_editor_on;
      platform_button_t button_editor0;
      platform_button_t button_editor1;
      platform_button_t button_editor2;
      platform_button_t button_editor3;
    };  
    platform_button_t buttons[15];
  };
  u8_t chars[32];
  u32_t char_count;

  
  //v2u_t screen_mouse_pos;
  //v2u_t render_mouse_pos;

  // NOTE(Momo): The mouse position is relative to the moe's dimensions given
  // via set_moe_dims(). It is possible to get back the normalized dimensions
  // by dividing the x/y by the width/height of the moe.
  v2f_t mouse_pos;
 
  /////////////////////////////////////////////////////////
  // Functions

  // File IO
  platform_open_file_f* open_file;
  platform_read_file_f* read_file;
  platform_write_file_f* write_file;
  platform_close_file_f* close_file;
  
  // Multithreading API
  platform_add_task_f* add_task;
  platform_complete_all_tasks_f* complete_all_tasks;

  // Memory allocation
  platform_allocate_memory_f* allocate_memory;
  platform_free_memory_f* free_memory;

  // Logging
  platform_debug_log_f* debug_log;

  // set window dimensions
  // TODO: change name
  platform_set_moe_dims_f* set_moe_dims;

  // Misc
  f32_t seconds_since_last_frame; //aka dt
  b32_t reloaded;

  // For moe to use
  void* moe;

} platform_t;

typedef void moe_update_and_render_f(platform_t* pf);

// To be called by platform
typedef struct moe_functions_t {
  moe_update_and_render_f* update_and_render;
} moe_functions_t;

static const char* moe_function_names[] {
  "moe_update_and_render",
};

static b32_t platform_is_button_poked(platform_button_t) ;
static b32_t platform_is_button_released(platform_button_t);
static b32_t platform_is_button_down(platform_button_t);
static b32_t platform_is_button_held(platform_button_t);
static void  platform_update_input(platform_button_t);


/////////////////////////////////////////////////////////////
// Implementation
static void 
platform_update_input(platform_t* pf) {
  for (u32_t i = 0; i < array_count(pf->buttons); ++i) {
    pf->buttons[i].before = pf->buttons[i].now;
  }
  pf->char_count = 0;
}

// before: 0, now: 1
static b32_t 
platform_is_button_poked(platform_button_t btn) {
  return !btn.before && btn.now;
}

// before: 1, now: 0
static b32_t
platform_is_button_released(platform_button_t btn) {
  return btn.before && !btn.now;
}


// before: X, now: 1
static b32_t
platform_is_button_down(platform_button_t btn){
  return btn.now;
}

// before: 1, now: 1
static b32_t
platform_is_button_held(platform_button_t btn) {
  return btn.before && btn.now;
}

#endif //MOE_PLATFORM_H
