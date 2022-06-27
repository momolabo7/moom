// Authors: Gerald Wong, momodevelop
// 
// This file contains implementation of a simple linear memory allocator.
//
// Notes:
// - While there could be several representation of colors,
//   I would prefer to use floating point values between [0-1] 
//   to cover all general cases. If the user needs to convert it to a more
//   usable and concrete value, say, [0 - 255], they would just need to 
//   multiply the values by 255 and go from there.
//
// Todo:
// - HSV support?
// 

#ifndef MOMO_COLORS_H
#define MOMO_COLORS_H

#include "momo_common.h"

// Each component of RGBA are in the range [0 - 1].
struct RGB {
  F32 r, g, b;   
};

// Each component are in the range of [0 - 1].
// For hue, normally it is a number between [0 - 360], but
// it will be mapped linearly to [0 - 1] in this case.
// i.e. hue 0 is 0 degrees, hue 1 is 360 degrees.
struct HSL {
  F32 h, s, l;  
};

struct RGBA {
  union {
    struct { F32 r, g, b; };  
    RGB rgb;
  };
  F32 a;
};

static RGBA rgba(F32 r, F32 g, F32 b, F32 a);
static HSL  hsl(F32 h, F32 s, F32 l);
static HSL  rbg_to_hsl(RGB c);
static RGB  hsl_to_rgb(HSL c);

#include "momo_colors.cpp"

#endif //MOMO_COLORS_H

