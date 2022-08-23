/* date = March 11th 2022 6:38 pm */

#ifndef GAME_H
#define GAME_H

#include "momo.h"

// NOTE(Momo): These are 'interfaces'
#include "game_platform.h"
#include "game_gfx.h"


#ifdef INTERNAL
Profiler* g_profiler;
Platform* g_platform;
#define game_log(...) g_platform->debug_log(__VA_ARGS__)
#define game_profile_block(...) prf_block(g_profiler, __VA_ARGS__)
#else
#define game_log(...)
#define game_profiler_block(...)
#endif 



#include "game_profiler.h"
#include "game_assets.h"
#include "game_painter.h"
#include "game_inspector.h"

#include "game_inspector_rendering.h"
#include "game_profiler_rendering.h"
#include "game_console.h"
//////////////////////////////////////////////////////
// GAME
enum Game_Show_Debug_Type {
  GAME_SHOW_DEBUG_NONE,
  GAME_SHOW_DEBUG_PROFILER,
  GAME_SHOW_DEBUG_CONSOLE,
  GAME_SHOW_DEBUG_INSPECTOR,
  
  GAME_SHOW_DEBUG_MAX
};

/////////////////////////////////////////////////////////////////////////////
// Game Modes
enum Game_Mode_Type {
  GAME_MODE_TYPE_SPLASH,
  GAME_MODE_TYPE_LIT
};



struct Game {
  Game_Show_Debug_Type show_debug_type;
    
  // Bump_Allocators
  Bump_Allocator asset_arena;
  Bump_Allocator frame_arena;
  Bump_Allocator debug_arena;
  Bump_Allocator mode_arena;
  

  // Mode Management 
  B32 is_done;
  B32 is_mode_changed;
  void* mode_context;
  Game_Mode_Type current_game_mode;

  // Other stuff
  Game_Assets game_assets;
  Console console;
  Inspector inspector;
};

static void 
game_goto_mode(Game* game, Game_Mode_Type type) {
  game->current_game_mode = type;
  game->is_mode_changed = true;
}

static B32
game_mode_initialized(Game* game) {
  return game->mode_context != null;
}


static void*
game_allocate_mode_size(Game* game, UMI size) {
  ba_clear(&game->mode_arena);
  game->mode_context = ba_push_size(&game->mode_arena, size, 16);
  return game->mode_context;
}

#define game_allocate_mode(t,g) (t*)game_allocate_mode_size(g,sizeof(t))


#include "game_mode_splash.h"
#include "game_mode_sb1.h"

typedef void (*Game_Mode_Tick)(Game*, Painter*, Platform*);
static Game_Mode_Tick game_modes[] = {
  splash_tick,
  lit_tick,
};


#endif //GAME_H
