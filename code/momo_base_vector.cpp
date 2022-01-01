//~ NOTE(Momo): V2F32
static V2F32 
V2F32_Add(V2F32 lhs, V2F32 rhs) {
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	return lhs;
}
static V2F32 
V2F32_Sub(V2F32 lhs, V2F32 rhs) {
	lhs.x -= rhs.x;
	lhs.y -= rhs.y;
	return lhs;
}

static V2F32 
V2F32_Scale(V2F32 lhs, F32 rhs) {
	lhs.x *= rhs;
	lhs.y *= rhs;
	return lhs;
}

static V2F32 
V2F32_InvScale(V2F32 lhs, F32 rhs) {
	Assert(F32_Match(rhs, 0.f));
	lhs.x /= rhs;
	lhs.y /= rhs;
	return lhs;
}

static V2F32 
V2F32_Negate(V2F32 v) {
	v.x = -v.x;
	v.y = -v.y;
	return v;
}

static F32 
V2F32_Dot(V2F32 lhs, V2F32 rhs) {
	return (lhs.x * rhs.x) + (lhs.y * rhs.y);
}

static F32  
V2F32_DistanceSq(V2F32 lhs, V2F32 rhs) {
	return V2F32_LengthSq(V2F32_Sub(lhs, rhs));
}
static F32  
V2F32_Distance(V2F32 lhs, V2F32 rhs) {
	return F32_Sqrt(V2F32_DistanceSq(lhs, rhs));
}

static F32  
V2F32_LengthSq(V2F32 v) {
	return V2F32_Dot(v, v);
}

static F32   
V2F32_Length(V2F32 v) {
	return F32_Sqrt(V2F32_LengthSq(v));
}

static V2F32 
V2F32_Normalize(V2F32 v) {
	F32 len = V2F32_Length(v);
  return V2F32_InvScale(v, len);
}

static B8
V2F32_Match(V2F32 lhs, V2F32 rhs) {
	return (F32_Match(lhs.x, rhs.x) &&
          F32_Match(lhs.y, rhs.y)); 
}

static V2F32 
V2F32_Midpoint(V2F32 lhs, V2F32 rhs) {
	return V2F32_Scale(V2F32_Add(lhs, rhs), 0.5f); 
	
}
static V2F32 
V2F32_Project(V2F32 v, V2F32 onto) {
	// (to . from)/LenSq(to) * to
  F32 onto_len_sq = V2F32_LengthSq(onto);
	Assert(!F32_Match(onto_len_sq, 0.f));
	F32 dot = V2F32_Dot(v, onto);
	F32 scalar = dot / onto_len_sq;
	V2F32 ret = V2F32_Scale(onto, scalar);
	
	return ret;
}

static inline F32
V2F32_AngleBetween(V2F32 lhs, V2F32 rhs) {
  F32 l_len = V2F32_Length(lhs);
  F32 r_len = V2F32_Length(rhs);
  F32 lr_dot = V2F32_Dot(lhs, rhs);
  F32 ret = F32_Acos(lr_dot/(l_len * r_len));
  return ret;
}

static V2F32 
V2F32_Rotate(V2F32 v, F32 rad) {
  // Technically, we can use matrices but
  // meh, it's easy to code this out without it.
  // Removes dependencies too
  F32 c = F32_Cos(rad);
  F32 s = F32_Sin(rad);
  
  V2F32 ret = {};
  ret.x = (c * v.x) - (s * v.y);
  ret.y = (s * v.x) + (c * v.y);
  return ret;
}


//~ NOTE(Momo): V3F32
static V3F32 
V3F32_Add(V3F32 lhs, V3F32 rhs) {
	lhs.x += rhs.x;
	lhs.y += rhs.y;
	lhs.z += rhs.z;
	return lhs;
}
static V3F32 
V3F32_Sub(V3F32 lhs, V3F32 rhs) {
	lhs.x -= rhs.x;
	lhs.y -= rhs.y;
	lhs.z -= rhs.z;
	return lhs;
}

static V3F32 
V3F32_Scale(V3F32 lhs, F32 rhs) {
	lhs.x *= rhs;
	lhs.y *= rhs;
	lhs.z *= rhs;
	return lhs;
}

static V3F32 
V3F32_InvScale(V3F32 lhs, F32 rhs) {
	Assert(F32_Match(rhs, 0.f));
	lhs.x /= rhs;
	lhs.y /= rhs;
	lhs.z /= rhs;
	return lhs;
}

static V3F32 
V3F32_Negate(V3F32 v) {
	v.x = -v.x;
	v.y = -v.y;
	v.z = -v.z;
	return v;
}

static F32 
V3F32_Dot(V3F32 lhs, V3F32 rhs) {
	return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
}

static F32  
V3F32_DistanceSq(V3F32 lhs, V3F32 rhs) {
	return V3F32_LengthSq(V3F32_Sub(lhs, rhs));
}
static F32  
V3F32_Distance(V3F32 lhs, V3F32 rhs) {
	return F32_Sqrt(V3F32_DistanceSq(lhs, rhs));
}

static F32  
V3F32_LengthSq(V3F32 v) {
	return V3F32_Dot(v, v);
}

static F32   
V3F32_Length(V3F32 v) {
	return F32_Sqrt(V3F32_LengthSq(v));
}

static V3F32 
V3F32_Normalize(V3F32 v) {
	F32 len = V3F32_Length(v);
  return V3F32_InvScale(v, len);
}

static B8
V3F32_Match(V3F32 lhs, V3F32 rhs) {
	return (F32_Match(lhs.x, rhs.x) &&
          F32_Match(lhs.y, rhs.y)); 
}

static V3F32 
V3F32_Midpoint(V3F32 lhs, V3F32 rhs) {
	return V3F32_Scale(V3F32_Add(lhs, rhs), 0.5f); 
	
}
static V3F32 
V3F32_Project(V3F32 v, V3F32 onto) {
	// (to . from)/LenSq(to) * to
  F32 onto_len_sq = V3F32_LengthSq(onto);
	Assert(!F32_Match(onto_len_sq, 0.f));
	F32 dot = V3F32_Dot(v, onto);
	F32 scalar = dot / onto_len_sq;
	V3F32 ret = V3F32_Scale(onto, scalar);
	
	return ret;
}

static F32
V3F32_AngleBetween(V3F32 lhs, V3F32 rhs) {
  F32 l_len = V3F32_Length(lhs);
  F32 r_len = V3F32_Length(rhs);
  F32 lr_dot = V3F32_Dot(lhs, rhs);
  F32 ret = F32_Acos(lr_dot/(l_len * r_len));
  return ret;
}

static V3F32
V3F32_Cross(V3F32 lhs, V3F32 rhs) {
  V3F32 ret;
  ret.x = (lhs.y * rhs.z) - (lhs.z * rhs.y);
  ret.y = (lhs.z * rhs.x) - (lhs.x * rhs.z);
  ret.z = (lhs.x * rhs.y) - (lhs.y * rhs.x);
  
  return ret;
}

