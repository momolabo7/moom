//~ArrayList

template<typename T, U32 N>
static U32 
al_cap(ArrayList<T,N>* l) {
  return N;
}

template<typename T, U32 N>
static void
al_clear(ArrayList<T,N>* l) {
  l->count = 0;
}

template<typename T, U32 N>
static B32
al_has_space(ArrayList<T,N>* l) {
  return l->count < al_cap(l); 
}  

template<typename T, U32 N>
static B32
al_is_empty(ArrayList<T,N>* l) {
  return l->count == 0;
}  

template<typename T, U32 N>
static T*
al_push(ArrayList<T,N>* l) {
  return l->e + l->count++; 
}  

template<typename T, U32 N>
static B32
al_pop(ArrayList<T,N>* l) {
  return --l->count; 
}  

template<typename T, U32 N>
static B32
al_remaining(ArrayList<T,N>* l) {
  return l->cap - l->count; 
}  

template<typename T, U32 N>
static B32
al_can_get(ArrayList<T,N>* l, U32 index) {
  return index < l->count; 
}  

template<typename T, U32 N>
static T*
al_get(ArrayList<T,N>* l, U32 index) {
  return l->e + index; 
}  

template<typename T, U32 N>
static T
al_get_copy(ArrayList<T,N>* l, U32 index) {
  return l->e[index]; 
}  

template<typename T, U32 N>
static void
al_push_copy(ArrayList<T,N>* l, T item) {
  l->e[l->count++] = item; 
}

template<typename T, U32 N>
static void
al_slear(ArrayList<T,N>* l, U32 index) {
  l->e[index] = l->e[l->count-1]; 
  al_pop(l);
}


template<typename T, U32 N>
static void
al_remove(ArrayList<T,N>* l, U32 index) {
  copy_memory(l->e+i, 
              l->e+index+1, 
              sizeof(l->e[0])*(l->count--)-index);
}


//~SliceList
template<typename T>
static void
sl_init(SliceList<T>* l, T* arr, U32 cap) {
  l->e = arr;
  l->count = 0;
  l->cap = cap;
}

template<typename T>
static U32 
sl_cap(SliceList<T>* l) {
  return l->cap;
}

template<typename T>
static void
sl_clear(SliceList<T>* l) {
  l->count = 0;
}

template<typename T>
static B32
sl_has_space(SliceList<T>* l) {
  return l->count < sl_cap(l); 
}  

template<typename T>
static B32
sl_is_empty(SliceList<T>* l) {
  return l->count == 0;
}  

template<typename T>
static T*
sl_push(SliceList<T>* l) {
  return l->e + l->count++; 
}  

template<typename T>
static B32
sl_pop(SliceList<T>* l) {
  return --l->count; 
}  

template<typename T>
static B32
sl_remaining(SliceList<T>* l) {
  return l->cap - l->count; 
}  

template<typename T>
static B32
sl_can_get(SliceList<T>* l, U32 index) {
  return index < l->count; 
}  

template<typename T>
static T*
sl_get(SliceList<T>* l, U32 index) {
  return l->e + i; 
}  

template<typename T>
static T
sl_get_copy(SliceList<T>* l, U32 index) {
  return l->e[i]; 
}  

template<typename T>
static void
sl_push_copy(SliceList<T>* l, T item) {
  l->e[l->count++] = item; 
}

template<typename T>
static void
sl_slear(SliceList<T>* l, U32 index) {
  l->e[index] = l->e[l->count-1]; 
  sl_pop(l);
}


template<typename T>
static void
sl_remove(SliceList<T>* l, U32 index) {
  copy_memory(l->e+i, 
              l->e+index+1, 
              sizeof(l->e[0])*(l->count--)-index);
}
