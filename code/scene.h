#ifndef SCENE_H
#define SCENE_H


//#include "scene_splash.h"
//#include "scene_computer.h"
#include "scene_lit.h"
//#include "scene_sandbox.h"

//#include "scene_splash.cpp"
//#include "scene_computer.cpp"
//#include "scene_lit.cpp"


// Must have this so that engine knows 
// what the 'entry point' scene is
scene_set_entry(lit_tick);


#endif
