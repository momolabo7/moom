/* date = March 11th 2022 6:38 pm */

#ifndef GAME_H
#define GAME_H

#include "game_platform.h"
#include "game_renderer.h"
#include "game_assets.h"

struct Sandbox_Mode {
  F32 tmp_delta;
  B32 tmp_increase;
  F32 tmp_rot;
  
};

struct Game_State {
  union {
    Sandbox_Mode sandbox_mode;  
  };
  Game_Assets game_assets;
};



#endif //GAME_H
