#ifndef ENVIRONMENTMAPPING_DEMO_H
#define ENVIRONMENTMAPPING_DEMO_H

#include <SDL2/SDL.h>

void environment_mapping_example_setup(void);
void environment_mapping_example_process_input(SDL_Event* event, int delta_time);
void environment_mapping_example_free_resources(void);
void environment_mapping_example_update(int delta_time, int elapsed_time);
void environment_mapping_example_render(int delta_time, int elapsed_time);

#endif