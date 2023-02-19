#ifndef GAME_H
#define GAME_H

#include "lit.h"



static void 
game_tick(moe_t* moe) 
{
  

  assets_t* assets = &moe->assets;
  platform_t* platform = moe->platform;

  if(moe->game_context == nullptr) {
    auto* lit_memory = platform->allocate_memory(sizeof(lit_t));
    moe->game_context = lit_memory;

    auto* lit = (lit_t*)((platform_memory_t*)moe->game_context)->data;
    lit_init(moe, lit, platform);
  }

  auto* lit = (lit_t*)((platform_memory_t*)moe->game_context)->data;
  lit_tick(moe, lit, platform);
}



#endif
