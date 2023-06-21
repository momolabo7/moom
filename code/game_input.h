#ifndef GAME_INPUT_H
#define GAME_INPUT_H


//
// Input related API
//
struct input_button_t {
  b32_t before;
  b32_t now; 
};


// my god
enum input_button_code_t {
  // Keyboard keys
  INPUT_BUTTON_CODE_UNKNOWN,
  INPUT_BUTTON_CODE_0,
  INPUT_BUTTON_CODE_1,
  INPUT_BUTTON_CODE_2,
  INPUT_BUTTON_CODE_3,
  INPUT_BUTTON_CODE_4,
  INPUT_BUTTON_CODE_5,
  INPUT_BUTTON_CODE_6,
  INPUT_BUTTON_CODE_7,
  INPUT_BUTTON_CODE_8,
  INPUT_BUTTON_CODE_9,
  INPUT_BUTTON_CODE_F1,
  INPUT_BUTTON_CODE_F2,
  INPUT_BUTTON_CODE_F3,
  INPUT_BUTTON_CODE_F4,
  INPUT_BUTTON_CODE_F5,
  INPUT_BUTTON_CODE_F6,
  INPUT_BUTTON_CODE_F7,
  INPUT_BUTTON_CODE_F8,
  INPUT_BUTTON_CODE_F9,
  INPUT_BUTTON_CODE_F10,
  INPUT_BUTTON_CODE_F11,
  INPUT_BUTTON_CODE_F12,
  INPUT_BUTTON_CODE_BACKSPACE,
  INPUT_BUTTON_CODE_A,
  INPUT_BUTTON_CODE_B,
  INPUT_BUTTON_CODE_C,
  INPUT_BUTTON_CODE_D,
  INPUT_BUTTON_CODE_E,
  INPUT_BUTTON_CODE_F,
  INPUT_BUTTON_CODE_G,
  INPUT_BUTTON_CODE_H,
  INPUT_BUTTON_CODE_I,
  INPUT_BUTTON_CODE_J,
  INPUT_BUTTON_CODE_K,
  INPUT_BUTTON_CODE_L,
  INPUT_BUTTON_CODE_M,
  INPUT_BUTTON_CODE_N,
  INPUT_BUTTON_CODE_O,
  INPUT_BUTTON_CODE_P,
  INPUT_BUTTON_CODE_Q,
  INPUT_BUTTON_CODE_R,
  INPUT_BUTTON_CODE_S,
  INPUT_BUTTON_CODE_T,
  INPUT_BUTTON_CODE_U,
  INPUT_BUTTON_CODE_V,
  INPUT_BUTTON_CODE_W,
  INPUT_BUTTON_CODE_X,
  INPUT_BUTTON_CODE_Y,
  INPUT_BUTTON_CODE_Z,
  INPUT_BUTTON_CODE_SPACE,
  INPUT_BUTTON_CODE_RMB,
  INPUT_BUTTON_CODE_LMB,
  INPUT_BUTTON_CODE_MMB,

  INPUT_BUTTON_CODE_MAX,

};

// NOTE(momo): Input is SPECIFICALLY stuff that can be recorded and
// replayed by some kind of system. Other things go to game_t
struct input_t {
  input_button_t buttons[INPUT_BUTTON_CODE_MAX];
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


// before: 0, now: 1
static b32_t 
is_poked(input_button_t btn) {
  return !btn.before && btn.now;
}

// before: 1, now: 0
static b32_t
is_released(input_button_t btn) {
  return btn.before && !btn.now;
}

// before: X, now: 1
static b32_t
is_down(input_button_t btn){
  return btn.now;
}

// before: 1, now: 1
static b32_t
is_held(input_button_t btn) {
  return btn.before && btn.now;
}

#endif
