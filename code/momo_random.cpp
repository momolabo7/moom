
static RNG
CreateRNG(U32 seed)
{
  RNG series = {};
  series.index = seed;
  return series;
}

static U32 
Next(RNG* r)
{
  U32 result = r->index;
	result ^= result << 13;
	result ^= result >> 17;
	result ^= result << 5;
	r->index = result;
  return result;
}

static U32 
RandomChoice(RNG* r, U32 choice_count) {
  Assert(choice_count > 0);
  return Next(r) % choice_count;
}

// Get number within [0, 1]
static F32 
RandomUnilateral(RNG* r)
{
  F32 divisor = 1.0f / (F32)U32_max;
  F32 result = divisor*(F32)Next(r);
  
  return result;
}


// Get number within [-1, 1]
static F32 
RandomBilateral(RNG* r)
{
  F32 result = 2.0f * RandomUnilateral(r) - 1.0f;
  
  return(result);
}

static F32 
RandomRangeF32(RNG* r, F32 min, F32 max)
{
  Assert(max >= min);
  F32 result = Lerp(min, RandomUnilateral(r), max);
  return(result);
}

static S32 
RandomRangeS32(RNG* r, S32 min, S32 max)
{
  Assert(max >= min);
  S32 result = min + (S32)(Next(r)%((max + 1) - min));
  return(result);
}

static U32
RandomRangeU32(RNG* r, U32 min, U32 max)
{
  Assert(max >= min);
  U32 result = min + (U32)(Next(r)%((max + 1) - min));
  return(result);
}
