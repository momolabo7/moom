#ifndef MOMO_LISTS_H
#define MOMO_LISTS_H


template<typename T> 
struct Slice {
  U32 count;
  T* e;
};

template<typename T>
struct Slice_List {
  U32 count;
  U32 cap;
  T* e;
};

template<typename T>
static void sl_init(Slice_List<T>* l, T* arr, U32 cap);

#if 0
// NOTE(Momo): I don't actually know how useful this actually 
// is within our current use cases...
template<typename T>
static void sl_make(Slice_List<T>* l, U32 cap, Arena arena);
#endif

template<typename T>
static U32 sl_cap(Slice_List<T>* l);

template<typename T>
static void sl_clear(Slice_List<T>* l);

template<typename T>
static B32 sl_has_space(Slice_List<T>* l);

template<typename T>
static B32 sl_is_empty(Slice_List<T>* l);

template<typename T>
static T* sl_push(Slice_List<T>* l);

template<typename T>
static B32 sl_pop(Slice_List<T>* l);

template<typename T>
static B32 sl_remaining(Slice_List<T>* l);

template<typename T>
static B32 sl_can_get(Slice_List<T>* l, U32 index);

template<typename T>
static T* sl_get(Slice_List<T>* l, U32 index);

template<typename T>
static T sl_get_copy(Slice_List<T>* l, U32 index);

template<typename T>
static void sl_push_copy(Slice_List<T>* l, T item);

template<typename T>
static void sl_slear(Slice_List<T>* l, U32 index);

template<typename T> 
static void sl_remove(Slice_List<T>* l, U32 index);

#define sl_foreach(i,l)  for(decltype((l)->count) i = 0; i < (l)->count; ++i)

//~ Array_List
template<typename T, U32 N = 256>
struct Array_List {
  U32 count;
  U32 cap;
  T e[N];
};


template<typename T, U32 N>
static U32 al_cap(Array_List<T,N>* l);

template<typename T, U32 N>
static void al_clear(Array_List<T,N>* l);

template<typename T, U32 N>
static B32 al_has_space(Array_List<T,N>* l);

template<typename T, U32 N>
static B32 al_is_empty(Array_List<T,N>* l);

template<typename T, U32 N>
static T* al_push(Array_List<T,N>* l);

template<typename T, U32 N>
static B32 al_pop(Array_List<T,N>* l);

template<typename T, U32 N>
static B32 al_remaining(Array_List<T,N>* l);

template<typename T, U32 N>
static B32 al_can_get(Array_List<T,N>* l, U32 index);

template<typename T, U32 N>
static T* al_get(Array_List<T,N>* l, U32 index);

template<typename T, U32 N>
static T al_get_copy(Array_List<T,N>* l, U32 index);

template<typename T, U32 N>
static void al_push_copy(Array_List<T,N>* l, T item);

template<typename T, U32 N>
static void al_slear(Array_List<T,N>* l, U32 index);

template<typename T, U32 N> 
static void al_remove(Array_List<T,N>* l, U32 index);

#define al_foreach(i,l)  for(decltype((l)->count) i = 0; i < (l)->count; ++i)

#include "momo_lists.cpp"

#endif //MOMO_ARRAY_LIST_H
