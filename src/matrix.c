#include <math.h>
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
