#pragma once
#include "../../../Core/Components/NetworkViewComponent.h"
#include "../../../Core/IGameObject.h"

namespace MultiplayerArena
{
	class PlayerNetworkViewComponent : public NetworkViewComponent
	{
	public:

		PlayerNetworkViewComponent(std::weak_ptr<Core::IGameObject> parent, std::shared_ptr<networking::INetworkManager> networkingManager) : IComponent(parent), NetworkViewComponent(parent, networkingManager)
		{
		}

		~PlayerNetworkViewComponent()
		{
		}
		
		
		void ProcessMessages() override
		{
			std::shared_ptr<networking::MessageStructures::BaseMessage> lastState = nullptr;
			std::lock_guard<std::mutex> lock(mutexReceivedMsg);
			for (auto &i : receivedMessages)
			{
				if (i->simpleType == networking::MessageStructures::Disconnect)
					isFlaggedForDeletion = true;
			}

			//todo - handle corrections from server
			if (deadReckoning == None)
			{
				for (auto &i : receivedMessages)
				{
					//not sending updates means it's controlled by this client (don't update from server...) )
					if (i->simpleType == networking::MessageStructures::SnapShot && !this->IsSendUpdates())
					{
						lastState = i;
					}
				}

				if (lastState != nullptr)
				{
					parentGameObject.lock()->GetWorldTransform()[3].x = lastState->positionOrientationMessage.position.x;
					parentGameObject.lock()->GetWorldTransform()[3].y = lastState->positionOrientationMessage.position.y;
					parentGameObject.lock()->GetWorldTransform()[3].z = lastState->positionOrientationMessage.position.z;

					std::shared_ptr<IComponent> component = parentGameObject.lock()->GetComponentByType(DirectionalMovement);
					if (component != nullptr)
					{
						std::shared_ptr<DirectionalMovementComponent> dirMove = std::dynamic_pointer_cast<Core::DirectionalMovementComponent>(component);
						dirMove->SetOrientation(lastState->positionOrientationMessage.heading, lastState->positionOrientationMessage.pitch);
					}
				}
			}
		}

		//max 256
		int BuildPacket(std::shared_ptr<networking::MessageStructures::BaseMessage>& message) override
		{
			mat4 &transform = GetParentGameObject().lock()->GetWorldTransform();
			
			message->uniqueID = GetUniqueID();
			message->simpleType = networking::MessageStructures::SnapShot;
			message->messageType = networking::MessageStructures::Player;
			message->positionOrientationMessage.position = vec3(transform[3].x, transform[3].y, transform[3].z);

			std::shared_ptr<IComponent> component = parentGameObject.lock()->GetComponentByType(DirectionalMovement);
			if (component != nullptr)
			{
				std::shared_ptr<Core::DirectionalMovementComponent> dirMove = std::dynamic_pointer_cast<DirectionalMovementComponent>(component);
				message->positionOrientationMessage.heading = dirMove->GetHeadingAngle();
				message->positionOrientationMessage.pitch = dirMove->GetPitchAngle();
			}
			else
			{
				message->positionOrientationMessage.heading = 0.0f;
				message->positionOrientationMessage.pitch = 0.0f;
			}
			
			return sizeof(networking::MessageStructures::BaseMessage);
		}

		bool IsPrimaryPlayerView() override
		{
			return true;
		}
	};
}
