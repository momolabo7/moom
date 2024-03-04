#include "momo.h"
#include <stdio.h>


struct bigint_t 
{
  u8_t* arr;
  u32_t cap;
  u32_t count; 
};

static void 
bigint_zero(bigint_t* b) {
  for_cnt(i, b->cap) {
    b->arr[i] = 0;
  }
  b->count = 1;
}

static void 
bigint_set_max(bigint_t* b) {
  for_cnt(i, b->cap) {
    b->arr[i] = 9;
  }
  b->count = b->cap;
}

static b32_t 
bigint_init(bigint_t* b, u32_t cap, arena_t* arena) 
{
  assert(cap > 0);
  b->arr = arena_push_arr(u8_t, arena, cap);
  if (!b->arr) return false;
  b->cap = cap;
  bigint_zero(b);
  return true;
}

static void
bigint_set(bigint_t* b, u32_t value) {
  u32_t index = 0;
  while (value > 0) {
    if (index >= b->cap) {
      bigint_set_max(b);
      return;
    }

    b->arr[index] = (u8_t)(value % 10); 
    value /= 10;
    ++index;
  }

  if (index > b->count) {
    b->count = index;
  }
}

static void 
bigint_add(bigint_t* b, u32_t value) {
  u32_t index = 0;
  u8_t carry = 0;
  while (value > 0) {
    if (index >= b->cap) {
      bigint_set_max(b);
      return;
    }
    u8_t extracted_value = (u8_t)(value % 10);
    u8_t result = extracted_value + carry + b->arr[index];
    if (result >= 10) {
      carry = 1;
      result -= 10;
    }
    else {
      carry = 0;
    }
    b->arr[index] = result; 
    value /= 10;
    ++index;

  }

  while(carry > 0) {
    if (index >= b->cap) {
      bigint_set_max(b);
      return;
    }
    u8_t result = b->arr[index] + carry;
    if (result >= 10) {
      carry = 1;
      result -= 10;
    }
    else {
      carry = 0;
    }
    b->arr[index] = result;
    ++index;
  }

  if (index > b->count) {
    b->count = index;
  }
}


// -1 if lhs < rhs
// 0  if lhs == rhs
// 1  if lhs > rhs
static s32_t
bigint_compare(bigint_t* lhs, bigint_t* rhs) 
{
  if (lhs->count < rhs->count) 
    return -1;
  else if (lhs->count > rhs->count) 
    return 1;
  else 
  {
    for (u32_t i = 0; i < lhs->count; ++i) {
      u32_t index = lhs->count - 1 - i;
      if (lhs->arr[index] < rhs->arr[index]) {
        return -1;
      }
      else if (lhs->arr[index] > rhs->arr[index]) {
        return 1;
      }
    }

  }

  return 0;
}


int main() {
  make(arena_t, a);
  arena_alloc(a, megabytes(256), false);

  make(bigint_t, b1);
  bigint_init(b1, 32, a);
  bigint_add(b1, 123);
  bigint_add(b1, 123);

  make(bigint_t, b2);
  bigint_init(b2, 32, a);
  bigint_add(b2, 123);

  printf("%d\n", bigint_compare(b1, b2));
  for_cnt(i, b1->count) {
    printf("%d", b1->arr[i]);
  }
  printf("\n");


}
