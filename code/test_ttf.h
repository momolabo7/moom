/* date = January 31st 2022 0:39 pm */

#ifndef TEST_TTF_H
#define TEST_TTF_H

#include "momo_ttf.h"




void test_ttf() {
  test_create_log_section_until_scope;
  
  U32 memory_size = MB(1);
  void * memory = malloc(memory_size);
  if (!memory) { 
    test_log("Cannot allocate memory");
    return;
  }
  defer { free(memory); };
  
  
  Arena main_arena = create_arena(memory, memory_size);
  Memory ttf_memory = test_read_file_to_memory(&main_arena, 
#if 0
                                               test_assets_dir("nokiafc22.ttf")
#else
                                               test_assets_dir("DroidSansMono.ttf")
#endif
                                               );
  
  
  
  TTF ttf = read_ttf(ttf_memory);
  F32 scale_factor = get_scale_for_pixel_height(&ttf, 256.f);
  
  for (U32 codepoint = 85; codepoint <= 85; ++codepoint) {
    test_log("codepoint %X\n", codepoint);
    create_scratch(scratch, &main_arena);
    
    U32 glyph_index = get_glyph_index_from_codepoint(&ttf, codepoint);
    Image codepoint_image = rasterize_glyph(&ttf, glyph_index, scale_factor, scratch);
    {
      U8 buffer[256];
      Str8Bld strbld= create_str8bld(buffer, 256); 
      strbld.push_format(str8_from_lit("%d.png"), codepoint);
      strbld.push_C8(0);
      
      Memory image_mem = write_image_as_png(codepoint_image, scratch);
      test_write_memory_to_file(image_mem, (const char*)strbld.e);
    }
  }
  
  
  
  
}


#endif //TEST_TTF_H
