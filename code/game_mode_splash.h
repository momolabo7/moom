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
  Game_Assets* ga = &game->game_assets;
  
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
  
  draw_sprite(ga, cmds, SPRITE_BULLET_DOT, 
              800.f, 450.f, 128.f, 128.f,
              11.f);
  
  draw_sprite(ga, cmds, SPRITE_BLANK, 
              800.f, 450.f, 1600.f, 900.f,
              10.f, rgba(0x000000FF));
  
  draw_sprite(ga, cmds, SPRITE_BLANK, 
              800.f, 450.f, 1600.f/2, 900.f/2,
              9.f, rgba(0xFFFFFFFF));
}

#endif //GAME_MODE_SPLASH_H
