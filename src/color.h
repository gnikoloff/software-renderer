#ifndef COLOR_H
#define COLOR_H

typedef struct {
	int r;
	int g;
	int b;
} color_rgb_t;

color_rgb_t hsv_to_rgb(float h, float s, float v);
uint32_t rgb_to_uint32(color_rgb_t* color);

#endif