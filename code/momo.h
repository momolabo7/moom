// 
// DESCRIPTION
//   This file contains the entirety of useful and (hopefully)
//   portable functions and structures that are very useful for
//   anything that I do in C/C++
//
// FLAGS
//   ASSERTIVE - Enables/Disables asserts. Default is 1. 
//
//
// BOOKMARKS
//   
//   Includes    - Includes 
//   Context     - Context Defines (Arch, OS, compiler, etc)
//   Constants   - Common constant values
//   Helper      - Common helper macros and functions 
//   Float       - Common functions for floats (f32, f64)
//   Int         - Common functions for integers (s32, u32, etc)
//   ASCII       - Helper functions for ASCII 
//   Memory      - Helper functions for Memory manipulation
//   Hash        - Hash functions (using CString) 
//   Linked List - Linked List macros 
//   SI Units    - Helper functions to get SI units
//   Bonk        - Collision detection 
//   Buffer      - Simple type for holding a chunk of memory
//   Vector      - Math vectors
//   Matrix      - Math matrices
//   Color       - Color structs (RGBA, HSL, HSV, ...)
//   RNG         - Random Numer Generator
//   CRC         - CRC generators
//   Sorting     - Sorting algorithms
//   Arena       - Standard Linear Memory Arena
//   Garena      - General Purpose Heap Arena
//   String      - String manipulation
//   CString     - CString manipulation
//   Stream      - Memory stream
//   TTF         - TTF font file
//   WAV         - WAV audio file
//   PNG         - PNG image file
//   JSON        - JSON data file
//   RectPack    - Rectangle packer
//   Clex        - C Lexer
//   OS          - OS Layer
//
// TODO
//   - Stream API 
//


#ifndef MOMO_H
#define MOMO_H

//
// MARK:(Includes)
//
#include <stdarg.h> // for varadic arguments
#include <math.h>

//
// MARK:(Contexts)
//
#if defined(_MSC_VER) 
# define COMPILER_MSVC 1
#elif defined(__clang__)
# define COMPILER_CLANG 1
#elif defined(__GNUC__) 
# define COMPILER_GCC 1
#else 
# warning "[momo] unsupported compiler"
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

//
// OS contexts
//
#if defined(_WIN32)
# define OS_WINDOWS 1
#elif defined(__gnu_linux__)
# define OS_LINUX 1
#elif defined(__APPLE__) && defined(__MACH__) 
# define OS_MAC 1
#else 
# warning "[momo] unsupported OS"
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

//
// CPU architecture contexts
//
// NOTE(Momo): For ARM, there are probably different versions
//
#if COMPILER_MSVC
# if defined(_M_X86)
#  define ARCH_X86 1
# elif defined(_M_AMD64)
#  define ARCH_X64 1
# elif defined(_M_ARM)
#  define ARCH_ARM 1
# else
#  warning "[momo] unsupported architecture"
# endif
#elif COMPILER_GCC || COMPILER_CLANG
# if defined(__i386__)
#  define ARCH_X86 1
# elif defined(__x86_64)
#  define ARCH_X64 1
# elif defined(__arm__)
#  define ARCH_ARM 1
# else 
#  warning "[momo] unsupported architecture"
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

//
// Export helpers
//
#define c_link_begin extern "C" {
#define c_link_end }
#define c_link extern "C"

#if COMPILER_MSVC
# define exported c_link __declspec(dllexport)
#elif COMPILER_GCC
# define exported __attribute__((visibility("default")))  
#elif COMPILER_CLANG
# define exported __attribute__((visibility("default")))  
#else
# warning "[momo] 'exported' not defined for this compiler"
#endif

//
// Asserts
//
#if !defined(ASSERTIVE)
# define ASSERTIVE 1
#endif 

# if !defined(assert_callback)
#  define assert_callback(s) (*(volatile int*)0 = 0)
# endif 

#if ASSERTIVE
# define assert(s) stmt(if(!(s)) { assert_callback(s); })
#else 
# define assert(s)
#endif 

//
// Primitive Types 
//
#include <stdint.h>
#if COMPILER_GCC || COMPILER_CLANG
# include <stddef.h>
#endif

typedef uint8_t  u8_t;
typedef uint16_t u16_t;
typedef uint32_t u32_t;
typedef uint64_t u64_t;
typedef int8_t   s8_t;
typedef int16_t  s16_t;
typedef int32_t  s32_t;
typedef int64_t  s64_t;
typedef float    f32_t;
typedef double   f64_t;
typedef u8_t     b8_t;
typedef u32_t    b32_t;
typedef char     c8_t;

// Memory indices. Integer values for storing memory addresses.
// X32 when in 32-bit, X64 when in 64-bit, etc 
typedef uintptr_t umi_t; // aka 'unsigned memory index'
typedef intptr_t  smi_t; // aka 'signed memory index'
typedef size_t    usz_t; // Can contain up to the highest indexable value 

////
//
// Custom Types
//

//
// MARK:(Buffer)
//
struct buffer_t {
  u8_t* data;
  usz_t size;
};
static b32_t buffer_is_valid(buffer_t buffer);


//
// MARK:(Vector)
//
union v2u_t {
  struct { u32_t x, y; };
  struct { u32_t w, h; };
  u32_t e[2];
};

union v2s_t {
  struct { s32_t x, y; };
  struct { s32_t w, h; };
  s32_t e[2];
};

union v2f_t {
  struct { f32_t x, y; };
  struct { f32_t w, h; };
  struct { f32_t u, v; };
  f32_t e[2];
};

union v3f_t {
  struct { f32_t x, y, z; };
  struct { f32_t w, h, d; };
  f32_t e[3];
};

union v4f_t {
  struct { f32_t x, y, z, w; };
  f32_t e[4];
};

//
// MARK:(Matrix)
//
struct m44f_t {
  f32_t e[4][4];
};

//
// MARK:(Color)
//
struct rgb_t {
  f32_t r, g, b;   
};

// Each component are in the range of [0 - 1].
// For hue, normally it is a number between [0 - 360], but
// it will be mapped linearly to [0 - 1] in this case.
// i.e. hue 0 is 0 degrees, hue 1 is 360 degrees.
struct hsl_t {
  f32_t h, s, l;  
};

struct hsla_t {
  union {
    struct { f32_t h,s,l; };  
    hsl_t hsl;
  };
  f32_t a;
};


struct rgba_t {
  union {
    struct { f32_t r, g, b; };  
    rgb_t rgb;
  };
  f32_t a;
};

//
// MARK:(RNG Declarations)
//
struct rng_t {
  u32_t seed;
};

//
// MARK:(Sorting)
//
struct sort_entry_t {
  f32_t key;
  u32_t index;
};

//
// MARK:(CRC)
//
struct crc32_table_t {
  u32_t remainders[256];
};

struct crc16_table_t {
  u16_t remainders[256];
};

struct crc8_table_t {
  u8_t remainders[256];
};




//
// MARK:(Arena)
//
struct arena_t {
  u8_t* memory;
  usz_t pos;
  usz_t cap;

  usz_t highest_memory_usage;
};

// Temporary memory API used to arena_revert an arena to an original state;
struct arena_marker_t {
  arena_t* arena;
  usz_t old_pos;
};

// API to calculate how much memory would be used by emulating pushing
// sizes into an arena
struct arena_calc_t {
  usz_t result;
};

//
// MARK:(Garena) 
//
// NOTES
//   Header is always 16 bytes and aligned to 16 bytes.
//
//   This helps in getting back the header address whenever
//   the user frees a block (it's always -16 bytes). 
//
//   This also means that the user's block is always aligned to
//   16 bytes, which, for most use cases is the optimal 
//   alignment for any optmization operations.
//
// 
union garena_header_t {
  usz_t size; 
  struct {
    u64_t padding_1, padding_2;
  };
};

// This should also always be 16 bytes.
union garena_free_block_t {
  struct {
    usz_t size;
    garena_free_block_t* next;
  };
  struct {
    u64_t padding_1, padding_2;
  };
};
static_assert(sizeof(garena_header_t) == 16);
static_assert(sizeof(garena_free_block_t) == 16);

struct garena_t {
  u8_t* memory;
  usz_t cap;

  garena_free_block_t* free_list;
};

//
// MARK:(Strings)
//
struct st8_t {
  u8_t* e;
  usz_t count;
};

struct stb8_t{
  union {
    st8_t str;
    struct {
      u8_t* e;
      usz_t count;
    };
  };
  usz_t cap;
};

//
// MARK:(Stream)
//
struct stream_t {
  buffer_t contents;
  usz_t pos;

  // For bit reading
  u32_t bit_buffer;
  u32_t bit_count;
};


//
// MARK:(TTF) 
//
struct ttf_t {
  u8_t* data;
  u32_t glyph_count;

  // these are positions from data
  u32_t loca, head, glyf, maxp, cmap, hhea, hmtx, kern, gpos;
  u32_t cmap_mappings;

  u16_t loca_format;
};

//
// MARK:(WAV Declaration)
//
struct wav_riff_chunk_t {
  u32_t id; // big endian
  u32_t size;
  u32_t format; // big endian
};

struct wav_fmt_chunk_t {
  u32_t id;
  u32_t size;
  u16_t audio_format;
  u16_t num_channels;
  u32_t sample_rate;
  u32_t byte_rate;
  u16_t block_align;
  u16_t bits_per_sample;
};

struct wav_data_chunk_t {
  u32_t id;
  u32_t size;
};

struct wav_t {
  wav_riff_chunk_t riff_chunk;
  wav_fmt_chunk_t fmt_chunk;
  wav_data_chunk_t data_chunk;
  void* data;
};

//
// MARK:(PNG Declaration)
//
struct png_t {
  buffer_t contents;

  u32_t width;
  u32_t height;
  u8_t bit_depth;
  u8_t colour_type;
  u8_t compression_method;
  u8_t filter_method;
  u8_t interlace_method;
};

// 
// MARK:(RectPack)
//
enum rp_sort_type_t {
  RP_SORT_TYPE_WIDTH,
  RP_SORT_TYPE_HEIGHT,
  RP_SORT_TYPE_AREA,
  RP_SORT_TYPE_PERIMETER,
  RP_SORT_TYPE_BIGGER_SIDE,
  RP_SORT_TYPE_PATHOLOGICAL,
};

struct rp_rect_t{
  u32_t x, y, w, h;
  void* user_data;
};

//
// MARK:(Clex)
//

enum clex_token_type_t {
  CLEX_TOKEN_TYPE_UNKNOWN,
  CLEX_TOKEN_TYPE_OPEN_PAREN,          // (
  CLEX_TOKEN_TYPE_CLOSE_PAREN,         // )
  CLEX_TOKEN_TYPE_SEMICOLON,           // ;
  CLEX_TOKEN_TYPE_COMMA,               // ,
  CLEX_TOKEN_TYPE_DOT,                 // ,
  CLEX_TOKEN_TYPE_COLON,               // : 
  CLEX_TOKEN_TYPE_SCOPE,               // :: 
  CLEX_TOKEN_TYPE_OPEN_BRACKET,        // [
  CLEX_TOKEN_TYPE_CLOSE_BRACKET,       // ]
  CLEX_TOKEN_TYPE_OPEN_BRACE,          // {
  CLEX_TOKEN_TYPE_CLOSE_BRACE,         // } 
  CLEX_TOKEN_TYPE_PLUS,                // +
  CLEX_TOKEN_TYPE_PLUS_PLUS,           // ++ 
  CLEX_TOKEN_TYPE_PLUS_EQUAL,          // += 
  CLEX_TOKEN_TYPE_MINUS,               // -
  CLEX_TOKEN_TYPE_MINUS_MINUS,         // -- 
  CLEX_TOKEN_TYPE_MINUS_EQUAL,         // -= 
  CLEX_TOKEN_TYPE_ARROW,               // ->
  CLEX_TOKEN_TYPE_EQUAL,               // =
  CLEX_TOKEN_TYPE_EQUAL_EQUAL,         // ==
  CLEX_TOKEN_TYPE_GREATER,             // > 
  CLEX_TOKEN_TYPE_GREATER_EQUAL,       // >= 
  CLEX_TOKEN_TYPE_GREATER_GREATER,     // >>
  CLEX_TOKEN_TYPE_LESSER,              // < 
  CLEX_TOKEN_TYPE_LESSER_EQUAL,        // <= 
  CLEX_TOKEN_TYPE_LESSER_LESSER,       // <<
  CLEX_TOKEN_TYPE_OR,                  // |
  CLEX_TOKEN_TYPE_OR_EQUAL,            // |=
  CLEX_TOKEN_TYPE_OR_OR,               // ||
  CLEX_TOKEN_TYPE_AND,                 // & 
  CLEX_TOKEN_TYPE_AND_AND,             // &&
  CLEX_TOKEN_TYPE_AND_EQUAL,           // &=
  CLEX_TOKEN_TYPE_LOGICAL_NOT,         // !
  CLEX_TOKEN_TYPE_BITWISE_NOT,         // ~
  CLEX_TOKEN_TYPE_XOR,                 // ^
  CLEX_TOKEN_TYPE_XOR_EQUAL,           // ^=
  CLEX_TOKEN_TYPE_QUESTION,            // ?
  CLEX_TOKEN_TYPE_STAR,                // *
  CLEX_TOKEN_TYPE_STAR_EQUAL,          // *=
  CLEX_TOKEN_TYPE_SLASH,               // /
  CLEX_TOKEN_TYPE_SLASH_EQUAL,         // /=
  CLEX_TOKEN_TYPE_PERCENT,             // %
  CLEX_TOKEN_TYPE_PERCENT_EQUAL,       // %=
  CLEX_TOKEN_TYPE_IDENTIFIER,
  CLEX_TOKEN_TYPE_KEYWORD,
  CLEX_TOKEN_TYPE_STRING,
  CLEX_TOKEN_TYPE_NUMBER,
  CLEX_TOKEN_TYPE_CHAR,
  CLEX_TOKEN_TYPE_MACRO,
  CLEX_TOKEN_TYPE_EOF
};

// TODO: Change to st8_t?
struct clex_token_t {
  clex_token_type_t type;

  usz_t begin;
  usz_t ope;
};

struct clex_tokenizer_t {
  buffer_t text;
  usz_t at;
};

//
// MARK:(JSON)
//

// The "key" of a JSON entry, which can only be a string.
struct json_key_t {
  u8_t* at;
  usz_t count;
};

// Represents a JSON array, which is a linked list
// of nodes containing more values.
struct json_array_node_t;
struct json_array_t {
  json_array_node_t* head;
  json_array_node_t* tail;
};

// Represents a JSON element, which is a string.
struct json_element_t {
  union {
    struct {
      u8_t* at;
      usz_t count;
    };
    st8_t str;
  };
};

enum json_value_type_t {
  //JSON_VALUE_TYPE_BOOLEAN,

  JSON_VALUE_TYPE_TRUE,
  JSON_VALUE_TYPE_FALSE,

  JSON_VALUE_TYPE_STRING,
  JSON_VALUE_TYPE_NUMBER,
  JSON_VALUE_TYPE_NULL,
  JSON_VALUE_TYPE_ARRAY,
  JSON_VALUE_TYPE_OBJECT,
};

// An object contains a bunch of entries
struct json_object_t {
  struct _json_entry_t* head; // points to the first entry
};

struct json_value_t {
  json_value_type_t type;
  union {
    json_element_t element;
    json_array_t array;
    json_object_t object;
  };
};

struct json_array_node_t {
  json_array_node_t* next;
  json_value_t value;
};

struct json_t {
  // for tokenizing
  st8_t text;
  umi_t at;

  // The 'root' item in a JSON file is an object type.
  json_object_t root;
};


static json_object_t* json_read(json_t* j, const u8_t* json_string, u32_t json_string_size, arena_t* ba);
static json_value_t* json_get_value(json_object_t* j, st8_t key);
static b32_t json_is_true(json_value_t* val);
static b32_t json_is_false(json_value_t* val);
static b32_t json_is_null(json_value_t* val);
static b32_t json_is_string(json_value_t* val);
static b32_t json_is_number(json_value_t* val);
static b32_t json_is_array(json_value_t* val);
static b32_t json_is_object(json_value_t* val);
static b32_t json_is_element(json_value_t* val);
static json_element_t* json_get_element(json_value_t* val);
static json_array_t* json_get_array(json_value_t* val);
static json_object_t* json_get_object(json_value_t* val);

//
// MARK:(Constants)
//
#define S8_MIN  (-0x80)
#define S16_MIN (-0x8000)
#define S32_MIN (-0x80000000ll)
#define S64_MIN (-0x8000000000000001ll - 1)

#define S8_MAX  (0x7F)
#define S16_MAX (0x7FFF)
#define S32_MAX (0x7FFFFFFFl)
#define S64_MAX (0x7FFFFFFFFFFFFFFFll)

#define U8_MAX  (0xFF)
#define U16_MAX (0xFFFF)
#define U32_MAX (0xFFFFFFFF)
#define U64_MAX (0xFFFFFFFFFFFFFFFFllu)

#define F32_EPSILON       (1.1920929E-7f)
#define F32_INFINITY      (_F32_INFINITY())
#define F32_NEG_INFINITY  (_F32_NEG_INFINITY())
#define F32_NAN           (_F32_NAN())

#define F64_EPSILON       (2.220446E-16)
#define F64_INFINITY      (_F64_INFINITY())
#define F64_NEG_INFINITY  (_F64_NEG_INFINITY())
#define F64_NAN           (_F64_NAN())

#define PI_32   (3.14159265359f)
#define PI_64   (3.14159265359)
#define TAU_32  (6.28318530718f)
#define TAU_64  (6.28318530718)
#define GOLD_32 (1.61803398875f)
#define GOLD_64 (1.61803398875)

//
// MARK:(SI Units)
//
// These need to be macros instead of function
// because I don't want these to return or take in to a specific strict type.
// Returning a strict type almost always end up requiring an explicit
// conversion on the user side.
// 
#define kilobytes(n) ((1<<10) * n)
#define megabytes(n) ((1<<20) * n)
#define gigabytes(n) ((1<<30) * n)
#define hundreds(x) ((x) * 100) 
#define thousands(x) ((x) * 1000)

//
// MARK:(Helpers)
//
#define dref(expr) (*(expr))
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
#define stmt(s) do { s } while(0)
#define array_count(A) (sizeof(A)/sizeof(*A))
#define offset_of(type, member) (umi_t)&(((type*)0)->member)
#define make(t, name) \
  t glue(name##_,__LINE__) = {0}; \
  t* name = &(glue(name##_,__LINE__))

#define ns_begin(name) namespace name {
#define ns_end(name) }

#define min_of(l,r) ((l) < (r) ? (l) : (r))
#define max_of(l,r) ((l) > (r) ? (l) : (r))
#define clamp_of(x,b,t) (max_of(min_of(x,t),b))
#define swap(l,r) { auto tmp = (l); (l) = (r); (r) = tmp; } 

#define for_arr(id, arr) for(umi_t id = 0, cnt = array_count(arr); (id) < (cnt); ++id)
#define for_cnt(id, cnt) for(decltype(cnt) id = 0; id < (cnt); ++id)
#define for_range(id, beg, end) for(decltype(beg) id = (beg); id <= (end); ++id)

template<typename F> 
struct _defer_scope_guard {
  F f;
  ~_defer_scope_guard() { f(); }
};
struct _defer_dummy {};
template<typename F> _defer_scope_guard<F> operator+(_defer_dummy, F f) {
  return { f };
}
#define defer auto glue(_defer, __LINE__) = _defer_dummy{} + [&]()


//
// MARK:(Buffer)
//
static buffer_t buffer_set(u8_t* mem, usz_t size);

//
// MARK:(ASCII)
//
#define digit_to_ascii(d) ((d) + '0')
#define ascii_to_digit(a) ((a) - '0')
static b32_t is_whitespace(char c);
static b32_t is_alpha(char c);
static b32_t is_digit(char c);

//
// MARK:(Memory)
//
#define bit_is_set(mask,bit) ((mask) & ((u64_t)1 << (bit)))
#define bit_set(mask, bit) ((mask) |= ((u64_t)1 << (bit)))
#define bit_unset(mask, bit) ((mask) &= ~((u64_t)1 << (bit)))
#define align_down_pow2(v,a) ((v) & ~((a)-1))
#define align_up_pow2(v,a) ((v) + ((a)-1) & ~((a)-1))
#define is_pow2(v) ((v) & ((v)-1) == 0)
#define zero_struct(p)    zero_memory((p), sizeof(*(p)))
#define zero_array(p)     zero_memory((p), sizeof(p))
#define zero_range(p,s)   zero_memory((p), sizeof(*(p)) * (s))
#define copy_struct(d,s)  copy_memory((d), (s), sizeof(*(d)))
#define copy_array(d,s)   copy_memory((d), (s), sizeof(d))
#define copy_range(d,s,n) copy_memory((d), (s), sizeof(*(d)) * (n))
static void copy_memory(void* dest, const void* src, usz_t size);
static void copy_memory(void* dest, const void* src, usz_t size);
static void zero_memory(void* dest, usz_t size);
static b32_t is_memory_same(const void* lhs, const void* rhs, usz_t size);
static void swap_memory(void* lhs, void* rhs, usz_t size);
static umi_t ptr_to_umi(void* p);
static u8_t* umi_to_ptr(umi_t u);

//
// MARK:(Float)
//
static f32_t f32_abs(f32_t x);
static f32_t f32_lerp(f32_t s, f32_t e, f32_t f);
static f32_t f32_mod(f32_t lhs, f32_t rhs);
static f32_t f32_weight(f32_t v, f32_t min, f32_t max);
static f32_t f32_deg_to_rad(f32_t degrees);
static f32_t f32_rad_to_deg(f32_t radians);
static f32_t f32_turns_to_radians(f32_t turns);
static f32_t f32_factorial(f32_t x);
static f32_t f32_bpm_to_spb(f32_t bpm);
static b32_t f32_is_close(f32_t lhs, f32_t rhs); 
static b32_t f32_is_nan(f32_t f); 
static f32_t f32_sin(f32_t x);
static f32_t f32_cos(f32_t x);
static f32_t f32_tan(f32_t x);
static f32_t f32_sqrt(f32_t x);
static f32_t f32_asin(f32_t x);
static f32_t f32_acos(f32_t x);
static f32_t f32_atan(f32_t x);
static f32_t f32_pow(f32_t v, f32_t e);
static f32_t f32_ceil(f32_t value);
static f32_t f32_floor(f32_t value);
static f32_t f32_round(f32_t value);
static f32_t f32_ease_linear(f32_t t);
static f32_t f32_ease_in_sine(f32_t t);
static f32_t f32_ease_out_sine(f32_t t);
static f32_t f32_ease_inout_sine(f32_t t);
static f32_t f32_ease_in_quad(f32_t t);
static f32_t f32_ease_out_quad(f32_t t);
static f32_t f32_ease_inout_quad(f32_t t);
static f32_t f32_ease_in_cubic(f32_t t);
static f32_t f32_ease_out_cubic(f32_t t);
static f32_t f32_ease_inout_cubic(f32_t t);
static f32_t f32_ease_in_quart(f32_t t);
static f32_t f32_ease_out_quart(f32_t t);
static f32_t f32_ease_inout_quart(f32_t t);
static f32_t f32_ease_in_quint(f32_t t);
static f32_t f32_ease_out_quint(f32_t t);;
static f32_t f32_ease_inout_quint(f32_t t);
static f32_t f32_ease_in_circ(f32_t t);
static f32_t f32_ease_out_circ(f32_t t);
static f32_t f32_ease_inout_circ(f32_t t);
static f32_t f32_ease_in_back(f32_t t);
static f32_t f32_ease_out_back(f32_t t);
static f32_t f32_ease_inout_back(f32_t t);
static f32_t f32_ease_in_elastic(f32_t t);
static f32_t f32_ease_out_elastic(f32_t t);
static f32_t f32_ease_inout_elastic(f32_t t);
static f32_t f32_ease_in_bounce(f32_t t);
static f32_t f32_ease_out_bounce(f32_t t);
static f32_t f32_ease_inout_bounce(f32_t t);
static f32_t f32_ease_in_expo(f32_t t);
static f32_t f32_ease_out_expo(f32_t t);
static f32_t f32_ease_inout_expo(f32_t t);

static f64_t f64_abs(f64_t x);
static f64_t f64_lerp(f64_t s, f64_t e, f64_t f); 
static f64_t f64_mod(f64_t lhs, f64_t rhs); 
static f64_t f64_weight(f64_t v, f64_t min, f64_t max) ;
static f64_t f64_deg_to_rad(f64_t degrees);
static f64_t f64_rad_to_deg(f64_t radians);
static f64_t f64_turns_to_radians(f64_t turns);
static f64_t f64_factorial(f64_t x);
static f64_t f64_bpm_to_spb(f64_t bpm);
static b32_t f64_is_close(f64_t lhs, f64_t rhs); 
static b32_t f64_is_nan(f64_t f); 
static f64_t f64_sin(f64_t x);
static f64_t f64_cos(f64_t x);
static f64_t f64_tan(f64_t x);
static f64_t f64_sqrt(f64_t x);
static f64_t f64_asin(f64_t x);
static f64_t f64_acos(f64_t x);
static f64_t f64_atan(f64_t x);
static f64_t f64_pow(f64_t , f64_t e);
static f64_t f64_ceil(f64_t value);
static f64_t f64_floor(f64_t value);
static f64_t f64_round(f64_t value);
static f64_t f64_ease_linear(f64_t t);
static f64_t f64_ease_in_sine(f64_t t);
static f64_t f64_ease_out_sine(f64_t t);
static f64_t f64_ease_inout_sine(f64_t t);
static f64_t f64_ease_in_quad(f64_t t);
static f64_t f64_ease_out_quad(f64_t t);
static f64_t f64_ease_inout_quad(f64_t t);
static f64_t f64_ease_in_cubic(f64_t t);
static f64_t f64_ease_out_cubic(f64_t t);
static f64_t f64_ease_inout_cubic(f64_t t);
static f64_t f64_ease_in_quart(f64_t t);
static f64_t f64_ease_out_quart(f64_t t);
static f64_t f64_ease_inout_quart(f64_t t);
static f64_t f64_ease_in_quint(f64_t t);
static f64_t f64_ease_out_quint(f64_t t);;
static f64_t f64_ease_inout_quint(f64_t t);
static f64_t f64_ease_in_circ(f64_t t);
static f64_t f64_ease_out_circ(f64_t t);
static f64_t f64_ease_inout_circ(f64_t t);
static f64_t f64_ease_in_back(f64_t t);
static f64_t f64_ease_out_back(f64_t t);
static f64_t f64_ease_inout_back(f64_t t);
static f64_t f64_ease_in_elastic(f64_t t);
static f64_t f64_ease_out_elastic(f64_t t);
static f64_t f64_ease_inout_elastic(f64_t t);
static f64_t f64_ease_in_bounce(f64_t t);
static f64_t f64_ease_out_bounce(f64_t t);
static f64_t f64_ease_inout_bounce(f64_t t);
static f64_t f64_ease_in_expo(f64_t t);
static f64_t f64_ease_out_expo(f64_t t);
static f64_t f64_ease_inout_expo(f64_t t);

//
// Integers
//
static s8_t  s8_abs(s8_t x);
static s16_t s16_abs(s16_t x);
static s16_t s16_endian_swap(s16_t value);
static s32_t s32_abs(s32_t x); 
static s64_t s64_abs(s64_t x);

static u16_t u16_endian_swap(u16_t value);

static u32_t u32_factorial(u32_t x);
static u32_t u32_atomic_compare_assign(u32_t volatile* value, u32_t new_value, u32_t expected_value);
static u32_t u32_atomic_add(u32_t volatile* value, u32_t to_add);
static u32_t u32_endian_swap(u32_t value);

static u64_t u64_factorial(u64_t x);
static u64_t u64_atomic_assign(u64_t volatile* value, u64_t new_value);
static u64_t u64_atomic_add(u64_t volatile* value, u64_t to_add);

//
// MARK:(CString)
//
static usz_t cstr_len(const c8_t* str); 
static void  cstr_copy(c8_t * dest, const c8_t* src); 
static b32_t cstr_compare(const c8_t* lhs, const c8_t* rhs); 
static b32_t cstr_compare_n(const c8_t* lhs, const c8_t* rhs, usz_t n); 
static void  cstr_concat(c8_t* dest, const c8_t* Src);
static f64_t cstr_to_f64(const c8_t* p); 
static u32_t cstr_to_u32(const c8_t* p); 
static void  cstr_clear(c8_t* dest); 
static void  cstr_reverse(c8_t* dest); 
static void  cstr_itoa(c8_t* dest, s32_t num); 
static u32_t cstr_len_if(const char* str, b32_t (*pred)(char));

//
// MARK:(Hash)
// 
static u32_t hash_djb2(const c8_t* str);

//
// MARK:(Linked List)
//

// Singly Linked List
//
// f - first node
// l - last node
// n - node
#define sll_prepend(f,l,n) (f) ? ((n)->next = (f), (f) = (n)) : ((f) = (l) = (n))
#define sll_append(f,l,n)  (f) ? ((l)->next = (n), (l) = (n)) : ((f) = (l) = (n), (l)->next = 0)

//
// Circular Doubly Linked List with sentinel
// 
// s - sentinel
// n - node
#define cll_init(s)     (s)->prev = (s), (s)->next = (s) 
#define cll_append(s,n) (n)->next = (s), (n)->prev = (s)->prev, (n)->prev->next = (n), (n)->next->prev = (n)
#define cll_remove(n)   (n)->prev->next = (n)->next, (n)->next->prev = (n)->prev, (n)->next = 0, (n)->prev = 0;


//
// MARK:(Vector)
//
static v2f_t v2f_add(v2f_t lhs, v2f_t rhs); 
static v2f_t v2f_sub(v2f_t lhs, v2f_t rhs); 
static v2f_t v2f_scale(v2f_t lhs, f32_t rhs); 
static v2f_t v2f_div(v2f_t lhs, f32_t rhs); 
static v2f_t v2f_inv(v2f_t v); 
static v2f_t v2f_negate(v2f_t v); 
static f32_t v2f_dot(v2f_t lhs, v2f_t rhs); 
static f32_t v2f_len_sq(v2f_t v); 
static f32_t v2f_len(v2f_t v); 
static f32_t v2f_dist_sq(v2f_t lhs, v2f_t rhs); 
static f32_t v2f_dist(v2f_t lhs, v2f_t rhs); 
static v2f_t v2f_norm(v2f_t v); 
static b32_t v2f_is_close(v2f_t lhs, v2f_t rhs); 
static v2f_t v2f_mid(v2f_t lhs, v2f_t rhs); 
static v2f_t v2f_proj(v2f_t v, v2f_t onto); 
static f32_t v2f_angle(v2f_t lhs, v2f_t rhs); 
static v2f_t v2f_rotate(v2f_t v, f32_t rad); 
static f32_t v2f_cross(v2f_t lhs, v2f_t rhs); 
static v2f_t v2f_lerp(v2f_t s, v2f_t e, f32_t a); 
static v3f_t v3f_add(v3f_t lhs, v3f_t rhs); 
static v3f_t v3f_sub(v3f_t lhs, v3f_t rhs); 
static v3f_t v3f_scale(v3f_t lhs, f32_t rhs); 
static v3f_t v3f_div(v3f_t lhs, f32_t rhs); 
static v3f_t v3f_negate(v3f_t v); 
static f32_t v3f_dot(v3f_t lhs, v3f_t rhs); 
static f32_t v3f_len_sq(v3f_t v); 
static f32_t v3f_len(v3f_t v); 
static f32_t v3f_dist_sq(v3f_t lhs, v3f_t rhs); 
static f32_t v3f_dist(v3f_t lhs, v3f_t rhs); 
static v3f_t v3f_norm(v3f_t v); 
static b32_t v3f_is_close(v3f_t lhs, v3f_t rhs); 
static v3f_t v3f_mid(v3f_t lhs, v3f_t rhs); 
static v3f_t v3f_project(v3f_t v, v3f_t onto); 
static f32_t v3f_angle(v3f_t lhs, v3f_t rhs); 
static v3f_t v3f_cross(v3f_t lhs, v3f_t rhs); 
static v2u_t v2u_add(v2u_t lhs, v2u_t rhs); 
static v2u_t v2u_sub(v2u_t lhs, v2u_t rhs);
static v2f_t v2f_set(f32_t x, f32_t y);
static v2f_t v2f_zero();

static v2f_t  operator+(v2f_t lhs, v2f_t rhs); 
static v2f_t  operator-(v2f_t lhs, v2f_t rhs); 
static v2f_t  operator*(v2f_t lhs, f32_t rhs); 
static v2f_t  operator*(f32_t lhs, v2f_t rhs); 
static b32_t  operator==(v2f_t lhs, v2f_t rhs); 
static b32_t  operator!=(v2f_t lhs, v2f_t rhs); 
static v2f_t  operator-(v2f_t v); 
static v2f_t& operator+=(v2f_t& lhs, v2f_t rhs); 
static v2f_t& operator-=(v2f_t& lhs, v2f_t rhs); 
static v2f_t& operator*=(v2f_t& lhs, f32_t rhs); 
static v2u_t  operator+(v2u_t lhs, v2u_t rhs); 
static v2u_t  operator-(v2u_t lhs, v2u_t rhs); 
static v3f_t  operator+(v3f_t lhs, v3f_t rhs); 
static v3f_t  operator-(v3f_t lhs, v3f_t rhs); 
static v3f_t  operator*(v3f_t lhs, f32_t rhs); 
static v3f_t  operator*(f32_t lhs, v3f_t rhs); 
static b32_t  operator==(v3f_t lhs, v3f_t rhs); 
static b32_t  operator!=(v3f_t lhs, v3f_t rhs); 
static v3f_t  operator-(v3f_t v); 
static v3f_t& operator+=(v3f_t& lhs, v3f_t rhs); 
static v3f_t& operator-=(v3f_t& lhs, v3f_t rhs); 
static v3f_t& operator*=(v3f_t& lhs, f32_t rhs); 

//
// MARK:(Matrix)
//
static m44f_t m44f_concat(m44f_t lhs, m44f_t rhs);
static m44f_t m44f_transpose(m44f_t m);
static m44f_t m44f_scale(f32_t x, f32_t y, f32_t z);
static m44f_t m44f_identity();
static m44f_t m44f_translation(f32_t x, f32_t y, f32_t z = 0.f);
static m44f_t m44f_rotation_x(f32_t rad);
static m44f_t m44f_rotation_y(f32_t rad);
static m44f_t m44f_rotation_z(f32_t rad);
static m44f_t m44f_orthographic(f32_t left, f32_t right, f32_t bottom, f32_t top, f32_t near, f32_t far);
static m44f_t m44f_frustum(f32_t left, f32_t right, f32_t bottom, f32_t top, f32_t near, f32_t far);
static m44f_t m44f_perspective(f32_t fov, f32_t aspect, f32_t near, f32_t far);
static m44f_t operator*(m44f_t lhs, m44f_t rhs);

//
// MARK:(Colors)
//
// Notes:
// - While there could be several representation of colors,
//   I would prefer to use floating point values between [0-1] 
//   to cover all general cases. If the user needs to convert it to a more
//   usable and concrete value, say, [0 - 255], they would just need to 
//   multiply the values by 255 and go from there.
//
// Todo:
// - HSV support?
// 

// Each component of rgba_t are in the range [0 - 1].
static hsl_t  hsl_set(f32_t h, f32_t s, f32_t l);
static hsla_t hsla_set(f32_t h, f32_t s, f32_t l, f32_t a);
static rgba_t rgba_set(f32_t r, f32_t g, f32_t b, f32_t a);
static rgba_t rgba_hex(u32_t hex);  
static rgba_t hsla_to_rgba(hsla_t c);
static hsl_t  rbg_to_hsl(rgb_t c);
static rgb_t  hsl_to_rgb(hsl_t c);

#define RGBA_WHITE rgba_set(1.f, 1.f, 1.f, 1.f)


//
// MARK:(Bonk)
//
static b32_t bonk_tri2_pt2(v2f_t tp0, v2f_t tp1, v2f_t tp2, v2f_t pt); 

// 
// MARK:(RNG)
//
static void  rng_init(rng_t* r, u32_t seed);
static u32_t rng_next(rng_t* r);
static u32_t rng_choice(rng_t* r, u32_t choice_count);
static f32_t rng_unilateral(rng_t* r);
static f32_t rng_bilateral(rng_t* r);
static f32_t rng_range_F32(rng_t* r, f32_t min, f32_t max);
static s32_t rng_range_S32(rng_t* r, s32_t min, s32_t max);
static v2f_t rng_unit_circle(rng_t* r);

//
// MARK:(Sorting)
//

// These are for sort entries, which we should try to default to.
static void quicksort(sort_entry_t* entries, u32_t entry_count);

// These are for generic quicksort onto an array.
// Performance depends on size of the element
#define quicksort_generic_predicate_sig(name) b32_t name(const void* lhs, const void* rhs)
#define quicksort_generic(arr, count, predicate) _quicksort_generic(arr, count, sizeof(*arr), predicate)

//
// MARK:(CRC)
//
static void  crc32_init_table(crc32_table_t* table, u32_t polynomial);
static void  crc16_init_table(crc16_table_t* table, u16_t polynomial);
static void  crc8_init_table(crc8_table_t* table, u8_t polynomial); 
static u32_t crc32_slow(u8_t* data, u32_t data_size, u32_t start_register, u32_t polynomial);
static u32_t crc16_slow(u8_t* data, u32_t data_size, u16_t start_register, u16_t polynomial);
static u32_t crc8_slow(u8_t* data, u32_t data_size, u16_t start_register, u16_t polynomial);
static u32_t crc32(u8_t* data, u32_t data_size, u16_t start_register, crc32_table_t* table);
static u32_t crc16(u8_t* data, u32_t data_size, u16_t start_register, crc16_table_t* table);
static u32_t crc8(u8_t* data, u32_t data_size, u8_t start_register, crc8_table_t* table);

// 
// MARK:(Strings)
//
#define st8_from_lit(s) st8_set((u8_t*)(s), sizeof(s)-1)
static st8_t  st8_set(u8_t* str, usz_t size);
static st8_t  st8_substr(st8_t str, usz_t start, usz_t ope);
static b32_t  st8_match(st8_t lhs, st8_t rhs);
static st8_t  st8_from_cstr(const char* cstr);
static smi_t  st8_compare_lexographically(st8_t lhs, st8_t rhs);
static b32_t  st8_to_u32(st8_t s, u32_t* out);
static b32_t  st8_to_f32(st8_t s, f32_t* out);
static b32_t  st8_to_s32(st8_t s, s32_t* out);
static b32_t  st8_range_to(u32_t* out);

#define stb8_make(name, cap) \
  u8_t temp_buffer_##__LINE__[cap] = {0}; \
stb8_t name_; \
stb8_t* name = &name_; \
stb8_init(name, temp_buffer_##__LINE__, cap);

static usz_t    stb8_remaining(stb8_t* b);
static void     stb8_clear(stb8_t* b);
static void     stb8_pop(stb8_t* b);
static void     stb8_push_c8(stb8_t* b, c8_t num);
static void     stb8_push_u8(stb8_t* b, u8_t num);
static void     stb8_push_u32(stb8_t* b, u32_t num);
static void     stb8_push_u64(stb8_t* b, u64_t num);
static void     stb8_push_f32(stb8_t* b, f32_t value, u32_t precision);
static void     stb8_push_s32(stb8_t* b, s32_t num);
static void     stb8_push_s64(stb8_t* b, s64_t num);
static void     stb8_push_st8_set(stb8_t* b, st8_t num);
static void     stb8_push_hex_u8(stb8_t* b, u8_t num);
static void     stb8_push_hex_u32(stb8_t* b, u32_t num);
static void     stb8_push_fmt(stb8_t* b, st8_t fmt, ...);
static void     stb8_init(stb8_t* b, u8_t* data, usz_t cap);

//
// MARK:(Stream)
//
#define stream_consume(t,s) (t*) stream_consume_block((s), sizeof(t))
#define stream_peek(t,s) (t*) stream_peek_block((s), sizeof(t))
#define stream_write(s,item) stream_write_block((s), &(item), sizeof(item))
static void     stream_init(stream_t* s, buffer_t contents);
static void     stream_reset(stream_t* s);
static b32_t    stream_is_eos(stream_t* s);
static u8_t*    stream_consume_block(stream_t* s, usz_t amount);
static u8_t*    stream_peek_block(stream_t* s, usz_t amount);
static void     stream_flush_bits(stream_t* s);
static u32_t    stream_consume_bits(stream_t* s, u32_t amount);
static void     stream_write_block(stream_t* s, void* src, usz_t size);

//
// MARK:(Arena)
//

static void     arena_init(arena_t* a, void* mem, usz_t cap);
static void     arena_clear(arena_t* a);
static void*    arena_push_size(arena_t* a, usz_t size, usz_t align);
static void*    arena_push_size_zero(arena_t* a, usz_t size, usz_t align); 
static b32_t    arena_push_partition(arena_t* a, arena_t* partition, usz_t size, usz_t align);
static b32_t    arena_push_partition_with_remaining(arena_t* a, arena_t* partition, usz_t align);
static buffer_t arena_push_buffer(arena_t* a, usz_t size, usz_t align);
static usz_t    arena_remaining(arena_t* a);

#define arena_push_arr_align(t,b,n,a) (t*)arena_push_size(b, sizeof(t)*(n), a)
#define arena_push_arr(t,b,n)         (t*)arena_push_size(b, sizeof(t)*(n),alignof(t))
#define arena_push_align(t,b,a)       (t*)arena_push_size(b, sizeof(t), a)
#define arena_push(t,b)               (t*)arena_push_size((b), sizeof(t), alignof(t))

#define arena_push_arr_zero_align(t,b,n,a) (t*)arena_push_size_zero(b, sizeof(t)*(n), a)
#define arena_push_arr_zero(t,b,n)         (t*)arena_push_size_zero(b, sizeof(t)*(n),alignof(t))
#define arena_push_zero_align(t,b,a)       (t*)arena_push_size_zero(b, sizeof(t), a)
#define arena_push_zero(t,b)               (t*)arena_push_size_zero(b, sizeof(t), alignof(t))

static arena_marker_t arena_mark(arena_t* a);
static void arena_revert(arena_marker_t marker);

static void arena_calc_push(arena_calc_t* c, usz_t size, usz_t alignment);
static void arena_calc_clear(arena_calc_t* c);
static usz_t arena_calc_get_result(arena_calc_t* c);

# define __arena_set_revert_point(a,l) \
  auto _arena_marker_##l = arena_mark(a); \
defer{arena_revert(_arena_marker_##l);};
# define _arena_set_revert_point(a,l) __arena_set_revert_point(a,l)
# define arena_set_revert_point(arena) _arena_set_revert_point(arena, __LINE__) 


//
// MARK:(Garena)
//
static void  garena_clear(garena_t* ga);
static void  garena_init(garena_t* ga, u8_t* memory, usz_t cap);
static void* garena_push_size(garena_t* ga, usz_t size);
static void  garena_free(garena_t* ga, void* block);
#define garena_push(t,b) (t*)garena_push_size((b), sizeof(t))
#define garena_push_arr(t,b,n) (t*)garena_push_size((b), sizeof(t) * n)

//
// MARK:(TTF)
//

static b32_t ttf_read(ttf_t* ttf, buffer_t ttf_contents);

static u32_t ttf_get_glyph_index(const ttf_t* ttf, u32_t codepoint);
// returns 0 for invalid codepoints

static void ttf_get_glyph_horizontal_metrics(const ttf_t* ttf, u32_t glyph_index, s16_t* advance_width, s16_t* left_side_bearing);

static void ttf_get_glyph_vertical_metrics(const ttf_t* ttf, s16_t* ascent, s16_t* descent, s16_t* line_gap);
//you should advance the vertical position by "*ascent - *descent + *lineGap"

static f32_t ttf_get_scale_for_pixel_height(const ttf_t* ttf, f32_t pixel_height);
// This returns the 'scale factor' you need to apply to the font's coordinates
// (box, glyphs, etc) to scale it to a font height equals to pixel_height


static u32_t* ttf_rasterize_glyph(const ttf_t* ttf, u32_t glyph_index, f32_t scale, u32_t* out_w, u32_t* out_h, arena_t* allocator);
// Returns array of u32_t that represents 4 byte rgba_t pixels where the glyph is white and the background is transparent

static s32_t ttf_get_glyph_kerning(const ttf_t* ttf, u32_t glyph_index_1, u32_t glyph_index_2);
static b32_t ttf_get_glyph_box(const ttf_t* ttf, u32_t glyph_index, s32_t* x0, s32_t* y0, s32_t* x1, s32_t* y1);
static void ttf_get_glyph_bitmap_box(const ttf_t* ttf, u32_t glyph_index, f32_t scale, s32_t* x0, s32_t* y0, s32_t* x1, s32_t* y1);

//
// MARK:(PNG)
//
static b32_t     png_read(png_t* png, buffer_t png_contents);
static u32_t*    png_rasterize(png_t* png, u32_t* out_w, u32_t* out_h, arena_t* arena); 
static buffer_t  png_write(u8_t* pixels, u32_t width, u32_t height, arena_t* arena);

// 
// MARK:(RectPack)
//
static b32_t rp_pack(
    rp_rect_t* rects, 
    u32_t rect_count, 
    u32_t padding,
    u32_t total_width,
    u32_t total_height,
    rp_sort_type_t sort_type,
    arena_t* allocator);



//
// MARK:(Clex)
//
static b32_t clex_tokenizer_init(clex_tokenizer_t* t, buffer_t buffer);
static clex_token_t clex_next_token(clex_tokenizer_t* t);

//
// MARK:(OS)
//

// The memory returned is:
// - Not shared by other threads
struct os_memory_t {
  void* data;
  usz_t size;
};

static b32_t os_memory_allocate(os_memory_t* blk, usz_t size);
static b32_t os_memory_free(os_memory_t* blk);

//
//
// Implementation
//
//

static b32_t buffer_is_valid(buffer_t buffer) {
  return buffer.data != nullptr;
}




#if OS_WINDOWS
//
// Windows implementation
//
#include <windows.h>
#undef near
#undef far

static b32_t 
os_memory_allocate(os_memory_t* blk, usz_t size) {
  void* data = (u8_t*)VirtualAllocEx(
      GetCurrentProcess(),
      0, 
      size,
      MEM_RESERVE | MEM_COMMIT, 
      PAGE_READWRITE);
  if (!data) return false;

  blk->data = data;
  blk->size = size;

  return true;
}

static b32_t 
os_memory_free(os_memory_t* blk) {
  if (!VirtualFree(blk->data, 0, MEM_RELEASE)) {
    return false;
  }
  blk->data = nullptr;
  blk->size = 0;

  return true;
}


#else // OS_WINDOWS

// Non-windows (mac and linux?) implementation
// #include <stdlib.h>
#include <sys/mman.h> // mmap, munmap

static b32_t
os_memory_allocate(os_memory_t* blk, usz_t size) {
  void* data = (u8_t*)mmap(
      0, 
      size, 
      PROT_READ | PROT_WRITE, 
      MAP_PRIVATE | MAP_ANONYMOUS,
      -1, 
      0);

  if (!data) return false;

  blk->data = data;
  blk->size = size;

  return true;
}

static b32_t 
os_memory_free(os_memory_t* blk) {
  if (munmap(blk->data, blk->size) < 0)
    return false;
  blk->data = nullptr;
  blk->data = 0;

  return true;
}

#endif // OS_WINDOWS


#if FOOLISH // FOOLISH
#include <stdlib.h>
#include <stdio.h>
//
// MARK:(Foolish)
//
// These are foolish methods that should ideally not make 
// it into any SERIOUS projects. They are meant to be dumb
// but convienient for trying stuff out.
//
// TODO: Make this serious

static void* 
foolish_allocate_memory(usz_t size) {
  return malloc(size);
}

static void
foolish_free_memory(void* mem) {
  free(mem);
}

static arena_t 
foolish_allocate_arena(usz_t size) {
  arena_t ret = {};
  arena_init(&ret, foolish_allocate_memory(size), size);
  return ret;
}

static b32_t
foolish_write_buffer_to_file(const char* filename, buffer_t buffer) {
  FILE *file = fopen(filename, "wb");
  if (!file) return false;
  defer { fclose(file); };
  
  fwrite(buffer.data, 1, buffer.size, file);
  return true;
}

static buffer_t
foolish_read_file_into_buffer(const char* filename, b32_t null_terminate = false) {
  FILE *file = fopen(filename, "rb");
  if (!file) return buffer_set(0,0);
  defer { fclose(file); };

  fseek(file, 0, SEEK_END);
  usz_t file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  buffer_t ret;
  ret.size = file_size + null_terminate; // lol
  ret.data = (u8_t*)os_memory_allocate(ret.size);

  usz_t read_amount = fread(ret.data, 1, file_size, file);
  if(read_amount != file_size) return buffer_set(0,0);

  if (null_terminate) ret.data[file_size] = 0;  

  return ret;
}

static void
foolish_free_buffer(buffer_t buffer) {
  os_memory_free(buffer.data);
}

#endif // FOOLISH

enum _json_token_type_t {
  _JSON_TOKEN_TYPE_UNKNOWN,
  _JSON_TOKEN_TYPE_COLON,
  _JSON_TOKEN_TYPE_OPEN_BRACKET,
  _JSON_TOKEN_TYPE_CLOSE_BRACKET,
  _JSON_TOKEN_TYPE_OPEN_BRACE,
  _JSON_TOKEN_TYPE_CLOSE_BRACE,
  _JSON_TOKEN_TYPE_COMMA,
  
  _JSON_TOKEN_TYPE_STRING,
  _JSON_TOKEN_TYPE_NULL,
#if 0
  _JSON_TOKEN_TYPE_UNSIGNED_INTEGER,
  _JSON_TOKEN_TYPE_FLOATING_POINT,
  _JSON_TOKEN_TYPE_SIGNED_INTEGER,
#endif
  _JSON_TOKEN_TYPE_NUMBER,
#if 0
  _JSON_TOKEN_TYPE_BOOLEAN,
#endif
  _JSON_TOKEN_TYPE_TRUE,
  _JSON_TOKEN_TYPE_FALSE,
  
  _JSON_TOKEN_TYPE_EOF,
};

enum  _json_expect_type_t {
  _JSON_EXPECT_TYPE_OPEN,
  _JSON_EXPECT_TYPE_KEY_OR_CLOSE,
  _JSON_EXPECT_TYPE_VALUE,
  _JSON_EXPECT_TYPE_COMMA_OR_CLOSE,
  _JSON_EXPECT_TYPE_COLON,
};

struct _json_entry_t {
  json_key_t key;
  json_value_t value;

  // For simplified BST data structure.
  _json_entry_t* left;
  _json_entry_t* right;
};

struct _json_token_t {
  _json_token_type_t type;
  u8_t* at;
  usz_t count;
};

static b32_t _json_parse_object(json_object_t*, json_t* t, arena_t* ba);
static b32_t _json_set_value_based_on_token(json_t* t, json_value_t* value,  _json_token_t token, arena_t* ba);


static void
_json_append_array(json_array_t* arr, json_array_node_t* node) {
  sll_append(arr->head, arr->tail, node); 
}


static void
_json_eat_ignorables(json_t* t) {
  for (;;) {
    if(is_whitespace(t->text.e[t->at])) {
      ++t->at;
    }
    else if(t->text.e[t->at] == '/' && t->text.e[t->at+1] == '/') {
      t->at += 2;
      while(t->text.e[t->at] != '\n' && t->text.e[t->at] != '\r') {
        ++t->at;
      }
    }
    else if(t->text.e[t->at] == '/' && t->text.e[t->at+1] == '*') {
      t->at += 2;
      
    }
    else {
      break;
    }
  }
}

static _json_token_t
_json_next_token(json_t* t) {
  _json_eat_ignorables(t);
  
  _json_token_t ret = {};
  ret.at = t->text.e + t->at;
  ret.type = _JSON_TOKEN_TYPE_UNKNOWN;
  
  switch(t->text.e[t->at]) {
    case '\0': {
      ret.type = _JSON_TOKEN_TYPE_EOF; 
      ret.count = 1;
      ++t->at;
    } break;
    case '[': {
      ret.type = _JSON_TOKEN_TYPE_OPEN_BRACKET; 
      ret.count = 1;
      ++t->at;
    } break;
    case ']': {
      ret.type = _JSON_TOKEN_TYPE_CLOSE_BRACKET; 
      ret.count = 1;
      ++t->at;
    } break;
    case '{': {
      ret.type = _JSON_TOKEN_TYPE_OPEN_BRACE; 
      ret.count = 1;
      ++t->at;
    } break;
    case '}': {
      ret.type = _JSON_TOKEN_TYPE_CLOSE_BRACE; 
      ret.count = 1;
      ++t->at;
    } break;
    case ',': {
      ret.type = _JSON_TOKEN_TYPE_COMMA;
      ret.count = 1;
      ++t->at;
    } break;
    case ':': { 
      ret.type = _JSON_TOKEN_TYPE_COLON; 
      ret.count = 1;
      ++t->at;
    } break;
    case 't':{
      if(t->text.count - t->at >= 4 &&
         t->text.e[t->at+1] == 'r' && 
         t->text.e[t->at+2] == 'u' && 
         t->text.e[t->at+3] == 'e')
      {
        ret.type = _JSON_TOKEN_TYPE_TRUE;
        ret.count = 4;
        t->at += 4;
      }
      else {
        ++t->at;
      }
    } break;
    case 'f': {
      if(t->text.count - t->at >= 5 &&
         t->text.e[t->at+1] == 'a' && 
         t->text.e[t->at+2] == 'l' && 
         t->text.e[t->at+3] == 's' &&
         t->text.e[t->at+4] == 'e')
      {
        ret.type = _JSON_TOKEN_TYPE_FALSE;
        ret.count = 5;
        t->at += 5;
      }
      else {
        ++t->at;
      }

    } break;
    case 'n': {
      if ((t->text.count - t->at) >= 4 && 
           t->text.e[t->at+1] == 'u' && 
           t->text.e[t->at+2] == 'l' && 
           t->text.e[t->at+3] == 'l')
      {
        ret.type = _JSON_TOKEN_TYPE_NULL;
        t->at += 4;
        ret.count = 4;
      }
      else {
        ++t->at;
      }

    } break;

    case '"': // strings
    {
      ++t->at; // move past the initial double quote
      ret.at = t->text.e + t->at;
      while(t->text.e[t->at] &&
            t->text.e[t->at] != '"') 
      {
        if(t->text.e[t->at] == '\\' && 
           t->text.e[t->at+1]) 
        {
          ++t->at;
          ++ret.count;
        }
        ++t->at;
        ++ret.count;
      }
      ret.type = _JSON_TOKEN_TYPE_STRING;
      ++t->at;
    } break;
    
    default: {
      // Unsigned integer
      if (is_digit(t->text.e[t->at])) {
        while(true)
        {
          // TODO check for double dots?
          if (t->text.e[t->at] == '.') {
          }
          else if (!is_digit(t->text.e[t->at])) {
            break;
          }
          ++ret.count;
          ++t->at;
        }
        ret.type = _JSON_TOKEN_TYPE_NUMBER;
      }
      
      // Signed integer
      else if ((t->text.count - t->at) >= 2 && 
                t->text.e[t->at] == '-' && 
                is_digit(t->text.e[t->at+1])) 
      {
        ++t->at; // keeps the negative sign
        while(true)
        {
          // TODO check for double dots?
          if (t->text.e[t->at] == '.') {
          }
          else if (!is_digit(t->text.e[t->at])) {
            break;
          }
          ++ret.count;
          ++t->at;
        }
        ret.type = _JSON_TOKEN_TYPE_NUMBER;
      }      
      else {
        ++t->at;
      }
    }
  }

  return ret;

}



static b32_t
_json_insert_entry(json_t* t, _json_entry_t** entry, _json_entry_t* new_entry) {
  if ((*entry) == nullptr) {
    (*entry) = new_entry; 
    return true;
  }
  else {
    _json_entry_t* itr = (*entry);
    while(itr != nullptr) {
      st8_t lhs = st8_set(itr->key.at, itr->key.count);
      st8_t rhs = st8_set(new_entry->key.at, new_entry->key.count);

      smi_t cmp = st8_compare_lexographically(lhs, rhs);
      if (cmp > 0) {
        if (itr->left == nullptr) {
          itr->left = new_entry;
          return true;
        }
        else {
          itr = itr->left;
        }
      }
      else if (cmp < 0) {
        if (itr->right == nullptr) {
          itr->right = new_entry;
          return true;
        }
        else {
          itr = itr->right;
        }
      }
      else {
        return false;
      }
    }
    return false;
  }
}

static b32_t
_json_parse_array(json_array_t* arr, json_t* t, arena_t* ba) {
  b32_t is_done = false;
  u32_t expect_type = 0;
  b32_t error = false;
  

  while(!is_done) {
    _json_token_t token = _json_next_token(t);

    if (expect_type == 0) {
      json_value_t v = {};
      if(_json_set_value_based_on_token(t, &v, token, ba)) {
        json_array_node_t* array_node = arena_push(json_array_node_t, ba); 
        if (array_node) {
          array_node->value = v;
          _json_append_array(arr, array_node);

        }
        else {
          is_done = true;
          error = true;
        }
      }
      else {
        is_done = true;
        error = true;
      }
      expect_type = 1;
    }
    else if (expect_type == 1) {
      if (token.type == _JSON_TOKEN_TYPE_COMMA) {
        expect_type = 0;
      }
      else if (token.type == _JSON_TOKEN_TYPE_CLOSE_BRACKET) {
        is_done = true;
      }
      else {
        is_done = true;
        error = true;
      }
    }
  }
 
  return !error;
}

static b32_t 
_json_set_value_based_on_token(json_t* t, json_value_t* value, _json_token_t token, arena_t* ba) 
{
  b32_t error = false;
  if (token.type == _JSON_TOKEN_TYPE_NUMBER) {
    value->type = JSON_VALUE_TYPE_NUMBER;
    value->element.at = token.at;
    value->element.count = token.count;
  }
  else if(token.type == _JSON_TOKEN_TYPE_STRING) 
  {
    value->type = JSON_VALUE_TYPE_STRING;
    value->element.at = token.at;
    value->element.count = token.count;
  }
  else if (token.type == _JSON_TOKEN_TYPE_NULL) {
    value->type = JSON_VALUE_TYPE_NULL;
    value->element.at = token.at;
    value->element.count = token.count;
  }
  else if (token.type == _JSON_TOKEN_TYPE_FALSE) {
    value->type = JSON_VALUE_TYPE_FALSE;
    value->element.at = token.at;
    value->element.count = token.count;
  }
  else if (token.type == _JSON_TOKEN_TYPE_TRUE) {
    value->type = JSON_VALUE_TYPE_TRUE;
    value->element.at = token.at;
    value->element.count = token.count;
  }
  else if (token.type == _JSON_TOKEN_TYPE_OPEN_BRACE) { // Parse json object
    if (_json_parse_object(&value->object, t, ba)) {
      value->type = JSON_VALUE_TYPE_OBJECT;
    }
    else {
      error = true; 
    }
  }
  else if (token.type == _JSON_TOKEN_TYPE_OPEN_BRACKET) {
    if (_json_parse_array(&value->array, t, ba)) {
      value->type = JSON_VALUE_TYPE_ARRAY;
    }
    else {
      error = true;
    }
  }
  return !error;
}

static b32_t  
_json_parse_object(json_object_t* obj, json_t* t, arena_t* ba) {
  _json_entry_t* entry = nullptr;
  _json_expect_type_t expect_type = _JSON_EXPECT_TYPE_KEY_OR_CLOSE; 
  b32_t is_done = false;

  _json_entry_t* current_entry = nullptr;

  while(!is_done) {
    _json_token_t token = _json_next_token(t);
    if (token.type == _JSON_TOKEN_TYPE_UNKNOWN) return false;

    switch(expect_type) {
      case _JSON_EXPECT_TYPE_KEY_OR_CLOSE: {
        if (token.type == _JSON_TOKEN_TYPE_STRING) {
          current_entry = arena_push(_json_entry_t, ba);
          if (!current_entry) {
            is_done = true;
          }
          else {
            current_entry->key.at = token.at;
            current_entry->key.count = token.count;
            expect_type = _JSON_EXPECT_TYPE_COLON;
          }
        }
        else if(token.type == _JSON_TOKEN_TYPE_CLOSE_BRACE) {
          is_done = true;
        }
        else {
          return false;
        }
      } break;
      case _JSON_EXPECT_TYPE_COLON: {
        if ( token.type == _JSON_TOKEN_TYPE_COLON ) {
          expect_type = _JSON_EXPECT_TYPE_VALUE; 
        }
        else {
          return false;
        }
      } break;
      case _JSON_EXPECT_TYPE_VALUE: {
        if (_json_set_value_based_on_token(t, &current_entry->value, token, ba)) {
          _json_insert_entry(t, &entry, current_entry);
        }
        else {
          return false;
        }

        expect_type = _JSON_EXPECT_TYPE_COMMA_OR_CLOSE;

    
      } break;
      case _JSON_EXPECT_TYPE_COMMA_OR_CLOSE:{
        if (token.type == _JSON_TOKEN_TYPE_COMMA) {
          expect_type = _JSON_EXPECT_TYPE_KEY_OR_CLOSE;
        }
        if (token.type == _JSON_TOKEN_TYPE_CLOSE_BRACE) {
          is_done = true; 
        }
      } break;
      case _JSON_EXPECT_TYPE_OPEN: {} break;
    }
  }
  
  obj->head = entry; 
  return true;
}

#if JSON_DEBUG 
#include <stdio.h>

static u32_t sccount = 0;
static void _json_print_entries_in_order(json_t* t,  _json_entry_t* entry);

static void
_json_print_token(json_t* t, _json_token_t token)  {
#if 0
  switch(token.type) {
    case _JSON_TOKEN_TYPE_OPEN_PAREN: 
    case _JSON_TOKEN_TYPE_CLOSE_PAREN:
    case _JSON_TOKEN_TYPE_SEMICOLON:
    case _JSON_TOKEN_TYPE_COLON:
    case _JSON_TOKEN_TYPE_COMMA:
    case _JSON_TOKEN_TYPE_OPEN_BRACKET:
    case _JSON_TOKEN_TYPE_CLOSE_BRACKET:
    case _JSON_TOKEN_TYPE_OPEN_BRACE:
    case _JSON_TOKEN_TYPE_CLOSE_BRACE:{
      printf("token: ");
    } break;
    case _JSON_TOKEN_TYPE_IDENTIFIER: {
      printf("identifier: ");
    } break;
    case _JSON_TOKEN_TYPE_STRING: {
      printf("string: ");
    } break;
    case _JSON_TOKEN_TYPE_EOF: {
      printf("eof");
    } break;
    case _JSON_TOKEN_TYPE_UNKNOWN: {
      printf("unknown: ");
    } break;
    
  }
#endif 
  for(umi_t i = 0; i < token.count; ++i) {
    printf("%c", token.at[i]);
  }
}

static void
_json_print_value(json_t* t, json_value_t* value) {
  switch(value->type) {
    case JSON_VALUE_TYPE_TRUE: 
    case JSON_VALUE_TYPE_FALSE:
    case JSON_VALUE_TYPE_STRING: 
    case JSON_VALUE_TYPE_NUMBER: 
    case JSON_VALUE_TYPE_NULL: 
    {
      for(u32_t _i = 0;
          _i < value->element.count;
          ++_i)
      {
        printf("%c", value->element.at[_i]);
      }
    } break;

    case JSON_VALUE_TYPE_OBJECT: 
    {
      sccount++;
      printf("{\n");
      _json_print_entries_in_order(t, value->object.head);
      sccount--;
      for(u32_t _i = 0; _i < sccount; ++_i) 
        printf(" ");
      printf("}");
    } break;
    case JSON_VALUE_TYPE_ARRAY: 
    {
      printf("[");
      for (json_array_node_t* itr = value->array.head;
          itr != nullptr;
          itr = itr->next) 
      {
        _json_print_value(t, &itr->value);
        if (itr->next != nullptr)
          printf(",");
      }
      printf("]");
           

    } break;
  }
}

static void 
_json_print_entries_in_order(json_t* t, _json_entry_t* entry) 
{
  if (entry == nullptr) 
  {
    return;
  }
  else 
  {
    _json_print_entries_in_order(t, entry->left);

    for(u32_t i = 0; i < sccount; ++i) 
    {
      printf(" ");
    }

    for (u32_t i = 0; i < entry->key.count; ++i) 
    {
      printf("%c", entry->key.at[i]);
    }
    printf(":");

    _json_print_value(t, &entry->value);
    
    printf("\n");

    _json_print_entries_in_order(t, entry->right);
  }
}
#endif // JSON_DEBUG

static _json_entry_t* 
_json_get(json_object_t* json_object, st8_t key) {
  _json_entry_t* node = json_object->head;
  while(node) {
    st8_t lhs = st8_set(node->key.at, node->key.count);
    st8_t rhs = st8_set(key.e, key.count);
    smi_t cmp = st8_compare_lexographically(lhs, rhs); 
    if (cmp > 0) {
      node = node->left;
    }
    else if (cmp < 0) {
      node = node->right;
    }
    else {
      return node;
    }
  }

  return node;
}

static json_value_t* 
json_get_value(json_object_t* json_object, st8_t key) {
  _json_entry_t* entry = _json_get(json_object, key);
  if (!entry) return nullptr;
  return &entry->value;
}

static b32_t 
json_is_true(json_value_t* val) 
{
  return val->type == JSON_VALUE_TYPE_TRUE;
}

static b32_t 
json_is_false(json_value_t* val) 
{
  return val->type == JSON_VALUE_TYPE_FALSE;
}

static b32_t 
json_is_null(json_value_t* val) {
  return val->type == JSON_VALUE_TYPE_NULL;
}

static b32_t 
json_is_string(json_value_t* val) {
  return val->type == JSON_VALUE_TYPE_STRING;
}

static b32_t 
json_is_number(json_value_t* val) {
  return val->type == JSON_VALUE_TYPE_NUMBER;
}

static b32_t json_is_array(json_value_t* val) {
  return val->type == JSON_VALUE_TYPE_ARRAY;
}
static b32_t json_is_object(json_value_t* val) {
  return val->type == JSON_VALUE_TYPE_OBJECT;
}
static b32_t json_is_element(json_value_t* val) {
  return !json_is_object(val) && !json_is_array(val);
}

static json_element_t* json_get_element(json_value_t* val) {
  return json_is_element(val) ? &val->element : nullptr;
}
static json_array_t* json_get_array(json_value_t* val) {
  return json_is_array(val) ? &val->array : nullptr;
}
static json_object_t* json_get_object(json_value_t* val) {
  return json_is_object(val) ? &val->object : nullptr;
}


static json_object_t*
json_read(
    json_t* j, 
    u8_t* json_string, 
    u32_t json_string_size, 
    arena_t* ba) 
{
  j->text = st8_set(json_string, json_string_size);
  j->at = 0;
  _json_token_t token = _json_next_token(j);
  if (token.type != _JSON_TOKEN_TYPE_OPEN_BRACE) return nullptr;
  if (!_json_parse_object(&j->root, j, ba)) return nullptr;

  // print the node in order
#if JSON_DEBUG
  printf("=== Printing json tree in-order ===\n");
  _json_print_entries_in_order(j, j->root.head);
#endif //JSON_DEBUG
  return &j->root;
}


static void
_clex_eat_ignorables(clex_tokenizer_t* t) {
  for (;;) {
    if(is_whitespace(t->text.data[t->at])) {
      ++t->at;
    }
    else if(t->text.data[t->at] == '/' && t->text.data[t->at+1] == '/')  // line comments
    {
      while(t->text.data[t->at] != '\n') {
        ++t->at;
      }
    }
    else if(t->text.data[t->at] == '/' && t->text.data[t->at+1] == '*')  // block comments
    {
      t->at += 3;
      while(t->text.data[t->at] != '*' && t->text.data[t->at+1] != '/') {
        ++t->at;
      }

    }
    else {
      break;
    }
  }

}

static b32_t 
_clex_is_accepted_character_for_number(char c) {
  return c == '-' || c == '.' ||
    c == 'b' || c == 'x' || c == 'l' || c == 'f' || c == 'p' || c == 'e' ||
    c == 'B' || c == 'X' || c == 'L' || c == 'F' || c == 'P' || c == 'E';
}


static b32_t
_clex_compare_token_with_string(clex_tokenizer_t* t, clex_token_t token, st8_t str) {
  if( str.count != (token.ope - token.begin)) {
    return false;
  }

  for(u32_t i = 0; i < str.count; ++i) {
    if (str.e[i] != t->text.data[token.begin+i]) {
      return false;
    }
  }

  return true;
}


static b32_t
clex_tokenizer_init(clex_tokenizer_t* t, buffer_t buffer) {
  t->text = buffer; 
  t->at = 0;
  return !!t->text.size;
}

static b32_t
_clex_is_keyword(clex_tokenizer_t* t, clex_token_t token) {
  static st8_t keywords[] = {
    st8_from_lit("if"),
    st8_from_lit("else"),
    st8_from_lit("switch"),
    st8_from_lit("case"),
    st8_from_lit("default"),
    st8_from_lit("while"),
    st8_from_lit("for"),
    st8_from_lit("if"),
    st8_from_lit("operator"),
    st8_from_lit("auto"),
    st8_from_lit("goto"),
    st8_from_lit("return"),
  };
  for_arr(i, keywords) {
    if (_clex_compare_token_with_string(t, token, keywords[i]))
    {
      return true;
    }
  }
  return false;
}


static clex_token_t
clex_next_token(clex_tokenizer_t* t) {
  _clex_eat_ignorables(t);

  clex_token_t ret = {};
  ret.begin = t->at;
  ret.ope = t->at + 1;

  if (t->text.data[t->at] == 0) {
    ret.type = CLEX_TOKEN_TYPE_EOF; 
    ++t->at;
  }
  else if (t->text.data[t->at] == '(') {
    ret.type = CLEX_TOKEN_TYPE_OPEN_PAREN; 
    ++t->at;
  }
  else if (t->text.data[t->at] == '?') {
    ret.type = CLEX_TOKEN_TYPE_QUESTION; 
    ++t->at;
  }
  else if (t->text.data[t->at] == ')') {
    ret.type = CLEX_TOKEN_TYPE_CLOSE_PAREN; 
    ++t->at;
  }
  else if (t->text.data[t->at] == '[') {
    ret.type = CLEX_TOKEN_TYPE_OPEN_BRACKET; 
    ++t->at;
  } 
  else if (t->text.data[t->at] == ']') {
    ret.type = CLEX_TOKEN_TYPE_CLOSE_BRACKET; 
    ++t->at;
  }
  else if (t->text.data[t->at] == '{') {
    ret.type = CLEX_TOKEN_TYPE_OPEN_BRACE; 
    ++t->at;
  } 
  else if (t->text.data[t->at] == '}') {
    ret.type = CLEX_TOKEN_TYPE_CLOSE_BRACE; 
    ++t->at;
  } 
  else if (t->text.data[t->at] == ')') { 
    ret.type = CLEX_TOKEN_TYPE_COLON; 
    ++t->at;
  } 
  else if (t->text.data[t->at] == ';') {
    ret.type = CLEX_TOKEN_TYPE_SEMICOLON; 
    ++t->at;
  }
  else if (t->text.data[t->at] == '+') {
    ret.type = CLEX_TOKEN_TYPE_PLUS;
    ++t->at;
    if (t->text.data[t->at] == '+') { // ++
      ret.type = CLEX_TOKEN_TYPE_PLUS_PLUS;
      ret.ope = ++t->at;
    }
    else if (t->text.data[t->at] == '=') { // +=
      ret.type = CLEX_TOKEN_TYPE_PLUS_EQUAL;
      ret.ope = ++t->at;
    }
  }
  else if (t->text.data[t->at] == '-') {
    ret.type = CLEX_TOKEN_TYPE_MINUS;
    ++t->at;
    if (t->text.data[t->at] == '-') { // --
      ret.type = CLEX_TOKEN_TYPE_MINUS_MINUS;
      ret.ope = ++t->at;
    }
    else if (t->text.data[t->at] == '=') { // -=
      ret.type = CLEX_TOKEN_TYPE_MINUS_EQUAL;
      ret.ope = ++t->at;
    }
    else if (t->text.data[t->at] == '>') { // ->
      ret.type = CLEX_TOKEN_TYPE_ARROW;
      ret.ope = ++t->at;
    }
    else if (is_digit(t->text.data[t->at])) // negative number related literals
    {    
      ret.type = CLEX_TOKEN_TYPE_NUMBER;
      while(is_digit(t->text.data[t->at]) ||
          _clex_is_accepted_character_for_number(t->text.data[t->at]))
      {
        ++t->at;
      }
      ret.ope = t->at;
    }
  }
  else if (t->text.data[t->at] == '=') {
    ret.type = CLEX_TOKEN_TYPE_EQUAL;
    ++t->at;

    if (t->text.data[t->at] == '=') { // ==
      ret.type = CLEX_TOKEN_TYPE_EQUAL_EQUAL;
      ret.ope = ++t->at;
    }
  }

  else if (t->text.data[t->at] == '>') {
    ret.type = CLEX_TOKEN_TYPE_GREATER;
    ++t->at;

    if (t->text.data[t->at] == '=') { // >=
      ret.type = CLEX_TOKEN_TYPE_GREATER_EQUAL;
      ret.ope = ++t->at;
    }
    else if (t->text.data[t->at] == '>') { // >>
      ret.type = CLEX_TOKEN_TYPE_GREATER_GREATER;
      ret.ope = ++t->at;
    }
  }

  else if (t->text.data[t->at] == '<') {
    ret.type = CLEX_TOKEN_TYPE_LESSER;
    ++t->at;

    if (t->text.data[t->at] == '=') { // >=
      ret.type = CLEX_TOKEN_TYPE_LESSER_EQUAL;
      ret.ope = ++t->at;
    }
    else if (t->text.data[t->at] == '<') { // <<
      ret.type = CLEX_TOKEN_TYPE_LESSER_LESSER;
      ret.ope = ++t->at;
    }
  }
  else if (t->text.data[t->at] == '|') {
    ret.type = CLEX_TOKEN_TYPE_OR;
    ++t->at;

    if (t->text.data[t->at] == '|') { // ||
      ret.type = CLEX_TOKEN_TYPE_OR_OR;
      ret.ope = ++t->at;
    }
    else if (t->text.data[t->at] == '=') { // |=
      ret.type = CLEX_TOKEN_TYPE_OR_EQUAL;
      ret.ope = ++t->at;
    }
  }

  else if (t->text.data[t->at] == ':') {
    ret.type = CLEX_TOKEN_TYPE_COLON;
    ++t->at;

    if (t->text.data[t->at] == ':') { // ::
      ret.type = CLEX_TOKEN_TYPE_SCOPE;
      ret.ope = ++t->at;
    }
  }
  else if (t->text.data[t->at] == '&') {
    ret.type = CLEX_TOKEN_TYPE_AND;
    ++t->at;

    if (t->text.data[t->at] == '&') { // &&
      ret.type = CLEX_TOKEN_TYPE_AND_AND;
      ret.ope = ++t->at;
    }
    else if (t->text.data[t->at] == '=') { // &=
      ret.type = CLEX_TOKEN_TYPE_AND_EQUAL;
      ret.ope = ++t->at;
    }
  }
  else if (t->text.data[t->at] == '*') {
    ret.type = CLEX_TOKEN_TYPE_STAR;
    ++t->at;

    if (t->text.data[t->at] == '=') { // *=
      ret.type = CLEX_TOKEN_TYPE_STAR_EQUAL;
      ret.ope = ++t->at;
    }
  }

  else if (t->text.data[t->at] == '/') {
    ret.type = CLEX_TOKEN_TYPE_SLASH;
    ++t->at;

    if (t->text.data[t->at] == '=') { // /=
      ret.type = CLEX_TOKEN_TYPE_SLASH_EQUAL;
      ret.ope = ++t->at;
    }
  }
  else if (t->text.data[t->at] == '%') {
    ret.type = CLEX_TOKEN_TYPE_PERCENT;
    ++t->at;

    if (t->text.data[t->at] == '=') { // %=
      ret.type = CLEX_TOKEN_TYPE_PERCENT_EQUAL;
      ret.ope = ++t->at;
    }
  }
  else if (t->text.data[t->at] == '^') {
    ret.type = CLEX_TOKEN_TYPE_XOR;
    ++t->at;
    if (t->text.data[t->at] == '=') { // ^=
      ret.type = CLEX_TOKEN_TYPE_XOR_EQUAL;
      ret.ope = ++t->at;
    }
  }
  else if (t->text.data[t->at] == '~') {
    ret.type = CLEX_TOKEN_TYPE_BITWISE_NOT;
    ++t->at;
  }
  else if (t->text.data[t->at] == '!') {
    ret.type = CLEX_TOKEN_TYPE_LOGICAL_NOT;
    ++t->at;
  }
  else if (t->text.data[t->at] == '.')
  {
    ret.type = CLEX_TOKEN_TYPE_DOT; 
    ++t->at;

    if (is_digit(t->text.data[t->at])) // positive number related literals
    {    
      ret.type = CLEX_TOKEN_TYPE_NUMBER;
      while(is_digit(t->text.data[t->at]) ||
          _clex_is_accepted_character_for_number(t->text.data[t->at]))
      {
        ++t->at;
      }
      ret.ope = t->at;
    }
  }

  else if (t->text.data[t->at] == '#') {
    b32_t continue_to_next_line = false;

    ret.type = CLEX_TOKEN_TYPE_MACRO;
    ++t->at;
    while(t->text.data[t->at] != 0) 
    {

      if (t->text.data[t->at] == '\\') {
        continue_to_next_line = true;
      }

      if (t->text.data[t->at] == '\n') 
      {
        if (continue_to_next_line) {
          continue_to_next_line = false;
        }
        else {
          break;
        }
      }

      ++t->at;
    }
    ret.ope = t->at;
  }
  else if (t->text.data[t->at] == '"') // string literals
  {
    ++t->at;
    ret.begin = t->at;
    while(t->text.data[t->at] != '"') 
    {
      if(t->text.data[t->at] == '\\' && 
          t->text.data[t->at+1]) 
      {
        ++t->at;
      }
      ++t->at;
    }
    ret.type = CLEX_TOKEN_TYPE_STRING;
    ret.ope = t->at;
    ++t->at;
  }

  else if (is_alpha(t->text.data[t->at]) || t->text.data[t->at] == '_') 
  {
    while(is_alpha(t->text.data[t->at]) ||
        is_digit(t->text.data[t->at]) ||
        t->text.data[t->at] == '_') 
    {
      ++t->at;
    }
    ret.ope = t->at;

    if (_clex_is_keyword(t, ret)) {
      ret.type = CLEX_TOKEN_TYPE_KEYWORD;
    }

    else {
      ret.type = CLEX_TOKEN_TYPE_IDENTIFIER;
    } 
  }

  else if (is_digit(t->text.data[t->at])) // positive number related literals
  {    
    ret.type = CLEX_TOKEN_TYPE_NUMBER;
    while(is_digit(t->text.data[t->at]) ||
        _clex_is_accepted_character_for_number(t->text.data[t->at]))
    {
      ++t->at;
    }
    ret.ope = t->at;
  }

  else if (t->text.data[t->at] == '\'') // char literals
  {
    ++t->at;
    ret.begin = t->at;
    while(t->text.data[t->at] != '\'') {
      ++t->at;
    }
    ret.type = CLEX_TOKEN_TYPE_CHAR;
    ret.ope = t->at;
    ++t->at;
  }

  else {
    ret.type = CLEX_TOKEN_TYPE_UNKNOWN;
    ++t->at;
  }


  return ret;
}



static buffer_t 
buffer_set(u8_t* mem, usz_t size) {
  buffer_t ret;
  ret.data = mem;
  ret.size = size;
  return ret;
}

static f32_t _F32_INFINITY() {
  // NOTE(Momo): Use 'type pruning'
  // Infinity is when bits 1-8 are on
  union { f32_t f; u32_t u; } ret = {};
  ret.u = 0x7f800000;

  return ret.f;

}

static f32_t 
_F32_NEG_INFINITY() {
  // NOTE(Momo): Use 'type pruning'
  // Infinity is when bits 1-8 are on
  // Negative is when bit 0 is on
  union { f32_t f; u32_t u; } ret = {};
  ret.u = 0xff800000;

  return ret.f;	
}

static f32_t
_F32_NAN() {
  // NOTE(Momo): Use 'type pruning'
  // NAN is when bits 1-11 and 1 other bit is on
  // In this case, we will just turn on all bits
  union { f32_t f; u32_t u; } ret = {};
  ret.u = 0xFFFFFFFF;
  return ret.f;
}

static f64_t
_F64_NAN() {
  // NOTE(Momo): Use 'type pruning'
  // NAN is when bits 1-11 and 1 other bit is on
  // In this case, we will just turn on all bits
  union { f64_t f; u64_t u; } ret = {};
  ret.u = 0xFFFFFFFFFFFFFFFF;
  return ret.f;
}


static f64_t 
_F64_INFINITY() {
  // NOTE(Momo): Use 'type pruning'
  // Infinity is when bits 1-11 are on
  union { f64_t f; u64_t u; } ret = {};
  ret.u = 0x7FF0000000000000;

  return ret.f;

}

static f64_t 
_F64_NEG_INFINITY() {
  // NOTE(Momo): Use 'type pruning'
  // Infinity is when bits 1-11 are on
  // Negative is when bit 0 is on
  union { f64_t f; u64_t u; } ret = {};
  ret.u = 0xFFF0000000000000;

  return ret.f;	
}

static umi_t 
ptr_to_umi(void* p) { 
  return (umi_t)((c8_t*)p - (c8_t*)0); 
}

static u8_t* 
umi_to_ptr(umi_t u) { 
  return (u8_t*)((c8_t*)0 + u);
}

static b32_t
is_whitespace(char c) {
  return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

static b32_t
is_alpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static b32_t
is_digit(char c) {
  return (c >= '0' && c <= '9');
  // this gets compiled to (uint8_t)(c - '0') <= 9 on all decent compilers
}

static f32_t 
f32_abs(f32_t x) {
  union { f32_t f; u32_t u; } val = {};
  val.f = x;
  val.u &= 0x7fffffff;  
  return val.f;
}

static f64_t
f64_abs(f64_t x) {
  union { f64_t f; u64_t u; } val = {};
  val.f = x;
  val.u &= 0x7fffffffffffffff;

  return val.f;
}

  static f32_t
f32_mod(f32_t lhs, f32_t rhs) 
{
  return fmodf(lhs, rhs);
}

  static f64_t
f64_mod(f64_t lhs, f64_t rhs) 
{
  return fmod(lhs, rhs);
}

static s8_t   
s8_abs(s8_t x) {
  s8_t y = x >> 7;
  return (x ^ y)-y;
}

static s16_t  
s16_abs(s16_t x) {
  s16_t y = x >> 15;
  return (x ^ y)-y;
}

static s32_t  
s32_abs(s32_t x) {
  s32_t y = x >> 31;
  return (x ^ y)-y;
}

static s64_t  
s64_abs(s64_t x) {
  s64_t y = x >> 63;
  return (x ^ y)-y;
}

static f32_t
f32_lerp(f32_t s, f32_t e, f32_t f) { 
  return (f32_t)(s + (e-s) * f); 
}

static f64_t 
f64_lerp(f64_t s, f64_t e, f64_t f) { 
  return (f64_t)(s + (e-s) * f); 
}

static f32_t 
f32_weight(f32_t v, f32_t min, f32_t max) { 
  return (v - min)/(max - min); 
}

static f64_t 
f64_weight(f64_t v, f64_t min, f64_t max) { 
  return (v - min)/(max - min); 
}

static f32_t 
f32_deg_to_rad(f32_t degrees) {
  return degrees * PI_32 / 180.f;
}
static f32_t 
f32_rad_to_deg(f32_t radians) {
  return radians * 180.f / PI_32;	
}

static f32_t 
f32_turns_to_radians(f32_t turns) {
  return turns * TAU_32;
}

static f64_t
f64_deg_to_rad(f64_t degrees) {
  return degrees * PI_32 / 180.0;

}
static f64_t 
f64_rad_to_deg(f64_t radians) {
  return radians * 180.0 / PI_64;
}


static f64_t 
f64_turns_to_radians(f64_t turns) {
  return turns * TAU_64;
}

static f32_t
f32_bpm_to_spb(f32_t bpm) {
  return 60.f/bpm;
}

static f64_t
f64_bpm_to_spb(f64_t bpm) {
  return 60.0/bpm;
}

static u16_t
u16_endian_swap(u16_t value) {
  return (value << 8) | (value >> 8);
}

static s16_t
s16_endian_swap(s16_t value) {
  return (value << 8) | (value >> 8);
}

static u32_t
u32_endian_swap(u32_t value) {
  return  ((value << 24) |
      ((value & 0xFF00) << 8) |
      ((value >> 8) & 0xFF00) |
      (value >> 24));

}


static u32_t 
u32_factorial(u32_t x) {
  u32_t ret = 1;
  for (u32_t i = 1; i <= x; ++i)
    ret *= i;
  return ret;
}

static u64_t 
u64_factorial(u64_t x) {
  u64_t ret = 1;
  for (u64_t i = 1; i <= x; ++i)
    ret *= i;
  return ret;
}
static f32_t 
f32_factorial(f32_t x) {
  f32_t ret = 1;
  for (f32_t i = 1; i <= x; ++i)
    ret *= i;
  return ret;
}

static f64_t 
f64_factorial(f64_t x) {
  f64_t ret = 1;
  for (f64_t i = 1; i <= x; ++i)
    ret *= i;
  return ret;
}

#if 1
#include <string.h>
static void 
copy_memory(void* dest, const void* src, usz_t size) {
  memcpy(dest, src, size);
}

static void 
zero_memory(void* dest, usz_t size) {
  memset(dest, 0, size);
}
static b32_t
is_memory_same(const void* lhs, const void* rhs, usz_t size) {
  return memcmp(lhs, rhs, size) == 0; 
}

#else
static void
copy_memory(void* dest, const void* src, usz_t size) {
  u8_t *p = (u8_t*)dest;
  const u8_t *q = (const u8_t*)src;
  while(size--) {
    *p++ = *q++;
  }
}

static void 
zero_memory(void* dest, usz_t size) {
  u8_t *p = (u8_t*)dest;
  while(size--){
    *p++ = 0;
  }
}

static b32_t
is_memory_same(const void* lhs, const void* rhs, usz_t size) {
  const u8_t *p = (const u8_t*)lhs;
  const u8_t *q = (const u8_t*)rhs;
  while(size--) {
    if (*p != *q) {
      return false;
    }
  }
  return true;

}
#endif



static void 
swap_memory(void* lhs, void* rhs, usz_t size) {
  u8_t* l = (u8_t*)lhs;
  u8_t* r = (u8_t*)rhs;

  while(size--) {
    u8_t tmp = (*l);
    *l++ = *r;
    *r++ = tmp;
  }
}

static usz_t
cstr_len(const c8_t* str) {
  usz_t count = 0;
  for(; (*str) != 0 ; ++count, ++str);
  return count;
}

  static u32_t 
cstr_to_u32(const c8_t* str)
{
  u32_t ret = 0;
  while(*str >= '0' && *str <= '9') {
    ret *= 10;
    ret += (*str - '0'); 
    ++str; 
  }
  return ret;
}

  static u32_t 
cstr_len_if(const char* str, b32_t (*pred)(char))
{
  // common strlen that counts what I care
  u32_t ret = 0;
  while(pred(*str)) 
  {
    ++str;
    ++ret;
  }

  return ret;
}
static void
cstr_copy(c8_t * dest, const c8_t* src) {
  for(; (*src) != 0 ; ++src, ++dest) {
    (*dest) = (*src);
  }
  (*dest) = 0;
}

static b32_t
cstr_compare(const c8_t* lhs, const c8_t* rhs) {
  for(; (*rhs) != 0 ; ++rhs, ++lhs) {
    if ((*lhs) != (*rhs)) {
      return false;
    }
  }
  return true;
}

static b32_t
cstr_compare_n(const c8_t* lhs, const c8_t* rhs, usz_t n) {
  while(n--) {
    if ((*lhs++) != (*rhs++)) {
      return false;
    }
  }
  return true;
}
static void
cstr_concat(c8_t* dest, const c8_t* Src) {
  // Go to the end of dest
  for (; (*dest) != 0; ++dest);
  for (; (*Src) != 0; ++Src, ++dest) {
    (*dest) = (*Src);
  }
  (*dest) = 0;
}

  static f64_t
_compute_f64(s64_t power, u64_t i, b32_t negative) 
{
  static const f64_t power_of_ten[] = {
    1e0,  1e1,  1e2,  1e3,  1e4,  1e5,  1e6,  1e7,  1e8,  1e9,  1e10, 1e11,
    1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19, 1e20, 1e21, 1e22};
#if 0
  static const u64_t mantissa_64[] = {
    0xa5ced43b7e3e9188, 0xcf42894a5dce35ea,
    0x818995ce7aa0e1b2, 0xa1ebfb4219491a1f,
    0xca66fa129f9b60a6, 0xfd00b897478238d0,
    0x9e20735e8cb16382, 0xc5a890362fddbc62,
    0xf712b443bbd52b7b, 0x9a6bb0aa55653b2d,
    0xc1069cd4eabe89f8, 0xf148440a256e2c76,
    0x96cd2a865764dbca, 0xbc807527ed3e12bc,
    0xeba09271e88d976b, 0x93445b8731587ea3,
    0xb8157268fdae9e4c, 0xe61acf033d1a45df,
    0x8fd0c16206306bab, 0xb3c4f1ba87bc8696,
    0xe0b62e2929aba83c, 0x8c71dcd9ba0b4925,
    0xaf8e5410288e1b6f, 0xdb71e91432b1a24a,
    0x892731ac9faf056e, 0xab70fe17c79ac6ca,
    0xd64d3d9db981787d, 0x85f0468293f0eb4e,
    0xa76c582338ed2621, 0xd1476e2c07286faa,
    0x82cca4db847945ca, 0xa37fce126597973c,
    0xcc5fc196fefd7d0c, 0xff77b1fcbebcdc4f,
    0x9faacf3df73609b1, 0xc795830d75038c1d,
    0xf97ae3d0d2446f25, 0x9becce62836ac577,
    0xc2e801fb244576d5, 0xf3a20279ed56d48a,
    0x9845418c345644d6, 0xbe5691ef416bd60c,
    0xedec366b11c6cb8f, 0x94b3a202eb1c3f39,
    0xb9e08a83a5e34f07, 0xe858ad248f5c22c9,
    0x91376c36d99995be, 0xb58547448ffffb2d,
    0xe2e69915b3fff9f9, 0x8dd01fad907ffc3b,
    0xb1442798f49ffb4a, 0xdd95317f31c7fa1d,
    0x8a7d3eef7f1cfc52, 0xad1c8eab5ee43b66,
    0xd863b256369d4a40, 0x873e4f75e2224e68,
    0xa90de3535aaae202, 0xd3515c2831559a83,
    0x8412d9991ed58091, 0xa5178fff668ae0b6,
    0xce5d73ff402d98e3, 0x80fa687f881c7f8e,
    0xa139029f6a239f72, 0xc987434744ac874e,
    0xfbe9141915d7a922, 0x9d71ac8fada6c9b5,
    0xc4ce17b399107c22, 0xf6019da07f549b2b,
    0x99c102844f94e0fb, 0xc0314325637a1939,
    0xf03d93eebc589f88, 0x96267c7535b763b5,
    0xbbb01b9283253ca2, 0xea9c227723ee8bcb,
    0x92a1958a7675175f, 0xb749faed14125d36,
    0xe51c79a85916f484, 0x8f31cc0937ae58d2,
    0xb2fe3f0b8599ef07, 0xdfbdcece67006ac9,
    0x8bd6a141006042bd, 0xaecc49914078536d,
    0xda7f5bf590966848, 0x888f99797a5e012d,
    0xaab37fd7d8f58178, 0xd5605fcdcf32e1d6,
    0x855c3be0a17fcd26, 0xa6b34ad8c9dfc06f,
    0xd0601d8efc57b08b, 0x823c12795db6ce57,
    0xa2cb1717b52481ed, 0xcb7ddcdda26da268,
    0xfe5d54150b090b02, 0x9efa548d26e5a6e1,
    0xc6b8e9b0709f109a, 0xf867241c8cc6d4c0,
    0x9b407691d7fc44f8, 0xc21094364dfb5636,
    0xf294b943e17a2bc4, 0x979cf3ca6cec5b5a,
    0xbd8430bd08277231, 0xece53cec4a314ebd,
    0x940f4613ae5ed136, 0xb913179899f68584,
    0xe757dd7ec07426e5, 0x9096ea6f3848984f,
    0xb4bca50b065abe63, 0xe1ebce4dc7f16dfb,
    0x8d3360f09cf6e4bd, 0xb080392cc4349dec,
    0xdca04777f541c567, 0x89e42caaf9491b60,
    0xac5d37d5b79b6239, 0xd77485cb25823ac7,
    0x86a8d39ef77164bc, 0xa8530886b54dbdeb,
    0xd267caa862a12d66, 0x8380dea93da4bc60,
    0xa46116538d0deb78, 0xcd795be870516656,
    0x806bd9714632dff6, 0xa086cfcd97bf97f3,
    0xc8a883c0fdaf7df0, 0xfad2a4b13d1b5d6c,
    0x9cc3a6eec6311a63, 0xc3f490aa77bd60fc,
    0xf4f1b4d515acb93b, 0x991711052d8bf3c5,
    0xbf5cd54678eef0b6, 0xef340a98172aace4,
    0x9580869f0e7aac0e, 0xbae0a846d2195712,
    0xe998d258869facd7, 0x91ff83775423cc06,
    0xb67f6455292cbf08, 0xe41f3d6a7377eeca,
    0x8e938662882af53e, 0xb23867fb2a35b28d,
    0xdec681f9f4c31f31, 0x8b3c113c38f9f37e,
    0xae0b158b4738705e, 0xd98ddaee19068c76,
    0x87f8a8d4cfa417c9, 0xa9f6d30a038d1dbc,
    0xd47487cc8470652b, 0x84c8d4dfd2c63f3b,
    0xa5fb0a17c777cf09, 0xcf79cc9db955c2cc,
    0x81ac1fe293d599bf, 0xa21727db38cb002f,
    0xca9cf1d206fdc03b, 0xfd442e4688bd304a,
    0x9e4a9cec15763e2e, 0xc5dd44271ad3cdba,
    0xf7549530e188c128, 0x9a94dd3e8cf578b9,
    0xc13a148e3032d6e7, 0xf18899b1bc3f8ca1,
    0x96f5600f15a7b7e5, 0xbcb2b812db11a5de,
    0xebdf661791d60f56, 0x936b9fcebb25c995,
    0xb84687c269ef3bfb, 0xe65829b3046b0afa,
    0x8ff71a0fe2c2e6dc, 0xb3f4e093db73a093,
    0xe0f218b8d25088b8, 0x8c974f7383725573,
    0xafbd2350644eeacf, 0xdbac6c247d62a583,
    0x894bc396ce5da772, 0xab9eb47c81f5114f,
    0xd686619ba27255a2, 0x8613fd0145877585,
    0xa798fc4196e952e7, 0xd17f3b51fca3a7a0,
    0x82ef85133de648c4, 0xa3ab66580d5fdaf5,
    0xcc963fee10b7d1b3, 0xffbbcfe994e5c61f,
    0x9fd561f1fd0f9bd3, 0xc7caba6e7c5382c8,
    0xf9bd690a1b68637b, 0x9c1661a651213e2d,
    0xc31bfa0fe5698db8, 0xf3e2f893dec3f126,
    0x986ddb5c6b3a76b7, 0xbe89523386091465,
    0xee2ba6c0678b597f, 0x94db483840b717ef,
    0xba121a4650e4ddeb, 0xe896a0d7e51e1566,
    0x915e2486ef32cd60, 0xb5b5ada8aaff80b8,
    0xe3231912d5bf60e6, 0x8df5efabc5979c8f,
    0xb1736b96b6fd83b3, 0xddd0467c64bce4a0,
    0x8aa22c0dbef60ee4, 0xad4ab7112eb3929d,
    0xd89d64d57a607744, 0x87625f056c7c4a8b,
    0xa93af6c6c79b5d2d, 0xd389b47879823479,
    0x843610cb4bf160cb, 0xa54394fe1eedb8fe,
    0xce947a3da6a9273e, 0x811ccc668829b887,
    0xa163ff802a3426a8, 0xc9bcff6034c13052,
    0xfc2c3f3841f17c67, 0x9d9ba7832936edc0,
    0xc5029163f384a931, 0xf64335bcf065d37d,
    0x99ea0196163fa42e, 0xc06481fb9bcf8d39,
    0xf07da27a82c37088, 0x964e858c91ba2655,
    0xbbe226efb628afea, 0xeadab0aba3b2dbe5,
    0x92c8ae6b464fc96f, 0xb77ada0617e3bbcb,
    0xe55990879ddcaabd, 0x8f57fa54c2a9eab6,
    0xb32df8e9f3546564, 0xdff9772470297ebd,
    0x8bfbea76c619ef36, 0xaefae51477a06b03,
    0xdab99e59958885c4, 0x88b402f7fd75539b,
    0xaae103b5fcd2a881, 0xd59944a37c0752a2,
    0x857fcae62d8493a5, 0xa6dfbd9fb8e5b88e,
    0xd097ad07a71f26b2, 0x825ecc24c873782f,
    0xa2f67f2dfa90563b, 0xcbb41ef979346bca,
    0xfea126b7d78186bc, 0x9f24b832e6b0f436,
    0xc6ede63fa05d3143, 0xf8a95fcf88747d94,
    0x9b69dbe1b548ce7c, 0xc24452da229b021b,
    0xf2d56790ab41c2a2, 0x97c560ba6b0919a5,
    0xbdb6b8e905cb600f, 0xed246723473e3813,
    0x9436c0760c86e30b, 0xb94470938fa89bce,
    0xe7958cb87392c2c2, 0x90bd77f3483bb9b9,
    0xb4ecd5f01a4aa828, 0xe2280b6c20dd5232,
    0x8d590723948a535f, 0xb0af48ec79ace837,
    0xdcdb1b2798182244, 0x8a08f0f8bf0f156b,
    0xac8b2d36eed2dac5, 0xd7adf884aa879177,
    0x86ccbb52ea94baea, 0xa87fea27a539e9a5,
    0xd29fe4b18e88640e, 0x83a3eeeef9153e89,
    0xa48ceaaab75a8e2b, 0xcdb02555653131b6,
    0x808e17555f3ebf11, 0xa0b19d2ab70e6ed6,
    0xc8de047564d20a8b, 0xfb158592be068d2e,
    0x9ced737bb6c4183d, 0xc428d05aa4751e4c,
    0xf53304714d9265df, 0x993fe2c6d07b7fab,
    0xbf8fdb78849a5f96, 0xef73d256a5c0f77c,
    0x95a8637627989aad, 0xbb127c53b17ec159,
    0xe9d71b689dde71af, 0x9226712162ab070d,
    0xb6b00d69bb55c8d1, 0xe45c10c42a2b3b05,
    0x8eb98a7a9a5b04e3, 0xb267ed1940f1c61c,
    0xdf01e85f912e37a3, 0x8b61313bbabce2c6,
    0xae397d8aa96c1b77, 0xd9c7dced53c72255,
    0x881cea14545c7575, 0xaa242499697392d2,
    0xd4ad2dbfc3d07787, 0x84ec3c97da624ab4,
    0xa6274bbdd0fadd61, 0xcfb11ead453994ba,
    0x81ceb32c4b43fcf4, 0xa2425ff75e14fc31,
    0xcad2f7f5359a3b3e, 0xfd87b5f28300ca0d,
    0x9e74d1b791e07e48, 0xc612062576589dda,
    0xf79687aed3eec551, 0x9abe14cd44753b52,
    0xc16d9a0095928a27, 0xf1c90080baf72cb1,
    0x971da05074da7bee, 0xbce5086492111aea,
    0xec1e4a7db69561a5, 0x9392ee8e921d5d07,
    0xb877aa3236a4b449, 0xe69594bec44de15b,
    0x901d7cf73ab0acd9, 0xb424dc35095cd80f,
    0xe12e13424bb40e13, 0x8cbccc096f5088cb,
    0xafebff0bcb24aafe, 0xdbe6fecebdedd5be,
    0x89705f4136b4a597, 0xabcc77118461cefc,
    0xd6bf94d5e57a42bc, 0x8637bd05af6c69b5,
    0xa7c5ac471b478423, 0xd1b71758e219652b,
    0x83126e978d4fdf3b, 0xa3d70a3d70a3d70a,
    0xcccccccccccccccc, 0x8000000000000000,
    0xa000000000000000, 0xc800000000000000,
    0xfa00000000000000, 0x9c40000000000000,
    0xc350000000000000, 0xf424000000000000,
    0x9896800000000000, 0xbebc200000000000,
    0xee6b280000000000, 0x9502f90000000000,
    0xba43b74000000000, 0xe8d4a51000000000,
    0x9184e72a00000000, 0xb5e620f480000000,
    0xe35fa931a0000000, 0x8e1bc9bf04000000,
    0xb1a2bc2ec5000000, 0xde0b6b3a76400000,
    0x8ac7230489e80000, 0xad78ebc5ac620000,
    0xd8d726b7177a8000, 0x878678326eac9000,
    0xa968163f0a57b400, 0xd3c21bcecceda100,
    0x84595161401484a0, 0xa56fa5b99019a5c8,
    0xcecb8f27f4200f3a, 0x813f3978f8940984,
    0xa18f07d736b90be5, 0xc9f2c9cd04674ede,
    0xfc6f7c4045812296, 0x9dc5ada82b70b59d,
    0xc5371912364ce305, 0xf684df56c3e01bc6,
    0x9a130b963a6c115c, 0xc097ce7bc90715b3,
    0xf0bdc21abb48db20, 0x96769950b50d88f4,
    0xbc143fa4e250eb31, 0xeb194f8e1ae525fd,
    0x92efd1b8d0cf37be, 0xb7abc627050305ad,
    0xe596b7b0c643c719, 0x8f7e32ce7bea5c6f,
    0xb35dbf821ae4f38b, 0xe0352f62a19e306e,
    0x8c213d9da502de45, 0xaf298d050e4395d6,
    0xdaf3f04651d47b4c, 0x88d8762bf324cd0f,
    0xab0e93b6efee0053, 0xd5d238a4abe98068,
    0x85a36366eb71f041, 0xa70c3c40a64e6c51,
    0xd0cf4b50cfe20765, 0x82818f1281ed449f,
    0xa321f2d7226895c7, 0xcbea6f8ceb02bb39,
    0xfee50b7025c36a08, 0x9f4f2726179a2245,
    0xc722f0ef9d80aad6, 0xf8ebad2b84e0d58b,
    0x9b934c3b330c8577, 0xc2781f49ffcfa6d5,
    0xf316271c7fc3908a, 0x97edd871cfda3a56,
    0xbde94e8e43d0c8ec, 0xed63a231d4c4fb27,
    0x945e455f24fb1cf8, 0xb975d6b6ee39e436,
    0xe7d34c64a9c85d44, 0x90e40fbeea1d3a4a,
    0xb51d13aea4a488dd, 0xe264589a4dcdab14,
    0x8d7eb76070a08aec, 0xb0de65388cc8ada8,
    0xdd15fe86affad912, 0x8a2dbf142dfcc7ab,
    0xacb92ed9397bf996, 0xd7e77a8f87daf7fb,
    0x86f0ac99b4e8dafd, 0xa8acd7c0222311bc,
    0xd2d80db02aabd62b, 0x83c7088e1aab65db,
    0xa4b8cab1a1563f52, 0xcde6fd5e09abcf26,
    0x80b05e5ac60b6178, 0xa0dc75f1778e39d6,
    0xc913936dd571c84c, 0xfb5878494ace3a5f,
    0x9d174b2dcec0e47b, 0xc45d1df942711d9a,
    0xf5746577930d6500, 0x9968bf6abbe85f20,
    0xbfc2ef456ae276e8, 0xefb3ab16c59b14a2,
    0x95d04aee3b80ece5, 0xbb445da9ca61281f,
    0xea1575143cf97226, 0x924d692ca61be758,
    0xb6e0c377cfa2e12e, 0xe498f455c38b997a,
    0x8edf98b59a373fec, 0xb2977ee300c50fe7,
    0xdf3d5e9bc0f653e1, 0x8b865b215899f46c,
    0xae67f1e9aec07187, 0xda01ee641a708de9,
    0x884134fe908658b2, 0xaa51823e34a7eede,
    0xd4e5e2cdc1d1ea96, 0x850fadc09923329e,
    0xa6539930bf6bff45, 0xcfe87f7cef46ff16,
    0x81f14fae158c5f6e, 0xa26da3999aef7749,
    0xcb090c8001ab551c, 0xfdcb4fa002162a63,
    0x9e9f11c4014dda7e, 0xc646d63501a1511d,
    0xf7d88bc24209a565, 0x9ae757596946075f,
    0xc1a12d2fc3978937, 0xf209787bb47d6b84,
    0x9745eb4d50ce6332, 0xbd176620a501fbff,
    0xec5d3fa8ce427aff, 0x93ba47c980e98cdf,
    0xb8a8d9bbe123f017, 0xe6d3102ad96cec1d,
    0x9043ea1ac7e41392, 0xb454e4a179dd1877,
    0xe16a1dc9d8545e94, 0x8ce2529e2734bb1d,
    0xb01ae745b101e9e4, 0xdc21a1171d42645d,
    0x899504ae72497eba, 0xabfa45da0edbde69,
    0xd6f8d7509292d603, 0x865b86925b9bc5c2,
    0xa7f26836f282b732, 0xd1ef0244af2364ff,
    0x8335616aed761f1f, 0xa402b9c5a8d3a6e7,
    0xcd036837130890a1, 0x802221226be55a64,
    0xa02aa96b06deb0fd, 0xc83553c5c8965d3d,
    0xfa42a8b73abbf48c, 0x9c69a97284b578d7,
    0xc38413cf25e2d70d, 0xf46518c2ef5b8cd1,
    0x98bf2f79d5993802, 0xbeeefb584aff8603,
    0xeeaaba2e5dbf6784, 0x952ab45cfa97a0b2,
    0xba756174393d88df, 0xe912b9d1478ceb17,
    0x91abb422ccb812ee, 0xb616a12b7fe617aa,
    0xe39c49765fdf9d94, 0x8e41ade9fbebc27d,
    0xb1d219647ae6b31c, 0xde469fbd99a05fe3,
    0x8aec23d680043bee, 0xada72ccc20054ae9,
    0xd910f7ff28069da4, 0x87aa9aff79042286,
    0xa99541bf57452b28, 0xd3fa922f2d1675f2,
    0x847c9b5d7c2e09b7, 0xa59bc234db398c25,
    0xcf02b2c21207ef2e, 0x8161afb94b44f57d,
    0xa1ba1ba79e1632dc, 0xca28a291859bbf93,
    0xfcb2cb35e702af78, 0x9defbf01b061adab,
    0xc56baec21c7a1916, 0xf6c69a72a3989f5b,
    0x9a3c2087a63f6399, 0xc0cb28a98fcf3c7f,
    0xf0fdf2d3f3c30b9f, 0x969eb7c47859e743,
    0xbc4665b596706114, 0xeb57ff22fc0c7959,
    0x9316ff75dd87cbd8, 0xb7dcbf5354e9bece,
    0xe5d3ef282a242e81, 0x8fa475791a569d10,
    0xb38d92d760ec4455, 0xe070f78d3927556a,
    0x8c469ab843b89562, 0xaf58416654a6babb,
    0xdb2e51bfe9d0696a, 0x88fcf317f22241e2,
    0xab3c2fddeeaad25a, 0xd60b3bd56a5586f1,
    0x85c7056562757456, 0xa738c6bebb12d16c,
    0xd106f86e69d785c7, 0x82a45b450226b39c,
    0xa34d721642b06084, 0xcc20ce9bd35c78a5,
    0xff290242c83396ce, 0x9f79a169bd203e41,
    0xc75809c42c684dd1, 0xf92e0c3537826145,
    0x9bbcc7a142b17ccb, 0xc2abf989935ddbfe,
    0xf356f7ebf83552fe, 0x98165af37b2153de,
    0xbe1bf1b059e9a8d6, 0xeda2ee1c7064130c,
    0x9485d4d1c63e8be7, 0xb9a74a0637ce2ee1,
    0xe8111c87c5c1ba99, 0x910ab1d4db9914a0,
    0xb54d5e4a127f59c8, 0xe2a0b5dc971f303a,
    0x8da471a9de737e24, 0xb10d8e1456105dad,
    0xdd50f1996b947518, 0x8a5296ffe33cc92f,
    0xace73cbfdc0bfb7b, 0xd8210befd30efa5a,
    0x8714a775e3e95c78, 0xa8d9d1535ce3b396,
    0xd31045a8341ca07c, 0x83ea2b892091e44d,
    0xa4e4b66b68b65d60, 0xce1de40642e3f4b9,
    0x80d2ae83e9ce78f3, 0xa1075a24e4421730,
    0xc94930ae1d529cfc, 0xfb9b7cd9a4a7443c,
    0x9d412e0806e88aa5, 0xc491798a08a2ad4e,
    0xf5b5d7ec8acb58a2, 0x9991a6f3d6bf1765,
    0xbff610b0cc6edd3f, 0xeff394dcff8a948e,
    0x95f83d0a1fb69cd9, 0xbb764c4ca7a4440f,
    0xea53df5fd18d5513, 0x92746b9be2f8552c,
    0xb7118682dbb66a77, 0xe4d5e82392a40515,
    0x8f05b1163ba6832d, 0xb2c71d5bca9023f8,
    0xdf78e4b2bd342cf6, 0x8bab8eefb6409c1a,
    0xae9672aba3d0c320, 0xda3c0f568cc4f3e8,
    0x8865899617fb1871, 0xaa7eebfb9df9de8d,
    0xd51ea6fa85785631, 0x8533285c936b35de,
    0xa67ff273b8460356, 0xd01fef10a657842c,
    0x8213f56a67f6b29b, 0xa298f2c501f45f42,
    0xcb3f2f7642717713, 0xfe0efb53d30dd4d7,
    0x9ec95d1463e8a506, 0xc67bb4597ce2ce48,
    0xf81aa16fdc1b81da, 0x9b10a4e5e9913128,
    0xc1d4ce1f63f57d72, 0xf24a01a73cf2dccf,
    0x976e41088617ca01, 0xbd49d14aa79dbc82,
    0xec9c459d51852ba2, 0x93e1ab8252f33b45,
    0xb8da1662e7b00a17, 0xe7109bfba19c0c9d,
    0x906a617d450187e2, 0xb484f9dc9641e9da,
    0xe1a63853bbd26451, 0x8d07e33455637eb2,
    0xb049dc016abc5e5f, 0xdc5c5301c56b75f7,
    0x89b9b3e11b6329ba, 0xac2820d9623bf429,
    0xd732290fbacaf133, 0x867f59a9d4bed6c0,
    0xa81f301449ee8c70, 0xd226fc195c6a2f8c,
    0x83585d8fd9c25db7, 0xa42e74f3d032f525,
    0xcd3a1230c43fb26f, 0x80444b5e7aa7cf85,
    0xa0555e361951c366, 0xc86ab5c39fa63440,
    0xfa856334878fc150, 0x9c935e00d4b9d8d2,
    0xc3b8358109e84f07, 0xf4a642e14c6262c8,
    0x98e7e9cccfbd7dbd, 0xbf21e44003acdd2c,
    0xeeea5d5004981478, 0x95527a5202df0ccb,
    0xbaa718e68396cffd, 0xe950df20247c83fd,
    0x91d28b7416cdd27e, 0xb6472e511c81471d,
    0xe3d8f9e563a198e5, 0x8e679c2f5e44ff8f};

  static const u64_t mantissa_128[] = {
    0x419ea3bd35385e2d, 0x52064cac828675b9,
    0x7343efebd1940993, 0x1014ebe6c5f90bf8,
    0xd41a26e077774ef6, 0x8920b098955522b4,
    0x55b46e5f5d5535b0, 0xeb2189f734aa831d,
    0xa5e9ec7501d523e4, 0x47b233c92125366e,
    0x999ec0bb696e840a, 0xc00670ea43ca250d,
    0x380406926a5e5728, 0xc605083704f5ecf2,
    0xf7864a44c633682e, 0x7ab3ee6afbe0211d,
    0x5960ea05bad82964, 0x6fb92487298e33bd,
    0xa5d3b6d479f8e056, 0x8f48a4899877186c,
    0x331acdabfe94de87, 0x9ff0c08b7f1d0b14,
    0x7ecf0ae5ee44dd9, 0xc9e82cd9f69d6150,
    0xbe311c083a225cd2, 0x6dbd630a48aaf406,
    0x92cbbccdad5b108, 0x25bbf56008c58ea5,
    0xaf2af2b80af6f24e, 0x1af5af660db4aee1,
    0x50d98d9fc890ed4d, 0xe50ff107bab528a0,
    0x1e53ed49a96272c8, 0x25e8e89c13bb0f7a,
    0x77b191618c54e9ac, 0xd59df5b9ef6a2417,
    0x4b0573286b44ad1d, 0x4ee367f9430aec32,
    0x229c41f793cda73f, 0x6b43527578c1110f,
    0x830a13896b78aaa9, 0x23cc986bc656d553,
    0x2cbfbe86b7ec8aa8, 0x7bf7d71432f3d6a9,
    0xdaf5ccd93fb0cc53, 0xd1b3400f8f9cff68,
    0x23100809b9c21fa1, 0xabd40a0c2832a78a,
    0x16c90c8f323f516c, 0xae3da7d97f6792e3,
    0x99cd11cfdf41779c, 0x40405643d711d583,
    0x482835ea666b2572, 0xda3243650005eecf,
    0x90bed43e40076a82, 0x5a7744a6e804a291,
    0x711515d0a205cb36, 0xd5a5b44ca873e03,
    0xe858790afe9486c2, 0x626e974dbe39a872,
    0xfb0a3d212dc8128f, 0x7ce66634bc9d0b99,
    0x1c1fffc1ebc44e80, 0xa327ffb266b56220,
    0x4bf1ff9f0062baa8, 0x6f773fc3603db4a9,
    0xcb550fb4384d21d3, 0x7e2a53a146606a48,
    0x2eda7444cbfc426d, 0xfa911155fefb5308,
    0x793555ab7eba27ca, 0x4bc1558b2f3458de,
    0x9eb1aaedfb016f16, 0x465e15a979c1cadc,
    0xbfacd89ec191ec9, 0xcef980ec671f667b,
    0x82b7e12780e7401a, 0xd1b2ecb8b0908810,
    0x861fa7e6dcb4aa15, 0x67a791e093e1d49a,
    0xe0c8bb2c5c6d24e0, 0x58fae9f773886e18,
    0xaf39a475506a899e, 0x6d8406c952429603,
    0xc8e5087ba6d33b83, 0xfb1e4a9a90880a64,
    0x5cf2eea09a55067f, 0xf42faa48c0ea481e,
    0xf13b94daf124da26, 0x76c53d08d6b70858,
    0x54768c4b0c64ca6e, 0xa9942f5dcf7dfd09,
    0xd3f93b35435d7c4c, 0xc47bc5014a1a6daf,
    0x359ab6419ca1091b, 0xc30163d203c94b62,
    0x79e0de63425dcf1d, 0x985915fc12f542e4,
    0x3e6f5b7b17b2939d, 0xa705992ceecf9c42,
    0x50c6ff782a838353, 0xa4f8bf5635246428,
    0x871b7795e136be99, 0x28e2557b59846e3f,
    0x331aeada2fe589cf, 0x3ff0d2c85def7621,
    0xfed077a756b53a9, 0xd3e8495912c62894,
    0x64712dd7abbbd95c, 0xbd8d794d96aacfb3,
    0xecf0d7a0fc5583a0, 0xf41686c49db57244,
    0x311c2875c522ced5, 0x7d633293366b828b,
    0xae5dff9c02033197, 0xd9f57f830283fdfc,
    0xd072df63c324fd7b, 0x4247cb9e59f71e6d,
    0x52d9be85f074e608, 0x67902e276c921f8b,
    0xba1cd8a3db53b6, 0x80e8a40eccd228a4,
    0x6122cd128006b2cd, 0x796b805720085f81,
    0xcbe3303674053bb0, 0xbedbfc4411068a9c,
    0xee92fb5515482d44, 0x751bdd152d4d1c4a,
    0xd262d45a78a0635d, 0x86fb897116c87c34,
    0xd45d35e6ae3d4da0, 0x8974836059cca109,
    0x2bd1a438703fc94b, 0x7b6306a34627ddcf,
    0x1a3bc84c17b1d542, 0x20caba5f1d9e4a93,
    0x547eb47b7282ee9c, 0xe99e619a4f23aa43,
    0x6405fa00e2ec94d4, 0xde83bc408dd3dd04,
    0x9624ab50b148d445, 0x3badd624dd9b0957,
    0xe54ca5d70a80e5d6, 0x5e9fcf4ccd211f4c,
    0x7647c3200069671f, 0x29ecd9f40041e073,
    0xf468107100525890, 0x7182148d4066eeb4,
    0xc6f14cd848405530, 0xb8ada00e5a506a7c,
    0xa6d90811f0e4851c, 0x908f4a166d1da663,
    0x9a598e4e043287fe, 0x40eff1e1853f29fd,
    0xd12bee59e68ef47c, 0x82bb74f8301958ce,
    0xe36a52363c1faf01, 0xdc44e6c3cb279ac1,
    0x29ab103a5ef8c0b9, 0x7415d448f6b6f0e7,
    0x111b495b3464ad21, 0xcab10dd900beec34,
    0x3d5d514f40eea742, 0xcb4a5a3112a5112,
    0x47f0e785eaba72ab, 0x59ed216765690f56,
    0x306869c13ec3532c, 0x1e414218c73a13fb,
    0xe5d1929ef90898fa, 0xdf45f746b74abf39,
    0x6b8bba8c328eb783, 0x66ea92f3f326564,
    0xc80a537b0efefebd, 0xbd06742ce95f5f36,
    0x2c48113823b73704, 0xf75a15862ca504c5,
    0x9a984d73dbe722fb, 0xc13e60d0d2e0ebba,
    0x318df905079926a8, 0xfdf17746497f7052,
    0xfeb6ea8bedefa633, 0xfe64a52ee96b8fc0,
    0x3dfdce7aa3c673b0, 0x6bea10ca65c084e,
    0x486e494fcff30a62, 0x5a89dba3c3efccfa,
    0xf89629465a75e01c, 0xf6bbb397f1135823,
    0x746aa07ded582e2c, 0xa8c2a44eb4571cdc,
    0x92f34d62616ce413, 0x77b020baf9c81d17,
    0xace1474dc1d122e, 0xd819992132456ba,
    0x10e1fff697ed6c69, 0xca8d3ffa1ef463c1,
    0xbd308ff8a6b17cb2, 0xac7cb3f6d05ddbde,
    0x6bcdf07a423aa96b, 0x86c16c98d2c953c6,
    0xe871c7bf077ba8b7, 0x11471cd764ad4972,
    0xd598e40d3dd89bcf, 0x4aff1d108d4ec2c3,
    0xcedf722a585139ba, 0xc2974eb4ee658828,
    0x733d226229feea32, 0x806357d5a3f525f,
    0xca07c2dcb0cf26f7, 0xfc89b393dd02f0b5,
    0xbbac2078d443ace2, 0xd54b944b84aa4c0d,
    0xa9e795e65d4df11, 0x4d4617b5ff4a16d5,
    0x504bced1bf8e4e45, 0xe45ec2862f71e1d6,
    0x5d767327bb4e5a4c, 0x3a6a07f8d510f86f,
    0x890489f70a55368b, 0x2b45ac74ccea842e,
    0x3b0b8bc90012929d, 0x9ce6ebb40173744,
    0xcc420a6a101d0515, 0x9fa946824a12232d,
    0x47939822dc96abf9, 0x59787e2b93bc56f7,
    0x57eb4edb3c55b65a, 0xede622920b6b23f1,
    0xe95fab368e45eced, 0x11dbcb0218ebb414,
    0xd652bdc29f26a119, 0x4be76d3346f0495f,
    0x6f70a4400c562ddb, 0xcb4ccd500f6bb952,
    0x7e2000a41346a7a7, 0x8ed400668c0c28c8,
    0x728900802f0f32fa, 0x4f2b40a03ad2ffb9,
    0xe2f610c84987bfa8, 0xdd9ca7d2df4d7c9,
    0x91503d1c79720dbb, 0x75a44c6397ce912a,
    0xc986afbe3ee11aba, 0xfbe85badce996168,
    0xfae27299423fb9c3, 0xdccd879fc967d41a,
    0x5400e987bbc1c920, 0x290123e9aab23b68,
    0xf9a0b6720aaf6521, 0xf808e40e8d5b3e69,
    0xb60b1d1230b20e04, 0xb1c6f22b5e6f48c2,
    0x1e38aeb6360b1af3, 0x25c6da63c38de1b0,
    0x579c487e5a38ad0e, 0x2d835a9df0c6d851,
    0xf8e431456cf88e65, 0x1b8e9ecb641b58ff,
    0xe272467e3d222f3f, 0x5b0ed81dcc6abb0f,
    0x98e947129fc2b4e9, 0x3f2398d747b36224,
    0x8eec7f0d19a03aad, 0x1953cf68300424ac,
    0x5fa8c3423c052dd7, 0x3792f412cb06794d,
    0xe2bbd88bbee40bd0, 0x5b6aceaeae9d0ec4,
    0xf245825a5a445275, 0xeed6e2f0f0d56712,
    0x55464dd69685606b, 0xaa97e14c3c26b886,
    0xd53dd99f4b3066a8, 0xe546a8038efe4029,
    0xde98520472bdd033, 0x963e66858f6d4440,
    0xdde7001379a44aa8, 0x5560c018580d5d52,
    0xaab8f01e6e10b4a6, 0xcab3961304ca70e8,
    0x3d607b97c5fd0d22, 0x8cb89a7db77c506a,
    0x77f3608e92adb242, 0x55f038b237591ed3,
    0x6b6c46dec52f6688, 0x2323ac4b3b3da015,
    0xabec975e0a0d081a, 0x96e7bd358c904a21,
    0x7e50d64177da2e54, 0xdde50bd1d5d0b9e9,
    0x955e4ec64b44e864, 0xbd5af13bef0b113e,
    0xecb1ad8aeacdd58e, 0x67de18eda5814af2,
    0x80eacf948770ced7, 0xa1258379a94d028d,
    0x96ee45813a04330, 0x8bca9d6e188853fc,
    0x775ea264cf55347d, 0x95364afe032a819d,
    0x3a83ddbd83f52204, 0xc4926a9672793542,
    0x75b7053c0f178293, 0x5324c68b12dd6338,
    0xd3f6fc16ebca5e03, 0x88f4bb1ca6bcf584,
    0x2b31e9e3d06c32e5, 0x3aff322e62439fcf,
    0x9befeb9fad487c2, 0x4c2ebe687989a9b3,
    0xf9d37014bf60a10, 0x538484c19ef38c94,
    0x2865a5f206b06fb9, 0xf93f87b7442e45d3,
    0xf78f69a51539d748, 0xb573440e5a884d1b,
    0x31680a88f8953030, 0xfdc20d2b36ba7c3d,
    0x3d32907604691b4c, 0xa63f9a49c2c1b10f,
    0xfcf80dc33721d53, 0xd3c36113404ea4a8,
    0x645a1cac083126e9, 0x3d70a3d70a3d70a3,
    0xcccccccccccccccc, 0x0,
    0x0, 0x0,
    0x0, 0x0,
    0x0, 0x0,
    0x0, 0x0,
    0x0, 0x0,
    0x0, 0x0,
    0x0, 0x0,
    0x0, 0x0,
    0x0, 0x0,
    0x0, 0x0,
    0x0, 0x0,
    0x0, 0x0,
    0x0, 0x0,
    0x0, 0x4000000000000000,
    0x5000000000000000, 0xa400000000000000,
    0x4d00000000000000, 0xf020000000000000,
    0x6c28000000000000, 0xc732000000000000,
    0x3c7f400000000000, 0x4b9f100000000000,
    0x1e86d40000000000, 0x1314448000000000,
    0x17d955a000000000, 0x5dcfab0800000000,
    0x5aa1cae500000000, 0xf14a3d9e40000000,
    0x6d9ccd05d0000000, 0xe4820023a2000000,
    0xdda2802c8a800000, 0xd50b2037ad200000,
    0x4526f422cc340000, 0x9670b12b7f410000,
    0x3c0cdd765f114000, 0xa5880a69fb6ac800,
    0x8eea0d047a457a00, 0x72a4904598d6d880,
    0x47a6da2b7f864750, 0x999090b65f67d924,
    0xfff4b4e3f741cf6d, 0xbff8f10e7a8921a4,
    0xaff72d52192b6a0d, 0x9bf4f8a69f764490,
    0x2f236d04753d5b4, 0x1d762422c946590,
    0x424d3ad2b7b97ef5, 0xd2e0898765a7deb2,
    0x63cc55f49f88eb2f, 0x3cbf6b71c76b25fb,
    0x8bef464e3945ef7a, 0x97758bf0e3cbb5ac,
    0x3d52eeed1cbea317, 0x4ca7aaa863ee4bdd,
    0x8fe8caa93e74ef6a, 0xb3e2fd538e122b44,
    0x60dbbca87196b616, 0xbc8955e946fe31cd,
    0x6babab6398bdbe41, 0xc696963c7eed2dd1,
    0xfc1e1de5cf543ca2, 0x3b25a55f43294bcb,
    0x49ef0eb713f39ebe, 0x6e3569326c784337,
    0x49c2c37f07965404, 0xdc33745ec97be906,
    0x69a028bb3ded71a3, 0xc40832ea0d68ce0c,
    0xf50a3fa490c30190, 0x792667c6da79e0fa,
    0x577001b891185938, 0xed4c0226b55e6f86,
    0x544f8158315b05b4, 0x696361ae3db1c721,
    0x3bc3a19cd1e38e9, 0x4ab48a04065c723,
    0x62eb0d64283f9c76, 0x3ba5d0bd324f8394,
    0xca8f44ec7ee36479, 0x7e998b13cf4e1ecb,
    0x9e3fedd8c321a67e, 0xc5cfe94ef3ea101e,
    0xbba1f1d158724a12, 0x2a8a6e45ae8edc97,
    0xf52d09d71a3293bd, 0x593c2626705f9c56,
    0x6f8b2fb00c77836c, 0xb6dfb9c0f956447,
    0x4724bd4189bd5eac, 0x58edec91ec2cb657,
    0x2f2967b66737e3ed, 0xbd79e0d20082ee74,
    0xecd8590680a3aa11, 0xe80e6f4820cc9495,
    0x3109058d147fdcdd, 0xbd4b46f0599fd415,
    0x6c9e18ac7007c91a, 0x3e2cf6bc604ddb0,
    0x84db8346b786151c, 0xe612641865679a63,
    0x4fcb7e8f3f60c07e, 0xe3be5e330f38f09d,
    0x5cadf5bfd3072cc5, 0x73d9732fc7c8f7f6,
    0x2867e7fddcdd9afa, 0xb281e1fd541501b8,
    0x1f225a7ca91a4226, 0x3375788de9b06958,
    0x52d6b1641c83ae, 0xc0678c5dbd23a49a,
    0xf840b7ba963646e0, 0xb650e5a93bc3d898,
    0xa3e51f138ab4cebe, 0xc66f336c36b10137,
    0xb80b0047445d4184, 0xa60dc059157491e5,
    0x87c89837ad68db2f, 0x29babe4598c311fb,
    0xf4296dd6fef3d67a, 0x1899e4a65f58660c,
    0x5ec05dcff72e7f8f, 0x76707543f4fa1f73,
    0x6a06494a791c53a8, 0x487db9d17636892,
    0x45a9d2845d3c42b6, 0xb8a2392ba45a9b2,
    0x8e6cac7768d7141e, 0x3207d795430cd926,
    0x7f44e6bd49e807b8, 0x5f16206c9c6209a6,
    0x36dba887c37a8c0f, 0xc2494954da2c9789,
    0xf2db9baa10b7bd6c, 0x6f92829494e5acc7,
    0xcb772339ba1f17f9, 0xff2a760414536efb,
    0xfef5138519684aba, 0x7eb258665fc25d69,
    0xef2f773ffbd97a61, 0xaafb550ffacfd8fa,
    0x95ba2a53f983cf38, 0xdd945a747bf26183,
    0x94f971119aeef9e4, 0x7a37cd5601aab85d,
    0xac62e055c10ab33a, 0x577b986b314d6009,
    0xed5a7e85fda0b80b, 0x14588f13be847307,
    0x596eb2d8ae258fc8, 0x6fca5f8ed9aef3bb,
    0x25de7bb9480d5854, 0xaf561aa79a10ae6a,
    0x1b2ba1518094da04, 0x90fb44d2f05d0842,
    0x353a1607ac744a53, 0x42889b8997915ce8,
    0x69956135febada11, 0x43fab9837e699095,
    0x94f967e45e03f4bb, 0x1d1be0eebac278f5,
    0x6462d92a69731732, 0x7d7b8f7503cfdcfe,
    0x5cda735244c3d43e, 0x3a0888136afa64a7,
    0x88aaa1845b8fdd0, 0x8aad549e57273d45,
    0x36ac54e2f678864b, 0x84576a1bb416a7dd,
    0x656d44a2a11c51d5, 0x9f644ae5a4b1b325,
    0x873d5d9f0dde1fee, 0xa90cb506d155a7ea,
    0x9a7f12442d588f2, 0xc11ed6d538aeb2f,
    0x8f1668c8a86da5fa, 0xf96e017d694487bc,
    0x37c981dcc395a9ac, 0x85bbe253f47b1417,
    0x93956d7478ccec8e, 0x387ac8d1970027b2,
    0x6997b05fcc0319e, 0x441fece3bdf81f03,
    0xd527e81cad7626c3, 0x8a71e223d8d3b074,
    0xf6872d5667844e49, 0xb428f8ac016561db,
    0xe13336d701beba52, 0xecc0024661173473,
    0x27f002d7f95d0190, 0x31ec038df7b441f4,
    0x7e67047175a15271, 0xf0062c6e984d386,
    0x52c07b78a3e60868, 0xa7709a56ccdf8a82,
    0x88a66076400bb691, 0x6acff893d00ea435,
    0x583f6b8c4124d43, 0xc3727a337a8b704a,
    0x744f18c0592e4c5c, 0x1162def06f79df73,
    0x8addcb5645ac2ba8, 0x6d953e2bd7173692,
    0xc8fa8db6ccdd0437, 0x1d9c9892400a22a2,
    0x2503beb6d00cab4b, 0x2e44ae64840fd61d,
    0x5ceaecfed289e5d2, 0x7425a83e872c5f47,
    0xd12f124e28f77719, 0x82bd6b70d99aaa6f,
    0x636cc64d1001550b, 0x3c47f7e05401aa4e,
    0x65acfaec34810a71, 0x7f1839a741a14d0d,
    0x1ede48111209a050, 0x934aed0aab460432,
    0xf81da84d5617853f, 0x36251260ab9d668e,
    0xc1d72b7c6b426019, 0xb24cf65b8612f81f,
    0xdee033f26797b627, 0x169840ef017da3b1,
    0x8e1f289560ee864e, 0xf1a6f2bab92a27e2,
    0xae10af696774b1db, 0xacca6da1e0a8ef29,
    0x17fd090a58d32af3, 0xddfc4b4cef07f5b0,
    0x4abdaf101564f98e, 0x9d6d1ad41abe37f1,
    0x84c86189216dc5ed, 0x32fd3cf5b4e49bb4,
    0x3fbc8c33221dc2a1, 0xfabaf3feaa5334a,
    0x29cb4d87f2a7400e, 0x743e20e9ef511012,
    0x914da9246b255416, 0x1ad089b6c2f7548e,
    0xa184ac2473b529b1, 0xc9e5d72d90a2741e,
    0x7e2fa67c7a658892, 0xddbb901b98feeab7,
    0x552a74227f3ea565, 0xd53a88958f87275f,
    0x8a892abaf368f137, 0x2d2b7569b0432d85,
    0x9c3b29620e29fc73, 0x8349f3ba91b47b8f,
    0x241c70a936219a73, 0xed238cd383aa0110,
    0xf4363804324a40aa, 0xb143c6053edcd0d5,
    0xdd94b7868e94050a, 0xca7cf2b4191c8326,
    0xfd1c2f611f63a3f0, 0xbc633b39673c8cec,
    0xd5be0503e085d813, 0x4b2d8644d8a74e18,
    0xddf8e7d60ed1219e, 0xcabb90e5c942b503,
    0x3d6a751f3b936243, 0xcc512670a783ad4,
    0x27fb2b80668b24c5, 0xb1f9f660802dedf6,
    0x5e7873f8a0396973, 0xdb0b487b6423e1e8,
    0x91ce1a9a3d2cda62, 0x7641a140cc7810fb,
    0xa9e904c87fcb0a9d, 0x546345fa9fbdcd44,
    0xa97c177947ad4095, 0x49ed8eabcccc485d,
    0x5c68f256bfff5a74, 0x73832eec6fff3111,
    0xc831fd53c5ff7eab, 0xba3e7ca8b77f5e55,
    0x28ce1bd2e55f35eb, 0x7980d163cf5b81b3,
    0xd7e105bcc332621f, 0x8dd9472bf3fefaa7,
    0xb14f98f6f0feb951, 0x6ed1bf9a569f33d3,
    0xa862f80ec4700c8, 0xcd27bb612758c0fa,
    0x8038d51cb897789c, 0xe0470a63e6bd56c3,
    0x1858ccfce06cac74, 0xf37801e0c43ebc8,
    0xd30560258f54e6ba, 0x47c6b82ef32a2069,
    0x4cdc331d57fa5441, 0xe0133fe4adf8e952,
    0x58180fddd97723a6, 0x570f09eaa7ea7648,};
#endif

  if (-22 <= power && power <= 22 && i <= 9007199254740991) {
    f64_t d = f64_t(i);
    if (power < 0) {
      d = d / power_of_ten[-power];
    }
    else {
      d = d * power_of_ten[power];
    }
    if (negative) {
      d = -d;
    }

    return d;
  }

  if (i == 0) {
    return negative ? -0.0 : 0.0;
  }

  //assert(false);
  return F64_INFINITY;
}

static f64_t
cstr_to_f64(const c8_t* p) {
  b32_t found_minus = (*p == '-');
  b32_t negative = false;
  if (found_minus) {
    ++p;
    negative = true;
    if (!is_digit(*p)) { // a negative sign must be followed by an integer
      return 0.0;
    }
  }
  const c8_t *const start_digits = p;

  u64_t i;      // an unsigned int avoids signed overflows (which are bad)
  if (*p == '0') { // 0 cannot be followed by an integer
    ++p;
    if (is_digit(*p)) {
      return 0.0;
    }
    i = 0;
  } 
  else {
    if (!(is_digit(*p))) { // must start with an integer
      return 0.0;
    }
    u8_t digit = *p - '0';
    i = digit;
    p++;
    // the is_made_of_eight_digits_fast routine is unlikely to help here because
    // we rarely see large integer parts like 123456789
    while (is_digit(*p)) {
      digit = *p - '0';
      // a multiplication by 10 is cheaper than an arbitrary integer
      // multiplication
      i = 10 * i + digit; // might overflow, we will handle the overflow later
      ++p;
    }
  }
  s64_t exponent = 0;
  const c8_t *first_after_period = NULL;
  if (*p == '.') {
    ++p;
    first_after_period = p;
    if (is_digit(*p)) {
      u8_t digit = *p - '0';
      ++p;
      i = i * 10 + digit; // might overflow + multiplication by 10 is likely
                          // cheaper than arbitrary mult.
                          // we will handle the overflow later
    } else {
      return 0.0;
    }
    while (is_digit(*p)) {
      u8_t digit = *p - '0';
      ++p;
      i = i * 10 + digit; // in rare cases, this will overflow, but that's ok
                          // because we have parse_highprecision_float later.
    }
    exponent = first_after_period - p;
  }
  int digit_count = int(p - start_digits - 1); // used later to guard against overflows
  if (('e' == *p) || ('E' == *p)) {
    ++p;
    bool neg_exp = false;
    if ('-' == *p) {
      neg_exp = true;
      ++p;
    } else if ('+' == *p) {
      ++p;
    }
    if (!is_digit(*p)) {
      return 0.0;
    }
    u8_t digit = *p - '0';
    int64_t exp_number = digit;
    p++;
    if (is_digit(*p)) {
      digit = *p - '0';
      exp_number = 10 * exp_number + digit;
      ++p;
    }
    if (is_digit(*p)) {
      digit = *p - '0';
      exp_number = 10 * exp_number + digit;
      ++p;
    }
    while (is_digit(*p)) {
      digit = *p - '0';
      if (exp_number < 0x100000000) { // we need to check for overflows
        exp_number = 10 * exp_number + digit;
      }
      ++p;
    }
    exponent += (neg_exp ? -exp_number : exp_number);
  }

  if (digit_count >= 19) {
    return F64_NAN;
  }
  if (exponent < -325 || exponent > 308) {
    return F64_NAN;
  }

  // Unlikely cases. Can go for 'slow' path instead of asserting
  assert(digit_count < 19);
  assert(exponent >= -325 || exponent <= 308);

  return _compute_f64(exponent, i, negative);
}

static void 
cstr_clear(c8_t* dest) {
  (*dest) = 0;
}

static void
cstr_reverse(c8_t* dest) {
  c8_t* back_ptr = dest;
  for (; *(back_ptr+1) != 0; ++back_ptr);
  for (;dest < back_ptr; ++dest, --back_ptr) {
    swap(*dest, *back_ptr);
  }
}



static void 
cstr_itoa(c8_t* dest, s32_t num) {
  // Naive method. 
  // Extract each number starting from the back and fill the buffer. 
  // Then reverse it.

  // Special case for 0
  if (num == 0) {
    dest[0] = '0';
    dest[1] = 0;
    return;
  }

  b32_t negative = num < 0;
  num = s32_abs(num);

  c8_t* it = dest;
  for(; num != 0; num /= 10) {
    s32_t digit_to_convert = num % 10;
    *(it++) = (c8_t)(digit_to_convert + '0');
  }

  if (negative) {
    *(it++) = '-';
  }
  (*it) = 0;

  cstr_reverse(dest);
}

static b32_t 
f32_is_close(f32_t lhs, f32_t rhs) {
  return f32_abs(lhs - rhs) <= F32_EPSILON;
}

static b32_t 
f64_is_close(f64_t lhs, f64_t rhs) {
  return f64_abs(lhs - rhs) <= F64_EPSILON;
}

static b32_t 
f32_is_nan(f32_t f) {
  union { f32_t f; u64_t u; } ret = {};
  ret.f = f;
  return (ret.u & 0xFFFFFFFF) == 0xFFFFFFFF;
}

static b32_t 
f64_is_nan(f64_t f) {
  union { f64_t f; u64_t u; } ret = {};
  ret.f = f;
  return (ret.u & 0xFFFFFFFFFFFFFFFF) == 0xFFFFFFFFFFFFFFFF;
}

static u32_t 
hash_djb2(const c8_t* str)
{
  // DJB2
  //
  // this algorithm (k=33) was first reported by dan bernstein many 
  // years ago in comp.lang.c. another version of this algorithm 
  // (now favored by bernstein) uses xor: hash(i) = hash(i - 1) * 33 ^ str[i]; 
  // the magic of number 33 (why it works better than many other constants, prime or not) 
  // has never been adequately explained.

  u32_t hash = 5381;
  s32_t c;
  while (c = *str++) {
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  }
  return hash;
}

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
# warning "[momo] Atomic functions are not implemented!"
#endif

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
  x = clamp_of(x, -1.f, 1.f);
  return asinf(x);
}
static f32_t 
f32_acos(f32_t x) {
  x = clamp_of(x, -1.f, 1.f);
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
  x = clamp_of(x, -1.f, 1.f);
  return asin(x);
}
static f64_t 
f64_acos(f64_t x) {
  x = clamp_of(x, -1.f, 1.f);
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


// TODO(momo): IEEE version of these?
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

static f32_t 
f32_ease_linear(f32_t t) {
  return t;
}

static f32_t 
f32_ease_in_sine(f32_t t)  {
  return f32_sin(PI_32 * 0.5f * t);
}


static f32_t 
f32_ease_out_sine(f32_t t) {
  return 1.0f + f32_sin(PI_32 * 0.5f * (--t));
}

static f32_t 
f32_ease_inout_sine(f32_t t)  {
  return 0.5f * (1.f + f32_sin(PI_32 * (t - 0.5f)));
}

static f32_t 
f32_ease_in_quad(f32_t t)  {
  return t * t;
}

static f32_t 
f32_ease_out_quad(f32_t t)  {
  return t * (2.f -t);
}

static f32_t 
f32_ease_inout_quad(f32_t t)  {
  return t < 0.5f ? 2.f * t * t : t * (4.f -2.f * t) - 1.f;
}

static f32_t 
f32_ease_in_cubic(f32_t t)  {
  return t * t * t;
}

static f32_t 
f32_ease_out_cubic(f32_t t)  {
  return 1.f + (t-1) * (t-1) * (t-1);
}

static f32_t 
f32_ease_inout_cubic(f32_t t)  {
  return t < 0.5f ? 4.f * t * t * t : 1.f + (t-1) * (2.f * (t-2)) * (2.f * (t-2));
}

static f32_t 
f32_ease_in_quart(f32_t t)  {
  t *= t;
  return t * t;
}

static f32_t 
f32_ease_out_quart(f32_t t) {
  --t;
  t = t * t;
  return 1.f - t * t;
}

static f32_t 
f32_ease_inout_quart(f32_t t)  {
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

static f32_t
f32_ease_in_quint(f32_t t)  {
  f32_t t2 = t * t;
  return t * t2 * t2;
}

static f32_t
f32_ease_out_quint(f32_t t)  {
  --t;
  f32_t t2 = t * t;
  return 1.f +t * t2 * t2;
}

static f32_t
f32_ease_inout_quint(f32_t t)  {
  f32_t t2;
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



static f32_t 
f32_ease_in_circ(f32_t t)  {
  return 1.f -f32_sqrt(1.f -t);
}

static f32_t 
f32_ease_out_circ(f32_t t)  {
  return f32_sqrt(t);
}

static f32_t 
f32_ease_inout_circ(f32_t t)  {
  if (t < 0.5f) {
    return (1.f -f32_sqrt(1.f -2.f * t)) * 0.5f;
  }
  else {
    return (1.f +f32_sqrt(2.f * t - 1.f)) * 0.5f;
  }
}

static f32_t 
f32_ease_in_back(f32_t t)  {
  return t * t * (2.7f * t - 1.7f);
}

static f32_t 
f32_ease_out_back(f32_t t)  {
  --t;
  return 1.f + t * t * (2.7f * t + 1.7f);
}

static f32_t 
f32_ease_inout_back(f32_t t)  {
  if (t < 0.5f) {
    return t * t * (7.f * t - 2.5f) * 2.f;
  }
  else {
    --t;
    return 1.f + t * t * 2.f * (7.f * t + 2.5f);
  }
}

static f32_t 
f32_ease_in_elastic(f32_t t)  {
  f32_t t2 = t * t;
  return t2 * t2 * f32_sin(t * PI_32 * 4.5f);
}

static f32_t 
f32_ease_out_elastic(f32_t t)  {
  f32_t t2 = (t - 1.f) * (t - 1.f);
  return 1.f -t2 * t2 * f32_cos(t * PI_32 * 4.5f);
}

static f32_t 
f32_ease_inout_elastic(f32_t t)  {
  f32_t t2;
  if (t < 0.45f) {
    t2 = t * t;
    return 8.f * t2 * t2 * f32_sin(t * PI_32 * 9.f);
  }
  else if (t < 0.55f) {
    return 0.5f +0.75f * f32_sin(t * PI_32 * 4.f);
  }
  else {
    t2 = (t - 1.f) * (t - 1.f);
    return 1.f -8.f * t2 * t2 * f32_sin(t * PI_32 * 9.f);
  }
}

static f32_t 
f32_ease_in_bounce(f32_t t)  {
  return f32_pow(2.f, 6.f * (t - 1.f)) * f32_abs(f32_sin(t * PI_32 * 3.5f));
}


static f32_t 
f32_ease_out_bounce(f32_t t) {
  return 1.f -f32_pow(2.f, -6.f * t) * f32_abs(f32_cos(t * PI_32 * 3.5f));
}

static f32_t 
f32_ease_inout_bounce(f32_t t) {
  if (t < 0.5f) {
    return 8.f * f32_pow(2.f, 8.f * (t - 1.f)) * f32_abs(f32_sin(t * PI_32 * 7.f));
  }
  else {
    return 1.f -8.f * f32_pow(2.f, -8.f * t) * f32_abs(f32_sin(t * PI_32 * 7.f));
  }
}

static f32_t
f32_ease_in_expo(f32_t t)  {
  return (f32_pow(2.f, 8.f * t) - 1.f) / 255.f;
}


static f32_t 
f32_ease_out_expo(f32_t t)  {
  return t == 1.f ? 1.f : 1.f -f32_pow(2.f, -10.f * t);
}

static f32_t 
f32_ease_inout_expo(f32_t t)  {
  if (t < 0.5f) {
    return (f32_pow(2.f, 16.f * t) - 1.f) / 510.f;
  }
  else {
    return 1.f -0.5f * f32_pow(2.f, -16.f * (t - 0.5f));
  }
}

static f64_t 
f64_ease_linear(f64_t t) {
  return t;
}

static f64_t 
f64_ease_in_sine(f64_t t)  {
  return f64_sin(PI_64 * 0.5 * t);
}


static f64_t 
f64_ease_out_sine(f64_t t) {
  return 1.0f + f64_sin(PI_64 * 0.5 * (--t));
}

static f64_t 
f64_ease_inout_sine(f64_t t)  {
  return 0.5 * (1.0 + f64_sin(PI_64 * (t - 0.5)));
}

static f64_t 
f64_ease_in_quad(f64_t t)  {
  return t * t;
}

static f64_t 
f64_ease_out_quad(f64_t t)  {
  return t * (2.0 -t);
}

static f64_t 
f64_ease_inout_quad(f64_t t)  {
  return t < 0.5 ? 2.0 * t * t : t * (4.0 -2.0 * t) - 1.0;
}

static f64_t 
f64_ease_in_cubic(f64_t t)  {
  return t * t * t;
}

static f64_t 
f64_ease_out_cubic(f64_t t)  {
  return 1.0 + (t-1) * (t-1) * (t-1);
}

static f64_t 
f64_ease_inout_cubic(f64_t t)  {
  return t < 0.5 ? 4.0 * t * t * t : 1.0 + (t-1) * (2.0 * (t-2)) * (2.0 * (t-2));
}

static f64_t 
f64_ease_in_quart(f64_t t)  {
  t *= t;
  return t * t;
}

static f64_t 
f64_ease_out_quart(f64_t t) {
  --t;
  t = t * t;
  return 1.0 - t * t;
}

static f64_t 
f64_ease_inout_quart(f64_t t)  {
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

static f64_t
f64_ease_in_quint(f64_t t)  {
  f64_t t2 = t * t;
  return t * t2 * t2;
}

static f64_t
f64_ease_out_quint(f64_t t)  {
  --t;
  f64_t t2 = t * t;
  return 1.0 +t * t2 * t2;
}

static f64_t
f64_ease_inout_quint(f64_t t)  {
  f64_t t2;
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



static f64_t 
f64_ease_in_circ(f64_t t)  {
  return 1.0 -f64_sqrt(1.0 -t);
}

static f64_t 
f64_ease_out_circ(f64_t t)  {
  return f64_sqrt(t);
}

static f64_t 
f64_ease_inout_circ(f64_t t)  {
  if (t < 0.5) {
    return (1.0 -f64_sqrt(1.0 -2.0 * t)) * 0.5;
  }
  else {
    return (1.0 +f64_sqrt(2.0 * t - 1.0)) * 0.5;
  }
}

static f64_t 
f64_ease_in_back(f64_t t)  {
  return t * t * (2.7 * t - 1.7);
}

static f64_t 
f64_ease_out_back(f64_t t)  {
  --t;
  return 1.0 + t * t * (2.7 * t + 1.7);
}

static f64_t 
f64_ease_inout_back(f64_t t)  {
  if (t < 0.5) {
    return t * t * (7.0 * t - 2.5) * 2.0;
  }
  else {
    --t;
    return 1.0 + t * t * 2.0 * (7.0 * t + 2.5);
  }
}

static f64_t 
f64_ease_in_elastic(f64_t t)  {
  f64_t t2 = t * t;
  return t2 * t2 * f64_sin(t * PI_64 * 4.5);
}

static f64_t 
f64_ease_out_elastic(f64_t t)  {
  f64_t t2 = (t - 1.0) * (t - 1.0);
  return 1.0 -t2 * t2 * f64_cos(t * PI_64 * 4.5);
}

static f64_t 
f64_ease_inout_elastic(f64_t t)  {
  f64_t t2;
  if (t < 0.45) {
    t2 = t * t;
    return 8.0 * t2 * t2 * f64_sin(t * PI_64 * 9.0);
  }
  else if (t < 0.55) {
    return 0.5 +0.75 * f64_sin(t * PI_64 * 4.0);
  }
  else {
    t2 = (t - 1.0) * (t - 1.0);
    return 1.0 -8.0 * t2 * t2 * f64_sin(t * PI_64 * 9.0);
  }
}



// NOTE(Momo): These require power function. 
static f64_t 
f64_ease_in_bounce(f64_t t)  {
  return f64_pow(2.0, 6.0 * (t - 1.0)) * f64_abs(f64_sin(t * PI_64 * 3.5));
}


static f64_t 
f64_ease_out_bounce(f64_t t) {
  return 1.0 -f64_pow(2.0, -6.0 * t) * f64_abs(f64_cos(t * PI_64 * 3.5));
}

static f64_t 
f64_ease_inout_bounce(f64_t t) {
  if (t < 0.5) {
    return 8.0 * f64_pow(2.0, 8.0 * (t - 1.0)) * f64_abs(f64_sin(t * PI_64 * 7.0));
  }
  else {
    return 1.0 -8.0 * f64_pow(2.0, -8.0 * t) * f64_abs(f64_sin(t * PI_64 * 7.0));
  }
}

static f64_t
f64_ease_in_expo(f64_t t)  {
  return (f64_pow(2.0, 8.0 * t) - 1.0) / 255.0;
}


static f64_t 
f64_ease_out_expo(f64_t t)  {
  return t == 1.0 ? 1.0 : 1.0 -f64_pow(2.0, -10.0 * t);
}

static f64_t 
f64_ease_inout_expo(f64_t t)  {
  if (t < 0.5) {
    return (f64_pow(2.0, 16.0 * t) - 1.0) / 510.0;
  }
  else {
    return 1.0 -0.5 * f64_pow(2.0, -16.0 * (t - 0.5));
  }
}



//
// MARK:(Vector)
//

static v2f_t 
v2f_add(v2f_t lhs, v2f_t rhs) {
  lhs.x += rhs.x;
  lhs.y += rhs.y;
  return lhs;
}

static v2f_t 
v2f_sub(v2f_t lhs, v2f_t rhs) {
  lhs.x -= rhs.x;
  lhs.y -= rhs.y;
  return lhs;
}

static v2f_t 
v2f_scale(v2f_t lhs, f32_t rhs) {
  lhs.x *= rhs;
  lhs.y *= rhs;
  return lhs;
}

static v2f_t 
v2f_div(v2f_t lhs, f32_t rhs) {
  assert(!f32_is_close(rhs, 0.f));
  lhs.x /= rhs;
  lhs.y /= rhs;
  return lhs;
}

static v2f_t
v2f_inv(v2f_t v) {
  v.x = 1.f/v.x;
  v.y = 1.f/v.y;
  return v;
}

static v2f_t 
v2f_negate(v2f_t v) {
  v.x = -v.x;
  v.y = -v.y;
  return v;
}

static f32_t 
v2f_dot(v2f_t lhs, v2f_t rhs) {
  return (lhs.x * rhs.x) + (lhs.y * rhs.y);
}

static f32_t  
v2f_len_sq(v2f_t v) {
  return v2f_dot(v, v);
}

static f32_t   
v2f_len(v2f_t v) {
  return f32_sqrt(v2f_len_sq(v));
}
static f32_t  
v2f_dist_sq(v2f_t lhs, v2f_t rhs) {
  return v2f_len_sq(v2f_sub(lhs, rhs));
}
static f32_t  
v2f_dist(v2f_t lhs, v2f_t rhs) {
  return f32_sqrt(v2f_dist_sq(lhs, rhs));
}


static v2f_t 
v2f_norm(v2f_t v) {
  f32_t len = v2f_len(v);
  return v2f_div(v, len);
}

static b32_t
v2f_is_close(v2f_t lhs, v2f_t rhs) {
  return (f32_is_close(lhs.x, rhs.x) &&
      f32_is_close(lhs.y, rhs.y)); 
}

static v2f_t 
v2f_mid(v2f_t lhs, v2f_t rhs) {
  return v2f_scale(v2f_add(lhs, rhs), 0.5f); 

}
static v2f_t 
v2f_proj(v2f_t v, v2f_t onto) {
  // (to . from)/LenSq(to) * to
  f32_t onto_len_sq = v2f_len_sq(onto);
  assert(!f32_is_close(onto_len_sq, 0.f));
  f32_t v_dot_onto = v2f_dot(v, onto);
  f32_t scalar = v_dot_onto / onto_len_sq;
  v2f_t ret = v2f_scale(onto, scalar);

  return ret;
}

// Angle Between
static f32_t
v2f_angle(v2f_t lhs, v2f_t rhs) {
  f32_t l_len = v2f_len(lhs);
  f32_t r_len = v2f_len(rhs);
  f32_t lr_dot = v2f_dot(lhs, rhs);
  f32_t ret = f32_acos(lr_dot/(l_len * r_len));
  return ret;
}

static v2f_t 
v2f_rotate(v2f_t v, f32_t rad) {
  // Technically, we can use matrices but
  // meh, it's easy to code this out without it.
  // Removes dependencies too
  f32_t c = f32_cos(rad);
  f32_t s = f32_sin(rad);

  v2f_t ret = {};
  ret.x = (c * v.x) - (s * v.y);
  ret.y = (s * v.x) + (c * v.y);
  return ret;
}

static f32_t
v2f_cross(v2f_t lhs, v2f_t rhs) {
  return  lhs.x * rhs.y - lhs.y * rhs.x;
}

  static v2f_t  
v2f_lerp(v2f_t s, v2f_t e, f32_t a) 
{
  v2f_t ret = {0};
  ret.x = f32_lerp(s.x,e.x,a);
  ret.y = f32_lerp(s.y,e.y,a);
  return ret;
}


static v3f_t 
v3f_add(v3f_t lhs, v3f_t rhs) {
  lhs.x += rhs.x;
  lhs.y += rhs.y;
  lhs.z += rhs.z;
  return lhs;
}
static v3f_t 
v3f_sub(v3f_t lhs, v3f_t rhs) {
  lhs.x -= rhs.x;
  lhs.y -= rhs.y;
  lhs.z -= rhs.z;
  return lhs;
}

static v3f_t 
v3f_scale(v3f_t lhs, f32_t rhs) {
  lhs.x *= rhs;
  lhs.y *= rhs;
  lhs.z *= rhs;
  return lhs;
}

static v3f_t 
v3f_div(v3f_t lhs, f32_t rhs) {
  assert(!f32_is_close(rhs, 0.f));
  lhs.x /= rhs;
  lhs.y /= rhs;
  lhs.z /= rhs;
  return lhs;
}

static v3f_t 
v3f_negate(v3f_t v) {
  v.x = -v.x;
  v.y = -v.y;
  v.z = -v.z;
  return v;
}

static f32_t 
v3f_dot(v3f_t lhs, v3f_t rhs) {
  return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
}

static f32_t  
v3f_len_sq(v3f_t v) {
  return v3f_dot(v, v);
}

static f32_t   
v3f_len(v3f_t v) {
  return f32_sqrt(v3f_len_sq(v));
}

static f32_t  
v3f_dist_sq(v3f_t lhs, v3f_t rhs) {
  return v3f_len_sq(v3f_sub(lhs, rhs));
}

static f32_t  
v3f_dist(v3f_t lhs, v3f_t rhs) {
  return f32_sqrt(v3f_dist_sq(lhs, rhs));
}


static v3f_t 
v3f_norm(v3f_t v) {
  f32_t len = v3f_len(v);
  return v3f_div(v, len);
}

static b32_t
v3f_is_close(v3f_t lhs, v3f_t rhs) {
  return (f32_is_close(lhs.x, rhs.x) &&
      f32_is_close(lhs.y, rhs.y)); 
}

static v3f_t 
v3f_mid(v3f_t lhs, v3f_t rhs) {
  return v3f_scale(v3f_add(lhs, rhs), 0.5f); 

}
static v3f_t 
v3f_project(v3f_t v, v3f_t onto) {
  // (to . from)/LenSq(to) * to
  f32_t onto_len_sq = v3f_len_sq(onto);
  assert(!f32_is_close(onto_len_sq, 0.f));
  f32_t v_dot_onto = v3f_dot(v, onto);
  f32_t scalar = v_dot_onto / onto_len_sq;
  v3f_t ret = v3f_scale(onto, scalar);

  return ret;
}

static f32_t
v3f_angle(v3f_t lhs, v3f_t rhs) {
  f32_t l_len = v3f_len(lhs);
  f32_t r_len = v3f_len(rhs);
  f32_t lr_dot = v3f_dot(lhs, rhs);
  f32_t ret = f32_acos(lr_dot/(l_len * r_len));
  return ret;
}

static v3f_t
v3f_cross(v3f_t lhs, v3f_t rhs) {
  v3f_t ret = {};
  ret.x = (lhs.y * rhs.z) - (lhs.z * rhs.y);
  ret.y = (lhs.z * rhs.x) - (lhs.x * rhs.z);
  ret.z = (lhs.x * rhs.y) - (lhs.y * rhs.x);

  return ret;
}

//~ v2u_t
static v2u_t    
v2u_add(v2u_t lhs, v2u_t rhs) {
  lhs.x += rhs.x;
  lhs.y += rhs.y;
  return lhs;
}

static v2u_t    
v2u_sub(v2u_t lhs, v2u_t rhs){
  lhs.x -= rhs.x;
  lhs.y -= rhs.y;
  return lhs;
}

static v2f_t    
v2f_set(f32_t x, f32_t y){
  return { x, y };
}

static v2f_t    
v2f_zero(void){
  return { 0, 0 };
}

static v2f_t  operator+(v2f_t lhs, v2f_t rhs) { return v2f_add(lhs, rhs); }
static v2f_t  operator-(v2f_t lhs, v2f_t rhs) { return v2f_sub(lhs, rhs); }
static v2f_t  operator*(v2f_t lhs, f32_t rhs) { return v2f_scale(lhs, rhs); }
static v2f_t  operator*(f32_t lhs, v2f_t rhs) { return v2f_scale(rhs, lhs); }
static b32_t  operator==(v2f_t lhs, v2f_t rhs) { return v2f_is_close(lhs, rhs); }
static b32_t  operator!=(v2f_t lhs, v2f_t rhs) { return !v2f_is_close(lhs, rhs); }
static v2f_t  operator-(v2f_t v) { return v2f_negate(v); }
static v2f_t& operator+=(v2f_t& lhs, v2f_t rhs) { return lhs = v2f_add(lhs, rhs); } 
static v2f_t& operator-=(v2f_t& lhs, v2f_t rhs) { return lhs = v2f_sub(lhs, rhs); } 
static v2f_t& operator*=(v2f_t& lhs, f32_t rhs) { return lhs = v2f_scale(lhs, rhs); }

static v2u_t operator+(v2u_t lhs, v2u_t rhs) { return v2u_add(lhs, rhs); }
static v2u_t operator-(v2u_t lhs, v2u_t rhs) { return v2u_sub(lhs, rhs); }

static v3f_t  operator+(v3f_t lhs, v3f_t rhs) { return v3f_add(lhs, rhs); }
static v3f_t  operator-(v3f_t lhs, v3f_t rhs) { return v3f_sub(lhs, rhs); }
static v3f_t  operator*(v3f_t lhs, f32_t rhs) { return v3f_scale(lhs, rhs); }
static v3f_t  operator*(f32_t lhs, v3f_t rhs) { return v3f_scale(rhs, lhs); }
static b32_t  operator==(v3f_t lhs, v3f_t rhs) { return v3f_is_close(lhs, rhs); }
static b32_t  operator!=(v3f_t lhs, v3f_t rhs) { return !v3f_is_close(lhs, rhs); }
static v3f_t  operator-(v3f_t v) { return v3f_negate(v); }
static v3f_t& operator+=(v3f_t& lhs, v3f_t rhs) { return lhs = v3f_add(lhs, rhs); } 
static v3f_t& operator-=(v3f_t& lhs, v3f_t rhs) { return lhs = v3f_sub(lhs, rhs); } 
static v3f_t& operator*=(v3f_t& lhs, f32_t rhs) { return lhs = v3f_scale(lhs, rhs); }

// 
// MARK:(Matrix)
// 

static m44f_t
m44f_concat(m44f_t lhs, m44f_t rhs) {
  m44f_t ret = {};
  for (u32_t r = 0; r < 4; r++) { 
    for (u32_t c = 0; c < 4; c++) { 
      for (u32_t i = 0; i < 4; i++) {
        ret.e[r][c] += lhs.e[r][i] *  rhs.e[i][c]; 
      }
    } 
  } 
  return ret;
}

static m44f_t 
m44f_transpose(m44f_t m) {
  m44f_t ret = {};
  for (u32_t i = 0; i < 4; ++i ) {
    for (u32_t j = 0; j < 4; ++j) {
      ret.e[i][j] = m.e[j][i];
    }
  }
  return ret;
}
static m44f_t m44f_scale(f32_t x, f32_t y, f32_t z) {
  m44f_t ret = {};
  ret.e[0][0] = x;
  ret.e[1][1] = y;
  ret.e[2][2] = z;
  ret.e[3][3] = 1.f;

  return ret;
}

static m44f_t 
m44f_identity() {
  m44f_t ret = {};
  ret.e[0][0] = 1.f;
  ret.e[1][1] = 1.f;
  ret.e[2][2] = 1.f;
  ret.e[3][3] = 1.f;

  return ret;
}

static m44f_t 
m44f_translation(f32_t x, f32_t y, f32_t z) {
  m44f_t ret = m44f_identity();
  ret.e[0][3] = x;
  ret.e[1][3] = y;
  ret.e[2][3] = z;
  ret.e[3][3] = 1.f;

  return ret;
}

static m44f_t 
m44f_rotation_x(f32_t rad) {
  // NOTE(Momo): 
  // 1  0  0  0
  // 0  c -s  0
  // 0  s  c  0
  // 0  0  0  1
  f32_t c = f32_cos(rad);
  f32_t s = f32_sin(rad);
  m44f_t ret = {};
  ret.e[0][0] = 1.f;
  ret.e[3][3] = 1.f;
  ret.e[1][1] = c;
  ret.e[1][2] = -s;
  ret.e[2][1] = s;
  ret.e[2][2] = c;

  return ret;
}
static m44f_t m44f_rotation_y(f32_t rad) {

  // NOTE(Momo): 
  //  c  0  s  0
  //  0  1  0  0
  // -s  0  c  0
  //  0  0  0  1
  f32_t c = f32_cos(rad);
  f32_t s = f32_sin(rad);
  m44f_t ret = {};
  ret.e[0][0] = c;
  ret.e[0][2] = s;
  ret.e[1][1] = 1.f;
  ret.e[2][0] = -s;
  ret.e[2][2] = c;
  ret.e[3][3] = 1.f;

  return ret;
}

static m44f_t 
m44f_rotation_z(f32_t rad) {
  // NOTE(Momo): 
  //  c -s  0  0
  //  s  c  0  0
  //  0  0  1  0
  //  0  0  0  1

  f32_t c = f32_cos(rad);
  f32_t s = f32_sin(rad);
  m44f_t ret = {};
  ret.e[0][0] = c;
  ret.e[0][1] = -s;
  ret.e[1][0] = s;
  ret.e[1][1] = c;
  ret.e[2][2] = 1.f;
  ret.e[3][3] = 1.f;

  return ret;
}

static m44f_t 
m44f_orthographic(f32_t left, f32_t right, f32_t bottom, f32_t top, f32_t near, f32_t far) {

  m44f_t ret = {0};
  ret.e[0][0] = 2.f/(right-left);
  ret.e[1][1] = 2.f/(top-bottom);
  ret.e[2][2] = 2.f/(far-near);
  ret.e[3][3] = 1.f;
  ret.e[0][3] = -(right+left)/(right-left);
  ret.e[1][3] = -(top+bottom)/(top-bottom);
  ret.e[2][3] = -(far+near)/(far-near);

  return ret;
}

static m44f_t 
m44f_frustum(f32_t left, f32_t right, f32_t bottom, f32_t top, f32_t near, f32_t far) {
  m44f_t ret = {};
  ret.e[0][0] = (2.f*near)/(right-left);
  ret.e[1][1] = (2.f*near)/(top-bottom);
  ret.e[2][2] = -(far+near)/(far-near);
  ret.e[3][2] = 1;  
  ret.e[0][2] = (right+left)/(right-left);
  ret.e[1][2] = (top+bottom)/(top-bottom);
  ret.e[1][3] = -near*(top+bottom)/(top-bottom);
  ret.e[2][3] = 2.f*far*near/(far-near);

  return ret;
}

static m44f_t 
m44f_perspective(f32_t fov, f32_t aspect, f32_t near, f32_t far){
  f32_t top = near * f32_tan(fov*0.5f);
  f32_t right = top * aspect;
  return m44f_frustum(-right, right,
      -top, top,
      near, far);
}

static m44f_t operator*(m44f_t lhs, m44f_t rhs) {
  return m44f_concat(lhs, rhs);
}
//
// MARK:(Colors)
//
static rgba_t 
rgba_set(f32_t r, f32_t g, f32_t b, f32_t a){
  rgba_t ret;
  ret.r = r;
  ret.g = g;
  ret.b = b;
  ret.a = a;

  return ret;
}

static hsla_t 
hsla_set(f32_t h, f32_t s, f32_t l, f32_t a){
  hsla_t ret;
  ret.h = h;
  ret.s = s;
  ret.l = l;
  ret.a = a;

  return ret;
}

static rgba_t
rgba_hex(u32_t hex) {
  rgba_t ret;

  ret.r = (f32_t)((hex & 0xFF000000) >> 24)/255.f;
  ret.g = (f32_t)((hex & 0x00FF0000) >> 16)/255.f;
  ret.b = (f32_t)((hex & 0x0000FF00) >> 8)/255.f;
  ret.a = (f32_t)(hex & 0x000000FF)/255.f;

  return ret;
}

static hsl_t
hsl_set(f32_t h, f32_t s, f32_t l) {
  hsl_t ret;
  ret.h = h;
  ret.s = s;
  ret.l = l;

  return ret;     
}

static hsl_t 
rbg_to_hsl(rgb_t c) {
  assert(c.r >= 0.f &&
      c.r <= 1.f &&
      c.g >= 0.f &&
      c.g <= 1.f &&
      c.b >= 0.f &&
      c.b <= 1.f);
  hsl_t ret;
  f32_t max = max_of(max_of(c.r, c.g), c.b);
  f32_t min = min_of(min_of(c.r, c.g), c.b);

  f32_t delta = max - min; // aka chroma


  if (f32_is_close(max, c.r)) {
    f32_t segment = (c.g - c.b)/delta;
    f32_t shift = 0.f / 60;
    if (segment < 0) {
      shift = 360 / 60;
    }
    else {
      shift = 0.f / 60;
    }
    ret.h = (segment + shift) * 60.f;
  }

  else if (f32_is_close(max, c.g)) {
    f32_t segment = (c.b - c.r)/delta;
    f32_t shift = 120.f / 60.f;
    ret.h = (segment + shift) * 60.f;
  }

  else if (f32_is_close(max, c.b)) {
    f32_t segment = (c.r - c.g)/delta;
    f32_t shift = 240.f / 60.f;
    ret.h = ((segment + shift) * 60.f);
  }
  else {
    ret.h = 0.f;
  }
  ret.h /= 360.f;


  ret.l = (max + min) * 0.5f;

  if (f32_is_close(delta, 0.f)) {
    ret.s = 0.f;
  }
  else {
    ret.s = delta/(1.f - f32_abs(2.f * ret.l - 1.f));
  }

  return ret;
}

static f32_t 
_hue_to_color(f32_t p, f32_t q, f32_t t) {
  if (t < 0) 
    t += 1.f;
  if (t > 1.f) 
    t -= 1.f;
  if (t < 1./6.f) 
    return p + (q - p) * 6.f * t;
  if (t < 1./2.f) 
    return q;
  if (t < 2./3.f)   
    return p + (q - p) * (2.f/3.f - t) * 6.f;

  return p;
}


static rgb_t 
hsl_to_rgb(hsl_t c) {
  assert(c.h >= 0.f &&
      c.h <= 360.f &&
      c.s >= 0.f &&
      c.s <= 1.f &&
      c.l >= 0.f &&
      c.l <= 1.f);
  rgb_t ret;
  if(f32_is_close(c.s, 0.f)) {
    ret.r = ret.g = ret.b = c.l; // achromatic
  }
  else {
    f32_t q = c.l < 0.5f ? c.l * (1 + c.s) : c.l + c.s - c.l * c.s;
    f32_t p = 2.f * c.l - q;
    ret.r = _hue_to_color(p, q, c.h + 1.f/3.f);
    ret.g = _hue_to_color(p, q, c.h);
    ret.b = _hue_to_color(p, q, c.h - 1.f/3.f);
  }


  return ret;

}


static rgba_t hsla_to_rgba(hsla_t c) {
  rgba_t ret = {};
  ret.rgb = hsl_to_rgb(c.hsl);
  ret.a = c.a;

  return ret;
}

//
// MARK:(Sorting)
//

//
// Generic version of quicksort
//

typedef b32_t _quicksort_generic_cmp_t(const void* lhs, const void* rhs);

static u32_t 
_quicksort_generic_partition(
    void* a,
    u32_t start, 
    u32_t ope,
    u32_t element_size,
    _quicksort_generic_cmp_t cmp) {

  // Save the rightmost index as pivot
  // This frees up the right most index as a slot
  u32_t pivot_idx = ope-1;
  u32_t eventual_pivot_idx = start;

  for (u32_t i = start; i < ope-1; ++i) {
    u8_t* i_ptr = (u8_t*)a + (i * element_size);
    u8_t* pivot_ptr = (u8_t*)a + (pivot_idx * element_size);
    //if (st8_compare_lexographically(*i_ptr, *pivot_ptr) < 0) {
    if (cmp(i_ptr, pivot_ptr)) {
      u8_t* eventual_pivot_ptr = (u8_t*)a + (eventual_pivot_idx * element_size);
      swap_memory(i_ptr, eventual_pivot_ptr, element_size);
      ++eventual_pivot_idx;
    }
  }
  swap_memory((u8_t*)a+(eventual_pivot_idx*element_size), (u8_t*)a+(pivot_idx*element_size), element_size);
  return eventual_pivot_idx;
}

// NOTE(Momo): This is done inplace
static void
_quicksort_generic_range(
  void* a, 
  u32_t start, 
  u32_t ope,
  u32_t element_size,
  _quicksort_generic_cmp_t cmp) 
{
  if (ope - start <= 1) {
    return;
  }
  u32_t pivot = _quicksort_generic_partition(a, start, ope, element_size, cmp);
  _quicksort_generic_range(a, start, pivot, element_size, cmp);
  _quicksort_generic_range(a, pivot+1, ope, element_size, cmp);
}


static void
_quicksort_generic(void* entries, u32_t entry_count, u32_t element_size, _quicksort_generic_cmp_t cmp) {
  _quicksort_generic_range(entries, 0, entry_count, element_size, cmp);
}

static void
_sort_swap_entries(sort_entry_t* a, sort_entry_t* b) {
  sort_entry_t temp = *b;
  *b = *a;
  *a = temp;
}

//
// Quick sort
//
static u32_t
_quicksort_partition(sort_entry_t* a,
                     u32_t start, 
                     u32_t ope) 
{
  // Save the rightmost index as pivot
  // This frees up the right most index as a slot
  u32_t pivot_idx = ope-1;
  u32_t eventual_pivot_idx = start;

  for (u32_t i = start; i < ope-1; ++i) {
    sort_entry_t* i_ptr = a + i;
    sort_entry_t* pivot_ptr = a + pivot_idx;
    if (i_ptr->key < pivot_ptr->key) {
      sort_entry_t* eventual_pivot_ptr = a + eventual_pivot_idx;
      _sort_swap_entries(i_ptr, eventual_pivot_ptr);
      ++eventual_pivot_idx;
    }

  }

  _sort_swap_entries(a + eventual_pivot_idx, a + pivot_idx);

  return eventual_pivot_idx;
}

// NOTE(Momo): This is done inplace
static void
_quicksort_range(sort_entry_t* a, 
                 u32_t start, 
                 u32_t ope) 
{
  if (ope - start <= 1) {
    return;
  }
  u32_t pivot = _quicksort_partition(a, start, ope);
  _quicksort_range(a, start, pivot);
  _quicksort_range(a, pivot+1, ope);
}

static void
quicksort(sort_entry_t* entries, u32_t entry_count) {
  _quicksort_range(entries, 0, entry_count);

}


//
// MARK:(Bonk)
//
static b32_t
_bonk_tri2_pt2_parametric(v2f_t tp0, v2f_t tp1, v2f_t tp2, v2f_t pt) {
  f32_t denominator = (tp0.x*(tp1.y - tp2.y) + 
    tp0.y*(tp2.x - tp1.x) + 
    tp1.x*tp2.y - tp1.y*tp2.x);

  f32_t t1 = (pt.x*(tp2.y - tp0.y) + 
    pt.y*(tp0.x - tp2.x) - 
    tp0.x*tp2.y + tp0.y*tp2.x) / denominator;

  f32_t t2 = (pt.x*(tp1.y - tp0.y) + 
    pt.y*(tp0.x - tp1.x) - 
    tp0.x*tp1.y + tp0.y*tp1.x) / -denominator;

  f32_t s = t1 + t2;

  return 0 <= t1 && t1 <= 1 && 0 <= t2 && t2 <= 1 && s <= 1;
}

// Unoptimized: ~72 cycles/hit
// -O2: ~27 cycles/hit 
static b32_t
_bonk_tri2_pt2_barycentric(v2f_t tp0, v2f_t tp1, v2f_t tp2, v2f_t pt) {

  f32_t denominator = ((tp1.y - tp2.y)*
    (tp0.x - tp2.x) + (tp2.x - tp1.x)*
    (tp0.y - tp2.y));

  f32_t a = ((tp1.y - tp2.y)*
    (pt.x - tp2.x) + (tp2.x - tp1.x)*
    (pt.y - tp2.y)) / denominator;

  f32_t b = ((tp2.y - tp0.y)*
    (pt.x - tp2.x) + (tp0.x - tp2.x)*
    (pt.y - tp2.y)) / denominator;

  f32_t c = 1.f - a - b;

  return 0.f <= a && a <= 1.f && 0.f <= b && b <= 1.f && 0.f <= c && c <= 1.f;

}

// Unoptimized: ~262 cycles/hit
// -O2: ~27 cycles/hit 
static b32_t
_bonk_tri2_pt2_dot_product(v2f_t tp0, v2f_t tp1, v2f_t tp2, v2f_t pt) {
  v2f_t vec0 = v2f_set(pt.x - tp0.x, pt.y - tp0.y);      
  v2f_t vec1 = v2f_set(pt.x - tp1.x, pt.y - tp1.y);      
  v2f_t vec2 = v2f_set(pt.x - tp2.x, pt.y - tp2.y);      

  v2f_t n0 = v2f_set(tp1.y - tp0.y, -tp1.x + tp0.x);
  v2f_t n1 = v2f_set(tp2.y - tp1.y, -tp2.x + tp1.x);
  v2f_t n2 = v2f_set(tp0.y - tp2.y, -tp0.x + tp2.x);

  b32_t side0 = v2f_dot(n0,vec0) < 0.f;
  b32_t side1 = v2f_dot(n1,vec1) < 0.f;
  b32_t side2 = v2f_dot(n2,vec2) < 0.f;

  return side0 == side1 && side0 == side2;
}


static b32_t
bonk_tri2_pt2(v2f_t tp0, v2f_t tp1, v2f_t tp2, v2f_t pt) {
  // NOTE(momo): this is the fastest of the 3 apparently
  return _bonk_tri2_pt2_barycentric(tp0, tp1, tp2, pt);
}

//
// MARK:(RNG)
//
static void
rng_init(rng_t* r, u32_t seed)
{
  r->seed = seed;
}

static u32_t
rng_next(rng_t* r)
{
  u32_t result = r->seed;
  result ^= result << 13;
  result ^= result >> 17;
  result ^= result << 5;
  r->seed = result;
  return result;
}

static u32_t 
rng_choice(rng_t* r, u32_t choice_count) {
  return rng_next(r) % choice_count;
}

// Get number within [0, 1]
static f32_t 
rng_unilateral(rng_t* r)
{
  f32_t divisor = 1.0f / (f32_t)U32_MAX;
  f32_t result = divisor*(f32_t)rng_next(r);

  return result;
}


// Get number within [-1, 1]
static f32_t 
rng_bilateral(rng_t* r)
{
  f32_t result = 2.0f * rng_unilateral(r) - 1.0f;  
  return(result);
}

static f32_t 
rng_range_F32(rng_t* r, f32_t min, f32_t max)
{
  f32_t result = f32_lerp(min, rng_unilateral(r), max);
  return(result);
}

static s32_t 
rng_range_S32(rng_t* r, s32_t min, s32_t max)
{
  s32_t result = min + (s32_t)(rng_next(r)%((max + 1) - min));
  return(result);
}

static u32_t
rng_range_U32(rng_t* r, u32_t min, u32_t max)
{
  u32_t result = min + (u32_t)(rng_next(r)%((max + 1) - min));
  return(result);
}

static v2f_t 
rng_unit_circle(rng_t* r) {
  f32_t rand_angle = 2.f * PI_32 * rng_unilateral(r);
  v2f_t ret = {0};
  ret.x = f32_cos(rand_angle);
  ret.y = f32_sin(rand_angle);

  return ret;
}

//
// MARK:(CRC)
//

//
// Slow versions
//
// These are for quick and easy use. 
// Try not to use this for production
//
static u32_t
crc32_slow(u8_t* data, u32_t data_size, u32_t start_register, u32_t polynomial) {
  u32_t r = start_register;
  for (u32_t i = 0; i < data_size; ++i ){
    r ^= data[i] <<  24;
    for (u32_t j = 0; j < 8; ++j) {
      if((r & 0x80000000) != 0) {
        r = (u32_t)((r << 1) ^ polynomial);
      }
      else {
        r <<= 1;
      }
    }
  }
  return r;
}


static u32_t
crc16_slow(u8_t* data, u32_t data_size, u16_t start_register, u16_t polynomial) {
  u32_t r = start_register;
  for (u32_t i = 0; i < data_size; ++i ){
    r ^= data[i] << 8;
    for (u32_t j = 0; j < 8; ++j) {
      if((r & 0x8000) != 0) {
        r = (u32_t)((r << 1) ^ polynomial);
      }
      else {
        r <<= 1;
      }
    }
  }
  return r;
}

static u32_t
crc8_slow(u8_t* data, u32_t data_size, u16_t start_register, u16_t polynomial) {
  u32_t r = start_register;
  for (u32_t i = 0; i < data_size; ++i ){
    r ^= data[i];
    for (u32_t j = 0; j < 8; ++j) {
      if((r & 0x80) != 0) {
        r = (u32_t)((r << 1) ^ polynomial);
      }
      else {
        r <<= 1;
      }
    }
  }
  return r;
}

//
// Table generation
//
static void 
crc32_init_table(crc32_table_t* table, u32_t polynomial) {
  for (u32_t divident = 0; divident < 256; ++divident) {
    u32_t remainder = divident <<  24;
    for (u32_t j = 0; j < 8; ++j) {
      if((remainder & 0x80000000) != 0) {
        remainder = (u32_t)((remainder << 1) ^ polynomial);
      }
      else {
        remainder <<= 1;
      }
    }
    table->remainders[divident] = remainder;
  }
}

static void
crc16_init_table(crc16_table_t* table, u16_t polynomial) {
  for (u32_t divident = 0; divident < 256; ++divident) {
    u32_t remainder = divident << 8;
    for (u32_t j = 0; j < 8; ++j) {
      if((remainder & 0x8000) != 0) {
        remainder = (u16_t)((remainder << 1) ^ polynomial);
      }
      else {
        remainder <<= 1;
      }
    }
    table->remainders[divident] = (u16_t)remainder;
  }
}

static void 
crc8_init_table(crc8_table_t* table, u8_t polynomial) {
  for (u32_t divident = 0; divident < 256; ++divident) {
    u32_t remainder = divident;
    for (u32_t j = 0; j < 8; ++j) {
      if((remainder & 0x80) != 0) {
        remainder = (u8_t)((remainder << 1) ^ polynomial);
      }
      else {
        remainder <<= 1;
      }
    }
    table->remainders[divident] = (u8_t)remainder;
  }
}

//
// Faster version
//
static u32_t
crc32(u8_t* data, u32_t data_size, u16_t start_register, crc32_table_t* table) {
  u32_t crc = start_register;
  for (u32_t i = 0; i < data_size; ++i) {
    u32_t divident = (u32_t)((crc ^ (data[i] << 24)) >> 24);
    crc = (u32_t)((crc << 8) ^ (u32_t)(table->remainders[divident]));
  }
  return crc;
}

static u32_t
crc16(u8_t* data, u32_t data_size, u16_t start_register, crc16_table_t* table) {
  u16_t crc = start_register;
  for (u32_t i = 0; i < data_size; ++i) {
    u16_t divident = (u16_t)((crc ^ (data[i] << 8)) >> 8);
    crc = (u16_t)((crc << 8) ^ (u16_t)(table->remainders[divident]));
  }
  return crc;
}

static u32_t
crc8(u8_t* data, u32_t data_size, u8_t start_register, crc8_table_t* table) {
  u8_t crc = start_register;
  for (u32_t i = 0; i < data_size; ++i) {
    u8_t divident = (u8_t)(crc ^ data[i]);
    crc = table->remainders[divident];
  }
  return crc;
}

//
// MARK:(String)
//
static st8_t
st8_set(u8_t* str, usz_t size) {
  st8_t ret;
  ret.e = str;
  ret.count = size;
  return ret;
}


static st8_t
st8_from_cstr(const c8_t* cstr) {
  return {(u8_t*)cstr, cstr_len(cstr)};
}

static st8_t 
st8_substr(st8_t str, usz_t start, usz_t count) {
  st8_t ret;
  ret.e = str.e + start;
  ret.count = count;

  return ret;
}

static b32_t
st8_match(st8_t lhs, st8_t rhs) {
  if(lhs.count != rhs.count) {
    return false;
  }
  for (usz_t i = 0; i < lhs.count; ++i) {
    if (lhs.e[i] != rhs.e[i]) {
      return false;
    }
  }
  return true;
}



// Compares lexographical order
// If an unmatched character is found at an index, it will return the 'difference' between those characters
// If no unmatched character is found at an index, it will return the size different between the strings 
static smi_t 
st8_compare_lexographically(st8_t lhs, st8_t rhs) {
  for (usz_t i = 0; i < lhs.count && i < rhs.count; ++i) {
    if (lhs.e[i] == rhs.e[i]) continue;
    else {
      return lhs.e[i] - rhs.e[i];
    }
  }

  // Edge case for strings like:
  // lhs == "asd" and rhs == "asdfg"
  if (lhs.count == rhs.count) {
    return 0;
  }
  else {
    return (smi_t)(lhs.count - rhs.count);
  }

}


static b32_t 
st8_to_u32_range(st8_t s, usz_t begin, usz_t ope, u32_t* out) {
  if (ope > s.count) return false;

  u32_t number = 0;
  for (usz_t i = begin; i < ope; ++i) {
    if (!is_digit(s.e[i]))
      return false;
    number *= 10;
    number += ascii_to_digit(s.e[i]);
  }
  (*out) = number;
  return true;
}

// Parsing functions


static b32_t 
st8_to_s32_range(st8_t s, usz_t begin, usz_t ope, s32_t* out) {

  if (ope > s.count) return false;

  b32_t is_negative = false;
  if (s.e[begin] == '-') {
    is_negative = true;
    ++begin;
  }


  s32_t number = 0;
  for (usz_t i = begin; i < ope; ++i) {
    if (!is_digit(s.e[i]))
      return false;
    number *= 10;
    number += ascii_to_digit(s.e[i]);
  }
  (*out) = is_negative ? -number : number;

  return true;
}

static b32_t 
st8_to_f32_range(st8_t s, usz_t begin, usz_t ope, f32_t* out) {
  if (ope > s.count) return false;
  u32_t place = 0;

  // Really lousy algorithm
  f32_t number = 0.f;

  for(usz_t i = begin; i < ope; ++i) {
    if (s.e[i] == '.') {
      place = 1;
      continue;
    }

    u8_t digit = ascii_to_digit(s.e[i]);
    if (place == 0) {
      number *= 10.f;
      number += (f32_t)digit;
    }
    else {
      f32_t value_to_add = (f32_t)digit / (f32_t)(10 * place);
      number += value_to_add;
      place *= 10;
    }
  }
  (*out) = number;
  return true; 
}

static b32_t 
st8_to_f32(st8_t s, f32_t* out) {
  return st8_to_f32_range(s, 0, s.count, out);
}
static b32_t 
st8_to_u32(st8_t s, u32_t* out) {
  return st8_to_u32_range(s, 0, s.count, out);
}

// Parsing functions
static b32_t 
st8_to_s32(st8_t s, s32_t* out) {
  return st8_to_s32_range(s, 0, s.count, out);
}



static void  
stb8_init(stb8_t* b, u8_t* data, usz_t cap) {
  b->e = data;
  b->count = 0;
  b->cap = cap;
}

static usz_t
stb8_remaining(stb8_t* b) {
  return b->cap - b->count; 
}

static void     
stb8_clear(stb8_t* b) {
  b->count = 0;
}

static void     
stb8_pop(stb8_t* b) {
  assert(b->count > 0);
  --b->count;
}

static void     
stb8_push_u8(stb8_t* b, u8_t num) {
  assert(b->count < b->cap); b->e[b->count++] = num;
}

static void     
stb8_push_c8(stb8_t* b, c8_t num) {
  assert(b->count < b->cap);
  b->e[b->count++] = num;
}

static void     
stb8_push_u32(stb8_t* b, u32_t num) {
  if (num == 0) {
    stb8_push_c8(b, '0');
    return;
  }
  usz_t start_pt = b->count; 

  for(; num != 0; num /= 10) {
    u8_t digit_to_convert = (u8_t)(num % 10);
    stb8_push_c8(b, digit_to_ascii(digit_to_convert));
  }

  // Reverse starting from start point to count
  usz_t sub_str_len_half = (b->count - start_pt)/2;
  for(usz_t i = 0; i < sub_str_len_half; ++i) {
    swap(b->e[start_pt + i], b->e[ b->count - 1 - i]);
  }
}
static void     
stb8_push_u64(stb8_t* b, u64_t num) {
  if (num == 0) {
    stb8_push_c8(b, '0');
    return;
  }
  usz_t start_pt = b->count; 

  for(; num != 0; num /= 10) {
    u8_t digit_to_convert = (u8_t)(num % 10);
    stb8_push_c8(b, digit_to_ascii(digit_to_convert));
  }

  // Reverse starting from start point to count
  usz_t sub_str_len_half = (b->count - start_pt)/2;
  for(usz_t i = 0; i < sub_str_len_half; ++i) {
    swap(b->e[start_pt + i], b->e[b->count - 1 - i]);
  }
}
static void     
stb8_push_s32(stb8_t* b, s32_t num) {
  if (num == 0) {
    stb8_push_c8(b, '0');
    return;
  }

  usz_t start_pt = b->count; 

  b32_t negate = num < 0;
  num = s32_abs(num);

  for(; num != 0; num /= 10) {
    u8_t digit_to_convert = (u8_t)(num % 10);
    stb8_push_c8(b, digit_to_ascii(digit_to_convert));
  }

  if (negate) {
    stb8_push_c8(b, '-');
  }

  // Reverse starting from start point to count
  usz_t sub_str_len_half = (b->count - start_pt)/2;
  for(usz_t i = 0; i < sub_str_len_half; ++i) {
    swap(b->e[start_pt+i], b->e[b->count-1-i]);

  }

}

static void     
stb8_push_s64(stb8_t* b, s64_t num) {
  if (num == 0) {
    stb8_push_c8(b, '0');
    return;
  }

  usz_t start_pt = b->count; 

  b32_t negate = num < 0;
  num = s64_abs(num);

  for(; num != 0; num /= 10) {
    u8_t digit_to_convert = (u8_t)(num % 10);
    stb8_push_c8(b, digit_to_ascii(digit_to_convert));
  }

  if (negate) {
    stb8_push_c8(b, '-');
  }

  // Reverse starting from start point to count
  usz_t sub_str_len_half = (b->count - start_pt)/2;
  for(usz_t i = 0; i < sub_str_len_half; ++i) {
    swap(b->e[start_pt+i], b->e[b->count-1-i]);

  }

}

static void     
stb8_push_f32(stb8_t* b, f32_t value, u32_t precision) {
  if (value < 0.f) {
    stb8_push_c8(b, '-');	
    value = -value;
  }

  // NOTE(Momo): won't work for values that u32_t can't contain
  u32_t integer_part = (u32_t)value;
  stb8_push_u32(b, integer_part);
  stb8_push_c8(b, '.');

  value -= (f32_t)integer_part;

  for (u32_t i = 0; i < precision; ++i) {
    value *= 10.f;
  }

  u32_t decimal_part = (u32_t)value;
  stb8_push_u32(b, decimal_part);
}

static void     
stb8_push_f64(stb8_t* b, f64_t value, u32_t precision) {
  if (value < 0.0) {
    stb8_push_c8(b, '-');	
    value = -value;
  }
  // NOTE(Momo): won't work for values that u32_t can't contain
  u32_t integer_part = (u32_t)value;
  stb8_push_u32(b, integer_part);
  stb8_push_c8(b, '.');

  value -= (f64_t)integer_part;

  for (u32_t i = 0; i < precision; ++i) {
    value *= 10.0;
  }

  u32_t decimal_part = (u32_t)value;
  stb8_push_u32(b, decimal_part);
}


static void
stb8_push_hex_u8(stb8_t* b, u8_t value) {

  c8_t parts[2] = {
    (c8_t)(value >> 4),
    (c8_t)(value & 0xF),

  };

  for(u32_t i = 0; i < array_count(parts); ++i) {
    if (parts[i] >= 0 && parts[i] <= 9) {
      stb8_push_c8(b, parts[i] + '0');
    }
    else if (parts[i] >= 10 && parts[i] <= 15) {
      stb8_push_c8(b, parts[i] - 10 + 'A');
    }
  }




}

static void
stb8_push_hex_u32(stb8_t* b, u32_t value) {
  union { u32_t v; u8_t b[4]; } combine;
  combine.v = value;
  for(s32_t i = 3; i >= 0; --i) {
    stb8_push_hex_u8(b, combine.b[i]);
  }
}

static void
_stb8_push_fmt_list(stb8_t* b, st8_t format, va_list args) {
  usz_t at = 0;
  while(at < format.count) {

    if (format.e[at] == '%') {
      ++at;

      // Width
      u32_t width = 0;
      while (format.e[at] >= '0' && format.e[at] <= '9') {
        u32_t digit = ascii_to_digit(format.e[at]);
        width = (width * 10) + digit;
        ++at;
      }

      stb8_make(tb, 64);

      switch(format.e[at]) {
        case 'i': {
          s32_t value = va_arg(args, s32_t);
          stb8_push_s32(tb, value);
        } break;
        case 'I': {
          s64_t value = va_arg(args, s64_t);
          stb8_push_s64(tb, value);
        } break;
        case 'U': {
          u64_t value = va_arg(args, u64_t);
          stb8_push_u64(tb, value);
        } break;
        case 'u': {
          u32_t value = va_arg(args, u32_t);
          stb8_push_u32(tb, value);
        } break;
        case 'f': {
          f64_t value = va_arg(args, f64_t);
          stb8_push_f32(tb, (f32_t)value, 5);
        } break;
        case 'F': {
          f64_t value = va_arg(args, f64_t);
          stb8_push_f64(tb, (f64_t)value, 5);
        } break;
        case 'x':
        case 'X': {
          u32_t value = va_arg(args, u32_t);
          stb8_push_hex_u32(tb, value);
        } break;
        case 's': {
          // c-string
          const char* cstr = va_arg(args, const char*);
          while(cstr[0] != 0) {
            stb8_push_c8(tb, (u8_t)cstr[0]);
            ++cstr;
          }
        } break;

        case 'S': {
          // st8_t, or 'text'.
          st8_t str = va_arg(args, st8_t);
          stb8_push_st8_set(tb, str);
        } break;

        default: {
          // death
          assert(false);
        } break;
      }
      ++at;

      if (width > 0 && tb->str.count < width) {
        usz_t spaces_to_pad = width - tb->str.count;
        while(spaces_to_pad--) {
          stb8_push_c8(b, ' ');
        }
        stb8_push_st8_set(b, tb->str);
      }
      else {
        stb8_push_st8_set(b, tb->str);
      }


    }
    else {
      stb8_push_c8(b, format.e[at++]);
    }

  }
}


static void     
stb8_push_fmt(stb8_t* b, st8_t fmt, ...) {
  va_list args;
  va_start(args, fmt);
  _stb8_push_fmt_list(b, fmt, args);
  va_end(args);
}

static void     
stb8_push_st8_set(stb8_t* b, st8_t src) {
  assert(b->count + src.count <= b->cap);
  for (usz_t i = 0; i < src.count; ++i ) {
    b->e[b->count++] = src.e[i];
  }
}


//
// MARK:(Stream)
//
static void
stream_init(stream_t* s, buffer_t contents) {
  s->contents = contents;
  s->pos = 0;
  s->bit_buffer = 0;
  s->bit_count = 0;
}

static void
stream_reset(stream_t* s) {
  s->pos = 0;
}

static b32_t
stream_is_eos(stream_t* s) {
  return s->pos >= s->contents.size;
}

static u8_t*
stream_consume_block(stream_t* s, usz_t amount) {
  if(s->pos + amount <= s->contents.size) {
    u8_t* ret = s->contents.data + s->pos;
    s->pos += amount;
    return ret;
  }
  return nullptr;
}

static u8_t*
stream_peek_block(stream_t* s, usz_t amount) {
  if(s->pos + amount <= s->contents.size) {
    u8_t* ret = s->contents.data + s->pos;
    return ret;
  }
  return nullptr;
}

static void
stream_write_block(stream_t* s, void* src, usz_t src_size) {
  assert(s->pos + src_size <= s->contents.size);
  copy_memory(s->contents.data + s->pos, src, src_size);
  s->pos += src_size; 
}

static void
stream_flush_bits(stream_t* s){
  s->bit_buffer = 0;
  s->bit_count = 0;
}

// Bits are consumed from LSB to MSB
static u32_t
stream_consume_bits(stream_t* s, u32_t amount){
  assert(amount <= 32);

  while(s->bit_count < amount) {
    u32_t byte = *stream_consume(u8_t, s);
    s->bit_buffer |= (byte << s->bit_count);
    s->bit_count += 8;
  }

  u32_t result = s->bit_buffer & ((1 << amount) - 1); 

  s->bit_count -= amount;
  s->bit_buffer >>= amount;

  return result;
}

//
// MARK:(WAV)
//
struct _wav_head_t { 
  u32_t id, size;
};
// http://soundfile.sapp.org/doc/Waveformat/

static b32_t 
wav_read(wav_t* w, buffer_t contents) 
{
  const static u32_t riff_id_signature = u32_endian_swap(0x52494646);
  const static u32_t riff_format_signature = u32_endian_swap(0x57415645);
  const static u32_t fmt_id_signature = u32_endian_swap(0x666d7420);
  const static u32_t data_id_signature = u32_endian_swap(0x64617461);

  make(stream_t, stream);
  stream_init(stream, contents);

  // NOTE(Momo): Load Riff Chunk
  wav_riff_chunk_t* riff_chunk = stream_consume(wav_riff_chunk_t, stream);
  if (!riff_chunk) {
    return 0;
  }
  if (riff_chunk->id != riff_id_signature) {
    return 0;
  }
  if (riff_chunk->format != riff_format_signature) {
    return 0;
  }

  // NOTE(Momo): Load fmt Chunk
  auto* fmt_chunk = stream_consume(wav_fmt_chunk_t, stream);
  if (!fmt_chunk) {
    return 0;
  }

  if (fmt_chunk->id != fmt_id_signature) {
    return 0;
  }
  if (fmt_chunk->size != 16) {
    return 0;
  }
  if (fmt_chunk->audio_format != 1) {
    return 0;
  }

  u32_t bytes_per_sample = fmt_chunk->bits_per_sample/8;
  if (fmt_chunk->byte_rate != 
    fmt_chunk->sample_rate * fmt_chunk->num_channels * bytes_per_sample) {
    return 0;
  }
  if (fmt_chunk->block_align != fmt_chunk->num_channels * bytes_per_sample) {
    return 0;
  }

  _wav_head_t* head;

  // Search until we find the 'data' chunk
  while(true) {
    head = stream_peek(_wav_head_t, stream);
    if (head == nullptr) {
      return 0;
    }

    if (head->id != data_id_signature) {
      stream_consume_block(stream, sizeof(_wav_head_t) + head->size);
    }
    else{
      break;
    }

  };


  // Load data Chunk
  auto* data_chunk = stream_consume(wav_data_chunk_t, stream);
  if (!data_chunk) {
    return 0;
  }


  // NOTE(momo): we don't endian swap anymore because we already did above
  // data_chunk->id = u32_endian_swap(data_chunk->id);
  if (data_chunk->id != data_id_signature) {
    return 0;
  }

  void* data = stream_consume_block(stream, data_chunk->size);
  if (!data) {
    return 0;
  }

  w->riff_chunk = (*riff_chunk);
  w->fmt_chunk = (*fmt_chunk);
  w->data_chunk = (*data_chunk);
  w->data = data;

  return 1;
}

//
// MARK:(TTF)
//

struct _ttf_glyph_point_t{
  s16_t x, y; 
  u8_t flags;
};

struct _ttf_glyph_outline_t{
  _ttf_glyph_point_t* points;
  u32_t point_count;

  u16_t* end_point_indices; // as many as contour_counts
  u32_t contour_count;
};

struct _ttf_glyph_paths_t{
  v2f_t* vertices;
  u32_t vertex_count;

  u32_t* path_lengths;
  u32_t path_count;
};


struct _ttf_edge_t{
  v2f_t p0, p1;
  b32_t is_inverted;
  f32_t x_intersect;
};

struct _ttf_edge_list_t{
  u32_t cap;
  u32_t count;
  _ttf_edge_t** e;
};

enum {
  _TTF_CMAP_PF_ID_UNICODE = 0,
  _TTF_CMAP_PF_ID_MACINTOSH = 1,
  _TTF_CMAP_PF_ID_RESERVED = 2,
  _TTF_CMAP_PF_ID_MICROSOFT = 3,

};

enum {
  _TTF_CMAP_MS_ID_SYMBOL = 0,
  _TTF_CMAP_MS_ID_UNICODE_BMP = 1,
  _TTF_CMAP_MS_ID_SHIFT_JIS = 2,
  _TTF_CMAP_MS_ID_PRC = 3,
  _TTF_CMAP_MS_ID_BIG_FIVE = 4,
  _TTF_CMAP_MS_ID_JOHAB = 5,
  _TTF_CMAP_MS_ID_UNICODE_FULL = 10,
};

static u16_t
_ttf_read_u16(u8_t* location) {
  return u16_endian_swap(*(u16_t*)location);
};

static s16_t
_ttf_read_s16(u8_t* location) {
  return u16_endian_swap(*(u16_t*)location);
};
static u32_t
_ttf_read_u32(u8_t* location) {
  return u32_endian_swap(*(u32_t*)location);
};

// returns 0 is failure
static u32_t
_ttf_get_offset_to_glyph(const ttf_t* ttf, u32_t glyph_index) {

  if(glyph_index >= ttf->glyph_count) return 0;

  u32_t g1 = 0, g2 = 0;
  switch(ttf->loca_format) {
    case 0: { // short format
      g1 = ttf->glyf + _ttf_read_u16(ttf->data + ttf->loca + glyph_index * 2) * 2;
      g2 = ttf->glyf + _ttf_read_u16(ttf->data + ttf->loca + glyph_index * 2 + 2) * 2;
    } break;
    case 1: { // long format
      g1 = ttf->glyf + _ttf_read_u32(ttf->data + ttf->loca + glyph_index * 4);
      g2 = ttf->glyf + _ttf_read_u32(ttf->data + ttf->loca + glyph_index * 4 + 4);
    } break;
    default: {
      return 0;
    }
  }

  return g1 == g2 ? 0 : g1;

}


static b32_t 
ttf_get_glyph_box(const ttf_t* ttf, u32_t glyph_index, s32_t* x0, s32_t* y0, s32_t* x1, s32_t* y1) {

  u32_t g = _ttf_get_offset_to_glyph(ttf, glyph_index);
  if (g <= 0) return false;

  if (x0) (*x0) = _ttf_read_s16(ttf->data + g + 2);
  if (y0) (*y0) = _ttf_read_s16(ttf->data + g + 4);
  if (x1) (*x1) = _ttf_read_s16(ttf->data + g + 6);
  if (y1) (*y1) = _ttf_read_s16(ttf->data + g + 8);
  return true;

}

static void
ttf_get_glyph_bitmap_box(const ttf_t* ttf, u32_t glyph_index, f32_t scale, s32_t* x0, s32_t* y0, s32_t* x1, s32_t* y1) {
  s32_t bx0, by0, bx1, by1;
  if (ttf_get_glyph_box(ttf, glyph_index, &bx0, &by0, &bx1, &by1)) {
    if(x0) (*x0) = (s32_t)(f32_floor((f32_t)bx0 * scale));
    if(y0) (*y0) = (s32_t)(f32_floor((f32_t)by0 * scale));
    if(x1) (*x1) = (s32_t)(f32_ceil((f32_t)bx1 * scale));
    if(y1) (*y1) = (s32_t)(f32_ceil((f32_t)by1 * scale));
  }
  else {
    if(x0) (*x0) = 0;
    if(y0) (*y0) = 0;
    if(x1) (*x1) = 0;
    if(y1) (*y1) = 0;
  }
}

static s32_t
_ttf_get_kern_advance(const ttf_t* ttf, s32_t g1, s32_t g2) 
{
  // NOTE(Momo): We only care about format 0, which Windows cares
  // For now, OSX has too many things to handle for this table 
  // and I am not going to care because I mostly develop in Windows.
  //
  if (!ttf->kern) return 0;

  u8_t* data = ttf->data + ttf->kern;

  // number of tables must be 1 or more
  if (_ttf_read_u16(data+2) < 1) return 0;

  // horizontal flag must be set
  if (_ttf_read_u16(data+8) != 1) return 0;

  // format must be 0
  //if ((_ttf_read_u16(data+8) & 0x0F) != 0) return 0;

  // We will be performing some kind of binary search
  s32_t l = 0;
  s32_t r = _ttf_read_u16(data+10) -1;

  // the value we are looking for
  u32_t needle = g1 << 16 | g2;   
  while(l <= r) {
    s32_t m = (l + r) >> 1; // half

    // 18 is where the kerning table is
    u32_t straw = _ttf_read_u32(data+18+(m*6));
    if (needle < straw) {
      r = m - 1;
    }
    else if (needle > straw) {
      l = m + 1;
    }
    else {
      return _ttf_read_u16(data+22+(m*6));
    }
  }

  return 0;
}

//~Glyph outline retrieval

// A glyph point's coordinate system's origin is at the bottom left.
// x moves towards the right, y moves towards the top
//
// y
// |
// | 
// ----x
//
static b32_t 
_ttf_get_glyph_outline(const ttf_t* ttf, 
                       _ttf_glyph_outline_t* outline,
                       u32_t glyph_index, 
                       arena_t* allocator) 
{
  u32_t g = _ttf_get_offset_to_glyph(ttf, glyph_index);
  s16_t number_of_contours = _ttf_read_s16(ttf->data + g + 0);


  if (number_of_contours > 0) { // single glyph case
    u16_t point_count = _ttf_read_u16(ttf->data + g + 10 + number_of_contours*2-2) + 1;
    u16_t instruction_length = _ttf_read_u16(ttf->data + g + 10 + number_of_contours*2);

    u32_t flags = g + 10 + number_of_contours*2 + 2 + instruction_length*2;

    // output end pts of contours
    //test_eval_d(number_of_contours);
    //test_eval_d(point_count);

    _ttf_glyph_point_t* points = arena_push_arr(_ttf_glyph_point_t, allocator, point_count);
    if (!points) return false;
    zero_range(points, point_count);
    u8_t* point_itr = ttf->data +  g + 10 + number_of_contours*2 + 2 + instruction_length;

    // Load the flags
    // flag info: https://docs.microsoft.com/en-us/typography/opentype/spec/glyf    
    {
      u8_t current_flags = 0;
      u8_t flag_count = 0;
      for (u32_t i = 0; i < point_count; ++i) {
        if (flag_count == 0) {
          current_flags = *point_itr++;
          if (current_flags & 0x8) {
            flag_count = *point_itr++;
          }
        }
        else {
          --flag_count;
        }
        points[i].flags = current_flags;
      }
    }


    // Load the x coordinates
    {
      s16_t x = 0;
      for (u32_t i = 0; i < point_count; ++i ){
        flags = points[i].flags;
        if (flags & 0x2) {
          // if this is set, corresponding x-coordinate is 1 byte long
          // and the sign is determined by 0x10
          s16_t dx = (s16_t)*point_itr++;
          x += (flags & 0x10) ? dx : -dx;            
        }
        else {
          // if this is not set, then...
          if (flags & 0x10) {
            // if this is set, then this x-coord is same as prev x-coord
            // i.e. we do nothing
          }
          else {
            // otherwise, this is 2 bytes long and intepreted as s16_t
            x += _ttf_read_s16(point_itr);
            point_itr += 2;
          }

        }
        points[i].x = x;
      }
    }

    // Load the y coordinates
    {
      s16_t y = 0;
      for (u32_t i = 0; i < point_count; ++i ){
        flags = points[i].flags;
        if (flags & 0x4) {
          // if this is set, corresponding y-coordinate is 1 byte long
          // and the sign is determined by 0x10
          s16_t dy = (s16_t)*point_itr++;
          y += (flags & 0x20) ? dy : -dy;            
        }
        else {
          // if this is not set, then...
          if (flags & 0x20) {
            // if this is set, then this y-coord is same as prev y-coord
            // i.e. we do nothing
          }
          else {
            // otherwise, this is 2 bytes long and intepreted as s16_t
            y += _ttf_read_s16(point_itr);
            point_itr += 2;
          }

        }
        points[i].y = y;
      }
    }

    // mark the points that are contour endpoints
    u16_t* end_pt_indices = arena_push_arr(u16_t, allocator, number_of_contours);
    if (!end_pt_indices) return false;
    zero_range(end_pt_indices, number_of_contours); 
    {
      u32_t contour_end_pts = g + 10; 
      for (s16_t i = 0; i < number_of_contours; ++i) {
        end_pt_indices[i] =_ttf_read_u16(ttf->data + contour_end_pts + i*2) ;
      }
    }

    outline->points = points; 
    outline->point_count = point_count;
    outline->end_point_indices = end_pt_indices;
    outline->contour_count = number_of_contours;

    return 1;
  }

  else if (number_of_contours < 0) { // compound glyph case
    return 0;
  }
  else { //contour_count == 0
    return 0;
  } 
}

//
// Glyph path generation
//
static void
_ttf_add_vertex(v2f_t* vertices, u32_t n, f32_t x, f32_t y) {
  if (!vertices) return;
  vertices[n].x = x;
  vertices[n].y = y;
}

static void
_ttf_add_vertex(v2f_t* vertices, u32_t n, v2f_t v) {
  if (!vertices) return;
  vertices[n] = v;
}


static void
_ttf_tessellate_bezier(v2f_t* vertices,
                       u32_t* vertex_count,
                       v2f_t p0, 
                       v2f_t p1,
                       v2f_t p2,
                       f32_t flatness_squared,
                       u32_t n) 
{
  v2f_t mid = (p0 + p1*2.f + p2) * 0.25f;
  v2f_t d = v2f_mid(p0, p2) - mid;

  // if n == 16, that's 65535 segments which should be
  // more than enough. Increase this number if we are 
  // looking at abnormally large images...?
  if (n > 16) { return; }

  if (d.x*d.x + d.y*d.y > flatness_squared) {
    _ttf_tessellate_bezier(vertices, vertex_count, p0,
                           v2f_mid(p0, p1), 
                           mid, flatness_squared, n+1 );
    _ttf_tessellate_bezier(vertices, vertex_count, mid,
                           v2f_mid(p1, p2), 
                           p2, flatness_squared, n+1 );
  }
  else {
    _ttf_add_vertex(vertices, (*vertex_count)++, p2);      
  }



}

  static b32_t 
    _ttf_get_paths_from_glyph_outline(_ttf_glyph_outline_t* outline,
        _ttf_glyph_paths_t* paths,
        arena_t* allocator) 
    {
      // Count the amount of points generated
      v2f_t* vertices = 0;
      u32_t vertex_count = 0;
      f32_t flatness = 0.35f;
      f32_t flatness_squared = flatness*flatness;

      u32_t* path_lengths = arena_push_arr(u32_t, allocator, outline->contour_count);
      if (!path_lengths) return false;
      zero_range(path_lengths, outline->contour_count);
      u32_t path_count = 0;

      // On the first pass, we count the number of points we will generate.
      // On the second pass, we will push the list and actually fill 
      // the list with generated points.
      for (u32_t pass = 0; pass < 2; ++pass)
      {
        if (pass == 1) {
          vertices = arena_push_arr(v2f_t, allocator, vertex_count);
          if (!vertices) return false;
          zero_range(vertices, vertex_count);
          vertex_count = 0;
          path_count = 0;
        }

        // NOTE(Momo): For now, we assume that the first point is 
        // always on curve, which is not always the case.
        v2f_t anchor_pt = {};
        u32_t j = 0;
        for (u32_t i = 0; i < outline->contour_count; ++i) {
          u32_t contour_start_index = j;
          u32_t start_vertex_count = vertex_count;

          for(; j <= outline->end_point_indices[i]; ++j) {
            u8_t flags = outline->points[j].flags;

            if (flags & 0x1) { // on curve 
              anchor_pt.x = (f32_t)outline->points[j].x;
              anchor_pt.y = (f32_t)outline->points[j].y;

              _ttf_add_vertex(vertices, vertex_count++, anchor_pt);
            }
            else{ // not on curve
              u32_t next_index = (j == outline->end_point_indices[i]) ? contour_start_index : j+1;

              // Check if next point is on curve
              v2f_t p0 = anchor_pt;
              v2f_t p1 = { (f32_t)outline->points[j].x, (f32_t)outline->points[j].y };
              v2f_t p2 = { (f32_t)outline->points[next_index].x, (f32_t)outline->points[next_index].y } ;

              u8_t next_flags = outline->points[next_index].flags;
              if (!(next_flags & 0x1)) {
                // not on curve, thus it's a cubic curve, 
                // so we have to generate midpoint
                p2.x = p1.x + (p2.x - p1.x)*0.5f;
                p2.y = p1.y + (p2.y - p1.y)*0.5f;
              }
#if 0
              // prevent duplicates?
              else {
                ++j;
              }
#endif
              _ttf_tessellate_bezier(vertices, &vertex_count,
                  p0, p1, p2, flatness_squared, 0);
              anchor_pt = p2;
            }
          }
          path_lengths[path_count++] = vertex_count - start_vertex_count;
        }
      }
      paths->vertices = vertices;
      paths->vertex_count = vertex_count;
      paths->path_lengths = path_lengths;
      paths->path_count = path_count;

      return true;

    }

  static u32_t
    ttf_get_glyph_index(const ttf_t* ttf, u32_t codepoint) {

      u16_t format = _ttf_read_u16(ttf->data + ttf->cmap_mappings + 0);

      switch(format) {
        case 4: { // 
          u16_t seg_count = _ttf_read_u16(ttf->data + ttf->cmap_mappings + 6) >> 1;
          //u16_t search_range = _ttf_read_u16(ttf->data + ttf->cmap_mappings + 8) >> 1;
          //u16_t entry_selector = _ttf_read_u16(ttf->data + ttf->cmap_mappings + 10);
          //u16_t range_shift = _ttf_read_u16(ttf->data + ttf->cmap_mappings + 12) >> 1;

          u32_t end_codes = ttf->cmap_mappings + 14;
          u32_t start_codes = end_codes + 2 + (2*seg_count);
          u32_t id_deltas = start_codes + (2*seg_count);
          u32_t id_range_offsets = id_deltas + (2*seg_count);
          //u32_t glyph_index_array = id_range_offsets + (2*seg_count);

          if (codepoint == 0xffff) return 0;

          // Find the first end code that is greater than or equal to the codepoint
          //
          // NOTE(Momo): To optimize this, we could do a binary search based
          // on the data given but there are some documentations that seem
          // to suggest against this...
          // 
          u16_t seg_id = 0;
          u16_t end_code = 0;
          for(u16_t i = 0; i < seg_count; ++i) {
            end_code = _ttf_read_u16(ttf->data + end_codes + (2 * i));
            if( end_code >= codepoint ){
              seg_id = i;
              break;
            }
          }

          u16_t start_code = _ttf_read_u16(ttf->data + start_codes + 2*seg_id);

          if (start_code > codepoint) return 0;

          u16_t offset = _ttf_read_u16(ttf->data + id_range_offsets + 2*seg_id);
          s16_t delta = _ttf_read_s16(ttf->data + id_deltas + 2*seg_id);

          if (offset == 0 ){
            return codepoint + delta;
          }
          else {
            return _ttf_read_u16(ttf->data +
                id_range_offsets + 2*seg_id + // &id_range_offset[i]
                offset + (codepoint - start_code)*2);

          }

        } break;

        default: {
          return 0; // invalid codepoint
        }
      }
    }


static f32_t
    ttf_get_scale_for_pixel_height(const ttf_t* ttf, f32_t pixel_height) {
      s32_t font_height = _ttf_read_s16(ttf->data + ttf->hhea + 4) - _ttf_read_s16(ttf->data + ttf->hhea + 6);
      return (f32_t)pixel_height/font_height;
    }


  static void 
    ttf_get_glyph_vertical_metrics(const ttf_t* ttf, s16_t* ascent, s16_t* descent, s16_t* line_gap) 
    {
      if (ascent) *ascent = _ttf_read_s16(ttf->data + ttf->hhea + 4);
      if (descent) *descent = _ttf_read_s16(ttf->data + ttf->hhea + 6);
      if (line_gap) *line_gap = _ttf_read_s16(ttf->data + ttf->hhea + 8);
    }

  static void 
    ttf_get_glyph_horizontal_metrics(const ttf_t* ttf, 
        u32_t glyph_index, 
        s16_t* advance_width, 
        s16_t* left_side_bearing)
    {
      u16_t num_of_long_horizontal_metrices = _ttf_read_u16(ttf->data + ttf->hhea + 34);

      if (glyph_index < num_of_long_horizontal_metrices) {
        if (advance_width) (*advance_width) = _ttf_read_s16(ttf->data + ttf->hmtx + 4*glyph_index);
        if (left_side_bearing) (*left_side_bearing) = _ttf_read_s16(ttf->data + ttf->hmtx + 4*glyph_index + 2);
      }
      else {
        if(advance_width) (*advance_width) = _ttf_read_s16(ttf->data + ttf->hmtx + 4*(num_of_long_horizontal_metrices-1));
        if(left_side_bearing) (*left_side_bearing) = _ttf_read_s16(ttf->data + ttf->hmtx + 4*num_of_long_horizontal_metrices + 2*(glyph_index - num_of_long_horizontal_metrices));
      }

    }


  static b32_t
    ttf_read(ttf_t* ttf, buffer_t ttf_contents) {
      ttf->data = ttf_contents.data;

      u32_t num_tables = _ttf_read_u16(ttf->data + 4);

      for (u32_t i= 0 ; i < num_tables; ++i ) {
        u32_t directory = 12 + (16 * i);
        u32_t tag = _ttf_read_u32(ttf->data + directory + 0);


        switch(tag) {
          case 'loca': {
            ttf->loca = _ttf_read_u32(ttf->data + directory + 8);
          }; break;
          case 'head': {
            ttf->head = _ttf_read_u32(ttf->data + directory + 8);
          }; break;
          case 'glyf': {
            ttf->glyf = _ttf_read_u32(ttf->data + directory + 8);
          }; break;
          case 'maxp': {
            ttf->maxp = _ttf_read_u32(ttf->data + directory + 8);
          } break;
          case 'cmap': {
            ttf->cmap = _ttf_read_u32(ttf->data + directory + 8);
          } break;
          case 'hhea': {
            ttf->hhea = _ttf_read_u32(ttf->data + directory + 8);
          } break;
          case 'hmtx': {
            ttf->hmtx = _ttf_read_u32(ttf->data + directory + 8);
          } break;
          case 'kern': {
            ttf->kern = _ttf_read_u32(ttf->data + directory + 8);
          } break;
          case 'GPOS': {
            ttf->gpos = _ttf_read_u32(ttf->data + directory + 8);
          } break;
          default: {
#if 0
            char* tags = (char*)&tag;
            test_log("found: %c%c%c%c\n", tags[3], tags[2], tags[1], tags[0]);
#endif
          };
        }

      }

      if (!ttf->loca || 
          !ttf->maxp ||
          !ttf->head ||
          !ttf->glyf ||
          !ttf->cmap ||
          !ttf->hhea ||
          !ttf->hmtx) return false;

      ttf->loca_format = _ttf_read_u16(ttf->data + ttf->head + 50);
      if (ttf->loca_format >= 2) return false;

      ttf->glyph_count = _ttf_read_u16(ttf->data + ttf->maxp + 4);

      // Get index map
      {
        u32_t subtable_count = _ttf_read_u16(ttf->data + ttf->cmap + 2);

        b32_t found_index_table = false;

        for( u32_t i = 0; i < subtable_count; ++i) {
          u32_t subtable = ttf->cmap + 4 + (8 * i);


          // We only support unicode encoding...
          // NOTE(Momo): They say mac is discouraged, so we won't care about it.
          u32_t pf_id = _ttf_read_u16(ttf->data + subtable + 0);
          switch(pf_id) {
            case _TTF_CMAP_PF_ID_MICROSOFT: {
              u32_t pf_specific_id = _ttf_read_u16(ttf->data + subtable + 2);
              switch(pf_specific_id) {
                case _TTF_CMAP_MS_ID_UNICODE_BMP:
                case _TTF_CMAP_MS_ID_UNICODE_FULL: {
                  ttf->cmap_mappings = ttf->cmap + _ttf_read_u32(ttf->data + subtable + 4);
                  found_index_table =  true;
                }break;

              }
            }
            case _TTF_CMAP_PF_ID_UNICODE: {
              ttf->cmap_mappings = ttf->cmap + _ttf_read_u32(ttf->data + subtable + 4);
              found_index_table = true;
            } break;

          }

          if (found_index_table) break;
        }

        if (!found_index_table) 
          return false;
      }



      return true;
    }

  static s32_t 
    ttf_get_glyph_kerning(const ttf_t* ttf, u32_t glyph_index_1, u32_t glyph_index_2) {

      if (ttf->gpos) {
        assert(false);
        //return _ttf_get_gpos_advance(ttf, glyph_index_1, glyph_index_2);
      }
      else if (ttf->kern) {
        return _ttf_get_kern_advance(ttf, glyph_index_1, glyph_index_2);
      }
      return 0;
    }

  static u32_t* 
    ttf_rasterize_glyph(const ttf_t* ttf, u32_t glyph_index, f32_t scale, u32_t* out_w, u32_t* out_h, arena_t* allocator) 
    {
      u32_t* pixels = 0;
      make(_ttf_glyph_outline_t, outline);
      make(_ttf_glyph_paths_t, paths);

      s32_t x0, y0, x1, y1;
      ttf_get_glyph_bitmap_box(ttf, glyph_index, scale, &x0, &y0, &x1, &y1);

      u32_t width = x1 - x0;
      u32_t height = y1 - y0;
      u32_t size = width * height * 4;

      if (width == 0 || height == 0) {
        //ttf_log("[ttf] Glyph dimension are bad\nj");
        return nullptr;
      }

      pixels = arena_push_arr(u32_t, allocator, size);
      if (!pixels) {
        //ttf_log("[ttf] Unable to push bitmap pixel\n");
        return nullptr;
      }
      zero_memory(pixels, size);

      arena_set_revert_point(allocator);

      if(!_ttf_get_glyph_outline(ttf, outline, glyph_index, allocator)) {
        //ttf_log("[ttf] Unable to get glyph outline\n");
        return nullptr;
      }
      if (!_ttf_get_paths_from_glyph_outline(outline, paths, allocator)) {
        //ttf_log("[ttf] Unable glyph paths\n");
        return nullptr;
      }

      // generate scaled edges based on points
      _ttf_edge_t* edges = arena_push_arr(_ttf_edge_t, allocator, paths->vertex_count);
      if (!edges) {
        //ttf_log("[ttf] Unable to push edges\n");
        return nullptr;
      }
      zero_range(edges, paths->vertex_count);

      u32_t edge_count = 0;
      {
        u32_t vertex_index = 0;
        for (u32_t path_index = 0; 
            path_index < paths->path_count; 
            ++path_index)
        {
          u32_t path_length = paths->path_lengths[path_index];
          for (u32_t i = 0; i < path_length; ++i) {
            _ttf_edge_t edge = {};
            v2f_t v0 = paths->vertices[vertex_index];
            v2f_t v1 = (i == path_length-1) ? paths->vertices[vertex_index-i] : paths->vertices[vertex_index+1];
            ++vertex_index;

            // Skip if edge is going to be completely horizontal
            if (v0.y == v1.y) {
              continue;
            }

            edge.p0.x = v0.x * scale - x0;
            edge.p0.y = height - ((v0.y * scale) - y0);

            edge.p1.x = v1.x * scale - x0;
            edge.p1.y = height - ((v1.y * scale) - y0);

            // Check if edge's points need to be flipped.
            // NOTE(Momo): It's easier for the rasterization algorithm to have the edges'
            // p0 be on top of p1. If we flip, we will indicate it within the edge.
            if (edge.p0.y > edge.p1.y) {
              swap(edge.p0, edge.p1);
              edge.is_inverted = true;
            }
            edges[edge_count++] = edge;
          }
        }  
      }


      // Rasterazation algorithm starts here
      // Sort edges by top most edge
      sort_entry_t* y_edges = arena_push_arr(sort_entry_t, allocator, edge_count);
      if (!y_edges) { 
        //ttf_log("[ttf] Unable to push sort entries for edges\n");
        return nullptr;
      }

      for (u32_t i = 0; i < edge_count; ++i) {
        y_edges[i].index = i;
        y_edges[i].key = -(f32_t)max_of(edges[i].p0.y, edges[i].p1.y);
      }
      quicksort(y_edges, edge_count);

      sort_entry_t* active_edges = arena_push_arr(sort_entry_t, allocator, edge_count);
      if (!active_edges) {
        //ttf_log("[ttf] Unable to push sort entries for active edges\n");
        return nullptr;
      }

      // NOTE(Momo): Currently, I'm lazy, so I'll just keep 
      // clearing and refilling the active_edges list per scan line
      for(u32_t y = 0; y <= height; ++y) {
        u32_t act_edge_count = 0; 
        f32_t yf = (f32_t)y; // 'center' of pixel

        // Add to 'active edge list' any edges which have an 
        // uppermost vertex (p0) before y and lowermost vertex (p1) after this y.
        // Also, ignore p1 that ends EXACTLY on this y.
        for (u32_t y_edge_id = 0; y_edge_id < edge_count; ++y_edge_id){
          _ttf_edge_t* edge = edges + y_edges[y_edge_id].index;

          if (edge->p0.y <= yf && edge->p1.y > yf) {
            // calculate the x intersection
            f32_t dx = edge->p1.x - edge->p0.x;
            f32_t dy = edge->p1.y - edge->p0.y;
            if (dy != 0.f) {
              f32_t t = (yf - edge->p0.y) / dy;
              edge->x_intersect = edge->p0.x + (t * dx);

              // prepare sort_entry_t for active_edges
              active_edges[act_edge_count].index = y_edges[y_edge_id].index;
              active_edges[act_edge_count].key = edge->x_intersect;

              ++act_edge_count;
            }
          }
        }
        quicksort(active_edges, act_edge_count);

        if (act_edge_count >= 2) {
          u32_t crossings = 0;
          for (u32_t act_edge_id = 0; 
              act_edge_id < act_edge_count-1;
              ++act_edge_id) 
          {
            _ttf_edge_t* start_edge = edges + active_edges[act_edge_id].index; 
            _ttf_edge_t* end_edge = edges + active_edges[act_edge_id+1].index; 

            start_edge->is_inverted ? ++crossings : --crossings;

            if (crossings > 0) {
              u32_t start_x = (u32_t)start_edge->x_intersect;
              u32_t end_x = (u32_t)end_edge->x_intersect;
              for(u32_t x = start_x; x < end_x; ++x) {
                pixels[x + y * width] = 0xFFFFFFFF;
              }
            }
          }
        }

#if 0 
        // Draw edges in green
        for (u32_t i =0 ; i < edge_count; ++i) 
        {
          _ttf_edge_t* edge = edges + i;
          f32_t ex0 = edge->p0.x;
          f32_t ey0 = edge->p0.y;

          f32_t ex1 = edge->p1.x;
          f32_t ey1 = edge->p1.y;

          f32_t dx = (ex1 - ex0)/100;
          f32_t dy = (ey1 - ey0)/100;

          f32_t xx = ex0;
          f32_t yy = ey0;
          for (u32_t z = 0; z < 100; ++z) {
            xx += dx;
            yy += dy;
            pixels[(u32_t)xx + (u32_t)yy * width] = 0xFF00FF00;      
          }
        }
#endif


      }


#if 0
      // Draw vertices in red

      for (u32_t i =0 ; i < edge_count; ++i) 
      {
        auto* edge = edges + i;
        u32_t x0 = (u32_t)edge->p0.x;
        u32_t y0 = (u32_t)edge->p0.y;
        pixels[x0 + y0 * bitmap_dims.w] = 0xFF0000FF;


        u32_t x1 = (u32_t)edge->p1.x;
        u32_t y1 = (u32_t)edge->p1.y;
        pixels[x1 + y1 * bitmap_dims.w] = 0xFF0000FF;

      }
#endif

      if (out_w) *out_w = width;
      if (out_h) *out_h = height;

      return pixels;
    }

  // 
  // MARK:(PNG)
  //

  // We are only interested in 4-channel images in rgba_t format
#define _PNG_CHANNELS 4 


struct _png_context_t {
  stream_t stream;
  arena_t* arena; 

  stream_t image_stream;
  u32_t image_width;
  u32_t image_height;

  stream_t unfiltered_image_stream; // for filtering and deflating

  // other useful info
  u32_t bit_depth;

  stream_t compressed_image_stream;
};


struct _png_chunk_t {
  u8_t signature[8];
}; 

// 5.3 Chunk layout
// | length | type | data | CRC
struct _png_chunk_header_t {
  u32_t length;
  union {
    u32_t type_U32;
    u8_t type[4];
  };
};


#pragma pack(push, 1)
struct _png_ihdr_t {
  u32_t width;
  u32_t height;
  u8_t bit_depth;
  u8_t colour_type;
  u8_t compression_method;
  u8_t filter_method;
  u8_t interlace_method;
};
#pragma pack(pop)

struct _png_chunk_footer_t {
  u32_t crc; 
};

// ZLIB header notes:
// Bytes[0]:
// - compression flags bit 0-3: Compression Method (CM)
// - compression flags bit 4-7: Compression Info (CINFO)
// Bytes[1]:
// - additional flags bit 0-4: FCHECK 
// - additional flags bit 5: Preset dictionary (FDICT)
// - additional flags bit 6-7: Compression level (FLEVEL)
struct _png_idat_header_t {
  u8_t compression_flags;
  u8_t additional_flags;
};


struct _png_huffman_t {
  // Canonical ordered symbols
  u16_t* symbols; 
  u32_t symbol_count;

  // Number of symbols per length
  // i.e. code_lengths[1] is the number of symbols with length 1.
  u16_t* lengths;
  u32_t length_count;
};

// Modified from Annex D of png_t specification:
// https://www.w3.org/TR/2003/REC-png_t-20031110/#D-CRCAppendix
// crc variable indicates the starting register
static u32_t
_png_calculate_crc32(u8_t* data, u32_t data_size) {
  static const u32_t crc_table[256] =
    {
      0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
      0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
      0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
      0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
      0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
      0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
      0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
      0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
      0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
      0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
      0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
      0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
      0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
      0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
      0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
      0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
      0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
      0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
      0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
      0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
      0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
      0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
      0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
      0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
      0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
      0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
      0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
      0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
      0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
      0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
      0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
      0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d,
    };

  u32_t r = 0xFFFFFFFFL;
  for (u32_t i = 0; i < data_size; ++i) {
    r = crc_table[(r ^ data[i]) & 0xFF] ^ (r >> 8);
  }
  return r ^ 0xFFFFFFFFL;
}




static s32_t
_png_huffman_decode(stream_t* src_stream, _png_huffman_t huffman) {
  s32_t code = 0;
  s32_t first = 0;
  s32_t index = 0;

  for (u32_t len = 1; 
  len <= huffman.length_count - 1; 
  ++len) 
  {
    u32_t bits = stream_consume_bits(src_stream, 1);
    code |= bits;
    s32_t count = huffman.lengths[len];
    if(code - count < first) {
      return huffman.symbols[index + (code - first)];
    }
    index += count;
    first += count;
    first <<= 1;
    code <<= 1;
  }

  return -1;
}

// NOTE(Momo): 
// https://datatracker.ietf.org/doc/html/rfc1951
// Section 3.2.2
static void
_png_huffman_compute(_png_huffman_t* h,
                     arena_t* arena, 
                     u16_t* codes,
                     u32_t codes_size, 
                     u32_t max_lengths) 
{
  // Each code corresponds to a symbol
  h->symbol_count = codes_size;
  h->symbols = arena_push_arr(u16_t, arena, codes_size);
  zero_memory(h->symbols, h->symbol_count * sizeof(u16_t));


  // We add +1 because lengths[0] is not possible
  h->length_count = max_lengths + 1;
  h->lengths = arena_push_arr(u16_t, arena, max_lengths + 1);
  zero_memory(h->lengths, h->length_count * sizeof(u16_t));

  // 1. Count the number of codes for each code length
  for (u32_t sym = 0; sym < codes_size; ++sym)  {
    u16_t len = codes[sym];
    ++h->lengths[len];
  }

  // 2. Numerical value of smallest code for each code length
  arena_marker_t mark = arena_mark(arena);

  u16_t* len_offset_table = arena_push_arr(u16_t, arena, max_lengths+1);
  zero_memory(len_offset_table, (max_lengths+1) * sizeof(u16_t));

  for (u32_t len = 1; len < max_lengths; ++len) {
    len_offset_table[len+1] = len_offset_table[len] + h->lengths[len]; 
  }

  // 3. Assign numerical values to all codes
  for (u32_t sym = 0; sym < codes_size; ++sym)
  {
    u16_t len = codes[sym];
    if (len > 0) {
      u16_t code = len_offset_table[len]++;
      h->symbols[code] = (u16_t)sym;
    }
  }
  arena_revert(mark); 

}


static b32_t
_png_deflate(stream_t* src_stream, stream_t* dest_stream, arena_t* arena) 
{

  static const u16_t lens[29] = { /* Size base for length codes 257..285 */
    3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
    35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258};
  static const u16_t len_ex_bits[29] = { /* Extra bits for length codes 257..285 */
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
    3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};
  static const u16_t dists[30] = { /* Offset base for distance codes 0..29 */
    1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
    257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
    8193, 12289, 16385, 24577};
  static const u16_t dist_ex_bits[30] = { /* Extra bits for distance codes 0..29 */
    0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
    7, 7, 8, 8, 9, 9, 10, 10, 11, 11,
    12, 12, 13, 13 };


  u8_t BFINAL = 0;
  while(BFINAL == 0){
    arena_set_revert_point(arena);

    BFINAL = (u8_t)stream_consume_bits(src_stream, 1);
    u16_t BTYPE = (u8_t)stream_consume_bits(src_stream, 2);
    switch(BTYPE) {
      case 0b00: {
        stream_flush_bits(src_stream);

        stream_consume_bits(src_stream, 5);
        u16_t LEN = (u16_t)stream_consume_bits(src_stream, 16);
        u16_t NLEN = (u16_t)stream_consume_bits(src_stream, 16);
        if ((u16_t)LEN != ~((u16_t)(NLEN))) {
          return false; 
        }
        // TODO(Momo): support this type?
        return false;
      } break;
      case 0b01: 
      case 0b10: {
        _png_huffman_t lit_huffman = {};
        _png_huffman_t dist_huffman = {};

        if (BTYPE == 0b01) {
          // Fixed huffman
          u16_t lit_codes[288] = {};
          u16_t dist_codes[32] = {};

          u32_t lit = 0;
          for (; lit < 144; ++lit) {
            lit_codes[lit] = 8;
          }
          for (; lit < 256; ++lit) {
            lit_codes[lit] = 9;
          }
          for (; lit < 280; ++lit) {
            lit_codes[lit] = 7;
          }
          for (; lit < array_count(lit_codes); ++lit) {
            lit_codes[lit] = 8;
          }
          for (lit = 0; lit < array_count(dist_codes); ++lit) {
            dist_codes[lit] = 5;
          }


          _png_huffman_compute(&lit_huffman,
                               arena, 
                               lit_codes, 
                               array_count(lit_codes),
                               15);
          _png_huffman_compute(&dist_huffman,
                               arena,
                               dist_codes,
                               array_count(dist_codes),
                               15);

        }
        else // BTYPE == 0b10
        {
          u32_t HLIT = stream_consume_bits(src_stream, 5) + 257;
          u32_t HDIST = stream_consume_bits(src_stream, 5) + 1;
          u32_t HCLEN = stream_consume_bits(src_stream, 4) + 4;

          static const u32_t order[] = {
            16, 17, 18, 0, 8 ,7, 9, 6, 10, 5, 
            11, 4, 12, 3, 13, 2, 14, 1, 15,
          };

          u16_t code_codes[19] = {};

          for(u32_t i = 0; i < HCLEN; ++i) {
            code_codes[order[i]] = (u16_t)stream_consume_bits(src_stream, 3);
          }

          _png_huffman_t code_huffman = {};
          _png_huffman_compute(&code_huffman,
                               arena,
                               code_codes,
                               array_count(code_codes),
                               15); 


          u16_t* lit_dist_codes = arena_push_arr(u16_t, arena, HDIST + HLIT);

          // NOTE(Momo): Decode
          // Loop until end of block code recognize
          for(u32_t i = 0; i < (HDIST + HLIT);) {

            s32_t sym = _png_huffman_decode(src_stream, code_huffman);

            if(sym >= 0 && sym <= 15) {
              lit_dist_codes[i++] = (u16_t)sym;
            }
            else 
            {	
              u32_t times_to_repeat = 0;
              u16_t code_to_repeat = 0;
              if (sym == 16) {
                // Copy the previous code length 3-6 times
                if (i == 0) return false;

                times_to_repeat = 3 + stream_consume_bits(src_stream, 2);
                code_to_repeat = lit_dist_codes[i-1];

              }

              else if (sym == 17) {
                // Repeat a code length of 0 for 3-10 times
                times_to_repeat = 3 + stream_consume_bits(src_stream, 3);
              }
              else if (sym == 18) {
                // Repeat a code length of 0 for 11-138 times
                times_to_repeat = 11 + stream_consume_bits(src_stream, 7);
              }
              else {
                // Invalid symbol
                return false;
              }

              while(times_to_repeat--) {
                lit_dist_codes[i++] = code_to_repeat;
              }

            }

          }

          _png_huffman_compute(&lit_huffman,
                               arena, 
                               lit_dist_codes, 
                               HLIT,
                               15);
          _png_huffman_compute(&dist_huffman,
                               arena,
                               lit_dist_codes + HLIT,
                               HDIST,
                               15);					
        }

        static int pass =0;
        ++pass;

        // NOTE(Momo): Actual decoding
        for (;;) 
        {

          s32_t sym = _png_huffman_decode(src_stream, lit_huffman);
          if (pass == 2) {
            //test_log("%d\n", sym);
          }
          //_png_log("sym: %d\n", sym);

          // NOTE(Momo): Normal case
          if (sym <= 255) { 
            u8_t byte_to_write = (u8_t)(sym & 0xFF); 
            stream_write(dest_stream, byte_to_write);
          }
            // NOTE(Momo): Extra code case
          else if (sym >= 257) {

            sym -= 257;
            if (sym >= 29) {
              return false;
            }
            u32_t len = lens[sym];
            if (len_ex_bits[sym]) len += stream_consume_bits(src_stream, len_ex_bits[sym]);

            sym = _png_huffman_decode(src_stream, dist_huffman);
            if (sym < 0) return false;

            u32_t dist = dists[sym];
            if (dist_ex_bits[sym]) dist += stream_consume_bits(src_stream, dist_ex_bits[sym]);


            // test_log("%d\n", len);
            while(len--) {
              usz_t target_index = dest_stream->pos - dist;
              u8_t byte_to_write = dest_stream->contents.data[target_index];
              stream_write(dest_stream, byte_to_write);
            }
          }
          else { 
            // sym == 256
            break;
          }
        }
      } break;
      default: {
        return false;
      }
    }
  }
  return true;
}




static u32_t 
_png_get_channels_from_colour_type(u32_t colour_type) {
  // NOTE(Momo): Determine the channels
  // colour_type 1 = Pallete used
  // colour_type 2 = Colour used 
  // colour_type 4 = alpha used
  switch(colour_type){
    case 0: {
      return 1; // Grayscale
    } break;
    case 2: {
      return 3; // rgb_t
    } break;
    case 3: { // Palette
      return 0;
    } break;
    case 4: {
      return 2; // Grayscale + alpha
    } break;
    case 6: { 
      return 4; // rgba_t
    } break;
    default: {
      return 0;
    }
  }
}

static b32_t
_png_is_format_supported(_png_ihdr_t* IHDR){
  if (IHDR->colour_type != 6 ||
    IHDR->bit_depth != 8 ||
    IHDR->compression_method != 0 ||
    IHDR->filter_method != 0 ||
    IHDR->interlace_method != 0) 
  {

    return false;
  }
  return true;
}

static b32_t
_png_is_signature_valid(u8_t* comparee) {
  static const u8_t signature[] = { 
    137, 80, 78, 71, 13, 10, 26, 10 
  };

  for (u32_t i = 0; i < array_count(signature); ++i) {
    if (signature[i] != comparee[i]) {
      return false;
    }
  }

  return true;
}

//~ NOTE(Momo): Filtering
static b32_t
_png_filter_none(_png_context_t* c) {
  u32_t bpl = c->image_width * _PNG_CHANNELS; // bytes per line
  for (u32_t i = 0; i < bpl; ++i ){
    u8_t* pixel_byte = stream_consume(u8_t, &c->unfiltered_image_stream);
    if (pixel_byte == nullptr) {
      return false;
    }
    stream_write(&c->image_stream, *pixel_byte);
  }
  return true;
}

static b32_t
_png_filter_sub(_png_context_t* c) {
  u32_t bpp = _PNG_CHANNELS; // bytes per pixel
  u32_t bpl = c->image_width * _PNG_CHANNELS; // bytes per line
  for (u32_t i = 0; i < bpl; ++i ){

    u8_t* pixel_byte_p = stream_consume(u8_t, &c->unfiltered_image_stream);
    if (pixel_byte_p == nullptr)return false;

    u8_t pixel_byte = (*pixel_byte_p); // sub(x)
    if (i < bpp) {
      stream_write(&c->image_stream, pixel_byte);
    }
    else {
      usz_t current_index = c->image_stream.pos;
      u8_t left_reference = c->image_stream.contents.data[current_index - bpp]; // Raw(x-bpp)
      u8_t pixel_byte_to_write = (pixel_byte + left_reference) % 256;  

      stream_write(&c->image_stream, pixel_byte_to_write);
    }

  }    

  return true;
}

static b32_t
_png_filter_average(_png_context_t* c) {
  u32_t bpp = _PNG_CHANNELS; // bytes per pixel
  u32_t bpl = c->image_width * _PNG_CHANNELS; // bytes per line

  for (u32_t i = 0; i < bpl; ++i ){

    u8_t* pixel_byte_p = stream_consume(u8_t, &c->unfiltered_image_stream);
    if (pixel_byte_p == nullptr) return false;

    u8_t pixel_byte = (*pixel_byte_p); // sub(x)

    usz_t current_index = c->image_stream.pos;
    u8_t left = (i < bpp) ? 0 :  c->image_stream.contents.data[current_index - bpp]; // Raw(x-bpp)
    u8_t top = (current_index < bpl) ? 0 : c->image_stream.contents.data[current_index - bpl]; // Prior(x)

    // NOTE(Momo): Formula uses floor((left+top)/2). 
    // Integer Truncation should do the job!
    u8_t pixel_byte_to_write = (pixel_byte + (left + top)/2) % 256;  

    stream_write(&c->image_stream, pixel_byte_to_write);
  }


  return true;
}

static b32_t
_png_filter_paeth(_png_context_t* cx) {
  u32_t bpp = _PNG_CHANNELS; // bytes per pixel
  u32_t bpl = cx->image_width * _PNG_CHANNELS; // bytes per line

  for (u32_t i = 0; i < bpl; ++i ){
    u8_t* pixel_byte_p = stream_consume(u8_t, &cx->unfiltered_image_stream);
    if (pixel_byte_p == nullptr) return false;
    u8_t pixel_byte = (*pixel_byte_p); // Paeth(x)

    // NOTE(Momo): PaethPredictor
    // https://www.w3.org/TR/png_t-Filters.html
    u8_t paeth_predictor; 
    {
      usz_t current_index = cx->image_stream.pos;

      // respectively: left, top, top left
      s32_t a, b, c;

      a = (i < bpp) ? 0 : (s32_t)(cx->image_stream.contents.data[current_index - bpp]); // Raw(x-bpp)
      b = (current_index < bpl) ? 0 : (s32_t)(cx->image_stream.contents.data[current_index - bpl]); // Prior(x)
      c = (i < bpp || current_index < bpl) ? 0 : (s32_t)(cx->image_stream.contents.data[current_index - bpl - bpp]); // Prior(x)

      s32_t p = a + b - c; //initial estimate
      s32_t pa = s32_abs(p - a);
      s32_t pb = s32_abs(p - b);
      s32_t pc = s32_abs(p - c);
      // Return nearest of a,b,c
      // breaking ties in order a, b,c
      if (pa <= pb && pa <= pc) {
        paeth_predictor = (u8_t)a;
      }
      else if (pb <= pc) {
        paeth_predictor = (u8_t)b;
      }
      else {
        paeth_predictor = (u8_t)c;
      }
    }

    u8_t pixel_byte_to_write = (pixel_byte + paeth_predictor)%256;  

    stream_write(&cx->image_stream, pixel_byte_to_write);
  }
  return true;
}

static b32_t
_png_filter_up(_png_context_t* c) {
  u32_t bpl = c->image_width * _PNG_CHANNELS; // bytes per line
  for (u32_t i = 0; i < bpl; ++i ){
    u8_t* pixel_byte_p = stream_consume(u8_t, &c->unfiltered_image_stream);
    if (pixel_byte_p == nullptr) {
      return false;
    }
    u8_t pixel_byte = (*pixel_byte_p); // Up(x)

    // NOTE(Momo): Ignore first scanline
    if (c->image_stream.pos < bpl) {
      stream_write(&c->image_stream, pixel_byte);
    }
    else {
      usz_t current_index = c->image_stream.pos;
      u8_t top = c->image_stream.contents.data[current_index - bpl]; 
      u8_t pixel_byte_to_write = (pixel_byte + top) % 256;  

      stream_write(&c->image_stream, pixel_byte_to_write);
    }
  }

  return true;
}


static b32_t
_png_filter(_png_context_t* c) {

  stream_reset(&c->unfiltered_image_stream);

  // NOTE(Momo): Filter
  // data always starts with 1 byte indicating the type of filter
  // followed by the rest of the chunk.
  while(!stream_is_eos(&c->unfiltered_image_stream)) {
    u8_t* filter_type_p = stream_consume(u8_t, &c->unfiltered_image_stream);
    u8_t filter_type = (*filter_type_p);
    // NOTE(Momo): https://www.w3.org/TR/png_t-Filters.html
    switch(filter_type) {
      case 0: { // None
        if (!_png_filter_none(c)) return false;
      } break;
      case 1: { // Sub
        if (!_png_filter_sub(c)) return false;
      } break;
      case 2: {
        if (!_png_filter_up(c)) return false;
      } break;
      case 3: {
        if (!_png_filter_average(c)) return false;
      } break;
      case 4: {
        if (!_png_filter_paeth(c)) return false;
      } break;
      default: {
        return false;
      };
    };
  }
  return true;

}

static b32_t
_png_decompress_zlib(_png_context_t* c, stream_t* zlib_stream) {
  _png_idat_header_t* IDAT = stream_consume(_png_idat_header_t, zlib_stream);

  u32_t CM = IDAT->compression_flags & 0x0F;
  u32_t CINFO = IDAT->compression_flags >> 4;
  //u32_t FCHECK = IDAT->additional_flags & 0x1F; //not needed?
  u32_t FDICT = (IDAT->additional_flags >> 5) & 0x01;
  //u32_t FLEVEL = (IDAT->additional_flags >> 6); //useless?


  if (CM != 8 || FDICT != 0 || CINFO > 7) {
    return false;
  }

  return _png_deflate(zlib_stream, &c->unfiltered_image_stream, c->arena);
}


// NOTE(Momo): For the code here, we are going to assume that 
// the PNG file we are reading is correct. i.e. we don't emphasize on 
// checking correctness of the PNG outside of the most basic of checks (e.g. sig)
//
static u32_t* 
png_rasterize(png_t* png, u32_t* out_w, u32_t* out_h, arena_t* arena) 
{
  make(stream_t, zlib_stream);

  _png_context_t ctx = {0};
  ctx.arena = arena;
  stream_init(&ctx.stream, png->contents);
  ctx.image_width = png->width;
  ctx.image_height = png->height;
  ctx.bit_depth = png->bit_depth;

  u32_t image_size = png->width * png->height * _PNG_CHANNELS;
  buffer_t image_buffer =  arena_push_buffer(arena, image_size, 16);
  if (buffer_is_valid(image_buffer)) return nullptr;
  stream_init(&ctx.image_stream, image_buffer);

  //arena_marker_t mark = arena_mark(arena);
  arena_set_revert_point(arena);

  u32_t unfiltered_size = png->width * png->height * _PNG_CHANNELS + png->height;
  buffer_t unfiltered_image_buffer = arena_push_buffer(arena, unfiltered_size, 16);
  if (buffer_is_valid(unfiltered_image_buffer)) return nullptr;
  stream_init(&ctx.unfiltered_image_stream, unfiltered_image_buffer);

  stream_consume(_png_chunk_t, &ctx.stream);

  // NOTE(Momo): This is really lousy method.
  // We will go through all the IDATs and push a giant contiguous 
  // chunk of memory to DEFLATE.
  usz_t zlib_size = 0;
  {
    stream_t stream = ctx.stream;
    while(!stream_is_eos(&stream)) {
      _png_chunk_header_t* chunk_header = stream_consume(_png_chunk_header_t, &stream);
      if (!chunk_header) return nullptr;
      u32_t chunk_length = u32_endian_swap(chunk_header->length);
      u32_t chunk_type = u32_endian_swap(chunk_header->type_U32);
      if (chunk_type == 'IDAT') {
        zlib_size += chunk_length;
      }
      stream_consume_block(&stream, chunk_length);
      stream_consume(_png_chunk_footer_t, &stream);
    }
  }

  buffer_t zlib_data = arena_push_buffer(arena, zlib_size, 16);
  if (buffer_is_valid(zlib_data)) return nullptr;

  stream_init(zlib_stream, zlib_data);

  // Second pass to push memory
  while(!stream_is_eos(&ctx.stream)) {
    _png_chunk_header_t* chunk_header = stream_consume(_png_chunk_header_t, &ctx.stream);
    if (!chunk_header) return nullptr;
    u32_t chunk_length = u32_endian_swap(chunk_header->length);
    u32_t chunk_type = u32_endian_swap(chunk_header->type_U32);
    if (chunk_type == 'IDAT') {
      stream_write_block(zlib_stream, 
                         ctx.stream.contents.data + ctx.stream.pos,
                         chunk_length);
    }
    stream_consume_block(&ctx.stream, chunk_length);
    stream_consume(_png_chunk_footer_t, &ctx.stream);
  }
  stream_reset(zlib_stream);

  if (!_png_decompress_zlib(&ctx, zlib_stream)) {
    return nullptr;
  }

  if(!_png_filter(&ctx)) {					
    return nullptr;
  }

  if (out_w) (*out_w) = ctx.image_width;
  if (out_h) (*out_h) = ctx.image_height;
  return (u32_t*)ctx.image_stream.contents.data;


}
// NOTE(Momo): Really dumb way to write.
// Just have a IHDR, IEND and a single IDAT that's not encoded lul
static buffer_t
png_write(u32_t* pixels, u32_t width, u32_t height, arena_t* arena) {
  static const u8_t signature[] = { 
    137, 80, 78, 71, 13, 10, 26, 10 
  };
  u32_t image_bpl = (width * 4);
  u32_t data_bpl = image_bpl + 1; // bytes per line
  u32_t data_size = data_bpl * height;
  u32_t max_chunk_size = 65535;
  u32_t signature_size = sizeof(signature);
  u32_t chunk_size = sizeof(_png_chunk_header_t) + sizeof(_png_chunk_footer_t);
  u32_t IHDR_size = chunk_size + sizeof(_png_ihdr_t);
  u32_t IEND_size = chunk_size;
  u32_t IDAT_size = chunk_size + sizeof(_png_idat_header_t);
  u32_t lines_per_chunk = max_chunk_size / data_bpl;
  u32_t chunk_count = height / lines_per_chunk;

  if (height % lines_per_chunk) {
    chunk_count += 1;
  }
  u32_t IDAT_chunk_size = 5 * chunk_count;

  u32_t expected_memory_required = (signature_size + 
    IHDR_size + 
    IEND_size + 
    IDAT_size + 
    data_size + 
    IDAT_chunk_size);

  buffer_t stream_memory = arena_push_buffer(arena, expected_memory_required, 16);
  if (buffer_is_valid(stream_memory)) return buffer_set(0,0);

  make(stream_t, stream);
  stream_init(stream, stream_memory);
  stream_write_block(stream, (void*)signature, sizeof(signature));


  // NOTE(Momo): write IHDR
  {
    u8_t* crc_start = nullptr;

    _png_chunk_header_t header = {};
    header.type_U32 = u32_endian_swap('IHDR');
    header.length = sizeof(_png_ihdr_t);
    header.length = u32_endian_swap(header.length);
    stream_write(stream, header);
    crc_start = stream->contents.data + stream->pos - sizeof(header.type_U32);

    _png_ihdr_t IHDR = {};
    IHDR.width = u32_endian_swap(width);
    IHDR.height = u32_endian_swap(height);
    IHDR.bit_depth = 8; // ??
    IHDR.colour_type = 6;
    IHDR.compression_method = 0;
    IHDR.filter_method = 0;
    IHDR.interlace_method = 0;
    stream_write(stream, IHDR);

    _png_chunk_footer_t footer = {};
    u32_t crc_size = (u32_t)(stream->contents.data + stream->pos - crc_start);
    footer.crc = _png_calculate_crc32(crc_start, crc_size); 
    footer.crc = u32_endian_swap(footer.crc);
    stream_write(stream, footer);

  }

  // NOTE(Momo): write IDAT
  // TODO(Momo): Adler32
  {

    u32_t chunk_overhead = sizeof(u16_t)*2 + sizeof(u8_t)*1;

    u8_t* crc_start = nullptr;

    _png_chunk_header_t header = {};
    header.type_U32 = u32_endian_swap('IDAT');
    header.length = sizeof(_png_idat_header_t) + (chunk_overhead*chunk_count) + data_size; 
    header.length = u32_endian_swap(header.length);    
    stream_write(stream, header);
    crc_start = stream->contents.data + stream->pos - sizeof(header.type_U32);

    // NOTE(Momo): Hardcoded IDAT chunk header header that fits our use-case
    //
    // CM = 8
    // CINFO = any number < 7? 1?
    // FCHECK = 23? if CM == 8 and CINFO == 1
    // FDIC = 0;
    // FLEVEL = 1? Documentation says it doesn't matter;
    _png_idat_header_t IDAT;
    IDAT.compression_flags = 8;
    IDAT.additional_flags = 29;
    stream_write(stream, IDAT);


    // NOTE(Momo): Deflate chunk header
    //
    // BFINAL = 1 (1 bit); // indicates if it's the final block
    // BTYPE = 0 (2 bits); // indicates no compression
    // 
    u32_t lines_remaining = height;
    u32_t current_line = 0;

    for (u32_t chunk_index = 0; chunk_index < chunk_count; ++chunk_index){
      u32_t lines_to_write = min_of(lines_remaining, lines_per_chunk);
      lines_remaining -= lines_to_write;

      u8_t BFINAL = ((chunk_index + 1) == chunk_count) ? 1 : 0;
      stream_write(stream, BFINAL);

      u16_t LEN = (u16_t)(lines_to_write * data_bpl); // number of data bytes in the block
      u16_t NLEN = ~LEN; // one's complement of LEN
      stream_write(stream, LEN);
      stream_write(stream, NLEN);

      // NOTE(Momo): Output data here
      // We have to do it row by row to add the filter byte at the front
      for (u32_t line_index = 0; line_index < lines_to_write; ++line_index) 
      {
        u8_t no_filter = 0;
        stream_write(stream, no_filter); // Filter type: None

        stream_write_block(stream,
                           (u8_t*)pixels + (current_line * image_bpl),
                           image_bpl);

        ++current_line;

      }


    }


    _png_chunk_footer_t footer = {};
    u32_t crc_size = (u32_t)(stream->contents.data + stream->pos - crc_start);
    footer.crc = _png_calculate_crc32(crc_start, crc_size); 
    footer.crc = u32_endian_swap(footer.crc);
    stream_write(stream, footer);
  }

  // NOTE(Momo): stream_write IEND
  {
    u8_t* crc_start = nullptr;

    _png_chunk_header_t header = {};
    header.type_U32 = u32_endian_swap('IEND');
    header.length = 0;
    stream_write(stream, header);
    crc_start = stream->contents.data + stream->pos - sizeof(header.type_U32);


    _png_chunk_footer_t footer = {};
    u32_t crc_size = (u32_t)(stream->contents.data + stream->pos - crc_start);
    footer.crc = _png_calculate_crc32(crc_start, crc_size); 
    footer.crc = u32_endian_swap(footer.crc);
    stream_write(stream, footer);
  }



  return buffer_set(stream->contents.data, stream->pos);
}


static b32_t     
png_read(png_t* png, buffer_t png_contents)
{
  make(stream_t, stream);
  stream_init(stream, png_contents);

  // Read Signature
  _png_chunk_t* png_header = stream_consume(_png_chunk_t, stream);  
  if (!_png_is_signature_valid(png_header->signature)) return false; 

  // Read Chunk Header
  _png_chunk_header_t* chunk_header = stream_consume(_png_chunk_header_t, stream);
  //u32_t chunk_length = u32_endian_swap(chunk_header->length);
  u32_t chunk_type = u32_endian_swap(chunk_header->type_U32);


  if(chunk_type != 'IHDR') { return false; }

  _png_ihdr_t* IHDR = stream_consume(_png_ihdr_t, stream);

  // NOTE(Momo): Width and height is in Big Endian
  // We assume that we are currently in a Little Endian system
  u32_t width = u32_endian_swap(IHDR->width);
  u32_t height = u32_endian_swap(IHDR->height);

  if (!_png_is_format_supported(IHDR)) { return false; }

  png->contents = png_contents;
  png->width = width;
  png->height = height;
  png->bit_depth = IHDR->bit_depth;
  png->colour_type = IHDR->colour_type;
  png->compression_method = IHDR->compression_method;
  png->filter_method = IHDR->filter_method;
  png->interlace_method = IHDR->interlace_method;

  return true;
}

// 
// MARK:(Arena)
//

static void
arena_init(arena_t* a, void* mem, usz_t cap) {
  a->memory = (u8_t*)mem;
  a->pos = 0; 
  a->cap = cap;
  a->highest_memory_usage = 0;
}



static void
arena_clear(arena_t* a) {
  a->pos = 0;
}


static usz_t 
arena_remaining(arena_t* a) {
  return a->cap - a->pos;
}

static void* 
arena_push_size(arena_t* a, usz_t size, usz_t align) {
  if (size == 0) return nullptr;

  usz_t imem = ptr_to_umi(a->memory);
  umi_t adjusted_pos = align_up_pow2(imem + a->pos, align) - imem;

  if (imem + adjusted_pos + size >= imem + a->cap) return nullptr;

  u8_t* ret = umi_to_ptr(imem + adjusted_pos);
  a->pos = adjusted_pos + size;

  a->highest_memory_usage = max_of(a->pos, a->highest_memory_usage);

  return ret;

}

static void*
arena_push_size_zero(arena_t* a, usz_t size, usz_t align) 
{
  void* mem = arena_push_size(a, size, align);
  if (!mem) return nullptr;
  zero_memory(mem, size);
  return mem;
}


static b32_t
arena_push_partition(arena_t* a, arena_t* partition, usz_t size, usz_t align) {	
  void* mem = arena_push_size(a, size, align);
  if (!mem) return false; 
  arena_init(partition, mem, size);
  return true;

}

static buffer_t
arena_push_buffer(arena_t* a, usz_t size, usz_t align) {
  buffer_t buffer = {};
  buffer.data = arena_push_arr_align(u8_t, a, size, align);
  buffer.size = size;

  return buffer;
}


static b32_t    
arena_push_partition_with_remaining(arena_t* a, arena_t* partition, usz_t align){
  usz_t imem = ptr_to_umi(a->memory);
  usz_t adjusted_pos = align_up_pow2(imem + a->pos, align) - imem;

  if (imem + adjusted_pos >= imem + a->cap) return false;
  usz_t size = a->cap - adjusted_pos;	
  void* mem = umi_to_ptr(imem + adjusted_pos);
  a->pos = a->cap;

  arena_init(partition, mem, size);
  return true;

}

static arena_marker_t
arena_mark(arena_t* a) {
  arena_marker_t ret;
  ret.arena = a;
  ret.old_pos = a->pos;
  return ret;
}

static void
arena_revert(arena_marker_t marker) {
  marker.arena->pos = marker.old_pos;
}

static void 
arena_calc_push(arena_calc_t* c, usz_t size, usz_t alignment) {
  c->result = align_up_pow2(c->result, alignment);
  c->result += size;
}


static void 
arena_calc_clear(arena_calc_t* c) {
  c->result = 0;
}

static usz_t 
arena_calc_get_result(arena_calc_t* c) {
  return c->result;
}


//
// MARK:(Garena)
//
static void
garena_clear(garena_t* ga) {
  ga->free_list = (garena_free_block_t*)ga->memory;
  ga->free_list->next = nullptr;
  ga->free_list->size = ga->cap;

}

static void
garena_init(garena_t* ga, u8_t* memory, usz_t cap) {
  ga->memory = memory;
  ga->cap = cap;
  garena_clear(ga);
}

static void* 
garena_push_size(garena_t* ga, usz_t size) {
  // The total required size is (header size + block size) rounded up to 16
  usz_t total_required_size = align_up_pow2(size + sizeof(garena_header_t), 16) ;
  usz_t total_actual_size = total_required_size;

  // TODO: header must be aligned
  // Right now it's not.

  // First Fit Strategy
  garena_free_block_t* itr = ga->free_list;
  garena_free_block_t* prev = nullptr;
  while (itr != nullptr) 
  {
    if (itr->size >= total_required_size) {
      break;
    }
    prev = itr;
    itr = itr->next;
  }

  // Cannot find a block that fits.
  if (itr == nullptr) {
    return nullptr;
  }

  // Here, we found a block that fits.
  // Split if the remaining size is more than the block size
  garena_free_block_t* new_free_block;
  usz_t remaining_size = itr->size - total_required_size;

  // Case where future allocation of the 
  // current free block is impossible.
  // Thus, the next free block is the new free block
  if (remaining_size <= sizeof(garena_header_t)) {
    new_free_block = itr->next;
    total_actual_size = itr->size;
  }

    // Case where we split the block 
  else {
    u8_t* new_free_block_memory = (u8_t*)(itr) + total_required_size;
    new_free_block = (garena_free_block_t*)new_free_block_memory;
    new_free_block->size = remaining_size;
    new_free_block->next = itr->next;
  }

  // Update the free list
  // If there is a previous block, set it's next pointer to the new free block
  if (prev) {
    prev->next = new_free_block;
  }

    // If there isn't a previous block, that means we need to update the head.
  else {
    ga->free_list = new_free_block; 
  }

  // Update header of block to return
  auto* header = (garena_header_t*)itr;
  header->size = total_actual_size;

  // Return the pointer to the user
  u8_t* ret = (u8_t*)(itr) + sizeof(garena_header_t);
  return ret;
}

static void
garena_free(garena_t* ga, void* block) {
  if (!block) return;

  u8_t* block_u8 = (u8_t*)block;

  // NOTE(momo): Header is always 16 bytes behind block.
  auto* header = (garena_header_t*)(block_u8 - sizeof(garena_header_t));
  umi_t block_end = ptr_to_umi((u8_t*)header + header->size);

  garena_free_block_t* itr = ga->free_list;
  garena_free_block_t* prev = nullptr;

  // Search until we are past the current block.
  // At the end of this, itr should the next free block
  // AFTER the block we are freeing.
  while(itr != nullptr) {
    umi_t itr_location = ptr_to_umi(itr);
    if (itr_location >= block_end) {
      break;
    }
    prev = itr;
    itr = itr->next;
  }


  // If there is no previous block, it means itr is the start of the block
  // Thus we simply set the head of the free list to this
  if (prev == nullptr) {
    prev = (garena_free_block_t*)(header);

    // NOTE(momo): this is a bit dangerous
    // since prev is overlapping header BUT
    // it should be okay.
    prev->size = header->size; 
    prev->next = ga->free_list;
    ga->free_list = prev;
  }

    // If the previous block is directly next to this block, 
    // combine both blocks simply by adding to the previous block size
  else if ( ((u8_t*)(prev) + prev->size) == (u8_t*)header) {
    prev->size += header->size;
  }

    // The previous block is not next to the current block, so we turn 
    // the current block directly into a free block
  else {
    auto* temp = (garena_free_block_t*)(header);

    // NOTE(momo): this is a bit dangerous
    // since prev is over590ping header BUT
    // it should be okay.
    temp->size = header->size;
    temp->next = prev->next;
    prev->next = temp;
    prev = temp;
  }

  // Check if we can combine prev with the next free block
  if (itr != nullptr && ptr_to_umi(itr) == block_end) {
    prev->size += itr->size;
    prev->next = itr->next;
  }



}

//
// MARK:(RectPack)
//

struct _rp_node{
  u32_t x, y, w, h;
};

static void
_rp_sort(rp_rect_t* rects,
         sort_entry_t* entries,
         u32_t count,
         rp_sort_type_t sort_type) 
{
  switch(sort_type) {
    case RP_SORT_TYPE_HEIGHT: {
      for (u32_t i = 0; i < count; ++i) {
        entries[i].key = -(f32_t)rects[i].h;
        entries[i].index = i;
      }
    } break;
    case RP_SORT_TYPE_WIDTH: {
      for (u32_t i = 0; i < count; ++i) {
        entries[i].key = -(f32_t)rects[i].w;
        entries[i].index = i;
      }
    } break;
    case RP_SORT_TYPE_AREA: {
      for (u32_t i = 0; i < count; ++i) {
        entries[i].key = -((f32_t)rects[i].h * (f32_t)rects[i].w);
        entries[i].index = i;
      }
    } break;
    case RP_SORT_TYPE_PERIMETER: {
      for (u32_t i = 0; i < count; ++i) {
        entries[i].key = -((f32_t)rects[i].h + (f32_t)rects[i].w);
        entries[i].index = i;
      }
    } break;
    case RP_SORT_TYPE_BIGGER_SIDE: {
      for (u32_t i = 0; i < count; ++i) {
        f32_t key = -(f32_t)(max_of(rects[i].w, rects[i].h));
        entries[i].key = key;
        entries[i].index = i;
      }
    } break;
    case RP_SORT_TYPE_PATHOLOGICAL: {
      for (u32_t i = 0; i < count; ++i) {
        u32_t wh_max = max_of(rects[i].w, rects[i].h);
        u32_t wh_min = min_of(rects[i].w, rects[i].h);
        f32_t key = -(f32_t)(wh_max/wh_min * rects[i].w * rects[i].h);
        entries[i].key = key;
        entries[i].index = i;
      }
    } break;
  }
  quicksort(entries, count);
}

static b32_t
rp_pack(rp_rect_t* rects, 
        u32_t rect_count, 
        u32_t padding,
        u32_t total_width,
        u32_t total_height,
        rp_sort_type_t sort_type,
        arena_t* allocator) 
{
  arena_marker_t restore_point = arena_mark(allocator);

  sort_entry_t* sort_entries = arena_push_arr(sort_entry_t, allocator, rect_count);
  _rp_sort(rects, sort_entries, rect_count, sort_type);
  _rp_node* nodes = arena_push_arr(_rp_node, allocator, rect_count+1);

  u32_t current_node_count = 1;
  nodes[0].x = 0;
  nodes[0].y = 0;
  nodes[0].w = total_width;
  nodes[0].h = total_height;

  for (u32_t i = 0; i < rect_count; ++i) {
    rp_rect_t* rect = rects + sort_entries[i].index;

    // ignore rects with 0 width or height
    if(rect->w == 0 || rect->h == 0) continue;

    // padding*2 because there are 2 sides
    u32_t rect_width = rect->w + padding*2;
    u32_t rect_height = rect->h + padding*2;

    // NOTE(Momo): Iterate the empty spaces backwards to find the best fit index
    u32_t chosen_space_index = current_node_count;
    for (u32_t  j = 0; j < chosen_space_index ; ++j ) {
      u32_t index = chosen_space_index - j - 1;
      _rp_node space = nodes[index];

      // NOTE(Momo): Check if the image fits
      if (rect_width <= space.w && rect_height <= space.h) {
        chosen_space_index = index;
        break;
      }
    }


    // NOTE(Momo): If an empty space that can fit is found, 
    // we remove that space and split.
    if(chosen_space_index == current_node_count) { 
      arena_revert(restore_point);
      return false;
    }

    // NOTE(Momo): swap and pop the chosen space
    _rp_node chosen_space = nodes[chosen_space_index];

    if (current_node_count > 0) {
      nodes[chosen_space_index] = nodes[current_node_count-1];
      --current_node_count;
    }

    // NOTE(Momo): Split if not perfect fit
    if (chosen_space.w != rect_width && chosen_space.h == rect_height) {
      // Split right
      _rp_node split_space_right;
      split_space_right.x = chosen_space.x + rect_width;
      split_space_right.y = chosen_space.y;
      split_space_right.w = chosen_space.w - rect_width;
      split_space_right.h = chosen_space.h;

      nodes[current_node_count++] = split_space_right;
    }
    else if (chosen_space.w == rect_width && chosen_space.h != rect_height) {
      // Split down
      _rp_node split_space_down;
      split_space_down.x = chosen_space.x;
      split_space_down.y = chosen_space.y + rect_height;
      split_space_down.w = chosen_space.w;
      split_space_down.h = chosen_space.h - rect_height;
      nodes[current_node_count++] = split_space_down;
    }
    else if (chosen_space.w != rect_width && chosen_space.h != rect_height) {
      // Split right
      _rp_node split_space_right;
      split_space_right.x = chosen_space.x + rect_width;
      split_space_right.y = chosen_space.y;
      split_space_right.w = chosen_space.w - rect_width;
      split_space_right.h = rect_height;

      // Split down
      _rp_node split_space_down;
      split_space_down.x = chosen_space.x;
      split_space_down.y = chosen_space.y + rect_height;
      split_space_down.w = chosen_space.w;
      split_space_down.h = chosen_space.h - rect_height;

      // Choose to insert the bigger one first before the smaller one
      u32_t right_area = split_space_right.w * split_space_right.h;
      u32_t down_area = split_space_down.w * split_space_down.h;

      if (right_area > down_area) {
        nodes[current_node_count++] = split_space_right;
        nodes[current_node_count++] = split_space_down;
      }
      else {
        nodes[current_node_count++] = split_space_down;
        nodes[current_node_count++] = split_space_right;
      }

    }

    // NOTE(Momo): Translate the rect
    rect->x = chosen_space.x + padding;
    rect->y = chosen_space.y + padding;
  }

  arena_revert(restore_point);
  return true;
}


#endif

//
// JOURNAL
// 
// = 2023-09-22= 
//   I'm thinking of writing an API for the OS layer soon. Or maybe it set of 
//   'pig' APIs that are dumb and stupid but good enough to just 'get stuff out'.
//   Basically stuff like 'get file size' or 'read file into buffer'. I am a little
//   sick and tired of writing the same functions over and over again.
//
// = 2023-08-04 =
//   Now that everything is in one file, it might be wise to add and
//   maintain 'bookmarks' and a 'table of content' section at the top 
//   of the file for easier navigation. We probably don't care about
//   bookmarking the implementation section; we treat that section as 
//   a heap of instructions. 
//
// = 2023-08-03 =
//   I'm not entirely sure if inspector and profiler should
//   be under momo.h, which is meant to contain general purpose
//   bag of useful things. I'm trying to visualize how general
//   purpose they REALLY are.
//   
//   The inspector, I feel, is not general purpose enough to
//   warrant being placed here. It needs a simpler way to allow
//   inspection of ANY objects. Right now, to add a new object,
//   one will need to change the definition of the whole 
//   structure, which is not ideal.
//
//   TECHNICALLY, I could just support types that the framework
//   supports (like u32, v2f, etc) and call it a day. 
//
//
// = 2023-07-25 =
//   I have made momo.h a single header file with no .cpp.
//   Maybe I am foolish, but maybe I am damn smart.
//
// = 2023-07-22 =
//   I kind of have a brainfart and hated myself for only thinking about this
//   after 18 whooping years of programming.
//
//   It might be a LOT better for organizational purposes to just have a  
//   single header file with ALL declarations and defines, THEN multiple 
//   other files that implement function definitions. 
//
//   This would avoid ALL problems regarding 'organizing' declarations/definition.
//   I mean, C isn't meant to be organized that way anyways.
//   
