/* date = April 27th 2022 8:10 pm */

#ifndef GAME_MODE_SPLASH_H
#define GAME_MODE_SPLASH_H

//////////////////////////////////////////////////
// SPLASH MODE
//
struct Splash {
  F32 timer;
};

static void 
splash_tick(Game* game,
            Painter* painter,
            Platform* pf) 
{

  if (!game_mode_initialized(game)) {
    Splash* splash = game_allocate_mode(Splash, game);
    splash->timer = 1.f;
  }
  
  Splash* splash = (Splash*)game->mode_context;
  
  F32 dt = pf->seconds_since_last_frame;
  splash->timer -= dt;
  
  if (splash->timer < 0.f) {
    // game_set_mode(game, 0, 0); 
    // game_set_mode(game, lit_init, lit_tick);
    game_goto_mode(game, GAME_MODE_TYPE_COMPUTER);
  }
  
  Game_Font_ID font_id = find_first_font(painter->ga, GAME_ASSET_GROUP_TYPE_DEFAULT_FONT);

  RGBA color = rgba(splash->timer, splash->timer, splash->timer, splash->timer);
  paint_text(painter,
             font_id, 
             str8_from_lit("momo"),
             color,
             450.f, 400.f, 
             256.f);
  advance_depth(painter);
}



#endif // GAME_MODE_SPLASH_H


