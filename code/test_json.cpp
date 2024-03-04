#include <stdlib.h>
#include <stdio.h>

#define FOOLISH
#include "momo.h"

int main() {
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

