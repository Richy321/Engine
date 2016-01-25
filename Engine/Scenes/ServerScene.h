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
#include <mutex>

using namespace Core;
namespace MultiplayerArena
{
	class ServerScene : public ClientScene, public IConnectionEventHandler
	{
	public:
		typedef std::map<GUID, std::shared_ptr<networking::NetworkPlayer>, Utils::GUIDComparer> PlayerConnectionMap;
		PlayerConnectionMap connectedPlayerMap;
		
		networking::IConnection* connection;
		std::chrono::time_point<std::chrono::system_clock> startTime;
		std::chrono::time_point<std::chrono::system_clock> lastTime;
		networking::FlowControl flowControl;
		std::unique_ptr<TickTimer> timer;

		std::mutex mutexConnectedPlayerMap;

		GUID lastConnectedPlayer;
		//std::function<void(int, const ServerScene*)> commsCallWrapper = [](const ServerScene* thisRef, float delta) { if (thisRef != nullptr) { thisRef->OnCommsUpdate(delta); }};

		ServerScene(Initialisation::WindowInfo windowInfo) : ClientScene(windowInfo), timer(std::make_unique<TickTimer>())
		{
			timer->SetTickIntervalMilliseconds(std::chrono::milliseconds(15));
			timer->AddOnTickCallback(std::bind(&ServerScene::OnCommsUpdate, this, 0.0f));
			//timer->AddOnTickCallback(std::bind(commsCallWrapper, this, 0.0f));
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

			objectFactoryPool->CreateFactoryObjects(IObjectFactoryPool::Player, GameOptions::MaxPlayers);

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
			std::shared_ptr<SceneManager> ptrToThis = shared_from_this();
			std::shared_ptr<IConnectionEventHandler> connectionEventHandler = std::dynamic_pointer_cast<IConnectionEventHandler>(ptrToThis);
			connection->SetConnectionEventHandler(connectionEventHandler);
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
			{
				std::lock_guard<std::mutex> lockConnectedPlayerMap(mutexConnectedPlayerMap);
				//update server visualisations
				for (auto const& value : connectedPlayerMap)
				{
					if (value.second->messages.size() > 0)
					{
						//just use last message received for the moment
						std::shared_ptr<networking::MessageStructures::BaseMessage> message = value.second->messages[value.second->messages.size() - 1];

						std::shared_ptr<Core::IComponent> component = value.second->relatedGameObject->GetComponentByType(Core::IComponent::NetworkView);
						if (component != nullptr)
						{
							std::shared_ptr<INetworkViewComponent> netView = std::dynamic_pointer_cast<INetworkViewComponent>(component);
							//todo - convert to std::shared_ptr<>
							netView->ReadPacket(*message.get());
						}
					}
				}
			}

			//todo - collision detection confirmation...

			//todo - possible physics simulation
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
			std::lock_guard<std::mutex> lockConnectedPlayerMap(mutexConnectedPlayerMap);
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
			mutexConnectedPlayerMap.lock();
			if (message->messageType == networking::MessageStructures::PlayerConnect || connectedPlayerMap.find(message->uniqueID) == connectedPlayerMap.end())
			{
				vec3 pos;
				if (message->messageType == networking::MessageStructures::BasicPosition)
					pos = message->positionMessage.position;
				else
					pos = vec3(floorWidth * 0.35f, 5.0f, floorDepth * 0.35f);


				vec3 spawnPosition(floorWidth / 2.0f, 0.0f, floorDepth / 2.0f);
				ConnectPlayer(message->uniqueID, spawnPosition);
			}
			mutexConnectedPlayerMap.unlock();

			if (message->messageType == networking::MessageStructures::PlayerDisconnect)
			{
				DisconnectPlayer(message->uniqueID);
			}
			else
			{
				mutexConnectedPlayerMap.lock();
				connectedPlayerMap[message->uniqueID]->messages.push_back(message);
				mutexConnectedPlayerMap.unlock();
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
			std::lock_guard<std::mutex> lock(mutexGameObjectManager);

			std::shared_ptr<PlayerGameObject> player = std::dynamic_pointer_cast<PlayerGameObject>(objectFactoryPool->GetFactoryObject(IObjectFactoryPool::Player));
			InitialisePlayer(player, position, false);

			connectedPlayerMap[id] = std::make_shared<networking::NetworkPlayer>();
			connectedPlayerMap[id]->relatedGameObject = player;

			printf("Connected new player at %f %f %f \n", position.x, position.y, position.z);
			lastConnectedPlayer = id;
		}

		void DisconnectPlayer(GUID id)
		{
			gameObjectManager.erase(std::remove(gameObjectManager.begin(), gameObjectManager.end(), connectedPlayerMap[id]->relatedGameObject), gameObjectManager.end());
			connectedPlayerMap.erase(id);
		}

		void OnStart() override
		{
		}

		void OnStop() override
		{
		}

		void OnConnect() override
		{	
		}

		void OnDisconnect() override
		{
			std::lock_guard<std::mutex> lockGameObjectManager(mutexGameObjectManager);
			std::lock_guard<std::mutex> lockConnectedPlayerMap(mutexConnectedPlayerMap);

			DisconnectPlayer(lastConnectedPlayer);
		}
	};
}
