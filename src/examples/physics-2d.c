#include "stdio.h"
#include "math.h"
#include <time.h> 
#include <SDL2/SDL.h>
#include "../utils.h"
#include "../array.h"
#include "../vector.h"
#include "../matrix.h"
#include "../display.h"

#include "physics-2d.h"

#define PARTICLES_COUNT 1024
#define LINES_COUNT 13
#define GRAVITY 0.00005

static particle_t particles[PARTICLES_COUNT];
static line_t lines[LINES_COUNT];

void draw_line_bound(line_t* line) {
	float line_left = line->bounding_box.x;
	float line_top = line->bounding_box.y;
	float line_right = line_left + line->bounding_box.z;
	float line_bottom = line_top + line->bounding_box.w;
	draw_line_on_screen(line_left, line_top, line_right, line_top, 0xff0000ff, get_screen_color_buffer());
	draw_line_on_screen(line_right, line_top, line_right, line_bottom, 0xff0000ff, get_screen_color_buffer());
	draw_line_on_screen(line_left, line_top, line_left, line_bottom, 0xff0000ff, get_screen_color_buffer());
	draw_line_on_screen(line_left, line_bottom, line_right, line_bottom, 0xff0000ff, get_screen_color_buffer());
}

void get_line_bounds(line_t* line) {
	float x0 = line->x0;
	float y0 = line->y0;
	float x1 = line->x1;
	float y1 = line->y1;
	if (x1 < x0) {
		float_swap(&x0, &x1);
	}
	if (y1 < y0) {
		float_swap(&y0, &y1);
	}
	float min_x = MIN(x0, x1);
	float min_y = MIN(y0, y1);
	float max_x = MAX(x0, x1);
	float max_y = MAX(y0, y1);
	float width = max_x - min_x;
	float height = max_y - min_y;
	line->bounding_box = vec4_new(x0, y0, width, height);
}

line_t make_line(float x0, float y0, float x1, float y1) {
	line_t line = {
		.x0 = x0,
		.y0 = y0,
		.x1 = x1,
		.y1 = y1,
		.mid_x = x0 + (x1 - x0) / 2,
		.mid_y = y0 + (y1 - y0) / 2
	};
	get_line_bounds(&line);
	return line;
}

float get_line_rotation(line_t* line) {
	return atan2f(line->y1 - line->y0, line->x1 - line->x0);
}

bool particle_intersect_line_bounds(particle_t* particle, line_t* line) {
	return
		particle->x > line->bounding_box.x &&
		particle->x < line->bounding_box.x + line->bounding_box.z &&
		particle->y > line->bounding_box.y &&
		particle->y < line->bounding_box.y + line->bounding_box.w;
}

void bounce_particle_off_line(line_t* line) {
	for (int i = 0; i < PARTICLES_COUNT; i++) {
		particle_t* particle = &particles[i];
		if (particle_intersect_line_bounds(particle, line)) {
			float rotation = get_line_rotation(line);
			float c = cosf(rotation);
			float s = sinf(rotation);
			float x = particle->x - line->mid_x;
			float y = particle->y - line->mid_y;
			
			
			float vy1 = c * particle->vy - s * particle->vx;

			float y1 = c * y - s * x;

			if (y1 > -particle->radius && y1 < vy1) {
				float x2 = c * x + s * y;
				float vx1 = c * particle->vx + s * particle->vy;
				vy1 *= -0.4;

				y1 = -particle->radius;

				x = c * x2 - s * y1;
				y = c * y1 + s * x2;

				particle->vx = c * vx1 - s * vy1;
				particle->vy = c * vy1 + s * vx1;

				particle->x = line->mid_x + x;
				particle->y = line->mid_y + y;
			};
		}
	}
}

void physics2D_example_setup(void) {
	int vwidth = get_viewport_width();
	int vheight = get_viewport_height();
	for (int i = 0; i < PARTICLES_COUNT; i++) {
		float screen_padding = rand() % 150;
		float x = rand() % vwidth;
		float y = rand() % vheight - screen_padding;
		particle_t particle = {
			.x = x,
			.y = y,
			.screen_padding = screen_padding,
			.vx = 0,
			.vy = 0.2,
			.radius = 3
		};
		particles[i] = particle;
	}

	float vwidthf = (float)vwidth;
	float vheightf = (float)vheight;

	float step_y = vheight / LINES_COUNT;

	float step = M_PI / (LINES_COUNT - 3);

	for (int i = 0; i < LINES_COUNT; i++) {
		float x0 = cos(i * 2 * step) * vwidth * 0.35 + vwidthf / 2 - vwidthf * 0.1;
		float y0 = step_y * i + (i % 2 == 0 ? -10 : 10);
		float x1 = cos(i * 2 * step) * vwidth * 0.35 + vwidthf / 2 + vwidthf * 0.1;
		float y1 = step_y * i + (i % 2 == 0 ? 10 : -10);
		if (i == LINES_COUNT - 3) {
			x0 = 0;
			y0 = vheightf * 0.825;
			x1 = vwidth * 0.15;
			y1 = y0 + 100;
		} else if (i == LINES_COUNT - 2) {
			x0 = vwidthf * 0.85;
			y0 = y0 + 100;
			x1 = vwidthf;
			y1 = vheightf * 0.825;
		} else if (i == LINES_COUNT - 1) {
			x0 = vwidthf * 0.425;
			y0 = vheightf * 0.95;
			x1 = vwidth * 0.575;
			y1 = vheightf * 0.98;
		}
		line_t line = make_line(x0, y0, x1, y1);
		lines[i] = line;
	}

// printf("bbox.x %f bbox.y %f bbox.w %f bbox.h %f midx %f midy %f\n", line1.bounding_box.x, line1.bounding_box.y, line1.bounding_box.z, line1.bounding_box.w, line1.mid_x, line1.mid_y);
	
}

void physics2D_example_process_input(SDL_Event* event, int delta_time) {
	switch (event->type) {
		
	}
}

void physics2D_example_update(int delta_time, int elapsed_time) {
	int vheight = get_viewport_height();
	for (int i = 0; i < PARTICLES_COUNT; i++) {
		particle_t* particle = &particles[i];
		particle->vy += delta_time * GRAVITY;
		particle->x += delta_time * particle->vx;
		particle->y += delta_time * particle->vy;
		if (particle->x > get_viewport_width()) {
			particle->vx *= -1;
		}
		if (particle->x < 0) {
			particle->vx *= -1;
		}

		if (particle->y > vheight + particle->screen_padding) {
			particle->y = -(rand() % (int)particle->screen_padding);
		}
	}

	for (int i = 0; i < LINES_COUNT; i++) {
		line_t* line = &lines[i];
		bounce_particle_off_line(line);
	}

	
}

void physics2D_example_render(int delta_time, int elapsed_time) {
	for (int i = 0; i < PARTICLES_COUNT; i++) {
		particle_t* particle = &particles[i];
		draw_rect_on_screen(particle->x, particle->y, particle->radius, particle->radius, 0xffaaaaaa, get_screen_color_buffer());
	}

	for (int i = 0; i < LINES_COUNT; i++) {
		line_t* line = &lines[i];
		draw_line_on_screen(line->x0, line->y0, line->x1, line->y1, 0xffffffff, get_screen_color_buffer());
		// draw_line_bound(&line);
	}
}

void physics2D_example_free_resources(void) {

}