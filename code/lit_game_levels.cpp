
static void lit_level_menu(lit_game_t* m);

static void 
lit_game_sensor_trigger_solved(lit_game_t* game, void* context) {
  lit_game_load_level_t* level_to_load = (lit_game_load_level_t*)context;
  game->solved = true;
  game->level_to_load = level_to_load;
}


static void
lit_game_init_level(lit_game_t* m, str8_t str) {
  m->stage_flash_timer = 0.f;
  m->stage_fade_timer = LIT_ENTER_DURATION;
  m->state = LIT_GAME_STATE_TYPE_TRANSITION_IN;

  m->sensor_count = 0;
  m->light_count = 0;
  m->edge_count = 0;
  m->animator_count = 0;
  m->point_count = 0;
  m->sensor_group_count = 0;

  m->selected_sensor_group_id = array_count(m->sensor_groups);
  m->selected_sensor = nullptr;
  m->selected_animator = nullptr; 
  m->level_to_load = nullptr;
  m->solved = false;

  lit_game_push_edge(m, 0.f, 0.f, 800.f, 0.f);
  lit_game_push_edge(m, 800.f, 0.f, 800.f, 800.f);
  lit_game_push_edge(m, 800.f, 800.f, 0.f, 800.f);
  lit_game_push_edge(m, 0.f, 800.f, 0.f, 0.f);
  lit_init_player(m, 400.f, 400.f);

  lit_game_set_title(m, str);
}




//
// LEVEL 0
//
// Tutorial level
// - Learn to move
// - Learn to rotate
// - Learn that light need to shine on sensors 
//
static void
lit_level_move(lit_game_t* m) {
  lit_game_init_level(m, str8_from_lit("MOVE"));

  lit_game_push_light(m, 400.f, 400, 0x880000FF, 45.f, 0.75f);

  lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_menu);
  lit_game_push_sensor(m, 400.f, 600.f, 0x880000FF); 
  lit_game_end_sensor_group(m);
}

#if 0
//
// - Learn about obstacles
//
static void
lit_level_obstruct(lit_game_t* m) {
  lit_game_set_title(m, str8_from_lit("OBSTRUCT"));
  
  lit_game_begin_sensor_group(m, lit_game_load_next_level);
  lit_game_push_sensor(m, 400.f, 600.f, 0x008800FF); 
  lit_game_end_sensor_group(m);

  lit_game_push_light(m, 400.f, 200.f, 0x008800FF, 45.f, 0.75f);
  
  // Need to 'enclose' the shape
  lit_game_push_double_edge(m, 100.f, 400.f, 700.f, 400.f);
}

//
// - Learn about light saturation 
//
static void
lit_level_add(lit_game_t* m) { 
  // Need to 'enclose' the shape
  lit_game_set_title(m, str8_from_lit("ADD"));
  lit_game_push_double_edge(m, 100.f, 400.f, 700.f, 400.f);

  lit_game_begin_sensor_group(m, lit_game_load_next_level);
  lit_game_push_sensor(m, 400.f, 600.f, 0x444488FF); 
  lit_game_end_sensor_group(m);

  lit_game_push_light(m, 300.f, 200.f, 0x222244FF, 45.f, 0.75f);
  lit_game_push_light(m, 500.f, 200.f, 0x222244FF, 45.f, 0.75f);
 
}

//
// - Learn about light saturation 2
// - Sensor is in a box 
//
static void
lit_level_corners(lit_game_t* m) { 

  lit_game_set_title(m, str8_from_lit("CORNERS"));

  // Need to 'enclose' the shape
  lit_game_push_double_edge(m, 500.f, 200.f, 600.f, 300.f);
  lit_game_push_double_edge(m, 300.f, 200.f, 200.f, 300.f);
  lit_game_push_double_edge(m, 200.f, 500.f, 300.f, 600.f);
  lit_game_push_double_edge(m, 500.f, 600.f, 600.f, 500.f);

  lit_game_begin_sensor_group(m, lit_game_load_next_level);
  lit_game_push_sensor(m, 400.f, 400.f, 0xCCCC00FF); 
  lit_game_end_sensor_group(m);

  lit_game_push_light(m, 150.f, 150.f, 0x333300FF, 45.f, 0.125f);
  lit_game_push_light(m, 650.f, 150.f, 0x333300FF, 45.f, 0.376f);
  lit_game_push_light(m, 650.f, 650.f, 0x333300FF, 45.f, 0.626f);
  lit_game_push_light(m, 150.f, 650.f, 0x333300FF, 45.f, 0.876f);

  
}

//
// - Learn about color combinations
// - R + G = Y
//
static void
lit_level_mix(lit_game_t* m) {

  lit_game_set_title(m, str8_from_lit("MIX"));

  lit_game_begin_sensor_group(m, lit_game_load_next_level);
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
// - Learn about more color combinations
// - G + B = P
// - R + G + B = W
//
static void
lit_level_blend(lit_game_t* m) {
  lit_game_set_title(m, str8_from_lit("BLEND"));

  lit_game_begin_sensor_group(m, lit_game_load_next_level);
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
// - Sensors on inside room and outside room
// - Point lights
//
static void
lit_level_rooms(lit_game_t* m) {

  lit_game_set_title(m, str8_from_lit("ROOMS"));

  // middle
  lit_game_begin_sensor_group(m, lit_game_load_next_level);
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

  
static void
lit_level_disco(lit_game_t* m) {
  lit_game_set_title(m, str8_from_lit("DISCO"));

  lit_game_begin_sensor_group(m, lit_game_load_next_level);
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
// Onion 
//  
static void
lit_level_onion(lit_game_t* m) {
  lit_game_set_title(m, str8_from_lit("ONION"));

  lit_game_begin_sensor_group(m, lit_game_load_next_level);
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

static void
lit_level_spectrum(lit_game_t* m) {
  lit_game_set_title(m, str8_from_lit("SPECTRUM"));

  lit_game_begin_sensor_group(m, lit_game_load_next_level);
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

static void
lit_level_split(lit_game_t* m) {
  lit_game_set_title(m, str8_from_lit("SPLIT"));

  lit_game_push_light(m, 400.f, 410.f, 0x880000FF, 360.f, 0.f);
  lit_game_push_light(m, 390.f, 390.f, 0x008800FF, 360.f, 0.f);
  lit_game_push_light(m, 410.f, 390.f, 0x000088FF, 360.f, 0.f);

#if 0
  lit_game_push_light(m, 400.f, 410.f, 0x660000FF, 360.f, 0.f);
  lit_game_push_light(m, 390.f, 390.f, 0x006600FF, 360.f, 0.f);
  lit_game_push_light(m, 410.f, 390.f, 0x000066FF, 360.f, 0.f);
#endif

  lit_game_begin_sensor_group(m, lit_game_load_next_level);

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

static void
lit_level_interval(lit_game_t* m) {
  lit_game_set_title(m, str8_from_lit("INTERVAL"));

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

  lit_game_begin_sensor_group(m, lit_game_load_next_level);
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


static void
lit_level_movement(lit_game_t* m) {
  lit_game_set_title(m, str8_from_lit("MOVEMENT"));

//  lit_game_push_light(m, 400.f, 100.f, 0x880000FF, 15.f, 0.25f);
  lit_game_push_light(m, 400.f, 700.f, 0x008800FF, 15.f, 0.25f);
  lit_game_push_light(m, 150.f, 700.f, 0x880000FF, 15.f, 0.25f);
  lit_game_push_light(m, 600.f, 700.f, 0x000088FF, 15.f, 0.25f);

  f32_t duration = 5.25f;

  lit_game_begin_sensor_group(m, lit_game_load_next_level);
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
  lit_game_set_title(m, str8_from_lit("PATIENCE"));
  lit_game_push_light(m, 400.f, 100.f, 0x880000FF, 15.f, 0.25f);
  lit_game_push_light(m, 400.f, 700.f, 0x008800FF, 15.f, 0.75f);

  lit_game_begin_sensor_group(m, lit_game_load_next_level);
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
  lit_game_set_title(m, str8_from_lit("BUSY"));
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
 
  lit_game_begin_sensor_group(m, lit_game_load_next_level);
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

static void
lit_level_spin(lit_game_t* m) {
  lit_game_set_title(m, str8_from_lit("SPIN"));

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

  lit_game_begin_sensor_group(m, lit_game_load_next_level);
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
static void
lit_level_test(lit_game_t* m) {
  lit_game_set_title(m, str8_from_lit("SPINEROONI"));

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

  lit_game_begin_sensor_group(m, lit_game_load_next_level);
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
#endif 


#if 0
static lit_game_level_t g_lit_levels[] = {
  // Basics
  lit_level_move,   
  lit_level_obstruct, 
  lit_level_add,
  lit_level_corners,
  lit_level_mix,

  lit_level_blend,
  lit_level_rooms,
  lit_level_disco,
  lit_level_onion,
  lit_level_spectrum,

  lit_level_split,
  lit_level_movement,
  lit_level_patience,
  lit_level_busy,
  lit_level_interval,

  lit_level_spin,

  //lit_level_test,
  lit_level_move


};

#endif

#if 0

static void
lit_game_load_level(lit_game_t* m, u32_t level_id) {
  m->stage_flash_timer = 0.f;
  m->stage_fade_timer = LIT_ENTER_DURATION;
  m->state = LIT_GAME_STATE_TYPE_TRANSITION_IN;

  m->sensor_count = 0;
  m->light_count = 0;
  m->edge_count = 0;
  m->animator_count = 0;
  m->point_count = 0;
  m->sensor_group_count = 0;

  m->selected_sensor_group_id = array_count(m->sensor_groups);
  m->selected_sensor = nullptr;
  m->selected_animator = nullptr; 
  m->exit_callback = nullptr;
  m->solved = false;

  lit_game_push_edge(m, 0.f, 0.f, 800.f, 0.f);
  lit_game_push_edge(m, 800.f, 0.f, 800.f, 800.f);
  lit_game_push_edge(m, 800.f, 800.f, 0.f, 800.f);
  lit_game_push_edge(m, 0.f, 800.f, 0.f, 0.f);

  lit_init_player(m, 400.f, 400.f);

  g_lit_levels[level_id](m);

}



static void
lit_game_load_next_level(lit_game_t* m){
  m->current_level_id = (m->current_level_id + 1)%array_count(g_lit_levels);
  lit_game_load_level(m, m->current_level_id);  
}

#endif
//
// Menu Level
//
static void lit_level_menu(lit_game_t* m) {
  lit_game_init_level(m, str8_from_lit("LEVEL SELECT"));

  lit_game_begin_sensor_group(m, lit_game_sensor_trigger_solved, lit_level_move);
  lit_game_push_sensor(m, 400.f, 600.f, 0x880000FF); 
  lit_game_end_sensor_group(m);
 
  lit_game_push_light(m, 400.f, 400, 0x880000FF, 45.f, 0.75f);
}
