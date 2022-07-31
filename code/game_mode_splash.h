/* date = April 27th 2022 8:10 pm */

#ifndef GAME_MODE_SPLASH_H
#define GAME_MODE_SPLASH_H

#include "game.h"

static void sb1_init(Game*);
static void sb1_tick(Game*, Painter*, Platform* );

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
            Painter* painter,
            Platform* pf) 
{
  auto* splash = (Splash*)game->mode_context;
  
  F32 dt = pf->seconds_since_last_frame;
  splash->timer -= dt;
  
  if (splash->timer < 0.f) {
    //game_set_mode(game, 0, 0); 

    game_set_mode(game, sb1_init, sb1_tick);
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


