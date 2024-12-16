
#include "momo.h"

#include <stdio.h>

int main() 
{
  const char pat[] = "привет мир";
  for_arr(i, pat) {
    printf("%2X\n", pat[i]);
  }
}
