
#ifndef MOMO_COLORS_H
#define MOMO_COLORS_H


// NOTE(Momo): Each component of RGBA are in the range [0 - 1].
struct RGBF32 {
  F32 r, g, b;   
};

// NOTE(Momo): Each component are in the range of [0 - 1].
// For hue, normally it is a number between [0 - 360], but
// it will be mapped linearly to [0 - 1] in this case.
// i.e. hue 0 is 0 degrees, hue 1 is 360 degrees.
struct HSLF32 {
  F32 h, s, l;  
};

struct RGBAF32 {
  union {
    struct { F32 r, g, b; };  
    RGBF32 rgb;
  };
  F32 a;
};

static RGBAF32 RGBAF32_Create(F32 r, F32 g, F32 b, F32 a);
static HSLF32 HSLF32_Create(F32 h, F32 s, F32 l);


#include "momo_colors.cpp"

#endif //MOMO_COLORS_H

