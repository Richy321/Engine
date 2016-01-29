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

class NetworkViewComponent : public Core::INetworkViewComponent, public std::enable_shared_from_this<Core::INetworkViewComponent>
{
public:
	GUID uniqueID;
	networking::IClientNetworkManager& networkingManager;
	bool sendUpdates; // should we send updates about this network view to the server
	bool IsSendUpdates() override { return sendUpdates; }

	std::vector < std::shared_ptr<networking::MessageStructures::BaseMessage>> receivedMessages;

	enum DeadReckoningType
	{
		None,
		Exact,
		Linear
	};

	DeadReckoningType deadReckoning = Exact;

	NetworkViewComponent(std::weak_ptr<Core::IGameObject> parent, networking::IClientNetworkManager& networkingManager) : INetworkViewComponent(parent), networkingManager(networkingManager)
	{
		CoCreateGuid(&uniqueID);
		sendUpdates = false;
	}

	void AddToNetworkingManager()
	{
		networkingManager.AddNetworkViewComponent(shared_from_this());
	}

	virtual ~NetworkViewComponent()
	{

	}

	void Update(float deltaTime) override
	{
		std::shared_ptr<networking::MessageStructures::BaseMessage> lastState;

		if (deadReckoning == Exact)
		{
			for (auto &i : receivedMessages)
			{
				if (i->simpleType == networking::MessageStructures::SnapShot)
				{
					lastState = i;
				}
			}
		}

		if(lastState != nullptr)
		{
			parentGameObject.lock()->GetWorldTransform()[3].x = lastState->positionMessage.position.x;
			parentGameObject.lock()->GetWorldTransform()[3].y = lastState->positionMessage.position.y;
			parentGameObject.lock()->GetWorldTransform()[3].z = lastState->positionMessage.position.z;
		}
	}

	virtual void UpdateComms(float deltaTime) override
	{
		
	}

	//max 256
	virtual int BuildPacket(std::shared_ptr<networking::MessageStructures::BaseMessage> message) override
	{
		mat4 transform = GetParentGameObject().lock()->GetWorldTransform();

		message->uniqueID = GetUniqueID();
		message->messageType = networking::MessageStructures::None;
		message->positionOrientationMessage.position = vec3(transform[3].x, transform[3].y, transform[3].z);

		return sizeof(networking::MessageStructures::BaseMessage);
	}

	virtual void ReadPacket(std::shared_ptr<networking::MessageStructures::BaseMessage> packet) override
	{
		receivedMessages.push_back(packet);
	}

	GUID GetUniqueID() override
	{
		return uniqueID;
	}

	bool GetIsSendUpdates() override
	{
		return sendUpdates;
	}

	void SetIsSendUpdates(bool value)
	{
		sendUpdates = value;
	}

	void Destroy() override
	{
	}


	ComponentTypes GetComponentType() const override { return NetworkView; }
};

