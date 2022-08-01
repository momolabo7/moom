#ifndef MOMO_LISTS_H
#define MOMO_LISTS_H

#include "momo_common.h"

// idk man i feel like i have been dealing with this problem for too long
struct Slice_List {
  void* data;
  UMI count;
  UMI cap;
};

static void  
sl_init(Slice_List* l, void* data, UMI cap) {
  l->data = data;
  l->count = 0;
  l->cap = cap;
}

static void 
sl_clear(Slice_List* l) {
  l->count = 0;
}

static B32 
sl_has_space(Slice_List* l) {
  return l->count < l->cap;
}


static void*
sl_push_block(Slice_List* l, void* e, UMI size) {
  // This is surprisingly easy to reason with;
  // since the compiler does the same thing for
  // copying 'objects'. 
  if (sl_has_space(l)) {
    void* dst = (U8*)l->data + size*l->count++;
    copy_memory(dst, e, size);
    return dst;
  }

  return 0;
}
static void*
sl_get_block(Slice_List* l, UMI index, UMI size) {
  return index < l->count ? (U8*)l->data + index * size : 0; 
}


#define sl_push(l,e) sl_push_block(l, &(e), sizeof(e))
#define sl_get(l,t,i) (t*)sl_get_block(l,i,sizeof(t)) 

//~ Array_List
template<typename T, UMI N = 256>
struct Array_List {
  UMI count;
  T e[N];
};


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

#define al_foreach(i,l)  for(decltype((l)->count) i = 0; i < (l)->count; ++i)


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




#endif //MOMO_ARRAY_LIST_H
