
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
  lit_set_title(m, str8_from_lit("MOVE"));
  lit_push_light(m, 400.f, 400, 0x880000FF, 45.f, 0.75f);

  lit_push_sensor(m, 400.f, 600.f, 0x880000FF); 

}

//
// - Learn about obstacles
//
static void
lit_level_obstruct(lit_game_t* m) {
  lit_set_title(m, str8_from_lit("OBSTRUCT"));
  lit_push_sensor(m, 400.f, 600.f, 0x008800FF); 
  lit_push_light(m, 400.f, 200, 0x008800FF, 45.f, 0.75f);
  
  // Need to 'enclose' the shape
  lit_push_double_edge(m, 100.f, 400.f, 700.f, 400.f);
}

//
// - Learn about light saturation 
//
static void
lit_level_add(lit_game_t* m) { 
  // Need to 'enclose' the shape
  lit_set_title(m, str8_from_lit("ADD"));
  lit_push_double_edge(m, 100.f, 400.f, 700.f, 400.f);

  lit_push_sensor(m, 400.f, 600.f, 0x444488FF); 
  lit_push_light(m, 300.f, 200.f, 0x222244FF, 45.f, 0.75f);
  lit_push_light(m, 500.f, 200.f, 0x222244FF, 45.f, 0.75f);
 
}

//
// - Learn about light saturation 2
// - Sensor is in a box 
//
static void
lit_level_corners(lit_game_t* m) { 

  lit_set_title(m, str8_from_lit("CORNERS"));

  // Need to 'enclose' the shape
  lit_push_double_edge(m, 500.f, 200.f, 600.f, 300.f);
  lit_push_double_edge(m, 300.f, 200.f, 200.f, 300.f);
  lit_push_double_edge(m, 200.f, 500.f, 300.f, 600.f);
  lit_push_double_edge(m, 500.f, 600.f, 600.f, 500.f);

  lit_push_sensor(m, 400.f, 400.f, 0xCCCC00FF); 
  lit_push_light(m, 150.f, 150.f, 0x333300FF, 45.f, 0.125f);
  lit_push_light(m, 650.f, 150.f, 0x333300FF, 45.f, 0.376f);
  lit_push_light(m, 650.f, 650.f, 0x333300FF, 45.f, 0.626f);
  lit_push_light(m, 150.f, 650.f, 0x333300FF, 45.f, 0.876f);

  
}

//
// - Learn about color combinations
// - R + G = Y
//
static void
lit_level_mix(lit_game_t* m) {

  lit_set_title(m, str8_from_lit("MIX"));

  lit_push_sensor(m, 400.f, 400.f, 0x888800FF); 
  
  lit_push_sensor(m, 200.f, 400.f, 0x008800FF); 
  lit_push_sensor(m, 600.f, 400.f, 0x008800FF); 

  lit_push_sensor(m, 400.f, 200.f, 0x880000FF); 
  lit_push_sensor(m, 400.f, 600.f, 0x880000FF); 

  lit_push_light(m, 200.f, LIT_HEIGHT * 0.25f, 0x008800FF, 30.f, 0.5f);
  lit_push_light(m, 600.f, LIT_HEIGHT * 0.75f, 0x880000FF, 30.f, 0.0f);
}



//
// - Learn about more color combinations
// - G + B = P
// - R + G + B = W
//
static void
lit_level_blend(lit_game_t* m) {
  lit_set_title(m, str8_from_lit("BLEND"));

  lit_push_sensor(m, 400.f,  400.f, 0x888888FF); 

  lit_push_sensor(m, 200.f, 400.f, 0x008888FF); 
  lit_push_sensor(m, 600.f, 400.f, 0x008800FF); 

  lit_push_sensor(m, 400.f, 200.f, 0x880000FF); 
  lit_push_sensor(m, 400.f, 600.f, 0x880088FF); 

  lit_push_light(m, 200.f, 150.f, 0x008800FF, 30.f, 0.75f);
  lit_push_light(m, 400.f, 150.f, 0x880000FF, 30.f, 0.75f);
  lit_push_light(m, 600.f, 150.f, 0x000088FF, 30.f, 0.75f);
  

}

//
// - Sensors on inside room and outside room
// - Point lights
//
static void
lit_level_rooms(lit_game_t* m) {

  lit_set_title(m, str8_from_lit("ROOMS"));

  // middle
  lit_push_sensor(m, 050.f,  450.f, 0x666666FF); 
  lit_push_sensor(m, 750.f,  450.f, 0x666666FF); 
  lit_push_sensor(m, 050.f,  350.f, 0x666666FF); 
  lit_push_sensor(m, 750.f,  350.f, 0x666666FF); 

  lit_push_double_edge(m, 000.f, 400.f, 300.f, 400.f);
  lit_push_double_edge(m, 300.f, 250.f, 300.f, 550.f);
  lit_push_double_edge(m, 300.f, 650.f, 300.f, 800.f);
  lit_push_double_edge(m, 300.f, 000.f, 300.f, 150.f);

  lit_push_double_edge(m, 500.f, 400.f, 800.f, 400.f); 
  lit_push_double_edge(m, 500.f, 250.f, 500.f, 550.f);
  lit_push_double_edge(m, 500.f, 650.f, 500.f, 800.f);
  lit_push_double_edge(m, 500.f, 000.f, 500.f, 150.f);

  //lit_push_sensor(m, 200.f, 400.f, 0x008888FF); 
  //lit_push_sensor(m, 600.f, 400.f, 0x008800FF); 

 //lit_push_sensor(m, 400.f, 200.f, 0x880000FF); 
  //lit_push_sensor(m, 400.f, 600.f, 0x880088FF); 

  lit_push_light(m, 400.f, 400.f, 0x666666FF, 360.f, 0.f);
  lit_push_light(m, 400.f, 400.f, 0x666666FF, 360.f, 0.f);
  

}

  
static void
lit_level_disco(lit_game_t* m) {
  lit_set_title(m, str8_from_lit("DISCO"));

  // bottom left room
  lit_push_sensor(m, 050.f,  350.f, 0x660000FF); 
  lit_push_sensor(m, 050.f,  050.f, 0x660000FF); 
  lit_push_sensor(m, 250.f,  350.f, 0x660000FF); 
  lit_push_sensor(m, 250.f,  050.f, 0x660000FF); 

  // bottom right room
  lit_push_sensor(m, 550.f,  350.f, 0x006600FF); 
  lit_push_sensor(m, 550.f,  050.f, 0x006600FF); 
  lit_push_sensor(m, 750.f,  350.f, 0x006600FF); 
  lit_push_sensor(m, 750.f,  050.f, 0x006600FF); 

  // top right room
  lit_push_sensor(m, 550.f,  450.f, 0x000066FF); 
  lit_push_sensor(m, 550.f,  750.f, 0x000066FF); 
  lit_push_sensor(m, 750.f,  450.f, 0x000066FF); 
  lit_push_sensor(m, 750.f,  750.f, 0x000066FF); 


  // top left room
  lit_push_sensor(m, 050.f,  450.f, 0x666666FF); 
  lit_push_sensor(m, 050.f,  750.f, 0x666666FF); 
  lit_push_sensor(m, 250.f,  450.f, 0x666666FF); 
  lit_push_sensor(m, 250.f,  750.f, 0x666666FF); 



  lit_push_double_edge(m, 000.f, 400.f, 300.f, 400.f);
  lit_push_double_edge(m, 300.f, 250.f, 300.f, 550.f);
  lit_push_double_edge(m, 300.f, 650.f, 300.f, 800.f);
  lit_push_double_edge(m, 300.f, 000.f, 300.f, 150.f);

  lit_push_double_edge(m, 500.f, 400.f, 800.f, 400.f); 
  lit_push_double_edge(m, 500.f, 250.f, 500.f, 550.f);
  lit_push_double_edge(m, 500.f, 650.f, 500.f, 800.f);
  lit_push_double_edge(m, 500.f, 000.f, 500.f, 150.f);
  //lit_push_sensor(m, 200.f, 400.f, 0x008888FF); 
  //lit_push_sensor(m, 600.f, 400.f, 0x008800FF); 

 //lit_push_sensor(m, 400.f, 200.f, 0x880000FF); 
  //lit_push_sensor(m, 400.f, 600.f, 0x880088FF); 

  lit_push_light(m, 400.f, 400.f, 0x660000FF, 360.f, 0.f);
  lit_push_light(m, 400.f, 400.f, 0x006600FF, 360.f, 0.f);
  lit_push_light(m, 400.f, 400.f, 0x000066FF, 360.f, 0.f);
  lit_push_light(m, 400.f, 400.f, 0x666666FF, 360.f, 0.f);
}

//
// Onion 
//  
static void
lit_level_onion(lit_game_t* m) {
  lit_set_title(m, str8_from_lit("ONION"));
  // layer 1
  lit_push_sensor(m, 550.f,  250.f, 0x006600FF); 
  lit_push_sensor(m, 250.f,  550.f, 0x660000FF); 
  lit_push_sensor(m, 250.f,  250.f, 0x666600FF); 
  lit_push_sensor(m, 550.f,  550.f, 0x666600FF); 

  // layer 2
  lit_push_sensor(m, 150.f,  150.f, 0x660066FF); 
  lit_push_sensor(m, 650.f,  150.f, 0x660066FF); 
  lit_push_sensor(m, 150.f,  650.f, 0x660000FF); 
  lit_push_sensor(m, 650.f,  650.f, 0x000066FF); 

  // layer 3
  lit_push_sensor(m, 050.f,  050.f, 0x666666FF); 
  lit_push_sensor(m, 750.f,  750.f, 0x666666FF); 

  lit_push_box(m, 100.f, 100.f, 700.f, 700.f);
  lit_push_box(m, 200.f, 200.f, 600.f, 600.f);
  lit_push_box(m, 300.f, 300.f, 500.f, 500.f);



  lit_push_light(m, 400.f, 400.f, 0x660000FF, 360.f, 0.f);
  lit_push_light(m, 400.f, 400.f, 0x660000FF, 360.f, 0.f);
  lit_push_light(m, 400.f, 400.f, 0x006600FF, 360.f, 0.f);
  lit_push_light(m, 400.f, 400.f, 0x000066FF, 360.f, 0.f);
  lit_push_light(m, 400.f, 400.f, 0x333333FF, 360.f, 0.f);
  lit_push_light(m, 400.f, 400.f, 0x333333FF, 360.f, 0.f);
}

static void
lit_level_spectrum(lit_game_t* m) {
  lit_set_title(m, str8_from_lit("SPECTRUM"));

  // cyans
  lit_push_sensor(m, 050.f,   750.f, 0x008888FF); 
  lit_push_sensor(m, 050.f,   50.f,  0x008888FF); 

  // magentas
  lit_push_sensor(m, 750.f,   750.f, 0x880088FF); 
  lit_push_sensor(m, 750.f,   50.f,  0x880088FF); 
  

  // whites
  lit_push_sensor(m, 400.f,   750.f, 0x888888FF); 
  lit_push_sensor(m, 400.f,   50.f,  0x888888FF); 

  // yellows
  lit_push_sensor(m, 250.f,   750.f, 0x888800FF); 
  lit_push_sensor(m, 550.f,   750.f, 0x888800FF); 
  lit_push_sensor(m, 250.f,   050.f, 0x888800FF); 
  lit_push_sensor(m, 550.f,   50.f, 0x888800FF); 

  //lit_push_sensor(m, 0.f,   0.f, 0xFFFFFFFF); 

  //lit_push_double_edge(m, 300.f, 300.f, 500.f, 300.f);
  lit_push_double_edge(m, 340.f, 300.f, 380.f, 300.f);
  lit_push_double_edge(m, 420.f, 300.f, 460.f, 300.f);

  lit_push_double_edge(m, 340.f, 500.f, 380.f, 500.f);
  lit_push_double_edge(m, 420.f, 500.f, 460.f, 500.f);

  lit_push_double_edge(m, 500.f, 340.f, 500.f, 380.f);
  lit_push_double_edge(m, 500.f, 420.f, 500.f, 460.f);

  lit_push_double_edge(m, 300.f, 340.f, 300.f, 380.f);
  lit_push_double_edge(m, 300.f, 420.f, 300.f, 460.f);

  lit_push_light(m, 400.f, 410.f, 0x880000FF, 360.f, 0.f);
  lit_push_light(m, 390.f, 390.f, 0x008800FF, 360.f, 0.f);
  lit_push_light(m, 410.f, 390.f, 0x000088FF, 360.f, 0.f);
}

static void
lit_level_split(lit_game_t* m) {
  lit_set_title(m, str8_from_lit("SPLIT"));

  lit_push_light(m, 400.f, 410.f, 0x880000FF, 360.f, 0.f);
  lit_push_light(m, 390.f, 390.f, 0x008800FF, 360.f, 0.f);
  lit_push_light(m, 410.f, 390.f, 0x000088FF, 360.f, 0.f);

#if 0
  lit_push_light(m, 400.f, 410.f, 0x660000FF, 360.f, 0.f);
  lit_push_light(m, 390.f, 390.f, 0x006600FF, 360.f, 0.f);
  lit_push_light(m, 410.f, 390.f, 0x000066FF, 360.f, 0.f);
#endif


  lit_push_sensor(m, 400.f,   750.f, 0x888888FF); 
  lit_push_sensor(m, 50.f,   50.f, 0x888888FF); 
  lit_push_sensor(m, 750.f,   50.f, 0x888888FF); 

  lit_push_sensor(m, 50.f,   750.f, 0x888800FF); 
  lit_push_sensor(m, 750.f,   750.f, 0x880088FF); 
  lit_push_sensor(m, 400.f,   50.f, 0x008888FF); 

  lit_push_sensor(m, 400.f,   550.f, 0x880000FF); 
  lit_push_sensor(m, 275.f,   380.f, 0x008800FF); 
  lit_push_sensor(m, 525.f,   380.f, 0x000088FF); 


  //lit_push_sensor(m, 0.f,   0.f, 0xFFFFFFFF); 
  lit_push_box(m, 300, 300, 500, 500);
  //lit_push_box(m, 150, 350, 250, 450);

}

static void
lit_level_interval(lit_game_t* m) {
  lit_set_title(m, str8_from_lit("INTERVAL"));

  const u32_t sections = 20;
  f32_t offset = LIT_WIDTH/20; 
  {
    f32_t cur_offset = 0.f;
    for (u32_t i = 0; i < sections; ++i) {
      lit_push_double_edge(m, cur_offset, 0.f, cur_offset, LIT_HEIGHT);
      cur_offset += offset; 
    }
  }



  u32_t color = 0x444488FF;
  {
    f32_t cur_offset = offset/2+ offset;
    lit_push_light(m, cur_offset, 50.f, color, 360.f, 0.25f);
    lit_push_light(m, cur_offset += offset*2, 50.f, color, 360.f, 0.25f);
    lit_push_light(m, cur_offset += offset*2, 50.f, color, 360.f, 0.25f);
    lit_push_light(m, cur_offset += offset*2, 50.f, color, 360.f, 0.25f);
    lit_push_light(m, cur_offset += offset*2, 50.f, color, 360.f, 0.25f);
    lit_push_light(m, cur_offset += offset*2, 50.f, color, 360.f, 0.25f);
    lit_push_light(m, cur_offset += offset*2, 50.f, color, 360.f, 0.25f);
    lit_push_light(m, cur_offset += offset*2, 50.f, color, 360.f, 0.25f);
    lit_push_light(m, cur_offset += offset*2, 50.f, color, 360.f, 0.25f);
    lit_push_light(m, cur_offset += offset*2, 50.f, color, 360.f, 0.25f);
 }

  f32_t duration = 10.f;
  f32_t start_x = -LIT_WIDTH;
  f32_t end_x = LIT_WIDTH;

  lit_begin_patrolling_sensor(m, start_x + offset, 400.f, color, duration);
  lit_push_patrolling_sensor_waypoint(m, end_x + offset, 400.f); 
  lit_end_patrolling_sensor(m);

  lit_begin_patrolling_sensor(m, start_x + offset*2, 400.f, color, duration);
  lit_push_patrolling_sensor_waypoint(m, end_x + offset*2, 400.f); 
  lit_end_patrolling_sensor(m);

  lit_begin_patrolling_sensor(m, start_x + offset*4, 400.f, color, duration);
  lit_push_patrolling_sensor_waypoint(m, end_x + offset*4, 400.f); 
  lit_end_patrolling_sensor(m);


  lit_begin_patrolling_sensor(m, start_x + offset*5, 400.f, color, duration);
  lit_push_patrolling_sensor_waypoint(m, end_x + offset*5, 400.f); 
  lit_end_patrolling_sensor(m);

  lit_begin_patrolling_sensor(m, start_x + offset*8, 400.f, color, duration);
  lit_push_patrolling_sensor_waypoint(m, end_x + offset*8, 400.f); 
  lit_end_patrolling_sensor(m);

  lit_begin_patrolling_sensor(m, start_x + offset*9, 400.f, color, duration);
  lit_push_patrolling_sensor_waypoint(m, end_x + offset*9, 400.f); 
  lit_end_patrolling_sensor(m);

  lit_begin_patrolling_sensor(m, start_x + offset*11, 400.f, color, duration);
  lit_push_patrolling_sensor_waypoint(m, end_x + offset*11, 400.f); 
  lit_end_patrolling_sensor(m);

  lit_begin_patrolling_sensor(m, start_x + offset*13, 400.f, color, duration);
  lit_push_patrolling_sensor_waypoint(m, end_x + offset*13, 400.f); 
  lit_end_patrolling_sensor(m);

  lit_begin_patrolling_sensor(m, start_x + offset*15, 400.f, color, duration);
  lit_push_patrolling_sensor_waypoint(m, end_x + offset*15, 400.f); 
  lit_end_patrolling_sensor(m);

  lit_begin_patrolling_sensor(m, start_x + offset*18, 400.f, color, duration);
  lit_push_patrolling_sensor_waypoint(m, end_x + offset*18, 400.f); 
  lit_end_patrolling_sensor(m);

  //lit_push_sensor(m, 0.f,   0.f, 0xFFFFFFFF); 
  //lit_push_box(m, 300, 300, 500, 500);
  //lit_push_box(m, 150, 350, 250, 450);

}


static void
lit_level_movement(lit_game_t* m) {
  lit_set_title(m, str8_from_lit("MOVEMENT"));

//  lit_push_light(m, 400.f, 100.f, 0x880000FF, 15.f, 0.25f);
  lit_push_light(m, 400.f, 700.f, 0x008800FF, 15.f, 0.25f);
  lit_push_light(m, 150.f, 700.f, 0x880000FF, 15.f, 0.25f);
  lit_push_light(m, 600.f, 700.f, 0x000088FF, 15.f, 0.25f);

  f32_t duration = 5.25f;
  lit_begin_patrolling_sensor(m, 150.f, 150.f, 0x880088FF, duration);
  lit_push_patrolling_sensor_waypoint(m, 650.f, 650.f);
  lit_end_patrolling_sensor(m);

  lit_begin_patrolling_sensor(m, 650.f, 650.f, 0x880088FF, duration);
  lit_push_patrolling_sensor_waypoint(m, 150.f, 150.f);
  lit_end_patrolling_sensor(m);

  lit_begin_patrolling_sensor(m, 150.f, 400.f, 0x880088FF, duration);
  lit_push_patrolling_sensor_waypoint(m, 650.f, 400.f);
  lit_end_patrolling_sensor(m);

  lit_begin_patrolling_sensor(m, 650.f, 400.f, 0x880088FF, duration);
  lit_push_patrolling_sensor_waypoint(m, 150.f, 400.f);
  lit_end_patrolling_sensor(m);

  lit_begin_patrolling_sensor(m, 150.f, 650.f, 0x880088FF, duration);
  lit_push_patrolling_sensor_waypoint(m, 650.f, 150.f);
  lit_end_patrolling_sensor(m);

  lit_begin_patrolling_sensor(m, 650.f, 150.f, 0x880088FF, duration);
  lit_push_patrolling_sensor_waypoint(m, 150.f, 650.f);
  lit_end_patrolling_sensor(m);

  lit_begin_patrolling_sensor(m, 400.f, 650.f, 0x880088FF, duration);
  lit_push_patrolling_sensor_waypoint(m, 400.f, 150.f);
  lit_end_patrolling_sensor(m);

  lit_begin_patrolling_sensor(m, 400.f, 150.f, 0x880088FF, duration);
  lit_push_patrolling_sensor_waypoint(m, 400.f, 650.f);
  lit_end_patrolling_sensor(m);

}

static void
lit_level_patience(lit_game_t* m) {
  lit_set_title(m, str8_from_lit("PATIENCE"));
  lit_push_light(m, 400.f, 100.f, 0x880000FF, 15.f, 0.25f);
  lit_push_light(m, 400.f, 700.f, 0x008800FF, 15.f, 0.75f);
  lit_push_sensor(m, 400.f, 400.f, 0x888888FF); 

  lit_begin_patrolling_sensor(m, 650.f, 150.f, 0x880000FF, 5.f);
  lit_push_patrolling_sensor_waypoint(m, 650.f, 650.f);
  lit_end_patrolling_sensor(m);

  lit_begin_patrolling_sensor(m, 150.f, 650.f, 0x008800FF, 5.25f);
  lit_push_patrolling_sensor_waypoint(m, 150.f, 150.f);
  lit_end_patrolling_sensor(m);
}

static void
lit_level_busy(lit_game_t* m) {
  lit_set_title(m, str8_from_lit("BUSY"));
  //lit_push_light(m, 400.f, 100.f, 0x880000FF, 15.f, 0.25f);
#if LIT_INIT_WITH_SOLUTION
  lit_push_light(m, 600.f, 700.f, 0x660000FF, 15.f, 0.75f);
  lit_push_light(m, 600.f, 700.f, 0x660000FF, 15.f, 0.75f);
  lit_push_light(m, 200.f, 700.f, 0x006600FF, 15.f, 0.75f);
  lit_push_light(m, 200.f, 700.f, 0x006600FF, 15.f, 0.75f);
#else
  lit_push_light(m, 400.f, 750.f, 0x006600FF, 15.f, 0.75f);
  lit_push_light(m, 050.f, 400.f, 0x006600FF, 15.f, 0.f);
  lit_push_light(m, 050.f, 750.f, 0x660000FF, 15.f, 0.87f);
  lit_push_light(m, 750.f, 750.f, 0x660000FF, 15.f, 0.62f);
#endif
  //lit_push_light(m, 200.f, 700.f, 0x006600FF, 15.f, 0.75f);

  //lit_push_light(m, 200.f, 700.f, 0x002200FF, 15.f, 0.75f);
  //lit_push_light(m, 200.f, 700.f, 0x002200FF, 15.f, 0.75f);
 
  // patrols in a square
  lit_begin_patrolling_sensor(m, 150.f, 650.f, 0x006600FF, 5.f);
  lit_push_patrolling_sensor_waypoint(m, 150.f, 150.f);
  lit_push_patrolling_sensor_waypoint(m, 650.f, 150.f);
  lit_push_patrolling_sensor_waypoint(m, 650.f, 650.f);
  lit_end_patrolling_sensor(m);
 
  // patrols in a small square
  lit_begin_patrolling_sensor(m, 550.f, 250.f, 0x006600FF, 5.f);
  lit_push_patrolling_sensor_waypoint(m, 250.f, 250.f);
  lit_push_patrolling_sensor_waypoint(m, 250.f, 550.f);
  lit_push_patrolling_sensor_waypoint(m, 550.f, 550.f);
  lit_end_patrolling_sensor(m);

  // patrols in an hourglass (1)
  lit_begin_patrolling_sensor(m, 650.f, 650.f, 0x660000FF, 2.5f);
  lit_push_patrolling_sensor_waypoint(m, 650.f, 150.f);
  lit_push_patrolling_sensor_waypoint(m, 150.f, 650.f);
  lit_push_patrolling_sensor_waypoint(m, 150.f, 150.f);
  lit_end_patrolling_sensor(m);
#if 1
  // patrols in an hourglass (2)
  lit_begin_patrolling_sensor(m, 150.f, 150.f, 0x660000FF, 2.5f);
  lit_push_patrolling_sensor_waypoint(m, 150.f, 650.f);
  lit_push_patrolling_sensor_waypoint(m, 650.f, 150.f);
  lit_push_patrolling_sensor_waypoint(m, 650.f, 650.f);
  lit_end_patrolling_sensor(m);
#endif



  // Runs across the scene vertially
  lit_begin_patrolling_sensor(m, 400.f, -100.f, 0xCCCC00FF, 5.f);
  lit_push_patrolling_sensor_waypoint(m, 400.f, 900.f);
  lit_end_patrolling_sensor(m);
 

  // Runs across the scene horizontally
  lit_begin_patrolling_sensor(m, -100.f, 400.f, 0xCCCC00FF, 5.f);
  lit_push_patrolling_sensor_waypoint(m, 900.f, 400.f);
  lit_end_patrolling_sensor(m);


#if 0
  lit_push_patrolling_sensor(m, 5.f, 
                             v2f_set(150.f, 150.f), 
                             v2f_set(650.f, 150.f),
                             0x008800FF); 
  lit_push_patrolling_sensor(m, 5.25f, 
                             v2f_set(150.f, 250.f), 
                             v2f_set(650.f, 250.f),
                             0x880000FF); 
  lit_push_patrolling_sensor(m, 5.25f, 
                             v2f_set(150.f, 350.f), 
                             v2f_set(650.f, 350.f),
                             0x880000FF); 

#endif

}

static void
lit_level_test(lit_game_t* m) {
  lit_set_title(m, str8_from_lit("TEST"));

  const u32_t sections = 20;
  f32_t offset = LIT_WIDTH/20; 
  {
    f32_t cur_offset = 0.f;
    for (u32_t i = 0; i < sections; ++i) {
      lit_push_double_edge(m, cur_offset, 0.f, cur_offset, LIT_HEIGHT);
      cur_offset += offset; 
    }
  }



  u32_t color = 0x444488FF;
  {
    f32_t cur_offset = offset/2+ offset;
    lit_push_light(m, cur_offset, 50.f, color, 360.f, 0.25f);
    lit_push_light(m, cur_offset += offset*2, 50.f, color, 360.f, 0.25f);
    lit_push_light(m, cur_offset += offset*2, 50.f, color, 360.f, 0.25f);
    lit_push_light(m, cur_offset += offset*2, 50.f, color, 360.f, 0.25f);
    lit_push_light(m, cur_offset += offset*2, 50.f, color, 360.f, 0.25f);
    lit_push_light(m, cur_offset += offset*2, 50.f, color, 360.f, 0.25f);
    lit_push_light(m, cur_offset += offset*2, 50.f, color, 360.f, 0.25f);
    lit_push_light(m, cur_offset += offset*2, 50.f, color, 360.f, 0.25f);
    lit_push_light(m, cur_offset += offset*2, 50.f, color, 360.f, 0.25f);
    lit_push_light(m, cur_offset += offset*2, 50.f, color, 360.f, 0.25f);
 }

  f32_t duration = 10.f;
  f32_t start_x = -LIT_WIDTH;
  f32_t end_x = LIT_WIDTH;

  lit_begin_patrolling_sensor(m, start_x + offset, 400.f, color, duration);
  lit_push_patrolling_sensor_waypoint(m, end_x + offset, 400.f); 
  lit_end_patrolling_sensor(m);

  lit_begin_patrolling_sensor(m, start_x + offset*2, 400.f, color, duration);
  lit_push_patrolling_sensor_waypoint(m, end_x + offset*2, 400.f); 
  lit_end_patrolling_sensor(m);

  lit_begin_patrolling_sensor(m, start_x + offset*4, 400.f, color, duration);
  lit_push_patrolling_sensor_waypoint(m, end_x + offset*4, 400.f); 
  lit_end_patrolling_sensor(m);


  lit_begin_patrolling_sensor(m, start_x + offset*5, 400.f, color, duration);
  lit_push_patrolling_sensor_waypoint(m, end_x + offset*5, 400.f); 
  lit_end_patrolling_sensor(m);

  lit_begin_patrolling_sensor(m, start_x + offset*8, 400.f, color, duration);
  lit_push_patrolling_sensor_waypoint(m, end_x + offset*8, 400.f); 
  lit_end_patrolling_sensor(m);

  lit_begin_patrolling_sensor(m, start_x + offset*9, 400.f, color, duration);
  lit_push_patrolling_sensor_waypoint(m, end_x + offset*9, 400.f); 
  lit_end_patrolling_sensor(m);

  lit_begin_patrolling_sensor(m, start_x + offset*11, 400.f, color, duration);
  lit_push_patrolling_sensor_waypoint(m, end_x + offset*11, 400.f); 
  lit_end_patrolling_sensor(m);

  lit_begin_patrolling_sensor(m, start_x + offset*13, 400.f, color, duration);
  lit_push_patrolling_sensor_waypoint(m, end_x + offset*13, 400.f); 
  lit_end_patrolling_sensor(m);

  lit_begin_patrolling_sensor(m, start_x + offset*15, 400.f, color, duration);
  lit_push_patrolling_sensor_waypoint(m, end_x + offset*15, 400.f); 
  lit_end_patrolling_sensor(m);

  lit_begin_patrolling_sensor(m, start_x + offset*18, 400.f, color, duration);
  lit_push_patrolling_sensor_waypoint(m, end_x + offset*18, 400.f); 
  lit_end_patrolling_sensor(m);

  //lit_push_sensor(m, 0.f,   0.f, 0xFFFFFFFF); 
  //lit_push_box(m, 300, 300, 500, 500);
  //lit_push_box(m, 150, 350, 250, 450);

}

typedef void (*Lit_Level)(lit_game_t* mode); 
static Lit_Level g_lit_levels[] = {
#if 0 
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

  // Here on it's all movement
  lit_level_movement,
  lit_level_patience,
  lit_level_busy,
  lit_level_interval,
#endif
  lit_level_test,

};



static void
lit_load_level(lit_game_t* m, u32_t level_id) {
  m->stage_flash_timer = 0.f;
  m->stage_fade_timer = LIT_ENTER_DURATION;
  m->state = LIT_STATE_TYPE_TRANSITION_IN;

  m->sensor_count = 0;
  m->light_count = 0;
  m->edge_count = 0;
  m->animator_count = 0;

  lit_push_edge(m, 0.f, 0.f, 800.f, 0.f);
  lit_push_edge(m, 800.f, 0.f, 800.f, 800.f);
  lit_push_edge(m, 800.f, 800.f, 0.f, 800.f);
  lit_push_edge(m, 0.f, 800.f, 0.f, 0.f);

  lit_init_player(m, 400.f, 400.f);

  g_lit_levels[level_id](m);

}

static void
lit_load_next_level(lit_game_t* m){
  m->current_level_id = (m->current_level_id + 1)%array_count(g_lit_levels);
  lit_load_level(m, m->current_level_id);  
}

