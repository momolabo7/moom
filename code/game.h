/* date = March 11th 2022 6:38 pm */

#ifndef GAME_H
#define GAME_H

#include "momo.h"

// NOTE(Momo): These are 'interfaces'
#include "game_platform.h"
#include "game_renderer.h"

Platform* platform;

#ifdef INTERNAL
#define game_log(...) g_platform->debug_log(__VA_ARGS__)
#define game_profile_block(...) prf_block(g_platform->profiler, __VA_ARGS__)
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

// Main game structure
typedef void Game_Mode_Init(struct Game*);
typedef void Game_Mode_Update(struct Game*, Painter*, Platform*);

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
  Game_Mode_Init* init_mode;
  Game_Mode_Update* update_mode;
  void* mode_context;
  
  // Other stuff
  Game_Assets game_assets;
  Console console;
  Inspector inspector;
};

static void
game_set_mode(Game* game, Game_Mode_Init* init, Game_Mode_Update* update) 
{
  game->init_mode = init;
  game->update_mode = update;
  game->is_mode_changed = true;
}



template<typename T>
static T*
game_allocate_mode(Game* game) {
  ba_clear(&game->mode_arena);
  game->mode_context = ba_push<T>(&game->mode_arena, 4);
  return (T*)game->mode_context;
}
#endif //GAME_H
