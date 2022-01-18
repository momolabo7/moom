#include <stdio.h>
#include "../code/momo.h"

B32 CMP(int* lhs, int* rhs) {
  return (*lhs) < (*rhs);
}

int main() {
  int arr[] = { 1,4,6,8,9,20,13,-1, -20 };
  quicksort(arr, ArrayCount(arr), CMP);
  
  for( auto&& itr : arr) {
    printf("%d ", itr);
  }
  printf("\n");
}