#ifndef GAME_MODE_COMPUTER
#define GAME_MODE_COMPUTER



typedef struct {
  String8 text;
  Game_Mode_Type mode_type;
} Computer_Selection;

typedef struct {
  U32 count;
  Computer_Selection e[10];
} Computer_Selection_List;

typedef struct {
  U32 selected_id;
  Computer_Selection_List selection_list;
} Computer;

static void 
computer_add_selection(Computer* com, String8 text, Game_Mode_Type mode_type) {
  Computer_Selection* s = al_append(&com->selection_list); 
  s->text = text;
  s->mode_type = mode_type;
}

static void
computer_tick(Game* game,
              Painter* painter,
              Platform* pf)
{
  if (!game_mode_initialized(game)) {
    Computer* com = game_allocate_mode(Computer, game);
    al_clear(&com->selection_list);
    computer_add_selection(com, str8_from_lit("Splash"), GAME_MODE_TYPE_SPLASH);
    computer_add_selection(com, str8_from_lit("GAME: Lit"), GAME_MODE_TYPE_LIT);
    computer_add_selection(com, str8_from_lit("Exit"), GAME_MODE_TYPE_LIT);
    com->selected_id = 0;
  }
  Computer* com = (Computer*)game->mode_context;

  // Input
  if (pf_is_button_poked(pf->button_up)) {
    if (com->selected_id > 0)
      --com->selected_id;
    else 
      com->selected_id = com->selection_list.count - 1;
  }
  if (pf_is_button_poked(pf->button_down)) {
    com->selected_id = ++com->selected_id % com->selection_list.count;
  }

  if (pf_is_button_poked(pf->button_use)) {
    game_goto_mode(game, com->selection_list.e[com->selected_id].mode_type);
  }


  // Draw
  const RGBA color = rgba(1.f, 1.f, 1.f, 1.f);
    
  // box
  {
    V2 min = v2(100.f, 100.f);
    V2 max = v2(800.f, 800.f);

    Line2 l1 = line2(v2(min.x, min.y), v2(max.x, min.y));
    Line2 l2 = line2(v2(min.x, min.y), v2(min.x, max.y));
    Line2 l3 = line2(v2(min.x, max.y), v2(max.x, max.y));
    Line2 l4 = line2(v2(max.x, min.y), v2(max.x, max.y));

    paint_line(painter, l1, 5.f);
    paint_line(painter, l2, 5.f);
    paint_line(painter, l3, 5.f);
    paint_line(painter, l4, 5.f);
  }

  

  // Text
  {
    const F32 selection_x = 200.f;
    const F32 selection_y = 700.f;
    const F32 font_size = 48.f;
    const F32 padding = 10.f;

    F32 current_selection_y = selection_y;
 
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
      const F32 x_offset_from_selection = 32.f; 
      const F32 selector_start_x = 170.f;
      const F32 circle_radius = 16.f;
      const F32 selector_start_y = selection_y + circle_radius;
      const F32 selector_offset_y = 80.f;
      const F32 y_offset = font_size + padding;

      Circ2 circle = circ2(v2(selector_start_x, selector_start_y - y_offset * com->selected_id), 16.f);
      paint_filled_circle(painter, circle, 16);  
    }
  }
}


#endif // GAME_MODE_COMPUTER