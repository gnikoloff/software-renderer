#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "matrix.h"
#include "quaternion.h"
#include "triangle.h"
#include "upng.h"

typedef struct {
	vec3_t* vertices;
	face_t* faces;
	upng_t* texture;

	vec3_t rotation;
	vec3_t scale;
	vec3_t translation;
	
	mat4_t scale_matrix;
	mat4_t translation_matrix;
	mat4_t rotation_matrix;
	mat4_t world_matrix;
	
	quat_t quaternion;

	int idx;
} mesh_t;

mesh_t* load_mesh(
	char* obj_filename,
	char* png_filename,
	vec3_t scale,
	vec3_t translation,
	vec3_t rotation
);
void load_mesh_obj_data(mesh_t* mesh, char* obj_filename);
void load_mesh_png_data(mesh_t* mesh, char* png_filename);

void mesh_update_world_matrix(mesh_t *mesh);

mesh_t* make_plane(
	float width,
	float height,
	int width_segments,
	int height_segments
);

mesh_t* make_sphere(
	float radius,
	int width_segments,
	int height_segments,
	float phi_start,
	float phi_length,
	float theta_start,
	float theta_length
);
mesh_t* make_box(
	float width,
	float height,
	float depth,
	int width_segments,
	int height_segments,
	int depth_segments
);
mesh_t* make_ring(
	float inner_radius,
	float outer_radius,
	int theta_segments,
	int phi_segments,
	float theta_start,
	float theta_length
);
mesh_t* make_torus(
	float radius,
	float tube,
	int radial_segments,
	int tubular_segments,
	float arc
);
void init_mesh_common_properties(mesh_t* mesh);

int get_meshes_count(void);
mesh_t* get_mesh(int index);
void dispose_mesh(mesh_t* mesh);
void dispose_meshes(void);

#endif