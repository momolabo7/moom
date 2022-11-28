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
static void* 
sui_read_file(const char* filename, UMI* out_size, Arena* allocator) {
  FILE *file = fopen(filename, "rb");
  if (!file) return false;
  defer { fclose(file); };

  fseek(file, 0, SEEK_END);
  UMI file_size = ftell(file);
  fseek(file, 0, SEEK_SET);
 
  //sui_log("%s, %lld\n", filename, file_size);
  void* file_blk = arn_push_size(allocator, file_size, 16); 
  if (!file_blk) return false;
  UMI read_amount = fread(file_blk, 1, file_size, file);
  if(read_amount != file_size) return false;
  
  if (out_size) *out_size = file_size;
  
  return file_blk;;
  
}

static B32
sui_write_file(const char* filename, void* memory, UMI memory_size) {
  FILE *file = fopen(filename, "wb");
  if (!file) return false;
  defer { fclose(file); };
  
  fwrite(memory, 1, memory_size, file);
  return true;
}

static B32 
sui_read_font_from_file(TTF* ttf, const char* filename, Arena* allocator) {

  UMI size;
  void* mem = sui_read_file(filename, &size, allocator); 

  if (!sui_read_file(filename, &size, allocator)) 
    return false;
  return ttf_read(ttf, mem, size);
}

static B32 
sui_read_wav_from_file(WAV* wav, const char* filename, Arena* allocator) {

  UMI size;
  void* mem = sui_read_file(filename, &size, allocator); 
  if(!mem)
    return false;
  return wav_read(wav, mem, size);
}

#include "karu.h"
#include "sui_atlas.h"
#include "sui_pack.h"

#endif 
