#ifndef MOMO_SHAPES_H
#define MOMO_SHAPES_H

//~ NOTE(Momo): Rects
typedef struct Rect2F32 {
  V2F32 min, max;
} Rect2F32;



typedef struct Rect2U32 {
  V2U32 min, max;
} Rect2U32;


typedef struct Rect3F32 {
  V3F32 min, max;
} Rect3F32;



#define Rect_Width(r) (r.max.x - r.min.x)
#define Rect_Height(r) (r.max.y - r.min.y)

//~ NOTE(Momo): Circles
typedef struct Circ2F32
{
  F32 radius;
  V2F32 center;
} Circ2F32;


//~ NOTE(Momo): Lines
typedef struct Line2F32 {
  V2F32 min, max;
} Line2F32;

#endif //MOMO_SHAPES_H
