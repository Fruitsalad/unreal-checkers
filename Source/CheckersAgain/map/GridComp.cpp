#include "GridComp.h"

UGridComp::UGridComp() {
	PrimaryComponentTick.bCanEverTick = false;
}

Vec UGridComp::calc_cell_center(int x, int y) const {
	let local_x = x * tileWorldSize + tileWorldSize / 2;
	let local_y = y * tileWorldSize + tileWorldSize / 2;
	let local_cell_center = Vec(local_x, local_y, 0);
	let local_to_world = GetOwner()->GetActorTransform();
	return local_to_world.TransformPosition(local_cell_center);
}

Vec UGridComp::calc_cell_center(Vec2i pos) const {
	return calc_cell_center(pos.X, pos.Y);
}

Vec2i UGridComp::calc_nearest_cell(Vec world_pos) const {
	let local_to_world = GetOwner()->GetActorTransform();
	let local_pos = local_to_world.InverseTransformPosition(world_pos);
	int x = floor(local_pos.X / tileWorldSize);
	int y = floor(local_pos.Y / tileWorldSize);
	return {x, y};
}

bool UGridComp::intersects_ray(Ray ray, Vec2i* result_cell) const {
	let plane_normal = grid_plane.GetNormal();

	// Check if the ray and the plane are parallel...
	let direction_onto_normal = dot(ray.Direction, plane_normal);
	if (direction_onto_normal <= FLT_EPSILON)
		return false;

	// Calculate where a *line* would intersect with the plane...
	let plane_origin = plane_normal * grid_plane.W;
	let distance = dot(plane_origin - ray.Origin, plane_normal) /
				   direction_onto_normal;  // Distance to the intersection

	// If the distance was smaller than zero, then the intersection falls
	// outside of the part of the line that is covered by the ray, and therefore
	// we don't have an intersection...
	if (distance < 0)
		return false;

	// We do have an intersection :)
	let world_space_intersection = ray.Origin + ray.Direction * distance;
	if (result_cell != nullptr)
		*result_cell = calc_nearest_cell(world_space_intersection);
	return true;
}


