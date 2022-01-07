static M44F32 Concat(M44F32 lhs, M44F32 rhs) {
	M44F32 ret = {0};
  for (U8 r = 0; r < 4; r++) { 
    for (U8 c = 0; c < 4; c++) { 
      for (U8 i = 0; i < 4; i++) {
				ret.e[r][c] += lhs.e[r][i] *  rhs.e[i][c]; 
			}
		} 
	} 
	return ret;
}

static M44F32 Transpose(M44F32 m) {
	M44F32 ret = {0};
	for (U32 i = 0; i < 4; ++i ) {
		for (U32 j = 0; j < 4; ++j) {
			ret.e[i][j] = m.e[j][i];
		}
	}
	return ret;
}
static M44F32 M44F32_Scale(F32 x, F32 y, F32 z) {
	M44F32 ret = {0};
	ret.e[0][0] = x;
	ret.e[1][1] = y;
	ret.e[2][2] = z;
	ret.e[3][3] = 1.f;
	
	return ret;
}
static M44F32 M44F32_Identity() {
	M44F32 ret = {0};
	ret.e[0][0] = 1.f;
	ret.e[1][1] = 1.f;
	ret.e[2][2] = 1.f;
	ret.e[3][3] = 1.f;
	
	return ret;
}
static M44F32 M44F32_Translation(F32 x, F32 y, F32 z) {
	M44F32 ret = M44F32_Identity();
	ret.e[0][3] = x;
	ret.e[1][3] = y;
	ret.e[2][3] = z;
	ret.e[3][3] = 1.f;
	
	return ret;
}
static M44F32 M44F32_RotationX(F32 rad) {
	
	// NOTE(Momo): 
	// 1  0  0  0
	// 0  c -s  0
	// 0  s  c  0
	// 0  0  0  1
	F32 c = Cos(rad);
	F32 s = Sin(rad);
	M44F32 ret = {};
	ret.e[0][0] = 1.f;
	ret.e[3][3] = 1.f;
	ret.e[1][1] = c;
	ret.e[1][2] = -s;
	ret.e[2][1] = s;
	ret.e[2][2] = c;
	
	return ret;
}
static M44F32 M44F32_RotationY(F32 rad) {
	
	// NOTE(Momo): 
	//  c  0  s  0
	//  0  1  0  0
	// -s  0  c  0
	//  0  0  0  1
	F32 c = Cos(rad);
	F32 s = Sin(rad);
	M44F32 ret = {};
	ret.e[0][0] = c;
	ret.e[0][2] = s;
	ret.e[1][1] = 1.f;
	ret.e[2][0] = -s;
	ret.e[2][2] = c;
	ret.e[3][3] = 1.f;
	
	return ret;
}
static M44F32 M44F32_RotationZ(F32 rad) {
	
	// NOTE(Momo): 
	//  c -s  0  0
	//  s  c  0  0
	//  0  0  1  0
	//  0  0  0  1
	
	F32 c = Cos(rad);
	F32 s = Sin(rad);
	M44F32 ret = {};
	ret.e[0][0] = c;
	ret.e[0][1] = -s;
	ret.e[1][0] = s;
	ret.e[1][1] = c;
	ret.e[2][2] = 1.f;
	ret.e[3][3] = 1.f;
	
	return ret;
}
static M44F32 M44F32_Orthographic(F32 left, F32 right, F32 bottom, F32 top, F32 near, F32 far) {
	
	M44F32 ret = {0};
	ret.e[0][0] = 2.f/(right-left);
	ret.e[1][1] = 2.f/(top-bottom);
	ret.e[2][2] = 2.f/(far-near);
	ret.e[3][3] = 1.f;
	ret.e[0][3] = -(right+left)/(right-left);
	ret.e[1][3] = -(top+bottom)/(top-bottom);
	ret.e[2][3] = -(far+near)/(far-near);
	
	return ret;
}
static M44F32 M44F32_Frustum(F32 left, F32 right, F32 bottom, F32 top, F32 near, F32 far) {
	M44F32 ret = {0};
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
static M44F32 M44F32_Perspective(F32 fov, F32 aspect, F32 near, F32 far){
	F32 top = near * Tan(fov*0.5f);
	F32 right = top * aspect;
	return M44F32_Frustum(-right, right,
                        -top, top,
                        near, far);
}

static M44F32 operator*(M44F32 lhs, M44F32 rhs) {
  return Concat(lhs, rhs);
}
