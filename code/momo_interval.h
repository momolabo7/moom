/* date = December 11th 2021 1:04 pm */

#ifndef MOMO_INTERVAL_H
#define MOMO_INTERVAL_H

typedef struct I1U32 {
  U32 min, max;
} I1U32;

typedef struct I1F32 {
  F32 min, max;
} I1F32;

typedef struct I1S32 {
  S32 min, max;
} I1S32;

typedef struct I2U32 {
  V2U32 min, max;
} I2U32;

typedef struct I2F32 {
  V2F32 min, max;
} I2F32;


typedef struct I3F32 {
  V3F32 min, max;
} I3F32;

#endif //MOMO_INTERVAL_H
