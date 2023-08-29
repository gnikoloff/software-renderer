#ifndef PIPELINE_H
#define PIPELINE_H

#include "camera.h"
#include "mesh.h"

void pipeline_draw(
	int camera_type,
	void* camera,
	mesh_t* mesh,
	void (*vertex_shader)(
		int camera_type,
		void* camera,
		mesh_t* mesh,
		vertex_t* vertex
	),
	void (*fragment_shader)(
		int camera_type,
		void* camera,
		mesh_t* mesh,
		int x, int y, float interpolated_reciprocal_w,
		float u, float v,
		float interpolated_x,
		float interpolated_y,
		float interpolated_z
	)
);

#endif