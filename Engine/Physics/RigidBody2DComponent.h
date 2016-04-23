#pragma once
#include "../Core/Components/Interfaces/IPhysicsComponent.h"
#include "../Dependencies/glm/detail/type_vec2.hpp"
#include "PhysicsMaterial.h"
#include "../Core/Utils.h"

using namespace Core;

class RigidBody2DComponent : public IPhysicsComponent
{
public:

	RigidBody2DComponent(std::weak_ptr<Core::IGameObject> gameObj) : IPhysicsComponent(gameObj)
	{
		Reset();
	}

	~RigidBody2DComponent()
	{
	}

	ComponentTypes GetComponentType() const override
	{
		return RigidBody;
	}

	//basic
	glm::vec2 velocity;
	glm::vec2 force;

	//orientated
	float angularVelocity;
	float torque;
	float orient;

	//mass data
	float mass;
	float inverseMass;
	float inertia;
	float inverseInertia;

	float staticFriction;
	float dynamicFriction;

	std::shared_ptr<PhysicsMaterial> physicsMaterial;

	float gravityScale; //needed?

	void UpdatePhysics(float deltaTime) override
	{

	}

	void Update(float deltaTime)
	{

	}

	void ApplyForce(const glm::vec2& f)
	{
		force += f;
	}

	void ApplyImpulse(const glm::vec2& impulse, const glm::vec2& contactVector)
	{
		velocity += inverseMass * impulse;
		//angularVelocity += inverseInertia * Utils::CrossVec2(contactVector, impulse);
	}

	void SetStatic()
	{
		inverseMass = 0.0f;
		mass = 0.0f;
		inverseInertia = 0.0f;
		inertia = 0.0f;
	}

	void Reset()
	{
		velocity.x = 0.0f;
		velocity.y = 0.0f;
		angularVelocity = 0;
		torque = 0;
		orient = glm::linearRand(-M_PI, M_PI);
		force.x = 0.0f;
		force.y = 0.0f;
		staticFriction = 0.5f;
		dynamicFriction = 0.3f;
		
		physicsMaterial = std::make_shared<PhysicsMaterial>(0.3f, 0.2f);
	}

};

