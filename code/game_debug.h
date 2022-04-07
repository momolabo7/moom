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
  
  String_Builder info_lines[9];
  String_Builder input_line;
};

static void
init_debug_console(Debug_Console* dc, Arena* arena) {
  dc->is_showing = false;
  
  
  UMI line_size = 256;
  init_string_builder(&dc->input_line,
                      push_array<U8>(arena, line_size),
                      line_size);
  
  for (U32 info_line_index = 0;
       info_line_index < array_count(dc->info_lines);
       ++info_line_index) 
  {    
    String_Builder* info_line = dc->info_lines + info_line_index;
    init_string_builder(info_line,
                        push_array<U8>(arena, line_size),
                        line_size);
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
_push_info(Debug_Console* dc, String str) {
  // NOTE(Momo): There's probably a better to do with via some
  // crazy indexing scheme, but this is debug so we don't care for now
  
  // Copy everything from i + 1 from i
  for (U32 i = 0; 
       i < array_count(dc->info_lines) - 1;
       ++i)
  {
    U32 line_index = array_count(dc->info_lines) - 1 - i;
    String_Builder* line_to = dc->info_lines + line_index;
    String_Builder* line_from = dc->info_lines + line_index - 1;
    clear(line_to);
    push_string(line_to, line_from->str);
  } 
  clear(dc->info_lines + 0);
  push_string(dc->info_lines + 0, str);
}

static void
_execute(Debug_Console* dc) {
  for(U32 command_index = 0; 
      command_index < dc->command_count; 
      ++command_index) 
  {
    Console_Command* cmd = dc->commands + command_index;
    if (match(cmd->key, dc->input_line.str)) {
      cmd->func(cmd->ctx);
    }
  }
  
  _push_info(dc, dc->input_line.str);
  clear(&dc->input_line);
}

static void
update_debug_console(Debug_Console* dc, Game_Input* input) {
  // Update console code
  if (is_poked(input->button_console)) {
    dc->is_showing = !dc->is_showing;
  }
  
  if (!dc->is_showing) return;
  
  for (U32 char_index = 0; 
       char_index < input->char_count;
       ++char_index) 
  {
    // NOTE(Momo): Not very portable to other platforms....
    U8 c = input->chars[char_index];
    if (c >= 32 && c <= 126) {
      push_u8(&dc->input_line, c);
    }
    // backspace 
    if (c == 8) {
      if (dc->input_line.count > 0) 
        pop(&dc->input_line);
    }    
    
    if (c == '\r') {
      _execute(dc);
      break;
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
      // TODO(Momo): This values should come from
      // some 'design width' and 'design height' value from elsewhere.
      V3 position = {};
      Rect3 frustum;
      frustum.min.x = frustum.min.y = frustum.min.z = 0;
      frustum.max.x = 1600;
      frustum.max.y = 900;
      frustum.max.z = 500;
      push_orthographic_camera(render_commands, position, frustum);
    }
    
    // TODO(Momo): These should be percentages
    // but we'll work with this for now
    const F32 console_width = 1600.f;
    const F32 console_height = 400.f;
    const F32 line_height = console_height/(array_count(dc->info_lines)+1);
    
    // Draw background
#if 0
    {
      RGBA bg_color = create_rgba(0.5f, 0.5f, 0.5f, 1.f);
      M44 bgs = create_m44_scale(console_width, console_height, 10.f);
      M44 bgt = create_m44_translation(console_width/2, console_height/2, 10.f);
      
      // Blank sprite
      Sprite_Asset* sprite =  ga->sprites + 0;
      push_subsprite(render_commands, 
                     bg_color,
                     bgt*bgs,
                     0, 
                     sprite->uv);
    }
#endif
    
    draw_rect(ga, render_commands,
              create_rgba(0x787878FF),
              console_width/2,
              console_height/2,
              console_width, 
              console_height,
              100.f);
    
    draw_rect(ga, render_commands,
              create_rgba(0x505050FF),
              console_width/2,
              line_height/2,
              console_width,
              line_height,
              90.f);
    
    
    // Draw info text
    for (U32 line_index = 0;
         line_index < array_count(dc->info_lines);
         ++line_index)
    {
      String_Builder* line = dc->info_lines + line_index;
      
      Font_Asset* font = get_font(ga, FONT_DEFAULT);
      V2 position = {};
      position.x = 0;
      position.y = (line_height*(line_index+1));
      
      for(U32 char_index = 0; 
          char_index < line->str.count;
          ++char_index) 
      {
        U32 curr_cp = line->str.e[char_index];
        if (char_index > 0) {
          U32 prev_cp = line->str.e[char_index-1];
          position.x += get_horizontal_advance(font, prev_cp, curr_cp)*line_height;
        }
        Font_Glyph_Asset *glyph = get_glyph(font, curr_cp);
        
        F32 width = (glyph->box.max.x - glyph->box.min.x)*line_height;
        F32 height = (glyph->box.max.y - glyph->box.min.y)*line_height;
        
        M44 transform = 
          create_m44_translation(position.x + (glyph->box.min.x*line_height), 
                                 position.y + (glyph->box.min.y*line_height), 
                                 9.f)*
          create_m44_scale(width, height, 1.f)*
          create_m44_translation(0.5f, 0.5f, 1.f);
        
        
        RGBA colors = create_rgba(1.f, 1.f, 1.f, 1.f);
        
        push_subsprite(render_commands, 
                       colors,
                       transform,
                       0, 
                       glyph->uv);
      }
    }
    
    // Draw input text
    {
      Font_Asset* font = get_font(ga, FONT_DEFAULT);
      V2 position = {};
      for(U32 char_index = 0; 
          char_index < dc->input_line.str.count;
          ++char_index) 
      {
        U32 curr_cp = dc->input_line.str.e[char_index];
        if (char_index > 0) {
          U32 prev_cp = dc->input_line.str.e[char_index-1];
          position.x += get_horizontal_advance(font, prev_cp, curr_cp)*line_height;
        }
        Font_Glyph_Asset *glyph = get_glyph(font, curr_cp);
        
        F32 width = (glyph->box.max.x - glyph->box.min.x)*line_height;
        F32 height = (glyph->box.max.y - glyph->box.min.y)*line_height;
        
        M44 transform = 
          create_m44_translation(position.x + (glyph->box.min.x*line_height), 
                                 position.y + (glyph->box.min.y*line_height), 
                                 9.f)*
          create_m44_scale(width, height, 1.f)*
          create_m44_translation(0.5f, 0.5f, 1.f);
        
        
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
