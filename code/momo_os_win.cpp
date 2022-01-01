#include <windows.h>


static void* 
OS_Memory_Alloc(UMI size) {
  void* result = VirtualAlloc(0, 
                              size,
                              MEM_RESERVE | MEM_COMMIT, 
                              PAGE_READWRITE);
  return result;
}

static void
OS_Memory_Free(void* mem) {
  VirtualFree(mem, 0, MEM_RELEASE);
}