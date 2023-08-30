// ported from three.js
// https://github.com/mrdoob/three.js/tree/master/src/geometries

#include <stdlib.h>
#include "stdio.h"
#include "math.h"
#include "array.h"
#include "geometry.h"
#include "utils.h"

void make_plane_geometry(
	mesh_t *mesh,
	float width,
	float height,
	int width_segments,
	int height_segments
) {
	float half_width = width / 2;
	float half_height = height / 2;
	int grid_x1 = width_segments + 1;
	int grid_y1 = height_segments + 1;
	float segment_width = width / (float)width_segments;
	float segment_height = height / (float)height_segments;
	
	vec3_t vertex;
	tex2_t uv;

	tex2_t *texcoords = NULL;

	int vertices_count = 0;

	for (int iy = 0; iy < grid_y1; iy++) {
		float y = (float)iy * segment_height - half_height;
		for (int ix = 0; ix < grid_x1; ix++) {
			float x = (float)ix * segment_width - half_width;
			vertex.x = -x;
			vertex.y = -y;
			vertex.z = 0;
			array_push(mesh->vertices, vertex);
			vertices_count++;

			uv.u = 1 - (float)ix / width_segments;
			uv.v = 1 - (float)iy / height_segments;
			array_push(texcoords, uv);
		}
	}

	for (int iy = 0; iy < height_segments; iy++) {
		for (int ix = 0; ix < width_segments; ix++) {

			int a = ix + grid_x1 * iy;
			int b = ix + grid_x1 * (iy + 1);
			int c = (ix + 1) + grid_x1 * (iy + 1);
			int d = (ix + 1) + grid_x1 * iy;

			face_t face_a = {
				.a = a,
				.b = b,
				.c = d,
				.a_uv = texcoords[a],
				.b_uv = texcoords[b],
				.c_uv = texcoords[d],
				.color = MESH_DEBUG_COLOR
			};
			array_push(mesh->faces, face_a);

			face_t face_b = {
				.a = b,
				.b = c,
				.c = d,
				.a_uv = texcoords[b],
				.b_uv = texcoords[c],
				.c_uv = texcoords[d],
				.color = MESH_DEBUG_COLOR
			};
			array_push(mesh->faces, face_b);
		}

	}

	mesh->vertices_count = vertices_count;

	array_free(texcoords);
}

void make_sphere_geometry(
	mesh_t *mesh,
	float radius,
	int width_segments,
	int height_segments,
	float phi_start,
	float phi_length,
	float theta_start,
	float theta_length
) {
	width_segments = MAX(3, width_segments);
	height_segments = MAX(2, height_segments);

	float theta_end = MIN(theta_start + theta_length, M_PI);
	int index = 0;
	int grid[height_segments + 1][width_segments + 1];

	vec3_t* vertices = NULL;
	tex2_t* texcoords = NULL;

	tex2_t uv;

	for (int iy = 0; iy <= height_segments; iy++) {
		float v = (float)iy / (float)height_segments;
		float u_offset = 0;

		if (iy == 0 && theta_start == 0) {
			u_offset = 0.5 / (float)(width_segments);
		} else if (iy == height_segments && M_PI - theta_end < 0.001) {
			u_offset = -0.5 / (float)(width_segments);
		}

		for (int ix = 0; ix <= width_segments; ix++) {
			float u = (float)ix / (float)width_segments;
			vec3_t vertex;
			vertex.x = -radius * cos(phi_start + u * phi_length) * sin(theta_start + v * theta_length);
			vertex.y = radius * cos(theta_start + v * theta_length);
			vertex.z = radius * sin(phi_start + u * phi_length) * sin(theta_start + v * theta_length);
			array_push(vertices, vertex);

			uv.u = 1 - (u + u_offset);
			uv.v = 1 - v;
			array_push(texcoords, uv);

			grid[iy][ix] = index++;
		}

	}

	for (int iy = 0; iy < height_segments; iy++) {
		for (int ix = 0; ix < width_segments; ix++) {

			int a = grid[iy][ix + 1];
			int b = grid[iy][ix];
			int c = grid[iy + 1][ix];
			int d = grid[iy + 1][ix + 1];

			face_t face;

			if (iy != 0 || theta_start > 0) {
				face.a = a;
				face.b = b;
				face.c = d;
				face.a_uv = texcoords[a];
				face.b_uv = texcoords[b];
				face.c_uv = texcoords[d];
				face.color = MESH_DEBUG_COLOR;
				array_push(mesh->faces, face);
			}

			if (iy != height_segments - 1 || theta_end < M_PI) {
				face.a = b;
				face.b = c;
				face.c = d;
				face.a_uv = texcoords[b];
				face.b_uv = texcoords[c];
				face.c_uv = texcoords[d];
				face.color = MESH_DEBUG_COLOR;
				array_push(mesh->faces, face);
			}
		}
	}

	mesh->vertices = vertices;
	array_free(texcoords);
}

typedef enum {
	ZYX,
	XZY,
	XYZ
} plane_xyz_arrangement;

void build_plane(
	int xyz_arrangement,
	float udir,
	float vdir,
	float width,
	float height,
	float depth,
	int grid_x,
	int grid_y,
	int* vertices_count,
	mesh_t* mesh
) {
	float segment_width = width / (float)grid_x;
	float segment_height = height / (float)grid_y;

	float half_width = width / 2;
	float half_height = height / 2;
	float half_depth = depth / 2;

	int grid_x1 = grid_x + 1;
	int grid_y1 = grid_y + 1;
	int vertex_counter = 0;

	vec3_t vertex;
	tex2_t uv;
	
	tex2_t* texcoords = NULL;

	for (int iy = 0; iy < grid_y1; iy++) {
		
		float y = (float)iy * segment_height - half_height;

		for (int ix = 0; ix < grid_x1; ix++) {
			float x = (float)ix * segment_width - half_width;
			if (xyz_arrangement == ZYX) {
				vertex.z = x * udir;
				vertex.y = y * vdir;
				vertex.x = half_depth;
			} else if (xyz_arrangement == XZY) {
				vertex.x = x * udir;
				vertex.z = y * vdir;
				vertex.y = half_depth;
			} else if (xyz_arrangement == XYZ) {
				vertex.x = x * udir;
				vertex.y = y * vdir;
				vertex.z = half_depth;
			}
			array_push(mesh->vertices, vertex);
			
			uv.u = 1 - (float)ix / (float)grid_x;
			uv.v = 1 - ((float)iy / (float)grid_y);

			array_push(texcoords, uv);

			vertex_counter++;

		}
	}

	for (int iy = 0; iy < grid_y; iy++) {

		for (int ix = 0; ix < grid_x; ix++) {
			int a = ix + grid_x1 * iy;
			int b = ix + grid_x1 * (iy + 1);
			int c = (ix + 1) + grid_x1 * (iy + 1);
			int d = (ix + 1) + grid_x1 * iy;

			// faces

			face_t face_a = {
				.a = *vertices_count + a,
				.b = *vertices_count + b,
				.c = *vertices_count + d,
				.a_uv = texcoords[a],
				.b_uv = texcoords[b],
				.c_uv = texcoords[d],
				.color = MESH_DEBUG_COLOR
			};
			array_push(mesh->faces, face_a);

			face_t face_b = {
				.a = *vertices_count + b,
				.b = *vertices_count + c,
				.c = *vertices_count + d,
				.a_uv = texcoords[b],
				.b_uv = texcoords[c],
				.c_uv = texcoords[d],
				.color = MESH_DEBUG_COLOR
			};
			array_push(mesh->faces, face_b);

		}
	}

	*vertices_count += vertex_counter;
	mesh->vertices_count = vertex_counter;

	array_free(texcoords);

}

void make_box_geometry(
	mesh_t* mesh,
	float width,
	float height,
	float depth,
	int width_segments,
	int height_segments,
	int depth_segments
) {
	int vertices_count = 0;
	build_plane(ZYX, -1, -1, depth, height, width, depth_segments, height_segments, &vertices_count, mesh);
	build_plane(ZYX, 1, -1, depth, height, - width, depth_segments, height_segments, &vertices_count, mesh);
	build_plane(XZY, 1, 1, width, depth, height, width_segments, depth_segments, &vertices_count, mesh);
	build_plane(XZY, 1, -1, width, depth, - height, width_segments, depth_segments, &vertices_count, mesh);
	build_plane(XYZ, 1, -1, width, height, depth, width_segments, height_segments, &vertices_count, mesh);
	build_plane(XYZ, -1, -1, width, height, - depth, width_segments, height_segments, &vertices_count, mesh);
	mesh->vertices_count = vertices_count;
}

void make_ring_geometry(
	mesh_t* mesh,
	float inner_radius,
	float outer_radius,
	int theta_segments,
	int phi_segments,
	float theta_start,
	float theta_length
) {
	tex2_t* texcoords = NULL;

	theta_segments = MAX(3, theta_segments);
	phi_segments = MAX(1, phi_segments);

	float radius = inner_radius;
	float radius_step = (outer_radius - inner_radius) / (float)phi_segments;

	vec3_t vertex;
	tex2_t uv;

	int vertices_count = 0;

	for (int j = 0; j <= phi_segments; j++) {
		for (int i = 0; i <= theta_segments; i++) {
			float segment = theta_start + ((float)i / (float)theta_segments) * theta_length;
			vertex.x = -radius * cos(segment);
			vertex.y = radius * sin(segment);
			vertex.z = 0;

			array_push(mesh->vertices, vertex);
			vertices_count++;

			uv.u = (vertex.x / outer_radius + 1) / 2;
			uv.v = (vertex.y / outer_radius + 1) / 2;
			array_push(texcoords, uv);
			printf("UV: %f %f\n", uv.u, uv.v);
		}
		radius += radius_step;
	}

	for (int j = 0; j < phi_segments; j++) {
		int theta_segment_level = j * (theta_segments + 1);

		for (int i = 0; i < theta_segments; i++) {
			int segment = i + theta_segment_level;
			int a = segment;
			int b = segment + theta_segments + 1;
			int c = segment + theta_segments + 2;
			int d = segment + 1;

			face_t face_a = {
				.a = a,
				.b = b,
				.c = d,
				.a_uv = texcoords[a],
				.b_uv = texcoords[b],
				.c_uv = texcoords[d],
				.color = MESH_DEBUG_COLOR
			};
			array_push(mesh->faces, face_a);

			face_t face_b = {
				.a = b,
				.b = c,
				.c = d,
				.a_uv = texcoords[b],
				.b_uv = texcoords[c],
				.c_uv = texcoords[d],
				.color = MESH_DEBUG_COLOR
			};
			array_push(mesh->faces, face_b);
		}
	}
	mesh->vertices_count = vertices_count;
	array_free(texcoords);
}

void make_torus_geometry(
	mesh_t* mesh,
	float radius,
	float tube,
	int radial_segments,
	int tubular_segments,
	float arc
) {
	tex2_t *texcoords = NULL;

	vec3_t vertex;
	tex2_t uv;

	int vertices_count = 0;

	for (int j = 0; j <= radial_segments; j++) {
		for (int i = 0; i <= tubular_segments; i++) {
			float u = (float)i / (float)tubular_segments * arc;
			float v = (float)j / (float)radial_segments * M_PI * 2;

			vertex.x = (radius + tube * cos(v)) * cos(u);
			vertex.y = (radius + tube * cos(v)) * sin(u);
			vertex.z = tube * sin(v);

			array_push(mesh->vertices, vertex);
			vertices_count++;

			uv.u = 1 - i / (float)tubular_segments;
			uv.v = j / (float)radial_segments;
			array_push(texcoords, uv);
		}
	}

	for (int j = 1; j <= radial_segments; j++) {
		for (int i = 1; i <= tubular_segments; i++) {
			int a = (tubular_segments + 1) * j + i - 1;
			int b = (tubular_segments + 1) * (j - 1) + i - 1;
			int c = (tubular_segments + 1) * (j - 1) + i;
			int d = (tubular_segments + 1) * j + i;

			face_t face_a = {
				.a = a,
				.b = b,
				.c = d,
				.a_uv = texcoords[a],
				.b_uv = texcoords[b],
				.c_uv = texcoords[d],
				.color = MESH_DEBUG_COLOR
			};
			array_push(mesh->faces, face_a);

			face_t face_b = {
				.a = b,
				.b = c,
				.c = d,
				.a_uv = texcoords[b],
				.b_uv = texcoords[c],
				.c_uv = texcoords[d],
				.color = MESH_DEBUG_COLOR
			};
			array_push(mesh->faces, face_b);
		}

	}

	mesh->vertices_count = vertices_count;

}