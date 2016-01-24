#pragma once

#include "IObjectPool.h"
#include "../../Core/GameObject.h"
#include <map>
#include <stack>
#include "../../Core/AssetManager.h"
#include "../../Core/Networking/ClientNetworkManager.h"
#include "../../Core/Components/DirectionalMovementComponent.h"
#include "PlayerGameObject.h"

namespace MultiplayerArena
{
	class ObjectFactoryPool : public IObjectFactoryPool
	{
		struct ObjectPool
		{
			std::vector<std::shared_ptr<Core::GameObject>> activeObjects;
			std::stack<std::shared_ptr<Core::GameObject>> inactiveObjects;
		};

		const vec3 offscreenSpawnPoint = vec3(1000.0f, 1000.0f, 1000.0f);
		std::map <FactoryObjectType, std::shared_ptr<ObjectPool>>  objectPoolMap;
		std::vector<std::shared_ptr<Core::GameObject>>& gameObjectManager;

		std::shared_ptr<GameObject> CreatePlayerObject() const
		{
			std::shared_ptr<PlayerGameObject> player = std::make_shared<PlayerGameObject>();

			player->AddComponent(AssetManager::GetInstance().LoadMeshFromFile(std::string("Resources/Models/Dwarf/dwarf.x")));
			player->Translate(offscreenSpawnPoint);
			player->Scale(0.05f);
			gameObjectManager.push_back(player);

			std::shared_ptr<NetworkViewComponent> networkView = std::make_shared<NetworkViewComponent>(std::weak_ptr<Core::GameObject>(), networking::ClientNetworkManager::GetInstance());
			networkView->AddToNetworkingManager();
			player->AddComponent(networkView);

			std::shared_ptr<DirectionalMovementComponent> directionalMove = std::make_shared<DirectionalMovementComponent>(std::weak_ptr<Core::GameObject>());
			player->AddComponent(directionalMove);

			return player;
		}

		std::shared_ptr<Core::GameObject> CreateBulletObject() const
		{
			std::shared_ptr<Core::GameObject> bullet = std::make_shared<Core::GameObject>();

			return bullet;
		}
		std::mutex mutexObjectPoolMap;

	public:

		ObjectFactoryPool(std::vector<std::shared_ptr<GameObject>>& gameObjectManager) : gameObjectManager(gameObjectManager)
		{
		}

		~ObjectFactoryPool() override
		{
		}

		std::shared_ptr<GameObject> GetFactoryObject(FactoryObjectType objectType) override
		{
			std::lock_guard<std::mutex> lock(mutexObjectPoolMap);

			auto objectPool = objectPoolMap.find(objectType);
			if (objectPool != objectPoolMap.end())
			{
				assert(objectPool->second->inactiveObjects.size() > 0); //assert to ensure no on the fly Object construction
				auto gameObj = objectPool->second->inactiveObjects.top();
				objectPool->second->inactiveObjects.pop();
				objectPool->second->activeObjects.push_back(gameObj);

				return gameObj;
			}
			return nullptr;
		}

		void CreateFactoryObjects(FactoryObjectType objectType, unsigned int count) override
		{
			std::lock_guard<std::mutex> lock(mutexObjectPoolMap);

			if (objectPoolMap.find(objectType) == objectPoolMap.end())
				objectPoolMap[objectType] = std::make_shared<ObjectPool>();

			for (size_t i = 0; i < count; i++)
			{
				std::shared_ptr<Core::GameObject> obj;

				switch (objectType)
				{
				case Player:
					obj = CreatePlayerObject();
					break;
				case Bullet:
					obj = CreateBulletObject();
					break;
				}
				objectPoolMap[objectType]->inactiveObjects.push(obj);
			}
		}

		void ReleaseFactoryObject(FactoryObjectType objectType, std::shared_ptr<GameObject>& gameObject)
		{
			assert(objectPoolMap.find(objectType) == objectPoolMap.end());
			objectPoolMap[objectType]->activeObjects.erase(std::remove(objectPoolMap[objectType]->activeObjects.begin(), objectPoolMap[objectType]->activeObjects.end(), gameObject), objectPoolMap[objectType]->activeObjects.end());
		}
	};
}