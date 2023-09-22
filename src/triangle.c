#include "triangle.h"
#include "display.h"
#include "utils.h"

static vec2_t ac = { .x = 0, .y = 0 };
static vec2_t ab = { .x = 0, .y = 0 };
static vec2_t ap = { .x = 0, .y = 0 };
static vec2_t pc = { .x = 0, .y = 0 };
static vec2_t pb = { .x = 0, .y = 0 };

vec3_t barycentric_weights(vec2_t a, vec2_t b, vec2_t c, vec2_t p) {
	vec2_sub(&ac, &c, &a);
	vec2_sub(&ab, &b, &a);
	vec2_sub(&ap, &p, &a);
	vec2_sub(&pc, &c, &p);
	vec2_sub(&pb, &b, &p);

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
