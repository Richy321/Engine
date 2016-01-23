#pragma once
#include "IConnectionEventHandler.h"

//#include "Address.h"
namespace networking
{
	class IConnection
	{
	public:

		enum Mode
		{
			None,
			Client,
			Server
		};

		IConnection(unsigned int protocolId, float timeout)
		{
		}

		virtual ~IConnection()
		{
		}

		virtual bool Start(int port) = 0;
		virtual void Listen() = 0;
		virtual void Stop() = 0;
		virtual void Connect(const networking::Address address) = 0;

		virtual bool IsConnecting() const = 0;
		virtual bool ConnectFailed() const = 0;
		virtual bool IsConnected() const = 0;
		virtual bool IsListening() const = 0;
		virtual Mode GetMode() const = 0;

		virtual void Update(float deltaTime) = 0;
		virtual bool SendPacket(const unsigned char data[], const int size) = 0;
		virtual int ReceivePacket(unsigned char data[], int size) = 0;
		virtual void SetConnectionEventHandler(std::shared_ptr<IConnectionEventHandler>& handler) = 0;
	};
}

