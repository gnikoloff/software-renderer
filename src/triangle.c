#include "triangle.h"
#include "display.h"

void int_swap(int* a, int* b) {
	int tmp = *a;
	*a = *b;
	*b = tmp;
}


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
void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
	// We need to sort the vertices by y coordinates (y0 < y1 < y2)
	if (y0 > y1) {
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
	}
	if (y1 > y2) {
		int_swap(&y1, &y2);
		int_swap(&x1, &x2);
	}
	// We need to sort y0 and y1 again because y1 might have changed!
	if (y0 > y1) {
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
	}

	if (y1 == y2) {
		// We can simply draw the flat-bottom triangle
		fill_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, color);
		return;
	}
	
	if (y0 == y1) {
		// We can simply draw the flat-top triangle
		fill_flat_top_triangle(x0, y0, x1, y1, x2, y2, color);
		return;
	}

	// Calculate the new vertex (Mx,My) using triangle similarity
	int Mx = (float)((x2 - x0) * (y1 - y0)) / (float)(y2 - y0) + x0;
	int My = y1;

	// Draw flat-bottom triangle
	fill_flat_bottom_triangle(x0, y0, x1, y1, Mx, My, color);

	fill_flat_top_triangle(x1, y1, Mx, My, x2, y2, color);

}
