#include <stdio.h>
#include "upng.h"
#include "texture.h"

tex2_t tex2_clone(tex2_t* t) {
	tex2_t result = { .u = t->u, .v = t->v };
	return result;
};
