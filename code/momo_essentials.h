
#ifndef MOMO_ESSENTIALS_H
#define MOMO_ESSENTIALS_H

#include <stdarg.h>

//~Contexts
// Language specs
#if defined(__cplusplus)
# define IS_CPP 1
#else
# define IS_CPP 0
#endif // __cplusplus

// Compiler contexts
#if defined(_MSC_VER) 
# define COMPILER_MSVC 1
#elif defined(__clang__)
# define COMPILER_CLANG 1
#elif defined(__GNUC__) 
# define COMPILER_GCC 1
#else 
# error unsupported compiler
#endif

#if !defined(COMPILER_MSVC)
# define COMPILER_MSVC 0
#endif 
#if !defined(COMPILER_CLANG)
# define COMPILER_CLANG 0
#endif 
#if !defined(COMPILER_GCC) 
# define COMPILER_GCC 0
#endif

// OS contexts
#if defined(_WIN32)
# define OS_WINDOWS 1
#elif defined(__gnu_linux__)
# define OS_LINUX 1
#elif defined(__APPLE__) && defined(__MACH__) 
# define OS_MAC 1
#else 
# error unsupported OS
#endif

#if !defined(OS_WINDOWS)
# define OS_WINDOWS 0
#endif
#if !defined(OS_LINUX)
# define OS_LINUX 0
#endif
#if !defined(OS_MAC)
# define OS_MAC 0
#endif 

// CPU architecture contexts
// TODO(Momo): For ARM, there are probably different versions
#if COMPILER_MSVC
# if defined(_M_X86)
#  define ARCH_X86 1
# elif defined(_M_AMD64)
#  define ARCH_X64 1
# elif defined(_M_ARM)
#  define ARCH_ARM 1
# else
#  error unsupported architecture
# endif
#elif COMPILER_GCC || COMPILER_CLANG
# if defined(__i386__)
#  define ARCH_X86 1
# elif defined(__x86_64)
#  define ARCH_X64 1
# elif defined(__arm__)
#  define ARCH_ARM 1
# else 
#  error unsupported architecture
# endif
#endif

#if !defined(ARCH_X86)
# define ARCH_X86 0
#endif
#if !defined(ARCH_X64)
# define ARCH_X64 0
#endif
#if !defined(ARCH_ARM)
# define ARCH_ARM 0
#endif 

///////////////////////////////////////////
//~function specifier helpers
#define c_link_begin extern "C" {
#define c_link_end }
#define c_link extern "C"

#if COMPILER_MSVC
# define exported c_link __declspec(dllexport)
#else
# error exported not defined for this compiler
#endif

//////////////////////////////////////////
//~Compile options
#if !defined(ENABLE_ASSERT)
#define ENABLE_ASSERT 1
#endif // ENABLE_ASSERT

//////////////////////////////////////////
//~Basic types
// TODO(Momo): Cater for cases where stdint.h does not exist?
#include <stdint.h>
typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
typedef int8_t S8;
typedef int16_t S16;
typedef int32_t S32;
typedef int64_t S64;
typedef float F32;
typedef double F64;
typedef U8 B8;
typedef U16 B16;
typedef U32 B32;
typedef U64 B64;
typedef char C8;

// Memory indices. Integer values for storing memory addresses.
// X32 when in 32-bit, X64 when in 64-bit, etc 
#if COMPILER_GCC || COMPILER_CLANG
# include <stddef.h>
#endif
typedef uintptr_t UMI; // aka 'unsigned memory index'
typedef ptrdiff_t SMI; // aka 'signed memory index'

//~Helper Macros
#define Stringify_(s) #s
#define Stringify(s) Stringify(s)
#define Glue_(a,b) a##b
#define Glue(a,b) Glue_(a,b)
// NOTE(Momo): We need this do/while  
// macro to cater for if/else statements
// that looks like this:
// >> if (...) 
// >>     Swap(...); 
// >> else 
// >>     ...
// because it will expand to:
// >> if (...) 
// >>    {...}; 
// >> else 
// >>    ...
// which causes an invalid ';' error
// TODO: Blog this?
//
#define Stmt(s) do { s } while(0)
#define ArrayCount(A) (sizeof(A)/sizeof(*A))
#define OffsetOf(type, member) (UMI)&(((type*)0)->member)

// These need to be macros instead of constexpr function
// because I don't want these to return or take in to a specific strict type.
// Returning a strict type almost always end up requiring an explicit
// conversion on the user side.
// 
// I mean...we can also create different versions of KB/MB/GB too...
//
#define KB(n) ((1<<10) * n)
#define MB(n) ((1<<20) * n)
#define GB(n) ((1<<30) * n)
#define DigitToASCII(d) ((d) + '0')
#define ASCIIToDigit(a) ((a) - '0')



//~Helper functions
static constexpr UMI PtrToInt(void* p);
static constexpr U8* IntToPtr(UMI u);

// NOTE(Momo): It's ridiculous how much goes into the implementation of 
// a generic Min/Max function in modern C++. 
// https://www.drdobbs.com/generic-min-and-max-redivivus/184403774
// For now we keep these simple and use template functions, so that
// we do not need to reevaluate the arguments every time.
// TODO(Momo): We might want t 'comparison' function version for min and max
//
template<class T> static constexpr T Min(T l, T r);
template<class T> static constexpr T Max(T l, T r);
template<class T> static constexpr T Clamp(T x, T b, T t);

template<class T> static constexpr T Abs(T x);
static constexpr F32 Abs(F32 value);
static constexpr F64 Abs(F64 value);
static constexpr S8  Abs(S8 value);
static constexpr S16 Abs(S16 value);
static constexpr S32 Abs(S32 value);
static constexpr S64 Abs(S64 value);

// NOTE(Momo): Lerp is tricky because the 'f' variable the 'percentage'.
// and must be of a floating point type. I'm not sure if I want to go into
// the hellhole of checking if 'f' is a floating point via TMP. Seems overkill
// since there are only 2 floating point types I generally care about.
template<class T> static constexpr T Lerp(T s, T e, F32 f); 
template<class T> static constexpr T Lerp(T s, T e, F64 f); 

// NOTE(Momo): Ratio is an interesting function. 
// All 1D Ratios will end up with a 1D FXX type.
// All 2D ratios will end up with a 2D FXX type.
// The problem is that, there doesn't seem to be a neat way to express that
// without forcing users to specify the return type somewhere in the function.
// Maybe we will to resort to a RatioF32 and a RatioF64?
// 
// For now, we will just overload the Ratio function, until it bugs us.
static constexpr F32 Ratio(F32 v, F32 min, F32 max);
static constexpr F64 Ratio(F64 v, F64 min, F64 max);

template<class T, class U> static constexpr T AlignDownPow2(T value, U align);
template<class T, class U> static T AlignUpPow2(T value, U align);
template<class T> static constexpr B32 IsPow2(T value);
template<class T> static constexpr void Swap(T* lhs, T* rhs); 

//~NOTE(Momo): Assert
// NOTE(Momo): Others can provide their own 'AssertCallback' 
#if !defined(AssertCallback)
#define AssertCallback(s) (*(volatile int*)0 = 0)
#endif // AssertBreak

#if ENABLE_ASSERT
#define Assert(s) Stmt(if(!(s)) { AssertCallback(s); })
#else // !ENABLE_ASSERT
#define Assert(s)
#endif // ENABLE_ASSERT

//////////////////////////////////////////
//~NOTE(Momo): Constants
// I'm fairly convinced that if we compile with C++, 
// static variables should be better than #define literals.
// Memory-wise they *should* the produces the same results, 
// but constexpr provides stronger typing. 
// This should be my attitude for all constants.
static constexpr S8  S8_min  = -0x80;
static constexpr S16 S16_min = -0x8000; 
static constexpr S32 S32_min = -0x80000000ll;
static constexpr S64 S64_min = -0x8000000000000001ll - 1;

static constexpr S8  S8_max  = 0x7F;
static constexpr S16 S16_max = 0x7FFF; 
static constexpr S32 S32_max = 0x7FFFFFFFl;
static constexpr S64 S64_max = 0x7FFFFFFFFFFFFFFFll;

static constexpr U8  U8_max  = 0xFF;
static constexpr U16 U16_max = 0xFFFF; 
static constexpr U32 U32_max = 0xFFFFFFFF;
static constexpr U64 U64_max = 0xFFFFFFFFFFFFFFFFllu;

static constexpr F32 F32_epsilon = 1.1920929E-7f;
static constexpr F64 F64_epsilon = 2.220446E-16;

static constexpr F32 F32_Inf();
static constexpr F32 F32_NegInf();
static constexpr F64 F64_Inf();
static constexpr F64 F64_NegInf();
//~ NOTE(Momo): Memory-related helpers

// This is a really useful construct I find myself using 
// more and more. 
struct Memory  {
  void* data;
  UMI size;  
  operator B32(){ return data && size; };
};

static void Bin_Copy(void* dest, const void* src, UMI size);
static void Bin_Zero(void* dest, UMI size);
static void Bin_Swap(void* lhs, void* rhs, UMI size);
static B32  Bin_Match(const void* lhs, const void* rhs, UMI size);

#define Bin_ZeroStruct(p)    Bin_Zero((p), sizeof(*(p)))
#define Bin_ZeroArray(p)     Bin_Zero((p), sizeof(p))
#define Bin_ZeroRange(p,s)   Bin_Zero((p), sizeof(*(p)) * (s))

#define Bin_CopyStruct(p)    Bin_Copy((p), sizeof(*(p)))
#define Bin_CopyArray(p)     Bin_Copy((p), sizeof(p))
#define Bin_CopyRange(p,s)   Bin_Copy((p), sizeof(*(p)) * (s))

//~ NOTE(Momo): C-string
static UMI  Sistr_Length(const char* str);
static void Sistr_Copy(char * dest, const char* Src);
static B32  Sistr_Compare(const char* lhs, const char* rhs);
static B32  Sistr_CompareN(const char* lhs, const char* rhs, UMI n);
static void Sistr_Concat(char* dest, const char* Src);
static void Sistr_Clear(char* dest);
static void Sistr_Reverse(char* dest);
static void Sistr_Itoa(char* dest, S32 num);

//~ NOTE(Momo): IEEE floating point functions 
static constexpr B32 Match(F32 lhs, F32 rhs);
static constexpr B32 Match(F64 lhs, F64 rhs);
// Ah...cannot overload operator==...

//~ NOTE(Momo): Useful calculations
static constexpr F32 DegToRad(F32 degrees) ;
static constexpr F64 DegToRad(F64 degrees) ;
static constexpr F32 RadToDeg(F32 radians);
static constexpr F64 RadToDeg(F64 radians);

// Beats per min to Secs per beat
static constexpr F32 BPMToSPB(F32 bpm); 
static constexpr F64 BPMToSPB(F64 bpm); 


// NOTE(Momo): I'm not entirely sure if this prototype makes sense.
// It sounds more reasonable to endian swap ANY type. 
// We COULD use a template approach like so:
//   template<class T> EndianSwap16(T value);
//   template<class T> EndianSwap32(T value); 
// Or we COULD just ignore the concept of type:
//   void _EndianSwap16(U8* ptr)
//   #define EndianSwap16(value) _EndianSwap16((U8*)&value)
// TODO(Momo): Let's figure EndianSwap one day
static constexpr U16 EndianSwap16(U16 value);
static constexpr U32 EndianSwap32(U32 value);


//~ NOTE(Momo): Math functions that are not trivial
static constexpr F32 pi_F32 = 3.14159265359f;
static constexpr F64 pi_F64 = 3.14159265359;
static constexpr F32 tau_F32 = 6.28318530718f;
static constexpr F64 tau_F64 = 6.28318530718;
static constexpr F32 gold_F32 = 1.61803398875f;
static constexpr F64 gold_F64 = 1.61803398875;

static F32 Sin(F32 x);
static F32 Cos(F32 x);
static F32 Tan(F32 x);
static F32 Sqrt(F32 x);
static F32 Asin(F32 x);
static F32 Acos(F32 x);
static F32 Atan(F32 x);
static F32 Pow(F32 v, F32 e);

static F64 Sin(F64 x);
static F64 Cos(F64 x);
static F64 Tan(F64 x);
static F64 Sqrt(F64 x);
static F64 Asin(F64 x);
static F64 Acos(F64 x);
static F64 Atan(F64 x);
static F64 Pow(F64 , F64 e);

//~ NOTE(Momo): Easing functions
static F32 EaseInSine(F32 t);
static F32 EaseOutSine(F32 t);
static F32 EaseInOutSine(F32 t);
static F32 EaseInQuad(F32 t);
static F32 EaseOutQuad(F32 t);
static F32 EaseInOutQuad(F32 t);
static F32 EaseInCubic(F32 t);
static F32 EaseOutCubic(F32 t);
static F32 EaseInOutCubic(F32 t);
static F32 EaseInQuart(F32 t);
static F32 EaseOutQuart(F32 t);
static F32 EaseInOutQuart(F32 t);
static F32 EaseInQuint(F32 t);
static F32 EaseOutQuint(F32 t);;
static F32 EaseInOutQuint(F32 t);
static F32 EaseInCirc(F32 t);
static F32 EaseOutCirc(F32 t);
static F32 EaseInOutCirc(F32 t);
static F32 EaseInBack(F32 t);
static F32 EaseOutBack(F32 t);
static F32 EaseInOutBack(F32 t);
static F32 EaseInElastic(F32 t);
static F32 EaseOutElastic(F32 t);
static F32 EaseInOutElastic(F32 t);
static F32 EaseInBounce(F32 t);
static F32 EaseOutBounce(F32 t);
static F32 EaseInOutBounce(F32 t);
static F32 EaseInExpo(F32 t);
static F32 EaseOutExpo(F32 t);
static F32 EaseInOutExpo(F32 t);

static F64 EaseInSine(F64 t);
static F64 EaseOutSine(F64 t);
static F64 EaseInOutSine(F64 t);
static F64 EaseInQuad(F64 t);
static F64 EaseOutQuad(F64 t);
static F64 EaseInOutQuad(F64 t);
static F64 EaseInCubic(F64 t);
static F64 EaseOutCubic(F64 t);
static F64 EaseInOutCubic(F64 t);
static F64 EaseInQuart(F64 t);
static F64 EaseOutQuart(F64 t);
static F64 EaseInOutQuart(F64 t);
static F64 EaseInQuint(F64 t);
static F64 EaseOutQuint(F64 t);;
static F64 EaseInOutQuint(F64 t);
static F64 EaseInCirc(F64 t);
static F64 EaseOutCirc(F64 t);
static F64 EaseInOutCirc(F64 t);
static F64 EaseInBack(F64 t);
static F64 EaseOutBack(F64 t);
static F64 EaseInOutBack(F64 t);
static F64 EaseInElastic(F64 t);
static F64 EaseOutElastic(F64 t);
static F64 EaseInOutElastic(F64 t);
static F64 EaseInBounce(F64 t);
static F64 EaseOutBounce(F64 t);
static F64 EaseInOutBounce(F64 t);
static F64 EaseInExpo(F64 t);
static F64 EaseOutExpo(F64 t);
static F64 EaseInOutExpo(F64 t);

//~ NOTE(Momo): Defer construct
template<class F> 
struct zawarudo_ScopeGuard {
  F f;
  ~zawarudo_ScopeGuard() { f(); }
};
struct zawarudo_defer_dummy {};
template<class F> zawarudo_ScopeGuard<F> operator+(zawarudo_defer_dummy, F f) {
  return { f };
}
#define zawarudo_AnonVarSub(x) zawarudo_defer##x
#define zawarudo_AnonVar(x) zawarudo_AnonVarSub(x)
#define defer auto zawarudo_AnonVar(__LINE__) = zawarudo_defer_dummy{} + [&]()



#include "momo_essentials.cpp"

#endif //MOMO_ESSENTIALS_H
