#include "lit.h"




static b32_t
lit_tick() {
  if(moe->game_context == nullptr) {
    auto* lit_memory = pf->allocate_memory(sizeof(lit_t));
    if (!lit_memory) return false;
    moe->game_context = lit_memory;

    lit = (lit_t*)((pf_memory_t*)moe->game_context)->data;
    lit->level_to_start = 0;
    lit->next_mode = LIT_MODE_GAME;

    //
    // Initialize assets
    //
    auto* asset_memory = pf->allocate_memory(megabytes(20));
    if (asset_memory == nullptr) return false;
    arena_init(&lit->asset_arena, asset_memory->data, asset_memory->size);
    assets_init(&lit->assets, pf, gfx, "test_pack.sui", &lit->asset_arena);

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
    auto* debug_memory = pf->allocate_memory(megabytes(1));
    arena_init(&lit->debug_arena, debug_memory->data, debug_memory->size);
    console_init(&lit->console, 32, 256, &lit->debug_arena);

    auto* frame_memory = pf->allocate_memory(megabytes(1));
    arena_init(&lit->frame_arena, frame_memory->data, frame_memory->size);

    auto* mode_memory = pf->allocate_memory(megabytes(1));
    arena_init(&lit->mode_arena, mode_memory->data, mode_memory->size);

    pf->set_moe_dims(LIT_WIDTH, LIT_HEIGHT);
    gfx_set_view(gfx, 0.f, LIT_WIDTH, 0.f, LIT_HEIGHT, 0.f, 0.f);

    //
    // Check save data
    //
    lit_init_save_file(); 

    
  }

  lit = (lit_t*)((pf_memory_t*)moe->game_context)->data;

  // NOTE(momo): Frame arena needs to be cleared each frame.
  arena_clear(&lit->frame_arena);

  // NOTE(momo): inspector need to clear each frame
  inspector_clear(&lit->inspector);

  if (lit->next_mode != lit->mode || input->reloaded) {
    lit->mode = lit->next_mode;
    
    switch(lit->mode) {
      case LIT_MODE_SPLASH: {
        lit_init_splash();
      } break;
      case LIT_MODE_GAME: {
        lit_game_init();
      } break;
      case LIT_MODE_MENU: {
        lit_menu_init();
      } break;

    }
  }

  switch(lit->mode) {
    case LIT_MODE_SPLASH: {
      lit_update_splash();
    } break;
    case LIT_MODE_GAME: {
      lit_game_update();
    } break;
    case LIT_MODE_MENU: {
      lit_update_menu();
    } break;

  }

  // Debug
  if (input_is_button_poked(input->buttons[INPUT_BUTTON_CODE_F1])) {
    lit->show_debug_type = 
      (lit_show_debug_type_t)((lit->show_debug_type + 1)%LIT_SHOW_DEBUG_MAX);
  }

  switch (lit->show_debug_type) {
    case LIT_SHOW_DEBUG_CONSOLE: {
      lit_update_and_render_console(); 
    }break;
    case LIT_SHOW_DEBUG_PROFILER: {
      profiler_update_and_render(); 
    }break;
    case LIT_SHOW_DEBUG_INSPECTOR: {
      inspector_update_and_render();
    }break;
    default: {}
  }

  return true;
}

exported b32_t 
moe_update_and_render(
    moe_t* in_moe, 
    pf_t* in_pf, 
    gfx_t* in_gfx, 
    pf_audio_t* in_audio, 
    profiler_t* in_profiler, 
    input_t* in_input)
{ 
  moe_begin;
  return lit_tick();
}
