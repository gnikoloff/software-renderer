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

void draw_triangle(
	int x0, int y0,
	int x1, int y1,
	int x2, int y2,
	uint32_t color,
	color_framebuffer* color_buffer
);
void draw_filled_triangle(
	int x0, int y0, float z0, float w0,
	int x1, int y1, float z1, float w1,
	int x2, int y2, float z2, float w2,
	uint32_t color,
	color_framebuffer* color_buffer,
	depth_framebuffer* z_buffer
);
void draw_textured_triangle(
	int x0, int y0, float z0, float w0, float u0, float v0,
	int x1, int y1, float z1, float w1, float u1, float v1,
	int x2, int y2, float z2, float w2, float u2, float v2,
	upng_t* texture,
	color_framebuffer* color_buffer,
	depth_framebuffer* z_buffer
);
void draw_texel(
	int x, int y, upng_t* texture,
	float interpolated_reciprocal_w,
	float u, float v,
	color_framebuffer* color_buffer,
	depth_framebuffer* z_buffer
);
void draw_triangle_pixel(
	int x, int y, uint32_t color,
	vec4_t point_a, vec4_t point_b, vec4_t point_c,
	color_framebuffer* color_buffer,
	depth_framebuffer* z_buffer
);
void draw_depth_pixel(int x, int y);
vec3_t barycentric_weights(vec2_t a, vec2_t b, vec2_t c, vec2_t p);

#endif