#pragma once
#include "INetworkService.h"
#include "WinSocket.h"
#include "Connection.h"

class Connection;

#if PLATFORM == PLATFORM_WINDOWS 
	#include <WinSock2.h>
	#include <Windows.h>
	#pragma comment( lib, "wsock32.lib" ) 
#endif

namespace networking
{
	class NetworkServicesWindows : public INetworkService
	{
	public:
		#pragma region Construction/Destruction
		NetworkServicesWindows()
		{
			bool initialised = Initialise();
			assert(initialised);
		}

		~NetworkServicesWindows()
		{
			CleanUp();
		}

		#pragma endregion

		bool Initialise()
		{
			WSADATA wsaData;
			return WSAStartup(MAKEWORD(2, 0), &wsaData) == NO_ERROR;
		}

		void CleanUp()
		{
			WSACleanup();
		}

		std::shared_ptr<ISocket> CreateSocket(SocketType sockType) override
		{
			return std::make_shared<WinSocket>();
		}

		std::shared_ptr<Connection> CreateConnection(const int protocolId, const float timeout) override
		{
			return std::make_shared<Connection>(protocolId, timeout, CreateSocket(UDP));
		}

		std::shared_ptr<ReliableConnection> CreateReliableConnection(const int protocolId, const float timeout) override
		{
			return std::make_shared<ReliableConnection>(protocolId, timeout, CreateSocket(UDP));
		}

		std::shared_ptr<MultiConnection> CreateMultiConnection(const int protocolId, const float timeout) override
		{
			return std::make_shared<MultiConnection>(protocolId, timeout, CreateSocket(UDP));
		}
	};
}

