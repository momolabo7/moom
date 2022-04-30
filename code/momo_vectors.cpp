//~ NOTE(Momo): V2
static V2 
add(V2 lhs, V2 rhs) {
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	return lhs;
}

static V2 
sub(V2 lhs, V2 rhs) {
  lhs.x -= rhs.x;
  lhs.y -= rhs.y;
  return lhs;
}

static V2 
scale(V2 lhs, F32 rhs) {
  lhs.x *= rhs;
  lhs.y *= rhs;
  return lhs;
}

static V2 
div(V2 lhs, F32 rhs) {
  assert(!is_close(rhs, 0.f));
  lhs.x /= rhs;
  lhs.y /= rhs;
  return lhs;
}

static V2 
negate(V2 v) {
  v.x = -v.x;
  v.y = -v.y;
  return v;
}

static F32 
dot(V2 lhs, V2 rhs) {
  return (lhs.x * rhs.x) + (lhs.y * rhs.y);
}

static F32  
distance_sq(V2 lhs, V2 rhs) {
  return length_sq(sub(lhs, rhs));
}
static F32  
distance(V2 lhs, V2 rhs) {
  return sqrt(distance_sq(lhs, rhs));
}

static F32  
length_sq(V2 v) {
  return dot(v, v);
}

static F32   
length(V2 v) {
  return sqrt(length_sq(v));
}

static V2 
normalize(V2 v) {
  F32 leng = length(v);
  return div(v, leng);
}

static B32
is_close(V2 lhs, V2 rhs) {
  return (is_close(lhs.x, rhs.x) &&
          is_close(lhs.y, rhs.y)); 
}

static V2 
midpoint(V2 lhs, V2 rhs) {
  return scale(add(lhs, rhs), 0.5f); 
  
}
static V2 
project(V2 v, V2 onto) {
  // (to . from)/LenSq(to) * to
  F32 onto_len_sq = length_sq(onto);
  assert(!is_close(onto_len_sq, 0.f));
  F32 v_dot_onto = dot(v, onto);
  F32 scalar = v_dot_onto / onto_len_sq;
  V2 ret = scale(onto, scalar);
  
  return ret;
}

static F32
angle_between(V2 lhs, V2 rhs) {
  F32 l_len = length(lhs);
  F32 r_len = length(rhs);
  F32 lr_dot = dot(lhs, rhs);
  F32 ret = acos(lr_dot/(l_len * r_len));
  return ret;
}

static V2 
rotate(V2 v, F32 rad) {
  // Technically, we can use matrices but
  // meh, it's easy to code this out without it.
  // Removes dependencies too
  F32 c = cos(rad);
  F32 s = sin(rad);
  
  V2 ret = {};
  ret.x = (c * v.x) - (s * v.y);
  ret.y = (s * v.x) + (c * v.y);
  return ret;
}


//~ NOTE(Momo): V3
static V3 
add(V3 lhs, V3 rhs) {
  lhs.x += rhs.x;
  lhs.y += rhs.y;
  lhs.z += rhs.z;
  return lhs;
}
static V3 
sub(V3 lhs, V3 rhs) {
  lhs.x -= rhs.x;
  lhs.y -= rhs.y;
  lhs.z -= rhs.z;
  return lhs;
}

static V3 
scale(V3 lhs, F32 rhs) {
  lhs.x *= rhs;
  lhs.y *= rhs;
  lhs.z *= rhs;
  return lhs;
}

static V3 
div(V3 lhs, F32 rhs) {
  assert(!is_close(rhs, 0.f));
  lhs.x /= rhs;
  lhs.y /= rhs;
  lhs.z /= rhs;
  return lhs;
}

static V3 
negate(V3 v) {
  v.x = -v.x;
  v.y = -v.y;
  v.z = -v.z;
  return v;
}

static F32 
dot(V3 lhs, V3 rhs) {
  return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
}

static F32  
distance_sq(V3 lhs, V3 rhs) {
  return length_sq(sub(lhs, rhs));
}

static F32  
distance(V3 lhs, V3 rhs) {
  return sqrt(distance_sq(lhs, rhs));
}

static F32  
length_sq(V3 v) {
  return dot(v, v);
}

static F32   
length(V3 v) {
  return sqrt(length_sq(v));
}

static V3 
normalize(V3 v) {
  F32 len = length(v);
  return div(v, len);
}

static B32
is_close(V3 lhs, V3 rhs) {
  return (is_close(lhs.x, rhs.x) &&
          is_close(lhs.y, rhs.y)); 
}

static V3 
midpoint(V3 lhs, V3 rhs) {
  return scale(add(lhs, rhs), 0.5f); 
  
}
static V3 
project(V3 v, V3 onto) {
  // (to . from)/LenSq(to) * to
  F32 onto_len_sq = length_sq(onto);
  assert(!is_close(onto_len_sq, 0.f));
  F32 v_dot_onto = dot(v, onto);
  F32 scalar = v_dot_onto / onto_len_sq;
  V3 ret = scale(onto, scalar);
  
  return ret;
}

static F32
angle_between(V3 lhs, V3 rhs) {
  F32 l_len = length(lhs);
  F32 r_len = length(rhs);
  F32 lr_dot = dot(lhs, rhs);
  F32 ret = acos(lr_dot/(l_len * r_len));
  return ret;
}

static V3
Cross(V3 lhs, V3 rhs) {
  V3 ret = {};
  ret.x = (lhs.y * rhs.z) - (lhs.z * rhs.y);
  ret.y = (lhs.z * rhs.x) - (lhs.x * rhs.z);
  ret.z = (lhs.x * rhs.y) - (lhs.y * rhs.x);
  
  return ret;
}


//~Operator Overloading
static V2 operator+(V2 lhs, V2 rhs) { return add(lhs, rhs); }
static V2 operator-(V2 lhs, V2 rhs) { return sub(lhs, rhs); }
static V2 operator*(V2 lhs, F32 rhs) { return scale(lhs, rhs); }
static V2 operator*(F32 lhs, V2 rhs) { return scale(rhs, lhs); }
static B32   operator==(V2 lhs, V2 rhs) { return is_close(lhs, rhs); }
static B32   operator!=(V2 lhs, V2 rhs) { return !is_close(lhs, rhs); }
static V2 operator-(V2 v) { return negate(v); }
static V2& operator+=(V2& lhs, V2 rhs) { return lhs = add(lhs, rhs); } 
static V2& operator-=(V2& lhs, V2 rhs) { return lhs = sub(lhs, rhs); } 
static V2& operator*=(V2& lhs, F32 rhs) { return lhs = scale(lhs, rhs); }

static V3 operator+(V3 lhs, V3 rhs) { return add(lhs, rhs); }
static V3 operator-(V3 lhs, V3 rhs) { return sub(lhs, rhs); }
static V3 operator*(V3 lhs, F32 rhs) { return scale(lhs, rhs); }
static V3 operator*(F32 lhs, V3 rhs) { return scale(rhs, lhs); }
static B32   operator==(V3 lhs, V3 rhs) { return is_close(lhs, rhs); }
static B32   operator!=(V3 lhs, V3 rhs) { return !is_close(lhs, rhs); }
static V3 operator-(V3 v) { return negate(v); }
static V3& operator+=(V3& lhs, V3 rhs) { return lhs = add(lhs, rhs); } 
static V3& operator-=(V3& lhs, V3 rhs) { return lhs = sub(lhs, rhs); } 
static V3& operator*=(V3& lhs, F32 rhs) { return lhs = scale(lhs, rhs); }

