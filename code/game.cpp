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
    memory->game = mp_push<Game_State>(memory->game_arena);
    Game_State* game = memory->game;
    
    game->asset_arena = mp_partition(memory->game_arena, MB(20));
    game->debug_arena = mp_partition(memory->game_arena, MB(1));
    game->frame_arena = mp_partition(memory->game_arena, MB(1));
    
    
    B32 success = load_game_assets(&game->game_assets, 
                                   memory->renderer_texture_queue,
                                   "test.sui",
                                   &game->asset_arena);
    if(!success) return false;
    
    game->next_mode = GAME_MODE_SPLASH;
    
    
    // Initialize Debug Console
    Console* dc = &memory->game->console;
    init_console(dc, &memory->game->debug_arena);
    
    game->show_debug_type = SHOW_DEBUG_NONE;
    game_log("Initialized!");
  }
  
  Game_State* game = memory->game;
  Console* dc = &game->console;
  Game_Assets* ga = &game->game_assets;
  Renderer_Command_Queue* cmds = memory->renderer_command_queue;
  Painter* p = &game->painter;
  Inspector* in = &game->inspector;
  
  begin_painting(p, ga, cmds, 1600.f, 900.f);
  begin_inspector(in);
  
  
  static U32 test_value = 32;
  add_inspector_entry(in, string_from_lit("Test"), &test_value);
  
  //-Game state management
  B32 is_done = false;
  {
    if (game->next_mode != game->current_mode) {
      switch(game->next_mode) {
        case GAME_MODE_SPLASH: {
          init_splash_mode(memory, input);
        } break;
        case GAME_MODE_LEVEL: {
          init_level_mode(memory, input);
        } break;
        default: {
          is_done = true;
        }
      }
      game->current_mode = game->next_mode;
    }
    
    switch(game->current_mode) {
      case GAME_MODE_SPLASH: {
        update_and_render_splash_mode(memory, input);
      } break;
      case GAME_MODE_LEVEL: {
        update_and_render_level_mode(memory, input);
      } break;
    }
  }
  
  
  //-Debug
  if (is_poked(input->button_console)) {
    game->show_debug_type = (Show_Debug_Type)((game->show_debug_type + 1)%SHOW_DEBUG_MAX);
  }
  
  switch (game->show_debug_type) {
    case SHOW_DEBUG_CONSOLE: {
      update_and_render_console(dc, input, p); 
    }break;
    case SHOW_DEBUG_PROFILER: {
      update_and_render_profiler(memory->profiler, p); 
    }break;
    case SHOW_DEBUG_INSPECTOR: {
      // Render inspector
      // TODO: finish this
      paint_sprite(p, SPRITE_BLANK, 
                   game_wh * 0.5f, 
                   game_wh,
                   {0.f, 0.f, 0.f, 0.5f});
      advance_depth(p);
      
      F32 line_height = 32.f;
      make_string_builder(sb, 256);
      
      al_foreach(entry_index, &in->entries)
      {
        Inspector_Entry* entry = al_get(&in->entries, entry_index);
        
        
        U32 item = *(U32*)entry->item;
        push_format(sb, string_from_lit("[%10S] %7u"),
                    entry->name, item);
        F32 y = game_wh.h - line_height * (entry_index+1);
        
        paint_text(p,
                   FONT_DEBUG, 
                   sb->str,
                   rgba(0xFFFFFFFF),
                   0.f, 
                   y, 
                   line_height);
        advance_depth(p);
        
        
      }
    }break;
  }
  return is_done;
}