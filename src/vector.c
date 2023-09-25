#include <math.h>
#include "vector.h"

// static const float cEpslion = 1e-6f;

// https://en.wikipedia.org/wiki/Fast_inverse_square_root
float InvSqrt(float x)
{
    float xhalf = 0.5f * x;
    int i = *(int*)&x;              // get bits for floating value
    i = 0x5f375a86 - (i >> 1);      // gives initial guess y0
    x = *(float*)&i;                // convert bits back to float
    x = x * (1.5f - xhalf * x * x); // Newton step, repeating increases accuracy
    return x;
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

inline void vec2_sub(vec2_t* result, vec2_t* a, vec2_t* b) {
	result->x = a->x - b->x;
	result->y = a->y - b->y;
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
	float inv_sqrt = InvSqrt(v->x * v->x + v->y * v->y);
	v->x *= inv_sqrt;
	v->y *= inv_sqrt;
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

// https://asawicki.info/news_1301_reflect_and_refract_functions.html
// reflect and refract methods
inline vec3_t vec3_reflect(vec3_t incident_vec, vec3_t normal) {
	float dot = vec3_dot(incident_vec, normal);
	vec3_t normal_scaled = vec3_mul(normal, dot * 2);
	return vec3_sub(incident_vec, normal_scaled);
}

inline vec3_t vec3_refract(vec3_t incident_vec, vec3_t normal, float eta) {
	float N_dot_I = vec3_dot(normal, incident_vec);
  float k = 1.f - eta * eta * (1.f - N_dot_I * N_dot_I);
	vec3_t out;
  if (k < 0.f) {
    out = vec3_new(0, 0, 0);
  } else {
		vec3_t eta_incident_vec = vec3_mul(incident_vec, eta);
    out = vec3_sub(eta_incident_vec, vec3_mul(normal, eta * N_dot_I + sqrtf(k)));
	}
	return out;
}

inline float vec3_dot(vec3_t a, vec3_t b) {
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

inline void vec3_normalize(vec3_t* v) {
	float inv_sqrt = InvSqrt(v->x * v->x + v->y * v->y + v->z * v->z);
	v->x *= inv_sqrt;
	v->y *= inv_sqrt;
	v->z *= inv_sqrt;
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
