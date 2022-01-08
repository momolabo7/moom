#include <stdio.h>

#include "../code/momo.h"

#define EvalD(s) printf(#s " = %d\n", s);
#define EvalF(s) printf(#s " = %f\n", s);

int main() {
  
  
  EvalD(Min(5, 10));
  EvalD(Max(5, 10));
  EvalD(Clamp(11, 0, 10));
  EvalD(Clamp(-1, 0, 10));
  EvalD(Abs(-1));    // invokes S32 version
  EvalF(Abs(-12.f)); // invokes F32 version
  EvalF(Abs(-12.0)); // involes F64 version
  EvalD(Lerp(10, 20, 0.5f));
  EvalD(AlignDownPow2(15, 4));
  EvalD(AlignUpPow2(15, 4));
  EvalD(IsPow2(10));
  EvalD(IsPow2(16));
  EvalF(Ratio(2.f, 1.f, 10.f));
  EvalF(Ratio(2.0, 0.0, 100));
  EvalF(DegToRad(180.f));
  EvalF(DegToRad(180.0));
  EvalF(RadToDeg(pi_F32));
  EvalF(RadToDeg(pi_F64));
  
  
  
  // Test Swap
  {
    int i = 5;
    int j = 10;
    printf("Before swap: i = %d, j = %d\n", i, j);
    Swap(i, j);
    printf("After swap: i = %d, j = %d\n", i, j);
  }
  
  // Test PtrToInt() and IntToPtr()
  {
    int i = 5;
    printf("PtrToInt(%p) = %llX\n", &i, PtrToInt(&i));
    printf("IntToPtr(%X) = %p\n", 0x1234, IntToPtr(0x1234));
  } 
  
  // Test endian swap 16
  {
    union {
      U16 u;
      char c[2];
    } val;
    
    val.u = 12345;
    printf("Before EndianSwap16: ");
    for(int i = 0; i < ArrayCount(val.c); ++i) 
      printf("%02X ", val.c[i]);
    printf("\n");
    val.u = EndianSwap16(val.u);
    
    printf("After EndianSwap16: ");
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
    printf("Before EndianSwap32: ");
    for(int i = 0; i < ArrayCount(val.c); ++i) 
      printf("%02X ", val.c[i]);
    printf("\n");
    val.u = EndianSwap32(val.u);
    
    printf("After EndianSwap32: ");
    for(int i = 0; i < ArrayCount(val.c); ++i) 
      printf("%02X ", val.c[i]);
    printf("\n");
    
  }
  
}

