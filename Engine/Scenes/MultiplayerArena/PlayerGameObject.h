#pragma once
#include "../../Core/GameObject.h"
#include "../../Core/Components/DirectionalMovementComponent.h"
#include "NetworkViews/PlayerNetworkViewComponent.h"

namespace MultiplayerArena
{
	class PlayerGameObject : public Core::GameObject
	{
		std::weak_ptr<DirectionalMovementComponent> directionalMovement;
		std::weak_ptr<PlayerNetworkViewComponent> networkView;
		float movementSpeed = 2000.0f;
		float angularVelocity = 50.0f;
		
	public:
		bool isLocal = true;
		void AddComponent(std::shared_ptr<Core::IComponent> component) override
		{
			GameObject::AddComponent(component);

			//cache these for performance
			if (component->GetComponentType() == Core::IComponent::DirectionalMovement)
				directionalMovement = std::dynamic_pointer_cast<DirectionalMovementComponent>(component);

			if (component->GetComponentType() == Core::IComponent::NetworkView)
				networkView = std::dynamic_pointer_cast<PlayerNetworkViewComponent>(component);
		}

		PlayerGameObject()
		{
		}

		virtual ~PlayerGameObject()
		{
		}

		std::shared_ptr<PlayerNetworkViewComponent> GetNetworkView()
		{
			auto component = GetComponentByType(Core::IComponent::NetworkView);
			if (component != nullptr)
				return std::dynamic_pointer_cast<PlayerNetworkViewComponent>(component);
			return nullptr;
		}

		std::shared_ptr<DirectionalMovementComponent> GetDirectionalMove()
		{
			auto component = GetComponentByType(Core::IComponent::DirectionalMovement);
			if (component != nullptr)
				return std::dynamic_pointer_cast<DirectionalMovementComponent>(component);
			return nullptr;
		}

		void HandleKeyMovement(float deltaTime)
		{
			if (directionalMovement.expired())
				return;

			if (Managers::SceneManager::keyState['a'])
				directionalMovement.lock()->MoveRight(movementSpeed * deltaTime);
			if (Managers::SceneManager::keyState['d'])
				directionalMovement.lock()->MoveRight(-movementSpeed * deltaTime);
			if (Managers::SceneManager::keyState['w'])
				directionalMovement.lock()->MoveForward(movementSpeed * deltaTime);
			if (Managers::SceneManager::keyState['s'])
				directionalMovement.lock()->MoveForward(-movementSpeed * deltaTime);
			if (Managers::SceneManager::keyState['z'])
				directionalMovement.lock()->MoveUp(movementSpeed * deltaTime);
			if (Managers::SceneManager::keyState['c'])
				directionalMovement.lock()->MoveUp(-movementSpeed * deltaTime);
			if (Managers::SceneManager::keyState['q'])
				directionalMovement.lock()->Rotate(angularVelocity * deltaTime, 0.0f);
			if (Managers::SceneManager::keyState['e'])
				directionalMovement.lock()->Rotate(-angularVelocity * deltaTime, 0.0f);
		}

		void Update(float deltaTime) override
		{
			if(isLocal)
				HandleKeyMovement(deltaTime);

			GameObject::Update(deltaTime);
		}
		
	};
}
