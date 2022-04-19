

#include "momo.h"
#include "game.h"

Platform_API g_platform;
Profiler* g_profiler;


static void
init_splash_mode(Game_Memory* memory,
                 Game_Input* input) 
{
  Game_State* game = memory->game;
  Splash_Mode* splash = &game->splash_mode;
  splash->timer = 3.f;
}

static void 
init_sandbox_mode(Game_Memory* memory,
                  Game_Input* input) 
{
  Game_State* game = memory->game;
  Sandbox_Mode* sandbox = &game->sandbox_mode;
  Game_Assets* ga = &game->game_assets;
  Renderer_Command_Queue* cmds = memory->renderer_command_queue;
  
  sandbox->tmp_delta = 0.f;
  sandbox->tmp_increase = true;
  sandbox->tmp_rot = 0.f;
  
}

static void 
update_splash_mode(Game_Memory* memory,
                   Game_Input* input) 
{
  Game_State* game = memory->game;
  Splash_Mode* splash = &game->splash_mode;
  
  F32 dt = input->seconds_since_last_frame;
  splash->timer -= dt;
  
  if (splash->timer < 0.f) {
    game->next_mode = GAME_MODE_SANDBOX;
  }
  
}

static void 
update_sandbox_mode(Game_Memory* memory,
                    Game_Input* input) 
{
  Game_State* game = memory->game;
  Sandbox_Mode* sandbox = &game->sandbox_mode;
  Game_Assets* ga = &game->game_assets;
  Renderer_Command_Queue* cmds = memory->renderer_command_queue;
  F32 dt = input->seconds_since_last_frame;
  
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

exported B32 
game_update_and_render(Game_Memory* memory,
                       Game_Input* input) 
{ 
  
  g_platform = memory->platform_api;
  g_profiler = memory->profiler;
  
  profile_block("game.dll");
  
  
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
    
    game->next_mode = GAME_MODE_SPLASH;
    
    
    // Initialize Debug Console
    Console* dc = &memory->game->console;
    init_console(dc, &memory->game->debug_arena);
    
    game_log("Initialized!");
  }
  
  
  Game_State* game = memory->game;
  
  // Actual update here.
  Console* dc = &game->console;
  Game_Assets* ga = &game->game_assets;
  Renderer_Command_Queue* cmds = memory->renderer_command_queue;
  
  // Update console code
  if (is_poked(input->button_console)) {
    game->show_profiler = !game->show_profiler;
  }
  
  update_console(dc, input);
  
  
  // Game state management
  B32 is_done = false;
  if (game->next_mode != game->current_mode) {
    switch(game->next_mode) {
      case GAME_MODE_SPLASH: {
        game->mode_init = init_splash_mode;
        game->mode_update = update_splash_mode;
      } break;
      case GAME_MODE_SANDBOX: {
        game->mode_init = init_sandbox_mode;
        game->mode_update = update_sandbox_mode;
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
  
  
  
  render_console(dc, ga, cmds);
  
  
  if (game->show_profiler)
    render_profiler(memory->profiler, ga, cmds);
  
  return is_done;
  
}
