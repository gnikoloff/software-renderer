#ifndef LIGHT_H
#define LIGHT_H

#include "vector.h"
#include "stdint.h"

typedef struct {
	vec3_t direction;
} light_t;

void init_light(vec3_t direction);
vec3_t* get_light_direction(void);
uint32_t light_apply_intensity(uint32_t color, float percentage_factor);

#endif