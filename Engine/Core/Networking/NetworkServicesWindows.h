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

		ISocket* CreateSocket(SocketType sockType)
		{
			return new WinSocket();
		}

		Connection* CreateConnection(const int protocolId, const float timeout)
		{
			return new Connection(protocolId, timeout, CreateSocket(SocketType::UDP));
		}

		ReliableConnection* CreateReliableConnection(const int protocolId, const float timeout)
		{
			return new ReliableConnection(protocolId, timeout, CreateSocket(SocketType::UDP));
		}
	};
}

