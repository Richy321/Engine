#pragma once
#include "ISocket.h"
#include "Connection.h"
#include "ReliableConnection.h"
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
		virtual ISocket* CreateSocket(SocketType sockType = INetworkService::SocketType::UDP) = 0;
		virtual Connection* CreateConnection(const int protocolId, const float timeout) = 0;
		virtual ReliableConnection* CreateReliableConnection(const int protocolId, const float timeout) = 0;
	};
}

