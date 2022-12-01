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
splash_tick(Game* game)
{

  if (!game_mode_initialized(game)) {
    Splash* splash = game_allocate_mode(Splash, game);
    splash->timer = 1.f;
  }
  
  Splash* splash = (Splash*)game->mode_context;
  
  F32 dt = platform->seconds_since_last_frame;
  splash->timer -= dt;
  
  if (splash->timer < 0.f) {
    // game_set_mode(game, 0, 0); 
    // game_set_mode(game, lit_init, lit_tick);
    game_goto_mode(game, GAME_MODE_TYPE_COMPUTER);
  }
 
  make(Game_Asset_Match, match);
  set_match_entry(match, asset_tag(FONT), 1.f, 1.f); 
  Game_Font_ID font_id = find_best_font(assets, GAME_ASSET_GROUP_TYPE_FONTS, match);

  RGBA color = rgba_set(splash->timer, splash->timer, splash->timer, splash->timer);
  paint_text(font_id, 
             str8_from_lit("momo"),
             color,
             450.f, 400.f, 
             256.f);
  gfx_advance_depth(gfx);
}



#endif // GAME_MODE_SPLASH_H


