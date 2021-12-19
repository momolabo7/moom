/* date = November 19th 2021 3:24 pm */

#ifndef MOMO_COLORS_H
#define MOMO_COLORS_H

typedef struct RGBF32 {
  F32 r, g, b; 
} RGBF32;

typedef union RGBAF32 {
	struct { F32 r, g, b, a; };  
  struct { RGBF32 rgb; F32 a; };
  
} RGBAF32;

static RGBAF32 RGBAF32_Create(F32 r, F32 g, F32 b, F32 a);


typedef struct HSLF32 {
} HSLF32;
// struct HSV {} ???


#if 0
static HSLF32 
RGBF32_ToHSLF32(RGBF32 c) {
  HSLF32 ret;
  F32 max = Max(Max(c.r, c.g), c.b);
  F32 min = Min(Min(c.r, c.g), c.b);
  
  F32 delta = max - min;
  
  
  //TODO
}

static HSLF32 
HSLF32_ToRGBF32(RGBF32 c) {
  
}
#endif

#endif //MOMO_COLORS_H

