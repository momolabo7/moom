// NOTE(Momo): All matrices are in F32. I don't see
// any reason why they shouldn't be, at least for my use cases.

#ifndef MOMO_MATRIX_H
#define MOMO_MATRIX_H

// All matrices here are row-major
struct M44 {
	F32 e[4][4];
};

static M44 Concat(M44 lhs, M44 rhs);
static M44 Transpose(M44 m);

//-Constructors
static M44 M44_Scale(F32 x, F32 y, F32 z);
static M44 M44_Identity();
static M44 M44_Translation(F32 x, F32 y, F32 z);
static M44 M44_RotationX(F32 radians);
static M44 M44_RotationY(F32 radians);
static M44 M44_RotationZ(F32 radians);
static M44 M44_Orthographic(F32 left, F32 right, F32 bottom, F32 top, F32 near, F32 far);
static M44 M44_Frustum(F32 left, F32 right, F32 bottom, F32 top, F32 near, F32 far);
static M44 M44_Perspective(F32 fov, F32 aspect, F32 near, F32 far);

static M44 operator*(M44 lhs, M44 rhs);


#include "momo_matrix.cpp"

#endif //MOMO_MATRIX_H
