#ifndef DEPTHBUFFER_DEMO_H
#define DEPTHBUFFER_DEMO_H

#include <SDL2/SDL.h>

void depth_buffer_example_setup(void);
void depth_buffer_example_process_input(SDL_Event* event, int delta_time);
void depth_buffer_example_free_resources(void);
void depth_buffer_example_update(int delta_time, int elapsed_time);
void depth_buffer_example_render(int delta_time, int elapsed_time);

#endif