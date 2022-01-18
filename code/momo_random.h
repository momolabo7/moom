// NOTE(Momo): We might be able to constexpr most of these

#ifndef MOMO_RANDOM_H
#define MOMO_RANDOM_H

struct RNG
{
  U32 index;
  
  U32 next();
  U32 next_choice(U32 choice_count);
  F32 next_unilateral();
  F32 next_bilateral();
  F32 next_range_F32(F32 min, F32 max);
  S32 next_range_S32(S32 min, S32 max);
  U32 next_range_U32(U32 min, U32 max);
};

static RNG create_rng(U32 seed);

#include "momo_random.cpp"

#endif //MOMO_RANDOM_H
