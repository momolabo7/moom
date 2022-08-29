struct Lit_Particle {
  V2 pos, vel;
  RGBA color_start, color_end;

  F32 lifespan;
  F32 lifespan_now;
};

struct Lit_Particle_System {
  F32 particle_lifespan_min;
  F32 particle_lifespan_max;

  F32 emit_cooldown_min;
  F32 emit_cooldown_max;
  F32 emit_cooldown_now; 

  F32 particle_color_start;
  F32 particle_color_end;

  // TODO: customizable positions?
  // TODO: customizable velocity?

  U32 particle_count;
  Lit_Particle particles[256];
};

static Lit_Particle*
lit_spawn_particle(Lit_Particle_System* ps) {
  if (ps->particle_count < array_count(ps->particles)) 
    return ps->particles + ps->particle_count++; 
  else return null;
}

static void
lit_update_particles(Lit_Particle_System* ps, F32 dt) {
  ps->emit_cooldown_now -= dt;
  if (ps->emit_cooldown_now <= 0.f) {
    // Spawn
    ps->emit_cooldown_now = 1.0f; // TODO rng this
                               
    Lit_Particle* p = lit_spawn_particle(ps);
    p->pos = { 400.f, 400.f };
    p->vel = { 10.f, 10.f };
    p->color_start = hex_to_rgba(0xFFFFFFFF);
    p->color_end = hex_to_rgba(0xFFFFFF00);
    p->lifespan = p->lifespan_now = 1.f;
  }

  for(U32 particle_id = 0; 
      particle_id < ps->particle_count; ) 
  {
    Lit_Particle* p = ps->particles + particle_id;
    if (p->lifespan_now <= 0.f) {
      // Copy last item to current item
      (*p) = ps->particles[ps->particle_count--]; 

    }
    else { 
      p->lifespan_now -= dt;
      p->pos += p->vel * dt;
      ++particle_id;
    }
    
  }
}

static void
lit_render_particles(Lit_Particle_System* ps, Painter* painter) {
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

