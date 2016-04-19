#pragma once
#include "RigidBody2DComponent.h"
#include <vector>

class Manifold
{
public:

	Manifold(std::shared_ptr<RigidBody2DComponent> bodyA, std::shared_ptr<RigidBody2DComponent> bodyB) : bodyA(bodyA), bodyB(bodyB)
	{
	}

	~Manifold()
	{
	}

	std::shared_ptr<RigidBody2DComponent> bodyA;
	std::shared_ptr<RigidBody2DComponent> bodyB;

	float penetration;
	glm::vec2 normal;
	std::vector<glm::vec2> contacts;
	size_t contactCount;

	float e;
	float df;
	float sf;

};

