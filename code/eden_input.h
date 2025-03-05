
// @note: Input is SPECIFICALLY stuff that can be recorded and
// replayed by some kind of system. Other things go to eden_t
// 


struct eden_button_t {
  b32_t before : 1;
  b32_t now: 1; 
};

enum eden_button_code_t {
  // my god
  // keyboard keys
  EDEN_BUTTON_CODE_UNKNOWN,
  EDEN_BUTTON_CODE_0,
  EDEN_BUTTON_CODE_1,
  EDEN_BUTTON_CODE_2,
  EDEN_BUTTON_CODE_3,
  EDEN_BUTTON_CODE_4,
  EDEN_BUTTON_CODE_5,
  EDEN_BUTTON_CODE_6,
  EDEN_BUTTON_CODE_7,
  EDEN_BUTTON_CODE_8,
  EDEN_BUTTON_CODE_9,
  EDEN_BUTTON_CODE_F1,
  EDEN_BUTTON_CODE_F2,
  EDEN_BUTTON_CODE_F3,
  EDEN_BUTTON_CODE_F4,
  EDEN_BUTTON_CODE_F5,
  EDEN_BUTTON_CODE_F6,
  EDEN_BUTTON_CODE_F7,
  EDEN_BUTTON_CODE_F8,
  EDEN_BUTTON_CODE_F9,
  EDEN_BUTTON_CODE_F10,
  EDEN_BUTTON_CODE_F11,
  EDEN_BUTTON_CODE_F12,
  EDEN_BUTTON_CODE_BACKSPACE,
  EDEN_BUTTON_CODE_A,
  EDEN_BUTTON_CODE_B,
  EDEN_BUTTON_CODE_C,
  EDEN_BUTTON_CODE_D,
  EDEN_BUTTON_CODE_E,
  EDEN_BUTTON_CODE_F,
  EDEN_BUTTON_CODE_G,
  EDEN_BUTTON_CODE_H,
  EDEN_BUTTON_CODE_I,
  EDEN_BUTTON_CODE_J,
  EDEN_BUTTON_CODE_K,
  EDEN_BUTTON_CODE_L,
  EDEN_BUTTON_CODE_M,
  EDEN_BUTTON_CODE_N,
  EDEN_BUTTON_CODE_O,
  EDEN_BUTTON_CODE_P,
  EDEN_BUTTON_CODE_Q,
  EDEN_BUTTON_CODE_R,
  EDEN_BUTTON_CODE_S,
  EDEN_BUTTON_CODE_T,
  EDEN_BUTTON_CODE_U,
  EDEN_BUTTON_CODE_V,
  EDEN_BUTTON_CODE_W,
  EDEN_BUTTON_CODE_X,
  EDEN_BUTTON_CODE_Y,
  EDEN_BUTTON_CODE_Z,
  EDEN_BUTTON_CODE_SPACE,
  EDEN_BUTTON_CODE_RMB,
  EDEN_BUTTON_CODE_LMB,
  EDEN_BUTTON_CODE_MMB,

  EDEN_BUTTON_CODE_LEFT,
  EDEN_BUTTON_CODE_RIGHT,
  EDEN_BUTTON_CODE_UP,
  EDEN_BUTTON_CODE_DOWN,

  EDEN_BUTTON_CODE_MAX,

};


struct eden_input_characters_t {
  u8_t* data;
  u32_t count;
};

struct eden_input_t {
  eden_button_t buttons[EDEN_BUTTON_CODE_MAX];
  u8_t chars[32];
  u32_t char_count;

  // @note: The mouse position is relative to the moe's dimensions given
  // via set_design_dims(). It is possible to get back the normalized dimensions
  // by dividing the x/y by the width/height of the moe.
  v2f_t mouse_pos;

  // @note: Mouse wheels values are not normally analog!
  // +ve is forwards
  // -ve is backwards
  s32_t mouse_scroll_delta;

  // @todo(Momo): not sure if this should even be here
  f32_t delta_time; //aka dt
};
