/* date = April 1st 2022 5:36 pm */

#ifndef GAME_DEBUG_H
#define GAME_DEBUG_H


struct Console_Command {
  String key;
  void* ctx;
  void (*func)(void*);
};

struct Console_Line {
  U8 buffer[256];
  String str;
};

struct Debug_Console {
  B32 is_showing;
  
  U32 command_count;
  Console_Command commands[10];
  
  Console_Line info_lines[5];
  
  Console_Line input_line;
};

static void
init_debug_console(Debug_Console* dc) {
  dc->is_showing = false;
  
  // REMOVE WHEN DONE
  dc->input_line.str.e = dc->input_line.buffer;
  
  for (U32 info_line_index = 0;
       info_line_index < array_count(dc->info_lines);
       ++info_line_index) 
  {
    Console_Line* info_line = dc->info_lines + info_line_index;
    info_line->str.e = info_line->buffer;
  }
}

static void
add_debug_command(Debug_Console* dc, 
                  String key, 
                  void* ctx,
                  void(*func)(void*)) {
  
  // simulate adding commands
  Console_Command* cmd = dc->commands + dc->command_count++;
  cmd->key = key;
  cmd->ctx = ctx;
  cmd->func = func;
}


static void
push(Console_Line* line, U8 c) {
  if (line->str.count < array_count(line->buffer)) {
    line->str.e[line->str.count++] = c;
  }
}

static void
pop(Console_Line* line) {
  if (line->str.count > 0) {
    line->str.count--;
  }
}

static void
clear(Console_Line* line) {
  line->str.count = 0;
}

static void
exec(Debug_Console* dc) {
  for(U32 command_index = 0; 
      command_index < dc->command_count; 
      ++command_index) 
  {
    Console_Command* cmd = dc->commands + command_index;
    if (match(cmd->key, dc->input_line.str)) {
      cmd->func(cmd->ctx);
    }
  }
  
  
  clear(&dc->input_line);
}

static void
update_debug_console(Debug_Console* dc, Game_Input* input) {
  // Update console code
  if (is_poked(input->button_console)) {
    dc->is_showing = !dc->is_showing;
  }
  
  for (U32 char_index = 0; 
       char_index < input->char_count;
       ++char_index) 
  {
    // NOTE(Momo): Not very portable to other platforms....
    U8 c = input->chars[char_index];
    if (c >= 32 && c <= 126) {
      push(&dc->input_line, c);
    }
    // backspace 
    if (c == 8) {
      pop(&dc->input_line);
    }    
    
    if (c == '\r') {
      exec(dc);
    }
  }
}


static void
render_debug_console(Debug_Console* dc, 
                     Game_Assets* ga, 
                     Game_Render_Commands* render_commands)
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
    
#if 0
    // Draw info text
    for (U32 info_line_index = 0;
         info_line_index < array_count(dc->info_lines);
         ++info_line_index)
    {
      Console_Line* info_line = dc->info_lines + info_line_index;
      
      Font_Asset* font = get_font(ga, FONT_DEFAULT);
      V2 position = {};
      const F32 font_height = 40.f;
      for(U32 char_index = 0; 
          char_index < info_line->str.count;
          ++char_index) 
      {
        U32 curr_cp = info_line->str.e[char_index];
        if (char_index > 0) {
          U32 prev_cp = info_line->str.e[char_index-1];
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
#endif
    
    // Draw input text
    {
      Font_Asset* font = get_font(ga, FONT_DEFAULT);
      V2 position = {};
      const F32 font_height = 40.f;
      for(U32 char_index = 0; 
          char_index < dc->input_line.str.count;
          ++char_index) 
      {
        U32 curr_cp = dc->input_line.str.e[char_index];
        if (char_index > 0) {
          U32 prev_cp = dc->input_line.str.e[char_index-1];
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
