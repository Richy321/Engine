#pragma once
#include <bemapiset.h>
#include "../../Dependencies/glm/detail/type_vec3.hpp"

namespace networking
{
	static const int MaxPacketSize = 256;
	class MessageStructures
	{
	public:
		enum SimpleMessageType
		{
			NoneSimple,
			Connect,
			Disconnect,
			SnapShot
		};
		enum MessageType
		{
			None,
			Player,
			Bullet,
			Collectable,
		};

		struct BasicPositionMessage
		{
			vec3 position;
		};

		struct PositionOrientationMessage
		{
			vec3 position;
			float heading;
			float pitch;
		};

		struct BaseMessage
		{
			BaseMessage() : simpleType(NoneSimple), messageType(None)
			{
			}

			BaseMessage(const BaseMessage& other)
			{
				memcpy(this, &other, sizeof(BaseMessage));
			}

			GUID uniqueID;
			SimpleMessageType simpleType;
			MessageType messageType;
			union
			{
				BasicPositionMessage positionMessage;
				PositionOrientationMessage positionOrientationMessage;
			};
		};
	};
}
