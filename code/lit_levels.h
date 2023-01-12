

/////////////////////////////////////////////////////////
// LEVEL 0
////////////////////////////////////////////
// Tutorial level
// - Learn to move
// - Learn to rotate
// - Learn that light need to shine on sensors 
static void
lit_level_0_0(lit_game_t* m) {
  lit_set_title(m, str8_from_lit("MOVE"));
  lit_push_sensor(m, 400.f, 600.f, 0x880000FF); 
  lit_push_light(m, 400.f, 400, 0x880000FF, 45.f, 0.75f);
}

////////////////////////////////////////////
// - Learn about obstacles
static void
lit_level_0_1(lit_game_t* m) {
  lit_set_title(m, str8_from_lit("OBSTRUCT"));
  lit_push_sensor(m, 400.f, 600.f, 0x008800FF); 
  lit_push_light(m, 400.f, 200, 0x008800FF, 45.f, 0.75f);
  
  // Need to 'enclose' the shape
  lit_push_double_edge(m, 100.f, 400.f, 700.f, 400.f);
}

//////////////////////////////////////////
// - Learn about light saturation 
static void
lit_level_0_2(lit_game_t* m) { 
  // Need to 'enclose' the shape
  lit_set_title(m, str8_from_lit("ADD"));
  lit_push_double_edge(m, 100.f, 400.f, 700.f, 400.f);

  lit_push_sensor(m, 400.f, 600.f, 0x444488FF); 
  lit_push_light(m, 300.f, 200.f, 0x222244FF, 45.f, 0.75f);
  lit_push_light(m, 500.f, 200.f, 0x222244FF, 45.f, 0.75f);
 
}

//////////////////////////////////////
// - Learn about light saturation 2
// - Sensor is in a box 
static void
lit_level_0_3(lit_game_t* m) { 

  lit_set_title(m, str8_from_lit("BOX"));

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


//////////////////////////////////////
// - Learn about color combinations
// - R + G = Y
static void
lit_level_0_4(lit_game_t* m) {

  lit_set_title(m, str8_from_lit("MIX"));

  lit_push_sensor(m, 400.f, 400.f, 0x888800FF); 
  
  lit_push_sensor(m, 200.f, 400.f, 0x008800FF); 
  lit_push_sensor(m, 600.f, 400.f, 0x008800FF); 

  lit_push_sensor(m, 400.f, 200.f, 0x880000FF); 
  lit_push_sensor(m, 400.f, 600.f, 0x880000FF); 

  lit_push_light(m, 200.f, MOE_HEIGHT * 0.25f, 0x008800FF, 30.f, 0.5f);
  lit_push_light(m, 600.f, MOE_HEIGHT * 0.75f, 0x880000FF, 30.f, 0.0f);
}



///////////////////////////////////////
// - Learn about more color combinations
// - G + B = P
// - R + G + B = W
static void
lit_level_0_5(lit_game_t* m) {
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

///////////////////////////////////////
// - Sensors on inside room and outside room
// - Point lights
static void
lit_level_0_6(lit_game_t* m) {

  lit_set_title(m, str8_from_lit("4 ROOM"));

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

///////////////////////////////////////
// More point lights but with more colors
//  
static void
lit_level_0_7(lit_game_t* m) {
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

///////////////////////////////////////
// Onion 
//  
static void
lit_level_0_8(lit_game_t* m) {
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

///////////////////////////////////////
// Onion 
//  
static void
lit_level_0_9(lit_game_t* m) {
  lit_set_title(m, str8_from_lit("FACE"));

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

 
  lit_push_box(m, 100.f, 700.f, 200.f, 600.f);
  lit_push_box(m, 600.f, 700.f, 700.f, 600.f);
  lit_push_box(m, 100.f, 300.f, 700.f, 200.f);
  
  lit_push_light(m, 400.f, 400.f, 0x660000FF, 360.f, 0.f);
  lit_push_light(m, 400.f, 400.f, 0x660000FF, 360.f, 0.f);
  lit_push_light(m, 400.f, 400.f, 0x006600FF, 360.f, 0.f);
  lit_push_light(m, 400.f, 400.f, 0x000066FF, 360.f, 0.f);
  lit_push_light(m, 400.f, 400.f, 0x333333FF, 360.f, 0.f);
  lit_push_light(m, 400.f, 400.f, 0x333333FF, 360.f, 0.f);
}

typedef void (*Lit_Level)(lit_game_t* mode); 
static Lit_Level lit_levels[] = {
#if 0
  lit_level_0_0, 
  lit_level_0_1, 
  lit_level_0_2,
  lit_level_0_3,
  lit_level_0_4,
  lit_level_0_5,
  lit_level_0_6,
  lit_level_0_7,

#endif
  lit_level_0_8,
  lit_level_0_9,
};


static void
lit_load_level(lit_game_t* m, u32_t level_id) {
  m->stage_flash_timer = 0.f;
  m->stage_fade_timer = LIT_ENTER_DURATION;
  m->state = LIT_STATE_TYPE_TRANSITION_IN;

  m->sensor_count = 0;;
  m->light_count = 0;
  m->edge_count = 0;;

  lit_push_edge(m, 0.f, 0.f, 800.f, 0.f);
  lit_push_edge(m, 800.f, 0.f, 800.f, 800.f);
  lit_push_edge(m, 800.f, 800.f, 0.f, 800.f);
  lit_push_edge(m, 0.f, 800.f, 0.f, 0.f);

  lit_init_player(m, 400.f, 400.f);

  lit_levels[level_id](m);

}

static void
lit_load_next_level(lit_game_t* m){
  m->current_level_id = (m->current_level_id + 1)%array_count(lit_levels);
  lit_load_level(m, m->current_level_id);  
}

