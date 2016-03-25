#pragma once
#include "ISocket.h"
#include "Connection.h"
#include "ReliableConnection.h"
#include "MultiConnection.h"
class Connection;

namespace networking
{
	class INetworkService
	{
	public:
		enum SocketType
		{
			UDP,
			TCP
		};
		virtual std::shared_ptr<ISocket> CreateSocket(SocketType sockType = INetworkService::SocketType::UDP) = 0;
		virtual std::shared_ptr<Connection> CreateConnection(const int protocolId, const float timeout) = 0;
		virtual std::shared_ptr<ReliableConnection> CreateReliableConnection(const int protocolId, const float timeout) = 0;
		virtual std::shared_ptr<MultiConnection> CreateMultiConnection(const int protocolId, const float timeout) = 0;
	};
}

