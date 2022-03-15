/* date = January 20th 2022 10:14 am */

#ifndef SUI_H
#define SUI_H

#include <stdlib.h>
#include <stdio.h>

#define assert_callback(s) printf("[ass][assert] %s:%d:%s\n", __FILE__, __LINE__, #s); fflush(stdout);

#include "momo.h"

#define asset_dir(filename) "../assets/" ##filename

static unsigned sui_log_spaces = 0;
#define sui_log(...) { for(unsigned sui_log_spaces_index = 0; sui_log_spaces_index < sui_log_spaces; ++sui_log_spaces_index) { printf(" "); } printf(__VA_ARGS__); };
#define sui_create_log_section_until_scope sui_log_spaces += 2; defer {sui_log_spaces -= 2;}

// Utility files for ass
Memory sui_malloc(UMI size) {
  void* mem = malloc(size);
  assert(mem);
  return { mem, size };
}

void sui_free(Memory* mem) {
  free(mem->data);
  mem->data = nullptr;
  mem->size = 0;
}

Memory sui_read_file(const char* filename, Arena* arena) {
  FILE* file = fopen(filename, "rb");
  
  if (!file) {
    return {};
  }
  defer { fclose(file); };
  fseek(file, 0, SEEK_END);
  UMI file_size = ftell(file);
  fseek(file, 0, SEEK_SET);
  
  void* file_memory = push_block(arena, file_size); 
  assert(file_memory);
  UMI read_amount = fread(file_memory, 1, file_size, file);
  assert(read_amount == file_size);
  
  Memory ret;
  ret.data = file_memory;
  ret.size = file_size; 
  
  return ret;
  
}

void sui_write_file(const char* filename, Memory memory) {
  FILE *file  = fopen(filename, "wb");
  if (!file) return;
  defer { fclose(file); };
  
  fwrite(memory.data, 1, memory.size, file);
}

static TTF 
sui_load_font(const char* filename, Arena* arena) {
  Memory mem = sui_read_file(filename, arena);
  assert(is_ok(mem));
  
  TTF ret = read_ttf(mem);
  return ret;
}

#include "game_asset_types.h"
#include "karu.h"
#include "sui_atlas.h"
#include "sui_pack.h"

#endif //Karu_EXPORT_H
