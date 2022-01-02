#include "momo.h"

#include "game_gfx.h"
#include "game_platform.h"

exported Game_Info
Game_GetInfo() {
  Game_Info ret;
  ret.game_design_width = 1600;
  ret.game_design_height = 900;
  return ret;
}



exported B32
Game_Update(Gfx* gfx, F32 dt) {
  return false;
}
