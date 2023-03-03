#ifndef MOE_MODE_LIT_H
#define MOE_MODE_LIT_H

#define LIT_DEBUG_INTERSECTIONS 0
#define LIT_DEBUG_COORDINATES 1

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
#define LIT_PLAYER_ROTATE_SPEED 3.5f





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

struct lit_splash_t {
  f32_t timer;
  asset_font_id_t font;
};

struct lit_menu_t {
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
lit_init_splash(lit_t* lit, lit_splash_t* splash) {
  make(asset_match_t, match);
  set_match_entry(match, ASSET_TAG_TYPE_FONT, 0.f, 1.f);
  splash->font = find_best_font(&lit->assets, ASSET_GROUP_TYPE_FONTS, match);
  splash->timer = 1.5f;
}

static void
lit_update_splash(lit_t* lit, lit_splash_t* splash) {
  splash->timer -= lit->platform->seconds_since_last_frame;
 
  moe_painter_draw_text_center_aligned(lit->gfx, &lit->assets, splash->font, str8_from_lit("moom"), rgba_set(1.f, 1.f, 1.f, 1.f), LIT_WIDTH/2, LIT_HEIGHT/2, 128.f);

  if (splash->timer <= -1.f) {
    //lit->next_mode = LIT_MODE_GAME; 
  }

}

static void
lit_init_menu(lit_t* lit, lit_menu_t* menu) {
  // TODO
}

static void
lit_update_menu(lit_t* lit, lit_menu_t* menu) {
  // TODO

}




#include "lit_game.cpp"

#include "lit_console_rendering.h"
#include "lit_profiler_rendering.h"
#include "lit_inspector_rendering.h"
#include "lit_levels.h"


static b32_t
lit_tick(platform_t* platform) {

  if(platform->game_context == nullptr) {
    auto* lit_memory = platform->allocate_memory(sizeof(lit_t));
    platform->game_context = lit_memory;

    auto* lit = (lit_t*)((platform_memory_t*)platform->game_context)->data;
    lit->platform = platform;
    lit->gfx = platform->gfx;
    lit->profiler = platform->profiler;

    lit->next_mode = LIT_MODE_SPLASH; 

    // TODO: Error checking
    // TODO: Better memory management
    // Initialize assets
    auto* asset_memory = platform->allocate_memory(megabytes(20));
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

  }

  auto* lit = (lit_t*)((platform_memory_t*)platform->game_context)->data;

  // NOTE(momo): Frame arena needs to be cleared each frame.
  arena_clear(&lit->frame_arena);

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

  return false;
}

#endif 
