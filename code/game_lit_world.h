
struct lit_edge_t {
  b32_t is_disabled;
  v2f_t start_pt;
  v2f_t end_pt;
};



struct lit_light_intersection_t {
  b32_t is_shell;
  v2f_t pt;
};

struct lit_light_triangle_t {
  v2f_t p0, p1, p2;
};

struct lit_light_t {
  v2f_t dir;
  f32_t half_angle;
  
  v2f_t pos;  
  u32_t color;

  u32_t triangle_count;
  lit_light_triangle_t triangles[256];

  u32_t intersection_count;
  lit_light_intersection_t intersections[256];

};



enum lit_light_type_t {
  Lit_LIGHT_TYPE_POINT,
  Lit_LIGHT_TYPE_DIRECTIONAL,
  Lit_LIGHT_TYPE_WEIRD
};


