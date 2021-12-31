/* date = November 29th 2021 9:58 pm */

#ifndef MOMO_LIST_H
#define MOMO_LIST_H


//~ NOTE(Momo): 'Dynamic' list API
//
// A dynamic array MUST consist of the following named variables:
//  - count: integral type that represents the current 
//           count of the array
//  - cap: integral type that represents the capacity 
//         of the array
//  - e: a pointer to a type. It is expected to be an array.
//
// Example int array struct that fits the requirements:
//  struct {
//    U32 count;
//    U32 cap;
//    int* e;
//  };
//

#define List_Init(l,d,c) 	((l)->cap = (c), (l)->count = 0, (l)->e = (d))
#define List_Alloc(l,a,c)  ((l)->cap = c, (l)->count = 0, Arena_PushBlock(a, sizeof((l)->e[0]) * c))
#define List_Clear(l)      ((l)->count = 0)
#define List_Push(l)       (((l)->count < ((l)->cap)) ? ((l)->e + (l)->count++) : 0)
#define List_Pop(l)        (((l)->count > 0) ? (--(l)->count), 1 : 0)
#define List_Get(l,i)      ((i < (l)->count) ? ((l)->e + i) : 0)
#define List_Slear(l,i)    ((i < (l)->count) ? ((l)->e[i] = (l)->e[(l)->count-1]), List_Pop(l), 1 : 0)
#define List_Remove(l,i)   ((i < (l)->count) ? (Bin_Copy((l)->e+i, (l)->e+i+1, sizeof((l)->e[0])*((l)->count--)-i)), 1 : 0)
#define List_Remain(l)     ((l)->cap - (l)->count)

//~ NOTE(Momo): 'Static' list API
//
// A static array MUST consist of the following named variables:
//  - count: integral type that represents the current 
//           count of the array
//  
//  - e: an array  of elements of the type that the array stores
//
// Example array struct that fits the requirements:
//  struct {
//    U32 count;
//    int e[10];
//  };
//

#define SList_Cap(l) 	     (ArrayCount((l)->e))
#define SList_Clear(l)      ((l)->count = 0)
#define SList_Push(l)       (((l)->count < (SList_Cap(l))) ? ((l)->e + (l)->count++) : 0)
#define SList_Pop(l)        (((l)->count > 0) ? (--(l)->count), 1 : 0)
#define SList_Get(l,i)      ((i < (l)->count) ? ((l)->e + i) : 0)
#define SList_Slear(l,i)    ((i < (l)->count) ? ((l)->e[i] = (l)->e[(l)->count-1]), SList_Pop(l), 1 : 0)
#define SList_Remove(l,i)   ((i < (l)->count) ? (Bin_Copy((l)->e+i, (l)->e+i+1, sizeof((l)->e[0])*((l)->count--)-i)), 1 : 0)
#define SList_Remain(l)     (SList_Cap(l) - (l)->count)

#endif //MOMO_ARRAY_H
