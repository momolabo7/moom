#ifndef GAME_H
#define GAME_H

#include "lit.h"



static void 
game_tick(moe_t* moe) 
{
  assets_t* assets = &moe->assets;
  platform_t* platform = moe->platform;

  if(moe->game_context == nullptr) {
    auto* lit = (lit_t*)platform->allocate_memory(sizeof(lit_t));
    moe->game_context = lit;
    
    //
    // init game
    // 
    lit_game_t* game = &lit->game;
    lit_load_level(game, 0); 
    rng_init(&game->rng, 65535); // don't really need to be strict 

    {
      make(asset_match_t, match);
      set_match_entry(match, ASSET_TAG_TYPE_FONT, 0.f, 1.f);
      game->tutorial_font = find_best_font(assets, ASSET_GROUP_TYPE_FONTS, match);
    }

    game->blank_sprite = find_first_sprite(assets, ASSET_GROUP_TYPE_BLANK_SPRITE);
    game->circle_sprite = find_first_sprite(assets, ASSET_GROUP_TYPE_CIRCLE_SPRITE);
    game->filled_circle_sprite = find_first_sprite(assets, ASSET_GROUP_TYPE_FILLED_CIRCLE_SPRITE);

    platform->set_moe_dims(LIT_WIDTH, LIT_HEIGHT);
    gfx_push_view(platform->gfx, 0.f, LIT_WIDTH, 0.f, LIT_HEIGHT, 0.f, 0.f);
  }

  lit_t* lit = (lit_t*)moe->game_context;
  lit_game_t* game = &lit->game;
  lit_update_game(moe, game, platform);
  lit_render_game(moe, game, platform);

}



#endif
