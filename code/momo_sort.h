/* date = december 2nd 2021 10:41 am */

#ifndef MOMO_SORT_H
#define MOMO_SORT_H


static void QuickSortBlock(void* arr, UMI count, UMI item_size, B32 (*cmp)(void*, void*));
#define QuickSort(a,t,c,p) QuickSortBlock(a,c, sizeof(t), p); 

#include "momo_sort.cpp"

#endif //MOMO_SORT_H
