#include <stdio.h>
#include <stdlib.h>
#include "momo.h"

#define test_assets_dir(filename) "../assets/test/" ##filename
#define test_log(...) printf(__VA_ARGS__);
#define test_eval_d(s) printf(#s " = %d\n", s);
#define test_eval_f(s) printf(#s " = %f\n", s);
#define test_eval_member_d(var, member) test_log(#member ": %d\n", (var)->member); 

#define test_unit(unit_name) test_log("=== "#unit_name " start ===\n"); unit_name; test_log("=== " #unit_name " end ===\n\n"); 


static inline Memory
test_read_file_to_memory(Arena* arena, const char* filename) {
	Memory result = {0};
  FILE* file = fopen(filename, "rb");
  if (!file) { 
    test_log("Cannot find file\n");
    return result;
  }
  
  fseek(file, 0, SEEK_END);
  S32 file_size = ftell(file);
  fseek(file, 0, SEEK_SET);
  
  void* file_memory = push_block(arena, file_size, 4);
  fread(file_memory, 1, file_size, file); 
  
  result.data = file_memory;
  result.size = file_size;
	
  fclose(file);
  
  return result;
}


static inline B32
test_write_memory_to_file(Memory block, const char* filename) {
	FILE* file = fopen(filename, "wb");
	if (!file) {
		test_log("Cannot open file for writing\n");
		return false;
	}
	fwrite(block.data, sizeof(char), block.size, file);
  
  fclose(file);
	
	return true;
	
}

struct TTF_Offset_Table {
  U32 scaler_type;
  U16 num_tables;
  U16 search_range;
  U16 entry_selector;
  U16 range_shift;
};

struct TTF_Table_Directory {
  U32 tag;
  U32 checksum;
  U32 offset;
  U32 len;
};


void test_ttf() {
  U32 memory_size = MB(10);
  void * memory = malloc(memory_size);
  if (!memory) { return 1; }
  defer { free(memory); };
  
  Arena main_arena = create_arena(memory, memory_size);
  Memory ttf_memory = read_file_to_memory(&main_arena, "nokiafc22.ttf");
  {
    auto* offsets = (TTF_Offset_Table*)ttf_memory.data;
    offsets->scaler_type = endian_swap_32(offsets->scaler_type);
    offsets->num_tables = endian_swap_16(offsets->num_tables);
    offsets->search_range = endian_swap_16(offsets->search_range);
    offsets->entry_selector = endian_swap_16(offsets->entry_selector);
    offsets->range_shift = endian_swap_16(offsets->range_shift);
    
    test_log("=== OFFSET TABLE ===\n");
    test_eval_member_d(offsets, scaler_type);
    test_eval_member_d(offsets, num_tables);
    test_eval_member_d(offsets, search_range);
    test_eval_member_d(offsets, entry_selector);
    test_eval_member_d(offsets, range_shift);
    
  }
}




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
  }; 
  
  U32 memory_size = MB(10);
  U8* memory = (U8*)malloc(memory_size);
  if (!memory) { 
    test_log("Cannot allocate memory\n");
    return;
  }
  
  for (int i = 0; i < ArrayCount(test_cases); ++i)
  {
    test_log("Test Case: %d ===\n", i);
    Arena app_arena = create_arena(memory, memory_size);
    Memory png_file = test_read_file_to_memory(&app_arena, test_cases[i].in);
    
    if (!is_ok(png_file)){
      test_log("Cannot read file: %d\n", i);
      continue;
    }
    
    Image bitmap = read_png(png_file, &app_arena);
    if (!is_ok(bitmap)) {
      test_log("Read PNG failed: %d\n", i);
      continue;
    }
    test_log("Read success: %d\n", i);
    
    Memory png_output = write_png(bitmap,
                                  &app_arena); 
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

void test_sort() {
  int arr[] = { 1,4,6,8,9,20,13,-1, -20 };
  quicksort(arr, ArrayCount(arr), 
            [](int* lhs, int* rhs) { return (*lhs) < (*rhs); } );
  
  for( auto&& itr : arr) {
    printf("%d ", itr);
  }
  printf("\n");
}
void test_essentials() { 
  test_eval_d(min_of(5, 10));
  test_eval_d(max_of(5, 10));
  test_eval_d(clamp(11, 0, 10));
  test_eval_d(clamp(-1, 0, 10));
  test_eval_d(abs_of(-1));    // invokes S32 version
  test_eval_f(abs_of(-12.f)); // invokes F32 version
  test_eval_f(abs_of(-12.0)); // involes F64 version
  test_eval_d(lerp(10, 20, 0.5f));
  test_eval_d(align_down_pow2(15, 4));
  test_eval_d(align_up_pow2(15, 4));
  test_eval_d(is_pow2(10));
  test_eval_d(is_pow2(16));
  test_eval_f(ratio(2.f, 1.f, 10.f));
  test_eval_f(ratio(2.0, 0.0, 100));
  test_eval_f(deg_to_rad(180.f));
  test_eval_f(deg_to_rad(180.0));
  test_eval_f(rad_to_deg(PI_32));
  test_eval_f(rad_to_deg(PI_64));
  
  
  // Test swap
  {
    int i = 5;
    int j = 10;
    test_log("Before swap: i = %d, j = %d\n", i, j);
    swap(&i, &j);
    test_log("After swap: i = %d, j = %d\n", i, j);
  }
  
  // Test ptr_to_int() and int_to_ptr()
  {
    int i = 5;
    test_log("ptr_to_int(%p) = %llX\n", &i, ptr_to_int(&i));
    test_log("int_to_ptr(%X) = %p\n", 0x1234, int_to_ptr(0x1234));
  } 
  
  // Test endian swap 16
  {
    union {
      U16 u;
      char c[2];
    } val;
    
    val.u = 12345;
    test_log("Before endian_swap_16: ");
    for(int i = 0; i < ArrayCount(val.c); ++i) 
      test_log("%02X ", val.c[i]);
    test_log("\n");
    val.u = endian_swap_16(val.u);
    
    test_log("After endian_swap_16: ");
    for(int i = 0; i < ArrayCount(val.c); ++i) 
      test_log("%02X ", val.c[i]);
    test_log("\n");
    
  }
  
  // Test endian swap 32
  {
    union {
      U32 u;
      char c[4];
    } val;
    
    val.u = 12345;
    test_log("Before endian_swap_32: ");
    for(int i = 0; i < ArrayCount(val.c); ++i) 
      test_log("%02X ", val.c[i]);
    test_log("\n");
    val.u = endian_swap_32(val.u);
    
    test_log("After endian_swap_32: ");
    for(int i = 0; i < ArrayCount(val.c); ++i) 
      test_log("%02X ", val.c[i]);
    test_log("\n");
    
  }
  
}

int main() {
  test_unit(test_essentials());
  test_unit(test_sort());
  test_unit(test_png());
}