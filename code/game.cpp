#include "game.h"

/////////////////////////////////////////////////////////
// GAME
// 

exported B32 
game_update_and_render(Platform* pf)
{ 
  // Set globals from platform
  platform = pf;
  gfx = platform->gfx;
#if INTERNAL
  profiler = platform->profiler;
#endif


  game_profile_block(GAME);
  // Initialization
  if (!platform->game || platform->reloaded) {
    arn_clear(platform->game_arena);
    platform->game = arn_push(Game, platform->game_arena);
    Game* game = (Game*)platform->game;

    // around 32MB worth
    if (!arn_partition(platform->game_arena, &game->asset_arena, MB(20), 16)) 
      return false;
    if (!arn_partition(platform->game_arena, &game->mode_arena, MB(5), 16)) 
      return false; 
    if (!arn_partition(platform->game_arena, &game->debug_arena, MB(1), 16)) 
      return false;
    if (!arn_partition(platform->game_arena, &game->frame_arena, MB(1), 16)) 
      return false;
    
    if(!init_game_assets(&game->assets, 
                         "test_pack.sui",
                         &game->asset_arena))
    {
      return false;
    }
   
    game->blank_sprite = find_first_sprite(&game->assets, asset_group(BLANK_SPRITE));
    // Debug font
    {
      make(Game_Asset_Match, match);
      set_match_entry(match, asset_tag(FONT), 1.f, 1.f);
      game->debug_font = find_best_font(&game->assets, asset_group(FONTS), match);
    }

    game_goto_mode(game, GAME_MODE_TYPE_LIT);

    
    //game_set_mode(game, splash_init, splash_tick);
    //game_set_mode(game, lit_init, lit_tick);
    
    // Initialize Debug Console
    Console* console = &game->console;
    init_console(console, &game->debug_arena);
    
    game->show_debug_type = GAME_SHOW_DEBUG_NONE;
    game->is_done = false;
    
    // Inform platform what our game's dimensions are
    platform->set_game_dims(GAME_WIDTH, GAME_HEIGHT);

    // set up view for game
    gfx_push_view(gfx, 0.f, GAME_WIDTH, 0.f, GAME_HEIGHT, 0.f, 0.f);

    game_log("Initialized!");
   
  }
 
  // Set globals from game
  Game* game = (Game*)platform->game;
  assets = &game->assets;
  inspector = &game->inspector;
  Console* console = &game->console;

 
#if 0
  // TODO: should probably be in modes instead
  insp_clear(inspector);
  static U32 test_value = 32;
  insp_add_u32(in, str8_from_lit("Test"), &test_value);
#endif

  
  // Game state management
  if (game->is_mode_changed) {
    arn_clear(&game->mode_arena);
    game->mode_context = null;
    game->is_mode_changed = false;
  }
  game_modes[game->current_game_mode](game);

  // Debug Rendering Stuff
  if (pf_is_button_poked(platform->button_console)) {
    game->show_debug_type = 
      (Game_Show_Debug_Type)((game->show_debug_type + 1)%GAME_SHOW_DEBUG_MAX);
  }

  game_profile_begin(DEBUG);
  switch (game->show_debug_type) {
    case GAME_SHOW_DEBUG_CONSOLE: {
      update_and_render_console(console,
                                game->blank_sprite, 
                                game->debug_font); 
    }break;
    case GAME_SHOW_DEBUG_PROFILER: {
      update_and_render_profiler(game->blank_sprite, 
                                 game->debug_font); 
    }break;
    case GAME_SHOW_DEBUG_INSPECTOR: {
      update_and_render_inspector(game->blank_sprite, 
                                  game->debug_font);
    }break;
    default: {}
  }
  game_profile_end(DEBUG);

#if 0
  static F32 sine = 0.f;
  Platform_Audio* audio = platform->audio;
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
