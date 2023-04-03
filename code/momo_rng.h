#ifndef MOMO_RANDOM_H
#define MOMO_RANDOM_H


struct rng_t {
  u32_t seed;
};

static void rng_init(rng_t* r, u32_t seed);
static u32_t rng_next(rng_t* r);
static u32_t rng_choice(rng_t* r, u32_t choice_count);
static f32_t rng_unilateral(rng_t* r);
static f32_t rng_bilateral(rng_t* r);
static f32_t rng_range_F32(rng_t* r, f32_t min, f32_t max);
static s32_t rng_range_S32(rng_t* r, s32_t min, s32_t max);
static v2f_t rng_unit_circle(rng_t* r);

//
// IMPLEMENTATION
//
static void
rng_init(rng_t* r, u32_t seed)
{
  r->seed = seed;
}

static u32_t
rng_next(rng_t* r)
{
  u32_t result = r->seed;
	result ^= result << 13;
	result ^= result >> 17;
	result ^= result << 5;
	r->seed = result;
  return result;
}

static u32_t 
rng_choice(rng_t* r, u32_t choice_count) {
  return rng_next(r) % choice_count;
}

// Get number within [0, 1]
static f32_t 
rng_unilateral(rng_t* r)
{
  f32_t divisor = 1.0f / (f32_t)U32_MAX;
  f32_t result = divisor*(f32_t)rng_next(r);
  
  return result;
}


// Get number within [-1, 1]
static f32_t 
rng_bilateral(rng_t* r)
{
  f32_t result = 2.0f * rng_unilateral(r) - 1.0f;  
  return(result);
}

static f32_t 
rng_range_F32(rng_t* r, f32_t min, f32_t max)
{
  f32_t result = f32_lerp(min, rng_unilateral(r), max);
  return(result);
}

static s32_t 
rng_range_S32(rng_t* r, s32_t min, s32_t max)
{
  s32_t result = min + (s32_t)(rng_next(r)%((max + 1) - min));
  return(result);
}

static u32_t
rng_range_U32(rng_t* r, u32_t min, u32_t max)
{
  u32_t result = min + (u32_t)(rng_next(r)%((max + 1) - min));
  return(result);
}

static v2f_t 
rng_unit_circle(rng_t* r) {
  f32_t rand_angle = 2.f * PI_32 * rng_unilateral(r);
  v2f_t ret = {0};
  ret.x = f32_cos(rand_angle);
  ret.y = f32_sin(rand_angle);

  return ret;
}


#endif //MOMO_RANDOM_H
