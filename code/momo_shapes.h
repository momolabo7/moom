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
static Rect2 ratio(Rect2 lhs, Rect2 rhs);


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


#include "momo_shapes.cpp"

#endif //MOMO_SHAPES_H
