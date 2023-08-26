#ifndef GEOMETRY
#define GEOMETRY

#include "mesh.h"

void make_plane_geometry(
	mesh_t *mesh,
	float width,
	float height,
	int width_segments,
	int height_segments
);

void make_sphere_geometry(
	mesh_t *mesh,
	float radius,
	int width_segments,
	int height_segments,
	float phi_start,
	float phi_length,
	float theta_start,
	float theta_length
);

void make_box_geometry(
	mesh_t* mesh,
	float width,
	float heigth,
	float depth,
	int width_segments,
	int height_segments,
	int depth_segments
);

void make_ring_geometry(
	mesh_t* mesh,
	float inner_radius,
	float outer_radius,
	int theta_segments,
	int phi_segments,
	float theta_start,
	float theta_length
);

void make_torus_geometry(
	mesh_t* mesh,
	float radius,
	float tube,
	int radial_segments,
	int tubular_segments,
	float arc
);

#endif