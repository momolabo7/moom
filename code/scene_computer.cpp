
static void 
computer_add_selection(Computer* com, str8_t text, Moe_Mode_Type mode_type) {
  Computer_Selection* s = al_append(&com->selection_list); 
  s->text = text;
  s->mode_type = mode_type;
}

static void
computer_tick(moe_t* moe)
{
  if (!moe_mode_initialized(moe)) {
    Computer* com = moe_allocate_mode(Computer, moe);
    al_clear(&com->selection_list);
    computer_add_selection(com, str8_from_lit("Splash"), MOE_MODE_TYPE_SPLASH);
    computer_add_selection(com, str8_from_lit("GAME: Lit"), MOE_MODE_TYPE_LIT);
    computer_add_selection(com, str8_from_lit("Exit"), MOE_MODE_TYPE_LIT);
    com->selected_id = 0;
  }
  Computer* com = (Computer*)moe->mode_context;

  // Input
  if (pf_is_button_poked(platform->button_up)) {
    if (com->selected_id > 0)
      --com->selected_id;
    else 
      com->selected_id = com->selection_list.count - 1;
  }
  if (pf_is_button_poked(platform->button_down)) {
    com->selected_id = ++com->selected_id % com->selection_list.count;
  }

  if (pf_is_button_poked(platform->button_use)) {
    moe_goto_mode(moe, com->selection_list.e[com->selected_id].mode_type);
  }


  // Draw
  const rgba_t color = rgba_set(1.f, 1.f, 1.f, 1.f);
    
  // box
  {
    v2f_t min = v2f_set(100.f, 100.f);
    v2f_t max = v2f_set(800.f, 800.f);
    rgba_t white = rgba_set(1.f, 1.f, 1.f, 1.f);

    gfx_push_line(gfx, v2f_set(min.x, min.y), v2f_set(max.x, min.y), 5.f, white);
    gfx_push_line(gfx, v2f_set(min.x, min.y), v2f_set(min.x, max.y), 5.f, white);
    gfx_push_line(gfx, v2f_set(min.x, max.y), v2f_set(max.x, max.y), 5.f, white);
    gfx_push_line(gfx, v2f_set(max.x, min.y), v2f_set(max.x, max.y), 5.f, white);
  }

  

  // Text
#if 0
  {
    const f32_t selection_x = 200.f;
    const f32_t selection_y = 700.f;
    const f32_t font_size = 48.f;
    const f32_t padding = 10.f;

    f32_t current_selection_y = selection_y;
 
    al_foreach(selection_id, &com->selection_list) {
      Computer_Selection* s = al_at(&com->selection_list, selection_id);
      paint_text(painter,
                 FONT_DEFAULT, 
                 s->text,
                 color,
                 selection_x, 
                 current_selection_y,
                 font_size);
      current_selection_y -= font_size + padding;
    }

    // Selector
    {
      const f32_t x_offset_from_selection = 32.f; 
      const f32_t selector_start_x = 170.f;
      const f32_t circle_radius = 16.f;
      const f32_t selector_start_y = selection_y + circle_radius;
      const f32_t selector_offset_y = 80.f;
      const f32_t y_offset = font_size + padding;

      Circ2 circle = circ2_set(v2f_set(selector_start_x, selector_start_y - y_offset * com->selected_id), 16.f);
      paint_filled_circle(painter, circle, 16);  
    }
  }
#endif
}


