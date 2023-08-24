#include <assert.h>
#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include "mesh.h"
#include "array.h"

#define MAX_NUM_MESHES 10

static mesh_t meshes[MAX_NUM_MESHES];
static int mesh_count = 0;

void mesh_set_scale(mesh_t *mesh, float sx, float sy, float sz) {
    mesh->scale.x = sx;
    mesh->scale.y = sy;
    mesh->scale.z = sz;
    mesh->scale_matrix = mat4_make_scale(sx, sy, sz);
}

void mesh_set_translation(mesh_t *mesh, float tx, float ty, float tz) {
    mesh->translation.x = tx;
    mesh->translation.y = ty;
    mesh->translation.z = tz;
    mesh->translation_matrix = mat4_make_translation(tx, ty, tz);
}

void mesh_update_world_matrix(mesh_t *mesh) {
    mesh->world_matrix = mat4_identity();
    mesh->world_matrix = mat4_mul_mat4(mesh->scale_matrix, mesh->world_matrix);
    // mat4_mul_mat4(&mesh->world_matrix, &mesh->rotation_matrix_x, &mesh->world_matrix);
    // mat4_mul_mat4(&mesh->world_matrix, &mesh->rotation_matrix_y, &mesh->world_matrix);
    // mat4_mul_mat4(&mesh->world_matrix, &mesh->rotation_matrix_z, &mesh->world_matrix);
    mesh->world_matrix = mat4_mul_mat4(mesh->translation_matrix, mesh->world_matrix);
}

void load_mesh(
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

    mesh->rotation = rotation;

    mesh_set_scale(mesh, scale.x, scale.y, scale.z);
    mesh_set_translation(mesh, translation.x, translation.y, translation.z);

    mesh_count++;
}

int get_meshes_count(void) {
    return mesh_count;
}

mesh_t* get_mesh(int index) {
    return &meshes[index];
}

void dispose_mesh(int index) {
    mesh_t* mesh = &meshes[index];
	array_free(mesh->vertices);
	array_free(mesh->faces);
	upng_free(mesh->texture);
}

void dispose_meshes(void) {
    for (int i = 0; i < mesh_count; i++) {
        dispose_mesh(i);
    }
}

void load_mesh_obj_data(mesh_t* mesh, char* obj_filename) {
    FILE* file;
    file = fopen(obj_filename, "r");

    char line[255];

    tex2_t* texcoords = NULL;

    while (fgets(line, 255, file)) {
        // Vertex information
        if (strncmp(line, "v ", 2) == 0) {
            vec3_t vertex;
            sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            array_push(mesh->vertices, vertex);
        }

        // Texture coordinate information
        if (strncmp(line, "vt ", 3) == 0) {
            tex2_t texcoord;
            sscanf(line, "vt %f %f", &texcoord.u, &texcoord.v);
            array_push(texcoords, texcoord);
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

            // printf("%i %i %i\n", face.a, face.b, face.c);

            array_push(mesh->faces, face);
        }
    }

    array_free(texcoords);

}

void load_mesh_png_data(mesh_t* mesh, char* png_filename) {
    upng_t* png_image = upng_new_from_file(png_filename);
    if (png_image != NULL) {
        upng_decode(png_image);
        if (upng_get_error(png_image) == UPNG_EOK) {
            mesh->texture = png_image;
        }
    }
}
