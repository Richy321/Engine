#pragma once
#include "Address.h"
namespace networking
{
	class ISocket
	{
	public:

		ISocket()
		{
		}

		~ISocket()
		{
		}

		virtual bool Open(unsigned short port) = 0;
		virtual void Close() = 0;
		virtual bool Send(const Address& destination, const void* data, int size) = 0;
		virtual int Receive(Address& sender, void* data, int size) = 0;
	};
}

