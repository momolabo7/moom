/* date = January 27th 2022 9:19 am */

#ifndef TEST_ESSENTIALS_H
#define TEST_ESSENTIALS_H

void test_essentials() { 
  test_eval_d(min_of(5, 10));
  test_eval_d(max_of(5, 10));
  test_eval_d(clamp(11, 0, 10));
  test_eval_d(clamp(-1, 0, 10));
  test_eval_d(abs_of(-1));    // invokes S32 version
  test_eval_f(abs_of(-12.f)); // invokes F32 version
  test_eval_f(abs_of(-12.0)); // involes F64 version
  test_eval_d(lerp(10, 20, 0.5f));
  test_eval_d(align_down_pow2(15, 4));
  test_eval_d(align_up_pow2(15, 4));
  test_eval_d(is_pow2(10));
  test_eval_d(is_pow2(16));
  test_eval_f(percent(2.f, 1.f, 10.f));
  test_eval_f(percent(2.0, 0.0, 100));
  test_eval_f(deg_to_rad(180.f));
  test_eval_f(deg_to_rad(180.0));
  test_eval_f(rad_to_deg(PI_32));
  test_eval_f(rad_to_deg(PI_64));
  test_eval_f(floor(12.3f));
  test_eval_f(floor(12.3));
  test_eval_f(floor(123.0f));
  test_eval_f(floor(123.0));
  test_eval_f(ceil(12.3f));
  test_eval_f(ceil(12.3));
  test_eval_f(ceil(123.0f));
  test_eval_f(ceil(123.0));
  test_eval_f(cstr_to_f64("123.123"));
  test_eval_f(cstr_to_f64("123456789.123"));
  test_eval_d(is_nan(F64_NAN()));
  test_eval_d(is_nan(F32_NAN()));
  
  
  
  // Test swap
  {
    int i = 5;
    int j = 10;
    test_log("Before swap: i = %d, j = %d\n", i, j);
    swap(&i, &j);
    test_log("After swap: i = %d, j = %d\n", i, j);
  }
  
  // Test ptr_to_int() and int_to_ptr()
  {
    int i = 5;
    test_log("ptr_to_int(%p) = %llX\n", &i, ptr_to_int(&i));
    test_log("int_to_ptr(%X) = %p\n", 0x1234, int_to_ptr(0x1234));
  } 
  
  // Test endian swap 16
  {
    union {
      U16 u;
      char c[2];
    } val;
    
    val.u = 12345;
    test_log("Before endian_swap_16: ");
    for(int i = 0; i < array_count(val.c); ++i) 
      test_log("%02X ", val.c[i]);
    test_log("\n");
    val.u = endian_swap_16(val.u);
    
    test_log("After endian_swap_16: ");
    for(int i = 0; i < array_count(val.c); ++i) 
      test_log("%02X ", val.c[i]);
    test_log("\n");
    
  }
  
  // Test endian swap 32
  {
    union {
      U32 u;
      char c[4];
    } val;
    
    val.u = 12345;
    test_log("Before endian_swap_32: ");
    for(int i = 0; i < array_count(val.c); ++i) 
      test_log("%02X ", val.c[i]);
    test_log("\n");
    val.u = endian_swap_32(val.u);
    
    test_log("After endian_swap_32: ");
    for(int i = 0; i < array_count(val.c); ++i) 
      test_log("%02X ", val.c[i]);
    test_log("\n");
    
  }
  
}

#endif //TEST_ESSENTIALS_H
