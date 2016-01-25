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
		float movementSpeed = 50.0f;
		float angularVelocity = 5.0f;
	public:

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

		void OnKey(unsigned char key, int x, int y)
		{
			if (directionalMovement.expired())
				return;

			if (key == 'a')
				directionalMovement.lock()->MoveRight(-movementSpeed);
			if (key == 'd')
				directionalMovement.lock()->MoveRight(movementSpeed);
			if (key == 'w')
				directionalMovement.lock()->MoveForward(movementSpeed);
			if (key == 's')
				directionalMovement.lock()->MoveForward(-movementSpeed);
			if (key == 'z')
				directionalMovement.lock()->MoveUp(movementSpeed);
			if (key == 'c')
				directionalMovement.lock()->MoveUp(-movementSpeed);
			if (key == 'q')
				directionalMovement.lock()->Rotate(angularVelocity, 0.0f);
			if (key == 'e')
				directionalMovement.lock()->Rotate(-angularVelocity, 0.0f);
		}
		
	};
}
