#pragma once
#include "Core/Components/Interfaces/IComponent.h"
#include "Dependencies/glm/detail/type_vec4.hpp"
#include "Material.h"

namespace Core
{
	class MaterialComponent : public IComponent
	{
	public:
		MaterialComponent(std::weak_ptr<Core::IGameObject> gameObj) : IComponent(gameObj)
		{
		}

		~MaterialComponent()
		{
		}

		std::shared_ptr<Material> GetMaterial() const
		{
			return material;
		}

	protected:
		std::shared_ptr<Material> material;
	};
}
