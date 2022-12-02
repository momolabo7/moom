/* date = March 11th 2022 6:38 pm */

#ifndef MOE_H
#define MOE_H

#include "momo.h"

// NOTE(Momo): These are 'interfaces'
#include "moe_platform.h"
#include "moe_gfx.h"

#define GAME_WIDTH 800.f
#define GAME_HEIGHT 800.f

#ifdef INTERNAL
static struct Profiler* profiler;
#define game_log(...) platform->debug_log(__VA_ARGS__)
#define game_profile_block(name) prf_block(profiler, name)
#define game_profile_begin(name) prf_begin_block(profiler, name)
#define game_profile_end(name) prf_end_block(profiler, name)
#else
#define game_log(...)
#define game_profiler_block(...)
#define game_profile_begin(...) 
#define game_profile_end(...) 
#endif 

static Platform* platform;
static Gfx* gfx;


#include "moe_profiler.h"
#include "game_assets.h"
#include "moe_inspector.h"

static Game_Assets* assets;
static Inspector* inspector;

#include "moe_asset_rendering.h"
#include "moe_inspector_rendering.h"
#include "moe_profiler_rendering.h"
#include "moe_console.h"



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
  GAME_MODE_TYPE_LIT,
  GAME_MODE_TYPE_COMPUTER,
  GAME_MODE_TYPE_SANDBOX,
};



struct Game {
  Game_Show_Debug_Type show_debug_type;

  F32 design_width;
  F32 design_height;
    
  // Arenas
  Arena asset_arena;
  Arena frame_arena;
  Arena debug_arena;
  Arena mode_arena;
  

  // Mode Management 
  B32 is_done;
  B32 is_mode_changed;
  void* mode_context;
  Game_Mode_Type current_game_mode;

  // Other stuff
  Game_Assets assets;
  Console console;
  Inspector inspector;
  Profiler profiler;

  // Interested game assets
  Game_Sprite_ID blank_sprite;
  Game_Font_ID debug_font;
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

static void 
game_set_design_dims(Game* game, F32 design_width, F32 design_height) {
  game->design_width = design_width;
  game->design_height = design_height;
}

static void*
game_allocate_mode_size(Game* game, UMI size) {
  arn_clear(&game->mode_arena);
  game->mode_context = arn_push_size(&game->mode_arena, size, 16);
  return game->mode_context;
}

#define game_allocate_mode(t,g) (t*)game_allocate_mode_size(g,sizeof(t))


#include "game_mode_splash.h"
#include "game_mode_lit.h"
#include "game_mode_computer.h"
#include "game_mode_sandbox.h"

typedef void (*Game_Mode_Tick)(Game*);
static Game_Mode_Tick game_modes[] = {
  splash_tick,
  lit_tick,
  computer_tick,
  sandbox_tick,
};


#endif //GAME_H
