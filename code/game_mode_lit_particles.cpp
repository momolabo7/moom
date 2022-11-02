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

    Game_Sprite_ID sprite_id = find_first_sprite(assets, GAME_ASSET_GROUP_TYPE_BLANK_SPRITE);
    paint_sprite(sprite_id, p->pos, size, color);
    gfx_advance_depth(gfx);
  }
}

