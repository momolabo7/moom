#ifndef MOE_GAME_API
#define MOE_GAME_API

//
// moe's API for the game layer
//

static void
moe_painter_draw_sprite(gfx_t* gfx, assets_t* assets, asset_sprite_id_t sprite_id, v2f_t pos, v2f_t size, rgba_t color = rgba_set(1.f,1.f,1.f,1.f))
{
  asset_sprite_t* sprite = get_sprite(assets, sprite_id);
  asset_bitmap_t* bitmap = get_bitmap(assets, sprite->bitmap_asset_id);
  v2f_t anchor = v2f_set(0.5f, 0.5f); 
  
  gfx_push_sprite(gfx, 
                  color,
                  pos, size, anchor,
                  bitmap->renderer_texture_handle, 
                  sprite->texel_x0,
                  sprite->texel_y0,
                  sprite->texel_x1,
                  sprite->texel_y1);
}


static void
moe_painter_draw_text(gfx_t* gfx, assets_t* assets, asset_font_id_t font_id, str8_t str, rgba_t color, f32_t px, f32_t py, f32_t font_height) 
{

  asset_font_t* font = get_font(assets, font_id);
  for(u32_t char_index = 0; 
      char_index < str.count;
      ++char_index) 
  {
    u32_t curr_cp = str.e[char_index];

    if (char_index > 0) {
      u32_t prev_cp = str.e[char_index-1];
      asset_font_glyph_t *prev_glyph = get_glyph(font, prev_cp);

      f32_t kerning = get_kerning(font, prev_cp, curr_cp);
      f32_t advance = prev_glyph->horizontal_advance;
      px += (kerning + advance) * font_height;
    }

    asset_font_glyph_t *glyph = get_glyph(font, curr_cp);
    asset_bitmap_t* bitmap = get_bitmap(assets, glyph->bitmap_asset_id);
    f32_t width = (glyph->box_x1 - glyph->box_x0)*font_height;
    f32_t height = (glyph->box_y1 - glyph->box_y0)*font_height;
    
    v2f_t pos = v2f_set(px + (glyph->box_x0*font_height), py + (glyph->box_y0*font_height));
    v2f_t size = v2f_set(width, height);
    v2f_t anchor = v2f_set(0.f, 0.f); // bottom left
    gfx_push_sprite(gfx, 
                    color,
                    pos, size, anchor,
                    bitmap->renderer_texture_handle, 
                    glyph->texel_x0,
                    glyph->texel_y0,
                    glyph->texel_x1,
                    glyph->texel_y1);
  }
  
}

static void
moe_painter_draw_text_center_aligned(gfx_t* gfx, assets_t* assets, asset_font_id_t font_id, str8_t str, rgba_t color, f32_t px, f32_t py, f32_t font_height) 
{
  asset_font_t* font = get_font(assets, font_id);
  
  // Calculate the total width of the text
  f32_t offset = 0.f;
  for(u32_t char_index = 1; 
      char_index < str.count;
      ++char_index)
  {

    u32_t curr_cp = str.e[char_index];
    u32_t prev_cp = str.e[char_index-1];

    asset_font_glyph_t *prev_glyph = get_glyph(font, prev_cp);
    asset_font_glyph_t *curr_glyph = get_glyph(font, curr_cp);

    f32_t kerning = get_kerning(font, prev_cp, curr_cp);
    f32_t advance = prev_glyph->horizontal_advance;
    offset += (kerning + advance) * font_height;
    if (char_index == str.count-1) {
      offset += advance * font_height;
    }
  }
  px -= offset/2 ;

  for(u32_t char_index = 0; 
      char_index < str.count;
      ++char_index) 
  {
    u32_t curr_cp = str.e[char_index];

    if (char_index > 0) {
      u32_t prev_cp = str.e[char_index-1];
      asset_font_glyph_t *prev_glyph = get_glyph(font, prev_cp);

      f32_t kerning = get_kerning(font, prev_cp, curr_cp);
      f32_t advance = prev_glyph->horizontal_advance;
      px += (kerning + advance) * font_height;
    }

    asset_font_glyph_t *glyph = get_glyph(font, curr_cp);
    asset_bitmap_t* bitmap = get_bitmap(assets, glyph->bitmap_asset_id);
    f32_t width = (glyph->box_x1 - glyph->box_x0)*font_height;
    f32_t height = (glyph->box_y1 - glyph->box_y0)*font_height;
    
    v2f_t pos = v2f_set(px + (glyph->box_x0*font_height), py + (glyph->box_y0*font_height));
    v2f_t size = v2f_set(width, height);
    v2f_t anchor = v2f_set(0.f, 0.f); // bottom left
    gfx_push_sprite(gfx, 
                    color,
                    pos, size, anchor,
                    bitmap->renderer_texture_handle, 
                    glyph->texel_x0,
                    glyph->texel_y0,
                    glyph->texel_x1,
                    glyph->texel_y1);
  }

}


#endif // MOE_GAME_API
