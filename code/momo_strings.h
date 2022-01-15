/* date = november 20th 2021 11:58 am */

#ifndef momo_string_h
#define momo_string_h


//~ NOTE(Momo): 'Immutable' Strings
typedef struct {
	U8* e;
	UMI count;
} Str8;

static Str8 CreateStr8(U8* Str, UMI size);
static Str8 Substr(Str8 Str, UMI start, UMI ope);
static B32  Match(Str8 lhs, Str8 rhs);

// No one should be using this
// TODO: Maybe make a macro to enable/disable this?
#define Str8FromLit(s) CreateStr8((U8*)(s), sizeof(s)-1)

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
};

static Str8Bld  CreateStr8Bld(U8* data, UMI cap);

static UMI      Remaining(Str8Bld* sb);
static void     Clear(Str8Bld* sb);
static void     Pop(Str8Bld* sb);
static void     PushC8(Str8Bld* sb, C8 num);
static void     PushU32(Str8Bld* sb, U32 num);
static void     PushF32(Str8Bld* sb, F32 value, U32 precision);
static void     PushS32(Str8Bld* sb, S32 num);
static void     PushStr8(Str8Bld* sb, Str8 num);

// TODO: We should remove this and figure out a better way to do format strings.
static void     PushFmt(Str8Bld* sb, Str8 fmt, ...);

//#define Str8Bld_temp(name, cap) U8 temp##__line__[cap]; Str8Bld name = Str8Bld_Create(temp##__line__, cap);


#include "momo_strings.cpp"

#endif //MOMO_STRING_H