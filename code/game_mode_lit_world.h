
struct Lit_Edge {
  B32 is_disabled;
  V2 start_pt;
  V2 end_pt;
};


struct Lit_Edge_List {
  U32 count;
  Lit_Edge e[256];
};

struct Lit_Light_Intersection {
  B32 is_shell;
  V2 pt;
};

struct Lit_Light_Intersection_List {
  U32 count;
  Lit_Light_Intersection e[256];
};

struct Lit_Light_Triangle_List {
  U32 count;
  Tri2 e[256];
};

#if LIT_DEBUG_LIGHT
struct Lit_Light_Debug_Ray_List {
  U32 count;
  Ray2 e[256];
};
#endif //LIT_DEBUG_LIGHT

struct Lit_Light {
  V2 dir;
  F32 half_angle;
  
  V2 pos;  
  U32 color;

  Lit_Light_Triangle_List triangles;
  Lit_Light_Intersection_List intersections;

#if LIT_DEBUG_LIGHT
  Lit_Light_Debug_Ray_List debug_rays;
#endif
};

struct Lit_Light_List {
  U32 count;
  Lit_Light e[32];
};


enum Lit_Light_Type {
  Lit_LIGHT_TYPE_POINT,
  Lit_LIGHT_TYPE_DIRECTIONAL,
  Lit_LIGHT_TYPE_WEIRD
};


