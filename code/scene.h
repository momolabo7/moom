#ifndef SCENE_H
#define SCENE_H

#include "scene_lit.h"


static void 
game_init(moe_t* moe) {
  assets_t* assets = &moe->assets;
  platform_t* platform = moe->platform;

  lit_t* lit = moe_allocate_scene(lit_t, moe);
  lit_load_level(lit, 0); 
  rng_init(&lit->rng, 65535); // don't really need to be strict 

  {
    make(asset_match_t, match);
    set_match_entry(match, ASSET_TAG_TYPE_FONT, 0.f, 1.f);
    lit->tutorial_font = find_best_font(assets, ASSET_GROUP_TYPE_FONTS, match);
  }

  lit->blank_sprite = find_first_sprite(assets, ASSET_GROUP_TYPE_BLANK_SPRITE);
  lit->circle_sprite = find_first_sprite(assets, ASSET_GROUP_TYPE_CIRCLE_SPRITE);
  lit->filled_circle_sprite = find_first_sprite(assets, ASSET_GROUP_TYPE_FILLED_CIRCLE_SPRITE);

  platform->set_moe_dims(LIT_WIDTH, LIT_HEIGHT);
  gfx_push_view(platform->gfx, 0.f, LIT_WIDTH, 0.f, LIT_HEIGHT, 0.f, 0.f);
}

static void 
game_tick(moe_t* moe) 
{
  lit_t* lit = (lit_t*)moe->scene_context;
  assets_t* assets = &moe->assets;
  platform_t* platform = moe->platform;

  lit_update_game(moe, lit, platform);
  lit_render_game(moe, lit, platform);

}

static void 
game_exit(moe_t* moe) {
}


#endif
