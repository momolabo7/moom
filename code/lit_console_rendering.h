
static void
lit_update_and_render_console(lit_t* lit) 
{

  console_t* console = &lit->console;
  moe_t* moe = lit->moe;
  gfx_t* gfx = lit->gfx;
  input_t* input = lit->input;
  assets_t* assets = &lit->assets;

  for (u32_t char_index = 0; 
       char_index < input->char_count;
       ++char_index) 
  {
    // NOTE(Momo): Not very portable to other platforms....
    u8_t c = input->chars[char_index];
    if (c >= 32 && c <= 126) {
      sb8_push_u8(&console->input_line, c);
    }
    // backspace 
    if (c == 8) {
      if (console->input_line.count > 0) 
        sb8_pop(&console->input_line);
    }    
    
    if (c == '\r') {
      console_execute(console);
      break;
    }
  }
  
  // Render
  static const f32_t console_width = LIT_WIDTH;
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
  
  gfx_push_asset_sprite(gfx, assets, lit->blank_sprite, 
                          v2f_set(LIT_WIDTH/2, LIT_HEIGHT/2), 
                          v2f_set(LIT_WIDTH, LIT_HEIGHT),
                          rgba_set(0.f, 0.f, 0.f, 0.8f));
  gfx_advance_depth(gfx);
  
  gfx_push_asset_sprite(gfx, assets, lit->blank_sprite, console_pos, console_size, rgba_hex(0x787878FF));
  gfx_advance_depth(gfx);
  gfx_push_asset_sprite(gfx, assets, lit->blank_sprite, input_area_pos, input_area_size, rgba_hex(0x505050FF));
  gfx_advance_depth(gfx);
  
  
  // Draw info text
  for (u32_t line_index = 0;
       line_index < array_count(console->info_lines);
       ++line_index)
  {
    sb8_t* line = console->info_lines + line_index;
    
    gfx_push_text(gfx, assets, lit->debug_font,
                          line->str,
                          rgba_hex(0xFFFFFFFF),
                          left_pad, 
                          line_height * (line_index+1) + font_bottom_pad,
                          font_height);
    
  }

  gfx_advance_depth(gfx);
  gfx_push_text(gfx, assets, lit->debug_font,
                        console->input_line.str,
                        rgba_hex(0xFFFFFFFF),
                        left_pad, 
                        font_bottom_pad,
                        font_height);
  gfx_advance_depth(gfx);
}
