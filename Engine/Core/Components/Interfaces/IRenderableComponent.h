#pragma once
#include "../../IComponent.h"

namespace Core
{
	class IRenderableComponent : public IComponent
	{
	public:

		IRenderableComponent(std::weak_ptr<IGameObject> gameObj) : IComponent(gameObj)
		{
			componentFlags |= Renderable;
		}
		virtual void Render(Math::mat4 &view, Math::mat4 &projection) = 0;
	};
}

