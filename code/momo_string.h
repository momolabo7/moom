/* date = November 20th 2021 11:58 am */

#ifndef MOMO_STRING_H
#define MOMO_STRING_H


//~ NOTE(Momo): Immutable strings
typedef struct Str8 {
	U8* e;
	UMI count;
} Str8;

static Str8 Str8_Create(U8* str, UMI size);
static Str8 Str8_Substr(Str8 str, UMI start, UMI ope);
static B32  Str8_Match(Str8 lhs, Str8 rhs);

#define Str8_Lit(s) Str8_Create((U8*)(s), sizeof(s)-1)

#if IS_CPP
static B32 operator==(Str8 lhs, Str8 rhs);
static B32 operator!=(Str8 lhs, Str8 rhs);
#endif //IS_CPP


// TODO(Momo): String split
// TODO(Momo): string nodes?


//~ NOTE(Momo): String builders
typedef struct Str8Bld {
	union {
		Str8 str;
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

#define Str8Bld_Temp(name, cap) U8 temp##__LINE__[cap]; Str8Bld name = Str8Bld_Create(temp##__LINE__, cap);

#endif //MOMO_STRING_H