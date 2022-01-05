#include "momo.h"

#include "game_gfx.h"
#include "game_pf.h"


static F32 tmp_delta = 0.f;
static B32 tmp_increase = true;
static F32 tmp_rot = 0.f;
static B32 first_time = false;
static void* game_memory = nullptr;

// TODO(Momo): this should be a function callback to 
// Platform API
exported Game_Info
Game_GetInfo() {
  Game_Info ret;
  ret.game_design_width = 1600;
  ret.game_design_height = 900;
  return ret;
}


// TODO(Momo): Game should probably return a status.
exported B32
Game_Update(PF* pf, Input* input, Gfx* gfx, F32 dt) {
  if (!first_time) {
    game_memory = pf->alloc(MB(100));
    if (!game_memory) {
      return true;
    }
    first_time = true;
  }
  
  
  
  if (Input_IsDown(input->button_up)) {
    pf->hot_reload();
  }
  
  {
    RGBAF32 colors;
    colors.r = colors.g = colors.b  = colors.a = 0.3f;
    Gfx_Clear(gfx, colors);
  }
  
  {
    V3F32 position = {0};
    Rect3F32 frustum;
    frustum.min.x = frustum.min.y = frustum.min.z = 0;
    frustum.max.x = 1600;
    frustum.max.y = 900;
    frustum.max.z = 500;
    Gfx_SetOrthoCamera(gfx, position, frustum);
  }
  
  {
    if (tmp_increase)
      tmp_delta += dt; 
    else
      tmp_delta -= dt;
    
    if (tmp_delta >= 1.f ){
      tmp_delta = 1.f;
      tmp_increase = false;
    }
    
    if (tmp_delta <= 0.f) {
      tmp_delta = 0.f;
      tmp_increase = true;
    }
    
    RGBAF32 colors = RGBAF32_Create(0.f, 0.f, 0.f, 1.f);
    HSLF32 hsl = HSLF32_Create(tmp_delta, 1.f, 0.5f);
    colors.rgb = HSLF32_ToRGBF32(hsl);
    
    M44F32 scale = M44F32_Scale(600.f, 600.f, 10.f);
    M44F32 rot = M44F32_RotationZ(tmp_rot += dt);
    M44F32 trans = M44F32_Translation(800.f, 450.f, 300.f);
    M44F32 t = M44F32_Concat(trans, M44F32_Concat(scale, rot));
    Gfx_DrawSprite(gfx, colors, t, 0);
  }
  
  return false;
  
  
}
