
#include "momo.h"
#include <stdio.h>




template<typename... T>
struct tuple_t{};

template<typename H, typename... T>
struct tuple_t<H, T...>
{
  H head;
  tuple_t<T...> tail;
};

template<typename H, typename... T>
static tuple_t<H, T...>
tuple_set(H value, T... rest)
{
  tuple_t<H, T...> ret;
  ret.head = value;
  ret.tail = tuple_set(rest...);
  return ret;
}

static tuple_t<> 
tuple_set()
{
  return tuple_t<>{};
}

template<usz_t I, typename H, typename... T>
static auto 
tuple_get(tuple_t<H, T...> t)
{
  if constexpr(I == 0) 
    return t.head;
  else
    return tuple_get<I-1>(t.tail);
}



int main() 
{
  tuple_t<int, float, int> t = tuple_set(10, 20.f, 30);

  printf("%d\n", tuple_get<0>(t));
  printf("%f\n", tuple_get<1>(t));
  printf("%d\n", tuple_get<2>(t));

}
