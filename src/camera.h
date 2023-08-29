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
	mat4_t inverse_view_matrix;
	mat4_t inverse_projection_matrix;
} perspective_camera_t;

typedef struct {
	vec3_t position;
	vec3_t target;
	vec3_t rotation;

	float left;
	float right;
	float top;
	float bottom;
	float z_near;
	float z_far;

	mat4_t view_matrix;
	mat4_t projection_matrix;
	mat4_t inverse_view_matrix;
	mat4_t inverse_projection_matrix;
} orthographic_camera_t;

enum camera_type {
	PERSPECTIVE_CAMERA,
	ORTHOGRAPHIC_CAMERA
};

perspective_camera_t* make_perspective_camera(
	float fov,
	float aspect,
	float z_near,
	float z_far,
	vec3_t position,
	vec3_t target
);
orthographic_camera_t* make_orthographic_camera(
	float left,
	float right,
	float top,
	float bottom,
	float z_near,
	float z_far,
	vec3_t position,
	vec3_t target
);
void update_camera_on_drag(perspective_camera_t* camera, int mouse_x, int mouse_y);

#endif
