#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>

#define MAX_NUM_COLORBUFFERS 5
#define MAX_NUM_DEPTHBUFFERS 5

typedef struct {
	int width;
	int height;
	uint32_t* buffer;
} color_framebuffer;

typedef struct {
	int width;
	int height;
	float* buffer;
} depth_framebuffer;

color_framebuffer* make_color_buffer(int width, int height);
void update_color_buffer_at(color_framebuffer* framebuffer, int x, int y, uint32_t color);
void clear_color_buffer(color_framebuffer* framebuffer, uint32_t color);
void destroy_color_buffer(color_framebuffer* framebuffer);

depth_framebuffer* make_depth_buffer(int width, int height);
void clear_depth_buffer(depth_framebuffer* framebuffer);
float get_depth_buffer_at(depth_framebuffer* framebuffer, int x, int y);
void update_depth_buffer_at(depth_framebuffer* framebuffer, int x, int y, float value);
void destroy_depth_buffer(depth_framebuffer* framebuffer);

#endif