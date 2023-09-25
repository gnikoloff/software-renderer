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

#ifdef __EMSCRIPTEN__
EM_BOOL emsc_window_size_changed(int eventType, const EmscriptenUiEvent *e, void *rawState);
#endif

bool initialize_window(void);
void destroy_window(void);

color_framebuffer* get_screen_color_buffer(void);
depth_framebuffer* get_screen_depth_buffer(void);

int get_viewport_width(void);
int get_viewport_height(void);

void set_window_scale(float scale);

void draw_rect_on_screen(int start_x, int start_y, int width, int height, uint32_t color, color_framebuffer* color_buffer);
void draw_line_on_screen(int x0, int y0, int x1, int y1, uint32_t color);
void render_color_buffer(void);
void clear_color(uint32_t color);
void clear_depth();

#endif