/* date = June 8th 2022 6:27 pm */

#ifndef MOE_PAINTER_H
#define MOE_PAINTER_H


static void
paint_sprite(moe_t* moe,
             asset_sprite_id_t sprite_id,
             v2f_t pos,
             v2f_t size,
             rgba_t color = rgba_set(1.f,1.f,1.f,1.f))
{
  assets_t* assets = &moe->assets;
  Platform* platform = moe->platform;

  asset_sprite_t* sprite = get_sprite(assets, sprite_id);
  asset_bitmap_t* bitmap = get_bitmap(assets, sprite->bitmap_asset_id);
  v2f_t anchor = {0.5f, 0.5f}; 
  
  gfx_push_sprite(platform->gfx, 
                  color,
                  pos, size, anchor,
                  bitmap->renderer_texture_handle, 
                  sprite->texel_x0,
                  sprite->texel_y0,
                  sprite->texel_x1,
                  sprite->texel_y1);
}


static void
paint_text(moe_t* moe,
           asset_font_id_t font_id,
           str8_t str,
           rgba_t color,
           f32_t px, f32_t py,
           f32_t font_height) 
{
  assets_t* assets = &moe->assets;
  Platform* platform = moe->platform;

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
    f32_t width = (glyph->box.max.x - glyph->box.min.x)*font_height;
    f32_t height = (glyph->box.max.y - glyph->box.min.y)*font_height;
    
    v2f_t pos = { px + (glyph->box.min.x*font_height), py + (glyph->box.min.y*font_height)};
    v2f_t size = { width, height };
    v2f_t anchor = {0.f, 0.f}; // bottom left
    gfx_push_sprite(platform->gfx, 
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
paint_text_center_aligned(moe_t* moe,
                          asset_font_id_t font_id,
                          str8_t str,
                          rgba_t color,
                          f32_t px, f32_t py,
                          f32_t font_height) 
{
  assets_t* assets = &moe->assets;
  Platform* platform = moe->platform;
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

    f32_t kerning = get_kerning(font, prev_cp, curr_cp);
    f32_t advance = prev_glyph->horizontal_advance;
    offset += (kerning + advance) * font_height;
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
    f32_t width = (glyph->box.max.x - glyph->box.min.x)*font_height;
    f32_t height = (glyph->box.max.y - glyph->box.min.y)*font_height;
    
    v2f_t pos = { px + (glyph->box.min.x*font_height), py + (glyph->box.min.y*font_height)};
    v2f_t size = { width, height };
    v2f_t anchor = {0.f, 0.f}; // bottom left
    gfx_push_sprite(platform->gfx, 
                    color,
                    pos, size, anchor,
                    bitmap->renderer_texture_handle, 
                    glyph->texel_x0,
                    glyph->texel_y0,
                    glyph->texel_x1,
                    glyph->texel_y1);
  }

}







#endif //MOE_PAINTER_H
