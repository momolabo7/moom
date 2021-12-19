#ifndef MOMO_MATRIX_H
#define MOMO_MATRIX_H


// NOTE(Momo): All matrices here are row-major
struct M44F32 {
	F32 e[4][4];
};

static M44F32 M44F32_Concat(M44F32 lhs, M44F32 rhs);
static M44F32 M44F32_Transpose(M44F32 m);
static M44F32 M44F32_Scale(F32 x, F32 y, F32 z);
static M44F32 M44F32_Identity();
static M44F32 M44F32_Translation(F32 x, F32 y, F32 z);
static M44F32 M44F32_RotationX(F32 radians);
static M44F32 M44F32_RotationY(F32 radians);
static M44F32 M44F32_RotationZ(F32 radians);
static M44F32 M44F32_Orthographic(F32 left, F32 right, F32 bottom, F32 top, F32 near, F32 far);
static M44F32 M44F32_Frustum(F32 left, F32 right, F32 bottom, F32 top, F32 near, F32 far);
static M44F32 M44F32_Perspective(F32 fov, F32 aspect, F32 near, F32 far);


#if IS_CPP
static M44F32 operator*(M44F32 lhs, M44F32 rhs);
#endif // IS_CPP



#endif //MOMO_MATRIX_H
