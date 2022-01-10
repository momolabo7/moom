//~ NOTE(Momo): Memory functions
// NOTE(Momo): These functions can use memset/memcopy/memcmp
// but they might not be avaliable. 
// TODO(Momo): Figure out a nice flag to indicate if libraries
// are avaliable for use or not. Maybe IS_CRT?

static B32
IsValid(Memory mem) {
  return mem.data && mem.size;
}

#if 1
#include <string.h>
static void 
Bin_Copy(void* dest, const void* src, UMI size) {
  memmove(dest, src, size);
}

static void 
Bin_Zero(void* dest, UMI size) {
  memset(dest, 0, size);
}
static B32
Bin_Match(const void* lhs, const void* rhs, UMI size) {
  return memcmp(lhs, rhs, size) == 0; 
}

#else

static void
Bin_Copy(void* dest, const void* src, UMI size) {
  U8 *p = (U8*)dest;
  const U8 *q = (const U8*)src;
  while(size--) {
    *p++ = *q++;
  }
}

static void 
Bin_Zero(void* dest, UMI size) {
  U8 *p = (U8*)dest;
  while(size--){
    *p++ = 0;
  }
}

static B32
Bin_Match(const void* lhs, const void* rhs, UMI size) {
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
Bin_Swap(void* lhs, void* rhs, UMI size) {
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
PtrToInt(void* p) { 
  return (UMI)((char*)p - (char*)0); 
}

static constexpr U8* 
IntToPtr(UMI u) { 
  return (U8*)((char*)0 + u);
}

template<class T> static constexpr T 
Min(T l, T r) { 
  return l < r ? l : r; 
}

template<class T> static constexpr T 
Max(T l, T r) { 
  return l > r ? l : r; 
}

template<class T> static constexpr T 
Clamp(T x, T b, T t) { 
  return Max(Min(x,t),b); 
}

template<class T> static constexpr T
Abs(T x) { 
  return x < 0 ? -x : x; 
}

static constexpr F32 
Abs(F32 x) {
  union { F32 f; U32 u; } val = {0};
  val.f = x;
  val.u &= 0x7fffffff;  
  return val.f;
}

static constexpr F64
Abs(F64 x) {
  union { F64 f; U64 u; } val = {0};
  val.f = x;
  val.u &= 0x7fffffffffffffff;
  
  return val.f;
}


static constexpr S8   
Abs(S8 x) {
  S8 y = x >> 7;
  return (x ^ y)-y;
}
static constexpr S16  
Abs(S16 x) {
  S16 y = x >> 15;
  return (x ^ y)-y;
}
static constexpr S32  
Abs(S32 x) {
  S32 y = x >> 31;
  return (x ^ y)-y;
}
static constexpr S64  
Abs(S64 x) {
  S64 y = x >> 63;
  return (x ^ y)-y;
}


template<class T> static constexpr T
Lerp(T s, T e, F32 f) { 
  return (T)(s + (e-s) * f); 
}

template<class T> static constexpr T 
Lerp(T s, T e, F64 f) { 
  return (T)(s + (e-s) * f); 
}

static constexpr F32 
Ratio(F32 v, F32 min, F32 max) { 
  return (v - min)/(max - min); 
}

static constexpr F64 
Ratio(F64 v, F64 min, F64 max) { 
  return (v - min)/(max - min); 
}

template<class T, class U> static constexpr T
AlignDownPow2(T value, U align) { 
  return value & ~(align-1); 
}

template<class T, class U> static T
AlignUpPow2(T value, U align) { 
  return (value + (align-1)) & ~(align-1); 
}

template<class T> static constexpr B32 
IsPow2(T value) { 
  return (value & (value - 1)) == 0; 
}
template<class T> static constexpr void 
Swap(T* lhs, T* rhs) { 
  T tmp = *lhs; 
  *lhs = *rhs; 
  *rhs = tmp; 
} 


//~C-strings, aka null-terminated strings
static UMI
Sistr_Length(const char* str) {
  UMI count = 0;
  for(; (*str) != 0 ; ++count, ++str);
  return count;
}

static void
Sistr_Copy(char * dest, const char* src) {
  for(; (*src) != 0 ; ++src, ++dest) {
    (*dest) = (*src);
  }
  (*dest) = 0;
}

static B32
Sistr_Compare(const char* lhs, const char* rhs) {
  for(; (*rhs) != 0 ; ++rhs, ++lhs) {
    if ((*lhs) != (*rhs)) {
      return false;
    }
  }
  return true;
}

static B32
Sistr_CompareN(const char* lhs, const char* rhs, UMI n) {
  while(n--) {
    if ((*lhs++) != (*rhs++)) {
      return false;
    }
  }
  return true;
}
static void
Sistr_Concat(char* dest, const char* Src) {
  // Go to the end of dest
  for (; (*dest) != 0; ++dest);
  for (; (*Src) != 0; ++Src, ++dest) {
    (*dest) = (*Src);
  }
  (*dest) = 0;
}


static void 
Sistr_Clear(char* dest) {
  (*dest) = 0;
}

static void
Sistr_Reverse(char* dest) {
  char* back_ptr = dest;
  for (; *(back_ptr+1) != 0; ++back_ptr);
  for (;dest < back_ptr; ++dest, --back_ptr) {
    Swap(dest, back_ptr);
  }
}



static void 
Sistr_Itoa(char* dest, S32 num) {
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
  num = Abs(num);
  
  char* it = dest;
  for(; num != 0; num /= 10) {
    S32 digit_to_convert = num % 10;
    *(it++) = (char)(digit_to_convert + '0');
  }
  
  if (negative) {
    *(it++) = '-';
  }
  (*it) = 0;
  
  Sistr_Reverse(dest);
}


//~ NOTE(Momo): Constants
static constexpr F32 
F32_Inf() {
  // NOTE(Momo): Use 'type pruning'
  // Infinity is when bits 1-8 are on
  union { F32 f; U32 u; } ret = {0};
  ret.u = 0x7f800000;
  
  return ret.f;
  
}

static constexpr F32 
F32_NegInf() {
  // NOTE(Momo): Use 'type pruning'
  // Infinity is when bits 1-8 are on
  // Negative is when bit 0 is on
  union { F32 f; U32 u; } ret = {0};
  ret.u = 0xff800000;
  
  return ret.f;	
}




static constexpr F64 
F64_Inf() {
  // NOTE(Momo): Use 'type pruning'
  // Infinity is when bits 1-11 are on
  union { F64 f; U64 u; } ret = {0};
  ret.u = 0x7FF0000000000000;
  
  return ret.f;
  
}

static constexpr F64 
F64_NegInf() {
  // NOTE(Momo): Use 'type pruning'
  // Infinity is when bits 1-11 are on
  // Negative is when bit 0 is on
  union { F64 f; U64 u; } ret = {0};
  ret.u = 0xFFF0000000000000;
  
  return ret.f;	
}


//~ NOTE(Momo): IEEE floating point functions 
static constexpr B32 
Match(F32 lhs, F32 rhs) {
  return Abs(lhs - rhs) <= F32_epsilon;
}

static constexpr B32 
Match(F64 lhs, F64 rhs) {
  return Abs(lhs - rhs) <= F64_epsilon;
}


static constexpr F32
BPMToSPB(F32 bpm) {
  Assert(bpm >= 0.f);
  return 60.f/bpm;
}

static constexpr F64
BPMToSPB(F64 bpm) {
  Assert(bpm >= 0.f);
  return 60.0/bpm;
}


static constexpr F32 
DegToRad(F32 degrees) {
  return degrees * pi_F32 / 180.f;
}
static constexpr F32 
RadToDeg(F32 radians) {
  return radians * 180.f / pi_F32;	
}

static constexpr F64
DegToRad(F64 degrees) {
  return degrees * pi_F32 / 180.0;
  
}
static constexpr F64 
RadToDeg(F64 radians) {
  return radians * 180.0 / pi_F64;
  
}




static constexpr U16
EndianSwap16(U16 value) {
  return (value << 8) | (value >> 8);
}

static constexpr U32
EndianSwap32(U32 value) {
  return  ((value << 24) |
           ((value & 0xFF00) << 8) |
           ((value >> 8) & 0xFF00) |
           (value >> 24));
  
}

///////////////////////////////////
//~ NOTE(Momo): Non-trivial math functions
// We have to use math.h here as default
// TODO(Momo): Write our own trig function
#include <math.h>
static F32 
Sin(F32 x) {
  return sinf(x);
}

static F32 
Cos(F32 x) {
  return cosf(x);
}

static F32
Tan(F32 x) {
  return tanf(x);
}
static F32 
Sqrt(F32 x) {
  return sqrtf(x);
  
}
static F32 
Asin(F32 x) {
  return asinf(x);
}
static F32 
Acos(F32 x) {
  return acosf(x);
}

static F32
Atan(F32 x){
  return atanf(x);
}
static F32 
Pow(F32 b, F32 e){
  return powf(b,e);
}


static F64 
Sin(F64 x) {
  return sin(x);
}

static F64 
Cos(F64 x) {
  return cos(x);
}

static F64
Tan(F64 x) {
  return tan(x);
}
static F64 
Sqrt(F64 x) {
  return sqrt(x);
  
}
static F64 
Asin(F64 x) {
  return asin(x);
}
static F64 
Acos(F64 x) {
  return acos(x);
}

static F64
Atan(F64 x){
  return atan(x);
}
static F64 
Pow(F64 b, F64 e){
  return pow(b,e);
}

//~ NOTE(Momo): Easing functions

static F32 
EaseInSine(F32 t)  {
  return Sin(pi_F32 * 0.5f * t);
}


static F32 
EaseOutSine(F32 t) {
  return 1.0f + Sin(pi_F32 * 0.5f * (--t));
}

static F32 
EaseInOutSine(F32 t)  {
  return 0.5f * (1.f + Sin(pi_F32 * (t - 0.5f)));
}

static F32 
EaseInQuad(F32 t)  {
  return t * t;
}

static F32 
EaseOutQuad(F32 t)  {
  return t * (2.f -t);
}

static F32 
EaseInOutQuad(F32 t)  {
  return t < 0.5f ? 2.f * t * t : t * (4.f -2.f * t) - 1.f;
}

static F32 
EaseInCubic(F32 t)  {
  return t * t * t;
}

static F32 
EaseOutCubic(F32 t)  {
  return 1.f + (t-1) * (t-1) * (t-1);
}

static F32 
EaseInOutCubic(F32 t)  {
  return t < 0.5f ? 4.f * t * t * t : 1.f + (t-1) * (2.f * (t-2)) * (2.f * (t-2));
}

static F32 
EaseInQuart(F32 t)  {
  t *= t;
  return t * t;
}

static F32 
EaseOutQuart(F32 t) {
  --t;
  t = t * t;
  return 1.f - t * t;
}

static F32 
EaseInOutQuart(F32 t)  {
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
EaseInQuint(F32 t)  {
  F32 t2 = t * t;
  return t * t2 * t2;
}

static F32
EaseOutQuint(F32 t)  {
  --t;
  F32 t2 = t * t;
  return 1.f +t * t2 * t2;
}

static F32
EaseInOutQuint(F32 t)  {
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
EaseInCirc(F32 t)  {
  return 1.f -Sqrt(1.f -t);
}

static F32 
EaseOutCirc(F32 t)  {
  return Sqrt(t);
}

static F32 
EaseInOutCirc(F32 t)  {
  if (t < 0.5f) {
    return (1.f -Sqrt(1.f -2.f * t)) * 0.5f;
  }
  else {
    return (1.f +Sqrt(2.f * t - 1.f)) * 0.5f;
  }
}

static F32 
EaseInBack(F32 t)  {
  return t * t * (2.7f * t - 1.7f);
}

static F32 
EaseOutBack(F32 t)  {
  --t;
  return 1.f + t * t * (2.7f * t + 1.7f);
}

static F32 
EaseInOutBack(F32 t)  {
  if (t < 0.5f) {
    return t * t * (7.f * t - 2.5f) * 2.f;
  }
  else {
    --t;
    return 1.f + t * t * 2.f * (7.f * t + 2.5f);
  }
}

static F32 
EaseInElastic(F32 t)  {
  F32 t2 = t * t;
  return t2 * t2 * Sin(t * pi_F32 * 4.5f);
}

static F32 
EaseOutElastic(F32 t)  {
  F32 t2 = (t - 1.f) * (t - 1.f);
  return 1.f -t2 * t2 * Cos(t * pi_F32 * 4.5f);
}

static F32 
EaseInOutElastic(F32 t)  {
  F32 t2;
  if (t < 0.45f) {
    t2 = t * t;
    return 8.f * t2 * t2 * Sin(t * pi_F32 * 9.f);
  }
  else if (t < 0.55f) {
    return 0.5f +0.75f * Sin(t * pi_F32 * 4.f);
  }
  else {
    t2 = (t - 1.f) * (t - 1.f);
    return 1.f -8.f * t2 * t2 * Sin(t * pi_F32 * 9.f);
  }
}

static F32 
EaseInBounce(F32 t)  {
  return Pow(2.f, 6.f * (t - 1.f)) * Abs(Sin(t * pi_F32 * 3.5f));
}


static F32 
EaseOutBounce(F32 t) {
  return 1.f -Pow(2.f, -6.f * t) * Abs(Cos(t * pi_F32 * 3.5f));
}

static F32 
EaseInOutBounce(F32 t) {
  if (t < 0.5f) {
    return 8.f * Pow(2.f, 8.f * (t - 1.f)) * Abs(Sin(t * pi_F32 * 7.f));
  }
  else {
    return 1.f -8.f * Pow(2.f, -8.f * t) * Abs(Sin(t * pi_F32 * 7.f));
  }
}

static F32
EaseInExpo(F32 t)  {
  return (Pow(2.f, 8.f * t) - 1.f) / 255.f;
}


static F32 
EaseOutExpo(F32 t)  {
  return t == 1.f ? 1.f : 1.f -Pow(2.f, -10.f * t);
}

static F32 
EaseInOutExpo(F32 t)  {
  if (t < 0.5f) {
    return (Pow(2.f, 16.f * t) - 1.f) / 510.f;
  }
  else {
    return 1.f -0.5f * Pow(2.f, -16.f * (t - 0.5f));
  }
}



static F64 
EaseInSine(F64 t)  {
  return Sin(pi_F64 * 0.5 * t);
}


static F64 
EaseOutSine(F64 t) {
  return 1.0f + Sin(pi_F64 * 0.5 * (--t));
}

static F64 
EaseInOutSine(F64 t)  {
  return 0.5 * (1.0 + Sin(pi_F64 * (t - 0.5)));
}

static F64 
EaseInQuad(F64 t)  {
  return t * t;
}

static F64 
EaseOutQuad(F64 t)  {
  return t * (2.0 -t);
}

static F64 
EaseInOutQuad(F64 t)  {
  return t < 0.5 ? 2.0 * t * t : t * (4.0 -2.0 * t) - 1.0;
}

static F64 
EaseInCubic(F64 t)  {
  return t * t * t;
}

static F64 
EaseOutCubic(F64 t)  {
  return 1.0 + (t-1) * (t-1) * (t-1);
}

static F64 
EaseInOutCubic(F64 t)  {
  return t < 0.5 ? 4.0 * t * t * t : 1.0 + (t-1) * (2.0 * (t-2)) * (2.0 * (t-2));
}

static F64 
EaseInQuart(F64 t)  {
  t *= t;
  return t * t;
}

static F64 
EaseOutQuart(F64 t) {
  --t;
  t = t * t;
  return 1.0 - t * t;
}

static F64 
EaseInOutQuart(F64 t)  {
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
EaseInQuint(F64 t)  {
  F64 t2 = t * t;
  return t * t2 * t2;
}

static F64
EaseOutQuint(F64 t)  {
  --t;
  F64 t2 = t * t;
  return 1.0 +t * t2 * t2;
}

static F64
EaseInOutQuint(F64 t)  {
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
EaseInCirc(F64 t)  {
  return 1.0 -Sqrt(1.0 -t);
}

static F64 
EaseOutCirc(F64 t)  {
  return Sqrt(t);
}

static F64 
EaseInOutCirc(F64 t)  {
  if (t < 0.5) {
    return (1.0 -Sqrt(1.0 -2.0 * t)) * 0.5;
  }
  else {
    return (1.0 +Sqrt(2.0 * t - 1.0)) * 0.5;
  }
}

static F64 
EaseInBack(F64 t)  {
  return t * t * (2.7 * t - 1.7);
}

static F64 
EaseOutBack(F64 t)  {
  --t;
  return 1.0 + t * t * (2.7 * t + 1.7);
}

static F64 
EaseInOutBack(F64 t)  {
  if (t < 0.5) {
    return t * t * (7.0 * t - 2.5) * 2.0;
  }
  else {
    --t;
    return 1.0 + t * t * 2.0 * (7.0 * t + 2.5);
  }
}

static F64 
EaseInElastic(F64 t)  {
  F64 t2 = t * t;
  return t2 * t2 * Sin(t * pi_F64 * 4.5);
}

static F64 
EaseOutElastic(F64 t)  {
  F64 t2 = (t - 1.0) * (t - 1.0);
  return 1.0 -t2 * t2 * Cos(t * pi_F64 * 4.5);
}

static F64 
EaseInOutElastic(F64 t)  {
  F64 t2;
  if (t < 0.45) {
    t2 = t * t;
    return 8.0 * t2 * t2 * Sin(t * pi_F64 * 9.0);
  }
  else if (t < 0.55) {
    return 0.5 +0.75 * Sin(t * pi_F64 * 4.0);
  }
  else {
    t2 = (t - 1.0) * (t - 1.0);
    return 1.0 -8.0 * t2 * t2 * Sin(t * pi_F64 * 9.0);
  }
}



// NOTE(Momo): These require power function. 
static F64 
EaseInBounce(F64 t)  {
  return Pow(2.0, 6.0 * (t - 1.0)) * Abs(Sin(t * pi_F64 * 3.5));
}


static F64 
EaseOutBounce(F64 t) {
  return 1.0 -Pow(2.0, -6.0 * t) * Abs(Cos(t * pi_F64 * 3.5));
}

static F64 
EaseInOutBounce(F64 t) {
  if (t < 0.5) {
    return 8.0 * Pow(2.0, 8.0 * (t - 1.0)) * Abs(Sin(t * pi_F64 * 7.0));
  }
  else {
    return 1.0 -8.0 * Pow(2.0, -8.0 * t) * Abs(Sin(t * pi_F64 * 7.0));
  }
}

static F64
EaseInExpo(F64 t)  {
  return (Pow(2.0, 8.0 * t) - 1.0) / 255.0;
}


static F64 
EaseOutExpo(F64 t)  {
  return t == 1.0 ? 1.0 : 1.0 -Pow(2.0, -10.0 * t);
}

static F64 
EaseInOutExpo(F64 t)  {
  if (t < 0.5) {
    return (Pow(2.0, 16.0 * t) - 1.0) / 510.0;
  }
  else {
    return 1.0 -0.5 * Pow(2.0, -16.0 * (t - 0.5));
  }
}
