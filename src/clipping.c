#include "clipping.h"
#include "math.h"

#include "texture.h"

#define NUM_PLANES 6
plane_t frustum_planes[6];

// Frustum planes are defined by a point and a normal vector
// Near plane   : P = (0, 0, z_near), N = (0, 0,  1)
// Far plane    : P = (0, 0, z_far),  N = (0, 0, -1)
// Top plane    : P = (0, 0, 0),      N = (0, -cos(fov / 2), sin(fov / 2))
// Bottom plane : P = (0, 0, 0),      N = (0, cos(fov / 2), sin(fov / 2))
// Left plane   : P = (0, 0, 0),      N = (cos(fov / 2), 0, sin(fov / 2))
// Right plane  : P = (0, 0, 0),      N = (-cos(fov / 2), 0, sin(fov / 2))

//           /|\
//         /  | |
//       /    | |
//     /\     | |
//   /        | |
// P*|->    <-|*| ---> +z_axis
//    \       | |
//      \/    | |
//        \   | |
//          \ | |
//            \|/


void init_frustum_planes(float fovx, float fovy, float z_near, float z_far) {
	float cos_half_fovx = cos(fovx / 2);
	float sin_half_fovx = sin(fovx / 2);
	float cos_half_fovy = cos(fovy / 2);
	float sin_half_fovy = sin(fovy / 2);

	frustum_planes[LEFT_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
	frustum_planes[LEFT_FRUSTUM_PLANE].normal = vec3_new(cos_half_fovx, 0, sin_half_fovx);

	frustum_planes[RIGHT_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
	frustum_planes[RIGHT_FRUSTUM_PLANE].normal = vec3_new(-cos_half_fovx, 0, sin_half_fovx);

	frustum_planes[TOP_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
	frustum_planes[TOP_FRUSTUM_PLANE].normal = vec3_new(0, -cos_half_fovy, sin_half_fovy);

	frustum_planes[BOTTOM_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
	frustum_planes[BOTTOM_FRUSTUM_PLANE].normal = vec3_new(0, cos_half_fovy, sin_half_fovy);

	frustum_planes[NEAR_FRUSTUM_PLANE].point = vec3_new(0, 0, z_near);
	frustum_planes[NEAR_FRUSTUM_PLANE].normal = vec3_new(0, 0, 1);

	frustum_planes[FAR_FRUSTUM_PLANE].point = vec3_new(0, 0, z_far);
	frustum_planes[FAR_FRUSTUM_PLANE].normal = vec3_new(0, 0, -1);
}

polygon_t create_polygon_from_triangles(vec3_t v0, vec3_t v1, vec3_t v2, tex2_t t0, tex2_t t1, tex2_t t2) {
	polygon_t polygon = {
		.vertices = { v0, v1, v2 },
		.texcoords = { t0, t1, t2 },
		.num_vertices = 3
	};

	return polygon;
}

float float_lerp(float a, float b, float t) {
	return a + (b - a) * t;
}

void triangles_from_polygon(polygon_t* polygon, triangle_t triangles[], int* num_triangles) {
	for (int i = 0; i < polygon->num_vertices - 2; i++) {
		int index0 = 0;
		int index1 = i + 1;
		int index2 = i + 2;
		
		triangles[i].points[0] = vec4_from_vec3(&polygon->vertices[index0]);
		triangles[i].points[1] = vec4_from_vec3(&polygon->vertices[index1]);
		triangles[i].points[2] = vec4_from_vec3(&polygon->vertices[index2]);

		triangles[i].tex_coords[0] = polygon->texcoords[index0];
		triangles[i].tex_coords[1] = polygon->texcoords[index1];
		triangles[i].tex_coords[2] = polygon->texcoords[index2];
	}
	*num_triangles = polygon->num_vertices - 2;
}

void clip_polygon_against_plane(polygon_t* polygon, int plane) {
	vec3_t plane_point = frustum_planes[plane].point;
	vec3_t plane_normal = frustum_planes[plane].normal;

	// Declare a static array of inside vertices that will
	// be part of the final polygon returned via parameter
	vec3_t inside_vertices[MAX_NUMBER_POLY_VERTICES];
	tex2_t inside_texcoords[MAX_NUMBER_POLY_VERTICES];
	int num_inside_vertices = 0;

	// Keep track of current and prev vertice

	// Start the current vertex with the first polygon vertex and texture coordinate
	vec3_t* current_vertex = &polygon->vertices[0];
	tex2_t* current_texcoord = &polygon->texcoords[0];

	// Start the previous vertex with the last polygon vertex and texture coordinate
	vec3_t* prev_vertex = &polygon->vertices[polygon->num_vertices - 1];
	tex2_t* prev_texcoord = &polygon->texcoords[polygon->num_vertices - 1];

	// Calculate the dot product of the current and previous vertex
	float current_dot;
	float prev_dot;

	static vec3_t prev_vertex_point_diff = { 0, 0, 0 };
	static vec3_t curr_vertex_point_diff = { 0, 0, 0 };
	vec3_sub(&prev_vertex_point_diff, prev_vertex, &plane_point);
	prev_dot = vec3_dot(&prev_vertex_point_diff, &plane_normal);

	// // Loop all the polygon vertices while the current is different than the last one
	while (current_vertex != &polygon->vertices[polygon->num_vertices]) {
		vec3_sub(&curr_vertex_point_diff, current_vertex, &plane_point);
		current_dot = vec3_dot(&curr_vertex_point_diff, &plane_normal);

		// If we changed from inside to outside or outside to inside
		if (current_dot * prev_dot < 0) {
			// Find the interpolation factor t
			float t = prev_dot / (prev_dot - current_dot);

			// Calculate the intersection point I = Q1 + t(Q2 - Q1)
			vec3_t intersection_point = {
				.x = float_lerp(prev_vertex->x, current_vertex->x, t),
				.y = float_lerp(prev_vertex->y, current_vertex->y, t),
				.z = float_lerp(prev_vertex->z, current_vertex->z, t)
			};

			// Use the linear interpolation formulate to get the interpolated U and V texture coordinates
			tex2_t interpolated_texcoord = {
				.u = float_lerp(prev_texcoord->u, current_texcoord->u, t),
				.v = float_lerp(prev_texcoord->v, current_texcoord->v, t),
			};

			inside_vertices[num_inside_vertices] = vec3_clone(&intersection_point);
			inside_texcoords[num_inside_vertices] = interpolated_texcoord;

			num_inside_vertices++;
		}

		// Current vertex is inside the plane
		if (current_dot > 0) {
			// Insert the current vertex to the list of "inside vertices"
			inside_vertices[num_inside_vertices] = vec3_clone(current_vertex);
			// Insert the current texcoord to the list of "inside texcoords"
			inside_texcoords[num_inside_vertices] = tex2_clone(current_texcoord);
			num_inside_vertices++;
		}
		
		// Move to the next vertex
		prev_dot = current_dot;
		prev_vertex = current_vertex;
		prev_texcoord = current_texcoord;
		// Pointer arithmetic: the compiler goes to the next vec4_t in the array
		current_vertex++;
		current_texcoord++;
	}

	// At the end, copy the list of inside vertices into the destination polygon (our parameter)
	for (int i = 0; i < num_inside_vertices; i++) {
		polygon->vertices[i] = vec3_clone(&inside_vertices[i]);
		polygon->texcoords[i] = tex2_clone(&inside_texcoords[i]);
	}
	polygon->num_vertices = num_inside_vertices;
}

void clip_polygon(polygon_t* polygon) {
	clip_polygon_against_plane(polygon, LEFT_FRUSTUM_PLANE);
	clip_polygon_against_plane(polygon, RIGHT_FRUSTUM_PLANE);
	clip_polygon_against_plane(polygon, TOP_FRUSTUM_PLANE);
	clip_polygon_against_plane(polygon, BOTTOM_FRUSTUM_PLANE);
	clip_polygon_against_plane(polygon, NEAR_FRUSTUM_PLANE);
	clip_polygon_against_plane(polygon, FAR_FRUSTUM_PLANE);
}
