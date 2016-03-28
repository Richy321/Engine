#pragma once
#include <vector>
#include "Particle.h"
#include "../../Core/Components/Interfaces/IRenderableComponent.h"
#include "../PhysicsManager.h"
#include "../../Core/Components/Interfaces/IPhysicsComponent.h"
#include "../IPhysicsManager.h"

using namespace glm;

class ParticleSystemComponent : public Core::IRenderableComponent , public IPhysicsComponent
{
public:
	
	std::vector<std::shared_ptr<Particle>> particles;
	size_t particleCount = 100;

	ComponentTypes GetComponentType() const override { return ComponentTypes::ParticleSystem; }



	ParticleSystemComponent(std::weak_ptr<Core::IGameObject> gameObj) : IComponent(gameObj), IRenderableComponent(gameObj), IPhysicsComponent(gameObj)
	{
		Initialise();
	}

	~ParticleSystemComponent()
	{
	}


	void Initialise()
	{
		for (auto i = 0; i < particleCount; i++)
		{
			std::shared_ptr<Particle> particle = std::make_shared<Particle>();

			particle->mass = 1.0f;
			particle->position = vec3(0.0f, 0.0f, 0.0f);
			particle->velocity = vec3(0.0f, 0.0f, 0.0f);
			particles.push_back(particle);
		}
	}

	void Render(std::shared_ptr<Core::Camera> mainCamera) override
	{
		for (auto i = 0; i < particleCount; i++)
		{
			//todo render something
		}
	}

	void UpdatePhysics(float deltaTime) override
	{
		std::vector<vec3> forces
		{
			vec3(0.0f, IPhysicsManager::gravity, 0.0f)
		};

		for (auto i = 0; i < particles.size(); i++)
		{
			particles[i]->ApplyForces(forces, deltaTime);
		}
	}
};

