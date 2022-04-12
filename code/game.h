/* date = March 11th 2022 6:38 pm */

#ifndef GAME_H
#define GAME_H

// NOTE(Momo): These are 'interfaces'
#include "game_platform.h"
#include "game_debugger.h"
#include "game_renderer.h"

#ifdef INTERNAL
#define game_log(...) g_platform.debug_log(__VA_ARGS__)
#else
#define game_log(...)
#endif // INTERNAL

#include "game_profiler.h"

#include "game_assets.h"
#include "game_assets_rendering.h"
#include "game_profiler_rendering.h"

#include "game_console.h"


struct Sandbox_Mode {
  F32 tmp_delta;
  B32 tmp_increase;
  F32 tmp_rot;
  
};

struct Game_State {
  // Arenas
  Arena asset_arena;
  Arena frame_arena;
  Arena debug_arena;
  
  // Mode Management
  union {
    Sandbox_Mode sandbox_mode;  
  };
  Game_Assets game_assets;
  Console console;
};



#endif //GAME_H
