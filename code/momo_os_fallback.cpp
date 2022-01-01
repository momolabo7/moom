#include <stdlib.h>


static void* 
OS_Memory_Alloc(UMI size) {
  void* result = malloc(size);
  return result;
}

static void 
OS_Memory_Free(void* mem) {
  free(size);
}