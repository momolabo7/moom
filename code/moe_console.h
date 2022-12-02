
#ifndef MOE_CONSOLE_H
#define MOE_CONSOLE_H


struct Console_Command {
  String8 key;
  void* ctx;
  void (*func)(void*);
};

struct Console_Line {
  U8 buffer[256];
  String8 str;
};

struct Console {
  U32 command_count;
  Console_Command commands[10];
  
  String8_Builder info_lines[9];
  String8_Builder input_line;
};

static void
init_console(Console* dc, Arena* allocator) {
  U32 line_size = 256;
  sb8_init(&dc->input_line,
           arn_push_arr(U8, allocator, line_size),
           line_size);
  
  for (U32 info_line_index = 0;
       info_line_index < array_count(dc->info_lines);
       ++info_line_index) 
  {    
    String8_Builder* info_line = dc->info_lines + info_line_index;
    sb8_init(info_line,
             arn_push_arr(U8, allocator, line_size),
             line_size);
  }
}

static void
add_command(Console* dc, 
            String8 key, 
            void* ctx,
            void(*func)(void*)) {
  
  // simulate adding commands
  Console_Command* cmd = dc->commands + dc->command_count++;
  cmd->key = key;
  cmd->ctx = ctx;
  cmd->func = func;
}

static void
_push_info(Console* dc, String8 str) {
  // NOTE(Momo): There's probably a better to do with via some
  // crazy indexing scheme, but this is debug so we don't care for now
  
  // Copy everything from i + 1 from i
  for (U32 i = 0; 
       i < array_count(dc->info_lines) - 1;
       ++i)
  {
    U32 line_index = array_count(dc->info_lines) - 1 - i;
    String8_Builder* line_to = dc->info_lines + line_index;
    String8_Builder* line_from = dc->info_lines + line_index - 1;
    sb8_clear(line_to);
    sb8_push_str8(line_to, line_from->str);
  } 
  sb8_clear(dc->info_lines + 0);
  sb8_push_str8(dc->info_lines + 0, str);
}

static void
_execute(Console* dc) {
  for(U32 command_index = 0; 
      command_index < dc->command_count; 
      ++command_index) 
  {
    Console_Command* cmd = dc->commands + command_index;
    if (str8_match(cmd->key, dc->input_line.str)) {
      cmd->func(cmd->ctx);
    }
  }
  
  _push_info(dc, dc->input_line.str);
  sb8_clear(&dc->input_line);
}

static void
update_and_render_console(Console* dc, Moe_Sprite_ID blank_sprite, Moe_Font_ID font) 
{
  for (U32 char_index = 0; 
       char_index < platform->char_count;
       ++char_index) 
  {
    // NOTE(Momo): Not very portable to other platforms....
    U8 c = platform->chars[char_index];
    if (c >= 32 && c <= 126) {
      sb8_push_u8(&dc->input_line, c);
    }
    // backspace 
    if (c == 8) {
      if (dc->input_line.count > 0) 
        sb8_pop(&dc->input_line);
    }    
    
    if (c == '\r') {
      _execute(dc);
      break;
    }
  }
  
  // Render
  // TODO(Momo): These should be percentages
  // but we'll work with this for now
  static const F32 console_width = 1600.f;
  static const F32 console_height = 400.f;
  static const U32 line_count = array_count(dc->info_lines)+1;
  static const F32 line_height = console_height/line_count;
  static const F32 font_height = line_height * 0.8f;
  static const F32 font_bottom_pad = (line_height - font_height);
  static const F32 left_pad = 10.f;
  
  V2 console_size = { console_width, console_height };
  V2 console_pos = { console_width/2, console_height/2 };
  V2 input_area_size = { console_width, line_height };
  V2 input_area_pos = { console_width/2, line_height/2 };
  
  paint_sprite(blank_sprite, 
               v2_set(MOE_WIDTH/2, MOE_HEIGHT/2), 
               v2_set(MOE_WIDTH, MOE_HEIGHT),
               rgba_set(0.f, 0.f, 0.f, 0.8f));
  gfx_advance_depth(gfx);
  
  paint_sprite(blank_sprite, console_pos, console_size, rgba_hex(0x787878FF));
  gfx_advance_depth(gfx);
  paint_sprite(blank_sprite, input_area_pos, input_area_size, rgba_hex(0x505050FF));
  gfx_advance_depth(gfx);
  
  
  // Draw info text
  for (U32 line_index = 0;
       line_index < array_count(dc->info_lines);
       ++line_index)
  {
    String8_Builder* line = dc->info_lines + line_index;
    
    paint_text(font,
               line->str,
               rgba_hex(0xFFFFFFFF),
               left_pad, 
               line_height * (line_index+1) + font_bottom_pad,
               font_height);
    
  }
  gfx_advance_depth(gfx);
  paint_text(font,
             dc->input_line.str,
             rgba_hex(0xFFFFFFFF),
             left_pad, 
             font_bottom_pad,
             font_height);
  gfx_advance_depth(gfx);
}


#endif //MOE_CONSOLE_H
