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
			BasicPosition,
			PlayerConnect,
			PlayerDisconnect
		};

		struct BasicPositionMessage
		{
			glm::vec3 position;
		};

		struct BaseMessage
		{
			BaseMessage() : messageType(BasicPosition)
			{
			}

			GUID uniqueID;
			MessageType messageType;
			union
			{
				BasicPositionMessage positionMessage;
			};
		};
	};
}
