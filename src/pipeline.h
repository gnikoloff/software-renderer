#ifndef PIPELINE_H
#define PIPELINE_H

#include "camera.h"
#include "mesh.h"

enum cull_mode {
	CULL_NONE,
	CULL_BACKFACE
};

enum render_mode {
	RENDER_VERTEX,
	RENDER_WIRE,
	RENDER_TRIANGLE
};

typedef void (*vertex_shader_callback)(
	int camera_type,
	void* camera,
	mesh_t* mesh,
	vertex_t*
);

typedef struct {
	uint32_t color;
	float depth;
	color_framebuffer* color_buffer;
	depth_framebuffer* depth_buffer;
} fragment_shader_result_t;

typedef struct {
	int x;
	int y;
	float u;
	float v;
	float interpolated_w;
	float interpolated_world_space_pos_x;
	float interpolated_world_space_pos_y;
	float interpolated_world_space_pos_z;
} fragment_shader_triangle_inputs;

typedef struct {
	int x;
	int y;
} fragment_shader_point_inputs;

typedef struct {
	int x;
	int y;
} fragment_shader_line_inputs;

typedef fragment_shader_result_t (*fragment_shader_callback)(
	int camera_type,
	void* camera,
	mesh_t* mesh,
	void* shader_inputs
);

void pipeline_draw(
	int camera_type,
	void* camera,
	mesh_t* mesh,
	int cull_mode,
	int render_mode,
	vertex_shader_callback vs_shader,
	fragment_shader_callback fs_shader
);

#endif