#include "momo.h"
#include <stdio.h>



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
