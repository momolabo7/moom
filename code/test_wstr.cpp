#include "momo.h"

#include <stdio.h>

struct str16_t 
{
  u16_t* e;
  usz_t size;
};

static str16_t
str16_set(u16_t* data, usz_t size)
{
  str16_t ret;
  ret.e = data;
  ret.size = size;

  return ret;
}

#define str16_from_lit(lit) str16_set((u16_t*)(L ## lit), sizeof(lit)/2-1)

int main()
{
  arena_t arena;
  str16_t str;
  arena_alloc(&arena, gigabytes(1), false);
  
  str = str16_from_lit("←");


  printf("%ls\n", str.e);
}
