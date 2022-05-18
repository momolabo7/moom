#ifndef MOMO_STRING_H
#define MOMO_STRING_H


//~ NOTE(Momo): 'Immutable' Strings
struct String{
	U8* e;
	UMI count;
};

static String substr(String Str, UMI start, UMI ope);
static B32  match(String lhs, String rhs);
static B32 operator==(String lhs, String rhs);
static B32 operator!=(String lhs, String rhs);

//- Constructors
static String string(U8* Str, UMI size);
static String string_from_cstr(const char* cstr);
static String string_from_cstr(char* cstr);
// NOTE(Momo): No one should be using this
// Maybe make a macro to enable/disable this?
#define string_from_lit(s) string((U8*)(s), sizeof(s)-1)




//~ String builders
struct String_Builder{
	union {
		String str;
		struct {
			U8* e;
			UMI count;
		};
	};
	UMI cap;
};

static UMI      remaining(String_Builder* b);
static void     clear(String_Builder* b);
static void     pop(String_Builder* b);
static void     push_c8(String_Builder* b, C8 num);
static void     push_u8(String_Builder* b, U8 num);
static void     push_u32(String_Builder* b, U32 num);
static void     push_u64(String_Builder* b, U64 num);
static void     push_f32(String_Builder* b, F32 value, U32 precision);
static void     push_s32(String_Builder* b, S32 num);
static void     push_s64(String_Builder* b, S64 num);
static void     push_string(String_Builder* b, String num);
static void     push_format(String_Builder* b, String fmt, ...);

static void     init_string_builder(String_Builder* b, U8* data, UMI cap);

#define make_string_builder(name, cap) U8 temp_buffer_##__LINE__[cap] = {}; String_Builder name_; String_Builder* name = &name_; init_string_builder(name, temp_buffer_##__LINE__, cap);

//#define StringBld_temp(name, cap) U8 temp##__line__[cap]; StringBld name = StringBld_Create(temp##__line__, cap);


#include "momo_strings.cpp"

#endif //MOMO_STRING_H