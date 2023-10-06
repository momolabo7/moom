#include "momo.h"
#include <stdlib.h>
#include <stdio.h>

struct test_t {
  int a;
  int b;
};

static test_t* foo() {
  test_t ret = { 10, 12 };
  return &ret;
}

int main() {
  test_t* t1  = foo();
  test_t* t2  = foo();

  printf("%d %d\n", t1->a, t1->b);
  printf("%d %d\n", t2->a, t2->b);


}

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
