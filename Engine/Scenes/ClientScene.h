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

using namespace Core;
namespace MultiplayerArena
{
	class ClientScene : public Managers::SceneManager
	{
	private:
		std::shared_ptr<GameObject> player;
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

		std::vector<std::shared_ptr<networking::NetworkPlayer>> otherPlayers;

		std::shared_ptr<ObjectFactoryPool> objectFactoryPool;

		const std::string defaultCheckeredTexture = "Default Checkered";
		ClientScene(Initialisation::WindowInfo windowInfo) : SceneManager(windowInfo)
		{
			camera = std::make_shared<CameraFPS>();
			objectFactoryPool = std::make_shared<ObjectFactoryPool>(gameObjectManager);
		}

		~ClientScene()
		{
		}

		void Initialise() override
		{
			SceneManager::Initialise();
			//CaptureCursor(true);

			camera->SetPerspectiveProjection(45.0f, static_cast<float>(windowInfo.width), static_cast<float>(windowInfo.height), 1.0f, 100.0f);
			SetMainCamera(camera);
			camera->Translate(floorWidth * 0.5f, 3.0f, floorDepth * 1.3f);

			objectFactoryPool->CreateFactoryObjects(IObjectFactoryPool::Player, GameOptions::MaxPlayers);

			InitialiseEnvironment();
			InitialiseLocalPlayer();

			networking::ClientNetworkManager::GetInstance().InitialiseConnectionToServer();
		}

		void InitialiseEnvironment()
		{
			InitialiseTextures();
			InitialiseLights();

			floor = std::make_shared<GameObject>();
			floor->AddComponent(AssetManager::GetInstance().CreateQuadPrimitiveMeshComponent(floorWidth, floorDepth, defaultCheckeredTexture));
			gameObjectManager.push_back(floor);
		}

		void InitialiseLocalPlayer()
		{
			vec3 spawnPosition(floorWidth / 2.0f, 0.0f, floorDepth / 2.0f);
			player = objectFactoryPool->GetFactoryObject(IObjectFactoryPool::Player);
			InitialisePlayer(player, spawnPosition, true);
		}

		void InitialisePlayer(std::shared_ptr<GameObject> &player, vec3 &position, bool isLocal)
		{
			player->GetWorldTransform()[3].x = position.x;
			player->GetWorldTransform()[3].y = position.y;
			player->GetWorldTransform()[3].z = position.z;

			if (isLocal)
			{
				auto component = player->GetComponentByType(Core::IComponent::NetworkView);
				if (component != nullptr)
				{
					std::shared_ptr<NetworkViewComponent> networkView = std::dynamic_pointer_cast<NetworkViewComponent>(component);
					networkView->SetIsSendUpdates(true);
				}
			}
		}

		void SpawnOpponentPlayer(vec3 position)
		{
			std::shared_ptr<GameObject> opponent = std::make_shared<GameObject>();
			InitialisePlayer(opponent, position, false);
			//otherPlayers.push_back(opponent);
		}


		void InitialiseTextures()
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

		void OnCommsUpdate(float deltaTime) override
		{
			networking::ClientNetworkManager::GetInstance().UpdateComms();
		}

		void notifyProcessNormalKeys(unsigned char key, int x, int y) override
		{
			camera->OnKey(key, x, y);
		}

		virtual void OnMousePassiveMove(int posX, int posY, int deltaX, int deltaY) override
		{
			camera->OnMouseMove(deltaX, deltaY);
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
	};
}