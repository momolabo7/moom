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

