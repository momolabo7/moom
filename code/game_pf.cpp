//- Implementation

static B32
is_ok(PF_File* file) {
  return file->platform_data && !file->error;
}


static void 
update(Game_Input* input) {
  for (U32 i = 0; i < array_count(input->buttons); ++i) {
    input->buttons[i].before = input->buttons[i].now;
  }
}



// before: 0, now: 1
static B32 
is_poked(Game_Input_Button btn) {
  return !btn.before && btn.now;
}

// before: 1, now: 0
static B32
is_released(Game_Input_Button btn) {
  return btn.before && !btn.now;
}


// before: X, now: 1
static B32
is_down(Game_Input_Button btn){
  return btn.now;
}

// before: 1, now: 1
static B32
is_held(Game_Input_Button btn) {
  return btn.before && btn.now;
}
