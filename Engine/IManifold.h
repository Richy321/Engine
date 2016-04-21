#pragma once
class IManifold : public std::enable_shared_from_this<IManifold>
{
public:

	IManifold()
	{
	}

	virtual ~IManifold()
	{
	}

	std::shared_ptr<RigidBody2DComponent> bodyA;
	std::shared_ptr<RigidBody2DComponent> bodyB;

	float penetration;
	glm::vec2 normal;
	std::vector<glm::vec2> contacts;
	size_t contactCount;

	float e; //avg restitution
	float dynamicFriction;
	float staticFriction;
	
	virtual void Solve() = 0;
	virtual void Initialise(float dt, vec2 gravity) = 0;
	virtual void ApplyImpulse() = 0;
	virtual void PositionalCorrection() = 0;
	virtual void InfiniteMassCorrection() = 0;
};

