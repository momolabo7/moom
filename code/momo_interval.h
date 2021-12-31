/* date = December 11th 2021 1:04 pm */

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
  V2U32 min, max;
} I2U32;

typedef struct {
  V2F32 min, max;
} I2F32;


typedef struct I3F32 {
  V3F32 min, max;
} I3F32;

#endif //MOMO_INTERVAL_H
