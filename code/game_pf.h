// This file and game_gfx.h contain structs that need to be 
// initialized by the OS and passed to the main Game_Update() 
// function. 
#ifndef GAME_PLATFORM_H
#define GAME_PLATFORM_H


//~NOTE(Momo): Platform API
typedef void  Platform_HotReloadFn(); // trigger hot reloading of game code
typedef void  Platform_ShutdownFn(); // trigger shutdown of application
typedef void* Platform_AllocFn(UMI size); // allocate memory
typedef void  Platform_FreeFn(void* ptr);     // frees memory
typedef void  Platform_SetAspectRatioFn(U32 width, U32 height); // sets aspect ratio of game

struct Platform {
  Platform_HotReloadFn* hot_reload;
  Platform_ShutdownFn* shutdown;
  Platform_AllocFn* alloc;
  Platform_FreeFn* free;
  Platform_SetAspectRatioFn* set_aspect_ratio;
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
  void* memory;
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
