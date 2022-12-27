#ifndef MOMO_DS_H
#define MOMO_DS_H


//
// Array lists
// 
#if 0 
#define al_cap(l)           (array_count((l)->e))
#define al_is_full(l)       ((l)->count == array_count((l)->e))
#define al_is_empty(l)      ((l)->count == 0)
#define al_is_valid(l,i)    ((i) < (l)->count) 
#define al_append(l)        (al_is_full(l) ? 0 : (l)->e + (l)->count++)
#define al_clear(l)         ((l)->count = 0) 
#define al_at(l,i)          (al_is_valid(l,i) ? (l)->e + i : 0)
#define al_foreach(i,l)     for(UMI i = 0; i < (l)->count; ++i)
#endif



// Singly Linked Lists
// f - first node
// l - last node
// n - node
#define sll_prepend(f,l,n) (f) ? ((n)->next = (f), (f) = (n)) : ((f) = (l) = (n))
#define sll_append(f,l,n)  (f) ? ((l)->next = (n), (l) = (n)) : ((f) = (l) = (n), (l)->next = 0)

// TODO: Change name to CDL (circular doubly linked list)
// Circular Doubly Linked List with sentinel
// 
// s - sentinel
// n - node
#define cll_init(s)     (s)->prev = (s), (s)->next = (s) 
#define cll_append(s,n) (n)->next = (s), (n)->prev = (s)->prev, (n)->prev->next 
#define cll_remove(n)   (n)->prev->next = (n)->next, (n)->next->prev = (n)->prev;

//
// Circular buffer (queue)
//
#define cb

#endif //MOMO_LISTS_H
