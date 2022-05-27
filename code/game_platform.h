// This file and GFX.h contain structs that need to be 
// initialized by the OS and passed to the main Game_Update() 
// function. 
#ifndef GAME_PLATFORM_H
#define GAME_PLATFORM_H

const F32 game_width = 1600.f;
const F32 game_height = 900.f;
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
  B32 error;
  void* platform_data; // pointer for platform's usage
};
typedef Platform_File  
Platform_Open_File(const char* filename,
                   Platform_File_Access file_access,
                   Platform_File_Path file_path);

typedef void Platform_Close_File(Platform_File* file);
typedef void Platform_Read_File(Platform_File* file, UMI size, UMI offset, void* dest);
typedef void Platform_Write_File(Platform_File* file, UMI size, UMI offset, void* src);

//~Platform multithreaded work API
typedef void Platform_Task_Callback(void* data);
typedef void Platform_Add_Task(Platform_Task_Callback callback, void* data);
typedef void Platform_Complete_All_Tasks();


//~Other platform API
typedef void  Platform_Shutdown(); // trigger shutdown of application
#if 0
typedef void* Platform_Alloc(UMI size); // allocate memory
typedef void  Platform_Free(void* ptr);     // frees memory
#endif
typedef void  Platform_Set_Aspect_Ratio(U32 width, U32 height); // sets aspect ratio of game
typedef void  Platform_Debug_Log(const char* fmt, ...);
typedef U64   Platform_Get_Performance_Counter();


struct Platform_API {
  Platform_Shutdown* shutdown;
#if 0
  Platform_Alloc* alloc;
  Platform_Free* free;
#endif
  Platform_Open_File* open_file;
  Platform_Read_File* read_file;
  Platform_Write_File* write_file;
  Platform_Close_File* close_file;
  Platform_Add_Task* add_task;
  Platform_Complete_All_Tasks* complete_all_tasks;
  Platform_Get_Performance_Counter* get_performance_counter;
  Platform_Debug_Log* debug_log;
};
extern Platform_API g_platform;


//~Input API
// NOTE(Momo): Game_Input is not just 'controllers'.
// It is filled with 'things the game need to respond to' 
// so hence it's name and also why delta time is in there


struct Game_Input_Button {
  B32 before;
  B32 now; 
};

static B32 is_poked(Game_Input_Button) ;
static B32 is_released(Game_Input_Button);
static B32 is_down(Game_Input_Button);
static B32 is_held(Game_Input_Button);

struct Game_Input {
  union {
    struct {
      Game_Input_Button button_up;
      Game_Input_Button button_down;
      Game_Input_Button button_left;
      Game_Input_Button button_right;
      Game_Input_Button button_console;
      
      Game_Input_Button button_rotate_left;
      Game_Input_Button button_rotate_right;
      
      Game_Input_Button button_use;
      
      Game_Input_Button button_editor0;
      Game_Input_Button button_editor1;
    };  
    Game_Input_Button buttons[10];
  };
  
  V2 design_mouse_pos;
  V2U screen_mouse_pos;
  V2U render_mouse_pos;
  
  F32 seconds_since_last_frame; //aka dt
  
  U8 chars[32];
  U32 char_count;
};

void update(Game_Input_Button button);


//~ NOTE(Momo): Game API
// For things that don't change from the platform after setting it once
struct Renderer_Texture_Queue;
struct Renderer_Command_Queue;
struct Profiler;
struct Game_Memory {
  Arena* game_arena; // Require 32MB
  Platform_API platform_api;
  Renderer_Texture_Queue* renderer_texture_queue;
  Renderer_Command_Queue* renderer_command_queue;
  Profiler* profiler; 
  
  
  struct Game_State* game; // do not touch!
};

typedef void Game_Update_And_Render(Game_Memory* memory,
                                    Game_Input* input);

typedef void Game_Debug_Update_And_Render(Game_Memory* memory,
                                          Game_Input* input);

// To be called by platform
struct Game_Functions {
  Game_Update_And_Render* update_and_render;
};

static const char* game_function_names[] {
  "game_update_and_render",
};


#include "game_platform.cpp"

#endif //GAME_PLATFORM_H
