/* date = November 19th 2021 3:24 pm */

#ifndef MOMO_COLORS_H
#define MOMO_COLORS_H


// NOTE(Momo): Each component of RGBA are in the range [0 - 1].
typedef struct {
  F32 r, g, b; 
} RGBF32;

typedef struct {
  union {
    struct { F32 r, g, b; };  
    RGBF32 rgb;
  };
  F32 a;
} RGBAF32;



// NOTE(Momo): Each component are in the range of [0 - 1].
// For hue, normally it is a number between [0 - 360], but
// it will be mapped linearly to [0 - 1] in this case.
// i.e. hue 0 is 0 degrees, hue 1 is 360 degrees.
typedef union {
  struct { F32 h, s, l; };
} HSLF32;


static RGBAF32 RGBAF32_Create(F32 r, F32 g, F32 b, F32 a);
static HSLF32 HSLF32_Create(F32 h, F32 s, F32 l);
static RGBF32 HSLF32_ToRGBF32(HSLF32 c);


#endif //MOMO_COLORS_H

