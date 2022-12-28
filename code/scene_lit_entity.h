


struct Lit_Particle {
  v2f_t pos, vel;
  v2f_t size_start, size_end;
  rgba_t color_start, color_end;
  asset_sprite_id_t sprite_id;
  f32_t lifespan;
  f32_t lifespan_now;
};

struct Lit_Particle_Pool {
  f32_t particle_lifespan_min;
  f32_t particle_lifespan_max;

  f32_t particle_color_start;
  f32_t particle_color_end;

  u32_t particle_count;
  Lit_Particle particles[256];
};




struct Lit_Sensor {
  v2f_t pos;
  u32_t target_color;
  u32_t current_color;
  f32_t particle_cd;
};



struct Lit_Player {
  v2f_t pos;
  Lit_Light* held_light;
  b32_t is_holding_light;

  // For animating getting the light
  f32_t light_retrival_time;
  v2f_t old_light_pos;
};

