//
// USAGE:
//  build 

#include "momo.h"
#include <stdio.h>

#define BUILD_COMPILER_DEFAULT "clang++ -std=c++17 -Wall -Wno-unused-function -Wno-parentheses -Wno-macro-redefined -Wno-deprecated-declarations -Wno-missing-braces -Wno-format"
#define BUILD_CODE_PATH "../code"


struct build_t {
  u8_t buffer[2048];
  bufio_t sb;
};

static void
build_init(build_t* b) 
{
  bufio_init(&b->sb, buf_set(b->buffer, sizeof(b->buffer)));
}

static void
build_app_build(build_t* b, const char* target)
{

  printf("Building app_%s\n", target);
#if 0
  bufio_push_cstr(&b->sb, BUILD_COMPILER_DEFAULT " " BUILD_CODE_PATH "app_");
  bufio_push_cstr(&b->sb, target);
  bufio_push_cstr(&b->sb, ".cpp -shared -o eden.dll");
  bufio_push_null_terminate(&b->sb);
#endif
  bufio_push_fmt(&b->sb, 
      buf_from_lit("%s %s/app_%s.cpp -shared -o -eden.dll"),
      BUILD_COMPILER_DEFAULT,
      BUILD_CODE_PATH,
      target);
  bufio_push_null_terminate(&b->sb);
  //printf((const char*)b->sb.str.e);
  system((char*)b->sb.str.e);
  bufio_clear(&b->sb);
}

static void 
build_pass_build(build_t* b, const char* target)
{
  printf("Building pass_%s\n", target);
  bufio_push_fmt(&b->sb, 
      buf_from_lit("%s %s/pass_%s.cpp -o -pass_%s.exe"),
      BUILD_COMPILER_DEFAULT,
      BUILD_CODE_PATH,
      target,
      target);
  bufio_push_null_terminate(&b->sb);

  //printf((const char*)b->sb.str.e);
  system((char*)b->sb.str.e);
  bufio_clear(&b->sb);
}

// @todo: args
static void 
build_pass_run(build_t* b, const char* target)
{
  printf("Running pass_%s\n", target);
  bufio_push_fmt(&b->sb, 
      buf_from_lit("pass_%s.exe"),
      target);
  bufio_push_null_terminate(&b->sb);

  //printf((const char*)b->sb.str.e);
  system((char*)b->sb.str.e);
  bufio_clear(&b->sb);
}

int main(int argc, char* argv[])
{
  if (argc < 3) {
    // @todo: error message
    printf("Error\n");
    return 1;
  }
  const char* target = argv[1];
  const char* commands = argv[2];

  build_t build;
  build_init(&build);

  const char* itr = commands;
  while(itr[0] != 0)
  {
    if (itr[0] == 'p')
    {
      if (itr[1] == 'b') 
      {
        build_pass_build(&build, target);
      }
      else if (itr[1] == 'r') 
      {
        build_pass_run(&build, target);
      }
      else 
      {
        build_pass_build(&build, target);
        build_pass_run(&build, target);
      }
    }
    else if (itr[0] == 'a')
    {
      build_app_build(&build, target);
    }
    else if (itr[0] == 'w')
    {
      if (itr[1] == 'b') 
      {
      }
      else if (itr[1] == 'r') 
      {
      }
      else 
      {
      }
    }
    else if (itr[0] == 'n')
    {
      if (itr[1] == 'b') 
      {
      }
      else if (itr[1] == 'r') 
      {
      }
      else 
      {
      }
    }

    itr++;
  }

    
}

