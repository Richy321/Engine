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
#include "../Core/Networking/ServerNetworkingManager.h"


using namespace Core;
namespace MultiplayerArena
{
	class ServerScene : public ClientScene
	{
	public:
		networking::IConnection* connection;
		std::chrono::time_point<std::chrono::system_clock> startTime;
		std::chrono::time_point<std::chrono::system_clock> lastTime;
		networking::FlowControl flowControl;

		

		ServerScene(Initialisation::WindowInfo windowInfo) : ClientScene(windowInfo)
		{
		}

		~ServerScene() override 
		{
		}

		void Initialise() override
		{
			SceneManager::Initialise();
			CaptureCursor(false);

			objectFactoryPool = std::make_shared<ObjectFactoryPool>(gameObjectManager, std::dynamic_pointer_cast<networking::INetworkManager>(networking::ServerNetworkingManager::GetInstance()));

			camera->SetPerspectiveProjection(45.0f, static_cast<float>(windowInfo.width), static_cast<float>(windowInfo.height), 1.0f, 100.0f);
			SetMainCamera(camera);
			camera->Translate(floorWidth * 0.5f, 3.0f, floorDepth * 1.3f);

			objectFactoryPool->CreateFactoryObjects(IObjectFactoryPool::Player, GameOptions::MaxPlayers);

			InitialiseEnvironment();
			
			InitialiseServerComms();
		}

		void InitialiseServerComms() const
		{
			ServerScene* nonCostThis = const_cast<ServerScene*>(this);

			networking::ServerNetworkingManager::GetInstance()->InitialiseServerComms();
			networking::ServerNetworkingManager::GetInstance()->SetOnNetworkViewConnectCallback(std::bind(&ServerScene::OnNetworkViewConnect, nonCostThis, std::placeholders::_1, std::placeholders::_2));
			networking::ServerNetworkingManager::GetInstance()->SetOnNetworkViewDisconnectCallback(std::bind(&ServerScene::OnNetworkViewDisconnect, nonCostThis, std::placeholders::_1, std::placeholders::_2));
			networking::ServerNetworkingManager::GetInstance()->SetDoMessageProcessingCallback(std::bind(&ServerScene::DoMessageProcessing, nonCostThis));
		}

		void OnCommsUpdate(float deltaTime) const
		{
			//Not used unless threaded network comms is turned off
			networking::ServerNetworkingManager::GetInstance()->UpdateComms();
		}

		void notifyProcessNormalKeys(unsigned char key, int x, int y) override
		{
			//disable keys inherited from client
		}

		virtual void OnMousePassiveMove(int posX, int posY, int deltaX, int deltaY) override
		{ 
			//disable mousemove inherited from client
		}

		std::shared_ptr<INetworkViewComponent> ConnectPlayer(GUID id, vec3 position)
		{
			std::lock_guard<std::mutex> lock(mutexGameObjectManager);

			std::shared_ptr<PlayerGameObject> player = std::dynamic_pointer_cast<PlayerGameObject>(objectFactoryPool->GetFactoryObject(IObjectFactoryPool::Player));
			std::shared_ptr<INetworkViewComponent> netView = InitialisePlayer(player, position, id, false);
			connectedPlayerMap[id] = std::make_shared<networking::NetworkPlayer>();
			connectedPlayerMap[id]->relatedGameObject = player;

			OLECHAR szGuid[40] = { 0 };
			StringFromGUID2(id, szGuid, 40);

			printf("Connected new player %ls at %f %f %f \n", szGuid, position.x, position.y, position.z);

			return netView;
		}

		void DisconnectPlayer(GUID id)
		{
			std::lock_guard<std::mutex> lock(mutexGameObjectManager);
			std::lock_guard<std::mutex> connectedMapLock(mutexConnectedPlayerMap);

			
			gameObjectManager.erase(std::remove(gameObjectManager.begin(), gameObjectManager.end(), connectedPlayerMap[id]->relatedGameObject), gameObjectManager.end());
			connectedPlayerMap.erase(id);
			
			OLECHAR szGuid[40] = { 0 };
			StringFromGUID2(id, szGuid, 40);
			printf("Disconnected player at %ls \n", szGuid);
		}

		std::shared_ptr<INetworkViewComponent> ConnectBullet(GUID id, vec3 position)
		{
			return nullptr;
		}

		void DisconnectBullet(GUID id)
		{
			
		}


		void ConnectCollectable(GUID id, vec3 position)
		{
			
		}

		void DisconnectCollectable(GUID id)
		{
			
		}

		void OnNetworkViewConnect(std::shared_ptr<networking::MessageStructures::BaseMessage> message, std::shared_ptr<INetworkViewComponent> netView)
		{
			switch(message->messageType)
			{
			case networking::MessageStructures::None:
				break;
			case networking::MessageStructures::Player:
				netView = ConnectPlayer(message->uniqueID, message->positionOrientationMessage.position);
				break;
			case networking::MessageStructures::Bullet:
				netView = ConnectBullet(message->uniqueID, message->positionOrientationMessage.position);
				break;
			case networking::MessageStructures::Collectable:
				break;
			}

		}

		void OnNetworkViewDisconnect(GUID id, networking::MessageStructures::MessageType msgType)
		{
			switch (msgType)
			{
			case networking::MessageStructures::None:
				break;
			case networking::MessageStructures::Player:
				DisconnectPlayer(id);
				break;
			case networking::MessageStructures::Bullet:
				DisconnectBullet(id);
				break;
			case networking::MessageStructures::Collectable:
				DisconnectCollectable(id);
				break;
			}
		}

		void DoMessageProcessing()
		{
			
		}

	};
}
