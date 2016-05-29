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
		radius2 = radius * radius; //commonly used so cache it.
	}

	~BoundingSphere()
	{
	}

	float radius;
	float radius2;
};

