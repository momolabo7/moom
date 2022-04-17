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
  F32 dt = input->seconds_since_last_frame;
  
  
  // Initialization
  if (!memory->game) {
    g_platform.set_aspect_ratio(16, 9);
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
    
    
    // Initialize perm memory
    Sandbox_Mode* sandbox = &game->sandbox_mode;
    sandbox->tmp_delta = 0.f;
    sandbox->tmp_increase = true;
    sandbox->tmp_rot = 0.f;
    
    // Initialize Debug Console
    Console* dc = &memory->game->console;
    init_console(dc, &memory->game->debug_arena);
    
    game_log("Initialized!");
  }
  
  
  Game_State* game = memory->game;
  
  // Actual update here.
  Sandbox_Mode* sandbox = &game->sandbox_mode;
  Console* dc = &game->console;
  Game_Assets* ga = &game->game_assets;
  Renderer_Command_Queue* cmds = memory->renderer_command_queue;
  
  update_console(dc, input);
  {
    profile_block;
    // Clear colors
    {
      RGBA colors;
      colors.r = colors.g = colors.b  = colors.a = 0.3f;
      push_colors(cmds, colors);
    }
    
    // Set camera
    {
      V3 position = {};
      Rect3 frustum;
      frustum.min.x = frustum.min.y = frustum.min.z = 0;
      frustum.max.x = 1600;
      frustum.max.y = 900;
      frustum.max.z = 500;
      push_orthographic_camera(cmds, position, frustum);
    }
    
    {
      if (sandbox->tmp_increase)
        sandbox->tmp_delta += dt; 
      else
        sandbox->tmp_delta -= dt;
      
      if (sandbox->tmp_delta >= 1.f ){
        sandbox->tmp_delta = 1.f;
        sandbox->tmp_increase = false;
      }
      
      if (sandbox->tmp_delta <= 0.f) {
        sandbox->tmp_delta = 0.f;
        sandbox->tmp_increase = true;
      }
      
      RGBA colors = rgba(1.f, 1.f, 1.f, 1.f);
      M44 s = m44_scale(600.f, 600.f, 10.f);
      M44 r = m44_rotation_z(sandbox->tmp_rot += dt);
      M44 t = m44_translation(800.f, 450.f, 300.f);
      
      {
        Sprite_Asset* sprite = get_sprite(ga, SPRITE_BULLET_CIRCLE);
        assert(sprite);
        Bitmap_Asset* bitmap = get_bitmap(ga, sprite->bitmap_id);
        assert(bitmap);
        
        push_subsprite(cmds, 
                       colors,
                       t*r*s,
                       bitmap->renderer_texture_handle, 
                       sprite->uv);
        
      }
    }
  }
  
  render_console(dc, ga, cmds);
  
  // Do together?
  update_entries(g_profiler); 
  render_profiler(g_profiler, ga, cmds);
  
  
  return true;
  
}
