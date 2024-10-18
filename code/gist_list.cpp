// This is code for a 'list' with push and pop functionality
// that uses an array as it's underlying data structure.
//
// The 'count' variable is mostly used to avoid having special 
// states for 'front' and 'rear' variable.
//

#include "momo.h"
#include <stdio.h>


struct gist_list_t {
  u32_t e[10];
  u32_t count;
};

static void
gist_list_push(gist_list_t* l, u32_t item)
{
  assert(l->count < array_count(l->e));
  l->e[l->count++] = item;
}

static void
gist_list_pop(gist_list_t* l)
{
  l->count--;
}

static u32_t 
gist_list_last(gist_list_t* l)
{
  assert(l->count > 0);
  return l->e[l->count-1];
}

static void 
gist_list_remove_maintain_order(gist_list_t* l, u32_t index)
{
  assert(index < l->count);
  for_range(list_index, index, l->count-1) {
    l->e[list_index] = l->e[list_index+1];
  }
  l->count--;
}

static void 
gist_list_remove_fast_ignore_order(gist_list_t* l, u32_t index)
{
  assert(index < l->count);
  l->e[index] = l->e[l->count-1];
  l->count--;
}

static void 
gist_list_insert(gist_list_t* l, u32_t index, u32_t item)
{
  //
  // @note: Asserting here is probably a good idea?
  // If you want to insert to the back, should just use
  // the push method. Shrugs.
  //
  assert(index < l->count); 
  assert(l->count < array_count(l->e));

  // push everything forward
  for_cnt(i, l->count - index) 
  {
    usz_t actual_index = l->count - i;
    l->e[actual_index] = l->e[actual_index-1];
  }
  l->e[index] = item;
  l->count++;
}

static void 
gist_list_insert_fast_ignore_order(gist_list_t* l, u32_t index, u32_t item)
{
  //
  // @note: Asserting here is probably a good idea?
  // If you want to insert to the back, should just use
  // the push method. Shrugs.
  //
  assert(index < l->count); 
  assert(l->count < array_count(l->e));

  l->e[l->count] = l->e[index];
  l->e[index] = item;
  l->count++;
}
int main() 
{
  // @todo
  make(gist_list_t, list);
  for (int i = 0; i < 10; ++i) {
    gist_list_push(list, i);
  }

  for_cnt (i, list->count) printf("%d ", list->e[i]); printf("\n");

  gist_list_pop(list);
  for_cnt (i, list->count) printf("%d ", list->e[i]); printf("\n");

  gist_list_pop(list);
  for_cnt (i, list->count) printf("%d ", list->e[i]); printf("\n");

  gist_list_remove_maintain_order(list, 3);
  for_cnt (i, list->count) printf("%d ", list->e[i]); printf("\n");

  gist_list_remove_fast_ignore_order(list, 3);
  for_cnt (i, list->count) printf("%d ", list->e[i]); printf("\n");

  gist_list_insert(list, 3, 0);
  for_cnt (i, list->count) printf("%d ", list->e[i]); printf("\n");

  gist_list_insert_fast_ignore_order(list, 3, 0);
  for_cnt (i, list->count) printf("%d ", list->e[i]); printf("\n");
}


