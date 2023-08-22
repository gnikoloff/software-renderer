#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
	float x;
	float y;
} vec2_t;

typedef struct {
	float x;
	float y;
	float z;
} vec3_t;

typedef struct {
	float x;
	float y;
	float z;
	float w;
} vec4_t;

/* -------------------------------------------------------------------------- */
// Vector2
/* -------------------------------------------------------------------------- */
vec2_t vec2_new(float x, float y);
vec2_t vec2_clone(vec2_t *v);
vec2_t vec2_from_vec4(vec4_t *v);
void vec2_add(vec2_t *out, vec2_t *v0, vec2_t *v1);
void vec2_sub(vec2_t *out, vec2_t *v0, vec2_t *v1);
void vec2_mul(vec2_t *out, vec2_t *v, float factor);
void vec2_div(vec2_t *out, vec2_t *v, float factor);
void vec2_normalize(vec2_t *v);
float vec2_length(vec2_t *v);
float vec2_dot(vec2_t *v0, vec2_t *v1);

/* -------------------------------------------------------------------------- */
// Vector3
/* -------------------------------------------------------------------------- */
vec3_t vec3_clone(vec3_t *v);
vec3_t vec3_new(float x, float y, float z);
vec3_t vec3_from_vec4(vec4_t *v);
float vec3_length(vec3_t *v);
float vec3_dot(vec3_t *v0, vec3_t *v1);
void vec3_normalize(vec3_t* v);
void vec3_add(vec3_t *out, vec3_t *v0, vec3_t *v1);
void vec3_sub(vec3_t *out, vec3_t *v0, vec3_t *v1);
void vec3_mul(vec3_t *out, vec3_t *v, float factor);
void vec3_div(vec3_t *out, vec3_t *v, float factor);
void vec3_cross(vec3_t *out, vec3_t *v0, vec3_t *v1);
void vec3_rotate_x(vec3_t* out, vec3_t *v, float angle);
void vec3_rotate_y(vec3_t* out, vec3_t *v, float angle);
void vec3_rotate_z(vec3_t* out, vec3_t *v, float angle);

/* -------------------------------------------------------------------------- */
// Vector 4
/* -------------------------------------------------------------------------- */
vec4_t vec4_new(float x, float y, float z, float w);
vec4_t vec4_from_vec3(vec3_t *v);

#endif