static void
moe_console_init(moe_console_t* dc, u32_t characters_per_line, arena_t* allocator) 
{
  u32_t line_size = characters_per_line;
  sb8_init(&dc->input_line,
           arena_push_arr(u8_t, allocator, line_size),
           line_size);
  
  for (u32_t info_line_index = 0;
       info_line_index < array_count(dc->info_lines);
       ++info_line_index) 
  {    
    sb8_t* info_line = dc->info_lines + info_line_index;
    sb8_init(info_line,
             arena_push_arr(u8_t, allocator, line_size),
             line_size);
  }
}

static void
moe_console_add_command(moe_console_t* dc, str8_t key, void* ctx, void(*func)(void*)) 
{
  // simulate adding commands
  moe_console_command_t* cmd = dc->commands + dc->command_count++;
  cmd->key = key;
  cmd->ctx = ctx;
  cmd->func = func;
}

static void
_moe_console_push_info(moe_console_t* dc, str8_t str) {
  // NOTE(Momo): There's probably a better to do with via some
  // crazy indexing scheme, but this is debug so we don't care for now
  
  // Copy everything from i + 1 from i
  for (u32_t i = 0; 
       i < array_count(dc->info_lines) - 1;
       ++i)
  {
    u32_t line_index = array_count(dc->info_lines) - 1 - i;
    sb8_t* line_to = dc->info_lines + line_index;
    sb8_t* line_from = dc->info_lines + line_index - 1;
    sb8_clear(line_to);
    sb8_push_str8(line_to, line_from->str);
  } 
  sb8_clear(dc->info_lines + 0);
  sb8_push_str8(dc->info_lines + 0, str);
}

static void
_moe_console_execute(moe_console_t* dc) {
  for(u32_t command_index = 0; 
      command_index < dc->command_count; 
      ++command_index) 
  {
    moe_console_command_t* cmd = dc->commands + command_index;
    if (str8_match(cmd->key, dc->input_line.str)) {
      cmd->func(cmd->ctx);
    }
  }
  
  _moe_console_push_info(dc, dc->input_line.str);
  sb8_clear(&dc->input_line);
}

static void
moe_console_update_and_render(moe_t* moe, asset_sprite_id_t blank_sprite, asset_font_id_t font) 
{
  platform_t* platform = moe->platform;
  moe_console_t* dc = &moe->console;
  for (u32_t char_index = 0; 
       char_index < platform->char_count;
       ++char_index) 
  {
    // NOTE(Momo): Not very portable to other platforms....
    u8_t c = platform->chars[char_index];
    if (c >= 32 && c <= 126) {
      sb8_push_u8(&dc->input_line, c);
    }
    // backspace 
    if (c == 8) {
      if (dc->input_line.count > 0) 
        sb8_pop(&dc->input_line);
    }    
    
    if (c == '\r') {
      _moe_console_execute(dc);
      break;
    }
  }
  
  // Render
  // TODO(Momo): These should be percentages
  // but we'll work with this for now
  static const f32_t console_width = 1600.f;
  static const f32_t console_height = 400.f;
  static const u32_t line_count = array_count(dc->info_lines)+1;
  static const f32_t line_height = console_height/line_count;
  static const f32_t font_height = line_height * 0.8f;
  static const f32_t font_bottom_pad = (line_height - font_height);
  static const f32_t left_pad = 10.f;
  
  v2f_t console_size = v2f_set(console_width, console_height);
  v2f_t console_pos = v2f_set(console_width/2, console_height/2);
  v2f_t input_area_size = v2f_set(console_width, line_height);
  v2f_t input_area_pos = v2f_set(console_width/2, line_height/2);
  
  moe_painter_draw_sprite(moe, blank_sprite, 
                          v2f_set(MOE_WIDTH/2, MOE_HEIGHT/2), 
                          v2f_set(MOE_WIDTH, MOE_HEIGHT),
                          rgba_set(0.f, 0.f, 0.f, 0.8f));
  moe_painter_advance_depth(moe);
  
  moe_painter_draw_sprite(moe, blank_sprite, console_pos, console_size, rgba_hex(0x787878FF));
  moe_painter_advance_depth(moe);
  moe_painter_draw_sprite(moe, blank_sprite, input_area_pos, input_area_size, rgba_hex(0x505050FF));
  moe_painter_advance_depth(moe);
  
  
  // Draw info text
  for (u32_t line_index = 0;
       line_index < array_count(dc->info_lines);
       ++line_index)
  {
    sb8_t* line = dc->info_lines + line_index;
    
    moe_painter_draw_text(moe, font,
                          line->str,
                          rgba_hex(0xFFFFFFFF),
                          left_pad, 
                          line_height * (line_index+1) + font_bottom_pad,
                          font_height);
    
  }
  moe_painter_advance_depth(moe);
  moe_painter_draw_text(moe, font,
                        dc->input_line.str,
                        rgba_hex(0xFFFFFFFF),
                        left_pad, 
                        font_bottom_pad,
                        font_height);
  moe_painter_advance_depth(moe);
}


