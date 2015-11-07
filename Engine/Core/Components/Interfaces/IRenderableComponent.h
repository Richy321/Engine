#pragma once
#include "../../IComponent.h"

namespace Core
{
	class IRenderableComponent : public IComponent
	{
	public:

		IRenderableComponent()
		{
			componentFlags |= ComponentFlags::Renderable;
		}
		virtual void Render() = 0;
	};
}

