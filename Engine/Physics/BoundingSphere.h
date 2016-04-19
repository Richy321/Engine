#pragma once
#include "../Dependencies/glm/detail/type_vec3.hpp"

struct BoundingSphere
{
public:

	BoundingSphere()
	{
	}

	BoundingSphere(float radius, glm::vec3 position)
	{
		this->radius = radius;
		this->position = position;
	}

	~BoundingSphere()
	{
	}

	float radius;
	glm::vec3 position;
};

