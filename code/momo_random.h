/* date = November 20th 2021 11:58 am */

#ifndef MOMO_RANDOM_H
#define MOMO_RANDOM_H

// Very pseudo, much wow.
struct Rng
{
    U32 index;
};

static Rng Rng_Create(U32 seed);
static U32 Rng_Next(Rng* r);
static U32 Rng_Choice(Rng* r, U32 choice_count);
static F32 Rng_Unilateral(Rng* r);
static F32 Rng_Bilateral(Rng* r);
static F32 Rng_BetweenF32(Rng* r, F32 min, F32 max);
static S32 Rng_BetweenS32(Rng* r, S32 min, S32 max);
static U32 Rng_BetweenU32(Rng* r, U32 min, U32 max);

#endif //MOMO_RANDOM_H
