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
			SphereCollider,
			AABBCollider
		};

		virtual ColliderType GetColliderType() = 0;
	};
}
