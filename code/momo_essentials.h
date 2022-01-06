
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
// TODO(Momo): I mean...we can also create different versions of KB/MB/GB too...
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
template<class T, class U> T AlignUpPow2(T value, U align);

template<class T> static constexpr B32 IsPow2(T value);
template<class T> static constexpr void Swap(T& lhs, T& rhs); 


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
static constexpr S64 S64_min = -0x8000000000000000ll;

static constexpr S8  S8_max  = 0x7F;
static constexpr S16 S16_max = 0x7FFF; 
static constexpr S32 S32_max = 0x7FFFFFFFl;
static constexpr S64 S64_max = 0x7FFFFFFFFFFFFFFFll;

static constexpr U8  U8_max  = 0xFF;
static constexpr U16 U16_max = 0xFFFF; 
static constexpr U32 U32_max = 0xFFFFFFFF;
static constexpr U64 U64_max = 0xFFFFFFFFFFFFFFFFllu;

static constexpr F32 F32_epsilon = 1.1920929E-7f;
static constexpr F32 F32_pi = 3.14159265359f;
static constexpr F32 F32_tau = 6.28318530718f;
static constexpr F32 F32_gold = 1.61803398875f;

static constexpr F64 F64_epsilon = 2.220446E-16;
static constexpr F64 F64_pi = 3.14159265359;
static constexpr F64 F64_tau = 6.28318530718;
static constexpr F64 F64_gold = 1.61803398875;

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
static F32 F32_Inf();
static F32 F32_NegInf();
static B32 F32_Match(F32 lhs, F32 rhs);


static F64 F64_Inf();
static F64 F64_NegInf();
static B32 F64_Match(F64 lhs, F64 rhs);

//~ NOTE(Momo): Useful calculations
static F32 F32_DegToRad(F32 degrees) ;
static F32 F32_RadToDeg(F32 radians);
static F32 F32_BPMToSPB(F32 bpm); 

static F64 F64_DegToRad(F64 degrees) ;
static F64 F64_RadToDeg(F64 radians);
static F64 F64_BPMToSPB(F64 bpm); 

static U16 U16_EndianSwap(U16 value);
static U32 U32_EndianSwap(U32 value);


//~ NOTE(Momo): Math functions that are not trivial
static F32 F32_Sin(F32 x);
static F32 F32_Cos(F32 x);
static F32 F32_Tan(F32 x);
static F32 F32_Sqrt(F32 x);
static F32 F32_Asin(F32 x);
static F32 F32_Acos(F32 x);
static F32 F32_Atan(F32 x);
static F32 F32_Pow(F32 v, F32 e);

static F64 F64_Sin(F64 x);
static F64 F64_Cos(F64 x);
static F64 F64_Tan(F64 x);
static F64 F64_Sqrt(F64 x);
static F64 F64_Asin(F64 x);
static F64 F64_Acos(F64 x);
static F64 F64_Atan(F64 x);
static F64 F64_Pow(F64 , F64 e);

//~ NOTE(Momo): Easing functions
static F32 F32_EaseInSine(F32 t);
static F32 F32_EaseOutSine(F32 t);
static F32 F32_EaseInOutSine(F32 t);
static F32 F32_EaseInQuad(F32 t);
static F32 F32_EaseOutQuad(F32 t);
static F32 F32_EaseInOutQuad(F32 t);
static F32 F32_EaseInCubic(F32 t);
static F32 F32_EaseOutCubic(F32 t);
static F32 F32_EaseInOutCubic(F32 t);
static F32 F32_EaseInQuart(F32 t);
static F32 F32_EaseOutQuart(F32 t);
static F32 F32_EaseInOutQuart(F32 t);
static F32 F32_EaseInQuint(F32 t);
static F32 F32_EaseOutQuint(F32 t);;
static F32 F32_EaseInOutQuint(F32 t);
static F32 F32_EaseInCirc(F32 t);
static F32 F32_EaseOutCirc(F32 t);
static F32 F32_EaseInOutCirc(F32 t);
static F32 F32_EaseInBack(F32 t);
static F32 F32_EaseOutBack(F32 t);
static F32 F32_EaseInOutBack(F32 t);
static F32 F32_EaseInElastic(F32 t);
static F32 F32_EaseOutElastic(F32 t);
static F32 F32_EaseInOutElastic(F32 t);
static F32 F32_EaseInBounce(F32 t);
static F32 F32_EaseOutBounce(F32 t);
static F32 F32_EaseInOutBounce(F32 t);
static F32 F32_EaseInExpo(F32 t);
static F32 F32_EaseOutExpo(F32 t);
static F32 F32_EaseInOutExpo(F32 t);

static F64 F64_EaseInSine(F64 t);
static F64 F64_EaseOutSine(F64 t);
static F64 F64_EaseInOutSine(F64 t);
static F64 F64_EaseInQuad(F64 t);
static F64 F64_EaseOutQuad(F64 t);
static F64 F64_EaseInOutQuad(F64 t);
static F64 F64_EaseInCubic(F64 t);
static F64 F64_EaseOutCubic(F64 t);
static F64 F64_EaseInOutCubic(F64 t);
static F64 F64_EaseInQuart(F64 t);
static F64 F64_EaseOutQuart(F64 t);
static F64 F64_EaseInOutQuart(F64 t);
static F64 F64_EaseInQuint(F64 t);
static F64 F64_EaseOutQuint(F64 t);;
static F64 F64_EaseInOutQuint(F64 t);
static F64 F64_EaseInCirc(F64 t);
static F64 F64_EaseOutCirc(F64 t);
static F64 F64_EaseInOutCirc(F64 t);
static F64 F64_EaseInBack(F64 t);
static F64 F64_EaseOutBack(F64 t);
static F64 F64_EaseInOutBack(F64 t);
static F64 F64_EaseInElastic(F64 t);
static F64 F64_EaseOutElastic(F64 t);
static F64 F64_EaseInOutElastic(F64 t);
static F64 F64_EaseInBounce(F64 t);
static F64 F64_EaseOutBounce(F64 t);
static F64 F64_EaseInOutBounce(F64 t);
static F64 F64_EaseInExpo(F64 t);
static F64 F64_EaseOutExpo(F64 t);
static F64 F64_EaseInOutExpo(F64 t);


//~ NOTE(Momo): CPP helpers. Additional cool stuff we can do in C++.
#if IS_CPP 
// NOTE(Momo): Defer construct
template<class F> 
struct zawarudo_ScopeGuard {
  F f;
  ~zawarudo_ScopeGuard() { f(); }
};
struct zawarudo_defer_dummy {};
template<class F> zawarudo_ScopeGuard<F> operator+(zawarudo_defer_dummy, F f) {
  return { f };
}

# define zawarudo_AnonVarSub(x) zawarudo_defer##x
# define zawarudo_AnonVar(x) zawarudo_AnonVarSub(x)
# define defer auto zawarudo_AnonVar(__LINE__) = zawarudo_defer_dummy{} + [&]()

#endif // IS_CPP


#include "momo_essentials.cpp"

#endif //MOMO_ESSENTIALS_H
