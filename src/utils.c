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

// https://stackoverflow.com/a/63680339
uint32_t u8_to_u32(const uint8_t* bytes) {
  // Every uint32_t consists of 4 bytes, therefore we can shift each uint8_t
  // to an appropriate location.
  // u32   ff  ff  ff  ff
  // u8[]  0   1   2   3
  uint32_t u32 = (bytes[0] << 24) + (bytes[1] << 16) + (bytes[2] << 8) + bytes[3];
  return u32;
}

// We pass an output array in the arguments because we can not return arrays
uint8_t* u32_to_u8(const uint32_t u32, uint8_t* u8) {
  // To extract each byte, we can mask them using bitwise AND (&)
  // then shift them right to the first byte.
  u8[0] = (u32 & 0xff000000) >> 24;
  u8[1] = (u32 & 0x00ff0000) >> 16;
  u8[2] = (u32 & 0x0000ff00) >> 8;
  u8[3] = u32 & 0x000000ff;
	return u8;
}
