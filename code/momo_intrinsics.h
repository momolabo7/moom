/* date = March 7th 2022 1:08 pm */

#ifndef MOMO_INTRINSICS_H
#define MOMO_INTRINSICS_H

// Returns the old value before the exchange
static u32_t u32_atomic_compare_assign(u32_t volatile* value, u32_t new_value, u32_t expected_value);
static u64_t u64_atomic_assign(u64_t volatile* value, u64_t new_value);
static u32_t u32_atomic_add(u32_t volatile* value, u32_t to_add);
static u64_t u64_atomic_add(u64_t volatile* value, u64_t to_add);

static f32_t f32_sin(f32_t x);
static f32_t f32_cos(f32_t x);
static f32_t f32_tan(f32_t x);
static f32_t f32_sqrt(f32_t x);
static f32_t f32_asin(f32_t x);
static f32_t f32_acos(f32_t x);
static f32_t f32_atan(f32_t x);
static f32_t f32_pow(f32_t v, f32_t e);

static f64_t f64_sin(f64_t x);
static f64_t f64_cos(f64_t x);
static f64_t f64_tan(f64_t x);
static f64_t f64_sqrt(f64_t x);
static f64_t f64_asin(f64_t x);
static f64_t f64_acos(f64_t x);
static f64_t f64_atan(f64_t x);
static f64_t f64_pow(f64_t , f64_t e);

static f32_t f32_ceil(f32_t value);
static f32_t f32_floor(f32_t value);
static f32_t f32_round(f32_t value);

static f64_t f64_ceil(f64_t value);
static f64_t f64_floor(f64_t value);
static f64_t f64_round(f64_t value);


//
// IMPLEMENTATION
// 
#if COMPILER_MSVC
#include <intrin.h>
static u32_t 
u32_atomic_compare_assign(u32_t volatile* value,
                          u32_t new_value,
                          u32_t expected_value)
{
  u32_t ret = _InterlockedCompareExchange((long volatile*)value,
                                        new_value,
                                        expected_value);
  return ret;
}

static u64_t 
u64_atomic_assign(u64_t volatile* value,
                  u64_t new_value)
{
  u64_t ret = _InterlockedExchange64((__int64 volatile*)value,
                                   new_value);
  return ret;
}
static u32_t 
u32_atomic_add(u32_t volatile* value, u32_t to_add) {
  u32_t result = _InterlockedExchangeAdd((long volatile*)value, to_add);
  return result;
}

static u64_t 
u64_atomic_add(u64_t volatile* value, u64_t to_add) {
  u64_t result = _InterlockedExchangeAdd64((__int64 volatile*)value, to_add);
  return result;
}

#else
#error "Not defined"
#endif

#include <math.h>
static f32_t 
f32_sin(f32_t x) {
  return sinf(x);
}

static f32_t 
f32_cos(f32_t x) {
  return cosf(x);
}

static f32_t
f32_tan(f32_t x) {
  return tanf(x);
}
static f32_t 
f32_sqrt(f32_t x) {
  return sqrtf(x);
  
}
static f32_t 
f32_asin(f32_t x) {
  x = clamp_of(x, 1.f, -1.f);
  return asinf(x);
}
static f32_t 
f32_acos(f32_t x) {
  x = clamp_of(x, 1.f, -1.f);
  return acosf(x);
}

static f32_t
f32_atan(f32_t x){
  return atanf(x);
}
static f32_t 
f32_pow(f32_t b, f32_t e){
  return powf(b,e);
}


static f64_t 
f64_sin(f64_t x) {
  return sin(x);
}

static f64_t 
f64_cos(f64_t x) {
  return cos(x);
}

static f64_t
f64_tan(f64_t x) {
  return tan(x);
}
static f64_t 
f64_sqrt(f64_t x) {
  return sqrt(x);
  
}
static f64_t 
f64_asin(f64_t x) {
  x = clamp_of(x, 1.f, -1.f);
  return asin(x);
}
static f64_t 
f64_acos(f64_t x) {
  x = clamp_of(x, 1.f, -1.f);
  return acos(x);
}

static f64_t
f64_atan(f64_t x){
  return atan(x);
}
static f64_t 
f64_pow(f64_t b, f64_t e){
  return pow(b,e);
}


// TODO(Momo): IEEE version of these?
static f32_t f32_floor(f32_t value) {
  return floorf(value);
  
}
static f64_t f64_floor(f64_t value){
  return floor(value);
}

static f32_t f32_ceil(f32_t value) {
  return ceilf(value);
}

static f64_t f64_ceil(f64_t value) {
  return ceil(value);
}

static f32_t f32_round(f32_t value) {
  return roundf(value);
}

static f64_t f64_round(f64_t value) {
  return round(value);
}


#endif //MOMO_INTRINSICS_H
