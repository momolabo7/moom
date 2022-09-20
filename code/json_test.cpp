#include <stdio.h>
#include <stdlib.h>

struct Json_Memory {
  char* data;
  long size;
};

static Json_Memory 
json_read_file_to_memory(const char* filename) {
  FILE* fp = fopen(filename, "r");
  if (!fp) { 
    printf("Cannot open file\n");
    return {0};
  }
  printf("Hel\n");
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

  Json_Memory ret = {0};
  ret.data = (char*)mem;
  ret.size = len;


  return ret;
}

typedef struct  {

} Json_Tokenizer;

int main() {
  printf("Hello World\n");
  Json_Memory json_memory = json_read_file_to_memory("test_json.json");
  printf("Json read!\n");

  for(long i = 0; i < json_memory.size; ++i) {
    printf("%c", *(json_memory.data + i));
  }
  printf("\n");

  free(json_memory.data);
}



