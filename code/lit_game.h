
typedef void lit_game_exit_callback_f();

//
// Game states
//
enum lit_game_state_type_t {
  LIT_GAME_STATE_TYPE_TRANSITION_IN,
  LIT_GAME_STATE_TYPE_NORMAL,

  LIT_GAME_STATE_TYPE_SOLVED_IN,
  LIT_GAME_STATE_TYPE_SOLVED_OUT,
  LIT_GAME_STATE_TYPE_TRANSITION_OUT,

  LIT_GAME_STATE_TYPE_EXIT,
};


//
// Edge
//
struct lit_game_edge_t {
  //b32_t is_disabled;
  v2f_t start_pt;
  v2f_t end_pt;
};

struct lit_game_double_edge_t {
  lit_game_edge_t* e1;
  lit_game_edge_t* e2;

};

// 
// Light
//
struct lit_light_intersection_t {
  b32_t is_shell;
  v2f_t pt;
};

struct lit_game_light_triangle_t {
  v2f_t p0, p1, p2;
};


// NOTE(momo): Probably better to put all the triangles together
// so that stuff is more data oriented, instead of having 
// a bunch of triangles per light
struct lit_game_light_t {
  v2f_t dir;
  f32_t half_angle;
  
  v2f_t pos;  
  u32_t color;

  u32_t triangle_count;
  lit_game_light_triangle_t triangles[512];

  u32_t intersection_count;
  lit_light_intersection_t intersections[512];

};


enum lit_game_light_type_t {
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
  lit_particle_t particles[512];
};



//
// Sensors
//

struct lit_game_sensor_group_t {
  lit_game_exit_callback_f* callback;
  u32_t sensor_count; // how many sensors there are
};

struct lit_game_sensor_t {
  v2f_t pos;
  u32_t target_color;
  u32_t current_color;
  f32_t particle_cd;
  u32_t group_id;
};


//
// Player
//

enum lit_game_player_light_hold_mode_t {
  LIT_PLAYER_LIGHT_HOLD_MODE_NONE,
  LIT_PLAYER_LIGHT_HOLD_MODE_MOVE,
  LIT_PLAYER_LIGHT_HOLD_MODE_ROTATE,
};

struct lit_game_player_t {
  v2f_t pos;
  f32_t locked_pos_x;

  lit_game_player_light_hold_mode_t light_hold_mode;
  lit_game_light_t* nearest_light;
  lit_game_light_t* held_light;
  b32_t is_holding_light;

  // For animating getting the light
  f32_t light_retrival_time;
  v2f_t old_light_pos;
};

//
// Animators
// 
enum lit_game_animator_type_t {
  LIT_ANIMATOR_TYPE_PATROL_POINT,
  LIT_ANIMATOR_TYPE_ROTATE_POINT,
};

struct lit_game_animator_rotate_point_t {
  v2f_t* point; 
  f32_t speed;
  v2f_t* point_of_rotation;
  v2f_t delta;
};

struct lit_game_animator_patrol_point_t {
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


struct lit_game_animator_t {
  lit_game_animator_type_t type;
  union {
    lit_game_animator_rotate_point_t rotate_point;
    lit_game_animator_patrol_point_t move_point;
  };
};

//
// Main game struct
//


struct lit_game_t {
  
  // Assets
  asset_font_id_t tutorial_font;
  asset_sprite_id_t blank_sprite;
  asset_sprite_id_t circle_sprite;
  asset_sprite_id_t filled_circle_sprite;
  asset_sprite_id_t move_sprite;
  asset_sprite_id_t rotate_sprite;

  lit_game_state_type_t state;

  u32_t current_level_id;
  lit_game_player_t player;
 
  u32_t edge_count;
  lit_game_edge_t edges[256];

  u32_t light_count;
  lit_game_light_t lights[32];

  u32_t sensor_count;
  lit_game_sensor_t sensors[128];

  u32_t animator_count;
  lit_game_animator_t animators[64];

  u32_t point_count;
  v2f_t points[32];

  // for sensor group API
  u32_t selected_sensor_group_id;

  u32_t sensor_group_count;
  lit_game_sensor_group_t sensor_groups[32];

  // for animated objects APIs
  lit_game_animator_t* selected_animator_for_sensor; 
  lit_game_animator_t* selected_animator_for_double_edge_min[2]; 
  lit_game_animator_t* selected_animator_for_double_edge_max[2]; 


  lit_particle_pool_t particles;

  f32_t stage_fade_timer;
  f32_t stage_flash_timer;
  f32_t exit_fade;

  rng_t rng;

  str8_t title;
  f32_t title_timer;
  u32_t title_wp_index;
    
  // for exiting the level
  lit_game_exit_callback_f* exit_callback;
  
  // stop time
  b32_t freeze;

};



struct lit_game_title_waypoint_t {
  f32_t x;
  f32_t arrival_time;
};

static lit_game_title_waypoint_t lit_title_wps[] = {
  { -800.0f,  0.0f },
  { 300.0f,   1.0f },
  { 500.0f,   2.0f },
  { 1600.0f,  3.0f },
};
