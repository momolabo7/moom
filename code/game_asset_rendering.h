/* date = June 8th 2022 6:27 pm */

#ifndef GAME_PAINTER_H
#define GAME_PAINTER_H


static void
paint_sprite(Game_Sprite_ID sprite_id,
             V2 pos,
             V2 size,
             RGBA color = rgba(1.f,1.f,1.f,1.f))
{
  Game_Sprite* sprite = get_sprite(assets, sprite_id);
  Game_Bitmap* bitmap = get_bitmap(assets, sprite->bitmap_asset_id);
  V2 anchor = {0.5f, 0.5f}; 
  
  gfx_push_sprite(gfx, 
                  color,
                  pos, size, anchor,
                  bitmap->renderer_texture_handle, 
                  sprite->texel_uv);
}


static void
paint_text(Game_Font_ID font_id,
           String8 str,
           RGBA color,
           F32 px, F32 py,
           F32 font_height) 
{
  Game_Font* font = get_font(assets, font_id);
  for(U32 char_index = 0; 
      char_index < str.count;
      ++char_index) 
  {
    U32 curr_cp = str.e[char_index];

    if (char_index > 0) {
      U32 prev_cp = str.e[char_index-1];
      Game_Font_Glyph *prev_glyph = get_glyph(font, prev_cp);

      F32 kerning = get_kerning(font, prev_cp, curr_cp);
      F32 advance = prev_glyph->horizontal_advance;
      px += (kerning + advance) * font_height;
    }

    Game_Font_Glyph *glyph = get_glyph(font, curr_cp);
    Game_Bitmap* bitmap = get_bitmap(assets, glyph->bitmap_asset_id);
    F32 width = (glyph->box.max.x - glyph->box.min.x)*font_height;
    F32 height = (glyph->box.max.y - glyph->box.min.y)*font_height;
    
    V2 pos = { px + (glyph->box.min.x*font_height), py + (glyph->box.min.y*font_height)};
    V2 size = { width, height };
    V2 anchor = {0.f, 0.f}; // bottom left
    gfx_push_sprite(gfx, 
                    color,
                    pos, size, anchor,
                    bitmap->renderer_texture_handle, 
                    glyph->texel_uv);
  }
  
}

static void
paint_text_center_aligned(Game_Font_ID font_id,
                          String8 str,
                          RGBA color,
                          F32 px, F32 py,
                          F32 font_height) 
{
  Game_Font* font = get_font(assets, font_id);

  
  // Calculate the total width of the text
  F32 offset = 0.f;
  for(U32 char_index = 1; 
      char_index < str.count;
      ++char_index)
  {
    U32 curr_cp = str.e[char_index];
    U32 prev_cp = str.e[char_index-1];
    F32 advance = get_kerning(font, prev_cp, curr_cp);
    offset += advance * font_height;
  }

  px -= offset ;

  for(U32 char_index = 0; 
      char_index < str.count;
      ++char_index) 
  {
    
    U32 curr_cp = str.e[char_index];
    if (char_index > 0) {
      U32 prev_cp = str.e[char_index-1];
      F32 advance = get_kerning(font, prev_cp, curr_cp);
      px += advance * font_height;
    }
    Game_Font_Glyph *glyph = get_glyph(font, curr_cp);
    Game_Bitmap* bitmap = get_bitmap(assets, glyph->bitmap_asset_id);

    F32 width = (glyph->box.max.x - glyph->box.min.x)*font_height;
    F32 height = (glyph->box.max.y - glyph->box.min.y)*font_height;
    
    V2 pos = v2_set(px + (glyph->box.min.x*font_height), py + (glyph->box.min.y*font_height));
    V2 size = v2_set(width, height);
    V2 anchor = v2_set(0.f, 0.f); // bottom left
    gfx_push_sprite(gfx, 
                    color,
                    pos, size, anchor,
                    bitmap->renderer_texture_handle, 
                    glyph->texel_uv);
  }
  
}







#endif //GAME_PAINTER_H
