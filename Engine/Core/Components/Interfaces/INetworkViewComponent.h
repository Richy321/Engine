#pragma once
#include "IComponent.h"
#include "../../Networking/MessageStructures.h"

namespace Core
{
	class INetworkViewComponent : public virtual IComponent
	{
	public:

		INetworkViewComponent(std::weak_ptr<IGameObject> gameObj) : IComponent(gameObj)
		{
			componentFlags |= NetworkSyncable;
		}

		virtual void UpdateComms(float deltaTime) = 0;
		virtual GUID GetUniqueID() = 0;
		virtual void SetUniqueID(GUID id)= 0;

		virtual void ReadPacket(std::shared_ptr<networking::MessageStructures::BaseMessage>& packet) = 0;
		virtual int BuildPacket(std::shared_ptr<networking::MessageStructures::BaseMessage>& message) = 0;

		virtual bool& IsSendUpdates() = 0;
		virtual void ClearReceivedMessages() = 0;

		virtual void SendReceivedMessages(networking::IConnection* connection) = 0;
		virtual void ProcessMessages() = 0;
		
		virtual bool& IsFlaggedForDeletion() = 0;
		virtual bool IsPrimaryPlayerView() = 0;

	};
}

