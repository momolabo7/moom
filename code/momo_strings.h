/* date = november 20th 2021 11:58 am */

#ifndef momo_string_h
#define momo_string_h


//~ note(momo): immutable Strings
typedef struct {
	U8* e;
	UMI count;
} Str8;

static Str8 Str8_Create(U8* Str, UMI size);
static Str8 Str8_Substr(Str8 Str, UMI start, UMI ope);
static B32  Str8_Match(Str8 lhs, Str8 rhs);

#define Str8_lit(s) Str8_Create((U8*)(s), sizeof(s)-1)

#if IS_CPP
static B32 operator==(Str8 lhs, Str8 rhs);
static B32 operator!=(Str8 lhs, Str8 rhs);
#endif // IS_CPP


// todo(momo): String split
// todo(momo): String nodes?


//~ note(momo): String builders
typedef struct {
	union {
		Str8 Str;
		struct {
			U8* e;
			UMI count;
		};
	};
	UMI cap;
} Str8Bld;

static Str8Bld  Str8Bld_Create(U8* data, UMI cap);
static UMI      Str8Bld_Remain(Str8Bld* sb);
static void     Str8Bld_Clear(Str8Bld* sb);
static void     Str8Bld_Pop(Str8Bld* sb);
static void     Str8Bld_PushC8(Str8Bld* sb, C8 num);
static void     Str8Bld_PushU32(Str8Bld* sb, U32 num);
static void     Str8Bld_PushF32(Str8Bld* sb, F32 value, U32 precision);
static void     Str8Bld_PushS32(Str8Bld* sb, S32 num);
static void     Str8Bld_PushFmt(Str8Bld* sb, Str8 fmt, ...);
static void     Str8Bld_PushStr8(Str8Bld* sb, Str8 num);

#define Str8Bld_temp(name, cap) U8 temp##__line__[cap]; Str8Bld name = Str8Bld_Create(temp##__line__, cap);


#include "momo_strings.cpp"

#endif //MOMO_STRING_H