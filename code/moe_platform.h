// This file contain the API through which the engine/moe interface with.
// Because it is just the API, it is written in pure C so that it can be used
// in other languages.
//
#ifndef MOE_PLATFORM_H
#define MOE_PLATFORM_H

// TODO: remove this and make this file as standalone as possible?
//
#include "momo.h"

/////////////////////////////////////////////////////////////////////
// Platform Memory API
typedef struct {
  void* data;
  UMI size;
} Platform_Memory_Block;

typedef Platform_Memory_Block* Platform_Allocate_Memory(UMI size); // allocate memory
typedef void  Platform_Free_Memory(Platform_Memory_Block* ptr);     // frees memory

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

typedef B32  
Platform_Open_File(Platform_File* file,
                   const char* filename,
                   Platform_File_Access file_access,
                   Platform_File_Path file_path);

typedef void Platform_Close_File(Platform_File* file);
typedef B32 Platform_Read_File(Platform_File* file, UMI size, UMI offset, void* dest);
typedef B32 Platform_Write_File(Platform_File* file, UMI size, UMI offset, void* src);

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
typedef U64   Platform_Get_Performance_Counter();
typedef void  Platform_Set_Moe_Dims(F32 width, F32 height);


//////////////////////////////////////////////////////////////////////////
// Platform Audio API
//
typedef struct {
  S16* sample_buffer;
  U32 sample_count;
  U32 channels; //TODO: remove this?
}Platform_Audio;

/////////////////////////////////////////////////////////////////////////
// Input related API
//
typedef struct {
  B32 before;
  B32 now; 
}Platform_Button;


struct Gfx;
struct Profiler;



// These could really all be functions on the platform side
typedef struct {

  //Arena* moe_arena; // Require 32MB
  Gfx* gfx;
  Profiler* profiler; 
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
  U8 chars[32];
  U32 char_count;

  
  //V2U screen_mouse_pos;
  //V2U render_mouse_pos;

  // NOTE(Momo): The mouse position is relative to the moe's dimensions given
  // via set_moe_dims(). It is possible to get back the normalized dimensions
  // by dividing the x/y by the width/height of the moe.
  V2 mouse_pos;
 
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
  F32 seconds_since_last_frame; //aka dt
  B32 reloaded;

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

static B32 pf_is_button_poked(Platform_Button) ;
static B32 pf_is_button_released(Platform_Button);
static B32 pf_is_button_down(Platform_Button);
static B32 pf_is_button_held(Platform_Button);
static void pf_update_input(Platform_Button);


/////////////////////////////////////////////////////////////
// Implementation
static void 
pf_update_input(Platform* pf) {
  for (U32 i = 0; i < array_count(pf->buttons); ++i) {
    pf->buttons[i].before = pf->buttons[i].now;
  }
  pf->char_count = 0;
}

// before: 0, now: 1
static B32 
pf_is_button_poked(Platform_Button btn) {
  return !btn.before && btn.now;
}

// before: 1, now: 0
static B32
pf_is_button_released(Platform_Button btn) {
  return btn.before && !btn.now;
}


// before: X, now: 1
static B32
pf_is_button_down(Platform_Button btn){
  return btn.now;
}

// before: 1, now: 1
static B32
pf_is_button_held(Platform_Button btn) {
  return btn.before && btn.now;
}

#endif //MOE_PLATFORM_H
