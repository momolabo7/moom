#ifndef MOMO_RANDOM_H
#define MOMO_RANDOM_H


typedef struct {
  U32 index;
} RNG;

static RNG
rng_create(U32 seed)
{
  RNG series = {};
  series.index = seed;
  return series;
}

static U32 
rng_next(RNG* r)
{
  U32 result = r->index;
	result ^= result << 13;
	result ^= result >> 17;
	result ^= result << 5;
	r->index = result;
  return result;
}

static U32 
rng_choice(RNG* r, U32 choice_count) {
  return rng_next(r) % choice_count;
}

// Get number within [0, 1]
static F32 
rng_unilateral(RNG* r)
{
  F32 divisor = 1.0f / (F32)U32_MAX;
  F32 result = divisor*(F32)rng_next(r);
  
  return result;
}


// Get number within [-1, 1]
static F32 
rng_bilateral(RNG* r)
{
  F32 result = 2.0f * rng_unilateral(r) - 1.0f;  
  return(result);
}

static F32 
rng_range_F32(RNG* r, F32 min, F32 max)
{
  F32 result = lerp_f32(min, rng_unilateral(r), max);
  return(result);
}

static S32 
rng_range_S32(RNG* r, S32 min, S32 max)
{
  S32 result = min + (S32)(rng_next(r)%((max + 1) - min));
  return(result);
}

static U32
rng_range_U32(RNG* r, U32 min, U32 max)
{
  U32 result = min + (U32)(rng_next(r)%((max + 1) - min));
  return(result);
}

static V2 
rng_unit_circle(RNG* r) {
  F32 rand_angle = 2.f * PI_32 * rng_unilateral(r);
  V2 ret = {0};
  ret.x = cos_f32(rand_angle);
  ret.y = sin_f32(rand_angle);

  return ret;
}


#endif //MOMO_RANDOM_H
