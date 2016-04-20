#pragma once
#include "Interfaces/ICollider.h"
#include "Interfaces/IComponent.h"
#include "../../Physics/BoundingPolygon.h"

namespace Core
{
	class PolygonColliderComponent : public ICollider, public IComponent
	{
	public:
		BoundingPolygon polygonCollider;

		PolygonColliderComponent(std::weak_ptr<Core::IGameObject> gameObj, BoundingPolygon polygonCollider) : IComponent(gameObj), polygonCollider(polygonCollider)
		{
		}

		PolygonColliderComponent(std::weak_ptr<Core::IGameObject> gameObj, std::vector<glm::vec2> verts) : IComponent(gameObj)
		{
			polygonCollider.vertices = verts;
		}

		~PolygonColliderComponent()
		{}

		void Update(float deltaTime) override
		{

		}

		ComponentTypes GetComponentType() const override
		{
			return Collider;
		}

		ColliderType GetColliderType() override
		{
			return PolygonCollider;
		}

	};
}

