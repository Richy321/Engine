#pragma once
#include "../IPhysicsManager.h"
#include <memory>
#include "RigidBody2DComponent.h"

class PhysicsManager: public IPhysicsManager
{
public:

	PhysicsManager()
	{
	}

	~PhysicsManager() override
	{
	}
};

