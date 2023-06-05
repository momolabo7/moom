#ifndef SUI_H
#define SUI_H

#include <stdlib.h>
#include <stdio.h>

#define assert_callback(s) printf("[sui][assert] %s:%d:%s\n", __FILE__, __LINE__, #s); fflush(stdout);

#include "momo.h"
#include "momo_asset_file.h"

#define sui_code_dir(filename) "../code/" filename

static u32_t sui_log_paces = 0;
#define sui_log(...) { \
  for(u32_t sui_log_paces_index = 0; \
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
static buffer_t  
sui_read_file(const char* filename, arena_t* allocator) {
  FILE *file = fopen(filename, "rb");
  if (!file) return buffer_invalid();
  defer { fclose(file); };

  fseek(file, 0, SEEK_END);
  usz_t file_size = ftell(file);
  fseek(file, 0, SEEK_SET);
 
  //sui_log("%s, %lld\n", filename, file_size);
  buffer_t file_contents = arena_push_buffer(allocator, file_size, 16);
  if (!file_contents) return buffer_invalid();
  usz_t read_amount = fread(file_contents.data, 1, file_size, file);
  if(read_amount != file_size) return buffer_invalid();
  
  return file_contents;
  
}

static b32_t
sui_write_file(const char* filename, buffer_t buffer) {
  FILE *file = fopen(filename, "wb");
  if (!file) return false;
  defer { fclose(file); };
  
  fwrite(buffer.data, 1, buffer.size, file);
  return true;
}

static b32_t 
sui_read_font_from_file(ttf_t* ttf, const char* filename, arena_t* allocator) {
  buffer_t file_contents = sui_read_file(filename, allocator); 
  if (!file_contents) return false;
  return ttf_read(ttf, file_contents);
}

static b32_t 
sui_read_wav_from_file(wav_t* wav, const char* filename, arena_t* allocator) {
  buffer_t file_contents = sui_read_file(filename, allocator); 
  if(!file_contents) return false;
  return wav_read(wav, file_contents);
}

#include "sui_atlas.h"
#include "sui_pack.h"

#endif 
