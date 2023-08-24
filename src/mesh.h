#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "matrix.h"
#include "triangle.h"
#include "upng.h"

typedef struct {
	vec3_t* vertices;
	face_t* faces;
	upng_t* texture;
	vec3_t rotation;
	vec3_t scale;
	mat4_t scale_matrix;
	vec3_t translation;
	mat4_t translation_matrix;
	mat4_t rotation_matrix;
	mat4_t world_matrix;
} mesh_t;

void load_mesh(
	char* obj_filename,
	char* png_filename,
	vec3_t scale,
	vec3_t translation,
	vec3_t rotation
);
void load_mesh_obj_data(mesh_t* mesh, char* obj_filename);
void load_mesh_png_data(mesh_t* mesh, char* png_filename);

void mesh_set_scale(mesh_t* mesh, float sx, float sy, float sz);
void mesh_set_translation(mesh_t* mesh, float tx, float ty, float tz);
void mesh_update_world_matrix(mesh_t *mesh);

int get_meshes_count(void);
mesh_t* get_mesh(int index);
void dispose_mesh(int index);
void dispose_meshes(void);

#endif