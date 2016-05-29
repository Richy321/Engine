#pragma once
#include "ClothParticle.h"
#include "../../Core/Components/Interfaces/IRenderableComponent.h"
#include "../PhysicsManager.h"
#include "../../Core/Components/Interfaces/IPhysicsComponent.h"
#include "../IPhysicsManager.h"
#include "Cloth.h"
#include "Cloth2.h"

class ClothComponent : public IRenderableComponent, public IPhysicsComponent
{
public:

	ComponentTypes GetComponentType() const override { return Cloth; }

	std::shared_ptr<SphereColliderComponent> sphere;
	unsigned long lastUpdate;

	ClothComponent(std::weak_ptr<Core::IGameObject> gameObj, vec2 size, vec2 particleColRowCount) : IComponent(gameObj), IRenderableComponent(gameObj), IPhysicsComponent(gameObj)
	{
		//cloth = std::make_unique<Physics::Cloth::Cloth>(size, particleColRowCount);
		cloth2 = std::make_unique<Cloth2>(size, particleColRowCount);
		//cloth2->mesh->renderWireframe = true;
		lastUpdate = glutGet(GLUT_ELAPSED_TIME);
	}

	~ClothComponent()
	{
	}

	void Reset()
	{
	}
	std::unique_ptr<Physics::Cloth::Cloth> cloth;
	std::unique_ptr<Cloth2> cloth2;

	void OnFixedTimeStep(float deltaTime) override
	{
		//cloth->ApplyConstraints();
		//cloth->OnFixedTimeStep(deltaTime);
	}

	void Render(std::shared_ptr<Core::Camera> mainCamera) override
	{
		glDepthFunc(GL_LEQUAL);
		//cloth->Render(mainCamera, parentGameObject.lock()->GetWorldTransform());
		cloth2->Render(mainCamera, parentGameObject.lock()->GetWorldTransform());
	}

	void SetTexture(std::string texture)
	{
		//cloth->mesh->materialID = texture;
		cloth2->mesh->materialID = texture;
	}

	void AddForce(const vec3 force)
	{
		//cloth->AddForce(force);
	}

	void AddWindForce(const vec3 direction)
	{
		//cloth->AddWindForce(direction);
	}

	void HandleCollisions(std::vector<std::shared_ptr<ICollider>> colliders)
	{
		//cloth->HandleCollisions(colliders, parentGameObject.lock()->GetWorldTransform());
	}

	void Update(float deltaTime) override
	{
		mat4 trans = inverse(parentGameObject.lock()->GetWorldTransform());

		vec4 spherePosInClothSpace = trans * vec4(sphere->Position(), 1.0f);
		cloth2->CalculateForces(vec3(0.04f, -0.02f, 0.02f), vec3(spherePosInClothSpace), sphere->GetRadius() * 1.1f);

		unsigned long currentUpdate = glutGet(GLUT_ELAPSED_TIME);
		cloth2->Update((currentUpdate - lastUpdate)/100.0f);
		lastUpdate = currentUpdate;

		//cloth2->Update(deltaTime);
		//cloth->ApplyConstraints();
		//cloth->OnFixedTimeStep(deltaTime);
	}
};

