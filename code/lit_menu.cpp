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
  //
  // Draw text
  //
  moe_painter_draw_text_center_aligned(lit->gfx, &lit->assets, menu->font, str8_from_lit("LEVEL SELECTION"), rgba_set(1.f, 1.f, 1.f, 1.f), LIT_WIDTH/2, LIT_HEIGHT - 125.f, 72.f);

  const u32_t total_levels = 20;
  const u32_t levels_per_row = 5;

  // input
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

  menu->current_level_selection %= total_levels;

  f32_t start_x = 100.f;
  f32_t start_y = LIT_HEIGHT - 250.f;
  f32_t offset_x = 150.f;
  f32_t offset_y = 150.f;
  f32_t cur_x = start_x;
  f32_t cur_y = start_y;
  for (u32_t level_index = 0; level_index < total_levels; ++level_index) 
  {
    
    lit_draw_menu_button_frame(lit, menu, v2f_set(cur_x, cur_y), v2f_set(100.f, 100.f), 16.f, rgba_set(0.5f, 0.5f, 0.5f, 1.f)); 
    cur_x += offset_x;

    if (level_index % levels_per_row == levels_per_row-1) {
      cur_x = start_x;
      cur_y -= offset_y; 
    }
  }

  // cursor
  u32_t cursor_x_index = menu->current_level_selection % levels_per_row;
  u32_t cursor_y_index = menu->current_level_selection / levels_per_row; 
  f32_t cursor_x = start_x + cursor_x_index * offset_x;
  f32_t cursor_y = start_y - cursor_y_index * offset_y;

  lit_draw_menu_cursor(lit, menu, v2f_set(cursor_x, cursor_y), v2f_set(150.f, 150.f), 16.f, rgba_set(1.f, 1.f, 1.f, 1.f)); 
  //moe_painter_draw_text_center_aligned(lit->gfx, &lit->assets, menu->font, str8_from_lit("menu"), rgba_set(1.f, 1.f, 1.f, 1.f), LIT_WIDTH/2, LIT_HEIGHT/2, 128.f);

  inspector_add_u32(&lit->inspector, str8_from_lit("num"), &menu->current_level_selection);
}
