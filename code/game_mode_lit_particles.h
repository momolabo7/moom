struct Lit_Particle {
  V2 pos, vel;
  RGBA color_start, color_end;

  F32 lifespan;
  F32 lifespan_now;
};

struct Lit_Particle_Pool {
  F32 particle_lifespan_min;
  F32 particle_lifespan_max;

  F32 particle_color_start;
  F32 particle_color_end;

  U32 particle_count;
  Lit_Particle particles[256];
};

static void
lit_spawn_particle(Lit_Particle_Pool* ps,
                   F32 lifespan,
                   V2 pos, V2 vel,
                   RGBA color_start,
                   RGBA color_end) 
{
  if (ps->particle_count < array_count(ps->particles)) {
    Lit_Particle* p = ps->particles + ps->particle_count++; 
    p->pos = pos;
    p->vel = vel;
    p->color_start = color_start;
    p->color_end = color_end;
    p->lifespan = p->lifespan_now = lifespan;

  }
}

static void
lit_update_particles(Lit_Particle_Pool* ps, F32 dt) {
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
lit_render_particles(Lit_Particle_Pool* ps, Painter* painter) {
  // Render particles
  for(U32 particle_id = 0; 
      particle_id < ps->particle_count;
      ++particle_id) 
  {
    Lit_Particle* p = ps->particles + particle_id;
    RGBA color = {};

    F32 lifespan_ratio = 1.f -  p->lifespan_now / p->lifespan;
    color.r = lerp_f32(p->color_start.r, p->color_end.r, lifespan_ratio);  
    color.g = lerp_f32(p->color_start.g, p->color_end.g, lifespan_ratio);  
    color.b = lerp_f32(p->color_start.b, p->color_end.b, lifespan_ratio);  
    color.a = lerp_f32(p->color_start.a, p->color_end.a, lifespan_ratio);  
    paint_sprite(painter, SPRITE_BLANK, p->pos, {64.f, 64.f}, color);
  }
}

