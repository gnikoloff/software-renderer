#include "stdio.h"
#include "math.h"
#include <SDL2/SDL.h>
#include "../utils.h"
#include "../array.h"
#include "../vector.h"
#include "../matrix.h"
#include "../camera.h"
#include "../display.h"
#include "../clipping.h"
#include "../geometry.h"
#include "../triangle.h"
#include "../light.h"
#include "../pipeline.h"

#define SHADOW_DEPTH_BUFFER_SIZE 256

static perspective_camera_t* persp_camera = NULL;
static orthographic_camera_t* depth_camera = NULL;
static depth_framebuffer* shadow_depth_buffer = NULL;
static mesh_t* efa = NULL;
static mesh_t* plane = NULL;

void shadow_map_example_setup(void) {
	set_render_method(RENDER_TEXTURED);

	int vwidth = get_viewport_width();
	int vheight = get_viewport_height();

	float vwidthf = (float)vwidth;
	float vheightf = (float)vheight;

	float aspectx = vwidthf / vheightf;
	float aspecty = vheightf / vwidthf;
	float fovy = 3.141592 / 3.0;
	float fovx = atan(tan(fovy / 2) * aspectx) * 2;
	float z_near = 1.0;
	float z_far = 30.0;

	vec3_t persp_cam_position = { .x = 4, .y = 5, .z = -7 };
	vec3_t persp_cam_target = { .x = 0, .y = 0, .z = 0 };
	persp_camera = make_perspective_camera(
		fovy,
		aspecty,
		z_near,
		z_far,
		persp_cam_position,
		persp_cam_target
	);

	vec3_t ortho_cam_position = { .x = 0, .y = 10, .z = 10.1 };
	vec3_t ortho_cam_target = { .x = 0, .y = 0, .z = 0 };
	depth_camera = make_orthographic_camera(
		-0.5,
		 0.5,
		 0.5,
		-1.5,
		z_near,
		z_far,
		ortho_cam_position,
		ortho_cam_target
	);

	shadow_depth_buffer = make_depth_buffer(SHADOW_DEPTH_BUFFER_SIZE, SHADOW_DEPTH_BUFFER_SIZE);
	
	init_frustum_planes(fovx, fovy, z_near, z_far);

	efa = load_mesh(
		"./assets/efa.obj",
		"./assets/efa.png",
		vec3_new(1, 1, 1),
		vec3_new(0, 0, 0),
		vec3_new(0, 0, 0)
	);
	efa->scale.x = 1.0;
	efa->scale.y = 1.0;
	efa->scale.z = 1.0;
	efa->translation.y = 2.0;

	plane = make_plane(5, 5, 1, 1);
	plane->rotation.x = M_PI / 2;
}

void shadow_map_example_process_input(SDL_Event* event, int delta_time) {
	switch (event->type) {
		case SDL_MOUSEMOTION:
			if (event->motion.state & SDL_BUTTON_LMASK) {
				update_camera_on_drag(persp_camera, event->motion.xrel, event->motion.yrel);
			}
			break;
		case SDL_MOUSEWHEEL:
			persp_camera->distance += -event->wheel.preciseY * 0.01 * delta_time;
			update_camera_on_drag(persp_camera, 0, 0);
			break;
	}
}

void shadow_map_example_update(int delta_time) {
	float delta_multiplier = 0.00075;
	efa->rotation.y += delta_time * delta_multiplier;
	// depth_camera->position.z -= delta_time * 0.001;
	// static vec3_t up = { 0, 1, 0 };
	// persp_camera->view_matrix = mat4_look_at(depth_camera->position, depth_camera->target, up);
}

void depth_vertex_shader(
	int camera_type,
	void* camera,
	mesh_t* mesh,
	vertex_t* vertex
) {
	float half_depth_width = SHADOW_DEPTH_BUFFER_SIZE / 2;
	float half_depth_height = SHADOW_DEPTH_BUFFER_SIZE / 2;
	vertex->position.x *= half_depth_width;
	vertex->position.y *= half_depth_height;
	vertex->position.y *= -1;
	
	vertex->position.x += half_depth_width;
	vertex->position.y += half_depth_height;
}
void depth_fragment_shader(
	int camera_type,
	void* camera,
	mesh_t* mesh,
	int x, int y, float interpolated_reciprocal_w,
	float u, float v,
	float interpolated_x,
	float interpolated_y,
	float interpolated_z
) {
	uint8_t a = round(interpolated_reciprocal_w * 255.0);
	uint8_t test_color[4] = {1, a, a, a};
	uint32_t color = u8_to_u32(test_color);
	// update_color_buffer_at(get_screen_color_buffer(), x, y, color);
	update_depth_buffer_at(shadow_depth_buffer, x, y, interpolated_reciprocal_w);
}

void main_vertex_shader(
	int camera_type,
	void* camera,
	mesh_t* mesh,
	vertex_t* vertex
) {
	float half_viewport_width = get_viewport_width() / 2;
	float half_viewport_height = get_viewport_height() / 2;
	vertex->position.x *= half_viewport_width;
	vertex->position.y *= half_viewport_height;
	vertex->position.y *= -1;
	
	vertex->position.x += half_viewport_width;
	vertex->position.y += half_viewport_height;
}

static int a = 0;

void main_fragment_shader(
	int camera_type,
	void* camera,
	mesh_t* mesh,
	int x, int y,
	float interpolated_reciprocal_w,
	float u, float v,
	float interpolated_x,
	float interpolated_y,
	float interpolated_z
) {
	// vec4_t model_pos = vec4_new(model_space_x, model_space_y, model_space_z, 1);
	// // printf("model_pos %f %f %f\n", model_pos.x, model_pos.y, model_pos.z);
	
	
	// vec3_t model_pos3 = vec3_from_vec4(model_pos);
	// vec3_normalize(&model_pos3);

	// mat4_t inverse_model_matrix = mat4_inverse(mesh->world_matrix);

	vec4_t pos = vec4_new(interpolated_x, interpolated_y, interpolated_z, interpolated_reciprocal_w);
	mat4_t inverse_vp_matrix = mat4_mul_mat4(depth_camera->projection_matrix, depth_camera->view_matrix);
	vec4_t shadow_pos = mat4_mul_vec4(inverse_vp_matrix, pos);
	shadow_pos.x /= shadow_pos.w;
	shadow_pos.y /= shadow_pos.w;
	shadow_pos.z /= shadow_pos.w;

	float half_depth_width = SHADOW_DEPTH_BUFFER_SIZE / 2;
	float half_depth_height = SHADOW_DEPTH_BUFFER_SIZE / 2;

	int shadow_x = shadow_pos.x * SHADOW_DEPTH_BUFFER_SIZE;
	int shadow_y = shadow_pos.y * SHADOW_DEPTH_BUFFER_SIZE;

	shadow_x += half_depth_width;
	shadow_y += half_depth_height;

	int idx = shadow_y * SHADOW_DEPTH_BUFFER_SIZE + shadow_x; // index in the shadowbuffer array

	draw_texel(
		x, y,
		mesh->texture,
		interpolated_reciprocal_w,
		u, v,
		get_screen_color_buffer(),
		get_screen_depth_buffer()
	);
	if ((get_depth_buffer_at_idx(shadow_depth_buffer, idx) < (1 - shadow_pos.z))) {
		update_color_buffer_at(get_screen_color_buffer(), x, y, a % 2 == 0 ? 0xff00ff00 : 0xffff0000);
	}

	
}

void shadow_map_example_render(int delta_time) {
	// render shadow map

	clear_depth_buffer(shadow_depth_buffer);

	pipeline_draw(
		ORTHOGRAPHIC_CAMERA,
		depth_camera,
		plane,
		depth_vertex_shader,
		depth_fragment_shader
	);
	pipeline_draw(
		ORTHOGRAPHIC_CAMERA,
		depth_camera,
		efa,
		depth_vertex_shader,
		depth_fragment_shader
	);

	// render main scene
	pipeline_draw(
		PERSPECTIVE_CAMERA,
		persp_camera,
		plane,
		main_vertex_shader,
		main_fragment_shader
	);
	a++;
	
	pipeline_draw(
		PERSPECTIVE_CAMERA,
		persp_camera,
		efa,
		main_vertex_shader,
		main_fragment_shader
	);
	
	a++;
	if (a == 2) {
		a = 0;
	}
	// get_triangles_to_render_from_mesh(
	// 	efa,
	// 	&depth_camera->view_matrix,
	// 	&depth_camera->projection_matrix,
	// 	SHADOW_DEPTH_BUFFER_SIZE,
	// 	SHADOW_DEPTH_BUFFER_SIZE,
	// 	false
	// );
	// render_triangles(NULL, shadow_depth_buffer, NULL);
	// get_triangles_to_render_from_mesh(
	// 	plane,
	// 	&depth_camera->view_matrix,
	// 	&depth_camera->projection_matrix,
	// 	SHADOW_DEPTH_BUFFER_SIZE,
	// 	SHADOW_DEPTH_BUFFER_SIZE,
	// 	false
	// );
	// render_triangles(NULL, shadow_depth_buffer, NULL);

	// // render main scene
	// get_triangles_to_render_from_mesh(
	// 	efa,
	// 	&persp_camera->view_matrix,
	// 	&persp_camera->projection_matrix,
	// 	get_viewport_width(),
	// 	get_viewport_height(),
	// 	true
	// );
	// render_triangles(
	// 	get_screen_color_buffer(),
	// 	get_screen_depth_buffer(),
	// 	&depth_camera->view_projection_matrix
	// );
	// get_triangles_to_render_from_mesh(
	// 	plane,
	// 	&persp_camera->view_matrix,
	// 	&persp_camera->projection_matrix,
	// 	get_viewport_width(),
	// 	get_viewport_height(),
	// 	true
	// );
	// render_triangles(
	// 	get_screen_color_buffer(),
	// 	get_screen_depth_buffer(),
	// 	&depth_camera->view_projection_matrix
	// );
}

void shadow_map_example_free_resources(void) {
	dispose_meshes();
}