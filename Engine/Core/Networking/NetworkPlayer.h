#pragma once

namespace networking
{
	class NetworkPlayer
	{
	public:
		std::string name;
		std::shared_ptr<Address> address;
		std::shared_ptr<GameObject> relatedGameObject;
	};
}
