//- Implementation

static B32
is_ok(PF_File* file) {
  return file->platform_data && !file->error;
}


static void 
update(PF_Input* input) {
  for (U32 i = 0; i < array_count(input->buttons); ++i) {
    input->buttons[i].before = input->buttons[i].now;
  }
}



// before: 0, now: 1
static B32 
is_poked(PF_Input_Button btn) {
  return !btn.before && btn.now;
}

// before: 1, now: 0
static B32
is_released(PF_Input_Button btn) {
  return btn.before && !btn.now;
}


// before: X, now: 1
static B32
is_down(PF_Input_Button btn){
  return btn.now;
}

// before: 1, now: 1
static B32
is_held(PF_Input_Button btn) {
  return btn.before && btn.now;
}
