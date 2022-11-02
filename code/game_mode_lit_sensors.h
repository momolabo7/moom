#define LIT_SENSOR_PARTICLE_CD 0.2f
#define LIT_SENSOR_COLOR_MASK 0xFFFFFF00
#define LIT_SENSOR_RADIUS 8.f

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



