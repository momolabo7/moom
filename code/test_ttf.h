/* date = January 31st 2022 0:39 pm */

#ifndef TEST_TTF_H
#define TEST_TTF_H




void test_ttf() {
  test_create_log_section_until_scope;
  
  U32 memory_size = MB(2);
  void * memory = malloc(memory_size);
  if (!memory) { 
    test_log("Cannot allocate memory");
    return;
  }
  defer { free(memory); };
  
  declare_and_pointerize(Arena, arena);
  init_arena(arena, memory, memory_size);
  Memory ttf_memory = 
    test_read_file_to_memory(arena, 
#if 0 
                             test_assets_dir("nokiafc22.ttf")                                          
#else                                               
                             test_assets_dir("arial.ttf")
#endif
                             );
  
  
  assert(is_ok(ttf_memory));
  TTF ttf = read_ttf(ttf_memory);
  
  test_log("Testing rasterization\n");
  {
    test_create_log_section_until_scope;
    F32 scale_factor = get_scale_for_pixel_height(&ttf, 512.f);
    for (U32 codepoint = 65; codepoint <= 65+26; ++codepoint) {
      //for (U32 codepoint = 87; codepoint <= 87; ++codepoint) {
      test_log("rasterizing codepoint %X\n", codepoint);
      set_arena_reset_point(arena);
      
      U32 glyph_index = get_glyph_index_from_codepoint(&ttf, codepoint);
      Bitmap codepoint_image = rasterize_glyph(&ttf, glyph_index, scale_factor, arena);
      {
        U8 buffer[256];
        StringBld strbld= create_stringbld(buffer, 256); 
        strbld.push_format(string_from_lit("%d.png"), codepoint);
        strbld.push_C8(0);
        
        Memory image_mem = write_bitmap_as_png(codepoint_image, arena);
        test_write_memory_to_file(image_mem, (const char*)strbld.e);
      }
    }
  }
  
  
  test_log("Testing kerning\n");
  {
    test_create_log_section_until_scope;
    U32 s = 65;
    U32 e = 65+26;
    for (U32 cp1 = s; cp1 <= e; ++cp1) {
      for (U32 cp2 = s; cp2 <= e; ++cp2) {
        test_log("(%d, %d) = %d\n", cp1, cp2, get_glyph_kerning(&ttf, cp1, cp2));
      }
    }
  }
  
  
}
#endif //TEST_TTF_H
