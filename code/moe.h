/* date = March 11th 2022 6:38 pm */

#ifndef MOE_H
#define MOE_H

#include "momo.h"

// NOTE(Momo): These are 'interfaces'
#include "moe_platform.h"
#include "moe_gfx.h"

#define MOE_WIDTH 800.f
#define MOE_HEIGHT 800.f

#ifdef INTERNAL
static struct Profiler* profiler;
#define moe_log(...) platform->debug_log(__VA_ARGS__)
#define moe_profile_block(name) prf_block(profiler, name)
#define moe_profile_begin(name) prf_begin_block(profiler, name)
#define moe_profile_end(name) prf_end_block(profiler, name)
#else
#define moe_log(...)
#define moe_profiler_block(...)
#define moe_profile_begin(...) 
#define moe_profile_end(...) 
#endif 

static Platform* platform;
static Gfx* gfx;


#include "moe_profiler.h"
#include "moe_assets.h"
#include "moe_inspector.h"

static Moe_Assets* assets;
static Inspector* inspector;

#include "moe_asset_rendering.h"
#include "moe_inspector_rendering.h"
#include "moe_profiler_rendering.h"
#include "moe_console.h"



enum Moe_Show_Debug_Type {
  MOE_SHOW_DEBUG_NONE,
  MOE_SHOW_DEBUG_PROFILER,
  MOE_SHOW_DEBUG_CONSOLE,
  MOE_SHOW_DEBUG_INSPECTOR,
  
  MOE_SHOW_DEBUG_MAX
};

/////////////////////////////////////////////////////////////////////////////
// Game Modes
enum Moe_Mode_Type {
  MOE_MODE_TYPE_SPLASH,
  MOE_MODE_TYPE_LIT,
  MOE_MODE_TYPE_COMPUTER,
  MOE_MODE_TYPE_SANDBOX,
};



struct Game {
  Moe_Show_Debug_Type show_debug_type;

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
  Moe_Mode_Type current_moe_mode;

  // Other stuff
  Moe_Assets assets;
  Console console;
  Inspector inspector;
  Profiler profiler;

  // Interested moe assets
  Moe_Sprite_ID blank_sprite;
  Moe_Font_ID debug_font;
};

static void 
moe_goto_mode(Game* moe, Moe_Mode_Type type) {
  moe->current_moe_mode = type;
  moe->is_mode_changed = true;
}

static B32
moe_mode_initialized(Game* moe) {
  return moe->mode_context != null;
}

static void 
moe_set_design_dims(Game* moe, F32 design_width, F32 design_height) {
  moe->design_width = design_width;
  moe->design_height = design_height;
}

static void*
moe_allocate_mode_size(Game* moe, UMI size) {
  arn_clear(&moe->mode_arena);
  moe->mode_context = arn_push_size(&moe->mode_arena, size, 16);
  return moe->mode_context;
}

#define moe_allocate_mode(t,g) (t*)moe_allocate_mode_size(g,sizeof(t))


#include "moe_mode_splash.h"
#include "moe_mode_lit.h"
#include "moe_mode_computer.h"
#include "moe_mode_sandbox.h"

typedef void (*Moe_Mode_Tick)(Game*);
static Moe_Mode_Tick moe_modes[] = {
  splash_tick,
  lit_tick,
  computer_tick,
  sandbox_tick,
};


#endif //MOE_H
