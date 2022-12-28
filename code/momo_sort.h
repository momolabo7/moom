#ifndef MOMO_SORT_H
#define MOMO_SORT_H


// Chances are that we won't be sorting 
// more than 32-bits worth of indices?
struct sort_entry_t {
  f32_t key;
  u32_t index;
};

static void
_sort_swap_entries(sort_entry_t* a, sort_entry_t* b) {
  sort_entry_t temp = *b;
  *b = *a;
  *a = temp;
}

///////////////////////////////////////////////////////////////////
// Quick sort
//
static u32_t
_quicksort_partition(sort_entry_t* a,
                     u32_t start, 
                     u32_t ope) 
{
  // Save the rightmost index as pivot
  // This frees up the right most index as a slot
  u32_t pivot_idx = ope-1;
  u32_t eventual_pivot_idx = start;
  
  for (u32_t i = start; i < ope-1; ++i) {
    sort_entry_t* i_ptr = a + i;
    sort_entry_t* pivot_ptr = a + pivot_idx;
    if (i_ptr->key < pivot_ptr->key) {
      sort_entry_t* eventual_pivot_ptr = a + eventual_pivot_idx;
      _sort_swap_entries(i_ptr, eventual_pivot_ptr);
      ++eventual_pivot_idx;
    }
    
  }
  
  _sort_swap_entries(a + eventual_pivot_idx, a + pivot_idx);
  
  return eventual_pivot_idx;
}

// NOTE(Momo): This is done inplace
static void
_quicksort_range(sort_entry_t* a, 
                 u32_t start, 
                 u32_t ope) 
{
  if (ope - start <= 1) {
    return;
  }
  u32_t pivot = _quicksort_partition(a, start, ope);
  _quicksort_range(a, start, pivot);
  _quicksort_range(a, pivot+1, ope);
}

static void
quicksort(sort_entry_t* entries, u32_t entry_count) {
  _quicksort_range(entries, 0, entry_count);

}

#endif //MOMO_SORT_H
