#include "clipping.h"
#include "math.h"

#include "texture.h"
#include "utils.h"

#define NUM_PLANES 6
plane_t frustum_planes[6];

void init_frustum_planes(float fovx, float fovy, float z_near, float z_far) {
	float cos_half_fov_x = cos(fovx / 2);
	float sin_half_fov_x = sin(fovx / 2);
	float cos_half_fov_y = cos(fovy / 2);
	float sin_half_fov_y = sin(fovy / 2);

	frustum_planes[LEFT_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
	frustum_planes[LEFT_FRUSTUM_PLANE].normal.x = cos_half_fov_x;
	frustum_planes[LEFT_FRUSTUM_PLANE].normal.y = 0;
	frustum_planes[LEFT_FRUSTUM_PLANE].normal.z = sin_half_fov_x;

	frustum_planes[RIGHT_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
	frustum_planes[RIGHT_FRUSTUM_PLANE].normal.x = -cos_half_fov_x;
	frustum_planes[RIGHT_FRUSTUM_PLANE].normal.y = 0;
	frustum_planes[RIGHT_FRUSTUM_PLANE].normal.z = sin_half_fov_x;

	frustum_planes[TOP_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
	frustum_planes[TOP_FRUSTUM_PLANE].normal.x = 0;
	frustum_planes[TOP_FRUSTUM_PLANE].normal.y = -cos_half_fov_y;
	frustum_planes[TOP_FRUSTUM_PLANE].normal.z = sin_half_fov_y;

	frustum_planes[BOTTOM_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
	frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.x = 0;
	frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.y = cos_half_fov_y;
	frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.z = sin_half_fov_y;

	frustum_planes[NEAR_FRUSTUM_PLANE].point = vec3_new(0, 0, z_near);
	frustum_planes[NEAR_FRUSTUM_PLANE].normal.x = 0;
	frustum_planes[NEAR_FRUSTUM_PLANE].normal.y = 0;
	frustum_planes[NEAR_FRUSTUM_PLANE].normal.z = 1;

	frustum_planes[FAR_FRUSTUM_PLANE].point = vec3_new(0, 0, z_far);
	frustum_planes[FAR_FRUSTUM_PLANE].normal.x = 0;
	frustum_planes[FAR_FRUSTUM_PLANE].normal.y = 0;
	frustum_planes[FAR_FRUSTUM_PLANE].normal.z = -1;
}

polygon_t create_polygon_from_triangles(vec3_t v0, vec3_t v1, vec3_t v2, tex2_t t0, tex2_t t1, tex2_t t2) {
	polygon_t polygon = {
		.vertices = { v0, v1, v2 },
		.texcoords = { t0, t1, t2 },
		.num_vertices = 3
	};

	return polygon;
}

void triangles_from_polygon(polygon_t* polygon, triangle_t triangles[], int* num_triangles) {
	for (int i = 0; i < polygon->num_vertices - 2; i++) {
		int index0 = 0;
		int index1 = i + 1;
		int index2 = i + 2;

		vertex_t vertex_a = {
			.position = vec4_from_vec3(polygon->vertices[index0]),
			.uv = polygon->texcoords[index0],
		};
		vertex_t vertex_b = {
			.position = vec4_from_vec3(polygon->vertices[index1]),
			.uv = polygon->texcoords[index1],
		};
		vertex_t vertex_c = {
			.position = vec4_from_vec3(polygon->vertices[index2]),
			.uv = polygon->texcoords[index2],
		};

		triangles[i].vertices[0] = vertex_a;
		triangles[i].vertices[1] = vertex_b;
		triangles[i].vertices[2] = vertex_c;
		
	}
	*num_triangles = polygon->num_vertices - 2;
}

void clip_polygon_against_plane(polygon_t* polygon, int plane) {
	vec3_t plane_point = frustum_planes[plane].point;
	vec3_t plane_normal = frustum_planes[plane].normal;

	vec3_t inside_vertices[MAX_NUMBER_POLY_VERTICES];
	tex2_t inside_texcoords[MAX_NUMBER_POLY_VERTICES];
	int num_inside_vertices = 0;

	vec3_t* current_vertex = &polygon->vertices[0];
	vec3_t* prev_vertex = &polygon->vertices[polygon->num_vertices - 1];

	tex2_t* current_texcoord = &polygon->texcoords[0];
	tex2_t* prev_texcoord = &polygon->texcoords[polygon->num_vertices - 1];

	float current_dot;
	float prev_dot;

	vec3_t prev_vertex_point_diff = { 0, 0, 0 };
	vec3_t curr_vertex_point_diff = { 0, 0, 0 };
	prev_vertex_point_diff = vec3_sub(*prev_vertex, plane_point);
	prev_dot = vec3_dot(prev_vertex_point_diff, plane_normal);

	while (current_vertex != &polygon->vertices[polygon->num_vertices]) {
		curr_vertex_point_diff = vec3_sub(*current_vertex, plane_point);
		current_dot = vec3_dot(curr_vertex_point_diff, plane_normal);

		// if we changed from inside to outside or outside to inside
		if (current_dot * prev_dot < 0) {
			float t = prev_dot / (prev_dot - current_dot);

			vec3_t intersection_point = {
				.x = float_lerp(prev_vertex->x, current_vertex->x, t),
				.y = float_lerp(prev_vertex->y, current_vertex->y, t),
				.z = float_lerp(prev_vertex->z, current_vertex->z, t)
			};

			tex2_t interpolated_texcoord = {
				.u = float_lerp(prev_texcoord->u, current_texcoord->u, t),
				.v = float_lerp(prev_texcoord->v, current_texcoord->v, t),
			};

			inside_vertices[num_inside_vertices] = vec3_clone(&intersection_point);
			inside_texcoords[num_inside_vertices] = interpolated_texcoord;

			num_inside_vertices++;
		}

		if (current_dot > 0) {
			inside_vertices[num_inside_vertices] = vec3_clone(current_vertex);
			inside_texcoords[num_inside_vertices] = tex2_clone(current_texcoord);
			num_inside_vertices++;
		}
		
		prev_dot = current_dot;
		prev_vertex = current_vertex;
		prev_texcoord = current_texcoord;
		// pointer arithmetic
		current_vertex++;
		current_texcoord++;
	}

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
