#pragma once
#include "../Networking/NetworkServices.h"
#include "../Networking/FlowControl.h"
#include <chrono>
#include "Interfaces/INetworkViewComponent.h"
#include "../Networking/Connection.h"


class NetworkViewComponent : public Core::INetworkViewComponent
{
public:

	networking::IConnection* serverConnection;
	std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> lastTime;
	networking::FlowControl flowControl;
	bool connected = false;

	NetworkViewComponent(std::weak_ptr<Core::IGameObject> parent): INetworkViewComponent(parent)
	{

	}

	void InitialiseConnectionToServer()
	{
		if (isUseReliableConnection)
			serverConnection = networking::NetworkServices::GetInstance().CreateReliableConnection(ProtocolId, TimeOut);
		else
			serverConnection = networking::NetworkServices::GetInstance().CreateConnection(ProtocolId, TimeOut);

		if (!serverConnection->Start(ClientPort))
		{
			printf("could not start connection on port %d\n", ClientPort);
			return;
		}

		serverConnection->Connect(networking::Address(127, 0, 0, 1, ServerPort));
	}

	virtual ~NetworkViewComponent()
	{
	}

	void Update(float deltaTime) override
	{

	}

	void UpdateComms(float deltaTime) override
	{
		if (!connected && serverConnection->IsConnected())
		{
			printf("client connected to server\n");
			connected = true;
		}

		if (!connected && serverConnection->ConnectFailed())
		{
			printf("connection failed\n");
		}

		if (serverConnection->IsConnected())
		{
			if (isUseReliableConnection)
				flowControl.Update(deltaTime, ((networking::ReliableConnection*)serverConnection)->GetReliabilitySystem().GetRoundTripTime() * 1000.0f);
		}

		const float sendRate = flowControl.GetSendRate();

		unsigned char packet[] = "client to server";
		serverConnection->SendPacket(packet, sizeof(packet));

		while (true)
		{
			unsigned char packet[256];
			int bytes_read = serverConnection->ReceivePacket(packet, sizeof(packet));
			if (bytes_read == 0)
				break;
			printf("received packet from server\n");
		}

		serverConnection->Update(deltaTime);
	}

	void Destroy() override
	{
		delete serverConnection;
	}
};

