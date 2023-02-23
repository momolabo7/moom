static void
console_init(console_t* console, u32_t characters_per_line, arena_t* allocator) 
{
  u32_t line_size = characters_per_line;
  sb8_init(&console->input_line,
           arena_push_arr(u8_t, allocator, line_size),
           line_size);
  
  for (u32_t info_line_index = 0;
       info_line_index < array_count(console->info_lines);
       ++info_line_index) 
  {    
    sb8_t* info_line = console->info_lines + info_line_index;
    sb8_init(info_line,
             arena_push_arr(u8_t, allocator, line_size),
             line_size);
  }
}

static void
console_add_command(console_t* console, str8_t key, void* ctx, void(*func)(void*)) 
{
  // simulate adding commands
  console_command_t* cmd = console->commands + console->command_count++;
  cmd->key = key;
  cmd->ctx = ctx;
  cmd->func = func;
}

static void
_console_push_info(console_t* console, str8_t str) {
  // NOTE(Momo): There's probably a better to do with via some
  // crazy indexing scheme, but this is debug so we don't care for now
  
  // Copy everything from i + 1 from i
  for (u32_t i = 0; 
       i < array_count(console->info_lines) - 1;
       ++i)
  {
    u32_t line_index = array_count(console->info_lines) - 1 - i;
    sb8_t* line_to = console->info_lines + line_index;
    sb8_t* line_from = console->info_lines + line_index - 1;
    sb8_clear(line_to);
    sb8_push_str8(line_to, line_from->str);
  } 
  sb8_clear(console->info_lines + 0);
  sb8_push_str8(console->info_lines + 0, str);
}

static void
_console_execute(console_t* console) {
  for(u32_t command_index = 0; 
      command_index < console->command_count; 
      ++command_index) 
  {
    console_command_t* cmd = console->commands + command_index;
    if (str8_match(cmd->key, console->input_line.str)) {
      cmd->func(cmd->ctx);
    }
  }
  
  _console_push_info(console, console->input_line.str);
  sb8_clear(&console->input_line);
}

static void
console_update_and_render(console_t* console, platform_t* platform, gfx_t* gfx, assets_t* assets,  asset_sprite_id_t blank_sprite, asset_font_id_t font) 
{
  for (u32_t char_index = 0; 
       char_index < platform->char_count;
       ++char_index) 
  {
    // NOTE(Momo): Not very portable to other platforms....
    u8_t c = platform->chars[char_index];
    if (c >= 32 && c <= 126) {
      sb8_push_u8(&console->input_line, c);
    }
    // backspace 
    if (c == 8) {
      if (console->input_line.count > 0) 
        sb8_pop(&console->input_line);
    }    
    
    if (c == '\r') {
      _console_execute(console);
      break;
    }
  }
  
  // Render
  // TODO(Momo): These should be percentages
  // but we'll work with this for now
  static const f32_t console_width = 1600.f;
  static const f32_t console_height = 400.f;
  static const u32_t line_count = array_count(console->info_lines)+1;
  static const f32_t line_height = console_height/line_count;
  static const f32_t font_height = line_height * 0.8f;
  static const f32_t font_bottom_pad = (line_height - font_height);
  static const f32_t left_pad = 10.f;
  
  v2f_t console_size = v2f_set(console_width, console_height);
  v2f_t console_pos = v2f_set(console_width/2, console_height/2);
  v2f_t input_area_size = v2f_set(console_width, line_height);
  v2f_t input_area_pos = v2f_set(console_width/2, line_height/2);
  
  moe_painter_draw_sprite(gfx, assets, blank_sprite, 
                          v2f_set(MOE_WIDTH/2, MOE_HEIGHT/2), 
                          v2f_set(MOE_WIDTH, MOE_HEIGHT),
                          rgba_set(0.f, 0.f, 0.f, 0.8f));
  gfx_advance_depth(gfx);
  
  moe_painter_draw_sprite(gfx, assets, blank_sprite, console_pos, console_size, rgba_hex(0x787878FF));
  gfx_advance_depth(gfx);
  moe_painter_draw_sprite(gfx, assets, blank_sprite, input_area_pos, input_area_size, rgba_hex(0x505050FF));
  gfx_advance_depth(gfx);
  
  
  // Draw info text
  for (u32_t line_index = 0;
       line_index < array_count(console->info_lines);
       ++line_index)
  {
    sb8_t* line = console->info_lines + line_index;
    
    moe_painter_draw_text(gfx, assets, font,
                          line->str,
                          rgba_hex(0xFFFFFFFF),
                          left_pad, 
                          line_height * (line_index+1) + font_bottom_pad,
                          font_height);
    
  }
  gfx_advance_depth(gfx);
  moe_painter_draw_text(gfx, assets, font,
                        console->input_line.str,
                        rgba_hex(0xFFFFFFFF),
                        left_pad, 
                        font_bottom_pad,
                        font_height);
  gfx_advance_depth(gfx);
}


