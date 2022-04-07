/* date = november 20th 2021 11:58 am */

#ifndef momo_string_h
#define momo_string_h


//~ NOTE(Momo): 'Immutable' Strings
struct String{
	U8* e;
	UMI count;
};

static String create_string(U8* Str, UMI size);
static String substr(String Str, UMI start, UMI ope);
static B32  match(String lhs, String rhs);

// NOTE(Momo): No one should be using this
// Maybe make a macro to enable/disable this?
#define string_from_lit(s) create_string((U8*)(s), sizeof(s)-1)

static B32 operator==(String lhs, String rhs);
static B32 operator!=(String lhs, String rhs);



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
static void     push_f32(String_Builder* b, F32 value, U32 precision);
static void     push_s32(String_Builder* b, S32 num);
static void     push_string(String_Builder* b, String num);
static void     push_format(String_Builder* b, String fmt, ...);

static void     init_string_builder(String_Builder* b, U8* data, UMI cap);
//#define StringBld_temp(name, cap) U8 temp##__line__[cap]; StringBld name = StringBld_Create(temp##__line__, cap);


#include "momo_strings.cpp"

#endif //MOMO_STRING_H