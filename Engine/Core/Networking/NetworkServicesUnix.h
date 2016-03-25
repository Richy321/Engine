#pragma once
#include "INetworkService.h"
#include "NetworkServices.h"

#if PLATFORM == PLATFORM_UNIX
	#include <sys/socket.h> 
	#include <netinet/in.h> 
	#include <fcntl.h>
#endif
namespace networking
{
	class NetworkServicesUnix : public INetworkService
	{
	public:

		NetworkServicesUnix()
		{
		}

		~NetworkServicesUnix()
		{
		}

		std::shared_ptr<ISocket> CreateSocket(SocketType sockType) override
		{
			return std::make_shared<WinSocket>();
		}

		std::shared_ptr<Connection> CreateConnection(const int protocolId, const float timeout) override
		{
			return std::make_shared<Connection>(protocolId, timeout, CreateSocket(SocketType::UDP));
		}

		std::shared_ptr<ReliableConnection> CreateReliableConnection(const int protocolId, const float timeout) override
		{
			return std::make_shared<ReliableConnection>(protocolId, timeout, CreateSocket(SocketType::UDP));
		}
	};
}

