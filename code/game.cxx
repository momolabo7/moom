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


exported B32 
game_update(Game* game, Platform* pf, Input* input, Gfx* gfx, F32 dt) { 
  // Initialization
  if (!game->game_data) {
    // TODO(Momo): free allocated memory
    game->game_data = pf->alloc(sizeof(GameMemory));
    if (!game->game_data) return false;
    
    auto* game_memory = (GameMemory*)game->game_data;
    game_memory->perm = (PermanentMemory*)pf->alloc(sizeof(PermanentMemory));
    if (!game_memory->perm) return false;
    
    // Initialize perm memory
    PermanentMemory* perm = game_memory->perm;
    perm->tmp_delta = 0.f;
    perm->tmp_increase = true;
    perm->tmp_rot = 0.f;
    
    // Set aspect ratio of the game
    pf->set_aspect_ratio(16, 9);    
    
    
    //TODO: Load assets
    // assets->load(ASSET_NAME)
    // assets->unload(ASSET_NAME);
    
    
  }
  
  GameMemory* game_memory = (GameMemory*)game->game_data;
  PermanentMemory* perm = game_memory->perm;
  
  if (input->button_up.is_poked()) {
    pf->hot_reload();
  }
  
  
  // Clear colors
  {
    RGBA colors;
    colors.r = colors.g = colors.b  = colors.a = 0.3f;
    clear(gfx, colors);
  }
  
  // Set camera
  {
    V3 position = {};
    Rect3 frustum;
    frustum.min.x = frustum.min.y = frustum.min.z = 0;
    frustum.max.x = 1600;
    frustum.max.y = 900;
    frustum.max.z = 500;
    set_orthographic_camera(gfx, position, frustum);
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
    
    RGBA colors = create_rgba(0.f, 0.f, 0.f, 1.f);
    HSL hsl = create_hsl(perm->tmp_delta, 1.f, 0.5f);
    colors.rgb = hsl_to_rgb(hsl);
    
    M44 s = create_m44_scale(600.f, 600.f, 10.f);
    M44 r = create_m44_rotation_z(perm->tmp_rot += dt);
    M44 t = create_m44_translation(800.f, 450.f, 300.f);
    
    //draw_sprite(gfx, colors, t*r*s, 0);
    {
      
      draw_subsprite(gfx, colors, t*r*s, img.texture_id, img.uv);
    }
  }
  
  return false;
  
  
}
