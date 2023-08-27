#ifndef PIPELINE_H
#define PIPELINE_H

#include "mesh.h"
#include "camera.h"
#include "framebuffer.h"

void render_mesh(
	mesh_t* mesh,
	camera_t* camera,
	depth_framebuffer* z_buffer,
	void (*render_callback)(triangle_t*, depth_framebuffer*)
);

#endif