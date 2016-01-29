#pragma once
#include "../../../Core/Components/NetworkViewComponent.h"
#include "../../../Core/IGameObject.h"

namespace MultiplayerArena
{
	class PlayerNetworkViewComponent : public NetworkViewComponent
	{
	public:

		PlayerNetworkViewComponent(std::weak_ptr<Core::IGameObject> parent, networking::IClientNetworkManager& networkingManager) : NetworkViewComponent(parent, networkingManager)
		{
		}

		~PlayerNetworkViewComponent()
		{
		}

		//max 256
		int BuildPacket(networking::MessageStructures::BaseMessage& message) override
		{
			mat4 &transform = GetParentGameObject().lock()->GetWorldTransform();
			
			message.uniqueID = GetUniqueID();
			message.messageType = networking::MessageStructures::PlayerSnapshot;
			message.positionOrientationMessage.position = vec3(transform[3].x, transform[3].y, transform[3].z);

			std::shared_ptr<IComponent> component = parentGameObject.lock()->GetComponentByType(DirectionalMovement);
			if (component != nullptr)
			{
				std::shared_ptr<Core::DirectionalMovementComponent> dirMove = std::dynamic_pointer_cast<Core::DirectionalMovementComponent>(component);
				message.positionOrientationMessage.heading = dirMove->GetHeadingAngle();
				message.positionOrientationMessage.pitch = dirMove->GetPitchAngle();
			}
			else
			{
				message.positionOrientationMessage.heading = 0.0f;
				message.positionOrientationMessage.pitch = 0.0f;
			}
			
			return sizeof(networking::MessageStructures::BaseMessage);
		}

		void ReadPacket(networking::MessageStructures::BaseMessage& packet) override
		{
			if (packet.messageType == networking::MessageStructures::PlayerSnapshot)
			{
				if (deadReckoning == Exact)
				{
					parentGameObject.lock()->GetWorldTransform()[3].x = packet.positionOrientationMessage.position.x;
					parentGameObject.lock()->GetWorldTransform()[3].y = packet.positionOrientationMessage.position.y;
					parentGameObject.lock()->GetWorldTransform()[3].z = packet.positionOrientationMessage.position.z;

					std::shared_ptr<IComponent> component = parentGameObject.lock()->GetComponentByType(DirectionalMovement);
					if (component != nullptr)
					{
						std::shared_ptr<Core::DirectionalMovementComponent> dirMove = std::dynamic_pointer_cast<Core::DirectionalMovementComponent>(component);
						dirMove->SetOrientation(packet.positionOrientationMessage.heading, packet.positionOrientationMessage.pitch);
					}
				}
			}
		}
	};
}
