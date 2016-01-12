#pragma once
#include "ISocket.h"

#include <WinSock2.h>
#include <Windows.h>
#include <assert.h>
#include <fstream>

namespace networking
{
	class WinSocket : public ISocket
	{
	public:

		SOCKET socketHandle;

		WinSocket()
		{
			socketHandle = 0;
		}

		~WinSocket()
		{
		}

		bool Open(unsigned short port) override
		{
			//create socket
			assert(!IsOpen());

			socketHandle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if (socketHandle <= 0)
			{
				printf("Failed to open socket, error code:%d\n", WSAGetLastError());
				socketHandle = 0;
				return false;
			}

			//bind to a port (below 1024 are reserved for system use)
			sockaddr_in address;
			address.sin_family = AF_INET;
			address.sin_addr.s_addr = INADDR_ANY;
			address.sin_port = htons((unsigned short)port); //converts ushort from host to TCP/IP network byte order(big endian)

			if (bind(socketHandle, (const sockaddr*)&address, sizeof(sockaddr_in)) < 0)
			{
				printf("unable to bind socket, error code:%d\n", WSAGetLastError());
				Close();
				return false;
			}

			//set non-blocking
			DWORD nonBlocking = 1;
			if (ioctlsocket(socketHandle, FIONBIO, &nonBlocking) != 0) //ioctlsocket controls the I/O mode of the socket
			{
				printf("failed to set non-blocking socket, error code:%d\n", WSAGetLastError());
				Close();
				return false;
			}

			printf("sucessfully created socket on port %d\n", port);
			return true;
		}

		void Close() override
		{
			if (socketHandle != 0)
			{
				closesocket(socketHandle);
				socketHandle = 0;
			}
		}

		bool Send(const Address& destination, const void* data, int size) override
		{
			assert(data);
			assert(size > 0);

			if (socketHandle == 0)
				return false;

			sockaddr_in address;
			address.sin_family = AF_INET;
			address.sin_addr.s_addr = htonl(destination.GetAddress());
			address.sin_port = htons((unsigned short)destination.GetPort());

			int sentBytes = sendto(socketHandle, (const char*)data, size, 0, (sockaddr*)&address, sizeof(sockaddr_in));

			return sentBytes == size; //confirms whether the data was sent (don't know if it was received or not)
		}

		int Receive(Address& sender, void* data, int size) override
		{
			assert(data);
			assert(size > 0);

			if (socketHandle == 0)
				return false;

			typedef int socklen_t;

			sockaddr_in from;
			socklen_t fromLength = sizeof(from);

			int receivedBytes = recvfrom(socketHandle, (char*)data, size, 0, (sockaddr*)&from, &fromLength);

			if (receivedBytes <= 0)
				return false;

			unsigned int address = ntohl(from.sin_addr.s_addr);
			unsigned int port = ntohs(from.sin_port);

			sender = Address(address, port);

			return receivedBytes;
		}

		bool IsOpen()
		{
			return socketHandle != 0;
		}
	};
}

