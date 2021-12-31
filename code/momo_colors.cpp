
static RGBAF32 
RGBAF32_Create(F32 r, F32 g, F32 b, F32 a){
	RGBAF32 ret;
	ret.r = r;
	ret.g = g;
	ret.b = b;
	ret.a = a;
	
	return ret;
}

static HSLF32
HSLF32_Create(F32 h, F32 s, F32 l) {
  HSLF32 ret;
  ret.h = h;
  ret.s = s;
  ret.l = l;
  
  return ret;     
}

static HSLF32 
RGBF32_ToHSLF32(RGBF32 c) {
  Assert(c.r >= 0.f &&
         c.r <= 1.f &&
         c.g >= 0.f &&
         c.g <= 1.f &&
         c.b >= 0.f &&
         c.b <= 1.f);
  HSLF32 ret;
  F32 max = Max(Max(c.r, c.g), c.b);
  F32 min = Min(Min(c.r, c.g), c.b);
  
  F32 delta = max - min; // aka chroma
  
  
  if (F32_Match(max, c.r)) {
    F32 segment = (c.g - c.b)/delta;
    F32 shift = 0 / 60;
    if (segment < 0) {
      shift = 360 / 60;
    }
    else {
      shift = 0.f / 60;
    }
    ret.h = (segment + shift) * 60.f;
  }
  
  else if (F32_Match(max, c.g)) {
    F32 segment = (c.b - c.r)/delta;
    F32 shift = 120.f / 60.f;
    ret.h = (segment + shift) * 60.f;
  }
  
  else if (F32_Match(max, c.b)) {
    F32 segment = (c.r - c.g)/delta;
    F32 shift = 240.f / 60.f;
    ret.h = ((segment + shift) * 60.f);
  }
  else {
    ret.h = 0.f;
  }
  ret.h /= 360.f;
  
  
  ret.l = (max + min) * 0.5f;
  
  if (F32_Match(delta, 0.f)) {
    ret.s = 0.f;
  }
  else {
    ret.s = delta/(1.f - F32_Abs(2.f * ret.l - 1.f));
  }
  
  return ret;
}

static F32 
HSLF32__HueToColor(F32 p, F32 q, F32 t) {
  
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



static RGBF32 
HSLF32_ToRGBF32(HSLF32 c) {
  Assert(c.h >= 0.f &&
         c.h <= 360.f &&
         c.s >= 0.f &&
         c.s <= 1.f &&
         c.l >= 0.f &&
         c.l <= 1.f);
  RGBF32 ret;
  if(F32_Match(c.s, 0.f)) {
    ret.r = ret.g = ret.b = c.l; // achromatic
  }
  else {
    F32 q = c.l < 0.5f ? c.l * (1 + c.s) : c.l + c.s - c.l * c.s;
    F32 p = 2.f * c.l - q;
    ret.r = HSLF32__HueToColor(p, q, c.h + 1.f/3.f);
    ret.g = HSLF32__HueToColor(p, q, c.h);
    ret.b = HSLF32__HueToColor(p, q, c.h - 1.f/3.f);
  }
  
  
  return ret;
  
}


