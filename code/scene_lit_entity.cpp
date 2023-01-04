/////////////////////////////////////////////////////////////////////
// Player
//

static void
lit_init_player(lit_t* lit, f32_t x, f32_t y) {
  lit_player_t* player = &lit->player; 
  player->held_light = nullptr;
  player->pos.x = x;
  player->pos.y = y;
}

static void 
lit_update_player(moe_t* moe, lit_t* lit, f32_t dt) 
{
  lit_player_t* player = &lit->player; 
  platform_t* platform = moe->platform;
  

  // Get world mouse position
#if 0
  v2f_t world_mouse_pos = {0};
  {
    world_mouse_pos.x = platform->mouse_pos.x;
    world_mouse_pos.y = MOE_HEIGHT - platform->mouse_pos.y;
  }

  player->pos = world_mouse_pos;
#endif
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

      for(u32_t light_index = 0; light_index < lit->light_count; ++light_index) {
        lit_light_t* l = lit->lights +light_index;
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
lit_draw_player(moe_t* moe, lit_t* lit)
{
  lit_player_t* player = &lit->player;
  platform_t* platform = moe->platform;
  paint_sprite(moe, lit->circle_sprite, 
               player->pos, 
               v2f_set(LIT_PLAYER_RADIUS*2, LIT_PLAYER_RADIUS*2));
  gfx_advance_depth(platform->gfx);

}


/////////////////////////////////////////////////////////////////////
// Particles
//
static void
lit_spawn_particle(lit_t* lit,
                   f32_t lifespan,
                   v2f_t pos, v2f_t vel,
                   rgba_t color_start,
                   rgba_t color_end,
                   v2f_t size_start,
                   v2f_t size_end) 
{
  lit_particle_pool_t* ps = &lit->particles;
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
lit_update_particles(lit_t* lit, f32_t dt) {
  lit_particle_pool_t* ps = &lit->particles;
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
lit_render_particles(moe_t* moe, lit_t* lit) {
  lit_particle_pool_t* ps = &lit->particles;
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

    paint_sprite(moe, lit->filled_circle_sprite, p->pos, size, color);
    gfx_advance_depth(platform->gfx);
  }
}


//////////////////////////////////////////////////////////////////////
// Sensors
static void 
lit_push_sensor(lit_t* lit, f32_t pos_x, f32_t pos_y, u32_t target_color) 
{
  assert(lit->sensor_count < array_count(lit->sensors));
  lit_sensor_t* s = lit->sensors + lit->sensor_count++;
  s->pos.x = pos_x;
  s->pos.y = pos_y;
  s->target_color = target_color;
  s->current_color = 0;
}

static void 
lit_update_sensors(lit_t* lit,
                   f32_t dt) 
{
  lit_particle_pool_t* particles = &lit->particles;
  rng_t* rng = &lit->rng; 

  u32_t activated = 0;
  for(u32_t sensor_index = 0; sensor_index < lit->sensor_count; ++sensor_index)
  {
    lit_sensor_t* sensor = lit->sensors + sensor_index;
    u32_t current_color = 0x0000000;
    
    // For each light, for each triangle, add light
    for(u32_t light_index = 0; light_index < lit->light_count; ++light_index)
    {
      lit_light_t* light = lit->lights +light_index;
      
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
    lit->sensors_activated = activated;

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

      lit_spawn_particle(lit, 
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
lit_are_all_sensors_activated(lit_t* lit) {
  return lit->sensors_activated == lit->sensor_count;
}

static void 
lit_render_sensors(moe_t* moe, lit_t* lit) {
  platform_t* platform = moe->platform;

  for(u32_t sensor_index = 0; sensor_index < lit->sensor_count; ++sensor_index)
  {
    lit_sensor_t* sensor = lit->sensors + sensor_index;
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
