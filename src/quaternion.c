#include "math.h"
#include "quaternion.h"

inline quat_t quat_new(float x, float y, float z, float w) {
	quat_t result = { .x = x, .y = y, .z = z, .w = w };
	return result;
}

inline quat_t quat_from_vec3(vec3_t v, float s) {
	quat_t result = { .x = v.x, .y = v.y, .z = v.z, .w = s };
	return result;
}

inline vec3_t quat_get_vector_part(quat_t quat) {
	vec3_t result = { .x = quat.x, .y = quat.y, .z = quat.z };
	return result;
}

inline quat_t quat_mul_quat(quat_t q1, quat_t q2) {
	quat_t result = {
		.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
		.y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x,
		.z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w,
		.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z
	};
	return result;	
}

inline vec3_t quat_transform_vec3(quat_t q, vec3_t v) {
	float uvx = q.y * v.z - q.z * v.y;
	float uvy = q.z * v.x - q.x * v.z;
	float uvz = q.x * v.y - q.y * v.x;
  float uuvx = q.y * uvz - q.z * uvy;
	float uuvy = q.z * uvx - q.x * uvz;
	float uuvz = q.x * uvy - q.y * uvx;
  float w2 = q.w * 2;

  uvx *= w2;
  uvy *= w2;
  uvz *= w2;
  uuvx *= 2;
  uuvy *= 2;
  uuvz *= 2;

	vec3_t result = {
		.x = v.x + uvx + uuvx,
		.y = v.y + uvy + uuvy,
		.z = v.z + uvz + uuvz
	};
	
	return result;
}

inline mat4_t quat_get_rotation_matrix(quat_t q) {
	float x2 = q.x * q.x;
	float y2 = q.y * q.y;
	float z2 = q.z * q.z;
	float xy = q.x * q.y;
	float xz = q.x * q.z;
	float yz = q.y * q.z;
	float wx = q.w * q.x;
	float wy = q.w * q.y;
	float wz = q.w * q.z; 

	mat4_t result = mat4_identity();

	result.m[0][0] = 1 - 2 * (y2 + z2);
	result.m[0][1] = 2 * (xy - wz);
	result.m[0][2] = 2 * (xz + wy);

	result.m[1][0] = 2 * (xy + wz);
	result.m[1][1] = 1 - 2 * (x2 + z2);
	result.m[1][2] = 2 * (yz - wx);

	result.m[2][0] = 2 * (xz - wy);
	result.m[2][1] = 2 * (yz + wx);
	result.m[2][2] = 1 - 2 * (x2 + y2);

	return result;
}

inline quat_t quat_rotate_x(quat_t q, float a) {
	a /= 2;
	float bx = sin(a);
	float bw = cos(a);

	quat_t result = {
		.x = q.x * bw + q.w * bx,
		.y = q.y * bw + q.z * bx,
		.z = q.z * bw - q.y * bx,
		.w = q.w * bw - q.x * bx
	};

	return result;
}

inline quat_t quat_rotate_z(quat_t q, float a) {
	a /= 2;
	float bz = sin(a);
	float bw = cos(a);
	quat_t result = {
		.x = q.x * bw + q.y * bz,
		.y = q.y * bw - q.x * bz,
		.z = q.z * bw + q.w * bz,
		.w = q.w * bw - q.z * bz
	};
	return result;
}

inline quat_t quat_rotate_y(quat_t q, float a) {
	a /= 2;
	float by = sin(a);
	float bw = cos(a);
	quat_t result = {
		.x = q.x * bw - q.z * by,
		.y = q.y * bw + q.w * by,
		.z = q.z * bw + q.x * by,
		.w = q.w * bw - q.y * by
	};
	return result;
}

inline void quat_identity(quat_t* q) {
	q->x = 0;
	q->y = 0;
	q->z = 0;
	q->w = 1.0;
}
