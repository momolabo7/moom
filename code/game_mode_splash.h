/* date = April 27th 2022 8:10 pm */

#ifndef GAME_MODE_SPLASH_H
#define GAME_MODE_SPLASH_H

#include "game.h"

//////////////////////////////////////////////////
// SPLASH MODE
//
struct Splash {
  F32 timer;
};

static void
splash_init(Game* game) 
{
  auto* splash = game_allocate_mode<Splash>(game);
  splash->timer = 1.f;
}

static void 
splash_tick(Game* game,
            Game_Input* input,
            Painter* painter) 
{
  auto* splash = (Splash*)game->mode_context;
  
  F32 dt = input->seconds_since_last_frame;
  splash->timer -= dt;
  
  if (splash->timer < 0.f) {
    //game_set_mode(game, 0, 0); 

    game_set_mode(game, splash_init, splash_tick);
  }
 
  auto color = rgba(splash->timer, splash->timer, splash->timer, splash->timer);
  paint_text(painter,
             FONT_DEFAULT, 
             string_from_lit("momo"),
             color,
             450.f, 400.f, 
             256.f);
  advance_depth(painter);
}



#endif // GAME_MODE_SPLASH_H


