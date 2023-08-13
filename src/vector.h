#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
	float x; float y;
} vec2_t;

typedef struct {
	float x; float y; float z;
} vec3_t;

typedef struct {
	float x; float y; float z; float w;
} vec4_t;

// Vector 2
float vec2_length(vec2_t v);
vec2_t vec2_add(vec2_t v0, vec2_t v1);
vec2_t vec2_subtract(vec2_t v0, vec2_t v1);
vec2_t vec2_mul(vec2_t v, float factor);
vec2_t vec2_div(vec2_t v, float factor);
void vec2_normalize(vec2_t* v);
float vec2_dot(vec2_t v0, vec2_t v1);

// Vector 3
float vec3_length(vec3_t v);
vec3_t vec3_add(vec3_t v0, vec3_t v1);
vec3_t vec3_subtract(vec3_t v0, vec3_t v1);
vec3_t vec3_mul(vec3_t v, float factor);
vec3_t vec3_div(vec3_t v, float factor);
vec3_t vec3_cross(vec3_t v0, vec3_t v1);
void vec3_normalize(vec3_t* v);
float vec3_dot(vec3_t v0, vec3_t v1);
vec3_t vec3_rotate_x(vec3_t v, float angle);
vec3_t vec3_rotate_y(vec3_t v, float angle);
vec3_t vec3_rotate_z(vec3_t v, float angle);

// Vector 4
vec4_t vec4_from_vec3(vec3_t v);
vec3_t vec3_from_vec4(vec4_t v);

#endif