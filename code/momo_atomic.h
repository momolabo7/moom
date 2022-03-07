/* date = March 7th 2022 1:08 pm */

#ifndef MOMO_ATOMIC_H
#define MOMO_ATOMIC_H

// Returns the old value before the exchange
static U32 atomic_compare_exchange_u32(U32 volatile* value,
                                       U32 new_value,
                                       U32 expected_value);



#include "momo_atomic.cpp"
#endif //MOMO_ATOMIC_H
