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
               Renderer_Command_Queue* cmds) 
{
  p->ga = ga;
  p->cmds = cmds;
  
  // TODO: remove hard code
  p->current_depth = 498.f;
}

static F32
get_and_advance_depth(Painter* p) {
  F32 ret = p->current_depth;
  
  // TODO: remove hard code
  p->current_depth -= 0.1f;
  
  return ret;
}

static void
paint_sprite(Painter* p,
             Sprite_ID sprite_id,
             F32 px, F32 py, 
             F32 sw, F32 sh, 
             RGBA color = {1.f,1.f,1.f,1.f})
{
  
  M44 transform = m44_identity();
  transform.e[0][0] = sw;
  transform.e[1][1] = sh;
  transform.e[0][3] = px;
  transform.e[1][3] = py;
  transform.e[2][3] = get_and_advance_depth(p);
  
  
  Sprite_Asset* sprite = get_sprite(p->ga, sprite_id);
  Bitmap_Asset* bitmap = get_bitmap(p->ga, sprite->bitmap_id);
  push_subsprite(p->cmds, 
                 color,
                 transform,
                 bitmap->renderer_texture_handle, 
                 sprite->uv);
}
static void
paint_sprite(Painter* p,
             Sprite_ID sprite_id,
             V2 pos,
             V2 size,
             RGBA color = {1.f,1.f,1.f,1.f})
{
	paint_sprite(p, 
               sprite_id,
               pos.x, pos.y,
               size.x, size.y,
               color);
}

static void
paint_text(Painter* p,
           Font_ID font_id,
           String str,
           RGBA color,
           F32 px, F32 py,
           F32 font_height) 
{
  F32 depth = get_and_advance_depth(p);
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
    
    
    M44 transform = 
      m44_translation(px + (glyph->box.min.x*font_height), 
                      py + (glyph->box.min.y*font_height), 
                      depth)*
      m44_scale(width, height, 1.f)*
      m44_translation(0.5f, 0.5f, 1.f);
    
    
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
            get_and_advance_depth(p)); 
  
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
              get_and_advance_depth(p));
}

static void
paint_triangle(Painter* p,
               RGBA colors,
               V2 p0, V2 p1, V2 p2) 
{
  push_triangle(p->cmds, 
                colors,
                p0, p1, p2,
                get_and_advance_depth(p));
}

#endif //GAME_PAINTER_H
