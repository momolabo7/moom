//- Implementation

static 
B32 is_ok(Platform_File* file) {
  return file->platform_data && !file->error;
}


void 
Input::update() {
  for (U32 i = 0; i < ArrayCount(buttons); ++i) {
    buttons[i].before = buttons[i].now;
  }
}



// before: 0, now: 1
B32 
Input_Button::is_poked() {
  return !before && now;
}

// before: 1, now: 0
B32
Input_Button::is_released() {
  return before && !now;
}


// before: X, now: 1
B32
Input_Button::is_down(){
  return now;
}

// before: 1, now: 1
B32
Input_Button::is_held() {
  return before && now;
}
