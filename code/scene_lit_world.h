
struct Lit_Edge {
  b32_t is_disabled;
  v2f_t start_pt;
  v2f_t end_pt;
};



struct Lit_Light_Intersection {
  b32_t is_shell;
  v2f_t pt;
};

struct Lit_Light_Triangle {
  v2f_t p0, p1, p2;
};

struct Lit_Light {
  v2f_t dir;
  f32_t half_angle;
  
  v2f_t pos;  
  u32_t color;

  u32_t triangle_count;
  Lit_Light_Triangle triangles[256];

  u32_t intersection_count;
  Lit_Light_Intersection intersections[256];

};



enum Lit_Light_Type {
  Lit_LIGHT_TYPE_POINT,
  Lit_LIGHT_TYPE_DIRECTIONAL,
  Lit_LIGHT_TYPE_WEIRD
};


