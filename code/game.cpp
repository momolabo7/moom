#include "momo.h"
#include "game.h"

Platform_API g_platform;
Profiler* g_profiler;



exported B32 
game_update_and_render(Game_Memory* memory,
                       Game_Input* input) 
{ 
  
  g_platform = memory->platform_api;
  g_profiler = memory->profiler;
  
  profile_block("game.dll");
  
  // Initialization
  if (!memory->game) {
    memory->game = push<Game_State>(memory->game_arena);
    Game_State* game = memory->game;
    
    game->asset_arena = partition(memory->game_arena, MB(20));
    game->debug_arena = partition(memory->game_arena, MB(1));
    game->frame_arena = partition(memory->game_arena, MB(1));
    
    
    B32 success = load_game_assets(&game->game_assets, 
                                   memory->renderer_texture_queue,
                                   "test.sui",
                                   &game->asset_arena);
    if(!success) return false;
    
    game->next_mode = GAME_MODE_SPLASH;
    
    
    // Initialize Debug Console
    Console* dc = &memory->game->console;
    init_console(dc, &memory->game->debug_arena);
    
    game_log("Initialized!");
  }
  
  // calculate design dimentions
  
  Game_State* game = memory->game;
  
  // Actual update here.
  Console* dc = &game->console;
  Game_Assets* ga = &game->game_assets;
  Renderer_Command_Queue* cmds = memory->renderer_command_queue;
  
  // Update console code
  if (is_poked(input->button_console)) {
    game->show_debug = !game->show_debug;
  }
  
  if (game->show_debug) {
    update_console(dc, input);
  }
  
  
  // Game state management
  B32 is_done = false;
  if (game->next_mode != game->current_mode) {
    switch(game->next_mode) {
      case GAME_MODE_SPLASH: {
        game->mode_init = init_splash_mode;
        game->mode_update = update_splash_mode;
      } break;
      case GAME_MODE_LEVEL: {
        game->mode_init = init_level_mode;
        game->mode_update = update_level_mode;
      } break;
      default: {
        is_done = true;
        game->mode_init = mode_noop;
        game->mode_update = mode_noop;
      }
    }
    
    game->mode_init(memory, input);
    game->current_mode = game->next_mode;
  }
  
  
  game->mode_update(memory, input);
  
  
  // render debug stuff
  if (game->show_debug)
  {
    // background
    draw_sprite(ga, cmds, SPRITE_BLANK, 
                game_width/2, game_height/2, 
                game_width, game_height,
                50.f,
                {0.f, 0.f, 0.f, 0.8f});
    render_console(dc, ga, cmds);
    render_profiler(memory->profiler, ga, cmds);
  }
  return is_done;
}