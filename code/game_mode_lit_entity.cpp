/////////////////////////////////////////////////////////////////////
// Player
//

static void
lit_init_player(Lit* lit, F32 x, F32 y) {
  Lit_Player* player = &lit->player; 
  player->held_light = null;
  player->pos.x = x;
  player->pos.y = y;
}

static void 
lit_update_player(Lit* lit, F32 dt) 
{
  Lit_Player* player = &lit->player; 
  Lit_Light_List* lights = &lit->lights;

  // Get movement direction
  V2 direction = {0};
  if (pf_is_button_down(platform->button_up)) {
    direction.y += 1.f;
  }
  if (pf_is_button_down(platform->button_down)) {
    direction.y -= 1.f;
  }
  if (pf_is_button_down(platform->button_right)) {
    direction.x += 1.f;
  }
  if (pf_is_button_down(platform->button_left)) {
    direction.x -= 1.f;
  }
  
  // Held light controls
  if (player->held_light != null) {
    const F32 speed = 5.f;
    if (pf_is_button_down(platform->button_rotate_left)){ 
      player->held_light->dir = 
        v2_rotate(player->held_light->dir, speed * dt );
    }
    if (pf_is_button_down(platform->button_rotate_right)){ 
      player->held_light->dir = 
        v2_rotate(player->held_light->dir, -speed * dt);
    }
  }


  // Use button
  if (pf_is_button_poked(platform->button_use)) {
    if (player->held_light == null) {
      F32 shortest_dist = 512.f; // limit
      Lit_Light* nearest_light = null;
      al_foreach(light_index, lights) {
        Lit_Light* l = al_at(lights, light_index);
        F32 dist = v2_dist_sq(l->pos, player->pos);
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
    else{ 
      player->held_light = null;
    }
  }
  
  // Do Movement
  if (v2_len_sq(direction) > 0.f) {
    F32 speed = 300.f;
    V2 velocity = v2_norm(direction);
    velocity *= speed * dt;
    player->pos += velocity;
  }

  if (player->held_light) {
    if (player->light_retrival_time < LIT_PLAYER_LIGHT_RETRIEVE_DURATION) {
      player->light_retrival_time += dt;
    }
    else {
      player->light_retrival_time = LIT_PLAYER_LIGHT_RETRIEVE_DURATION;
    }
    F32 ratio = player->light_retrival_time / LIT_PLAYER_LIGHT_RETRIEVE_DURATION; 
    player->held_light->pos.x = lerp_f32(player->old_light_pos.x, player->pos.x, ratio) ;
    player->held_light->pos.y = lerp_f32(player->old_light_pos.y, player->pos.y,  ratio) ;
  }

  // Restrict movement
  if (player->pos.x > GAME_WIDTH - LIT_PLAYER_RADIUS) {
    player->pos.x = GAME_WIDTH - LIT_PLAYER_RADIUS;
  }
  if (player->pos.x < LIT_PLAYER_RADIUS) {
    player->pos.x = LIT_PLAYER_RADIUS;
  }
  if (player->pos.y > GAME_HEIGHT - LIT_PLAYER_RADIUS) {
    player->pos.y = GAME_HEIGHT - LIT_PLAYER_RADIUS;
  }
  if (player->pos.y < LIT_PLAYER_RADIUS) {
    player->pos.y = LIT_PLAYER_RADIUS;
  }
}

static void
lit_draw_player(Lit* lit){
  Lit_Player* player = &lit->player;
  paint_sprite(lit->circle_sprite, 
               player->pos, 
               v2(LIT_PLAYER_RADIUS*2, LIT_PLAYER_RADIUS*2));
  gfx_advance_depth(gfx);

}


/////////////////////////////////////////////////////////////////////
// Particles
//
static void
lit_spawn_particle(Lit* lit,
                   F32 lifespan,
                   V2 pos, V2 vel,
                   RGBA color_start,
                   RGBA color_end,
                   V2 size_start,
                   V2 size_end) 
{
  Lit_Particle_Pool* ps = &lit->particles;
  if (ps->particle_count < array_count(ps->particles)) {
    Lit_Particle* p = ps->particles + ps->particle_count++; 
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
lit_update_particles(Lit* lit, F32 dt) {
  Lit_Particle_Pool* ps = &lit->particles;
  for(U32 particle_id = 0; 
      particle_id < ps->particle_count; ) 
  {
    Lit_Particle* p = ps->particles + particle_id;
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
lit_render_particles(Lit* lit) {
  Lit_Particle_Pool* ps = &lit->particles;
  // Render particles
  for(U32 particle_id = 0; 
      particle_id < ps->particle_count;
      ++particle_id) 
  {
    Lit_Particle* p = ps->particles + particle_id;

    F32 lifespan_ratio = 1.f -  p->lifespan_now / p->lifespan;

    RGBA color = {0};
    color.r = lerp_f32(p->color_start.r, p->color_end.r, lifespan_ratio);  
    color.g = lerp_f32(p->color_start.g, p->color_end.g, lifespan_ratio);  
    color.b = lerp_f32(p->color_start.b, p->color_end.b, lifespan_ratio);  
    color.a = lerp_f32(p->color_start.a, p->color_end.a, lifespan_ratio);  
  
    V2 size = {0};
    size.w = lerp_f32(p->size_start.w , p->size_end.w, lifespan_ratio);
    size.h = lerp_f32(p->size_start.h , p->size_end.h, lifespan_ratio);

    paint_sprite(lit->filled_circle_sprite, p->pos, size, color);
    gfx_advance_depth(gfx);
  }
}


//////////////////////////////////////////////////////////////////////
// Sensors
static void 
lit_push_sensor(Lit* lit, F32 pos_x, F32 pos_y, U32 target_color) 
{
  Lit_Sensor_List* sensors = &lit->sensors;

  assert(!al_is_full(sensors));
  Lit_Sensor* s = al_append(sensors);
  s->pos.x = pos_x;
  s->pos.y = pos_y;
  s->target_color = target_color;
  s->current_color = 0;
}

static void 
lit_update_sensors(Lit* lit,
                   F32 dt) 
{
  Lit_Sensor_List* sensors = &lit->sensors; 
  Lit_Particle_Pool* particles = &lit->particles;
  Lit_Light_List* lights = &lit->lights; 
  RNG* rng = &lit->rng; 

  U32 activated = 0;
  al_foreach(sensor_index, sensors)
  {
    Lit_Sensor* sensor = al_at(sensors, sensor_index);
    U32 current_color = 0x0000000;
    
    // For each light, for each triangle, add light
    al_foreach(light_index, lights)
    {
      Lit_Light* light = al_at(lights, light_index);
      
      al_foreach(tri_index, &light->triangles)
      {
        Tri2 tri = *al_at(&light->triangles, tri_index);
        if (bonk_tri2_pt2(tri, sensor->pos)) 
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
    sensors->activated = activated;

    // Particle emission check
    sensor->particle_cd -= dt;
    if (sensor->particle_cd <= 0.f) 
    {
      sensor->particle_cd = LIT_SENSOR_PARTICLE_CD;
      V2 rand_dir = rng_unit_circle(rng);
      V2 particle_vel = v2_scale(rand_dir, 10.f); 
      RGBA target_color = hex_to_rgba(sensor->target_color); 

      RGBA start_color = target_color;
      start_color.a = 1.f;

      RGBA end_color = target_color;
      end_color.a = 0.f;

      V2 size_start = { 32.f, 32.f };
      V2 size_end = { 0.f, 0.f };

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

static B32
lit_are_all_sensors_activated(Lit* lit) {
  Lit_Sensor_List* sensors = &lit->sensors;
  return sensors->activated == sensors->count;
}

static void 
lit_render_sensors(Lit* lit) {
  Lit_Sensor_List* sensors = &lit->sensors;
  al_foreach(sensor_index, sensors)
  {
    Lit_Sensor* sensor = al_at(sensors, sensor_index);
    Circ2 circ = { LIT_SENSOR_RADIUS, sensor->pos };
    gfx_push_filled_circle(gfx, circ, 8,  hex_to_rgba(sensor->target_color)); 

    // only for debugging
#if 0
    sb8_make(sb, 128);
    sb8_push_fmt(sb, str8_from_lit("[%X]"), sensor->current_color);
    paint_text(painter,
               FONT_DEFAULT, 
               sb->str,
               hex_to_rgba(0xFFFFFFFF),
               sensor->pos.x - 100.f,
               sensor->pos.y + 10.f,
               32.f);
#endif

    gfx_advance_depth(gfx);
  }
}
