/* date = March 11th 2022 6:38 pm */

#ifndef GAME_H
#define GAME_H

#include "game_platform.h"

Platform_API platform;
#ifdef INTERNAL
#define game_log(...) platform.debug_log(__VA_ARGS__)
#else
#define game_log(...)
#endif // INTERNAL

#include "game_renderer.h"
#include "game_assets.h"
#include "game_debug.h"

struct Sandbox_Mode {
  F32 tmp_delta;
  B32 tmp_increase;
  F32 tmp_rot;
  
};


struct Game_State {
  union {
    Sandbox_Mode sandbox_mode;  
  };
  Game_Assets game_assets;
  Debug_Console debug_console;
};



#endif //GAME_H
