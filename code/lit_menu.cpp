static void
lit_init_menu(lit_t* lit, lit_menu_t* menu) {
  make(asset_match_t, match);
  set_match_entry(match, ASSET_TAG_TYPE_FONT, 0.f, 1.f);
  menu->font = find_best_font(&lit->assets, ASSET_GROUP_TYPE_FONTS, match);
  menu->blank_sprite = find_first_sprite(&lit->assets, ASSET_GROUP_TYPE_BLANK_SPRITE);

  menu->current_level_selection = 0;
}

static void
lit_draw_menu_button_frame(lit_t* lit, lit_menu_t* menu, v2f_t pos, v2f_t size, f32_t thickness, rgba_t color) 
{
  const f32_t half_w = size.w/2.f;
  const f32_t half_h = size.h/2.f;

  // left
  moe_painter_draw_sprite(lit->gfx, &lit->assets, menu->blank_sprite, v2f_set(pos.x - half_w, pos.y), v2f_set(thickness, size.y), color); 

  // right
  moe_painter_draw_sprite(lit->gfx, &lit->assets, menu->blank_sprite, v2f_set(pos.x + half_w, pos.y), v2f_set(thickness, size.y), color); 

  // top
  moe_painter_draw_sprite(lit->gfx, &lit->assets, menu->blank_sprite, v2f_set(pos.x, pos.y + half_h), v2f_set(size.x, thickness), color); 

  // bottom
  moe_painter_draw_sprite(lit->gfx, &lit->assets, menu->blank_sprite, v2f_set(pos.x, pos.y - half_h), v2f_set(size.x, thickness), color); 
}

static void
lit_draw_menu_cursor(lit_t* lit, lit_menu_t* menu, v2f_t pos, v2f_t size, f32_t thickness, rgba_t color) 
{
  const f32_t half_w = size.w/2.f;
  const f32_t half_h = size.h/2.f;

  // left
  moe_painter_draw_sprite(lit->gfx, &lit->assets, menu->blank_sprite, v2f_set(pos.x - half_w, pos.y), v2f_set(thickness, size.y), color); 

  // right
  moe_painter_draw_sprite(lit->gfx, &lit->assets, menu->blank_sprite, v2f_set(pos.x + half_w, pos.y), v2f_set(thickness, size.y), color); 

  // top
  moe_painter_draw_sprite(lit->gfx, &lit->assets, menu->blank_sprite, v2f_set(pos.x, pos.y + half_h), v2f_set(size.x, thickness), color); 

  // bottom
  moe_painter_draw_sprite(lit->gfx, &lit->assets, menu->blank_sprite, v2f_set(pos.x, pos.y - half_h), v2f_set(size.x, thickness), color); 
}



static void
lit_update_menu(lit_t* lit, lit_menu_t* menu) {
  const u32_t total_levels = 20;
  const u32_t levels_per_row = 5;

  rgba_t selector_color = RGBA_WHITE;

  //
  // input
  //
  if (platform_is_button_poked(lit->platform->button_up)) {
    if (menu->current_level_selection > levels_per_row) {
      menu->current_level_selection -= levels_per_row;
    }
  }
  if (platform_is_button_poked(lit->platform->button_down)) {
    if (menu->current_level_selection < total_levels - levels_per_row)
      menu->current_level_selection += levels_per_row;
  }
  if (platform_is_button_poked(lit->platform->button_left)) 
  {
    if (menu->current_level_selection % levels_per_row != 0)
    {
      menu->current_level_selection -= 1;
    }
  }
  if (platform_is_button_poked(lit->platform->button_right))
  {
    if (menu->current_level_selection % levels_per_row != levels_per_row-1)
    {
      menu->current_level_selection += 1;
    }
  }

  if (platform_is_button_down(lit->platform->button_use)) {
    selector_color = rgba_set(1.f, 0.f, 0.f, 1.f);
  }
  if (platform_is_button_released(lit->platform->button_use))
  {
    lit_goto_specific_level(lit, 0);
  }
  menu->current_level_selection %= total_levels;

  // 
  // Rendering
  //

  // Title
  moe_painter_draw_text_center_aligned(lit->gfx, &lit->assets, menu->font, str8_from_lit("LEVEL SELECTION"), rgba_set(1.f, 1.f, 1.f, 1.f), LIT_WIDTH/2, LIT_HEIGHT - 125.f, 72.f);


  // 'Buttons'
  f32_t start_x = 100.f;
  f32_t start_y = LIT_HEIGHT - 250.f;
  f32_t offset_x = 150.f;
  f32_t offset_y = 150.f;
  v2f_t cur_xy = v2f_set(start_x, start_y); 
  v2f_t button_wh = v2f_set(100.f, 100.f);
  rgba_t frame_color = rgba_set(0.5f, 0.5f, 0.5f, 1.f);

  // Levels
  for (u32_t level_index = 0; level_index < total_levels; ++level_index) 
  {
    
    // Frame
    lit_draw_menu_button_frame(lit, menu, cur_xy, button_wh, 16.f, frame_color); 

    // Draw level number or icon depending on whether it's unlocked
    // TODO: terribly hardcoded
    // TODO: locked/unlocked
    {
      // NOTE(Momo): I'm too lazy.
      f32_t font_height = 72.f;
      u32_t cp = digit_to_ascii(level_index < 10 ? level_index : level_index + 7);

      asset_font_t* font = get_font(&lit->assets, menu->font);
      asset_font_glyph_t *glyph = get_glyph(font, cp);
      asset_bitmap_t* bitmap = get_bitmap(&lit->assets, glyph->bitmap_asset_id);
      f32_t w = (glyph->box_x1 - glyph->box_x0)*font_height;
      f32_t h = (glyph->box_y1 - glyph->box_y0)*font_height;
      v2f_t pos = cur_xy;
      v2f_t size = v2f_set(w,h);
      v2f_t anchor = v2f_set(0.5f, 0.5f);
      gfx_push_sprite(lit->gfx, 
                      RGBA_WHITE,
                      pos, size, anchor,
                      bitmap->renderer_texture_handle, 
                      glyph->texel_x0,
                      glyph->texel_y0,
                      glyph->texel_x1,
                      glyph->texel_y1);

    }
    //moe_painter_draw_text_center_aligned(lit->gfx, &lit->assets, menu->font, str8_from_lit("2"), RGBA_WHITE, cur_xy.x + 5.f, cur_xy.y - 25.f, 72.f);

    cur_xy.x += offset_x;

    if (level_index % levels_per_row == levels_per_row-1) {
      cur_xy.x = start_x;
      cur_xy.y -= offset_y; 
    }


  }

  // "Cursor"
  // Well this is not really a cursor in a classic sense, but it
  // is an indication to show what the user is currently selecting.
  {
    u32_t cursor_x_index = menu->current_level_selection % levels_per_row;
    u32_t cursor_y_index = menu->current_level_selection / levels_per_row; 
    v2f_t cursor_xy = v2f_set(start_x + cursor_x_index * offset_x, start_y - cursor_y_index * offset_y);
    v2f_t cursor_wh = v2f_set(150.f, 150.f);

    lit_draw_menu_cursor(lit, menu, cursor_xy, cursor_wh, 16.f, selector_color); 
    //moe_painter_draw_text_center_aligned(lit->gfx, &lit->assets, menu->font, str8_from_lit("menu"), rgba_set(1.f, 1.f, 1.f, 1.f), LIT_WIDTH/2, LIT_HEIGHT/2, 128.f);

    //inspector_add_u32(&lit->inspector, str8_from_lit("num"), &menu->current_level_selection);
  }
}
