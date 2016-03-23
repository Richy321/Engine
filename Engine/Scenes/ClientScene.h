#pragma once
#include "../Managers/SceneManager.h"
#include "../Core/Components/MeshComponent.h"
#include "../Core/Camera.h"
#include "../Core/CameraFPS.h"
#include "../Core/AssetManager.h"
#include "../Core/CameraFPS.h"
#include "../Core/ShaderEffects/LitTexturedMeshEffect.h"
#include "../Core/Components/NetworkViewComponent.h"
#include "../Core/Networking/ClientNetworkManager.h"

#include "../Scenes/MultiplayerArena/ObjectPool.h"
#include "MultiplayerArena/GameOptions.h"

#include <memory>
#include "../Core/Networking/ServerNetworkingManager.h"

using namespace Core;
namespace MultiplayerArena
{
	class ClientScene : public Managers::SceneManager
	{
	private:
		std::shared_ptr<PlayerGameObject> player;
	public:

		std::shared_ptr<CameraFPS> camera;
		GLuint gViewUniform;
		GLuint gProjectionUniform;
		GLuint gWP;

		
		std::shared_ptr<GameObject> floor;

		std::shared_ptr<DirectionalLight> directionalLight;
		std::vector<std::shared_ptr<SpotLight>> spotLights;
		std::vector<std::shared_ptr<PointLight>> pointLights;

		const float floorWidth = 30.0f;
		const float floorDepth = 30.0f;
		float m_scale = 0.0f;

		std::shared_ptr<ObjectFactoryPool> objectFactoryPool;

		typedef std::map<GUID, std::shared_ptr<networking::NetworkPlayer>, Utils::GUIDComparer> PlayerConnectionMap;
		PlayerConnectionMap connectedPlayerMap;
		std::mutex mutexConnectedPlayerMap;

		 
		typedef std::map<GUID, networking::MessageStructures::MessageType, Utils::GUIDComparer> NetworkingIdToMessageTypeType;
		NetworkingIdToMessageTypeType networkIDToType;


		const std::string defaultCheckeredTexture = "Default Checkered";
		ClientScene(Initialisation::WindowInfo windowInfo) : SceneManager(windowInfo)
		{
			camera = std::make_shared<CameraFPS>();
		}

		virtual ~ClientScene()
		{
			DisconnectComms();
		}

		void Initialise() override
		{
			SceneManager::Initialise();
			//CaptureCursor(true);

			objectFactoryPool = std::make_shared<ObjectFactoryPool>(gameObjectManager, std::dynamic_pointer_cast<networking::INetworkManager>(networking::ClientNetworkManager::GetInstance()));

			camera->SetPerspectiveProjection(45.0f, static_cast<float>(windowInfo.width), static_cast<float>(windowInfo.height), 1.0f, 100.0f);
			SetMainCamera(camera);
			camera->Translate(floorWidth * 0.5f, 3.0f, floorDepth * 1.3f);

			objectFactoryPool->CreateFactoryObjects(IObjectFactoryPool::Player, GameOptions::MaxPlayers);

			InitialiseEnvironment();
			InitialiseLocalPlayer();

			//ConnectComms();
		}

		void ConnectComms()
		{
			if (!networking::ClientNetworkManager::GetInstance()->IsConnected())
			{
				ClientScene* nonCostThis = const_cast<ClientScene*>(this);
				networking::ClientNetworkManager::GetInstance()->ConnectToServer();
				networking::ClientNetworkManager::GetInstance()->SetOnNetworkViewConnectCallback(std::bind(&ClientScene::OnNetworkViewConnect, nonCostThis, std::placeholders::_1));
				networking::ClientNetworkManager::GetInstance()->SetOnNetworkViewDisconnectCallback(std::bind(&ClientScene::OnNetworkViewDisconnect, nonCostThis, std::placeholders::_1));
			}
		}

		void DisconnectComms()
		{
			if (networking::ClientNetworkManager::GetInstance()->IsConnected())
			{
				if (player != nullptr)
					networking::ClientNetworkManager::GetInstance()->DisconnectFromServer(player->GetNetworkView()->GetUniqueID());
			}
		}

		void InitialiseEnvironment()
		{
			InitialiseTextures();
			InitialiseLights();

			floor = std::make_shared<GameObject>();
			floor->AddComponent(AssetManager::GetInstance().CreateQuadPrimitiveMeshComponent(floorWidth, floorDepth, defaultCheckeredTexture));
			gameObjectManager.push_back(floor);
		}

		std::shared_ptr<NetworkViewComponent> InitialiseLocalPlayer()
		{
			vec3 spawnPosition(floorWidth / 2.0f, 0.0f, floorDepth / 2.0f);
			player = std::dynamic_pointer_cast<PlayerGameObject>(objectFactoryPool->GetFactoryObject(IObjectFactoryPool::Player));
			return InitialisePlayer(player, spawnPosition, GUID_NULL, true);
		}

		std::shared_ptr<NetworkViewComponent> InitialisePlayer(std::shared_ptr<PlayerGameObject> &player, vec3 &position, GUID uniqueID, bool isLocal)
		{
			player->isLocal = isLocal;
			player->GetWorldTransform()[3].x = position.x;
			player->GetWorldTransform()[3].y = position.y;
			player->GetWorldTransform()[3].z = position.z;

			auto component = player->GetComponentByType(Core::IComponent::NetworkView);
			if (component != nullptr)
			{
				std::shared_ptr<NetworkViewComponent> networkView = std::dynamic_pointer_cast<NetworkViewComponent>(component);
					
				if (isLocal)
				{
					networkView->IsSendUpdates() = true;
					networkView->deadReckoning = NetworkViewComponent::None;
				}
				else
				{
					networkView->IsSendUpdates() = false;
					networkView->deadReckoning = NetworkViewComponent::DeadReckoningType::None; //linear
					networkView->SetUniqueID(uniqueID);
				}

				networkView->AddToNetworkingManager();

				return networkView;
			}
			return nullptr;
		}

		void InitialiseTextures() const
		{
			AssetManager::GetInstance().LoadTextureFromFile("Resources/checkered.jpg", defaultCheckeredTexture, GL_BGRA, GL_RGBA, 0, 0);
		}

		void InitialiseLights()
		{
			directionalLight = std::make_shared<DirectionalLight>();
			directionalLight->Color = vec3(1.0f, 1.0f, 1.0f);
			directionalLight->AmbientIntensity = 0.01f;
			directionalLight->DiffuseIntensity = 0.2f;
			directionalLight->Direction = vec3(0.0f, 0.0, -1.0);

			pointLights.push_back(std::make_shared<PointLight>());
			pointLights[0]->DiffuseIntensity = 0.75f;
			pointLights[0]->Color = vec3(1.0f, 0.5f, 0.0f);
			pointLights[0]->Position = vec3(3.0f, 1.0f, 5.0f);
			pointLights[0]->Attenuation.Linear = 0.1f;

			pointLights.push_back(std::make_shared<PointLight>());
			pointLights[1]->DiffuseIntensity = 0.75f;
			pointLights[1]->Color = vec3(0.0f, 0.5f, 1.0f);
			pointLights[0]->Position = vec3(7.0f, 1.0f, 1.0f);
			pointLights[1]->Attenuation.Linear = 0.1f;

			spotLights.push_back(std::make_shared<SpotLight>());
			spotLights[0]->DiffuseIntensity = 0.9f;
			spotLights[0]->Color = vec3(1.0f, 0.0f, 0.0f);
			spotLights[0]->Position = vec3(floorWidth * 0.5f, 5.0f, floorDepth * 0.5f);
			spotLights[0]->Direction = vec3(0.0, -1.0f, 0.0f);
			spotLights[0]->Attenuation.Linear = 0.1f;
			spotLights[0]->Cutoff = 20.0f;
		}

		void OnUpdate(float deltaTime) override
		{
			m_scale += 0.0057f;
			camera->Update(deltaTime);
			pointLights[0]->Position = vec3(floorWidth * 0.25f, 1.0f, floorDepth * (cosf(m_scale) + 1.0f) / 2.0f);
			pointLights[1]->Position = vec3(floorWidth * 0.75f, 1.0f, floorDepth * (sinf(m_scale) + 1.0f) / 2.0f);
		}

		void OnCommsUpdate(float deltaTime) const 
		{
			//Not used unless threaded network comms is turned off
			networking::ClientNetworkManager::GetInstance()->UpdateComms();
		}

		void notifyProcessNormalKeys(unsigned char key, int x, int y) override
		{
			SceneManager::notifyProcessNormalKeys(key, x, y);
			camera->OnKey(key, x, y);

			if (key == '=')
				ConnectComms();
			if (key == '-')
				DisconnectComms();
		}

		virtual void OnMousePassiveMove(int posX, int posY, int deltaX, int deltaY) override
		{
			//camera->OnMouseMove(deltaX, deltaY);
		}

		virtual void notifyDisplayFrame() override
		{
			Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetViewMatrix(camera->view);
			Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetProjectionMatrix(camera->projection);

			vec3 cameraPos = vec3(camera->GetWorldTransform()[3]);
			Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetEyeWorldPos(cameraPos);
			Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetMatSpecularIntensity(1.0f);
			Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetMatSpecularPower(32.0f);

			Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetDirectionalLight(directionalLight);
			Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetPointLights(pointLights);
			Managers::ShaderManager::GetInstance().litTexturedMeshEffect->SetSpotLights(spotLights);

			SceneManager::notifyDisplayFrame();
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

		virtual std::shared_ptr<INetworkViewComponent> OnNetworkViewConnect(std::shared_ptr<networking::MessageStructures::BaseMessage> message)
		{
			std::shared_ptr<INetworkViewComponent> netView = nullptr;
			networkIDToType[message->uniqueID] = message->messageType;
			switch (message->messageType)
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
			return netView;
		}

		virtual void OnNetworkViewDisconnect(GUID id)
		{
			NetworkingIdToMessageTypeType::iterator it = networkIDToType.find(id);
			if (it != networkIDToType.end())
			{
				switch (it->second)
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

				networkIDToType.erase(it);
			}
		}
	};
}