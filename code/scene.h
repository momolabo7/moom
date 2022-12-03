#ifndef SCENE_H
#define SCENE_H

//#include "scene_splash.h"
//#include "scene_computer.h"
#include "scene_lit.h"
//#include "scene_sandbox.h"

//#include "scene_splash.cpp"
//#include "scene_computer.cpp"
//#include "scene_lit.cpp"

#if 0

static Moe_Mode_Tick moe_modes[] = {
  splash_tick,
  lit_tick,
  computer_tick,
  sandbox_tick,
};
#endif


// TODO: Is this terrible?
static Scene_Tick first_scene_tick = lit_tick;

#endif
