#include "game.h"


/////////////////////////////////////////////////////////
// GAME
exported B32 
game_update_and_render(Platform* pf)
{ 
#if INTERNAL
  g_platform = pf;
  g_profiler = pf->profiler;
#endif
  game_profile_block("game");
  // Initialization
  if (!pf->game || pf->reloaded) {
    ba_clear(pf->game_arena);
    pf->game = ba_push(Game, pf->game_arena);
    Game* game = (Game*)pf->game;
   
    // around 32MB worth
    if (!ba_partition(pf->game_arena, &game->asset_arena, MB(20), 16)) 
      return false;
    if (!ba_partition(pf->game_arena, &game->mode_arena, MB(5), 16)) 
      return false; 
    if (!ba_partition(pf->game_arena, &game->debug_arena, MB(1), 16)) 
      return false;
    if (!ba_partition(pf->game_arena, &game->frame_arena, MB(1), 16)) 
      return false;
    
    if(!load_game_assets(&game->game_assets, 
                         pf,
                         "test.sui",
                         &game->asset_arena))
    {
      return false;
    }
   
    game_goto_mode(game, GAME_MODE_TYPE_SPLASH);
    
    //game_set_mode(game, splash_init, splash_tick);
    //game_set_mode(game, lit_init, lit_tick);
    
    // Initialize Debug Console
    Console* console = &game->console;
    init_console(console, &game->debug_arena);
    
    game->show_debug_type = GAME_SHOW_DEBUG_NONE;
    game->is_done = false;
    game_log("Initialized!");
  }
  
  Game* game = (Game*)pf->game;
  Console* console = &game->console;
  Game_Assets* ga = &game->game_assets;
  Gfx* gfx = pf->gfx;
  Inspector* in = &game->inspector;
 
  make(Painter, painter);
  begin_painting(painter, ga, gfx, 1600.f, 900.f);
  begin_inspector(in);
 
#if 0
  static U32 test_value = 32;
  add_inspector_entry(in, str8_from_lit("Test"), &test_value);
#endif

  
  // Game state management
  if (game->is_mode_changed) {
    ba_clear(&game->mode_arena);
    game->mode_context = null;
    game->is_mode_changed = false;
  }
  game_modes[game->current_game_mode](game, painter, pf);

  // Debug Rendering Stuff
  if (pf_is_button_poked(pf->button_console)) {
    game->show_debug_type = 
      (Game_Show_Debug_Type)((game->show_debug_type + 1)%GAME_SHOW_DEBUG_MAX);
  }
  switch (game->show_debug_type) {
    case GAME_SHOW_DEBUG_CONSOLE: {
      update_and_render_console(console, painter, pf); 
    }break;
    case GAME_SHOW_DEBUG_PROFILER: {
      update_and_render_profiler(pf->profiler, painter); 
    }break;
    case GAME_SHOW_DEBUG_INSPECTOR: {
      update_and_render_inspector(in, painter);
    }break;
    default: {}
  }

#if 0
  static F32 sine = 0.f;
  Platform_Audio* audio = pf->audio;
  S16* sample_out = audio->sample_buffer;
  S16 volume = 3000;
  for(U32 sample_index = 0; sample_index < audio->sample_count; ++sample_index) {
    for (U32 channel_index = 0; channel_index < audio->channels; ++channel_index) {
      F32 sine_value = sin(sine);
      sample_out[channel_index] = S16(sine_value * volume);
    }
    sample_out += audio->channels;
    sine += 2.0f;
  }
#endif

  return game->is_done;
  
}
