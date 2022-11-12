#include "game.h"

/////////////////////////////////////////////////////////
// GAME
// 

static void
game_get_render_region(U32 window_w, U32 window_h, F32 aspect_ratio, U32* x0, U32* y0, U32* x1, U32* y1) 
{
  assert(aspect_ratio > 0.f && window_w > 0 && window_h > 0);
	F32 optimal_window_w = (F32)window_h * aspect_ratio;
	F32 optimal_window_h = (F32)window_w * 1.f/aspect_ratio;
	
	if (optimal_window_w > (F32)window_w) {
		// NOTE(Momo): width has priority - top and bottom bars
    if (x0) *x0 = 0;
    if (x1) *x1 = window_w;
		
		F32 empty_height = (F32)window_h - optimal_window_h;
		
		if(y0) *y0 = (U32)(empty_height * 0.5f);
		if(y1) *y1 = (*y0) + (U32)optimal_window_h;
	}
	else {
		// NOTE(Momo): height has priority - left and right bars
		if(y0) *y0 = 0;
		if(y1) *y1 = window_h;
		
		F32 empty_width = (F32)window_w - optimal_window_w;
		
		if (x0) *x0 = (U32)(empty_width * 0.5f);
		if (x1) *x1 = (*x0) + (U32)optimal_window_w;
	}
	
}

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
    // Initialize globals
    ba_clear(platform->game_arena);
    platform->game = ba_push(Game, platform->game_arena);
    Game* game = (Game*)platform->game;

    // around 32MB worth
    if (!ba_partition(platform->game_arena, &game->asset_arena, MB(20), 16)) 
      return false;
    if (!ba_partition(platform->game_arena, &game->mode_arena, MB(5), 16)) 
      return false; 
    if (!ba_partition(platform->game_arena, &game->debug_arena, MB(1), 16)) 
      return false;
    if (!ba_partition(platform->game_arena, &game->frame_arena, MB(1), 16)) 
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
    
    // Render region
    {
      U32 x0, y0, x1, y1; 
      game_get_render_region((U32)GAME_WIDTH, (U32)GAME_HEIGHT, GAME_WIDTH/GAME_HEIGHT, &x0, &y0, &x1, &y1); 

      platform->set_render_region(x0, y0, x1, y1);
    }
    gfx_push_view(gfx, 0.f, GAME_WIDTH, 0.f, GAME_HEIGHT, 0.f, 0.f);
    game_log("Initialized!");
   
  }
  Game* game = (Game*)platform->game;

  // Set globals from game
  assets = &game->assets;
  inspector = &game->inspector;
  Console* console = &game->console;

  // TODO: should probably be in modes instead

  begin_inspector(inspector);
 
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
