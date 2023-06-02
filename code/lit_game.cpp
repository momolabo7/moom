
static b32_t 
lit_game_is_exiting(lit_game_t* g) {
  return g->state > LIT_GAME_STATE_TYPE_NORMAL; 
}

static void
lit_game_set_title(lit_game_t* g, str8_t str = str8_t{}) {
  g->title = str;
  g->title_timer = 0.f;
  g->title_wp_index = 0;
}

#include "lit_game_objects.cpp"
#include "lit_game_levels.cpp"

static void lit_game_load_level(lit_game_t* m, u32_t level_id);


static void
lit_game_update() 
{
  lit_game_t* g = &lit->game;
  lit_game_player_t* player = &g->player;

  if (is_poked(input->buttons[INPUT_BUTTON_CODE_SPACE])) {
    g->freeze = !g->freeze;
  }

  
  f32_t dt = input->delta_time;
  if (g->freeze) {
    dt = 0;
  }

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
#if LIT_SAVE_FILE_ENABLE
      lit_unlock_next_level(g->current_level_id);
#endif
      g->level_to_load(g);
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
    lit_game_animate_everything(g, dt);
    lit_profile_end(animate);

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
    if (g->solved) 
    {
      pf.show_cursor();
      pf.unlock_cursor();
      g->state = LIT_GAME_STATE_TYPE_SOLVED_IN;
    }
  }


  //
  // RENDERING
  //

  // This is the default and happier blend mode
  gfx_set_blend_alpha(gfx);


  lit_profile_begin(rendering);

#if 0
  lit_draw_edges(g); 
  lit_draw_light_rays(g);
#endif // LIT_DEBUG_LINES
  if (g->state == LIT_GAME_STATE_TYPE_NORMAL) {
    lit_game_render_player(g);
  }
  lit_game_render_lights(g);

  gfx_set_blend_alpha(gfx);

  if (!lit_game_is_exiting(g)) {
    lit_game_render_sensors(g); 
    lit_game_render_particles(g);
  }

  // Draw the overlay for fade in/out
  {
    rgba_t color = rgba_set(0.f, 0.f, 0.f, g->stage_fade_timer);
    gfx_push_asset_sprite(gfx, &lit->assets, g->blank_sprite, v2f_set(LIT_WIDTH/2, LIT_HEIGHT/2), v2f_set(LIT_WIDTH, LIT_HEIGHT), color);
    gfx_advance_depth(gfx);
  }

  // Draw the overlay for white flash
  {
    f32_t alpha = g->stage_flash_timer/LIT_EXIT_FLASH_DURATION * LIT_EXIT_FLASH_BRIGHTNESS;
    rgba_t color = rgba_set(1.f, 1.f, 1.f, alpha);
    gfx_push_asset_sprite(gfx, &lit->assets, g->blank_sprite, v2f_set(LIT_WIDTH/2, LIT_HEIGHT/2), v2f_set(LIT_WIDTH, LIT_HEIGHT), color);
    gfx_advance_depth(gfx);
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

    gfx_push_text_center_aligned(gfx, &lit->assets, g->tutorial_font, g->title, color, title_x, LIT_HEIGHT/2, 128.f);
    gfx_advance_depth(gfx);

  }
  lit_profile_end(rendering);
}

static void 
lit_game_init() 
{
  lit_game_t* g = &lit->game;
  rng_init(&g->rng, 65535); // don't really need to be strict 

  make(asset_match_t, match);
  set_match_entry(match, ASSET_TAG_TYPE_FONT, 0.f, 1.f);
  g->tutorial_font = find_best_font(&lit->assets, ASSET_GROUP_TYPE_FONTS, match);
  g->blank_sprite = find_first_sprite(&lit->assets, ASSET_GROUP_TYPE_BLANK_SPRITE);
  g->circle_sprite = find_first_sprite(&lit->assets, ASSET_GROUP_TYPE_CIRCLE_SPRITE);
  g->move_sprite = find_first_sprite(&lit->assets, ASSET_GROUP_TYPE_MOVE_SPRITE);
  g->rotate_sprite = find_first_sprite(&lit->assets, ASSET_GROUP_TYPE_ROTATE_SPRITE);
  g->filled_circle_sprite = find_first_sprite(&lit->assets, ASSET_GROUP_TYPE_FILLED_CIRCLE_SPRITE);

  g->freeze = false;

  // Go to level based on user's progress
  switch(lit_get_levels_unlocked_count())
  {
    case 1: lit_level_move(g); break;
    case 2: lit_level_obstruct(g); break;
    case 3: lit_level_add(g); break;
    //default: lit_level_menu(g);
    default: lit_level_test(g);
  }
  
}

