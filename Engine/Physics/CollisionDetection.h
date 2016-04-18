#pragma once
#include  "BoundingSphere.h"
#include "AABB.h"
#include <memory>
#include "../Dependencies/glm/detail/type_vec2.hpp"
#include "../Dependencies/glm/detail/func_geometric.inl"

class Collisions
{
private:

public:
	static bool CircleVsCircle(std::shared_ptr<BoundingSphere> sphereA, std::shared_ptr<BoundingSphere> sphereB)
	{
		float radiusSquared = sphereA->radius * sphereA->radius + sphereB->radius * sphereB->radius;
		glm::vec2 normal = sphereB->position - sphereA->position;
		float distanceSquared = normal.x * normal.x + normal.y * normal.y;

		return radiusSquared < distanceSquared;
	}

	static bool AABBvsAABB(std::shared_ptr<AABB> a, std::shared_ptr<AABB> b)
	{
		if (a->max.x < b->min.x || a->min.x > b->max.x) return false;
		if (a->max.y < b->min.y || a->min.y > b->max.y) return false;

		//no seperating axis, at least one overlapping axis
		return true;
	}

};

