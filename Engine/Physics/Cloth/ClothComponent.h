#pragma once
#include "ClothParticle.h"
#include "../../Core/Components/Interfaces/IRenderableComponent.h"
#include "../PhysicsManager.h"
#include "../../Core/Components/Interfaces/IPhysicsComponent.h"
#include "../IPhysicsManager.h"
#include "Cloth.h"

class ClothComponent : public IRenderableComponent, public IPhysicsComponent
{
public:

	ComponentTypes GetComponentType() const override { return Cloth; }

	ClothComponent(std::weak_ptr<Core::IGameObject> gameObj, vec2 size, vec2 particleColRowCount) : IComponent(gameObj), IRenderableComponent(gameObj), IPhysicsComponent(gameObj)
	{
		cloth = std::make_unique<Physics::Cloth::Cloth>(size, particleColRowCount);
	}

	~ClothComponent()
	{
	}

	void Reset()
	{
	}
	std::unique_ptr<Physics::Cloth::Cloth> cloth;


	void OnFixedTimeStep(float deltaTime) override
	{
		//cloth->ApplyConstraints();
		//cloth->OnFixedTimeStep(deltaTime);
	}

	void Render(std::shared_ptr<Core::Camera> mainCamera) override
	{
		cloth->Render(mainCamera, parentGameObject.lock()->GetWorldTransform());
	}

	void SetTexture(std::string texture)
	{
		cloth->mesh->materialID = texture;
	}

	void AddForce(const vec3 force)
	{
		cloth->AddForce(force);
	}

	void AddWindForce(const vec3 direction)
	{
		cloth->AddWindForce(direction);
	}

	void HandleCollisions(std::vector<std::shared_ptr<ICollider>> colliders)
	{
		cloth->HandleCollisions(colliders, parentGameObject.lock()->GetWorldTransform());
	}

	void Update(float deltaTime) override
	{
		cloth->ApplyConstraints();
		cloth->OnFixedTimeStep(deltaTime);
	}
};

