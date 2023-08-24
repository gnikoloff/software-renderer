#include <assert.h>
#include <math.h>
#include "stdio.h"
#include "camera.h"

#define MAX_NUM_CAMERAS 10
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define CLAMP(min, max, v) MIN(max, MAX(min, v))

static camera_t cameras[MAX_NUM_CAMERAS];
static int camera_count = 0;
static vec3_t up = { 0, 1, 0 };
static mat4_t rotation_matrix = {{{ 0 }}};
static mat4_t yaw_rotation_matrix = {{{ 0 }}};
static mat4_t pitch_rotation_matrix = {{{ 0 }}};
static float rotation_x = 0.0;
static float rotation_y = 0.0;

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
	// mat4_make_rotation_y(&yaw_rotation_matrix, camera->yaw);
	// mat4_make_rotation_x(&pitch_rotation_matrix, camera->pitch);
	// // mat4_mul_mat4(&rotation_matrix, &yaw_rotation_matrix, &pitch_rotation_matrix);

	vec4_t target = vec4_from_vec3(camera->target);
	// mat4_mul_vec4(&target, &yaw_rotation_matrix, &target);
	// mat4_mul_vec4(&target, &pitch_rotation_matrix, &target);
	// // mat4_mul_vec4(&target, &rotation_matrix, &target);
	
	vec3_t target3 = vec3_from_vec4(target);

	camera->view_matrix = mat4_look_at(camera->position, target3, up);
	camera->view_projection_matrix = mat4_mul_mat4(camera->view_matrix, camera->projection_matrix);
}


// distance = origDistance! + pinchFactor!
// distance = min(maxDistance, distance)
// distance = max(minDistance, distance)

// if input.leftMouseDown {
// 	let sensitivity = Settings.mousePanSensitivity
// 	rotation.x += input.mouseDelta.y * sensitivity
// 	rotation.y += input.mouseDelta.x * sensitivity
// 	rotation.x = max(minPolarAngle, min(rotation.x,   ))
// }
// let rotateMatrix = float4x4(
// 	rotationYXZ: [-rotation.x, rotation.y, 0])
// let distanceVector = float4(0, 0, -distance, 0)
// let rotatedVector = rotateMatrix * distanceVector
// position = target + rotatedVector.xyz

void update_camera_on_drag(camera_t *camera, int xrel, int yrel) {
	// printf("%i %i\n", xrel, yrel);
	
	camera->distance = CLAMP(camera->min_distance, camera->max_distance, camera->distance);

	camera->rotation.x += ((float)xrel) * 0.005;
	camera->rotation.y += ((float)yrel) * 0.005;

	camera->rotation.x = CLAMP(camera->min_polar_angle, camera->max_polar_angle, camera->rotation.x);

	rotation_matrix = mat4_make_rotation_yxz(
		-camera->rotation.x,
		-camera->rotation.y,
		camera->rotation.z
	);

	vec4_t distance_vector = vec4_new(0, 0, -camera->distance, 0);
	vec4_t rotated_vector;
	rotated_vector = mat4_mul_vec4(rotation_matrix, distance_vector);

	vec3_t rotated_vector3 = vec3_from_vec4(rotated_vector);
	camera->position = vec3_add(camera->target, rotated_vector3);

	// printf("%f %f %f\n", camera->position.x, camera->position.y, camera->position.z);
	update_camera_view_matrix(camera);
}
