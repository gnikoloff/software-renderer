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

static perspective_camera_t* persp_camera = NULL;
static mesh_t* efa = NULL;

static int vwidth = 0;
static int vheight = 0;
static int mask_right_border_x = 0;
static float mask_border_velocity = 0.5;

static float zoom_distance = 0;
static float zoom_distance_end = 0;

void depth_buffer_example_setup(void) {
	vwidth = get_viewport_width();
	vheight = get_viewport_height();

	mask_right_border_x = 0;

	float vwidthf = (float)vwidth;
	float vheightf = (float)vheight;

	float aspectx = vwidthf / vheightf;
	float aspecty = vheightf / vwidthf;
	float fovy = 3.141592 / 3.0;
	float fovx = atan(tan(fovy / 2) * aspectx) * 2;
	float z_near = 1.0;
	float z_far = 30.0;

	vec3_t persp_cam_position = { .x = 0, .y = 0, .z = -4 };
	vec3_t persp_cam_target = { .x = 0, .y = 0, .z = 0 };
	persp_camera = make_perspective_camera(
		fovy,
		aspecty,
		z_near,
		z_far,
		persp_cam_position,
		persp_cam_target,
		1
	);
	
	init_frustum_planes(fovx, fovy, z_near, z_far);

	efa = load_mesh(
		"./assets/crab.obj",
		"./assets/crab.png",
		vec3_new(1, 1, 1),
		vec3_new(0, 0, 0),
		vec3_new(0, 0, 0)
	);
	efa->scale.x = 0.5;
	efa->scale.y = 0.5;
	efa->scale.z = 0.5;
}

void depth_buffer_example_process_input(SDL_Event* event, int delta_time) {
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

void depth_buffer_example_update(int delta_time, int elapsed_time) {
	mask_right_border_x += delta_time * mask_border_velocity;
	if (mask_right_border_x <= 0 || mask_right_border_x >= vwidth) {
		mask_border_velocity *= -1;
	}
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
	uint8_t a = round((1 - inputs->interpolated_w) * 255.0);
	uint8_t depth_color_arr[4] = {1, a, a, a};
	uint32_t depth_color = u8_to_u32(depth_color_arr);
	bool is_mask = inputs->x < mask_right_border_x;
	fragment_shader_result_t fs_out = {
		.color_buffer = get_screen_color_buffer(),
		.depth_buffer = get_screen_depth_buffer(),
		.depth = inputs->interpolated_w,
		.color = is_mask ? depth_color : sample_texture(mesh->texture, inputs->u, inputs->v)
	};
	return fs_out;
}

void depth_buffer_example_render(int delta_time, int elapsed_time) {
	
	pipeline_draw(
		PERSPECTIVE_CAMERA,
		persp_camera,
		efa,
		CULL_BACKFACE,
		RENDER_TRIANGLE,
		main_vertex_shader,
		efa_triangle_fragment_shader
	);

	draw_line_on_screen(mask_right_border_x, 0, mask_right_border_x, vheight, 0xffff0000);
}

void depth_buffer_example_free_resources(void) {
	dispose_meshes();
}