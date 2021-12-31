////////////////////////////////////////////////////////////////
// NOTE(Momo): This unit defines common units used by
// the rest of the library units. 
#ifndef MOMO_BASE_H
#define MOMO_BASE_H

#include <stdarg.h>

//////////////////////////////////////////
//~ NOTE(Momo): Contexts
// NOTE(Momo): Language specs
#if defined(__cplusplus)
# define IS_CPP 1
#else
# define IS_CPP 0
#endif // __cplusplus


// NOTE(Momo): Compiler contexts
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

// NOTE(Momo) OS contexts
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

// NOTE(Momo): CPU architecture contexts
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


//////////////////////////////////////////
//~NOTE(Momo): Compile options
#if !defined(ENABLE_ASSERT)
#define ENABLE_ASSERT 1
#endif // ENABLE_ASSERT

//////////////////////////////////////////
//~ NOTE(Momo): Basic types
// TODO(Momo): Cater for cases where stdint.h does not exist
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

// NOTE(Momo): Memory indices. Integer values for storing
// memory addresses
// X32 when in 32-bit, X64 when in 64-bit, etc 
#if COMPILER_GCC || COMPILER_CLANG
# include <stddef.h>
#endif
typedef uintptr_t UMI; // aka 'unsigned memory index'
typedef ptrdiff_t SMI; // aka 'signed memory index'

//////////////////////////////////////////
//~NOTE(Momo): Helper Macros
#define Stringify_(s) #s
#define Stringify(s) Stringify(s)
#define Glue_(a,b) a##b
#define Glue(a,b) Glue_(a,b)

#define PtrToInt(p) (UMI)((char*)p - (char*)0)
#define IntToPtr(u) ((U8*)((char*)0 + (u)))
#define OffsetOfMember(type,member) PtrToInt(&(((type*)0)->member))

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

#define Abs(x) ((x)<0?(-(x)):(x))
#define Min(l,r) ((l)<(r)?(l):(r))
#define Max(l,r) ((l)>(r)?(l):(r))
#define Clamp(b,x,t) (Max(Min(x,t),b))
#define Lerp(s,e,f) ((s)+((e)-(s))*(f))
#define Ratio(v, min, max) (((v)-(min))/((max)-(min)))
#define DigitToASCII(d) ((d) + '0')
#define ASCIIToDigit(a) ((a) - '0')
#define AlignDownPow2(p,a) ((p) & ~((a)-1))
#define AlignUpPow2(p,a) (((p)+((a)-1)) & ~((a)-1))
#define OffsetOf(type, member) (UMI)&(((type*)0)->member)
#define IsPow2(v) (((v) & ((v) - 1)) == 0)

// This isn't very useful
//#define AlignOf(type) ((UMI)&((struct { char c; type d; } *)0)->d)

// auto might be useful here. C++ version to note:
// #define Swap(A,B) do{ auto tmp = (A); (A) = (B); (B) = tmp; } while(0);
// #define Swap(type,a,b) Stmt(type tmp = (a); (a) = (b); (b) = tmp;)
#define Swap(type,a,b) \
Stmt(char tmp[sizeof(type)]; \
Bin_Copy(tmp, &(a), sizeof(type)); \
Bin_Copy(&(a), &(b), sizeof(type)); \
Bin_Copy(&(b), tmp, sizeof(type));\
);


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
static const S8  S8_min  = -0x80;
static const S16 S16_min = -0x8000; 
static const S32 S32_min = -0x80000000ll;
static const S64 S64_min = -0x8000000000000000ll;

static const S8  S8_max  = 0x7F;
static const S16 S16_max = 0x7FFF; 
static const S32 S32_max = 0x7FFFFFFFl;
static const S64 S64_max = 0x7FFFFFFFFFFFFFFFll;

static const U8  U8_max  = 0xFF;
static const U16 U16_max = 0xFFFF; 
static const U32 U32_max = 0xFFFFFFFF;
static const U64 U64_max = 0xFFFFFFFFFFFFFFFFllu;

static const F32 F32_epsilon = 1.1920929E-7f;
static const F32 F32_pi = 3.14159265359f;
static const F32 F32_tau = 6.28318530718f;
static const F32 F32_gold = 1.61803398875f;

static const F64 F64_epsilon = 2.220446E-16;
static const F64 F64_pi = 3.14159265359;
static const F64 F64_tau = 6.28318530718;
static const F64 F64_gold = 1.61803398875;

#define KB(n) ((1<<10) * n)
#define MB(n) ((1<<20) * n)
#define GB(n) ((1<<30) * n)


// NOTE(Momo): define our own true and false
// if we are in C land
#if !IS_CPP
# define true 1
# define false 0
# define nullptr ((void*)0)
#endif

//~ NOTE(Momo): Memory-related helpers
typedef struct  {
  void* data;
  UMI size;
} Memory_Block;

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
static F32 F32_Abs(F32 x); // Better and more accurate than using usual abs() algo
static F32 F32_Inf();
static F32 F32_NegInf();
static B32 F32_Match(F32 lhs, F32 rhs);

static F64 F64_Abs(F64 x);
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

static S8   S8_Abs(S8 value);
static S16  S16_Abs(S16 value);
static S32  S32_Abs(S32 value);
static S64  S64_Abs(S64 value);

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



#if IS_CPP 
/////////////////////////////////////////////////
// NOTE(Momo): If we are coding in CPP, I can do
// a little more cool stuff.

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



#endif //MOMO_BASE_H
