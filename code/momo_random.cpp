static const U32 Rng__max = 0x05f5c21f;
static const U32 Rng__min = 0x000025a0;

static Rng 
Rng_Create(U32 seed)
{
    Rng series = {};
    series.index = seed;
    return series;
}

static U32 
Rng_Next(Rng* r)
{
    U32 result = r->index;
	result ^= result << 13;
	result ^= result >> 17;
	result ^= result << 5;
	r->index = result;
    return result;
}

static U32 
Rng_Choice(Rng* r, U32 choice_count) {
    Assert(choice_count > 0);
    return Rng_Next(r) % choice_count;
}

// Get number within [0, 1]
static F32 
Rng_Unilateral(Rng* r)
{
    F32 divisor = 1.0f / (F32)U32_max;
    F32 result = divisor*(F32)Rng_Next(r);
    
    return result;
}


// Get number within [-1, 1]
static F32 
Rng_Bilateral(Rng* r)
{
    F32 result = 2.0f * Rng_Unilateral(r) - 1.0f;
    
    return(result);
}

static F32 
Rng_BetweenF32(Rng* r, F32 min, F32 max)
{
    Assert(max >= min);
    F32 result = Lerp(min, Rng_Unilateral(r), max);
    return(result);
}

static S32 
Rng_BetweenS32(Rng* r, S32 min, S32 max)
{
    Assert(max >= min);
    S32 result = min + (S32)(Rng_Next(r)%((max + 1) - min));
    return(result);
}

static U32
Rng_BetweenU32(Rng* r, U32 min, U32 max)
{
    Assert(max >= min);
    U32 result = min + (U32)(Rng_Next(r)%((max + 1) - min));
    return(result);
}
