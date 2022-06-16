/* date = March 11th 2022 6:38 pm */

#ifndef GAME_H
#define GAME_H

// NOTE(Momo): These are 'interfaces'
#include "game_platform.h"
#include "game_renderer.h"

#ifdef INTERNAL
#define game_log(...) g_platform.debug_log(__VA_ARGS__)
#else
#define game_log(...)
#endif // INTERNAL

#include "game_profiler.h"
#include "game_assets.h"
#include "game_painter.h"

#include "game_profiler_rendering.h"
#include "game_inspector.h"
#include "game_console.h"
#include "game_mode_level.h"

struct Splash_Mode {
  F32 timer;
};

enum Show_Debug_Type {
  SHOW_DEBUG_NONE,
  SHOW_DEBUG_PROFILER,
  SHOW_DEBUG_CONSOLE,
  SHOW_DEBUG_INSPECTOR,
  
  SHOW_DEBUG_MAX
};

enum Game_Mode {
  GAME_MODE_NONE, 
  
  GAME_MODE_SPLASH,
  GAME_MODE_LEVEL,
};

struct Game_State {
  Show_Debug_Type show_debug_type;
  
  // Memory_Pools
  Memory_Pool asset_arena;
  Memory_Pool frame_arena;
  Memory_Pool debug_arena;
  
  // Mode Management
  enum Game_Mode current_mode;
  enum Game_Mode next_mode;
  union {
    Splash_Mode splash_mode;
    Level_Mode level_mode;  
  };
  
  
  // Other stuff
  Game_Assets game_assets;
  Console console;
  Painter painter;
  Inspector inspector;
};

#include "game_mode_splash.h"
#include "game_mode_level.cpp"


#endif //GAME_H
