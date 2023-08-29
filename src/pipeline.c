#include <stdlib.h>
#include "array.h"
#include "pipeline.h"
#include "clipping.h"
#include "display.h"
#include "utils.h"

static vertex_t varying_vertices[3];

void pipeline_draw(
	int camera_type,
	void* camera,
	mesh_t* mesh,
	void (*vertex_shader)(
		int camera_type,
		void* camera,
		mesh_t* mesh,
		vertex_t*
	),
	void (*fragment_shader)(
		int camera_type,
		void* camera,
		mesh_t* mesh,
		int x, int y, float interpolated_reciprocal_w,
		float u, float v,
		float interpolated_x,
		float interpolated_y,
		float interpolated_z
	)
) {
	mesh_update_world_matrix(mesh);
	int num_faces = array_length(mesh->faces);

	perspective_camera_t* persp_camera = NULL;
	orthographic_camera_t* ortho_camera = NULL;

	if (camera_type == PERSPECTIVE_CAMERA) {
		persp_camera = (perspective_camera_t*)camera;
	}
	if (camera_type == ORTHOGRAPHIC_CAMERA) {
		ortho_camera = (orthographic_camera_t*)camera;
	}

	for (int i = 0; i < num_faces; i++) {
		face_t face = mesh->faces[i];
		vec3_t face_vertices[3];
		vec4_t transformed_vertices[3];
		
		face_vertices[0] = mesh->vertices[face.a];
		face_vertices[1] = mesh->vertices[face.b];
		face_vertices[2] = mesh->vertices[face.c];

		for (int j = 0; j < 3; j++) {
			vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);
			transformed_vertices[j] = mat4_mul_vec4(mesh->world_matrix, transformed_vertex);
			
			if (camera_type == PERSPECTIVE_CAMERA) {
				transformed_vertices[j] = mat4_mul_vec4(persp_camera->view_matrix, transformed_vertices[j]);
			} else {
				transformed_vertices[j] = mat4_mul_vec4(ortho_camera->view_matrix, transformed_vertices[j]);
			}
			varying_vertices[j].position = mat4_mul_vec4(mesh->world_matrix, transformed_vertex);
		}

		vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]); /*   A   */
		vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]); /*  / \  */
		vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]); /* C---B */

		vec3_t vector_ab = vec3_sub(vector_b, vector_a);
		vec3_normalize(&vector_ab);
		vec3_t vector_ac = vec3_sub(vector_c, vector_a);
		vec3_normalize(&vector_ac);
		
		vec3_t vector_normal = vec3_cross(vector_ab, vector_ac);
		vec3_normalize(&vector_normal);

		vec3_t origin = { 0, 0, 0 };
		vec3_t camera_ray = vec3_sub(origin, vector_a);

		float dot_normal_camera = vec3_dot(vector_normal, camera_ray);

		if (is_cull_backface()) {
			if (dot_normal_camera < 0) {
				continue;
			}
		}

		polygon_t polygon = create_polygon_from_triangles(
			vec3_from_vec4(transformed_vertices[0]),
			vec3_from_vec4(transformed_vertices[1]),
			vec3_from_vec4(transformed_vertices[2]),
			face.a_uv,
			face.b_uv,
			face.c_uv
		);

		clip_polygon(&polygon);

		// break the clipped polygon apart back to individual triangles
		triangle_t triangles_after_clipping[MAX_NUMBER_POLY_TRIANGLES];
		int num_triangles_after_clipping = 0;

		triangles_from_polygon(&polygon, triangles_after_clipping, &num_triangles_after_clipping);

		// loops all the assembled triangles after clipping
		for (int t = 0; t < num_triangles_after_clipping; t++) {
			triangle_t triangle_to_render = triangles_after_clipping[t];

			for (int j = 0; j < 3; j++) {
				triangle_to_render.vertices[j].color = face.color;

				if (camera_type == PERSPECTIVE_CAMERA) {
					triangle_to_render.vertices[j].position = mat4_mul_vec4_project(
						persp_camera->projection_matrix,
						triangle_to_render.vertices[j].position
					);
				} else {
					triangle_to_render.vertices[j].position = mat4_mul_vec4_project(
						ortho_camera->projection_matrix,
						triangle_to_render.vertices[j].position
					);
				}
				// run vertex shader for each vertice
				vertex_shader(camera_type, camera, mesh, &triangle_to_render.vertices[j]);
			}

			int x0 = triangle_to_render.vertices[0].position.x;
			int y0 = triangle_to_render.vertices[0].position.y;
			float z0 = triangle_to_render.vertices[0].position.z;
			float w0 = triangle_to_render.vertices[0].position.w;
			float u0 = triangle_to_render.vertices[0].uv.u;
			float v0 = triangle_to_render.vertices[0].uv.v;
			
			int x1 = triangle_to_render.vertices[1].position.x;
			int y1 = triangle_to_render.vertices[1].position.y;
			float z1 = triangle_to_render.vertices[1].position.z;
			float w1 = triangle_to_render.vertices[1].position.w;
			float u1 = triangle_to_render.vertices[1].uv.u;
			float v1 = triangle_to_render.vertices[1].uv.v;

			int x2 = triangle_to_render.vertices[2].position.x;
			int y2 = triangle_to_render.vertices[2].position.y;
			float z2 = triangle_to_render.vertices[2].position.z;
			float w2 = triangle_to_render.vertices[2].position.w;
			float u2 = triangle_to_render.vertices[2].uv.u;
			float v2 = triangle_to_render.vertices[2].uv.v;

			if (y0 > y1) {
				int_swap(&y0, &y1);
				int_swap(&x0, &x1);
				float_swap(&u0, &u1);
				float_swap(&v0, &v1);
				float_swap(&z0, &z1);
				float_swap(&w0, &w1);
			}
			if (y1 > y2) {
				int_swap(&y1, &y2);
				int_swap(&x1, &x2);
				float_swap(&u1, &u2);
				float_swap(&v1, &v2);
				float_swap(&z1, &z2);
				float_swap(&w1, &w2);
			}
			// We need to sort y0 and y1 again because y1 might have changed!
			if (y0 > y1) {
				int_swap(&y0, &y1);
				int_swap(&x0, &x1);
				float_swap(&u0, &u1);
				float_swap(&v0, &v1);
				float_swap(&z0, &z1);
				float_swap(&w0, &w1);
			}

			// Flip the V component to account for invertex textures
			v0 = 1 - v0;
			v1 = 1 - v1;
			v2 = 1 - v2;

			vec4_t point_a = { x0, y0, z0, w0 };
			vec4_t point_b = { x1, y1, z1, w1 };
			vec4_t point_c = { x2, y2, z2, w2 };

			tex2_t a_uv = { u0, v0 };
			tex2_t b_uv = { u1, v1 };
			tex2_t c_uv = { u2, v2 };

			float inv_slope1 = 0;
			float inv_slope2 = 0;

			if (y1 - y0 != 0) {
				inv_slope1 = (float)(x1 - x0) / abs(y1 - y0);
			}
			if (y2 - y0 != 0) {
				inv_slope2 = (float)(x2 - x0) / abs(y2 - y0);
			}

			if (y1 - y0 != 0) {
				for (int y = y0; y <= y1; y++) {
					int x_start = (y - y1) * inv_slope1 + x1;
					int x_end = (y - y0) * inv_slope2 + x0;

					// Swap if x_start is to the right of x_end
					if (x_end < x_start) {
						int_swap(&x_start, &x_end);
					}

					for (int x = x_start; x < x_end; x++) {
						vec2_t p = { x, y };
						vec2_t a = vec2_from_vec4(point_a);
						vec2_t b = vec2_from_vec4(point_b);
						vec2_t c = vec2_from_vec4(point_c);
						vec3_t weights = barycentric_weights(a, b, c, p);
						float alpha = weights.x;
						float beta = weights.y;
						float gamma = weights.z;

						float interpolated_reciprocal_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta + (1 / point_c.w) * gamma;

						float interpolated_u = (a_uv.u / point_a.w) * alpha + (b_uv.u / point_b.w) * beta + (c_uv.u / point_c.w) * gamma;
						float interpolated_v = (a_uv.v / point_a.w) * alpha + (b_uv.v / point_b.w) * beta + (c_uv.v / point_c.w) * gamma;
						interpolated_v /= interpolated_reciprocal_w;
						interpolated_u /= interpolated_reciprocal_w;

						float interpolated_x = (varying_vertices[0].position.x) * alpha + (varying_vertices[1].position.x) * beta + (varying_vertices[2].position.x) * gamma;
						float interpolated_y = (varying_vertices[0].position.y) * alpha + (varying_vertices[1].position.y) * beta + (varying_vertices[2].position.y) * gamma;
						float interpolated_z = (varying_vertices[0].position.z) * alpha + (varying_vertices[1].position.z) * beta + (varying_vertices[2].position.z) * gamma;
						// interpolated_x /= interpolated_reciprocal_w;
						// interpolated_y /= interpolated_reciprocal_w;
						// interpolated_z /= interpolated_reciprocal_w;

						interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

						// run fragment shader for each pixel
						fragment_shader(
							camera_type,
							camera,
							mesh,
							x, y,
							interpolated_reciprocal_w,
							interpolated_u,
							interpolated_v,
							interpolated_x,
							interpolated_y,
							interpolated_z
						);
					}
				}
			}

			inv_slope1 = 0;
			inv_slope2 = 0;

			if (y2 - y1 != 0) {
				inv_slope1 = (float)(x2 - x1) / abs(y2 - y1);
			}
			if (y2 - y0 != 0) {
				inv_slope2 = (float)(x2 - x0) / abs(y2 - y0);
			}

			if (y2 - y1 != 0) {
				for (int y = y1; y <= y2; y++) {
					int x_start = (y - y1) * inv_slope1 + x1;
					int x_end = (y - y0) * inv_slope2 + x0;


					// Swap if x_start is to the right of x_end
					if (x_end < x_start) {
						int_swap(&x_start, &x_end);
					}

					for (int x = x_start; x < x_end; x++) {
						vec2_t p = { x, y };
						vec2_t a = vec2_from_vec4(point_a);
						vec2_t b = vec2_from_vec4(point_b);
						vec2_t c = vec2_from_vec4(point_c);
						vec3_t weights = barycentric_weights(a, b, c, p);
						float alpha = weights.x;
						float beta = weights.y;
						float gamma = weights.z;
						
						float interpolated_reciprocal_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta + (1 / point_c.w) * gamma;

						float interpolated_u = (a_uv.u / point_a.w) * alpha + (b_uv.u / point_b.w) * beta + (c_uv.u / point_c.w) * gamma;
						float interpolated_v = (a_uv.v / point_a.w) * alpha + (b_uv.v / point_b.w) * beta + (c_uv.v / point_c.w) * gamma;
						
						interpolated_v /= interpolated_reciprocal_w;
						interpolated_u /= interpolated_reciprocal_w;

						float interpolated_x = (varying_vertices[0].position.x) * alpha + (varying_vertices[1].position.x) * beta + (varying_vertices[2].position.x) * gamma;
						float interpolated_y = (varying_vertices[0].position.y) * alpha + (varying_vertices[1].position.y) * beta + (varying_vertices[2].position.y) * gamma;
						float interpolated_z = (varying_vertices[0].position.z) * alpha + (varying_vertices[1].position.z) * beta + (varying_vertices[2].position.z) * gamma;

						interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

						// run fragment shader for each pixel
						fragment_shader(
							camera_type,
							camera,
							mesh,
							x, y,
							interpolated_reciprocal_w,
							interpolated_u,
							interpolated_v,
							interpolated_x,
							interpolated_y,
							interpolated_z
						);
					}
				}
			}
			
		}
	}
}
