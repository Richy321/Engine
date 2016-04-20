#pragma once
#include "../Dependencies/glm/detail/type_vec2.hpp"
#include <vector>

struct BoundingPolygon
{
public:
	BoundingPolygon()
	{
	}

	~BoundingPolygon()
	{
	}


	std::vector<glm::vec2> vertices;
	std::vector<glm::vec2> normals;
};

