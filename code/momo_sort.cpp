static UMI
QuickSort__Partition(U8* a,
                     UMI start, 
                     UMI ope,
                     UMI stride,
                     B32 (*cmp)(void*, void*)) 
{
  // Save the rightmost index as pivot
  // This frees up the right most index as a slot
  UMI pivot_idx = ope-1;
  UMI eventual_pivot_idx = start;
  
  for (UMI i = start; i < ope-1; ++i) {
    U8* i_ptr = a + (stride * i);
    U8* pivot_ptr = a + (stride * pivot_idx);
    if (cmp(i_ptr, pivot_ptr)) {
      U8* eventual_pivot_ptr = a + (stride * eventual_pivot_idx);
      Bin_Swap(i_ptr, eventual_pivot_ptr, stride);
      ++eventual_pivot_idx;
    }
    
  }
  
  Bin_Swap(a + (stride * eventual_pivot_idx),
              a + (stride * pivot_idx),
              stride);
  
  return eventual_pivot_idx;
  
  
}

// NOTE(Momo): This is done inplace
static void 
QuickSort__Range(U8* a, 
                 UMI start, 
                 UMI ope,
                 UMI stride,
                 B32 (*cmp)(void*, void*)) 
{
  if (ope - start <= 1) {
    return;
  }
  UMI pivot = QuickSort__Partition(a, 
                                   start, 
                                   ope,
                                   stride,
                                   cmp);
  
  QuickSort__Range(a, start, pivot, stride, cmp);
  QuickSort__Range(a, pivot+1, ope, stride, cmp);
  
}

static void
QuickSortBlock(void* a, 
               UMI count, 
               UMI stride, 
               B32(*cmp)(void*, void*)) 
{
  QuickSort__Range((U8*)a, 0, count, stride, cmp);
}
