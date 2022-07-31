#ifndef SUI_H
#define SUI_H

#include <stdlib.h>
#include <stdio.h>

#define assert_callback(s) printf("[sui][assert] %s:%d:%s\n", __FILE__, __LINE__, #s); fflush(stdout);

#include "momo.h"

#define code_dir(filename) "../code/" filename
#define asset_dir(filename) "../assets/" filename

static unsigned sui_log_spaces = 0;
#define sui_log(...) { for(unsigned sui_log_spaces_index = 0; sui_log_spaces_index < sui_log_spaces; ++sui_log_spaces_index) { printf(" "); } printf(__VA_ARGS__); };
#define sui_create_log_section_until_scope sui_log_spaces += 2; defer {sui_log_spaces -= 2;}


// Utility files for ass
static Memory 
sui_malloc(UMI size) {
  void* mem = malloc(size);
  assert(mem);
  return { mem, size };
}

static void 
sui_free(Memory* mem) {
  free(mem->data);
  mem->data = nullptr;
  mem->size = 0;
}

static B32 
sui_read_file_to_memory(Memory* mem, const char* filename, Bump_Allocator* allocator) {
  FILE *file = fopen(filename, "rb");
  if (!file) return false;
  defer { fclose(file); };

  fseek(file, 0, SEEK_END);
  UMI file_size = ftell(file);
  fseek(file, 0, SEEK_SET);
 
  //sui_log("%s, %lld\n", filename, file_size);
  void* file_memory = ba_push_block(allocator, file_size); 
  assert(file_memory);
  UMI read_amount = fread(file_memory, 1, file_size, file);
  assert(read_amount == file_size);
  
  mem->data = file_memory;
  mem->size = file_size; 
  
  return true;
  
}

static B32
sui_write_file_from_memory(const char* filename, Memory memory) {
  FILE *file = fopen(filename, "wb");
  if (!file) return false;
  defer { fclose(file); };
  
  fwrite(memory.data, 1, memory.size, file);
  return true;
}

static B32 
sui_read_font_from_file(TTF* ttf, const char* filename, Bump_Allocator* allocator) {
  declare_and_pointerize(Memory, mem);
  if (!sui_read_file_to_memory(mem, filename, allocator)) 
    return false;
  return ttf_read(ttf, mem->data, mem->size);
}

static B32 
sui_read_wav_from_file(WAV* wav, const char* filename, Bump_Allocator* allocator) {
  declare_and_pointerize(Memory, mem);
  if(!sui_read_file_to_memory(mem, filename, allocator))
    return false;
  return wav_read(wav, mem->data, mem->size);
}

#endif //Karu_EXPORT_H
