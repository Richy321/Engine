#pragma once
#include "Interfaces/ICollider.h"
#include "../../Dependencies/glm/detail/type_vec3.hpp"

namespace Core
{
	class SphereColliderComponent : public ICollider, public IComponent
	{
	public:

		~SphereColliderComponent()
		{
		}

		SphereColliderComponent(std::weak_ptr<Core::IGameObject> gameObj, float radius, glm::vec3 position) : IComponent(gameObj)
		{
			this->radius = radius;
			this->position = position;
		}

		float radius;
		vec3 position;

		void Update(float deltaTime) override
		{
			
		}

		ComponentTypes GetComponentType() const override
		{
			return Collider;
		}

		ColliderType GetColliderType() override
		{
			return Sphere;
		}
	};
}

