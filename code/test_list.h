/* date = May 15th 2022 8:47 pm */

#ifndef TEST_LIST_H
#define TEST_LIST_H



static void
test_list() {
  struct {
    U32 count;
    U32 e[10];
  } _a = {};
  auto* a = &_a;
  
  
  test_log("is empty (should be 1): %d\n", als_is_empty(a));
  test_log("has space (should be 1): %d\n", als_has_space(a));
  
  test_log("adding numbers list...\n");
  {
    test_create_log_section_until_scope;
    for (U32 i = 0; i < 20; ++i) {
      if (als_has_space(a)) {
        als_push_item(a,i);
      }
    }
    
    test_log("a is now: ");
    for (U32 i = 0; i < a->count; ++i) {
      printf("%d ", a->e[i]);
    }
    test_log("\n");
  }
  test_log("is empty (should be 0): %d\n", als_is_empty(a));
  test_log("has space (should be 0): %d\n", als_has_space(a));
  
  
  
  test_log("popping last two values...\n");
  {
    test_create_log_section_until_scope;
    als_pop(a);
    als_pop(a);
    test_log("a is now: ");
    for (U32 i = 0; i < a->count; ++i) {
      printf("%d ", a->e[i]);
    }
    test_log("\n");
  }
  test_log("has space (should be 0): %d\n", als_has_space(a));
  
  test_log("is index 7 valid (should be 1): %d\n", als_is_index_valid(a,7));
  test_log("is index 8 valid (should be 0): %d\n", als_is_index_valid(a,8));
  test_log("is index 9 valid (should be 0): %d\n", als_is_index_valid(a,9));
  
  test_log("removing index 3 (4th element)...\n");
  {
    test_create_log_section_until_scope;
    als_remove(a, 3);
    test_log("a is now: ");
    for (U32 i = 0; i < a->count; ++i) {
      printf("%d ", als_get_copy(a,i));
    }
    test_log("\n");
  }
  
  test_log("slearing index 3 (4th element)...\n");
  {
    test_create_log_section_until_scope;
    als_slear(a, 3);
    test_log("a is now: ");
    for (U32 i = 0; i < a->count; ++i) {
      printf("%d ", *als_get(a,i));
    }
    test_log("\n");
  }
  
  
  test_log("remaining: %lld\n", als_remaining(a));
  test_log("cap: %lld\n", als_cap(a));
  
  
}
#endif //TEST_LIST_H
