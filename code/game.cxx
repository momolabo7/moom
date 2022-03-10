#include "momo.h"

#include "game_gfx.h"
#include "game_pf.h"
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

static int arr[100] = {};

static void 
test_work(void* context) {
  int* i = (int*)context;
  (*i) += 100;
}


exported B32 
game_update(Game_Memory* game,
            Game_Input* input, 
            Gfx* gfx) { 
  Platform_API pf = game->platform_api;
  F32 dt = input->seconds_since_last_frame;
  
#if 0
  // test threading
  for (int i = 0; i < array_count(arr); ++i) {
    pf.add_work(test_work, arr+i);
  }  
  pf.complete_all_work();
#endif
  // Initialization
  if (!game->state) {
    pf.set_aspect_ratio(16, 9);
    
    game->state = (Game_State*)pf.alloc(sizeof(Game_State));
    if (!game->state) return false;
    
    B32 success = init_game_assets(&game->state->game_assets, pf, gfx);
    if(!success) return false;
    
    
    // Initialize perm memory
    Sandbox_Mode* sandbox = &game->state->sandbox_mode;
    
    sandbox->tmp_delta = 0.f;
    sandbox->tmp_increase = true;
    sandbox->tmp_rot = 0.f;
    
    // Test pre-fetching bitmap
    Game_Assets* game_assets = &game->state->game_assets;
    Font_Asset_ID font_id = get_first_font(game_assets, ASSET_GROUP_FONTS);
    Font_Asset* font = get_font(game_assets, font_id);
    U16 glyph_id = font->codepoint_map[65];
    auto* glyph = font->glyphs + glyph_id;
    Bitmap_Asset_ID bitmap_id = glyph->bitmap_id;
    load_bitmap(game_assets, gfx, pf, bitmap_id);
    pf.complete_all_work();
    
  }
  Sandbox_Mode* sandbox = &game->state->sandbox_mode;
  
  if (is_poked(input->button_up)) {
    pf.hot_reload();
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
    if (sandbox->tmp_increase)
      sandbox->tmp_delta += dt; 
    else
      sandbox->tmp_delta -= dt;
    
    if (sandbox->tmp_delta >= 1.f ){
      sandbox->tmp_delta = 1.f;
      sandbox->tmp_increase = false;
    }
    
    if (sandbox->tmp_delta <= 0.f) {
      sandbox->tmp_delta = 0.f;
      sandbox->tmp_increase = true;
    }
    
    RGBA colors = create_rgba(1.f, 1.f, 1.f, 1.f);
#if 0
    HSL hsl = create_hsl(sandbox->tmp_delta, 1.f, 0.5f);
    colors.rgb = hsl_to_rgb(hsl);
#endif
    
    M44 s = create_m44_scale(600.f, 600.f, 10.f);
    M44 r = create_m44_rotation_z(sandbox->tmp_rot += dt);
    M44 t = create_m44_translation(800.f, 450.f, 300.f);
    
#if 0
    {
      Game_Assets* game_assets = &sandbox->game_assets;
      
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
      Game_Assets* game_assets = &sandbox->game_assets;
      Asset_Bitmap_ID bitmap_id = get_first_bitmap(game_assets, ASSET_GROUP_ATLASES);
      Asset_Bitmap* bitmap = get_bitmap(game_assets, bitmap_id);
      draw_sprite(gfx, colors, t*r*s, bitmap->gfx_bitmap_id);
      
    }
#endif
    {
      Game_Assets* game_assets = &game->state->game_assets;
      Font_Asset_ID font_id = get_first_font(game_assets, ASSET_GROUP_FONTS);
      Font_Asset* font = get_font(game_assets, font_id);
      
      U16 glyph_id = font->codepoint_map[65];
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
