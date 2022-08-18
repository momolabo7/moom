/* date = March 7th 2022 1:08 pm */

#ifndef MOMO_INTRINSICS_H
#define MOMO_INTRINSICS_H

// Returns the old value before the exchange
static U32 atomic_compare_assign_u32(U32 volatile* value, U32 new_value, U32 expected_value);
static U64 atomic_assign_u64(U64 volatile* value, U64 new_value);
static U32 atomic_add_u32(U32 volatile* value, U32 to_add);
static U64 atomic_add_u64(U64 volatile* value, U64 to_add);

static F32 sin_f32(F32 x);
static F32 cos_f32(F32 x);
static F32 tan_f32(F32 x);
static F32 sqrt_f32(F32 x);
static F32 asin_f32(F32 x);
static F32 acos_f32(F32 x);
static F32 atan_f32(F32 x);
static F32 pow_F32(F32 v, F32 e);

static F64 sin_f64(F64 x);
static F64 cos_f64(F64 x);
static F64 tan_f64(F64 x);
static F64 sqrt_f64(F64 x);
static F64 asin_f64(F64 x);
static F64 acos_f64(F64 x);
static F64 atan_f64(F64 x);
static F64 pow_f64(F64 , F64 e);

static F32 ceil_f32(F32 value);
static F32 floor_f32(F32 value);
static F32 round_f32(F32 value);

static F64 ceil_f64(F64 value);
static F64 floor_f64(F64 value);
static F64 round_f64(F64 value);

////////////////////////////////////////////////////////////////////////
// IMPLEMENTATION
#if COMPILER_MSVC
#include <intrin.h>
static U32 
atomic_compare_assign_u32(U32 volatile* value,
                          U32 new_value,
                          U32 expected_value)
{
  U32 ret = _InterlockedCompareExchange((long volatile*)value,
                                        new_value,
                                        expected_value);
  return ret;
}

static U64 
atomic_assign_u64(U64 volatile* value,
                  U64 new_value)
{
  U64 ret = _InterlockedExchange64((__int64 volatile*)value,
                                   new_value);
  return ret;
}
static U32 
atomic_add_u32(U32 volatile* value, U32 to_add) {
  U32 result = _InterlockedExchangeAdd((long volatile*)value, to_add);
  return result;
}

static U64 
atomic_add_u64(U64 volatile* value, U64 to_add) {
  U64 result = _InterlockedExchangeAdd64((__int64 volatile*)value, to_add);
  return result;
}

#else
#error "Not defined"
#endif

ns_begin(std)
#include <math.h>
ns_end(std)

static F32 
sin_f32(F32 x) {
  return std::sinf(x);
}

static F32 
cos_f32(F32 x) {
  return std::cosf(x);
}

static F32
tan_f32(F32 x) {
  return std::tanf(x);
}
static F32 
sqrt_f32(F32 x) {
  return std::sqrtf(x);
  
}
static F32 
asin_f32(F32 x) {
  return std::asinf(x);
}
static F32 
acos_f32(F32 x) {
  return std::acosf(x);
}

static F32
atan_f32(F32 x){
  return std::atanf(x);
}
static F32 
pow_f32(F32 b, F32 e){
  return std::powf(b,e);
}


static F64 
sin_f64(F64 x) {
  return std::sin(x);
}

static F64 
cos_f64(F64 x) {
  return std::cos(x);
}

static F64
tan_f64(F64 x) {
  return std::tan(x);
}
static F64 
sqrt_f64(F64 x) {
  return std::sqrt(x);
  
}
static F64 
asin_f64(F64 x) {
  return std::asin(x);
}
static F64 
acos_f64(F64 x) {
  return std::acos(x);
}

static F64
atan_f64(F64 x){
  return std::atan(x);
}
static F64 
pow_f64(F64 b, F64 e){
  return std::pow(b,e);
}


// TODO(Momo): IEEE version of these?
static F32 floor_f32(F32 value) {
  return std::floorf(value);
  
}
static F64 floor_f64(F64 value){
  return std::floor(value);
}

static F32 ceil_f32(F32 value) {
  return std::ceilf(value);
}

static F64 ceil_f64(F64 value) {
  return std::ceil(value);
}

static F32 round_f32(F32 value) {
  return std::roundf(value);
}

static F64 round_f64(F64 value) {
  return std::round(value);
}


#endif //MOMO_INTRINSICS_H
