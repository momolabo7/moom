#include "momo.h"


#include <stdio.h>

int main() 
{
  buf_t a = buf_from_lit("wtf 123 123 1.23");
  buf_t b = buf_from_lit("wtf %d %d %f");

  s32_t c, d;
  f32_t e;
  buf_to_vars(a, b, &c, &d, &e);
  printf("%d %d %f\n", c, d, e);

}
