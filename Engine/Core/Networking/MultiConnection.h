#pragma once
#include "IConnection.h"
#include "INetworkService.h"
#include "ISocket.h"

namespace networking
{
	class ConnectionInfo
	{
	public:
		enum State
		{
			Connected,
			Disconnected,
			Connecting,
			ConnectFail
		};
		State state;
		float timeoutAccumulator;
		float timeout;
		std::shared_ptr<IConnectionEventHandler> connectionEventHandler = nullptr;
		
		void OnStart() const
		{
			if (connectionEventHandler != nullptr)
				connectionEventHandler->OnStart();
		}

		void OnStop() const
		{
			if (connectionEventHandler != nullptr)
				connectionEventHandler->OnStop();
		}

		void OnConnect() const
		{
			if (connectionEventHandler != nullptr)
				connectionEventHandler->OnConnect();
		}

		void OnDisconnect() const
		{
			if (connectionEventHandler != nullptr)
				connectionEventHandler->OnDisconnect();
		}
		void Update(float deltaTime)
		{
			timeoutAccumulator += deltaTime;
			if (timeoutAccumulator > timeout)
			{
				if (state == Connecting)
				{
					printf("connect timed out\n");
					ClearData();
					state = ConnectFail;
					OnDisconnect();
				}
				else if (state == Connected)
				{
					printf("connection timed out\n");
					ClearData();
					if (state == Connecting)
						state = ConnectFail;
					OnDisconnect();
				}
			}
		}

		void ClearData()
		{
			state = Disconnected;
			timeoutAccumulator = 0.0f;
		}
	};
	class MultiConnection : public IConnection
	{
	public:

		MultiConnection(unsigned int protocolId, float timeout, ISocket* socket) : IConnection(protocolId, timeout)
		{
			this->protocolId = protocolId;
			mode = Server;
			this->defaultTimeout = timeout;
			this->socket = socket;
			running = false;
			defaultTimeout = timeout;
		}

		~MultiConnection()
		{
			if (running)
				Stop();
		}

		void Listen() override
		{
			printf("server listening for connection\n");
			bool connected = IsListening();
			
			ClearData();
			mode = Server;
			state = Listening;
		}

		void Stop() override
		{
			ClearData();
		}

		bool Start(int port) override
		{
			return false;
		}

		void Connect(const networking::Address address) override
		{
			
		}

		virtual bool SendPacket(const unsigned char data[], const int size) override
		{
			assert(running);
			if(connections.size() == 0)
				return false;
			unsigned char *packet;
			packet = new unsigned char[size + HeaderSize];

			packet[0] = (unsigned char)(protocolId >> 24);
			packet[1] = (unsigned char)((protocolId >> 16) & 0xFF);
			packet[2] = (unsigned char)((protocolId >> 8) & 0xFF);
			packet[3] = (unsigned char)((protocolId)& 0xFF);
			memcpy(&packet[4], data, size);
			
			bool sendResults = true;
			for (std::map<std::shared_ptr<Address>, std::shared_ptr<ConnectionInfo>>::iterator iter = connections.begin(); iter != connections.end(); ++iter)
			{
				sendResults &= socket->Send(*(iter->first.get()), packet, size + HeaderSize);
			}

			return sendResults;
		}

		virtual int ReceivePacket(unsigned char data[], int size) override
		{
			assert(running);
			unsigned char* packet;
			packet = new unsigned char[size + HeaderSize];
			Address sender;
			int bytesRead = socket->Receive(sender, packet, size + HeaderSize);
			if (bytesRead == 0)
				return false;
			if (bytesRead <= HeaderSize)
				return 0;

			//check the packet header matches the protocolId
			if (packet[0] != static_cast<unsigned char>(protocolId >> 24) ||
				packet[1] != static_cast<unsigned char>((protocolId >> 16) & 0xFF) ||
				packet[2] != static_cast<unsigned char>((protocolId >> 8) & 0xFF) ||
				packet[3] != static_cast<unsigned char>(protocolId & 0xFF))
				return 0;

			if (mode == Server && !IsListening())
			{
				std::shared_ptr<Address> senderAddress = std::make_shared<Address>(sender);
				if(connections.find(senderAddress) == connections.end())
				{
					connections[senderAddress] = std::make_shared<ConnectionInfo>();
					printf("server accepts connection from client %d.%d.%d.%d:%d\n",
						sender.GetA(), sender.GetB(), sender.GetC(), sender.GetD(), sender.GetPort());
				}
				
				connections[senderAddress]->state = ConnectionInfo::Connected;
				connections[senderAddress]->timeoutAccumulator = 0.0f;
				connections[senderAddress]->connectionEventHandler = connectionEventHandler;
				connections[senderAddress]->OnConnect();

				memcpy(data, &packet[HeaderSize], size - HeaderSize);
				return size - HeaderSize;
			}
			return 0;
		}


		void ClearData()
		{
			state = NotListening;
			connections.clear();
		}

		void Update(float deltaTime) override
		{
			for (std::map<std::shared_ptr<Address>, std::shared_ptr<ConnectionInfo>>::iterator iter = connections.begin(); iter != connections.end(); ++iter)
			{
				iter->second->Update(deltaTime);
			}
		}

	private:
		enum State
		{
			NotListening,
			Listening
		};
		unsigned int protocolId;
		float defaultTimeout;
		static const int HeaderSize = 4;
		ISocket* socket;
		Mode mode;
		bool running;
		std::map<std::shared_ptr<Address>, std::shared_ptr<ConnectionInfo>> connections;
		State state;
		std::shared_ptr<IConnectionEventHandler> connectionEventHandler = nullptr;

		virtual bool IsConnecting() const { return false; }
		virtual bool ConnectFailed() const { return false; }
		virtual bool IsConnected() const { return false; }
		virtual bool IsListening() const { return state == Listening; }
		virtual Mode GetMode() const { return Server; }

		virtual void SetConnectionEventHandler(std::shared_ptr<IConnectionEventHandler>& handler) override
		{
			connectionEventHandler = handler;
		}
	};
}
