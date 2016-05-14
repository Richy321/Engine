#pragma once
#include "../Core/Components/Interfaces/IPhysicsComponent.h"
#include "../Dependencies/glm/detail/type_vec2.hpp"
#include "PhysicsMaterial.h"
#include "../Core/Utils.h"

using namespace Core;

class RigidBody2DComponent : public IPhysicsComponent
{
public:

	RigidBody2DComponent(std::weak_ptr<Core::IGameObject> gameObj) : IComponent(gameObj), IPhysicsComponent(gameObj)
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

	//linear
	float mass;
	float inverseMass;

	//angular
	float angularVelocity;
	float torque;
	float orient; // in radians
	float inertia;
	float inverseInertia;

	float staticFriction;
	float dynamicFriction;

	mat2 u;
	vec2 position;

	std::shared_ptr<PhysicsMaterial> physicsMaterial;

	void OnFixedTimeStep(float deltaTime) override
	{

	}

	void Update(float deltaTime)
	{

	}

	void ApplyForce(const glm::vec2& f)
	{
		force += f;
	}

	void ApplyImpulse(const glm::vec2& impulse, const glm::vec2& contactVector, bool applyAngular = false)
	{
		velocity += inverseMass * impulse;
		if(applyAngular)
			angularVelocity += inverseInertia * Utils::CrossVec2(contactVector, impulse);
	}

	void SetStatic()
	{
		inverseMass = 0.0f;
		mass = 0.0f;
		inverseInertia = 0.0f;
		inertia = 0.0f;
		GetParentGameObject().lock()->SetPosition2D(position);
		GetParentGameObject().lock()->SetOrientation2D(orient);
	}

	void Reset()
	{
		velocity.x = 0.0f;
		velocity.y = 0.0f;
		angularVelocity = 0;
		torque = 0;
		orient = 0;
		force.x = 0.0f;
		force.y = 0.0f;
		staticFriction = 0.2f;
		dynamicFriction = 0.1f;
		
		physicsMaterial = std::make_shared<PhysicsMaterial>(0.3f, 0.2f);
		u = mat2();
		position = vec2();
	}

	void SetOrientation2D(float rad)
	{
		float c = std::cos(rad);
		float s = std::sin(rad);
		u[0][0] = c;
		u[0][1] = -s;
		u[1][0] = s;
		u[1][1] = c;
	}
};

