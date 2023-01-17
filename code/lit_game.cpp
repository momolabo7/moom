// Forward declarations
static void lit_load_level(lit_game_t* m, u32_t level_id);
static void lit_load_next_level(lit_game_t* m);


static void 
lit_calc_ghost_edge_line(lit_edge_t* e, v2f_t* min, v2f_t* max) {
  v2f_t dir = v2f_norm(e->end_pt - e->start_pt) * 0.0001f;
  
  *min = v2f_sub(e->start_pt, dir);
  *max = v2f_add(e->end_pt, dir);
}

static lit_edge_t*
lit_push_edge(lit_game_t* m, f32_t min_x, f32_t min_y, f32_t max_x, f32_t max_y) {
  assert(m->edge_count < array_count(m->edges));
  lit_edge_t* edge = m->edges + m->edge_count++;
  edge->start_pt = v2f_set(min_x, min_y);
  edge->end_pt = v2f_set(max_x, max_y);;

  edge->is_disabled = false;

  return edge;
}

static void 
lit_push_box(lit_game_t* m, f32_t min_x, f32_t min_y, f32_t max_x, f32_t max_y) 
{
  lit_push_edge(m, min_x, min_y, max_x, min_y);
  lit_push_edge(m, max_x, min_y, max_x, max_y);
  lit_push_edge(m, max_x, max_y, min_x, max_y);
  lit_push_edge(m, min_x, max_y, min_x, min_y);
}

static void 
lit_push_double_edge(lit_game_t* m, f32_t min_x, f32_t min_y, f32_t max_x, f32_t max_y) {
  lit_push_edge(m, min_x, min_y, max_x, max_y);
  lit_push_edge(m, max_x, max_y, min_x, min_y);
}

static lit_light_t*
lit_push_light(lit_game_t* m, f32_t pos_x, f32_t pos_y, u32_t color, f32_t angle, f32_t turn) {
  assert(m->light_count < array_count(m->lights));
  lit_light_t* light = m->lights + m->light_count++;
  light->pos.x = pos_x;
  light->pos.y = pos_y;
  light->color = color;

  light->dir.x = f32_cos(turn*TAU_32);
  light->dir.y = f32_sin(turn*TAU_32);
  light->half_angle = deg_to_rad_f32(angle/2.f);
  
  return light;
}

// Returns F32_INFINITY() if cannot find
static f32_t
lit_get_ray_intersection_time_wrt_edges(v2f_t ray_origin, 
                                        v2f_t ray_dir,
                                        lit_edge_t* edges,
                                        u32_t edge_count,
                                        b32_t clamp_to_ray_max = false)
{
  f32_t lowest_t1 = clamp_to_ray_max ? 1.f : F32_INFINITY;
 
  for(u32_t edge_index = 0; edge_index < edge_count; ++edge_index)
  {
    lit_edge_t* edge = edges + edge_index;

    if (edge->is_disabled) continue;

    v2f_t edge_ray_origin;
    v2f_t edge_ray_dir;
    {
      v2f_t p0, p1;
      lit_calc_ghost_edge_line(edge, &p0, &p1);
      edge_ray_origin = p0;
      edge_ray_dir = p1 - p0; 
    }
    
    // Check for parallel
    v2f_t ray_normal = {};
    ray_normal.x = ray_dir.y;
    ray_normal.y = -ray_dir.x;
    
    
    if (!is_close_f32(v2f_dot(ray_normal, edge_ray_dir), 0.f)) {
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
lit_push_triangle(lit_light_t* l, v2f_t p0, v2f_t p1, v2f_t p2, u32_t color) {
  assert(l->triangle_count < array_count(l->triangles));
  lit_light_triangle_t* tri = l->triangles + l->triangle_count++;
  tri->p0 = p0;
  tri->p1 = p1;
  tri->p2 = p2;
}


static void
lit_gen_light_intersections(lit_light_t* l,
                            lit_edge_t* edges,
                            u32_t edge_count,
                            arena_t* tmp_arena)
{
  //moe_profile_block(light_generation);
  arena_set_revert_point(tmp_arena);

  lit_light_type_t light_type = Lit_LIGHT_TYPE_POINT;
  if (l->half_angle < PI_32/2) {
    light_type = Lit_LIGHT_TYPE_DIRECTIONAL; 
  }
  else if(l->half_angle < PI_32) {
    light_type = Lit_LIGHT_TYPE_WEIRD;
  }

  l->intersection_count = 0;
  l->triangle_count = 0;

  f32_t offset_angles[] = {0.0f, 0.01f, -0.01f};
  //f32_t offset_angles[] = {0.0f};
  for (u32_t offset_index = 0;
       offset_index < array_count(offset_angles);
       ++offset_index) 
  {
    f32_t offset_angle = offset_angles[offset_index];
    // For each endpoint
    for(u32_t edge_index = 0; edge_index < edge_count; ++edge_index) 
    {
      lit_edge_t* edge = edges + edge_index;
      
      if (edge->is_disabled) continue;

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
      f32_t t = lit_get_ray_intersection_time_wrt_edges(l->pos, light_ray_dir, edges, edge_count, offset_index == 0);
      
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
        f32_t t = lit_get_ray_intersection_time_wrt_edges(l->pos, dirs[i], edges, edge_count);

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
          lit_push_triangle(l, p0, p1, p2, l->color);
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
        lit_push_triangle(l, p0, p1, p2, l->color);
      }
    }
  }
}


static void
lit_gen_lights(lit_light_t* lights, 
               u32_t light_count,
               lit_edge_t* edges,
               u32_t edge_count,
               arena_t* tmp_arena) 
{
  // Update all lights
  for(u32_t light_index = 0; light_index < light_count; ++light_index)
  {
    lit_light_t* light = lights + light_index;
    lit_gen_light_intersections(light, edges, edge_count, tmp_arena);
  }

}

static void
lit_draw_lights(moe_t* moe, lit_game_t* game) {
  platform_t* platform = moe->platform;

  // Emitters
  for(u32_t light_index = 0; light_index < game->light_count; ++light_index)
  {
    lit_light_t* light = game->lights + light_index;
    paint_sprite(moe, game->filled_circle_sprite, 
                 light->pos,
                 {16.f, 16.f},
                 {0.8f, 0.8f, 0.8f, 1.f});
    gfx_advance_depth(platform->gfx);
  }
 
  // Lights
  gfx_push_blend(platform->gfx, GFX_BLEND_TYPE_SRC_ALPHA, GFX_BLEND_TYPE_ONE); 
  

  for(u32_t light_index = 0; light_index < game->light_count; ++light_index)
  {
    lit_light_t* l = game->lights + light_index;
    for(u32_t tri_index = 0; tri_index < l->triangle_count; ++tri_index)
    {
      lit_light_triangle_t* lt = l->triangles + tri_index;
      gfx_push_filled_triangle(platform->gfx, 
                               rgba_hex(l->color),
                               lt->p0,
                               lt->p1,
                               lt->p2);
    } 
    gfx_advance_depth(platform->gfx);
  }

}



static void
lit_draw_edges(moe_t* moe, lit_game_t* game) {

  platform_t* platform = moe->platform;
  for(u32_t edge_index = 0; edge_index < game->edge_count; ++edge_index) 
  {
    lit_edge_t* edge = game->edges + edge_index;
    if (edge->is_disabled) continue;

    gfx_push_line(platform->gfx, edge->start_pt, edge->end_pt, 3.f, rgba_hex(0x888888FF));
  }
  gfx_advance_depth(platform->gfx);
}

/////////////////////////////////////////////////////////////////////
// Player
//

static void
lit_init_player(lit_game_t* game, f32_t x, f32_t y) {
  lit_player_t* player = &game->player; 
  player->held_light = nullptr;
  player->pos.x = x;
  player->pos.y = y;
}

static void 
lit_update_player(moe_t* moe, lit_game_t* game, f32_t dt) 
{
  lit_player_t* player = &game->player; 
  platform_t* platform = moe->platform;
  

#if 1
  // Get movement direction
  v2f_t direction = {0};
  if (platform_is_button_down(platform->button_up)) {
    direction.y += 1.f;
  }
  if (platform_is_button_down(platform->button_down)) {
    direction.y -= 1.f;
  }
  if (platform_is_button_down(platform->button_right)) {
    direction.x += 1.f;
  }
  if (platform_is_button_down(platform->button_left)) {
    direction.x -= 1.f;
  }
  
  // Held light controls
  if (player->held_light != nullptr) {
    if (platform_is_button_down(platform->button_rotate_left)){ 
      player->held_light->dir = 
        v2f_rotate(player->held_light->dir, LIT_PLAYER_ROTATE_SPEED * dt );
    }
    if (platform_is_button_down(platform->button_rotate_right)){ 
      player->held_light->dir = 
        v2f_rotate(player->held_light->dir, -LIT_PLAYER_ROTATE_SPEED * dt);
    }
  }
  
  // Do Movement
  if (v2f_len_sq(direction) > 0.f) {
    f32_t speed = 300.f;
    v2f_t velocity = v2f_norm(direction);
    velocity *= speed * dt;
    player->pos += velocity;
  }
#endif 
  // 'Pick up'  button
  if (platform_is_button_poked(platform->button_use)) {
    if (player->held_light == nullptr) {
      f32_t shortest_dist = LIT_PLAYER_PICKUP_DIST; // limit
      lit_light_t* nearest_light = nullptr;

      for(u32_t light_index = 0; light_index < game->light_count; ++light_index) {
        lit_light_t* l = game->lights +light_index;
        f32_t dist = v2f_dist_sq(l->pos, player->pos);
        if (shortest_dist > dist) {
          nearest_light = l;
          shortest_dist = dist;
        }
      }
      
      if (nearest_light) {          
        player->held_light = nearest_light;
        player->old_light_pos = nearest_light->pos;
        player->light_retrival_time = 0.f;
      }
    }
    else {
      player->held_light = nullptr;
    }
  }


  // Move the held light to player's position
  if (player->held_light) {
    if (player->light_retrival_time < LIT_PLAYER_LIGHT_RETRIEVE_DURATION) {
      player->light_retrival_time += dt;
    }
    else {
      player->light_retrival_time = LIT_PLAYER_LIGHT_RETRIEVE_DURATION;
    }
    f32_t ratio = player->light_retrival_time / LIT_PLAYER_LIGHT_RETRIEVE_DURATION; 
    player->held_light->pos.x = lerp_f32(player->old_light_pos.x, player->pos.x, ratio) ;
    player->held_light->pos.y = lerp_f32(player->old_light_pos.y, player->pos.y,  ratio) ;
  }

  // Restrict movement
  if (player->pos.x > MOE_WIDTH - LIT_PLAYER_RADIUS) {
    player->pos.x = MOE_WIDTH - LIT_PLAYER_RADIUS;
  }
  if (player->pos.x < LIT_PLAYER_RADIUS) {
    player->pos.x = LIT_PLAYER_RADIUS;
  }
  if (player->pos.y > MOE_HEIGHT - LIT_PLAYER_RADIUS) {
    player->pos.y = MOE_HEIGHT - LIT_PLAYER_RADIUS;
  }
  if (player->pos.y < LIT_PLAYER_RADIUS) {
    player->pos.y = LIT_PLAYER_RADIUS;
  }
}

static void
lit_draw_player(moe_t* moe, lit_game_t* game)
{
  lit_player_t* player = &game->player;
  platform_t* platform = moe->platform;
  paint_sprite(moe, game->circle_sprite, 
               player->pos, 
               v2f_set(LIT_PLAYER_RADIUS*2, LIT_PLAYER_RADIUS*2));
  gfx_advance_depth(platform->gfx);

}


/////////////////////////////////////////////////////////////////////
// Particles
//
static void
lit_spawn_particle(lit_game_t* game,
                   f32_t lifespan,
                   v2f_t pos, v2f_t vel,
                   rgba_t color_start,
                   rgba_t color_end,
                   v2f_t size_start,
                   v2f_t size_end) 
{
  lit_particle_pool_t* ps = &game->particles;
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
lit_update_particles(lit_game_t* game, f32_t dt) {
  lit_particle_pool_t* ps = &game->particles;
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
lit_render_particles(moe_t* moe, lit_game_t* game) {
  lit_particle_pool_t* ps = &game->particles;
  platform_t* platform = moe->platform;

  // Render particles
  for(u32_t particle_id = 0; 
      particle_id < ps->particle_count;
      ++particle_id) 
  {
    lit_particle_t* p = ps->particles + particle_id;

    f32_t lifespan_ratio = 1.f -  p->lifespan_now / p->lifespan;

    rgba_t color = {0};
    color.r = lerp_f32(p->color_start.r, p->color_end.r, lifespan_ratio);  
    color.g = lerp_f32(p->color_start.g, p->color_end.g, lifespan_ratio);  
    color.b = lerp_f32(p->color_start.b, p->color_end.b, lifespan_ratio);  
    color.a = lerp_f32(p->color_start.a, p->color_end.a, lifespan_ratio);  
  
    v2f_t size = {0};
    size.w = lerp_f32(p->size_start.w , p->size_end.w, lifespan_ratio);
    size.h = lerp_f32(p->size_start.h , p->size_end.h, lifespan_ratio);

    paint_sprite(moe, game->filled_circle_sprite, p->pos, size, color);
    gfx_advance_depth(platform->gfx);
  }
}


//////////////////////////////////////////////////////////////////////
// Sensors
static lit_sensor_t* 
lit_push_sensor(lit_game_t* game, f32_t pos_x, f32_t pos_y, u32_t target_color) 
{
  assert(game->sensor_count < array_count(game->sensors));
  lit_sensor_t* s = game->sensors + game->sensor_count++;
  s->pos.x = pos_x;
  s->pos.y = pos_y;
  s->target_color = target_color;
  s->current_color = 0;

  return s;

}

static void 
lit_update_sensors(lit_game_t* game, f32_t dt) 
{
  lit_particle_pool_t* particles = &game->particles;
  rng_t* rng = &game->rng; 

  u32_t activated = 0;
  for(u32_t sensor_index = 0; sensor_index < game->sensor_count; ++sensor_index)
  {
    lit_sensor_t* sensor = game->sensors + sensor_index;
    u32_t current_color = 0x0000000;
    
    // For each light, for each triangle, add light
    for(u32_t light_index = 0; light_index < game->light_count; ++light_index)
    {
      lit_light_t* light = game->lights +light_index;
      
      for(u32_t tri_index = 0; tri_index < light->triangle_count; ++tri_index)
      {
        lit_light_triangle_t* tri = light->triangles +tri_index;
        if (bonk_tri2_pt2(tri->p0, tri->p1, tri->p2, sensor->pos)) 
        {
          // TODO(Momo): Probably not the right way do sensor
          current_color += light->color >> 8 << 8; // ignore alpha
          break; // ignore the rest of the triangles
        }
      }
    
    }

    // Sensor color check
    sensor->current_color = current_color;
    if ((sensor->current_color & LIT_SENSOR_COLOR_MASK) == 
        (sensor->target_color & LIT_SENSOR_COLOR_MASK)) 
    {
      ++activated;
    }
    game->sensors_activated = activated;

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

      lit_spawn_particle(game, 
                         1.f,
                         sensor->pos,
                         particle_vel,
                         start_color,
                         end_color,
                         size_start,
                         size_end);
    }
  }
}

static b32_t
lit_are_all_sensors_activated(lit_game_t* game) {
  return game->sensors_activated == game->sensor_count;
}

static void 
lit_render_sensors(moe_t* moe, lit_game_t* game) {
  platform_t* platform = moe->platform;

  for(u32_t sensor_index = 0; sensor_index < game->sensor_count; ++sensor_index)
  {
    lit_sensor_t* sensor = game->sensors + sensor_index;
    gfx_push_filled_circle(platform->gfx, sensor->pos, LIT_SENSOR_RADIUS, 8, rgba_hex(sensor->target_color)); 

    // only for debugging
#if 0
    sb8_make(sb, 128);
    sb8_push_fmt(sb, str8_from_lit("[%X]"), sensor->current_color);
    paint_text(painter,
               FONT_DEFAULT, 
               sb->str,
               rgba_hex(0xFFFFFFFF),
               sensor->pos.x - 100.f,
               sensor->pos.y + 10.f,
               32.f);
#endif

    gfx_advance_depth(platform->gfx);
  }
}


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
lit_init_game(moe_t* moe, lit_game_t* game, platform_t* platform) 
{
  assets_t* assets = &moe->assets;
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
