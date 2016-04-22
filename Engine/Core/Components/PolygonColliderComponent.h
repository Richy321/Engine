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

		PolygonColliderComponent(std::weak_ptr<IGameObject> gameObj, BoundingPolygon polygonCollider) : IComponent(gameObj), polygonCollider(polygonCollider)
		{
		}

		PolygonColliderComponent(std::weak_ptr<IGameObject> gameObj, std::vector<vec2> verts, std::vector<vec2> norms): IComponent(gameObj)
		{
			polygonCollider.vertices = verts;
			polygonCollider.normals = norms;
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

