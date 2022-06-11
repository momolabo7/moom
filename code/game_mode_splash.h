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
update_and_render_splash_mode(Game_Memory* memory,
                              Game_Input* input) 
{
  Game_State* game = memory->game;
  Splash_Mode* splash = &game->splash_mode;
  Renderer_Command_Queue* cmds = memory->renderer_command_queue;
  Game_Assets* ga = &game->game_assets;
  Painter* p = &game->painter;
  
  F32 dt = input->seconds_since_last_frame;
  splash->timer -= dt;
  
  if (splash->timer < 0.f) {
    game->next_mode = GAME_MODE_LEVEL;
  }
  
  
  paint_text(p, FONT_DEFAULT, 
             string_from_lit("momo"),
             rgba(0x38c677FF),
             450.f, 400.f, 
             256.f);
  advance_depth(p);
}

#endif //GAME_MODE_SPLASH_H
