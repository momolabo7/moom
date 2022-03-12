#include "momo.h"

#include "game.h"




Platform_API g_platform;

exported B32 
game_update(Game_Memory* memory,
            Game_Input* input, 
            Game_Render_Commands* render_commands) { 
  g_platform = memory->platform_api;
  F32 dt = input->seconds_since_last_frame;
  
  // Initialization
  if (!memory->state) {
    g_platform.set_aspect_ratio(16, 9);
    
    memory->state = (Game_State*)g_platform.alloc(sizeof(Game_State));
    if (!memory->state) return false;
    
    B32 success = init_game_assets(&memory->state->game_assets, memory->texture_queue);
    if(!success) return false;
    
    
    // Initialize perm memory
    Sandbox_Mode* sandbox = &memory->state->sandbox_mode;
    
    sandbox->tmp_delta = 0.f;
    sandbox->tmp_increase = true;
    sandbox->tmp_rot = 0.f;
    
    // Test pre-fetching bitmap
    Game_Assets* game_assets = &memory->state->game_assets;
    Font_Asset_ID font_id = get_first_font(game_assets, ASSET_GROUP_FONTS);
    Font_Asset* font = get_font(game_assets, font_id);
    U16 glyph_id = font->codepoint_map[65];
    auto* glyph = font->glyphs + glyph_id;
    Bitmap_Asset_ID bitmap_id = glyph->bitmap_id;
    load_bitmap(game_assets, bitmap_id);
    g_platform.complete_all_work();
    
  }
  Sandbox_Mode* sandbox = &memory->state->sandbox_mode;
  
  if (is_poked(input->button_up)) {
    g_platform.hot_reload();
  }
  
  
  // Clear colors
  {
    RGBA colors;
    colors.r = colors.g = colors.b  = colors.a = 0.3f;
    push_colors(render_commands, colors);
  }
  
  // Set camera
  {
    V3 position = {};
    Rect3 frustum;
    frustum.min.x = frustum.min.y = frustum.min.z = 0;
    frustum.max.x = 1600;
    frustum.max.y = 900;
    frustum.max.z = 500;
    push_orthographic_camera(render_commands, position, frustum);
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
    M44 s = create_m44_scale(600.f, 600.f, 10.f);
    M44 r = create_m44_rotation_z(sandbox->tmp_rot += dt);
    M44 t = create_m44_translation(800.f, 450.f, 300.f);
    
    {
      Game_Assets* game_assets = &memory->state->game_assets;
      Font_Asset_ID font_id = get_first_font(game_assets, ASSET_GROUP_FONTS);
      Font_Asset* font = get_font(game_assets, font_id);
      
      U16 glyph_id = font->codepoint_map[68];
      auto* glyph = font->glyphs + glyph_id;
      Bitmap_Asset_ID bitmap_id = glyph->bitmap_id;
      
      Bitmap_Asset* bitmap = get_bitmap(game_assets, bitmap_id);
      push_subsprite(render_commands, 
                     colors, t*r*s, 
                     bitmap->renderer_bitmap_id,
                     glyph->uv);
    }
  }
  
  return true;
  
}