#ifdef __EMSCRIPTEN__
	#include "emscripten.h"
#endif
#include "stdio.h"
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <SDL2/SDL.h>
#include "upng.h"
#include "utils.h"
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
#include "geometry.h"

#define MAX_TRIANGLES_PER_MESH 10000
triangle_t triangles_to_render[MAX_TRIANGLES_PER_MESH];
int num_triangles_to_render = 0;

bool is_running = false;
int previous_frame_time = 0;
int delta_time = 0;

void setup(void) {
	set_render_method(RENDER_WIRE);
	set_cull_method(CULL_BACKFACE);

	// load_mesh_png_data(torus, "./assets/cube.png");
	// load_mesh_png_data(sphere, "./assets/cube.png");

	// mesh_t* plane = make_plane(5, 5, 5, 5);
	load_mesh_png_data(sphere, "./assets/uv-map.png");
	load_mesh_png_data(box, "./assets/uv-map.png");
	load_mesh_png_data(ring, "./assets/uv-map.png");
	load_mesh_png_data(plane, "./assets/uv-map.png");
	load_mesh_png_data(torus, "./assets/uv-map.png");

	// load_mesh(
	// 	"./assets/crab.obj",
	// 	"./assets/crab.png",
	// 	vec3_new(1, 1, 1),
	// 	vec3_new(0, 0, 0),
	// 	vec3_new(0, 0, 0)
	// );

	// load_mesh(
	// 	"./assets/crab.obj",
	// 	"./assets/crab.png",
	// 	vec3_new(1, 1, 1),
	// 	vec3_new(1, 0, 0),
	// 	vec3_new(0, 0, 0)
	// );

	// Initialize the scene light direction
	init_light(vec3_new(0, 0, 1));

	// Initialize the perspective projection matrix
	float aspectx = (float)get_window_width() / (float)get_window_height();
	float aspecty = (float)get_window_height() / (float)get_window_width();
	float fovy = 3.141592 / 3.0;
	float fovx = atan(tan(fovy / 2) * aspectx) * 2;
	float z_near = 1.0;
	float z_far = 100.0;

	vec3_t cam_position = { .x = 0, .y = 0, .z = -5 };
	vec3_t cam_target = { .x = 0, .y = 0, .z = 0 };
	camera = make_perspective_camera(fovy, aspecty, z_near, z_far, cam_position, cam_target);
	
	init_frustum_planes(fovx, fovy, z_near, z_far);
}

void process_input(void) {
	static vec3_t camera_direction = { 0, 0, 0 };

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			is_running = false;
			break;
		case SDL_MOUSEMOTION:
			if (event.motion.state & SDL_BUTTON_LMASK) {
				update_camera_on_drag(camera, event.motion.xrel, event.motion.yrel);
			}
			break;
		case SDL_MOUSEWHEEL:
			camera->distance += -event.wheel.preciseY * 0.01 * delta_time;
			update_camera_on_drag(camera, 0, 0);
			break;
		case SDL_MOUSEBUTTONDOWN:
			// ...
			break;
		case SDL_MOUSEBUTTONUP:
			// ...
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
				// add_camera_position_y(0.01 * delta_time);
				break;
			}
			if (event.key.keysym.sym == SDLK_DOWN) {
				// add_camera_position_y(-0.01 * delta_time);
				break;
			}
			if (event.key.keysym.sym == SDLK_a) {
				// add_camera_yaw(0.01 * delta_time);
				break;
			}
			if (event.key.keysym.sym == SDLK_d) {
				// add_camera_yaw(-0.01 * delta_time);
				break;
			}
			if (event.key.keysym.sym == SDLK_q) {
				// add_camera_pitch(0.01 * delta_time);
				break;
			}
			if (event.key.keysym.sym == SDLK_e) {
				// add_camera_pitch(-0.01 * delta_time);
				break;
			}
			if (event.key.keysym.sym == SDLK_w) {
				camera->position.z += 0.1;
				update_camera_view_matrix(camera);
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

	sphere->rotation.x += delta_time * 0.0001;
	sphere->rotation.y += delta_time * 0.0001;
	sphere->rotation.z += delta_time * 0.0001;
	
	box->rotation.x += delta_time * 0.0001;
	box->rotation.y += delta_time * 0.0001;
	box->rotation.z += delta_time * 0.0001;
	
	ring->rotation.x += delta_time * 0.0001;
	ring->rotation.y += delta_time * 0.0001;
	ring->rotation.z += delta_time * 0.0001;
	
	plane->rotation.x += delta_time * 0.0001;
	plane->rotation.y += delta_time * 0.0001;
	plane->rotation.z += delta_time * 0.0001;

	torus->rotation.x += delta_time * 0.0001;
	torus->rotation.y += delta_time * 0.0001;
	torus->rotation.z += delta_time * 0.0001;

	num_triangles_to_render = 0;

	// Loop all the meshes of our scene
	for (int mesh_index = 0; mesh_index < get_meshes_count(); mesh_index++) {
		mesh_t* mesh = get_mesh(mesh_index);
		mesh_update_world_matrix(mesh);

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
				float light_intensity_factor = -vec3_dot(vector_normal, get_light_direction());

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
				int size = 10;
				draw_rect(triangle.points[j].x - size / 2, triangle.points[j].y - size / 2, size, size, 0xFFFF0000);
			}
		}
	}

	render_color_buffer();
}

void free_resources(void) {
	dispose_meshes();
	destroy_window();
}

void onFrame(void) {
	process_input();
	update();
	render();
}

int main(void) {
	is_running = initialize_window();

	setup();

	#ifdef __EMSCRIPTEN__
		emscripten_set_main_loop_timing(EM_TIMING_RAF, 1);
		emscripten_set_main_loop(onFrame, 0, 1);
	#else
		while(is_running) {
			onFrame();
		}
	#endif

	free_resources();

	return 0;
}