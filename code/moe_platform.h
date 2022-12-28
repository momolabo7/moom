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
  umi_t size;
} Platform_Memory_Block;

typedef Platform_Memory_Block* Platform_Allocate_Memory(umi_t size);
typedef void  Platform_Free_Memory(Platform_Memory_Block* ptr);

//////////////////////////////////////////////////////////////////////
// Platform File API
typedef enum {
  PLATFORM_FILE_PATH_EXE,
  PLATFORM_FILE_PATH_USER,
  PLATFORM_FILE_PATH_CACHE,
  
}Platform_File_Path;

// Maybe for 'overwrite' or creating a new file, we 
// use a compl
typedef enum {
  PLATFORM_FILE_ACCESS_READ,
  PLATFORM_FILE_ACCESS_OVERWRITE,
}Platform_File_Access;

typedef struct {
  void* platform_data; // pointer for platform's usage
} Platform_File;

typedef b32_t  
Platform_Open_File(Platform_File* file,
                   const char* filename,
                   Platform_File_Access file_access,
                   Platform_File_Path file_path);

typedef void Platform_Close_File(Platform_File* file);
typedef b32_t Platform_Read_File(Platform_File* file, umi_t size, umi_t offset, void* dest);
typedef b32_t Platform_Write_File(Platform_File* file, umi_t size, umi_t offset, void* src);

//////////////////////////////////////////////////////////////////////////
// Platform multithreaded work API
typedef void Platform_Task_Callback(void* data);
typedef void Platform_Add_Task(Platform_Task_Callback callback, void* data);
typedef void Platform_Complete_All_Tasks();

/////////////////////////////////////////////////////////////////////////
//~Other platform API
#if 0
typedef void  Platform_Shutdown(); // trigger shutdown of application
                                   //
#endif
typedef void  Platform_Debug_Log(const char* fmt, ...);
typedef u64_t   Platform_Get_Performance_Counter();
typedef void  Platform_Set_Moe_Dims(f32_t width, f32_t height);


//////////////////////////////////////////////////////////////////////////
// Platform Audio API
//
typedef struct {
  s16_t* sample_buffer;
  u32_t sample_count;
  u32_t channels; //TODO: remove this?
}Platform_Audio;

/////////////////////////////////////////////////////////////////////////
// Input related API
//
typedef struct {
  b32_t before;
  b32_t now; 
}Platform_Button;


struct gfx_t;
struct profiler_t;



// These could really all be functions on the platform side
typedef struct {

  //arena_t* moe_arena; // Require 32MB
  gfx_t* gfx;
  profiler_t* profiler; 
  Platform_Audio* audio;

  // Input API
  // TODO Maybe this should be a seperate struct
  // Or maybe this should be handled on the platform side.
  union {
    struct {
      Platform_Button button_up;
      Platform_Button button_down;
      Platform_Button button_left;
      Platform_Button button_right;
      Platform_Button button_console;
      
      Platform_Button button_rotate_left;
      Platform_Button button_rotate_right;
      
      Platform_Button button_use;
      
      Platform_Button button_editor_on;
      Platform_Button button_editor0;
      Platform_Button button_editor1;
      Platform_Button button_editor2;
      Platform_Button button_editor3;
    };  
    Platform_Button buttons[15];
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
  Platform_Open_File* open_file;
  Platform_Read_File* read_file;
  Platform_Write_File* write_file;
  Platform_Close_File* close_file;
  
  // Multithreading API
  Platform_Add_Task* add_task;
  Platform_Complete_All_Tasks* complete_all_tasks;

  // Memory allocation
  Platform_Allocate_Memory* allocate_memory;
  Platform_Free_Memory* free_memory;

  // Logging
  Platform_Debug_Log* debug_log;

  // set window dimensions
  // TODO: change name
  Platform_Set_Moe_Dims* set_moe_dims;

  // Misc
  f32_t seconds_since_last_frame; //aka dt
  b32_t reloaded;

  // For moe to use
  void* moe;

} Platform;

typedef void Moe_Update_And_Render(Platform* pf);

// To be called by platform
typedef struct Moe_Functions {
  Moe_Update_And_Render* update_and_render;
} Moe_Functions;

static const char* moe_function_names[] {
  "moe_update_and_render",
};

static b32_t pf_is_button_poked(Platform_Button) ;
static b32_t pf_is_button_released(Platform_Button);
static b32_t pf_is_button_down(Platform_Button);
static b32_t pf_is_button_held(Platform_Button);
static void pf_update_input(Platform_Button);


/////////////////////////////////////////////////////////////
// Implementation
static void 
pf_update_input(Platform* pf) {
  for (u32_t i = 0; i < array_count(pf->buttons); ++i) {
    pf->buttons[i].before = pf->buttons[i].now;
  }
  pf->char_count = 0;
}

// before: 0, now: 1
static b32_t 
pf_is_button_poked(Platform_Button btn) {
  return !btn.before && btn.now;
}

// before: 1, now: 0
static b32_t
pf_is_button_released(Platform_Button btn) {
  return btn.before && !btn.now;
}


// before: X, now: 1
static b32_t
pf_is_button_down(Platform_Button btn){
  return btn.now;
}

// before: 1, now: 1
static b32_t
pf_is_button_held(Platform_Button btn) {
  return btn.before && btn.now;
}

#endif //MOE_PLATFORM_H
