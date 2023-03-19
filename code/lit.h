#ifndef MOE_MODE_LIT_H
#define MOE_MODE_LIT_H

#define LIT_DEBUG_INTERSECTIONS 0
#define LIT_DEBUG_COORDINATES 0

#define LIT_WIDTH  800.f
#define LIT_HEIGHT 800.f

#define LIT_SENSOR_PARTICLE_CD 0.1f
#define LIT_SENSOR_COLOR_MASK 0xFFFFFF00
#define LIT_SENSOR_RADIUS 8.f
#define LIT_SENSOR_PARTICLE_SIZE 14.f
#define LIT_SENSOR_PARTICLE_SPEED 20.f

#define LIT_EXIT_FLASH_DURATION 0.1f
#define LIT_EXIT_FLASH_BRIGHTNESS 0.6f

#define LIT_ENTER_DURATION 3.f

#define LIT_PLAYER_RADIUS 16.f
#define LIT_PLAYER_LIGHT_RETRIEVE_DURATION 0.05f
#define LIT_PLAYER_BREATH_DURATION 2.f
#define LIT_PLAYER_PICKUP_DIST 512.f
#define LIT_PLAYER_ROTATE_SPEED 1.f


#ifdef INTERNAL
#define lit_log(...) lit->platform->debug_log(__VA_ARGS__)
#define lit_profile_block(name) profiler_block(moe->platform->profiler, name)
#define lit_profile_begin(name) profiler_begin_block(moe->platform->profiler, name)
#define lit_profile_end(name) profiler_end_block(moe->platform->profiler, name)
#else
#define lit_log(...)
#define lit_profiler_block(...)
#define lit_profile_begin(...) 
#define lit_profile_end(...) 
#endif 



#include "lit_splash.h"
#include "lit_menu.h"
#include "lit_game.h"

enum lit_show_debug_type_t {
  LIT_SHOW_DEBUG_NONE,
  LIT_SHOW_DEBUG_PROFILER,
  LIT_SHOW_DEBUG_CONSOLE,
  LIT_SHOW_DEBUG_INSPECTOR,
  
  LIT_SHOW_DEBUG_MAX
};

enum lit_mode_t {
  LIT_MODE_SPLASH,
  LIT_MODE_MENU,
  LIT_MODE_GAME,
};

struct lit_save_data_t {
  // TODO
};

struct lit_t {
  platform_t* platform;
  gfx_t* gfx; 
  profiler_t* profiler;

  lit_show_debug_type_t show_debug_type;
  lit_mode_t next_mode;
  lit_mode_t mode;
  union {
    lit_splash_t splash;
    lit_game_t game;
    lit_menu_t menu;
  };
  u32_t level_to_start;

  //
  // Arenas
  //
  arena_t main_arena;
  arena_t asset_arena;
  arena_t debug_arena;
  arena_t frame_arena;

  // 
  // Debug Tools
  //
  inspector_t inspector;
  console_t console;

  assets_t assets;
  asset_sprite_id_t blank_sprite;
  asset_font_id_t debug_font;
};

static void 
lit_goto_specific_level(lit_t* lit, u32_t level) {
  //assert(level < array_count(g_lit_levels));
  lit->next_mode = LIT_MODE_GAME;
  lit->level_to_start = level;
}

#include "lit_splash.cpp"
#include "lit_menu.cpp"
#include "lit_game.cpp"

#include "lit_console_rendering.h"
#include "lit_profiler_rendering.h"
#include "lit_inspector_rendering.h"
#include "lit_levels.h"


static b32_t
lit_tick(platform_t* platform) {

  if(platform->game_context == nullptr) {
    auto* lit_memory = platform->allocate_memory(sizeof(lit_t));
    if (!lit_memory) return false;
    platform->game_context = lit_memory;

    auto* lit = (lit_t*)((platform_memory_t*)platform->game_context)->data;
    lit->platform = platform;
    lit->gfx = platform->gfx;
    lit->profiler = platform->profiler;
    lit->level_to_start = 0;

    lit->next_mode = LIT_MODE_MENU;

    //
    // Initialize assets
    //
    auto* asset_memory = platform->allocate_memory(megabytes(20));
    if (asset_memory == nullptr) return false;
    arena_init(&lit->asset_arena, asset_memory->data, asset_memory->size);
    assets_init(&lit->assets, platform, "test_pack.sui", &lit->asset_arena);

    //
    // Initialize important assets stuf
    //
    lit->blank_sprite = find_first_sprite(&lit->assets, ASSET_GROUP_TYPE_BLANK_SPRITE);
    make(asset_match_t, match);
    set_match_entry(match, ASSET_TAG_TYPE_FONT, 1.f, 1.f);
    lit->debug_font = find_best_font(&lit->assets, ASSET_GROUP_TYPE_FONTS, match);


    //
    // Initialize debug stuff
    //
    auto* debug_memory = platform->allocate_memory(megabytes(1));
    arena_init(&lit->debug_arena, debug_memory->data, debug_memory->size);
    console_init(&lit->console, 32, 256, &lit->debug_arena);

    auto* frame_memory = platform->allocate_memory(megabytes(1));
    arena_init(&lit->frame_arena, frame_memory->data, frame_memory->size);

    lit->platform->set_moe_dims(LIT_WIDTH, LIT_HEIGHT);
    gfx_push_view(lit->gfx, 0.f, LIT_WIDTH, 0.f, LIT_HEIGHT, 0.f, 0.f);

#if 0
    // TODO: test write file
    platform_file_t file = {};
    platform->open_file(&file, "hello.txt", PLATFORM_FILE_ACCESS_OVERWRITE, PLATFORM_FILE_PATH_USER);
    const char hey[] = "hey";
    platform->write_file(&file, 3, 0, (void*)hey);
#endif

  }

  auto* lit = (lit_t*)((platform_memory_t*)platform->game_context)->data;

  // NOTE(momo): Frame arena needs to be cleared each frame.
  arena_clear(&lit->frame_arena);

  // NOTE(momo): inspector need to clear each frame
  inspector_clear(&lit->inspector);

  if (lit->next_mode != lit->mode || platform->reloaded) {
    lit->mode = lit->next_mode;
    
    switch(lit->mode) {
      case LIT_MODE_SPLASH: {
        lit_init_splash(lit, &lit->splash);
      } break;
      case LIT_MODE_GAME: {
        lit_init_game(lit, &lit->game);
      } break;
      case LIT_MODE_MENU: {
        lit_init_menu(lit, &lit->menu);
      } break;

    }
  }
  switch(lit->mode) {
    case LIT_MODE_SPLASH: {
      lit_update_splash(lit, &lit->splash);
    } break;
    case LIT_MODE_GAME: {
      lit_update_game(lit, &lit->game);
      lit_render_game(lit, &lit->game);
    } break;
    case LIT_MODE_MENU: {
      lit_update_menu(lit, &lit->menu);
    } break;

  }

  // Debug
  if (platform_is_button_poked(platform->button_console)) {
    lit->show_debug_type = 
      (lit_show_debug_type_t)((lit->show_debug_type + 1)%LIT_SHOW_DEBUG_MAX);
  }

  switch (lit->show_debug_type) {
    case LIT_SHOW_DEBUG_CONSOLE: {
      lit_update_and_render_console(lit); 
    }break;
    case LIT_SHOW_DEBUG_PROFILER: {
      profiler_update_and_render(lit); 
    }break;
    case LIT_SHOW_DEBUG_INSPECTOR: {
      inspector_update_and_render(lit);
    }break;
    default: {}
  }

  return true;
}

#endif 
