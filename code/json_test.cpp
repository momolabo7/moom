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

  void* mem = malloc(len);
  if (!mem) {
    printf("Cannot allocate memory\n");
    return {0};
  }
  fread(mem, len, 1, fp);
  fclose(fp);

  Bump_Allocator ba = {0}; 
  ba_init(&ba, malloc(MB(1)), MB(1)); 

  make(JSON_Object, json);
  json_read(json, mem, len, &ba);

  JSON_Object* one = json_get_object(json, str8_from_lit("obj"));
  U32* two = json_get_u32(one, str8_from_lit("item3"));

  printf("hello: %d", *two);

}



