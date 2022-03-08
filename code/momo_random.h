#ifndef MOMO_RANDOM_H
#define MOMO_RANDOM_H

struct RNG
{
  U32 index;
};

static RNG create_rng(U32 seed);
static U32 next();
static U32 next_choice(U32 choice_count);
static F32 next_unilateral();
static F32 next_bilateral();
static F32 next_range_F32(F32 min, F32 max);
static S32 next_range_S32(S32 min, S32 max);
static U32 next_range_U32(U32 min, U32 max);

#include "momo_random.cpp"

#endif //MOMO_RANDOM_H
