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

#include "depth-buffer-demo.h"

#define BOXES_COUNT 6
#define BOX_SIDES 6

static texture_cube_t cube_texture;
static perspective_camera_t* persp_camera = NULL;
static mesh_t* sphere = NULL;
static mesh_t* boxes[BOXES_COUNT];
static mesh_t* skybox_sides[BOX_SIDES];
static vec3_t boxes_positions[BOXES_COUNT] = {
	{ .x = 5, .y = 0, .z = 0 },
	{ .x = -5, .y = 0, .z = 0 },
	{ .x = 0, .y = 5, .z = 0 },
	{ .x = 0, .y = -5, .z = 0 },
	{ .x = 0, .y = 0, .z = 5 },
	{ .x = 0, .y = 0, .z = -5 }
};
static vec3_t skybox_positions[BOX_SIDES] = {
	{ .x = 9, .y = 0, .z = 0 },
	{ .x = -9, .y = 0, .z = 0 },
	{ .x = 0, .y = 9, .z = 0 },
	{ .x = 0, .y = -9, .z = 0 },
	{ .x = 0, .y = 0, .z = 9 },
	{ .x = 0, .y = 0, .z = -9 }
};
static vec3_t skybox_rotations[BOX_SIDES] = {
	{ .x = 0, .y = M_PI / 2, .z = 0 },
	{ .x = 0, .y = -M_PI / 2, .z = 0 },
	{ .x = -M_PI / 2, .y = 0, .z = 0 },
	{ .x = M_PI / 2, .y = 0, .z = 0 },
	{ .x = 0, .y = 0, .z = 0 },
	{ .x = 0, .y = M_PI, .z = 0 }
};
static char* skybox_images[BOX_SIDES] = {
	"./assets/debug.png",
	"./assets/debug.png",
	"./assets/debug.png",
	"./assets/debug.png",
	"./assets/debug.png",
	"./assets/debug.png"
};

static int vwidth = 0;
static int vheight = 0;

static float zoom_distance = 0;
static float zoom_distance_end = 0;

void environment_mapping_example_setup(void) {
	vwidth = get_viewport_width();
	vheight = get_viewport_height();

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
	
	init_frustum_planes(fovx, fovy, z_near, z_far);

	sphere = make_sphere(1, 20, 20, 0, M_PI * 2, 0, M_PI);

	cube_texture = make_cube_texture(skybox_images);

	for (int i = 0; i < BOXES_COUNT; i++) {
		boxes[i] = make_box(1, 1, 1, 1, 1, 1);
		boxes[i]->scale = vec3_new(0.2, 0.2, 0.2);
		boxes[i]->translation = boxes_positions[i];
	}

	for (int i = 0; i < BOX_SIDES; i++) {
		skybox_sides[i] = make_plane(18, 18, 1, 1);
		skybox_sides[i]->rotation = skybox_rotations[i];
		skybox_sides[i]->translation = skybox_positions[i];
		skybox_sides[i]->texture = &cube_texture.face_textures[i];
	}
}

void environment_mapping_example_process_input(SDL_Event* event, int delta_time) {
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
		case SDL_FINGERDOWN: {
			SDL_Finger *finger_0 = SDL_GetTouchFinger(event->tfinger.touchId, 0);
			SDL_Finger *finger_1 = SDL_GetTouchFinger(event->tfinger.touchId, 1);

			if (finger_0 != NULL && finger_1 != NULL) {
				float dx = finger_1->x - finger_0->x;
				float dy = finger_1->y - finger_0->y;
				zoom_distance = sqrtf(dx * dx + dy * dy);
			}
		}
		case SDL_FINGERMOTION: {
			SDL_Finger *finger_0 = SDL_GetTouchFinger(event->tfinger.touchId, 0);
			SDL_Finger *finger_1 = SDL_GetTouchFinger(event->tfinger.touchId, 1);

			if (finger_0 != NULL && finger_1 != NULL) {
				float dx = finger_1->x - finger_0->x;
				float dy = finger_1->y - finger_0->y;
				zoom_distance_end = sqrtf(dx * dx + dy * dy);
				float dist = zoom_distance_end - zoom_distance;

				persp_camera->distance += -dist * delta_time;
				update_camera_on_drag(persp_camera, 0, 0);

				zoom_distance = zoom_distance_end;
			}

			break;
		}
	}
}

void environment_mapping_example_update(int delta_time, int elapsed_time) {
	// ...
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

static fragment_shader_result_t fragment_shader(
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
		.color = mesh->texture == NULL ? 0xff0000ff : sample_texture(mesh->texture, inputs->u, inputs->v)
	};
	return fs_out;
}

static fragment_shader_result_t fragment_shader_sphere(
	int camera_type,
	void* camera,
	mesh_t* mesh,
	void* fs_inputs
) {
	fragment_shader_triangle_inputs* inputs = (fragment_shader_triangle_inputs*)fs_inputs;
	vec3_t normal = vec3_new(inputs->interpolated_normal_x, inputs->interpolated_normal_y, inputs->interpolated_normal_z);
	vec3_normalize(&normal);

	perspective_camera_t* persp_camera = (perspective_camera_t*)camera;

	vec3_t eye_to_surface_dir = vec3_sub(mesh->translation, persp_camera->position);
	vec3_t direction = vec3_reflect(eye_to_surface_dir, normal);

	fragment_shader_result_t fs_out = {
		.color_buffer = get_screen_color_buffer(),
		.depth_buffer = get_screen_depth_buffer(),
		.depth = inputs->interpolated_w,
		.color = sample_cube_texture(&cube_texture, direction)
	};
	return fs_out;
}

void environment_mapping_example_render(int delta_time, int elapsed_time) {
	
	pipeline_draw(
		PERSPECTIVE_CAMERA,
		persp_camera,
		sphere,
		CULL_BACKFACE,
		RENDER_TRIANGLE,
		main_vertex_shader,
		fragment_shader_sphere
	);

	// for (int i = 0; i < BOXES_COUNT; i++) {
	// 	mesh_t* box = boxes[i];
	// 	pipeline_draw(
	// 		PERSPECTIVE_CAMERA,
	// 		persp_camera,
	// 		box,
	// 		CULL_BACKFACE,
	// 		RENDER_TRIANGLE,
	// 		main_vertex_shader,
	// 		fragment_shader
	// 	);
	// }

	for (int i = 0; i < BOX_SIDES; i++) {
		mesh_t* skybox_side = skybox_sides[i];
		pipeline_draw(
			PERSPECTIVE_CAMERA,
			persp_camera,
			skybox_side,
			CULL_BACKFACE,
			RENDER_TRIANGLE,
			main_vertex_shader,
			fragment_shader
		);
	}
}

void environment_mapping_example_free_resources(void) {
	dispose_meshes();
}