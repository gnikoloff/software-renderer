#include <assert.h>
#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "mesh.h"
#include "array.h"
#include "geometry.h"

#define MAX_NUM_MESHES 20

static mesh_t meshes[MAX_NUM_MESHES];
static int mesh_count = 0;

mesh_t* make_plane(
	float width,
	float height,
	int width_segments,
	int height_segments
) {
	printf("Create mesh %d\n", mesh_count);
	assert(mesh_count < MAX_NUM_MESHES);
	mesh_t *mesh = &meshes[mesh_count];

	make_plane_geometry(mesh, width, height, width_segments, height_segments);
	init_mesh_common_properties(mesh);
	mesh->idx = mesh_count;

	mesh_count++;

	return mesh;
}

mesh_t* make_sphere(
	float radius,
	int width_segments,
	int height_segments,
	float phi_start,
	float phi_length,
	float theta_start,
	float theta_length
) {
	printf("Create mesh %d\n", mesh_count);
	assert(mesh_count < MAX_NUM_MESHES);
	mesh_t *mesh = &meshes[mesh_count];
    
	make_sphere_geometry(
		mesh,
		radius,
		width_segments,
		height_segments,
		phi_start,
		phi_length,
		theta_start,
		theta_length
	);
	init_mesh_common_properties(mesh);

	mesh_count++;

	return mesh;
}

mesh_t* make_box(
	float width,
	float height,
	float depth,
	int width_segments,
	int height_segments,
	int depth_segments
) {
	printf("Create mesh %d\n", mesh_count);
	assert(mesh_count < MAX_NUM_MESHES);
	mesh_t *mesh = &meshes[mesh_count];

	make_box_geometry(
		mesh,
		width,
		height,
		depth,
		width_segments,
		height_segments,
		depth_segments
	);
	init_mesh_common_properties(mesh);

	mesh_count++;

	return mesh;
}

mesh_t* make_ring(
	float inner_radius,
	float outer_radius,
	int theta_segments,
	int phi_segments,
	float theta_start,
	float theta_length
) {
	printf("Create mesh %d\n", mesh_count);
	assert(mesh_count < MAX_NUM_MESHES);
	mesh_t *mesh = &meshes[mesh_count];

	make_ring_geometry(
		mesh,
		inner_radius,
		outer_radius,
		theta_segments,
		phi_segments,
		theta_start,
		theta_length
	);
	init_mesh_common_properties(mesh);

	mesh_count++;

	return mesh;
}

mesh_t* make_torus(
	float radius,
	float tube,
	int radial_segments,
	int tubular_segments,
	float arc
) {
	printf("Create mesh %d\n", mesh_count);
	assert(mesh_count < MAX_NUM_MESHES);
	mesh_t *mesh = &meshes[mesh_count];

	make_torus_geometry(
		mesh,
		radius,
		tube,
		radial_segments,
		tubular_segments,
		arc
	);
	init_mesh_common_properties(mesh);

	mesh_count++;
	return mesh;
}

void mesh_update_world_matrix(mesh_t *mesh) {
	mesh->scale_matrix = mat4_make_scale(
		mesh->scale.x,
		mesh->scale.y,
		mesh->scale.z
	);
	mesh->translation_matrix = mat4_make_translation(
		mesh->translation.x,
		mesh->translation.y,
		mesh->translation.z
	);
	quat_identity(&mesh->quaternion);
	mesh->quaternion = quat_rotate_x(mesh->quaternion, mesh->rotation.x);
	mesh->quaternion = quat_rotate_y(mesh->quaternion, mesh->rotation.y);
	mesh->quaternion = quat_rotate_z(mesh->quaternion, mesh->rotation.z);
	mesh->rotation_matrix = quat_get_rotation_matrix(mesh->quaternion);

	mesh->world_matrix = mat4_identity();
	mesh->world_matrix = mat4_mul_mat4(mesh->scale_matrix, mesh->world_matrix);
	mesh->world_matrix = mat4_mul_mat4(mesh->rotation_matrix, mesh->world_matrix);
	mesh->world_matrix = mat4_mul_mat4(mesh->translation_matrix, mesh->world_matrix);

}

mesh_t* load_mesh(
	char* obj_filename,
	char* png_filename,
	vec3_t scale,
	vec3_t translation,
	vec3_t rotation
) {
	printf("Create mesh %d\n", mesh_count);
	assert(mesh_count < MAX_NUM_MESHES);
	mesh_t *mesh = &meshes[mesh_count];
	load_mesh_obj_data(mesh, obj_filename);
	load_mesh_png_data(mesh, png_filename);
	init_mesh_common_properties(mesh);

	mesh_count++;

	return mesh;
}

int get_meshes_count(void) {
	return mesh_count;
}

mesh_t* get_mesh(int index) {
	return &meshes[index];
}

void dispose_mesh(mesh_t* mesh) {
	array_free(mesh->vertices);
	array_free(mesh->faces);
	upng_free(mesh->texture);
}

void dispose_meshes(void) {
	for (int i = 0; i < mesh_count; i++) {
		mesh_t* mesh = &meshes[i];
		dispose_mesh(mesh);
	}
}

void load_mesh_obj_data(mesh_t* mesh, char* obj_filename) {
	FILE* file;
	file = fopen(obj_filename, "r");

	char line[255];

	tex2_t* texcoords = NULL;
	int vertices_count = 0;

	while (fgets(line, 255, file)) {
		// Vertex information
		if (strncmp(line, "v ", 2) == 0) {
			vec3_t vertex;
			sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
			array_push(mesh->vertices, vertex);
			vertices_count++;
		}

		// Texture coordinate information
		if (strncmp(line, "vt ", 3) == 0) {
			tex2_t texcoord;
			sscanf(line, "vt %f %f", &texcoord.u, &texcoord.v);
			array_push(texcoords, texcoord);
		}

		// Normal
		if (strncmp(line, "vn ", 3) == 0) {
			vec3_t normal;
			sscanf(line, "vn %f %f %f", &normal.x, &normal.y, &normal.z);
			array_push(mesh->normals, normal);
		}

		// Face information
		if (strncmp(line, "f ", 2) == 0) {
			int vertex_indices[3];
			int uv_indices[3];
			int normal_indices[3];
			sscanf(
					line,
					"f %d/%d/%d %d/%d/%d %d/%d/%d",
					&vertex_indices[0], &uv_indices[0], &normal_indices[0],
					&vertex_indices[1], &uv_indices[1], &normal_indices[1],
					&vertex_indices[2], &uv_indices[2], &normal_indices[2]
			);

			face_t face;

			face.a = vertex_indices[0] - 1;
			face.b = vertex_indices[1] - 1;
			face.c = vertex_indices[2] - 1;
			face.a_uv = texcoords[uv_indices[0] - 1];
			face.b_uv = texcoords[uv_indices[1] - 1];
			face.c_uv = texcoords[uv_indices[2] - 1];
			face.color = MESH_DEBUG_COLOR;

			// printf("%i %i %i\n", face.a, face.b, face.c);

			array_push(mesh->faces, face);
		}
	}

	mesh->vertices_count = vertices_count;

	array_free(texcoords);
}

void load_mesh_png_data(mesh_t* mesh, char* png_filename) {
	mesh->texture = load_png_data(png_filename);
}

void init_mesh_common_properties(mesh_t* mesh) {
	mesh->translation.x = 0;
	mesh->translation.y = 0;
	mesh->translation.z = 0;

	mesh->rotation.x = 0;
	mesh->rotation.y = 0;
	mesh->rotation.z = 0;

	mesh->quaternion = quat_from_vec3(mesh->rotation, 1);

	mesh->scale.x = 1;
	mesh->scale.y = 1;
	mesh->scale.z = 1;
}
