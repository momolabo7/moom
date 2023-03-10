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
  json_read(json, mem, len, &ba);

#if 0
  auto* val = json_get_value(json, str8_from_lit("boolean"));
  if(val) {
    printf("debug: %d\n", val->element.begin);
    printf("debug: %d\n", json_is_value_true(json, val));
  }
#endif



  //json_object_t* one = json_get_object(json, str8_from_lit("obj"));
  //u32_t* two = json_get_u32(one, str8_from_lit("item3"));

  //printf("hello: %d", *two);

}



