#include "stdio.h"
#include "math.h"
#include <time.h> 
#include "../utils.h"
#include "../array.h"
#include "../vector.h"
#include "../matrix.h"
#include "../camera.h"
#include "../display.h"
#include "../clipping.h"
#include "../geometry.h"
#include "../triangle.h"
#include "../pipeline.h"

#define SHADOW_DEPTH_BUFFER_SIZE 512
#define SHADOW_DEPTH_BUFFER_HALF_SIZE 256

static perspective_camera_t* persp_camera = NULL;
static orthographic_camera_t* depth_camera = NULL;
static depth_framebuffer* shadow_depth_buffer = NULL;
static mesh_t* efa = NULL;
static mesh_t* plane = NULL;
static time_t timer_elapsed_time;
static time_t next_time;

static vec3_t jet_rotation_target = { .x = 0, .y = 0, .z = 0 };
static vec3_t jet_position_target = { .x = 0, .y = 2, .z = 0 };
static vec3_t jet_position_dt_mult = { .x = 0.001, .y = 0.001, .z = 0.001 };
static vec3_t jet_position_dt_mult_target = { .x = 0.001, .y = 0.001, .z = 0.001 };

void shadow_map_example_setup(void) {
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

	vec3_t ortho_cam_position = { .x = 0, .y = 10, .z = 1.1 };
	vec3_t ortho_cam_target = { .x = 0, .y = 1, .z = 0 };
	depth_camera = make_orthographic_camera(
		-0.5,
		 0.5,
		 0.5,
		-0.5,
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
	efa->scale.x = 0.5;
	efa->scale.y = 0.5;
	efa->scale.z = 0.5;
	efa->translation.y = 2.0;

	plane = make_plane(5, 5, 10, 10);
	plane->rotation.x = M_PI / 2;

	timer_elapsed_time = time(NULL);
	next_time = timer_elapsed_time + (uint32_t)2;
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

void reorient_jet() {
	int r = rand() % 6;
	if (r == 0) {
		jet_rotation_target.x += M_PI * 2;
		jet_position_target.x = 1;
		jet_position_dt_mult_target.x = 0.003;
		jet_position_dt_mult_target.y = 0.001;
	} else if (r == 1) {
		jet_rotation_target.x -= M_PI * 2;
		jet_position_target.x = 1.5;
		jet_position_dt_mult_target.x = 0.001;
		jet_position_dt_mult_target.y = 0.003;
	} else if (r == 2) {
		jet_rotation_target.x += M_PI * 4;
		jet_position_target.x = -1.5;
		jet_position_dt_mult_target.x = 0.001;
		jet_position_dt_mult_target.y = 0.003;
	} else if (r == 3) {
		jet_rotation_target.x -= M_PI * 4;
		jet_position_target.x = -1.5;
		jet_position_dt_mult_target.x = 0.003;
		jet_position_dt_mult_target.y = 0.0025;
	} else if (r == 4) {
		jet_position_target.x = 1.2;
	} else if (r == 5) {
		jet_position_target.x = -0.7;
	}
}

void shadow_map_example_update(int delta_time, int elapsed_time) {
	timer_elapsed_time = time(NULL);
	if (timer_elapsed_time > next_time) {
		reorient_jet();
		next_time = time(NULL) + (uint32_t)2;
	}

	for (int i = 0; i < plane->vertices_count; i++) {
		vec3_t* vertex = &plane->vertices[i];
		vertex->z = sin(elapsed_time * 0.002 + vertex->x) * 0.2;
	}

	jet_position_target.z = sin(elapsed_time * 0.001) * 3;
	jet_position_target.y = cos(elapsed_time * 0.001) * 1 + 2;

	float old_translation_y = efa->translation.y;

	jet_position_dt_mult.x += (jet_position_dt_mult_target.x - jet_position_dt_mult.x) * (delta_time * 0.001);
	jet_position_dt_mult.y += (jet_position_dt_mult_target.y - jet_position_dt_mult.y) * (delta_time * 0.001);
	jet_position_dt_mult.z += (jet_position_dt_mult_target.z - jet_position_dt_mult.z) * (delta_time * 0.001);

	efa->translation.x += (jet_position_target.x - efa->translation.x) * (delta_time * jet_position_dt_mult.x);
	efa->translation.y += (jet_position_target.y - efa->translation.y) * (delta_time * jet_position_dt_mult.y);
	efa->translation.z += (jet_position_target.z - efa->translation.z) * (delta_time * jet_position_dt_mult.z);

	float y_delta = efa->translation.y - old_translation_y;

	efa->rotation.x += (jet_rotation_target.x - efa->rotation.x) * (delta_time * 0.0015);
	efa->rotation.y += (jet_rotation_target.y - efa->rotation.y) * (delta_time * 0.0015);

	efa->rotation.z += y_delta * 0.2;

	efa->rotation.z += (jet_rotation_target.z - efa->rotation.z) * (delta_time * 0.0015);
}

void depth_vertex_shader(
	int camera_type,
	void* camera,
	mesh_t* mesh,
	vertex_t* vertex
) {
	vertex->position.x *= SHADOW_DEPTH_BUFFER_HALF_SIZE;
	vertex->position.y *= SHADOW_DEPTH_BUFFER_HALF_SIZE;
	vertex->position.y *= -1;
	
	vertex->position.x += SHADOW_DEPTH_BUFFER_HALF_SIZE;
	vertex->position.y += SHADOW_DEPTH_BUFFER_HALF_SIZE;
}

fragment_shader_result_t depth_fragment_shader(
	int camera_type,
	void* camera,
	mesh_t* mesh,
	void* fs_inputs
) {
	fragment_shader_triangle_inputs* inputs = (fragment_shader_triangle_inputs*)fs_inputs;
	fragment_shader_result_t fs_out = {
		.depth_buffer = shadow_depth_buffer,
		.depth = 1 - inputs->interpolated_w
	};
	return fs_out;
}

static void main_vertex_shader(
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

static fragment_shader_result_t efa_triangle_fragment_shader(
	int camera_type,
	void* camera,
	mesh_t* mesh,
	void* fs_inputs
) {
	fragment_shader_triangle_inputs* inputs = (fragment_shader_triangle_inputs*)fs_inputs;
	fragment_shader_result_t fs_out = {
		.color_buffer = get_screen_color_buffer(),
		.depth_buffer = get_screen_depth_buffer(),
		.depth = inputs->interpolated_w,
		.color = sample_texture(mesh->texture, inputs->u, inputs->v)
	};
	return fs_out;
}

fragment_shader_result_t efa_line_fragment_shader(
	int camera_type,
	void* camera,
	mesh_t* mesh,
	void* fs_inputs
) {
	fragment_shader_result_t fs_out = {
		.color_buffer = get_screen_color_buffer(),
		.color = 0xff0000ff
	};
	return fs_out;
}

fragment_shader_result_t efa_points_fragment_shader(
	int camera_type,
	void* camera,
	mesh_t* mesh,
	void* fs_inputs
) {
	fragment_shader_result_t fs_out = {
		.color_buffer = get_screen_color_buffer(),
		.color = 0xff0000ff
	};
	return fs_out;
}

fragment_shader_result_t plane_fragment_shader(
	int camera_type,
	void* camera,
	mesh_t* mesh,
	void* fs_inputs
) {
	fragment_shader_triangle_inputs* inputs = (fragment_shader_triangle_inputs*)fs_inputs;

	fragment_shader_result_t fs_out = {
		.color_buffer = get_screen_color_buffer(),
		.depth_buffer = get_screen_depth_buffer(),
		.depth = inputs->interpolated_w
	};

	vec4_t pos = vec4_new(
		inputs->interpolated_world_space_pos_x,
		inputs->interpolated_world_space_pos_y,
		inputs->interpolated_world_space_pos_z,
		inputs->interpolated_w
	);

	mat4_t inverse_vp_matrix = mat4_mul_mat4(depth_camera->projection_matrix, depth_camera->view_matrix);
	vec4_t shadow_pos = mat4_mul_vec4_project(inverse_vp_matrix, pos);

	int shadow_x = shadow_pos.x * SHADOW_DEPTH_BUFFER_HALF_SIZE;
	int shadow_y = shadow_pos.y * SHADOW_DEPTH_BUFFER_HALF_SIZE;
	shadow_y *= -1;

	shadow_x += SHADOW_DEPTH_BUFFER_HALF_SIZE;
	shadow_y += SHADOW_DEPTH_BUFFER_HALF_SIZE;

	int idx = shadow_y * SHADOW_DEPTH_BUFFER_SIZE + shadow_x; // index in the shadowbuffer array

	fs_out.color = 0xffbbbbbb;

	if ((get_depth_buffer_at_idx(shadow_depth_buffer, idx) < (1 - shadow_pos.z))) {
		fs_out.color += 0xffeeeeee;
	}

	return fs_out;
}

void shadow_map_example_render(int delta_time, int elapsed_time) {
	
	clear_depth_buffer(shadow_depth_buffer);

	// render shadow map

	pipeline_draw(
		ORTHOGRAPHIC_CAMERA,
		depth_camera,
		efa,
		CULL_BACKFACE,
		RENDER_TRIANGLE,
		depth_vertex_shader,
		depth_fragment_shader
	);

	// render main scene

	pipeline_draw(
		PERSPECTIVE_CAMERA,
		persp_camera,
		plane,
		CULL_BACKFACE,
		RENDER_TRIANGLE,
		main_vertex_shader,
		plane_fragment_shader
	);
	
	pipeline_draw(
		PERSPECTIVE_CAMERA,
		persp_camera,
		efa,
		CULL_BACKFACE,
		RENDER_TRIANGLE,
		main_vertex_shader,
		efa_triangle_fragment_shader
	);
}

void shadow_map_example_free_resources(void) {
	dispose_meshes();
	destroy_depth_buffer(shadow_depth_buffer);
}