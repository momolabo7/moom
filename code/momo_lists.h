#ifndef MOMO_LISTS_H
#define MOMO_LISTS_H

#include "momo_common.h"

#define al_is_full(l)       ((l)->count == array_count((l)->e))
#define al_is_empty(l)      ((l)->count == 0)
#define al_is_valid(l,i)    ((i) < (l)->count) 
#define al_append(l)        (al_is_full(l) ? 0 : (l)->e + (l)->count++)
#define al_clear(l)         ((l)->count = 0) 
#define al_at(l,i)          (al_is_valid(l,i) ? (l)->e + i : 0)
#define al_foreach(i,l)     for(U32 i = 0; i < (l)->count; ++i)


#define sll_prepend(f,l,n) (f) ? (n)->next = (f), (f) = (n) : (f) = (l) = (n) 
#define sll_append(f,l,n) (f) ? (l)->next = (n), (l) = (n) : (f) = (l) = (n)


#if 0

template<typename T, UMI N>
static UMI al_cap(Array_List<T,N>* l);

template<typename T, UMI N>
static void al_clear(Array_List<T,N>* l);

template<typename T, UMI N>
static B32 al_has_space(Array_List<T,N>* l);

template<typename T, UMI N>
static B32 al_is_empty(Array_List<T,N>* l);

template<typename T, UMI N>
static T* al_push(Array_List<T,N>* l);

template<typename T, UMI N>
static void al_pop(Array_List<T,N>* l);

template<typename T, UMI N>
static B32 al_remaining(Array_List<T,N>* l);

template<typename T, UMI N>
static B32 al_can_get(Array_List<T,N>* l, UMI index);

template<typename T, UMI N>
static T* al_get(Array_List<T,N>* l, UMI index);

template<typename T, UMI N>
static T al_get_copy(Array_List<T,N>* l, UMI index);

template<typename T, UMI N>
static void al_push_copy(Array_List<T,N>* l, T item);

template<typename T, UMI N>
static void al_slear(Array_List<T,N>* l, UMI index);

template<typename T, UMI N> 
static void al_remove(Array_List<T,N>* l, UMI index);



////////////////////////////////////////////////////////////////////
// IMPLEMENTATION
template<typename T, UMI N>
static UMI 
al_cap(Array_List<T,N>* l) {
  return N;
}

template<typename T, UMI N>
static void
al_clear(Array_List<T,N>* l) {
  l->count = 0;
}

template<typename T, UMI N>
static B32
al_has_space(Array_List<T,N>* l) {
  return l->count < al_cap(l); 
}  

template<typename T, UMI N>
static B32
al_is_empty(Array_List<T,N>* l) {
  return l->count == 0;
}  

template<typename T, UMI N>
static T*
al_push(Array_List<T,N>* l) {
  assert(l->count < N);
  return l->e + l->count++; 
}  

template<typename T, UMI N>
static void
al_pop(Array_List<T,N>* l) {
  assert(l->count > 0);
  --l->count; 
}  

template<typename T, UMI N>
static B32
al_remaining(Array_List<T,N>* l) {
  return l->cap - l->count; 
}  

template<typename T, UMI N>
static B32
al_can_get(Array_List<T,N>* l, UMI index) {
  return index < l->count; 
}  

template<typename T, UMI N>
static T*
al_get(Array_List<T,N>* l, UMI index) {
  return l->e + index; 
}  

template<typename T, UMI N>
static T
al_get_copy(Array_List<T,N>* l, UMI index) {
  return l->e[index]; 
}  

template<typename T, UMI N>
static void
al_push_copy(Array_List<T,N>* l, T item) {
  assert(l->count < N);
  l->e[l->count++] = item; 
}

template<typename T, UMI N>
static void
al_slear(Array_List<T,N>* l, UMI index) {
  l->e[index] = l->e[l->count-1]; 
  al_pop(l);
}


template<typename T, UMI N>
static void
al_remove(Array_List<T,N>* l, UMI index) {
  copy_memory(l->e+i, 
              l->e+index+1, 
              sizeof(l->e[0])*(l->count--)-index);
}

#endif


#endif //MOMO_ARRAY_LIST_H
