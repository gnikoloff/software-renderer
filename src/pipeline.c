#include <stdlib.h>
#include "array.h"
#include "pipeline.h"
#include "clipping.h"
#include "display.h"
#include "utils.h"
#include "light.h"

static vertex_t varying_world_vertices[3];

void render_triangle(
	triangle_t* triangle_to_render,
	int camera_type,
	void* camera,
	mesh_t* mesh,
	fragment_shader_callback fs_shader
) {
	int x0 = triangle_to_render->vertices[0].position.x;
	int y0 = triangle_to_render->vertices[0].position.y;
	float z0 = triangle_to_render->vertices[0].position.z;
	float w0 = triangle_to_render->vertices[0].position.w;
	float n0x = triangle_to_render->vertices[0].normal.x;
	float n0y = triangle_to_render->vertices[0].normal.y;
	float n0z = triangle_to_render->vertices[0].normal.z;
	float u0 = triangle_to_render->vertices[0].uv.u;
	float v0 = triangle_to_render->vertices[0].uv.v;
	float model_x0 = triangle_to_render->vertices[0].world_space_position.x;
	float model_y0 = triangle_to_render->vertices[0].world_space_position.y;
	float model_z0 = triangle_to_render->vertices[0].world_space_position.z;
	float model_w0 = triangle_to_render->vertices[0].world_space_position.w;
	
	int x1 = triangle_to_render->vertices[1].position.x;
	int y1 = triangle_to_render->vertices[1].position.y;
	float z1 = triangle_to_render->vertices[1].position.z;
	float w1 = triangle_to_render->vertices[1].position.w;
	float n1x = triangle_to_render->vertices[1].normal.x;
	float n1y = triangle_to_render->vertices[1].normal.y;
	float n1z = triangle_to_render->vertices[1].normal.z;
	float u1 = triangle_to_render->vertices[1].uv.u;
	float v1 = triangle_to_render->vertices[1].uv.v;
	float model_x1 = triangle_to_render->vertices[1].world_space_position.x;
	float model_y1 = triangle_to_render->vertices[1].world_space_position.y;
	float model_z1 = triangle_to_render->vertices[1].world_space_position.z;
	float model_w1 = triangle_to_render->vertices[1].world_space_position.w;

	int x2 = triangle_to_render->vertices[2].position.x;
	int y2 = triangle_to_render->vertices[2].position.y;
	float z2 = triangle_to_render->vertices[2].position.z;
	float w2 = triangle_to_render->vertices[2].position.w;
	float n2x = triangle_to_render->vertices[2].normal.x;
	float n2y = triangle_to_render->vertices[2].normal.y;
	float n2z = triangle_to_render->vertices[2].normal.z;
	float u2 = triangle_to_render->vertices[2].uv.u;
	float v2 = triangle_to_render->vertices[2].uv.v;
	float model_x2 = triangle_to_render->vertices[2].world_space_position.x;
	float model_y2 = triangle_to_render->vertices[2].world_space_position.y;
	float model_z2 = triangle_to_render->vertices[2].world_space_position.z;
	float model_w2 = triangle_to_render->vertices[2].world_space_position.w;

	if (y0 > y1) {
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
		float_swap(&z0, &z1);
		float_swap(&w0, &w1);
		float_swap(&n0x, &n1x);
		float_swap(&n0y, &n1y);
		float_swap(&n0z, &n1z);
		float_swap(&u0, &u1);
		float_swap(&v0, &v1);
		float_swap(&model_x0, &model_x1);
		float_swap(&model_y0, &model_y1);
		float_swap(&model_z0, &model_z1);
		float_swap(&model_w0, &model_w1);
	}
	if (y1 > y2) {
		int_swap(&y1, &y2);
		int_swap(&x1, &x2);
		float_swap(&z1, &z2);
		float_swap(&w1, &w2);
		float_swap(&n1x, &n2x);
		float_swap(&n1y, &n2y);
		float_swap(&n1z, &n2z);
		float_swap(&u1, &u2);
		float_swap(&v1, &v2);
		float_swap(&model_x1, &model_x2);
		float_swap(&model_y1, &model_y2);
		float_swap(&model_z1, &model_z2);
		float_swap(&model_w1, &model_w2);
	}
	// We need to sort y0 and y1 again because y1 might have changed!
	if (y0 > y1) {
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
		float_swap(&z0, &z1);
		float_swap(&w0, &w1);
		float_swap(&n0x, &n1x);
		float_swap(&n0y, &n1y);
		float_swap(&n0z, &n1z);
		float_swap(&u0, &u1);
		float_swap(&v0, &v1);
		float_swap(&model_x0, &model_x1);
		float_swap(&model_y0, &model_y1);
		float_swap(&model_z0, &model_z1);
		float_swap(&model_w0, &model_w1);
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

				float interpolated_x = (model_x0 / point_a.w) * alpha + (model_x1 / point_b.w) * beta + (model_x2 / point_c.w) * gamma;
				float interpolated_y = (model_y0 / point_a.w) * alpha + (model_y1 / point_b.w) * beta + (model_y2 / point_c.w) * gamma;
				float interpolated_z = (model_z0 / point_a.w) * alpha + (model_z1 / point_b.w) * beta + (model_z2 / point_c.w) * gamma;
				interpolated_x /= interpolated_reciprocal_w;
				interpolated_y /= interpolated_reciprocal_w;
				interpolated_z /= interpolated_reciprocal_w;

				float interpolated_normal_x = (n0x / point_a.w) * alpha + (n1x / point_b.w) * beta + (n2x / point_c.w) * gamma;
				float interpolated_normal_y = (n0y / point_a.w) * alpha + (n1y / point_b.w) * beta + (n2y / point_c.w) * gamma;
				float interpolated_normal_z = (n0z / point_a.w) * alpha + (n1z / point_b.w) * beta + (n2z / point_c.w) * gamma;
				interpolated_normal_x /= interpolated_reciprocal_w;
				interpolated_normal_y /= interpolated_reciprocal_w;
				interpolated_normal_z /= interpolated_reciprocal_w;

				interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

				fragment_shader_triangle_inputs fs_inputs = {
					.x = x,
					.y = y,
					.u = interpolated_u,
					.v = interpolated_v,
					.interpolated_w = interpolated_reciprocal_w,
					.interpolated_world_space_pos_x = interpolated_x,
					.interpolated_world_space_pos_y = interpolated_y,
					.interpolated_world_space_pos_z = interpolated_z,
					.interpolated_normal_x = interpolated_normal_x,
					.interpolated_normal_y = interpolated_normal_y,
					.interpolated_normal_z = interpolated_normal_z
				};

				fragment_shader_result_t fs_out = fs_shader(
					camera_type,
					camera,
					mesh,
					&fs_inputs
				);
				
				if (fs_out.depth_buffer == NULL) {
					if (fs_out.color_buffer != NULL) {
						update_color_buffer_at(fs_out.color_buffer, x, y, fs_out.color);
					}
				} else {
					if (interpolated_reciprocal_w < get_depth_buffer_at(fs_out.depth_buffer, x, y)) {
						if (fs_out.color_buffer != NULL) {
							update_color_buffer_at(fs_out.color_buffer, x, y, fs_out.color);
						}
						update_depth_buffer_at(fs_out.depth_buffer, x, y, fs_out.depth);
					}
				}
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

				float interpolated_x = (model_x0 / point_a.w) * alpha + (model_x1 / point_b.w) * beta + (model_x2 / point_c.w) * gamma;
				float interpolated_y = (model_y0 / point_a.w) * alpha + (model_y1 / point_b.w) * beta + (model_y2 / point_c.w) * gamma;
				float interpolated_z = (model_z0 / point_a.w) * alpha + (model_z1 / point_b.w) * beta + (model_z2 / point_c.w) * gamma;
				interpolated_x /= interpolated_reciprocal_w;
				interpolated_y /= interpolated_reciprocal_w;
				interpolated_z /= interpolated_reciprocal_w;

				float interpolated_normal_x = (n0x / point_a.w) * alpha + (n1x / point_b.w) * beta + (n2x / point_c.w) * gamma;
				float interpolated_normal_y = (n0y / point_a.w) * alpha + (n1y / point_b.w) * beta + (n2y / point_c.w) * gamma;
				float interpolated_normal_z = (n0z / point_a.w) * alpha + (n1z / point_b.w) * beta + (n2z / point_c.w) * gamma;
				interpolated_normal_x /= interpolated_reciprocal_w;
				interpolated_normal_y /= interpolated_reciprocal_w;
				interpolated_normal_z /= interpolated_reciprocal_w;

				interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

				fragment_shader_triangle_inputs fs_inputs = {
					.x = x,
					.y = y,
					.u = interpolated_u,
					.v = interpolated_v,
					.interpolated_w = interpolated_reciprocal_w,
					.interpolated_world_space_pos_x = interpolated_x,
					.interpolated_world_space_pos_y = interpolated_y,
					.interpolated_world_space_pos_z = interpolated_z,
					.interpolated_normal_x = interpolated_normal_x,
					.interpolated_normal_y = interpolated_normal_y,
					.interpolated_normal_z = interpolated_normal_z
				};

				fragment_shader_result_t fs_out = fs_shader(
					camera_type,
					camera,
					mesh,
					&fs_inputs
				);

				if (fs_out.depth_buffer == NULL) {
					if (fs_out.color_buffer != NULL) {
						update_color_buffer_at(fs_out.color_buffer, x, y, fs_out.color);
					}
				} else {
					if (interpolated_reciprocal_w < get_depth_buffer_at(fs_out.depth_buffer, x, y)) {
						if (fs_out.color_buffer != NULL) {
							update_color_buffer_at(fs_out.color_buffer, x, y, fs_out.color);
						}
						update_depth_buffer_at(fs_out.depth_buffer, x, y, fs_out.depth);
					}
				}
			}
		}
	}
}

void render_line(
	int x0, int y0,
	int x1, int y1,
	triangle_t* triangle_to_render,
	int camera_type,
	void* camera,
	mesh_t* mesh,
	fragment_shader_callback fs_shader
) {
	int delta_x = x1 - x0;
	int delta_y = y1 - y0;

	int longest_side_length = abs(delta_x) >= abs(delta_y) ? abs(delta_x) : abs(delta_y);

	float x_inc = delta_x / (float)longest_side_length;
	float y_inc = delta_y / (float)longest_side_length;

	float current_x = x0;
	float current_y = y0;

	for (int i = 0; i <= longest_side_length; i++) {
		int x = round(current_x);
		int y = round(current_y);

		fragment_shader_line_inputs fs_inputs = {
			.x = x,
			.y = y
		};

		fragment_shader_result_t fs_out = fs_shader(
			camera_type,
			camera,
			mesh,
			&fs_inputs
		);

		if (fs_out.color_buffer != NULL) {
			update_color_buffer_at(fs_out.color_buffer, x, y, fs_out.color);
		}
		if (fs_out.depth_buffer != NULL) {
			update_depth_buffer_at(fs_out.depth_buffer, x, y, fs_out.depth);
		}
		current_x += x_inc;
		current_y += y_inc;
	}
}

void draw_rect(
	int x, int y,
	int width, int height,
	int camera_type,
	void* camera,
	mesh_t* mesh,
	fragment_shader_callback fs_shader
) {
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			int current_x = x + i - width / 2;
			int current_y = y + j - height / 2;

			fragment_shader_point_inputs fs_inputs = {
				.x = current_x,
				.y = current_y
			};

			fragment_shader_result_t fs_out = fs_shader(
				camera_type,
				camera,
				mesh,
				&fs_inputs
			);

			if (fs_out.color_buffer != NULL) {
				update_color_buffer_at(fs_out.color_buffer, current_x, current_y, fs_out.color);
			}
			if (fs_out.depth_buffer != NULL) {
				update_depth_buffer_at(fs_out.depth_buffer, current_x, current_y, fs_out.depth);
			}
		}
	}
}

void render_triangle_as_points(
	triangle_t* triangle_to_render,
	int camera_type,
	void* camera,
	mesh_t* mesh,
	fragment_shader_callback fs_shader
) {
	// TODO: make configurable from outside
	int width = 10;
	int height = 10;

	draw_rect(
		triangle_to_render->vertices[0].position.x,
		triangle_to_render->vertices[0].position.y,
		width,
		height,
		camera_type,
		camera,
		mesh,
		fs_shader
	);
	draw_rect(
		triangle_to_render->vertices[1].position.x,
		triangle_to_render->vertices[1].position.y,
		width,
		height,
		camera_type,
		camera,
		mesh,
		fs_shader
	);
	draw_rect(
		triangle_to_render->vertices[2].position.x,
		triangle_to_render->vertices[2].position.y,
		width,
		height,
		camera_type,
		camera,
		mesh,
		fs_shader
	);
}

void render_triangle_as_line(
	triangle_t* triangle_to_render,
	int camera_type,
	void* camera,
	mesh_t* mesh,
	fragment_shader_callback fs_shader
) {
	render_line(
		triangle_to_render->vertices[0].position.x,
		triangle_to_render->vertices[0].position.y, 
		triangle_to_render->vertices[1].position.x,
		triangle_to_render->vertices[1].position.y, 
		triangle_to_render,
		camera_type,
		camera,
		mesh,
		fs_shader
	);
	render_line(
		triangle_to_render->vertices[1].position.x,
		triangle_to_render->vertices[1].position.y, 
		triangle_to_render->vertices[2].position.x,
		triangle_to_render->vertices[2].position.y, 
		triangle_to_render,
		camera_type,
		camera,
		mesh,
		fs_shader
	);
	render_line(
		triangle_to_render->vertices[2].position.x,
		triangle_to_render->vertices[2].position.y, 
		triangle_to_render->vertices[0].position.x,
		triangle_to_render->vertices[0].position.y, 
		triangle_to_render,
		camera_type,
		camera,
		mesh,
		fs_shader
	);
}

void pipeline_draw(
	int camera_type,
	void* camera,
	mesh_t* mesh,
	int cull_mode,
	int render_mode,
	vertex_shader_callback vs_shader,
	fragment_shader_callback fs_shader
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
		vec3_t face_normals[3];
		vec4_t transformed_vertices[3];
		
		face_vertices[0] = mesh->vertices[face.a];
		face_vertices[1] = mesh->vertices[face.b];
		face_vertices[2] = mesh->vertices[face.c];

		face_normals[0] = mesh->normals[face.a];
		face_normals[1] = mesh->normals[face.b];
		face_normals[2] = mesh->normals[face.c];

		for (int j = 0; j < 3; j++) {
			vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);
			vec4_t world_space_vertex = mat4_mul_vec4(mesh->world_matrix, transformed_vertex);
			transformed_vertices[j] = world_space_vertex;
			varying_world_vertices[j].position = world_space_vertex;
			varying_world_vertices[j].normal = face_normals[j];
			
			if (camera_type == PERSPECTIVE_CAMERA) {
				transformed_vertices[j] = mat4_mul_vec4(persp_camera->view_matrix, transformed_vertices[j]);
			} else {
				transformed_vertices[j] = mat4_mul_vec4(ortho_camera->view_matrix, transformed_vertices[j]);
			}
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

		if (cull_mode == CULL_BACKFACE) {
			if (dot_normal_camera < 0) {
				continue;
			}
		}
		if (cull_mode == CULL_FRONTFACE) {
			if (dot_normal_camera > 0) {
				continue;
			}
		}

		polygon_t polygon = create_polygon_from_triangles(
			vec3_from_vec4(transformed_vertices[0]),
			vec3_from_vec4(transformed_vertices[1]),
			vec3_from_vec4(transformed_vertices[2]),
			vec3_from_vec4(varying_world_vertices[0].position),
			vec3_from_vec4(varying_world_vertices[1].position),
			vec3_from_vec4(varying_world_vertices[2].position),
			varying_world_vertices[0].normal,
			varying_world_vertices[1].normal,
			varying_world_vertices[2].normal,
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
				(*vs_shader)(camera_type, camera, mesh, &triangle_to_render.vertices[j]);
			}

			if (render_mode == RENDER_TRIANGLE) {
				render_triangle(&triangle_to_render, camera_type, camera, mesh, fs_shader);
			}

			if (render_mode == RENDER_WIRE) {
				render_triangle_as_line(&triangle_to_render, camera_type, camera, mesh, fs_shader);
			}

			if (render_mode == RENDER_VERTEX) {
				render_triangle_as_points(&triangle_to_render, camera_type, camera, mesh, fs_shader);
			}
			
		}
	}
}
