#pragma once
#include "SharedNetworkConfiguration.h"
#include "Address.h"
#include <cstdio>
#include "NetworkServices.h"
#include "FlowControl.h"
#include <chrono>
#include <map>
#include <memory>
#include "../Components/NetworkViewComponent.h"
#include "../Utils.h"
#include "IClientNetworkManager.h"
#include "Core/TickTimer.h"
#include <ctime>

namespace networking
{
	class ClientNetworkManager : public INetworkManager, public IConnectionEventHandler
	{
	private:
		IConnection* serverConnection;
		std::chrono::time_point<std::chrono::system_clock> startTime;
		std::chrono::time_point<std::chrono::system_clock> lastTime;
		FlowControl flowControl;
		bool connected = false;
		std::unique_ptr<TickTimer> timer;

		std::function<void(std::shared_ptr<MessageStructures::BaseMessage>, std::shared_ptr<INetworkViewComponent>)> onNetworkViewConnectCallback;
		std::function<void(GUID)> onNetworkViewDisconnectCallback;

		std::mutex mutexConnectedNetViewMap;

		typedef std::map<GUID, std::shared_ptr<INetworkViewComponent>, Utils::GUIDComparer> NetworkIDMapType;
		NetworkIDMapType networkIDToComponent;
	public:
		static std::shared_ptr<ClientNetworkManager>& GetInstance()
		{
			static std::shared_ptr<ClientNetworkManager> instance = std::make_shared<ClientNetworkManager>();
			return instance;
		}

		ClientNetworkManager() : timer(std::make_unique<TickTimer>())
		{
			SetTickRate(std::chrono::milliseconds(5));
			timer->AddOnTickCallback(std::bind(&ClientNetworkManager::UpdateComms, this));
		}

		~ClientNetworkManager()
		{
			timer->Stop();
		}

		void ConnectToServer()
		{
			switch (clientConnectionType)
			{
			case Unreliable:
				serverConnection = NetworkServices::GetInstance().CreateConnection(ProtocolId, TimeOut);
				break;
			case Reliable:
				serverConnection = NetworkServices::GetInstance().CreateReliableConnection(ProtocolId, TimeOut);
				break;
			//case MultiUnreliable:
			//	serverConnection = networking::NetworkServices::GetInstance().CreateMultiConnection(ProtocolId, TimeOut);
			//	break;
			}

			serverConnection->SetConnectionEventHandler(shared_from_this());

			int startClientPort = ClientPort;
			while(!serverConnection->Start(startClientPort) && startClientPort < startClientPort + 50)
			{
				printf("could not start connection on port %d\n", startClientPort);
				startClientPort++;
			}

			if (startClientPort == ClientPort + 50)
				return; //could not find a free port

			serverConnection->Connect(ServerAddress);

			timer->Stop();
			timer->Start();
			startTime = std::chrono::system_clock::now();
			lastTime = startTime;
		}

		void DisconnectFromServer(GUID playerID)
		{
			SendClientDisconnect(playerID);
			
			std::this_thread::sleep_for(std::chrono::milliseconds(500)); //give time to send disconnect message

			serverConnection->Stop();
			timer->Stop();
			connected = false;
		}

		void UpdateComms()
		{
			std::chrono::time_point<std::chrono::system_clock> nowTime = std::chrono::system_clock::now();
			std::chrono::milliseconds deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - lastTime);
			float deltaTimeSecs = deltaTime.count() * 0.001f;
			std::chrono::milliseconds fromStartTime = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - startTime);
			 
			//printf("Comms update: %f \n", fromStartTime.count() * 0.001f);

			if (!connected && serverConnection->IsConnected())
			{
				printf("client connected to server\n");
				connected = true;
			}

			if (!connected && serverConnection->ConnectFailed())
			{
				//printf("connection failed\n");
			}

			if (serverConnection->IsConnected())
			{
				if (clientConnectionType == Reliable)
					flowControl.Update(deltaTimeSecs, static_cast<ReliableConnection*>(serverConnection)->GetReliabilitySystem().GetRoundTripTime() * 1000.0f);
			}

			const float sendRate = flowControl.GetSendRate();

			ReceiveAndRoutePackets();
			ProcessMessages();
			SendComponentPackets();
			if(serverConnection->IsConnected())
				serverConnection->Update(deltaTimeSecs);
			lastTime = nowTime;
		}

		void ReceiveAndRoutePackets()
		{
			//receive until theres no more packets left
			while (true)
			{
				unsigned char packet[256];
				std::shared_ptr<Address> sender = std::make_shared<Address>();
				int bytes_read = 0;
				if (serverConnection->IsConnected() || serverConnection->IsConnecting())
					bytes_read = serverConnection->ReceivePacket(packet, sizeof(packet), sender);

				if (bytes_read == 0)
					break;

				std::shared_ptr<MessageStructures::BaseMessage> message = std::make_shared<MessageStructures::BaseMessage>();
				memcpy(message.get(), packet, sizeof(MessageStructures::BaseMessage));

				ReadPacket(message);
			}
		}

		void ReadPacket(std::shared_ptr<MessageStructures::BaseMessage>& message)
		{
			switch (message->simpleType)
			{
			case MessageStructures::NoneSimple:
				break;

			case MessageStructures::Connect:
				ConnectNetworkView(message);
				break;
			case MessageStructures::Disconnect:
				DisconnectNetworkView(message->uniqueID);
				break;
			case MessageStructures::SnapShot:

				//if a snapshot is received before a connect, make the connection best we can
				if (networkIDToComponent.find(message->uniqueID) == networkIDToComponent.end())
				{
					OLECHAR szGuid[40] = { 0 };
					StringFromGUID2(message->uniqueID, szGuid, 40);
					printf("Force Connection: %ls \n", szGuid);
					ConnectNetworkView(message);
					
				}
				//route packet to network view
				mutexConnectedNetViewMap.lock();
				if (networkIDToComponent.find(message->uniqueID) != networkIDToComponent.end())
					networkIDToComponent[message->uniqueID]->ReadPacket(message);
				mutexConnectedNetViewMap.unlock();
				break;
			}
		}

		void ProcessMessages()
		{
			std::lock_guard<std::mutex> lockConnectedPlayerMap(mutexConnectedNetViewMap);

			for (NetworkIDMapType::iterator it = networkIDToComponent.begin(); it != networkIDToComponent.end(); ++it)
			{
				it->second->ProcessMessages();
			}
		}

		void SendComponentPackets()
		{
			if (serverConnection->IsConnected() || serverConnection->IsConnecting())
			{
				std::lock_guard<std::mutex> lockConnectedPlayerMap(mutexConnectedNetViewMap);
				//todo - look at grouping into one packet for all components (group into state snapshot)
				for (NetworkIDMapType::iterator it = networkIDToComponent.begin(); it != networkIDToComponent.end(); ++it)
				{
					if (it->second->IsSendUpdates())
					{
						std::shared_ptr<MessageStructures::BaseMessage> message = std::make_shared<MessageStructures::BaseMessage>();
						int size = it->second->BuildPacket(message);
						serverConnection->SendPacket(reinterpret_cast<unsigned char*>(message.get()), size);
					}
					it->second->ClearReceivedMessages();
				}
			}
		}

		void Destroy()
		{
			delete serverConnection;
		}

		void AddNetworkViewComponent(std::shared_ptr<INetworkViewComponent> component) override
		{
			std::lock_guard<std::mutex> lock(mutexConnectedNetViewMap);
			networkIDToComponent[component->GetUniqueID()] = component;
		}

		void RemoveNetworkViewComponent(std::shared_ptr<INetworkViewComponent> component) override
		{
			RemoveNetworkViewComponent(component->GetUniqueID());
		}

		void RemoveNetworkViewComponent(GUID id)
		{
			std::lock_guard<std::mutex> lock(mutexConnectedNetViewMap);

			NetworkIDMapType::iterator it = networkIDToComponent.find(id);
			if (it != networkIDToComponent.end())
				networkIDToComponent.erase(it);
		}

		void SetTickRate(std::chrono::milliseconds interval)
		{
			timer->SetTickIntervalMilliseconds(interval);
		}

		void ConnectNetworkView(std::shared_ptr<MessageStructures::BaseMessage> message)
		{
			if (onNetworkViewConnectCallback != nullptr)
			{
				std::shared_ptr<INetworkViewComponent> netView;

				onNetworkViewConnectCallback(message, std::ref(netView));
				if (netView != nullptr)
					AddNetworkViewComponent(netView);
			}
		}

		void DisconnectNetworkView(GUID id)
		{
			if (onNetworkViewDisconnectCallback != nullptr)
				onNetworkViewDisconnectCallback(id);

			mutexConnectedNetViewMap.lock();
			NetworkIDMapType::iterator it = networkIDToComponent.find(id);

			if (it != networkIDToComponent.end())
				networkIDToComponent.erase(it);
			mutexConnectedNetViewMap.unlock();
		}

		void OnStart()
		{
			
		}

		virtual void OnStop()
		{
			
		}

		virtual void OnConnect(std::shared_ptr<Address> address)
		{
			//connected to server
		}
		virtual void OnDisconnect(std::shared_ptr<Address> address)
		{
			//timeout from server
		}

		void SendClientDisconnect(GUID playerNetViewID) const
		{
			if (serverConnection->IsConnected())
			{
				MessageStructures::BaseMessage message;
				message.uniqueID = playerNetViewID;
				message.simpleType = MessageStructures::Disconnect;
				message.messageType = MessageStructures::Player;
				serverConnection->SendPacket(reinterpret_cast<unsigned char*>(&message), sizeof(MessageStructures::BaseMessage));
			}
		}

		void SetOnNetworkViewConnectCallback(std::function<void(std::shared_ptr<MessageStructures::BaseMessage>, std::shared_ptr<INetworkViewComponent>)> callback)
		{
			onNetworkViewConnectCallback = callback;
		}

		void SetOnNetworkViewDisconnectCallback(std::function<void(GUID)> callback)
		{
			onNetworkViewDisconnectCallback = callback;
		}

		bool IsConnected() const
		{ return connected; }
	};
}
