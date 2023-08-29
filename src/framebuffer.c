#include <stdlib.h>
#include <assert.h>
#include "stdio.h"
#include "framebuffer.h"

static color_framebuffer color_framebuffers[MAX_NUM_COLORBUFFERS];
static depth_framebuffer depth_framebuffers[MAX_NUM_DEPTHBUFFERS];
static int color_framebuffer_count = 0;
static int depth_framebuffer_count = 0;

color_framebuffer* make_color_buffer(int width, int height) {
	printf("Create color framebuffer %d\n", color_framebuffer_count);
	assert(color_framebuffer_count < MAX_NUM_COLORBUFFERS);
	color_framebuffer* framebuffer = &color_framebuffers[color_framebuffer_count];

	framebuffer->width = width;
	framebuffer->height = height;
	framebuffer->buffer = (uint32_t*)malloc(sizeof(uint32_t) * width * height);

	color_framebuffer_count++;

	return framebuffer;
}

void update_color_buffer_at(color_framebuffer* framebuffer, int x, int y, uint32_t color) {
	if (x < 0 || x >= framebuffer->width || y < 0 || y >= framebuffer->height) {
		return;
	}
	
	framebuffer->buffer[y * framebuffer->width + x] = color;
}

void clear_color_buffer(color_framebuffer* framebuffer, uint32_t color) {
	assert(framebuffer != NULL);
	for (int i = 0; i < framebuffer->width * framebuffer->height; i++) {
		framebuffer->buffer[i] = color;
	}
}
void destroy_color_buffer(color_framebuffer* framebuffer) {
	free(framebuffer->buffer);
}

depth_framebuffer* make_depth_buffer(int width, int height) {
	printf("Create depth framebuffer %d\n", depth_framebuffer_count);
	assert(depth_framebuffer_count < MAX_NUM_COLORBUFFERS);
	depth_framebuffer* framebuffer = &depth_framebuffers[depth_framebuffer_count];

	framebuffer->width = width;
	framebuffer->height = height;
	framebuffer->buffer = (float*)malloc(sizeof(float) * width * height);	

	clear_depth_buffer(framebuffer);

	depth_framebuffer_count++;

	return framebuffer;
}

void clear_depth_buffer(depth_framebuffer* framebuffer) {
	for (int i = 0; i < framebuffer->width * framebuffer->height; i++) {
		framebuffer->buffer[i] = 1.0f;
	}
}

depth_framebuffer* get_depth_buffer(int idx) {
	return &depth_framebuffers[idx];
}

float get_depth_buffer_at(depth_framebuffer* framebuffer, int x, int y) {
	if (x < 0 || x >= framebuffer->width || y < 0 || y >= framebuffer->height) {
		return 1.0;
	}
	float depth = framebuffer->buffer[y * framebuffer->width + x];
	return depth;
}

float get_depth_buffer_at_idx(depth_framebuffer* framebuffer, int idx) {
	if (idx < 0 || idx >= framebuffer->width * framebuffer->height) {
		return 1.0;
	}
	return framebuffer->buffer[idx];
}

void update_depth_buffer_at(depth_framebuffer* framebuffer, int x, int y, float value) {
	if (x < 0 || x >= framebuffer->width || y < 0 || y >= framebuffer->height) {
		return;
	}
	framebuffer->buffer[y * framebuffer->width + x] = value;
}

void destroy_depth_buffer(depth_framebuffer* framebuffer) {
	free(framebuffer->buffer);
	depth_framebuffer_count--;
}
