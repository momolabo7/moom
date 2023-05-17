
static b32_t 
lit_game_is_exiting(lit_game_t* game) {
  return game->state > LIT_GAME_STATE_TYPE_NORMAL; 
}

static void
lit_game_set_title(lit_game_t* game, str8_t str = str8_t{}) {
  game->title = str;
  game->title_timer = 0.f;
  game->title_wp_index = 0;
}

#include "lit_game_objects.cpp"
#include "lit_game_levels.cpp"

static void lit_game_load_level(lit_game_t* m, u32_t level_id);

static void
lit_game_update() 
{
  lit_game_t* game = &lit->game;
  lit_game_player_t* player = &game->player;
  f32_t dt = input->delta_time;

  //
  // Transition Logic
  //
  
  lit_profile_begin(transition);
  if (game->state == LIT_GAME_STATE_TYPE_TRANSITION_IN) 
  {
    // Title 
    if (game->title_wp_index < array_count(lit_title_wps)-1) 
    {
      game->title_timer += dt;
      lit_game_title_waypoint_t* next_wp = lit_title_wps + game->title_wp_index+1;
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
      game->state = LIT_GAME_STATE_TYPE_NORMAL;
    }
  }

  else if (game->state == LIT_GAME_STATE_TYPE_SOLVED_IN) {
    game->stage_flash_timer += dt;
    if (game->stage_flash_timer >= LIT_EXIT_FLASH_DURATION) {
      game->stage_flash_timer = LIT_EXIT_FLASH_DURATION;
      game->state = LIT_GAME_STATE_TYPE_SOLVED_OUT;
    }
  }
  else if (game->state == LIT_GAME_STATE_TYPE_SOLVED_OUT) {
    game->stage_flash_timer -= dt;
    if (game->stage_flash_timer <= 0.f) {
      game->stage_flash_timer = 0.f;
      game->state = LIT_GAME_STATE_TYPE_TRANSITION_OUT;
    }
  }
  else if (game->state == LIT_GAME_STATE_TYPE_TRANSITION_OUT) {
    if (game->stage_fade_timer <= 1.f) {
      game->stage_fade_timer += dt;
    }
    else {
#if LIT_SAVE_FILE_ENABLE
      lit_unlock_next_level(game->current_level_id);
#endif
      game->level_to_load(game);
      return;
    }
  }
  lit_profile_end(transition);

  //
  // What to update based on state
  //
  if (game->state == LIT_GAME_STATE_TYPE_NORMAL) 
  {
    lit_profile_begin(animate);
    lit_game_animate_everything(game, dt);
    lit_profile_end(animate);

    lit_game_update_player(game, dt);
  }

  lit_profile_begin(lights);
  lit_game_generate_light(game);
  lit_profile_end(lights);

  if (!lit_game_is_exiting(game)) 
  {
    lit_profile_begin(sensors);
    lit_game_update_sensors(game, dt);
    lit_profile_end(sensors);

    lit_profile_begin(particles);
    lit_game_update_particles(game, dt);
    lit_profile_end(particles);

    //
    // win condition
    //
    if (game->solved) 
    {
      pf->show_cursor();
      pf->unlock_cursor();
      game->state = LIT_GAME_STATE_TYPE_SOLVED_IN;
    }
  }


  //
  // RENDERING
  //

  // This is the default and happier blend mode
  gfx_set_blend_alpha(gfx);


  lit_profile_begin(rendering);
  //lit_draw_edges(game); 
  //lit_draw_debug_light_rays(game, moe);
  if (game->state == LIT_GAME_STATE_TYPE_NORMAL) {
    lit_game_render_player(game);
  }
  lit_game_render_lights(game);

  gfx_set_blend_alpha(gfx);

  if (!lit_game_is_exiting(game)) {
    lit_game_render_sensors(game); 
    lit_game_render_particles(game);
  }

#if LIT_DEBUG_COORDINATES 
  // Debug coordinates
  {
    sb8_make(sb, 64);
    sb8_push_fmt(sb, str8_from_lit("[%f %f]"), pf->mouse_pos.x,LIT_HEIGHT - pf->mouse_pos.y);
    gfx_push_text(gfx, &lit->assets, game->tutorial_font, sb->str, RGBA_WHITE, 0.f, 0.f, 32.f);
  }
#endif


  // Draw the overlay for fade in/out
  {
    rgba_t color = rgba_set(0.f, 0.f, 0.f, game->stage_fade_timer);
    gfx_push_asset_sprite(gfx, &lit->assets, game->blank_sprite, v2f_set(LIT_WIDTH/2, LIT_HEIGHT/2), v2f_set(LIT_WIDTH, LIT_HEIGHT), color);
    gfx_advance_depth(gfx);
  }

  // Draw the overlay for white flash
  {
    f32_t alpha = game->stage_flash_timer/LIT_EXIT_FLASH_DURATION * LIT_EXIT_FLASH_BRIGHTNESS;
    rgba_t color = rgba_set(1.f, 1.f, 1.f, alpha);
    gfx_push_asset_sprite(gfx, &lit->assets, game->blank_sprite, v2f_set(LIT_WIDTH/2, LIT_HEIGHT/2), v2f_set(LIT_WIDTH, LIT_HEIGHT), color);
    gfx_advance_depth(gfx);
  }

  // Draw title
  if (game->title_wp_index < array_count(lit_title_wps)-1) 
  { 
    lit_game_title_waypoint_t* cur_wp = lit_title_wps + game->title_wp_index;
    lit_game_title_waypoint_t* next_wp = lit_title_wps + game->title_wp_index+1;

    f32_t duration = next_wp->arrival_time - cur_wp->arrival_time;
    f32_t timer = game->title_timer - cur_wp->arrival_time;
    f32_t a = f32_ease_linear(timer/duration); 
    f32_t title_x = cur_wp->x + a * (next_wp->x - cur_wp->x); 
    rgba_t color = rgba_set(1.f, 1.f, 1.f, 1.f);

    gfx_push_text_center_aligned(gfx, &lit->assets, game->tutorial_font, game->title, color, title_x, LIT_HEIGHT/2, 128.f);
    gfx_advance_depth(gfx);

  }
  lit_profile_end(rendering);
}

static void 
lit_game_init() 
{
  lit_game_t* game = &lit->game;
  rng_init(&game->rng, 65535); // don't really need to be strict 

  make(asset_match_t, match);
  set_match_entry(match, ASSET_TAG_TYPE_FONT, 0.f, 1.f);
  game->tutorial_font = find_best_font(&lit->assets, ASSET_GROUP_TYPE_FONTS, match);
  game->blank_sprite = find_first_sprite(&lit->assets, ASSET_GROUP_TYPE_BLANK_SPRITE);
  game->circle_sprite = find_first_sprite(&lit->assets, ASSET_GROUP_TYPE_CIRCLE_SPRITE);
  game->move_sprite = find_first_sprite(&lit->assets, ASSET_GROUP_TYPE_MOVE_SPRITE);
  game->rotate_sprite = find_first_sprite(&lit->assets, ASSET_GROUP_TYPE_ROTATE_SPRITE);
  game->filled_circle_sprite = find_first_sprite(&lit->assets, ASSET_GROUP_TYPE_FILLED_CIRCLE_SPRITE);

  // Go to level based on user's progress
  switch(lit_get_levels_unlocked_count())
  {
    case 1: lit_level_move(game); break;
    case 2: lit_level_obstruct(game); break;
    case 3: lit_level_add(game); break;
    //default: lit_level_menu(game);
    default: lit_level_test(game);
  }
  
}

