#include "camera.h"

static camera_t camera = {
	.position = { 0, 0, -5 },
	.direction = { 0, 0, 1 },
	.forward_velocity = { 0, 0, 0 },
	.yaw = 0,
	.pitch = 0
};

// Camera position

vec3_t* get_camera_position(void) {
	return &camera.position;
}

void set_camera_position(vec3_t pos) {
	camera.position = pos;
}

void set_camera_position_x(float x) {
	camera.position.x = x;
}

void add_camera_position_x(float x) {
	camera.position.x += x;
}

void set_camera_position_y(float y) {
	camera.position.y = y;
}

void add_camera_position_y(float y) {
	camera.position.y += y;
}

void set_camera_position_z(float z) {
	camera.position.z = z;
}

void add_camera_position_z(float z) {
	camera.position.z += z;
}

// Camera direction

vec3_t* get_camera_direction(void) {
	return &camera.direction;
}

void set_camera_direction(vec3_t pos) {
	camera.direction = pos;
}

void set_camera_direction_x(float x) {
	camera.direction.x = x;
}

void add_camera_direction_x(float x) {
	camera.direction.x += x;
}

void set_camera_direction_y(float y) {
	camera.direction.y = y;
}

void add_camera_direction_y(float y) {
	camera.direction.y += y;
}

void set_camera_direction_z(float z) {
	camera.direction.z = z;
}

void add_camera_direction_z(float z) {
	camera.direction.y += z;
}

// Camera velocity

void set_camera_forward_velocity(vec3_t vel) {
	camera.forward_velocity = vel;
}

vec3_t get_camera_forward_velocity(void) {
	return camera.forward_velocity;
}

void set_camera_forward_velocity_x(float x) {
	camera.forward_velocity.x = x;
}

void set_camera_forward_velocity_y(float y) {
	camera.forward_velocity.y = y;
}

void set_camera_forward_velocity_z(float z) {
	camera.forward_velocity.z = z;
}

// Camera yaw

float get_camera_yaw(void) {
	return camera.yaw;
}

void set_camera_yaw(float yaw) {
	camera.yaw = yaw;
}

void add_camera_yaw(float yaw) {
	camera.yaw += yaw;
}

// Camera pitch

float get_camera_pitch(void) {
	return camera.pitch;
}

void set_camera_pitch(float pitch) {
	camera.pitch = pitch;
}

void add_camera_pitch(float pitch) {
	camera.pitch += pitch;
}

