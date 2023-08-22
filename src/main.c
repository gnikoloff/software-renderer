#include "stdio.h"
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <SDL2/SDL.h>
#include "upng.h"
#include "array.h"
#include "display.h"
#include "vector.h"
#include "matrix.h"
#include "texture.h"
#include "triangle.h"
#include "camera.h"
#include "light.h"
#include "mesh.h"
#include "clipping.h"

#define MAX_TRIANGLES_PER_MESH 10000
triangle_t triangles_to_render[MAX_TRIANGLES_PER_MESH];
int num_triangles_to_render = 0;

bool is_running = false;
int previous_frame_time = 0;
int delta_time = 0;

mat4_t proj_matrix;
mat4_t world_matrix;
mat4_t view_matrix;
mat4_t model_view_matrix;
mat4_t scale_matrix;
mat4_t translate_matrix;
mat4_t rotation_matrix_x;
mat4_t rotation_matrix_y;
mat4_t rotation_matrix_z;

void setup(void) {
	set_render_method(RENDER_WIRE);
	set_cull_method(CULL_BACKFACE);

	load_mesh(
		"./assets/crab.obj",
		"./assets/crab.png",
		vec3_new(1, 1, 1),
		vec3_new(-3, 0, 0),
		vec3_new(0, 0, 0)
	);

	load_mesh(
		"./assets/crab.obj",
		"./assets/crab.png",
		vec3_new(1, 1, 1),
		vec3_new(3, 0, 0),
		vec3_new(0, 0, 0)
	);

	// Initialize the scene light direction
	init_light(vec3_new(0, 0, 1));

	// Initialize the perspective projection matrix
	float aspectx = (float)get_window_width() / get_window_height();
	float aspecty = (float)get_window_height() / get_window_width();
	float fovy = 3.14 / 3.0;
	float fovx = atan(tan(fovy / 2) * aspectx) * 2;
	float z_near = 0.1;
	float z_far = 100.0;
	proj_matrix = mat4_make_perspective(fovy, aspecty, z_near, z_far);
	
	init_frustum_planes(fovx, fovy, z_near, z_far);

}

void process_input(void) {

	static camera_direction = { 0, 0, 0 };

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			is_running = false;
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE) {
				is_running = false;
			}
			if (event.key.keysym.sym == SDLK_1) {
				set_render_method(RENDER_FILL_TRIANGLE);
				break;
			}
			if (event.key.keysym.sym == SDLK_2) {
				set_render_method(RENDER_FILL_TRIANGLE_WIRE);
				break;
			}
			if (event.key.keysym.sym == SDLK_3) {
				set_render_method(RENDER_WIRE);
				break;
			}
			if (event.key.keysym.sym == SDLK_4) {
				set_render_method(RENDER_WIRE_VERTEX);
				break;
			}
			if (event.key.keysym.sym == SDLK_5) {
				set_render_method(RENDER_TEXTURED);
				break;
			}
			if (event.key.keysym.sym == SDLK_6) {
				set_render_method(RENDER_TEXTURED_WIRE);
				break;
			}
			if (event.key.keysym.sym == SDLK_c) {
				set_cull_method(CULL_BACKFACE);
				break;
			}
			if (event.key.keysym.sym == SDLK_x) {
				set_cull_method(CULL_NONE);
				break;
			}
			if (event.key.keysym.sym == SDLK_UP) {
				add_camera_position_y(0.01 * delta_time);
				break;
			}
			if (event.key.keysym.sym == SDLK_DOWN) {
				add_camera_position_y(-0.01 * delta_time);
				break;
			}
			if (event.key.keysym.sym == SDLK_a) {
				add_camera_yaw(0.01 * delta_time);
				break;
			}
			if (event.key.keysym.sym == SDLK_d) {
				add_camera_yaw(-0.01 * delta_time);
				break;
			}
			if (event.key.keysym.sym == SDLK_q) {
				add_camera_pitch(0.01 * delta_time);
				break;
			}
			if (event.key.keysym.sym == SDLK_e) {
				add_camera_pitch(-0.01 * delta_time);
				break;
			}
			if (event.key.keysym.sym == SDLK_w) {
				// vec3_mul(&camera_direction, get_camera_direction(), 0.1 * delta_time);
				// set_camera_forward_velocity(&camera_direction);
				// set_camera_position(vec3_add(get_camera_position(), get_camera_forward_velocity()));
				break;
			}
			if (event.key.keysym.sym == SDLK_s) {
				// set_camera_forward_velocity(vec3_mul(get_camera_direction(), 0.1 * delta_time));
				// set_camera_position(vec3_sub(get_camera_position(), get_camera_forward_velocity()));
				break;
			}
			break;
	}	
	}
}

void update(void) {

	int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
	if (time_to_wait > 0 && time_to_wait < FRAME_TARGET_TIME) {
		SDL_Delay(time_to_wait);
	}

	// Get a delta time factor converted to seconds to be used to update our game logic
	delta_time = (SDL_GetTicks() - previous_frame_time);
	previous_frame_time = SDL_GetTicks();

	num_triangles_to_render = 0;

	// Create a view matrix
	// TODO: compute the new camera rotation and translation for FPS camera movement
	// Initialize the target
	vec3_t target = vec3_new(0, 0, 1);
	vec4_t target4 = vec4_new(0, 0, 1, 1);
	mat4_t camera_yaw_rotation;
	mat4_make_rotation_y(&camera_yaw_rotation, get_camera_yaw());
	mat4_t camera_pitch_rotation;
	mat4_make_rotation_x(&camera_pitch_rotation, get_camera_pitch());
	// mat4_mul_vec4(camera_yaw_rotation, vec4_from_vec3(target)))
	mat4_mul_vec4(&target4, &camera_yaw_rotation, &target4);
	mat4_mul_vec4(&target4, &camera_pitch_rotation, &target4);

	target = vec3_from_vec4(&target4);

	set_camera_direction(target);


	// Offset the camera position in the direction where the camera is pointing at
	vec3_add(&target, get_camera_position(), get_camera_direction());

	vec3_t up = { 0, 1, 0 };
	mat4_look_at(&view_matrix, get_camera_position(), &target, &up);

	// Loop all the meshes of our scene
	for (int mesh_index = 0; mesh_index < get_meshes_count(); mesh_index++) {
		mesh_t* mesh = get_mesh(mesh_index);

		// Create a scale, translation and rotation matrices that will be used to multiply the mesh vertices
		mat4_make_scale(&scale_matrix, mesh->scale.x, mesh->scale.y, mesh->scale.z);
		mat4_make_translation(&translate_matrix, mesh->translation.x, mesh->translation.y, mesh->translation.z);
		mat4_make_rotation_x(&rotation_matrix_x, mesh->rotation.x);
		mat4_make_rotation_y(&rotation_matrix_y, mesh->rotation.y);
		mat4_make_rotation_z(&rotation_matrix_z, mesh->rotation.z);

		int num_faces = array_length(mesh->faces);

		for (int i = 0; i < num_faces; i++) {

			face_t face = mesh->faces[i];

			vec3_t face_vertices[3];
			face_vertices[0] = mesh->vertices[face.a];
			face_vertices[1] = mesh->vertices[face.b];
			face_vertices[2] = mesh->vertices[face.c];

			vec4_t transformed_vertices[3];

			// First we transform the vertices in world spacw

			for (int j = 0; j < 3; j++) {
				vec4_t transformed_vertex = vec4_from_vec3(&face_vertices[j]);

				// Create a World Matrix combining scale, rotation and translation matrices
				mat4_identity(&world_matrix);
				mat4_mul_mat4(&world_matrix, &scale_matrix, &world_matrix);
				mat4_mul_mat4(&world_matrix, &rotation_matrix_x, &world_matrix);
				mat4_mul_mat4(&world_matrix, &rotation_matrix_y, &world_matrix);
				mat4_mul_mat4(&world_matrix, &rotation_matrix_z, &world_matrix);
				mat4_mul_mat4(&world_matrix, &translate_matrix, &world_matrix);

				mat4_mul_mat4(&model_view_matrix, &view_matrix, &world_matrix);

				// Save transformed vertex in the array of transformed vertices
				mat4_mul_vec4(&transformed_vertices[j], &model_view_matrix, &transformed_vertex);
			}

			// Second we perform backface culling and disregard backfacing triangles
			vec3_t vector_a = vec3_from_vec4(&transformed_vertices[0]); /*   A   */
			vec3_t vector_b = vec3_from_vec4(&transformed_vertices[1]); /*  / \  */
			vec3_t vector_c = vec3_from_vec4(&transformed_vertices[2]); /* C---B */

			// Get the vector subtraction of B-A and C-a
			vec3_t vector_ab;
			vec3_sub(&vector_ab, &vector_b, &vector_a);
			vec3_normalize(&vector_ab);
			vec3_t vector_ac;
			vec3_sub(&vector_ac, &vector_c, &vector_a);
			vec3_normalize(&vector_ac);
			
			// Compute the face normal using cross product
			vec3_t vector_normal;
			vec3_cross(&vector_normal, &vector_ab, &vector_ac);
			vec3_normalize(&vector_normal);

			// Find the vector between a point in the triangle and the camera origin
			// Our camera is always at origin (0 0 0) (camera space)
			vec3_t origin = { 0, 0, 0 };
			vec3_t camera_ray;
			vec3_sub(&camera_ray, &origin, &vector_a);

			// Calculate how aligned the camera_ray is with the face normal using dot product
			float dot_normal_camera = vec3_dot(&vector_normal, &camera_ray);

			if (is_cull_backface()) {
				if (dot_normal_camera < 0) {
					continue;
				}
			}

			// Create a polygon from the original transformed triangle to be clipped
			polygon_t polygon = create_polygon_from_triangles(
				vec3_from_vec4(&transformed_vertices[0]),
				vec3_from_vec4(&transformed_vertices[1]),
				vec3_from_vec4(&transformed_vertices[2]),
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

				// Third we project the frontfacing triangles
				for (int j = 0; j < 3; j++) {
					// Multiply point with projection matrix and perspective divide!
					mat4_mul_vec4_project(&projected_points[j], &proj_matrix, &triangle_after_clipping.points[j]);

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
				float light_intensity_factor = -vec3_dot(&vector_normal, get_light_direction());

				// Calculate the triangle color based on the light angle
				uint32_t triangle_color = face.color;

				triangle_color = light_apply_intensity(triangle_color, light_intensity_factor);

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
	}
}

void render(void) {
	clear_color_buffer(0xFF000000);
	clear_z_buffer();

	for (int i = 0; i < num_triangles_to_render; i++) {
		triangle_t triangle = triangles_to_render[i];
		
		if (should_render_filled_triangles()) {
			draw_filled_triangle(
				triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w,
				triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w,
				triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w,
				triangle.color
			);
		}

		if (should_render_textured_triangles()) {
			draw_textured_triangle(
				triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.tex_coords[0].u, triangle.tex_coords[0].v,
				triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.tex_coords[1].u, triangle.tex_coords[1].v,
				triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.tex_coords[2].u, triangle.tex_coords[2].v,
				triangle.texture
			);
		}

		if (should_render_wireframe()) {
			draw_triangle(
				triangle.points[0].x, triangle.points[0].y,
				triangle.points[1].x, triangle.points[1].y,
				triangle.points[2].x, triangle.points[2].y,
				0xFFFF0000
			);
		}

		if (should_render_vertex()) {
			for (int j = 0; j < 3; j++) {
				draw_rect(triangle.points[j].x, triangle.points[j].y, 10, 10, 0xFFFF0000);
			}
		}
	}

	render_color_buffer();
}

void free_resources(void) {
	dispose_meshes();
	destroy_window();
}

int main(void) {
	is_running = initialize_window();

	setup();

	while(is_running) {
		process_input();
		update();
		render();
	}

	free_resources();

	return 0;
}