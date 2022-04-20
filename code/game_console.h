/* date = April 9th 2022 8:27 pm */

#ifndef GAME_CONSOLE_H
#define GAME_CONSOLE_H

struct Console_Command {
  String key;
  void* ctx;
  void (*func)(void*);
};

struct Console_Line {
  U8 buffer[256];
  String str;
};

struct Console {
  U32 command_count;
  Console_Command commands[10];
  
  String_Builder info_lines[9];
  String_Builder input_line;
};

static void
init_console(Console* dc, Arena* arena) {
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
add_command(Console* dc, 
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
_push_info(Console* dc, String str) {
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
_execute(Console* dc) {
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
update_console(Console* dc, Game_Input* input) {
  
  profile_block();
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
render_console(Console* dc, 
               Game_Assets* ga, 
               Renderer_Command_Queue* render_commands)
{
  profile_block();
  
  // Debug console
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
  static const F32 console_width = 1600.f;
  static const F32 console_height = 400.f;
  static const F32 line_height = console_height/(array_count(dc->info_lines)+1);
  static const F32 font_height = line_height * 0.8f;
  static const F32 font_bottom_pad = (line_height - font_height);
  static const F32 left_pad = 10.f;
  
  draw_sprite(ga, render_commands,
              SPRITE_BLANK,
              rgba(0x787878FF),
              console_width/2,
              console_height/2,
              console_width, 
              console_height,
              100.f);
  
  draw_sprite(ga, render_commands,
              SPRITE_BLANK,
              rgba(0x505050FF),
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
    
    draw_text(ga, render_commands,
              FONT_DEFAULT,
              line->str,
              rgba(0xFFFFFFFF),
              left_pad, 
              line_height * (line_index+1) + font_bottom_pad,
              font_height,
              80.f);
    
  }
  
  draw_text(ga, render_commands,
            FONT_DEFAULT,
            dc->input_line.str,
            rgba(0xFFFFFFFF),
            left_pad, 
            font_bottom_pad,
            font_height,
            80.f);
}

#endif //GAME_CONSOLE_H
