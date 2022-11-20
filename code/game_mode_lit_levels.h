static B32
lit_trigger_fade_next_text_on_move(Lit* m) {
  if (pf_is_button_down(platform->button_up) || 
      pf_is_button_down(platform->button_down) || 
      pf_is_button_down(platform->button_right) ||
      pf_is_button_down(platform->button_left)) 
  {
    lit_fade_out_next_tutorial_text(&m->tutorial_texts);
    return true;
  }

  return false;
}
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

////////////////////////////////////////////
// Tutorial level
// - Learn to move
// - Learn to rotate
// - Learn that light need to shine on sensors 
static void
lit_level_0_0(Lit* m) {
  lit_push_sensor(m, 400.f, 600.f, 0x880000FF); 
  lit_push_light(m, 400.f, 400, 0x880000FF, 45.f, 0.75f);
  
  // initialize player
  lit_init_player(m, 400.f, 100.f);

  // tutorial text
#if 0
  lit_push_tutorial_text(&m->tutorial_texts, str8_from_lit("WASD to move"), 100.f, 480.f);
  lit_push_tutorial_text(&m->tutorial_texts, str8_from_lit("SPACE to pick up"), 680.f, 480.f);
  lit_push_tutorial_text(&m->tutorial_texts, str8_from_lit("Q/R to rotate light"), 680.f, 480.f);
  lit_push_tutorial_text(&m->tutorial_texts, str8_from_lit("Shine same colored"), 1100.f, 510.f);
  lit_push_tutorial_text(&m->tutorial_texts, str8_from_lit("light on this"), 1100.f, 480.f);
  lit_fade_in_next_tutorial_text(&m->tutorial_texts);

  lit_push_tutorial_trigger(&m->tutorial_triggers, lit_level_0_tutorial_trigger_0);
  lit_push_tutorial_trigger(&m->tutorial_triggers, lit_level_0_tutorial_trigger_1);
#endif
}

////////////////////////////////////////////
// - Learn about obstacles
static void
lit_level_0_1(Lit* m) {
  lit_push_sensor(m, 400.f, 600.f, 0x008800FF); 
  lit_push_light(m, 400.f, 200, 0x008800FF, 45.f, 0.75f);
  
  // initialize player
  lit_init_player(m, 400.f, 100.f);

  // Need to 'enclose' the shape
  lit_push_double_edge(m, 100.f, 400.f, 700.f, 400.f);
}

//////////////////////////////////////////
// - Learn about light saturation 
static void
lit_level_0_2(Lit* m) { 
  // Need to 'enclose' the shape
  lit_push_double_edge(m, 100.f, 400.f, 700.f, 400.f);

  // initialize player
  lit_init_player(m, 400.f, 100.f);

  lit_push_sensor(m, 400.f, 600.f, 0x444488FF); 
  lit_push_light(m, 300.f, 200.f, 0x222244FF, 45.f, 0.75f);
  lit_push_light(m, 500.f, 200.f, 0x222244FF, 45.f, 0.75f);
 
}

//////////////////////////////////////
// - Learn about light saturation 2
// - Sensor is in a box 
static void
lit_level_0_3(Lit* m) { 
  // Need to 'enclose' the shape
  lit_push_double_edge(m, 500.f, 200.f, 600.f, 300.f);
  lit_push_double_edge(m, 300.f, 200.f, 200.f, 300.f);
  lit_push_double_edge(m, 200.f, 500.f, 300.f, 600.f);
  lit_push_double_edge(m, 500.f, 600.f, 600.f, 500.f);

  // initialize player
  lit_init_player(m, 400.f, 100.f);

  lit_push_sensor(m, 400.f, 400.f, 0xCCCC00FF); 
  lit_push_light(m, 150.f, 150.f, 0x333300FF, 45.f, 0.125f);
  lit_push_light(m, 650.f, 150.f, 0x333300FF, 45.f, 0.376f);
  lit_push_light(m, 650.f, 650.f, 0x333300FF, 45.f, 0.626f);
  lit_push_light(m, 150.f, 650.f, 0x333300FF, 45.f, 0.876f);

  
}


//////////////////////////////////////
// - Learn about color combinations
// - R + G = Y
static void
lit_level_0_4(Lit* m) {
  lit_init_player(m, 200.f, GAME_HEIGHT * 0.5f);


  lit_push_sensor(m, 400.f, 400.f, 0x888800FF); 
  
  lit_push_sensor(m, 200.f, 400.f, 0x008800FF); 
  lit_push_sensor(m, 600.f, 400.f, 0x008800FF); 

  lit_push_sensor(m, 400.f, 200.f, 0x880000FF); 
  lit_push_sensor(m, 400.f, 600.f, 0x880000FF); 

  lit_push_light(m, 200.f, GAME_HEIGHT * 0.25f, 0x008800FF, 30.f, 0.5f);
  lit_push_light(m, 600.f, GAME_HEIGHT * 0.75f, 0x880000FF, 30.f, 0.0f);
}

///////////////////////////////////////
// - Learn about color combinations
// - Learn about multiple sensors
// - R + G = Y
static void
lit_level_0_4b(Lit* m) {
  lit_init_player(m, 200.f, GAME_HEIGHT * 0.5f);

  lit_push_edge(m, 0.f, 0.f, 1600.f, 0.f);
  lit_push_edge(m, 1600.f, 0.f, 1600.f, 900.f);
  lit_push_edge(m, 1600.f, 900.f, 0.f, 900.f);
  lit_push_edge(m, 0.f, 900.f, 0.f, 0.f);

  lit_push_sensor(m, 1200.f, GAME_HEIGHT * 0.5f, 0x666600FF); 
  lit_push_sensor(m, 1200.f, GAME_HEIGHT * 0.5f, 0x999999FF); 

  lit_push_light(m, 500.f, GAME_HEIGHT * 0.25f, 0x330000FF, 30.f, 0.5f);
  lit_push_light(m, 500.f, GAME_HEIGHT * 0.75f, 0x003300FF, 30.f, 0.5f);
  lit_push_light(m, 500.f, GAME_HEIGHT * 0.5f, 0x000033FF, 90.f, 0.5f);
}

///////////////////////////////////////
// - Learn about more color combinations
// - G + B = P
// - R + G + B = W
static void
lit_level_0_5(Lit* m) {
  lit_init_player(m, 200.f, GAME_HEIGHT * 0.5f);

  // middle
  lit_push_sensor(m, 400.f,  400.f, 0x888888FF); 

  lit_push_sensor(m, 200.f, 400.f, 0x008888FF); 
  lit_push_sensor(m, 600.f, 400.f, 0x008800FF); 

  lit_push_sensor(m, 400.f, 200.f, 0x880000FF); 
  lit_push_sensor(m, 400.f, 600.f, 0x880088FF); 

  lit_push_light(m, 200.f, 150.f, 0x008800FF, 30.f, 0.75f);
  lit_push_light(m, 400.f, 150.f, 0x880000FF, 30.f, 0.75f);
  lit_push_light(m, 600.f, 150.f, 0x000088FF, 30.f, 0.75f);
  

}

typedef void (*Lit_Level)(Lit* mode); 
static Lit_Level lit_levels[] = {
  //lit_level_0_0, 
  //lit_level_0_1, 
  //lit_level_0_2,
  //lit_level_0_3,
  //lit_level_0_4,
  lit_level_0_5,
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

  lit_push_edge(m, 0.f, 0.f, 800.f, 0.f);
  lit_push_edge(m, 800.f, 0.f, 800.f, 800.f);
  lit_push_edge(m, 800.f, 800.f, 0.f, 800.f);
  lit_push_edge(m, 0.f, 800.f, 0.f, 0.f);
  lit_levels[level_id](m);
}

static void
lit_load_next_level(Lit* m){
  m->current_level_id = (m->current_level_id + 1)%array_count(lit_levels);
  lit_load_level(m, m->current_level_id);  
}

