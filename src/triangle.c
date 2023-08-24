#include "triangle.h"
#include "display.h"
#include "swap.h"

// Draw a filled triangle with a flat bottom
// 
//          (x0,y0)
//            / \
//           /   \
//          /     \
//         /       \
//     (x1,y1)---(x2,y2)
// 
void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
	// Find the two slopes (two triangle legs)
	// We need to inverse them so it is not rise / run

	// That's because we are looping our scanlines from top to bottom
	// so y is our independent variable.
	// How much x changes as y progresses?
	// We need to solve for x!!!
	// So what we are really looking for is run / rise

	// Left leg
	float inverse_slope_1 = (float)(x1 - x0) / (y1 - y0);
	// Right leg
	float inverse_slope_2 = (float)(x2 - x0) / (y2 - y0);

	// Start x_start and x_end at the top vertex (x0,y0)
	float start_x = x0;
	float end_x = x0;

	// Loop all the scanlines from top to bottom
	for (int y = y0; y <= y2; y++) {
		// Add the slope increment based on run / rise
		// to both start and end
		draw_line(start_x, y, end_x, y, color);
		start_x += inverse_slope_1;
		end_x += inverse_slope_2;
	}
}

// Draw a filled triangle with a flat top
// 
//     (x0,y0)----(x1,y1)
//         \        /
//          \      /
//           \    /
//            \  /
//           (x2,y2)
//
void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
	// Left leg
	float inverse_slope_1 = (float)(x2 - x0) / (y2 - y0);
	// Right leg
	float inverse_slope_2 = (float)(x2 - x1) / (y2 - y0);

	// Start x_start and x_end at the bottom vertex (x2,y2)
	float start_x = x2;
	float end_x = x2;

	// Loop all the scanlines from top to bottom
	for (int y = y2; y >= y0; y--) {
		// Add the slope increment based on run / rise
		// to both start and end
		draw_line(start_x, y, end_x, y, color);
		start_x -= inverse_slope_1;
		end_x -= inverse_slope_2;
	}
}

// Draw a filled triangle with the flat-top / flat-bottom method
// We split the original triangle into two, half flat-bottom and flat-top triangles
// 
//          (x0, y0)
//            / \
//           /   \
//          /     \
//         /       \
//     (x1,y1)-------
//        \_          \
//          \_         \
//            \_        \
//              \_       \
//                \_      \
//                  \_     \
//                    \_    \
//                      \_   \
//                        \_  \
//                          \_ \
//                          (x2,y2)
//
void draw_filled_triangle(
	int x0, int y0, float z0, float w0,
	int x1, int y1, float z1, float w1,
	int x2, int y2, float z2, float w2,
	uint32_t color
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

	// if (y1 - y0 == 0) {
	// 	return;
	// }

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
				draw_triangle_pixel(x, y, color, point_a, point_b, point_c);
			}
		}
	}

	// Render the bottom part of the triangle
	inv_slope1 = 0;
	inv_slope2 = 0;

	// if (y2 - y1 == 0) {
	// 	return;
	// }

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
				// Draw our pixel with color coming from the texture
				draw_triangle_pixel(x, y, color, point_a, point_b, point_c);
			}
		}
	}

}

// Return the barycentric weights alpha, beta and gamma for point P
//          B
//         /|\
//        / | \
//       /  |  \
//      /   P   \
//     /   / \   \
//    /  /     \  \
//   / /         \ \
//  A ------------ C
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
	vec4_t point_a, vec4_t point_b, vec4_t point_c
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
	if (interpolated_reciprocal_w < get_zbuffer_at(x, y)) {
		// Draw a pixel with the correct texture
		draw_pixel(x, y, color);
		
		// Update the z-buffer value with the 1/w of this current buffer
		update_zbuffer_at(x, y, interpolated_reciprocal_w);
	}
}

// Function to draw the textured pixel at position x and y using interpolation
void draw_texel(
	int x, int y, upng_t* texture,
	vec4_t point_a, vec4_t point_b, vec4_t point_c,
	tex2_t a_uv, tex2_t b_uv, tex2_t c_uv
) {
	vec2_t p = { x, y };
	vec2_t a = vec2_from_vec4(point_a);
	vec2_t b = vec2_from_vec4(point_b);
	vec2_t c = vec2_from_vec4(point_c);
	vec3_t weights = barycentric_weights(a, b, c, p);
	float alpha = weights.x;
	float beta = weights.y;
	float gamma = weights.z;

	// Store interpolated values for U, V and 1 / w for the current pixel
	float interpolated_u;
	float interpolated_v;
	float interpolated_reciprocal_w;

	// Perform the interpolation of all U/w and V/w values using barycentric coordinates
	// OpenGL does that for us! We need to do ourselves
	interpolated_u = (a_uv.u / point_a.w) * alpha + (b_uv.u / point_b.w) * beta + (c_uv.u / point_c.w) * gamma;
	interpolated_v = (a_uv.v / point_a.w) * alpha + (b_uv.v / point_b.w) * beta + (c_uv.v / point_c.w) * gamma;

	// Also interpolate the value of 1 / w for the current pixel
	interpolated_reciprocal_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta + (1 / point_c.w) * gamma;

	// Now we can divide back both interpolated values by 1 / w
	interpolated_v /= interpolated_reciprocal_w;
	interpolated_u /= interpolated_reciprocal_w;

	// Get the mesh texture width and heigth
	int texture_width = upng_get_width(texture);
	int texture_height = upng_get_height(texture);

	// Map the UV coordinates from (0,0 -> 1, 1) to full texture size (0,0 -> texture_width,texture_height)
	int tex_x = abs((int)(interpolated_u * texture_width)) % texture_width;
	int tex_y = abs((int)(interpolated_v * texture_height)) % texture_height;

	// Adjust 1/w so the pixel that are closer to the camera have smaller values
	interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

	// Only draw the pixel if the depth value is less than the one previously stored in the z-buffer
	if (interpolated_reciprocal_w < get_zbuffer_at(x, y)) {
		// Get a pointer to the buffer of texture colors
		uint32_t* texture_buffer = (uint32_t*)upng_get_buffer(texture);

		// Draw a pixel with the correct texture
		draw_pixel(x, y, texture_buffer[tex_y * texture_width + tex_x]);
		
		// Update the z-buffer value with the 1/w of this current buffer
		update_zbuffer_at(x, y, interpolated_reciprocal_w);
	}
}

// Draw a textured triangle with the flat-top / flat-bottom method
// We split the original triangle into two, half flat-bottom and flat-top triangles
// 
//          (x0, y0)
//            / \
//           /   \
//          /     \
//         /       \
//     (x1,y1)---(Mx,My)
//        \_          \
//          \_         \
//            \_        \
//              \_       \
//                \_      \
//                  \_     \
//                    \_    \
//                      \_   \
//                        \_  \
//                          \_ \
//                          (x2,y2)
//
void draw_textured_triangle(
	int x0, int y0, float z0, float w0, float u0, float v0,
	int x1, int y1, float z1, float w1, float u1, float v1,
	int x2, int y2, float z2, float w2, float u2, float v2,
	upng_t* texture
) {
	// We need to sort the vertices by y coordinates (y0 < y1 < y2)
	if (y0 > y1) {
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
		float_swap(&u0, &u1);
		float_swap(&v0, &v1);
		float_swap(&z0, &z1);
		float_swap(&w0, &w1);
	}
	if (y1 > y2) {
		int_swap(&y1, &y2);
		int_swap(&x1, &x2);
		float_swap(&u1, &u2);
		float_swap(&v1, &v2);
		float_swap(&z1, &z2);
		float_swap(&w1, &w2);
	}
	// We need to sort y0 and y1 again because y1 might have changed!
	if (y0 > y1) {
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
		float_swap(&u0, &u1);
		float_swap(&v0, &v1);
		float_swap(&z0, &z1);
		float_swap(&w0, &w1);
	}

	// Flip the V component to account for invertex textures
	v0 = 1 - v0;
	v1 = 1 - v1;
	v2 = 1 - v2;

	// Create vector points and texture coordinates after we sort the vertices
	vec4_t point_a = { x0, y0, z0, w0 };
	vec4_t point_b = { x1, y1, z1, w1 };
	vec4_t point_c = { x2, y2, z2, w2 };
	tex2_t a_uv = { u0, v0 };
	tex2_t b_uv = { u1, v1 };
	tex2_t c_uv = { u2, v2 };

	// Render the upper part of the triangle
	// run / rise
	float inv_slope1 = 0;
	float inv_slope2 = 0;

	// if (y1 - y0 == 0) {
	// 	return;
	// }

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
				draw_texel(x, y, texture, point_a, point_b, point_c, a_uv, b_uv, c_uv);
			}
		}
	}

	// Render the bottom part of the triangle
	inv_slope1 = 0;
	inv_slope2 = 0;

	// if (y2 - y1 == 0) {
	// 	return;
	// }

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
				// Draw our pixel with color coming from the texture
				draw_texel(x, y, texture, point_a, point_b, point_c, a_uv, b_uv, c_uv);
			}
		}
	}

}

inline void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
	draw_line(x0, y0, x1, y1, color);
	draw_line(x1, y1, x2, y2, color);
	draw_line(x2, y2, x0, y0, color);
}
