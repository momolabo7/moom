//~ NOTE(Momo): V2F32
static constexpr V2F32 
Add(V2F32 lhs, V2F32 rhs) {
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	return lhs;
}

static constexpr V2F32 
Sub(V2F32 lhs, V2F32 rhs) {
  lhs.x -= rhs.x;
  lhs.y -= rhs.y;
  return lhs;
}

static constexpr V2F32 
Scale(V2F32 lhs, F32 rhs) {
  lhs.x *= rhs;
  lhs.y *= rhs;
  return lhs;
}

static constexpr V2F32 
InvScale(V2F32 lhs, F32 rhs) {
  Assert(Match(rhs, 0.f));
  lhs.x /= rhs;
  lhs.y /= rhs;
  return lhs;
}

static constexpr V2F32 
Negate(V2F32 v) {
  v.x = -v.x;
  v.y = -v.y;
  return v;
}

static constexpr F32 
Dot(V2F32 lhs, V2F32 rhs) {
  return (lhs.x * rhs.x) + (lhs.y * rhs.y);
}

static constexpr F32  
DistanceSq(V2F32 lhs, V2F32 rhs) {
  return LengthSq(Sub(lhs, rhs));
}
static F32  
Distance(V2F32 lhs, V2F32 rhs) {
  return Sqrt(DistanceSq(lhs, rhs));
}

static constexpr F32  
LengthSq(V2F32 v) {
  return Dot(v, v);
}

static F32   
Length(V2F32 v) {
  return Sqrt(LengthSq(v));
}

static V2F32 
Normalize(V2F32 v) {
  F32 len = Length(v);
  return InvScale(v, len);
}

static constexpr B32
Match(V2F32 lhs, V2F32 rhs) {
  return (Match(lhs.x, rhs.x) &&
          Match(lhs.y, rhs.y)); 
}

static constexpr V2F32 
Midpoint(V2F32 lhs, V2F32 rhs) {
  return Scale(Add(lhs, rhs), 0.5f); 
  
}
static V2F32 
Project(V2F32 v, V2F32 onto) {
  // (to . from)/LenSq(to) * to
  F32 onto_len_sq = LengthSq(onto);
  Assert(!Match(onto_len_sq, 0.f));
  F32 dot = Dot(v, onto);
  F32 scalar = dot / onto_len_sq;
  V2F32 ret = Scale(onto, scalar);
  
  return ret;
}

static F32
AngleBetween(V2F32 lhs, V2F32 rhs) {
  F32 l_len = Length(lhs);
  F32 r_len = Length(rhs);
  F32 lr_dot = Dot(lhs, rhs);
  F32 ret = Acos(lr_dot/(l_len * r_len));
  return ret;
}

static V2F32 
Rotate(V2F32 v, F32 rad) {
  // Technically, we can use matrices but
  // meh, it's easy to code this out without it.
  // Removes dependencies too
  F32 c = Cos(rad);
  F32 s = Sin(rad);
  
  V2F32 ret = {};
  ret.x = (c * v.x) - (s * v.y);
  ret.y = (s * v.x) + (c * v.y);
  return ret;
}


//~ NOTE(Momo): V3F32
static constexpr V3F32 
Add(V3F32 lhs, V3F32 rhs) {
  lhs.x += rhs.x;
  lhs.y += rhs.y;
  lhs.z += rhs.z;
  return lhs;
}
static constexpr V3F32 
Sub(V3F32 lhs, V3F32 rhs) {
  lhs.x -= rhs.x;
  lhs.y -= rhs.y;
  lhs.z -= rhs.z;
  return lhs;
}

static constexpr V3F32 
Scale(V3F32 lhs, F32 rhs) {
  lhs.x *= rhs;
  lhs.y *= rhs;
  lhs.z *= rhs;
  return lhs;
}

static constexpr V3F32 
InvScale(V3F32 lhs, F32 rhs) {
  Assert(Match(rhs, 0.f));
  lhs.x /= rhs;
  lhs.y /= rhs;
  lhs.z /= rhs;
  return lhs;
}

static constexpr V3F32 
Negate(V3F32 v) {
  v.x = -v.x;
  v.y = -v.y;
  v.z = -v.z;
  return v;
}

static constexpr F32 
Dot(V3F32 lhs, V3F32 rhs) {
  return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
}

static constexpr F32  
DistanceSq(V3F32 lhs, V3F32 rhs) {
  return LengthSq(Sub(lhs, rhs));
}

static F32  
Distance(V3F32 lhs, V3F32 rhs) {
  return Sqrt(DistanceSq(lhs, rhs));
}

static constexpr F32  
LengthSq(V3F32 v) {
  return Dot(v, v);
}

static F32   
Length(V3F32 v) {
  return Sqrt(LengthSq(v));
}

static V3F32 
Normalize(V3F32 v) {
  F32 len = Length(v);
  return InvScale(v, len);
}

static constexpr B32
Match(V3F32 lhs, V3F32 rhs) {
  return (Match(lhs.x, rhs.x) &&
          Match(lhs.y, rhs.y)); 
}

static constexpr V3F32 
Midpoint(V3F32 lhs, V3F32 rhs) {
  return Scale(Add(lhs, rhs), 0.5f); 
  
}
static V3F32 
Project(V3F32 v, V3F32 onto) {
  // (to . from)/LenSq(to) * to
  F32 onto_len_sq = LengthSq(onto);
  Assert(!Match(onto_len_sq, 0.f));
  F32 dot = Dot(v, onto);
  F32 scalar = dot / onto_len_sq;
  V3F32 ret = Scale(onto, scalar);
  
  return ret;
}

static F32
AngleBetween(V3F32 lhs, V3F32 rhs) {
  F32 l_len = Length(lhs);
  F32 r_len = Length(rhs);
  F32 lr_dot = Dot(lhs, rhs);
  F32 ret = Acos(lr_dot/(l_len * r_len));
  return ret;
}

static constexpr V3F32
Cross(V3F32 lhs, V3F32 rhs) {
  V3F32 ret = {0};
  ret.x = (lhs.y * rhs.z) - (lhs.z * rhs.y);
  ret.y = (lhs.z * rhs.x) - (lhs.x * rhs.z);
  ret.z = (lhs.x * rhs.y) - (lhs.y * rhs.x);
  
  return ret;
}


//~Operator Overloading
static constexpr V2F32 operator+(V2F32 lhs, V2F32 rhs) { return Add(lhs, rhs); }
static constexpr V2F32 operator-(V2F32 lhs, V2F32 rhs) { return Sub(lhs, rhs); }
static constexpr V2F32 operator*(V2F32 lhs, F32 rhs) { return Scale(lhs, rhs); }
static constexpr V2F32 operator*(F32 lhs, V2F32 rhs) { return Scale(rhs, lhs); }
static constexpr B32   operator==(V2F32 lhs, V2F32 rhs) { return Match(lhs, rhs); }
static constexpr B32   operator!=(V2F32 lhs, V2F32 rhs) { return !Match(lhs, rhs); }
static constexpr V2F32 operator-(V2F32 v) { return Negate(v); }
static constexpr V2F32& operator+=(V2F32& lhs, V2F32 rhs) { return lhs = Add(lhs, rhs); } 
static constexpr V2F32& operator-=(V2F32& lhs, V2F32 rhs) { return lhs = Sub(lhs, rhs); } 
static constexpr V2F32& operator*=(V2F32& lhs, F32 rhs) { return lhs = Scale(lhs, rhs); }

static constexpr V3F32 operator+(V3F32 lhs, V3F32 rhs) { return Add(lhs, rhs); }
static constexpr V3F32 operator-(V3F32 lhs, V3F32 rhs) { return Sub(lhs, rhs); }
static constexpr V3F32 operator*(V3F32 lhs, F32 rhs) { return Scale(lhs, rhs); }
static constexpr V3F32 operator*(F32 lhs, V3F32 rhs) { return Scale(rhs, lhs); }
static constexpr B32   operator==(V3F32 lhs, V3F32 rhs) { return Match(lhs, rhs); }
static constexpr B32   operator!=(V3F32 lhs, V3F32 rhs) { return !Match(lhs, rhs); }
static constexpr V3F32 operator-(V3F32 v) { return Negate(v); }
static constexpr V3F32& operator+=(V3F32& lhs, V3F32 rhs) { return lhs = Add(lhs, rhs); } 
static constexpr V3F32& operator-=(V3F32& lhs, V3F32 rhs) { return lhs = Sub(lhs, rhs); } 
static constexpr V3F32& operator*=(V3F32& lhs, F32 rhs) { return lhs = Scale(lhs, rhs); }

