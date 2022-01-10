template<class T, class BinaryCompare> static UMI
QuickSort__Partition(T* a,
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
      Swap(i_ptr, eventual_pivot_ptr);
      ++eventual_pivot_idx;
    }
    
  }
  
  Swap(a + eventual_pivot_idx,
       a + pivot_idx);
  
  return eventual_pivot_idx;
  
  
}

// NOTE(Momo): This is done inplace
template<typename T, class BinaryCompare> static void 
QuickSort__Range(T* a, 
                 UMI start, 
                 UMI ope,
                 BinaryCompare cmp) 
{
  if (ope - start <= 1) {
    return;
  }
  UMI pivot = QuickSort__Partition(a, start, ope, cmp);
  QuickSort__Range(a, start, pivot, cmp);
  QuickSort__Range(a, pivot+1, ope, cmp);
  
}


template<class T, class BinaryCompare> static void
QuickSort(T* arr, UMI count, BinaryCompare cmp) {
  QuickSort__Range(arr, 0, count, cmp);
  
  //QuickSortBlock(arr, count, sizeof(T), pred);
}