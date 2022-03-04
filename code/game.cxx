#include "momo.h"

#include "game_gfx.h"
#include "game_pf.h"
#include "game_assets.h"

struct PermanentMemory {
  F32 tmp_delta;
  B32 tmp_increase;
  F32 tmp_rot;
  
  Game_Assets game_assets;
};

struct GameMemory {
  PermanentMemory* perm;  
};


exported B32 
game_update(Game* game, Platform* pf, Input* input, Gfx* gfx, F32 dt) { 
  // Initialization
  if (!game->game_data) {
    pf->set_aspect_ratio(16, 9);
    
    // TODO(Momo): free allocated memory
    game->game_data = pf->alloc(sizeof(GameMemory));
    if (!game->game_data) return false;
    
    auto* game_memory = (GameMemory*)game->game_data;
    game_memory->perm = (PermanentMemory*)pf->alloc(sizeof(PermanentMemory));
    if (!game_memory->perm) return false;
    
    // Initialize perm memory
    PermanentMemory* perm = game_memory->perm;
    
    // TODO(Momo): for now...
    perm->game_assets = create_assets(pf, gfx);
    perm->tmp_delta = 0.f;
    perm->tmp_increase = true;
    perm->tmp_rot = 0.f;
    
    
    
  }
  
  GameMemory* game_memory = (GameMemory*)game->game_data;
  PermanentMemory* perm = game_memory->perm;
  
  if (is_poked(input->button_up)) {
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
    
    RGBA colors = create_rgba(1.f, 1.f, 1.f, 1.f);
#if 0
    HSL hsl = create_hsl(perm->tmp_delta, 1.f, 0.5f);
    colors.rgb = hsl_to_rgb(hsl);
#endif
    
    M44 s = create_m44_scale(600.f, 600.f, 10.f);
    M44 r = create_m44_rotation_z(perm->tmp_rot += dt);
    M44 t = create_m44_translation(800.f, 450.f, 300.f);
    
#if 0
    {
      Game_Assets* game_assets = &perm->game_assets;
      
      Asset_Vector m = {};
      Asset_Vector w = {};
      m.e[ASSET_TAG_TYPE_MOOD] = 0.6f;
      w.e[ASSET_TAG_TYPE_MOOD] = 1.f;
      
      Asset_Image_ID image_id = get_best_image(game_assets, ASSET_GROUP_BULLET, &m, &w);
      auto* image = get_image(game_assets, image_id);
      auto* bitmap = get_bitmap(game_assets, image->bitmap_id);
      
      //      draw_sprite(gfx, colors, t*r*s, bitmap_asset->gfx_bitmap_id);
      draw_subsprite(gfx, colors, t*r*s, 
                     bitmap->gfx_bitmap_id, 
                     image->uv);
    }
#endif
#if 0
    {
      Game_Assets* game_assets = &perm->game_assets;
      Asset_Bitmap_ID bitmap_id = get_first_bitmap(game_assets, ASSET_GROUP_ATLASES);
      Asset_Bitmap* bitmap = get_bitmap(game_assets, bitmap_id);
      draw_sprite(gfx, colors, t*r*s, bitmap->gfx_bitmap_id);
      
    }
#endif
    {
      Game_Assets* game_assets = &perm->game_assets;
      Font_Asset_ID font_id = get_first_font(game_assets, ASSET_GROUP_FONTS);
      Font_Asset* font = get_font(game_assets, font_id);
      
      U16 glyph_id = font->codepoint_map[70];
      auto* glyph = font->glyphs + glyph_id;
      Bitmap_Asset_ID bitmap_id = glyph->bitmap_id;
      
      Bitmap_Asset* bitmap = get_bitmap(game_assets, bitmap_id);
      draw_subsprite(gfx, colors, t*r*s, 
                     bitmap->gfx_bitmap_id,
                     glyph->uv);
    }
  }
  
  return true;
  
  
}
