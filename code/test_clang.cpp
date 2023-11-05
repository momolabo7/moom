

#include "momo.h"

#include <stdio.h>
#define assert_callback(s) printf("[pass][assert] %s:%d:%s\n", __FILE__, __LINE__, #s); fflush(stdout);

int main() {
  printf("Hello World\n");

}
