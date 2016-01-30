#pragma once
#include "../Networking/NetworkServices.h"
#include "../Networking/FlowControl.h"
#include <chrono>
#include "Interfaces/INetworkViewComponent.h"
#include "../Networking/Connection.h"
#include "../../Dependencies/glm/glm.hpp"
#include <Objbase.h>
#include "../Networking/MessageStructures.h"
#include "../Networking/IClientNetworkManager.h"
#include "DirectionalMovementComponent.h"
#include <mutex>

class NetworkViewComponent : public Core::INetworkViewComponent, public std::enable_shared_from_this<Core::INetworkViewComponent>
{
public:
	GUID uniqueID;
	std::shared_ptr<networking::INetworkManager> networkingManager;
	bool sendUpdates; // should we send updates about this network view to the server
	bool& IsSendUpdates() override { return sendUpdates; }

	std::vector<std::shared_ptr<networking::MessageStructures::BaseMessage>> receivedMessages;
	
	std::mutex mutexReceivedMsg;

	enum DeadReckoningType
	{
		None,
		Linear
	};

	DeadReckoningType deadReckoning = None;

	NetworkViewComponent(std::weak_ptr<Core::IGameObject> parent, std::shared_ptr<networking::INetworkManager> networkingManager) : INetworkViewComponent(parent), networkingManager(networkingManager)
	{
		CoCreateGuid(&uniqueID);
		sendUpdates = false;
	}

	void AddToNetworkingManager()
	{
		networkingManager->AddNetworkViewComponent(shared_from_this());
	}

	virtual ~NetworkViewComponent()
	{

	}

	virtual void ProcessMessages() override
	{
		std::shared_ptr<networking::MessageStructures::BaseMessage> lastState = nullptr;

		switch (deadReckoning)
		{
			case None:
			{
				std::lock_guard<std::mutex> lock(mutexReceivedMsg);
				for (auto &i : receivedMessages)
				{
					if (i->simpleType == networking::MessageStructures::SnapShot)
						lastState = i;
				}

				if (lastState != nullptr)
				{
					parentGameObject.lock()->GetWorldTransform()[3].x = lastState->positionMessage.position.x;
					parentGameObject.lock()->GetWorldTransform()[3].y = lastState->positionMessage.position.y;
					parentGameObject.lock()->GetWorldTransform()[3].z = lastState->positionMessage.position.z;
				}
			}
			break;
			case Linear:
				break;
		}
	}


	void Update(float deltaTime) override
	{
		
	}

	virtual void UpdateComms(float deltaTime) override
	{
		
	}

	//max 256
	virtual int BuildPacket(std::shared_ptr<networking::MessageStructures::BaseMessage>& message) override
	{
		mat4 transform = GetParentGameObject().lock()->GetWorldTransform();

		message->uniqueID = GetUniqueID();
		message->messageType = networking::MessageStructures::None;
		message->positionOrientationMessage.position = vec3(transform[3].x, transform[3].y, transform[3].z);

		return sizeof(networking::MessageStructures::BaseMessage);
	}

	virtual void ReadPacket(std::shared_ptr<networking::MessageStructures::BaseMessage>& packet) override
	{
		std::lock_guard<std::mutex> lock(mutexReceivedMsg);
		receivedMessages.push_back(packet);
	}

	GUID GetUniqueID() override
	{
		return uniqueID;
	}

	void SetUniqueID(GUID id) override
	{
		uniqueID = id;
	}
	
	void Destroy() override
	{
	}

	ComponentTypes GetComponentType() const override { return NetworkView; }

	void SendReceivedMessages(networking::IConnection* connection) override
	{
		if (connection == nullptr)
			return;

		std::lock_guard<std::mutex> lock(mutexReceivedMsg);

		//fwd all messages on to clients
		for (auto &message : receivedMessages)
		{
			connection->SendPacket(reinterpret_cast<unsigned char*>(message.get()), sizeof(networking::MessageStructures::BaseMessage));
		}
	}

	void ClearReceivedMessages()
	{
		std::lock_guard<std::mutex> lock(mutexReceivedMsg);
		receivedMessages.clear();
	}
};

