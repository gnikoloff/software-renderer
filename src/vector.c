#include <math.h>
#include "vector.h"

// static const float cEpslion = 1e-6f;

// https://en.wikipedia.org/wiki/Fast_inverse_square_root
float Q_rsqrt(float number) {
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;                       // evil floating point bit level hacking
	i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
	// y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

	return y;
}

inline vec2_t vec2_new(float x, float y) {
	vec2_t result = { x, y };
	return result;
}

inline void vec2_reset(vec2_t* v) {
	v->x = 0;
	v->y = 0;
}

inline float vec2_length(vec2_t v) {
	return sqrt(v.x * v.x + v.y * v.y);
}

inline vec2_t vec2_add(vec2_t a, vec2_t b) {
	vec2_t result = {
		.x = a.x + b.x,
		.y = a.y + b.y
	};
	return result;
}

inline vec2_t vec2_sub(vec2_t a, vec2_t b) {
	vec2_t result = {
		.x = a.x - b.x,
		.y = a.y - b.y
	};
	return result;
}

inline vec2_t vec2_mul(vec2_t v, float factor) {
	vec2_t result = {
		.x = v.x * factor,
		.y = v.y * factor
	};
	return result;
}

inline vec2_t vec2_div(vec2_t v, float factor) {
	vec2_t result = {
		.x = v.x / factor,
		.y = v.y / factor
	};
	return result;
}

inline float vec2_dot(vec2_t a, vec2_t b) {
	return (a.x * b.x) + (a.y * b.y);
}

inline void vec2_normalize(vec2_t* v) {
	float len = vec2_length(*v);
	// float inv = cEpslion;
	// if (abs(len) > cEpslion) {
	// 	inv = 1.0f / len;
	// }
	v->x /= len;
	v->y /= len;
}

inline vec3_t vec3_new(float x, float y, float z) {
	vec3_t result = { x, y, z };
	return result;
}

inline void vec3_reset(vec3_t* v) {
	v->x = 0;
	v->y = 0;
	v->z = 0;
}

inline vec3_t vec3_clone(vec3_t* v) {
	vec3_t result = { v->x, v->y, v->z };
	return result;
}

inline float vec3_length(vec3_t v) {
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

inline vec3_t vec3_add(vec3_t a, vec3_t b) {
	vec3_t result = {
		.x = a.x + b.x,
		.y = a.y + b.y,
		.z = a.z + b.z
	};
	return result;
}

inline vec3_t vec3_sub(vec3_t a, vec3_t b) {
	vec3_t result = {
		.x = a.x - b.x,
		.y = a.y - b.y,
		.z = a.z - b.z
	};
	return result;
}

inline vec3_t vec3_mul(vec3_t v, float factor) {
	vec3_t result = {
		.x = v.x * factor,
		.y = v.y * factor,
		.z = v.z * factor
	};
	return result;
}

inline vec3_t vec3_div(vec3_t v, float factor) {
	vec3_t result = {
		.x = v.x / factor,
		.y = v.y / factor,
		.z = v.z / factor
	};
	return result;
}

inline vec3_t vec3_cross(vec3_t a, vec3_t b) {
	vec3_t result = {
		.x = a.y * b.z - a.z * b.y,
		.y = a.z * b.x - a.x * b.z,
		.z = a.x * b.y - a.y * b.x
	};
	return result;
}

inline float vec3_dot(vec3_t a, vec3_t b) {
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

inline void vec3_normalize(vec3_t* v) {
	float len = vec3_length(*v);
	// float inv = cEpslion;
	// if (abs(len) > cEpslion) {
	// 	inv = 1.0f / len;
	// }
	v->x /= len;
	v->y /= len;
	v->z /= len;
}

inline vec3_t vec3_rotate_x(vec3_t v, float angle) {
	vec3_t rotated_vector = {
		.x = v.x,
		.y = v.y * cos(angle) - v.z * sin(angle),
		.z = v.y * sin(angle) + v.z * cos(angle)
	};
	return rotated_vector;
}

inline vec3_t vec3_rotate_y(vec3_t v, float angle) {
	vec3_t rotated_vector = {
		.x = v.x * cos(angle) + v.z * sin(angle),
		.y = v.y,
		.z = -v.x * sin(angle) + v.z * cos(angle)
	};
	return rotated_vector;
}

inline vec3_t vec3_rotate_z(vec3_t v, float angle) {
	vec3_t rotated_vector = {
		.x = v.x * cos(angle) - v.y * sin(angle),
		.y = v.x * sin(angle) + v.y * cos(angle),
		.z = v.z
	};
	return rotated_vector;
}

inline vec4_t vec4_new(float x, float y, float z, float w) {
	vec4_t result = { .x = x, .y = y, .z = z, .w = w };
	return result;
}

inline vec4_t vec4_from_vec3(vec3_t v) {
	vec4_t result = { v.x, v.y, v.z, 1.0 };
	return result;
}

inline vec3_t vec3_from_vec4(vec4_t v) {
	vec3_t result = { v.x, v.y, v.z };
	return result;
}

inline vec2_t vec2_from_vec4(vec4_t v) {
	vec2_t result = { v.x, v.y };
	return result;
}
