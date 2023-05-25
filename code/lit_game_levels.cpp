typedef void lit_game_level_t(lit_game_t*);


static void lit_level_menu(lit_game_t* m);
static void lit_level_obstruct(lit_game_t* m);
static void lit_level_add(lit_game_t* m);



static void 
lit_game_sensor_trigger_solved(lit_game_t* game, void* context) {
  lit_game_load_level_t* level_to_load = (lit_game_load_level_t*)context;
  game->solved = true;
  game->level_to_load = level_to_load;
}


static void
lit_game_init_level(lit_game_t* m, str8_t str, u32_t level_id) {
  m->stage_flash_timer = 0.f;
  m->stage_fade_timer = LIT_ENTER_DURATION;

#if LIT_SKIP_TRANSITIONS
  m->stage_fade_timer = 0.f;
  m->state = LIT_GAME_STATE_TYPE_NORMAL;
#else
  m->state = LIT_GAME_STATE_TYPE_TRANSITION_IN;
#endif // LIT_SKIP_TRANSITIONS

  m->sensor_count = 0;
  m->light_count = 0;
  m->edge_count = 0;
  m->animator_count = 0;
  m->point_count = 0;
  m->sensor_group_count = 0;
  m->current_level_id = level_id;

  m->selected_sensor_group_id = array_count(m->sensor_groups);
  m->selected_animator_for_sensor = nullptr; 
  m->selected_animator_for_double_edge_min[0] = nullptr; 
  m->selected_animator_for_double_edge_min[1] = nullptr;
  m->selected_animator_for_double_edge_max[0] = nullptr; 
  m->selected_animator_for_double_edge_max[1] = nullptr;
  m->level_to_load = nullptr;
  m->solved = false;

  lit_game_push_edge(m, 0.f, 0.f, LIT_WIDTH, 0.f);
  lit_game_push_edge(m, LIT_WIDTH, 0.f, LIT_WIDTH, LIT_HEIGHT);
  lit_game_push_edge(m, LIT_WIDTH, LIT_HEIGHT, 0.f, LIT_HEIGHT);
  lit_game_push_edge(m, 0.f, LIT_HEIGHT, 0.f, 0.f);
  lit_init_player(m, 400.f, 400.f); // TODO: any point to this?

  lit_game_set_title(m, str);
}




//
// LEVEL 1
//
// Tutorial level
// - Learn to move
// - Learn to rotate
// - Learn that light need to shine on sensors 
//
static void
lit_level_move(lit_game_t* m) {
  lit_game_init_level(m, str8_from_lit("MOVE"), 1);

  lit_game_push_light(m, 400.f, 400, 0x880000FF, 45.f, 0.75f);

  // If 'tutorial' completed, to go menu, else go to 'obstruct'
  if (lit_is_in_tutorial()) {
    lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_obstruct);
  }
  else {
    lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_menu);
  }
  lit_game_push_sensor(m, 400.f, 600.f, 0x880000FF); 
  lit_game_end_sensor_group(m);
}

//
// LEVEL 2
//
// - Learn about obstacles
//
static void
lit_level_obstruct(lit_game_t* m) {
  lit_game_init_level(m, str8_from_lit("OBSTRUCT"), 2);
  
  if (lit_is_in_tutorial()) {
    lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_add);
  }
  else {
    lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_menu);
  }
  lit_game_push_sensor(m, 400.f, 600.f, 0x008800FF); 
  lit_game_end_sensor_group(m);

  lit_game_push_light(m, 400.f, 200.f, 0x008800FF, 45.f, 0.75f);
  
  // Need to 'enclose' the shape
  lit_game_push_double_edge(m, 100.f, 400.f, 700.f, 400.f);
}

//
// LEVEL 3
//
// - Learn about light saturation 
//
static void
lit_level_add(lit_game_t* m) { 
  // Need to 'enclose' the shape
  lit_game_init_level(m, str8_from_lit("ADD"), 3);
  lit_game_push_double_edge(m, 100.f, 400.f, 700.f, 400.f);

  lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_menu);
  lit_game_push_sensor(m, 400.f, 600.f, 0x444488FF); 
  lit_game_end_sensor_group(m);

  lit_game_push_light(m, 300.f, 200.f, 0x222244FF, 45.f, 0.75f);
  lit_game_push_light(m, 500.f, 200.f, 0x222244FF, 45.f, 0.75f);
 
}

//
// LEVEL 4
//
// - Learn about light saturation 2
// - Sensor is in a box 
//
static void
lit_level_corners(lit_game_t* m) { 
  lit_game_init_level(m, str8_from_lit("CORNERS"), 4);

  // Need to 'enclose' the shape
  lit_game_push_double_edge(m, 500.f, 200.f, 600.f, 300.f);
  lit_game_push_double_edge(m, 300.f, 200.f, 200.f, 300.f);
  lit_game_push_double_edge(m, 200.f, 500.f, 300.f, 600.f);
  lit_game_push_double_edge(m, 500.f, 600.f, 600.f, 500.f);

  lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_menu);
  lit_game_push_sensor(m, 400.f, 400.f, 0xCCCC00FF); 
  lit_game_end_sensor_group(m);

  lit_game_push_light(m, 150.f, 150.f, 0x333300FF, 45.f, 0.125f);
  lit_game_push_light(m, 650.f, 150.f, 0x333300FF, 45.f, 0.376f);
  lit_game_push_light(m, 650.f, 650.f, 0x333300FF, 45.f, 0.626f);
  lit_game_push_light(m, 150.f, 650.f, 0x333300FF, 45.f, 0.876f);

  
}


//
// LEVEL 5
//
// - Learn about color combinations
// - R + G = Y
//
static void
lit_level_mix(lit_game_t* m) {

  lit_game_init_level(m, str8_from_lit("MIX"), 5);

  lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_menu);
  lit_game_push_sensor(m, 400.f, 400.f, 0x888800FF); 
  
  lit_game_push_sensor(m, 200.f, 400.f, 0x008800FF); 
  lit_game_push_sensor(m, 600.f, 400.f, 0x008800FF); 

  lit_game_push_sensor(m, 400.f, 200.f, 0x880000FF); 
  lit_game_push_sensor(m, 400.f, 600.f, 0x880000FF); 
  lit_game_end_sensor_group(m);

  lit_game_push_light(m, 200.f, LIT_HEIGHT * 0.25f, 0x008800FF, 30.f, 0.5f);
  lit_game_push_light(m, 600.f, LIT_HEIGHT * 0.75f, 0x880000FF, 30.f, 0.0f);
}


//
// LEVEL 6
//
// - Learn about more color combinations
// - G + B = P
// - R + G + B = W
//
static void
lit_level_blend(lit_game_t* m) {
  lit_game_init_level(m, str8_from_lit("BLEND"), 6);

  lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_menu);
  lit_game_push_sensor(m, 400.f,  400.f, 0x888888FF); 

  lit_game_push_sensor(m, 200.f, 400.f, 0x008888FF); 
  lit_game_push_sensor(m, 600.f, 400.f, 0x008800FF); 

  lit_game_push_sensor(m, 400.f, 200.f, 0x880000FF); 
  lit_game_push_sensor(m, 400.f, 600.f, 0x880088FF); 
  lit_game_end_sensor_group(m);

  lit_game_push_light(m, 200.f, 150.f, 0x008800FF, 30.f, 0.75f);
  lit_game_push_light(m, 400.f, 150.f, 0x880000FF, 30.f, 0.75f);
  lit_game_push_light(m, 600.f, 150.f, 0x000088FF, 30.f, 0.75f);
  

}

//
// LEVEL 7
//
// - Sensors on inside room and outside room
// - Point lights
//
static void
lit_level_rooms(lit_game_t* m) {

  lit_game_init_level(m, str8_from_lit("ROOMS"), 7);

  // middle
  lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_menu);
  lit_game_push_sensor(m, 050.f,  450.f, 0x666666FF); 
  lit_game_push_sensor(m, 750.f,  450.f, 0x666666FF); 
  lit_game_push_sensor(m, 050.f,  350.f, 0x666666FF); 
  lit_game_push_sensor(m, 750.f,  350.f, 0x666666FF); 
  lit_game_end_sensor_group(m);

  lit_game_push_double_edge(m, 000.f, 400.f, 300.f, 400.f);
  lit_game_push_double_edge(m, 300.f, 250.f, 300.f, 550.f);
  lit_game_push_double_edge(m, 300.f, 650.f, 300.f, 800.f);
  lit_game_push_double_edge(m, 300.f, 000.f, 300.f, 150.f);

  lit_game_push_double_edge(m, 500.f, 400.f, 800.f, 400.f); 
  lit_game_push_double_edge(m, 500.f, 250.f, 500.f, 550.f);
  lit_game_push_double_edge(m, 500.f, 650.f, 500.f, 800.f);
  lit_game_push_double_edge(m, 500.f, 000.f, 500.f, 150.f);

  //lit_game_push_sensor(m, 200.f, 400.f, 0x008888FF); 
  //lit_game_push_sensor(m, 600.f, 400.f, 0x008800FF); 

 //lit_game_push_sensor(m, 400.f, 200.f, 0x880000FF); 
  //lit_game_push_sensor(m, 400.f, 600.f, 0x880088FF); 

  lit_game_push_light(m, 400.f, 400.f, 0x666666FF, 360.f, 0.f);
  lit_game_push_light(m, 400.f, 400.f, 0x666666FF, 360.f, 0.f);
  

}

 
// LEVEL 8
static void
lit_level_disco(lit_game_t* m) {
  lit_game_init_level(m, str8_from_lit("DISCO"), 8);

  lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_menu);
  // bottom left room
  lit_game_push_sensor(m, 050.f,  350.f, 0x660000FF); 
  lit_game_push_sensor(m, 050.f,  050.f, 0x660000FF); 
  lit_game_push_sensor(m, 250.f,  350.f, 0x660000FF); 
  lit_game_push_sensor(m, 250.f,  050.f, 0x660000FF); 

  // bottom right room
  lit_game_push_sensor(m, 550.f,  350.f, 0x006600FF); 
  lit_game_push_sensor(m, 550.f,  050.f, 0x006600FF); 
  lit_game_push_sensor(m, 750.f,  350.f, 0x006600FF); 
  lit_game_push_sensor(m, 750.f,  050.f, 0x006600FF); 

  // top right room
  lit_game_push_sensor(m, 550.f,  450.f, 0x000066FF); 
  lit_game_push_sensor(m, 550.f,  750.f, 0x000066FF); 
  lit_game_push_sensor(m, 750.f,  450.f, 0x000066FF); 
  lit_game_push_sensor(m, 750.f,  750.f, 0x000066FF); 


  // top left room
  lit_game_push_sensor(m, 050.f,  450.f, 0x666666FF); 
  lit_game_push_sensor(m, 050.f,  750.f, 0x666666FF); 
  lit_game_push_sensor(m, 250.f,  450.f, 0x666666FF); 
  lit_game_push_sensor(m, 250.f,  750.f, 0x666666FF); 

  lit_game_end_sensor_group(m);


  lit_game_push_double_edge(m, 000.f, 400.f, 300.f, 400.f);
  lit_game_push_double_edge(m, 300.f, 250.f, 300.f, 550.f);
  lit_game_push_double_edge(m, 300.f, 650.f, 300.f, 800.f);
  lit_game_push_double_edge(m, 300.f, 000.f, 300.f, 150.f);

  lit_game_push_double_edge(m, 500.f, 400.f, 800.f, 400.f); 
  lit_game_push_double_edge(m, 500.f, 250.f, 500.f, 550.f);
  lit_game_push_double_edge(m, 500.f, 650.f, 500.f, 800.f);
  lit_game_push_double_edge(m, 500.f, 000.f, 500.f, 150.f);
  //lit_game_push_sensor(m, 200.f, 400.f, 0x008888FF); 
  //lit_game_push_sensor(m, 600.f, 400.f, 0x008800FF); 

 //lit_game_push_sensor(m, 400.f, 200.f, 0x880000FF); 
  //lit_game_push_sensor(m, 400.f, 600.f, 0x880088FF); 

  lit_game_push_light(m, 400.f, 400.f, 0x660000FF, 360.f, 0.f);
  lit_game_push_light(m, 400.f, 400.f, 0x006600FF, 360.f, 0.f);
  lit_game_push_light(m, 400.f, 400.f, 0x000066FF, 360.f, 0.f);
  lit_game_push_light(m, 400.f, 400.f, 0x666666FF, 360.f, 0.f);
}

//
// LEVEL 9
//
// Onion 
//  
static void
lit_level_onion(lit_game_t* m) {
  lit_game_init_level(m, str8_from_lit("ONION"), 9);

  lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_menu);
  // layer 1
  lit_game_push_sensor(m, 550.f,  250.f, 0x006600FF); 
  lit_game_push_sensor(m, 250.f,  550.f, 0x660000FF); 
  lit_game_push_sensor(m, 250.f,  250.f, 0x666600FF); 
  lit_game_push_sensor(m, 550.f,  550.f, 0x666600FF); 

  // layer 2
  lit_game_push_sensor(m, 150.f,  150.f, 0x660066FF); 
  lit_game_push_sensor(m, 650.f,  150.f, 0x660066FF); 
  lit_game_push_sensor(m, 150.f,  650.f, 0x660000FF); 
  lit_game_push_sensor(m, 650.f,  650.f, 0x000066FF); 

  // layer 3
  lit_game_push_sensor(m, 050.f,  050.f, 0x666666FF); 
  lit_game_push_sensor(m, 750.f,  750.f, 0x666666FF); 
  lit_game_end_sensor_group(m);

  lit_game_push_box(m, 100.f, 100.f, 700.f, 700.f);
  lit_game_push_box(m, 200.f, 200.f, 600.f, 600.f);
  lit_game_push_box(m, 300.f, 300.f, 500.f, 500.f);



  lit_game_push_light(m, 400.f, 400.f, 0x660000FF, 360.f, 0.f);
  lit_game_push_light(m, 400.f, 400.f, 0x660000FF, 360.f, 0.f);
  lit_game_push_light(m, 400.f, 400.f, 0x006600FF, 360.f, 0.f);
  lit_game_push_light(m, 400.f, 400.f, 0x000066FF, 360.f, 0.f);
  lit_game_push_light(m, 400.f, 400.f, 0x333333FF, 360.f, 0.f);
  lit_game_push_light(m, 400.f, 400.f, 0x333333FF, 360.f, 0.f);
}

// LEVEL 10
static void
lit_level_spectrum(lit_game_t* m) {
  lit_game_init_level(m, str8_from_lit("SPECTRUM"), 10);

  lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_menu);
  // cyans
  lit_game_push_sensor(m, 050.f,   750.f, 0x008888FF); 
  lit_game_push_sensor(m, 050.f,   50.f,  0x008888FF); 

  // magentas
  lit_game_push_sensor(m, 750.f,   750.f, 0x880088FF); 
  lit_game_push_sensor(m, 750.f,   50.f,  0x880088FF); 
  

  // whites
  lit_game_push_sensor(m, 400.f,   750.f, 0x888888FF); 
  lit_game_push_sensor(m, 400.f,   50.f,  0x888888FF); 

  // yellows
  lit_game_push_sensor(m, 250.f,   750.f, 0x888800FF); 
  lit_game_push_sensor(m, 550.f,   750.f, 0x888800FF); 
  lit_game_push_sensor(m, 250.f,   050.f, 0x888800FF); 
  lit_game_push_sensor(m, 550.f,   50.f, 0x888800FF); 

  lit_game_end_sensor_group(m);
  //lit_game_push_sensor(m, 0.f,   0.f, 0xFFFFFFFF); 

  //lit_game_push_double_edge(m, 300.f, 300.f, 500.f, 300.f);
  lit_game_push_double_edge(m, 340.f, 300.f, 380.f, 300.f);
  lit_game_push_double_edge(m, 420.f, 300.f, 460.f, 300.f);

  lit_game_push_double_edge(m, 340.f, 500.f, 380.f, 500.f);
  lit_game_push_double_edge(m, 420.f, 500.f, 460.f, 500.f);

  lit_game_push_double_edge(m, 500.f, 340.f, 500.f, 380.f);
  lit_game_push_double_edge(m, 500.f, 420.f, 500.f, 460.f);

  lit_game_push_double_edge(m, 300.f, 340.f, 300.f, 380.f);
  lit_game_push_double_edge(m, 300.f, 420.f, 300.f, 460.f);

  lit_game_push_light(m, 400.f, 410.f, 0x880000FF, 360.f, 0.f);
  lit_game_push_light(m, 390.f, 390.f, 0x008800FF, 360.f, 0.f);
  lit_game_push_light(m, 410.f, 390.f, 0x000088FF, 360.f, 0.f);
}

// LEVEL 11
static void
lit_level_split(lit_game_t* m) {
  lit_game_init_level(m, str8_from_lit("SPLIT"), 11);

  lit_game_push_light(m, 400.f, 410.f, 0x880000FF, 360.f, 0.f);
  lit_game_push_light(m, 390.f, 390.f, 0x008800FF, 360.f, 0.f);
  lit_game_push_light(m, 410.f, 390.f, 0x000088FF, 360.f, 0.f);

  lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_menu);

  lit_game_push_sensor(m, 400.f,   750.f, 0x888888FF); 
  lit_game_push_sensor(m, 50.f,   50.f, 0x888888FF); 
  lit_game_push_sensor(m, 750.f,   50.f, 0x888888FF); 

  lit_game_push_sensor(m, 50.f,   750.f, 0x888800FF); 
  lit_game_push_sensor(m, 750.f,   750.f, 0x880088FF); 
  lit_game_push_sensor(m, 400.f,   50.f, 0x008888FF); 

  lit_game_push_sensor(m, 400.f,   550.f, 0x880000FF); 
  lit_game_push_sensor(m, 275.f,   380.f, 0x008800FF); 
  lit_game_push_sensor(m, 525.f,   380.f, 0x000088FF); 

  lit_game_end_sensor_group(m);

  //lit_game_push_sensor(m, 0.f,   0.f, 0xFFFFFFFF); 
  lit_game_push_box(m, 300, 300, 500, 500);
  //lit_game_push_box(m, 150, 350, 250, 450);

}

//LEVEL 12
static void
lit_level_interval(lit_game_t* m) {
  lit_game_init_level(m, str8_from_lit("INTERVAL"), 12);

  const u32_t sections = 20;
  f32_t offset = LIT_WIDTH/20; 
  {
    f32_t cur_offset = 0.f;
    for (u32_t i = 0; i < sections; ++i) {
      lit_game_push_double_edge(m, cur_offset, 0.f, cur_offset, LIT_HEIGHT);
      cur_offset += offset; 
    }
  }



  u32_t color = 0x444488FF;
  {
    f32_t cur_offset = offset/2+ offset;
    lit_game_push_light(m, cur_offset, 50.f, color, 360.f, 0.25f);
    lit_game_push_light(m, cur_offset += offset*2, 50.f, color, 360.f, 0.25f);
    lit_game_push_light(m, cur_offset += offset*2, 50.f, color, 360.f, 0.25f);
    lit_game_push_light(m, cur_offset += offset*2, 50.f, color, 360.f, 0.25f);
    lit_game_push_light(m, cur_offset += offset*2, 50.f, color, 360.f, 0.25f);
    lit_game_push_light(m, cur_offset += offset*2, 50.f, color, 360.f, 0.25f);
    lit_game_push_light(m, cur_offset += offset*2, 50.f, color, 360.f, 0.25f);
    lit_game_push_light(m, cur_offset += offset*2, 50.f, color, 360.f, 0.25f);
    lit_game_push_light(m, cur_offset += offset*2, 50.f, color, 360.f, 0.25f);
    lit_game_push_light(m, cur_offset += offset*2, 50.f, color, 360.f, 0.25f);
 }

  f32_t duration = 10.f;
  f32_t start_x = -LIT_WIDTH;
  f32_t end_x = LIT_WIDTH;

  lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_menu);
  {
    lit_game_begin_patrolling_sensor(m, start_x + offset, 400.f, color, duration);
    lit_game_push_patrolling_sensor_waypoint(m, end_x + offset, 400.f); 
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, start_x + offset*2, 400.f, color, duration);
    lit_game_push_patrolling_sensor_waypoint(m, end_x + offset*2, 400.f); 
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, start_x + offset*4, 400.f, color, duration);
    lit_game_push_patrolling_sensor_waypoint(m, end_x + offset*4, 400.f); 
    lit_game_end_patrolling_sensor(m);


    lit_game_begin_patrolling_sensor(m, start_x + offset*5, 400.f, color, duration);
    lit_game_push_patrolling_sensor_waypoint(m, end_x + offset*5, 400.f); 
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, start_x + offset*8, 400.f, color, duration);
    lit_game_push_patrolling_sensor_waypoint(m, end_x + offset*8, 400.f); 
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, start_x + offset*9, 400.f, color, duration);
    lit_game_push_patrolling_sensor_waypoint(m, end_x + offset*9, 400.f); 
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, start_x + offset*11, 400.f, color, duration);
    lit_game_push_patrolling_sensor_waypoint(m, end_x + offset*11, 400.f); 
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, start_x + offset*13, 400.f, color, duration);
    lit_game_push_patrolling_sensor_waypoint(m, end_x + offset*13, 400.f); 
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, start_x + offset*15, 400.f, color, duration);
    lit_game_push_patrolling_sensor_waypoint(m, end_x + offset*15, 400.f); 
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, start_x + offset*18, 400.f, color, duration);
    lit_game_push_patrolling_sensor_waypoint(m, end_x + offset*18, 400.f); 
    lit_game_end_patrolling_sensor(m);
  }
  lit_game_end_sensor_group(m);

  //lit_game_push_sensor(m, 0.f,   0.f, 0xFFFFFFFF); 
  //lit_game_push_box(m, 300, 300, 500, 500);
  //lit_game_push_box(m, 150, 350, 250, 450);

}


// LEVEL 13
static void
lit_level_movement(lit_game_t* m) {
  lit_game_init_level(m, str8_from_lit("MOVEMENT"), 13);

//  lit_game_push_light(m, 400.f, 100.f, 0x880000FF, 15.f, 0.25f);
  lit_game_push_light(m, 400.f, 700.f, 0x008800FF, 15.f, 0.25f);
  lit_game_push_light(m, 150.f, 700.f, 0x880000FF, 15.f, 0.25f);
  lit_game_push_light(m, 600.f, 700.f, 0x000088FF, 15.f, 0.25f);

  f32_t duration = 5.25f;

  lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_menu);
  {
    lit_game_begin_patrolling_sensor(m, 150.f, 150.f, 0x880088FF, duration);
    lit_game_push_patrolling_sensor_waypoint(m, 650.f, 650.f);
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, 650.f, 650.f, 0x880088FF, duration);
    lit_game_push_patrolling_sensor_waypoint(m, 150.f, 150.f);
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, 150.f, 400.f, 0x880088FF, duration);
    lit_game_push_patrolling_sensor_waypoint(m, 650.f, 400.f);
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, 650.f, 400.f, 0x880088FF, duration);
    lit_game_push_patrolling_sensor_waypoint(m, 150.f, 400.f);
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, 150.f, 650.f, 0x880088FF, duration);
    lit_game_push_patrolling_sensor_waypoint(m, 650.f, 150.f);
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, 650.f, 150.f, 0x880088FF, duration);
    lit_game_push_patrolling_sensor_waypoint(m, 150.f, 650.f);
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, 400.f, 650.f, 0x880088FF, duration);
    lit_game_push_patrolling_sensor_waypoint(m, 400.f, 150.f);
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, 400.f, 150.f, 0x880088FF, duration);
    lit_game_push_patrolling_sensor_waypoint(m, 400.f, 650.f);
    lit_game_end_patrolling_sensor(m);
  }
  lit_game_end_sensor_group(m);

}

static void
lit_level_patience(lit_game_t* m) {
  lit_game_init_level(m, str8_from_lit("PATIENCE"), 14);
  lit_game_push_light(m, 400.f, 100.f, 0x880000FF, 15.f, 0.25f);
  lit_game_push_light(m, 400.f, 700.f, 0x008800FF, 15.f, 0.75f);

  lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_menu);
  {
    lit_game_push_sensor(m, 400.f, 400.f, 0x888888FF); 

    lit_game_begin_patrolling_sensor(m, 650.f, 150.f, 0x880000FF, 5.f);
    lit_game_push_patrolling_sensor_waypoint(m, 650.f, 650.f);
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, 150.f, 650.f, 0x008800FF, 5.25f);
    lit_game_push_patrolling_sensor_waypoint(m, 150.f, 150.f);
    lit_game_end_patrolling_sensor(m);
  }
  lit_game_end_sensor_group(m);
}

static void
lit_level_busy(lit_game_t* m) {
  lit_game_init_level(m, str8_from_lit("BUSY"), 15);
  //lit_game_push_light(m, 400.f, 100.f, 0x880000FF, 15.f, 0.25f);
#if LIT_INIT_WITH_SOLUTION
  lit_game_push_light(m, 600.f, 700.f, 0x660000FF, 15.f, 0.75f);
  lit_game_push_light(m, 600.f, 700.f, 0x660000FF, 15.f, 0.75f);
  lit_game_push_light(m, 200.f, 700.f, 0x006600FF, 15.f, 0.75f);
  lit_game_push_light(m, 200.f, 700.f, 0x006600FF, 15.f, 0.75f);
#else
  lit_game_push_light(m, 400.f, 750.f, 0x006600FF, 15.f, 0.75f);
  lit_game_push_light(m, 050.f, 400.f, 0x006600FF, 15.f, 0.f);
  lit_game_push_light(m, 050.f, 750.f, 0x660000FF, 15.f, 0.87f);
  lit_game_push_light(m, 750.f, 750.f, 0x660000FF, 15.f, 0.62f);
#endif
  //lit_game_push_light(m, 200.f, 700.f, 0x006600FF, 15.f, 0.75f);

  //lit_game_push_light(m, 200.f, 700.f, 0x002200FF, 15.f, 0.75f);
  //lit_game_push_light(m, 200.f, 700.f, 0x002200FF, 15.f, 0.75f);
 
  lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_menu);
  {
    // patrols in a square
    lit_game_begin_patrolling_sensor(m, 150.f, 650.f, 0x006600FF, 5.f);
    lit_game_push_patrolling_sensor_waypoint(m, 150.f, 150.f);
    lit_game_push_patrolling_sensor_waypoint(m, 650.f, 150.f);
    lit_game_push_patrolling_sensor_waypoint(m, 650.f, 650.f);
    lit_game_end_patrolling_sensor(m);

    // patrols in a small square
    lit_game_begin_patrolling_sensor(m, 550.f, 250.f, 0x006600FF, 5.f);
    lit_game_push_patrolling_sensor_waypoint(m, 250.f, 250.f);
    lit_game_push_patrolling_sensor_waypoint(m, 250.f, 550.f);
    lit_game_push_patrolling_sensor_waypoint(m, 550.f, 550.f);
    lit_game_end_patrolling_sensor(m);

    // patrols in an hourglass (1)
    lit_game_begin_patrolling_sensor(m, 650.f, 650.f, 0x660000FF, 2.5f);
    lit_game_push_patrolling_sensor_waypoint(m, 650.f, 150.f);
    lit_game_push_patrolling_sensor_waypoint(m, 150.f, 650.f);
    lit_game_push_patrolling_sensor_waypoint(m, 150.f, 150.f);
    lit_game_end_patrolling_sensor(m);

    // patrols in an hourglass (2)
    lit_game_begin_patrolling_sensor(m, 150.f, 150.f, 0x660000FF, 2.5f);
    lit_game_push_patrolling_sensor_waypoint(m, 150.f, 650.f);
    lit_game_push_patrolling_sensor_waypoint(m, 650.f, 150.f);
    lit_game_push_patrolling_sensor_waypoint(m, 650.f, 650.f);
    lit_game_end_patrolling_sensor(m);
  

    // Runs across the scene vertially
    lit_game_begin_patrolling_sensor(m, 400.f, -100.f, 0xCCCC00FF, 5.f);
    lit_game_push_patrolling_sensor_waypoint(m, 400.f, 900.f);
    lit_game_end_patrolling_sensor(m);


    // Runs across the scene horizontally
    lit_game_begin_patrolling_sensor(m, -100.f, 400.f, 0xCCCC00FF, 5.f);
    lit_game_push_patrolling_sensor_waypoint(m, 900.f, 400.f);
    lit_game_end_patrolling_sensor(m);
  }
  lit_game_end_sensor_group(m);



#if 0
  lit_game_push_patrolling_sensor(m, 5.f, 
                             v2f_set(150.f, 150.f), 
                             v2f_set(650.f, 150.f),
                             0x008800FF); 
  lit_game_push_patrolling_sensor(m, 5.25f, 
                             v2f_set(150.f, 250.f), 
                             v2f_set(650.f, 250.f),
                             0x880000FF); 
  lit_game_push_patrolling_sensor(m, 5.25f, 
                             v2f_set(150.f, 350.f), 
                             v2f_set(650.f, 350.f),
                             0x880000FF); 

#endif

}

// LEVEL 16
static void
lit_level_spin(lit_game_t* m) {
  lit_game_init_level(m, str8_from_lit("SPIN"), 16);

  f32_t speed = 1.f;

  u32_t colors[] = {
    0x880000FF,
    0x008800FF,
    0x000088FF,
    0x888800FF,
    0x008888FF,
    0x880088FF,
  };
  f32_t angle_per_color = TAU_32/array_count(colors);

  v2f_t* origin = lit_game_push_point(m, v2f_set(400.f, 400.f));
  {
    v2f_t dir = v2f_set(0.f, 25.f);
    for_arr(i, colors) {
      f32_t turn = i * (1.f/array_count(colors)) + 0.25f; 

      lit_game_push_light(m, origin->x + dir.x, origin->y + dir.y, 
          colors[array_count(colors)-i-1], 
          f32_rad_to_deg(angle_per_color/2), turn);

      dir = v2f_rotate(dir, angle_per_color);
    }
  }

  lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_menu);
  // Inner circle
  {
    v2f_t dir = v2f_set(0.f, 100.f);
    for_arr(i, colors) {
      dir = v2f_rotate(dir, angle_per_color);
      lit_game_push_rotating_sensor(m, origin->x + dir.x, origin->y + dir.y , origin, speed, colors[i]); 
    }
  }

  // Outer circle
  {
    v2f_t dir = v2f_set(0.f, -200.f);
    for_arr(i, colors) {
      dir = v2f_rotate(dir, angle_per_color);
      lit_game_push_rotating_sensor(m, origin->x + dir.x, origin->y + dir.y , origin, -speed, colors[i]); 
    }
  }
  lit_game_end_sensor_group(m);

}

// LEVEL 17
static void
lit_level_orbit(lit_game_t* m) {
  lit_game_init_level(m, str8_from_lit("ORBIT"), 17);

  f32_t speed = 1.f;

  u32_t colors[] = {
    0x880000FF,
    0x008800FF,
    0x000088FF,
    0x888800FF,
    0x008888FF,
    0x880088FF,
  };
  f32_t angle_per_color = TAU_32/array_count(colors);

  v2f_t* origin = lit_game_push_point(m, v2f_set(400.f, 600.f));
  v2f_t* origin_2 = lit_game_push_point(m, v2f_set(400.f, 400.f));

  lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_menu);
  {
    v2f_t dir = v2f_set(0.f, 25.f);
    for_arr(i, colors) {
      f32_t turn = i * (1.f/array_count(colors)) + 0.25f; 

      lit_game_push_light(m, origin_2->x + dir.x, origin_2->y + dir.y, 
          colors[array_count(colors)-i-1], 
          f32_rad_to_deg(angle_per_color/2), turn);

      dir = v2f_rotate(dir, angle_per_color);
    }
  }

  // Inner circle
  {
    v2f_t dir = v2f_set(0.f, 50.f);
    for_arr(i, colors) {
      dir = v2f_rotate(dir, angle_per_color);
      lit_game_push_rotating_sensor(m, origin->x + dir.x, origin->y + dir.y , origin, speed, colors[i]); 
    }
  }

  // Outer circle
  {
    v2f_t dir = v2f_set(0.f, -100.f);
    for_arr(i, colors) {
      dir = v2f_rotate(dir, angle_per_color);
      lit_game_push_rotating_sensor(m, origin->x + dir.x, origin->y + dir.y , origin, -speed, colors[i]); 
    }
  }
  lit_game_end_sensor_group(m);

  lit_game_animator_push_rotate_point(m, origin, origin_2, 1.f); 
}

// TODO LEVEL 18
// TODO LEVEL 19
// TODO LEVEL 20
static void
lit_level_test2(lit_game_t* m) {
  lit_game_init_level(m, str8_from_lit("MOVE"), 1);

  lit_game_push_light(m, 250, 400, 0x880000FF, 360.f, 0.75f);
  lit_game_push_light(m, 550, 400, 0x008800FF, 360.f, 0.75f);
  lit_game_push_light(m, 400, 250, 0x000088FF, 360.f, 0.75f);
  lit_game_push_light(m, 400, 550, 0x444444FF, 360.f, 0.75f);

  lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_menu);
  lit_game_push_sensor(m, 100.f, 100.f, 0x888800FF); 
  lit_game_push_sensor(m, 400.f, 700.f, 0x008888FF); 
  lit_game_push_sensor(m, 700.f, 100.f, 0x880088FF); 
  lit_game_push_sensor(m, 400.f, 400.f, 0x444444FF); 
  lit_game_end_sensor_group(m);

  lit_game_begin_patrolling_double_edge(m, 100.f, 100.f, 200.f, 100.f, 2.f);
  lit_game_push_patrolling_double_edge_waypoint_for_min(m, 600.f, 100.f);
  lit_game_push_patrolling_double_edge_waypoint_for_max(m, 700.f, 100.f);

  lit_game_push_patrolling_double_edge_waypoint_for_min(m, 100.f, 600.f);
  lit_game_push_patrolling_double_edge_waypoint_for_max(m, 200.f, 600.f);

  lit_game_push_patrolling_double_edge_waypoint_for_min(m, 600.f, 600.f);
  lit_game_push_patrolling_double_edge_waypoint_for_max(m, 700.f, 600.f);
  lit_game_end_patrolling_double_edge(m);

  lit_game_begin_patrolling_double_edge(m, 100.f, 200.f, 200.f, 200.f, 2.f);
  lit_game_push_patrolling_double_edge_waypoint_for_min(m, 600.f, 200.f);
  lit_game_push_patrolling_double_edge_waypoint_for_max(m, 700.f, 200.f);

  lit_game_push_patrolling_double_edge_waypoint_for_min(m, 100.f, 700.f);
  lit_game_push_patrolling_double_edge_waypoint_for_max(m, 200.f, 700.f);

  lit_game_push_patrolling_double_edge_waypoint_for_min(m, 600.f, 700.f);
  lit_game_push_patrolling_double_edge_waypoint_for_max(m, 700.f, 700.f);
  lit_game_end_patrolling_double_edge(m);


  lit_game_begin_patrolling_double_edge(m, 100.f, 100.f, 100.f, 200.f, 2.f);
  lit_game_push_patrolling_double_edge_waypoint_for_min(m, 600.f, 100.f);
  lit_game_push_patrolling_double_edge_waypoint_for_max(m, 600.f, 200.f);

  lit_game_push_patrolling_double_edge_waypoint_for_min(m, 100.f, 600.f);
  lit_game_push_patrolling_double_edge_waypoint_for_max(m, 100.f, 700.f);

  lit_game_push_patrolling_double_edge_waypoint_for_min(m, 600.f, 600.f);
  lit_game_push_patrolling_double_edge_waypoint_for_max(m, 600.f, 700.f);
  lit_game_end_patrolling_double_edge(m);


  lit_game_begin_patrolling_double_edge(m, 200.f, 100.f, 200.f, 200.f, 2.f);
  lit_game_push_patrolling_double_edge_waypoint_for_min(m, 700.f, 100.f);
  lit_game_push_patrolling_double_edge_waypoint_for_max(m, 700.f, 200.f);

  lit_game_push_patrolling_double_edge_waypoint_for_min(m, 200.f, 600.f);
  lit_game_push_patrolling_double_edge_waypoint_for_max(m, 200.f, 700.f);

  lit_game_push_patrolling_double_edge_waypoint_for_min(m, 700.f, 600.f);
  lit_game_push_patrolling_double_edge_waypoint_for_max(m, 700.f, 700.f);
  lit_game_end_patrolling_double_edge(m);
}

#if 0
static void
lit_level_test(lit_game_t* m) {
  lit_game_init_level(m, str8_from_lit("TEST"), 1);

  //lit_game_push_light(m, 250, 400, 0x880000FF, 360.f, 0.75f);
  //lit_game_push_light(m, 550, 400, 0x008800FF, 360.f, 0.75f);
  //lit_game_push_light(m, 400, 250, 0x000088FF, 360.f, 0.75f);
  lit_game_push_light(m, 400, 550, 0x444444FF, 360.f, 0.75f);

  lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_menu);
  lit_game_push_sensor(m, 100.f, 100.f, 0x888800FF); 
  lit_game_push_sensor(m, 400.f, 700.f, 0x008888FF); 
  lit_game_push_sensor(m, 700.f, 100.f, 0x880088FF); 
  lit_game_push_sensor(m, 400.f, 400.f, 0x444444FF); 
  lit_game_end_sensor_group(m);

  // Square #1
  lit_game_begin_patrolling_double_edge(m, 350.f, 350.f, 450.f, 350.f, 2.f);
  lit_game_push_patrolling_double_edge_waypoint_for_min(m, 100.f, 100.f);
  lit_game_push_patrolling_double_edge_waypoint_for_max(m, 700.f, 100.f);
  lit_game_end_patrolling_double_edge(m);

  lit_game_begin_patrolling_double_edge(m, 350.f, 450.f, 450.f, 450.f, 2.f);
  lit_game_push_patrolling_double_edge_waypoint_for_min(m, 100.f, 700.f);
  lit_game_push_patrolling_double_edge_waypoint_for_max(m, 700.f, 700.f);
  lit_game_end_patrolling_double_edge(m);


  lit_game_begin_patrolling_double_edge(m, 350.f, 350.f, 350.f, 450.f, 2.f);
  lit_game_push_patrolling_double_edge_waypoint_for_min(m, 100.f, 100.f);
  lit_game_push_patrolling_double_edge_waypoint_for_max(m, 100.f, 700.f);
  lit_game_end_patrolling_double_edge(m);

  lit_game_begin_patrolling_double_edge(m, 450.f, 350.f, 450.f, 450.f, 2.f);
  lit_game_push_patrolling_double_edge_waypoint_for_min(m, 700.f, 100.f);
  lit_game_push_patrolling_double_edge_waypoint_for_max(m, 700.f, 700.f);
  lit_game_end_patrolling_double_edge(m);

  // Square #2
  lit_game_begin_patrolling_double_edge(m, 100.f, 400.f, 400.f, 700.f, 2.f);
  lit_game_push_patrolling_double_edge_waypoint_for_min(m, 300.f, 400.f);
  lit_game_push_patrolling_double_edge_waypoint_for_max(m, 400.f, 500.f);
  lit_game_end_patrolling_double_edge(m);

  lit_game_begin_patrolling_double_edge(m, 400.f, 700.f, 700.f, 400.f, 2.f);
  lit_game_push_patrolling_double_edge_waypoint_for_min(m, 400.f, 500.f);
  lit_game_push_patrolling_double_edge_waypoint_for_max(m, 500.f, 400.f);
  lit_game_end_patrolling_double_edge(m);

  lit_game_begin_patrolling_double_edge(m, 700, 400, 400, 100, 2.f);
  lit_game_push_patrolling_double_edge_waypoint_for_min(m, 500, 400);
  lit_game_push_patrolling_double_edge_waypoint_for_max(m, 400, 300);
  lit_game_end_patrolling_double_edge(m);

  lit_game_begin_patrolling_double_edge(m, 400, 100, 100, 400, 2.f);
  lit_game_push_patrolling_double_edge_waypoint_for_min(m, 400, 300);
  lit_game_push_patrolling_double_edge_waypoint_for_max(m, 300, 400);
  lit_game_end_patrolling_double_edge(m);


}

#endif

static void
lit_level_triforce(lit_game_t* m) {
  lit_game_init_level(m, str8_from_lit("TRIFORCE"), 1);

  lit_game_push_light(m, 400, 427, 0x880000FF, 360.f, 0.75f);
  lit_game_push_light(m, 380, 380, 0x008800FF, 360.f, 0.75f);
  lit_game_push_light(m, 420, 380, 0x000088FF, 360.f, 0.75f);

  lit_game_push_light(m, 400, 427, 0x880000FF, 360.f, 0.75f);
  lit_game_push_light(m, 380, 380, 0x008800FF, 360.f, 0.75f);
  lit_game_push_light(m, 420, 380, 0x000088FF, 360.f, 0.75f);

  lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_menu);
  lit_game_push_sensor(m, 400.f, 600.f, 0x000088FF); 
  lit_game_push_sensor(m, 250.f, 350.f, 0x880000FF); 
  lit_game_push_sensor(m, 550.f, 350.f, 0x008800FF); 

  lit_game_push_sensor(m, 700.f, 700.f, 0x888800FF); 
  lit_game_push_sensor(m, 100.f, 700.f, 0x008888FF); 
  lit_game_push_sensor(m, 400.f, 100.f, 0x880088FF); 
  lit_game_end_sensor_group(m);



  // Triangle #1
  {
    v2f_t a = v2f_set(0.f, 1.f);
    v2f_t b = v2f_rotate(a, f32_deg_to_rad(120.f));
    v2f_t c = v2f_rotate(a, f32_deg_to_rad(-120.f));

    v2f_t a_max = a * 350.f + v2f_set(400.f, 400.f);
    v2f_t b_max = b * 350.f + v2f_set(400.f, 400.f);
    v2f_t c_max = c * 350.f + v2f_set(400.f, 400.f);

    v2f_t a_min = a * 100.f + v2f_set(400.f, 400.f);
    v2f_t b_min = b * 100.f + v2f_set(400.f, 400.f);
    v2f_t c_min = c * 100.f + v2f_set(400.f, 400.f);

    lit_game_begin_patrolling_double_edge(m, a_max.x, a_max.y, b_max.x, b_max.y,  2.f);
    lit_game_push_patrolling_double_edge_waypoint_for_min(m, a_min.x, a_min.y);
    lit_game_push_patrolling_double_edge_waypoint_for_max(m, b_min.x, b_min.y);
    lit_game_end_patrolling_double_edge(m);

    lit_game_begin_patrolling_double_edge(m, b_max.x, b_max.y, c_max.x, c_max.y,  2.f);
    lit_game_push_patrolling_double_edge_waypoint_for_min(m, b_min.x, b_min.y);
    lit_game_push_patrolling_double_edge_waypoint_for_max(m, c_min.x, c_min.y);
    lit_game_end_patrolling_double_edge(m);

    lit_game_begin_patrolling_double_edge(m, c_max.x, c_max.y, a_max.x, a_max.y,  2.f);
    lit_game_push_patrolling_double_edge_waypoint_for_min(m, c_min.x, c_min.y);
    lit_game_push_patrolling_double_edge_waypoint_for_max(m, a_min.x, a_min.y);
    lit_game_end_patrolling_double_edge(m);
  }

  // Triangle #2
  {

    v2f_t a = v2f_set(0.f, -1.f);
    v2f_t b = v2f_rotate(a, f32_deg_to_rad(120.f));
    v2f_t c = v2f_rotate(a, f32_deg_to_rad(-120.f));

    v2f_t a_max = a * 175.f + v2f_set(400.f, 400.f);
    v2f_t b_max = b * 175.f + v2f_set(400.f, 400.f);
    v2f_t c_max = c * 175.f + v2f_set(400.f, 400.f);

    v2f_t a_min = a * 50.f + v2f_set(400.f, 400.f);
    v2f_t b_min = b * 50.f + v2f_set(400.f, 400.f);
    v2f_t c_min = c * 50.f + v2f_set(400.f, 400.f);

    lit_game_begin_patrolling_double_edge(m, a_max.x, a_max.y, b_max.x, b_max.y,  2.f);
    lit_game_push_patrolling_double_edge_waypoint_for_min(m, a_min.x, a_min.y);
    lit_game_push_patrolling_double_edge_waypoint_for_max(m, b_min.x, b_min.y);
    lit_game_end_patrolling_double_edge(m);

    lit_game_begin_patrolling_double_edge(m, b_max.x, b_max.y, c_max.x, c_max.y,  2.f);
    lit_game_push_patrolling_double_edge_waypoint_for_min(m, b_min.x, b_min.y);
    lit_game_push_patrolling_double_edge_waypoint_for_max(m, c_min.x, c_min.y);
    lit_game_end_patrolling_double_edge(m);

    lit_game_begin_patrolling_double_edge(m, c_max.x, c_max.y, a_max.x, a_max.y,  2.f);
    lit_game_push_patrolling_double_edge_waypoint_for_min(m, c_min.x, c_min.y);
    lit_game_push_patrolling_double_edge_waypoint_for_max(m, a_min.x, a_min.y);
    lit_game_end_patrolling_double_edge(m);
  }

}

static void
lit_level_test(lit_game_t* m) {
  lit_game_init_level(m, str8_from_lit("TEST"), 1);

  lit_game_push_light(m, 400, 427, 0x880000FF, 360.f, 0.75f);
  lit_game_push_light(m, 380, 380, 0x008800FF, 360.f, 0.75f);
  lit_game_push_light(m, 420, 380, 0x000088FF, 360.f, 0.75f);

  lit_game_push_light(m, 400, 427, 0x880000FF, 360.f, 0.75f);
  lit_game_push_light(m, 380, 380, 0x008800FF, 360.f, 0.75f);
  lit_game_push_light(m, 420, 380, 0x000088FF, 360.f, 0.75f);

  lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_menu);
  lit_game_push_sensor(m, 400.f, 600.f, 0x000088FF); 
  lit_game_push_sensor(m, 250.f, 350.f, 0x880000FF); 
  lit_game_push_sensor(m, 550.f, 350.f, 0x008800FF); 

  lit_game_push_sensor(m, 700.f, 700.f, 0x888800FF); 
  lit_game_push_sensor(m, 100.f, 700.f, 0x008888FF); 
  lit_game_push_sensor(m, 400.f, 100.f, 0x880088FF); 
  lit_game_end_sensor_group(m);



  // Triangle #1
  {
    v2f_t a = v2f_set(0.f, 1.f);
    v2f_t b = v2f_rotate(a, f32_deg_to_rad(120.f));
    v2f_t c = v2f_rotate(a, f32_deg_to_rad(-120.f));

    v2f_t a_max = a * 350.f + v2f_set(400.f, 400.f);
    v2f_t b_max = b * 350.f + v2f_set(400.f, 400.f);
    v2f_t c_max = c * 350.f + v2f_set(400.f, 400.f);

    v2f_t a_min = a * 100.f + v2f_set(400.f, 400.f);
    v2f_t b_min = b * 100.f + v2f_set(400.f, 400.f);
    v2f_t c_min = c * 100.f + v2f_set(400.f, 400.f);

    lit_game_begin_patrolling_double_edge(m, a_max.x, a_max.y, b_max.x, b_max.y,  2.f);
    lit_game_push_patrolling_double_edge_waypoint_for_min(m, a_min.x, a_min.y);
    lit_game_push_patrolling_double_edge_waypoint_for_max(m, b_min.x, b_min.y);
    lit_game_end_patrolling_double_edge(m);

    lit_game_begin_patrolling_double_edge(m, b_max.x, b_max.y, c_max.x, c_max.y,  2.f);
    lit_game_push_patrolling_double_edge_waypoint_for_min(m, b_min.x, b_min.y);
    lit_game_push_patrolling_double_edge_waypoint_for_max(m, c_min.x, c_min.y);
    lit_game_end_patrolling_double_edge(m);

    lit_game_begin_patrolling_double_edge(m, c_max.x, c_max.y, a_max.x, a_max.y,  2.f);
    lit_game_push_patrolling_double_edge_waypoint_for_min(m, c_min.x, c_min.y);
    lit_game_push_patrolling_double_edge_waypoint_for_max(m, a_min.x, a_min.y);
    lit_game_end_patrolling_double_edge(m);
  }

  // Triangle #2
  {

    v2f_t a = v2f_set(0.f, -1.f);
    v2f_t b = v2f_rotate(a, f32_deg_to_rad(120.f));
    v2f_t c = v2f_rotate(a, f32_deg_to_rad(-120.f));

    v2f_t a_max = a * 175.f + v2f_set(400.f, 400.f);
    v2f_t b_max = b * 175.f + v2f_set(400.f, 400.f);
    v2f_t c_max = c * 175.f + v2f_set(400.f, 400.f);

    v2f_t a_min = a * 50.f + v2f_set(400.f, 400.f);
    v2f_t b_min = b * 50.f + v2f_set(400.f, 400.f);
    v2f_t c_min = c * 50.f + v2f_set(400.f, 400.f);

    lit_game_begin_patrolling_double_edge(m, a_max.x, a_max.y, b_max.x, b_max.y,  2.f);
    lit_game_push_patrolling_double_edge_waypoint_for_min(m, a_min.x, a_min.y);
    lit_game_push_patrolling_double_edge_waypoint_for_max(m, b_min.x, b_min.y);
    lit_game_end_patrolling_double_edge(m);

    lit_game_begin_patrolling_double_edge(m, b_max.x, b_max.y, c_max.x, c_max.y,  2.f);
    lit_game_push_patrolling_double_edge_waypoint_for_min(m, b_min.x, b_min.y);
    lit_game_push_patrolling_double_edge_waypoint_for_max(m, c_min.x, c_min.y);
    lit_game_end_patrolling_double_edge(m);

    lit_game_begin_patrolling_double_edge(m, c_max.x, c_max.y, a_max.x, a_max.y,  2.f);
    lit_game_push_patrolling_double_edge_waypoint_for_min(m, c_min.x, c_min.y);
    lit_game_push_patrolling_double_edge_waypoint_for_max(m, a_min.x, a_min.y);
    lit_game_end_patrolling_double_edge(m);
  }

}
//
// Menu Level
//

static void 
lit_level_menu(lit_game_t* m) {
  const f32_t box_hw = 50.f;
  const f32_t box_hh = 50.f;
  f32_t cx, cy;
  u32_t grey = 0x888888FF;

  lit_game_init_level(m, str8_from_lit("HOME"), 0);

  //
  // 1st row
  //
  cy = 600;
  cx = 100;
  lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
  lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_move);
  lit_game_push_sensor(m, cx, cy, grey); 
  lit_game_end_sensor_group(m);

  if (lit_get_levels_unlocked_count() >= 2) {
    cx = 250;
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_obstruct);
    lit_game_push_sensor(m, cx+25, cy, grey); 
    lit_game_push_sensor(m, cx-25, cy, grey); 
    lit_game_end_sensor_group(m);
  }


  if (lit_get_levels_unlocked_count() >= 3) {
    cx = 400;
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_add);
    lit_game_push_sensor(m, cx+25, cy, grey); 
    lit_game_push_sensor(m, cx,    cy, grey); 
    lit_game_push_sensor(m, cx-25, cy, grey); 
    lit_game_end_sensor_group(m);
  }

  if (lit_get_levels_unlocked_count() >= 4) {
    cx = 550;
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_corners);
    lit_game_push_sensor(m, cx-25, cy-25, grey); 
    lit_game_push_sensor(m, cx-25, cy+25, grey); 
    lit_game_push_sensor(m, cx+25, cy-25, grey); 
    lit_game_push_sensor(m, cx+25, cy+25, grey); 
    lit_game_end_sensor_group(m);
  }

  // 5
  if (lit_get_levels_unlocked_count() >= 5) {
    cx = 700;
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_mix);
    lit_game_push_sensor(m, cx-25, cy-25, grey); 
    lit_game_push_sensor(m, cx-25, cy+25, grey); 
    lit_game_push_sensor(m, cx,    cy,    grey); 
    lit_game_push_sensor(m, cx+25, cy-25, grey); 
    lit_game_push_sensor(m, cx+25, cy+25, grey); 
    lit_game_end_sensor_group(m);
  }

  //
  // 2nd row
  //
  u32_t red = 0x884444FF;
  cy = 450;
  if (lit_get_levels_unlocked_count() >= 6) {
    cx = 100;
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_blend);
    lit_game_push_sensor(m, cx, cy, red); 
    lit_game_end_sensor_group(m);
  }

  if (lit_get_levels_unlocked_count() >= 7) {
    cx = 250;
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_rooms);
    lit_game_push_sensor(m, cx+25, cy+25, red); 
    lit_game_push_sensor(m, cx-25, cy-25, red); 
    lit_game_end_sensor_group(m);
  }

  if (lit_get_levels_unlocked_count() >= 8 ) {
    cx = 400;
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_disco);
    lit_game_push_sensor(m, cx-25, cy+25, red); 
    lit_game_push_sensor(m, cx, cy, red); 
    lit_game_push_sensor(m, cx+25, cy-25, red); 
    lit_game_end_sensor_group(m);
  }


  if (lit_get_levels_unlocked_count() >= 9) {
    cx = 550;
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_onion);
    lit_game_push_sensor(m, cx, cy+35, red); 
    lit_game_push_sensor(m, cx-35, cy, red); 
    lit_game_push_sensor(m, cx, cy-35, red); 
    lit_game_push_sensor(m, cx+35, cy, red); 
    lit_game_end_sensor_group(m);
  }

  if (lit_get_levels_unlocked_count() >= 10) {
    cx = 700;
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_spectrum);
    lit_game_push_sensor(m, cx-25, cy+15, red); 
    lit_game_push_sensor(m, cx+25, cy+15, red); 
    lit_game_push_sensor(m, cx, cy+30, red); 
    lit_game_push_sensor(m, cx+20, cy-15, red); 
    lit_game_push_sensor(m, cx-20, cy-15, red); 
    lit_game_end_sensor_group(m);
  }

  //
  // 3rd row
  //
  u32_t yellow = 0x888844FF;
  cy = 300;
  if (lit_get_levels_unlocked_count() >= 11) {
    cx = 100;
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_split);
    lit_game_begin_patrolling_sensor(m, cx-25, cy, yellow, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx+25, cy);
    lit_game_end_patrolling_sensor(m);
    lit_game_end_sensor_group(m);
  }

  if (lit_get_levels_unlocked_count() >= 12) {
    cx = 250;
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_interval);

    lit_game_begin_patrolling_sensor(m, cx-25, cy+25, yellow, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx+25, cy-25);
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, cx+25, cy-25, yellow, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx-25, cy+25);
    lit_game_end_patrolling_sensor(m);

    lit_game_end_sensor_group(m);
  }

  if (lit_get_levels_unlocked_count() >= 13) {
    cx = 400;
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_movement);

    lit_game_begin_patrolling_sensor(m, cx, cy+25, yellow, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx+25, cy-25);
    lit_game_push_patrolling_sensor_waypoint(m, cx-25, cy-25);
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, cx+25, cy-25, yellow, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx-25, cy-25);
    lit_game_push_patrolling_sensor_waypoint(m, cx, cy+25);
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, cx-25, cy-25, yellow, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx, cy+25);
    lit_game_push_patrolling_sensor_waypoint(m, cx+25, cy-25);
    lit_game_end_patrolling_sensor(m);


    lit_game_end_sensor_group(m);
  }

  if (lit_get_levels_unlocked_count() >= 14) {
    cx = 550;
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_patience);

    lit_game_begin_patrolling_sensor(m, cx-30, cy, yellow, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx+30, cy);
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, cx+30, cy, yellow, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx-30, cy);
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, cx, cy-30, yellow, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx, cy+30);
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, cx, cy+30, yellow, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx, cy-30);
    lit_game_end_patrolling_sensor(m);


    lit_game_end_sensor_group(m);
  }

  if (lit_get_levels_unlocked_count() >= 14) {
    cx = 550;
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_busy);

    lit_game_begin_patrolling_sensor(m, cx-30, cy, yellow, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx+30, cy);
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, cx+30, cy, yellow, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx-30, cy);
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, cx, cy-30, yellow, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx, cy+30);
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, cx, cy+30, yellow, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx, cy-30);
    lit_game_end_patrolling_sensor(m);


    lit_game_end_sensor_group(m);
  }

  if (lit_get_levels_unlocked_count() >= 15) {
    cx = 700;
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_busy);

    lit_game_begin_patrolling_sensor(m, cx-30, cy+30, yellow, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx+30, cy-30);
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, cx+30, cy+30, yellow, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx-30, cy-30);
    lit_game_end_patrolling_sensor(m);

    lit_game_push_sensor(m, cx, cy, yellow);

    lit_game_begin_patrolling_sensor(m, cx-30, cy-30, yellow, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx+30, cy+30);
    lit_game_end_patrolling_sensor(m);


    lit_game_begin_patrolling_sensor(m, cx+30, cy-30, yellow, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx-30, cy+30);
    lit_game_end_patrolling_sensor(m);


    lit_game_end_sensor_group(m);
  }

  // row 4
  u32_t green = 0x448844FF;
  f32_t rotating_speed = 2.f;
  cy = 200;
  if (lit_get_levels_unlocked_count() >= 16) {
    cx = 100;
    v2f_t* o = lit_game_push_point(m, v2f_set(cx, cy));
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_spin);
    lit_game_push_rotating_sensor(m, cx,  cy-25, o, -rotating_speed, green); 
    lit_game_end_sensor_group(m);
  }


  if (lit_get_levels_unlocked_count() >= 17) {
    cx = 250;
    v2f_t* o = lit_game_push_point(m, v2f_set(cx, cy));
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_orbit);
    
    lit_game_push_rotating_sensor(m, cx,  cy+25, o, rotating_speed, green); 
    lit_game_push_rotating_sensor(m, cx,  cy-25, o, rotating_speed, green); 

    lit_game_end_sensor_group(m);
  }

  if (lit_get_levels_unlocked_count() >= 18) {
    cx = 400;
    v2f_t* o = lit_game_push_point(m, v2f_set(cx, cy));
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_test2);
 
    lit_game_push_rotating_sensor(m, cx,  cy+25, o,    -rotating_speed, green); 
    lit_game_push_rotating_sensor(m, cx+25,  cy-25, o, -rotating_speed, green); 
    lit_game_push_rotating_sensor(m, cx-25,  cy-25, o, -rotating_speed, green); 

    lit_game_end_sensor_group(m);
  }

  if (lit_get_levels_unlocked_count() >= 19) {
    cx = 550;
 
    v2f_t* o = lit_game_push_point(m, v2f_set(cx, cy));
    
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_test);
    lit_game_push_rotating_sensor(m, cx+25,  cy-25, o, rotating_speed, green); 
    lit_game_push_rotating_sensor(m, cx-25,  cy+25, o, rotating_speed, green); 
    lit_game_push_rotating_sensor(m, cx+25,  cy+25, o, rotating_speed, green); 
    lit_game_push_rotating_sensor(m, cx-25,  cy-25, o, rotating_speed, green); 


    lit_game_end_sensor_group(m);
  }

 

  if (lit_get_levels_unlocked_count() >= 20) {
    cx = 700;
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    v2f_t* o = lit_game_push_point(m, v2f_set(cx, cy));

    lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_move);

    lit_game_push_rotating_sensor(m, cx+25,  cy-25, o, -rotating_speed, green); 
    lit_game_push_rotating_sensor(m, cx-25,  cy+25, o, -rotating_speed, green); 
    lit_game_push_sensor(m, cx,  cy,  green); 
    lit_game_push_rotating_sensor(m, cx+25,  cy+25, o, -rotating_speed, green); 
    lit_game_push_rotating_sensor(m, cx-25,  cy-25, o, -rotating_speed, green); 

   

    lit_game_end_sensor_group(m);
  }


  // white lights
  if (lit_get_levels_unlocked_count() >= 7)  {
    lit_game_push_light(m, 25.f, 775.f, 0x444444FF, 360.f, 0.f);
    lit_game_push_light(m, 775.f, 25.f, 0x444444FF, 360.f, 0.f);
  }
  else {
    lit_game_push_light(m, 25.f, 775.f, 0x444444FF, 90.f, 7/8.f);
    lit_game_push_light(m, 775.f, 25.f, 0x444444FF, 90.f, 3/8.f);
  }


  // red light
  if (lit_get_levels_unlocked_count() >= 6) {
    // if player has learnt about point lights
    if (lit_get_levels_unlocked_count() >= 7) {
      lit_game_push_light(m, 775.f, 775.f, 0x440000FF, 360.f, 0.f);
    }
    else { // player has not learnt about point lights
      lit_game_push_light(m, 775.f, 775.f, 0x440000FF, 90.f, 5/8.f);
    }
  }
  if (lit_get_levels_unlocked_count() >= 10) {
    lit_game_push_light(m, 25.f, 25.f, 0x004400FF, 360.f, 0.f);
  }
}
