#pragma once
#include "../../Dependencies/glm/detail/type_vec3.hpp"

class Particle
{
public:

	Particle()
	{
	}

	virtual ~Particle()
	{
	}

	void ComputeForces(std::vector<glm::vec3> forces)
	{
		
	}

	float mass;
	glm::vec3 velocity;
	glm::vec3 position;
	
};

