#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include "mesh.h"
#include "array.h"

mesh_t mesh = {
    .vertices = NULL,
    .faces = NULL,
    .rotation = { 0, 0, 0 },
    .scale = { 1, 1, 1 },
    .translation = { 0, 0, 0 }
};

vec3_t cube_vertices[N_CUBE_VERTICES] = {
    { .x = -1, .y = -1, .z = -1 }, // 1
    { .x = -1, .y =  1, .z = -1 }, // 2
    { .x =  1, .y =  1, .z = -1 }, // 3
    { .x =  1, .y = -1, .z = -1 }, // 4
    { .x =  1, .y =  1, .z =  1 }, // 5
    { .x =  1, .y = -1, .z =  1 }, // 6
    { .x = -1, .y =  1, .z =  1 }, // 7
    { .x = -1, .y = -1, .z =  1 }  // 8
};

face_t cube_faces[N_CUBE_FACES] = {
    // front
    { .a = 1, .b = 2, .c = 3, .color = 0xFFFFFFFF },
    { .a = 1, .b = 3, .c = 4, .color = 0xFFFFFFFF },
    // right
    { .a = 4, .b = 3, .c = 5, .color = 0xFFFFFFFF },
    { .a = 4, .b = 5, .c = 6, .color = 0xFFFFFFFF },
    // back
    { .a = 6, .b = 5, .c = 7, .color = 0xFFFFFFFF },
    { .a = 6, .b = 7, .c = 8, .color = 0xFFFFFFFF },
    // left
    { .a = 8, .b = 7, .c = 2, .color = 0xFFFFFFFF },
    { .a = 8, .b = 2, .c = 1, .color = 0xFFFFFFFF },
    // top
    { .a = 2, .b = 7, .c = 5, .color = 0xFFFFFFFF },
    { .a = 2, .b = 5, .c = 3, .color = 0xFFFFFFFF },
    // bottom
    { .a = 6, .b = 8, .c = 1, .color = 0xFFFFFFFF },
    { .a = 6, .b = 1, .c = 4, .color = 0xFFFFFFFF }
};

void load_cube_mesh_data(void) {
    for (int i = 0; i < N_CUBE_VERTICES; i++) {
        array_push(mesh.vertices, cube_vertices[i]);
    }
    for (int i = 0; i < N_CUBE_FACES; i++) {
        array_push(mesh.faces, cube_faces[i]);
    }
}

void load_obj_file_data(char* filename) {
    FILE* file;
    file = fopen(filename, "r");

    char line[255];

    while (fgets(line, 255, file)) {
        // Vertex information
        if (strncmp(line, "v ", 2) == 0) {
            vec3_t vertex;
            sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            // printf("%f %f %f\n", vertex.x, vertex.y, vertex.z);
            array_push(mesh.vertices, vertex);
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

            face.a = vertex_indices[0];
            face.b = vertex_indices[1];
            face.c = vertex_indices[2];

            // printf("%i %i %i\n", face.a, face.b, face.c);

            array_push(mesh.faces, face);
        }
    }
}
