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

#define PLASMA_BUFFER_SIZE 128

static perspective_camera_t* persp_camera = NULL;
static mesh_t* mesh = NULL;
static uint32_t plasma[PLASMA_BUFFER_SIZE][PLASMA_BUFFER_SIZE];
static uint32_t palette[256];

static int vwidth = 0;
static int vheight = 0;
static int paletteShift;

static float zoom_distance = 0;
static float zoom_distance_end = 0;

void plasma_demo_setup(void) {
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

	for(int y = 0; y < PLASMA_BUFFER_SIZE; y++) {
  	for(int x = 0; x < PLASMA_BUFFER_SIZE; x++) {
			//the plasma buffer is a sum of sines
			uint32_t color = (
        128.0 + (128.0 * sin(x / 8.0)) +
      	128.0 + (128.0 * sin(y / 8.0)) +
      	128.0 + (128.0 * sin(sqrt((double)((x - PLASMA_BUFFER_SIZE / 2.0)* (x - PLASMA_BUFFER_SIZE / 2.0) + (y - PLASMA_BUFFER_SIZE / 2.0) * (y - PLASMA_BUFFER_SIZE / 2.0))) / 8.0)) +
      	128.0 + (128.0 * sin(sqrt((double)(x * x + y * y)) / 32.0))
    	) / 4;
    	plasma[y][x] = color;
		}
	}

	color_rgb_t color_rgb;
  for(int x = 0; x < 256; x++) {
		color_rgb.r = 128.0 + 128 * sin(3.1415 * x / 32.0);
		color_rgb.g = 128.0 + 128 * sin(3.1415 * x / 64.0);
		color_rgb.b = 128.0 + 128 * sin(3.1415 * x / 128.0);
		palette[x] = rgb_to_uint32(&color_rgb);
  }
}

void plasma_demo_process_input(SDL_Event* event, int delta_time) {
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

void plasma_demo_update(int delta_time, int elapsed_time) {
	paletteShift = elapsed_time / 10.0;
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

	int x = (int)(fabs(inputs->u * PLASMA_BUFFER_SIZE)) % PLASMA_BUFFER_SIZE;
	int y = (int)(fabs(inputs->v * PLASMA_BUFFER_SIZE)) % PLASMA_BUFFER_SIZE;
	uint32_t color = palette[(plasma[y][x] + paletteShift) % 256];

	fragment_shader_result_t fs_out = {
		.color_buffer = get_screen_color_buffer(),
		.depth_buffer = get_screen_depth_buffer(),
		.depth = inputs->interpolated_w,
		.color = color
	};
	return fs_out;
}

void plasma_demo_render(int delta_time, int elapsed_time) {
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

void plasma_demo_free_resources(void) {
	dispose_meshes();
}
