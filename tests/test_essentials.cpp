#include <stdio.h>

#include "../code/momo.h"

#define EvalD(s) printf(#s " = %d\n", s);
#define EvalF(s) printf(#s " = %f\n", s);

int main() {
  
  
  EvalD(min_of(5, 10));
  EvalD(max_of(5, 10));
  EvalD(clamp(11, 0, 10));
  EvalD(clamp(-1, 0, 10));
  EvalD(abs_of(-1));    // invokes S32 version
  EvalF(abs_of(-12.f)); // invokes F32 version
  EvalF(abs_of(-12.0)); // involes F64 version
  EvalD(lerp(10, 20, 0.5f));
  EvalD(align_down_pow2(15, 4));
  EvalD(align_up_pow2(15, 4));
  EvalD(is_pow2(10));
  EvalD(is_pow2(16));
  EvalF(ratio(2.f, 1.f, 10.f));
  EvalF(ratio(2.0, 0.0, 100));
  EvalF(deg_to_rad(180.f));
  EvalF(deg_to_rad(180.0));
  EvalF(rad_to_deg(PI_32));
  EvalF(rad_to_deg(PI_64));
  
  
  
  // Test swap
  {
    int i = 5;
    int j = 10;
    printf("Before swap: i = %d, j = %d\n", i, j);
    swap(i, j);
    printf("After swap: i = %d, j = %d\n", i, j);
  }
  
  // Test ptr_to_int() and int_to_ptr()
  {
    int i = 5;
    printf("ptr_to_int(%p) = %llX\n", &i, ptr_to_int(&i));
    printf("int_to_ptr(%X) = %p\n", 0x1234, int_to_ptr(0x1234));
  } 
  
  // Test endian swap 16
  {
    union {
      U16 u;
      char c[2];
    } val;
    
    val.u = 12345;
    printf("Before endian_swap_16: ");
    for(int i = 0; i < ArrayCount(val.c); ++i) 
      printf("%02X ", val.c[i]);
    printf("\n");
    val.u = endian_swap_16(val.u);
    
    printf("After endian_swap_16: ");
    for(int i = 0; i < ArrayCount(val.c); ++i) 
      printf("%02X ", val.c[i]);
    printf("\n");
    
  }
  
  // Test endian swap 32
  {
    union {
      U32 u;
      char c[4];
    } val;
    
    val.u = 12345;
    printf("Before endian_swap_32: ");
    for(int i = 0; i < ArrayCount(val.c); ++i) 
      printf("%02X ", val.c[i]);
    printf("\n");
    val.u = endian_swap_32(val.u);
    
    printf("After endian_swap_32: ");
    for(int i = 0; i < ArrayCount(val.c); ++i) 
      printf("%02X ", val.c[i]);
    printf("\n");
    
  }
  
}

