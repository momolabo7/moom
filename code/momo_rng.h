#ifndef MOMO_RANDOM_H
#define MOMO_RANDOM_H

struct RNG
{
  U32 index;
};

static RNG rng_create(U32 seed);
static U32 rng_next(RNG* r);
static U32 rng_choice(RNG* r, U32 choice_count);
static F32 rng_unilateral(RNG* r);
static F32 rng_bilateral(RNG* r);
static F32 rng_range_F32(RNG* r, F32 min, F32 max);
static S32 rng_range_S32(RNG* r, S32 min, S32 max);
static U32 rng_range_U32(RNG* r, U32 min, U32 max);

// TODO: rng_unit_circle()

#include "momo_rng.cpp"

#endif //MOMO_RANDOM_H
