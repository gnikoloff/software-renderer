#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

#define MESH_DEBUG_COLOR 0xff0000ff
#define LINE_DEBUG_COLOR 0xffff0000

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define CLAMP(min, max, v) MIN(max, MAX(min, v))
#define GET_BYTE(value, n) (value >> (n * 8) & 0xFF)

// make emscripten happy
#ifndef M_PI
#define M_PI 3.14159265359f
#endif

void int_swap(int* a, int* b);
void float_swap(float* a, float* b);
float float_lerp(float a, float b, float t);
float blerp(float c00, float c10, float c01, float c11, float tx, float ty);
uint32_t u8_to_u32(const uint8_t* bytes);
uint8_t* u32_to_u8(const uint32_t u32, uint8_t* u8);

#endif