//
// This is a visualization test on how a templated list API would look 
//
// Supposedly if we have the arena system already setup, we don't have to 
// actually do anything special; just use a static array and be on our way
//
// Supposedly this will also work well for arrays as well...
//

#include "momo.h"




int main() 
{
  make(arena_t, a);
  arena_alloc(a, megabytes(256), false);
  using test_t = list_t<u32_t, 1024>;

  auto* list = arena_push(test_t, a);
  list_init(list);
  for (u32_t i = 0; i < 10; ++i) 
    list_push(list, i); 

  for (u32_t i = 0; i < list->count; ++i)  
  {
    printf("%d ", list->e[i]);
  }
  printf("\n");

  arena_free(a);
}
