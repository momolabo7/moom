#ifndef MOMO_H
#define MOMO_H

//
// Varadic arguments
//
#include <stdarg.h>

//
// Compiler contexts
//
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
#else
# error exported not defined for this compiler
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
// Buffer: a piece of memory
struct buffer_t {
  union {
    void* data;
    u8_t* data_u8;
  };
  usz_t size;

  operator bool() {
    return data != nullptr;
  }
};

//
// Vectors
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
// Matrices
//
struct m44f_t {
	f32_t e[4][4];
};

//
// Colors
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
// RNG
//
struct rng_t {
  u32_t seed;
};

//
// Sorting
//
struct sort_entry_t {
  f32_t key;
  u32_t index;
};

//
// CRC
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
// Arena
//
struct arena_t {
	u8_t* memory;
	usz_t pos;
	usz_t cap;
};

// Temporary memory API used to arena_revert an arena to an original state;
struct arena_marker_t {
  arena_t* arena;
  usz_t old_pos;
};

//
// Strings
//
struct str8_t {
	u8_t* e;
	usz_t count;
};

//
// String Builders
//
struct sb8_t{
	union {
		str8_t str;
		struct {
			u8_t* e;
			usz_t count;
		};
	};
	usz_t cap;
};

//
// Streams
//
struct stream_t {
  buffer_t contents;
  umi_t pos;
	
  // For bit reading
  u32_t bit_buffer;
  u32_t bit_count;
};


//
// TTF 
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
// PNG
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
// Rect Packer
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
// JSON
//

// The "key" of a JSON entry, which can only be a string.
struct json_key_t {
  u8_t* at;
  umi_t count;
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
      umi_t count;
    };
    str8_t str;
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
  str8_t text;
  umi_t at;

  // The 'root' item in a JSON file is an object type.
  json_object_t root;
};


////
//
// Primitive Constants
//
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
// Helper Macros
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

//
// Defer
//
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
// Foreach
//
#define for_arr(id, arr) for(umi_t id = 0, cnt = array_count(arr); (id) < (cnt); ++id)
#define for_cnt(id, cnt) for(decltype(cnt) id = 0; id < (cnt); ++id)
#define for_range(id, beg, end) for(decltype(beg) id = (beg); id <= (end); ++id)

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
// bit manipulation
//
#define bit_is_set(mask,bit) ((mask) & ((u64_t)1 << (bit)))
#define bit_set(mask, bit) ((mask) |= ((u64_t)1 << (bit)))
#define bit_unset(mask, bit) ((mask) &= ~((u64_t)1 << (bit)))


//
// Pointer to Int conversions 
//
static umi_t ptr_to_umi(void* p);
static u8_t* umi_to_ptr(umi_t u)

//
// Ascii helpers
//
#define digit_to_ascii(d) ((d) + '0')
#define ascii_to_digit(a) ((a) - '0')

static b32_t is_whitespace(char c);
static b32_t is_alpha(char c)
static b32_t is_digit(char c);

//
// Memory helpers
//
#define zero_struct(p)    zero_memory((p), sizeof(*(p)))
#define zero_array(p)     zero_memory((p), sizeof(p))
#define zero_range(p,s)   zero_memory((p), sizeof(*(p)) * (s))

#define copy_struct(p)    copy_memory((p), sizeof(*(p)))
#define copy_array(p)     copy_memory((p), sizeof(p))
#define copy_range(p,s)   copy_memory((p), sizeof(*(p)) * (s))

static void copy_memory(void* dest, const void* src, umi_t size);
static void zero_memory(void* dest, umi_t size);
static b32_t is_memory_same(const void* lhs, const void* rhs, umi_t size);
static void swap_memory(void* lhs, void* rhs, umi_t size);

//
// Absolutes
//
static f32_t f32_abs(f32_t x);
static f64_t f64_abs(f64_t x);
static s8_t s8_abs(s8_t x);
static s16_t s16_abs(s16_t x)
static s32_t s32_abs(s32_t x)  
static s64_t s64_abs(s64_t x);

//
// Lerps
//
static f32_t f32_lerp(f32_t s, f32_t e, f32_t f);
static f64_t f64_lerp(f64_t s, f64_t e, f64_t f); 

//
// Weights
//
static f32_t f32_weight(f32_t v, f32_t min, f32_t max);
static f64_t f64_weight(f64_t v, f64_t min, f64_t max) ;

//
// Degrees, Radians and Turns
//

static f32_t f32_deg_to_rad(f32_t degrees);
static f32_t f32_rad_to_deg(f32_t radians);
static f32_t f32_turns_to_radians(f32_t turns);
static f64_t f64_deg_to_rad(f64_t degrees);
static f64_t f64_rad_to_deg(f64_t radians);
static f64_t f64_turns_to_radians(f64_t turns);

//
// Endian Swap
// 
// NOTE(Momo): I'm not entirely sure if this prototype makes sense.
// It sounds more reasonable to endian swap ANY type. 
// We COULD use a template approach like so:
//   template<typename T> endian_swap_16(T value);
//   template<typename T> endian_swap_32(T value); 
// Or we COULD just ignore the concept of type:
//   void _EndianSwap16(u8_t* ptr)
//   #define endian_swap_16(value) _EndianSwap16((u8_t*)&value)
//
static u16_t u16_endian_swap(u16_t value);
static s16_t s16_endian_swap(s16_t value);
static u32_t u32_endian_swap(u32_t value);


//
// Beats Per Minute to Seconds Per Beat
//
static f32_t bpm_to_spb_f32(f32_t bpm);
static f64_t bpm_to_spb_f64(f64_t bpm);

//
// Buffer
//
static buffer_t buffer_set(void* mem, usz_t size);

//
// CString manipulation
//
static umi_t cstr_len(const c8_t* str); 
static void  cstr_copy(c8_t * dest, const c8_t* src); 
static b32_t cstr_compare(const c8_t* lhs, const c8_t* rhs); 
static b32_t cstr_compare_n(const c8_t* lhs, const c8_t* rhs, umi_t n); 
static void  cstr_concat(c8_t* dest, const c8_t* Src);
static f64_t cstr_to_f64(const c8_t* p); 
static void  cstr_clear(c8_t* dest); 
static void  cstr_reverse(c8_t* dest); 
static void  cstr_itoa(c8_t* dest, s32_t num); 



// F32 functions
static b32_t is_close_f32(f32_t lhs, f32_t rhs); 
static b32_t is_close_f64(f64_t lhs, f64_t rhs); 
static b32_t is_nan_f32(f32_t f); 
static b32_t is_nan_f64(f64_t f); 

//
// Hash functions
// 
static u32_t djb2(const c8_t* str);

//
// Singly Linked Lists
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
// Atomic functions
// 

// Returns the old value before the exchange
static u32_t u32_atomic_compare_assign(u32_t volatile* value, u32_t new_value, u32_t expected_value);
static u64_t u64_atomic_assign(u64_t volatile* value, u64_t new_value);
static u32_t u32_atomic_add(u32_t volatile* value, u32_t to_add);
static u64_t u64_atomic_add(u64_t volatile* value, u64_t to_add);

//
// Math functions
//
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

// Easing functions

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
static v2u_t v2u_sub(v2u_t lhs, v2u_t rhs;)
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
// Colors
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
// Collision detection
//
static b32_t bonk_tri2_pt2(v2f_t tp0, v2f_t tp1, v2f_t tp2, v2f_t pt); 

// 
// PRNG
//

static void  rng_init(rng_t* r, u32_t seed);
static u32_t rng_next(rng_t* r);
static u32_t rng_choice(rng_t* r, u32_t choice_count);
static f32_t rng_unilateral(rng_t* r);
static f32_t rng_bilateral(rng_t* r);
static f32_t rng_range_F32(rng_t* r, f32_t min, f32_t max);
static s32_t rng_range_S32(rng_t* r, s32_t min, s32_t max);
static v2f_t rng_unit_circle(rng_t* r);

// Sorting
static void quicksort(sort_entry_t* entries, u32_t entry_count);

//
// CRC
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
// Strings
//
#define str8_from_lit(s) str8((u8_t*)(s), sizeof(s)-1)
#define str8_lit(s) str8((u8_t*)(s), sizeof(s)-1)
static str8_t str8(u8_t* str, usz_t size);
static str8_t str8_substr(str8_t str, usz_t start, usz_t ope);
static b32_t  str8_match(str8_t lhs, str8_t rhs);
static str8_t str8_from_cstr(const char* cstr);
static b32_t  str8_to_u32(str8_t s, u32_t* out);
static b32_t  str8_to_f32(str8_t s, f32_t* out);
static b32_t  str8_to_s32(str8_t s, s32_t* out);
static b32_t  str8_range_to(u32_t* out);
static b32_t  operator==(str8_t lhs, str8_t rhs);
static b32_t  operator!=(str8_t lhs, str8_t rhs);

//
// String Builders
//
#define sb8_make(name, cap) \
  u8_t temp_buffer_##__LINE__[cap] = {0}; \
  sb8_t name_; \
  sb8_t* name = &name_; \
  sb8_init(name, temp_buffer_##__LINE__, cap);

static usz_t    sb8_remaining(sb8_t* b);
static void     sb8_clear(sb8_t* b);
static void     sb8_pop(sb8_t* b);
static void     sb8_push_c8(sb8_t* b, c8_t num);
static void     sb8_push_u8(sb8_t* b, u8_t num);
static void     sb8_push_u32(sb8_t* b, u32_t num);
static void     sb8_push_u64(sb8_t* b, u64_t num);
static void     sb8_push_f32(sb8_t* b, f32_t value, u32_t precision);
static void     sb8_push_s32(sb8_t* b, s32_t num);
static void     sb8_push_s64(sb8_t* b, s64_t num);
static void     sb8_push_str8(sb8_t* b, str8_t num);
static void     sb8_push_hex_u8(sb8_t* b, u8_t num);
static void     sb8_push_hex_u32(sb8_t* b, u32_t num);
static void     sb8_push_fmt(sb8_t* b, str8_t fmt, ...);
static void     sb8_init(sb8_t* b, u8_t* data, usz_t cap);

//
// Streams
//
#define stream_consume(t,s) (t*) stream_consume_block((s), sizeof(t))
#define stream_write(s,item) stream_write_block((s), &(item), sizeof(item))
static void   stream_init(stream_t* s, buffer_t contents);
static void   stream_reset(stream_t* s);
static b32_t  stream_is_eos(stream_t* s);
static u8_t*  stream_consume_block(stream_t* s, umi_t amount);
static void   stream_write_block(stream_t* s, void* src, umi_t size);
static void   stream_flush_bits(stream_t* s);
static u32_t  stream_consume_bits(stream_t* s, u32_t amount);

//
// Arenas
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

# define __arena_set_revert_point(a,l) \
  auto _arena_marker_##l = arena_mark(a); \
  defer{arena_revert(_arena_marker_##l);};
# define _arena_set_revert_point(a,l) __arena_set_revert_point(a,l)
# define arena_set_revert_point(arena) _arena_set_revert_point(arena, __LINE__) 


//
// TTF
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
// SIG:(PNG)
//
static b32_t     png_read(png_t* png, buffer_t png_contents);
static u32_t*    png_rasterize(png_t* png, u32_t* out_w, u32_t* out_h, arena_t* arena); 
static buffer_t  png_write(png_t* png, u32_t width, u32_t height, arena_t* arena);

// 
// SIG:(Rect Packer)
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
// SIG:(JSON)
//
static json_object_t* json_read(json_t* j, const u8_t* json_string, u32_t json_string_size, arena_t* ba);
static json_value_t* json_get_value(json_object_t* j, str8_t key);
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
//
// Implementation
//
//

