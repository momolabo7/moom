struct Lit_Particle {
  V2 pos, vel;
  V2 size_start, size_end;
  RGBA color_start, color_end;
  Game_Sprite_ID sprite_id;
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

#define LIT_SENSOR_PARTICLE_CD 0.f
#define LIT_SENSOR_COLOR_MASK 0xFFFFFF00
#define LIT_SENSOR_RADIUS 8.f
#define LIT_SENSOR_PARTICLE_SIZE 16.f
#define LIT_SENSOR_PARTICLE_SPEED 20.f

struct Lit_Sensor {
  V2 pos;
  U32 target_color;
  U32 current_color;
  F32 particle_cd;
};

struct Lit_Sensor_List {
  U32 count;
  Lit_Sensor e[32];
  U32 activated;
};


#define LIT_PLAYER_RADIUS 16.f
#define LIT_PLAYER_LIGHT_RETRIEVE_DURATION 0.05f
#define LIT_PLAYER_BREATH_DURATION 2.f

struct Lit_Player {
  V2 pos;
  Lit_Light* held_light;

  // For animating getting the light
  F32 light_retrival_time;
  V2 old_light_pos;
};

