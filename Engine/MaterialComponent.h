#pragma once
#include "Core/Components/Interfaces/IComponent.h"
#include "Dependencies/glm/detail/type_vec4.hpp"

namespace Core
{
	class MaterialComponent : public IComponent
	{
	public:

		glm::vec4 colour;
		float reflection;


		MaterialComponent(std::weak_ptr<Core::IGameObject> gameObj) : IComponent(gameObj)
		{
		}

		~MaterialComponent()
		{
		}
	};
}
