#ifndef PLASMA_DEMO_H
#define PLASMA_DEMO_H

#include <SDL2/SDL.h>

void plasma_demo_setup(void);
void plasma_demo_process_input(SDL_Event* event, int delta_time);
void plasma_demo_free_resources(void);
void plasma_demo_update(int delta_time, int elapsed_time);
void plasma_demo_render(int delta_time, int elapsed_time);

#endif