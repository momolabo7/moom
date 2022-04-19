// TODO(Momo): This should really be just 'momo_intrinsics.h'
// So all the sqrt, sin, cos functions are gonna be here instead
// of litering #if everywhere

#if COMPILER_MSVC
#include <intrin.h>
static U32 
atomic_compare_assign(U32 volatile* value,
                      U32 new_value,
                      U32 expected_value)
{
  U32 ret = _InterlockedCompareExchange((long volatile*)value,
                                        new_value,
                                        expected_value);
  return ret;
}

static U64 
atomic_assign(U64 volatile* value,
              U64 new_value)
{
  U64 ret = _InterlockedExchange64((__int64 volatile*)value,
                                   new_value);
  return ret;
}
static U32 
atomic_add(U32 volatile* value, U32 to_add) {
  U32 result = _InterlockedExchangeAdd((long volatile*)value, to_add);
  return result;
}

static U64 
atomic_add(U64 volatile* value, U64 to_add) {
  U64 result = _InterlockedExchangeAdd64((__int64 volatile*)value, to_add);
  return result;
}

#else
#error "Not defined"

#endif
