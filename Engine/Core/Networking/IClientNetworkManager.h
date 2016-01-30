#pragma once
#include <memory>
#include "../Components/Interfaces/INetworkViewComponent.h"

namespace networking
{
	class INetworkManager
	{
	public:
		virtual ~INetworkManager()
		{
		}

		virtual void AddNetworkViewComponent(std::shared_ptr<Core::INetworkViewComponent> component) = 0;
		virtual void RemoveNetworkViewComponent(std::shared_ptr<Core::INetworkViewComponent> component) = 0;
	};
}