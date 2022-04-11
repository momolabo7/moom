//


template<typename T> static Array<T> 
array(T* data, UMI count) {
  Array<T> ret;
  ret.e = data;
  ret.count = count;
  return ret;
}

template<typename T> static List<T> 
list(T* data, UMI cap) {
  List<T> ret;
  ret.e = data;
  ret.count = 0;
  ret.cap = cap;
  
  return ret;
}

template<typename T> static void
push_back(List<T>* l, T item) {
  assert(l->count < l->cap);
  l->e[l->count++] = item;
}

template<typename T> static void
clear(List<T>* l) {
  l->count = 0;
}


// for ranged based loop in Array<T>
template<typename T> struct _Array_Itr {
  T* p;
  
  T& operator*() { return *p; };
  B32 operator!=(const _Array_Itr<T>& rhs) {
    return p != rhs.p;
  }
  void operator++() { ++p; };
};


template<typename T> static _Array_Itr<T>
begin(Array<T>& arr) {
  return { arr.e };
}

template<typename T> static _Array_Itr<T>
end(Array<T>& arr) {
  return { arr.e + arr.count };
}

// for ranged based loop in List<T>
template<typename T> struct _List_Itr {
  T* p;
  
  T& operator*() { return *p; };
  B32 operator!=(const _List_Itr<T>& rhs) {
    return p != rhs.p;
  }
  void operator++() { ++p; };
};


template<typename T> static _List_Itr<T>
begin(List<T>& arr) {
  return { arr.e };
}

template<typename T> static _List_Itr<T>
end(List<T>& arr) {
  return { arr.e + arr.count };
}
