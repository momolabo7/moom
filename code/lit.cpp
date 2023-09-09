// TODO: Introduce time stop mechanic
//


#include "momo.h"
#include "lit_asset_types.h"
#include "game.h"

// 
// Configs
//

// Main
#define LIT_WIDTH  800.f
#define LIT_HEIGHT 800.f
#define LIT_EXIT_FLASH_DURATION 0.1f
#define LIT_EXIT_FLASH_BRIGHTNESS 0.6f
#define LIT_ENTER_DURATION 3.f
#define LIT_SKIP_TRANSITIONS false

// Debug
#define LIT_DEBUG_LINES 0

// Splash
#define LIT_SPLASH_SCROLL_POS_Y_START (-100.f)
#define LIT_SPLASH_SCROLL_POS_Y_END (300.f)
#define LIT_SPLASH_SCROLL_DURATION 1.5f

// Credits
#define LIT_CREDITS_START_COOLDOWN_DURATION (2.f)
#define LIT_CREDITS_SCROLL_SPEED (200.f)

// Save file
#define LIT_SAVE_FILE_ENABLE true
#define LIT_SAVE_FILE "g_lit.sav"

// Sensor
#define LIT_SENSOR_COLOR_MASK 0xFFFFFF00
#define LIT_SENSOR_RADIUS 8.f
#define LIT_SENSOR_PARTICLE_CD 0.1f
#define LIT_SENSOR_PARTICLE_SIZE 14.f
#define LIT_SENSOR_PARTICLE_SPEED 20.f

// Player
#define LIT_PLAYER_RADIUS 16.f
#define LIT_PLAYER_LIGHT_RETRIEVE_DURATION 0.05f
#define LIT_PLAYER_BREATH_DURATION 2.f
#define LIT_PLAYER_PICKUP_DIST 512.f
#define LIT_PLAYER_ROTATE_SPEED 1.f

// Tutorial
#define LIT_LEARNT_BASICS_LEVEL_ID (3)
#define LIT_LEARNT_POINT_LIGHT_LEVEL_ID (7)

#define lit_log(...) game_debug_log(g_game, __VA_ARGS__)
#define lit_profile_block(name) game_profile_block(g_game, name)
#define lit_profile_begin(name) game_profile_begin(g_game, name)
#define lit_profile_end(name) game_profile_end(g_game, name)


//
// Profiler stats
//
struct lit_profiler_stat_t {
  f64_t min;
  f64_t max;
  f64_t average;
  u32_t count;
};

//
// Slash screen
//
struct lit_splash_t {
  f32_t timer;
  f32_t scroll_in_timer;
  game_asset_font_t* font;
};


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
  game_asset_sprite_id_t sprite_id;
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
// Game 
//
struct lit_game_t {
  
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

  st8_t title;
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

//
// Credits
//
struct lit_credits_t {
  f32_t timer;
};

// 
// Lit 
//
enum lit_show_debug_type_t {
  LIT_SHOW_DEBUG_NONE,
  LIT_SHOW_DEBUG_PROFILER,
  LIT_SHOW_DEBUG_CONSOLE,
  LIT_SHOW_DEBUG_INSPECTOR,
  
  LIT_SHOW_DEBUG_MAX
};

enum lit_mode_t {
  LIT_MODE_NONE,
  LIT_MODE_SPLASH,
  LIT_MODE_GAME,
  LIT_MODE_CREDITS,
  LIT_MODE_SANDBOX,
};

struct lit_save_data_t {
  u32_t unlocked_levels;
};

struct lit_t {
  lit_save_data_t save_data;
  lit_show_debug_type_t show_debug_type;
  lit_mode_t next_mode;
  lit_mode_t mode;
  union {
    lit_splash_t splash;
    lit_game_t g_game;
    lit_credits_t credits;
  };
  u32_t level_to_start;

  // Arenas
  arena_t mode_arena;
  arena_t asset_arena;
  arena_t debug_arena;
  arena_t frame_arena;

  // Assets
  game_assets_t assets;

  // Debug Stuff 
  console_t console;
};

//
// Globals
// 
static game_t* g_game; 
static lit_t* g_lit;
static lit_game_title_waypoint_t lit_title_wps[] = {
  { -800.0f,  0.0f },
  { 300.0f,   1.0f },
  { 500.0f,   2.0f },
  { 1600.0f,  3.0f },
};

//
// Save Data Functions
//
static b32_t
lit_is_in_tutorial() {
  return g_lit->save_data.unlocked_levels <= LIT_LEARNT_BASICS_LEVEL_ID;
}

static b32_t 
lit_unlock_next_level(u32_t current_level_id) {

  if (current_level_id >= g_lit->save_data.unlocked_levels) {
    game_file_t file = {};

    g_lit->save_data.unlocked_levels = current_level_id + 1;

    if (game_open_file(g_game, &file, LIT_SAVE_FILE, GAME_FILE_ACCESS_OVERWRITE, GAME_FILE_PATH_USER)) 
    {
      game_write_file(g_game, &file, sizeof(g_lit->save_data), 0, (void*)&g_lit->save_data);
      game_close_file(g_game, &file);
      return true;
    }
    else {
      // if we reach here, something is wrong with the file system
      return false;
    }
  }
  return true;
}


static b32_t
lit_init_save_data() {
  g_lit->save_data.unlocked_levels = 0;
  game_file_t file = {};

  // save data actually found
  if (game_open_file(g_game, &file, LIT_SAVE_FILE, GAME_FILE_ACCESS_READ, GAME_FILE_PATH_USER)) {
    if(game_read_file(g_game, &file, sizeof(lit_save_data_t), 0, &g_lit->save_data)) {
      // hgamey! :3
      game_close_file(g_game, &file);
      return true;
    }
    else { // data is somehow corrupted?
      return false;
    }

  }
  else { // save data not found
    return lit_unlock_next_level(0);
  }

}

//
// Misc Functions
//
static u32_t
lit_get_levels_unlocked_count() {
  return g_lit->save_data.unlocked_levels;
}


static void
lit_goto_credits() {
  g_lit->next_mode = LIT_MODE_CREDITS;
}

//
// Console rendering
//
static void
lit_update_and_render_console() 
{
  console_t* console = &g_lit->console;
  game_assets_t* assets = &g_lit->assets;
  game_input_characters_t characters = game_get_input_characters(g_game);

  for (u32_t char_index = 0; 
       char_index < characters.count;
       ++char_index) 
  {
    // NOTE(Momo): Not very portable to other platforms....
    u8_t c = characters.data[char_index];
    if (c >= 32 && c <= 126) {
      stb8_push_u8(&console->input_line, c);
    }
    // backspace 
    if (c == 8) {
      if (console->input_line.count > 0) 
        stb8_pop(&console->input_line);
    }    
    
    if (c == '\r') {
      console_execute(console);
      break;
    }
  }
  
  // Render
  static const f32_t console_width = LIT_WIDTH;
  static const f32_t console_height = 400.f;
  static const u32_t line_count = array_count(console->info_lines)+1;
  static const f32_t line_height = console_height/line_count;
  static const f32_t font_height = line_height * 0.8f;
  static const f32_t font_bottom_pad = (line_height - font_height);
  static const f32_t left_pad = 10.f;
  
  v2f_t console_size = v2f_set(console_width, console_height);
  v2f_t console_pos = v2f_set(console_width/2, console_height/2);
  v2f_t input_area_size = v2f_set(console_width, line_height);
  v2f_t input_area_pos = v2f_set(console_width/2, line_height/2);
  
  game_draw_asset_sprite(
      g_game, assets, 
      ASSET_SPRITE_ID_BLANK_SPRITE,
      v2f_set(LIT_WIDTH/2, LIT_HEIGHT/2), 
      v2f_set(LIT_WIDTH, LIT_HEIGHT),
      rgba_set(0.f, 0.f, 0.f, 0.8f));
  game_advance_depth(g_game);
  
  game_draw_asset_sprite(g_game, assets, ASSET_SPRITE_ID_BLANK_SPRITE, console_pos, console_size, rgba_hex(0x787878FF));
  game_advance_depth(g_game);
  game_draw_asset_sprite(g_game, assets, ASSET_SPRITE_ID_BLANK_SPRITE, input_area_pos, input_area_size, rgba_hex(0x505050FF));
  game_advance_depth(g_game);
  
  
  // Draw info text
  for (u32_t line_index = 0;
       line_index < array_count(console->info_lines);
       ++line_index)
  {
    stb8_t* line = console->info_lines + line_index;

    game_draw_text(
        g_game, assets, 
        ASSET_FONT_ID_DEBUG,
        line->str,
        rgba_hex(0xFFFFFFFF),
        left_pad, 
        line_height * (line_index+1) + font_bottom_pad,
        font_height);
    
  }

  game_advance_depth(g_game);
  game_draw_text(g_game, assets, 
      ASSET_FONT_ID_DEBUG,
      console->input_line.str,
      rgba_hex(0xFFFFFFFF),
      left_pad, 
      font_bottom_pad,
      font_height);
  game_advance_depth(g_game);
}

//
// Profiler Update and Rendering
//
static void
lit_profiler_begin_stat(lit_profiler_stat_t* stat) {
  stat->min = F64_INFINITY;
  stat->max = F64_NEG_INFINITY;
  stat->average = 0.0;
  stat->count = 0;
}

static void
lit_profiler_accumulate_stat(lit_profiler_stat_t* stat, f64_t value) {
  ++stat->count;
  if (stat->min > value) {
    stat->min = value;
  }
  if (stat->max < value) {
    stat->max = value;
  }
  stat->average += value;
}

static void
lit_profiler_end_stat(lit_profiler_stat_t* stat) {
  if(stat->count) {
    stat->average /= (f64_t)stat->count;
  }
  else {
    stat->min = 0.0;
    stat->max = 0.0;
  }
}

static void
lit_profiler_update_and_render() 
{
  game_assets_t* assets = &g_lit->assets;

  const f32_t render_width = LIT_WIDTH;
  const f32_t render_height = LIT_HEIGHT;
  const f32_t font_height = 20.f;

  // Overlay
  game_draw_asset_sprite(
      g_game, assets, ASSET_SPRITE_ID_BLANK_SPRITE, 
      v2f_set(LIT_WIDTH/2, LIT_HEIGHT/2), 
      v2f_set(LIT_WIDTH, LIT_HEIGHT),
      rgba_set(0.f, 0.f, 0.f, 0.5f));
  game_advance_depth(g_game);
  
  u32_t line_num = 1;
  
  for(u32_t entry_id = 0; entry_id < g_game->profiler.entry_count; ++entry_id)
  {
    game_profiler_entry_t* entry = g_game->profiler.entries + entry_id;

    lit_profiler_stat_t cycles;
    lit_profiler_stat_t hits;
    lit_profiler_stat_t cycles_per_hit;
    
    lit_profiler_begin_stat(&cycles);
    lit_profiler_begin_stat(&hits);
    lit_profiler_begin_stat(&cycles_per_hit);
    
    for (u32_t snapshot_index = 0;
         snapshot_index < g_game->profiler.entry_snapshot_count;
         ++snapshot_index)
    {
      
      game_profiler_snapshot_t * snapshot = entry->snapshots + snapshot_index;
      
      lit_profiler_accumulate_stat(&cycles, (f64_t)snapshot->cycles);
      lit_profiler_accumulate_stat(&hits, (f64_t)snapshot->hits);
      
      f64_t cph = 0.0;
      if (snapshot->hits) {
        cph = (f64_t)snapshot->cycles/(f64_t)snapshot->hits;
      }
      lit_profiler_accumulate_stat(&cycles_per_hit, cph);
    }
    lit_profiler_end_stat(&cycles);
    lit_profiler_end_stat(&hits);
    lit_profiler_end_stat(&cycles_per_hit);
    
    stb8_make(sb, 256);
    stb8_push_fmt(sb, 
                 st8_from_lit("[%20s] %8ucy %4uh %8ucy/h"),
                 entry->block_name,
                 (u32_t)cycles.average,
                 (u32_t)hits.average,
                 (u32_t)cycles_per_hit.average);
    
    game_draw_text(g_game, assets, 
        ASSET_FONT_ID_DEBUG, 
        sb->str,
        rgba_hex(0xFFFFFFFF),
        0.f, 
        render_height - font_height * (line_num), 
        font_height);
    game_advance_depth(g_game);

    
    // Draw graph
    for (u32_t snapshot_index = 0;
         snapshot_index < g_game->profiler.entry_snapshot_count;
         ++snapshot_index)
    {
      game_profiler_snapshot_t * snapshot = entry->snapshots + snapshot_index;
      
      const f32_t snapshot_bar_width = 1.5f;
      f32_t height_scale = 1.0f / (f32_t)cycles.max;
      f32_t snapshot_bar_height = 
        height_scale * font_height * (f32_t)snapshot->cycles * 0.95f;
     
      v2f_t pos = v2f_set(
        560.f + snapshot_bar_width * (snapshot_index), 
        render_height - font_height * (line_num) + font_height/4);

      v2f_t size = v2f_set(snapshot_bar_width, snapshot_bar_height);
      game_draw_asset_sprite(g_game, assets, ASSET_SPRITE_ID_BLANK_SPRITE, pos, size, rgba_hex(0x00FF00FF));
    }
    game_advance_depth(g_game);
    ++line_num;
  }
}

//
// Inspector update and render
//
static void 
lit_inspector_update_and_render() 
{
  game_inspector_t* inspector = &g_game->inspector;
  game_assets_t* assets = &g_lit->assets;
  game_draw_asset_sprite(
      g_game, 
      assets, 
      ASSET_SPRITE_ID_BLANK_SPRITE, 
      v2f_set(LIT_WIDTH/2, LIT_HEIGHT/2), 
      v2f_set(LIT_WIDTH, LIT_HEIGHT),
      rgba_set(0.f, 0.f, 0.f, 0.5f));
  game_advance_depth(g_game);

  for(u32_t entry_index = 0; entry_index < inspector->entry_count; ++entry_index)
  {
    f32_t line_height = 32.f;
    stb8_make(sb, 256);


    game_inspector_entry_t* entry = inspector->entries + entry_index;
    switch(entry->type){
      case GAME_INSPECTOR_ENTRY_TYPE_U32: {
        stb8_push_fmt(sb, st8_from_lit("[%10S] %7u"),
            entry->name, entry->item_u32);
      } break;
      case GAME_INSPECTOR_ENTRY_TYPE_F32: {
        stb8_push_fmt(sb, st8_from_lit("[%10S] %7f"),
            entry->name, entry->item_f32);
      } break;
    }

    f32_t y = LIT_HEIGHT - line_height * (entry_index+1);
    game_draw_text(g_game, assets, ASSET_FONT_ID_DEBUG, sb->str, rgba_hex(0xFFFFFFFF), 0.f, y, line_height);
    game_advance_depth(g_game);
  }
}

// 
// Splash Functions
//

static void
lit_splash_init() {
  lit_splash_t* splash = &g_lit->splash;

  splash->timer = 3.f;
  splash->scroll_in_timer = LIT_SPLASH_SCROLL_DURATION;
}

static void
lit_splash_update() {
  lit_splash_t* splash = &g_lit->splash;
  game_asset_font_id_t font = ASSET_FONT_ID_DEFAULT;

  splash->timer -= game_get_dt(g_game);
  splash->scroll_in_timer -= game_get_dt(g_game);

  if (game_is_button_poked(g_game, GAME_BUTTON_CODE_LMB)) {
    if (splash->scroll_in_timer > 0.f)  {
      splash->scroll_in_timer = 0.f;
    }
    else {
      splash->timer = 0.f;
    }
  }

  f32_t y = LIT_HEIGHT/2 + 100.f;

  game_draw_text_center_aligned(
      g_game, 
      &g_lit->assets, 
      font, 
      st8_from_lit("--------"), 
      rgba_set(1.f, 1.f, 1.f, 1.f),
      LIT_WIDTH/2, y, 
      128.f);
  y -= 70;
  game_draw_text_center_aligned(
      g_game, 
      &g_lit->assets, 
      font,
      st8_from_lit("PRIMIX"), 
      rgba_set(1.f, 1.f, 1.f, 1.f),
      LIT_WIDTH/2, y, 
      128.f);

  y -= 70;
  game_draw_text_center_aligned(
      g_game, 
      &g_lit->assets, 
      font,
      st8_from_lit("--------"), 
      rgba_set(1.f, 1.f, 1.f, 1.f),
      LIT_WIDTH/2, y, 
      128.f);
  y -= 60.f;

  
  // 
  //
  //
 
  f32_t alpha = 1.f-f32_ease_linear(splash->scroll_in_timer/LIT_SPLASH_SCROLL_DURATION);
  if (splash->scroll_in_timer < 0.f) {
    splash->scroll_in_timer = 0.f;
  }


  f32_t scroll_y = (alpha)*LIT_SPLASH_SCROLL_POS_Y_END + (1.f-alpha)*LIT_SPLASH_SCROLL_POS_Y_START; 
  rgba_t grey = rgba_set(0.7f, 0.7f, 0.7f, 1.f); 
  game_draw_text_center_aligned(
      g_game, 
      &g_lit->assets, 
      font,
      st8_from_lit("a silly game by"), 
      grey,
      LIT_WIDTH/2, scroll_y, 
      36.f);

  scroll_y -= 60.f;
  game_draw_text_center_aligned(
      g_game, 
      &g_lit->assets, 
      font,
      st8_from_lit("momohoudai"), 
      grey,
      LIT_WIDTH/2, scroll_y, 
      72.f);
  game_advance_depth(g_game);


  if (splash->timer <= 0.f) {
    g_lit->next_mode = LIT_MODE_GAME; 
  }

}

//
// Game Functions
//


static b32_t 
lit_game_is_exiting(lit_game_t* g) {
  return g->state > LIT_GAME_STATE_TYPE_NORMAL; 
}

static void
lit_game_set_title(lit_game_t* g, st8_t str = st8_t{}) {
  g->title = str;
  g->title_timer = 0.f;
  g->title_wp_index = 0;
}





//
// Points
//
static v2f_t*
lit_game_push_point(lit_game_t* m, v2f_t point) {
  assert(m->point_count < array_count(m->points));
  m->points[m->point_count] = point;
  return m->points + m->point_count++;
}

// 
// Animator
//


static void 
lit_game_animator_push_patrol_point_waypoint(
    lit_game_animator_t* animator, 
    f32_t pos_x, 
    f32_t pos_y)
{
  assert(animator->type == LIT_ANIMATOR_TYPE_PATROL_POINT);
  auto* a = &animator->move_point;

  // Add the waypoint
  assert(a->waypoint_count < array_count(a->waypoints));
  v2f_t* wp = a->waypoints + a->waypoint_count++;
  wp->x = pos_x;
  wp->y = pos_y;


}

static lit_game_animator_t*
lit_game_animator_begin_patrol_point(lit_game_t* m, f32_t duration_per_waypoint, v2f_t* point) 
{
  assert(m->animator_count < array_count(m->animators));
  auto* anim = m->animators + m->animator_count++;
  anim->type = LIT_ANIMATOR_TYPE_PATROL_POINT;

  auto* a = &anim->move_point;
  a->timer = 0.f;
  a->duration = duration_per_waypoint;
  a->point = point;
  a->waypoint_count = 0;
  a->current_waypoint_index = 0;

  lit_game_animator_push_patrol_point_waypoint(anim, point->x, point->y);

  return anim;
}

static void 
lit_game_animator_end_patrol_point(lit_game_animator_t* anim)
{
  auto* a = &anim->move_point;
  a->timer = 0.f;
  a->current_waypoint_index = 0;
  u32_t next_waypoint_index = (a->current_waypoint_index + 1) % a->waypoint_count;
  a->start = a->waypoints[a->current_waypoint_index];
  a->end = a->waypoints[next_waypoint_index];
}


static lit_game_animator_rotate_point_t* 
lit_game_animator_push_rotate_point(
    lit_game_t* m,
    v2f_t* pt_to_rotate,
    v2f_t* pt_of_rotation,
    f32_t speed) 
{

  assert(m->animator_count < array_count(m->animators));
  auto* anim = m->animators + m->animator_count++;
  anim->type = LIT_ANIMATOR_TYPE_ROTATE_POINT;

  auto* a = &anim->rotate_point;
  a->speed = speed;
  a->point_of_rotation = pt_of_rotation;
  a->point = pt_to_rotate;
  a->delta = dref(pt_to_rotate) - dref(pt_of_rotation);

  return a;
}

static void 
lit_game_animate(lit_game_animator_t* animator, f32_t dt) {
  switch(animator->type) {
    case LIT_ANIMATOR_TYPE_PATROL_POINT:{
      auto* a = &animator->move_point;
      a->timer += dt;
      if (a->timer > a->duration) {
        a->timer = 0.f;
        a->current_waypoint_index = ++a->current_waypoint_index % a->waypoint_count;
        u32_t next_waypoint_index = (a->current_waypoint_index + 1) % a->waypoint_count;
        a->start = a->waypoints[a->current_waypoint_index];
        a->end = a->waypoints[next_waypoint_index];
      }

      f32_t alpha = f32_ease_inout_sine(a->timer/a->duration);
      *a->point = v2f_lerp(a->start, a->end, alpha);
    } break;
    case LIT_ANIMATOR_TYPE_ROTATE_POINT: {
      auto* a = &animator->rotate_point;
      a->delta = v2f_rotate(a->delta, a->speed * dt);
      dref(a->point) = dref(a->point_of_rotation) + a->delta;
    } break;

  }
}

static void
lit_game_animate_everything(lit_game_t* m, f32_t dt) {
  for(u32_t animator_index = 0; animator_index < m->animator_count; ++animator_index)
  {
    lit_game_animate(m->animators + animator_index, dt);
  }
}

//
// Edges
//

static void 
lit_game_calc_ghost_edge_line(lit_game_edge_t* e, v2f_t* min, v2f_t* max) {
  v2f_t dir = v2f_norm(e->end_pt - e->start_pt) * 0.0001f;

  *min = v2f_sub(e->start_pt, dir);
  *max = v2f_add(e->end_pt, dir);
}

static lit_game_edge_t*
lit_game_push_edge(lit_game_t* m, f32_t min_x, f32_t min_y, f32_t max_x, f32_t max_y) {
  assert(m->edge_count < array_count(m->edges));
  lit_game_edge_t* edge = m->edges + m->edge_count++;

  edge->start_pt = v2f_set(min_x, min_y);
  edge->end_pt = v2f_set(max_x, max_y);

  // TODO: can we remove this part?
  v2f_t p0, p1;
  lit_game_calc_ghost_edge_line(edge, &p0, &p1);
  edge->start_pt = p0;
  edge->end_pt = p1;

  return edge;
}




static void 
lit_game_push_box(lit_game_t* m, f32_t min_x, f32_t min_y, f32_t max_x, f32_t max_y) 
{
  lit_game_push_edge(m, min_x, min_y, max_x, min_y);
  lit_game_push_edge(m, max_x, min_y, max_x, max_y);
  lit_game_push_edge(m, max_x, max_y, min_x, max_y);
  lit_game_push_edge(m, min_x, max_y, min_x, min_y);
}

static void 
lit_game_push_aabb(lit_game_t* m, f32_t cx, f32_t cy, f32_t hw, f32_t hh) {
  f32_t min_x = cx - hw;
  f32_t max_x = cx + hw;
  f32_t min_y = cy - hh;
  f32_t max_y = cy + hh;

  lit_game_push_edge(m, min_x, min_y, max_x, min_y);
  lit_game_push_edge(m, max_x, min_y, max_x, max_y);
  lit_game_push_edge(m, max_x, max_y, min_x, max_y);
  lit_game_push_edge(m, min_x, max_y, min_x, min_y);
}

static lit_game_double_edge_t 
lit_game_push_double_edge(lit_game_t* m, f32_t min_x, f32_t min_y, f32_t max_x, f32_t max_y) {
  auto* e1 = lit_game_push_edge(m, min_x, min_y, max_x, max_y);
  auto* e2 = lit_game_push_edge(m, max_x, max_y, min_x, min_y);
  return { e1, e2 };
}

static void
lit_game_push_rotating_double_edge(
    lit_game_t* g, 
    f32_t min_x, f32_t min_y, f32_t max_x, f32_t max_y,
    v2f_t* pt_of_rotation_for_min,
    v2f_t* pt_of_rotation_for_max,
    f32_t speed_for_min,
    f32_t speed_for_max)
{
  auto edges = lit_game_push_double_edge(g, min_x, min_y, max_x, max_y);

  if (pt_of_rotation_for_min) {
    lit_game_animator_push_rotate_point(g, &edges.e1->start_pt, pt_of_rotation_for_min, speed_for_min);
    lit_game_animator_push_rotate_point(g, &edges.e2->end_pt, pt_of_rotation_for_min, speed_for_min);
  }

  if (pt_of_rotation_for_max) {
    lit_game_animator_push_rotate_point(g, &edges.e1->end_pt, pt_of_rotation_for_max, speed_for_max);
    lit_game_animator_push_rotate_point(g, &edges.e2->start_pt, pt_of_rotation_for_max, speed_for_max);
  }
}

static void
lit_game_begin_patrolling_double_edge(lit_game_t* g, f32_t min_x, f32_t min_y, f32_t max_x, f32_t max_y, f32_t duration_per_waypoint)
{
  assert(!g->selected_animator_for_double_edge_min[0]);
  assert(!g->selected_animator_for_double_edge_min[1]);
  assert(!g->selected_animator_for_double_edge_max[0]);
  assert(!g->selected_animator_for_double_edge_max[1]);

  //
  // NOTE(momo): this is kind of disgusting because it is possible that not all points need to be patrolling,
  // so we are potentially wasting space, but whatevers....
  //
  auto edges = lit_game_push_double_edge(g, min_x, min_y, max_x, max_y);
  g->selected_animator_for_double_edge_min[0] = 
    lit_game_animator_begin_patrol_point(g, duration_per_waypoint, &edges.e1->start_pt);
  g->selected_animator_for_double_edge_min[1] = 
    lit_game_animator_begin_patrol_point(g, duration_per_waypoint, &edges.e2->end_pt);
  g->selected_animator_for_double_edge_max[0] = 
    lit_game_animator_begin_patrol_point(g, duration_per_waypoint, &edges.e1->end_pt);
  g->selected_animator_for_double_edge_max[1] = 
    lit_game_animator_begin_patrol_point(g, duration_per_waypoint, &edges.e2->start_pt);
}

static void
lit_game_push_patrolling_double_edge_waypoint_for_min(lit_game_t* g, f32_t x, f32_t y){

  assert(g->selected_animator_for_double_edge_min[0]);
  assert(g->selected_animator_for_double_edge_min[1]);

  lit_game_animator_push_patrol_point_waypoint(g->selected_animator_for_double_edge_min[0], x, y);
  lit_game_animator_push_patrol_point_waypoint(g->selected_animator_for_double_edge_min[1], x, y);
}
static void
lit_game_push_patrolling_double_edge_waypoint_for_max(lit_game_t* g, f32_t x, f32_t y){

  assert(g->selected_animator_for_double_edge_max[0]);
  assert(g->selected_animator_for_double_edge_max[1]);

  lit_game_animator_push_patrol_point_waypoint(g->selected_animator_for_double_edge_max[0], x, y);
  lit_game_animator_push_patrol_point_waypoint(g->selected_animator_for_double_edge_max[1], x, y);
}

static void
lit_game_end_patrolling_double_edge(lit_game_t* g) {

  assert(g->selected_animator_for_double_edge_min[0]);
  assert(g->selected_animator_for_double_edge_min[1]);
  assert(g->selected_animator_for_double_edge_max[0]);
  assert(g->selected_animator_for_double_edge_max[1]);

  lit_game_animator_end_patrol_point(g->selected_animator_for_double_edge_min[0]);
  lit_game_animator_end_patrol_point(g->selected_animator_for_double_edge_min[1]);
  lit_game_animator_end_patrol_point(g->selected_animator_for_double_edge_max[0]);
  lit_game_animator_end_patrol_point(g->selected_animator_for_double_edge_max[1]);

  g->selected_animator_for_double_edge_min[0] = nullptr; 
  g->selected_animator_for_double_edge_min[1] = nullptr;
  g->selected_animator_for_double_edge_max[0] = nullptr; 
  g->selected_animator_for_double_edge_max[1] = nullptr;
}


//
// LIGHT
//
static lit_game_light_t*
lit_game_push_light(lit_game_t* m, f32_t pos_x, f32_t pos_y, u32_t color, f32_t angle, f32_t turn) {
  assert(m->light_count < array_count(m->lights));
  lit_game_light_t* light = m->lights + m->light_count++;
  light->pos.x = pos_x;
  light->pos.y = pos_y;
  light->color = color;

  f32_t rad = f32_turns_to_radians(turn);
  light->dir.x = f32_cos(rad);
  light->dir.y = f32_sin(rad);
  light->half_angle = f32_deg_to_rad(angle/2.f);

  return light;
}

// Returns F32_INFINITY() if cannot find
static f32_t
lit_game_get_ray_intersection_time_wrt_edges(
    v2f_t ray_origin, 
    v2f_t ray_dir,
    lit_game_edge_t* edges,
    u32_t edge_count,
    b32_t clamp_to_ray_max = false)
{
  f32_t lowest_t1 = clamp_to_ray_max ? 1.f : F32_INFINITY;

  for(u32_t edge_index = 0; edge_index < edge_count; ++edge_index)
  {
    lit_game_edge_t* edge = edges + edge_index;


    //if (edge->is_disabled) continue;

    v2f_t edge_ray_origin = edge->start_pt;
    v2f_t edge_ray_dir = edge->end_pt - edge->start_pt;
    
    // Check for parallel
    v2f_t ray_normal = {};
    ray_normal.x = ray_dir.y;
    ray_normal.y = -ray_dir.x;


    if (!f32_is_close(v2f_dot(ray_normal, edge_ray_dir), 0.f)) {
      f32_t t2 = 
        (ray_dir.x*(edge_ray_origin.y - ray_origin.y) + 
         ray_dir.y*(ray_origin.x - edge_ray_origin.x))/
        (edge_ray_dir.x*ray_dir.y - edge_ray_dir.y*ray_dir.x);

      f32_t t1 = (edge_ray_origin.x + edge_ray_dir.x * t2 - ray_origin.x)/ray_dir.x;

      if (0.f < t1 && 
          0.f < t2 && 
          t2 < 1.f)
      {

        if (t1 < lowest_t1) {
          lowest_t1 = t1;
        }
      }
    }
  }
  return lowest_t1;
}
static void
lit_game_push_triangle(lit_game_light_t* l, v2f_t p0, v2f_t p1, v2f_t p2, u32_t color) {
  assert(l->triangle_count < array_count(l->triangles));
  lit_game_light_triangle_t* tri = l->triangles + l->triangle_count++;
  tri->p0 = p0;
  tri->p1 = p1;
  tri->p2 = p2;
}


static void
lit_gen_light_intersections(lit_game_light_t* l,
    lit_game_edge_t* edges,
    u32_t edge_count,
    arena_t* tmp_arena)
{
  //moe_profile_block(light_generation);
  arena_set_revert_point(tmp_arena);

  lit_game_light_type_t light_type = Lit_LIGHT_TYPE_POINT;
  if (l->half_angle < PI_32/2) {
    light_type = Lit_LIGHT_TYPE_DIRECTIONAL; 
  }
  else if(l->half_angle < PI_32) {
    light_type = Lit_LIGHT_TYPE_WEIRD;
  }

  l->intersection_count = 0;
  l->triangle_count = 0;


  
  f32_t offset_angles[] = {0.0f, 0.001f, -0.001f};
  //f32_t offset_angles[] = {0.0f};
  for (u32_t offset_index = 0;
      offset_index < array_count(offset_angles);
      ++offset_index) 
  {
    f32_t offset_angle = offset_angles[offset_index];
    // For each endpoint
    for(u32_t edge_index = 0; edge_index < edge_count; ++edge_index) 
    {
      lit_game_edge_t* edge = edges + edge_index;

      v2f_t ep = edge->end_pt;      

      // ignore endpoints that are not within the angle 
      f32_t angle = v2f_angle(l->dir, ep - l->pos);
      if (light_type == Lit_LIGHT_TYPE_WEIRD || 
          light_type == Lit_LIGHT_TYPE_DIRECTIONAL) 
      {
        if (angle > l->half_angle) continue;
      }
      else // light_type == Lit_LIGHT_TYPE_POINT 
      {
        // if it's a point light, we don't do anything here.
      }


      v2f_t light_ray_dir = v2f_rotate(ep - l->pos, offset_angle);

      f32_t t = lit_game_get_ray_intersection_time_wrt_edges(l->pos, light_ray_dir, edges, edge_count, offset_index == 0);

      assert(l->intersection_count < array_count(l->intersections));
      lit_light_intersection_t* intersection = l->intersections + l->intersection_count++;
      intersection->pt = (t == F32_INFINITY) ? ep : l->pos + t*light_ray_dir;
      intersection->is_shell = false;
    }
  }

  // Consider 'shell rays', which are rays that are at the 
  // extreme ends of the light (only for non-point lights)
  if (light_type != Lit_LIGHT_TYPE_POINT)
  {
    for (u32_t offset_index = 0;
        offset_index < array_count(offset_angles);
        ++offset_index) 
    { 
      f32_t offset_angle = offset_angles[offset_index];

      v2f_t dirs[2]; 
      dirs[0] = v2f_rotate(l->dir, l->half_angle + offset_angle);
      dirs[1] = v2f_rotate(l->dir, -l->half_angle + offset_angle);
      for (u32_t i = 0; i < 2; ++i) {
        f32_t t = lit_game_get_ray_intersection_time_wrt_edges(l->pos, dirs[i], edges, edge_count);

        assert(l->intersection_count < array_count(l->intersections));
        lit_light_intersection_t* intersection = l->intersections + l->intersection_count++;
        intersection->pt = l->pos + t*dirs[i];
        intersection->is_shell = true;
      }
    }
  }

  if (l->intersection_count > 0) {
    sort_entry_t* sorted_its = arena_push_arr(sort_entry_t, tmp_arena, l->intersection_count);
    assert(sorted_its);
    for (u32_t its_id = 0; 
        its_id < l->intersection_count; 
        ++its_id) 
    {
      lit_light_intersection_t* its = l->intersections + its_id;
      v2f_t basis_vec = v2f_t{1.f, 0.f};
      v2f_t intersection_vec = its->pt - l->pos;
      f32_t key = v2f_angle(basis_vec, intersection_vec);
      if (intersection_vec.y < 0.f) 
        key = PI_32*2.f - key;

      sorted_its[its_id].index = its_id;
      sorted_its[its_id].key = key; 
    }

    quicksort(sorted_its, l->intersection_count);

    for (u32_t sorted_its_id = 0;
        sorted_its_id < l->intersection_count - 1;
        sorted_its_id++)
    {
      lit_light_intersection_t* its0 = l->intersections + sorted_its[sorted_its_id].index;
      lit_light_intersection_t* its1 = l->intersections + sorted_its[sorted_its_id+1].index;

      b32_t ignore = false;

      // In the case of 'wierd' lights,
      // shell ray should not have a triangle to another shell ray 
      if (light_type == Lit_LIGHT_TYPE_WEIRD) {
        if (its0->is_shell && its1->is_shell) {
          ignore = true;
        }
      }

      if (!ignore) {
        v2f_t p0 = its0->pt;
        v2f_t p1 = l->pos;
        v2f_t p2 = its1->pt;

        // Make sure we are going CCW
        if (v2f_cross(p0-p1, p2-p1) > 0.f) {
          lit_game_push_triangle(l, p0, p1, p2, l->color);
        }
      }
    }

    lit_light_intersection_t* its0 = l->intersections + sorted_its[l->intersection_count-1].index;
    lit_light_intersection_t* its1 = l->intersections + sorted_its[0].index;

    // In the case of 'wierd' lights,
    // shell ray should not have a triangle to another shell ray 

    b32_t ignore = false;
    if (light_type == Lit_LIGHT_TYPE_WEIRD) {
      if (its0->is_shell && its1->is_shell) {
        ignore = true;
      }
    }

    if (!ignore) {
      v2f_t p0 = its0->pt;
      v2f_t p1 = l->pos;
      v2f_t p2 = its1->pt;

      // Make sure we are going CCW
      if (v2f_cross(p0-p1, p2-p1) > 0.f) {
        lit_game_push_triangle(l, p0, p1, p2, l->color);
      }
    }
  }

}


static void
lit_gen_lights(
    lit_game_light_t* lights, 
    u32_t light_count,
    lit_game_edge_t* edges,
    u32_t edge_count,
    arena_t* tmp_arena)  {
  // Update all lights
  for(u32_t light_index = 0; light_index < light_count; ++light_index)
  {
    lit_game_light_t* light = lights + light_index;
    lit_gen_light_intersections(light, edges, edge_count, tmp_arena);
  }

}

static void
lit_game_render_lights(lit_game_t* g) {
  //
  // Emitters
  //
  for(u32_t light_index = 0; light_index < g->light_count; ++light_index)
  {
    lit_game_light_t* light = g->lights + light_index;
    game_draw_asset_sprite(g_game, &g_lit->assets, 
        ASSET_SPRITE_ID_FILLED_CIRCLE_SPRITE,
        light->pos,
        v2f_set(16.f, 16.f),
        rgba_set(0.8f, 0.8f, 0.8f, 1.f));
    game_advance_depth(g_game);
  }

  //
  // Lights
  //
  game_set_blend_additive(g_game);
  for(u32_t light_index = 0; light_index < g->light_count; ++light_index)
  {
    lit_game_light_t* l = g->lights + light_index;
    for(u32_t tri_index = 0; tri_index < l->triangle_count; ++tri_index)
    {
      lit_game_light_triangle_t* lt = l->triangles + tri_index;
      game_draw_tri(
          g_game, 
          lt->p0,
          lt->p1,
          lt->p2, 
          rgba_hex(l->color));

    } 
    game_advance_depth(g_game);
  }
  game_set_blend_alpha(g_game);
}



static void
lit_draw_light_rays(lit_game_t* g) {

  for(u32_t light_index = 0; light_index < g->light_count; ++light_index) 
  {
    lit_game_light_t* light = g->lights + light_index;
    for_cnt(intersection_index, light->intersection_count){
      lit_light_intersection_t* intersection = light->intersections + intersection_index;
      game_draw_line(g_game, light->pos, intersection->pt, 2.f, rgba_hex(0xFF0000FF));

    }

  }
  game_advance_depth(g_game);
}

static void
lit_draw_edges(lit_game_t* g) {

  for(u32_t edge_index = 0; edge_index < g->edge_count; ++edge_index) 
  {
    lit_game_edge_t* edge = g->edges + edge_index;
    game_draw_line(g_game, edge->start_pt, edge->end_pt, 3.f, rgba_hex(0x888888FF));
  }
  game_advance_depth(g_game);
}

//
// Player
//
static void
lit_init_player(lit_game_t* g, f32_t x, f32_t y) {
  lit_game_player_t* player = &g->player; 
  player->held_light = nullptr;
  player->pos.x = x;
  player->pos.y = y;
  player->light_hold_mode = LIT_PLAYER_LIGHT_HOLD_MODE_NONE;
}

static void
lit_game_player_release_light(lit_game_t* g) {
  lit_game_player_t* player = &g->player;
  player->held_light = nullptr;
  player->light_hold_mode = LIT_PLAYER_LIGHT_HOLD_MODE_NONE;
  game_show_cursor(g_game);
}

static void
lit_game_player_find_nearest_light(lit_game_t* g) {
  lit_game_player_t* player = &g->player;
  f32_t shortest_dist = LIT_PLAYER_PICKUP_DIST; // limit
  player->nearest_light = nullptr;

  for(u32_t light_index = 0; light_index < g->light_count; ++light_index) {
    lit_game_light_t* l = g->lights +light_index;
    f32_t dist = v2f_dist_sq(l->pos, player->pos);
    if (shortest_dist > dist) {
      player->nearest_light = l;
      shortest_dist = dist;
    }
  }
}


static void
lit_game_player_hold_nearest_light_if_empty_handed(
    lit_game_t* g, 
    lit_game_player_light_hold_mode_t light_hold_mode) 
{
  lit_game_player_t* player = &g->player;

  if (player->light_hold_mode == LIT_PLAYER_LIGHT_HOLD_MODE_NONE) {
    if (player->nearest_light) {          
      player->held_light = player->nearest_light;
      player->old_light_pos = player->nearest_light->pos;
      player->light_retrival_time = 0.f;
      player->light_hold_mode = light_hold_mode;
      game_hide_cursor(g_game);
    }
  }
}

static void 
lit_game_update_player(lit_game_t* g, f32_t dt) 
{
  lit_game_player_t* player = &g->player; 

  player->pos.x = g_game->input.mouse_pos.x;
  player->pos.y = LIT_HEIGHT - g_game->input.mouse_pos.y;

  lit_game_player_find_nearest_light(g);

  //
  // Move light logic
  //
  if (game_is_button_poked(g_game, GAME_BUTTON_CODE_LMB)) {
    lit_game_player_hold_nearest_light_if_empty_handed(g, LIT_PLAYER_LIGHT_HOLD_MODE_MOVE);
  }

  else if (game_is_button_released(g_game, GAME_BUTTON_CODE_LMB))
  {
    lit_game_player_release_light(g);
  }

  //
  // Rotate light logic
  //
  if (game_is_button_poked(g_game, GAME_BUTTON_CODE_RMB))
  {
    lit_game_player_hold_nearest_light_if_empty_handed(g, LIT_PLAYER_LIGHT_HOLD_MODE_ROTATE);
    game_lock_cursor(g_game);
    player->locked_pos_x = player->pos.x;

  }
  else if (game_is_button_released(g_game, GAME_BUTTON_CODE_RMB)) 
  {
    lit_game_player_release_light(g);
    game_unlock_cursor(g_game);
  }

  // Restrict movement
  if (player->pos.x > LIT_WIDTH - LIT_PLAYER_RADIUS) {
    player->pos.x = LIT_WIDTH - LIT_PLAYER_RADIUS;
  }
  if (player->pos.x < LIT_PLAYER_RADIUS) {
    player->pos.x = LIT_PLAYER_RADIUS;
  }
  if (player->pos.y > LIT_HEIGHT - LIT_PLAYER_RADIUS) {
    player->pos.y = LIT_HEIGHT - LIT_PLAYER_RADIUS;
  }
  if (player->pos.y < LIT_PLAYER_RADIUS) {
    player->pos.y = LIT_PLAYER_RADIUS;
  }

  //
  // Actual player control logic
  //
  switch(player->light_hold_mode) {
    case LIT_PLAYER_LIGHT_HOLD_MODE_NONE: {
      // do nothing
    } break;
    case LIT_PLAYER_LIGHT_HOLD_MODE_MOVE: {
      // Move the held light to player's position
      if (player->held_light) {
        if (player->light_retrival_time < LIT_PLAYER_LIGHT_RETRIEVE_DURATION) {
          player->light_retrival_time += dt;
        }
        else {
          player->light_retrival_time = LIT_PLAYER_LIGHT_RETRIEVE_DURATION;
        }
        f32_t ratio = player->light_retrival_time / LIT_PLAYER_LIGHT_RETRIEVE_DURATION; 
        player->held_light->pos.x = f32_lerp(player->old_light_pos.x, player->pos.x, ratio) ;
        player->held_light->pos.y = f32_lerp(player->old_light_pos.y, player->pos.y,  ratio) ;
      }
    } break;
    case LIT_PLAYER_LIGHT_HOLD_MODE_ROTATE: {
      if (player->held_light != nullptr) {
        f32_t mouse_delta = player->pos.x - player->locked_pos_x;
        player->held_light->dir = 
          v2f_rotate(player->held_light->dir, LIT_PLAYER_ROTATE_SPEED * dt * mouse_delta );
      }
    } break;
  }
}

static void
lit_game_render_player(lit_game_t* g){
  lit_game_player_t* player = &g->player;


  if (player->light_hold_mode == LIT_PLAYER_LIGHT_HOLD_MODE_NONE) { 
    if (player->nearest_light) {
      game_draw_asset_sprite(g_game, &g_lit->assets,
          ASSET_SPRITE_ID_CIRCLE_SPRITE,
          player->nearest_light->pos, 
          v2f_set(LIT_PLAYER_RADIUS*2, LIT_PLAYER_RADIUS*2));
      game_advance_depth(g_game);
    }
  }
  else if (player->light_hold_mode == LIT_PLAYER_LIGHT_HOLD_MODE_ROTATE) {
    game_draw_asset_sprite(g_game, &g_lit->assets,
        ASSET_SPRITE_ID_ROTATE_SPRITE,
        player->held_light->pos, 
        v2f_set(LIT_PLAYER_RADIUS*2, LIT_PLAYER_RADIUS*2));
    game_advance_depth(g_game);

  }
  else if (player->light_hold_mode == LIT_PLAYER_LIGHT_HOLD_MODE_MOVE) {
    game_draw_asset_sprite(g_game, &g_lit->assets,
        ASSET_SPRITE_ID_MOVE_SPRITE,
        player->held_light->pos, 
        v2f_set(LIT_PLAYER_RADIUS*2, LIT_PLAYER_RADIUS*2));
    game_advance_depth(g_game);


  }
}


//
// Particles
//
static void
lit_spawn_particle(
    lit_game_t* g,
    f32_t lifespan,
    v2f_t pos, v2f_t vel,
    rgba_t color_start,
    rgba_t color_end,
    v2f_t size_start,
    v2f_t size_end) 
{
  lit_particle_pool_t* ps = &g->particles;
  if (ps->particle_count < array_count(ps->particles)) {
    lit_particle_t* p = ps->particles + ps->particle_count++; 
    p->pos = pos;
    p->vel = vel;
    p->color_start = color_start;
    p->color_end = color_end;
    p->lifespan = p->lifespan_now = lifespan;
    p->size_start = size_start;
    p->size_end = size_end;    
  }
}

static void
lit_game_update_particles(lit_game_t* g, f32_t dt) {
  lit_particle_pool_t* ps = &g->particles;
  for(u32_t particle_id = 0; 
      particle_id < ps->particle_count; ) 
  {
    lit_particle_t* p = ps->particles + particle_id;
    if (p->lifespan_now <= 0.f) {
      // Copy last item to current item
      (*p) = ps->particles[ps->particle_count-- - 1]; 

    }
    else { 
      p->lifespan_now -= dt;
      p->pos += p->vel * dt;
      ++particle_id;
    }

  }
}
static void
lit_game_render_particles(lit_game_t* g) {
  lit_particle_pool_t* ps = &g->particles;

  // Render particles
  for(u32_t particle_id = 0; 
      particle_id < ps->particle_count;
      ++particle_id) 
  {
    lit_particle_t* p = ps->particles + particle_id;

    f32_t lifespan_ratio = 1.f -  p->lifespan_now / p->lifespan;

    rgba_t color = {0};
    color.r = f32_lerp(p->color_start.r, p->color_end.r, lifespan_ratio);  
    color.g = f32_lerp(p->color_start.g, p->color_end.g, lifespan_ratio);  
    color.b = f32_lerp(p->color_start.b, p->color_end.b, lifespan_ratio);  
    color.a = f32_lerp(p->color_start.a, p->color_end.a, lifespan_ratio);  

    v2f_t size = {0};
    size.w = f32_lerp(p->size_start.w , p->size_end.w, lifespan_ratio);
    size.h = f32_lerp(p->size_start.h , p->size_end.h, lifespan_ratio);

    game_draw_asset_sprite(g_game, &g_lit->assets, ASSET_SPRITE_ID_FILLED_CIRCLE_SPRITE, p->pos, size, color);
    game_advance_depth(g_game);
  }
}


//
// Sensors
//
static lit_game_sensor_t* 
lit_game_push_sensor(lit_game_t* g, f32_t pos_x, f32_t pos_y, u32_t target_color) 
{
  assert(g->selected_sensor_group_id != array_count(g->sensor_groups));
  assert(g->sensor_count < array_count(g->sensors));
  lit_game_sensor_t* s = g->sensors + g->sensor_count++;
  s->pos.x = pos_x;
  s->pos.y = pos_y;
  s->target_color = target_color;
  s->current_color = 0;
  s->group_id = g->selected_sensor_group_id;

  g->sensor_groups[s->group_id].sensor_count++;

  return s;
}


static void
lit_game_push_rotating_sensor(
    lit_game_t* g, 
    f32_t pos_x, 
    f32_t pos_y, 
    v2f_t* origin,
    f32_t speed, 
    u32_t target_color)
{
  auto* sensor = lit_game_push_sensor(g, pos_x, pos_y, target_color);
  lit_game_animator_push_rotate_point(g, &sensor->pos, origin, speed);

}



static void
lit_game_push_patrolling_sensor_waypoint(
    lit_game_t* g, 
    f32_t pos_x, 
    f32_t pos_y) 
{
  assert(g->selected_animator_for_sensor);
  lit_game_animator_push_patrol_point_waypoint(g->selected_animator_for_sensor, pos_x, pos_y);
}


static void
lit_game_begin_patrolling_sensor(lit_game_t* g, f32_t pos_x, f32_t pos_y, u32_t target_color, f32_t duration_per_waypoint) 
{
  assert(!g->selected_animator_for_sensor);

  auto* sensor = lit_game_push_sensor(g, pos_x, pos_y, target_color);
  g->selected_animator_for_sensor = 
    lit_game_animator_begin_patrol_point(g, duration_per_waypoint, &sensor->pos);

  //lit_game_push_patrolling_sensor_waypoint(g, pos_x, pos_y);

}

static void
lit_game_end_patrolling_sensor(lit_game_t* g) {

  assert(g->selected_animator_for_sensor);
  lit_game_animator_end_patrol_point(g->selected_animator_for_sensor);

  g->selected_animator_for_sensor = nullptr;
}

static void 
lit_game_begin_sensor_group(
    lit_game_t* m, 
    lit_game_exit_callback_f* callback)
{
  assert(m->sensor_group_count < array_count(m->sensor_groups));
  m->selected_sensor_group_id = m->sensor_group_count++;

  lit_game_sensor_group_t* group = m->sensor_groups + m->selected_sensor_group_id;
  group->callback = callback;
  group->sensor_count = 0;
}

static void 
lit_game_end_sensor_group(lit_game_t* m) {
  m->selected_sensor_group_id = array_count(m->sensor_groups);
}

static void 
lit_game_update_sensors(lit_game_t* g, f32_t dt) 
{
  lit_particle_pool_t* particles = &g->particles;
  rng_t* rng = &g->rng; 

  // This is an array of activated sensors per sensor_group
  u32_t* activated = arena_push_arr_zero(u32_t, &g_lit->frame_arena, g->sensor_group_count);

  u32_t total_triangles = 0;

  // Go through each sensor and update what lights are on it
  for(u32_t sensor_index = 0; sensor_index < g->sensor_count; ++sensor_index)
  {
    lit_game_sensor_t* sensor = g->sensors + sensor_index;
    u32_t current_color = 0x0000000;

    // For each light, for each triangle, add light
    for(u32_t light_index = 0; light_index < g->light_count; ++light_index)
    {
      lit_game_light_t* light = g->lights +light_index;

      lit_profile_begin(sensor_light_overlap);
      for(u32_t tri_index = 0; tri_index < light->triangle_count; ++tri_index)
      {
        lit_game_light_triangle_t* tri = light->triangles +tri_index;
        if (bonk_tri2_pt2(tri->p0, tri->p1, tri->p2, sensor->pos)) 
        {
          current_color += light->color >> 8 << 8; // ignore alpha
          break; // ignore the rest of the triangles
        }
      }
      lit_profile_end(sensor_light_overlap);

      total_triangles += light->triangle_count;
    }

    // Sensor color check
    sensor->current_color = current_color;
    if ((sensor->current_color & LIT_SENSOR_COLOR_MASK) == 
        (sensor->target_color & LIT_SENSOR_COLOR_MASK)) 
    {
      ++activated[sensor->group_id];
    }

    // Particle emission check
    sensor->particle_cd -= dt;
    if (sensor->particle_cd <= 0.f) 
    {
      sensor->particle_cd = LIT_SENSOR_PARTICLE_CD;
      v2f_t rand_dir = rng_unit_circle(rng);
      v2f_t particle_vel = v2f_scale(rand_dir, LIT_SENSOR_PARTICLE_SPEED); 
      rgba_t target_color = rgba_hex(sensor->target_color); 

      rgba_t start_color = target_color;
      start_color.a = 1.f;

      rgba_t end_color = target_color;
      end_color.a = 0.f;

      v2f_t size_start = v2f_set(LIT_SENSOR_PARTICLE_SIZE, LIT_SENSOR_PARTICLE_SIZE);
      v2f_t size_end = v2f_zero();

      lit_spawn_particle(
          g, 
          1.f,
          sensor->pos,
          particle_vel,
          start_color,
          end_color,
          size_start,
          size_end);
    }
  }

  // For each sensor group, we find which one is actually activated
  for_cnt(group_index, g->sensor_group_count) {
    lit_game_sensor_group_t* group = g->sensor_groups + group_index;
    if (group->sensor_count == activated[group_index]) {
      // We found a group that can activate the exit callback
      group->callback();
    }
  }

  game_inspect_u32(g_game, st8_from_lit("total_triangles"), total_triangles);
}


static void 
lit_game_render_sensors(lit_game_t* g) {

  v2f_t size = v2f_set(LIT_SENSOR_RADIUS,LIT_SENSOR_RADIUS)*2;
  for(u32_t sensor_index = 0; sensor_index < g->sensor_count; ++sensor_index)
  {
    lit_game_sensor_t* sensor = g->sensors + sensor_index;
    game_draw_asset_sprite(g_game, &g_lit->assets, ASSET_SPRITE_ID_FILLED_CIRCLE_SPRITE, sensor->pos, size, rgba_hex(sensor->target_color));
    game_advance_depth(g_game);
  }
}

static void
lit_game_generate_light(lit_game_t* g) {
  for(u32_t light_index = 0; light_index < g->light_count; ++light_index)
  {
    lit_game_light_t* light = g->lights + light_index;
    lit_gen_light_intersections(light, g->edges, g->edge_count, &g_lit->frame_arena);

#if LIT_DEBUG_INTERSECTIONS
    // Generate debug lines
    for (u32_t intersection_index = 0;
        intersection_index < light->intersection_count;
        ++intersection_index)
    {
      v2f_t p0 = light->pos;
      v2f_t p1 = light->intersections[intersection_index].pt;
      game_draw_line(g_game, p0, p1, 1.f, rgba_hex(0xFFFFFFFF));
    }
#endif
  }
}

//
// Levels
// 


static void lit_level_menu();
static void lit_level_obstruct();
static void lit_level_add();


#define lit_level_exit_with(where) [](){ \
  lit_unlock_next_level(g_lit->g_game.current_level_id); \
  g_lit->g_game.exit_callback = where;  \
} 

static void
lit_game_init_level(lit_game_t* m, st8_t str, u32_t level_id) {
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
  m->exit_fade = 0.f;

  m->selected_sensor_group_id = array_count(m->sensor_groups);
  m->selected_animator_for_sensor = nullptr; 
  m->selected_animator_for_double_edge_min[0] = nullptr; 
  m->selected_animator_for_double_edge_min[1] = nullptr;
  m->selected_animator_for_double_edge_max[0] = nullptr; 
  m->selected_animator_for_double_edge_max[1] = nullptr;

  m->exit_callback = nullptr;

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
lit_level_move() {
  lit_game_t* m = &g_lit->g_game;
  lit_game_init_level(m, st8_from_lit("MOVE"), 1);

  lit_game_push_light(m, 400.f, 400, 0x880000FF, 45.f, 0.75f);

  // If 'tutorial' completed, to go menu, else go to 'obstruct'
  if (lit_is_in_tutorial()) {
    lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_obstruct));
  }
  else {
    lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_menu));
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
lit_level_obstruct() {
  lit_game_t* m = &g_lit->g_game;
  lit_game_init_level(m, st8_from_lit("OBSTRUCT"), 2);
  
  if (lit_is_in_tutorial()) {
    lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_add));
  }
  else {
    lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_menu));
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
lit_level_add() { 
  lit_game_t* m = &g_lit->g_game;
  // Need to 'enclose' the shape
  lit_game_init_level(m, st8_from_lit("ADD"), 3);
  lit_game_push_double_edge(m, 100.f, 400.f, 700.f, 400.f);

  lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_menu));
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
lit_level_corners() { 
  lit_game_t* m = &g_lit->g_game;
  lit_game_init_level(m, st8_from_lit("CORNERS"), 4);

  // Need to 'enclose' the shape
  lit_game_push_double_edge(m, 500.f, 200.f, 600.f, 300.f);
  lit_game_push_double_edge(m, 300.f, 200.f, 200.f, 300.f);
  lit_game_push_double_edge(m, 200.f, 500.f, 300.f, 600.f);
  lit_game_push_double_edge(m, 500.f, 600.f, 600.f, 500.f);

  lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_menu));
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
lit_level_mix() {
  lit_game_t* m = &g_lit->g_game;

  lit_game_init_level(m, st8_from_lit("MIX"), 5);

  lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_menu));
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
lit_level_blend() {
  lit_game_t* m = &g_lit->g_game;
  lit_game_init_level(m, st8_from_lit("BLEND"), 6);

  lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_menu));
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
lit_level_rooms() {
  lit_game_t* m = &g_lit->g_game;

  lit_game_init_level(m, st8_from_lit("ROOMS"), 7);

  // middle
  lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_menu));
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
lit_level_disco() {
  lit_game_t* m = &g_lit->g_game;
  lit_game_init_level(m, st8_from_lit("DISCO"), 8);

  lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_menu));
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
lit_level_onion() {
  lit_game_t* m = &g_lit->g_game;
  lit_game_init_level(m, st8_from_lit("ONION"), 9);

  lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_menu));
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
lit_level_spectrum() {
  lit_game_t* m = &g_lit->g_game;
  lit_game_init_level(m, st8_from_lit("SPECTRUM"), 10);

  lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_menu));
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
lit_level_split() {
  lit_game_t* m = &g_lit->g_game;
  lit_game_init_level(m, st8_from_lit("SPLIT"), 11);

  lit_game_push_light(m, 400.f, 410.f, 0x880000FF, 360.f, 0.f);
  lit_game_push_light(m, 390.f, 390.f, 0x008800FF, 360.f, 0.f);
  lit_game_push_light(m, 410.f, 390.f, 0x000088FF, 360.f, 0.f);

  lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_menu));

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
lit_level_movement() {
  lit_game_t* m = &g_lit->g_game;
  lit_game_init_level(m, st8_from_lit("MOVEMENT"), 13);

//  lit_game_push_light(m, 400.f, 100.f, 0x880000FF, 15.f, 0.25f);
  lit_game_push_light(m, 400.f, 700.f, 0x008800FF, 15.f, 0.25f);
  lit_game_push_light(m, 150.f, 700.f, 0x880000FF, 15.f, 0.25f);
  lit_game_push_light(m, 600.f, 700.f, 0x000088FF, 15.f, 0.25f);

  f32_t duration = 5.25f;

  lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_menu));
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

// LEVEL 13
static void
lit_level_interval() {
  lit_game_t* m = &g_lit->g_game;
  lit_game_init_level(m, st8_from_lit("INTERVAL"), 13);

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

  lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_menu));
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
lit_level_patience() {
  lit_game_t* m = &g_lit->g_game;
  lit_game_init_level(m, st8_from_lit("PATIENCE"), 14);
  lit_game_push_light(m, 400.f, 100.f, 0x880000FF, 15.f, 0.25f);
  lit_game_push_light(m, 400.f, 700.f, 0x008800FF, 15.f, 0.75f);

  lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_menu));
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
lit_level_busy() {
  lit_game_t* m = &g_lit->g_game;
  lit_game_init_level(m, st8_from_lit("SCRAMBLE"), 15);
  //lit_game_push_light(m, 400.f, 100.f, 0x880000FF, 15.f, 0.25f);
#if 1
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
 
  lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_menu));
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
lit_level_spin() {
  lit_game_t* m = &g_lit->g_game;
  lit_game_init_level(m, st8_from_lit("SPIN"), 16);

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

  lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_menu));
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
lit_level_orbit() {
  lit_game_t* m = &g_lit->g_game;
  lit_game_init_level(m, st8_from_lit("ORBIT"), 17);

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

  lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_menu));
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
static void
lit_level_hourglass() {
  lit_game_t* m = &g_lit->g_game;
  lit_game_init_level(m, st8_from_lit("HOURGLASS"), 18);

  lit_game_push_light(m, 250, 400, 0x880000FF, 360.f, 0.75f);
  lit_game_push_light(m, 550, 400, 0x008800FF, 360.f, 0.75f);
  lit_game_push_light(m, 400, 250, 0x000088FF, 360.f, 0.75f);
  lit_game_push_light(m, 400, 550, 0x444444FF, 360.f, 0.75f);

  lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_menu));
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
  lit_game_init_level(m, st8_from_lit("TEST"), 1);

  //lit_game_push_light(m, 250, 400, 0x880000FF, 360.f, 0.75f);
  //lit_game_push_light(m, 550, 400, 0x008800FF, 360.f, 0.75f);
  //lit_game_push_light(m, 400, 250, 0x000088FF, 360.f, 0.75f);
  lit_game_push_light(m, 400, 550, 0x444444FF, 360.f, 0.75f);

  lit_game_begin_sensor_group(m, lit_level_menu);
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
lit_level_triforce() {
  lit_game_t* m = &g_lit->g_game;
  lit_game_init_level(m, st8_from_lit("TRIFORCE"), 19);

  lit_game_push_light(m, 400, 427, 0x880000FF, 360.f, 0.75f);
  lit_game_push_light(m, 380, 380, 0x008800FF, 360.f, 0.75f);
  lit_game_push_light(m, 420, 380, 0x000088FF, 360.f, 0.75f);

  lit_game_push_light(m, 400, 427, 0x880000FF, 360.f, 0.75f);
  lit_game_push_light(m, 380, 380, 0x008800FF, 360.f, 0.75f);
  lit_game_push_light(m, 420, 380, 0x000088FF, 360.f, 0.75f);

  lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_menu));
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
lit_level_hands() {
  lit_game_t* m = &g_lit->g_game;
  lit_game_init_level(m, st8_from_lit("HANDS"), 20);

  lit_game_push_light(m, 100, 100, 0x880000FF, 360.f, 0.75f);
  lit_game_push_light(m, 100, 400, 0x008800FF, 360.f, 0.75f);
  lit_game_push_light(m, 100, 700, 0x000088FF, 360.f, 0.75f);
  lit_game_push_light(m, 700, 100, 0x880000FF, 360.f, 0.75f);
  lit_game_push_light(m, 700, 400, 0x008800FF, 360.f, 0.75f);
  lit_game_push_light(m, 700, 700, 0x000088FF, 360.f, 0.75f);


  lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_menu));
  lit_game_push_sensor(m, 750.f, 750.f, 0x880088FF); 
  lit_game_push_sensor(m, 50.f, 50.f, 0x880088FF); 
  lit_game_push_sensor(m, 750.f, 400.f, 0x888800FF); 
  lit_game_push_sensor(m, 50.f, 400.f, 0x888800FF); 
  lit_game_push_sensor(m, 750.f, 50.f, 0x008888FF); 
  lit_game_push_sensor(m, 50.f, 750.f, 0x008888FF); 
  lit_game_end_sensor_group(m);

  v2f_t* rotate_pt = lit_game_push_point(m, {400.f, 400.f});
  lit_game_push_rotating_double_edge(
      m, 
      400.f, 400.f, 400.f, 600.f,
      nullptr,
      rotate_pt, 
      0.f,
      0.5f);

  lit_game_push_rotating_double_edge(
      m, 
      400.f, 400.f, 400.f, 600.f,
      nullptr,
      rotate_pt, 
      0.f,
      1.f);
  lit_game_push_rotating_double_edge(
      m, 
      400.f, 400.f, 400.f, 600.f,
      nullptr,
      rotate_pt, 
      0.f,
      1.5f);


  // 
  lit_game_push_rotating_double_edge(
      m, 
      400.f, 400.f, 400.f, 600.f,
      nullptr,
      rotate_pt, 
      0.f,
      -0.5f);
  lit_game_push_rotating_double_edge(
      m, 
      400.f, 400.f, 400.f, 600.f,
      nullptr,
      rotate_pt, 
      0.f,
      -1.f);
  lit_game_push_rotating_double_edge(
      m, 
      400.f, 400.f, 400.f, 600.f,
      nullptr,
      rotate_pt, 
      0.f,
      -1.5f);
}
//
// Menu Level
//

static void 
lit_level_menu() {
  lit_game_t* m = &g_lit->g_game;

  const f32_t box_hw = 50.f;
  const f32_t box_hh = 50.f;
  f32_t cx, cy;
  u32_t grey = 0x888888FF;
  f32_t row_y_offset = 150.f;

  lit_game_init_level(m, st8_from_lit("HOME"), 0);

  //
  // 1st row
  //
  cy = 700;
  cx = 100;
  lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
  lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_move));
  lit_game_push_sensor(m, cx, cy, grey); 
  lit_game_end_sensor_group(m);

  if (lit_get_levels_unlocked_count() >= 2) {
    cx = 250;
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_obstruct));
    lit_game_push_sensor(m, cx+25, cy, grey); 
    lit_game_push_sensor(m, cx-25, cy, grey); 
    lit_game_end_sensor_group(m);
  }


  if (lit_get_levels_unlocked_count() >= 3) {
    cx = 400;
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_add));
    lit_game_push_sensor(m, cx+25, cy, grey); 
    lit_game_push_sensor(m, cx,    cy, grey); 
    lit_game_push_sensor(m, cx-25, cy, grey); 
    lit_game_end_sensor_group(m);
  }

  if (lit_get_levels_unlocked_count() >= 4) {
    cx = 550;
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_corners));
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
    lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_mix));
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
  u32_t row2_color = 0x884444FF;
  cy -= row_y_offset;
  if (lit_get_levels_unlocked_count() >= 6) {
    cx = 100;
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_blend));
    lit_game_push_sensor(m, cx, cy, row2_color); 
    lit_game_end_sensor_group(m);
  }

  if (lit_get_levels_unlocked_count() >= 7) {
    cx = 250;
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_rooms));
    lit_game_push_sensor(m, cx+25, cy+25, row2_color); 
    lit_game_push_sensor(m, cx-25, cy-25, row2_color); 
    lit_game_end_sensor_group(m);
  }

  if (lit_get_levels_unlocked_count() >= 8 ) {
    cx = 400;
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_disco));
    lit_game_push_sensor(m, cx-25, cy+25, row2_color); 
    lit_game_push_sensor(m, cx, cy, row2_color); 
    lit_game_push_sensor(m, cx+25, cy-25, row2_color); 
    lit_game_end_sensor_group(m);
  }


  if (lit_get_levels_unlocked_count() >= 9) {
    cx = 550;
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_onion));
    lit_game_push_sensor(m, cx, cy+35, row2_color); 
    lit_game_push_sensor(m, cx-35, cy, row2_color); 
    lit_game_push_sensor(m, cx, cy-35, row2_color); 
    lit_game_push_sensor(m, cx+35, cy, row2_color); 
    lit_game_end_sensor_group(m);
  }

  if (lit_get_levels_unlocked_count() >= 10) {
    cx = 700;
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_spectrum));
    lit_game_push_sensor(m, cx-25, cy+15, row2_color); 
    lit_game_push_sensor(m, cx+25, cy+15, row2_color); 
    lit_game_push_sensor(m, cx, cy+30, row2_color); 
    lit_game_push_sensor(m, cx+20, cy-15, row2_color); 
    lit_game_push_sensor(m, cx-20, cy-15, row2_color); 
    lit_game_end_sensor_group(m);
  }

  //
  // 3rd row
  //
  u32_t row3_color = 0x448844FF;
  cy -= row_y_offset;
  if (lit_get_levels_unlocked_count() >= 11) {
    cx = 100;
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_split));
    lit_game_begin_patrolling_sensor(m, cx-25, cy, row3_color, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx+25, cy);
    lit_game_end_patrolling_sensor(m);
    lit_game_end_sensor_group(m);
  }

  if (lit_get_levels_unlocked_count() >= 12) {
    cx = 250;
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_movement));

    lit_game_begin_patrolling_sensor(m, cx-25, cy+25, row3_color, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx+25, cy-25);
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, cx+25, cy-25, row3_color, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx-25, cy+25);
    lit_game_end_patrolling_sensor(m);

    lit_game_end_sensor_group(m);
  }

  if (lit_get_levels_unlocked_count() >= 13) {
    cx = 400;
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_interval));

    lit_game_begin_patrolling_sensor(m, cx, cy+25, row3_color, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx+25, cy-25);
    lit_game_push_patrolling_sensor_waypoint(m, cx-25, cy-25);
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, cx+25, cy-25, row3_color, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx-25, cy-25);
    lit_game_push_patrolling_sensor_waypoint(m, cx, cy+25);
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, cx-25, cy-25, row3_color, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx, cy+25);
    lit_game_push_patrolling_sensor_waypoint(m, cx+25, cy-25);
    lit_game_end_patrolling_sensor(m);


    lit_game_end_sensor_group(m);
  }

  if (lit_get_levels_unlocked_count() >= 14) {
    cx = 550;
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_patience));

    lit_game_begin_patrolling_sensor(m, cx-30, cy, row3_color, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx+30, cy);
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, cx+30, cy, row3_color, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx-30, cy);
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, cx, cy-30, row3_color, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx, cy+30);
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, cx, cy+30, row3_color, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx, cy-30);
    lit_game_end_patrolling_sensor(m);


    lit_game_end_sensor_group(m);
  }

  if (lit_get_levels_unlocked_count() >= 14) {
    cx = 550;
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_busy));

    lit_game_begin_patrolling_sensor(m, cx-30, cy, row3_color, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx+30, cy);
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, cx+30, cy, row3_color, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx-30, cy);
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, cx, cy-30, row3_color, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx, cy+30);
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, cx, cy+30, row3_color, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx, cy-30);
    lit_game_end_patrolling_sensor(m);


    lit_game_end_sensor_group(m);
  }

  if (lit_get_levels_unlocked_count() >= 15) {
    cx = 700;
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_busy));

    lit_game_begin_patrolling_sensor(m, cx-30, cy+30, row3_color, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx+30, cy-30);
    lit_game_end_patrolling_sensor(m);

    lit_game_begin_patrolling_sensor(m, cx+30, cy+30, row3_color, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx-30, cy-30);
    lit_game_end_patrolling_sensor(m);

    lit_game_push_sensor(m, cx, cy, row3_color);

    lit_game_begin_patrolling_sensor(m, cx-30, cy-30, row3_color, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx+30, cy+30);
    lit_game_end_patrolling_sensor(m);


    lit_game_begin_patrolling_sensor(m, cx+30, cy-30, row3_color, 2.f); 
    lit_game_push_patrolling_sensor_waypoint(m, cx-30, cy+30);
    lit_game_end_patrolling_sensor(m);


    lit_game_end_sensor_group(m);
  }

  // row 4
  u32_t row4_color = 0x888844FF;
  f32_t rotating_speed = 2.f;
  cy -= row_y_offset;
  if (lit_get_levels_unlocked_count() >= 16) {
    cx = 100;
    v2f_t* o = lit_game_push_point(m, v2f_set(cx, cy));
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_spin));
    lit_game_push_rotating_sensor(m, cx,  cy-25, o, -rotating_speed, row4_color); 
    lit_game_end_sensor_group(m);
  }


  if (lit_get_levels_unlocked_count() >= 17) {
    cx = 250;
    v2f_t* o = lit_game_push_point(m, v2f_set(cx, cy));
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_orbit));
    
    lit_game_push_rotating_sensor(m, cx,  cy+25, o, rotating_speed, row4_color); 
    lit_game_push_rotating_sensor(m, cx,  cy-25, o, rotating_speed, row4_color); 

    lit_game_end_sensor_group(m);
  }

  if (lit_get_levels_unlocked_count() >= 18) {
    cx = 400;
    v2f_t* o = lit_game_push_point(m, v2f_set(cx, cy));
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_hourglass));
 
    lit_game_push_rotating_sensor(m, cx,  cy+25, o,    -rotating_speed, row4_color); 
    lit_game_push_rotating_sensor(m, cx+25,  cy-25, o, -rotating_speed, row4_color); 
    lit_game_push_rotating_sensor(m, cx-25,  cy-25, o, -rotating_speed, row4_color); 

    lit_game_end_sensor_group(m);
  }

  if (lit_get_levels_unlocked_count() >= 19) {
    cx = 550;
 
    v2f_t* o = lit_game_push_point(m, v2f_set(cx, cy));
    
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);
    lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_triforce));
    lit_game_push_rotating_sensor(m, cx+25,  cy-25, o, rotating_speed, row4_color); 
    lit_game_push_rotating_sensor(m, cx-25,  cy+25, o, rotating_speed, row4_color); 
    lit_game_push_rotating_sensor(m, cx+25,  cy+25, o, rotating_speed, row4_color); 
    lit_game_push_rotating_sensor(m, cx-25,  cy-25, o, rotating_speed, row4_color); 


    lit_game_end_sensor_group(m);
  }

  if (lit_get_levels_unlocked_count() >= 20) {
    cx = 700;
 
    v2f_t* o = lit_game_push_point(m, v2f_set(cx, cy));
    
    lit_game_push_aabb(m, cx, cy, box_hw, box_hh);

    lit_game_begin_sensor_group(m, lit_level_exit_with(lit_level_hands));
    lit_game_push_rotating_sensor(m, cx+25,  cy-25, o, rotating_speed, row4_color); 
    lit_game_push_rotating_sensor(m, cx-25,  cy+25, o, rotating_speed, row4_color); 

    lit_game_push_sensor(m, cx,  cy,  row4_color); 
    lit_game_push_rotating_sensor(m, cx+25,  cy+25, o, rotating_speed, row4_color); 
    lit_game_push_rotating_sensor(m, cx-25,  cy-25, o, rotating_speed, row4_color); 


    lit_game_end_sensor_group(m);
  }
  cy -= row_y_offset;

  // 5th row
  u32_t row5_color = 0xCCCC88FF;
  if (lit_get_levels_unlocked_count() >= 21) {
    cx = 400;
    lit_game_push_aabb(m, cx, cy, 350.f, box_hh);
    v2f_t* o = lit_game_push_point(m, v2f_set(cx, cy));

    lit_game_begin_sensor_group(m, lit_level_exit_with(lit_goto_credits));
    lit_game_push_sensor(m, cx,  cy,  row5_color); 
    lit_game_end_sensor_group(m);
  }


  // white lights
  if (lit_get_levels_unlocked_count() > LIT_LEARNT_POINT_LIGHT_LEVEL_ID)  {
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
    if (lit_get_levels_unlocked_count() > LIT_LEARNT_POINT_LIGHT_LEVEL_ID) {
      lit_game_push_light(m, 775.f, 775.f, 0x440000FF, 360.f, 0.f);
    }
    else { // player has not learnt about point lights
      lit_game_push_light(m, 775.f, 775.f, 0x440000FF, 90.f, 5/8.f);
    }
  }

  // green light
  if (lit_get_levels_unlocked_count() > 10) {
    lit_game_push_light(m, 25.f, 25.f, 0x004400FF, 360.f, 0.f);
  }
}


// 
// Main g_game functions
//
static void
lit_game_update() 
{
  lit_game_t* g = &g_lit->g_game;
  lit_game_player_t* player = &g->player;

  if (game_is_button_poked(g_game, GAME_BUTTON_CODE_SPACE)) {
    g->freeze = !g->freeze;
  }
  
  f32_t dt = game_get_dt(g_game);
  
  //
  // Transition Logic
  //
  lit_profile_begin(transition);
  if (g->state == LIT_GAME_STATE_TYPE_TRANSITION_IN) 
  {
    // Title 
    if (g->title_wp_index < array_count(lit_title_wps)-1) 
    {
      g->title_timer += dt;
      lit_game_title_waypoint_t* next_wp = lit_title_wps + g->title_wp_index+1;
      if (g->title_timer >= next_wp->arrival_time) 
      {
        g->title_wp_index++;
      }
    }
    if (g->stage_fade_timer >= 0.f) 
    {
      g->stage_fade_timer -= dt;
    }
    else 
    {
      g->stage_fade_timer = 0.f;
      g->state = LIT_GAME_STATE_TYPE_NORMAL;
    }
  }

  else if (g->state == LIT_GAME_STATE_TYPE_SOLVED_IN) {
    g->stage_flash_timer += dt;
    if (g->stage_flash_timer >= LIT_EXIT_FLASH_DURATION) {
      g->stage_flash_timer = LIT_EXIT_FLASH_DURATION;
      g->state = LIT_GAME_STATE_TYPE_SOLVED_OUT;
    }
  }
  else if (g->state == LIT_GAME_STATE_TYPE_SOLVED_OUT) {
    g->stage_flash_timer -= dt;
    if (g->stage_flash_timer <= 0.f) {
      g->stage_flash_timer = 0.f;
      g->state = LIT_GAME_STATE_TYPE_TRANSITION_OUT;
    }
  }
  else if (g->state == LIT_GAME_STATE_TYPE_TRANSITION_OUT) {
    if (g->stage_fade_timer <= 1.f) {
      g->stage_fade_timer += dt;
    }
    else {
      g->exit_callback();
      return;
    }
  }
  lit_profile_end(transition);

  //
  // What to update based on state
  //
  if (g->state == LIT_GAME_STATE_TYPE_NORMAL) 
  {
    lit_profile_begin(animate);
    if (!g->freeze) {
      lit_game_animate_everything(g, dt);
    }
    lit_profile_end(animate);

    // Do input for exiting to HOME if not HOME
    if (g->current_level_id > 0) {
      if (g_game->input.mouse_scroll_delta < 0)
        g->exit_fade = min_of(1.f, g->exit_fade + 0.1f);
      else if (g_game->input.mouse_scroll_delta > 0) 
        g->exit_fade = max_of(0.f, g->exit_fade - 0.1f);
      if (g->exit_fade >= 1.f) {
        // go back to HOME
        lit_level_menu();
        return;
      }
    }


    lit_game_update_player(g, dt);
  }

  lit_profile_begin(lights);
  lit_game_generate_light(g);
  lit_profile_end(lights);

  if (!lit_game_is_exiting(g)) 
  {
    lit_profile_begin(sensors);
    lit_game_update_sensors(g, dt);
    lit_profile_end(sensors);

    lit_profile_begin(particles);
    lit_game_update_particles(g, dt);
    lit_profile_end(particles);

    //
    // win condition
    //
    if (g->exit_callback != nullptr) 
    {
      game_show_cursor(g_game);
      game_unlock_cursor(g_game);
      g->state = LIT_GAME_STATE_TYPE_SOLVED_IN;
    }
  }


  //
  // RENDERING
  //

  // This is the default and hgameier blend mode
  game_set_blend_alpha(g_game);


  lit_profile_begin(rendering);

#if LIT_DEBUG_LINES
  lit_draw_edges(g); 
  lit_draw_light_rays(g);
#endif // LIT_DEBUG_LINES
  if (g->state == LIT_GAME_STATE_TYPE_NORMAL) {
    lit_game_render_player(g);
  }
  lit_game_render_lights(g);

  game_set_blend_alpha(g_game);

  if (!lit_game_is_exiting(g)) {
    lit_game_render_sensors(g); 
    lit_game_render_particles(g);
  }

  // Draw the overlay for fade in/out
  {
    rgba_t color = rgba_set(0.f, 0.f, 0.f, g->stage_fade_timer);
    game_draw_asset_sprite(g_game, &g_lit->assets, ASSET_SPRITE_ID_BLANK_SPRITE, v2f_set(LIT_WIDTH/2, LIT_HEIGHT/2), v2f_set(LIT_WIDTH, LIT_HEIGHT), color);
    game_advance_depth(g_game);
  }

  // Overlay for pause fade
  {
    rgba_t color = rgba_set(0.f, 0.f, 0.f, g->exit_fade);
    game_draw_asset_sprite(g_game, &g_lit->assets, ASSET_SPRITE_ID_BLANK_SPRITE, v2f_set(LIT_WIDTH/2, LIT_HEIGHT/2), v2f_set(LIT_WIDTH, LIT_HEIGHT), color);
    game_advance_depth(g_game);
  }

  // Draw the overlay for white flash
  {
    f32_t alpha = g->stage_flash_timer/LIT_EXIT_FLASH_DURATION * LIT_EXIT_FLASH_BRIGHTNESS;
    rgba_t color = rgba_set(1.f, 1.f, 1.f, alpha);
    game_draw_asset_sprite(g_game, &g_lit->assets, ASSET_SPRITE_ID_BLANK_SPRITE, v2f_set(LIT_WIDTH/2, LIT_HEIGHT/2), v2f_set(LIT_WIDTH, LIT_HEIGHT), color);
    game_advance_depth(g_game);
  }


  // Draw title
  if (g->title_wp_index < array_count(lit_title_wps)-1) 
  { 
    lit_game_title_waypoint_t* cur_wp = lit_title_wps + g->title_wp_index;
    lit_game_title_waypoint_t* next_wp = lit_title_wps + g->title_wp_index+1;

    f32_t duration = next_wp->arrival_time - cur_wp->arrival_time;
    f32_t timer = g->title_timer - cur_wp->arrival_time;
    f32_t a = f32_ease_linear(timer/duration); 
    f32_t title_x = cur_wp->x + a * (next_wp->x - cur_wp->x); 
    rgba_t color = rgba_set(1.f, 1.f, 1.f, 1.f);

    game_draw_text_center_aligned(g_game, &g_lit->assets, ASSET_FONT_ID_DEFAULT, g->title, color, title_x, LIT_HEIGHT/2, 128.f);
    game_advance_depth(g_game);

  }
  lit_profile_end(rendering);
}

static void 
lit_game_init() 
{
  lit_game_t* g = &g_lit->g_game;
  rng_init(&g->rng, 65535); // don't really need to be strict 

  g->freeze = false;

  // Go to level based on user's progress
  switch(lit_get_levels_unlocked_count())
  {
    case 1: lit_level_move(); break;
    case 2: lit_level_obstruct(); break;
    case 3: lit_level_add(); break;
    default: lit_level_menu();
    //default: lit_level_test(g);
  }
  
}


// 
// Credits Functions
//

static void 
lit_credits_init() {
  lit_credits_t* credits = &g_lit->credits;
  credits->timer = 0.f;
}

static f32_t 
lit_credits_push_subtitle_and_name(
    f32_t y, st8_t subtitle, st8_t name)
{
  lit_credits_t* credits = &g_lit->credits;
  game_draw_text_center_aligned(
      g_game, 
      &g_lit->assets, 
      ASSET_FONT_ID_DEFAULT,
      subtitle,
      rgba_set(0.5f, 0.5f, 0.5f, 1.f),
      LIT_WIDTH/2, y, 
      48.f);
  y -= 50.f;
  game_draw_text_center_aligned(
      g_game, 
      &g_lit->assets, 
      ASSET_FONT_ID_DEFAULT,
      name,
      rgba_hex(0xF8C8DCFF),
      LIT_WIDTH/2, y, 
      48.f);
  y -= 100.f;
  return y;
}

static void 
lit_credits_update() {
  lit_credits_t* credits = &g_lit->credits;

  credits->timer += game_get_dt(g_game);
  if (game_is_button_poked(g_game, GAME_BUTTON_CODE_LMB)) {
    g_lit->next_mode = LIT_MODE_GAME;
    return;
  }

  f32_t y = LIT_HEIGHT/2 + 96.f;

  if (credits->timer > LIT_CREDITS_START_COOLDOWN_DURATION) 
  {
    y -= -(credits->timer - LIT_CREDITS_START_COOLDOWN_DURATION) * LIT_CREDITS_SCROLL_SPEED;
  }


  //
  // RENDERING
  //
  
  // Title
  game_draw_text_center_aligned(
      g_game, 
      &g_lit->assets, 
      ASSET_FONT_ID_DEFAULT,
      st8_from_lit("THANKS"), 
      rgba_set(1.f, 1.f, 1.f, 1.f),
      LIT_WIDTH/2, y, 
      96.f);
  y -= 96.f;
  game_draw_text_center_aligned(
      g_game, 
      &g_lit->assets, 
      ASSET_FONT_ID_DEFAULT,
      st8_from_lit("FOR"), 
      rgba_set(1.f, 1.f, 1.f, 1.f),
      LIT_WIDTH/2, y, 
      96.f);
  y-=96.f;
  game_draw_text_center_aligned(
      g_game, 
      &g_lit->assets, 
      ASSET_FONT_ID_DEFAULT,
      st8_from_lit("PLAYING!"), 
      rgba_set(1.f, 1.f, 1.f, 1.f),
      LIT_WIDTH/2, y, 
      96.f);
  y -= LIT_HEIGHT - 100.f;


  game_draw_text_center_aligned(
      g_game, 
      &g_lit->assets, 
      ASSET_FONT_ID_DEFAULT,
      st8_from_lit("--------"), 
      rgba_set(1.f, 1.f, 1.f, 1.f),
      LIT_WIDTH/2, y, 
      128.f);

  y -= 70;
  game_draw_text_center_aligned(
      g_game, 
      &g_lit->assets, 
      ASSET_FONT_ID_DEFAULT,
      st8_from_lit("PRIMIX"), 
      rgba_set(1.f, 1.f, 1.f, 1.f),
      LIT_WIDTH/2, y, 
      128.f);

  y -= 70;
  game_draw_text_center_aligned(
      g_game, 
      &g_lit->assets, 
      ASSET_FONT_ID_DEFAULT,
      st8_from_lit("--------"), 
      rgba_set(1.f, 1.f, 1.f, 1.f),
      LIT_WIDTH/2, y, 
      128.f);
  y -= 60.f;
  
  //
  //  
  //
  y = lit_credits_push_subtitle_and_name(y, st8_from_lit("engine"), st8_from_lit("momohoudai"));
  y = lit_credits_push_subtitle_and_name(y, st8_from_lit("gameplay"), st8_from_lit("momohoudai"));
  y = lit_credits_push_subtitle_and_name(y, st8_from_lit("art"), st8_from_lit("momohoudai"));
  y = lit_credits_push_subtitle_and_name(y, st8_from_lit("lighting"), st8_from_lit("momohoudai"));
  y = lit_credits_push_subtitle_and_name(y, st8_from_lit("particles"), st8_from_lit("momohoudai"));
  y = lit_credits_push_subtitle_and_name(y, st8_from_lit("sound"), st8_from_lit("your imagination"));
  y = lit_credits_push_subtitle_and_name(y, st8_from_lit("food"), st8_from_lit("my wife <3"));
  y = lit_credits_push_subtitle_and_name(y, st8_from_lit("coffee"), st8_from_lit("a lot"));

  if (y > LIT_HEIGHT) {
    g_lit->next_mode = LIT_MODE_GAME;
  }
}

// 
// Sandbox Functions
//

static void lit_sandbox_init() {
}
static void lit_sandbox_update() {
  rgba_t color = rgba_set(1.f, 1.f, 1.f, 1.f);
  game_draw_text_center_aligned(
      g_game, 
      &g_lit->assets, 
      ASSET_FONT_ID_DEFAULT, 
      st8_from_lit("A"), 
      color,
      LIT_WIDTH/2, 
      LIT_HEIGHT/2, 
      128.f);
}



//
// Main tick function
//
static b32_t
lit_tick() {
  if(g_game->game == nullptr) {
    void* lit_memory = game_allocate_memory(g_game, sizeof(lit_t));
    if (!lit_memory) return false;
    g_game->game = lit_memory;

    g_lit = (lit_t*)(g_game->game);
    g_lit->level_to_start = 0;
    g_lit->next_mode = LIT_MODE_SPLASH;
    g_lit->mode = LIT_MODE_NONE;
    

    //
    // Initialize assets
    //
    usz_t asset_memory_size = megabytes(20);
    void* asset_memory = game_allocate_memory(g_game, asset_memory_size);
    if (asset_memory == nullptr) return false;
    arena_init(&g_lit->asset_arena, asset_memory, asset_memory_size);
    game_assets_init(&g_lit->assets, g_game, LIT_ASSET_FILE, &g_lit->asset_arena);


    //
    // Initialize debug stuff
    //
    usz_t debug_memory_size = megabytes(1);
    void* debug_memory = game_allocate_memory(g_game, debug_memory_size);
    arena_init(&g_lit->debug_arena, debug_memory, debug_memory_size);
    console_init(&g_lit->console, &g_lit->debug_arena, 32, 256);

    usz_t frame_memory_size = megabytes(1);
    void* frame_memory = game_allocate_memory(g_game, frame_memory_size);
    arena_init(&g_lit->frame_arena, frame_memory, frame_memory_size);


    usz_t mode_memory_size = megabytes(1);
    auto* mode_memory = game_allocate_memory(g_game, mode_memory_size);
    arena_init(&g_lit->mode_arena, mode_memory, mode_memory_size);

    game_set_design_dimensions(g_game, LIT_WIDTH, LIT_HEIGHT);
    game_set_view(g_game, 0.f, LIT_WIDTH, 0.f, LIT_HEIGHT, 0.f, 0.f);

    //
    // Check save data
    //
#if LIT_SAVE_FILE_ENABLE
    lit_init_save_data(); 
#else
    g_lit->save_data.unlocked_levels = 100;
#endif

    
  }

  g_lit = (lit_t*)(g_game->game);

  arena_clear(&g_lit->frame_arena);

  if (g_lit->next_mode != g_lit->mode || game_is_dll_reloaded(g_game)) 
  {
    g_lit->mode = g_lit->next_mode;
    
    switch(g_lit->mode) {
      case LIT_MODE_SPLASH: {
        lit_splash_init();
      } break;
      case LIT_MODE_GAME: {
        lit_game_init();
      } break;
      case LIT_MODE_CREDITS: {
        lit_credits_init();
      } break;
      case LIT_MODE_SANDBOX: {
        lit_sandbox_init();
      } break;

    }
  }

  switch(g_lit->mode) {
    case LIT_MODE_SPLASH: {
      lit_splash_update();
    } break;
    case LIT_MODE_GAME: {
      lit_game_update();
    } break;
    case LIT_MODE_CREDITS: {
      lit_credits_update();
    } break;
    case LIT_MODE_SANDBOX: {
      lit_sandbox_update();
    } break;
  }

  // Debug
#if LIT_DEBUG
  if (is_poked(input, INPUT_BUTTON_CODE_F1)) {
    g_lit->show_debug_type = 
      (lit_show_debug_type_t)((g_lit->show_debug_type + 1)%LIT_SHOW_DEBUG_MAX);
  }

  switch (g_lit->show_debug_type) {
    case LIT_SHOW_DEBUG_CONSOLE: {
      lit_update_and_render_console(); 
    }break;
    case LIT_SHOW_DEBUG_PROFILER: {
      lit_profiler_update_and_render(); 
    }break;
    case LIT_SHOW_DEBUG_INSPECTOR: {
      lit_inspector_update_and_render();
    }break;
    default: {}
  }
#endif

  return true;
}

//
// Game functions
// 
exported 
game_get_config_sig(game_get_config) 
{
  game_config_t ret;

  ret.target_frame_rate = 60;

  ret.debug_arena_size = kilobytes(300);
  ret.max_inspector_entries = 256;
  ret.max_profiler_entries = 256;
  ret.max_profiler_snapshots = 120;

  ret.gfx_arena_size = megabytes(256);
  ret.texture_queue_size = megabytes(5);
  ret.render_command_size = megabytes(100);
  ret.max_textures = 1;
  ret.max_texture_payloads = 1;
  ret.max_sprites = 4096;
  ret.max_triangles = 0;

  ret.audio_enabled = false;
  ret.audio_arena_size = megabytes(256);
  
  ret.window_title = "PRISMIX v1.0";

  ret.max_sprites = 4096;
  return ret;
}

exported 
game_update_and_render_sig(game_update_and_render) 
{ 
  g_game = game;
  lit_tick();
}

//
// JOURNAL
//
// = 2023-08-26 =
//   Added "g_" prefix to some global variables
// 
// = 2023-08-02 = 
//   It is time to put everything into one file.
//   
