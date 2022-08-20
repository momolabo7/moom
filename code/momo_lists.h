#ifndef MOMO_LISTS_H
#define MOMO_LISTS_H

// Array Lists
#define al_is_full(l)       ((l)->count == array_count((l)->e))
#define al_is_empty(l)      ((l)->count == 0)
#define al_is_valid(l,i)    ((i) < (l)->count) 
#define al_append(l)        (al_is_full(l) ? 0 : (l)->e + (l)->count++)
#define al_clear(l)         ((l)->count = 0) 
#define al_at(l,i)          (al_is_valid(l,i) ? (l)->e + i : 0)
#define al_foreach(i,l)     for(UMI i = 0; i < (l)->count; ++i)

// Slice Lists
#define sl_is_full(l)       ((l)->count == (l)->cap))
#define sl_is_empty(l)      ((l)->count == 0)
#define sl_is_valid(l,i)    ((i) < (l)->count) 
#define sl_append(l)        (sl_is_full(l) ? 0 : (l)->e + (l)->count++)
#define sl_clear(l)         ((l)->count = 0) 
#define sl_at(l,i)          (sl_is_valid(l,i) ? (l)->e + i : 0)
#define sl_foreach(i,l)     for(UMI i = 0; i < (l)->count; ++i)


// Singly Linked Lists
#define sll_prepend(f,l,n) (f) ? (n)->next = (f), (f) = (n) : (f) = (l) = (n) 
#define sll_append(f,l,n) (f) ? (l)->next = (n), (l) = (n) : (f) = (l) = (n)


#endif //MOMO_LISTS_H
