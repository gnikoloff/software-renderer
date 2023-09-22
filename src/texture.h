#ifndef TEXTURE_H
#define TEXTURE_H
#include <stdint.h>
#include "vector.h"
#include "upng.h"

typedef upng_t texture_2d_t;

typedef struct {
	int width;
	int height;
	texture_2d_t* face_textures;
} texture_cube_t;

typedef struct {
	float u;
	float v;
} tex2_t;

texture_2d_t* load_png_data(char* png_filename);
tex2_t tex2_clone(tex2_t* t);
uint32_t sample_texture(texture_2d_t* texture, float u, float v);
uint32_t sample_texture_bilinear(texture_2d_t* texture, float u, float v);

texture_cube_t make_cube_texture(char* textures_paths[6]);
uint32_t sample_cube_texture(texture_cube_t* cube_texture, vec3_t coord);

#endif