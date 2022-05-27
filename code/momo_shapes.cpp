
static F32
width_of(Rect2 r) {
  return abs_of(r.max.x - r.min.x);
}

static F32
height_of(Rect2 r) {
  return abs_of(r.max.y - r.min.y);
}

static U32
width_of(Rect2U r) {
  return r.max.x - r.min.x;
}

static U32
height_of(Rect2U r) {
  return r.max.y - r.min.y;
}

// https://totologic.blogspot.com/2014/01/accurate-point-in-triangle-test.html
// TODO: We should really profile to see which is the best but I'm assuming
// it's the dot product one

static B32
_is_point_in_triangle_parametric(Tri2 tri, V2 pt) {
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
_is_point_in_triangle_barycentric(Tri2 tri, V2 pt) {
  
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
_is_point_in_triangle_dot_product(Tri2 tri, V2 pt) {
  V2 v0 = { pt.x - tri.pts[0].x, pt.y - tri.pts[0].y };      
  V2 v1 = { pt.x - tri.pts[1].x, pt.y - tri.pts[1].y };      
  V2 v2 = { pt.x - tri.pts[2].x, pt.y - tri.pts[2].y };      
  
  V2 n0 = { tri.pts[1].y - tri.pts[0].y, -tri.pts[1].x + tri.pts[0].x };
  V2 n1 = { tri.pts[2].y - tri.pts[1].y, -tri.pts[2].x + tri.pts[1].x };
  V2 n2 = { tri.pts[0].y - tri.pts[2].y, -tri.pts[0].x + tri.pts[2].x };
  
  B32 side0 = dot(n0,v0) < 0.f;
  B32 side1 = dot(n1,v1) < 0.f;
  B32 side2 = dot(n2,v2) < 0.f;
  
  return side0 == side1 && side0 == side2;
}


static B32
is_point_in_triangle(Tri2 tri, V2 pt) {
  return _is_point_in_triangle_dot_product(tri, pt);
}