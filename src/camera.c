#include <assert.h>
#include "stdio.h"
#include "camera.h"
#include "utils.h"

#define MAX_NUM_PERSP_CAMERAS 10
#define MAX_NUM_ORTHO_CAMERAS 10

static perspective_camera_t perspective_cameras[MAX_NUM_PERSP_CAMERAS];
static orthographic_camera_t orthographic_cameras[MAX_NUM_PERSP_CAMERAS];
static int perspective_camera_count = 0;
static int orthographic_camera_count = 0;
static vec3_t up = { 0, 1, 0 };
static mat4_t temp_rotation_matrix = {{{ 0 }}};

perspective_camera_t* make_perspective_camera(
	float fov,
	float aspect,
	float z_near,
	float z_far,
	vec3_t position,
	vec3_t target
) {
	printf("Create camera %d\n", perspective_camera_count);
	assert(perspective_camera_count < MAX_NUM_PERSP_CAMERAS);

	perspective_camera_t *camera = &perspective_cameras[perspective_camera_count];

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
	camera->max_distance = 25.0;

	camera->projection_matrix = mat4_make_perspective(camera->fov, camera->aspect, camera->z_near, camera->z_far);
	camera->inverse_projection_matrix = mat4_inverse(camera->projection_matrix);

	camera->view_matrix = mat4_look_at(position, target, up);
	camera->inverse_view_matrix = mat4_inverse(camera->view_matrix);

	perspective_camera_count++;

	return camera;
}

orthographic_camera_t* make_orthographic_camera(
	float left,
	float right,
	float top,
	float bottom,
	float z_near,
	float z_far,
	vec3_t position,
	vec3_t target
) {
	printf("Create camera %d\n", perspective_camera_count);
	assert(orthographic_camera_count < MAX_NUM_ORTHO_CAMERAS);

	orthographic_camera_t *camera = &orthographic_cameras[orthographic_camera_count];

	camera->left = left;
	camera->right = right;
	camera->top = top;
	camera->bottom = bottom;
	camera->z_near = z_near;
	camera->z_far = z_far;
	camera->position = position;
	camera->target = target;

	camera->projection_matrix = mat4_make_orthographic(left, right, top, bottom, z_near, z_far);
	camera->inverse_projection_matrix = mat4_inverse(camera->projection_matrix);

	camera->view_matrix = mat4_look_at(position, target, up);
	camera->inverse_view_matrix = mat4_inverse(camera->view_matrix);

	orthographic_camera_count++;
	return camera;
}

void update_camera_on_drag(perspective_camera_t* camera, int xrel, int yrel) {
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

	camera->view_matrix = mat4_look_at(camera->position, camera->target, up);
	camera->inverse_view_matrix = mat4_inverse(camera->view_matrix);
}
