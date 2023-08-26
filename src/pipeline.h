#ifndef PIPELINE_H
#define PIPELINE_H

#include "mesh.h"
#include "camera.h"

void render_mesh(mesh_t* mesh, camera_t* camera, void (*render_callback)(triangle_t*));

#endif