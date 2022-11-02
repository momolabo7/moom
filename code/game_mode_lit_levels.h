
/////////////////////////////////////////////////////////
// LEVEL 0
static B32
lit_level_0_tutorial_trigger_0(Lit* m) {
  if (pf_is_button_down(platform->button_up) || 
      pf_is_button_down(platform->button_down) || 
      pf_is_button_down(platform->button_right) ||
      pf_is_button_down(platform->button_left)) 
  {
    lit_fade_out_next_tutorial_text(&m->tutorial_texts);
    lit_fade_in_next_tutorial_text(&m->tutorial_texts);
    return true;
  }

  return false;
}

static B32
lit_level_0_tutorial_trigger_1(Lit* m) {
  if (m->player.held_light != null) 
  {
    lit_fade_out_next_tutorial_text(&m->tutorial_texts);
    lit_fade_in_next_tutorial_text(&m->tutorial_texts);
    lit_fade_in_next_tutorial_text(&m->tutorial_texts);
    lit_fade_in_next_tutorial_text(&m->tutorial_texts);
    return true;
  }
  return false;
}

static void
lit_level_0(Lit* m) {
  lit_push_edge(m, 0.f, 0.f, 1600.f, 0.f);
  lit_push_edge(m, 1600.f, 0.f, 1600.f, 900.f);
  lit_push_edge(m, 1600.f, 900.f, 0.f, 900.f);
  lit_push_edge(m, 0.f, 900.f, 0.f, 0.f);

  lit_push_sensor(m, 1200.f, GAME_HEIGHT * 0.5f, 0x880000FF); 
  lit_push_light(m, 800.f, GAME_HEIGHT * 0.5f, 0x880000FF, 90.f, 0.5f);
  
  // initialize player
  Lit_Player* p = &m->player;
  lit_init_player(m, 200.f, GAME_HEIGHT * 0.5f);

  // tutorial text
  lit_push_tutorial_text(&m->tutorial_texts, str8_from_lit("WASD to move"), 100.f, 480.f);

  lit_push_tutorial_text(&m->tutorial_texts, str8_from_lit("SPACE to pick up"), 680.f, 480.f);

  lit_push_tutorial_text(&m->tutorial_texts, str8_from_lit("Q/R to rotate light"), 680.f, 480.f);
  lit_push_tutorial_text(&m->tutorial_texts, str8_from_lit("Shine same colored"), 1100.f, 510.f);
  lit_push_tutorial_text(&m->tutorial_texts, str8_from_lit("light on this"), 1100.f, 480.f);
  lit_fade_in_next_tutorial_text(&m->tutorial_texts);

  lit_push_tutorial_trigger(&m->tutorial_triggers, lit_level_0_tutorial_trigger_0);
  lit_push_tutorial_trigger(&m->tutorial_triggers, lit_level_0_tutorial_trigger_1);

}

/////////////////////////////////////////////////////////////////////
// Level 1
static void
lit_level_1(Lit* m) {
  lit_push_edge(m, 0.f, 0.f, 1600.f, 0.f);
  lit_push_edge(m, 1600.f, 0.f, 1600.f, 900.f);
  lit_push_edge(m, 1600.f, 900.f, 0.f, 900.f);
  lit_push_edge(m, 0.f, 900.f, 0.f, 0.f);

  // Need to 'enclose' the shape
  lit_push_edge(m, 800.f, 100.f, 800.f, 800.f);
  lit_push_edge(m, 800.f, 800.f, 800.f, 100.f);

  // initialize player
  lit_init_player(m, 200.f, GAME_HEIGHT * 0.5f);

  lit_push_sensor(m, 1200.f, GAME_HEIGHT * 0.5f, 0x008800FF); 
  lit_push_light(m, 500.f, GAME_HEIGHT * 0.5f, 0x008800FF, 90.f, 0.5f);
 
  lit_push_tutorial_text(&m->tutorial_texts, str8_from_lit("Obstacles block light"), 650.f, 820.f);
  lit_fade_in_next_tutorial_text(&m->tutorial_texts);
}


/////////////////////////////////////////////////////////////////////
// Level 2
static void
lit_level_2(Lit* m) {
  lit_push_edge(m, 0.f, 0.f, 1600.f, 0.f);
  lit_push_edge(m, 1600.f, 0.f, 1600.f, 900.f);
  lit_push_edge(m, 1600.f, 900.f, 0.f, 900.f);
  lit_push_edge(m, 0.f, 900.f, 0.f, 0.f);

  // Need to 'enclose' the shape
  lit_push_edge(m, 800.f, 100.f, 800.f, 800.f);
  lit_push_edge(m, 800.f, 800.f, 800.f, 100.f);

  // initialize player
  lit_init_player(m, 200.f, GAME_HEIGHT * 0.5f);

  lit_push_sensor(m, 1200.f, GAME_HEIGHT * 0.5f, 0x008800FF); 
  lit_push_light(m, 500.f, GAME_HEIGHT * 0.5f, 0x008800FF, 90.f, 0.5f);
 
  lit_push_tutorial_text(&m->tutorial_texts, str8_from_lit("Obstacles block light"), 650.f, 820.f);
  lit_fade_in_next_tutorial_text(&m->tutorial_texts);
}

typedef void (*Lit_Level)(Lit* mode); 
static Lit_Level lit_levels[] = {
  lit_level_0,
  lit_level_1,
  lit_level_2,
};


static void
lit_load_level(Lit* m, U32 level_id) {
  al_clear(&m->sensors);
  al_clear(&m->lights);
  al_clear(&m->edges);
  al_clear(&m->tutorial_texts);
  al_clear(&m->tutorial_triggers);
  m->tutorial_texts.next_id_to_fade_in = 0;
  m->tutorial_texts.next_id_to_fade_out = 0;
  m->tutorial_triggers.current_id = 0;

  lit_levels[level_id](m);
}

static void
lit_load_next_level(Lit* m){
  m->current_level_id = (m->current_level_id + 1)%array_count(lit_levels);
  lit_load_level(m, m->current_level_id);  
}

