/* date = April 1st 2022 5:36 pm */

#ifndef GAME_DEBUG_H
#define GAME_DEBUG_H



struct Debug_Console {
  B32 is_showing;
};

static void
render_debug_console(Debug_Console* dc, Game_Assets* ga, Game_Render_Commands* render_commands)
{
  
  // Debug console
  if (dc->is_showing) {
    
    // Camera
    {
      V3 position = {};
      Rect3 frustum;
      frustum.min.x = frustum.min.y = frustum.min.z = 0;
      frustum.max.x = 1600;
      frustum.max.y = 900;
      frustum.max.z = 500;
      push_orthographic_camera(render_commands, position, frustum);
    }
    
    // Draw background
    {
      RGBA bg_color = create_rgba(0.5f, 0.5f, 0.5f, 1.f);
      M44 bgs = create_m44_scale(1600.f, 400.f, 10.f);
      M44 bgt = create_m44_translation(800.f, 200.f, 10.f);
      
      Sprite_Asset* sprite=  ga->sprites + 0;
      push_subsprite(render_commands, 
                     bg_color,
                     bgt*bgs,
                     0, 
                     sprite->uv);
    }
    
    // Draw text
    {
      Font_Asset* font = get_font(ga, FONT_DEFAULT);
      V2 position = {};
      const F32 font_height = 40.f;
      Str8 test_str = str8_from_lit("Hello World");
      for(U32 char_index = 0; 
          char_index < test_str.count;
          ++char_index) 
      {
        U32 curr_cp = test_str.e[char_index];
        if (char_index > 0) {
          U32 prev_cp = test_str.e[char_index-1];
          position.x += get_horizontal_advance(font, prev_cp, curr_cp)*font_height;
        }
        Font_Glyph_Asset *glyph = get_glyph(font, curr_cp);
        
        F32 width = (glyph->box.max.x - glyph->box.min.x)*font_height;
        F32 height = (glyph->box.max.y - glyph->box.min.y)*font_height;
        
        M44 transform = 
          create_m44_translation(position.x + (glyph->box.min.x*font_height), 
                                 position.y + (glyph->box.min.y*font_height), 
                                 9.f)*
          create_m44_scale(width, height, 1.f)*
          create_m44_translation(0.5f, 0.5f, 0.f);
        
        
        RGBA colors = create_rgba(1.f, 1.f, 1.f, 1.f);
        
        push_subsprite(render_commands, 
                       colors,
                       transform,
                       0, 
                       glyph->uv);
      }
      
      
    }
  }
}


#endif //GAME_DEBUG_H
