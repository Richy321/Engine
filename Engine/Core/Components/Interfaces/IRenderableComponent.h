#pragma once
#include "IComponent.h"

namespace Core
{
	class Camera;

	class IRenderableComponent : public virtual IComponent
	{
	public:

		IRenderableComponent(std::weak_ptr<IGameObject> gameObj) : IComponent(gameObj)
		{
			componentFlags |= Renderable;
		}
		virtual void Render(std::shared_ptr<Camera> mainCamera) = 0;
	};
}

