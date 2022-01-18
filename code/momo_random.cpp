
static RNG
create_rng(U32 seed)
{
  RNG series = {};
  series.index = seed;
  return series;
}

U32 
RNG::next()
{
  U32 result = this->index;
	result ^= result << 13;
	result ^= result >> 17;
	result ^= result << 5;
	this->index = result;
  return result;
}

U32 
RNG::next_choice(U32 choice_count) {
  assert(choice_count > 0);
  return next() % choice_count;
}

// Get number within [0, 1]
F32 
RNG::next_unilateral()
{
  F32 divisor = 1.0f / (F32)U32_MAX;
  F32 result = divisor*(F32)next();
  
  return result;
}


// Get number within [-1, 1]
F32 
RNG::next_bilateral()
{
  F32 result = 2.0f * next_unilateral() - 1.0f;  
  return(result);
}

F32 
RNG::next_range_F32(F32 min, F32 max)
{
  assert(max >= min);
  F32 result = lerp(min, next_unilateral(), max);
  return(result);
}

S32 
RNG::next_range_S32(S32 min, S32 max)
{
  assert(max >= min);
  S32 result = min + (S32)(next()%((max + 1) - min));
  return(result);
}

U32
RNG::next_range_U32(U32 min, U32 max)
{
  assert(max >= min);
  U32 result = min + (U32)(next()%((max + 1) - min));
  return(result);
}
