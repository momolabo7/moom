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
typedef struct  {
  F32 r, g, b;   
} RGB;

// Each component are in the range of [0 - 1].
// For hue, normally it is a number between [0 - 360], but
// it will be mapped linearly to [0 - 1] in this case.
// i.e. hue 0 is 0 degrees, hue 1 is 360 degrees.
typedef struct {
  F32 h, s, l;  
} HSL;

typedef struct  {
  union {
    struct { F32 r, g, b; };  
    RGB rgb;
  };
  F32 a;
} RGBA;

static RGBA rgba(F32 r, F32 g, F32 b, F32 a);
static RGBA hex_to_rgba(U32 hex);  
static HSL  hsl(F32 h, F32 s, F32 l);
static HSL  rbg_to_hsl(RGB c);
static RGB  hsl_to_rgb(HSL c);

////////////////////////////////////////////////////////////
// IMPLEMENTATION
static RGBA 
rgba(F32 r, F32 g, F32 b, F32 a){
	RGBA ret;
	ret.r = r;
	ret.g = g;
	ret.b = b;
	ret.a = a;
	
	return ret;
}

static RGBA
hex_to_rgba(U32 hex) {
  RGBA ret;
  
  ret.r = (F32)((hex & 0xFF000000) >> 24)/255.f;
  ret.g = (F32)((hex & 0x00FF0000) >> 16)/255.f;
  ret.b = (F32)((hex & 0x0000FF00) >> 8)/255.f;
  ret.a = (F32)(hex & 0x000000FF)/255.f;
  
  return ret;
}

static HSL
hsl(F32 h, F32 s, F32 l) {
  HSL ret;
  ret.h = h;
  ret.s = s;
  ret.l = l;
  
  return ret;     
}

HSL 
rbg_to_hsl(RGB c) {
  assert(c.r >= 0.f &&
         c.r <= 1.f &&
         c.g >= 0.f &&
         c.g <= 1.f &&
         c.b >= 0.f &&
         c.b <= 1.f);
  HSL ret;
  F32 max = max_of(max_of(c.r, c.g), c.b);
  F32 min = min_of(min_of(c.r, c.g), c.b);
  
  F32 delta = max - min; // aka chroma
  
  
  if (is_close_f32(max, c.r)) {
    F32 segment = (c.g - c.b)/delta;
    F32 shift = 0.f / 60;
    if (segment < 0) {
      shift = 360 / 60;
    }
    else {
      shift = 0.f / 60;
    }
    ret.h = (segment + shift) * 60.f;
  }
  
  else if (is_close_f32(max, c.g)) {
    F32 segment = (c.b - c.r)/delta;
    F32 shift = 120.f / 60.f;
    ret.h = (segment + shift) * 60.f;
  }
  
  else if (is_close_f32(max, c.b)) {
    F32 segment = (c.r - c.g)/delta;
    F32 shift = 240.f / 60.f;
    ret.h = ((segment + shift) * 60.f);
  }
  else {
    ret.h = 0.f;
  }
  ret.h /= 360.f;
  
  
  ret.l = (max + min) * 0.5f;
  
  if (is_close_f32(delta, 0.f)) {
    ret.s = 0.f;
  }
  else {
    ret.s = delta/(1.f - abs_of(2.f * ret.l - 1.f));
  }
  
  return ret;
}

static F32 
_hue_to_color(F32 p, F32 q, F32 t) {
  
  if (t < 0) 
    t += 1.f;
  if (t > 1.f) 
    t -= 1.f;
  if (t < 1./6.f) 
    return p + (q - p) * 6.f * t;
  if (t < 1./2.f) 
    return q;
  if (t < 2./3.f)   
    return p + (q - p) * (2.f/3.f - t) * 6.f;
  
  return p;
  
}



static RGB 
hsl_to_rgb(HSL c) {
  assert(c.h >= 0.f &&
         c.h <= 360.f &&
         c.s >= 0.f &&
         c.s <= 1.f &&
         c.l >= 0.f &&
         c.l <= 1.f);
  RGB ret;
  if(is_close_f32(c.s, 0.f)) {
    ret.r = ret.g = ret.b = c.l; // achromatic
  }
  else {
    F32 q = c.l < 0.5f ? c.l * (1 + c.s) : c.l + c.s - c.l * c.s;
    F32 p = 2.f * c.l - q;
    ret.r = _hue_to_color(p, q, c.h + 1.f/3.f);
    ret.g = _hue_to_color(p, q, c.h);
    ret.b = _hue_to_color(p, q, c.h - 1.f/3.f);
  }
  
  
  return ret;
  
}


#endif //MOMO_COLORS_H

