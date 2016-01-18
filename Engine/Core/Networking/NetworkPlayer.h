#pragma once

namespace networking
{
	class NetworkPlayer
	{
	public:
		std::string name;
		std::shared_ptr<GameObject> relatedGameObject;
		std::vector<std::shared_ptr<MessageStructures::BaseMessage>> messages;
	};
}
