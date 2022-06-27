/* date = January 27th 2022 9:19 am */

#ifndef TEST_PNG_H
#define TEST_PNG_H



void test_png() {
  struct {
    const char* in;
    const char* out;
  } test_cases[] = 
  { 
    test_assets_dir("test_in0.png"), "out0.png",
    test_assets_dir("test_in1.png"), "out1.png",
    test_assets_dir("test_in2.png"), "out2.png",
    test_assets_dir("test_in3.png"), "out3.png",
    test_assets_dir("test_in4.png"), "out4.png",
    test_assets_dir("test_in5.png"), "out5.png",
    test_assets_dir("test_in6.png"), "out6.png",
    test_assets_dir("test_in7.png"), "out7.png",
    test_assets_dir("test_in8.png"), "out8.png",
    
    test_assets_dir("libresprite.png"), "libresprite_out.png",
    test_assets_dir("clip.png"), "clip_out.png",
    test_assets_dir("paint.png"), "paint_out.png",
  }; 
  
  U32 memory_size = MB(100);
  U8* memory = (U8*)malloc(memory_size);
  if (!memory) { 
    test_log("Cannot allocate memory\n");
    return;
  }
  
  for (int i = 0; i < array_count(test_cases); ++i)
  {
    test_log("Test Case: %d\n", i);
    test_create_log_section_until_scope;
    
    Bump_Allocator app_arena = {};
    ba_init(&app_arena, memory, memory_size);
    Memory png_file = test_read_file_to_memory(&app_arena, test_cases[i].in);
    
    if (!is_ok(png_file)){
      test_log("Cannot read file: %d\n", i);
      continue;
    }
    
    PNG png = {};
    if (!png_read(&png, png_file.data, png_file.size)) {
      test_log("Create PNG failed: %d\n", i);
      continue;
    }
    else {
      test_eval_d(png.width);
      test_eval_d(png.height);
    }
    Bitmap bitmap = png_to_bitmap(&png, &app_arena);
    if (!is_ok(bitmap)) {
      test_log("Read PNG failed: %d\n", i);
      continue;
    }
    test_log("Read success: %d\n", i);
    
    Memory png_output = png_write(bitmap, &app_arena); 
    if (!is_ok(png_output)) {
      test_log("Write to memory failed: %d\n", i);
      continue;
    }
    
    if(!test_write_memory_to_file(png_output, test_cases[i].out)) {
      test_log("Cannot write to file: %d\n", i);
      continue;
    }
    test_log("Wrote to file\n");
  }
  
  free(memory);
  
}

#endif //TEST_PNG_H
