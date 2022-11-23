#ifndef SUI_H
#define SUI_H

#include <stdlib.h>
#include <stdio.h>

#define assert_callback(s) printf("[sui][assert] %s:%d:%s\n", __FILE__, __LINE__, #s); fflush(stdout);

#include "momo.h"

#define sui_code_dir(filename) "../code/" filename
#define sui_asset_dir(filename) "../assets/" filename

static U32 sui_log_paces = 0;
#define sui_log(...) { \
  for(U32 sui_log_paces_index = 0; \
      sui_log_paces_index < sui_log_paces; \
      ++sui_log_paces_index) \
  { \
    printf(" "); \
  } \
  printf(__VA_ARGS__); \
};

#define sui_create_log_section_until_scope \
  sui_log_paces += 2; \
  defer {sui_log_paces -= 2;}



// Utility files for ass
static Block 
sui_malloc(UMI size) {
  void* mem = malloc(size);
  assert(mem);
  return { mem, size };
}

static void 
sui_free(Block* mem) {
  free(mem->data);
  mem->data = nullptr;
  mem->size = 0;
}

static B32 
sui_read_file_to_blk(Block* mem, const char* filename, Bump_Allocator* allocator) {
  FILE *file = fopen(filename, "rb");
  if (!file) return false;
  defer { fclose(file); };

  fseek(file, 0, SEEK_END);
  UMI file_size = ftell(file);
  fseek(file, 0, SEEK_SET);
 
  //sui_log("%s, %lld\n", filename, file_size);
  void* file_blk = ba_push_size(allocator, file_size, 16); 
  if (!file_blk) return false;
  UMI read_amount = fread(file_blk, 1, file_size, file);
  if(read_amount != file_size) return false;
  
  mem->data = file_blk;
  mem->size = file_size; 
  
  return true;
  
}

static B32
sui_write_file_from_blk(const char* filename, Block blk) {
  FILE *file = fopen(filename, "wb");
  if (!file) return false;
  defer { fclose(file); };
  
  fwrite(blk.data, 1, blk.size, file);
  return true;
}

static B32 
sui_read_font_from_file(TTF* ttf, const char* filename, Bump_Allocator* allocator) {
  make(Block, mem);
  if (!sui_read_file_to_blk(mem, filename, allocator)) 
    return false;
  return ttf_read(ttf, mem->data, mem->size);
}

static B32 
sui_read_wav_from_file(WAV* wav, const char* filename, Bump_Allocator* allocator) {
  make(Block, mem);
  if(!sui_read_file_to_blk(mem, filename, allocator))
    return false;
  return wav_read(wav, mem->data, mem->size);
}

#include "karu.h"
#include "sui_atlas.h"
#include "sui_pack.h"

#endif 
