#include "momo.h"
#include "game.h"
#include "game_mode_splash.h"

Platform_API g_platform;
Profiler* g_profiler;


/////////////////////////////////////////////////////////
// GAME
exported B32 
game_update_and_render(Platform* pf)
{ 
  
  g_platform = pf->platform_api;
  g_profiler = pf->profiler;
  
  profile_block("game.dll");
  
  // Initialization
  if (!pf->game) {
    pf->game = ba_push<Game>(pf->game_arena);
    Game* game = (Game*)pf->game;
   
    // around 32MB worth
    game->asset_arena = ba_partition(pf->game_arena, MB(20));
    game->mode_arena = ba_partition(pf->game_arena, MB(5)); 
    game->debug_arena = ba_partition(pf->game_arena, MB(1));
    game->frame_arena = ba_partition(pf->game_arena, MB(1));
    
    B32 success = load_game_assets(&game->game_assets, 
                                   pf->renderer_texture_queue,
                                   "test.sui",
                                   &game->asset_arena);
    if(!success) return false;
    
    //game->next_mode = GAME_MODE_SPLASH;
    game_set_mode(game, splash_init, splash_tick);
     
    
    // Initialize Debug Console
    Console* console = &game->console;
    init_console(console, &game->debug_arena);
    
    game->show_debug_type = GAME_SHOW_DEBUG_NONE;
    game_log("Initialized!");
  }
  
  Game* game = (Game*)pf->game;
  Console* console = &game->console;
  Game_Assets* ga = &game->game_assets;
  Gfx_Command_Queue* cmds = pf->renderer_command_queue;
  Inspector* in = &game->inspector;
 
  declare_and_pointerize(Painter, painter);
  begin_painting(painter, ga, cmds, 1600.f, 900.f);
  begin_inspector(in);
  
  static U32 test_value = 32;
  add_inspector_entry(in, string_from_lit("Test"), &test_value);
  
  //-Game state management
  // TODO: Figure out is_done
  B32 is_done = false;
   
  //~ GSM
  if (game->is_mode_changed && game->init_mode) {
    game->init_mode(game);
    game->is_mode_changed = false;
  }

  if (game->update_mode) {
    game->update_mode(game, painter, pf);
  }
  
  //-Debug Rendering Stuff
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
  
  return is_done;
  
}
