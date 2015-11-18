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
		virtual void Render() = 0;
	};
}

