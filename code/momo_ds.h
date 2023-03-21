#ifndef MOMO_DS_H
#define MOMO_DS_H

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
#define cll_append(s,n) (n)->next = (s), (n)->prev = (s)->prev, (n)->prev->next = (n), (n)->next->prev = (n)
#define cll_remove(n)   (n)->prev->next = (n)->next, (n)->next->prev = (n)->prev, (n)->next = 0, (n)->prev = 0;



#endif 
