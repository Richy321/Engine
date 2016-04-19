#pragma once
#include "Interfaces/ICollider.h"
#include "../../Dependencies/glm/detail/type_vec3.hpp"
#include "Interfaces/IComponent.h"

namespace Core
{
	class BoxColliderComponent : public ICollider, public IComponent
	{
	public:

		BoxColliderComponent(std::weak_ptr<Core::IGameObject> gameObj, glm::vec3 min, glm::vec3 max) : IComponent(gameObj)
		{
			this->min = min;
			this->max = max;
		}

		~BoxColliderComponent()
		{
		}

		glm::vec3 min;
		glm::vec3 max;

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
