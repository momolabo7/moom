
struct Lit_Edge {
  B32 is_disabled;
  V2 start_pt;
  V2 end_pt;
};



struct Lit_Light_Intersection {
  B32 is_shell;
  V2 pt;
};

struct Lit_Light_Triangle {
  V2 p0, p1, p2;
};

struct Lit_Light {
  V2 dir;
  F32 half_angle;
  
  V2 pos;  
  U32 color;

  U32 triangle_count;
  Lit_Light_Triangle triangles[256];

  U32 intersection_count;
  Lit_Light_Intersection intersections[256];

};



enum Lit_Light_Type {
  Lit_LIGHT_TYPE_POINT,
  Lit_LIGHT_TYPE_DIRECTIONAL,
  Lit_LIGHT_TYPE_WEIRD
};


