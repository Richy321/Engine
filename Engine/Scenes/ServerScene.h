#pragma once
#include "../Managers/SceneManager.h"
#include "../Core/Components/MeshComponent.h"
#include "../Core/Camera.h"
#include "../Core/CameraFPS.h"
#include "../Core/AssetManager.h"
#include "../Core/CameraFPS.h"
#include "../Core/ShaderEffects/LitTexturedMeshEffect.h"
#include "../Core/Networking/NetworkPlayer.h"
#include "ClientScene.h"

using namespace Core;

class ServerScene : public ClientScene
{
public:
	std::map<GUID, std::shared_ptr<networking::NetworkPlayer>, Utils::GUIDComparer> connectedPlayerMap;
	networking::IConnection* connection;
	std::chrono::time_point<std::chrono::system_clock> startTime;
	std::chrono::time_point<std::chrono::system_clock> lastTime;
	networking::FlowControl flowControl;
	std::unique_ptr<TickTimer> timer;
	
	ServerScene(Initialisation::WindowInfo windowInfo) : ClientScene(windowInfo), timer(std::make_unique<TickTimer>())
	{
		timer->SetTickIntervalMilliseconds(std::chrono::milliseconds(15));
		//timer->AddOnTickCallback(std::bind(&ServerScene::OnCommsUpdate, this));
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
		startTime = std::chrono::system_clock::now();
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

		timer->Start();
	}

	void OnCommsUpdate(float delta) override
	{
		std::chrono::time_point<std::chrono::system_clock> nowTime = std::chrono::system_clock::now();
		std::chrono::milliseconds deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - lastTime);
		float deltaTimeSecs = deltaTime.count() * 0.001f;
		std::chrono::milliseconds fromStartTime = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - startTime);

		//printf("Comms update: %f \n", fromStartTime.count() * 0.001f);
		
		//if (connection->IsConnected())
		ReceiveAndStorePackets();

		RunSimulation();

		//if (connection->IsConnected())
		SendUpdatedSnapshots();

		connection->Update(deltaTimeSecs);
		if (isUseReliableConnection)
			flowControl.Update(deltaTimeSecs, static_cast<networking::ReliableConnection*>(connection)->GetReliabilitySystem().GetRoundTripTime() * 1000.0f);

		lastTime = nowTime;
	}


	void RunSimulation()
	{
		//update server visualisations
		for(auto const& value : connectedPlayerMap)
		{
			if (value.second->messages.size() > 0)
			{
				//just use last message received for the moment
				std::shared_ptr<networking::MessageStructures::BaseMessage> message = value.second->messages[value.second->messages.size() - 1];
				const float delta_x = 0.1;

				value.second->relatedGameObject->GetWorldTransform()[3].x = message->positionMessage.position.x;
				value.second->relatedGameObject->GetWorldTransform()[3].y = message->positionMessage.position.y;
				value.second->relatedGameObject->GetWorldTransform()[3].z = message->positionMessage.position.z;

				//value.second->relatedGameObject->GetWorldTransform()[3].x += 0.1f;
			}
		}

		//TODO collision detection confirmation...

		//TODO possible physics simulation
	}

	void ReceiveAndStorePackets()
	{
		//receive until theres no more packets left
		while (true)
		{
			unsigned char packet[256];
			int bytesRead = connection->ReceivePacket(packet, sizeof(packet));
			if (bytesRead == 0)
				break;
			std::shared_ptr<networking::MessageStructures::BaseMessage> message = std::make_shared<networking::MessageStructures::BaseMessage>();
			memcpy(message.get(), packet, sizeof(networking::MessageStructures::BaseMessage));

			ReadPacket(message);
		}
	}

	void SendUpdatedSnapshots()
	{
		for (auto const& value : connectedPlayerMap)
		{
			networking::MessageStructures::BaseMessage message;
			message.messageType = networking::MessageStructures::BasicPosition;
			message.uniqueID = value.first;
			message.positionMessage.position = value.second->relatedGameObject->GetPosition();

			connection->SendPacket(reinterpret_cast<unsigned char*>(&message), sizeof(networking::MessageStructures::BaseMessage));
		}
	}

	void ReadPacket(std::shared_ptr<networking::MessageStructures::BaseMessage> message)
	{
		//Add new player if not exists
		if(message->messageType == networking::MessageStructures::PlayerConnect || connectedPlayerMap.find(message->uniqueID) == connectedPlayerMap.end())
		{
			vec3 pos;
			if (message->messageType == networking::MessageStructures::BasicPosition)
				pos = message->positionMessage.position;
			else
				pos = vec3(floorWidth * 0.35f, 5.0f, floorDepth * 0.35f);


			vec3 spawnPosition(floorWidth / 2.0f, 0.0f, floorDepth / 2.0f);
			ConnectPlayer(message->uniqueID, spawnPosition);
		}
		else if(message->messageType == networking::MessageStructures::PlayerDisconnect)
		{
			DisconnectPlayer(message->uniqueID);
		}
		else
		{
			connectedPlayerMap[message->uniqueID]->messages.push_back(message);
		}
	}



	void notifyProcessNormalKeys(unsigned char key, int x, int y) override
	{
		//disable keys
	}

	virtual void OnMousePassiveMove(int posX, int posY, int deltaX, int deltaY) override
	{
		//disable mousemove
	}

	void ConnectPlayer(GUID id, vec3 position)
	{
		std::shared_ptr<GameObject> newPlayer = std::make_shared<GameObject>();
		connectedPlayerMap[id] = std::make_shared<networking::NetworkPlayer>();
		connectedPlayerMap[id]->relatedGameObject = newPlayer;
		InitialisePlayer(newPlayer, position);
		gameObjectManager.push_back(newPlayer);
		printf("Connected new player at %f %f %f \n", position.x, position.y, position.z);
	}

	void DisconnectPlayer(GUID id)
	{
		//todo delete objects on disconnect
	}
};

