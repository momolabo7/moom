// Authors: Gerald Wong, momodevelop
// 
// This file contains implementation of a simple linear memory arena.
//
// Notes:
// - All matrices are in F32. I don't see any reason why they shouldn't be, 
//   at least for my use cases.
// - All matrix operations assume row-major
//
// Todo:
// - column major?
// 

#ifndef MOMO_MATRIX_H
#define MOMO_MATRIX_H

struct M44 {
	F32 e[4][4];
};

static M44 m44_concat(M44 lhs, M44 rhs);
static M44 m44_transpose(M44 m);
static M44 operator*(M44 lhs, M44 rhs);

//-Constructors
static M44 m44_scale(F32 x, F32 y, F32 z);
static M44 m44_identity();
static M44 m44_translation(F32 x, F32 y, F32 z);
static M44 m44_rotation_x(F32 radians);
static M44 m44_rotation_y(F32 radians);
static M44 m44_rotation_z(F32 radians);
static M44 m44_orthographic(F32 left, F32 right, F32 bottom, F32 top, F32 near, F32 far);
static M44 m44_frustum(F32 left, F32 right, F32 bottom, F32 top, F32 near, F32 far);
static M44 m44_perspective(F32 fov, F32 aspect, F32 near, F32 far);



#include "momo_matrix.cpp"

#endif //MOMO_MATRIX_H
