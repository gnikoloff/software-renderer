#include "triangle.h"
#include "display.h"
#include "utils.h"

void draw_filled_triangle(
	int x0, int y0, float z0, float w0,
	int x1, int y1, float z1, float w1,
	int x2, int y2, float z2, float w2,
	uint32_t color,
	color_framebuffer* color_buffer,
	depth_framebuffer* z_buffer
) {
	// We need to sort the vertices by y coordinates (y0 < y1 < y2)
	if (y0 > y1) {
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
		float_swap(&z0, &z1);
		float_swap(&w0, &w1);
	}
	if (y1 > y2) {
		int_swap(&y1, &y2);
		int_swap(&x1, &x2);
		float_swap(&z1, &z2);
		float_swap(&w1, &w2);
	}
	// We need to sort y0 and y1 again because y1 might have changed!
	if (y0 > y1) {
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
		float_swap(&z0, &z1);
		float_swap(&w0, &w1);
	}

	vec4_t point_a = { x0, y0, z0, w0 };
	vec4_t point_b = { x1, y1, z1, w1 };
	vec4_t point_c = { x2, y2, z2, w2 };

	// Render the upper part of the triangle
	// run / rise
	float inv_slope1 = 0;
	float inv_slope2 = 0;

	if (y1 - y0 != 0) {
		inv_slope1 = (float)(x1 - x0) / abs(y1 - y0);
	}
	if (y2 - y0 != 0) {
		inv_slope2 = (float)(x2 - x0) / abs(y2 - y0);
	}

	if (y1 - y0 != 0) {
		for (int y = y0; y <= y1; y++) {
			int x_start = (y - y1) * inv_slope1 + x1;
			int x_end = (y - y0) * inv_slope2 + x0;

			// Swap if x_start is to the right of x_end
			if (x_end < x_start) {
				int_swap(&x_start, &x_end);
			}

			for (int x = x_start; x < x_end; x++) {
				// Draw our pixel with color coming from the texture
				draw_triangle_pixel(x, y, color, point_a, point_b, point_c, color_buffer, z_buffer);
			}
		}
	}

	// Render the bottom part of the triangle
	inv_slope1 = 0;
	inv_slope2 = 0;

	if (y2 - y1 != 0) {
		inv_slope1 = (float)(x2 - x1) / abs(y2 - y1);
	}
	if (y2 - y0 != 0) {
		inv_slope2 = (float)(x2 - x0) / abs(y2 - y0);
	}

	if (y2 - y1 != 0) {
		for (int y = y1; y <= y2; y++) {
			int x_start = (y - y1) * inv_slope1 + x1;
			int x_end = (y - y0) * inv_slope2 + x0;

			if (x_end < x_start) {
				int_swap(&x_start, &x_end);
			}

			for (int x = x_start; x < x_end; x++) {
				draw_triangle_pixel(x, y, color, point_a, point_b, point_c, color_buffer, z_buffer);
			}
		}
	}

}

vec3_t barycentric_weights(vec2_t a, vec2_t b, vec2_t c, vec2_t p) {
	vec2_t ac = vec2_sub(c, a);
	vec2_t ab = vec2_sub(b, a);
	vec2_t ap = vec2_sub(p, a);
	vec2_t pc = vec2_sub(c, p);
	vec2_t pb = vec2_sub(b, p);

	// Compute the area of the full parallelogram/triangle ABC using 2D cross product
	float area_parallelogram_abc = (ac.x * ab.y - ac.y * ab.x); // || AC x AB ||

	// Alpha is the area of the small parallelogram/triangle PBC divided by the area of the full parallelogram/triangle ABC
	float alpha = (pc.x * pb.y - pc.y * pb.x) / area_parallelogram_abc;

	// Beta is the area of the small parallelogram/triangle APC divided by the area of the full parallelogram/triangle ABC
	float beta = (ac.x * ap.y - ac.y * ap.x) / area_parallelogram_abc;

	float gamma = 1 - alpha - beta;
	
	vec3_t weights = { alpha, beta, gamma };
	return weights;
}

void draw_triangle_pixel(
	int x, int y, uint32_t color,
	vec4_t point_a, vec4_t point_b, vec4_t point_c,
	color_framebuffer* color_buffer,
	depth_framebuffer* z_buffer
) {
	vec2_t p = { x, y };
	vec2_t a = vec2_from_vec4(point_a);
	vec2_t b = vec2_from_vec4(point_b);
	vec2_t c = vec2_from_vec4(point_c);
	vec3_t weights = barycentric_weights(a, b, c, p);
	float alpha = weights.x;
	float beta = weights.y;
	float gamma = weights.z;

	float interpolated_reciprocal_w;

	// Also interpolate the value of 1 / w for the current pixel
	interpolated_reciprocal_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta + (1 / point_c.w) * gamma;

	// Adjust 1/w so the pixel that are closer to the camera have smaller values
	interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

	// Only draw the pixel if the depth value is less than the one previously stored in the z-buffer
	if (interpolated_reciprocal_w < get_depth_buffer_at(z_buffer, x, y)) {
		if (color_buffer != NULL) {
			update_color_buffer_at(color_buffer, x, y, color);
		}
		
		// Update the z-buffer value with the 1/w of this current buffer
		update_depth_buffer_at(z_buffer, x, y, interpolated_reciprocal_w);
	}
}

// Function to draw the textured pixel at position x and y using interpolation
void draw_texel(
	int x, int y, upng_t* texture,
	float interpolated_reciprocal_w,
	float u, float v,
	color_framebuffer* color_buffer,
	depth_framebuffer* z_buffer
) {
	if (interpolated_reciprocal_w < get_depth_buffer_at(z_buffer, x, y)) {
		if (color_buffer != NULL) {
			if (texture != NULL) {
				update_color_buffer_at(color_buffer, x, y, sample_texture(texture, u, v));
			} else {
				update_color_buffer_at(color_buffer, x, y, 0xff0000ff);
			}
		}
		
		// Update the z-buffer value with the 1/w of this current buffer
		update_depth_buffer_at(z_buffer, x, y, interpolated_reciprocal_w);
	}
}

void draw_textured_triangle(
	int x0, int y0, float z0, float w0, float u0, float v0,
	int x1, int y1, float z1, float w1, float u1, float v1,
	int x2, int y2, float z2, float w2, float u2, float v2,
	upng_t* texture,
	color_framebuffer* color_buffer,
	depth_framebuffer* z_buffer
) {
	// vec4_t point_a = { x0, y0, z0, w0 };
	// vec4_t point_b = { x1, y1, z1, w1 };
	// vec4_t point_c = { x2, y2, z2, w2 };
	// tex2_t a_uv = { u0, v0 };
	// tex2_t b_uv = { u1, v1 };
	// tex2_t c_uv = { u2, v2 };

	float inv_slope1 = 0;
	float inv_slope2 = 0;

	if (y1 - y0 != 0) {
		inv_slope1 = (float)(x1 - x0) / abs(y1 - y0);
	}
	if (y2 - y0 != 0) {
		inv_slope2 = (float)(x2 - x0) / abs(y2 - y0);
	}

	if (y1 - y0 != 0) {
		for (int y = y0; y <= y1; y++) {
			int x_start = (y - y1) * inv_slope1 + x1;
			int x_end = (y - y0) * inv_slope2 + x0;

			// Swap if x_start is to the right of x_end
			if (x_end < x_start) {
				int_swap(&x_start, &x_end);
			}

			for (int x = x_start; x < x_end; x++) {
				// draw_texel(
				// 	x, y,
				// 	texture,
				// 	point_a, point_b, point_c,
				// 	a_uv, b_uv, c_uv,
				// 	color_buffer,
				// 	z_buffer
				// );
			}
		}
	}

	inv_slope1 = 0;
	inv_slope2 = 0;

	if (y2 - y1 != 0) {
		inv_slope1 = (float)(x2 - x1) / abs(y2 - y1);
	}
	if (y2 - y0 != 0) {
		inv_slope2 = (float)(x2 - x0) / abs(y2 - y0);
	}

	if (y2 - y1 != 0) {
		for (int y = y1; y <= y2; y++) {
			int x_start = (y - y1) * inv_slope1 + x1;
			int x_end = (y - y0) * inv_slope2 + x0;


			// Swap if x_start is to the right of x_end
			if (x_end < x_start) {
				int_swap(&x_start, &x_end);
			}

			for (int x = x_start; x < x_end; x++) {
				// // Draw our pixel with color coming from the texture
				// draw_texel(
				// 	x, y,
				// 	texture,
				// 	point_a, point_b, point_c,
				// 	a_uv, b_uv, c_uv,
				// 	color_buffer,
				// 	z_buffer
				// );
			}
		}
	}

}

inline void draw_triangle(
	int x0, int y0,
	int x1, int y1,
	int x2, int y2,
	uint32_t color,
	color_framebuffer* color_buffer
) {
	draw_line(x0, y0, x1, y1, color, color_buffer);
	draw_line(x1, y1, x2, y2, color, color_buffer);
	draw_line(x2, y2, x0, y0, color, color_buffer);
}
