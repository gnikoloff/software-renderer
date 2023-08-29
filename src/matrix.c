#include <math.h>
#include <assert.h>
#include "matrix.h"

inline mat4_t mat4_identity(void) {
	mat4_t m = {{
		{ 1, 0, 0, 0 },
		{ 0, 1, 0, 0 },
		{ 0, 0, 1, 0 },
		{ 0, 0, 0, 1 }
	}};
	return m;
}

inline mat4_t mat4_make_scale(float sx, float sy, float sz) {
	mat4_t m = mat4_identity();
	m.m[0][0] = sx;
	m.m[1][1] = sy;
	m.m[2][2] = sz;
	return m;
}

inline mat4_t mat4_make_translation(float tx, float ty, float tz) {
	mat4_t m = mat4_identity();
	m.m[0][3] = tx;
	m.m[1][3] = ty;
	m.m[2][3] = tz;
	return m;
}

inline mat4_t mat4_make_rotation_x(float angle) {
	float c = cos(angle);
	float s = sin(angle);
	mat4_t m = mat4_identity();
	m.m[1][1] = c;
	m.m[1][2] = s;
	m.m[2][1] = -s;
	m.m[2][2] = c;
	return m;
}

inline mat4_t mat4_make_rotation_y(float angle) {
	float c = cos(angle);
	float s = sin(angle);
	mat4_t m = mat4_identity();
	m.m[0][0] = c;
	m.m[0][2] = -s;
	m.m[2][0] = s;
	m.m[2][2] = c;
	return m;
}

inline mat4_t mat4_make_rotation_z(float angle) {
	float c = cos(angle);
	float s = sin(angle);
	mat4_t m = mat4_identity();
	m.m[0][0] = c;
	m.m[0][1] = s;
	m.m[1][0] = -s;
	m.m[1][1] = c;
	return m;
}

inline vec4_t mat4_mul_vec4(mat4_t m, vec4_t v) {
	vec4_t result;
	result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
	result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
	result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
	result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;
	return result;
}

inline mat4_t mat4_mul_mat4(mat4_t a, mat4_t b) {
	mat4_t m;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m.m[i][j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] + a.m[i][2] * b.m[2][j] + a.m[i][3] * b.m[3][j];
		}
	}
	return m;
}

inline mat4_t mat4_make_perspective(float fov, float aspect, float znear, float zfar) {
	mat4_t m = {{{ 0 }}};
	m.m[0][0] = aspect * (1 / tan(fov / 2));
	m.m[1][1] = 1 / tan(fov / 2);
	m.m[2][2] = zfar / (zfar - znear);
	m.m[2][3] = (-zfar * znear) / (zfar - znear);
	m.m[3][2] = 1.0;
	return m;
}

inline mat4_t mat4_make_orthographic(float left, float right, float top, float bottom, float znear, float zfar) {
	mat4_t m = {{{ 0 }}};
	float lr = 1 / (left - right);
	float bt = 1 / (bottom - top);
	float nf = 1 / (zfar - znear);
	m.m[0][0] = -2 * lr;
	m.m[1][1] = -2 * bt;
	m.m[2][2] = 2 * nf;
	m.m[3][0] = (left + right) * lr;
	m.m[3][1] = (top + bottom) * bt;
	m.m[3][2] = (zfar - znear) * nf;
	m.m[3][3] = 1;
	return m;
}

inline mat4_t mat4_look_at(vec3_t eye, vec3_t target, vec3_t up) {
	// Compute the forward (z), right (x), and up (y) vectors
	vec3_t z = vec3_sub(target, eye);
	vec3_normalize(&z);
	vec3_t x = vec3_cross(up, z);
	vec3_normalize(&x);
	vec3_t y = vec3_cross(z, x);

	mat4_t view_matrix = {{
		{ x.x, x.y, x.z, -vec3_dot(x, eye) },
		{ y.x, y.y, y.z, -vec3_dot(y, eye) },
		{ z.x, z.y, z.z, -vec3_dot(z, eye) },
		{   0,   0,   0,                 1 }
	}};
	return view_matrix;
}

inline mat4_t mat4_make_rotation_yxz(float y, float x, float z) {
	mat4_t out = mat4_identity();
	mat4_t rotation_matrix_y = mat4_make_rotation_y(y);
	mat4_t rotation_matrix_x = mat4_make_rotation_x(x);
	mat4_t rotation_matrix_z = mat4_make_rotation_z(z);
	out = mat4_mul_mat4(out, rotation_matrix_y);
	out = mat4_mul_mat4(out, rotation_matrix_x);
	out = mat4_mul_mat4(out, rotation_matrix_z);
	return out;
}

inline vec4_t mat4_mul_vec4_project(mat4_t mat_proj, vec4_t v) {
	vec4_t out = mat4_mul_vec4(mat_proj, v);
	if (out.w != 0.0) {
		out.x /= out.w;
		out.y /= out.w;
		out.z /= out.w;
	}
	return out;
}

mat4_t mat4_inverse(mat4_t m) {
	mat4_t out = {{{ 0 }}};
	float a00 = m.m[0][0];
	float a01 = m.m[0][1];
	float a02 = m.m[0][2];
	float a03 = m.m[0][3];

  float a10 = m.m[1][0];
	float a11 = m.m[1][1];
	float a12 = m.m[1][2];
	float a13 = m.m[1][3];

  float a20 = m.m[2][0];
	float a21 = m.m[2][1];
	float a22 = m.m[2][2];
	float a23 = m.m[2][3];

  float a30 = m.m[3][0];
	float a31 = m.m[3][1];
	float a32 = m.m[3][2];
	float a33 = m.m[3][3];

	float b00 = a00 * a11 - a01 * a10;
  float b01 = a00 * a12 - a02 * a10;
  float b02 = a00 * a13 - a03 * a10;
  float b03 = a01 * a12 - a02 * a11;
  float b04 = a01 * a13 - a03 * a11;
  float b05 = a02 * a13 - a03 * a12;
  float b06 = a20 * a31 - a21 * a30;
  float b07 = a20 * a32 - a22 * a30;
  float b08 = a20 * a33 - a23 * a30;
  float b09 = a21 * a32 - a22 * a31;
  float b10 = a21 * a33 - a23 * a31;
  float b11 = a22 * a33 - a23 * a32;

	float det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;

  if (det == 0) {
		return mat4_identity();
	}

  det = 1.0 / det;
  
	out.m[0][0] = (a11 * b11 - a12 * b10 + a13 * b09) * det;
  out.m[0][1] = (a02 * b10 - a01 * b11 - a03 * b09) * det;
  out.m[0][2] = (a31 * b05 - a32 * b04 + a33 * b03) * det;
  out.m[0][3] = (a22 * b04 - a21 * b05 - a23 * b03) * det;

  out.m[1][0] = (a12 * b08 - a10 * b11 - a13 * b07) * det;
  out.m[1][1] = (a00 * b11 - a02 * b08 + a03 * b07) * det;
  out.m[1][2] = (a32 * b02 - a30 * b05 - a33 * b01) * det;
  out.m[1][3] = (a20 * b05 - a22 * b02 + a23 * b01) * det;

  out.m[2][0] = (a10 * b10 - a11 * b08 + a13 * b06) * det;
  out.m[2][1] = (a01 * b08 - a00 * b10 - a03 * b06) * det;
  out.m[2][2] = (a30 * b04 - a31 * b02 + a33 * b00) * det;
  out.m[2][3] = (a21 * b02 - a20 * b04 - a23 * b00) * det;

  out.m[3][0] = (a11 * b07 - a10 * b09 - a12 * b06) * det;
  out.m[3][1] = (a00 * b09 - a01 * b07 + a02 * b06) * det;
  out.m[3][2] = (a31 * b01 - a30 * b03 - a32 * b00) * det;
  out.m[3][3] = (a20 * b03 - a21 * b01 + a22 * b00) * det;

	return out;
}
