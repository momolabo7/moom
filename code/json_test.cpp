#include <stdlib.h>
#include <stdio.h>

#define JSON_DEBUG 1

#include "momo.h"



int main() {
 FILE* fp = fopen("test_json.json", "r");
  if (!fp) { 
    printf("Cannot open file\n");
    return {0};
  }
  fseek(fp, 0, SEEK_END);
  long len = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  u8_t* mem = (u8_t*)malloc(len);
  if (!mem) {
    printf("Cannot allocate memory\n");
    return {0};
  }
  fread(mem, len, 1, fp);
  fclose(fp);

  arena_t ba = {0}; 
  arena_init(&ba, malloc(megabytes(1)), megabytes(1)); 

  make(json_t, json);
  auto* obj =  json_read(json, mem, len, &ba);

  auto* val = json_get_value(obj, str8_from_lit("arr"));
  if(val) {
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
          str8_t s = str8(element->at, element->count);
          b32_t test = str8_to_s32(s, &out);
          printf("%d\n", out);
        }
      }
    }
  }



  //json_object_t* one = json_get_object(json, str8_from_lit("obj"));
  //u32_t* two = json_get_u32(one, str8_from_lit("item3"));

  //printf("hello: %d", *two);

}



