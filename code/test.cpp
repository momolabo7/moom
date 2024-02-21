#include <stdlib.h>
#include <stdio.h>

#define FOOLISH
#include "momo.h"

#if 0
void test_json() {
  const char json_str[] = "{\
    \"car\": 23, \
    \"bus\": [1,2,3], \
    \"str\": \"hello\", \
    \"student\": { \
      \"id\": 12345, \
      \"name\": \"Gerald\", \
    }\
  }";

  arena_t ba = {}; 
  arena_init(&ba, buffer_set(malloc(megabytes(1)), megabytes(1)); 

  make(json_t, json);
  auto* obj =  json_read(json, (u8_t*)json_str, array_count(json_str), &ba);


  // Printing "car"
  {
    auto* val = json_get_value(obj, str_from_lit("car"));
    if(val) {
      auto* element = json_get_element(val);
      s32_t out = 0;
      str_to_s32(element->str, &out);
      printf("%d\n", out);
    }
  }

  // Printing "str"
  {
    auto* val = json_get_value(obj, str_from_lit("str"));
    if(val) {
      auto* element = json_get_element(val);
      s32_t out = 0;
      str_to_s32(element->str, &out);
      for_cnt(i, element->str.size) {
        printf("%c", element->str.e[i]);
      }
      printf("\n");
    }
  }

#if 0
    json_array_t* arr = json_get_array(val);
    if (arr) {
      for(json_array_node_t* itr = arr->head;
          itr != 0; 
          itr = itr->next) 
      {
        json_value_t* val2 = &itr->value; 
        if (json_is_number(val2)) {
          json_element_t* element = json_get_element(val2);
          s32_t out = 0;
          str_to_s32(element->str, &out);
          printf("%d\n", out);
        }
      }
    }
#endif



  //json_object_t* one = json_get_object(json, str8_from_lit("obj"));
  //u32_t* two = json_get_u32(one, str8_from_lit("item3"));

  //printf("hello: %d", *two);

}
#endif

#if 0
static void
test_print_free_blocks(garena_t* ga) {
  auto* itr = ga->free_list;
  printf("Free Blocks: ");
  while(itr != nullptr) {
    printf("[%llu: %llu] ", ptr_to_umi(itr) - ptr_to_umi(ga->memory), itr->size);
    itr = itr->next;
  }
  printf("\n");
}


static void test_print_memory(garena_t* ga, void* addr, const char* name) 
{
  printf("[%s] %llu\n", name, ptr_to_umi(addr) - ptr_to_umi(ga->memory));
}

static void test_assert_16(void* addr) {
  assert(ptr_to_umi(addr) % 16 == 0);
}

int main() {
  printf("Starting test\n");
  u8_t* memory = (u8_t*)malloc(1000);
  make(garena_t, ga);
  garena_init(ga, memory, 1000);
  test_print_free_blocks(ga);

  printf("Allocating m1 @ 100 bytes\n");
  void* m1 = garena_allocate_size(ga,100);
  test_print_free_blocks(ga);
  printf("Allocating m2 @ 100 bytes\n");
  void* m2 = garena_allocate_size(ga,100);
  test_print_free_blocks(ga);
  printf("Allocating m3 @ 100 bytes\n");
  void* m3 = garena_allocate_size(ga,100);
  test_print_free_blocks(ga);
  printf("Allocating m4 @ 590 bytes\n");
  void* m4 = garena_allocate_size(ga,590);
  test_print_free_blocks(ga);

  test_print_memory(ga, m1, "m1");
  test_print_memory(ga, m2, "m2");
  test_print_memory(ga, m3, "m3");
  test_print_memory(ga, m4, "m4");

  test_assert_16(m1);
  test_assert_16(m2);
  test_assert_16(m3);
  test_assert_16(m4);

  test_print_free_blocks(ga);

  printf("Freeing m2\n");
  garena_free(ga, m2);
  test_print_free_blocks(ga);

  printf("Allocating m2 @ 100 bytes\n");
  m2 = garena_allocate_size(ga,100);
  test_print_free_blocks(ga);

  printf("Freeing m2\n");
  garena_free(ga, m2);
  test_print_free_blocks(ga);

  printf("Freeing m1\n");
  garena_free(ga, m1);
  test_print_free_blocks(ga);

  
  printf("Freeing m4\n");
  garena_free(ga, m4);
  test_print_free_blocks(ga);

  printf("Freeing m3\n");
  garena_free(ga, m3);
  test_print_free_blocks(ga);

}
#endif

int main() {
  printf("Hello Test\n");
  arena_t test = {};

  const u32_t runs = 10000000;
  // arena with committed memory
  {
    arena_alloc(&test, gigabytes(1), true);
    u64_t start_time = os_get_clock_time();
    for (int i = 0; i < runs; ++i) { 
      arena_push_size(&test, sizeof(u32_t), alignof(u32_t));  
    }
    u64_t end_time = os_get_clock_time();
    printf("arena #0: %f\n", (f32_t)(end_time - start_time)/runs);
  }

  // arena with non-committed memory
  {
    arena_alloc(&test, gigabytes(1));
    defer { arena_free(&test); };
    u64_t start_time = os_get_clock_time();
    for (int i = 0; i < runs; ++i) { 
      arena_push_size(&test, sizeof(u32_t), alignof(u32_t));  
    }
    u64_t end_time = os_get_clock_time();
    printf("arena #1: %f\n", (f32_t)(end_time - start_time)/runs);
  }

  // Olde arena
  {
    arena_t normal_arena = {};
    str_t buffer = foolish_allocate_memory(gigabytes(1));
    defer { foolish_free_memory(buffer); };
    arena_init(&normal_arena, buffer);

    u64_t start_time = os_get_clock_time();
    for (int i = 0; i < runs; ++i) { 
      arena_push_size(&normal_arena, sizeof(u32_t), alignof(u32_t));  
    }
    u64_t end_time = os_get_clock_time();

    printf("arena #2: %f\n", (f32_t)(end_time - start_time)/runs);
  }

 
}

