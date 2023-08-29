#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <SDL2/SDL.h>
#include "display.h"

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture* color_buffer_texture = NULL;

static int window_width = 0;
static int window_height = 0;

static color_framebuffer* color_buffer = NULL;
static depth_framebuffer* z_buffer = NULL;

static int render_method = 0;
static int cull_method = 0;

color_framebuffer* get_screen_color_buffer(void) {
	return color_buffer;
}

depth_framebuffer* get_screen_depth_buffer(void) {
	return z_buffer;
}

int get_viewport_width(void) {
	return window_width;
}

int get_viewport_height(void) {
	return window_height;
}

void set_render_method(int method) {
	render_method = method;
}

void set_cull_method(int method) {
	cull_method = method;
}

int is_cull_backface(void) {
	return cull_method == CULL_BACKFACE;
}

int get_cull_method(void) {
	return cull_method;
}

bool should_render_filled_triangles(void) {
	return render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE;
}

bool should_render_textured_triangles(void) {
	return render_method == RENDER_TEXTURED || render_method == RENDER_TEXTURED_WIRE;
}

bool should_render_wireframe(void) {
	return render_method == RENDER_WIRE || render_method == RENDER_WIRE_VERTEX || render_method == RENDER_FILL_TRIANGLE_WIRE || render_method == RENDER_TEXTURED_WIRE;
}

bool should_render_vertex(void) {
	return render_method == RENDER_WIRE_VERTEX;
}

float get_depth_at(int x, int y) {
	return get_depth_buffer_at(z_buffer, x, y);
}

void update_depth_at(int x, int y, float value) {
	update_depth_buffer_at(z_buffer, x, y, value);
}

#ifdef __EMSCRIPTEN__
EM_BOOL emsc_window_size_changed(int eventType, const EmscriptenUiEvent *e, void *rawState) {
	printf("fired\n");
	double fullscreen_width;
	double fullscreen_height;
	emscripten_get_element_css_size("canvas", &fullscreen_width, &fullscreen_height);
	emscripten_set_canvas_element_size("canvas", 100, 100);
	return 1;
}
#endif

bool initialize_window(void) {
	int sdl_success;
	#ifdef __EMSCRIPTEN__
		sdl_success = SDL_Init(SDL_INIT_VIDEO);
	#else
		sdl_success = SDL_Init(SDL_INIT_EVERYTHING);
	#endif

	if (sdl_success != 0) {
		fprintf(stderr, "Error initializing SDL.\n");
		return false;
	}


	double fullscreen_width;
	double fullscreen_height;

	#ifdef __EMSCRIPTEN__
		emscripten_get_element_css_size("canvas", &fullscreen_width, &fullscreen_height);
		emscripten_set_canvas_element_size("canvas", (int)fullscreen_width, (int)fullscreen_height);
		EMSCRIPTEN_RESULT is_set_callback = emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, NULL, true, emsc_window_size_changed);
		if (is_set_callback != EMSCRIPTEN_RESULT_SUCCESS) {
			printf("Unable to set resize callback!\n");
		}
	#else
		SDL_DisplayMode display_mode;
		SDL_GetCurrentDisplayMode(0, &display_mode);
		fullscreen_width = display_mode.w;
		fullscreen_height = display_mode.h;
	#endif

	window_width = (int)(fullscreen_width);
	window_height = (int)(fullscreen_height);
	
	window = SDL_CreateWindow(
		NULL,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		fullscreen_width,
		fullscreen_height,
		SDL_WINDOW_BORDERLESS
	);

	if (!window) {
		fprintf(stderr, "Error creating SDL window");
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) {
		fprintf(stderr, "Error creating SDL renderer");
		return false;
	}

	#ifndef __EMSCRIPTEN__
		// SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
		SDL_SetWindowSize(window, window_width, window_height);
	#endif
	
	color_buffer = make_color_buffer(window_width, window_height);
	z_buffer = make_depth_buffer(window_width, window_height);

	color_buffer_texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_RGBA32,
		SDL_TEXTUREACCESS_STREAMING,
		window_width,
		window_height
	);

	return true;
}

void clear_color(uint32_t color) {
	clear_color_buffer(color_buffer, color);
}

void clear_depth() {
	clear_depth_buffer(z_buffer);
}

inline void draw_rect(int x, int y, int width, int height, uint32_t color) {
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			int current_x = x + i;
			int current_y = y + j;
			update_color_buffer_at(color_buffer, current_x, current_y, color);
		}
	}
}

inline void draw_line(
	int x0, int y0,
	int x1, int y1,
	uint32_t color,
	color_framebuffer* color_buffer
) {
	int delta_x = x1 - x0;
	int delta_y = y1 - y0;

	int longest_side_length = abs(delta_x) >= abs(delta_y) ? abs(delta_x) : abs(delta_y);

	float x_inc = delta_x / (float)longest_side_length;
	float y_inc = delta_y / (float)longest_side_length;

	float current_x = x0;
	float current_y = y0;

	for (int i = 0; i <= longest_side_length; i++) {
		update_color_buffer_at(color_buffer, (current_x), round(current_y), color);
		current_x += x_inc;
		current_y += y_inc;
	}
}

void render_color_buffer(void) {
	
	SDL_UpdateTexture(
		color_buffer_texture,
		NULL,
		color_buffer->buffer,
		(int)(window_width * sizeof(uint32_t))
	);
	SDL_RenderCopy(
		renderer,
		color_buffer_texture,
		NULL,
		NULL
	);
	SDL_RenderPresent(renderer);
}

void destroy_window(void) {
	destroy_color_buffer(color_buffer);
	destroy_depth_buffer(z_buffer);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
