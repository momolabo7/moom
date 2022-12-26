#ifndef MOMO_SORT_H
#define MOMO_SORT_H


// Chances are that we won't be sorting 
// more than 32-bits worth of indices?
struct Sort_Entry {
  F32 key;
  U32 index;
};

static void
_sort_swap_entries(Sort_Entry* a, Sort_Entry* b) {
  Sort_Entry temp = *b;
  *b = *a;
  *a = temp;
}

///////////////////////////////////////////////////////////////////
// Quick sort
//
static U32
_quicksort_partition(Sort_Entry* a,
                     U32 start, 
                     U32 ope) 
{
  // Save the rightmost index as pivot
  // This frees up the right most index as a slot
  U32 pivot_idx = ope-1;
  U32 eventual_pivot_idx = start;
  
  for (U32 i = start; i < ope-1; ++i) {
    Sort_Entry* i_ptr = a + i;
    Sort_Entry* pivot_ptr = a + pivot_idx;
    if (i_ptr->key < pivot_ptr->key) {
      Sort_Entry* eventual_pivot_ptr = a + eventual_pivot_idx;
      _sort_swap_entries(i_ptr, eventual_pivot_ptr);
      ++eventual_pivot_idx;
    }
    
  }
  
  _sort_swap_entries(a + eventual_pivot_idx, a + pivot_idx);
  
  return eventual_pivot_idx;
}

// NOTE(Momo): This is done inplace
static void
_quicksort_range(Sort_Entry* a, 
                 U32 start, 
                 U32 ope) 
{
  if (ope - start <= 1) {
    return;
  }
  U32 pivot = _quicksort_partition(a, start, ope);
  _quicksort_range(a, start, pivot);
  _quicksort_range(a, pivot+1, ope);
}

static void
quicksort(Sort_Entry* entries, U32 entry_count) {
  _quicksort_range(entries, 0, entry_count);

}

#endif //MOMO_SORT_H
