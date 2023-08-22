#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"
#include "upng.h"

typedef struct {
	vec3_t* vertices;   // mesh dynamic array of vertices
	face_t* faces;		  // mesh dynamic array of faces
	upng_t* texture;    // mesh PNG texture pointer
	vec3_t rotation;    // mesh rotation xyz
	vec3_t scale;       // mesh scale xyz
	vec3_t translation; // mesh translation xyz
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

int get_meshes_count(void);
mesh_t* get_mesh(int index);
void dispose_mesh(int index);
void dispose_meshes(void);

#endif