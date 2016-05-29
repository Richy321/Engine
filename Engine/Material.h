#pragma once
#include "Dependencies/glm/detail/type_vec4.hpp"

class Material
{
public:

	glm::vec4 colour;
	glm::vec4 emissionColour;
	float transparency;
	float reflection;

	Material()
	{
	}

	~Material()
	{
	}
};

