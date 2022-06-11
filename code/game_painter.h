/* date = June 8th 2022 6:27 pm */

#ifndef GAME_PAINTER_H
#define GAME_PAINTER_H



struct Painter {
  Game_Assets* ga;
  Renderer_Command_Queue* cmds;
  F32 current_depth;
};


static void
begin_painting(Painter* p, 
               Game_Assets* ga, 
               Renderer_Command_Queue* cmds,
               F32 canvas_width,
               F32 canvas_height,
               U32 max_layers = 10000) 
{
  p->ga = ga;
  p->cmds = cmds;
  p->current_depth = (F32)max_layers;
  
  // Set camera
  {
    V3 position = {};
    Rect3 frustum = {};
    
    frustum.min.x = 0.f;
    frustum.min.y = 0.f;
    frustum.max.z = 0.f;
    frustum.max.x = canvas_width;
    frustum.max.y = canvas_height;
    frustum.max.z = p->current_depth + 1.f;
    
    push_orthographic_camera(cmds, position, frustum);
  }
  
}

static F32
advance_depth(Painter* p) {
  F32 ret = p->current_depth;
  p->current_depth -= 1.f;
  return ret;
}

static void
paint_sprite(Painter* p,
             Sprite_ID sprite_id,
             V2 pos,
             V2 size,
             RGBA color = {1.f,1.f,1.f,1.f})
{
  M44 transform = m44_identity();
  transform.e[0][0] = size.w;
  transform.e[1][1] = size.h;
  transform.e[0][3] = pos.x;
  transform.e[1][3] = pos.y;
  transform.e[2][3] = p->current_depth;
  
  
  Sprite_Asset* sprite = get_sprite(p->ga, sprite_id);
  Bitmap_Asset* bitmap = get_bitmap(p->ga, sprite->bitmap_id);
  push_subsprite(p->cmds, 
                 color,
                 transform,
                 bitmap->renderer_texture_handle, 
                 sprite->uv);
}


static void
paint_text(Painter* p,
           Font_ID font_id,
           String str,
           RGBA color,
           F32 px, F32 py,
           F32 font_height) 
{
  F32 depth = p->current_depth;
  Font_Asset* font = get_font(p->ga, font_id);
  Bitmap_Asset* bitmap = get_bitmap(p->ga, font->bitmap_id);
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
    
    // TODO: combine matrix
    M44 a = m44_translation(0.5f, 0.5f);
    M44 s = m44_scale(width, height, 1.f);
    M44 t = m44_translation(px + (glyph->box.min.x*font_height), 
                            py + (glyph->box.min.y*font_height), 
                            depth);
    M44 transform = t*s*a;
    
    
    
    push_subsprite(p->cmds, 
                   color,
                   transform,
                   bitmap->renderer_texture_handle, 
                   glyph->uv);
  }
  
}


static void
paint_line(Painter* p,
           Line2 line,
           F32 thickness,
           RGBA color = {1.f,1.f,1.f,1.f})
{
  push_line(p->cmds, 
            line, 
            thickness, 
            color, 
            p->current_depth); 
  
}


static void
paint_circle(Painter* p,
             Circ2 circle,
             F32 thickness, 
             U32 line_count,
             RGBA color) 
{
  push_circle(p->cmds, 
              circle,
              thickness,
              line_count,
              color,
              p->current_depth);
}

static void
paint_triangle(Painter* p,
               RGBA colors,
               V2 p0, V2 p1, V2 p2) 
{
  push_triangle(p->cmds, 
                colors,
                p0, p1, p2,
                p->current_depth);
}

#endif //GAME_PAINTER_H
