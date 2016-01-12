#pragma once
#include <memory>
#include <vector>
#include "../Dependencies/glm/glm.hpp"
#include "../Dependencies/glm/gtc/matrix_transform.hpp"

namespace Core
{
	class IGameObject
	{
	public:
		virtual std::vector<std::weak_ptr<IGameObject>>& GetChildren() = 0;
		virtual std::weak_ptr<IGameObject>& GetParent()= 0;
		virtual glm::mat4& GetWorldTransform() = 0;
		virtual unsigned int GetID() = 0;

		IGameObject()
		{
		}

		virtual ~IGameObject()
		{
		}
	};
}
