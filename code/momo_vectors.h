
#ifndef MOMO_VECTOR_H
#define MOMO_VECTOR_H

typedef union {
	struct { U32 x, y; };
	struct { U32 w, h; };
	U32 e[2];
} V2U32;

typedef union {
	struct { F32 x, y; };
	struct { F32 w, h; };
	struct { F32 u, v; };
	F32 e[2];
} V2F32;

typedef union {
	struct { F32 x, y, z; };
	struct { F32 w, h, d; };
  F32 e[3];
} V3F32;

typedef union {
	struct { F32 x, y, z, w; };
  F32 e[4];
} V4F32;


static V2F32 V2F32_Add(V2F32 lhs, V2F32 rhs);
static V2F32 V2F32_Sub(V2F32 lhs, V2F32 rhs);
static V2F32 V2F32_Scale(V2F32 lhs, F32 rhs);
static V2F32 V2F32_InvScale(V2F32 lhs, F32 rhs);
static F32   V2F32_Dot(V2F32 lhs, V2F32 rhs);
static F32   V2F32_DistanceSq(V2F32 lhs, V2F32 rhs);
static F32   V2F32_Distance(V2F32 lhs, V2F32 rhs);
static F32   V2F32_LengthSq(V2F32 v);
static F32   V2F32_Length(V2F32 v);
static V2F32 V2F32_Normalize(V2F32 v);
static V2F32 V2F32_Negate(V2F32 v);
static B8    V2Match_F32(V2F32 lhs, V2F32 rhs);
static V2F32 V2F32_Midpoint(V2F32 lhs, V2F32 rhs);
static V2F32 V2F32_Project(V2F32 v, V2F32 onto);
static V2F32 V2F32_Rotate(V2F32 v, F32 rad);

static V3F32 V3F32_Add(V3F32 lhs, V3F32 rhs);
static V3F32 V3F32_Sub(V3F32 lhs, V3F32 rhs);
static V3F32 V3F32_Scale(V3F32 lhs, F32 rhs);
static V3F32 V3F32_InvScale(V3F32 lhs, F32 rhs);
static F32   V3F32_Dot(V3F32 lhs, V3F32 rhs);
static F32   V3F32_DistanceSq(V3F32 lhs, V3F32 rhs);
static F32   V3F32_Distance(V3F32 lhs, V3F32 rhs);
static F32   V3F32_LengthSq(V3F32 v);
static F32   V3F32_Length(V3F32 v);
static V3F32 V3F32_Normalize(V3F32 v);
static V3F32 V3F32_Negate(V3F32 v);
static B8    V3Match_F32(V3F32 lhs, V3F32 rhs);
static V3F32 V3F32_Midpoint(V3F32 lhs, V3F32 rhs);
static V3F32 V3F32_Project(V3F32 v, V3F32 onto);
static V3F32 V3F32_Cross(V3F32 lhs, V3F32 rhs);


#if IS_CPP
// TODO(Momo): I'm not sure if we should actually
// do operator* because there's so many damn ways
// to multiply a vector. Maybe we'll use the most 
// commonly used operation?
static V2F32& operator+(V2F32 lhs, V2F32 rhs);
static V2F32& operator-(V2F32 lhs, V2F32 rhs);
static V2F32& operator*(V2F32 lhs, V2F32 rhs); // scale
static V2F32& operator==(V2F32 lhs, V2F32 rhs);
static V2F32& operator!=(V2F32 lhs, V2F32 rhs);
static V2F32& operator-(V2F32 v);
static V2F32& operator+=(V2F32& lhs, V2F32 rhs);
static V2F32& operator-=(V2F32& lhs, V2F32 rhs);
static V2F32& operator-=(V2F32& lhs, V2F32 rhs);

#endif //IS_CPP


#include "momo_vectors.cpp"

#endif //MOMO_VECTOR_H
