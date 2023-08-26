#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "math.h"
#include "upng.h"
#include "utils.h"
#include "texture.h"

uint32_t get_pixel(upng_t* texture, int x, int y) {
	int texture_width = upng_get_width(texture);
	int texture_height = upng_get_height(texture);
	if (x < 0 || x >= texture_width || y < 0 || y >= texture_height) {
		// return a nice debug color
		return 0xff0000ff;
	}
	uint32_t* texture_buffer = (uint32_t*)upng_get_buffer(texture);
	return texture_buffer[y * texture_width + x];
}

tex2_t tex2_clone(tex2_t* t) {
	tex2_t result = { .u = t->u, .v = t->v };
	return result;
};

uint32_t sample_texture(upng_t* texture, float u, float v) {
	int texture_width = upng_get_width(texture);
	int texture_height = upng_get_height(texture);
	int tex_x = abs((int)(u * texture_width)) % texture_width;
	int tex_y = abs((int)(v * texture_height)) % texture_height;
	if (tex_x < 0 || tex_x >= texture_width || tex_y < 0 || tex_y >= texture_height) {
		// return a nice debug color
		return 0xff0000ff;
	}
	uint32_t* texture_buffer = (uint32_t*)upng_get_buffer(texture);
	return texture_buffer[tex_y * texture_width + tex_x];
}

// https://gist.github.com/NickBeeuwsaert/5753386
uint32_t sample_texture_bilinear(upng_t* texture, float u, float v) {
	int texture_width = upng_get_width(texture);
	int texture_height = upng_get_height(texture);
	float tex_x = fabs(u * (float)(texture_width - 1));
	float tex_y = fabs(v * (float)(texture_height - 1));
	int gxi = (int)tex_x;
	int gyi = (int)tex_y;
	if (gxi < 0 || gxi >= texture_width - 1 || gyi < 0 || gyi >= texture_height - 1) {
		// return a nice debug color
		return 0xff0000ff;
	}
	uint32_t t00 = get_pixel(texture, gxi, gyi);
	uint32_t t01 = get_pixel(texture, gxi + 1, gyi);
	uint32_t t10 = get_pixel(texture, gxi, gyi + 1);
	uint32_t t11 = get_pixel(texture, gxi + 1, gyi + 1);

	uint32_t result = 0;
	
	for(int i = 0; i < sizeof(result); i++){
		result |= (uint8_t)blerp(
			GET_BYTE(t00, i),
			GET_BYTE(t10, i),
			GET_BYTE(t01, i),
			GET_BYTE(t11, i),
			(tex_x - gxi),
			(tex_y - gyi)
		) << (8 * i);
	}

	return result;
}
