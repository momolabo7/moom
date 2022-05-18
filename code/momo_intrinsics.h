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

#include "momo_intrinsics.cpp"

#endif //MOMO_INTRINSICS_H
