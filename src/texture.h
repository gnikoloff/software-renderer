#ifndef TEXTURE_H
#define TEXTURE_H
#include <stdint.h>
#include "upng.h"

typedef struct {
	float u;
	float v;
} tex2_t;

tex2_t tex2_clone(tex2_t* t);
uint32_t sample_texture(upng_t* texture, float u, float v);
uint32_t sample_texture_bilinear(upng_t* texture, float u, float v);

#endif