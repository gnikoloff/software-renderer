#ifndef TUNNEL_DEMO_H
#define TUNNEL_DEMO_H

#include <SDL2/SDL.h>

void tunnel_demo_setup(void);
void tunnel_demo_process_input(SDL_Event* event, int delta_time);
void tunnel_demo_free_resources(void);
void tunnel_demo_update(int delta_time, int elapsed_time);
void tunnel_demo_render(int delta_time, int elapsed_time);

#endif