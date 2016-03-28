#pragma once
#include <memory>

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
