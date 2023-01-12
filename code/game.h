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
  }

  lit_t* lit = (lit_t*)moe->game_context;
  if (platform->reloaded) {
    lit_init(moe, lit, platform);
  }
  lit_tick(moe, lit, platform);
}



#endif
