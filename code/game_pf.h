// This file and game_gfx.h contain structs that need to be 
// initialized by the OS and passed to the main Game_Update() 
// function. 


#ifndef GAME_PLATFORM_H
#define GAME_PLATFORM_H


//~NOTE(Momo): Platform API
typedef void PF_HotReloadFn(); // trigger hot reloading of game code
typedef void* PF_AllocFn(UMI size); // allocate memory
typedef void PF_FreeFn(void* ptr);     // frees memory

typedef struct {
  PF_HotReloadFn* hot_reload;
  PF_AllocFn* alloc;
  PF_FreeFn* free;
} PF;



//~ NOTE(Momo): Input API
// TODO(Momo): Perhaps we want something a lot more robust?
// But that will probably require some insane enum
// Or maybe we can put all these into Platform API?

typedef struct {
  B8 before;
  B8 now;
} Input_Button;

typedef struct {
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
  
} Input;

static void 
Input_Update(Input* in) {
  for (U32 i = 0; i < ArrayCount(in->buttons); ++i) {
    in->buttons[i].before = in->buttons[i].now;
  }
}



// before: 0, now: 1
static inline B32 
Input_IsPoked(Input_Button b) {
  return !b.before && b.now;
}

// before: 1, now: 0
static inline B32
Input_IsReleased(Input_Button b) {
  return b.before && !b.now;
}


// before: X, now: 1
static inline B32
Input_IsDown(Input_Button b) {
  return b.now;
}

// before: 1, now: 1
static inline B32
Input_IsHeld(Input_Button b) {
  return b.before && b.now;
}

//~ NOTE(Momo): Game API
// Returns true if game is done
typedef struct {
  U32 game_design_width;
  U32 game_design_height;
} Game_Info;

typedef Game_Info Game_GetInfoFn();

typedef B32 Game_UpdateFn(PF* pf,
                          Input* input,
                          Gfx* gfx,
                          F32 dt);

typedef struct {
  Game_GetInfoFn* get_info;
  Game_UpdateFn* update;
  
} Game_API;

#endif //GAME_PLATFORM_H
