#ifndef QUATERNION_H
#define QUATERNION_H

#include "vector.h"
#include "matrix.h"

typedef struct {
	float x, y, z, w;
} quat_t;

quat_t quat_new(float x, float y, float z, float w);
quat_t quat_from_vec3(vec3_t v, float s);
vec3_t quat_get_vector_part(quat_t quat);
quat_t quat_mul_quat(quat_t q1, quat_t q2);
vec3_t quat_transform_vec3(quat_t q, vec3_t v);
mat4_t quat_get_rotation_matrix(quat_t q);
quat_t quat_rotate_x(quat_t q, float a);
quat_t quat_rotate_z(quat_t q, float a);
quat_t quat_rotate_y(quat_t q, float a);

#endif
