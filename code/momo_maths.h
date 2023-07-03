

#ifndef MOMO_MATH_H
#define MOMO_MATH_H


struct m44f_t {
	f32_t e[4][4];
};

union v2u_t {
	struct { u32_t x, y; };
	struct { u32_t w, h; };
	u32_t e[2];
};

union v2s_t {
	struct { s32_t x, y; };
	struct { s32_t w, h; };
	s32_t e[2];
};

union v2f_t {
	struct { f32_t x, y; };
	struct { f32_t w, h; };
	struct { f32_t u, v; };
	f32_t e[2];
};

union v3f_t {
	struct { f32_t x, y, z; };
	struct { f32_t w, h, d; };
  f32_t e[3];
};

union v4f_t {
	struct { f32_t x, y, z, w; };
  f32_t e[4];
};



//
// NOTE(Momo): v2f_t
//
static v2f_t 
v2f_add(v2f_t lhs, v2f_t rhs) {
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	return lhs;
}

static v2f_t 
v2f_sub(v2f_t lhs, v2f_t rhs) {
  lhs.x -= rhs.x;
  lhs.y -= rhs.y;
  return lhs;
}

static v2f_t 
v2f_scale(v2f_t lhs, f32_t rhs) {
  lhs.x *= rhs;
  lhs.y *= rhs;
  return lhs;
}

static v2f_t 
v2f_div(v2f_t lhs, f32_t rhs) {
  assert(!is_close_f32(rhs, 0.f));
  lhs.x /= rhs;
  lhs.y /= rhs;
  return lhs;
}

static v2f_t
v2f_inv(v2f_t v) {
  v.x = 1.f/v.x;
  v.y = 1.f/v.y;
  return v;
}

static v2f_t 
v2f_negate(v2f_t v) {
  v.x = -v.x;
  v.y = -v.y;
  return v;
}

static f32_t 
v2f_dot(v2f_t lhs, v2f_t rhs) {
  return (lhs.x * rhs.x) + (lhs.y * rhs.y);
}

static f32_t  
v2f_len_sq(v2f_t v) {
  return v2f_dot(v, v);
}

static f32_t   
v2f_len(v2f_t v) {
  return f32_sqrt(v2f_len_sq(v));
}
static f32_t  
v2f_dist_sq(v2f_t lhs, v2f_t rhs) {
  return v2f_len_sq(v2f_sub(lhs, rhs));
}
static f32_t  
v2f_dist(v2f_t lhs, v2f_t rhs) {
  return f32_sqrt(v2f_dist_sq(lhs, rhs));
}


static v2f_t 
v2f_norm(v2f_t v) {
  f32_t len = v2f_len(v);
  return v2f_div(v, len);
}

static b32_t
v2f_is_close(v2f_t lhs, v2f_t rhs) {
  return (is_close_f32(lhs.x, rhs.x) &&
          is_close_f32(lhs.y, rhs.y)); 
}

static v2f_t 
v2f_mid(v2f_t lhs, v2f_t rhs) {
  return v2f_scale(v2f_add(lhs, rhs), 0.5f); 
  
}
static v2f_t 
v2f_proj(v2f_t v, v2f_t onto) {
  // (to . from)/LenSq(to) * to
  f32_t onto_len_sq = v2f_len_sq(onto);
  assert(!is_close_f32(onto_len_sq, 0.f));
  f32_t v_dot_onto = v2f_dot(v, onto);
  f32_t scalar = v_dot_onto / onto_len_sq;
  v2f_t ret = v2f_scale(onto, scalar);
  
  return ret;
}

// Angle Between
static f32_t
v2f_angle(v2f_t lhs, v2f_t rhs) {
  f32_t l_len = v2f_len(lhs);
  f32_t r_len = v2f_len(rhs);
  f32_t lr_dot = v2f_dot(lhs, rhs);
  f32_t ret = f32_acos(lr_dot/(l_len * r_len));
  return ret;
}

static v2f_t 
v2f_rotate(v2f_t v, f32_t rad) {
  // Technically, we can use matrices but
  // meh, it's easy to code this out without it.
  // Removes dependencies too
  f32_t c = f32_cos(rad);
  f32_t s = f32_sin(rad);
  
  v2f_t ret = {};
  ret.x = (c * v.x) - (s * v.y);
  ret.y = (s * v.x) + (c * v.y);
  return ret;
}

static f32_t
v2f_cross(v2f_t lhs, v2f_t rhs) {
  return  lhs.x * rhs.y - lhs.y * rhs.x;
}

static v2f_t  
v2f_lerp(v2f_t s, v2f_t e, f32_t a) 
{
  v2f_t ret = {0};
  ret.x = f32_lerp(s.x,e.x,a);
  ret.y = f32_lerp(s.y,e.y,a);
  return ret;
}

//
// NOTE(Momo): v3f_t
//
static v3f_t 
v3f_add(v3f_t lhs, v3f_t rhs) {
  lhs.x += rhs.x;
  lhs.y += rhs.y;
  lhs.z += rhs.z;
  return lhs;
}
static v3f_t 
v3f_sub(v3f_t lhs, v3f_t rhs) {
  lhs.x -= rhs.x;
  lhs.y -= rhs.y;
  lhs.z -= rhs.z;
  return lhs;
}

static v3f_t 
v3f_scale(v3f_t lhs, f32_t rhs) {
  lhs.x *= rhs;
  lhs.y *= rhs;
  lhs.z *= rhs;
  return lhs;
}

static v3f_t 
v3f_div(v3f_t lhs, f32_t rhs) {
  assert(!is_close_f32(rhs, 0.f));
  lhs.x /= rhs;
  lhs.y /= rhs;
  lhs.z /= rhs;
  return lhs;
}

static v3f_t 
v3f_negate(v3f_t v) {
  v.x = -v.x;
  v.y = -v.y;
  v.z = -v.z;
  return v;
}

static f32_t 
v3f_dot(v3f_t lhs, v3f_t rhs) {
  return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
}

static f32_t  
v3f_len_sq(v3f_t v) {
  return v3f_dot(v, v);
}

static f32_t   
v3f_len(v3f_t v) {
  return f32_sqrt(v3f_len_sq(v));
}

static f32_t  
v3f_dist_sq(v3f_t lhs, v3f_t rhs) {
  return v3f_len_sq(v3f_sub(lhs, rhs));
}

static f32_t  
v3f_dist(v3f_t lhs, v3f_t rhs) {
  return f32_sqrt(v3f_dist_sq(lhs, rhs));
}


static v3f_t 
v3f_norm(v3f_t v) {
  f32_t len = v3f_len(v);
  return v3f_div(v, len);
}

static b32_t
v3f_is_close(v3f_t lhs, v3f_t rhs) {
  return (is_close_f32(lhs.x, rhs.x) &&
          is_close_f32(lhs.y, rhs.y)); 
}

static v3f_t 
v3f_mid(v3f_t lhs, v3f_t rhs) {
  return v3f_scale(v3f_add(lhs, rhs), 0.5f); 
  
}
static v3f_t 
v3f_project(v3f_t v, v3f_t onto) {
  // (to . from)/LenSq(to) * to
  f32_t onto_len_sq = v3f_len_sq(onto);
  assert(!is_close_f32(onto_len_sq, 0.f));
  f32_t v_dot_onto = v3f_dot(v, onto);
  f32_t scalar = v_dot_onto / onto_len_sq;
  v3f_t ret = v3f_scale(onto, scalar);
  
  return ret;
}

static f32_t
v3f_angle(v3f_t lhs, v3f_t rhs) {
  f32_t l_len = v3f_len(lhs);
  f32_t r_len = v3f_len(rhs);
  f32_t lr_dot = v3f_dot(lhs, rhs);
  f32_t ret = f32_acos(lr_dot/(l_len * r_len));
  return ret;
}

static v3f_t
v3f_cross(v3f_t lhs, v3f_t rhs) {
  v3f_t ret = {};
  ret.x = (lhs.y * rhs.z) - (lhs.z * rhs.y);
  ret.y = (lhs.z * rhs.x) - (lhs.x * rhs.z);
  ret.z = (lhs.x * rhs.y) - (lhs.y * rhs.x);
  
  return ret;
}

//~ v2u_t
static v2u_t    
v2u_add(v2u_t lhs, v2u_t rhs) {
  lhs.x += rhs.x;
  lhs.y += rhs.y;
  return lhs;
}

static v2u_t    
v2u_sub(v2u_t lhs, v2u_t rhs){
  lhs.x -= rhs.x;
  lhs.y -= rhs.y;
  return lhs;
}

static v2f_t    
v2f_set(f32_t x, f32_t y){
  return { x, y };
}

static v2f_t    
v2f_zero(void){
  return { 0, 0 };
}


//
// m44f
//
static m44f_t
m44f_concat(m44f_t lhs, m44f_t rhs) {
	m44f_t ret = {};
  for (u32_t r = 0; r < 4; r++) { 
    for (u32_t c = 0; c < 4; c++) { 
      for (u32_t i = 0; i < 4; i++) {
				ret.e[r][c] += lhs.e[r][i] *  rhs.e[i][c]; 
			}
		} 
	} 
	return ret;
}

static m44f_t 
m44f_transpose(m44f_t m) {
	m44f_t ret = {};
	for (u32_t i = 0; i < 4; ++i ) {
		for (u32_t j = 0; j < 4; ++j) {
			ret.e[i][j] = m.e[j][i];
		}
	}
	return ret;
}
static m44f_t m44f_scale(f32_t x, f32_t y, f32_t z) {
	m44f_t ret = {};
	ret.e[0][0] = x;
	ret.e[1][1] = y;
	ret.e[2][2] = z;
	ret.e[3][3] = 1.f;
	
	return ret;
}

static m44f_t 
m44f_identity() {
	m44f_t ret = {};
	ret.e[0][0] = 1.f;
	ret.e[1][1] = 1.f;
	ret.e[2][2] = 1.f;
	ret.e[3][3] = 1.f;
	
	return ret;
}

static m44f_t 
m44f_translation(f32_t x, f32_t y, f32_t z = 0.f) {
	m44f_t ret = m44f_identity();
	ret.e[0][3] = x;
	ret.e[1][3] = y;
	ret.e[2][3] = z;
	ret.e[3][3] = 1.f;
	
	return ret;
}

static m44f_t 
m44f_rotation_x(f32_t rad) {
	// NOTE(Momo): 
	// 1  0  0  0
	// 0  c -s  0
	// 0  s  c  0
	// 0  0  0  1
	f32_t c = f32_cos(rad);
	f32_t s = f32_sin(rad);
	m44f_t ret = {};
	ret.e[0][0] = 1.f;
	ret.e[3][3] = 1.f;
	ret.e[1][1] = c;
	ret.e[1][2] = -s;
	ret.e[2][1] = s;
	ret.e[2][2] = c;
	
	return ret;
}
static m44f_t m44f_rotation_y(f32_t rad) {
	
	// NOTE(Momo): 
	//  c  0  s  0
	//  0  1  0  0
	// -s  0  c  0
	//  0  0  0  1
	f32_t c = f32_cos(rad);
	f32_t s = f32_sin(rad);
	m44f_t ret = {};
	ret.e[0][0] = c;
	ret.e[0][2] = s;
	ret.e[1][1] = 1.f;
	ret.e[2][0] = -s;
	ret.e[2][2] = c;
	ret.e[3][3] = 1.f;
	
	return ret;
}

static m44f_t 
m44f_rotation_z(f32_t rad) {
	// NOTE(Momo): 
	//  c -s  0  0
	//  s  c  0  0
	//  0  0  1  0
	//  0  0  0  1
	
	f32_t c = f32_cos(rad);
	f32_t s = f32_sin(rad);
	m44f_t ret = {};
	ret.e[0][0] = c;
	ret.e[0][1] = -s;
	ret.e[1][0] = s;
	ret.e[1][1] = c;
	ret.e[2][2] = 1.f;
	ret.e[3][3] = 1.f;
	
	return ret;
}

static m44f_t 
m44f_orthographic(f32_t left, f32_t right, f32_t bottom, f32_t top, f32_t near, f32_t far) {
	
	m44f_t ret = {0};
	ret.e[0][0] = 2.f/(right-left);
	ret.e[1][1] = 2.f/(top-bottom);
	ret.e[2][2] = 2.f/(far-near);
	ret.e[3][3] = 1.f;
	ret.e[0][3] = -(right+left)/(right-left);
	ret.e[1][3] = -(top+bottom)/(top-bottom);
	ret.e[2][3] = -(far+near)/(far-near);
	
	return ret;
}

static m44f_t 
m44f_frustum(f32_t left, f32_t right, f32_t bottom, f32_t top, f32_t near, f32_t far) {
	m44f_t ret = {};
	ret.e[0][0] = (2.f*near)/(right-left);
	ret.e[1][1] = (2.f*near)/(top-bottom);
	ret.e[2][2] = -(far+near)/(far-near);
	ret.e[3][2] = 1;  
	ret.e[0][2] = (right+left)/(right-left);
	ret.e[1][2] = (top+bottom)/(top-bottom);
	ret.e[1][3] = -near*(top+bottom)/(top-bottom);
	ret.e[2][3] = 2.f*far*near/(far-near);
	
	return ret;
}

static m44f_t 
m44f_perspective(f32_t fov, f32_t aspect, f32_t near, f32_t far){
	f32_t top = near * f32_tan(fov*0.5f);
	f32_t right = top * aspect;
	return m44f_frustum(-right, right,
                     -top, top,
                     near, far);
}

//
// Operator overloading
//
static v2f_t  operator+(v2f_t lhs, v2f_t rhs) { return v2f_add(lhs, rhs); }
static v2f_t  operator-(v2f_t lhs, v2f_t rhs) { return v2f_sub(lhs, rhs); }
static v2f_t  operator*(v2f_t lhs, f32_t rhs) { return v2f_scale(lhs, rhs); }
static v2f_t  operator*(f32_t lhs, v2f_t rhs) { return v2f_scale(rhs, lhs); }
static b32_t  operator==(v2f_t lhs, v2f_t rhs) { return v2f_is_close(lhs, rhs); }
static b32_t  operator!=(v2f_t lhs, v2f_t rhs) { return !v2f_is_close(lhs, rhs); }
static v2f_t  operator-(v2f_t v) { return v2f_negate(v); }
static v2f_t& operator+=(v2f_t& lhs, v2f_t rhs) { return lhs = v2f_add(lhs, rhs); } 
static v2f_t& operator-=(v2f_t& lhs, v2f_t rhs) { return lhs = v2f_sub(lhs, rhs); } 
static v2f_t& operator*=(v2f_t& lhs, f32_t rhs) { return lhs = v2f_scale(lhs, rhs); }

static v2u_t operator+(v2u_t lhs, v2u_t rhs) { return v2u_add(lhs, rhs); }
static v2u_t operator-(v2u_t lhs, v2u_t rhs) { return v2u_sub(lhs, rhs); }

static v3f_t  operator+(v3f_t lhs, v3f_t rhs) { return v3f_add(lhs, rhs); }
static v3f_t  operator-(v3f_t lhs, v3f_t rhs) { return v3f_sub(lhs, rhs); }
static v3f_t  operator*(v3f_t lhs, f32_t rhs) { return v3f_scale(lhs, rhs); }
static v3f_t  operator*(f32_t lhs, v3f_t rhs) { return v3f_scale(rhs, lhs); }
static b32_t  operator==(v3f_t lhs, v3f_t rhs) { return v3f_is_close(lhs, rhs); }
static b32_t  operator!=(v3f_t lhs, v3f_t rhs) { return !v3f_is_close(lhs, rhs); }
static v3f_t  operator-(v3f_t v) { return v3f_negate(v); }
static v3f_t& operator+=(v3f_t& lhs, v3f_t rhs) { return lhs = v3f_add(lhs, rhs); } 
static v3f_t& operator-=(v3f_t& lhs, v3f_t rhs) { return lhs = v3f_sub(lhs, rhs); } 
static v3f_t& operator*=(v3f_t& lhs, f32_t rhs) { return lhs = v3f_scale(lhs, rhs); }

static m44f_t operator*(m44f_t lhs, m44f_t rhs) {
  return m44f_concat(lhs, rhs);
}
#endif //MOMO_MATH_H
