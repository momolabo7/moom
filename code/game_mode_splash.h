/* date = April 27th 2022 8:10 pm */

#ifndef GAME_MODE_SPLASH_H
#define GAME_MODE_SPLASH_H

static void
init_splash_mode(Game_Memory* memory,
                 Game_Input* input) 
{
  Game_State* game = memory->game;
  Splash_Mode* splash = &game->splash_mode;
  splash->timer = 1.f;
}

static void 
update_splash_mode(Game_Memory* memory,
                   Game_Input* input) 
{
  Game_State* game = memory->game;
  Splash_Mode* splash = &game->splash_mode;
  Renderer_Command_Queue* cmds = memory->renderer_command_queue;
  
  F32 dt = input->seconds_since_last_frame;
  splash->timer -= dt;
  
  if (splash->timer < 0.f) {
    game->next_mode = GAME_MODE_SANDBOX;
  }
  // Clear colors
  {
    RGBA colors;
    colors.r = colors.g = colors.b  = colors.a = 0.2f;
    push_colors(cmds, colors);
  }
  
  // Set camera
  {
    V3 position = {};
    Rect3 frustum = {};
    
    frustum.min.x = 0.f;
    frustum.min.y = 0.f;
    frustum.max.x = 0.f;
    frustum.max.x = 1600.f;
    frustum.max.y = 900.f;
    frustum.max.z = 500.f;
    
    push_orthographic_camera(cmds, position, frustum);
  }
  
  {
    RGBA colors;
    colors.r = colors.g = colors.b  = colors.a = 1.f;
    
    V2 p0 = { 0.f, 0.f };
    V2 p1 = { 500.f, 500.f };
    V2 p2 = { 1000.f, 0.f };
    
    push_triangle(cmds, colors, p0, p1, p2, 0.f);
  }
}

#endif //GAME_MODE_SPLASH_H
