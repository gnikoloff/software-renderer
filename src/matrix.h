#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"

typedef struct {
	float m[4][4];
} mat4_t;

mat4_t mat4_new_empty(void);
void mat4_identity(mat4_t *out);
void mat4_mul_vec4(vec4_t *out, mat4_t *m, vec4_t *v);
void mat4_mul_vec4_project(vec4_t *out, mat4_t *mat_proj, vec4_t *v);
void mat4_mul_mat4(mat4_t *out, mat4_t *m0, mat4_t *m1);

void mat4_make_scale(mat4_t *out, float sx, float sy, float sz);
void mat4_make_translation(mat4_t *out, float tx, float ty, float tz);
void mat4_make_rotation_x(mat4_t *out, float angle);
void mat4_make_rotation_y(mat4_t *out, float angle);
void mat4_make_rotation_z(mat4_t *out, float angle);
void mat4_make_perspective(mat4_t *out, float fov, float aspect, float near, float far);
void mat4_look_at(mat4_t *out, vec3_t *eye, vec3_t *target, vec3_t *up);

#endif
