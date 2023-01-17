
#ifndef MOMO_VECTOR_H
#define MOMO_VECTOR_H


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

static v2f_t   v2f_zero(void);
static v2f_t   v2f_set(f32_t x, f32_t y); 
static v2f_t   v2f_add(v2f_t lhs, v2f_t rhs);
static v2f_t   v2f_sub(v2f_t lhs, v2f_t rhs);
static v2f_t   v2f_scale(v2f_t lhs, f32_t rhs);
static v2f_t	 v2f_inv(v2f_t v);
static v2f_t	 v2f_ratio(v2f_t lhs, v2f_t rhs);
static v2f_t   v2f_div(v2f_t lhs, f32_t rhs);
static f32_t   v2f_dot(v2f_t lhs, v2f_t rhs);
static f32_t   v2f_dist_sq(v2f_t lhs, v2f_t rhs);
static f32_t   v2f_len_sq(v2f_t v);
static v2f_t   v2f_negate(v2f_t v);
static b32_t   v2f_is_close(v2f_t lhs, v2f_t rhs);
static v2f_t   v2f_mid(v2f_t lhs, v2f_t rhs);
static f32_t   v2f_dist(v2f_t lhs, v2f_t rhs);
static f32_t   v2f_len(v2f_t v);
static v2f_t   v2f_norm(v2f_t v);
static v2f_t   v2f_proj(v2f_t v, v2f_t onto);
static v2f_t   v2f_rotate(v2f_t v, f32_t rad);
static f32_t   v2f_angle(v2f_t lhs, v2f_t rhs);
static f32_t   v2f_cross(v2f_t lhs, v2f_t rhs);
static v2f_t   v2f_lerp(v2f_t s, v2f_t e, f32_t a);

static v2u_t   v2u_add(v2u_t lhs, v2u_t rhs);
static v2u_t   v2u_sub(v2u_t lhs, v2u_t rhs);

static v3f_t   v3f_add(v3f_t lhs, v3f_t rhs);
static v3f_t   v3f_sub(v3f_t lhs, v3f_t rhs);
static v3f_t   v3f_scale(v3f_t lhs, f32_t rhs);
static v3f_t   v3f_div(v3f_t lhs, f32_t rhs);
static f32_t   v3f_dot(v3f_t lhs, v3f_t rhs);
static f32_t   v3f_dist_sq(v3f_t lhs, v3f_t rhs);
static f32_t   v3f_len_sq(v3f_t v);
static b32_t   v3f_is_close(v3f_t lhs, v3f_t rhs);
static v3f_t   v3f_negate(v3f_t v);
static v3f_t   v3f_mid(v3f_t lhs, v3f_t rhs);
static v3f_t   v3f_cross(v3f_t lhs, v3f_t rhs);
static f32_t   v3f_dist(v3f_t lhs, v3f_t rhs);
static f32_t   v3f_len(v3f_t v);
static v3f_t   v3f_norm(v3f_t v);
static v3f_t   v3f_proj(v3f_t v, v3f_t onto);
static f32_t 	 v3f_angle(v3f_t lhs, v3f_t rhs);

static v2f_t  operator+(v2f_t lhs, v2f_t rhs);
static v2f_t  operator-(v2f_t lhs, v2f_t rhs);
static v2f_t  operator*(v2f_t lhs, f32_t rhs); // scale
static v2f_t  operator*(f32_t lhs, v2f_t rhs); // scale
static b32_t  operator==(v2f_t lhs, v2f_t rhs);
static b32_t  operator!=(v2f_t lhs, v2f_t rhs);
static v2f_t  operator-(v2f_t v);
static v2f_t& operator+=(v2f_t& lhs, v2f_t rhs);
static v2f_t& operator-=(v2f_t& lhs, v2f_t rhs);
static v2f_t& operator*=(v2f_t& lhs, v2f_t rhs);

static v3f_t  operator+(v3f_t lhs, v3f_t rhs);
static v3f_t  operator-(v3f_t lhs, v3f_t rhs);
static v3f_t  operator*(v3f_t lhs, f32_t rhs); // scale
static v3f_t  operator*(f32_t lhs, v3f_t rhs); // scale
static b32_t operator==(v3f_t lhs, v3f_t rhs);
static b32_t operator!=(v3f_t lhs, v3f_t rhs);
static v3f_t  operator-(v3f_t v);
static v3f_t& operator+=(v3f_t& lhs, v3f_t rhs);
static v3f_t& operator-=(v3f_t& lhs, v3f_t rhs);
static v3f_t& operator*=(v3f_t& lhs, v3f_t rhs);

////////////////////////////////////////////////////////
// IMPLEMENTATION
// NOTE(Momo): v2f_t
static v2f_t
v2f_zero(void) {
  v2f_t ret = { 0.f, 0.f };
  return ret;
}
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
v2f_dist_sq(v2f_t lhs, v2f_t rhs) {
  return v2f_len_sq(v2f_sub(lhs, rhs));
}
static f32_t  
v2f_dist(v2f_t lhs, v2f_t rhs) {
  return f32_sqrt(v2f_dist_sq(lhs, rhs));
}

static f32_t  
v2f_len_sq(v2f_t v) {
  return v2f_dot(v, v);
}

static f32_t   
v2f_len(v2f_t v) {
  return f32_sqrt(v2f_len_sq(v));
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
  ret.x = lerp_f32(s.x,e.x,a);
  ret.y = lerp_f32(s.y,e.y,a);
  return ret;
}

//~ NOTE(Momo): v3f_t
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
v3f_dist_sq(v3f_t lhs, v3f_t rhs) {
  return v3f_len_sq(v3f_sub(lhs, rhs));
}

static f32_t  
v3f_dist(v3f_t lhs, v3f_t rhs) {
  return f32_sqrt(v3f_dist_sq(lhs, rhs));
}

static f32_t  
v3f_len_sq(v3f_t v) {
  return v3f_dot(v, v);
}

static f32_t   
v3f_len(v3f_t v) {
  return f32_sqrt(v3f_len_sq(v));
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

// Operator Overloading
static v2f_t  operator+(v2f_t lhs, v2f_t rhs) { return v2f_add(lhs, rhs); }
static v2f_t  operator-(v2f_t lhs, v2f_t rhs) { return v2f_sub(lhs, rhs); }
static v2f_t  operator*(v2f_t lhs, f32_t rhs) { return v2f_scale(lhs, rhs); }
static v2f_t  operator*(f32_t lhs, v2f_t rhs) { return v2f_scale(rhs, lhs); }
static b32_t operator==(v2f_t lhs, v2f_t rhs) { return v2f_is_close(lhs, rhs); }
static b32_t operator!=(v2f_t lhs, v2f_t rhs) { return !v2f_is_close(lhs, rhs); }
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
static b32_t operator==(v3f_t lhs, v3f_t rhs) { return v3f_is_close(lhs, rhs); }
static b32_t operator!=(v3f_t lhs, v3f_t rhs) { return !v3f_is_close(lhs, rhs); }
static v3f_t  operator-(v3f_t v) { return v3f_negate(v); }
static v3f_t& operator+=(v3f_t& lhs, v3f_t rhs) { return lhs = v3f_add(lhs, rhs); } 
static v3f_t& operator-=(v3f_t& lhs, v3f_t rhs) { return lhs = v3f_sub(lhs, rhs); } 
static v3f_t& operator*=(v3f_t& lhs, f32_t rhs) { return lhs = v3f_scale(lhs, rhs); }


#endif //MOMO_VECTOR_H
