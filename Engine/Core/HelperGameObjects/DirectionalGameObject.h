#pragma once
#include "../GameObject.h"

namespace Core
{
namespace Helper
{
	class DirectionalGameObject : GameObject
	{
	public:
		glm::vec3 forward;
		glm::vec3 right;
		glm::vec3 up;

		DirectionalGameObject()
		{
		}

		~DirectionalGameObject()
		{
		}

		void MoveForward(float amount)
		{
			
		}

		void MoveSideways(float amount)
		{
			
		}

		void MoveUp(float amount)
		{
			
		}

	};
}
}
