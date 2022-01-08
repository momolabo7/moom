// NOTE(Momo): We might be able to constexpr most of these

#ifndef MOMO_RANDOM_H
#define MOMO_RANDOM_H

struct RNG
{
  U32 index;
};

static RNG CreateRNG(U32 seed);
static U32 Next(RNG* r);
static U32 RandomChoice(RNG* r, U32 choice_count);
static F32 RandomUnilateral(RNG* r);
static F32 RandomBilateral(RNG* r);
static F32 RandomRangeF32(RNG* r, F32 min, F32 max);
static S32 RandomRangeS32(RNG* r, S32 min, S32 max);
static U32 RandomRangeU32(RNG* r, U32 min, U32 max);


#include "momo_random.cpp"

#endif //MOMO_RANDOM_H
