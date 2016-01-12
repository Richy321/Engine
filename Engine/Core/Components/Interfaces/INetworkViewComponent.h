#pragma once
#include "IComponent.h"

namespace Core
{
	class INetworkViewComponent : public IComponent
	{
	public:

		INetworkViewComponent(std::weak_ptr<IGameObject> gameObj) : IComponent(gameObj)
		{
			componentFlags |= NetworkSyncable;
		}

		virtual void UpdateComms(float deltaTime) = 0;
	};
}

