#ifdef __EMSCRIPTEN__
	#include "emscripten.h"
#endif
#include "stdio.h"
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
#include "clipping.h"
#include "geometry.h"

// #include "examples/geometry-demo.h"
#include "examples/shadow-map-demo.h"

bool is_running = false;
int previous_frame_time = 0;
int delta_time = 0;

void setup(void) {
	set_render_method(RENDER_WIRE);
	set_cull_method(CULL_BACKFACE);
	// geometry_example_setup();
	shadow_map_example_setup();
}

void process_input(void) {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
			is_running = false;
			return;
		}
		if (event.key.keysym.sym == SDLK_1) {
			set_render_method(RENDER_FILL_TRIANGLE);
			return;
		}
		if (event.key.keysym.sym == SDLK_2) {
			set_render_method(RENDER_FILL_TRIANGLE_WIRE);
			return;
		}
		if (event.key.keysym.sym == SDLK_3) {
			set_render_method(RENDER_WIRE);
			return;
		}
		if (event.key.keysym.sym == SDLK_4) {
			set_render_method(RENDER_WIRE_VERTEX);
			return;
		}
		if (event.key.keysym.sym == SDLK_5) {
			set_render_method(RENDER_TEXTURED);
			return;
		}
		if (event.key.keysym.sym == SDLK_6) {
			set_render_method(RENDER_TEXTURED_WIRE);
			return;
		}
		if (event.key.keysym.sym == SDLK_c) {
			set_cull_method(CULL_BACKFACE);
			return;
		}
		if (event.key.keysym.sym == SDLK_x) {
			set_cull_method(CULL_NONE);
			return;
		}

		shadow_map_example_process_input(&event, delta_time);
		// geometry_example_process_input(&event, delta_time);
	}
}

void update(void) {
	int now = SDL_GetTicks();
	int time_to_wait = FRAME_TARGET_TIME - (now - previous_frame_time);
	if (time_to_wait > 0 && time_to_wait < FRAME_TARGET_TIME) {
		SDL_Delay(time_to_wait);
	}

	delta_time = (SDL_GetTicks() - previous_frame_time);
	previous_frame_time = SDL_GetTicks();

	shadow_map_example_update(delta_time, now);
	// geometry_example_update(delta_time);
}

void render(void) {
	int now = SDL_GetTicks();
	clear_color(0xFF111111);
	clear_depth();
	shadow_map_example_render(delta_time, now);
	// geometry_example_render(delta_time);
	render_color_buffer();
}

void free_resources(void) {
	shadow_map_example_free_resources();
	// geometry_example_free_resources();
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