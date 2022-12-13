
#ifndef MOE_H
#define MOE_H

#include "momo.h"
#include "karu.h"

// NOTE(Momo): These are 'interfaces'
#include "moe_platform.h"
#include "moe_gfx.h"

#define MOE_WIDTH 800.f
#define MOE_HEIGHT 800.f

#ifdef INTERNAL
static struct Profiler* profiler;
#define moe_log(...) platform->debug_log(__VA_ARGS__)
#define moe_profile_block(name) prf_block(platform->profiler, name)
#define moe_profile_begin(name) prf_begin_block(platform->profiler, name)
#define moe_profile_end(name) prf_end_block(platform->profiler, name)
#else
#define moe_log(...)
#define moe_profiler_block(...)
#define moe_profile_begin(...) 
#define moe_profile_end(...) 
#endif 

static Platform* platform;


#include "moe_profiler.h"
#include "moe_assets.h"
#include "moe_inspector.h"

static Assets* assets;
static Inspector* inspector;

#include "moe_console.h"



enum Moe_Show_Debug_Type {
  MOE_SHOW_DEBUG_NONE,
  MOE_SHOW_DEBUG_PROFILER,
  MOE_SHOW_DEBUG_CONSOLE,
  MOE_SHOW_DEBUG_INSPECTOR,
  
  MOE_SHOW_DEBUG_MAX
};

typedef void (*Scene_Tick)(struct Moe*);

typedef struct Moe {
  Moe_Show_Debug_Type show_debug_type;
  
  F32 design_width;
  F32 design_height;
    
  // Arenas
  Arena main_arena;

  // Sub arenas
  Arena asset_arena;
  Arena frame_arena;
  Arena debug_arena;
  Arena scene_arena;
  

  // Mode Management 
  B32 is_done;
  B32 is_scene_changed;
  void* scene_context;
  Scene_Tick scene_tick;
  //Moe_Mode_Type current_moe_scene;

  // Other stuff
  Assets assets;
  Console console;
  Inspector inspector;
  Profiler* profiler;

  // Interested moe assets
  Moe_Sprite_ID blank_sprite;
  Moe_Font_ID debug_font;
} Moe;


#include "moe_assets.cpp"
#include "moe_asset_rendering.h"
#include "moe_inspector_rendering.h"
#include "moe_profiler_rendering.h"
#include "moe_console.cpp"


static void 
moe_goto_scene(Moe* moe, Scene_Tick scene_tick) {
  moe->scene_tick = scene_tick;
  moe->is_scene_changed = true;
}

static B32
moe_is_scene_initialized(Moe* moe) {
  return moe->scene_context != null;
}

static void 
moe_set_design_dims(Moe* moe, F32 design_width, F32 design_height) {
  moe->design_width = design_width;
  moe->design_height = design_height;
}

static void*
_moe_allocate_scene_size(Moe* moe, UMI size) {
  arn_clear(&moe->scene_arena);
  moe->scene_context = arn_push_size(&moe->scene_arena, size, 16);
  return moe->scene_context;
}



#define moe_allocate_scene(t,g) (t*)_moe_allocate_scene_size(g,sizeof(t))

// some scene apis
#define scene_set_entry(entry) static Scene_Tick moe_entry_scene = (entry)
#include "scene.h"



#endif //MOE_H
