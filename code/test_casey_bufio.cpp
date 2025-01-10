#include "momo.h"

static void
buf_put_u8(buf_t* dest, u8_t u8)
{
  if (dest->size)
  {
    *dest->e++ = u8;
    dest->size--;
  }
}

static void
foo(buf_t dest, u64_t num)
{
  usz_t start_pt = b->size; 
  buf_t helper = dest;
  for(; num != 0; num /= 10) 
  {
    u64_t digit_to_convert = num % 10;
    buf_put_u8(&helper, (u8_t)digit_to_ascii(digit_to_convert))
  }

  helper = buf_set(dest.e, helper.e - dest.e);

}

static buf_t 
buf_fmt(buf_t dest, u8_t* fmt, ...)
{
}

int main() 
{
  
  return 0;
}
