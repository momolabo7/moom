#ifndef MOMO_ARRAY_LIST_H
#define MOMO_ARRAY_LIST_H


// List helpers



//~ NOTE(Momo): Array List with it's own storage
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
#define als_cap(l)                (array_count((l)->e))
#define als_clear(l)              ((l)->count = 0)
#define als_has_space(l)          ((l)->count < als_cap(l)) 
#define als_is_empty(l)	 	      ((l)->count == 0)
#define als_push(l)               ((l)->e + (l)->count++)
#define als_pop(l)                (--((l)->count));
#define als_remaining(l)          (als_cap(l) - (l)->count)
#define als_push_copy(l,item)     ((l)->e[(l)->count++] = (item))
#define als_is_index_valid(l,i)   ((i) < (l)->count)
#define als_get(l,i)              ((l)->e + (i))
#define als_get_copy(l,i)         ((l)->e[i])
#define als_slear(l,i)            ((l)->e[i] = (l)->e[(l)->count-1]), als_pop(l)	
#define als_remove(l,i)           (copy_memory((l)->e+i, (l)->e+i+1, sizeof((l)->e[0])*((l)->count--)-i))
#define als_foreach(i,l)          for(decltype((l)->count) i = 0; i < (l)->count; ++i)


// NOTE(Momo): Same as above but don't need to be in struct
#define alsn_cap(n)                (array_count(n##_store))
#define alsn_clear(n)              (n##_count = 0)
#define alsn_has_space(n)          (n##_count < alsn_cap(n)) 
#define alsn_is_empty(n)	 	      (n##_count == 0)
#define alsn_push(n)               (n##_store + n##_count++)
#define alsn_pop(n)                (--(n##_count));
#define alsn_remaining(n)          (alsn_cap(l) - n##_count)
#define alsn_push_copy(n,item)     (n##_store[n##_count++] = (item))
#define alsn_is_index_valid(n,i)   ((i) < n##_count)
#define alsn_get(n,i)              (n##_store + (i))
#define alsn_get_copy(n,i)         (n##_store[i])
#define alsn_slear(n,i)            (n##_store[i] = n##_store[n##_count-1]), alsn_pop(l)	
#define alsn_remove(n,i)           (copy_memory(n##_store+i, n##_store+i+1, sizeof(n##_store[0])*(n##_count--)-i))
#define alsn_foreach(i,n)          for(decltype(n##_count) i = 0; i < n##_count; ++i)


//~ NOTE(Momo): Array List
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
#define al_init(l,d,c)         ((l)->cap = (c), (l)->count = 0, (l)->e = (d))
#define al_is_valid(l)         ((l)->cap > 0 && (l)->e != 0)
#define al_cap(l)              ((l)->cap)
#define al_clear(l)            (als_clear(l))
#define al_has_space(l)        ((l)->count < al_cap(l)) 
#define al_is_empty(l)         (als_is_empty(l))
#define al_push(l)             (als_push(l))
#define al_pop(l)              (als_pop(l));
#define al_remaining(l)        (al_cap(l) - (l)->count)
#define al_push_copy(l,item)   (als_push_copy(l,item))
#define al_is_index_valid(l,i) (als_is_index_valid(l,i)
#define al_get(l,i)            (als_get(l,i))
#define al_get_copy(l,i)       (als_get_copy(l,i))
#define al_slear(l,i)          (als_slear(l,i))	
#define al_remove(l,i)         (als_remove(l,i))
#define al_foreach(i,l)        for(decltype((l)->count) i = 0; i < (l)->count; ++i)



#endif //MOMO_ARRAY_LIST_H
