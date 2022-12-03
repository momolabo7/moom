#ifndef SCENE_H
#define SCENE_H

//#include "scene_splash.h"
//#include "scene_computer.h"
#include "scene_lit.h"
//#include "scene_sandbox.h"

//#include "scene_splash.cpp"
//#include "scene_computer.cpp"
//#include "scene_lit.cpp"


// NOTE(momo): Is this terrible?
// It's kind of like allowing Scene layer to pick what the entry scene is. 
// Maybe we can think of a better API.
static Scene_Tick entry_scene_tick = lit_tick;

#endif
