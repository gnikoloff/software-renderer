#include "utils.h"

void int_swap(int* a, int* b) {
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

void float_swap(float* a, float* b) {
	float tmp = *a;
	*a = *b;
	*b = tmp;
}

float float_lerp(float a, float b, float t) {
	return a + (b - a) * t;
}

float blerp(float c00, float c10, float c01, float c11, float tx, float ty) {
    return float_lerp(float_lerp(c00, c10, tx), float_lerp(c01, c11, tx), ty);
}
