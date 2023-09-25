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
#include "../color.h"
#include "../pipeline.h"

#include "plasma-demo.h"

#define TUNNEL_TEXTURE_SIZE 128
#define TUNNEL_TEXTURE_SIZE_X2 256
#define TUNNEL_BUFFER_SIZE 128

static perspective_camera_t* persp_camera = NULL;
static mesh_t* mesh = NULL;
static uint32_t tunnel_texture[TUNNEL_TEXTURE_SIZE][TUNNEL_TEXTURE_SIZE];
static uint32_t distance_table[TUNNEL_TEXTURE_SIZE_X2][TUNNEL_TEXTURE_SIZE_X2];
static uint32_t angle_table[TUNNEL_TEXTURE_SIZE_X2][TUNNEL_TEXTURE_SIZE_X2];

static int vwidth = 0;
static int vheight = 0;
static int shift_x = 0;
static int shift_y = 0;
static int shift_look_x = 0;
static int shift_look_y = 0;

static float zoom_distance = 0;
static float zoom_distance_end = 0;

void tunnel_demo_setup(void) {
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

	vec3_t persp_cam_position = { .x = 0, .y = 0, .z = -6 };
	vec3_t persp_cam_target = { .x = 0, .y = 0, .z = 0 };
	persp_camera = make_perspective_camera(
		fovy,
		aspecty,
		z_near,
		z_far,
		persp_cam_position,
		persp_cam_target,
		4
	);
	
	init_frustum_planes(fovx, fovy, z_near, z_far);

	mesh = make_box(2, 2, 2, 1, 1, 1);

	for(int y = 0; y < TUNNEL_TEXTURE_SIZE; y++) {
  	for(int x = 0; x < TUNNEL_TEXTURE_SIZE; x++) {
			tunnel_texture[y][x] = (uint32_t)(x * 256 / TUNNEL_TEXTURE_SIZE) ^ (y * 256 / TUNNEL_TEXTURE_SIZE);
		}
	}

	 //generate non-linear transformation table
  for (int y = 0; y < TUNNEL_TEXTURE_SIZE_X2; y++) {
		for (int x = 0; x < TUNNEL_TEXTURE_SIZE_X2; x++) {
			int angle, distance;
			float ratio = 32.0;
			int w = TUNNEL_BUFFER_SIZE;
			int h = TUNNEL_BUFFER_SIZE;
			distance = (int)(ratio * TUNNEL_TEXTURE_SIZE / sqrt((float)((x - w) * (x - w) + (y - h) * (y - h)))) % TUNNEL_TEXTURE_SIZE;
    	angle = (int)(0.5 * TUNNEL_TEXTURE_SIZE * atan2((float)(y - h), (float)(x - w)) / 3.1416);
			distance_table[y][x] = distance;
			angle_table[y][x] = angle;
		}
	}
}

void tunnel_demo_process_input(SDL_Event* event, int delta_time) {
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

void tunnel_demo_update(int delta_time, int elapsed_time) {
	float animation = elapsed_time * 0.001;
	shift_x = (int)(TUNNEL_TEXTURE_SIZE * 1.0 * animation);
	shift_y = (int)(TUNNEL_TEXTURE_SIZE * 0.25 * animation);

	shift_look_x = TUNNEL_TEXTURE_SIZE / 2 + (int)(TUNNEL_TEXTURE_SIZE / 3 * sin(animation));
	shift_look_y = TUNNEL_TEXTURE_SIZE / 2 + (int)(TUNNEL_TEXTURE_SIZE / 3 * sin(animation * 2.0));

	mesh->rotation.x += delta_time * 0.00075;
	mesh->rotation.y += delta_time * 0.00075;
	mesh->rotation.z += delta_time * 0.00075;
}

static void vertex_shader(
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
	int x = (int)(fabs(inputs->u * TUNNEL_TEXTURE_SIZE)) % TUNNEL_TEXTURE_SIZE;
	int y = (int)(fabs(inputs->v * TUNNEL_TEXTURE_SIZE)) % TUNNEL_TEXTURE_SIZE;

	// uint32_t color = tunnel_texture[(uint)(distance_table[y][x] + shift_x) % TUNNEL_TEXTURE_SIZE][(uint)(angle_table[y][x] + shift_y) % TUNNEL_TEXTURE_SIZE];
	uint32_t color = tunnel_texture[
		(unsigned int)(distance_table[x + shift_look_x][y + shift_look_y] + shift_x) % TUNNEL_TEXTURE_SIZE
	][
		(unsigned int)(angle_table[x + shift_look_x][y + shift_look_y]+ shift_y) % TUNNEL_TEXTURE_SIZE
	];

	fragment_shader_result_t fs_out = {
		.color_buffer = get_screen_color_buffer(),
		.depth_buffer = get_screen_depth_buffer(),
		.depth = inputs->interpolated_w,
		.color = color
	};
	return fs_out;
}

void tunnel_demo_render(int delta_time, int elapsed_time) {
	pipeline_draw(
		PERSPECTIVE_CAMERA,
		persp_camera,
		mesh,
		CULL_NONE,
		RENDER_TRIANGLE,
		vertex_shader,
		fragment_shader
	);
}

void tunnel_demo_free_resources(void) {
	dispose_meshes();
}
