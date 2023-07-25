#include "lit.h"

static b32_t
lit_tick() {
  if(app->game == nullptr) {
    void* lit_memory = app_allocate_memory(app, sizeof(lit_t));
    if (!lit_memory) return false;
    app->game = lit_memory;

    lit = (lit_t*)(app->game);
    lit->level_to_start = 0;
    lit->next_mode = LIT_MODE_SPLASH;
    lit->mode = LIT_MODE_NONE;
    

    //
    // Initialize assets
    //
    usz_t asset_memory_size = megabytes(20);
    void* asset_memory = app_allocate_memory(app, asset_memory_size);
    if (asset_memory == nullptr) return false;
    arena_init(&lit->asset_arena, asset_memory, asset_memory_size);
    assets_init(&lit->assets, app, LIT_ASSET_FILE, &lit->asset_arena);


    //
    // Initialize debug stuff
    //
    usz_t debug_memory_size = megabytes(1);
    void* debug_memory = app_allocate_memory(app, debug_memory_size);
    arena_init(&lit->debug_arena, debug_memory, debug_memory_size);
    inspector_init(&lit->inspector, &lit->debug_arena, 64);
    console_init(&lit->console, &lit->debug_arena, 32, 256);

    usz_t frame_memory_size = megabytes(1);
    void* frame_memory = app_allocate_memory(app, frame_memory_size);
    arena_init(&lit->frame_arena, frame_memory, frame_memory_size);


    usz_t mode_memory_size = megabytes(1);
    auto* mode_memory = app_allocate_memory(app, mode_memory_size);
    arena_init(&lit->mode_arena, mode_memory, mode_memory_size);

    app_set_design_dimensions(app, LIT_WIDTH, LIT_HEIGHT);
    app_set_view(app, 0.f, LIT_WIDTH, 0.f, LIT_HEIGHT, 0.f, 0.f);

    //
    // Check save data
    //
#if LIT_SAVE_FILE_ENABLE
    lit_init_save_data(); 
#else
    lit->save_data.unlocked_levels = 100;
#endif

    
  }

  lit = (lit_t*)(app->game);

  arena_clear(&lit->frame_arena);
  inspector_clear(&lit->inspector);


  if (lit->next_mode != lit->mode || app_is_dll_reloaded(app)) 
  {
    lit->mode = lit->next_mode;
    
    switch(lit->mode) {
      case LIT_MODE_SPLASH: {
        lit_splash_init();
      } break;
      case LIT_MODE_GAME: {
        lit_game_init();
      } break;
      case LIT_MODE_CREDITS: {
        lit_credits_init();
      } break;
      case LIT_MODE_SANDBOX: {
        lit_sandbox_init();
      } break;

    }
  }

  switch(lit->mode) {
    case LIT_MODE_SPLASH: {
      lit_splash_update();
    } break;
    case LIT_MODE_GAME: {
      lit_game_update();
    } break;
    case LIT_MODE_CREDITS: {
      lit_credits_update();
    } break;
    case LIT_MODE_SANDBOX: {
      lit_sandbox_update();
    } break;
  }

  // Debug
#if LIT_DEBUG
  if (is_poked(input, INPUT_BUTTON_CODE_F1)) {
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
#endif

  return true;
}

exported 
game_init_sig(game_init) 
{
  game_init_config_t ret;
  ret.texture_queue_size = megabytes(100);
  ret.render_command_size = megabytes(100);
  ret.window_title = "PRISMIX v1.0";

  return ret;
}

exported 
game_update_and_render_sig(game_update_and_render) 
{ 
  app = in_app;
  lit_tick();
}
