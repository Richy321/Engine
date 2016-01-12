#pragma once
#include "../Managers/SceneManager.h"
#include "../Core/Components/MeshComponent.h"
#include "../Core/Camera.h"
#include "../Core/CameraFPS.h"
#include "../Core/AssetManager.h"
#include "../Core/CameraFPS.h"
#include "../Core/ShaderEffects/LitTexturedMeshEffect.h"
#include "../NetworkPlayer.h"
#include "ClientScene.h"
using namespace Core;

class ServerScene : public ClientScene
{
public:
	
	std::vector<networking::NetworkPlayer> connectedPlayers;
	networking::IConnection* connection;
	std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> lastTime;
	networking::FlowControl flowControl;

	ServerScene(Initialisation::WindowInfo windowInfo) : ClientScene(windowInfo)
	{

	}

	~ServerScene()
	{
	}

	void Initialise() override
	{
		SceneManager::Initialise();
		CaptureCursor(false);
		
		camera->SetPerspectiveProjection(45.0f, static_cast<float>(windowInfo.width), static_cast<float>(windowInfo.height), 1.0f, 100.0f);
		SetMainCamera(camera);
		camera->Translate(floorWidth * 0.5f, 3.0f, floorDepth * 1.3f);

		InitialiseEnvironment();
		InitialiseServerComms();
	}

	void InitialiseServerComms()
	{
		startTime = std::chrono::high_resolution_clock::now();
		lastTime = startTime;
		if (isUseReliableConnection)
			connection = networking::NetworkServices::GetInstance().CreateReliableConnection(ProtocolId, TimeOut);
		else
			connection = networking::NetworkServices::GetInstance().CreateConnection(ProtocolId, TimeOut);

		if (!connection->Start(ServerPort))
		{
			printf("could not start connection on port %d\n", ServerPort);
			return;
		}
		connection->Listen();
	}

	void OnCommsUpdate(float deltaTime) override
	{
		if (connection->IsConnected())
		{
			if (isUseReliableConnection)
				flowControl.Update(deltaTime, ((networking::ReliableConnection*)connection)->GetReliabilitySystem().GetRoundTripTime() * 1000.0f);

			unsigned char packet[] = "server to client";

			connection->SendPacket(packet, sizeof(packet));
		}

		//receive until theres no more packets left
		while (true)
		{
			unsigned char packet[256];
			int bytesRead = connection->ReceivePacket(packet, sizeof(packet));
			if (bytesRead == 0)
				break;
			printf("received packet from client\n");
		}

		connection->Update(deltaTime);
	}


	void notifyProcessNormalKeys(unsigned char key, int x, int y) override
	{
		//disable keys
	}

	virtual void OnMousePassiveMove(int posX, int posY, int deltaX, int deltaY) override
	{
		//disable mousemove
	}

	void ConnectPlayer()
	{
		
	}

	void DisconnectPlayer()
	{
		
	}
};

