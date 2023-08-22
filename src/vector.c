#include "math.h"
#include "vector.h"

/* -------------------------------------------------------------------------- */
// Vector2
/* -------------------------------------------------------------------------- */
vec2_t vec2_new(float x, float y) {
	vec2_t result = { x, y };
	return result;
}

vec2_t vec2_clone(vec2_t *v) {
	vec2_t result = { .x = v->x, .y = v->y };
	return result;
}

vec2_t vec2_from_vec4(vec4_t *v) {
	vec2_t result = { .x = v->x, .y = v->y };
	return result;
}

void vec2_add(vec2_t *out, vec2_t *v0, vec2_t *v1) {
	out->x = v0->x + v1->x;
	out->y = v0->y + v1->y;
}

void vec2_sub(vec2_t *out, vec2_t *v0, vec2_t *v1) {
	out->x = v0->x - v1->x;
	out->y = v0->y - v1->y;
}

void vec2_mul(vec2_t *out, vec2_t *v, float factor) {
	out->x = v->x * factor;
	out->y = v->y * factor;
}

void vec2_div(vec2_t *out, vec2_t *v, float factor) {
	out->x = v->x / factor;
	out->y = v->y / factor;
}

void vec2_normalize(vec2_t *v) {
	float length = vec2_length(v);
	v->x /= length;
	v->y /= length;
}

float vec2_length(vec2_t *v) {
	return sqrt((v->x * v->x) + (v->y * v->y));
}

float vec2_dot(vec2_t *v0, vec2_t *v1) {
	return (v0->x * v1->x) + (v0->y * v1->y);
}

/* -------------------------------------------------------------------------- */
// Vector3
/* -------------------------------------------------------------------------- */
vec3_t vec3_clone(vec3_t *v) {
	vec3_t result = { .x = v->x, .y = v->y, .z = v->z };
	return result;
}

vec3_t vec3_new(float x, float y, float z) {
	vec3_t result = { x, y, z };
	return result;
}

vec3_t vec3_from_vec4(vec4_t *v) {
	vec3_t result = { .x = v->x, .y = v->y, .z = v->z };
	return result;
}

float vec3_length(vec3_t *v) {
	return sqrt((v->x * v->x) + (v->y * v->y) + (v->z * v->z));
}

float vec3_dot(vec3_t *v0, vec3_t *v1) {
	return v0->x * v1->x + v0->y * v1->y + v0->z * v1->z;
}

void vec3_normalize(vec3_t* v) {
	float length = vec3_length(v);
	v->x /= length;
	v->y /= length;
	v->z /= length;
}

void vec3_add(vec3_t *out, vec3_t *v0, vec3_t *v1) {
	out->x = v0->x + v1->x;
	out->y = v0->y + v1->y;
	out->z = v0->z + v1->z;
}

void vec3_sub(vec3_t *out, vec3_t *v0, vec3_t *v1) {
	out->x = v0->x - v1->x;
	out->y = v0->y - v1->y;
	out->z = v0->z - v1->z;
}

void vec3_mul(vec3_t *out, vec3_t *v, float factor) {
	out->x = v->x * factor;
	out->y = v->y * factor;
	out->z = v->z * factor;
}

void vec3_div(vec3_t *out, vec3_t *v, float factor) {
	out->x = v->x / factor;
	out->y = v->y / factor;
	out->z = v->z / factor;
}

void vec3_cross(vec3_t *out, vec3_t *v0, vec3_t *v1) {
	out->x = (v0->y * v1->z) - (v0->z * v1->y);
	out->y = (v0->z * v1->x) - (v0->x * v1->z);
	out->z = (v0->x * v1->y) - (v0->y * v1->x);
}

void vec3_rotate_x(vec3_t *out, vec3_t *v, float angle) {
	out->x = v->x;
	out->y = v->y * cos(angle) - v->z * sin(angle);
	out->z = v->y * sin(angle) + v->z * cos(angle);
}

void vec3_rotate_y(vec3_t *out, vec3_t *v, float angle) {
	out->x = v->x * cos(angle) - v->z * sin(angle);
	out->y = v->y;
	out->z = v->x * sin(angle) + v->z * cos(angle);
}

void vec3_rotate_z(vec3_t *out, vec3_t *v, float angle) {
	out->x = v->x * cos(angle) - v->y * sin(angle),
	out->y = v->x * sin(angle) + v->y * cos(angle),
	out->z = v->z;
}

/* -------------------------------------------------------------------------- */
// Vector4
/* -------------------------------------------------------------------------- */
vec4_t vec4_from_vec3(vec3_t *v) {
	vec4_t result = { .x = v->x, .y = v->y, .z = v->z, .w = 1 };
	return result;
}

vec4_t vec4_new(float x, float y, float z, float w) {
	vec4_t result = { .x = x, .y = y, .z = z, .w = w };
	return result;
}
