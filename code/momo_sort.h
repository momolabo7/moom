#ifndef MOMO_SORT_H
#define MOMO_SORT_H

template<typename T, typename BinaryCompare> 
static void quicksort(T* arr, UMI count, BinaryCompare pred);

#include "momo_sort.cpp"

#endif //MOMO_SORT_H
