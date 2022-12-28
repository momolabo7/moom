/* date = January 31st 2022 0:39 pm */

#ifndef TEST_TTF_H
#define TEST_TTF_H




void test_ttf() {
  test_create_log_section_until_scope;
  
  u32_t memory_size = MB(2);
  void * memory = malloc(memory_size);
  if (!memory) { 
    test_log("Cannot allocate memory");
    return;
  }
  defer { free(memory); };
  
  make(arena_t, allocator);
  arena_init(allocator, memory, memory_size);
  Block ttf_memory = 
    test_read_file_to_memory(allocator, 
#if 0 
                             test_assets_dir("nokiafc22.ttf")                                          
#else                                               
                             test_assets_dir("arial.ttf")
#endif
                             );
  
  
  assert(blk_ok(ttf_memory));
  make(ttf_t, ttf);
  ttf_read(ttf, ttf_memory.data, ttf_memory.size);
  
  test_log("Testing rasterization\n");
  {
    test_create_log_section_until_scope;
    f32_t scale_factor = ttf_get_scale_for_pixel_height(ttf, 512.f);
    for (u32_t codepoint = 65; codepoint <= 65+26; ++codepoint) {
      //for (u32_t codepoint = 87; codepoint <= 87; ++codepoint) {
      test_log("rasterizing codepoint %X\n", codepoint);
      arena_set_revert_point(allocator);
      
      u32_t glyph_index = ttf_get_glyph_index(ttf, codepoint);
      Image32 codepoint_image = ttf_rasterize_glyph(ttf, glyph_index, scale_factor, allocator);
      {
        sb8_make(strbld, 256);
        sb8_push_fmt(strbld, str8_from_lit("%u.png\0"), codepoint);
        
        Block image_mem = png_write_img32_to_blk(codepoint_image, allocator);
        test_write_memory_to_file(image_mem, (const char*)strbld->e);
      }
    }
  }
  
 
#if 0
  test_log("Testing kerning\n");
  {
    test_create_log_section_until_scope;
    u32_t s = 65;
    u32_t e = 65+26;
    for (u32_t cp1 = s; cp1 <= e; ++cp1) {
      for (u32_t cp2 = s; cp2 <= e; ++cp2) {
        test_log("(%d, %d) = %d\n", cp1, cp2, ttf_get_glyph_kerning(ttf, cp1, cp2));
      }
    }
  }
#endif
  
  
  
}
#endif //TEST_TTF_H
