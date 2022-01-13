#include "momo.h"

#include "game_gfx.h"
#include "game_pf.h"

struct PermanentMemory {
  F32 tmp_delta;
  B32 tmp_increase;
  F32 tmp_rot;
  
  
};

struct GameMemory {
  PermanentMemory* perm;  
};


// TODO(Momo): Game should probably return a status.
exported B32 
Game_Update(Game* game, Platform* pf, Input* input, Gfx* gfx, F32 dt) { 
  // Initialization
  U8 test[10];
  
  if (!game->game_data) {
    // TODO(Momo): free allocated memory
    game->game_data = pf->alloc(sizeof(GameMemory));
    if (!game->game_data) return false;
    
    auto* game_memory = (GameMemory*)game->game_data;
    game_memory->perm = (PermanentMemory*)pf->alloc(sizeof(PermanentMemory));
    if (!game_memory->perm) return false;
    
    PermanentMemory* perm = game_memory->perm;
    
    // Initialize perm memory
    perm->tmp_delta = 0.f;
    perm->tmp_increase = true;
    perm->tmp_rot = 0.f;
    
    // Set aspect ratio of the game
    pf->set_aspect_ratio(16, 9);
    
    Platform_File file = pf->open_file("test.txt", 
                                       Platform_FileAccess_Read, 
                                       Platform_FilePath_Executable);
    pf->read_file(file, 5, 0, test);
  }
  
  GameMemory* game_memory = (GameMemory*)game->game_data;
  PermanentMemory* perm = game_memory->perm;
  
  if (IsPoked(input->button_up)) {
    pf->hot_reload();
  }
  
  {
    RGBA colors;
    colors.r = colors.g = colors.b  = colors.a = 0.3f;
    Clear(gfx, colors);
  }
  
  {
    V3F32 position = {0};
    Rect3F32 frustum;
    frustum.min.x = frustum.min.y = frustum.min.z = 0;
    frustum.max.x = 1600;
    frustum.max.y = 900;
    frustum.max.z = 500;
    SetOrthoCamera(gfx, position, frustum);
  }
  
  {
    if (perm->tmp_increase)
      perm->tmp_delta += dt; 
    else
      perm->tmp_delta -= dt;
    
    if (perm->tmp_delta >= 1.f ){
      perm->tmp_delta = 1.f;
      perm->tmp_increase = false;
    }
    
    if (perm->tmp_delta <= 0.f) {
      perm->tmp_delta = 0.f;
      perm->tmp_increase = true;
    }
    
    RGBA colors = CreateRGBA(0.f, 0.f, 0.f, 1.f);
    HSL hsl = CreateHSL(perm->tmp_delta, 1.f, 0.5f);
    colors.rgb = ToRGB(hsl);
    
    M44 scale = CreateScaleM44(600.f, 600.f, 10.f);
    M44 rot = CreateRotationZM44(perm->tmp_rot += (0.5f*dt));
    M44 trans = CreateTranslationM44(800.f, 450.f, 300.f);
    DrawSprite(gfx, colors, trans*scale*rot, 0);
  }
  
  return false;
  
  
}
