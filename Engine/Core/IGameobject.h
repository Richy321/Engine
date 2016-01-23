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

		virtual void Translate(glm::vec3 trans) = 0;
		virtual void Translate(float x, float y, float z) = 0;
		virtual void Rotate(float angle, glm::vec3 axis) = 0;
		virtual void RotateX(float angle) = 0;
		virtual void RotateY(float angle) = 0;
		virtual void RotateZ(float angle) = 0;
		virtual void Scale(float s) = 0;
		virtual void Scale(float x, float y, float z) = 0;
		virtual void Scale(vec3 scale) = 0;

		IGameObject()
		{
		}

		virtual ~IGameObject()
		{
		}
	};
}
