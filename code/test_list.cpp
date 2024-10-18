//
// This is a visualization test on how a templated list API would look 
//
// Supposedly if we have the arena system already setup, we don't have to 
// actually do anything special; just use a static array and be on our way
//
// Supposedly this will also work well for arrays as well...
//

#include "momo.h"



template<typename type_t, u32_t cap, typename count_t = u32_t>
struct list_t {
  type_t e[cap];
  count_t count;
};

template<typename type_t, u32_t cap, typename count_t = u32_t>
static void 
list_init(list_t<type_t, cap, count_t>* list)
{
  list->count = 0;
}

template<typename type_t, u32_t cap, typename count_t = u32_t>
static void 
list_push(list_t<type_t, cap, count_t>* list, type_t item)
{
  assert(list->count < array_count(list->e))
  list->e[list->count++] = item;
}


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
