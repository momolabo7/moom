// Authors: Gerald Wong, momodevelop
// 
// This file contains implementation vectors.
//
// Notes:
// - As far as naming is concerned, 32-bit is the default size of each component,
//   and float is the default type of each component. If it's non-default, 
//   it will be explicitly named in the struct name itself. 
// - Thus V2's components are F32. V2S components are S32, V2F64 components are F64.
//  
// Todo:
// - 
// 

#ifndef MOMO_VECTOR_H
#define MOMO_VECTOR_H


union V2U {
	struct { U32 x, y; };
	struct { U32 w, h; };
	U32 e[2];
};

union V2S {
	struct { S32 x, y; };
	struct { S32 w, h; };
	S32 e[2];
};

union V2 {
	struct { F32 x, y; };
	struct { F32 w, h; };
	struct { F32 u, v; };
	F32 e[2];
};

union V3{
	struct { F32 x, y, z; };
	struct { F32 w, h, d; };
  F32 e[3];
};

union V4 {
	struct { F32 x, y, z, w; };
  F32 e[4];
};


static V2    add(V2 lhs, V2 rhs);
static V2    sub(V2 lhs, V2 rhs);
static V2    scale(V2 lhs, F32 rhs);
static V2    div(V2 lhs, F32 rhs);
static F32   dot(V2 lhs, V2 rhs);
static F32   distance_sq(V2 lhs, V2 rhs);
static F32   length_sq(V2 v);
static V2    negate(V2 v);
static B32   is_close(V2 lhs, V2 rhs);
static V2    midpoint(V2 lhs, V2 rhs);
static F32   distance(V2 lhs, V2 rhs);
static F32   length(V2 v);
static V2 normalize(V2 v);
static V2 project(V2 v, V2 onto);
static V2 rotate(V2 v, F32 rad);
static F32   angle_between(V2 lhs, V2 rhs);


static V3 add(V3 lhs, V3 rhs);
static V3 sub(V3 lhs, V3 rhs);
static V3 scale(V3 lhs, F32 rhs);
static V3 div(V3 lhs, F32 rhs);
static F32   dot(V3 lhs, V3 rhs);
static F32   distance_sq(V3 lhs, V3 rhs);
static F32   length_sq(V3 v);
static B32   is_close(V3 lhs, V3 rhs);
static V3 negate(V3 v);
static V3 midpoint(V3 lhs, V3 rhs);
static V3 Cross(V3 lhs, V3 rhs);
static F32   distance(V3 lhs, V3 rhs);
static F32   length(V3 v);
static V3 normalize(V3 v);
static V3 project(V3 v, V3 onto);
static F32 	angle_between(V3 lhs, V3 rhs);

static V2 operator+(V2 lhs, V2 rhs);
static V2 operator-(V2 lhs, V2 rhs);
static V2 operator*(V2 lhs, F32 rhs); // scale
static V2 operator*(F32 lhs, V2 rhs); // scale
static B32   operator==(V2 lhs, V2 rhs);
static B32   operator!=(V2 lhs, V2 rhs);
static V2 operator-(V2 v);
static V2& operator+=(V2& lhs, V2 rhs);
static V2& operator-=(V2& lhs, V2 rhs);
static V2& operator*=(V2& lhs, V2 rhs);

static V3 operator+(V3 lhs, V3 rhs);
static V3 operator-(V3 lhs, V3 rhs);
static V3 operator*(V3 lhs, F32 rhs); // scale
static V3 operator*(F32 lhs, V3 rhs); // scale
static B32   operator==(V3 lhs, V3 rhs);
static B32   operator!=(V3 lhs, V3 rhs);
static V3 operator-(V3 v);
static V3& operator+=(V3& lhs, V3 rhs);
static V3& operator-=(V3& lhs, V3 rhs);
static V3& operator*=(V3& lhs, V3 rhs);


#include "momo_vectors.cpp"

#endif //MOMO_VECTOR_H
