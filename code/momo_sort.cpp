template<typename T, typename BinaryCompare> static UMI
_quicksort_partition(T* a,
                    UMI start, 
                    UMI ope,
                    BinaryCompare cmp) 
{
  // Save the rightmost index as pivot
  // This frees up the right most index as a slot
  UMI pivot_idx = ope-1;
  UMI eventual_pivot_idx = start;
  
  for (UMI i = start; i < ope-1; ++i) {
    T* i_ptr = a + i;
    T* pivot_ptr = a + pivot_idx;
    if (cmp(i_ptr, pivot_ptr)) {
      T* eventual_pivot_ptr = a + eventual_pivot_idx;
      swap(i_ptr, eventual_pivot_ptr);
      ++eventual_pivot_idx;
    }
    
  }
  
  swap(a + eventual_pivot_idx,
       a + pivot_idx);
  
  return eventual_pivot_idx;
  
  
}

// NOTE(Momo): This is done inplace
template<typename T, typename BinaryCompare> static void 
_quicksort_range(T* a, 
                UMI start, 
                UMI ope,
                BinaryCompare cmp) 
{
  if (ope - start <= 1) {
    return;
  }
  UMI pivot = _quicksort_partition(a, start, ope, cmp);
  _quicksort_range(a, start, pivot, cmp);
  _quicksort_range(a, pivot+1, ope, cmp);
  
}


template<typename T, typename BinaryCompare> static void
quicksort(T* arr, UMI count, BinaryCompare cmp) {
  _quicksort_range(arr, 0, count, cmp);
  //QuickSortBlock(arr, count, sizeof(T), pred);
}