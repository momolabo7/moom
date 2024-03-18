#include "momo.h"
#include <stdio.h>

enum os_file_access_t {
  OS_FILE_ACCESS_READ,   // Only reads the file, does not write
  OS_FILE_ACCESS_CREATE, // Creates or truncates a file for writing, as if it's new
  OS_FILE_ACCESS_MODIFY, // Write to a file, but does not truncate.
};

struct os_file_t {
#if OS_WINDOWS
  HANDLE handle;
#elif OS_LINUX
  int handle;
#else
# warning "Not implemented"
#endif
};

#if OS_WINDOWS
#include <windows.h>
static void
os_file_close(os_file_t* fp) 
{
  if (fp->handle) {
    CloseHandle(fp->handle);
  }
}

static b32_t
os_file_open(
    os_file_t* fp, 
    const char* filename,
    os_file_access_t access_type) 
{
  DWORD w32_access_flag = 0;
  DWORD w32_creation_flag = 0;
  switch (access_type) {
    case OS_FILE_ACCESS_READ:
      w32_access_flag = GENERIC_READ;
      w32_creation_flag = OPEN_EXISTING;
      break;
    case OS_FILE_ACCESS_CREATE:
      w32_access_flag = GENERIC_WRITE | GENERIC_READ;
      w32_creation_flag = CREATE_ALWAYS;
      break;
    case OS_FILE_ACCESS_MODIFY:
      w32_access_flag = GENERIC_WRITE | GENERIC_READ;
      w32_creation_flag = OPEN_ALWAYS;
      break;
  }

  HANDLE handle = 
    CreateFile(
        filename,
        w32_access_flag,
        FILE_SHARE_READ,
        NULL,
        w32_creation_flag,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
  if (handle == INVALID_HANDLE_VALUE) return false;

  fp->handle = handle;
  return true;
}

static b32_t
os_file_read(os_file_t* fp, u8_t* dest, usz_t size, usz_t offset) {
  
  // Reading the file
  OVERLAPPED overledened = {};
  overledened.Offset = (u32_t)((offset >> 0) & 0xFFFFFFFF);
  overledened.OffsetHigh = (u32_t)((offset >> 32) & 0xFFFFFFFF);
  
  DWORD bytes_read;
  
  if(ReadFile(fp->handle, dest, (DWORD)size, &bytes_read, &overledened) &&
     (DWORD)size == bytes_read) 
  {
    return true;
  }
  else {
    return false;
  }
}

static b32_t
os_file_write(os_file_t* fp, const u8_t* src, usz_t size, usz_t offset) 
{
  OVERLAPPED overledened = {};
  overledened.Offset = (u32_t)((offset >> 0) & 0xFFFFFFFF);
  overledened.OffsetHigh = (u32_t)((offset >> 32) & 0xFFFFFFFF);
  
  DWORD bytes_wrote;
  if(WriteFile(fp->handle, src, (DWORD)size, &bytes_wrote, &overledened) &&
     (DWORD)size == bytes_wrote) 
  {
    return true;
  }
  else {
    return false;
  }
}

static u64_t
os_file_size(os_file_t* fp) {
  LARGE_INTEGER file_size;
  if (!GetFileSizeEx(fp->handle, &file_size)) {
    return 0;
  }
  
  u64_t ret = (u64_t)file_size.QuadPart;
  return ret;
}

#elif OS_LINUX
#include <fcntl.h>
#include <unistd.h>

static void
os_file_close(os_file_t* fp) 
{
  if (fp->handle) {
    close(fp->handle);
  }
}

static b32_t
os_file_open(
    os_file_t* fp, 
    const char* filename,
    os_file_access_t access_type) 
{
  // NOTE(momo): O_RDONLY is 0
  int flags = 0;
  switch (access_type) {
    case OS_FILE_ACCESS_READ:
      flags = O_RDONLY;
      break;
    case OS_FILE_ACCESS_CREATE:
      flags = O_CREAT | O_RDWR;
      break;
    case OS_FILE_ACCESS_MODIFY:
      flags = O_RDWR;
      break;
  }

  int handle = open(filename, flags);
  if (handle == -1) 
    return false;

  fp->handle = handle;
  return true;
}

static b32_t
os_file_read(os_file_t* fp, u8_t* dest, usz_t size, usz_t offset) {
  if (lseek(fp->handle, offset, SEEK_SET) == -1) {
    return false;
  }
  if (read(fp->handle, dest, size) == -1) {
    return false;
  }

  return true;
}

static b32_t
os_file_write(os_file_t* fp, const u8_t* src, usz_t size, usz_t offset) 
{
  if (lseek(fp->handle, offset, SEEK_SET) == -1) {
    return false;
  }
  if (write(fp->handle, src, size) == -1) {
    return false;
  }
  return true;
}

static u64_t
os_file_size(os_file_t* fp) {
  // NOTE(momo): We don't have to 'reset' the seek position 
  // because of how our read/write API works...at least for now
  // until we decide to store seek positions.
  return lseek(fp->handle, 0, SEEK_END);
}

#endif

static str_t 
os_read_file_into_str(const char* filename, arena_t* arena, b32_t null_terminate = false) {
  os_file_t file = {};
  if (!os_file_open(&file, filename, OS_FILE_ACCESS_READ)) {
    return str_bad();
  }
  defer { os_file_close(&file); };

  u64_t file_size = os_file_size(&file);

  str_t ret = arena_push_str(arena, file_size + null_terminate, 16);
  if (!ret) {
    return str_bad();
  }


  if (!os_file_read(&file, ret.e, file_size, 0)) {
    return str_bad();
  }

  if (null_terminate) 
    ret.e[file_size] = 0;  

  return ret;
}

static b32_t
os_write_str_to_file(const char* filename, str_t buffer) {
  os_file_t file = {};
  if (!os_file_open(&file, filename, OS_FILE_ACCESS_CREATE)) {
    return false; 
  }

  defer { os_file_close(&file); };
  os_file_write(&file, buffer.e, buffer.size, 0);

  return true;
}

int main() 
{
  arena_t arena = {};
  arena_alloc(&arena, gigabytes(1)); 
  defer { arena_free(&arena); }; 

  str_t str = os_read_file_into_str("test.csv", &arena);
  if (str) {
    printf("%s",str.e);
  }
  
  if (!os_write_str_to_file("test3.csv", str)) {
    return str_bad();
  }

}
