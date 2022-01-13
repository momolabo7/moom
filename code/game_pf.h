// This file and game_gfx.h contain structs that need to be 
// initialized by the OS and passed to the main Game_Update() 
// function. 
#ifndef GAME_PLATFORM_H
#define GAME_PLATFORM_H


//~Platform API
typedef void  Platform_HotReloadFn(); // trigger hot reloading of game code
typedef void  Platform_ShutdownFn(); // trigger shutdown of application
typedef U8*   Platform_AllocFn(UMI size); // allocate memory
typedef void  Platform_FreeFn(void* ptr);     // frees memory
typedef void  Platform_SetAspectRatioFn(U32 width, U32 height); // sets aspect ratio of game

//-Platform File API
struct Platform_File {
  void* platform_data; // pointer for platform's usage
};
static B32 IsValid(Platform_File);

enum Platform_FilePath {
  Platform_FilePath_Executable,
  Platform_FilePath_UserData,
  Platform_FilePath_Cache
};
enum Platform_FileAccess {
  Platform_FileAccess_Read,
  Platform_FileAccess_Write,
  Platform_FileAccess_ReadWrite,
};
// TODO(Momo): Get file path function?


typedef Platform_File  Platform_OpenFileFn(const char* filename,
                                           Platform_FileAccess file_access,
                                           Platform_FilePath file_path);

typedef void				   Platform_CloseFileFn(Platform_File* file);
typedef B32 					 Platform_ReadFileFn(Platform_File* file, UMI size, UMI offset, U8* dest);


struct Platform {
  Platform_HotReloadFn* hot_reload;
  Platform_ShutdownFn* shutdown;
  Platform_AllocFn* alloc;
  Platform_FreeFn* free;
  Platform_SetAspectRatioFn* set_aspect_ratio;
  
  Platform_OpenFileFn* open_file;
  Platform_ReadFileFn* read_file;
  Platform_CloseFileFn* close_file;
};



//~ NOTE(Momo): Input API
// TODO(Momo): Perhaps we want something a lot more robust?
// But that will probably require some insane enum
// Or maybe we can put all these into Platform API?

struct Input_Button{
  B32 before;
  B32 now;
};

static B32 IsPoked(Input_Button) ;
static B32 IsReleased(Input_Button);
static B32 IsDown(Input_Button);
static B32 IsHeld(Input_Button);

struct Input{
  Input_Button buttons[4];
  struct {
    Input_Button button_up;
    Input_Button button_down;
    Input_Button button_left;
    Input_Button button_right;
  };  
  
  V2F32 design_mouse_pos;
  V2U32 screen_mouse_pos;
  V2U32 render_mouse_pos;
  
};

static void Update(Input* input);



//~ NOTE(Momo): Game API
// Returns true if game is donew
struct Game {
  void* game_data; // pointer for game usage
};

typedef void Game_UpdateFn(Game* game_memory,
                           Platform* pf,
                           Input* input,
                           Gfx* gfx,
                           F32 dt);

// To be called by platform
struct Game_API {
  Game_UpdateFn* update;
};


#include "game_pf.cpp"

#endif //GAME_PLATFORM_H
