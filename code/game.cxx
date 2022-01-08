#include "momo.h"

#include "game_gfx.h"
#include "game_pf.h"


static F32 tmp_delta = 0.f;
static B32 tmp_increase = true;
static F32 tmp_rot = 0.f;
static B32 first_time = false;


static void* game_memory = nullptr;



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
  
  
  
  if (IsPoked(input->button_up)) {
    pf->hot_reload();
  }
  
  {
    RGBA colors;
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
    
    RGBA colors = CreateRGBA(0.f, 0.f, 0.f, 1.f);
    HSL hsl = CreateHSL(tmp_delta, 1.f, 0.5f);
    colors.rgb = ToRGB(hsl);
    
    M44 scale = M44_Scale(600.f, 600.f, 10.f);
    M44 rot = M44_RotationZ(tmp_rot += (dt));
    M44 trans = M44_Translation(800.f, 450.f, 300.f);
    Gfx_DrawSprite(gfx, colors, trans*scale*rot, 0);
  }
  
  return false;
  
  
}
