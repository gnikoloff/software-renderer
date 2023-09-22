#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "math.h"
#include "array.h"
#include "upng.h"
#include "utils.h"
#include "texture.h"

texture_2d_t* load_png_data(char* png_filename) {
	texture_2d_t* png_image = upng_new_from_file(png_filename);
	if (png_image != NULL) {
		upng_decode(png_image);
		if (upng_get_error(png_image) == UPNG_EOK) {
			return png_image;
		}
	}
	return NULL;
}

uint32_t get_pixel(upng_t* texture, int x, int y) {
	int texture_width = upng_get_width(texture);
	int texture_height = upng_get_height(texture);
	if (x < 0 || x >= texture_width || y < 0 || y >= texture_height) {
		// return a nice debug color
		return 0xff0000ff;
	}
	uint32_t* texture_buffer = (uint32_t*)upng_get_buffer(texture);
	return texture_buffer[y * texture_width + x];
}

tex2_t tex2_clone(tex2_t* t) {
	tex2_t result = { .u = t->u, .v = t->v };
	return result;
};

uint32_t sample_texture(texture_2d_t* texture, float u, float v) {
	int texture_width = upng_get_width(texture);
	int texture_height = upng_get_height(texture);
	int tex_x = abs((int)(u * texture_width)) % texture_width;
	int tex_y = abs((int)(v * texture_height)) % texture_height;
	uint32_t* texture_buffer = (uint32_t*)upng_get_buffer(texture);
	return texture_buffer[tex_y * texture_width + tex_x];
}

// https://gist.github.com/NickBeeuwsaert/5753386
uint32_t sample_texture_bilinear(texture_2d_t* texture, float u, float v) {
	int texture_width = upng_get_width(texture);
	int texture_height = upng_get_height(texture);
	float tex_x = fabs(u * (float)(texture_width - 1));
	float tex_y = fabs(v * (float)(texture_height - 1));
	int gxi = (int)tex_x;
	int gyi = (int)tex_y;
	if (gxi < 0 || gxi >= texture_width - 1 || gyi < 0 || gyi >= texture_height - 1) {
		// return a nice debug color
		return 0xff0000ff;
	}
	uint32_t t00 = get_pixel(texture, gxi, gyi);
	uint32_t t01 = get_pixel(texture, gxi + 1, gyi);
	uint32_t t10 = get_pixel(texture, gxi, gyi + 1);
	uint32_t t11 = get_pixel(texture, gxi + 1, gyi + 1);

	uint32_t result = 0;
	
	for(int i = 0; i < sizeof(result); i++){
		result |= (uint8_t)blerp(
			GET_BYTE(t00, i),
			GET_BYTE(t10, i),
			GET_BYTE(t01, i),
			GET_BYTE(t11, i),
			(tex_x - gxi),
			(tex_y - gyi)
		) << (8 * i);
	}

	return result;
}

// https://github.com/niepp/srpbr/blob/c33c99ed8122e4972d15b640e543a6c9ff6aa337/texture.h#L138
// the original method uses left hand z-up coordinate system
// my method is hacked around right handed y-up coordinate system
// TODO: need to revisit the calculations, less hacky
static int direction_to_cubeuv(vec3_t dir, tex2_t* tc) {
	int face_index = 0;
	float absX = fabs(dir.x);
	float absY = fabs(dir.y);
	float absZ = fabs(dir.z);
	float ma;
	if (absZ >= absX && absZ >= absY) {
		face_index = dir.z > 0.0f ? 2 : 3;
		tc->u = dir.x;
		tc->v = dir.z > 0.0f ? dir.y : -dir.y;
		ma = absZ;
		tc->u = (tc->u / ma + 1.0f) * 0.5f;
		tc->v = (tc->v / ma + 1.0f) * 0.5f;
		if (dir.z > 0) {
			tc->u = 1.0 - tc->u;
			tc->v = 1.0 - tc->v;
		}
	} else if (absY >= absX) {
		face_index = dir.y > 0.0f ? 5 : 4;
		tc->u = dir.y > 0.0f ? -dir.x : dir.x;
		tc->v = dir.z;
		ma = absY;
		tc->u = (tc->u / ma + 1.0f) * 0.5f;
		tc->v = (tc->v / ma + 1.0f) * 0.5f;
	} else {
		face_index = dir.x > 0.0f ? 0 : 1;
		tc->u = dir.x > 0.0f ? dir.z : -dir.z;
		tc->v = dir.y;
		ma = absX;
		tc->u = (tc->u / ma + 1.0f) * 0.5f;
		tc->v = 1 - (tc->v / ma + 1.0f) * 0.5f;
	}
	return face_index;
}

texture_cube_t make_cube_texture(char* textures_paths[6]) {
	texture_cube_t cube_texture = {
		.width = 0,
		.height = 0,
		.face_textures = NULL
	};
	for (int i = 0; i < 6; i++) {
		array_push(cube_texture.face_textures, *load_png_data(textures_paths[i]));
	}
	int width = upng_get_width(&cube_texture.face_textures[0]);
	int height = upng_get_height(&cube_texture.face_textures[0]);
	cube_texture.width = width;
	cube_texture.height = height;
	return cube_texture;
}

static tex2_t cube_uvs = { .u = 0, .v = 0 };
uint32_t sample_cube_texture(texture_cube_t* cube_texture, vec3_t coord) {
	int face_idx = direction_to_cubeuv(coord, &cube_uvs);
	texture_2d_t* face_texture = &cube_texture->face_textures[face_idx];
	return sample_texture(face_texture, cube_uvs.u, cube_uvs.v);
}
