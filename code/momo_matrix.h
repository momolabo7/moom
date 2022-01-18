// NOTE(Momo): All matrices are in F32. I don't see
// any reason why they shouldn't be, at least for my use cases.

#ifndef MOMO_MATRIX_H
#define MOMO_MATRIX_H

// All matrices here are row-major
struct M44 {
	F32 e[4][4];
};

static M44 concat(M44 lhs, M44 rhs);
static M44 transpose(M44 m);

//-Constructors
static M44 create_m44_scale(F32 x, F32 y, F32 z);
static M44 create_m44_identity();
static M44 create_m44_translation(F32 x, F32 y, F32 z);
static M44 create_m44_rotation_x(F32 radians);
static M44 create_m44_rotation_y(F32 radians);
static M44 create_m44_rotation_z(F32 radians);
static M44 create_m44_orthographic(F32 left, F32 right, F32 bottom, F32 top, F32 near, F32 far);
static M44 create_m44_frustum(F32 left, F32 right, F32 bottom, F32 top, F32 near, F32 far);
static M44 create_m44_perspective(F32 fov, F32 aspect, F32 near, F32 far);

static M44 operator*(M44 lhs, M44 rhs);


#include "momo_matrix.cpp"

#endif //MOMO_MATRIX_H
