// So all the sqrt, sin, cos functions are gonna be here instead
// of litering #if everywhere

#if COMPILER_MSVC
#include <intrin.h>
static U32 
atomic_compare_assign(U32 volatile* value,
                      U32 new_value,
                      U32 expected_value)
{
  U32 ret = _InterlockedCompareExchange((long volatile*)value,
                                        new_value,
                                        expected_value);
  return ret;
}

static U64 
atomic_assign(U64 volatile* value,
              U64 new_value)
{
  U64 ret = _InterlockedExchange64((__int64 volatile*)value,
                                   new_value);
  return ret;
}
static U32 
atomic_add(U32 volatile* value, U32 to_add) {
  U32 result = _InterlockedExchangeAdd((long volatile*)value, to_add);
  return result;
}

static U64 
atomic_add(U64 volatile* value, U64 to_add) {
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
sin(F32 x) {
  return std::sinf(x);
}

static F32 
cos(F32 x) {
  return std::cosf(x);
}

static F32
tan(F32 x) {
  return std::tanf(x);
}
static F32 
sqrt(F32 x) {
  return std::sqrtf(x);
  
}
static F32 
asin(F32 x) {
  return std::asinf(x);
}
static F32 
acos(F32 x) {
  return std::acosf(x);
}

static F32
atan(F32 x){
  return std::atanf(x);
}
static F32 
pow(F32 b, F32 e){
  return std::powf(b,e);
}


static F64 
sin(F64 x) {
  return std::sin(x);
}

static F64 
cos(F64 x) {
  return std::cos(x);
}

static F64
tan(F64 x) {
  return std::tan(x);
}
static F64 
sqrt(F64 x) {
  return std::sqrt(x);
  
}
static F64 
asin(F64 x) {
  return std::asin(x);
}
static F64 
acos(F64 x) {
  return std::acos(x);
}

static F64
atan(F64 x){
  return std::atan(x);
}
static F64 
pow(F64 b, F64 e){
  return std::pow(b,e);
}


// TODO(Momo): IEEE version of these?
static F32 floor(F32 value) {
  return std::floorf(value);
  
}
static F64 floor(F64 value){
  return std::floor(value);
}

static F32 ceil(F32 value) {
  return std::ceilf(value);
}

static F64 ceil(F64 value) {
  return std::ceil(value);
}

static F32 round(F32 value) {
  return std::roundf(value);
}

static F64 round(F64 value) {
  return std::round(value);
}

