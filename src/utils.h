#ifndef UTILS_H
#define UTILS_H

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define CLAMP(min, max, v) MIN(max, MAX(min, v))

// make emscripten happy
#ifndef M_PI
#define M_PI 3.14159265359f
#endif

void int_swap(int* a, int* b);
void float_swap(float* a, float* b);

#endif