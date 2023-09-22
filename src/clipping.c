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

polygon_t create_polygon_from_triangles(
	vec3_t view_projection_v0,
	vec3_t view_projection_v1,
	vec3_t view_projection_v2,
	vec3_t world_v0,
	vec3_t world_v1,
	vec3_t world_v2,
	vec3_t normal_v0,
	vec3_t normal_v1,
	vec3_t normal_v2,
	tex2_t t0,
	tex2_t t1,
	tex2_t t2
) {
	polygon_t polygon = {
		.camera_space_vertices = { view_projection_v0, view_projection_v1, view_projection_v2 },
		.world_space_vertices = { world_v0, world_v1, world_v2 },
		.normals = { normal_v0, normal_v1, normal_v2 },
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
			.position = vec4_from_vec3(polygon->camera_space_vertices[index0]),
			.world_space_position = vec4_from_vec3(polygon->world_space_vertices[index0]),
			.normal = vec3_clone(&polygon->normals[index0]),
			.uv = polygon->texcoords[index0],
		};
		vertex_t vertex_b = {
			.position = vec4_from_vec3(polygon->camera_space_vertices[index1]),
			.world_space_position = vec4_from_vec3(polygon->world_space_vertices[index1]),
			.normal = vec3_clone(&polygon->normals[index1]),
			.uv = polygon->texcoords[index1],
		};
		vertex_t vertex_c = {
			.position = vec4_from_vec3(polygon->camera_space_vertices[index2]),
			.world_space_position = vec4_from_vec3(polygon->world_space_vertices[index2]),
			.normal = vec3_clone(&polygon->normals[index2]),
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

	vec3_t inside_camera_space_vertices[MAX_NUMBER_POLY_VERTICES];
	vec3_t inside_world_space_vertices[MAX_NUMBER_POLY_VERTICES];
	vec3_t inside_normals[MAX_NUMBER_POLY_VERTICES];
	tex2_t inside_texcoords[MAX_NUMBER_POLY_VERTICES];
	int num_inside_vertices = 0;

	vec3_t* current_camera_space_vertex = &polygon->camera_space_vertices[0];
	vec3_t* prev_camera_space_vertex = &polygon->camera_space_vertices[polygon->num_vertices - 1];

	vec3_t* current_world_space_vertex = &polygon->world_space_vertices[0];
	vec3_t* prev_world_space_vertex = &polygon->world_space_vertices[polygon->num_vertices - 1];

	vec3_t* current_normal = &polygon->normals[0];
	vec3_t* prev_normal = &polygon->normals[polygon->num_vertices - 1];

	tex2_t* current_texcoord = &polygon->texcoords[0];
	tex2_t* prev_texcoord = &polygon->texcoords[polygon->num_vertices - 1];

	float current_dot;
	float prev_dot;

	vec3_t prev_vertex_point_diff = { 0, 0, 0 };
	vec3_t curr_vertex_point_diff = { 0, 0, 0 };
	prev_vertex_point_diff = vec3_sub(*prev_camera_space_vertex, plane_point);
	prev_dot = vec3_dot(prev_vertex_point_diff, plane_normal);

	while (current_camera_space_vertex != &polygon->camera_space_vertices[polygon->num_vertices]) {
		curr_vertex_point_diff = vec3_sub(*current_camera_space_vertex, plane_point);
		current_dot = vec3_dot(curr_vertex_point_diff, plane_normal);

		// if we changed from inside to outside or outside to inside
		if (current_dot * prev_dot < 0) {
			float t = prev_dot / (prev_dot - current_dot);

			vec3_t intersection_camera_space_point = {
				.x = float_lerp(prev_camera_space_vertex->x, current_camera_space_vertex->x, t),
				.y = float_lerp(prev_camera_space_vertex->y, current_camera_space_vertex->y, t),
				.z = float_lerp(prev_camera_space_vertex->z, current_camera_space_vertex->z, t)
			};
			vec3_t intersection_world_space_point = {
				.x = float_lerp(prev_world_space_vertex->x, current_world_space_vertex->x, t),
				.y = float_lerp(prev_world_space_vertex->y, current_world_space_vertex->y, t),
				.z = float_lerp(prev_world_space_vertex->z, current_world_space_vertex->z, t)
			};
			vec3_t intersection_normal = {
				.x = float_lerp(prev_normal->x, current_normal->x, t),
				.y = float_lerp(prev_normal->y, current_normal->y, t),
				.z = float_lerp(prev_normal->z, current_normal->z, t)
			};

			tex2_t interpolated_texcoord = {
				.u = float_lerp(prev_texcoord->u, current_texcoord->u, t),
				.v = float_lerp(prev_texcoord->v, current_texcoord->v, t),
			};

			inside_camera_space_vertices[num_inside_vertices] = vec3_clone(&intersection_camera_space_point);
			inside_world_space_vertices[num_inside_vertices] = vec3_clone(&intersection_world_space_point);
			inside_normals[num_inside_vertices] = vec3_clone(&intersection_normal);
			inside_texcoords[num_inside_vertices] = interpolated_texcoord;

			num_inside_vertices++;
		}

		if (current_dot > 0) {
			inside_camera_space_vertices[num_inside_vertices] = vec3_clone(current_camera_space_vertex);
			inside_world_space_vertices[num_inside_vertices] = vec3_clone(current_world_space_vertex);
			inside_normals[num_inside_vertices] = vec3_clone(current_normal);
			inside_texcoords[num_inside_vertices] = tex2_clone(current_texcoord);
			num_inside_vertices++;
		}
		
		prev_dot = current_dot;
		prev_camera_space_vertex = current_camera_space_vertex;
		prev_world_space_vertex = current_world_space_vertex;
		prev_normal = current_normal;
		prev_texcoord = current_texcoord;

		current_camera_space_vertex++;
		current_world_space_vertex++;
		current_normal++;
		current_texcoord++;
	}

	for (int i = 0; i < num_inside_vertices; i++) {
		polygon->camera_space_vertices[i] = vec3_clone(&inside_camera_space_vertices[i]);
		polygon->world_space_vertices[i] = vec3_clone(&inside_world_space_vertices[i]);
		polygon->normals[i] = vec3_clone(&inside_normals[i]);
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
