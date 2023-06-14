
#ifndef MOMO_GAME_H
#define MOMO_GAME_H

#include "game_platform.h"
#include "game_gfx.h"

//
struct game_audio_buffer_t {
  s16_t* sample_buffer;
  u32_t sample_count;
  u32_t channels; //TODO: remove this?
};


//
// Input related API
//
struct game_button_t {
  b32_t before;
  b32_t now; 
};


// my god
enum game_button_code_t {
  // Keyboard keys
  GAME_BUTTON_CODE_UNKNOWN,
  GAME_BUTTON_CODE_0,
  GAME_BUTTON_CODE_1,
  GAME_BUTTON_CODE_2,
  GAME_BUTTON_CODE_3,
  GAME_BUTTON_CODE_4,
  GAME_BUTTON_CODE_5,
  GAME_BUTTON_CODE_6,
  GAME_BUTTON_CODE_7,
  GAME_BUTTON_CODE_8,
  GAME_BUTTON_CODE_9,
  GAME_BUTTON_CODE_F1,
  GAME_BUTTON_CODE_F2,
  GAME_BUTTON_CODE_F3,
  GAME_BUTTON_CODE_F4,
  GAME_BUTTON_CODE_F5,
  GAME_BUTTON_CODE_F6,
  GAME_BUTTON_CODE_F7,
  GAME_BUTTON_CODE_F8,
  GAME_BUTTON_CODE_F9,
  GAME_BUTTON_CODE_F10,
  GAME_BUTTON_CODE_F11,
  GAME_BUTTON_CODE_F12,
  GAME_BUTTON_CODE_BACKSPACE,
  GAME_BUTTON_CODE_A,
  GAME_BUTTON_CODE_B,
  GAME_BUTTON_CODE_C,
  GAME_BUTTON_CODE_D,
  GAME_BUTTON_CODE_E,
  GAME_BUTTON_CODE_F,
  GAME_BUTTON_CODE_G,
  GAME_BUTTON_CODE_H,
  GAME_BUTTON_CODE_I,
  GAME_BUTTON_CODE_J,
  GAME_BUTTON_CODE_K,
  GAME_BUTTON_CODE_L,
  GAME_BUTTON_CODE_M,
  GAME_BUTTON_CODE_N,
  GAME_BUTTON_CODE_O,
  GAME_BUTTON_CODE_P,
  GAME_BUTTON_CODE_Q,
  GAME_BUTTON_CODE_R,
  GAME_BUTTON_CODE_S,
  GAME_BUTTON_CODE_T,
  GAME_BUTTON_CODE_U,
  GAME_BUTTON_CODE_V,
  GAME_BUTTON_CODE_W,
  GAME_BUTTON_CODE_X,
  GAME_BUTTON_CODE_Y,
  GAME_BUTTON_CODE_Z,
  GAME_BUTTON_CODE_SPACE,
  GAME_BUTTON_CODE_RMB,
  GAME_BUTTON_CODE_LMB,
  GAME_BUTTON_CODE_MMB,

  GAME_BUTTON_CODE_MAX,

};

// NOTE(momo): Input is SPECIFICALLY stuff that can be recorded and
// replayed by some kind of system. Other things go to game_t
struct input_t{
  game_button_t buttons[GAME_BUTTON_CODE_MAX];
  u8_t chars[32];
  u32_t char_count;

  // NOTE(Momo): The mouse position is relative to the moe's dimensions given
  // via set_design_dims(). It is possible to get back the normalized dimensions
  // by dividing the x/y by the width/height of the moe.
  v2f_t mouse_pos;

  // NOTE(Momo): Mouse wheels values are not normally analog!
  // +ve is forwards
  // -ve is backwards
  s32_t mouse_scroll_delta;


  // TODO(Momo): not sure if this should even be here
  f32_t delta_time; //aka dt

};

// 
// Platform API
//
struct gfx_t;
struct profiler_t;

struct game_platform_config_t {
  usz_t texture_queue_size;
  usz_t render_command_size;

  // must be null terminated
  const char* window_title; // TODO: change to str8_t?
};

struct game_t {
  pf_t platform;

  void* context;
          
  b32_t is_dll_reloaded;
  b32_t is_running;
};

typedef game_platform_config_t game_get_platform_config_f(void); 
typedef void game_update_and_render_f(
    game_t*, 
    gfx_t*, 
    game_audio_buffer_t*, 
    profiler_t*, 
    input_t*);

// To be called by platform
struct game_functions_t {
  game_get_platform_config_f* get_platform_config;
  game_update_and_render_f* update_and_render;
};

static const char* game_function_names[] {
  "game_get_platform_config",
  "game_update_and_render",
};


// before: 0, now: 1
static b32_t 
is_poked(game_button_t btn) {
  return !btn.before && btn.now;
}

// before: 1, now: 0
static b32_t
is_released(game_button_t btn) {
  return btn.before && !btn.now;
}

// before: X, now: 1
static b32_t
is_down(game_button_t btn){
  return btn.now;
}

// before: 1, now: 1
static b32_t
is_held(game_button_t btn) {
  return btn.before && btn.now;
}


#include "game_console.h"
#include "game_asset_file.h"
#include "game_assets.h"
#include "game_asset_rendering.h"

#endif //GAME_H
