#pragma once
#include "Interfaces/ICollider.h"
#include "../../Dependencies/glm/detail/type_vec3.hpp"
#include "../../Physics/BoundingSphere.h"

namespace Core
{
	class SphereColliderComponent : public ICollider, public IComponent
	{
	public:
		BoundingSphere boundingSphere;

		~SphereColliderComponent()
		{
		}

		SphereColliderComponent(std::weak_ptr<Core::IGameObject> gameObj, BoundingSphere boundingSphere) : IComponent(gameObj), boundingSphere(boundingSphere)
		{
		}

		SphereColliderComponent(std::weak_ptr<Core::IGameObject> gameObj, float radius) : IComponent(gameObj)
		{
			boundingSphere.radius = radius;
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

