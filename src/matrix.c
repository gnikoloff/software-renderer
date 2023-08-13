#include "math.h"
#include "matrix.h"

mat4_t mat4_identity(void) {
	//  |  1  0  0  0  |
	//  |  0  1  0  0  |
	//  |  0  0  1  0  |
	//  |  0  0  0  1  |
	mat4_t m = {{
		{ 1, 0, 0, 0 },
		{ 0, 1, 0, 0 },
		{ 0, 0, 1, 0 },
		{ 0, 0, 0, 1 }
	}};
	return m;
}

mat4_t mat4_make_scale(float sx, float sy, float sz) {
	//  |  sx 0  0  0  |
	//  |  0  sy 0  0  |
	//  |  0  0  sz 0  |
	//  |  0  0  0  1  |
	mat4_t m = mat4_identity();
	m.m[0][0] = sx;
	m.m[1][1] = sy;
	m.m[2][2] = sz;
	return m;
}

mat4_t mat4_make_translation(float tx, float ty, float tz) {
	//  |  1  0  0  tx |
	//  |  0  1  0  ty |
	//  |  0  0  1  tz |
	//  |  0  0  0  1  |
	mat4_t m = mat4_identity();
	m.m[0][3] = tx;
	m.m[1][3] = ty;
	m.m[2][3] = tz;
	return m;
}

mat4_t mat4_make_rotation_x(float angle) {
	//  |  1  0  0  0  |
	//  |  0  c -s  0  |
	//  |  0  s  c  0  |
	//  |  0  0  0  1  |
	float c = cos(angle);
	float s = sin(angle);
	mat4_t m = mat4_identity();
	m.m[1][1] = c;
	m.m[1][2] = -s;
	m.m[2][1] = s;
	m.m[2][2] = c;
	return m;
}

mat4_t mat4_make_rotation_y(float angle) {
	//  |  c  0  s  0  |
	//  |  0  1  0  0  |
	//  | -s  0  c  0  |
	//  |  0  0  0  1  |
	float c = cos(angle);
	float s = sin(angle);
	mat4_t m = mat4_identity();
	m.m[0][0] = c;
	m.m[0][2] = s;
	m.m[2][0] = -s;
	m.m[2][2] = c;
	return m;
}

mat4_t mat4_make_rotation_z(float angle) {
	//  |  c -s  s  0  |
	//  |  s  c  0  0  |
	//  |  0  0  1  0  |
	//  |  0  0  0  1  |
	float c = cos(angle);
	float s = sin(angle);
	mat4_t m = mat4_identity();
	m.m[0][0] = c;
	m.m[0][1] = -s;
	m.m[1][0] = s;
	m.m[1][1] = c;
	return m;
}

vec4_t mat4_mul_vec4(mat4_t m, vec4_t v) {
	vec4_t result;
	result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
	result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
	result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
	result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;
	return result;
}

mat4_t mat4_mul_mat4(mat4_t m0, mat4_t m1) {
	mat4_t m;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m.m[i][j] = m0.m[i][0] * m1.m[0][j] +
									m0.m[i][1] * m1.m[1][j] +
									m0.m[i][2] * m1.m[2][j] +
									m0.m[i][3] * m1.m[3][j];
		}
	}
	return m;
}

mat4_t mat4_make_perspective(float fov, float aspect, float near, float far) {
	// |  aspect * (1 / tan(fov / 2))	0									0										0  														|
	// |  0														1 / tan(fov / 2)	0										0  														|
	// |  0														0									far / (far - near)	(-far * near) / (far - near)  |
	// |  0														0									1										0  														|
	mat4_t m = {{{ 0 }}};
	m.m[0][0] = aspect * (1 / tan(fov / 2));
	m.m[1][1] = 1 / tan(fov / 2);
	m.m[2][2] = far / (far - near);
	m.m[2][3] = (-far * near) / (far - near);
	m.m[3][2] = 1.0;
	return m;
}

vec4_t mat4_mul_vec4_project(mat4_t mat_proj, vec4_t v) {
	vec4_t result = mat4_mul_vec4(mat_proj, v);
	if (result.w != 0.0) {
		result.x /= result.w;
		result.y /= result.w;
		result.z /= result.w;
	}
	return result;
}
