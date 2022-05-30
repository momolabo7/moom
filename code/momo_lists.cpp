//~Array_List

template<typename T, U32 N>
static U32 
al_cap(Array_List<T,N>* l) {
  return N;
}

template<typename T, U32 N>
static void
al_clear(Array_List<T,N>* l) {
  l->count = 0;
}

template<typename T, U32 N>
static B32
al_has_space(Array_List<T,N>* l) {
  return l->count < al_cap(l); 
}  

template<typename T, U32 N>
static B32
al_is_empty(Array_List<T,N>* l) {
  return l->count == 0;
}  

template<typename T, U32 N>
static T*
al_push(Array_List<T,N>* l) {
  return l->e + l->count++; 
}  

template<typename T, U32 N>
static B32
al_pop(Array_List<T,N>* l) {
  return --l->count; 
}  

template<typename T, U32 N>
static B32
al_remaining(Array_List<T,N>* l) {
  return l->cap - l->count; 
}  

template<typename T, U32 N>
static B32
al_can_get(Array_List<T,N>* l, U32 index) {
  return index < l->count; 
}  

template<typename T, U32 N>
static T*
al_get(Array_List<T,N>* l, U32 index) {
  return l->e + index; 
}  

template<typename T, U32 N>
static T
al_get_copy(Array_List<T,N>* l, U32 index) {
  return l->e[index]; 
}  

template<typename T, U32 N>
static void
al_push_copy(Array_List<T,N>* l, T item) {
  l->e[l->count++] = item; 
}

template<typename T, U32 N>
static void
al_slear(Array_List<T,N>* l, U32 index) {
  l->e[index] = l->e[l->count-1]; 
  al_pop(l);
}


template<typename T, U32 N>
static void
al_remove(Array_List<T,N>* l, U32 index) {
  copy_memory(l->e+i, 
              l->e+index+1, 
              sizeof(l->e[0])*(l->count--)-index);
}


//~Slice_List
template<typename T>
static void
sl_init(Slice_List<T>* l, T* arr, U32 cap) {
  l->e = arr;
  l->count = 0;
  l->cap = cap;
}

template<typename T>
static U32 
sl_cap(Slice_List<T>* l) {
  return l->cap;
}

template<typename T>
static void
sl_clear(Slice_List<T>* l) {
  l->count = 0;
}

template<typename T>
static B32
sl_has_space(Slice_List<T>* l) {
  return l->count < sl_cap(l); 
}  

template<typename T>
static B32
sl_is_empty(Slice_List<T>* l) {
  return l->count == 0;
}  

template<typename T>
static T*
sl_push(Slice_List<T>* l) {
  return l->e + l->count++; 
}  

template<typename T>
static B32
sl_pop(Slice_List<T>* l) {
  return --l->count; 
}  

template<typename T>
static B32
sl_remaining(Slice_List<T>* l) {
  return l->cap - l->count; 
}  

template<typename T>
static B32
sl_can_get(Slice_List<T>* l, U32 index) {
  return index < l->count; 
}  

template<typename T>
static T*
sl_get(Slice_List<T>* l, U32 index) {
  return l->e + i; 
}  

template<typename T>
static T
sl_get_copy(Slice_List<T>* l, U32 index) {
  return l->e[i]; 
}  

template<typename T>
static void
sl_push_copy(Slice_List<T>* l, T item) {
  l->e[l->count++] = item; 
}

template<typename T>
static void
sl_slear(Slice_List<T>* l, U32 index) {
  l->e[index] = l->e[l->count-1]; 
  sl_pop(l);
}


template<typename T>
static void
sl_remove(Slice_List<T>* l, U32 index) {
  copy_memory(l->e+i, 
              l->e+index+1, 
              sizeof(l->e[0])*(l->count--)-index);
}
