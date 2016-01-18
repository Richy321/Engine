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
		timer->AddOnTickCallback(std::bind(&ServerScene::OnCommsUpdate, this));
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

	void OnCommsUpdate()
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

				value.second->relatedGameObject->GetWorldTransform()[3].x = message->positionMessage.position.x;
				value.second->relatedGameObject->GetWorldTransform()[3].y = message->positionMessage.position.y;
				value.second->relatedGameObject->GetWorldTransform()[3].z = message->positionMessage.position.z;
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
			memcpy(message.get(), packet, bytesRead);

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
			ConnectPlayer(message->uniqueID);
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


	std::shared_ptr<GameObject> SpawnPlayer(vec3 position)
	{
		std::shared_ptr<GameObject> player = std::make_shared<GameObject>();
		InitialisePlayer(player, position);
		return player;
	}

	void notifyProcessNormalKeys(unsigned char key, int x, int y) override
	{
		//disable keys
	}

	virtual void OnMousePassiveMove(int posX, int posY, int deltaX, int deltaY) override
	{
		//disable mousemove
	}

	void ConnectPlayer(GUID id)
	{
		connectedPlayerMap[id] = std::make_shared<networking::NetworkPlayer>();
		connectedPlayerMap[id]->relatedGameObject = SpawnPlayer(vec3(0, 0, 0));
	}

	void DisconnectPlayer(GUID id)
	{
		//todo delete objects on disconnect
	}
};

