#include "stdio.h"
#include "math.h"
#include <SDL2/SDL.h>
#include "geometry-demo.h"
#include "../utils.h"
#include "../array.h"
#include "../vector.h"
#include "../matrix.h"
#include "../camera.h"
#include "../display.h"
#include "../clipping.h"
#include "../geometry.h"
#include "../triangle.h"
#include "../light.h"
#include "../pipeline.h"

static camera_t* camera = NULL;
static mesh_t* plane = NULL;
static mesh_t* sphere = NULL;
static mesh_t* box = NULL;
static mesh_t* ring = NULL;
static mesh_t* torus = NULL;
static mesh_t* efa = NULL;

void geometry_example_setup(void) {
	set_render_method(RENDER_TEXTURED);
	set_cull_method(CULL_NONE);

	float aspectx = (float)get_window_width() / (float)get_window_height();
	float aspecty = (float)get_window_height() / (float)get_window_width();
	float fovy = 3.141592 / 3.0;
	float fovx = atan(tan(fovy / 2) * aspectx) * 2;
	float z_near = 1.0;
	float z_far = 100.0;

	vec3_t cam_position = { .x = 4, .y = 5, .z = -7 };
	vec3_t cam_target = { .x = 0, .y = 0, .z = 0 };
	camera = make_perspective_camera(fovy, aspecty, z_near, z_far, cam_position, cam_target);
	
	init_frustum_planes(fovx, fovy, z_near, z_far);

	plane = make_plane(3, 3, 3, 3);
	load_mesh_png_data(plane, "./assets/debug.png");
	plane->translation.x = 2;
	plane->translation.z = 3;

	sphere = make_sphere(1, 10, 10, 0, M_PI * 2, 0, M_PI);
	load_mesh_png_data(sphere, "./assets/debug.png");
	sphere->translation.x = -2;
	sphere->translation.z = 3;

	box = make_box(1, 1, 1, 4, 4, 4);
	load_mesh_png_data(box, "./assets/debug.png");
	box->translation.x = 2;
	box->translation.z = 0;

	ring = make_ring(0.5, 1, 32, 10, 0, M_PI * 2);
	load_mesh_png_data(ring, "./assets/Debug.png");
	ring->translation.x = -2;
	ring->translation.z = 0;

	torus = make_torus(1, 0.4, 12, 48, M_PI * 2);
	load_mesh_png_data(torus, "./assets/debug.png");
	torus->translation.x = 2;
	torus->translation.z = -3;

	efa = load_mesh(
		"./assets/efa.obj",
		"./assets/efa.png",
		vec3_new(1, 1, 1),
		vec3_new(0, 0, 0),
		vec3_new(0, 0, 0)
	);
	efa->translation.x = -2;
	efa->translation.z = -3;
	efa->scale.x = 0.75;
	efa->scale.y = 0.75;
	efa->scale.z = 0.75;
}

void geometry_example_process_input(SDL_Event* event, int delta_time) {
	switch (event->type) {
		case SDL_MOUSEMOTION:
			if (event->motion.state & SDL_BUTTON_LMASK) {
				update_camera_on_drag(camera, event->motion.xrel, event->motion.yrel);
			}
			break;
		case SDL_MOUSEWHEEL:
			camera->distance += -event->wheel.preciseY * 0.01 * delta_time;
			update_camera_on_drag(camera, 0, 0);
			break;
	}
}

void geometry_example_update(int delta_time) {
	float delta_multiplier = 0.00075;
	box->rotation.x += delta_time * delta_multiplier;
	box->rotation.y += delta_time * delta_multiplier;
	box->rotation.z += delta_time * delta_multiplier;

	plane->rotation.x += delta_time * delta_multiplier;
	sphere->rotation.y += delta_time * delta_multiplier;
	box->rotation.z -= delta_time * delta_multiplier;
	ring->rotation.x += delta_time * delta_multiplier;
	ring->rotation.y -= delta_time * delta_multiplier;
	torus->rotation.y += delta_time * delta_multiplier;
	torus->rotation.z += delta_time * delta_multiplier;
	efa->rotation.y += delta_time * delta_multiplier;
}

void render_triangle(triangle_t* triangle) {
	if (should_render_filled_triangles()) {
		draw_filled_triangle(
			triangle->points[0].x, triangle->points[0].y, triangle->points[0].z, triangle->points[0].w,
			triangle->points[1].x, triangle->points[1].y, triangle->points[1].z, triangle->points[1].w,
			triangle->points[2].x, triangle->points[2].y, triangle->points[2].z, triangle->points[2].w,
			triangle->color
		);
	}

	if (should_render_textured_triangles()) {
		if (triangle->texture == NULL) {
			draw_triangle(
				triangle->points[0].x, triangle->points[0].y,
				triangle->points[1].x, triangle->points[1].y,
				triangle->points[2].x, triangle->points[2].y,
				0xFFFF0000
			);
		} else {
			draw_textured_triangle(
				triangle->points[0].x, triangle->points[0].y, triangle->points[0].z, triangle->points[0].w, triangle->tex_coords[0].u, triangle->tex_coords[0].v,
				triangle->points[1].x, triangle->points[1].y, triangle->points[1].z, triangle->points[1].w, triangle->tex_coords[1].u, triangle->tex_coords[1].v,
				triangle->points[2].x, triangle->points[2].y, triangle->points[2].z, triangle->points[2].w, triangle->tex_coords[2].u, triangle->tex_coords[2].v,
				triangle->texture
			);
		}
	}

	if (should_render_wireframe()) {
		draw_triangle(
			triangle->points[0].x, triangle->points[0].y,
			triangle->points[1].x, triangle->points[1].y,
			triangle->points[2].x, triangle->points[2].y,
			0xFFFF0000
		);
	}

	if (should_render_vertex()) {
		for (int j = 0; j < 3; j++) {
			int size = 10;
			draw_rect(triangle->points[j].x - size / 2, triangle->points[j].y - size / 2, size, size, 0xFFFF0000);
		}
	}
}

void geometry_example_render(int delta_time) {
	for (int mesh_index = 0; mesh_index < get_meshes_count(); mesh_index++) {
		mesh_t* mesh = get_mesh(mesh_index);
		render_mesh(mesh, camera, render_triangle);
	}
}

void geometry_example_free_resources(void) {
	dispose_meshes();
}