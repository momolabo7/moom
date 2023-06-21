#include "lit.h"

static b32_t
lit_tick() {
  if(game->context == nullptr) {
    void* lit_memory = pf.allocate_memory(sizeof(lit_t));
    if (!lit_memory) return false;
    game->context = lit_memory;

    lit = (lit_t*)(game->context);
    lit->level_to_start = 0;
    lit->next_mode = LIT_MODE_SPLASH;
    

    //
    // Initialize assets
    //
    usz_t asset_memory_size = megabytes(20);
    void* asset_memory = pf.allocate_memory(asset_memory_size);
    if (asset_memory == nullptr) return false;
    arena_init(&lit->asset_arena, asset_memory, asset_memory_size);
    assets_init(&lit->assets, gfx, LIT_ASSET_FILE, &lit->asset_arena);

    //
    // Initialize important assets stuf
    //
    lit->blank_sprite = assets_find_first_sprite(&lit->assets, ASSET_GROUP_TYPE_BLANK_SPRITE);
    make(asset_match_t, match);
    set_match_entry(match, ASSET_TAG_TYPE_FONT, 1.f, 1.f);
    lit->debug_font = assets_find_best_font(&lit->assets, ASSET_GROUP_TYPE_FONTS, match);


    //
    // Initialize debug stuff
    //
    usz_t debug_memory_size = megabytes(1);
    void* debug_memory = pf.allocate_memory(debug_memory_size);
    arena_init(&lit->debug_arena, debug_memory, debug_memory_size);
    inspector_init(&lit->inspector, &lit->debug_arena, 64);
    console_init(&lit->console, &lit->debug_arena, 32, 256);

    usz_t frame_memory_size = megabytes(1);
    void* frame_memory = pf.allocate_memory(frame_memory_size);
    arena_init(&lit->frame_arena, frame_memory, frame_memory_size);


    usz_t mode_memory_size = megabytes(1);
    auto* mode_memory = pf.allocate_memory(mode_memory_size);
    arena_init(&lit->mode_arena, mode_memory, mode_memory_size);

    pf.set_design_dims(LIT_WIDTH, LIT_HEIGHT);
    gfx_set_view(gfx, 0.f, LIT_WIDTH, 0.f, LIT_HEIGHT, 0.f, 0.f);

    //
    // Check save data
    //
#if LIT_SAVE_FILE_ENABLE
    lit_init_save_data(); 
#else
    lit->save_data.unlocked_levels = 100;
#endif

    
  }

  lit = (lit_t*)(game->context);

  // NOTE(momo): Frame arena needs to be cleared each frame.
  arena_clear(&lit->frame_arena);

  // NOTE(momo): inspector need to clear each frame
  inspector_clear(&lit->inspector);

  if (lit->next_mode != lit->mode || game->is_dll_reloaded) {
    lit->mode = lit->next_mode;
    
    switch(lit->mode) {
      case LIT_MODE_SPLASH: {
        lit_init_splash();
      } break;
      case LIT_MODE_GAME: {
        lit_game_init();
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

  }

  // Debug
  if (is_poked(input->buttons[INPUT_BUTTON_CODE_F1])) {
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

exported game_platform_config_t
game_get_platform_config(void) 
{
  game_platform_config_t ret;
  ret.texture_queue_size = megabytes(100);
  ret.render_command_size = megabytes(100);
  ret.window_title = "PRISMIX v1.0";

  return ret;
}

exported void 
game_update_and_render(
    game_t* in_game, 
    gfx_t* in_gfx, 
    audio_buffer_t* in_audio, 
    profiler_t* in_profiler, 
    input_t* in_input)
{ 
  pf = in_game->platform;  
  game = in_game;
  gfx = in_gfx;
  audio = in_audio;
  profiler = in_profiler;
  input = in_input;
  
  lit_tick();
}
