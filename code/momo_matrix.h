#ifndef MOMO_MATRIX_H
#define MOMO_MATRIX_H

struct m44f_t {
	f32_t e[4][4];
};

static m44f_t m44f_concat(m44f_t lhs, m44f_t rhs);
static m44f_t m44f_transpose(m44f_t m);
static m44f_t m44f_scale(f32_t x, f32_t y, f32_t z);
static m44f_t m44f_identity();
static m44f_t m44f_translation(f32_t x, f32_t y, f32_t z);
static m44f_t m44f_rotation_x(f32_t radians);
static m44f_t m44f_rotation_y(f32_t radians);
static m44f_t m44f_rotation_z(f32_t radians);
static m44f_t m44f_orthographic(f32_t left, f32_t right, f32_t bottom, f32_t top, f32_t near, f32_t far);
static m44f_t m44f_frustum(f32_t left, f32_t right, f32_t bottom, f32_t top, f32_t near, f32_t far);
static m44f_t m44f_perspective(f32_t fov, f32_t aspect, f32_t near, f32_t far);

static m44f_t operator*(m44f_t lhs, m44f_t rhs);

//////////////////////////////////////////////////////////////////
// IMPLEMENTATION

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

static m44f_t operator*(m44f_t lhs, m44f_t rhs) {
  return m44f_concat(lhs, rhs);
}


#endif //MOMO_MATRIX_H
