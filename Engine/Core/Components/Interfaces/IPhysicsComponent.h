#pragma once
#include "IComponent.h"

class IPhysicsComponent : public virtual Core::IComponent
{
public:

	IPhysicsComponent(std::weak_ptr<Core::IGameObject> gameObj) : IComponent(gameObj)
	{
		componentFlags |= Core::Physics;
	}

	virtual ~IPhysicsComponent()
	{
	}

	virtual void UpdatePhysics(float deltaTime) = 0;
};