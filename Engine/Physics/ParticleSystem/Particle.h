#pragma once
#include "../../Dependencies/glm/detail/type_vec3.hpp"
#include "../PhysicsManager.h"
#include <memory>
#include <vector>

class Particle
{
public:

	Particle()
	{
	}

	virtual ~Particle()
	{
	}

	void ApplyForces(std::vector<glm::vec3> forces, float deltaTime)
	{
		for each (glm::vec3 force in forces)
		{
			glm::vec3 acceleration = glm::vec3(force.x / mass, force.y / mass, force.z / mass);
			acceleration *= deltaTime;
			velocity.x += acceleration.x;
			velocity.y += acceleration.y;
			velocity.z += acceleration.z;
		}

		position.x += velocity.x * deltaTime;
		position.y += velocity.y * deltaTime;
		position.z += velocity.z * deltaTime;
	}

	float mass;
	glm::vec3 velocity;
	glm::vec3 position;
	
};

