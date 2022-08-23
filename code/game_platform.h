// This file and GFX.h contain structs that need to be 
// initialized by the OS and passed to the main Game_Update() 
// function. 
// 
// NOTE(Momo): This should probably be in pure C?
//
#ifndef GAME_PLATFORM_H
#define GAME_PLATFORM_H

#include "momo.h"

// TODO: This should be in a function give by game I think
const F32 game_width = 1600.f;
const F32 game_height = 900.f;
const V2 game_wh = V2{game_width, game_height};
const F32 game_aspect_ratio = game_width/game_height;

//~Platform File API
enum Platform_File_Path {
  PLATFORM_FILE_PATH_EXE,
  PLATFORM_FILE_PATH_USER,
  PLATFORM_FILE_PATH_CACHE,
  
};

// Maybe for 'overwrite' or creating a new file, we 
// use a compl
enum Platform_File_Access {
  PLATFORM_FILE_ACCESS_READ,
  PLATFORM_FILE_ACCESS_OVERWRITE,
};

struct Platform_File {
  void* platform_data; // pointer for platform's usage
};

typedef B32  
Platform_Open_File(Platform_File* file,
                   const char* filename,
                   Platform_File_Access file_access,
                   Platform_File_Path file_path);

typedef void Platform_Close_File(Platform_File* file);
typedef B32 Platform_Read_File(Platform_File* file, UMI size, UMI offset, void* dest);
typedef B32 Platform_Write_File(Platform_File* file, UMI size, UMI offset, void* src);

//~Platform multithreaded work API
typedef void Platform_Task_Callback(void* data);
typedef void Platform_Add_Task(Platform_Task_Callback callback, void* data);
typedef void Platform_Complete_All_Tasks();


//~Other platform API
#if 0
typedef void  Platform_Shutdown(); // trigger shutdown of application
typedef void* Platform_Alloc(UMI size); // allocate memory
typedef void  Platform_Free(void* ptr);     // frees memory
#endif
typedef void  Platform_Set_Aspect_Ratio(U32 width, U32 height); // sets aspect ratio of game
typedef void  Platform_Debug_Log(const char* fmt, ...);
typedef U64   Platform_Get_Performance_Counter();


struct Platform_API {
  Platform_Open_File* open_file;
  Platform_Read_File* read_file;
  Platform_Write_File* write_file;
  Platform_Close_File* close_file;
  Platform_Add_Task* add_task;
  Platform_Complete_All_Tasks* complete_all_tasks;
  Platform_Get_Performance_Counter* get_performance_counter;
  Platform_Debug_Log* debug_log;
};


//~Input API
// NOTE(Momo): Game_Input is not just 'controllers'.
// It is filled with 'things the game need to respond to' 
// so hence it's name and also why delta time is in there


//~ NOTE(Momo): Game Memory API
// For things that don't change from the platform after setting it once
//~ Audio API
struct Platform_Audio {
    S16* sample_buffer;
    U32 sample_count;
    U32 channels; //TODO: remove this?
};


struct Platform_Button {
  B32 before;
  B32 now; 
};

struct Gfx;
struct Profiler;

// These could really all be functions on the platform side
struct Platform {
  Bump_Allocator* game_arena; // Require 32MB
  //Platform_API platform_api;
  
  Gfx* gfx;
  
  Profiler* profiler; 
  Platform_Audio* audio;

  // Maybe this should be a seperate struct
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
    Platform_Button buttons[13];
  };
  
  V2 design_mouse_pos;
  V2U screen_mouse_pos;
  V2U render_mouse_pos;
  
  F32 seconds_since_last_frame; //aka dt
  
  U8 chars[32];
  U32 char_count;
  
  B32 reloaded;

  // API Functions
  // Ideally, everything in here should be like this
  Platform_Open_File* open_file;
  Platform_Read_File* read_file;
  Platform_Write_File* write_file;
  Platform_Close_File* close_file;
  Platform_Add_Task* add_task;
  Platform_Complete_All_Tasks* complete_all_tasks;
  Platform_Get_Performance_Counter* get_performance_counter;
  Platform_Debug_Log* debug_log;


  // For game to use
  void* game;
};

extern Platform* g_platform;

typedef void Game_Update_And_Render(Platform* pf);
typedef void Game_Debug_Update_And_Render(Platform* pf);

// To be called by platform
struct Game_Functions {
  Game_Update_And_Render* update_and_render;
};

static const char* game_function_names[] {
  "game_update_and_render",
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

#endif //GAME_PLATFORM_H
