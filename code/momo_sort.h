#ifndef MOMO_SORT_H
#define MOMO_SORT_H

template<typename T, typename BinaryCompare> 
static void quicksort(T* arr, UMI count, BinaryCompare pred);

//TODO: API for array and list?

#include "momo_sort.cpp"

#endif //MOMO_SORT_H
