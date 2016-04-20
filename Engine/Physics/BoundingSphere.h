#pragma once
#include "../Dependencies/glm/detail/type_vec3.hpp"

struct BoundingSphere
{
public:

	BoundingSphere()
	{
	}

	BoundingSphere(float radius)
	{
		this->radius = radius;
	}

	~BoundingSphere()
	{
	}

	float radius;
};

