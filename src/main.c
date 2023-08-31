#ifdef __EMSCRIPTEN__
	#include "emscripten.h"
#endif
#include <time.h>
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

#ifdef SHADOWMAP_EXAMPLE
#include "examples/shadow-map-demo.h"
#endif

#ifdef PHYSICS2D_EXAMPLE
#include "examples/physics-2d.h"
#endif

bool is_running = false;
int previous_frame_time = 0;
int delta_time = 0;

void setup(void) {
	srand(time(NULL));
	// geometry_example_setup();
	#ifdef SHADOWMAP_EXAMPLE
		shadow_map_example_setup();
	#endif
	#ifdef PHYSICS2D_EXAMPLE
		physics2D_example_setup();
	#endif
}

void process_input(void) {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
			is_running = false;
			return;
		}
		// geometry_example_process_input(&event, delta_time);
		#ifdef SHADOWMAP_EXAMPLE
			shadow_map_example_process_input(&event, delta_time);
		#endif
		#ifdef PHYSICS2D_EXAMPLE
			physics2D_example_process_input(&event, delta_time);
		#endif
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

	// geometry_example_update(delta_time);
	#ifdef SHADOWMAP_EXAMPLE
		shadow_map_example_update(delta_time, now);
	#endif
	#ifdef PHYSICS2D_EXAMPLE
		physics2D_example_update(delta_time, now);
	#endif
}

void render(void) {
	int now = SDL_GetTicks();
	clear_color(0xFF111111);
	clear_depth();
	// geometry_example_render(delta_time);
	#ifdef SHADOWMAP_EXAMPLE
		shadow_map_example_render(delta_time, now);
	#endif
	#ifdef PHYSICS2D_EXAMPLE
		physics2D_example_render(delta_time, now);
	#endif
	render_color_buffer();
}

void free_resources(void) {
	// geometry_example_free_resources();
	#ifdef SHADOWMAP_EXAMPLE
		shadow_map_example_free_resources();
	#endif
	#ifdef PHYSICS2D_EXAMPLE
		physics2D_example_free_resources();
	#endif
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