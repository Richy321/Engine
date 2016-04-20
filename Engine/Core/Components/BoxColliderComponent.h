#pragma once
#include "Interfaces/ICollider.h"
#include "../../Dependencies/glm/detail/type_vec3.hpp"
#include "Interfaces/IComponent.h"
#include "../../Physics/AABB.h"

namespace Core
{
	class BoxColliderComponent : public ICollider, public IComponent
	{
	public:
		AABB aabb;

		BoxColliderComponent(std::weak_ptr<Core::IGameObject> gameObj, AABB aabb) : IComponent(gameObj), aabb(aabb)
		{
		}

		BoxColliderComponent(std::weak_ptr<Core::IGameObject> gameObj, glm::vec3 min, glm::vec3 max) : IComponent(gameObj)
		{
			aabb.min = min;
			aabb.max = max;
		}

		~BoxColliderComponent()
		{
		}

		void Update(float deltaTime) override
		{

		}

		ComponentTypes GetComponentType() const override
		{
			return Collider;
		}

		ColliderType GetColliderType() override
		{
			return SphereCollider;
		}
	};
}
