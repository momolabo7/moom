#include "momo.h"

#include <stdio.h>

struct wstr_t 
{
  u16_t* e;
  usz_t size;
};

static wstr_t
wstr_set(u16_t* data, usz_t size)
{
  wstr_t ret;
  ret.e = data;
  ret.size = size;

  return ret;
}

#define wstr_from_lit(lit) wstr_set((u16_t*)(L ## lit), sizeof(lit)/2-1)

int main()
{
  arena_t arena;
  str_t str;
  arena_alloc(&arena, gigabytes(1), false);
  
  str = str_from_lit("←");


  printf("%ls\n", str.e);
}
