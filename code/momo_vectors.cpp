//~ NOTE(Momo): V2F32
static constexpr V2F32 
add(V2F32 lhs, V2F32 rhs) {
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	return lhs;
}

static constexpr V2F32 
sub(V2F32 lhs, V2F32 rhs) {
  lhs.x -= rhs.x;
  lhs.y -= rhs.y;
  return lhs;
}

static constexpr V2F32 
scale(V2F32 lhs, F32 rhs) {
  lhs.x *= rhs;
  lhs.y *= rhs;
  return lhs;
}

static constexpr V2F32 
div(V2F32 lhs, F32 rhs) {
  assert(match(rhs, 0.f));
  lhs.x /= rhs;
  lhs.y /= rhs;
  return lhs;
}

static constexpr V2F32 
negate(V2F32 v) {
  v.x = -v.x;
  v.y = -v.y;
  return v;
}

static constexpr F32 
dot(V2F32 lhs, V2F32 rhs) {
  return (lhs.x * rhs.x) + (lhs.y * rhs.y);
}

static constexpr F32  
distance_sq(V2F32 lhs, V2F32 rhs) {
  return length_sq(sub(lhs, rhs));
}
static F32  
distance(V2F32 lhs, V2F32 rhs) {
  return sqrt(distance_sq(lhs, rhs));
}

static constexpr F32  
length_sq(V2F32 v) {
  return dot(v, v);
}

static F32   
length(V2F32 v) {
  return sqrt(length_sq(v));
}

static V2F32 
normalize(V2F32 v) {
  F32 leng = length(v);
  return div(v, leng);
}

static constexpr B32
match(V2F32 lhs, V2F32 rhs) {
  return (match(lhs.x, rhs.x) &&
          match(lhs.y, rhs.y)); 
}

static constexpr V2F32 
midpoint(V2F32 lhs, V2F32 rhs) {
  return scale(add(lhs, rhs), 0.5f); 
  
}
static V2F32 
project(V2F32 v, V2F32 onto) {
  // (to . from)/LenSq(to) * to
  F32 onto_len_sq = length_sq(onto);
  assert(!match(onto_len_sq, 0.f));
  F32 v_dot_onto = dot(v, onto);
  F32 scalar = v_dot_onto / onto_len_sq;
  V2F32 ret = scale(onto, scalar);
  
  return ret;
}

static F32
angle_between(V2F32 lhs, V2F32 rhs) {
  F32 l_len = length(lhs);
  F32 r_len = length(rhs);
  F32 lr_dot = dot(lhs, rhs);
  F32 ret = acos(lr_dot/(l_len * r_len));
  return ret;
}

static V2F32 
rotate(V2F32 v, F32 rad) {
  // Technically, we can use matrices but
  // meh, it's easy to code this out without it.
  // Removes dependencies too
  F32 c = cos(rad);
  F32 s = sin(rad);
  
  V2F32 ret = {};
  ret.x = (c * v.x) - (s * v.y);
  ret.y = (s * v.x) + (c * v.y);
  return ret;
}


//~ NOTE(Momo): V3F32
static constexpr V3F32 
add(V3F32 lhs, V3F32 rhs) {
  lhs.x += rhs.x;
  lhs.y += rhs.y;
  lhs.z += rhs.z;
  return lhs;
}
static constexpr V3F32 
sub(V3F32 lhs, V3F32 rhs) {
  lhs.x -= rhs.x;
  lhs.y -= rhs.y;
  lhs.z -= rhs.z;
  return lhs;
}

static constexpr V3F32 
scale(V3F32 lhs, F32 rhs) {
  lhs.x *= rhs;
  lhs.y *= rhs;
  lhs.z *= rhs;
  return lhs;
}

static constexpr V3F32 
div(V3F32 lhs, F32 rhs) {
  assert(match(rhs, 0.f));
  lhs.x /= rhs;
  lhs.y /= rhs;
  lhs.z /= rhs;
  return lhs;
}

static constexpr V3F32 
negate(V3F32 v) {
  v.x = -v.x;
  v.y = -v.y;
  v.z = -v.z;
  return v;
}

static constexpr F32 
dot(V3F32 lhs, V3F32 rhs) {
  return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
}

static constexpr F32  
distance_sq(V3F32 lhs, V3F32 rhs) {
  return length_sq(sub(lhs, rhs));
}

static F32  
distance(V3F32 lhs, V3F32 rhs) {
  return sqrt(distance_sq(lhs, rhs));
}

static constexpr F32  
length_sq(V3F32 v) {
  return dot(v, v);
}

static F32   
length(V3F32 v) {
  return sqrt(length_sq(v));
}

static V3F32 
normalize(V3F32 v) {
  F32 len = length(v);
  return div(v, len);
}

static constexpr B32
match(V3F32 lhs, V3F32 rhs) {
  return (match(lhs.x, rhs.x) &&
          match(lhs.y, rhs.y)); 
}

static constexpr V3F32 
midpoint(V3F32 lhs, V3F32 rhs) {
  return scale(add(lhs, rhs), 0.5f); 
  
}
static V3F32 
project(V3F32 v, V3F32 onto) {
  // (to . from)/LenSq(to) * to
  F32 onto_len_sq = length_sq(onto);
  assert(!match(onto_len_sq, 0.f));
  F32 v_dot_onto = dot(v, onto);
  F32 scalar = v_dot_onto / onto_len_sq;
  V3F32 ret = scale(onto, scalar);
  
  return ret;
}

static F32
angle_between(V3F32 lhs, V3F32 rhs) {
  F32 l_len = length(lhs);
  F32 r_len = length(rhs);
  F32 lr_dot = dot(lhs, rhs);
  F32 ret = acos(lr_dot/(l_len * r_len));
  return ret;
}

static constexpr V3F32
Cross(V3F32 lhs, V3F32 rhs) {
  V3F32 ret = {};
  ret.x = (lhs.y * rhs.z) - (lhs.z * rhs.y);
  ret.y = (lhs.z * rhs.x) - (lhs.x * rhs.z);
  ret.z = (lhs.x * rhs.y) - (lhs.y * rhs.x);
  
  return ret;
}


//~Operator Overloading
static constexpr V2F32 operator+(V2F32 lhs, V2F32 rhs) { return add(lhs, rhs); }
static constexpr V2F32 operator-(V2F32 lhs, V2F32 rhs) { return sub(lhs, rhs); }
static constexpr V2F32 operator*(V2F32 lhs, F32 rhs) { return scale(lhs, rhs); }
static constexpr V2F32 operator*(F32 lhs, V2F32 rhs) { return scale(rhs, lhs); }
static constexpr B32   operator==(V2F32 lhs, V2F32 rhs) { return match(lhs, rhs); }
static constexpr B32   operator!=(V2F32 lhs, V2F32 rhs) { return !match(lhs, rhs); }
static constexpr V2F32 operator-(V2F32 v) { return negate(v); }
static constexpr V2F32& operator+=(V2F32& lhs, V2F32 rhs) { return lhs = add(lhs, rhs); } 
static constexpr V2F32& operator-=(V2F32& lhs, V2F32 rhs) { return lhs = sub(lhs, rhs); } 
static constexpr V2F32& operator*=(V2F32& lhs, F32 rhs) { return lhs = scale(lhs, rhs); }

static constexpr V3F32 operator+(V3F32 lhs, V3F32 rhs) { return add(lhs, rhs); }
static constexpr V3F32 operator-(V3F32 lhs, V3F32 rhs) { return sub(lhs, rhs); }
static constexpr V3F32 operator*(V3F32 lhs, F32 rhs) { return scale(lhs, rhs); }
static constexpr V3F32 operator*(F32 lhs, V3F32 rhs) { return scale(rhs, lhs); }
static constexpr B32   operator==(V3F32 lhs, V3F32 rhs) { return match(lhs, rhs); }
static constexpr B32   operator!=(V3F32 lhs, V3F32 rhs) { return !match(lhs, rhs); }
static constexpr V3F32 operator-(V3F32 v) { return negate(v); }
static constexpr V3F32& operator+=(V3F32& lhs, V3F32 rhs) { return lhs = add(lhs, rhs); } 
static constexpr V3F32& operator-=(V3F32& lhs, V3F32 rhs) { return lhs = sub(lhs, rhs); } 
static constexpr V3F32& operator*=(V3F32& lhs, F32 rhs) { return lhs = scale(lhs, rhs); }

