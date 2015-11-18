#pragma once
#include <memory>
#include <vector>

namespace Core
{
	class IGameObject
	{
	public:
		virtual std::vector<std::weak_ptr<IGameObject>>& GetChildren() = 0;
		virtual std::weak_ptr<IGameObject>& GetParent()= 0;
		virtual Math::mat4& GetWorldTransform() = 0;

		IGameObject()
		{
		}

		virtual ~IGameObject()
		{
		}
	};
}
