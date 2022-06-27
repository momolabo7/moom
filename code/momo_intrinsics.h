/* date = March 7th 2022 1:08 pm */

#ifndef MOMO_INTRINSICS_H
#define MOMO_INTRINSICS_H

// Returns the old value before the exchange
static U32 atomic_compare_assign(U32 volatile* value,
                                 U32 new_value,
                                 U32 expected_value);

static U64 atomic_assign(U64 volatile* value,
                         U64 new_value);

static U32 atomic_add(U32 volatile* value, U32 to_add);
static U64 atomic_add(U64 volatile* value, U64 to_add);


static F32 sin(F32 x);
static F32 cos(F32 x);
static F32 tan(F32 x);
static F32 sqrt(F32 x);
static F32 asin(F32 x);
static F32 acos(F32 x);
static F32 atan(F32 x);
static F32 pow(F32 v, F32 e);

static F64 sin(F64 x);
static F64 cos(F64 x);
static F64 tan(F64 x);
static F64 sqrt(F64 x);
static F64 asin(F64 x);
static F64 acos(F64 x);
static F64 atan(F64 x);
static F64 pow(F64 , F64 e);

static F32 ceil(F32 value);
static F32 floor(F32 value);
static F32 round(F32 value);

static F64 ceil(F64 value);
static F64 floor(F64 value);
static F64 round(F64 value);

#include "momo_intrinsics.cpp"

#endif //MOMO_INTRINSICS_H
