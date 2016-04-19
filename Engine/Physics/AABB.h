#pragma once
#include "../Dependencies/glm/detail/type_vec3.hpp"

struct AABB
{
public:
	AABB()
	{
	}

	AABB(glm::vec3 min, glm::vec3 max)
	{
		this->min = min;
		this->max = max;
	}

	~AABB()
	{
	}

	glm::vec3 min;
	glm::vec3 max;
};

