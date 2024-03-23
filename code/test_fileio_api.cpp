#include "momo.h"
#include <stdio.h>



int main() 
{
  os_arena_t arena = {};
  os_arena_init(&arena, gigabytes(1), false); 
  defer { os_arena_free(&arena); };


}
