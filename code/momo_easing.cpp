
static F32 
ease_in_sine(F32 t)  {
  return sin(PI_32 * 0.5f * t);
}


static F32 
ease_out_sine(F32 t) {
  return 1.0f + sin(PI_32 * 0.5f * (--t));
}

static F32 
ease_inout_sine(F32 t)  {
  return 0.5f * (1.f + sin(PI_32 * (t - 0.5f)));
}

static F32 
ease_in_quad(F32 t)  {
  return t * t;
}

static F32 
ease_out_quad(F32 t)  {
  return t * (2.f -t);
}

static F32 
ease_inout_quad(F32 t)  {
  return t < 0.5f ? 2.f * t * t : t * (4.f -2.f * t) - 1.f;
}

static F32 
ease_in_cubic(F32 t)  {
  return t * t * t;
}

static F32 
ease_out_cubic(F32 t)  {
  return 1.f + (t-1) * (t-1) * (t-1);
}

static F32 
ease_inout_cubic(F32 t)  {
  return t < 0.5f ? 4.f * t * t * t : 1.f + (t-1) * (2.f * (t-2)) * (2.f * (t-2));
}

static F32 
ease_in_quart(F32 t)  {
  t *= t;
  return t * t;
}

static F32 
ease_out_quart(F32 t) {
  --t;
  t = t * t;
  return 1.f - t * t;
}

static F32 
ease_inout_quart(F32 t)  {
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
ease_in_quint(F32 t)  {
  F32 t2 = t * t;
  return t * t2 * t2;
}

static F32
ease_out_quint(F32 t)  {
  --t;
  F32 t2 = t * t;
  return 1.f +t * t2 * t2;
}

static F32
ease_inout_quint(F32 t)  {
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
ease_in_circ(F32 t)  {
  return 1.f -sqrt(1.f -t);
}

static F32 
ease_out_circ(F32 t)  {
  return sqrt(t);
}

static F32 
ease_inout_circ(F32 t)  {
  if (t < 0.5f) {
    return (1.f -sqrt(1.f -2.f * t)) * 0.5f;
  }
  else {
    return (1.f +sqrt(2.f * t - 1.f)) * 0.5f;
  }
}

static F32 
ease_in_back(F32 t)  {
  return t * t * (2.7f * t - 1.7f);
}

static F32 
ease_out_back(F32 t)  {
  --t;
  return 1.f + t * t * (2.7f * t + 1.7f);
}

static F32 
ease_inout_back(F32 t)  {
  if (t < 0.5f) {
    return t * t * (7.f * t - 2.5f) * 2.f;
  }
  else {
    --t;
    return 1.f + t * t * 2.f * (7.f * t + 2.5f);
  }
}

static F32 
ease_in_elastic(F32 t)  {
  F32 t2 = t * t;
  return t2 * t2 * sin(t * PI_32 * 4.5f);
}

static F32 
ease_out_elastic(F32 t)  {
  F32 t2 = (t - 1.f) * (t - 1.f);
  return 1.f -t2 * t2 * cos(t * PI_32 * 4.5f);
}

static F32 
ease_inout_elastic(F32 t)  {
  F32 t2;
  if (t < 0.45f) {
    t2 = t * t;
    return 8.f * t2 * t2 * sin(t * PI_32 * 9.f);
  }
  else if (t < 0.55f) {
    return 0.5f +0.75f * sin(t * PI_32 * 4.f);
  }
  else {
    t2 = (t - 1.f) * (t - 1.f);
    return 1.f -8.f * t2 * t2 * sin(t * PI_32 * 9.f);
  }
}

static F32 
ease_in_bounce(F32 t)  {
  return pow(2.f, 6.f * (t - 1.f)) * abs_of(sin(t * PI_32 * 3.5f));
}


static F32 
ease_out_bounce(F32 t) {
  return 1.f -pow(2.f, -6.f * t) * abs_of(cos(t * PI_32 * 3.5f));
}

static F32 
ease_inout_bounce(F32 t) {
  if (t < 0.5f) {
    return 8.f * pow(2.f, 8.f * (t - 1.f)) * abs_of(sin(t * PI_32 * 7.f));
  }
  else {
    return 1.f -8.f * pow(2.f, -8.f * t) * abs_of(sin(t * PI_32 * 7.f));
  }
}

static F32
ease_in_expo(F32 t)  {
  return (pow(2.f, 8.f * t) - 1.f) / 255.f;
}


static F32 
ease_out_expo(F32 t)  {
  return t == 1.f ? 1.f : 1.f -pow(2.f, -10.f * t);
}

static F32 
ease_inout_expo(F32 t)  {
  if (t < 0.5f) {
    return (pow(2.f, 16.f * t) - 1.f) / 510.f;
  }
  else {
    return 1.f -0.5f * pow(2.f, -16.f * (t - 0.5f));
  }
}



static F64 
ease_in_sine(F64 t)  {
  return sin(PI_64 * 0.5 * t);
}


static F64 
ease_out_sine(F64 t) {
  return 1.0f + sin(PI_64 * 0.5 * (--t));
}

static F64 
ease_inout_sine(F64 t)  {
  return 0.5 * (1.0 + sin(PI_64 * (t - 0.5)));
}

static F64 
ease_in_quad(F64 t)  {
  return t * t;
}

static F64 
ease_out_quad(F64 t)  {
  return t * (2.0 -t);
}

static F64 
ease_inout_quad(F64 t)  {
  return t < 0.5 ? 2.0 * t * t : t * (4.0 -2.0 * t) - 1.0;
}

static F64 
ease_in_cubic(F64 t)  {
  return t * t * t;
}

static F64 
ease_out_cubic(F64 t)  {
  return 1.0 + (t-1) * (t-1) * (t-1);
}

static F64 
ease_inout_cubic(F64 t)  {
  return t < 0.5 ? 4.0 * t * t * t : 1.0 + (t-1) * (2.0 * (t-2)) * (2.0 * (t-2));
}

static F64 
ease_in_quart(F64 t)  {
  t *= t;
  return t * t;
}

static F64 
ease_out_quart(F64 t) {
  --t;
  t = t * t;
  return 1.0 - t * t;
}

static F64 
ease_inout_quart(F64 t)  {
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
ease_in_quint(F64 t)  {
  F64 t2 = t * t;
  return t * t2 * t2;
}

static F64
ease_out_quint(F64 t)  {
  --t;
  F64 t2 = t * t;
  return 1.0 +t * t2 * t2;
}

static F64
ease_inout_quint(F64 t)  {
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
ease_in_circ(F64 t)  {
  return 1.0 -sqrt(1.0 -t);
}

static F64 
ease_out_circ(F64 t)  {
  return sqrt(t);
}

static F64 
ease_inout_circ(F64 t)  {
  if (t < 0.5) {
    return (1.0 -sqrt(1.0 -2.0 * t)) * 0.5;
  }
  else {
    return (1.0 +sqrt(2.0 * t - 1.0)) * 0.5;
  }
}

static F64 
ease_in_back(F64 t)  {
  return t * t * (2.7 * t - 1.7);
}

static F64 
ease_out_back(F64 t)  {
  --t;
  return 1.0 + t * t * (2.7 * t + 1.7);
}

static F64 
ease_inout_back(F64 t)  {
  if (t < 0.5) {
    return t * t * (7.0 * t - 2.5) * 2.0;
  }
  else {
    --t;
    return 1.0 + t * t * 2.0 * (7.0 * t + 2.5);
  }
}

static F64 
ease_in_elastic(F64 t)  {
  F64 t2 = t * t;
  return t2 * t2 * sin(t * PI_64 * 4.5);
}

static F64 
ease_out_elastic(F64 t)  {
  F64 t2 = (t - 1.0) * (t - 1.0);
  return 1.0 -t2 * t2 * cos(t * PI_64 * 4.5);
}

static F64 
ease_inout_elastic(F64 t)  {
  F64 t2;
  if (t < 0.45) {
    t2 = t * t;
    return 8.0 * t2 * t2 * sin(t * PI_64 * 9.0);
  }
  else if (t < 0.55) {
    return 0.5 +0.75 * sin(t * PI_64 * 4.0);
  }
  else {
    t2 = (t - 1.0) * (t - 1.0);
    return 1.0 -8.0 * t2 * t2 * sin(t * PI_64 * 9.0);
  }
}



// NOTE(Momo): These require power function. 
static F64 
ease_in_bounce(F64 t)  {
  return pow(2.0, 6.0 * (t - 1.0)) * abs_of(sin(t * PI_64 * 3.5));
}


static F64 
ease_out_bounce(F64 t) {
  return 1.0 -pow(2.0, -6.0 * t) * abs_of(cos(t * PI_64 * 3.5));
}

static F64 
ease_inout_bounce(F64 t) {
  if (t < 0.5) {
    return 8.0 * pow(2.0, 8.0 * (t - 1.0)) * abs_of(sin(t * PI_64 * 7.0));
  }
  else {
    return 1.0 -8.0 * pow(2.0, -8.0 * t) * abs_of(sin(t * PI_64 * 7.0));
  }
}

static F64
ease_in_expo(F64 t)  {
  return (pow(2.0, 8.0 * t) - 1.0) / 255.0;
}


static F64 
ease_out_expo(F64 t)  {
  return t == 1.0 ? 1.0 : 1.0 -pow(2.0, -10.0 * t);
}

static F64 
ease_inout_expo(F64 t)  {
  if (t < 0.5) {
    return (pow(2.0, 16.0 * t) - 1.0) / 510.0;
  }
  else {
    return 1.0 -0.5 * pow(2.0, -16.0 * (t - 0.5));
  }
}

