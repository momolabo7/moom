#ifndef MOE_MODE_LIT_H
#define MOE_MODE_LIT_H

#define LIT_DEBUG_LIGHT 0
#define LIT_DEBUG_COORDINATES 1

#define LIT_WIDTH  800.f
#define LIT_HEIGHT 800.f

#define LIT_SENSOR_PARTICLE_CD 0.1f
#define LIT_SENSOR_COLOR_MASK 0xFFFFFF00
#define LIT_SENSOR_RADIUS 8.f
#define LIT_SENSOR_PARTICLE_SIZE 16.f
#define LIT_SENSOR_PARTICLE_SPEED 20.f

#define LIT_EXIT_FLASH_DURATION 0.1f
#define LIT_EXIT_FLASH_BRIGHTNESS 0.6f

#define LIT_ENTER_DURATION 3.f

#define LIT_PLAYER_RADIUS 16.f
#define LIT_PLAYER_LIGHT_RETRIEVE_DURATION 0.05f
#define LIT_PLAYER_BREATH_DURATION 2.f
#define LIT_PLAYER_PICKUP_DIST 512.f
#define LIT_PLAYER_ROTATE_SPEED 3.5f

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


#include "game_lit_world.h"
#include "game_lit_entity.h"

enum lit_state_type_t {
  LIT_STATE_TYPE_TRANSITION_IN,
  LIT_STATE_TYPE_NORMAL,

  // exiting states are after NORMAL
  LIT_STATE_TYPE_SOLVED_IN,
  LIT_STATE_TYPE_SOLVED_OUT,
  LIT_STATE_TYPE_TRANSITION_OUT,
};

struct lit_menu_t {

};
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

struct lit_t {
  //
  // Shared data
  //


  union {
    lit_game_t game;
    lit_menu_t menu;
  };

  //
  // Menu related
  //

  //
  // Game related
  //
  


};

#include "game_lit_world.cpp"
#include "game_lit_entity.cpp"



static b32_t 
lit_is_state_exiting(lit_game_t* game) {
  return game->state > LIT_STATE_TYPE_NORMAL; 
}

static void
lit_set_title(lit_game_t* game, str8_t str) {
  game->title = str;
  game->title_timer = 0.f;
  game->title_wp_index = 0;
}


#include "game_lit_levels.h"


static void
lit_update_game(moe_t* moe, lit_game_t* game, platform_t* platform) 
{
  lit_player_t* player = &game->player;
  f32_t dt = platform->seconds_since_last_frame;

  // Transition Logic
  if (game->state == LIT_STATE_TYPE_TRANSITION_IN) 
  {
      // Title 
    if (game->title_wp_index < array_count(lit_title_wps)-1) 
    {
      game->title_timer += dt;
      lit_title_waypoint_t* next_wp = lit_title_wps + game->title_wp_index+1;
      if (game->title_timer >= next_wp->arrival_time) 
      {
        game->title_wp_index++;
      }
    }
    if (game->stage_fade_timer >= 0.f) 
    {
      game->stage_fade_timer -= dt;
    }
    else 
    {
      game->stage_fade_timer = 0.f;
      game->state = LIT_STATE_TYPE_NORMAL;
    }
  }

  else if (game->state == LIT_STATE_TYPE_SOLVED_IN) {
    game->stage_flash_timer += dt;
    if (game->stage_flash_timer >= LIT_EXIT_FLASH_DURATION) {
      game->stage_flash_timer = LIT_EXIT_FLASH_DURATION;
      game->state = LIT_STATE_TYPE_SOLVED_OUT;
    }
  }
  else if (game->state == LIT_STATE_TYPE_SOLVED_OUT) {
    game->stage_flash_timer -= dt;
    if (game->stage_flash_timer <= 0.f) {
      game->stage_flash_timer = 0.f;
      game->state = LIT_STATE_TYPE_TRANSITION_OUT;
    }
  }
  else if (game->state == LIT_STATE_TYPE_TRANSITION_OUT) {
    if (game->stage_fade_timer <= 1.f) {
      game->stage_fade_timer += dt;
    }
    else {
      lit_load_next_level(game);
    }
  }

  if (game->state == LIT_STATE_TYPE_NORMAL) 
  {
    lit_update_player(moe, game, dt);
  }

  for(u32_t light_index = 0; light_index < game->light_count; ++light_index)
  {
    lit_light_t* light = game->lights + light_index;
    lit_gen_light_intersections(light, game->edges, game->edge_count, &moe->frame_arena);
  }

  if (!lit_is_state_exiting(game)) 
  {
    lit_update_sensors(game, dt);

    // win condition
    if (lit_are_all_sensors_activated(game)) 
    {
      game->state = LIT_STATE_TYPE_SOLVED_IN;
    }
    lit_update_particles(game, dt);
  }


}

static void 
lit_render_game(moe_t* moe, lit_game_t* game, platform_t* platform) {
  gfx_push_blend(platform->gfx, 
                 GFX_BLEND_TYPE_SRC_ALPHA,
                 GFX_BLEND_TYPE_INV_SRC_ALPHA); 

  //lit_draw_edges(game); 
  //lit_draw_debug_light_rays(game, moe);
  lit_draw_player(moe, game);
  lit_draw_lights(moe, game);
  
  gfx_push_blend(platform->gfx, GFX_BLEND_TYPE_SRC_ALPHA, GFX_BLEND_TYPE_INV_SRC_ALPHA); 

  lit_render_sensors(moe, game); 
  lit_render_particles(moe, game);

#if LIT_DEBUG_COORDINATES 
  // Debug coordinates
  {
    sb8_make(sb, 64);
    sb8_push_fmt(sb, str8_from_lit("[%f %f]"), 
        platform->mouse_pos.x,
        LIT_HEIGHT - platform->mouse_pos.y);
    paint_text(moe, game->tutorial_font, sb->str, RGBA_WHITE, 0.f, 0.f, 32.f);
  }
#endif


  // Draw the overlay for fade in/out
  {
    rgba_t color = rgba_set(0.f, 0.f, 0.f, game->stage_fade_timer);
    paint_sprite(moe, game->blank_sprite, v2f_set(LIT_WIDTH/2, LIT_HEIGHT/2), v2f_set(LIT_WIDTH, LIT_HEIGHT), color);
    gfx_advance_depth(platform->gfx);
  }

  // Draw the overlay for white flash
  {
    f32_t alpha = game->stage_flash_timer/LIT_EXIT_FLASH_DURATION * LIT_EXIT_FLASH_BRIGHTNESS;
    rgba_t color = rgba_set(1.f, 1.f, 1.f, alpha);
    paint_sprite(moe, game->blank_sprite, v2f_set(LIT_WIDTH/2, LIT_HEIGHT/2), v2f_set(LIT_WIDTH, LIT_HEIGHT), color);
    gfx_advance_depth(platform->gfx);
  }

  // Draw title
  if (game->title_wp_index < array_count(lit_title_wps)-1) 
  { 
    lit_title_waypoint_t* cur_wp = lit_title_wps + game->title_wp_index;
    lit_title_waypoint_t* next_wp = lit_title_wps + game->title_wp_index+1;

    f32_t duration = next_wp->arrival_time - cur_wp->arrival_time;
    f32_t timer = game->title_timer - cur_wp->arrival_time;
    f32_t a = ease_linear_f32(timer/duration); 
    f32_t title_x = cur_wp->x + a * (next_wp->x - cur_wp->x); 
    rgba_t color = rgba_set(1.f, 1.f, 1.f, 1.f);

    paint_text_center_aligned(moe, game->tutorial_font, game->title, color, title_x, LIT_HEIGHT/2, 128.f);
    gfx_advance_depth(platform->gfx);

  }
}

static void 
lit_init(moe_t* moe) {
  assets_t* assets = &moe->assets;
  platform_t* platform = moe->platform;
  lit_t* lit = moe_allocate_scene(lit_t, moe);

  //
  // init game
  //
  lit_game_t* game = &lit->game;
  lit_load_level(game, 0); 
  rng_init(&game->rng, 65535); // don't really need to be strict 

  {
    make(asset_match_t, match);
    set_match_entry(match, ASSET_TAG_TYPE_FONT, 0.f, 1.f);

    game->tutorial_font = find_best_font(assets, ASSET_GROUP_TYPE_FONTS, match);
  }

  game->blank_sprite = find_first_sprite(assets, ASSET_GROUP_TYPE_BLANK_SPRITE);
  game->circle_sprite = find_first_sprite(assets, ASSET_GROUP_TYPE_CIRCLE_SPRITE);
  game->filled_circle_sprite = find_first_sprite(assets, ASSET_GROUP_TYPE_FILLED_CIRCLE_SPRITE);

  platform->set_moe_dims(LIT_WIDTH, LIT_HEIGHT);
  gfx_push_view(platform->gfx, 0.f, LIT_WIDTH, 0.f, LIT_HEIGHT, 0.f, 0.f);
}

static void 
lit_tick(moe_t* moe) 
{
  lit_t* lit = (lit_t*)moe->scene_context;
  assets_t* assets = &moe->assets;
  platform_t* platform = moe->platform;
  lit_game_t* game = &lit->game;

  lit_update_game(moe, game, platform);
  lit_render_game(moe, game, platform);

}

static void 
lit_exit(moe_t* moe) {
}

#endif 
