struct Lit_Particle {
  V2 pos, vel;
  RGBA color_start, color_end;
  F32 life;
};

struct Lit_Particle_System {
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
lit_update_particles(Lit_Particle_System* ps, float dt) {
  for(U32 particle_id = 0; 
      particle_id < ps->particle_count; 
      ++particle_id) 
  {
    Lit_Particle* p = ps->particles + particle_id;
    if (p->life <= 0.f) {
      // Copy last item to current item
      (*p) = ps->particles[ps->particle_count--]; 
    }
    p->life -= dt;
    p->pos += p->vel * dt;
    
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
    paint_sprite(painter, SPRITE_BLANK, p->pos, {64.f, 64.f});
  }
}

