#ifndef MOMO_ARRAY_H
#define MOMO_ARRAY_H


// List helpers



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

// TODO: comments for each macro 
#define slist_cap(l)             (array_count((l)->e))
#define slist_clear(l)           ((l)->count = 0)
#define slist_has_space(l)       ((l)->count < slist_cap(l)) 
#define slist_is_empty(l)				((l)->count == 0)
#define slist_push(l)            ((l)->e + (l)->count++)
#define slist_pop(l)             (--((l)->count));
#define slist_remaining(l)       (slist_cap(l) - (l)->count)
#define slist_push_copy(l,item)  ((l)->e[(l)->count++] = (item))

#define slist_is_index_valid(l,i) ((i) < (l)->count)
#define slist_get(l,i)            ((l)->e + (i))
#define slist_get_copy(l,i)       ((l)->e[i])
#define slist_slear(l,i)				  ((l)->e[i] = (l)->e[(l)->count-1]), slist_pop(l)	
#define slist_remove(l,i)				 (copy_memory((l)->e+i, (l)->e+i+1, sizeof((l)->e[0])*((l)->count--)-i))

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

// TODO: comments for each macro
#define list_init(l,d,c) 	     ((l)->cap = (c), (l)->count = 0, (l)->e = (d))
#define list_is_valid(l)				((l)->cap > 0 && (l)->e != 0)
#define list_cap(l)             ((l)->cap)
#define list_clear(l)           (slist_clear(l))
#define list_has_space(l)       ((l)->count < list_cap(l)) 
#define list_is_empty(l)				(slist_is_empty(l))
#define list_push(l)            (slist_push(l))
#define list_pop(l)             (slist_pop(l));
#define list_remaining(l)       (list_cap(l) - (l)->count)
#define list_push_copy(l,item)  (slist_push_copy(l,item))

#define list_is_index_valid(l,i) (slist_is_index_valid(l,i)
#define list_get(l,i)            (slist_get(l,i))
#define list_get_copy(l,i)       (slist_get_copy(l,i))
#define list_slear(l,i)				  (slist_slear(l,i))	
#define list_remove(l,i)				 (slist_remove(l,i))



#endif //MOMO_ARRAY_H
