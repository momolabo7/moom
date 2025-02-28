
#include "momo.h"
#include <stdio.h>

#define BUILD_COMPILER_FLAGS "-std=c++17 -Wall -Wno-unused-function -Wno-parentheses -Wno-macro-redefined -Wno-deprecated-declarations -Wno-missing-braces -Wno-format"

#define BUILD_COMPILER "clang++"

#define BUILD_FLAG_OUTPUT "output="


int main(int argc, char* argv[])
{
  arena_t arena;
  arena_alloc(&arena, gigabytes(1));

  bufio_t sb;
  bufio_init(&sb, arena_push_buffer(&arena, 1024, 16));

  const char* target = 0;

#if 1 
  for(int i = 1; i < argc; ++i)
  {
    if (i == 1)
    {
      target = argv[i];
    }
  }
#endif 

  printf("Compiling %s\n", target);

  bufio_push_fmt(&sb, "%s %s %s\0", BUILD_COMPILER, BUILD_COMPILER_FLAGS, target);

  system(sb.str);
}

