#pragma once
#include <bemapiset.h>
#include "../../Dependencies/glm/detail/type_vec3.hpp"

namespace networking
{
	static const int MaxPacketSize = 256;
	class MessageStructures
	{
	public:
		enum MessageType
		{
			None,
			PlayerSnapshot,
			BulletSnapshot,
			CollectableSnapshot,
			PlayerConnect,
			PlayerDisconnect
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
			BaseMessage() : messageType(None)
			{
			}

			GUID uniqueID;
			MessageType messageType;
			union
			{
				BasicPositionMessage positionMessage;
				PositionOrientationMessage positionOrientationMessage;
			};
		};


	};
}
