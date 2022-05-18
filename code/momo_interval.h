// TODO: What do we do about this?

#ifndef MOMO_INTERVAL_H
#define MOMO_INTERVAL_H

typedef struct {
  U32 min, max;
} I1U32;

typedef struct {
  F32 min, max;
} I1F32;

typedef struct {
  S32 min, max;
} I1S32;

typedef struct {
  V2U min, max;
} I2U32;

typedef struct {
  V2 min, max;
} I2F32;


typedef struct {
  V3 min, max;
} I3F32;

#include "momo_interval.cpp"

#endif //MOMO_INTERVAL_H
