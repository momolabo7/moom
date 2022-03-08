
#ifndef MOMO_ESSENTIALS_H
#define MOMO_ESSENTIALS_H

#include <stdarg.h>
#undef min

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
// NOTE(Momo): For ARM, there are probably different versions
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

#define ns_begin(name) namespace name {
#define ns_end(name) }

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
#define stringify_(s) #s
#define stringify(s) stringify_(s)
#define glue_(a,b) a##b
#define glue(a,b) glue_(a,b)
// NOTE(Momo): We need this do/while  
// macro to cater for if/else statements
// that looks like this:
// >> if (...) 
// >>     swap(...); 
// >> else 
// >>     ...
// because it will expand to:
// >> if (...) 
// >>    {...}; 
// >> else 
// >>    ...
// which causes an invalid ';' error
//
#define Stmt(s) do { s } while(0)
#define array_count(A) (sizeof(A)/sizeof(*A))
#define OffsetOf(type, member) (UMI)&(((type*)0)->member)

// These need to be macros instead of constexpr function
// because I don't want these to return or take in to a specific strict type.
// Returning a strict type almost always end up requiring an explicit
// conversion on the user side.
// 
#define KB(n) ((1<<10) * n)
#define MB(n) ((1<<20) * n)
#define GB(n) ((1<<30) * n)
#define digit_to_ascii(d) ((d) + '0')
#define ascii_to_digit(a) ((a) - '0')



//~Helper functions
static constexpr UMI ptr_to_int(void* p);
static constexpr U8* int_to_ptr(UMI u);

// NOTE(Momo): It's ridiculous how much goes into the implementation of 
// a generic Min/Max function in modern C++. 
// https://www.drdobbs.com/generic-min-and-max-redivivus/184403774
// For now we keep these simple and use template functions, so that
// we do not need to reevaluate the arguments every time.
//
template<typename T> static constexpr T min_of(T l, T r);
template<typename T> static constexpr T max_of(T l, T r);
template<typename T> static constexpr T clamp(T x, T b, T t);

template<typename T> static constexpr T abs_of(T x);
static constexpr F32 abs_of(F32 value);
static constexpr F64 abs_of(F64 value);
static constexpr S8  abs_of(S8 value);
static constexpr S16 abs_of(S16 value);
static constexpr S32 abs_of(S32 value);
static constexpr S64 abs_of(S64 value);

static constexpr F32 sign_of(F32 value);
static constexpr F64 sign_of(F64 value);
static constexpr S8  sign_of(S8 value);
static constexpr S16 sign_of(S16 value);
static constexpr S32 sign_of(S32 value);
static constexpr S64 sign_of(S64 value);


// NOTE(Momo): Lerp is tricky because the 'f' variable the 'percentage'.
// and must be of a floating point type. I'm not sure if I want to go into
// the hellhole of checking if 'f' is a floating point via TMP. Seems overkill
// since there are only 2 floating point types I generally care about.
template<typename T> static constexpr T lerp(T s, T e, F32 f); 
template<typename T> static constexpr T lerp(T s, T e, F64 f); 

// NOTE(Momo): Ratio is an interesting function. 
// All 1D Ratios will end up with a 1D FXX type.
// All 2D ratios will end up with a 2D FXX type.
// The problem is that, there doesn't seem to be a neat way to express that
// without forcing users to specify the return type somewhere in the function.
// Maybe we will to resort to a RatioF32 and a RatioF64?
// 
// For now, we will just overload the Ratio function, until it bugs us.
static constexpr F32 ratio(F32 v, F32 min, F32 max);
static constexpr F64 ratio(F64 v, F64 min, F64 max);

template<typename T, typename U> static constexpr T align_down_pow2(T value, U align);
template<typename T, typename U> static T align_up_pow2(T value, U align);
template<typename T> static constexpr B32 is_pow2(T value);
template<typename T> static constexpr void swap(T* lhs, T* rhs); 


//~NOTE(Momo): assert
// NOTE(Momo): Others can provide their own 'assert_callback' 
#if !defined(assert_callback)
#define assert_callback(s) (*(volatile int*)0 = 0)
#endif // AssertBreak

#if ENABLE_ASSERT
#define assert(s) Stmt(if(!(s)) { assert_callback(s); })
#else // !ENABLE_ASSERT
#define assert(s)
#endif // ENABLE_ASSERT

//////////////////////////////////////////
//~NOTE(Momo): Constants
// I'm fairly convinced that if we compile with C++, 
// static variables should be better than #define literals.
// Memory-wise they *should* the produces the same results, 
// but constexpr provides stronger typing. 
// This should be my attitude for all constants.
static constexpr S8  S8_MIN  = -0x80;
static constexpr S16 S16_MIN = -0x8000; 
static constexpr S32 S32_MIN = -0x80000000ll;
static constexpr S64 S64_MIN = -0x8000000000000001ll - 1;

static constexpr S8  S8_MAX  = 0x7F;
static constexpr S16 S16_MAX = 0x7FFF; 
static constexpr S32 S32_MAX = 0x7FFFFFFFl;
static constexpr S64 S64_MAX = 0x7FFFFFFFFFFFFFFFll;

static constexpr U8  U8_MAX  = 0xFF;
static constexpr U16 U16_MAX = 0xFFFF; 
static constexpr U32 U32_MAX = 0xFFFFFFFF;
static constexpr U64 U64_MAX = 0xFFFFFFFFFFFFFFFFllu;

static constexpr F32 F32_EPSILON = 1.1920929E-7f;
static constexpr F64 F64_EPSILON = 2.220446E-16;

static constexpr F32 F32_INFINITY();
static constexpr F32 F32_NEG_INFINITY();
static constexpr F64 F64_INFINITY();
static constexpr F64 F64_NEG_INFINITY();
//~ NOTE(Momo): Memory-related helpers

// This is a really useful construct I find myself using 
// more and more. 
struct Memory  {
  union {
    void* data;
    U8* data_u8;
  };
  UMI size;  
};
static B32 is_ok(Memory);

static void copy_memory(void* dest, const void* src, UMI size);
static void zero_memory(void* dest, UMI size);
static void swap_memory(void* lhs, void* rhs, UMI size);
static B32  match_memory(const void* lhs, const void* rhs, UMI size);

#define zero_struct(p)    zero_memory((p), sizeof(*(p)))
#define zero_array(p)     zero_memory((p), sizeof(p))
#define zero_range(p,s)   zero_memory((p), sizeof(*(p)) * (s))

#define copy_struct(p)    copy_memory((p), sizeof(*(p)))
#define copy_array(p)     copy_memory((p), sizeof(p))
#define copy_range(p,s)   copy_memory((p), sizeof(*(p)) * (s))

//~ NOTE(Momo): C-string
static UMI  cstr_len(const char* str);
static void cstr_copy(char * dest, const char* Src);
static B32  cstr_compare(const char* lhs, const char* rhs);
static B32  cstr_compare_n(const char* lhs, const char* rhs, UMI n);
static void cstr_concat(char* dest, const char* Src);
static void cstr_clear(char* dest);
static void cstr_reverse(char* dest);
static void cstr_itoa(char* dest, S32 num);

//~ NOTE(Momo): IEEE floating point functions 
static constexpr B32 match(F32 lhs, F32 rhs);
static constexpr B32 match(F64 lhs, F64 rhs);
// Ah...cannot overload operator==...

//~ NOTE(Momo): Useful calculations
static constexpr F32 deg_to_rad(F32 degrees) ;
static constexpr F64 deg_to_rad(F64 degrees) ;
static constexpr F32 rad_to_deg(F32 radians);
static constexpr F64 rad_to_deg(F64 radians);

// Beats per min to Secs per beat
static constexpr F32 bpm_to_spb(F32 bpm); 
static constexpr F64 bpm_to_spb(F64 bpm); 


// NOTE(Momo): I'm not entirely sure if this prototype makes sense.
// It sounds more reasonable to endian swap ANY type. 
// We COULD use a template approach like so:
//   template<typename T> endian_swap_16(T value);
//   template<typename T> endian_swap_32(T value); 
// Or we COULD just ignore the concept of type:
//   void _EndianSwap16(U8* ptr)
//   #define endian_swap_16(value) _EndianSwap16((U8*)&value)
static constexpr U16 endian_swap_16(U16 value);
static constexpr U32 endian_swap_32(U32 value);


//~ NOTE(Momo): Math functions that are not trivial
static constexpr F32 PI_32 = 3.14159265359f;
static constexpr F64 PI_64 = 3.14159265359;
static constexpr F32 TAU_32 = 6.28318530718f;
static constexpr F64 TAU_64 = 6.28318530718;
static constexpr F32 GOLD_32 = 1.61803398875f;
static constexpr F64 GOLD_64 = 1.61803398875;

static F32 sin(F32 x);
static F32 cos(F32 x);
static F32 tan(F32 x);
static F32 sqrt(F32 x);
static F32 asin(F32 x);
static F32 acos(F32 x);
static F32 atan(F32 x);
static F32 pow(F32 v, F32 e);

static F64 sin(F64 x);
static F64 cos(F64 x);
static F64 tan(F64 x);
static F64 sqrt(F64 x);
static F64 asin(F64 x);
static F64 acos(F64 x);
static F64 atan(F64 x);
static F64 pow(F64 , F64 e);

static F32 ceil(F32 value);
static F32 floor(F32 value);
static F64 ceil(F64 value);
static F64 floor(F64 value);

//~ NOTE(Momo): Easing functions
static F32 ease_in_sine(F32 t);
static F32 ease_out_sine(F32 t);
static F32 ease_inout_sine(F32 t);
static F32 ease_in_quad(F32 t);
static F32 ease_out_quad(F32 t);
static F32 ease_inout_quad(F32 t);
static F32 ease_in_cubic(F32 t);
static F32 ease_out_cubic(F32 t);
static F32 ease_inout_cubic(F32 t);
static F32 ease_in_quart(F32 t);
static F32 ease_out_quart(F32 t);
static F32 ease_inout_quart(F32 t);
static F32 ease_in_quint(F32 t);
static F32 ease_out_quint(F32 t);;
static F32 ease_inout_quint(F32 t);
static F32 ease_in_circ(F32 t);
static F32 ease_out_circ(F32 t);
static F32 ease_inout_circ(F32 t);
static F32 ease_in_back(F32 t);
static F32 ease_out_back(F32 t);
static F32 ease_inout_back(F32 t);
static F32 ease_in_elastic(F32 t);
static F32 ease_out_elastic(F32 t);
static F32 ease_inout_elastic(F32 t);
static F32 ease_in_bounce(F32 t);
static F32 ease_out_bounce(F32 t);
static F32 ease_inout_bounce(F32 t);
static F32 ease_in_expo(F32 t);
static F32 ease_out_expo(F32 t);
static F32 ease_inout_expo(F32 t);

static F64 ease_in_sine(F64 t);
static F64 ease_out_sine(F64 t);
static F64 ease_inout_sine(F64 t);
static F64 ease_in_quad(F64 t);
static F64 ease_out_quad(F64 t);
static F64 ease_inout_quad(F64 t);
static F64 ease_in_cubic(F64 t);
static F64 ease_out_cubic(F64 t);
static F64 ease_inout_cubic(F64 t);
static F64 ease_in_quart(F64 t);
static F64 ease_out_quart(F64 t);
static F64 ease_inout_quart(F64 t);
static F64 ease_in_quint(F64 t);
static F64 ease_out_quint(F64 t);;
static F64 ease_inout_quint(F64 t);
static F64 ease_in_circ(F64 t);
static F64 ease_out_circ(F64 t);
static F64 ease_inout_circ(F64 t);
static F64 ease_in_back(F64 t);
static F64 ease_out_back(F64 t);
static F64 ease_inout_back(F64 t);
static F64 ease_in_elastic(F64 t);
static F64 ease_out_elastic(F64 t);
static F64 ease_inout_elastic(F64 t);
static F64 ease_in_bounce(F64 t);
static F64 ease_out_bounce(F64 t);
static F64 ease_inout_bounce(F64 t);
static F64 ease_in_expo(F64 t);
static F64 ease_out_expo(F64 t);
static F64 ease_inout_expo(F64 t);

//~ NOTE(Momo): Defer construct
template<typename F> 
struct zawarudo_ScopeGuard {
  F f;
  ~zawarudo_ScopeGuard() { f(); }
};
struct zawarudo_defer_dummy {};
template<typename F> zawarudo_ScopeGuard<F> operator+(zawarudo_defer_dummy, F f) {
  return { f };
}
#define zawarudo_AnonVarSub(x) zawarudo_defer##x
#define zawarudo_AnonVar(x) zawarudo_AnonVarSub(x)
#define defer auto zawarudo_AnonVar(__LINE__) = zawarudo_defer_dummy{} + [&]()



#include "momo_essentials.cpp"

#endif //MOMO_ESSENTIALS_H
