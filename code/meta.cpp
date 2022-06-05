#include <stdio.h>
#include "momo.h"

enum Token_Type {
  TOKEN_TYPE_IDENTIFIER,
  TOKEN_TYPE_PARAN_OPEN,
  TOKEN_TYPE_PARAN_CLOSE,
};

struct Token {
  Token_Type type;
  
  UMI begin;
  UMI ope;
};

static Memory
meta_read_file_into_memory(char* filename) {
  FILE* fp = fopen("meta_test.cpp", "r");
  if (fp) {
    fseek(fp, SEEK_END);
    int size = ftell(fp);
    fseek(fp, SEEK_SET);
    
    Memory memory = {};
    
    // TODO: use arena
    malloc(size);
    
    fread(fp, size, );
    
    fclose(fp);
  }
  return Memory;
  
}


int main() {
  
}
