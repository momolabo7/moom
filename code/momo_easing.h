#ifndef MOMO_EASING_H
#define MOMO_EASING_H

static f32_t f32_ease_linear(f32_t t);
static f32_t f32_ease_in_sine(f32_t t);
static f32_t f32_ease_out_sine(f32_t t);
static f32_t f32_ease_inout_sine(f32_t t);
static f32_t f32_ease_in_quad(f32_t t);
static f32_t f32_ease_out_quad(f32_t t);
static f32_t f32_ease_inout_quad(f32_t t);
static f32_t f32_ease_in_cubic(f32_t t);
static f32_t f32_ease_out_cubic(f32_t t);
static f32_t f32_ease_inout_cubic(f32_t t);
static f32_t f32_ease_in_quart(f32_t t);
static f32_t f32_ease_out_quart(f32_t t);
static f32_t f32_ease_inout_quart(f32_t t);
static f32_t f32_ease_in_quint(f32_t t);
static f32_t f32_ease_out_quint(f32_t t);;
static f32_t f32_ease_inout_quint(f32_t t);
static f32_t f32_ease_in_circ(f32_t t);
static f32_t f32_ease_out_circ(f32_t t);
static f32_t f32_ease_inout_circ(f32_t t);
static f32_t f32_ease_in_back(f32_t t);
static f32_t f32_ease_out_back(f32_t t);
static f32_t f32_ease_inout_back(f32_t t);
static f32_t f32_ease_in_elastic(f32_t t);
static f32_t f32_ease_out_elastic(f32_t t);
static f32_t f32_ease_inout_elastic(f32_t t);
static f32_t f32_ease_in_bounce(f32_t t);
static f32_t f32_ease_out_bounce(f32_t t);
static f32_t f32_ease_inout_bounce(f32_t t);
static f32_t f32_ease_in_expo(f32_t t);
static f32_t f32_ease_out_expo(f32_t t);
static f32_t f32_ease_inout_expo(f32_t t);

static f64_t f64_ease_linear(f64_t t);
static f64_t f64_ease_in_sine(f64_t t);
static f64_t f64_ease_out_sine(f64_t t);
static f64_t f64_ease_inout_sine(f64_t t);
static f64_t f64_ease_in_quad(f64_t t);
static f64_t f64_ease_out_quad(f64_t t);
static f64_t f64_ease_inout_quad(f64_t t);
static f64_t f64_ease_in_cubic(f64_t t);
static f64_t f64_ease_out_cubic(f64_t t);
static f64_t f64_ease_inout_cubic(f64_t t);
static f64_t f64_ease_in_quart(f64_t t);
static f64_t f64_ease_out_quart(f64_t t);
static f64_t f64_ease_inout_quart(f64_t t);
static f64_t f64_ease_in_quint(f64_t t);
static f64_t f64_ease_out_quint(f64_t t);;
static f64_t f64_ease_inout_quint(f64_t t);
static f64_t f64_ease_in_circ(f64_t t);
static f64_t f64_ease_out_circ(f64_t t);
static f64_t f64_ease_inout_circ(f64_t t);
static f64_t f64_ease_in_back(f64_t t);
static f64_t f64_ease_out_back(f64_t t);
static f64_t f64_ease_inout_back(f64_t t);
static f64_t f64_ease_in_elastic(f64_t t);
static f64_t f64_ease_out_elastic(f64_t t);
static f64_t f64_ease_inout_elastic(f64_t t);
static f64_t f64_ease_in_bounce(f64_t t);
static f64_t f64_ease_out_bounce(f64_t t);
static f64_t f64_ease_inout_bounce(f64_t t);
static f64_t f64_ease_in_expo(f64_t t);
static f64_t f64_ease_out_expo(f64_t t);
static f64_t f64_ease_inout_expo(f64_t t);

/////////////////////////////////////////////////////////
// IMPLEMENTATION
//
static f32_t 
f32_ease_linear(f32_t t) 
{
  return t;
}

static f32_t 
f32_ease_in_sine(f32_t t)  {
  return f32_sin(PI_32 * 0.5f * t);
}


static f32_t 
f32_ease_out_sine(f32_t t) {
  return 1.0f + f32_sin(PI_32 * 0.5f * (--t));
}

static f32_t 
f32_ease_inout_sine(f32_t t)  {
  return 0.5f * (1.f + f32_sin(PI_32 * (t - 0.5f)));
}

static f32_t 
f32_ease_in_quad(f32_t t)  {
  return t * t;
}

static f32_t 
f32_ease_out_quad(f32_t t)  {
  return t * (2.f -t);
}

static f32_t 
f32_ease_inout_quad(f32_t t)  {
  return t < 0.5f ? 2.f * t * t : t * (4.f -2.f * t) - 1.f;
}

static f32_t 
f32_ease_in_cubic(f32_t t)  {
  return t * t * t;
}

static f32_t 
f32_ease_out_cubic(f32_t t)  {
  return 1.f + (t-1) * (t-1) * (t-1);
}

static f32_t 
f32_ease_inout_cubic(f32_t t)  {
  return t < 0.5f ? 4.f * t * t * t : 1.f + (t-1) * (2.f * (t-2)) * (2.f * (t-2));
}

static f32_t 
f32_ease_in_quart(f32_t t)  {
  t *= t;
  return t * t;
}

static f32_t 
f32_ease_out_quart(f32_t t) {
  --t;
  t = t * t;
  return 1.f - t * t;
}

static f32_t 
f32_ease_inout_quart(f32_t t)  {
  if (t < 0.5f) {
    t *= t;
    return 8.f * t * t;
  }
  else {
    --t;
    t = t * t;
    return 1.f -8.f * t * t;
  }
}

static f32_t
f32_ease_in_quint(f32_t t)  {
  f32_t t2 = t * t;
  return t * t2 * t2;
}

static f32_t
f32_ease_out_quint(f32_t t)  {
  --t;
  f32_t t2 = t * t;
  return 1.f +t * t2 * t2;
}

static f32_t
f32_ease_inout_quint(f32_t t)  {
  f32_t t2;
  if (t < 0.5f) {
    t2 = t * t;
    return 16.f * t * t2 * t2;
  }
  else {
    --t;
    t2 = t * t;
    return 1.f +16.f * t * t2 * t2;
  }
}



static f32_t 
f32_ease_in_circ(f32_t t)  {
  return 1.f -f32_sqrt(1.f -t);
}

static f32_t 
f32_ease_out_circ(f32_t t)  {
  return f32_sqrt(t);
}

static f32_t 
f32_ease_inout_circ(f32_t t)  {
  if (t < 0.5f) {
    return (1.f -f32_sqrt(1.f -2.f * t)) * 0.5f;
  }
  else {
    return (1.f +f32_sqrt(2.f * t - 1.f)) * 0.5f;
  }
}

static f32_t 
f32_ease_in_back(f32_t t)  {
  return t * t * (2.7f * t - 1.7f);
}

static f32_t 
f32_ease_out_back(f32_t t)  {
  --t;
  return 1.f + t * t * (2.7f * t + 1.7f);
}

static f32_t 
f32_ease_inout_back(f32_t t)  {
  if (t < 0.5f) {
    return t * t * (7.f * t - 2.5f) * 2.f;
  }
  else {
    --t;
    return 1.f + t * t * 2.f * (7.f * t + 2.5f);
  }
}

static f32_t 
f32_ease_in_elastic(f32_t t)  {
  f32_t t2 = t * t;
  return t2 * t2 * f32_sin(t * PI_32 * 4.5f);
}

static f32_t 
f32_ease_out_elastic(f32_t t)  {
  f32_t t2 = (t - 1.f) * (t - 1.f);
  return 1.f -t2 * t2 * f32_cos(t * PI_32 * 4.5f);
}

static f32_t 
f32_ease_inout_elastic(f32_t t)  {
  f32_t t2;
  if (t < 0.45f) {
    t2 = t * t;
    return 8.f * t2 * t2 * f32_sin(t * PI_32 * 9.f);
  }
  else if (t < 0.55f) {
    return 0.5f +0.75f * f32_sin(t * PI_32 * 4.f);
  }
  else {
    t2 = (t - 1.f) * (t - 1.f);
    return 1.f -8.f * t2 * t2 * f32_sin(t * PI_32 * 9.f);
  }
}

static f32_t 
f32_ease_in_bounce(f32_t t)  {
  return f32_pow(2.f, 6.f * (t - 1.f)) * f32_abs(f32_sin(t * PI_32 * 3.5f));
}


static f32_t 
f32_ease_out_bounce(f32_t t) {
  return 1.f -f32_pow(2.f, -6.f * t) * f32_abs(f32_cos(t * PI_32 * 3.5f));
}

static f32_t 
f32_ease_inout_bounce(f32_t t) {
  if (t < 0.5f) {
    return 8.f * f32_pow(2.f, 8.f * (t - 1.f)) * f32_abs(f32_sin(t * PI_32 * 7.f));
  }
  else {
    return 1.f -8.f * f32_pow(2.f, -8.f * t) * f32_abs(f32_sin(t * PI_32 * 7.f));
  }
}

static f32_t
f32_ease_in_expo(f32_t t)  {
  return (f32_pow(2.f, 8.f * t) - 1.f) / 255.f;
}


static f32_t 
f32_ease_out_expo(f32_t t)  {
  return t == 1.f ? 1.f : 1.f -f32_pow(2.f, -10.f * t);
}

static f32_t 
f32_ease_inout_expo(f32_t t)  {
  if (t < 0.5f) {
    return (f32_pow(2.f, 16.f * t) - 1.f) / 510.f;
  }
  else {
    return 1.f -0.5f * f32_pow(2.f, -16.f * (t - 0.5f));
  }
}

static f64_t 
f64_ease_linear(f64_t t) 
{
  return t;
}

static f64_t 
f64_ease_in_sine(f64_t t)  {
  return f64_sin(PI_64 * 0.5 * t);
}


static f64_t 
f64_ease_out_sine(f64_t t) {
  return 1.0f + f64_sin(PI_64 * 0.5 * (--t));
}

static f64_t 
f64_ease_inout_sine(f64_t t)  {
  return 0.5 * (1.0 + f64_sin(PI_64 * (t - 0.5)));
}

static f64_t 
f64_ease_in_quad(f64_t t)  {
  return t * t;
}

static f64_t 
f64_ease_out_quad(f64_t t)  {
  return t * (2.0 -t);
}

static f64_t 
f64_ease_inout_quad(f64_t t)  {
  return t < 0.5 ? 2.0 * t * t : t * (4.0 -2.0 * t) - 1.0;
}

static f64_t 
f64_ease_in_cubic(f64_t t)  {
  return t * t * t;
}

static f64_t 
f64_ease_out_cubic(f64_t t)  {
  return 1.0 + (t-1) * (t-1) * (t-1);
}

static f64_t 
f64_ease_inout_cubic(f64_t t)  {
  return t < 0.5 ? 4.0 * t * t * t : 1.0 + (t-1) * (2.0 * (t-2)) * (2.0 * (t-2));
}

static f64_t 
f64_ease_in_quart(f64_t t)  {
  t *= t;
  return t * t;
}

static f64_t 
f64_ease_out_quart(f64_t t) {
  --t;
  t = t * t;
  return 1.0 - t * t;
}

static f64_t 
f64_ease_inout_quart(f64_t t)  {
  if (t < 0.5) {
    t *= t;
    return 8.0 * t * t;
  }
  else {
    --t;
    t = t * t;
    return 1.0 -8.0 * t * t;
  }
}

static f64_t
f64_ease_in_quint(f64_t t)  {
  f64_t t2 = t * t;
  return t * t2 * t2;
}

static f64_t
f64_ease_out_quint(f64_t t)  {
  --t;
  f64_t t2 = t * t;
  return 1.0 +t * t2 * t2;
}

static f64_t
f64_ease_inout_quint(f64_t t)  {
  f64_t t2;
  if (t < 0.5) {
    t2 = t * t;
    return 16.0 * t * t2 * t2;
  }
  else {
    --t;
    t2 = t * t;
    return 1.0 +16.0 * t * t2 * t2;
  }
}



static f64_t 
f64_ease_in_circ(f64_t t)  {
  return 1.0 -f64_sqrt(1.0 -t);
}

static f64_t 
f64_ease_out_circ(f64_t t)  {
  return f64_sqrt(t);
}

static f64_t 
f64_ease_inout_circ(f64_t t)  {
  if (t < 0.5) {
    return (1.0 -f64_sqrt(1.0 -2.0 * t)) * 0.5;
  }
  else {
    return (1.0 +f64_sqrt(2.0 * t - 1.0)) * 0.5;
  }
}

static f64_t 
f64_ease_in_back(f64_t t)  {
  return t * t * (2.7 * t - 1.7);
}

static f64_t 
f64_ease_out_back(f64_t t)  {
  --t;
  return 1.0 + t * t * (2.7 * t + 1.7);
}

static f64_t 
f64_ease_inout_back(f64_t t)  {
  if (t < 0.5) {
    return t * t * (7.0 * t - 2.5) * 2.0;
  }
  else {
    --t;
    return 1.0 + t * t * 2.0 * (7.0 * t + 2.5);
  }
}

static f64_t 
f64_ease_in_elastic(f64_t t)  {
  f64_t t2 = t * t;
  return t2 * t2 * f64_sin(t * PI_64 * 4.5);
}

static f64_t 
f64_ease_out_elastic(f64_t t)  {
  f64_t t2 = (t - 1.0) * (t - 1.0);
  return 1.0 -t2 * t2 * f64_cos(t * PI_64 * 4.5);
}

static f64_t 
f64_ease_inout_elastic(f64_t t)  {
  f64_t t2;
  if (t < 0.45) {
    t2 = t * t;
    return 8.0 * t2 * t2 * f64_sin(t * PI_64 * 9.0);
  }
  else if (t < 0.55) {
    return 0.5 +0.75 * f64_sin(t * PI_64 * 4.0);
  }
  else {
    t2 = (t - 1.0) * (t - 1.0);
    return 1.0 -8.0 * t2 * t2 * f64_sin(t * PI_64 * 9.0);
  }
}



// NOTE(Momo): These require power function. 
static f64_t 
f64_ease_in_bounce(f64_t t)  {
  return f64_pow(2.0, 6.0 * (t - 1.0)) * f64_abs(f64_sin(t * PI_64 * 3.5));
}


static f64_t 
f64_ease_out_bounce(f64_t t) {
  return 1.0 -f64_pow(2.0, -6.0 * t) * f64_abs(f64_cos(t * PI_64 * 3.5));
}

static f64_t 
f64_ease_inout_bounce(f64_t t) {
  if (t < 0.5) {
    return 8.0 * f64_pow(2.0, 8.0 * (t - 1.0)) * f64_abs(f64_sin(t * PI_64 * 7.0));
  }
  else {
    return 1.0 -8.0 * f64_pow(2.0, -8.0 * t) * f64_abs(f64_sin(t * PI_64 * 7.0));
  }
}

static f64_t
f64_ease_in_expo(f64_t t)  {
  return (f64_pow(2.0, 8.0 * t) - 1.0) / 255.0;
}


static f64_t 
f64_ease_out_expo(f64_t t)  {
  return t == 1.0 ? 1.0 : 1.0 -f64_pow(2.0, -10.0 * t);
}

static f64_t 
f64_ease_inout_expo(f64_t t)  {
  if (t < 0.5) {
    return (f64_pow(2.0, 16.0 * t) - 1.0) / 510.0;
  }
  else {
    return 1.0 -0.5 * f64_pow(2.0, -16.0 * (t - 0.5));
  }
}


#endif //MOMO_EASING_H
