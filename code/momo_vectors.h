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


#include "momo_common.h"

typedef union {
	struct { U32 x, y; };
	struct { U32 w, h; };
	U32 e[2];
} V2U;

typedef union {
	struct { S32 x, y; };
	struct { S32 w, h; };
	S32 e[2];
} V2S;

typedef union {
	struct { F32 x, y; };
	struct { F32 w, h; };
	struct { F32 u, v; };
	F32 e[2];
} V2;

typedef union {
	struct { F32 x, y, z; };
	struct { F32 w, h, d; };
  F32 e[3];
} V3;

typedef union {
	struct { F32 x, y, z, w; };
  F32 e[4];
} V4;

static V2    v2(F32 x, F32 y); 
static V2    v2_from_v2u(V2U v);
static V2    v2_add(V2 lhs, V2 rhs);
static V2    v2_sub(V2 lhs, V2 rhs);
static V2    v2_scale(V2 lhs, F32 rhs);
static V2		 v2_inv(V2 v);
static V2		 v2_ratio(V2 lhs, V2 rhs);
static V2    v2_div(V2 lhs, F32 rhs);
static F32   v2_dot(V2 lhs, V2 rhs);
static F32   v2_dist_sq(V2 lhs, V2 rhs);
static F32   v2_len_sq(V2 v);
static V2    v2_negate(V2 v);
static B32   v2_is_close(V2 lhs, V2 rhs);
static V2    v2_mid(V2 lhs, V2 rhs);
static F32   v2_dist(V2 lhs, V2 rhs);
static F32   v2_len(V2 v);
static V2    v2_norm(V2 v);
static V2    v2_proj(V2 v, V2 onto);
static V2    v2_rotate(V2 v, F32 rad);
static F32   v2_angle(V2 lhs, V2 rhs);
static F32   v2_cross(V2 lhs, V2 rhs);

static V2U   v2u_from_v2(V2 v);
static V2U   v2u_add(V2U lhs, V2U rhs);
static V2U   v2u_sub(V2U lhs, V2U rhs);

static V3    v3_add(V3 lhs, V3 rhs);
static V3    v3_sub(V3 lhs, V3 rhs);
static V3    v3_scale(V3 lhs, F32 rhs);
static V3    v3_div(V3 lhs, F32 rhs);
static F32   v3_dot(V3 lhs, V3 rhs);
static F32   v3_dist_sq(V3 lhs, V3 rhs);
static F32   v3_len_sq(V3 v);
static B32   v3_is_close(V3 lhs, V3 rhs);
static V3    v3_negate(V3 v);
static V3    v3_mid(V3 lhs, V3 rhs);
static V3    v3_cross(V3 lhs, V3 rhs);
static F32   v3_dist(V3 lhs, V3 rhs);
static F32   v3_len(V3 v);
static V3    v3_norm(V3 v);
static V3    v3_proj(V3 v, V3 onto);
static F32 	 v3_angle(V3 lhs, V3 rhs);

#if IS_CPP
static V2  operator+(V2 lhs, V2 rhs);
static V2  operator-(V2 lhs, V2 rhs);
static V2  operator*(V2 lhs, F32 rhs); // scale
static V2  operator*(F32 lhs, V2 rhs); // scale
static B32 operator==(V2 lhs, V2 rhs);
static B32 operator!=(V2 lhs, V2 rhs);
static V2  operator-(V2 v);
static V2& operator+=(V2& lhs, V2 rhs);
static V2& operator-=(V2& lhs, V2 rhs);
static V2& operator*=(V2& lhs, V2 rhs);

static V3  operator+(V3 lhs, V3 rhs);
static V3  operator-(V3 lhs, V3 rhs);
static V3  operator*(V3 lhs, F32 rhs); // scale
static V3  operator*(F32 lhs, V3 rhs); // scale
static B32 operator==(V3 lhs, V3 rhs);
static B32 operator!=(V3 lhs, V3 rhs);
static V3  operator-(V3 v);
static V3& operator+=(V3& lhs, V3 rhs);
static V3& operator-=(V3& lhs, V3 rhs);
static V3& operator*=(V3& lhs, V3 rhs);
#endif // IS_CPP

////////////////////////////////////////////////////////
// IMPLEMENTATION
// NOTE(Momo): V2
static V2 
v2_add(V2 lhs, V2 rhs) {
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	return lhs;
}

static V2U
v2u_from_v2(V2 v) {
  return {
    (U32)v.x,
    (U32)v.y,
  };
}

static V2 
v2_sub(V2 lhs, V2 rhs) {
  lhs.x -= rhs.x;
  lhs.y -= rhs.y;
  return lhs;
}

static V2 
v2_scale(V2 lhs, F32 rhs) {
  lhs.x *= rhs;
  lhs.y *= rhs;
  return lhs;
}

static V2 
v2_div(V2 lhs, F32 rhs) {
  assert(!is_close_f32(rhs, 0.f));
  lhs.x /= rhs;
  lhs.y /= rhs;
  return lhs;
}

static V2
v2_inv(V2 v) {
  v.x = 1.f/v.x;
  v.y = 1.f/v.y;
  return v;
}

static V2 
v2_negate(V2 v) {
  v.x = -v.x;
  v.y = -v.y;
  return v;
}

static F32 
v2_dot(V2 lhs, V2 rhs) {
  return (lhs.x * rhs.x) + (lhs.y * rhs.y);
}

static F32  
v2_dist_sq(V2 lhs, V2 rhs) {
  return v2_len_sq(v2_sub(lhs, rhs));
}
static F32  
v2_dist(V2 lhs, V2 rhs) {
  return sqrt_f32(v2_dist_sq(lhs, rhs));
}

static F32  
v2_len_sq(V2 v) {
  return v2_dot(v, v);
}

static F32   
v2_len(V2 v) {
  return sqrt_f32(v2_len_sq(v));
}

static V2 
v2_norm(V2 v) {
  F32 len = v2_len(v);
  return v2_div(v, len);
}

static B32
v2_is_close(V2 lhs, V2 rhs) {
  return (is_close_f32(lhs.x, rhs.x) &&
          is_close_f32(lhs.y, rhs.y)); 
}

static V2 
v2_mid(V2 lhs, V2 rhs) {
  return v2_scale(v2_add(lhs, rhs), 0.5f); 
  
}
static V2 
v2_proj(V2 v, V2 onto) {
  // (to . from)/LenSq(to) * to
  F32 onto_len_sq = v2_len_sq(onto);
  assert(is_close_f32(onto_len_sq, 0.f));
  F32 v_dot_onto = v2_dot(v, onto);
  F32 scalar = v_dot_onto / onto_len_sq;
  V2 ret = v2_scale(onto, scalar);
  
  return ret;
}

// Angle Between
static F32
v2_angle(V2 lhs, V2 rhs) {
  F32 l_len = v2_len(lhs);
  F32 r_len = v2_len(rhs);
  F32 lr_dot = v2_dot(lhs, rhs);
  F32 ret = acos_f32(lr_dot/(l_len * r_len));
  return ret;
}

static V2 
v2_rotate(V2 v, F32 rad) {
  // Technically, we can use matrices but
  // meh, it's easy to code this out without it.
  // Removes dependencies too
  F32 c = cos_f32(rad);
  F32 s = sin_f32(rad);
  
  V2 ret = {};
  ret.x = (c * v.x) - (s * v.y);
  ret.y = (s * v.x) + (c * v.y);
  return ret;
}

static F32
v2_cross(V2 lhs, V2 rhs) {
  return  lhs.x * rhs.y - lhs.y * rhs.x;
}

//~ NOTE(Momo): V3
static V3 
v3_add(V3 lhs, V3 rhs) {
  lhs.x += rhs.x;
  lhs.y += rhs.y;
  lhs.z += rhs.z;
  return lhs;
}
static V3 
v3_sub(V3 lhs, V3 rhs) {
  lhs.x -= rhs.x;
  lhs.y -= rhs.y;
  lhs.z -= rhs.z;
  return lhs;
}

static V3 
v3_scale(V3 lhs, F32 rhs) {
  lhs.x *= rhs;
  lhs.y *= rhs;
  lhs.z *= rhs;
  return lhs;
}

static V3 
v3_div(V3 lhs, F32 rhs) {
  assert(!is_close_f32(rhs, 0.f));
  lhs.x /= rhs;
  lhs.y /= rhs;
  lhs.z /= rhs;
  return lhs;
}

static V3 
v3_negate(V3 v) {
  v.x = -v.x;
  v.y = -v.y;
  v.z = -v.z;
  return v;
}

static F32 
v3_dot(V3 lhs, V3 rhs) {
  return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
}

static F32  
v3_dist_sq(V3 lhs, V3 rhs) {
  return v3_len_sq(v3_sub(lhs, rhs));
}

static F32  
v3_dist(V3 lhs, V3 rhs) {
  return sqrt_f32(v3_dist_sq(lhs, rhs));
}

static F32  
v3_len_sq(V3 v) {
  return v3_dot(v, v);
}

static F32   
v3_len(V3 v) {
  return sqrt_f32(v3_len_sq(v));
}

static V3 
v3_norm(V3 v) {
  F32 len = v3_len(v);
  return v3_div(v, len);
}

static B32
v3_is_close(V3 lhs, V3 rhs) {
  return (is_close_f32(lhs.x, rhs.x) &&
          is_close_f32(lhs.y, rhs.y)); 
}

static V3 
v3_mid(V3 lhs, V3 rhs) {
  return v3_scale(v3_add(lhs, rhs), 0.5f); 
  
}
static V3 
v3_project(V3 v, V3 onto) {
  // (to . from)/LenSq(to) * to
  F32 onto_len_sq = v3_len_sq(onto);
  assert(!is_close_f32(onto_len_sq, 0.f));
  F32 v_dot_onto = v3_dot(v, onto);
  F32 scalar = v_dot_onto / onto_len_sq;
  V3 ret = v3_scale(onto, scalar);
  
  return ret;
}

static F32
v3_angle(V3 lhs, V3 rhs) {
  F32 l_len = v3_len(lhs);
  F32 r_len = v3_len(rhs);
  F32 lr_dot = v3_dot(lhs, rhs);
  F32 ret = acos_f32(lr_dot/(l_len * r_len));
  return ret;
}

static V3
v3_cross(V3 lhs, V3 rhs) {
  V3 ret = {};
  ret.x = (lhs.y * rhs.z) - (lhs.z * rhs.y);
  ret.y = (lhs.z * rhs.x) - (lhs.x * rhs.z);
  ret.z = (lhs.x * rhs.y) - (lhs.y * rhs.x);
  
  return ret;
}

//~ V2U
static V2U    
v2u_add(V2U lhs, V2U rhs) {
  lhs.x += rhs.x;
  lhs.y += rhs.y;
  return lhs;
}

static V2U    
v2u_sub(V2U lhs, V2U rhs){
  lhs.x -= rhs.x;
  lhs.y -= rhs.y;
  return lhs;
}

static V2    
v2_from_v2u(V2U v){
  return { (F32)v.x, (F32)v.y };
}

static V2    
v2(F32 x, F32 y){
  return { x, y };
}

#if IS_CPP
//~Operator Overloading
static V2  operator+(V2 lhs, V2 rhs) { return v2_add(lhs, rhs); }
static V2  operator-(V2 lhs, V2 rhs) { return v2_sub(lhs, rhs); }
static V2  operator*(V2 lhs, F32 rhs) { return v2_scale(lhs, rhs); }
static V2  operator*(F32 lhs, V2 rhs) { return v2_scale(rhs, lhs); }
static B32 operator==(V2 lhs, V2 rhs) { return v2_is_close(lhs, rhs); }
static B32 operator!=(V2 lhs, V2 rhs) { return !v2_is_close(lhs, rhs); }
static V2  operator-(V2 v) { return v2_negate(v); }
static V2& operator+=(V2& lhs, V2 rhs) { return lhs = v2_add(lhs, rhs); } 
static V2& operator-=(V2& lhs, V2 rhs) { return lhs = v2_sub(lhs, rhs); } 
static V2& operator*=(V2& lhs, F32 rhs) { return lhs = v2_scale(lhs, rhs); }

static V2U operator+(V2U lhs, V2U rhs) { return v2u_add(lhs, rhs); }
static V2U operator-(V2U lhs, V2U rhs) { return v2u_sub(lhs, rhs); }

static V3  operator+(V3 lhs, V3 rhs) { return v3_add(lhs, rhs); }
static V3  operator-(V3 lhs, V3 rhs) { return v3_sub(lhs, rhs); }
static V3  operator*(V3 lhs, F32 rhs) { return v3_scale(lhs, rhs); }
static V3  operator*(F32 lhs, V3 rhs) { return v3_scale(rhs, lhs); }
static B32 operator==(V3 lhs, V3 rhs) { return v3_is_close(lhs, rhs); }
static B32 operator!=(V3 lhs, V3 rhs) { return !v3_is_close(lhs, rhs); }
static V3  operator-(V3 v) { return v3_negate(v); }
static V3& operator+=(V3& lhs, V3 rhs) { return lhs = v3_add(lhs, rhs); } 
static V3& operator-=(V3& lhs, V3 rhs) { return lhs = v3_sub(lhs, rhs); } 
static V3& operator*=(V3& lhs, F32 rhs) { return lhs = v3_scale(lhs, rhs); }
#endif // IS_CPP


#endif //MOMO_VECTOR_H
