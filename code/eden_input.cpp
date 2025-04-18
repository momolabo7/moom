
// before: 0, now: 1
static b32_t
eden_is_button_poked(eden_button_code_t code) {
  eden_input_t* in = &eden->input;
  auto btn = in->buttons[code];
  return !btn.before && btn.now;
}

// before: 1, now: 0
static b32_t
eden_is_button_released(eden_button_code_t code) {
  eden_input_t* in = &eden->input;
  auto btn = in->buttons[code];
  return btn.before && !btn.now;
}

// before: X, now: 1
static b32_t
eden_is_button_down(eden_button_code_t code){
  eden_input_t* in = &eden->input;
  return in->buttons[code].now;
}


// before: 1, now: 1
static b32_t
eden_is_button_held(eden_button_code_t code) {
  eden_input_t* in = &eden->input;
  auto btn = in->buttons[code];
  return btn.before && btn.now;
}

static b32_t
eden_is_dll_reloaded() {
  return eden->is_dll_reloaded;
}

static f32_t 
eden_get_dt() {
  return eden->input.delta_time;
}


static eden_input_characters_t
eden_get_input_characters() {
  eden_input_characters_t ret;
  ret.data = eden->input.chars;
  ret.count = eden->input.char_count; 

  return ret;
}
