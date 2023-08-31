#include "stdio.h"
#include "math.h"
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

#include "geometry-demo.h"

static perspective_camera_t* camera = NULL;
static mesh_t* plane = NULL;
static mesh_t* sphere = NULL;
static mesh_t* box = NULL;
static mesh_t* ring = NULL;
static mesh_t* torus = NULL;
static mesh_t* efa = NULL;

void geometry_example_setup(void) {
	float aspectx = (float)get_viewport_width() / (float)get_viewport_height();
	float aspecty = (float)get_viewport_height() / (float)get_viewport_width();
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
	plane->rotation.x = M_PI / 2;
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
	ring->rotation.x = M_PI / 2;
	load_mesh_png_data(ring, "./assets/debug.png");
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

	// plane->rotation.x += delta_time * delta_multiplier;
	sphere->rotation.y += delta_time * delta_multiplier;
	box->rotation.z -= delta_time * delta_multiplier;
	torus->rotation.y += delta_time * delta_multiplier;
	torus->rotation.z += delta_time * delta_multiplier;
	efa->rotation.y += delta_time * delta_multiplier;
}

void geometry_example_render(int delta_time) {
	for (int mesh_index = 0; mesh_index < get_meshes_count(); mesh_index++) {
		mesh_t* mesh = get_mesh(mesh_index);
		
	}
}

void geometry_example_free_resources(void) {
	dispose_meshes();
}