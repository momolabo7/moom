// Reference: https://github.com/reyoung/avx_mathfun/blob/be617bbcf66993c4f7e7d267ed9926cd8e7d4f02/avx_mathfun.h#L53
// Assumes __AVX2__ is defined
//
// TODO:
// - we need to figure out what happens if we want to use turns instead of radians

#include "momo.h"
#include <stdio.h>
#include <immintrin.h>
#if 1


#define TEST_CONST(t,n,v) \
  static const t _ps256_##n[8] __attribute__((aligned(32))) = {(v),(v),(v),(v),(v),(v),(v),(v)};

#define TEST_CONST_PI(t,n,v) \
  static const t _pi32_256_##n[8] __attribute__((aligned(32))) = {(v),(v),(v),(v),(v),(v),(v),(v)};

TEST_CONST(s32_t, sign_mask, (s32_t)0x80000000);
TEST_CONST(s32_t, inv_sign_mask, ~0x80000000);
TEST_CONST(f32_t, cephes_FOPI, 1.27323954473516f);
TEST_CONST(f32_t, minus_cephes_DP1, -0.78515625);
TEST_CONST(f32_t, minus_cephes_DP2, -2.4187564849853515625e-4);
TEST_CONST(f32_t, minus_cephes_DP3, -3.77489497744594108e-8);
TEST_CONST(f32_t, sincof_p0, -1.9515295891E-4);
TEST_CONST(f32_t, sincof_p1,  8.3321608736E-3);
TEST_CONST(f32_t, sincof_p2, -1.6666654611E-1);
TEST_CONST(f32_t, coscof_p0,  2.443315711809948E-005);
TEST_CONST(f32_t, coscof_p1, -1.388731625493765E-003);
TEST_CONST(f32_t, coscof_p2,  4.166664568298827E-002);
TEST_CONST(f32_t, 0p5, 0.5f);
//TEST_CONST(f32_t, 0,  0.f);
TEST_CONST(f32_t, 1,  1.f);

TEST_CONST_PI(s32_t, 0, 0);
TEST_CONST_PI(s32_t, 1, 1);
TEST_CONST_PI(s32_t, inv1, ~1);
TEST_CONST_PI(s32_t, 2, 2);
TEST_CONST_PI(s32_t, 4, 4);

  static __m256 
sin256_ps(__m256 x) 
{ // any x
  __m256 xmm1, xmm2 = _mm256_setzero_ps(), xmm3, sign_bit, y;
  __m256i imm0, imm2;


  sign_bit = x;
  /* take the absolute value */
  x = _mm256_and_ps(x, *(__m256*)_ps256_inv_sign_mask);
  /* extract the sign bit (upper one) */
  sign_bit = _mm256_and_ps(sign_bit, *(__m256*)_ps256_sign_mask);

  /* scale by 4/Pi */
  y = _mm256_mul_ps(x, *(__m256*)_ps256_cephes_FOPI);

  /*
     Here we start a series of integer operations, which are in the
     realm of AVX2.
     If we don't have AVX, let's perform them using SSE2 directives
     */

  /* store the integer part of y in mm0 */
  imm2 = _mm256_cvttps_epi32(y);
  /* j=(j+1) & (~1) (see the cephes sources) */
  // another two AVX2 instruction
  imm2 = _mm256_add_epi32(imm2, *(__m256i*)_pi32_256_1);
  imm2 = _mm256_and_si256(imm2, *(__m256i*)_pi32_256_inv1);
  y = _mm256_cvtepi32_ps(imm2);

  /* get the swap sign flag */
  imm0 = _mm256_and_si256(imm2, *(__m256i*)_pi32_256_4);
  imm0 = _mm256_slli_epi32(imm0, 29);
  /* get the polynom selection mask 
     there is one polynom for 0 <= x <= Pi/4
     and another one for Pi/4<x<=Pi/2

     Both branches will be computed.
     */
  imm2 = _mm256_and_si256(imm2, *(__m256i*)_pi32_256_2);
  imm2 = _mm256_cmpeq_epi32(imm2,*(__m256i*)_pi32_256_0);

  __m256 swap_sign_bit = _mm256_castsi256_ps(imm0);
  __m256 poly_mask = _mm256_castsi256_ps(imm2);
  sign_bit = _mm256_xor_ps(sign_bit, swap_sign_bit);

  /* The magic pass: "Extended precision modular arithmetic" 
     x = ((x - y * DP1) - y * DP2) - y * DP3; */
  xmm1 = *(__m256*)_ps256_minus_cephes_DP1;
  xmm2 = *(__m256*)_ps256_minus_cephes_DP2;
  xmm3 = *(__m256*)_ps256_minus_cephes_DP3;
  xmm1 = _mm256_mul_ps(y, xmm1);
  xmm2 = _mm256_mul_ps(y, xmm2);
  xmm3 = _mm256_mul_ps(y, xmm3);
  x = _mm256_add_ps(x, xmm1);
  x = _mm256_add_ps(x, xmm2);
  x = _mm256_add_ps(x, xmm3);

  /* Evaluate the first polynom  (0 <= x <= Pi/4) */
  y = *(__m256*)_ps256_coscof_p0;
  __m256 z = _mm256_mul_ps(x,x);

  y = _mm256_mul_ps(y, z);
  y = _mm256_add_ps(y, *(__m256*)_ps256_coscof_p1);
  y = _mm256_mul_ps(y, z);
  y = _mm256_add_ps(y, *(__m256*)_ps256_coscof_p2);
  y = _mm256_mul_ps(y, z);
  y = _mm256_mul_ps(y, z);
  __m256 tmp = _mm256_mul_ps(z, *(__m256*)_ps256_0p5);
  y = _mm256_sub_ps(y, tmp);
  y = _mm256_add_ps(y, *(__m256*)_ps256_1);

  /* Evaluate the second polynom  (Pi/4 <= x <= 0) */

  __m256 y2 = *(__m256*)_ps256_sincof_p0;
  y2 = _mm256_mul_ps(y2, z);
  y2 = _mm256_add_ps(y2, *(__m256*)_ps256_sincof_p1);
  y2 = _mm256_mul_ps(y2, z);
  y2 = _mm256_add_ps(y2, *(__m256*)_ps256_sincof_p2);
  y2 = _mm256_mul_ps(y2, z);
  y2 = _mm256_mul_ps(y2, x);
  y2 = _mm256_add_ps(y2, x);

  /* select the correct result from the two polynoms */  
  xmm3 = poly_mask;
  y2 = _mm256_and_ps(xmm3, y2); //, xmm3);
  y = _mm256_andnot_ps(xmm3, y);
  y = _mm256_add_ps(y,y2);
  /* update the sign */
  y = _mm256_xor_ps(y, sign_bit);

  return y;
}

static f32_t
f32_sin_test(f32_t rads)
{
  f32_t angles[8] = { 0.f, 0.5f, 1.f, 0, 0, 0, 0, PI_32 };
  __m256 input = _mm256_loadu_ps(angles);

  __m256 result = sin256_ps(input);

  f32_t result_array[8];
  _mm256_storeu_ps(result_array, result);

  return result_array[0];
}

#endif



int main()
{


#if 0
  u64_t s, e;
  f32_t v = 0.f;
  // profiling
  const u32_t times = 360;
  s = clock_time();
  for(f32_t i = 0; i < 360.f; i += 0.01)
    v += sinf(f32_deg_to_rad(i));
  e = clock_time();
  printf("sinf: %f\n", v);
  printf("%f\n", (f32_t)(e - s)/times);

  v = 0.f;
  s = clock_time();
  for(f32_t i = 0; i < 360.f; i += 0.01)
    v += f32_sins(f32_deg_to_rad(i));
  e = clock_time();
  printf("sin1: %f\n",v );
  printf("%f\n", (f32_t)(e - s)/times);

  v = 0.f;
  s = clock_time();
  for(f32_t i = 0; i < 360.f; i += 0.01)
    v += f32_sins2(f32_rad_to_turn(f32_deg_to_rad(i)));
  e = clock_time();
  printf("sin2: %f\n",v );
  printf("%f\n", (f32_t)(e - s)/times);
#else 
  for(f32_t i = 0; i < 360.f; i += 1.f)
  {
    printf("%f: %f\n",i, sinf(f32_deg_to_rad(i)));
    printf("%f: %f\n",i,  f32_sin(i/360.f));
  }
#endif 
}
