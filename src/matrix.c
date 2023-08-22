#include "math.h"
#include "matrix.h"

mat4_t mat4_new_empty(void) {
	mat4_t result = {{{ 0 }}};
	mat4_identity(&result);
	return result;
}

void mat4_identity(mat4_t *out) {
	out->m[0][0] = 1;
	out->m[0][1] = 0;
	out->m[0][2] = 0;
	out->m[0][3] = 0;

	out->m[1][0] = 0;
	out->m[1][1] = 1;
	out->m[1][2] = 0;
	out->m[1][3] = 0;

	out->m[2][0] = 0;
	out->m[2][1] = 0;
	out->m[2][2] = 1;
	out->m[2][3] = 0;

	out->m[3][0] = 0;
	out->m[3][1] = 0;
	out->m[3][2] = 0;
	out->m[3][3] = 1;
}

void mat4_mul_vec4(vec4_t *out, mat4_t *m, vec4_t *v) {
	out->x = m->m[0][0] * v->x + m->m[0][1] * v->y + m->m[0][2] * v->z + m->m[0][3] * v->w;
	out->y = m->m[1][0] * v->x + m->m[1][1] * v->y + m->m[1][2] * v->z + m->m[1][3] * v->w;
	out->z = m->m[2][0] * v->x + m->m[2][1] * v->y + m->m[2][2] * v->z + m->m[2][3] * v->w;
	out->w = m->m[3][0] * v->x + m->m[3][1] * v->y + m->m[3][2] * v->z + m->m[3][3] * v->w;
}

void mat4_mul_vec4_project(vec4_t *out, mat4_t *mat_proj, vec4_t *v) {
	mat4_mul_vec4(out, mat_proj, v);
	if (out->w != 0.0) {
		out->x /= out->w;
		out->y /= out->w;
		out->z /= out->w;
	}
}

void mat4_mul_mat4(mat4_t *out, mat4_t *m0, mat4_t *m1) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			out->m[i][j] =
				m0->m[i][0] * m1->m[0][j] +
				m0->m[i][1] * m1->m[1][j] +
				m0->m[i][2] * m1->m[2][j] +
				m0->m[i][3] * m1->m[3][j];
		}
	}
}

void mat4_make_scale(mat4_t *out, float sx, float sy, float sz) {
	mat4_identity(out);
	out->m[0][0] = sx;
	out->m[1][1] = sy;
	out->m[2][2] = sz;
}

void mat4_make_translation(mat4_t *out, float tx, float ty, float tz) {
	mat4_identity(out);
	out->m[0][3] = tx;
	out->m[1][3] = ty;
	out->m[2][3] = tz;
}

void mat4_make_rotation_x(mat4_t *out, float angle) {
	float c = cos(angle);
	float s = sin(angle);
	mat4_identity(out);
	out->m[1][1] = c;
	out->m[1][2] = -s;
	out->m[2][1] = s;
	out->m[2][2] = c;
}

void mat4_make_rotation_y(mat4_t *out, float angle) {
	float c = cos(angle);
	float s = sin(angle);
	mat4_identity(out);
	out->m[0][0] = c;
	out->m[0][2] = s;
	out->m[2][0] = -s;
	out->m[2][2] = c;
}

void mat4_make_rotation_z(mat4_t *out, float angle) {
	float c = cos(angle);
	float s = sin(angle);
	mat4_identity(out);
	out->m[0][0] = c;
	out->m[0][1] = -s;
	out->m[1][0] = s;
	out->m[1][1] = c;
}

void mat4_make_perspective(mat4_t *out, float fov, float aspect, float near, float far) {
	// |  aspect * (1 / tan(fov / 2))	0									0										0  														|
	// |  0														1 / tan(fov / 2)	0										0  														|
	// |  0														0									far / (far - near)	(-far * near) / (far - near)  |
	// |  0														0									1										0  														|
	mat4_identity(out);
	out->m[0][0] = aspect * (1 / tan(fov / 2));
	out->m[1][1] = 1 / tan(fov / 2);
	out->m[2][2] = far / (far - near);
	out->m[2][3] = (-far * near) / (far - near);
	out->m[3][2] = 1.0;
}

void mat4_look_at(mat4_t *out, vec3_t *eye, vec3_t *target, vec3_t *up) {
	vec3_t x = { 0 };
	vec3_t y = { 0 };
	vec3_t z = { 0 };

	vec3_sub(&z, target, eye);
	vec3_normalize(&z);

	vec3_cross(&x, up, &z);
	vec3_normalize(&x);

	vec3_cross(&y, &z, &x);

	// | x.x  x.y  x.z  -dot(x, eye) |
	// | y.x  y.y  y.z  -dot(y, eye) |
	// | z.x  z.y  z.z  -dot(z, eye) |
	// | 0      0    0             1 |
	out->m[0][0] = x.x;
	out->m[0][1] = x.y;
	out->m[0][2] = x.z;
	out->m[0][3] = -vec3_dot(&x, eye);

	out->m[1][0] = y.x;
	out->m[1][1] = y.y;
	out->m[1][2] = y.z;
	out->m[1][3] = -vec3_dot(&y, eye);

	out->m[2][0] = z.x;
	out->m[2][1] = z.y;
	out->m[2][2] = z.z;
	out->m[2][3] = -vec3_dot(&z, eye);

	out->m[3][0] = 0;
	out->m[3][1] = 0;
	out->m[3][2] = 0;
	out->m[3][3] = 1;
}