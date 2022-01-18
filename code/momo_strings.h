/* date = november 20th 2021 11:58 am */

#ifndef momo_string_h
#define momo_string_h


//~ NOTE(Momo): 'Immutable' Strings
typedef struct {
	U8* e;
	UMI count;
} Str8;

static Str8 create_str8(U8* Str, UMI size);
static Str8 substr(Str8 Str, UMI start, UMI ope);
static B32  match(Str8 lhs, Str8 rhs);

// No one should be using this
// TODO: Maybe make a macro to enable/disable this?
#define str8_from_lit(s) create_str8((U8*)(s), sizeof(s)-1)

static B32 operator==(Str8 lhs, Str8 rhs);
static B32 operator!=(Str8 lhs, Str8 rhs);


// TODO: String split
// TODO: String nodes?


//~ note(momo): String builders
struct Str8Bld{
	union {
		Str8 Str;
		struct {
			U8* e;
			UMI count;
		};
	};
	UMI cap;

	UMI      remaining();
	void     clear();
	void     pop();
	void     push_C8(C8 num);
	void     push_U32(U32 num);
	void     push_F32(F32 value, U32 precision);
	void     push_S32(S32 num);
	void     push_Str8(Str8 num);
	void     push_format(Str8 fmt, ...);

};

static Str8Bld  create_str8bld(U8* data, UMI cap);
// TODO: We should remove this and figure out a better way to do format strings.
//#define Str8Bld_temp(name, cap) U8 temp##__line__[cap]; Str8Bld name = Str8Bld_Create(temp##__line__, cap);


#include "momo_strings.cpp"

#endif //MOMO_STRING_H