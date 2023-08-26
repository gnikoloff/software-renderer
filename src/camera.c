#include <assert.h>
#include "stdio.h"
#include "camera.h"
#include "utils.h"

#define MAX_NUM_CAMERAS 10

static camera_t cameras[MAX_NUM_CAMERAS];
static int camera_count = 0;
static vec3_t up = { 0, 1, 0 };
static mat4_t temp_rotation_matrix = {{{ 0 }}};

camera_t* make_perspective_camera(
	float fov,
	float aspect,
	float z_near,
	float z_far,
	vec3_t position,
	vec3_t target
) {
	printf("Create camera %d\n", camera_count);
	assert(camera_count < MAX_NUM_CAMERAS);

	camera_t *camera = &cameras[camera_count];

	camera->fov = fov;
	camera->aspect = aspect;
	camera->z_near = z_near;
	camera->z_far = z_far;
	camera->position = position;
	camera->min_polar_angle = -M_PI / 2;
	camera->max_polar_angle = M_PI / 2;
	camera->target = target;
	camera->distance = vec3_length(position);
	camera->min_distance = 0.1;
	camera->max_distance = 10.0;

	update_camera_projection_matrix(camera);
	update_camera_view_matrix(camera);

	camera_count++;

	return camera;
}

void update_camera_projection_matrix(camera_t *camera) {
	camera->projection_matrix = mat4_make_perspective(camera->fov, camera->aspect, camera->z_near, camera->z_far);
	camera->view_projection_matrix = mat4_mul_mat4(camera->view_matrix, camera->projection_matrix);
}

void update_camera_view_matrix(camera_t *camera) {
	camera->view_matrix = mat4_look_at(camera->position, camera->target, up);
	camera->view_projection_matrix = mat4_mul_mat4(camera->view_matrix, camera->projection_matrix);
}

void update_camera_on_drag(camera_t *camera, int xrel, int yrel) {
	camera->distance = CLAMP(camera->min_distance, camera->max_distance, camera->distance);

	camera->rotation.x += ((float)xrel) * 0.005;
	camera->rotation.y += ((float)yrel) * 0.005;

	camera->rotation.y = CLAMP(camera->min_polar_angle, camera->max_polar_angle, camera->rotation.y);

	temp_rotation_matrix = mat4_make_rotation_yxz(
		-camera->rotation.x,
		-camera->rotation.y,
		camera->rotation.z
	);

	vec4_t distance_vector = vec4_new(0, 0, -camera->distance, 0);
	vec4_t rotated_vector;
	rotated_vector = mat4_mul_vec4(temp_rotation_matrix, distance_vector);

	vec3_t rotated_vector3 = vec3_from_vec4(rotated_vector);
	camera->position = vec3_add(camera->target, rotated_vector3);

	update_camera_view_matrix(camera);
}
