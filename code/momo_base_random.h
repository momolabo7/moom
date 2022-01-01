/* date = November 20th 2021 11:58 am */

#ifndef MOMO_BASE_RANDOM_H
#define MOMO_BASE_RANDOM_H

typedef struct 
{
  U32 index;
} Rng;

static Rng Rng_Create(U32 seed);
static U32 Rng_Next(Rng* r);
static U32 Rng_Choice(Rng* r, U32 choice_count);
static F32 Rng_Unilateral(Rng* r);
static F32 Rng_Bilateral(Rng* r);
static F32 Rng_BetweenF32(Rng* r, F32 min, F32 max);
static S32 Rng_BetweenS32(Rng* r, S32 min, S32 max);
static U32 Rng_BetweenU32(Rng* r, U32 min, U32 max);


#include "momo_base_random.cpp"

#endif //MOMO_BASE_RANDOM_H
