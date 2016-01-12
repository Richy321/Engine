#pragma once
#include "Core/Networking/Address.h"
#include <string>

namespace networking
{
	class NetworkPlayer
	{
	public:
		Address Address;
		std::string Name;
	};
}
