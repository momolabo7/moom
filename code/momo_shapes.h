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


static constexpr F32 width_of(Rect2);
static constexpr U32 width_of(Rect2U);
static constexpr F32 width_of(Rect3);

static constexpr F32 height_of(Rect2);
static constexpr U32 height_of(Rect2U);
static constexpr F32 height_of(Rect3);

static constexpr F32 depth_of(Rect3);


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


#include "momo_shapes.cpp"

#endif //MOMO_SHAPES_H
