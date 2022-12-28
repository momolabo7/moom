
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
#define moe_log(...) moe->platform->debug_log(__VA_ARGS__)
#define moe_profile_block(name) profiler_block(moe->platform->profiler, name)
#define moe_profile_begin(name) profiler_begin_block(moe->platform->profiler, name)
#define moe_profile_end(name) profiler_end_block(moe->platform->profiler, name)
#else
#define moe_log(...)
#define moe_profiler_block(...)
#define moe_profile_begin(...) 
#define moe_profile_end(...) 
#endif 


#include "moe_profiler.h"
#include "moe_assets.h"
#include "moe_inspector.h"
#include "moe_console.h"



enum moe_show_debug_type_t {
  MOE_SHOW_DEBUG_NONE,
  MOE_SHOW_DEBUG_PROFILER,
  MOE_SHOW_DEBUG_CONSOLE,
  MOE_SHOW_DEBUG_INSPECTOR,
  
  MOE_SHOW_DEBUG_MAX
};

typedef void (*Scene_Tick)(struct moe_t*);

typedef struct moe_t {
  moe_show_debug_type_t show_debug_type;
  
   
  // Arenas
  arena_t main_arena;

  // Sub arenas
  arena_t asset_arena;
  arena_t frame_arena;
  arena_t debug_arena;
  arena_t scene_arena;
  

  // Mode Management 
  b32_t is_done;
  b32_t is_scene_changed;
  void* scene_context;
  Scene_Tick scene_tick;

  // Other stuff
  assets_t assets;
  console_t console;
  inspector_t inspector;
  Platform* platform;

  // Interested moe assets
  asset_sprite_id_t blank_sprite;
  asset_font_id_t debug_font;
} moe_t;


#include "moe_assets.cpp"
#include "moe_asset_rendering.h"
#include "moe_inspector_rendering.h"
#include "moe_profiler_rendering.h"
#include "moe_console.cpp"


static void 
moe_goto_scene(moe_t* moe, Scene_Tick scene_tick) {
  moe->scene_tick = scene_tick;
  moe->is_scene_changed = true;
}

static b32_t
moe_is_scene_initialized(moe_t* moe) {
  return moe->scene_context != nullptr;
}

static void*
_moe_allocate_scene_size(moe_t* moe, umi_t size) {
  arena_clear(&moe->scene_arena);
  moe->scene_context = arena_push_size(&moe->scene_arena, size, 16);
  return moe->scene_context;
}



#define moe_allocate_scene(t,g) (t*)_moe_allocate_scene_size(g,sizeof(t))

// some scene apis
#define scene_set_entry(entry) static Scene_Tick moe_entry_scene = (entry)
#include "scene.h"



#endif //MOE_H
