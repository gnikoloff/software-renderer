#ifndef PHYSICS2D_H
#define PHYSICS2D_H

#include <SDL2/SDL.h>

typedef struct {
	float x0, y0, x1, y1, mid_x, mid_y;
	vec4_t bounding_box;
} line_t;

typedef struct {
	float x, y, vx, vy, screen_padding, radius;
} particle_t;

void physics2D_example_setup(void);
void physics2D_example_process_input(SDL_Event* event, int delta_time);
void physics2D_example_free_resources(void);
void physics2D_example_update(int delta_time, int elapsed_time);
void physics2D_example_render(int delta_time, int elapsed_time);

#endif