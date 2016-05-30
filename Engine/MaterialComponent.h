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
			material = std::make_shared<Material>();
		}

		~MaterialComponent()
		{
		}

		std::shared_ptr<Material> GetMaterial() const
		{
			return material;
		}


		ComponentTypes GetComponentType() const override
		{
			return RenderMaterial;
		}

	protected:
		std::shared_ptr<Material> material;
	};
}
