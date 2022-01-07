
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


static constexpr V2F32 Add(V2F32 lhs, V2F32 rhs);
static constexpr V2F32 Sub(V2F32 lhs, V2F32 rhs);
static constexpr V2F32 Scale(V2F32 lhs, F32 rhs);
static constexpr V2F32 InvScale(V2F32 lhs, F32 rhs);
static constexpr F32   Dot(V2F32 lhs, V2F32 rhs);
static constexpr F32   DistanceSq(V2F32 lhs, V2F32 rhs);
static constexpr F32   LengthSq(V2F32 v);
static constexpr V2F32 Negate(V2F32 v);
static constexpr B32   Match(V2F32 lhs, V2F32 rhs);
static constexpr V2F32 Midpoint(V2F32 lhs, V2F32 rhs);
static F32   Distance(V2F32 lhs, V2F32 rhs);
static F32   Length(V2F32 v);
static V2F32 Normalize(V2F32 v);
static V2F32 Project(V2F32 v, V2F32 onto);
static V2F32 Rotate(V2F32 v, F32 rad);

static constexpr V3F32 Add(V3F32 lhs, V3F32 rhs);
static constexpr V3F32 Sub(V3F32 lhs, V3F32 rhs);
static constexpr V3F32 Scale(V3F32 lhs, F32 rhs);
static constexpr V3F32 InvScale(V3F32 lhs, F32 rhs);
static constexpr F32   Dot(V3F32 lhs, V3F32 rhs);
static constexpr F32   DistanceSq(V3F32 lhs, V3F32 rhs);
static constexpr F32   LengthSq(V3F32 v);
static constexpr B32   Match(V3F32 lhs, V3F32 rhs);
static constexpr V3F32 Negate(V3F32 v);
static constexpr V3F32 Midpoint(V3F32 lhs, V3F32 rhs);
static constexpr V3F32 Cross(V3F32 lhs, V3F32 rhs);
static F32   Distance(V3F32 lhs, V3F32 rhs);
static F32   Length(V3F32 v);
static V3F32 Normalize(V3F32 v);
static V3F32 Project(V3F32 v, V3F32 onto);


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
