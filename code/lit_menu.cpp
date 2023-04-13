


static void
lit_menu_init(
    lit_t* lit, 
    lit_menu_t* menu) 
{
  make(asset_match_t, match);
  set_match_entry(match, ASSET_TAG_TYPE_FONT, 0.f, 1.f);
  menu->font = find_best_font(&lit->assets, ASSET_GROUP_TYPE_FONTS, match);
  menu->blank_sprite = find_first_sprite(&lit->assets, ASSET_GROUP_TYPE_BLANK_SPRITE);

  menu->current_level_selection = 0;
  menu->mode = LIT_MENU_MODE_TRANSITION_IN;
  menu->overlay_timer = LIT_MENU_TRANSITION_DURATION;
  menu->selector_color = RGBA_WHITE;


  // 'Buttons'
  {
    v2f_t start = v2f_set(LIT_MENU_BUTTON_START_X, LIT_MENU_BUTTON_START_Y);
    v2f_t cur = start; 

    for (u32_t button_id = 0; 
        button_id < array_count(menu->buttons); 
        ++button_id)
    {
      lit_menu_button_t* btn = menu->buttons + button_id;
      btn->oxy = btn->xy = cur;
      btn->wh = v2f_set(LIT_MENU_BUTTON_W, LIT_MENU_BUTTON_H);
      btn->cp = digit_to_ascii(button_id < 10 ? button_id : button_id + 7);
      btn->scale = 1.f;

      cur.x += LIT_MENU_BUTTON_OFFSET_X;
      if (button_id % LIT_MENU_LEVELS_PER_ROW == LIT_MENU_LEVELS_PER_ROW-1) {
        cur.x = start.x;
        cur.y -= LIT_MENU_BUTTON_OFFSET_Y; 
      }
    }
  }
}

static void
lit_menu_draw_rect(
    lit_t* lit, 
    lit_menu_t* menu,
    v2f_t pos,
    v2f_t size, 
    f32_t thickness, 
    rgba_t color)
{
  const f32_t half_w = size.w/2.f;
  const f32_t half_h = size.h/2.f;

  // left
  gfx_push_asset_sprite(
      gfx, 
      &lit->assets, 
      menu->blank_sprite, 
      v2f_set(pos.x - half_w, pos.y), 
      v2f_set(thickness, size.y), 
      color); 
  gfx_advance_depth(gfx);

  // right
  gfx_push_asset_sprite(
      gfx, 
      &lit->assets, 
      menu->blank_sprite, 
      v2f_set(pos.x + half_w, pos.y), 
      v2f_set(thickness, size.y), 
      color); 
  gfx_advance_depth(gfx);

  // top
  gfx_push_asset_sprite(
      gfx, 
      &lit->assets, 
      menu->blank_sprite,
      v2f_set(pos.x, pos.y + half_h), 
      v2f_set(size.x, thickness), 
      color); 
  gfx_advance_depth(gfx);

  // bottom
  gfx_push_asset_sprite(
      gfx, 
      &lit->assets, 
      menu->blank_sprite, 
      v2f_set(pos.x, pos.y - half_h), 
      v2f_set(size.x, thickness), 
      color); 
  gfx_advance_depth(gfx);
}

static void
lit_menu_tick_transition_in(
    lit_t* lit, 
    lit_menu_t* menu, 
    f32_t dt)
{
  menu->overlay_timer-= dt; 
  if (menu->overlay_timer <= 0.f) {
    menu->overlay_timer = 0.f;
    menu->mode = LIT_MENU_MODE_NORMAL;
  }
}

static void
lit_menu_tick_transition_out(
    lit_t* lit, 
    lit_menu_t* menu, 
    f32_t dt)
{
  menu->overlay_timer += dt; 
  if (menu->overlay_timer >= LIT_MENU_TRANSITION_DURATION) {
    menu->overlay_timer = LIT_MENU_TRANSITION_DURATION;

    lit_goto_specific_level(lit, 0); // TODO: change to appropriate level
  }
}

static void 
lit_menu_tick_normal(
    lit_t* lit, 
    lit_menu_t* menu, 
    f32_t dt)
{
  if (input_is_button_down(input->buttons[INPUT_BUTTON_CODE_SPACE])) 
  {
    // animate held button
    menu->selection_held_timer += dt;
    if (menu->selection_held_timer >= LIT_MENU_SELECT_DURATION) {
      menu->selection_held_timer = LIT_MENU_SELECT_DURATION;
      menu->mode = LIT_MENU_MODE_TRANSITION_OUT;
    }
  }

  else if (input_is_button_released(input->buttons[INPUT_BUTTON_CODE_SPACE]))
  {
    // menu->mode = LIT_MENU_MODE_TRANSITION_OUT;
    //menu->selection_held_timer = 0.f;
  }

  else {
    if (menu->selection_held_timer <= 0.f) {
      if (input_is_button_poked(input->buttons[INPUT_BUTTON_CODE_W])) {
        if (menu->current_level_selection >= LIT_MENU_LEVELS_PER_ROW) {
          menu->current_level_selection -= LIT_MENU_LEVELS_PER_ROW;
        }
      }
      if (input_is_button_poked(input->buttons[INPUT_BUTTON_CODE_S])) {
        if (menu->current_level_selection < LIT_MENU_TOTAL_LEVELS - LIT_MENU_LEVELS_PER_ROW)
          menu->current_level_selection += LIT_MENU_LEVELS_PER_ROW;
      }
      if (input_is_button_poked(input->buttons[INPUT_BUTTON_CODE_D])) 
      {
        if (menu->current_level_selection % LIT_MENU_LEVELS_PER_ROW != LIT_MENU_LEVELS_PER_ROW-1)
        {
          menu->current_level_selection++;
        }
      }
      if (input_is_button_poked(input->buttons[INPUT_BUTTON_CODE_A]))
      {
        if (menu->current_level_selection % LIT_MENU_LEVELS_PER_ROW != 0) 
        {
          menu->current_level_selection--;
        }
      }
    }
   
    // un-animate held button
    menu->selection_held_timer -= dt;
    menu->selection_held_timer = max_of(menu->selection_held_timer, 0.f);

  }
 
  lit_menu_button_t* btn = menu->buttons + menu->current_level_selection;
  f32_t a = f32_ease_out_quad(menu->selection_held_timer/LIT_MENU_SELECT_DURATION);
  btn->scale = 1.f + a * (LIT_MENU_BUTTON_SCALE_TARGET-1.f);
  btn->xy = btn->oxy + a*(v2f_set(LIT_WIDTH/2, LIT_HEIGHT/2) - btn->oxy);

  menu->current_level_selection %= LIT_MENU_TOTAL_LEVELS;

}

static void 
lit_menu_button_render(
    lit_t* lit, 
    lit_menu_t* menu, 
    lit_menu_button_t* btn) 
{
  // Frame
  rgba_t frame_color = rgba_set(0.5f, 0.5f, 0.5f, 1.f);
  lit_menu_draw_rect(
      lit, 
      menu, 
      btn->xy, 
      btn->wh * btn->scale, 
      16.f * btn->scale, 
      frame_color); 

  // Draw level number or icon depending on whether it's unlocked
  // TODO: locked/unlocked
  {
    // NOTE(Momo): This is bad and hacky.
    asset_font_t* font = get_font(&lit->assets, menu->font);
    asset_font_glyph_t *glyph = get_glyph(font, btn->cp);
    asset_bitmap_t* bitmap = get_bitmap(&lit->assets, glyph->bitmap_asset_id);
    f32_t font_height = 72.f * btn->scale; 
    v2f_t font_wh = v2f_set(
        (glyph->box_x1 - glyph->box_x0) * font_height,
        (glyph->box_y1 - glyph->box_y0) * font_height);
    v2f_t anchor = v2f_set(0.5f, 0.5f);
    gfx_push_sprite(gfx, 
        RGBA_WHITE,
        btn->xy, font_wh, anchor,
        bitmap->renderer_texture_handle, 
        glyph->texel_x0,
        glyph->texel_y0,
        glyph->texel_x1,
        glyph->texel_y1);

  }
  gfx_advance_depth(gfx);

}

static void
lit_menu_tick(lit_t* lit, lit_menu_t* menu) {
  f32_t dt = input->delta_time;

  if (menu->mode == LIT_MENU_MODE_TRANSITION_IN) {
    lit_menu_tick_transition_in(lit, menu, dt);
  }
  else if (menu->mode == LIT_MENU_MODE_TRANSITION_OUT) {
    lit_menu_tick_transition_out(lit, menu, dt);
    
  }
  else if (menu->mode == LIT_MENU_MODE_NORMAL) {
    lit_menu_tick_normal(lit, menu, dt);
  }
  
  
  

  // 
  // Rendering
  //
  gfx_set_blend_alpha(gfx);

  // Title
  gfx_push_text_center_aligned(gfx, &lit->assets, menu->font, str8_from_lit("LEVEL SELECTION"), rgba_set(1.f, 1.f, 1.f, 1.f), LIT_WIDTH/2, LIT_HEIGHT - 125.f, 72.f);

  for_arr(button_id, menu->buttons)
  {
    if (button_id == menu->current_level_selection) continue; 
    lit_menu_button_render(lit, menu, menu->buttons + button_id);
  }

  // Overlay to emphasize selected button
  {
    f32_t alpha = menu->selection_held_timer/(LIT_MENU_TRANSITION_DURATION/2);
    gfx_push_asset_sprite(gfx, &lit->assets, menu->blank_sprite, v2f_set(LIT_WIDTH/2, LIT_HEIGHT/2), v2f_set(LIT_WIDTH, LIT_HEIGHT), rgba_set(0.f, 0.f, 0.f, alpha));
    gfx_advance_depth(gfx);
  }


  lit_menu_button_render(lit, menu, menu->buttons + menu->current_level_selection);
  

  // "Cursor"
  // Well this is not really a cursor in a classic sense, but it
  // is an indication to show what the user is currently selecting.
  {
    u32_t cursor_x_index = menu->current_level_selection % LIT_MENU_LEVELS_PER_ROW;
    u32_t cursor_y_index = menu->current_level_selection / LIT_MENU_LEVELS_PER_ROW; 

    v2f_t start = v2f_set(LIT_MENU_BUTTON_START_X, LIT_MENU_BUTTON_START_Y);
    v2f_t cursor_xy = v2f_set(
        start.x + cursor_x_index * LIT_MENU_BUTTON_OFFSET_X, 
        start.y - cursor_y_index * LIT_MENU_BUTTON_OFFSET_Y);

    v2f_t cursor_wh = v2f_set(150.f, 150.f);

    lit_menu_draw_rect(lit, menu, cursor_xy, cursor_wh, 16.f, menu->selector_color); 
    //gfx_push_text_center_aligned(gfx, &lit->assets, menu->font, str8_from_lit("menu"), rgba_set(1.f, 1.f, 1.f, 1.f), LIT_WIDTH/2, LIT_HEIGHT/2, 128.f);

    //inspector_add_u32(&lit->inspector, str8_from_lit("num"), &menu->current_level_selection);
    gfx_advance_depth(gfx);

  }

  //
  // Front Overlay
  //
#if 1
  {
    f32_t alpha = menu->overlay_timer/LIT_MENU_TRANSITION_DURATION;
    gfx_push_asset_sprite(gfx, &lit->assets, menu->blank_sprite, v2f_set(LIT_WIDTH/2, LIT_HEIGHT/2), v2f_set(LIT_WIDTH, LIT_HEIGHT), rgba_set(0.f, 0.f, 0.f, alpha));
    gfx_advance_depth(gfx);
  }
#endif

}
