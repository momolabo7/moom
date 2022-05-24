/* date = April 8th 2022 9:02 pm */

#ifndef GAME_ASSETS_RENDER_H
#define GAME_ASSETS_RENDER_H

// TODO: Rotation
static void
draw_sprite(Game_Assets* ga,
            Renderer_Command_Queue* render_commands,
            Sprite_ID sprite_id,
            F32 px, F32 py, 
            F32 sw, F32 sh, 
            F32 depth,
            RGBA color = {1.f,1.f,1.f,1.f})
{
  
  M44 transform = m44_identity();
  transform.e[0][0] = sw;
  transform.e[1][1] = sh;
  transform.e[0][3] = px;
  transform.e[1][3] = py;
  transform.e[2][3] = depth;
  
  
  Sprite_Asset* sprite = get_sprite(ga, sprite_id);
  Bitmap_Asset* bitmap = get_bitmap(ga, sprite->bitmap_id);
  push_subsprite(render_commands, 
                 color,
                 transform,
                 bitmap->renderer_texture_handle, 
                 sprite->uv);
}

static void
draw_sprite(Game_Assets* ga,
            Renderer_Command_Queue* render_commands,
            Sprite_ID sprite_id,
            V2 pos,
            V2 size,
            F32 depth,
            RGBA color = {1.f,1.f,1.f,1.f})
{
	draw_sprite(ga, render_commands,
              sprite_id,
              pos.x, pos.y,
              size.x, size.y,
              depth, color);
}



static void
draw_text(Game_Assets* ga, 
          Renderer_Command_Queue* render_commands,
          Font_ID font_id,
          String str,
          RGBA color,
          F32 px, F32 py,
          F32 font_height,
          F32 depth) 
{
  Font_Asset* font = get_font(ga, font_id);
  Bitmap_Asset* bitmap = get_bitmap(ga, font->bitmap_id);
  for(U32 char_index = 0; 
      char_index < str.count;
      ++char_index) 
  {
    U32 curr_cp = str.e[char_index];
    if (char_index > 0) {
      U32 prev_cp = str.e[char_index-1];
      px += get_horizontal_advance(font, prev_cp, curr_cp)*font_height;
    }
    Font_Glyph_Asset *glyph = get_glyph(font, curr_cp);
    
    F32 width = (glyph->box.max.x - glyph->box.min.x)*font_height;
    F32 height = (glyph->box.max.y - glyph->box.min.y)*font_height;
    
    
    M44 transform = 
      m44_translation(px + (glyph->box.min.x*font_height), 
                      py + (glyph->box.min.y*font_height), 
                      depth)*
      m44_scale(width, height, 1.f)*
      m44_translation(0.5f, 0.5f, 1.f);
    
    
    push_subsprite(render_commands, 
                   color,
                   transform,
                   bitmap->renderer_texture_handle, 
                   glyph->uv);
  }
  
}


#endif //GAME_ASSETS_RENDER_H
