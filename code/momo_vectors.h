
#ifndef MOMO_VECTOR_H
#define MOMO_VECTOR_H


union V2U32 {
	struct { U32 x, y; };
	struct { U32 w, h; };
	U32 e[2];
};

union V2F32 {
	struct { F32 x, y; };
	struct { F32 w, h; };
	struct { F32 u, v; };
	F32 e[2];
};

union V3F32{
	struct { F32 x, y, z; };
	struct { F32 w, h, d; };
  F32 e[3];
};

union V4F32 {
	struct { F32 x, y, z, w; };
  F32 e[4];
};


static constexpr V2F32 add(V2F32 lhs, V2F32 rhs);
static constexpr V2F32 sub(V2F32 lhs, V2F32 rhs);
static constexpr V2F32 scale(V2F32 lhs, F32 rhs);
static constexpr V2F32 div(V2F32 lhs, F32 rhs);
static constexpr F32   dot(V2F32 lhs, V2F32 rhs);
static constexpr F32   distance_sq(V2F32 lhs, V2F32 rhs);
static constexpr F32   length_sq(V2F32 v);
static constexpr V2F32 negate(V2F32 v);
static constexpr B32   match(V2F32 lhs, V2F32 rhs);
static constexpr V2F32 midpoint(V2F32 lhs, V2F32 rhs);
static F32   distance(V2F32 lhs, V2F32 rhs);
static F32   length(V2F32 v);
static V2F32 normalize(V2F32 v);
static V2F32 project(V2F32 v, V2F32 onto);
static V2F32 rotate(V2F32 v, F32 rad);
static F32   angle_between(V2F32 lhs, V2F32 rhs);


static constexpr V3F32 add(V3F32 lhs, V3F32 rhs);
static constexpr V3F32 sub(V3F32 lhs, V3F32 rhs);
static constexpr V3F32 scale(V3F32 lhs, F32 rhs);
static constexpr V3F32 div(V3F32 lhs, F32 rhs);
static constexpr F32   dot(V3F32 lhs, V3F32 rhs);
static constexpr F32   distance_sq(V3F32 lhs, V3F32 rhs);
static constexpr F32   length_sq(V3F32 v);
static constexpr B32   match(V3F32 lhs, V3F32 rhs);
static constexpr V3F32 negate(V3F32 v);
static constexpr V3F32 midpoint(V3F32 lhs, V3F32 rhs);
static constexpr V3F32 Cross(V3F32 lhs, V3F32 rhs);
static F32   distance(V3F32 lhs, V3F32 rhs);
static F32   length(V3F32 v);
static V3F32 normalize(V3F32 v);
static V3F32 project(V3F32 v, V3F32 onto);
static F32 	angle_between(V3F32 lhs, V3F32 rhs);

// TODO(Momo): I'm not sure if we should actually
// do operator* because there's so many damn ways
// to multiply a vector. Maybe we'll use the most 
// commonly used operation?
static constexpr V2F32 operator+(V2F32 lhs, V2F32 rhs);
static constexpr V2F32 operator-(V2F32 lhs, V2F32 rhs);
static constexpr V2F32 operator*(V2F32 lhs, F32 rhs); // scale
static constexpr V2F32 operator*(F32 lhs, V2F32 rhs); // scale
static constexpr B32   operator==(V2F32 lhs, V2F32 rhs);
static constexpr B32   operator!=(V2F32 lhs, V2F32 rhs);
static constexpr V2F32 operator-(V2F32 v);
static constexpr V2F32& operator+=(V2F32& lhs, V2F32 rhs);
static constexpr V2F32& operator-=(V2F32& lhs, V2F32 rhs);
static constexpr V2F32& operator*=(V2F32& lhs, V2F32 rhs);

static constexpr V3F32 operator+(V3F32 lhs, V3F32 rhs);
static constexpr V3F32 operator-(V3F32 lhs, V3F32 rhs);
static constexpr V3F32 operator*(V3F32 lhs, F32 rhs); // scale
static constexpr V3F32 operator*(F32 lhs, V3F32 rhs); // scale
static constexpr B32   operator==(V3F32 lhs, V3F32 rhs);
static constexpr B32   operator!=(V3F32 lhs, V3F32 rhs);
static constexpr V3F32 operator-(V3F32 v);
static constexpr V3F32& operator+=(V3F32& lhs, V3F32 rhs);
static constexpr V3F32& operator-=(V3F32& lhs, V3F32 rhs);
static constexpr V3F32& operator*=(V3F32& lhs, V3F32 rhs);


#include "momo_vectors.cpp"

#endif //MOMO_VECTOR_H
