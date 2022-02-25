//~ NOTE(Momo): Memory functions
// NOTE(Momo): These functions can use memset/memcopy/memcmp
// but they might not be avaliable. 
// TODO(Momo): Figure out a nice flag to indicate if libraries
// are avaliable for use or not. Maybe IS_CRT?

static B32
is_ok(Memory mem) {
  return mem.data && mem.size;
}

#if 1
#include <string.h>
static void 
copy_memory(void* dest, const void* src, UMI size) {
  memmove(dest, src, size);
}

static void 
zero_memory(void* dest, UMI size) {
  memset(dest, 0, size);
}
static B32
match_memory(const void* lhs, const void* rhs, UMI size) {
  return memcmp(lhs, rhs, size) == 0; 
}

#else

static void
copy_memory(void* dest, const void* src, UMI size) {
  U8 *p = (U8*)dest;
  const U8 *q = (const U8*)src;
  while(size--) {
    *p++ = *q++;
  }
}

static void 
zero_memory(void* dest, UMI size) {
  U8 *p = (U8*)dest;
  while(size--){
    *p++ = 0;
  }
}

static B32
match_memory(const void* lhs, const void* rhs, UMI size) {
  const U8 *p = (const U8*)lhs;
  const U8 *q = (const U8*)rhs;
  while(size--) {
    if (*p != *q) {
      return false;
    }
  }
  return true;
  
}
#endif

static void 
swap_memory(void* lhs, void* rhs, UMI size) {
  U8* l = (U8*)lhs;
  U8* r = (U8*)rhs;
  
  while(size--) {
    U8 tmp = (*l);
    *l++ = *r;
    *r++ = tmp;
  }
}
//~ Helper functions
static constexpr UMI 
ptr_to_int(void* p) { 
  return (UMI)((char*)p - (char*)0); 
}

static constexpr U8* 
int_to_ptr(UMI u) { 
  return (U8*)((char*)0 + u);
}

template<typename T> static constexpr T 
min_of(T l, T r) { 
  return l < r ? l : r; 
}

template<typename T> static constexpr T 
max_of(T l, T r) { 
  return l > r ? l : r; 
}

template<typename T> static constexpr T 
clamp(T x, T b, T t) { 
  return max_of(min_of(x,t),b); 
}

template<typename T> static constexpr T
abs_of(T x) { 
  return x < 0 ? -x : x; 
}

static constexpr F32 
abs_of(F32 x) {
  union { F32 f; U32 u; } val = {};
  val.f = x;
  val.u &= 0x7fffffff;  
  return val.f;
}

static constexpr F64
abs_of(F64 x) {
  union { F64 f; U64 u; } val = {};
  val.f = x;
  val.u &= 0x7fffffffffffffff;
  
  return val.f;
}


static constexpr S8   
abs_of(S8 x) {
  S8 y = x >> 7;
  return (x ^ y)-y;
}
static constexpr S16  
abs_of(S16 x) {
  S16 y = x >> 15;
  return (x ^ y)-y;
}
static constexpr S32  
abs_of(S32 x) {
  S32 y = x >> 31;
  return (x ^ y)-y;
}
static constexpr S64  
abs_of(S64 x) {
  S64 y = x >> 63;
  return (x ^ y)-y;
}

static constexpr F32 
sign_of(F32 x) {
  return x >= 0 ? 1.f : -1.f;
}

static constexpr F64
sign_of(F64 x) {
  return x >= 0 ? 1.0 : -1.0;
}


static constexpr S8   
sign_of(S8 x) {
  return x >= 0 ? 1 : -1;
}
static constexpr S16  
sign_of(S16 x) {
  return x >= 0 ? 1 : -1;
}
static constexpr S32  
sign_of(S32 x) {
  return x >= 0 ? 1 : -1;
}
static constexpr S64  
sign_of(S64 x) {
  return x >= 0 ? 1 : -1;
}

template<typename T> static constexpr T
lerp(T s, T e, F32 f) { 
  return (T)(s + (e-s) * f); 
}

template<typename T> static constexpr T 
lerp(T s, T e, F64 f) { 
  return (T)(s + (e-s) * f); 
}

static constexpr F32 
ratio(F32 v, F32 min, F32 max) { 
  return (v - min)/(max - min); 
}

static constexpr F64 
ratio(F64 v, F64 min, F64 max) { 
  return (v - min)/(max - min); 
}

template<typename T, typename U> static constexpr T
align_down_pow2(T value, U align) { 
  return value & ~(align-1); 
}

template<typename T, typename U> static T
align_up_pow2(T value, U align) { 
  return (value + (align-1)) & ~(align-1); 
}

template<typename T> static constexpr B32 
is_pow2(T value) { 
  return (value & (value - 1)) == 0; 
}
template<typename T> static constexpr void 
swap(T* lhs, T* rhs) { 
  T tmp = *lhs; 
  *lhs = *rhs; 
  *rhs = tmp; 
} 


//~C-strings, aka null-terminated strings
static UMI
cstr_len(const char* str) {
  UMI count = 0;
  for(; (*str) != 0 ; ++count, ++str);
  return count;
}

static void
cstr_copy(char * dest, const char* src) {
  for(; (*src) != 0 ; ++src, ++dest) {
    (*dest) = (*src);
  }
  (*dest) = 0;
}

static B32
cstr_compare(const char* lhs, const char* rhs) {
  for(; (*rhs) != 0 ; ++rhs, ++lhs) {
    if ((*lhs) != (*rhs)) {
      return false;
    }
  }
  return true;
}

static B32
cstr_compare_n(const char* lhs, const char* rhs, UMI n) {
  while(n--) {
    if ((*lhs++) != (*rhs++)) {
      return false;
    }
  }
  return true;
}
static void
cstr_concat(char* dest, const char* Src) {
  // Go to the end of dest
  for (; (*dest) != 0; ++dest);
  for (; (*Src) != 0; ++Src, ++dest) {
    (*dest) = (*Src);
  }
  (*dest) = 0;
}


static void 
cstr_clear(char* dest) {
  (*dest) = 0;
}

static void
cstr_reverse(char* dest) {
  char* back_ptr = dest;
  for (; *(back_ptr+1) != 0; ++back_ptr);
  for (;dest < back_ptr; ++dest, --back_ptr) {
    swap(dest, back_ptr);
  }
}



static void 
cstr_itoa(char* dest, S32 num) {
  // Naive method. 
  // Extract each number starting from the back and fill the buffer. 
  // Then reverse it.
  
  // Special case for 0
  if (num == 0) {
    dest[0] = '0';
    dest[1] = 0;
    return;
  }
  
  B32 negative = num < 0;
  num = abs_of(num);
  
  char* it = dest;
  for(; num != 0; num /= 10) {
    S32 digit_to_convert = num % 10;
    *(it++) = (char)(digit_to_convert + '0');
  }
  
  if (negative) {
    *(it++) = '-';
  }
  (*it) = 0;
  
  cstr_reverse(dest);
}


//~ NOTE(Momo): Constants
static constexpr F32 
F32_INFINITY() {
  // NOTE(Momo): Use 'type pruning'
  // Infinity is when bits 1-8 are on
  union { F32 f; U32 u; } ret = {};
  ret.u = 0x7f800000;
  
  return ret.f;
  
}

static constexpr F32 
F32_NEG_INFINITY() {
  // NOTE(Momo): Use 'type pruning'
  // Infinity is when bits 1-8 are on
  // Negative is when bit 0 is on
  union { F32 f; U32 u; } ret = {};
  ret.u = 0xff800000;
  
  return ret.f;	
}




static constexpr F64 
F64_INFINITY() {
  // NOTE(Momo): Use 'type pruning'
  // Infinity is when bits 1-11 are on
  union { F64 f; U64 u; } ret = {};
  ret.u = 0x7FF0000000000000;
  
  return ret.f;
  
}

static constexpr F64 
F64_NEG_INFINITY() {
  // NOTE(Momo): Use 'type pruning'
  // Infinity is when bits 1-11 are on
  // Negative is when bit 0 is on
  union { F64 f; U64 u; } ret = {};
  ret.u = 0xFFF0000000000000;
  
  return ret.f;	
}


//~ NOTE(Momo): IEEE floating point functions 
static constexpr B32 
match(F32 lhs, F32 rhs) {
  return abs_of(lhs - rhs) <= F32_EPSILON;
}

static constexpr B32 
match(F64 lhs, F64 rhs) {
  return abs_of(lhs - rhs) <= F64_EPSILON;
}


static constexpr F32
bpm_to_spb(F32 bpm) {
  assert(bpm >= 0.f);
  return 60.f/bpm;
}

static constexpr F64
bpm_to_spb(F64 bpm) {
  assert(bpm >= 0.f);
  return 60.0/bpm;
}


static constexpr F32 
deg_to_rad(F32 degrees) {
  return degrees * PI_32 / 180.f;
}
static constexpr F32 
rad_to_deg(F32 radians) {
  return radians * 180.f / PI_32;	
}

static constexpr F64
deg_to_rad(F64 degrees) {
  return degrees * PI_32 / 180.0;
  
}
static constexpr F64 
rad_to_deg(F64 radians) {
  return radians * 180.0 / PI_64;
  
}




static constexpr U16
endian_swap_16(U16 value) {
  return (value << 8) | (value >> 8);
}

static S16
endian_swap_16(S16 value) {
  return (value << 8) | (value >> 8);
}

static constexpr U32
endian_swap_32(U32 value) {
  return  ((value << 24) |
           ((value & 0xFF00) << 8) |
           ((value >> 8) & 0xFF00) |
           (value >> 24));
  
}

///////////////////////////////////
//~ NOTE(Momo): Non-trivial math functions
// We have to use math.h here as default
// TODO(Momo): write our own versions
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

//~Easing functions

static F32 
ease_in_sine(F32 t)  {
  return sin(PI_32 * 0.5f * t);
}


static F32 
ease_out_sine(F32 t) {
  return 1.0f + sin(PI_32 * 0.5f * (--t));
}

static F32 
ease_inout_sine(F32 t)  {
  return 0.5f * (1.f + sin(PI_32 * (t - 0.5f)));
}

static F32 
ease_in_quad(F32 t)  {
  return t * t;
}

static F32 
ease_out_quad(F32 t)  {
  return t * (2.f -t);
}

static F32 
ease_inout_quad(F32 t)  {
  return t < 0.5f ? 2.f * t * t : t * (4.f -2.f * t) - 1.f;
}

static F32 
ease_in_cubic(F32 t)  {
  return t * t * t;
}

static F32 
ease_out_cubic(F32 t)  {
  return 1.f + (t-1) * (t-1) * (t-1);
}

static F32 
ease_inout_cubic(F32 t)  {
  return t < 0.5f ? 4.f * t * t * t : 1.f + (t-1) * (2.f * (t-2)) * (2.f * (t-2));
}

static F32 
ease_in_quart(F32 t)  {
  t *= t;
  return t * t;
}

static F32 
ease_out_quart(F32 t) {
  --t;
  t = t * t;
  return 1.f - t * t;
}

static F32 
ease_inout_quart(F32 t)  {
  if (t < 0.5f) {
    t *= t;
    return 8.f * t * t;
  }
  else {
    --t;
    t = t * t;
    return 1.f -8.f * t * t;
  }
}

static F32
ease_in_quint(F32 t)  {
  F32 t2 = t * t;
  return t * t2 * t2;
}

static F32
ease_out_quint(F32 t)  {
  --t;
  F32 t2 = t * t;
  return 1.f +t * t2 * t2;
}

static F32
ease_inout_quint(F32 t)  {
  F32 t2;
  if (t < 0.5f) {
    t2 = t * t;
    return 16.f * t * t2 * t2;
  }
  else {
    --t;
    t2 = t * t;
    return 1.f +16.f * t * t2 * t2;
  }
}



static F32 
ease_in_circ(F32 t)  {
  return 1.f -sqrt(1.f -t);
}

static F32 
ease_out_circ(F32 t)  {
  return sqrt(t);
}

static F32 
ease_inout_circ(F32 t)  {
  if (t < 0.5f) {
    return (1.f -sqrt(1.f -2.f * t)) * 0.5f;
  }
  else {
    return (1.f +sqrt(2.f * t - 1.f)) * 0.5f;
  }
}

static F32 
ease_in_back(F32 t)  {
  return t * t * (2.7f * t - 1.7f);
}

static F32 
ease_out_back(F32 t)  {
  --t;
  return 1.f + t * t * (2.7f * t + 1.7f);
}

static F32 
ease_inout_back(F32 t)  {
  if (t < 0.5f) {
    return t * t * (7.f * t - 2.5f) * 2.f;
  }
  else {
    --t;
    return 1.f + t * t * 2.f * (7.f * t + 2.5f);
  }
}

static F32 
ease_in_elastic(F32 t)  {
  F32 t2 = t * t;
  return t2 * t2 * sin(t * PI_32 * 4.5f);
}

static F32 
ease_out_elastic(F32 t)  {
  F32 t2 = (t - 1.f) * (t - 1.f);
  return 1.f -t2 * t2 * cos(t * PI_32 * 4.5f);
}

static F32 
ease_inout_elastic(F32 t)  {
  F32 t2;
  if (t < 0.45f) {
    t2 = t * t;
    return 8.f * t2 * t2 * sin(t * PI_32 * 9.f);
  }
  else if (t < 0.55f) {
    return 0.5f +0.75f * sin(t * PI_32 * 4.f);
  }
  else {
    t2 = (t - 1.f) * (t - 1.f);
    return 1.f -8.f * t2 * t2 * sin(t * PI_32 * 9.f);
  }
}

static F32 
ease_in_bounce(F32 t)  {
  return pow(2.f, 6.f * (t - 1.f)) * abs_of(sin(t * PI_32 * 3.5f));
}


static F32 
ease_out_bounce(F32 t) {
  return 1.f -pow(2.f, -6.f * t) * abs_of(cos(t * PI_32 * 3.5f));
}

static F32 
ease_inout_bounce(F32 t) {
  if (t < 0.5f) {
    return 8.f * pow(2.f, 8.f * (t - 1.f)) * abs_of(sin(t * PI_32 * 7.f));
  }
  else {
    return 1.f -8.f * pow(2.f, -8.f * t) * abs_of(sin(t * PI_32 * 7.f));
  }
}

static F32
ease_in_expo(F32 t)  {
  return (pow(2.f, 8.f * t) - 1.f) / 255.f;
}


static F32 
ease_out_expo(F32 t)  {
  return t == 1.f ? 1.f : 1.f -pow(2.f, -10.f * t);
}

static F32 
ease_inout_expo(F32 t)  {
  if (t < 0.5f) {
    return (pow(2.f, 16.f * t) - 1.f) / 510.f;
  }
  else {
    return 1.f -0.5f * pow(2.f, -16.f * (t - 0.5f));
  }
}



static F64 
ease_in_sine(F64 t)  {
  return sin(PI_64 * 0.5 * t);
}


static F64 
ease_out_sine(F64 t) {
  return 1.0f + sin(PI_64 * 0.5 * (--t));
}

static F64 
ease_inout_sine(F64 t)  {
  return 0.5 * (1.0 + sin(PI_64 * (t - 0.5)));
}

static F64 
ease_in_quad(F64 t)  {
  return t * t;
}

static F64 
ease_out_quad(F64 t)  {
  return t * (2.0 -t);
}

static F64 
ease_inout_quad(F64 t)  {
  return t < 0.5 ? 2.0 * t * t : t * (4.0 -2.0 * t) - 1.0;
}

static F64 
ease_in_cubic(F64 t)  {
  return t * t * t;
}

static F64 
ease_out_cubic(F64 t)  {
  return 1.0 + (t-1) * (t-1) * (t-1);
}

static F64 
ease_inout_cubic(F64 t)  {
  return t < 0.5 ? 4.0 * t * t * t : 1.0 + (t-1) * (2.0 * (t-2)) * (2.0 * (t-2));
}

static F64 
ease_in_quart(F64 t)  {
  t *= t;
  return t * t;
}

static F64 
ease_out_quart(F64 t) {
  --t;
  t = t * t;
  return 1.0 - t * t;
}

static F64 
ease_inout_quart(F64 t)  {
  if (t < 0.5) {
    t *= t;
    return 8.0 * t * t;
  }
  else {
    --t;
    t = t * t;
    return 1.0 -8.0 * t * t;
  }
}

static F64
ease_in_quint(F64 t)  {
  F64 t2 = t * t;
  return t * t2 * t2;
}

static F64
ease_out_quint(F64 t)  {
  --t;
  F64 t2 = t * t;
  return 1.0 +t * t2 * t2;
}

static F64
ease_inout_quint(F64 t)  {
  F64 t2;
  if (t < 0.5) {
    t2 = t * t;
    return 16.0 * t * t2 * t2;
  }
  else {
    --t;
    t2 = t * t;
    return 1.0 +16.0 * t * t2 * t2;
  }
}



static F64 
ease_in_circ(F64 t)  {
  return 1.0 -sqrt(1.0 -t);
}

static F64 
ease_out_circ(F64 t)  {
  return sqrt(t);
}

static F64 
ease_inout_circ(F64 t)  {
  if (t < 0.5) {
    return (1.0 -sqrt(1.0 -2.0 * t)) * 0.5;
  }
  else {
    return (1.0 +sqrt(2.0 * t - 1.0)) * 0.5;
  }
}

static F64 
ease_in_back(F64 t)  {
  return t * t * (2.7 * t - 1.7);
}

static F64 
ease_out_back(F64 t)  {
  --t;
  return 1.0 + t * t * (2.7 * t + 1.7);
}

static F64 
ease_inout_back(F64 t)  {
  if (t < 0.5) {
    return t * t * (7.0 * t - 2.5) * 2.0;
  }
  else {
    --t;
    return 1.0 + t * t * 2.0 * (7.0 * t + 2.5);
  }
}

static F64 
ease_in_elastic(F64 t)  {
  F64 t2 = t * t;
  return t2 * t2 * sin(t * PI_64 * 4.5);
}

static F64 
ease_out_elastic(F64 t)  {
  F64 t2 = (t - 1.0) * (t - 1.0);
  return 1.0 -t2 * t2 * cos(t * PI_64 * 4.5);
}

static F64 
ease_inout_elastic(F64 t)  {
  F64 t2;
  if (t < 0.45) {
    t2 = t * t;
    return 8.0 * t2 * t2 * sin(t * PI_64 * 9.0);
  }
  else if (t < 0.55) {
    return 0.5 +0.75 * sin(t * PI_64 * 4.0);
  }
  else {
    t2 = (t - 1.0) * (t - 1.0);
    return 1.0 -8.0 * t2 * t2 * sin(t * PI_64 * 9.0);
  }
}



// NOTE(Momo): These require power function. 
static F64 
ease_in_bounce(F64 t)  {
  return pow(2.0, 6.0 * (t - 1.0)) * abs_of(sin(t * PI_64 * 3.5));
}


static F64 
ease_out_bounce(F64 t) {
  return 1.0 -pow(2.0, -6.0 * t) * abs_of(cos(t * PI_64 * 3.5));
}

static F64 
ease_inout_bounce(F64 t) {
  if (t < 0.5) {
    return 8.0 * pow(2.0, 8.0 * (t - 1.0)) * abs_of(sin(t * PI_64 * 7.0));
  }
  else {
    return 1.0 -8.0 * pow(2.0, -8.0 * t) * abs_of(sin(t * PI_64 * 7.0));
  }
}

static F64
ease_in_expo(F64 t)  {
  return (pow(2.0, 8.0 * t) - 1.0) / 255.0;
}


static F64 
ease_out_expo(F64 t)  {
  return t == 1.0 ? 1.0 : 1.0 -pow(2.0, -10.0 * t);
}

static F64 
ease_inout_expo(F64 t)  {
  if (t < 0.5) {
    return (pow(2.0, 16.0 * t) - 1.0) / 510.0;
  }
  else {
    return 1.0 -0.5 * pow(2.0, -16.0 * (t - 0.5));
  }
}
