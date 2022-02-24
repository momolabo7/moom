/* date = January 27th 2022 9:19 am */

#ifndef TEST_SORT_H
#define TEST_SORT_H
void test_sort() {
  int arr[] = { 1,4,6,8,9,20,13,-1, -20 };
  quicksort(arr, array_count(arr), 
            [](int* lhs, int* rhs) { return (*lhs) < (*rhs); } );
  
  for( auto&& itr : arr) {
    printf("%d ", itr);
  }
  printf("\n");
}
#endif //TEST_SORT_H
