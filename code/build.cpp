// CASES:
// - pass
// - app
// - app optimized (TODO)
// - eden
// - eden optimized (TODO)
// - normal
// - normal optimized
//
//
//
// TODO:
// - remove system() and use OS_specific calls (add system calls to momo.h)
// - seperation between debug and optimized modes
// - shipping mode
//


#include "momo.h"
#include <stdio.h>
#include <direct.h>

#if OS_WINDOWS
# define BUILD_PATH_SEPERATOR "\\"
#else
# define BUILD_PATH_SEPERATOR "/"
#endif 


// @todo: -g and -gcodeview should only be in internal modes
#define BUILD_COMPILE "clang++ -std=c++17 -Wall -Wno-unused-function -Wno-parentheses -Wno-macro-redefined -Wno-deprecated-declarations -Wno-missing-braces -Wno-format -g -gcodeview -march=native "

#define BUILD_COMPILE_PASS \
  BUILD_COMPILE BUILD_CODE_PATH "pass_%s.cpp -o " BUILD_OUT_PATH "pass_%s.exe"

#define BUILD_COMPILE_EDEN \
  BUILD_COMPILE BUILD_CODE_PATH "w32_eden.cpp -o " BUILD_OUT_PATH "w32_eden.exe "

#define BUILD_COMPILE_EDEN_DEBUG \
  BUILD_COMPILE_EDEN "-DEDEN_DEBUG=1"

#define BUILD_COMPILE_EDEN_OPTIMIZED \
  BUILD_COMPILE_EDEN  "-O3" 

#define BUILD_COMPILE_NORMAL \
  BUILD_COMPILE BUILD_CODE_PATH "%s.cpp -o " BUILD_OUT_PATH "%s.exe "

#define BUILD_COMPILE_NORMAL_DEBUG \
  BUILD_COMPILE_NORMAL

#define BUILD_COMPILE_NORMAL_OPTIMIZED \
  BUILD_COMPILE_NORMAL "-O3" 

#define BUILD_COMPILE_APP \
  BUILD_COMPILE BUILD_CODE_PATH "app_%s.cpp -shared -o " BUILD_OUT_PATH "eden.dll "

#define BUILD_COMPILE_APP_DEBUG \
  BUILD_COMPILE_APP "-DEDEN_DEBUG=1" 

#define BUILD_COMPILE_APP_OPTIMIZED \
  BUILD_COMPILE_APP "-O3" 

// @note: build paths
#define BUILD_CODE_PATH "code" BUILD_PATH_SEPERATOR
#define BUILD_OUT_PATH "build" BUILD_PATH_SEPERATOR


struct build_t {
  char origin_path[4096];
  u8_t buffer[2048];
  bufio_t sb;
};

static void
build_init(build_t* b) 
{
  getcwd(b->origin_path, sizeof(b->origin_path));
  bufio_init(&b->sb, buf_set(b->buffer, sizeof(b->buffer)));
}

static void
build_normal_build(build_t* b, const char* target, b32_t optimized)
{
  if (optimized)
  {
    printf("Building %s.cpp optimized\n", target);
    bufio_push_fmt(
        &b->sb, 
        buf_from_lit(BUILD_COMPILE_NORMAL_OPTIMIZED),
        target,
        target);
  }
  else
  {
    printf("Building %s.cpp\n", target);
    bufio_push_fmt(
        &b->sb, 
        buf_from_lit(BUILD_COMPILE_NORMAL_DEBUG),
        target,
        target);
  }
  bufio_push_null_terminate(&b->sb);
  system((char*)b->sb.str.e);
  bufio_clear(&b->sb);
}

static void
build_normal_run(build_t* b, const char* target, char** args, u32_t arg_count)
{
  printf("Running %s.exe\n", target);
  bufio_push_fmt(&b->sb, 
      buf_from_lit(BUILD_OUT_PATH "%s.exe"),
      target);
  for(u32_t arg_index = 0; arg_index < arg_count; ++ arg_index)
  {
    bufio_push_c8(&b->sb, ' ');
    bufio_push_cstr(&b->sb, args[arg_index]);
  }

  bufio_push_null_terminate(&b->sb);
  system((char*)b->sb.str.e);
  bufio_clear(&b->sb);
}

static void
build_app_build(build_t* b, const char* target, b32_t optimized)
{
  if (optimized)
  {
    printf("Building app_%s optimized\n", target);
    bufio_push_fmt(
        &b->sb, 
        buf_from_lit(BUILD_COMPILE_APP_OPTIMIZED),
        target);
  }
  else {
    printf("Building app_%s\n", target);
    bufio_push_fmt(
        &b->sb, 
        buf_from_lit(BUILD_COMPILE_APP_DEBUG),
        target);
  }
  bufio_push_null_terminate(&b->sb);
  //printf((const char*)b->sb.str.e);
  system((char*)b->sb.str.e);
  bufio_clear(&b->sb);
}

static void 
build_eden_build(build_t* b, b32_t optimized)
{
  if (optimized)
  {
    printf("Building w32_eden optimized\n");
    system(BUILD_COMPILE_EDEN_OPTIMIZED);
  }
  else
  {
    printf("Building w32_eden\n");
    system(BUILD_COMPILE_EDEN_DEBUG);
  }
}

// @todo: args
static void 
build_eden_run(build_t* b)
{
  printf("Running w32_eden\n");
  chdir(BUILD_OUT_PATH);
  defer { chdir(b->origin_path); };
  system("w32_eden.exe");
}

static void 
build_pass_build(build_t* b, const char* target)
{
  printf("Building pass_%s\n", target);
  bufio_push_fmt(&b->sb, 
      buf_from_lit(BUILD_COMPILE_PASS),
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

  chdir(BUILD_OUT_PATH);
  defer { chdir(b->origin_path); };

  bufio_push_fmt(&b->sb, 
      buf_from_lit("pass_%s.exe"),
      target);
  bufio_push_null_terminate(&b->sb);

  //printf((const char*)b->sb.str.e);
  system((char*)b->sb.str.e);
  bufio_clear(&b->sb);
}




int main(int argc, char** argv)
{
  //printf("Version 3\n");
  if (argc < 3) {
    // @todo: error message
    printf("Error\n");
    return 1;
  }
  const char* target = argv[1];
  const char* commands = argv[2];
  char** args = argv + 3;
  u32_t arg_count = argc - 3;

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
        ++itr;
      }
      else if (itr[1] == 'r') 
      {
        build_pass_run(&build, target);
        ++itr;
      }
      else 
      {
        build_pass_build(&build, target);
        build_pass_run(&build, target);
      }
    }
    else if (itr[0] == 'a')
    {
      build_app_build(&build, target, false);
    }
    else if (itr[0] == 'A')
    {
      build_app_build(&build, target, true);
    }
    else if (itr[0] == 'e')
    {
      if (itr[1] == 'b') 
      {
        build_eden_build(&build, false);
        ++itr;
      }
      else if (itr[1] == 'r') 
      {
        build_eden_run(&build);
        ++itr;
      }
      else 
      {
        build_eden_build(&build, false);
        build_eden_run(&build);
      }
    }
    else if (itr[0] == 'E')
    {
      if (itr[1] == 'b') 
      {
        build_eden_build(&build, true);
        ++itr;
      }
      else if (itr[1] == 'r') 
      {
        build_eden_run(&build);
        ++itr;
      }
      else 
      {
        build_eden_build(&build, true);
        build_eden_run(&build);
      }
    }
    else if (itr[0] == 'n')
    {
      if (itr[1] == 'b') 
      {
        build_normal_build(&build, target, false);
        ++itr;
      }
      else if (itr[1] == 'r') 
      {
        build_normal_run(&build, target, args, arg_count);
        ++itr;
      }
      else 
      {
        build_normal_build(&build, target, false);
        build_normal_run(&build, target, args, arg_count);
      }
    }
    else if (itr[0] == 'N')
    {
      if (itr[1] == 'b') 
      {
        build_normal_build(&build, target, true);
        ++itr;
      }
      else if (itr[1] == 'r') 
      {
        build_normal_run(&build, target, args, arg_count);
        ++itr;
      }
      else 
      {
        build_normal_build(&build, target, true);
        build_normal_run(&build, target, args, arg_count);
      }
    }
    itr++;
  }

    
}

