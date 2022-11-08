#ifndef MOMO_SHAPES_H
#define MOMO_SHAPES_H


// TODO: width height more important than min/max
typedef struct {
  V2 min, max;
}Rect2;

typedef struct {
  V2S min, max;
}Rect2S;

typedef struct {
  V2U min, max;
}Rect2U;

typedef struct {
  V3 min, max;
}Rect3;

typedef struct {
  V2 anchor;
  V2 dims;
}Aabb2;

typedef struct 
{
  F32 radius;
  V2 center;
}Circ2;


typedef struct {
  V2 min, max;
}Line2;

typedef struct {
  V2 pt;
  V2 dir;
}Ray2;

typedef struct{
  V2 pts[3];
}Tri2;


///////////////////////////////////////////////////////////////////
// IMPLEMENTATION
static Circ2
circ2(V2 center, F32 radius) {
  Circ2 ret = {0};
  ret.center = center;
  ret.radius = radius;
  return ret;
}
static Line2
line2(V2 min, V2 max) {
  Line2 ret = {0};
  ret.min = min;
  ret.max = max;
  return ret;
}



// https://totologic.blogspot.com/2014/01/accurate-point-in-triangle-test.html
// NOTE(Momo): We should really profile to see which is the best but I'm assuming
// it's the dot product one
static B32
_t2_is_point_within_parametric(Tri2 tri, V2 pt) {
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
_t2_is_point_within_barycentric(Tri2 tri, V2 pt) {
  
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
_t2_is_point_within_dot_product(Tri2 tri, V2 pt) {
  V2 v0 = { pt.x - tri.pts[0].x, pt.y - tri.pts[0].y };      
  V2 v1 = { pt.x - tri.pts[1].x, pt.y - tri.pts[1].y };      
  V2 v2 = { pt.x - tri.pts[2].x, pt.y - tri.pts[2].y };      
  
  V2 n0 = { tri.pts[1].y - tri.pts[0].y, -tri.pts[1].x + tri.pts[0].x };
  V2 n1 = { tri.pts[2].y - tri.pts[1].y, -tri.pts[2].x + tri.pts[1].x };
  V2 n2 = { tri.pts[0].y - tri.pts[2].y, -tri.pts[0].x + tri.pts[2].x };
  
  B32 side0 = v2_dot(n0,v0) < 0.f;
  B32 side1 = v2_dot(n1,v1) < 0.f;
  B32 side2 = v2_dot(n2,v2) < 0.f;
  
  return side0 == side1 && side0 == side2;
}


static B32
t2_is_point_within(Tri2 tri, V2 pt) {
  return _t2_is_point_within_dot_product(tri, pt);
}

#endif //MOMO_SHAPES_H
