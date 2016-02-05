#pragma once
#include <memory>
#include "Core/Networking/Address.h"
#include "Core/Networking/IConnection.h"

namespace networking
{
	class IMultiConnection : public IConnection
	{
	public:
		IMultiConnection(unsigned int protocolId, float timeout) : IConnection(protocolId, timeout)
		{
			
		}
		virtual ~IMultiConnection() {}
		virtual void ForceDisconnectClient(std::shared_ptr<Address> clientAddress) = 0;
	};
}
