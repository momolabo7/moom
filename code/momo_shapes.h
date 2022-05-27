#ifndef MOMO_SHAPES_H
#define MOMO_SHAPES_H

//~ Rects
struct Rect2 {
  V2 min, max;
};

struct Rect2S{
  V2S min, max;
};

struct Rect2U {
  V2U min, max;
};

struct Rect3 {
  V3 min, max;
};

// Gets the normalized values of Rect lhs based on another Rect rhs
static F32 width_of(Rect2 lhs);
static F32 height_of(Rect2 lhs);
static U32 width_of(Rect2U lhs);
static U32 height_of(Rect2U lhs);



//~ Circles
struct Circ2
{
  F32 radius;
  V2 center;
};


//~ Lines
struct Line2 {
  V2 min, max;
};

struct Ray2 {
  V2 pt;
  V2 dir;
};

//~ Triangle
struct Tri2 {
  V2 pts[3];
};

static B32 is_point_in_triangle(Tri2 tri, V2 pt);

#include "momo_shapes.cpp"

#endif //MOMO_SHAPES_H
