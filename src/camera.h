#ifndef CAMERA_H
#define CAMERA_H

#include <math.h>
#include "vector.h"
#include "matrix.h"

typedef struct {
	vec3_t position;
	vec3_t target;
	vec3_t rotation;

	float fov;
	float aspect;
	float z_near;
	float z_far;
	float yaw;
	float pitch;
	float min_polar_angle;
	float max_polar_angle;
	float distance;
	float min_distance;
	float max_distance;

	mat4_t view_matrix;
	mat4_t projection_matrix;
	mat4_t view_projection_matrix;
} camera_t;

camera_t* make_perspective_camera(
	float fov,
	float aspect,
	float z_near,
	float z_far,
	vec3_t position,
	vec3_t target
);
void update_camera_projection_matrix(camera_t *camera);
void update_camera_view_matrix(camera_t *camera);
void update_camera_on_drag(camera_t *camera, int mouse_x, int mouse_y);

#endif
