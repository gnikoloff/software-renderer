#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"

typedef struct {
	vec3_t position;
	vec3_t direction;
	vec3_t forward_velocity;
	float yaw;
	float pitch;
} camera_t;

// Camera position
vec3_t* get_camera_position(void);
void set_camera_position(vec3_t pos);
void set_camera_position_x(float x);
void add_camera_position_x(float x);
void set_camera_position_y(float y);
void add_camera_position_y(float y);
void set_camera_position_z(float z);
void add_camera_position_z(float z);

// Camera direction
vec3_t* get_camera_direction(void);
void set_camera_direction(vec3_t pos);
void set_camera_direction_x(float x);
void add_camera_direction_x(float x);
void set_camera_direction_y(float y);
void add_camera_direction_y(float y);
void set_camera_direction_z(float z);
void add_camera_direction_z(float z);

// Camera velocity
void set_camera_forward_velocity(vec3_t vel);
vec3_t get_camera_forward_velocity(void);
void set_camera_forward_velocity_x(float x);
void set_camera_forward_velocity_y(float y);
void set_camera_forward_velocity_z(float z);

// Camera yaw
float get_camera_yaw(void);
void set_camera_yaw(float yaw);
void add_camera_yaw(float yaw);

// Camera pitch
float get_camera_pitch(void);
void set_camera_pitch(float pitch);
void add_camera_pitch(float pitch);

#endif