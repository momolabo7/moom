
#ifndef GAME_PLATFORM_H
#define GAME_PLATFORM_H
// NOTE(Momo): It might be better to have some of these 'included' in compilation?
// Just like how stdlib works




//~ NOTE(Momo): Input API
// TODO(Momo): Perhaps we want something a lot more robust?
// But that will probably require some insane enum

typedef struct {
  B8 before;
  B8 now;
} PF_Button;

typedef struct {
  PF_Button buttons[4];
  struct {
    PF_Button button_up;
    PF_Button button_down;
    PF_Button button_left;
    PF_Button button_right;
  };  
  
  V2F32 design_mouse_pos;
  V2U32 screen_mouse_pos;
  V2U32 render_mouse_pos;
  
} PF_Input;

static void 
PF_Input_Update(PF_Input* in) {
  for (U32 i = 0; i < ArrayCount(in->buttons); ++i) {
    in->buttons[i].before = in->buttons[i].now;
  }
}



// before: 0, now: 1
static inline B32 
PF_Button_IsPoked(PF_Button b) {
  return !b.before && b.now;
}

// before: 1, now: 0
static inline B32
PF_Button_IsReleased(PF_Button b) {
  return b.before && !b.now;
}


// before: X, now: 1
static inline B32
PF_Button_IsDown(PF_Button b) {
  return b.now;
}

// before: 1, now: 1
static inline B32
PF_Button_IsHeld(PF_Button b) {
  return b.before && b.now;
}

//~ NOTE(Momo): game.dll API
// Returns true if game is done
typedef B32 Game_UpdateFn(Gfx* gfx,
                          F32 dt);

#endif //GAME_PLATFORM_H
