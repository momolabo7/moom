#include "test.h"

int main() {
#if 0
  test_unit(test_essentials());
  test_unit(test_sort());
  test_unit(test_png());
  test_unit(test_ttf());
#endif
  test_unit(test_list());
}