
static RNG
create_rng(U32 seed)
{
  RNG series = {};
  series.index = seed;
  return series;
}

static U32 
next(RNG* r)
{
  U32 result = r->index;
	result ^= result << 13;
	result ^= result >> 17;
	result ^= result << 5;
	r->index = result;
  return result;
}

static U32 
next_choice(RNG* r, U32 choice_count) {
  assert(choice_count > 0);
  return next(r) % choice_count;
}

// Get number within [0, 1]
static F32 
next_unilateral(RNG* r)
{
  F32 divisor = 1.0f / (F32)U32_MAX;
  F32 result = divisor*(F32)next(r);
  
  return result;
}


// Get number within [-1, 1]
static F32 
next_bilateral(RNG* r)
{
  F32 result = 2.0f * next_unilateral(r) - 1.0f;  
  return(result);
}

static F32 
next_range_F32(RNG* r, F32 min, F32 max)
{
  assert(max >= min);
  F32 result = lerp(min, next_unilateral(r), max);
  return(result);
}

static S32 
next_range_S32(RNG* r, S32 min, S32 max)
{
  assert(max >= min);
  S32 result = min + (S32)(next(r)%((max + 1) - min));
  return(result);
}

static U32
next_range_U32(RNG* r, U32 min, U32 max)
{
  assert(max >= min);
  U32 result = min + (U32)(next(r)%((max + 1) - min));
  return(result);
}
