#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdint.h>
#include <stdbool.h>
#include "upng.h"
#include "matrix.h"
#include "vector.h"
#include "texture.h"
#include "triangle.h"
#include "framebuffer.h"

typedef struct {
	vec4_t position;
	vec3_t normal;
	vec4_t world_space_position;
	tex2_t uv;
	uint32_t color;
} vertex_t;

typedef struct {
	int a;
	int b;
	int c;
	tex2_t a_uv;
	tex2_t b_uv;
	tex2_t c_uv;
	uint32_t color;
} face_t;

typedef struct {
	vertex_t vertices[3];
} triangle_t;

vec3_t barycentric_weights(vec2_t a, vec2_t b, vec2_t c, vec2_t p);

#endif