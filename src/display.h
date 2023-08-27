#ifndef DISPLAY_H
#define DISPLAY_H

#ifdef __EMSCRIPTEN__
	#include "emscripten/html5.h"
#endif
#include <stdbool.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include "framebuffer.h"

#define FPS 60
#define FRAME_TARGET_TIME (1000 / FPS)

enum cull_method {
	CULL_NONE,
	CULL_BACKFACE
};

enum render_method {
	RENDER_WIRE,
	RENDER_WIRE_VERTEX,
	RENDER_FILL_TRIANGLE,
	RENDER_FILL_TRIANGLE_WIRE,
	RENDER_TEXTURED,
	RENDER_TEXTURED_WIRE
};

#ifdef __EMSCRIPTEN__
EM_BOOL emsc_window_size_changed(int eventType, const EmscriptenUiEvent *e, void *rawState);
#endif

bool initialize_window(void);
void destroy_window(void);

int get_viewport_width(void);
int get_viewport_height(void);

void set_render_method(int render_method);
void set_cull_method(int cull_method);
int get_render_method(void);
int is_cull_backface(void);

bool should_render_filled_triangles(void);
bool should_render_textured_triangles(void);
bool should_render_wireframe(void);
bool should_render_vertex(void);
void draw_pixel(int x, int y, uint32_t color);
void draw_rect(int start_x, int start_y, int width, int height, uint32_t color);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
void render_color_buffer(void);
void clear_color(uint32_t color);
void clear_depth();
depth_framebuffer* get_screen_depth_buffer(void);

#endif