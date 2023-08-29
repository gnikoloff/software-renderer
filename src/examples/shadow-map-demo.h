#ifndef SHADOWMAP_DEMO_H
#define SHADOWMAP_DEMO_H

#include <SDL2/SDL.h>
#include "../framebuffer.h"

void shadow_map_example_setup(void);
void shadow_map_example_process_input(SDL_Event* event, int delta_time);
void shadow_map_example_free_resources(void);
void shadow_map_example_update(int delta_time);
void shadow_map_example_render(int delta_time);

#endif