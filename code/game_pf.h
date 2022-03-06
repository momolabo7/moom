// This file and game_gfx.h contain structs that need to be 
// initialized by the OS and passed to the main Game_Update() 
// function. 
#ifndef GAME_PLATFORM_H
#define GAME_PLATFORM_H

//~Platform File API
enum PF_File_Path {
  PF_FILE_PATH_EXE,
  PF_FILE_PATH_USER,
  PF_FILE_PATH_CACHE,
  
};

// Maybe for 'overwrite' or creating a new file, we 
// use a compl
enum PF_File_Access {
  PF_FILE_ACCESS_READ,
  PF_FILE_ACCESS_OVERWRITE,
};

struct PF_File {
  B32 error;
  void* platform_data; // pointer for platform's usage
};
typedef PF_File  PF_Open_File_Fn(const char* filename,
                                 PF_File_Access file_access,
                                 PF_File_Path file_path);

typedef void PF_Close_File_Fn(PF_File* file);
typedef void PF_Read_File_Fn(PF_File* file, UMI size, UMI offset, void* dest);
typedef void PF_Write_File_Fn(PF_File* file, UMI size, UMI offset, void* src);

//~Platform multithreaded work API
typedef void PF_Work_Callback_Fn(void* data);
struct PF_Work {
  PF_Work_Callback_Fn callback;
  void* data;
};
struct PF_Work_Queue {};
typedef void PF_Add_Work_Fn(PF_Work_Queue* wq, PF_Work_Callback_Fn callback, void* data);
typedef void PF_Complete_All_Work(PF_Work_Queue* wq);


//~Other platform API
typedef void  PF_Hot_Reload_Fn(); // trigger hot reloading of game code
typedef void  PF_Shutdown_Fn(); // trigger shutdown of application
typedef void* PF_Alloc_Fn(UMI size); // allocate memory
typedef void  PF_Free_Fn(void* ptr);     // frees memory
typedef void  PF_Set_Aspect_Ratio_Fn(U32 width, U32 height); // sets aspect ratio of game



struct Platform_API {
  PF_Hot_Reload_Fn* hot_reload;
  PF_Shutdown_Fn* shutdown;
  PF_Alloc_Fn* alloc;
  PF_Free_Fn* free;
  PF_Set_Aspect_Ratio_Fn* set_aspect_ratio;
  
  PF_Open_File_Fn* open_file;
  PF_Read_File_Fn* read_file;
  PF_Write_File_Fn* write_file;
  PF_Close_File_Fn* close_file;
};



//~ NOTE(Momo): Input API
// TODO(Momo): Perhaps we want something a lot more robust?
// But that will probably require some insane enum
// Or maybe we can put all these into Platform API?

struct Input_Button {
  B32 before;
  B32 now; 
};

static B32 is_poked(Input_Button) ;
static B32 is_released(Input_Button);
static B32 is_down(Input_Button);
static B32 is_held(Input_Button);

struct Input {
  Input_Button buttons[4];
  struct {
    Input_Button button_up;
    Input_Button button_down;
    Input_Button button_left;
    Input_Button button_right;
  };  
  
  V2 design_mouse_pos;
  V2U screen_mouse_pos;
  V2U render_mouse_pos;
  
};

void update(Input_Button button);




//~ NOTE(Momo): Game API
// Returns true if game is done
struct Game {
  void* game_data; // pointer for game usage
};

typedef void Game_UpdateFn(Game* game_memory,
                           Platform_API* pf,
                           Input* input,
                           Gfx* gfx,
                           F32 dt);

// To be called by platform
struct Game_API {
  Game_UpdateFn* update;
};


#include "game_pf.cpp"

#endif //GAME_PLATFORM_H
