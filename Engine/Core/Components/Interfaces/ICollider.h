#pragma once
namespace Core
{
	class ICollider
	{
	public:
		virtual ~ICollider()
		{
		}

		enum ColliderType
		{
			Sphere,
			AABB
		};

		virtual ColliderType GetColliderType() = 0;
	};
}
