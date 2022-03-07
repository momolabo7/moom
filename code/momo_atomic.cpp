#if COMPILER_MSVC
#include <intrin.h>
static U32 atomic_compare_exchange_u32(U32 volatile* value,
                                       U32 new_value,
                                       U32 expected_value)
{
  U32 ret = _InterlockedCompareExchange((long volatile*)value,
                                        new_value,
                                        expected_value);
  return ret;
}

#else
// MORE?
#endif
