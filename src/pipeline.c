#include "stdio.h"
#include "pipeline.h"
#include "array.h"
#include "triangle.h"
#include "clipping.h"
#include "display.h"

#define MAX_TRIANGLES_PER_MESH 10000
static triangle_t triangles_to_render[MAX_TRIANGLES_PER_MESH];
static int num_triangles_to_render = 0;

void render_mesh(mesh_t* mesh, camera_t* camera, void (*render_callback)(triangle_t*)) {
	mesh_update_world_matrix(mesh);
	int num_faces = array_length(mesh->faces);

	num_triangles_to_render = 0;

	for (int i = 0; i < num_faces; i++) {
		face_t face = mesh->faces[i];
		vec3_t face_vertices[3];
		face_vertices[0] = mesh->vertices[face.a];
		face_vertices[1] = mesh->vertices[face.b];
		face_vertices[2] = mesh->vertices[face.c];
		vec4_t transformed_vertices[3];

		for (int j = 0; j < 3; j++) {
			vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);
			transformed_vertices[j] = mat4_mul_vec4(mesh->world_matrix, transformed_vertex);
			transformed_vertices[j] = mat4_mul_vec4(camera->view_matrix, transformed_vertices[j]);
		}

		// Second we perform backface culling and disregard backfacing triangles
		vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]); /*   A   */
		vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]); /*  / \  */
		vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]); /* C---B */

		// Get the vector subtraction of B-A and C-a
		vec3_t vector_ab = vec3_sub(vector_b, vector_a);
		vec3_normalize(&vector_ab);
		vec3_t vector_ac = vec3_sub(vector_c, vector_a);
		vec3_normalize(&vector_ac);
		
		// Compute the face normal using cross product
		vec3_t vector_normal = vec3_cross(vector_ab, vector_ac);
		vec3_normalize(&vector_normal);

		// Find the vector between a point in the triangle and the camera origin
		// Our camera is always at origin (0 0 0) (camera space)
		vec3_t origin = { 0, 0, 0 };
		vec3_t camera_ray = vec3_sub(origin, vector_a);

		// Calculate how aligned the camera_ray is with the face normal using dot product
		float dot_normal_camera = vec3_dot(vector_normal, camera_ray);

		if (is_cull_backface()) {
			if (dot_normal_camera < 0) {
				continue;
			}
		}

		// Create a polygon from the original transformed triangle to be clipped
		polygon_t polygon = create_polygon_from_triangles(
			vec3_from_vec4(transformed_vertices[0]),
			vec3_from_vec4(transformed_vertices[1]),
			vec3_from_vec4(transformed_vertices[2]),
			face.a_uv,
			face.b_uv,
			face.c_uv
		);

		// Clips the polygon and returns a new polygon with potentially new vertices
		clip_polygon(&polygon);

		// Break the clipped polygon apart back to individual triangles
		triangle_t triangles_after_clipping[MAX_NUMBER_POLY_TRIANGLES];
		int num_triangles_after_clipping = 0;

		triangles_from_polygon(&polygon, triangles_after_clipping, &num_triangles_after_clipping);

		// Loops all the assembled triangles after clipping
		for (int t = 0; t < num_triangles_after_clipping; t++) {
			triangle_t triangle_after_clipping = triangles_after_clipping[t];

			vec4_t projected_points[3];

			for (int j = 0; j < 3; j++) {
				// Multiply point with projection matrix and perspective divide!
				projected_points[j] = mat4_mul_vec4_project(camera->projection_matrix, triangle_after_clipping.points[j]);

				float half_window_width = get_window_width() / 2.0;
				float half_window_height = get_window_height() / 2.0;

				// AT THIS POINT projected_point[j] is in NDC!!!!

				// We need to scale it to match the size of the screen!!!!
				// and translate it to the center of the screen!!!!!

				// Scale into the view
				projected_points[j].x *= half_window_width;
				projected_points[j].y *= half_window_height;

				// Invert the y values to account for flipped y screen coordinate
				projected_points[j].y *= -1;

				// Translate the projected points to the middle of the screen
				projected_points[j].x += half_window_width;
				projected_points[j].y += half_window_height;
			}

			// Calculate the shade intensity based on how aligned the face normal is with the light direction
			// float light_intensity_factor = -vec3_dot(vector_normal, get_light_direction());

			// Calculate the triangle color based on the light angle
			uint32_t triangle_color = face.color;

			// triangle_color = light_apply_intensity(triangle_color, light_intensity_factor);

			triangle_t triangle_to_render = {
				.points = {
					{ projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w },
					{ projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w },
					{ projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w }
				},
				.color = triangle_color,
				.tex_coords = {
					{ triangle_after_clipping.tex_coords[0].u, triangle_after_clipping.tex_coords[0].v },
					{ triangle_after_clipping.tex_coords[1].u, triangle_after_clipping.tex_coords[1].v },
					{ triangle_after_clipping.tex_coords[2].u, triangle_after_clipping.tex_coords[2].v },
				},
				.texture = mesh->texture
			};

			if (num_triangles_to_render < MAX_TRIANGLES_PER_MESH) {
				triangles_to_render[num_triangles_to_render++] = triangle_to_render;
			}
		}
	}	

	for (int i = 0; i < num_triangles_to_render; i++) {
		triangle_t triangle = triangles_to_render[i];
		render_callback(&triangle);		
	}
}
