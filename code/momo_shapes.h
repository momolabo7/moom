#ifndef MOMO_SHAPES_H
#define MOMO_SHAPES_H


typedef struct Rect2 {
  V2 min, max;
}Rect2;


typedef struct Rect2U {
  V2U min, max;
}Rect2U;


typedef struct Ray2 {
  V2 pt;
  V2 dir;
}Ray2;

typedef struct Tri2 {
  V2 pts[3];
}Tri2;


static B32 bonk_tri2_pt2(Tri2 tri, V2 pt);

///////////////////////////////////////////////////////////////////
// IMPLEMENTATION



// https://totologic.blogspot.com/2014/01/accurate-point-in-triangle-test.html
// NOTE(Momo): We should really profile to see which is the best but I'm assuming
// it's the dot product one
static B32
_bonk_tri2_pt2_parametric(Tri2 tri, V2 pt) {
  F32 denominator = (tri.pts[0].x*(tri.pts[1].y - tri.pts[2].y) + 
                     tri.pts[0].y*(tri.pts[2].x - tri.pts[1].x) + 
                     tri.pts[1].x*tri.pts[2].y - tri.pts[1].y*tri.pts[2].x);
  
  F32 t1 = (pt.x*(tri.pts[2].y - tri.pts[0].y) + 
            pt.y*(tri.pts[0].x - tri.pts[2].x) - 
            tri.pts[0].x*tri.pts[2].y + tri.pts[0].y*tri.pts[2].x) / denominator;
  
  F32 t2 = (pt.x*(tri.pts[1].y - tri.pts[0].y) + 
            pt.y*(tri.pts[0].x - tri.pts[1].x) - 
            tri.pts[0].x*tri.pts[1].y + tri.pts[0].y*tri.pts[1].x) / -denominator;
  
  F32 s = t1 + t2;
  
  return 0 <= t1 && t1 <= 1 && 0 <= t2 && t2 <= 1 && s <= 1;
}

static B32
_bonk_tri2_pt2_barycentric(Tri2 tri, V2 pt) {
  
  F32 denominator = ((tri.pts[1].y - tri.pts[2].y)*
                     (tri.pts[0].x - tri.pts[2].x) + (tri.pts[2].x - tri.pts[1].x)*
                     (tri.pts[0].y - tri.pts[2].y));
  
  F32 a = ((tri.pts[1].y - tri.pts[2].y)*
           (pt.x - tri.pts[2].x) + (tri.pts[2].x - tri.pts[1].x)*
           (pt.y - tri.pts[2].y)) / denominator;
  
  F32 b = ((tri.pts[2].y - tri.pts[0].y)*
           (pt.x - tri.pts[2].x) + (tri.pts[0].x - tri.pts[2].x)*
           (pt.y - tri.pts[2].y)) / denominator;
  
  F32 c = 1.f - a - b;
  
  return 0.f <= a && a <= 1.f && 0.f <= b && b <= 1.f && 0.f <= c && c <= 1.f;
  
}


static B32
_bonk_tri2_pt2_dot_product(Tri2 tri, V2 pt) {
  V2 vec0 = v2_set(pt.x - tri.pts[0].x, pt.y - tri.pts[0].y);      
  V2 vec1 = v2_set(pt.x - tri.pts[1].x, pt.y - tri.pts[1].y);      
  V2 vec2 = v2_set(pt.x - tri.pts[2].x, pt.y - tri.pts[2].y);      
  
  V2 n0 = v2_set(tri.pts[1].y - tri.pts[0].y, -tri.pts[1].x + tri.pts[0].x);
  V2 n1 = v2_set(tri.pts[2].y - tri.pts[1].y, -tri.pts[2].x + tri.pts[1].x);
  V2 n2 = v2_set(tri.pts[0].y - tri.pts[2].y, -tri.pts[0].x + tri.pts[2].x);
  
  B32 side0 = v2_dot(n0,vec0) < 0.f;
  B32 side1 = v2_dot(n1,vec1) < 0.f;
  B32 side2 = v2_dot(n2,vec2) < 0.f;
  
  return side0 == side1 && side0 == side2;
}


static B32
bonk_tri2_pt2(Tri2 tri, V2 pt) {
  return _bonk_tri2_pt2_dot_product(tri, pt);
}



#endif //MOMO_SHAPES_H
