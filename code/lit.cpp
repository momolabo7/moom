#include "lit.h"

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
    gfx_set_view(lit->gfx, 0.f, LIT_WIDTH, 0.f, LIT_HEIGHT, 0.f, 0.f);

#if 0
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
        lit_menu_init(lit, &lit->menu);
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
      lit_menu_tick(lit, &lit->menu);
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
exported b32_t 
moe_update_and_render(platform_t* pf)
{ 
  return lit_tick(pf);
}
