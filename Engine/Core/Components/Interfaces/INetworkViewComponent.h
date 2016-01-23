#pragma once
#include "IComponent.h"
#include "../../Networking/MessageStructures.h"

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
		virtual GUID GetUniqueID() = 0;
		virtual bool GetIsSendUpdates() = 0;

		virtual void ReadPacket(networking::MessageStructures::BaseMessage& packet) = 0;
		virtual int BuildPacket(networking::MessageStructures::BaseMessage& message) = 0;



	};
}

