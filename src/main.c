#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <SDL2/SDL.h>
#include "array.h"
#include "display.h"
#include "vector.h"
#include "matrix.h"
#include "light.h"
#include "mesh.h"

vec3_t camera_position = { 0, 0, 0 };

triangle_t* triangles_to_render = NULL;

bool is_running = false;
int previous_frame_time = 0;
mat4_t proj_matrix;

void setup(void) {
	render_method = RENDER_FILL_TRIANGLE;
	cull_method = CULL_BACKFACE;

	color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height);
	color_buffer_texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		window_width,
		window_height
	);
	load_obj_file_data("./assets/f22.obj");
	// load_cube_mesh_data();

	// Initialize the perspective projection matrix
	float fov = 60.0 * M_PI / 180;
	float aspect = (float)window_height / window_width;
	float near = 0.1;
	float far = 100.0;
	proj_matrix = mat4_make_perspective(fov, aspect, near, far);

}

void process_input(void) {
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type) {
		case SDL_QUIT:
			is_running = false;
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE) {
				is_running = false;
			}
			if (event.key.keysym.sym == SDLK_1) {
				render_method = RENDER_FILL_TRIANGLE_WIRE;
			} else if (event.key.keysym.sym == SDLK_2) {
				render_method = RENDER_WIRE;
			} else if (event.key.keysym.sym == SDLK_3) {
				render_method = RENDER_FILL_TRIANGLE;
			} else if (event.key.keysym.sym == SDLK_c) {
				cull_method = CULL_BACKFACE;
			} else if (event.key.keysym.sym == SDLK_d) {
				cull_method = CULL_NONE;
			}
			break;
	}	
}

void update(void) {

	int time_to_wait = FRAME_TARGET_TIME - SDL_GetTicks() - previous_frame_time;
	previous_frame_time = SDL_GetTicks();
	if (time_to_wait > 0 && time_to_wait < FRAME_TARGET_TIME) {
		SDL_Delay(time_to_wait);
	}

	// Initialize the array of triangles to render
	triangles_to_render = NULL;

	// mesh.translation.x ;
	mesh.translation.z = 5.0;

	// mesh.rotation.x = -M_PI;
	// mesh.rotation.y += 0.0035;
	// mesh.rotation.z += 0.0035;

	// mesh.scale.x += 0.002;
	// mesh.scale.y += 0.002;

	// Create a scale, translation and rotation matrices that will be used to multiply the mesh vertices
	mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
	mat4_t translate_matrix = mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);
	mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
	mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh.rotation.y);
	mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh.rotation.z);

	int num_faces = array_length(mesh.faces);

	for (int i = 0; i < num_faces; i++) {
		face_t face = mesh.faces[i];

		vec3_t face_vertices[3];
		face_vertices[0] = mesh.vertices[face.a - 1];
		face_vertices[1] = mesh.vertices[face.b - 1];
		face_vertices[2] = mesh.vertices[face.c - 1];

		vec4_t transformed_vertices[3];

		// First we transform the vertices in world spacw

		for (int j = 0; j < 3; j++) {
			vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

			// Create a World Matrix combining scale, rotation and translation matrices
			mat4_t world_matrix = mat4_identity();
			world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
			world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
			world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
			world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
			world_matrix = mat4_mul_mat4(translate_matrix, world_matrix);
			

			// Use a matrix to scale our original vertex
			// transformed_vertex = mat4_mul_vec4(scale_matrix, transformed_vertex);
			// transformed_vertex = mat4_mul_vec4(rotation_matrix_x, transformed_vertex);
			// transformed_vertex = mat4_mul_vec4(rotation_matrix_y, transformed_vertex);
			// transformed_vertex = mat4_mul_vec4(rotation_matrix_z, transformed_vertex);
			// transformed_vertex = mat4_mul_vec4(translate_matrix, transformed_vertex);

			// transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
			// transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
			// transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);

			// Save transformed vertex in the array of transformed vertices
			transformed_vertices[j] = mat4_mul_vec4(world_matrix, transformed_vertex);
		}

		// Second we perform backface culling and disregard backfacing triangles
		vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]); /*   A   */
		vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]); /*  / \  */
		vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]); /* C---B */

		// Get the vector subtraction of B-A and C-a
		vec3_t vector_ab = vec3_subtract(vector_b, vector_a);
		vec3_normalize(&vector_ab);
		vec3_t vector_ac = vec3_subtract(vector_c, vector_a);
		vec3_normalize(&vector_ac);
		
		// Compute the face normal using cross product
		vec3_t vector_normal = vec3_cross(vector_ab, vector_ac);
		vec3_normalize(&vector_normal);

		// Find the vector between a point in the triangle and the camera origin
		vec3_t camera_ray = vec3_subtract(camera_position, vector_a);

		// Calculate how aligned the camera_ray is with the face normal using dot product
		float dot_normal_camera = vec3_dot(vector_normal, camera_ray);

		if (cull_method == CULL_BACKFACE && dot_normal_camera <= 0) {
			continue;
		}

		vec4_t projected_points[3];

		// Third we project the frontfacing triangles
		for (int j = 0; j < 3; j++) {
			// Multiply point with projection matrix and perspective divide!
			projected_points[j] = mat4_mul_vec4_project(proj_matrix, transformed_vertices[j]);

			float half_window_width = window_width / 2.0;
			float half_window_height = window_height / 2.0;

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

		// Calculate the avg depth for each face based on the vertices after transformation
		float avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3.0;

		// Calculate the shade intensity based on how aligned the face normal is with the light direction
		float light_intensity_factor = -vec3_dot(vector_normal, light.direction);

		// Calculate the triangle color based on the light angle
		uint32_t triangle_color = face.color;

		triangle_color = light_apply_intensity(triangle_color, light_intensity_factor);

		triangle_t projected_triangle = {
			.points = {
				{ projected_points[0].x, projected_points[0].y },
				{ projected_points[1].x, projected_points[1].y },
				{ projected_points[2].x, projected_points[2].y }
			},
			.color = triangle_color,
			.avg_depth = avg_depth
		};

		array_push(triangles_to_render, projected_triangle);
	}

	// Sort the triangles to render by their avg_depth
	int num_triangles = array_length(triangles_to_render);
	for (int i = 0; i < num_triangles; i++) {
		for (int j = i; j < num_triangles; j++) {
			if (triangles_to_render[i].avg_depth < triangles_to_render[j].avg_depth) {
				// Swap the triangles in the array
				triangle_t temp = triangles_to_render[i];
				triangles_to_render[i] = triangles_to_render[j];
				triangles_to_render[j] = temp;
			}
		}
	}
}

void render(void) {
	clear_color_buffer(0xFF000000);

	int count_of_triangles = array_length(triangles_to_render);

	// 			display_wireframe
// display_vertices
// display_filled_triangles
// enable_backface_culling

	for (int i = 0; i < count_of_triangles; i++) {
		triangle_t triangle = triangles_to_render[i];
		
		if (render_method == RENDER_FILL_TRIANGLE) {
			draw_filled_triangle(
				triangle.points[0].x,
				triangle.points[0].y,
				triangle.points[1].x,
				triangle.points[1].y,
				triangle.points[2].x,
				triangle.points[2].y,
				triangle.color
			);
			continue;
		}

		if (render_method == RENDER_FILL_TRIANGLE_WIRE) {
			draw_filled_triangle(
				triangle.points[0].x,
				triangle.points[0].y,
				triangle.points[1].x,
				triangle.points[1].y,
				triangle.points[2].x,
				triangle.points[2].y,
				triangle.color
			);
			draw_triangle(
				triangle.points[0].x,
				triangle.points[0].y,
				triangle.points[1].x,
				triangle.points[1].y,
				triangle.points[2].x,
				triangle.points[2].y,
				0xFFFF0000
			);
			continue;
		}

		if (render_method == RENDER_WIRE) {
			draw_triangle(
				triangle.points[0].x,
				triangle.points[0].y,
				triangle.points[1].x,
				triangle.points[1].y,
				triangle.points[2].x,
				triangle.points[2].y,
				0xFFFF0000
			);
			continue;
		}

		if (render_method == RENDER_WIRE_VERTEX) {
			draw_triangle(
				triangle.points[0].x,
				triangle.points[0].y,
				triangle.points[1].x,
				triangle.points[1].y,
				triangle.points[2].x,
				triangle.points[2].y,
				triangle.color
			);
			for (int j = 0; j < 3; j++) {
				draw_rect(triangle.points[j].x, triangle.points[j].y, 10, 10, 0xFFFF0000);
			}
			continue;
		}
	}

	// Clear the array of triangles to render on each loop!!!
	// It is not enough to simply set the pointer to NULL, as the memory
	// will never be reclaimed this way. Super important to clear it
	// ourselves before we lose the handle to it (set pointer to NULL)
	array_free(triangles_to_render);

	render_color_buffer();
	SDL_RenderPresent(renderer);
}

void free_resources(void) {
	free(color_buffer);
	array_free(mesh.vertices);
	array_free(mesh.faces);
}

int main(void) {
	is_running = initialize_window();

	setup();

	while(is_running) {
		process_input();
		update();
		render();
	}

	destroy_window();

	return 0;
}