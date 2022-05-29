#ifndef MOMO_LISTS_H
#define MOMO_LISTS_H


template<typename T> 
struct Slice {
  U32 count;
  T* e;
};

template<typename T>
struct SliceList {
  U32 count;
  U32 cap;
  T* e;
};

template<typename T>
static void sl_init(SliceList<T>* l, T* arr, U32 cap);

template<typename T>
static U32 sl_cap(SliceList<T>* l);

template<typename T>
static void sl_clear(SliceList<T>* l);

template<typename T>
static B32 sl_has_space(SliceList<T>* l);

template<typename T>
static B32 sl_is_empty(SliceList<T>* l);

template<typename T>
static T* sl_push(SliceList<T>* l);

template<typename T>
static B32 sl_pop(SliceList<T>* l);

template<typename T>
static B32 sl_remaining(SliceList<T>* l);

template<typename T>
static B32 sl_can_get(SliceList<T>* l, U32 index);

template<typename T>
static T* sl_get(SliceList<T>* l, U32 index);

template<typename T>
static T sl_get_copy(SliceList<T>* l, U32 index);

template<typename T>
static void sl_push_copy(SliceList<T>* l, T item);

template<typename T>
static void sl_slear(SliceList<T>* l, U32 index);

template<typename T> 
static void sl_remove(SliceList<T>* l, U32 index);

#define sl_foreach(i,l)  for(decltype((l)->count) i = 0; i < (l)->count; ++i)

//~ ArrayList
template<typename T, U32 N = 256>
struct ArrayList {
  U32 count;
  U32 cap;
  T e[N];
};


template<typename T, U32 N>
static U32 al_cap(ArrayList<T,N>* l);

template<typename T, U32 N>
static void al_clear(ArrayList<T,N>* l);

template<typename T, U32 N>
static B32 al_has_space(ArrayList<T,N>* l);

template<typename T, U32 N>
static B32 al_is_empty(ArrayList<T,N>* l);

template<typename T, U32 N>
static T* al_push(ArrayList<T,N>* l);

template<typename T, U32 N>
static B32 al_pop(ArrayList<T,N>* l);

template<typename T, U32 N>
static B32 al_remaining(ArrayList<T,N>* l);

template<typename T, U32 N>
static B32 al_can_get(ArrayList<T,N>* l, U32 index);

template<typename T, U32 N>
static T* al_get(ArrayList<T,N>* l, U32 index);

template<typename T, U32 N>
static T al_get_copy(ArrayList<T,N>* l, U32 index);

template<typename T, U32 N>
static void al_push_copy(ArrayList<T,N>* l, T item);

template<typename T, U32 N>
static void al_slear(ArrayList<T,N>* l, U32 index);

template<typename T, U32 N> 
static void al_remove(ArrayList<T,N>* l, U32 index);

#define al_foreach(i,l)  for(decltype((l)->count) i = 0; i < (l)->count; ++i)

#include "momo_lists.cpp"

#endif //MOMO_ARRAY_LIST_H
