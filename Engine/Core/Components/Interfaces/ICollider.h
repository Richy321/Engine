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
			PolygonCollider,
			Count
		};

		virtual ColliderType GetColliderType() = 0;
	};
}
