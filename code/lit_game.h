//
// Game states
//
enum lit_state_type_t {
  LIT_STATE_TYPE_TRANSITION_IN,
  LIT_STATE_TYPE_NORMAL,

  // exiting states are after NORMAL
  LIT_STATE_TYPE_SOLVED_IN,
  LIT_STATE_TYPE_SOLVED_OUT,
  LIT_STATE_TYPE_TRANSITION_OUT,
};


//
// Edge
//
struct lit_edge_t {
  //b32_t is_disabled;
  v2f_t start_pt;
  v2f_t end_pt;
};


// 
// Light
//
struct lit_light_intersection_t {
  b32_t is_shell;
  v2f_t pt;
};

struct lit_light_triangle_t {
  v2f_t p0, p1, p2;
};

struct lit_light_t {
  v2f_t dir;
  f32_t half_angle;
  
  v2f_t pos;  
  u32_t color;

  u32_t triangle_count;
  lit_light_triangle_t triangles[256];

  u32_t intersection_count;
  lit_light_intersection_t intersections[256];

};


enum lit_light_type_t {
  Lit_LIGHT_TYPE_POINT,
  Lit_LIGHT_TYPE_DIRECTIONAL,
  Lit_LIGHT_TYPE_WEIRD
};


//
// Particles
//
struct lit_particle_t {
  v2f_t pos, vel;
  v2f_t size_start, size_end;
  rgba_t color_start, color_end;
  asset_sprite_id_t sprite_id;
  f32_t lifespan;
  f32_t lifespan_now;
};

struct lit_particle_pool_t {
  f32_t particle_lifespan_min;
  f32_t particle_lifespan_max;

  f32_t particle_color_start;
  f32_t particle_color_end;

  u32_t particle_count;
  lit_particle_t particles[256];
};



//
// Sensors
//
struct lit_sensor_t {
  v2f_t pos;
  u32_t target_color;
  u32_t current_color;
  f32_t particle_cd;
};


//
// Player
//
struct lit_player_t {
  v2f_t pos;
  lit_light_t* held_light;
  b32_t is_holding_light;

  // For animating getting the light
  f32_t light_retrival_time;
  v2f_t old_light_pos;
};

//
// Animators
// 
enum lit_animator_type_t {
  LIT_ANIMATOR_TYPE_PATROL_EDGE,

  //LIT_ANIMATOR_TYPE_ROTATE_SENSOR,

  LIT_ANIMATOR_TYPE_ROTATE_EDGE,

  LIT_ANIMATOR_TYPE_PATROL_POINT,
  LIT_ANIMATOR_TYPE_ROTATE_POINT,
};

struct lit_animator_rotate_point_t {
  v2f_t* point; 
  f32_t speed;
  v2f_t point_of_rotation;
};

struct lit_animator_patrol_point_t {
  v2f_t* point;

  f32_t timer;
  f32_t duration;

  u32_t current_waypoint_index;
  u32_t next_waypoint_index;
  u32_t waypoint_count;
  v2f_t waypoints[8];

  v2f_t start;
  v2f_t end;

};

struct lit_animator_rotate_edge_t {
  // NOT IMPLEMENTED
};


struct lit_animator_patrol_edge_t {
  lit_edge_t* edge;
  f32_t timer;
  f32_t duration;
  lit_edge_t start_edge;
  lit_edge_t end_edge;

};

struct lit_animator_t {
  lit_animator_type_t type;
  union {
    lit_animator_patrol_edge_t patrol_edge;
    lit_animator_rotate_edge_t rotate_edge;
    lit_animator_rotate_point_t rotate_point;
    lit_animator_patrol_point_t move_point;
  };
};



//
// Main game struct
//
struct lit_game_t {
  
  asset_font_id_t tutorial_font;
  asset_sprite_id_t blank_sprite;
  asset_sprite_id_t circle_sprite;
  asset_sprite_id_t filled_circle_sprite;

  lit_state_type_t state;

  u32_t current_level_id;
  lit_player_t player;
 
  u32_t edge_count;
  lit_edge_t edges[256];

  u32_t light_count;
  lit_light_t lights[32];

  u32_t sensor_count;
  lit_sensor_t sensors[32];

  u32_t animator_count;
  lit_animator_t animators[64];

  // for animated sensor APIs
  lit_sensor_t* selected_sensor; 
  lit_animator_t* selected_animator; 

  lit_particle_pool_t particles;

  f32_t stage_fade_timer;
  f32_t stage_flash_timer;

  b32_t is_win_reached;
  rng_t rng;

  str8_t title;
  f32_t title_timer;
  u32_t title_wp_index;
    
  u32_t sensors_activated;


};



struct lit_title_waypoint_t {
  f32_t x;
  f32_t arrival_time;
};

static lit_title_waypoint_t lit_title_wps[] = {
  { -800.0f,  0.0f },
  { 300.0f,   1.0f },
  { 500.0f,   2.0f },
  { 1600.0f,  3.0f },
};
