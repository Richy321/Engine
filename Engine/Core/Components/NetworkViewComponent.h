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

class NetworkViewComponent : public Core::INetworkViewComponent, public std::enable_shared_from_this<Core::INetworkViewComponent>
{
public:
	GUID uniqueID;
	networking::IClientNetworkManager& networkingManager;
	bool sendUpdates; // should we send updates about this network view to the server

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

	}

	virtual void UpdateComms(float deltaTime) override
	{
		
	}

	//max 256
	virtual int BuildPacket(networking::MessageStructures::BaseMessage& message) override
	{
		mat4 transform = GetParentGameObject().lock()->GetWorldTransform();

		message.uniqueID = GetUniqueID();
		message.messageType = networking::MessageStructures::BasicPosition;
		message.positionMessage.position = vec3(transform[3].x, transform[3].y, transform[3].z);

		return sizeof(networking::MessageStructures::BaseMessage);
	}

	virtual void ReadPacket(networking::MessageStructures::BaseMessage& packet) override
	{
		if (packet.messageType == networking::MessageStructures::BasicPosition)
		{
			if (deadReckoning == Exact)
			{
				parentGameObject.lock()->GetWorldTransform()[3].x = packet.positionMessage.position.x;
				parentGameObject.lock()->GetWorldTransform()[3].y = packet.positionMessage.position.y;
				parentGameObject.lock()->GetWorldTransform()[3].z = packet.positionMessage.position.z;
			}
		}
	}

	GUID GetUniqueID() override
	{
		return uniqueID;
	}

	bool GetIsSendUpdates() override
	{
		return sendUpdates;
	}

	void Destroy() override
	{
	}
};

