//- Implementation

static B32 IsOk(Platform_File* file) {
  return file->platform_data && !file->error;
}

static void Update(Input* input) {
  for (U32 i = 0; i < ArrayCount(input->buttons); ++i) {
    input->buttons[i].before = input->buttons[i].now;
  }
}



// before: 0, now: 1
static B32 
IsPoked(Input_Button b) {
  return !b.before && b.now;
}

// before: 1, now: 0
static B32
IsReleased(Input_Button b) {
  return b.before && !b.now;
}


// before: X, now: 1
static B32
IsDown(Input_Button b){
  return b.now;
}

// before: 1, now: 1
static B32
IsHeld(Input_Button b) {
  return b.before && b.now;
}
