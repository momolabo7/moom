static void Update(Input* input) {
  for (U32 i = 0; i < ArrayCount(input->buttons); ++i) {
    input->buttons[i].before = input->buttons[i].now;
  }
}



// before: 0, now: 1
static B32 
IsPoked(InputButton b) {
  return !b.before && b.now;
}

// before: 1, now: 0
static B32
IsReleased(InputButton b) {
  return b.before && !b.now;
}


// before: X, now: 1
static B32
IsDown(InputButton b){
  return b.now;
}

// before: 1, now: 1
static B32
IsHeld(InputButton b) {
  return b.before && b.now;
}
