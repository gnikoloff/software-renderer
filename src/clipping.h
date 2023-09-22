#ifndef CLIPPING_H
#define CLIPPING_H

#include "vector.h"
#include "texture.h"
#include "triangle.h"

#define MAX_NUMBER_POLY_VERTICES 10
#define MAX_NUMBER_POLY_TRIANGLES 10

enum {
	TOP_FRUSTUM_PLANE,
	RIGHT_FRUSTUM_PLANE,
	BOTTOM_FRUSTUM_PLANE,
	LEFT_FRUSTUM_PLANE,
	NEAR_FRUSTUM_PLANE,
	FAR_FRUSTUM_PLANE
};

typedef struct {
	vec3_t point;
	vec3_t normal;
} plane_t;

typedef struct {
	vec3_t camera_space_vertices[MAX_NUMBER_POLY_VERTICES];
	vec3_t world_space_vertices[MAX_NUMBER_POLY_VERTICES];
	vec3_t normals[MAX_NUMBER_POLY_VERTICES];
	tex2_t texcoords[MAX_NUMBER_POLY_VERTICES];
	int num_vertices;
} polygon_t;

void init_frustum_planes(float fovx, float fovy, float z_near, float z_far);
polygon_t create_polygon_from_triangles(
	vec3_t view_projection_v0,
	vec3_t view_projection_v1,
	vec3_t view_projection_v2,
	vec3_t world_v0,
	vec3_t world_v1,
	vec3_t world_v2,
	vec3_t normal_v0,
	vec3_t normal_v1,
	vec3_t normal_v2,
	tex2_t t0,
	tex2_t t1,
	tex2_t t2
);
void triangles_from_polygon(polygon_t* polygon, triangle_t triangles[], int* num_triangles);
void clip_polygon(polygon_t* polygon);

#endif