#ifndef MOMO_EASING_H
#define MOMO_EASING_H

static F32 ease_in_sine_f32(F32 t);
static F32 ease_out_sine_f32(F32 t);
static F32 ease_inout_sine_f32(F32 t);
static F32 ease_in_quad_f32(F32 t);
static F32 ease_out_quad_f32(F32 t);
static F32 ease_inout_quad_f32(F32 t);
static F32 ease_in_cubic_f32(F32 t);
static F32 ease_out_cubic_f32(F32 t);
static F32 ease_inout_cubic_f32(F32 t);
static F32 ease_in_quart_f32(F32 t);
static F32 ease_out_quart_f32(F32 t);
static F32 ease_inout_quart_f32(F32 t);
static F32 ease_in_quint_f32(F32 t);
static F32 ease_out_quint_f32(F32 t);;
static F32 ease_inout_quint_f32(F32 t);
static F32 ease_in_circ_f32(F32 t);
static F32 ease_out_circ_f32(F32 t);
static F32 ease_inout_circ_f32(F32 t);
static F32 ease_in_back_f32(F32 t);
static F32 ease_out_back_f32(F32 t);
static F32 ease_inout_back_f32(F32 t);
static F32 ease_in_elastic_f32(F32 t);
static F32 ease_out_elastic_f32(F32 t);
static F32 ease_inout_elastic_f32(F32 t);
static F32 ease_in_bounce_f32(F32 t);
static F32 ease_out_bounce_f32(F32 t);
static F32 ease_inout_bounce_f32(F32 t);
static F32 ease_in_expo_f32(F32 t);
static F32 ease_out_expo_f32(F32 t);
static F32 ease_inout_expo_f32(F32 t);

static F64 ease_in_sine_f64(F64 t);
static F64 ease_out_sine_f64(F64 t);
static F64 ease_inout_sine_f64(F64 t);
static F64 ease_in_quad_f64(F64 t);
static F64 ease_out_quad_f64(F64 t);
static F64 ease_inout_quad_f64(F64 t);
static F64 ease_in_cubic_f64(F64 t);
static F64 ease_out_cubic_f64(F64 t);
static F64 ease_inout_cubic_f64(F64 t);
static F64 ease_in_quart_f64(F64 t);
static F64 ease_out_quart_f64(F64 t);
static F64 ease_inout_quart_f64(F64 t);
static F64 ease_in_quint_f64(F64 t);
static F64 ease_out_quint_f64(F64 t);;
static F64 ease_inout_quint_f64(F64 t);
static F64 ease_in_circ_f64(F64 t);
static F64 ease_out_circ_f64(F64 t);
static F64 ease_inout_circ_f64(F64 t);
static F64 ease_in_back_f64(F64 t);
static F64 ease_out_back_f64(F64 t);
static F64 ease_inout_back_f64(F64 t);
static F64 ease_in_elastic_f64(F64 t);
static F64 ease_out_elastic_f64(F64 t);
static F64 ease_inout_elastic_f64(F64 t);
static F64 ease_in_bounce_f64(F64 t);
static F64 ease_out_bounce_f64(F64 t);
static F64 ease_inout_bounce_f64(F64 t);
static F64 ease_in_expo_f64(F64 t);
static F64 ease_out_expo_f64(F64 t);
static F64 ease_inout_expo_f64(F64 t);

/////////////////////////////////////////////////////////
// IMPLEMENTATION
static F32 
ease_in_sine_f32(F32 t)  {
  return sin_f32(PI_32 * 0.5f * t);
}


static F32 
ease_out_sine_f32(F32 t) {
  return 1.0f + sin_f32(PI_32 * 0.5f * (--t));
}

static F32 
ease_inout_sine_f32(F32 t)  {
  return 0.5f * (1.f + sin_f32(PI_32 * (t - 0.5f)));
}

static F32 
ease_in_quad_f32(F32 t)  {
  return t * t;
}

static F32 
ease_out_quad_f32(F32 t)  {
  return t * (2.f -t);
}

static F32 
ease_inout_quad_f32(F32 t)  {
  return t < 0.5f ? 2.f * t * t : t * (4.f -2.f * t) - 1.f;
}

static F32 
ease_in_cubic_f32(F32 t)  {
  return t * t * t;
}

static F32 
ease_out_cubic_f32(F32 t)  {
  return 1.f + (t-1) * (t-1) * (t-1);
}

static F32 
ease_inout_cubic_f32(F32 t)  {
  return t < 0.5f ? 4.f * t * t * t : 1.f + (t-1) * (2.f * (t-2)) * (2.f * (t-2));
}

static F32 
ease_in_quart_f32(F32 t)  {
  t *= t;
  return t * t;
}

static F32 
ease_out_quart_f32(F32 t) {
  --t;
  t = t * t;
  return 1.f - t * t;
}

static F32 
ease_inout_quart_f32(F32 t)  {
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

static F32
ease_in_quint_f32(F32 t)  {
  F32 t2 = t * t;
  return t * t2 * t2;
}

static F32
ease_out_quint_f32(F32 t)  {
  --t;
  F32 t2 = t * t;
  return 1.f +t * t2 * t2;
}

static F32
ease_inout_quint_f32(F32 t)  {
  F32 t2;
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



static F32 
ease_in_circ_f32(F32 t)  {
  return 1.f -sqrt_f32(1.f -t);
}

static F32 
ease_out_circ_f32(F32 t)  {
  return sqrt_f32(t);
}

static F32 
ease_inout_circ(F32 t)  {
  if (t < 0.5f) {
    return (1.f -sqrt_f32(1.f -2.f * t)) * 0.5f;
  }
  else {
    return (1.f +sqrt_f32(2.f * t - 1.f)) * 0.5f;
  }
}

static F32 
ease_in_back_f32(F32 t)  {
  return t * t * (2.7f * t - 1.7f);
}

static F32 
ease_out_back_f32(F32 t)  {
  --t;
  return 1.f + t * t * (2.7f * t + 1.7f);
}

static F32 
ease_inout_back_f32(F32 t)  {
  if (t < 0.5f) {
    return t * t * (7.f * t - 2.5f) * 2.f;
  }
  else {
    --t;
    return 1.f + t * t * 2.f * (7.f * t + 2.5f);
  }
}

static F32 
ease_in_elastic_f32(F32 t)  {
  F32 t2 = t * t;
  return t2 * t2 * sin_f32(t * PI_32 * 4.5f);
}

static F32 
ease_out_elastic_f32(F32 t)  {
  F32 t2 = (t - 1.f) * (t - 1.f);
  return 1.f -t2 * t2 * cos_f32(t * PI_32 * 4.5f);
}

static F32 
ease_inout_elastic_f32(F32 t)  {
  F32 t2;
  if (t < 0.45f) {
    t2 = t * t;
    return 8.f * t2 * t2 * sin_f32(t * PI_32 * 9.f);
  }
  else if (t < 0.55f) {
    return 0.5f +0.75f * sin_f32(t * PI_32 * 4.f);
  }
  else {
    t2 = (t - 1.f) * (t - 1.f);
    return 1.f -8.f * t2 * t2 * sin_f32(t * PI_32 * 9.f);
  }
}

static F32 
ease_in_bounce_f32(F32 t)  {
  return pow_f32(2.f, 6.f * (t - 1.f)) * abs_f32(sin_f32(t * PI_32 * 3.5f));
}


static F32 
ease_out_bounce_f32(F32 t) {
  return 1.f -pow_f32(2.f, -6.f * t) * abs_f32(cos_f32(t * PI_32 * 3.5f));
}

static F32 
ease_inout_bounce_f32(F32 t) {
  if (t < 0.5f) {
    return 8.f * pow_f32(2.f, 8.f * (t - 1.f)) * abs_f32(sin_f32(t * PI_32 * 7.f));
  }
  else {
    return 1.f -8.f * pow_f32(2.f, -8.f * t) * abs_f32(sin_f32(t * PI_32 * 7.f));
  }
}

static F32
ease_in_expo_f32(F32 t)  {
  return (pow_f32(2.f, 8.f * t) - 1.f) / 255.f;
}


static F32 
ease_out_expo_f32(F32 t)  {
  return t == 1.f ? 1.f : 1.f -pow_f32(2.f, -10.f * t);
}

static F32 
ease_inout_expo_f32(F32 t)  {
  if (t < 0.5f) {
    return (pow_f32(2.f, 16.f * t) - 1.f) / 510.f;
  }
  else {
    return 1.f -0.5f * pow_f32(2.f, -16.f * (t - 0.5f));
  }
}



static F64 
ease_in_sine_f64(F64 t)  {
  return sin_f64(PI_64 * 0.5 * t);
}


static F64 
ease_out_sine_f64(F64 t) {
  return 1.0f + sin_f64(PI_64 * 0.5 * (--t));
}

static F64 
ease_inout_sine_f64(F64 t)  {
  return 0.5 * (1.0 + sin_f64(PI_64 * (t - 0.5)));
}

static F64 
ease_in_quad_f64(F64 t)  {
  return t * t;
}

static F64 
ease_out_quad_f64(F64 t)  {
  return t * (2.0 -t);
}

static F64 
ease_inout_quad_f64(F64 t)  {
  return t < 0.5 ? 2.0 * t * t : t * (4.0 -2.0 * t) - 1.0;
}

static F64 
ease_in_cubic_f64(F64 t)  {
  return t * t * t;
}

static F64 
ease_out_cubic_f64(F64 t)  {
  return 1.0 + (t-1) * (t-1) * (t-1);
}

static F64 
ease_inout_cubic_f64(F64 t)  {
  return t < 0.5 ? 4.0 * t * t * t : 1.0 + (t-1) * (2.0 * (t-2)) * (2.0 * (t-2));
}

static F64 
ease_in_quart_f64(F64 t)  {
  t *= t;
  return t * t;
}

static F64 
ease_out_quart_f64(F64 t) {
  --t;
  t = t * t;
  return 1.0 - t * t;
}

static F64 
ease_inout_quart_f64(F64 t)  {
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

static F64
ease_in_quint_f64(F64 t)  {
  F64 t2 = t * t;
  return t * t2 * t2;
}

static F64
ease_out_quint_f64(F64 t)  {
  --t;
  F64 t2 = t * t;
  return 1.0 +t * t2 * t2;
}

static F64
ease_inout_quint_f64(F64 t)  {
  F64 t2;
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



static F64 
ease_in_circ_f64(F64 t)  {
  return 1.0 -sqrt_f64(1.0 -t);
}

static F64 
ease_out_circ_f64(F64 t)  {
  return sqrt_f64(t);
}

static F64 
ease_inout_circ_f64(F64 t)  {
  if (t < 0.5) {
    return (1.0 -sqrt_f64(1.0 -2.0 * t)) * 0.5;
  }
  else {
    return (1.0 +sqrt_f64(2.0 * t - 1.0)) * 0.5;
  }
}

static F64 
ease_in_back_f64(F64 t)  {
  return t * t * (2.7 * t - 1.7);
}

static F64 
ease_out_back_f64(F64 t)  {
  --t;
  return 1.0 + t * t * (2.7 * t + 1.7);
}

static F64 
ease_inout_back_f64(F64 t)  {
  if (t < 0.5) {
    return t * t * (7.0 * t - 2.5) * 2.0;
  }
  else {
    --t;
    return 1.0 + t * t * 2.0 * (7.0 * t + 2.5);
  }
}

static F64 
ease_in_elastic_f64(F64 t)  {
  F64 t2 = t * t;
  return t2 * t2 * sin_f64(t * PI_64 * 4.5);
}

static F64 
ease_out_elastic_f64(F64 t)  {
  F64 t2 = (t - 1.0) * (t - 1.0);
  return 1.0 -t2 * t2 * cos_f64(t * PI_64 * 4.5);
}

static F64 
ease_inout_elastic_f64(F64 t)  {
  F64 t2;
  if (t < 0.45) {
    t2 = t * t;
    return 8.0 * t2 * t2 * sin_f64(t * PI_64 * 9.0);
  }
  else if (t < 0.55) {
    return 0.5 +0.75 * sin_f64(t * PI_64 * 4.0);
  }
  else {
    t2 = (t - 1.0) * (t - 1.0);
    return 1.0 -8.0 * t2 * t2 * sin_f64(t * PI_64 * 9.0);
  }
}



// NOTE(Momo): These require power function. 
static F64 
ease_in_bounce_f64(F64 t)  {
  return pow_f64(2.0, 6.0 * (t - 1.0)) * abs_f64(sin_f64(t * PI_64 * 3.5));
}


static F64 
ease_out_bounce_f64(F64 t) {
  return 1.0 -pow_f64(2.0, -6.0 * t) * abs_f64(cos_f64(t * PI_64 * 3.5));
}

static F64 
ease_inout_bounce_f64(F64 t) {
  if (t < 0.5) {
    return 8.0 * pow_f64(2.0, 8.0 * (t - 1.0)) * abs_f64(sin_f64(t * PI_64 * 7.0));
  }
  else {
    return 1.0 -8.0 * pow_f64(2.0, -8.0 * t) * abs_f64(sin_f64(t * PI_64 * 7.0));
  }
}

static F64
ease_in_expo_f64(F64 t)  {
  return (pow_f64(2.0, 8.0 * t) - 1.0) / 255.0;
}


static F64 
ease_out_expo_f64(F64 t)  {
  return t == 1.0 ? 1.0 : 1.0 -pow_f64(2.0, -10.0 * t);
}

static F64 
ease_inout_expo_f64(F64 t)  {
  if (t < 0.5) {
    return (pow_f64(2.0, 16.0 * t) - 1.0) / 510.0;
  }
  else {
    return 1.0 -0.5 * pow_f64(2.0, -16.0 * (t - 0.5));
  }
}


#endif //MOMO_EASING_H
