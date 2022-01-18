#include <stdio.h>
#include <stdlib.h>

#include "momo.h"




static inline Memory
ReadFileToMemory(Arena* arena, const char* filename) {
	Memory result = {};
  FILE* file = fopen(filename, "rb");
  if (!file) { 
    printf("Cannot find file\n");
    return result;
  }
  
  fseek(file, 0, SEEK_END);
  S32 file_size = ftell(file);
  fseek(file, 0, SEEK_SET);
  
  void* file_memory = Arena_PushBlock(arena, file_size, 4);
  fread(file_memory, 1, file_size, file); 
  
  result.data = file_memory;
  result.size = file_size;
	
  fclose(file);
  
  return result;
}


static inline B32
WriteMemoryToFile(Memory block, const char* filename) {
	FILE* file = fopen(filename, "wb");
	if (!file) {
		printf("Cannot open file for writing\n");
		return false;
	}
	fwrite(block.data, sizeof(char), block.size, file);
  
  fclose(file);
	
	return true;
	
}

typedef struct {
  const char* in;
  const char* out;
} TestCase;

TestCase g_test_cases[] = {
  "in0.png", "out0.png",
  "in1.png", "out1.png",
  "in2.png", "out2.png",
  "in3.png", "out3.png",
  "in4.png", "out4.png",
  "in5.png", "out5.png",
}; 

int main() {    
	
  U32 memory_size = MB(10);
  void * memory = malloc(memory_size);
  if (!memory) { return 1; }
  
  for (int i = 0; i < ArrayCount(g_test_cases); ++i)
  {
    printf("=== Test Case: %d ===\n", i);
    Arena app_arena = Arena_Create(memory, memory_size);
    Memory png_file = ReadFileToMemory(&app_arena, g_test_cases[i].in);
    
    if (!png_file.data){
      printf("Cannot read file: %d\n", i);
      continue;
    }
    
    Image bitmap = PNG_Read(png_file, &app_arena);
    if (!bitmap.data) {
      printf("Read PNG failed: %d\n", i);
      continue;
    }
    printf("Read success: %d\n", i);
    
    Memory png_output = PNG_Write(bitmap,
                                        &app_arena); 
    if (!png_output.data) {
      printf("write to memory failed: %d\n", i);
      continue;
    }
    
    if(!WriteMemoryToFile(png_output, g_test_cases[i].out)) {
      printf("Cannot write to file: %d\n", i);
      continue;
    }
    printf("Wrote to file\n");
  }
  
  free(memory);
  
  return 0;
}

